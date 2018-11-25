/*! @file	ZclSEMDUPairing.c
 *
 * @brief	This source file describes specific functionality implemented
 *			for the MDU Pairing cluster.
 *
 * @copyright Copyright(c) 2013, Freescale, Inc. All rights reserved.
 *
 * @license	Redistribution and use in source and binary forms, with or without modification,
 *			are permitted provided that the following conditions are met:
 *
 *			o Redistributions of source code must retain the above copyright notice, this list
 *			of conditions and the following disclaimer.
 *
 *			o Redistributions in binary form must reproduce the above copyright notice, this
 *   		list of conditions and the following disclaimer in the documentation and/or
 *   		other materials provided with the distribution.
 *
 * 			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   		contributors may be used to endorse or promote products derived from this
 *   		software without specific prior written permission.
 *
 * 			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * 			ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * 			WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * 			ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * 			(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 			LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * 			ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * 			(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * 			SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "EmbeddedTypes.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"

#include "SEProfile.h"
#include "ZclSE.h"
#include "ZclSEMDUPairing.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

static zbStatus_t zclMDUPairing_ProcessPairingReq(afAddrInfo_t* pAddrInfo, uint8_t zclTransactionId, zclCmdMDUPairing_PairingReq_t* pReq);
static zbStatus_t zclMDUPairing_ProcessPairingRsp(zclCmdMDUPairing_PairingRsp_t *pRsp, uint8_t numDevices);
static zclMDUPairing_MDUEntry_t* zclMDUPairing_GetMDUEntryByIdx(index_t iHANidx);
static zclMDUPairing_MDUEntry_t* zclMDUPairing_GetMDUEntryByAddress(zbIeeeAddr_t aIeeeAddr);
static zbStatus_t zclMDUPairing_SendPairingReq(zclMDUPairing_MDUEntry_t *pEntry);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

static zclMDUPairing_MDUEntry_t maMDUPairingTable[gNumOfVirtualHANs_c];
static zclSendPairingRspInfo_t mSendPairingRsp;
static zclMDUPairing_MDUEntry_t gVHANDevices;
static index_t mPairRspNextDevIdx;

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		zbStatus_t ZCL_MDUPairingServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the MDU Pairing server. 
 *
 */
zbStatus_t ZCL_MDUPairingServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo;
  zbStatus_t status = gZbSuccess_c;
  
  (void)pDevice;
  
   pFrame = (void *)pIndication->pAsdu;
   
   /*Create the destination address*/
   AF_PrepareForReply(&addrInfo, pIndication); 

   switch (pFrame->command) 
   {
      case gZclCmdMDUPairing_PairingReq_c:
        {
          zclCmdMDUPairing_PairingReq_t *pCmdPayload = (zclCmdMDUPairing_PairingReq_t *)((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t));
          status = zclMDUPairing_ProcessPairingReq(&addrInfo, pFrame->transactionId, pCmdPayload);
        }
        break;
        
      default:
        status = gZclUnsupportedClusterCommand_c;
        break;
  }
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_MDUPairingClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the MDU Pairing client. 
 *
 */
zbStatus_t ZCL_MDUPairingClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo;
  zbStatus_t status = gZbSuccess_c;
 
  pFrame = (void *)pIndication->pAsdu;
  /* Create the destination address */
  AF_PrepareForReply(&addrInfo, pIndication); 
  
  (void)pDevice;
  
  pFrame = (void *)pIndication->pAsdu;
  /* Handle the command */
  switch(pFrame->command){
     case gZclCmdMDUPairing_PairingRsp_c:
       {
         uint8_t profilePayloadLen =  (pIndication->asduLength - sizeof(zclFrame_t) - sizeof(uint32_t) - 2) / sizeof(zbIeeeAddr_t);
         status = zclMDUPairing_ProcessPairingRsp((zclCmdMDUPairing_PairingRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                     + sizeof(zclFrame_t))), profilePayloadLen);
         break;
       }
     default:
        status = gZclUnsupportedClusterCommand_c;    
        break;
  }
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_HandlePairingRsp(void)
 *
 * @brief	Handles received Pairing Response.
 *
 */
zbStatus_t ZCL_HandlePairingRsp
  (
  void
  )
{                   
  zbStatus_t status;
  zclMDUPairing_MDUEntry_t *pEntry = zclMDUPairing_GetMDUEntryByIdx(mSendPairingRsp.iVHANidx);
    
  status = zclMDUPairing_SendPairingReq(pEntry);
  
  if(status == gZclSuccess_c)
  {
    mSendPairingRsp.cmdIdxTotalNumCmd.cmdIdx += 1;
    if (mSendPairingRsp.cmdIdxTotalNumCmd.cmdIdx < mSendPairingRsp.cmdIdxTotalNumCmd.totalNoCmds)
      ZclSE_SendClusterEvt(gZclEvtHandleMDUPairingRspEvt_c);
  }
  return status;
}

/*!
 * @fn 		zbStatus_t zclMDUPairing_PairingReq(zclMDUPairing_PairingReq_t *pReq)
 *
 * @brief	Sends over-the-air a Pairing Request frame from the MDU Pairing client.
 *
 */
zbStatus_t zclMDUPairing_PairingReq
  (
  zclMDUPairing_PairingReq_t *pReq
  )
{
  return ZCL_SendClientReqSeqPassed(gZclCmdMDUPairing_PairingReq_c, sizeof(zclCmdMDUPairing_PairingReq_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclMDUPairing_PairingRsp(zclMDUPairing_PairingRsp_t *pReq, uint8_t numAddresses)
 *
 * @brief	Sends over-the-air a Pairing Response frame from the MDU Pairing server.
 *
 */
zbStatus_t zclMDUPairing_PairingRsp
  (
  zclMDUPairing_PairingRsp_t *pReq, 
  uint8_t numAddresses
  )
{
  /* Payload must have have minimum 1 address */
  uint8_t len = sizeof(zbIeeeAddr_t) * (numAddresses - 1);
  return ZCL_SendServerRspSeqPassed(gZclCmdMDUPairing_PairingRsp_c, sizeof(zclCmdMDUPairing_PairingRsp_t) + len, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclMDUPairing_AddVHAN(zclMDUPairing_AddVHAN_t *pReq)
 *
 * @brief	Adds a Virtual HAN to the MDU PairingTable.
 *
 */
zbStatus_t zclMDUPairing_AddVHAN
  (
  zclMDUPairing_AddVHAN_t *pReq
  )
{
  index_t i;
  
  zclMDUPairing_MDUEntry_t *pEntry = zclMDUPairing_GetMDUEntryByIdx(pReq->iVHANidx);
  
  if (pEntry != NULL)
    return gZclFailure_c;
  
  /* Prevent memory overflow */
  if (pReq->devCount > gNumOfVirtualHANDevices_c)
    return gZclFailure_c;
  
  for (i = 0; i < gNumOfVirtualHANs_c; i++)
  {
    zclMDUPairing_MDUEntry_t* pEntry = &maMDUPairingTable[i];
    
    if (pEntry->pairInfoVersion == gZclMDUPairing_InvalidPairingVersion_c)
    {
      /* Found an empty entry. Populate it */
      pEntry->iVHANidx = pReq->iVHANidx;
      pEntry->pairInfoVersion = 1;
      pEntry->devCount = pReq->devCount;
      FLib_MemCpy(pEntry->aDeviceList, pReq->aDeviceList, 
                  pReq->devCount * sizeof(zbIeeeAddr_t));
      return gZclSuccess_c;
    }
  }
  return gZclFailure_c;
}

/*!
 * @fn 		zbStatus_t zclMDUPairing_UpdateVHAN(zclMDUPairing_UpdateVHAN_t *pReq)
 *
 * @brief	Updates a Virtual HAN from the MDU PairingTable.
 *
 */
zbStatus_t zclMDUPairing_UpdateVHAN
  (
  zclMDUPairing_UpdateVHAN_t *pReq
  )
{
  zclMDUPairing_MDUEntry_t *pEntry = zclMDUPairing_GetMDUEntryByIdx(pReq->iVHANidx);
  
  if (pEntry == NULL)
    return gZclFailure_c;
  
  /* Prevent memory overflow */
  if (pReq->devCount > gNumOfVirtualHANDevices_c)
    return gZclFailure_c;
  
  pEntry->iVHANidx = pReq->iVHANidx;
  pEntry->pairInfoVersion++;
  pEntry->devCount = pReq->devCount;
  FLib_MemCpy(pEntry->aDeviceList, pReq->aDeviceList, 
              pReq->devCount * sizeof(zbIeeeAddr_t));
  
  return gZclSuccess_c;
}

/*!
 * @fn 		zbStatus_t zclMDUPairing_DeleteVHANs(void)
 *
 * @brief	Deletes all Virtual HAN from the MDU PairingTable.
 *
 */
zbStatus_t zclMDUPairing_DeleteVHANs(void)
{
  BeeUtilLargeZeroMemory(&maMDUPairingTable, 
                         gNumOfVirtualHANs_c * sizeof(zclMDUPairing_MDUEntry_t));
  return gZclSuccess_c;
} 


/*!
 * @fn 		zclMDUPairing_MDUEntry_t* zclMDUPairing_GetVHANDevices(void)
 *
 * @brief	Gets all devices from a Virtual HAN.
 *
 */
zclMDUPairing_MDUEntry_t* zclMDUPairing_GetVHANDevices
  (
  void
  )
{
  return &gVHANDevices;
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* zclMDUPairing_ProcessPairingReq
* Processes the Pairing Request.
*
* Returns Success if response is sent.
*****************************************************************************/
static zbStatus_t zclMDUPairing_ProcessPairingReq
  (
  afAddrInfo_t* pAddrInfo, 
  uint8_t zclTransactionId, 
  zclCmdMDUPairing_PairingReq_t* pReq
  )
{
  zclMDUPairing_MDUEntry_t *pEntry = zclMDUPairing_GetMDUEntryByAddress(pReq->aExtAddress);
  uint8_t maxDevInCmd = (uint8_t)(AF_MaxPayloadLen(pAddrInfo) - sizeof(zclFrame_t) -
            sizeof(uint32_t) - sizeof(zclCmdIdxTotalNumCmd_t)- 1) / sizeof(zbIeeeAddr_t);
  
  if (!pEntry)
    return gZclWaitForData_c;
  
  if(pEntry->pairInfoVersion == pReq->pairInfoVersion)
    return gZclWaitForData_c;
  
  /* Save data for handling responses */
  FLib_MemCpy(&mSendPairingRsp.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  mSendPairingRsp.iNextDevIdx = 0;
  mSendPairingRsp.cmdIdxTotalNumCmd.cmdIdx = 0;
  mSendPairingRsp.cmdIdxTotalNumCmd.totalNoCmds = pEntry->devCount / maxDevInCmd + 1;
  mSendPairingRsp.iVHANidx = pEntry->iVHANidx;
  mSendPairingRsp.zclTransactionId = zclTransactionId; 

  return ZCL_HandlePairingRsp();
}
                   

/*****************************************************************************
* zclMDUPairing_SendPairingReq
*
* Returns Success message is sent.
*****************************************************************************/
static zbStatus_t zclMDUPairing_SendPairingReq
  (
  zclMDUPairing_MDUEntry_t *pEntry
  ) 
{
  uint8_t maxDevInCmd, numRemDev, numDevInCmd;
  zbStatus_t status;
  zclMDUPairing_PairingRsp_t *pRsp = NULL;
  
  maxDevInCmd = (uint8_t)(AF_MaxPayloadLen(&mSendPairingRsp.addrInfo) - sizeof(zclFrame_t) -
            sizeof(uint32_t) - sizeof(zclCmdIdxTotalNumCmd_t)- 1) / sizeof(zbIeeeAddr_t);
  
  numRemDev = pEntry->devCount - mSendPairingRsp.iNextDevIdx;
  numDevInCmd = FLib_GetMin(maxDevInCmd, numRemDev);
  
  pRsp = MSG_Alloc(sizeof(zclMDUPairing_PairingRsp_t) + numDevInCmd * sizeof(zbIeeeAddr_t));
  
  if (!pRsp)
    return gZclFailure_c;
     
  FLib_MemCpy(&pRsp->addrInfo, &mSendPairingRsp.addrInfo, sizeof(afAddrInfo_t));
  pRsp->zclTransactionId = mSendPairingRsp.zclTransactionId;

  /* Fill the response payload */    
  pRsp->cmdFrame.pairInfoVersion = pEntry->pairInfoVersion;
  pRsp->cmdFrame.totalNoDev = pEntry->devCount;
  pRsp->cmdFrame.cmdIdxNumCmd.cmdIdx = mSendPairingRsp.cmdIdxTotalNumCmd.cmdIdx;
  pRsp->cmdFrame.cmdIdxNumCmd.totalNoCmds = mSendPairingRsp.cmdIdxTotalNumCmd.totalNoCmds;
  
  /* Add the addresses*/
  FLib_MemCpy(pRsp->cmdFrame.aDeviceList, &pEntry->aDeviceList[mSendPairingRsp.iNextDevIdx],
              sizeof(zbIeeeAddr_t) * numDevInCmd);
  
  /* Send the response */
  status = zclMDUPairing_PairingRsp(pRsp, numDevInCmd);
  
  if (status == gZclSuccess_c)
  {
    mSendPairingRsp.iNextDevIdx += numDevInCmd;
  }
  MSG_Free(pRsp);
  return status;
}                   

/*****************************************************************************
* zclMDUPairing_ProcessPairingRsp
* Processes the Pairing Response
*
* Returns Success if worked.
*****************************************************************************/
static zbStatus_t zclMDUPairing_ProcessPairingRsp
  (
  zclCmdMDUPairing_PairingRsp_t *pRsp, 
  uint8_t numDevices
  )
{
  if (pRsp->totalNoDev > gNumOfVirtualHANDevices_c)
   return gZclNoMem_c;
  
  /* This is the first command */
  if (pRsp->cmdIdxNumCmd.cmdIdx == 0)
  {
    gVHANDevices.pairInfoVersion = pRsp->pairInfoVersion;
    gVHANDevices.devCount = pRsp->totalNoDev;
  }
  
  FLib_MemCpy(&gVHANDevices.aDeviceList[mPairRspNextDevIdx], pRsp->aDeviceList, numDevices * sizeof(zbIeeeAddr_t));
  mPairRspNextDevIdx += numDevices;
  
  /* This is the last command */
  if (pRsp->cmdIdxNumCmd.cmdIdx == pRsp->cmdIdxNumCmd.totalNoCmds - 1)
  {
    TS_SendEvent(gSETaskID, gZclEvtPairingFinished_c);
  }
  
  return gZclSuccess_c;
}

 

/*****************************************************************************
* zclMDUPairing_GetMDUEntryByIdx
* Gets an entry the MDU PairingTable
*
* Returns Success message is sent.
*****************************************************************************/
static zclMDUPairing_MDUEntry_t* zclMDUPairing_GetMDUEntryByIdx
  (
  index_t iHANidx
  )
{
  index_t i;
  
  for (i = 0; i < gNumOfVirtualHANs_c; i++)
  {
    zclMDUPairing_MDUEntry_t* pEntry = &maMDUPairingTable[i];
    
    if ((iHANidx == pEntry->iVHANidx) && 
        (pEntry->pairInfoVersion != gZclMDUPairing_InvalidPairingVersion_c))
      return pEntry;
  }
  return NULL;
}

/*****************************************************************************
* zclMDUPairing_GetMDUEntryByAddress
* Gets an entry the MDU PairingTable
*
* Returns Success message is sent.
*****************************************************************************/
static zclMDUPairing_MDUEntry_t* zclMDUPairing_GetMDUEntryByAddress
  (
  zbIeeeAddr_t aIeeeAddr
  )
{
  index_t i, j;
  
  for (i = 0; i < gNumOfVirtualHANs_c; i++)
  {
    zclMDUPairing_MDUEntry_t* pEntry = &maMDUPairingTable[i];
   
    if (pEntry->pairInfoVersion != gZclMDUPairing_InvalidPairingVersion_c)
    {
     for (j = 0; j < pEntry->devCount; j++)
     {
       /* Find the Ieee Address in the VHAN*/
       if (Cmp8Bytes(pEntry->aDeviceList[j], aIeeeAddr))
         return pEntry;
     }
    }
  }
  return NULL;
}
