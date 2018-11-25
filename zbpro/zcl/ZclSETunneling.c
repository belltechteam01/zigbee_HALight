/*! @file	ZclSETunneling.c
 *
 * @brief	This source file describes specific functionality implemented
 *			for the Tunneling cluster.
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
#include "zigbee.h"
#include "ApsMgmtInterface.h"
#include "ApsDataInterface.h"

#include "SEProfile.h"
#include "ZclSETunneling.h"
#include "zcl.h"
#include "ZclFoundation.h"
#include "EndPointConfig.h"

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

static zbStatus_t ZCL_SETunnel_Server_Process_RequestTunnelReq(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SETunnel_Server_Process_CloseTunnelReq(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SETunnel_Server_Process_GetSupportedProtocolsReq(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SETunnel_Client_Process_RequestTunnelRsp(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SETunnel_Process_TransferDataReq(zbApsdeDataIndication_t *pIndication);
#if gASL_ZclSETunnelingOptionals_d
static zbStatus_t ZCL_SETunnel_Process_AckTransferDataRsp(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SETunnel_Process_ReadyDataReq(zbApsdeDataIndication_t *pIndication);
#endif
static zbStatus_t ZCL_SendTunnelClosureNotification(index_t tableIdx);
static void ResetTunnelTimeout(uint8_t iTTE);
static uint8_t getFreeTunnelTableEntry();
static bool_t IsProtocolSupported(uint8_t protocolID);
static uint8_t getTunnelTableEntryPending();
static void CloseTunnel(uint16_t tunnelID);
static void CloseTunnelTimerCallback(zbTmrTimerID_t tmrID);
#if gASL_ZclSETunneling_d
static uint16_t AddTunnelData(tunnelingTable_t* pTTE, afToApsdeMessage_t * pMsg, uint16_t requestLength); 
#endif
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
static uint16_t tunnelIDCrt = 0;
bool_t gAcceptTunnel;
tunnelingTable_t gaZclSETunnelingTable[gZclSETunnelMaxDataHandlingCapacity_c];
static zclSETunneling_Protocol_t protocolIDs[gZclSETunnelNumberOfSupportedProtocolIDs_c] = {{gZclSETunnel_NoManufacturerCode_c, TestProtocol}};

const zclAttrDef_t gaZclSETunnelingServerAttrDef[] = {
  { gZclAttrIdSETunnelingCloseTunnelTimeout_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAMRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(seTunnelingAttrRAM_t, closeTunnelTimeout)}
};

const zclAttrSet_t gaZclSETunnelingServerAttrSet[] = {
  {gZclAttrIdSETunnelingCloseTunnelTimeout_c, (void *)&gaZclSETunnelingServerAttrDef, NumberOfElements(gaZclSETunnelingServerAttrDef)}
};

const zclAttrSetList_t gZclSETunnelingServerAttrSetList = {
  NumberOfElements(gaZclSETunnelingServerAttrSet),
  gaZclSETunnelingServerAttrSet
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		zbStatus_t ZCL_SETunnelingServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Tunneling Cluster server. 
 *
 */
zbStatus_t ZCL_SETunnelingServer
(
zbApsdeDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
)
{
  zclCmd_t Cmd;
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  (void) pDev;
  
  pFrame = (void *)pIndication->pAsdu;
  Cmd = pFrame->command;
  
  switch(Cmd)
  {
    case gZclCmdSETunneling_Client_RequestTunnelReq_c: 
    {
      status = ZCL_SETunnel_Server_Process_RequestTunnelReq(pIndication);
    }
    break;
    
    case gZclCmdSETunneling_Client_CloseTunnelReq_c:
    {
      (void)ZCL_SETunnel_Server_Process_CloseTunnelReq(pIndication);
    }
    break;
    
    case gZclCmdSETunneling_Client_TransferData_c:
    {
      status = ZCL_SETunnel_Process_TransferDataReq(pIndication);
    }
    break;
    
    case gZclCmdSETunneling_Client_TransferDataError_c:
    {
      zclCmdSETunneling_TransferDataErrorReq_t *pReq = (zclCmdSETunneling_TransferDataErrorReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
      BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_TransferDataErrorReceived_c, 0, 0, pReq);
    }
    break;
    
#if gASL_ZclSETunnelingOptionals_d
    case gZclCmdSETunneling_Client_AckTransferDataRsp_c:
    {
      status = ZCL_SETunnel_Process_AckTransferDataRsp(pIndication);
    }
    break;
    
    case gZclCmdSETunneling_Client_ReadyDataReq_c:
    {
      status = ZCL_SETunnel_Process_ReadyDataReq(pIndication);
    }
    break;
    
#endif
    case gZclCmdSETunneling_Client_GetSupportedTunnelProtocols_c:
    {
      status = ZCL_SETunnel_Server_Process_GetSupportedProtocolsReq(pIndication);
    }
    break;
    
    default:
      status = gZclUnsupportedClusterCommand_c;
    break;  
  }
  
   /* Check if we need to send Default Response */
  if((pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) &&
     (status == gZclSuccessDefaultRsp_c))
    status = gZclSuccess_c;
  
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_SETunnelingClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Tunneling Cluster client. 
 *
 */
zbStatus_t ZCL_SETunnelingClient
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  (void) pDev;
  
  pFrame = (void *)pIndication->pAsdu;
  Cmd = pFrame->command;
  
  switch(Cmd)
  {
   case gZclCmdSETunneling_Server_RequestTunnelRsp_c: 
   {
     status = ZCL_SETunnel_Client_Process_RequestTunnelRsp(pIndication);
   }
   break;
     
   case gZclCmdSETunneling_Server_TransferData_c:
   {
     status = ZCL_SETunnel_Process_TransferDataReq(pIndication);
   }
   break;
   
   case gZclCmdSETunneling_Server_TransferDataError_c:
   {
     zclCmdSETunneling_TransferDataErrorReq_t *pReq = (zclCmdSETunneling_TransferDataErrorReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
     BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_TransferDataErrorReceived_c, 0, 0, pReq);
   }
   break;
   
#if gASL_ZclSETunnelingOptionals_d
   case gZclCmdSETunneling_Server_AckTransferDataRsp_c:
   {
     status = ZCL_SETunnel_Process_AckTransferDataRsp(pIndication);//zclCmdSETunneling_AckTransferDataRsp_t *pReq = (zclCmdSETunneling_AckTransferDataRsp_t *) (pIndication->pAsdu + sizeof(zclFrame_t));  
   }
   break;
   
   case gZclCmdSETunneling_Server_ReadyDataReq_c:
   {
     status = ZCL_SETunnel_Process_ReadyDataReq(pIndication);
   }
   break;
   
#endif
   case gZclCmdSETunneling_Server_SupportedTunnelProtocolsRsp_c:
   {
     zclCmdSETunneling_SupportedTunnelingProtocolsRsp_t *pRsp = (zclCmdSETunneling_SupportedTunnelingProtocolsRsp_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
     BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_TransferDataErrorReceived_c, 0, 0, pRsp);
   }
   break;
   
   case gZclCmdSETunneling_Server_TunnelClosureNotification_c:
    break;
   
   default:
     status = gZclUnsupportedClusterCommand_c;
     break;  
  }
  
   /* Check if we need to send Default Response */
  if((pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) &&
     (status == gZclSuccessDefaultRsp_c))
    status = gZclSuccess_c;
  
  return status;
}

/*!
 * @fn 		void ZCL_SETunnel_InitData(void)
 *
 * @brief	Initializes the Tunneling functionality.
 *
 */
void ZCL_SETunnel_InitData(void)
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    BeeUtilZeroMemory(&gaZclSETunnelingTable[i], sizeof(tunnelingTable_t));
    gaZclSETunnelingTable[i].tunnelID = gZclSETunnel_TunnelFail_c;
  }
}

/*!
 * @fn 		uint8_t getTunnelTableEntry(uint16_t tunnelID)
 *
 * @brief	Finds the Tunneling table entry, for a given tunnel id.
 *
 */
uint8_t getTunnelTableEntry(uint16_t tunnelID)
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    if(gaZclSETunnelingTable[i].tunnelID == tunnelID)
    {
      return i;
    }
  }
  
  return gZclSETunnel_Fail_c;
}

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Client_RequestTunnelReq(zclSETunneling_RequestTunnelReq_t *pReq)
 *
 * @brief	Sends over-the-air a Request Tunnel frame from the Tunneling client and 
 *			creates a new entry in the tunneling table.
 *
 */
zbStatus_t ZCL_SETunnel_Client_RequestTunnelReq
(
zclSETunneling_RequestTunnelReq_t *pReq
)
{
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  
  iTTE = getFreeTunnelTableEntry();
  pTTE = &gaZclSETunnelingTable[iTTE];
  
  if(iTTE == gZclSETunnel_TableFull_c)
  {
    return gZclFailure_c;
  }
  
  if(!APS_GetIeeeAddress(pReq->addrInfo.dstAddr.aNwkAddr, pTTE->partnerIeeeAddr))
  {
    return gZclFailure_c;
  }
  
  pTTE->tunnelStatus = gZclSETunnel_TTE_AwaitingTunnelRequestRsp_c;
  pTTE->protocolId = pReq->cmdFrame.protocolID;
  pTTE->flowControlSupport = pReq->cmdFrame.flowControlSupport;
  pTTE->manufacturerCode = OTA2Native16(pReq->cmdFrame.manufacturerCode);
  pTTE->rxDataLength = 0;
  pTTE->txDataLength = 0;
  pTTE->partnerAddrInfo = pReq->addrInfo;
  pTTE->isClient = TRUE;
  
  return ZCL_SendClientReqSeqPassed(gZclCmdSETunneling_Client_RequestTunnelReq_c, sizeof(zclCmdSETunneling_RequestTunnelReq_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Client_CloseTunnelReq(zclSETunneling_CloseTunnelReq_t *pReq)
 *
 * @brief	Sends over-the-air a Close Tunnel frame from the Tunneling client.
 *
 */
zbStatus_t ZCL_SETunnel_Client_CloseTunnelReq
(
zclSETunneling_CloseTunnelReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSETunneling_Client_CloseTunnelReq_c, sizeof(zclCmdSETunneling_CloseTunnelReq_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_TunnelClosureNotificationReq(zclSETunneling_TunnelClosureNotification_t *pReq)
 *
 * @brief	Sends over-the-air a Tunnel Closure Notification frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_TunnelClosureNotificationReq
(
zclSETunneling_TunnelClosureNotification_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdSETunneling_Server_TunnelClosureNotification_c, sizeof(zclCmdSETunneling_TunnelClosureNotification_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_RequestTunnelRsp(zclSETunneling_RequestTunnelRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Request Tunnel Response frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_RequestTunnelRsp
(
zclSETunneling_RequestTunnelRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdSETunneling_Server_RequestTunnelRsp_c, sizeof(zclCmdSETunneling_RequestTunnelRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		ZCL_SETunnel_Server_SetNextTunnelID(uint16_t tunnelID)
 *
 * @brief	Sets the next available tunnel ID on the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_SetNextTunnelID(uint16_t tunnelID)
{
  tunnelIDCrt = tunnelID;
  return gZclSuccess_c;
}

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_TransferDataErrorReq(zclSETunneling_TransferDataErrorReq_t *pReq)
 *
 * @brief	Sends over-the-air a Transfer Data Error frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_TransferDataErrorReq
(
zclSETunneling_TransferDataErrorReq_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdSETunneling_Server_TransferDataError_c, sizeof(zclCmdSETunneling_TransferDataErrorReq_t),(zclGenericReq_t *)pReq);
}

#if gASL_ZclSETunnelingOptionals_d

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_AckTransferDataRsp(zclSETunneling_AckTransferDataRsp_t *pReq)
 *
 * @brief	Sends over-the-air an Ack Transfer Data frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_AckTransferDataRsp
(
zclSETunneling_AckTransferDataRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdSETunneling_Server_AckTransferDataRsp_c, sizeof(zclCmdSETunneling_AckTransferDataRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_ReadyData(zclSETunneling_ReadyDataReq_t *pReq)
 *
 * @brief	Sends over-the-air a Ready Data frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_ReadyData
(
zclSETunneling_ReadyDataReq_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdSETunneling_Server_ReadyDataReq_c, sizeof(zclCmdSETunneling_ReadyDataReq_t),(zclGenericReq_t *)pReq);
}

#endif

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_SupportedTunnelProtocolsRsp(zclSETunneling_SupportedTunnelingProtocolsRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Ready Data frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_SupportedTunnelProtocolsRsp
(
zclSETunneling_SupportedTunnelingProtocolsRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdSETunneling_Server_SupportedTunnelProtocolsRsp_c, (sizeof(zclCmdSETunneling_SupportedTunnelingProtocolsRsp_t) + (pReq->cmdFrame.protocolCount - 1) * sizeof(zclSETunneling_Protocol_t)), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Client_TransferDataErrorReq(zclSETunneling_TransferDataErrorReq_t *pReq)
 *
 * @brief	Sends over-the-air a Transfer Data Error frame from the Tunneling client.
 *
 */
zbStatus_t ZCL_SETunnel_Client_TransferDataErrorReq
(
zclSETunneling_TransferDataErrorReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSETunneling_Client_TransferDataError_c, sizeof(zclCmdSETunneling_TransferDataErrorReq_t),(zclGenericReq_t *)pReq);
}

#if gASL_ZclSETunnelingOptionals_d
/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Client_AckTransferDataRsp(zclSETunneling_AckTransferDataRsp_t *pReq)
 *
 * @brief	Sends over-the-air an Ack Transfer Data frame from the Tunneling client.
 *
 */
zbStatus_t ZCL_SETunnel_Client_AckTransferDataRsp
(
zclSETunneling_AckTransferDataRsp_t *pReq
)
{
  return ZCL_SendClientRspSeqPassed(gZclCmdSETunneling_Client_AckTransferDataRsp_c, sizeof(zclCmdSETunneling_AckTransferDataRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_ReadyDataReq(zclSETunneling_ReadyDataReq_t *pReq, bool_t client)
 *
 * @brief	Sends over-the-air a Ready Data frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_ReadyDataReq
(
zclSETunneling_ReadyDataReq_t *pReq,
bool_t client
)
{
  if(client)
  {
  return ZCL_SendClientReqSeqPassed(gZclCmdSETunneling_Client_ReadyDataReq_c, sizeof(zclCmdSETunneling_ReadyDataReq_t),(zclGenericReq_t *)pReq);
  }
  else
  {
    return ZCL_SendServerReqSeqPassed(gZclCmdSETunneling_Server_ReadyDataReq_c, sizeof(zclCmdSETunneling_ReadyDataReq_t),(zclGenericReq_t *)pReq);
  }
}

#endif

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Client_GetSupportedProtocolsReq(zclSETunneling_GetSupportedTunnelProtocolsReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Supported Tunnel Protocols Data from the Tunneling client.
 *
 */
zbStatus_t ZCL_SETunnel_Client_GetSupportedProtocolsReq
(
zclSETunneling_GetSupportedTunnelProtocolsReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSETunneling_Client_GetSupportedTunnelProtocols_c, sizeof(zclCmdSETunneling_GetSupportedTunnelProtocolsReq_t),(zclGenericReq_t *)pReq);
}


/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

bool_t IsProtocolSupported(uint8_t protocolID)
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelNumberOfSupportedProtocolIDs_c; i++)
  {
    if(protocolIDs[i].protocolID == protocolID)
    {
      return TRUE;
    }
  }
  
  return FALSE;
}

uint8_t getFreeTunnelTableEntry()
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    if(gaZclSETunnelingTable[i].tunnelStatus == gZclSETunnel_TTE_Free_c)
    {
      return i;
    }
  }
  
  return gZclSETunnel_TableFull_c;
}



uint8_t getTunnelTableEntryPending()
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    if(gaZclSETunnelingTable[i].tunnelStatus == gZclSETunnel_TTE_AwaitingTunnelRequestRsp_c)
    {
      return i;
    }
  }
  
  return gZclSETunnel_Fail_c;
}

void CloseTunnel(uint16_t tunnelID)
{
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  
  /* Find tunnel entry data */
  iTTE = getTunnelTableEntry(tunnelID);
  if (iTTE == gZclSETunnel_Fail_c)
    return;
  
  pTTE = &gaZclSETunnelingTable[iTTE];
  
  /* Free stored data*/
  if(pTTE->pTxBuffers)
  {
    FreeSubsequentFragments(pTTE->pTxBuffers);
    MSG_Free(pTTE->pTxBuffers);
  }
  
  BeeUtilZeroMemory(pTTE, sizeof(tunnelingTable_t));
  pTTE->tunnelID = gZclSETunnel_TunnelFail_c;
}
  

void CloseTunnelTimerCallback(zbTmrTimerID_t tmrID)
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    if(gaZclSETunnelingTable[i].closeTunnelTmr == tmrID)
    {
      ZbTMR_FreeTimer(tmrID);
      
      /* Send TunnelClosureNotification command to the client*/
      ZCL_SendTunnelClosureNotification(i);
      
      /* Reset the Tunneling table entry */
      CloseTunnel(gaZclSETunnelingTable[i].tunnelID);
    }
  }
}

void ResetTunnelTimeout(uint8_t iTTE)
{
#if gNum_EndPoints_c != 0    
  uint16_t closeTunnelTimeout;
  zbClusterId_t tunnelClusterId = {gaZclClusterSETunneling_c};
  
  ZbTMR_StopTimer(gaZclSETunnelingTable[iTTE].closeTunnelTmr); 
  (void)ZCL_GetAttribute(EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint), tunnelClusterId , gZclAttrSETunnelingCloseTunnelTimeout_c,  gZclServerAttr_c, &closeTunnelTimeout, NULL);
  ZbTMR_StartSecondTimer(gaZclSETunnelingTable[iTTE].closeTunnelTmr, OTA2Native16(closeTunnelTimeout), CloseTunnelTimerCallback);
#endif  
}

#if gASL_ZclSETunnelingOptionals_d
zbStatus_t ZCL_SETunnel_ReadyRx(zclSETunneling_ReadyRx_t *pReq)
{
  uint8_t iTTE;
  bool_t SendReadyData = FALSE;
  tunnelingTable_t *pTTE;
  zclSETunneling_ReadyDataReq_t readyData;
  
  iTTE = getTunnelTableEntry(pReq->tunnelID);
  pTTE = &gaZclSETunnelingTable[iTTE];
  
  if(pTTE->rxDataLength >= gZclSETunnelRxBufferSize_c )
  {
    /* The ReadyData command must be sent to notify the other device that we can receive data*/
    SendReadyData = TRUE;
  }
  
  pTTE->rxDataLength = 0;
  
  if(SendReadyData)
  {
    readyData.addrInfo = pTTE->partnerAddrInfo;
    readyData.cmdFrame.tunnelId = pReq->tunnelID;
    readyData.cmdFrame.numberOfBytesLeft = pReq->numberOfBytesLeft;
    (void)ZCL_SETunnel_ReadyDataReq(&readyData, pTTE->isClient);
  }
  return gZclSuccess_c;
}
#endif

zbStatus_t ZCL_SETunnel_Server_Process_RequestTunnelReq(zbApsdeDataIndication_t *pIndication)
{
  uint8_t iTTE;
  uint16_t closeTunnelTimeout = 0, maxIncomingTransferSize;
  tunnelingTable_t *pTTE;
  zclSETunneling_RequestTunnelRsp_t requestTunnelResponse;
  zclCmdSETunneling_RequestTunnelReq_t *pReq;  
 #if gNum_EndPoints_c != 0   
  zbClusterId_t tunnelClusterId = {gaZclClusterSETunneling_c};
 #endif  
  pReq = (zclCmdSETunneling_RequestTunnelReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));
  AF_PrepareForReply(&requestTunnelResponse.addrInfo, pIndication);
  requestTunnelResponse.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  
  requestTunnelResponse.cmdFrame.tunnelId = gZclSETunnel_TunnelFail_c;
  requestTunnelResponse.cmdFrame.maximumIncomingtransferSize = 0;
  iTTE = getFreeTunnelTableEntry();
  pTTE = &gaZclSETunnelingTable[iTTE];
  gAcceptTunnel = FALSE;
  /* Notify the application of the tunnel request */
  BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_RequestTunnelReqReceived_c, 0, 0, pReq);
  
  /* Check if the application accepts the tunnel */
  if(iTTE == gZclSETunnel_TableFull_c)
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_Busy_c;
  }
  
  /* Check if we have the ieee address of the partner*/
  else if(!APS_GetIeeeAddress(pIndication->aSrcAddr, pTTE->partnerIeeeAddr))
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_GeneralFailiure_c;
  }
  
  /* Check if the protocol ID is supported */
  else if(!IsProtocolSupported(pReq->protocolID))
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_ProtocolNotSupported_c;
  }
  
#if !gASL_ZclSETunnelingOptionals_d
  /* Check for flow control */
  else if(pReq->flowControlSupport)
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_FlowControlNotSupported_c;
  }
#endif
  
  /* Check for available resources */
  else if(iTTE == gZclSETunnel_TableFull_c)
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_Busy_c;
  }
  
  else if(!gAcceptTunnel)
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_GeneralFailiure_c;
  }
  
  else
  {
    /* Initialize the tunnel */
    pTTE->protocolId = pReq->protocolID;
    pTTE->manufacturerCode = OTA2Native16(pReq->manufacturerCode);
    pTTE->flowControlSupport = pReq->flowControlSupport;
    pTTE->isClient = FALSE;
    
    /* Set the maximum incoming transfer size */
    pTTE->maximumIncomingTransferSize = OTA2Native16(pReq->maximumIncomingtransferSize);
    pTTE->numberOfOctetsLeft = pTTE->maximumIncomingTransferSize;
    
    pTTE->rxDataLength = 0;
    pTTE->txDataLength = 0;   
    
    maxIncomingTransferSize = gZclSETunnelRxBufferSize_c;
    requestTunnelResponse.cmdFrame.maximumIncomingtransferSize = Native2OTA16(maxIncomingTransferSize);
    
#if gNum_EndPoints_c != 0       
    if(ZCL_GetAttribute(EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint), tunnelClusterId , gZclAttrSETunnelingCloseTunnelTimeout_c, gZclServerAttr_c, &closeTunnelTimeout, NULL) != gZbSuccess_c)
    {
      CloseTunnel(iTTE);
      requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_GeneralFailiure_c;
    }
    /* Check if all the tunnel ids were exhausted */
    else 
#endif
    if(tunnelIDCrt == gZclSETunnel_TunnelFail_c)
    {
      CloseTunnel(iTTE);
      requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_NoMoreTunnelIDs_c;
    }
    else
    {
      pTTE->partnerAddrInfo = requestTunnelResponse.addrInfo;
      pTTE->tunnelID = tunnelIDCrt;
      requestTunnelResponse.cmdFrame.tunnelId = Native2OTA16(tunnelIDCrt);
      tunnelIDCrt++;
      pTTE->closeTunnelTmr = ZbTMR_AllocateTimer();
      /* Start the close tunnel timeout timer */
      ZbTMR_StartSecondTimer(pTTE->closeTunnelTmr, OTA2Native16(closeTunnelTimeout), CloseTunnelTimerCallback);
      requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_Success_c;
      pTTE->tunnelStatus = gZclSETunnel_TTE_Active_c;
    }
  } 
  /* Send the response */
  return ZCL_SETunnel_Server_RequestTunnelRsp(&requestTunnelResponse); 
}

zbStatus_t ZCL_SETunnel_Server_Process_CloseTunnelReq(zbApsdeDataIndication_t *pIndication)
{
  zclCmdSETunneling_CloseTunnelReq_t *pReq;
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  zbIeeeAddr_t sourceIeeeAddr;
  
  pReq = (zclCmdSETunneling_CloseTunnelReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
  iTTE = getTunnelTableEntry(pReq->tunnelId);
  
  /* Check if the tunnel exists */
  if(iTTE != gZclSETunnel_Fail_c)
  {
    /* Check if the device that sent the request is authorized to access to tunnel */
    if(APS_GetIeeeAddress(pIndication->aSrcAddr, (uint8_t*)sourceIeeeAddr))
    {
      pTTE = &gaZclSETunnelingTable[iTTE];
      if(Cmp8Bytes(pTTE->partnerIeeeAddr, (uint8_t*)sourceIeeeAddr))
      {
        pTTE = &gaZclSETunnelingTable[iTTE];
        ZbTMR_FreeTimer(pTTE->closeTunnelTmr);
        CloseTunnel(pReq->tunnelId);
      }
    }
  }
  
  return gZclSuccessDefaultRsp_c;
}

zbStatus_t ZCL_SETunnel_Server_Process_GetSupportedProtocolsReq(zbApsdeDataIndication_t *pIndication)
{
  zclCmdSETunneling_GetSupportedTunnelProtocolsReq_t *pReq;
  zclSETunneling_SupportedTunnelingProtocolsRsp_t	 *pResponse;
  uint8_t protocolIDsEndIndex;
  uint8_t i;
  zbStatus_t status;
  
  // Get the request
  pReq  = (zclCmdSETunneling_GetSupportedTunnelProtocolsReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
  
  // If the protocol offset is higher than the number of protocols no response will be sent
  if(pReq->protocolOffset >= gZclSETunnelNumberOfSupportedProtocolIDs_c)
  {
    return gZclSuccess_c;
  }
  
  // Calculate the end index in the protocolIDs list so that the received offset and the end index represent the list of protocols that would be sent in the response. gZclSETunnelNumberOfProtocolIDsInSTPR_c represent the maximum number of protocols that can be sent in a response
  if((gZclSETunnelNumberOfSupportedProtocolIDs_c - pReq->protocolOffset) <= gZclSETunnelNumberOfProtocolIDsInSTPR_c)
  {
    protocolIDsEndIndex = gZclSETunnelNumberOfSupportedProtocolIDs_c;
  }
  else
  {
    protocolIDsEndIndex = pReq->protocolOffset + gZclSETunnelNumberOfProtocolIDsInSTPR_c;
  }
  
  // Allocate memory for the response
  pResponse = MSG_Alloc(sizeof(zclSETunneling_SupportedTunnelingProtocolsRsp_t) + (protocolIDsEndIndex - pReq->protocolOffset - 1) * sizeof(zclSETunneling_Protocol_t));
  // Add the address information
  AF_PrepareForReply(&pResponse->addrInfo, pIndication);
  pResponse->zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  
  // Check if the protocol list is completed
  if(protocolIDsEndIndex == gZclSETunnelNumberOfSupportedProtocolIDs_c)
  {
    pResponse->cmdFrame.protocolListComplete = TRUE;
  }
  else
  {
    pResponse->cmdFrame.protocolListComplete = FALSE;
  }
  
  // Set the number of protocols
  pResponse->cmdFrame.protocolCount = protocolIDsEndIndex - pReq->protocolOffset;
  
  // Add the protocols
  for(i = pReq->protocolOffset; i<protocolIDsEndIndex; i++)
  {
    pResponse->cmdFrame.protocolList[i].manufacturerCode = protocolIDs[i].manufacturerCode;
    pResponse->cmdFrame.protocolList[i].protocolID = protocolIDs[i].protocolID;
  }
  
  // Send the response OTA
  status = ZCL_SETunnel_Server_SupportedTunnelProtocolsRsp(pResponse);
  MSG_Free(pResponse);
  return status;
}

zbStatus_t ZCL_SETunnel_Client_Process_RequestTunnelRsp(zbApsdeDataIndication_t *pIndication)
{
  zclCmdSETunneling_RequestTunnelRsp_t *pRsp;
  
  pRsp = (zclCmdSETunneling_RequestTunnelRsp_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));
  
  if((pRsp->tunnelStatus == gZclSETunnel_Success_c) && (pRsp->tunnelId != gZclSETunnel_TunnelFail_c))
  {
    uint8_t iTTE;
    tunnelingTable_t *pTTE;
    zbIeeeAddr_t sourceIeeeAddr;
    
    iTTE = getTunnelTableEntryPending();
    
    if(iTTE == gZclSETunnel_TableFull_c)
    {
      return gZclSuccess_c;
    }
    
    pTTE = &gaZclSETunnelingTable[iTTE];
    
    if(APS_GetIeeeAddress(pIndication->aSrcAddr, (uint8_t*)sourceIeeeAddr))
    {
      if(!Cmp8Bytes(pTTE->partnerIeeeAddr, (uint8_t*)sourceIeeeAddr))
      {
        return gZclSuccess_c;
      }
    }
    else
    {
      return gZclSuccess_c;
    }
    
    pTTE->tunnelID = OTA2Native16(pRsp->tunnelId);
    pTTE->tunnelStatus = gZclSETunnel_TTE_Active_c;
    pTTE->numberOfOctetsLeft = OTA2Native16(pRsp->maximumIncomingtransferSize);
    pTTE->maximumIncomingTransferSize = OTA2Native16(pRsp->maximumIncomingtransferSize);
  }
  
  BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_TransferDataErrorReceived_c, 0, 0, pRsp);
  return gZclSuccess_c;
}

zbStatus_t ZCL_SETunnel_Process_TransferDataReq(zbApsdeDataIndication_t *pIndication)
{
  zclCmdSETunneling_TransferDataReq_t *pReq;
  zbRxFragmentedHdr_t *pNextFrag;
  zclSETunneling_TransferDataErrorReq_t transferDataError;
  uint8_t iTTE;
  uint16_t  rxLength;
  tunnelingTable_t *pTTE;
  uint8_t status = gZclSETunnel_NoError_c;
  zbIeeeAddr_t sourceIeeeAddr;
  bool_t fIsClient;
  
#if gASL_ZclSETunnelingOptionals_d
  zclSETunneling_AckTransferDataRsp_t transferDataAck;
#endif
  
  pReq = (zclCmdSETunneling_TransferDataReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
  iTTE = getTunnelTableEntry(pReq->tunnelId);
  fIsClient = ((zclFrame_t *)(pIndication->pAsdu))->frameControl & gZclFrameControl_DirectionRsp;
  
  AF_PrepareForReply(&transferDataError.addrInfo, pIndication);
  transferDataError.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  transferDataError.cmdFrame.tunnelId = pReq->tunnelId;
  
  if(iTTE == gZclSETunnel_Fail_c)
  {
    status = gZclSETunnel_NoSuchTunnel_c;
  }
  else
  {
    pTTE = &gaZclSETunnelingTable[iTTE];

    if(APS_GetIeeeAddress(pIndication->aSrcAddr, (uint8_t*)sourceIeeeAddr))
    {
      if(!Cmp8Bytes(pTTE->partnerIeeeAddr, (uint8_t*)sourceIeeeAddr))
      {
        status = gZclSETunnel_WrongDevice_c;
      }
    }
    else
    {
      status = gZclSETunnel_WrongDevice_c;
    }
  }
  
  if(status != gZclSETunnel_NoError_c)
  {
    transferDataError.cmdFrame.transferDataStatus = status;
    
    if(fIsClient)
    {
      return ZCL_SETunnel_Client_TransferDataErrorReq(&transferDataError);
    }
    else
    {
      return ZCL_SETunnel_Server_TransferDataErrorReq(&transferDataError);
    }
  }
  
  pNextFrag = pIndication->fragmentHdr.pNextDataBlock;
  rxLength = pIndication->asduLength - sizeof(zclFrame_t) - sizeof(uint16_t);//tunnel id is 2 octests
  
  /* Get the length of the packet */
  while(pNextFrag)
  {
    rxLength += pNextFrag->iDataSize;
    pNextFrag = pNextFrag->pNextDataBlock;
  }
    
  /* Check for data overflow */
  if(pTTE->rxDataLength + rxLength > gZclSETunnelRxBufferSize_c)
  {
    transferDataError.cmdFrame.transferDataStatus = gZclSETunnel_DataOverflow_c;
    
    if(fIsClient)
    {
      return ZCL_SETunnel_Client_TransferDataErrorReq(&transferDataError);
    }
    else
    {
      return ZCL_SETunnel_Server_TransferDataErrorReq(&transferDataError);
    }
  }
  
  pTTE->rxDataLength += rxLength;
  
  if(!pTTE->isClient)
  {
    ResetTunnelTimeout(iTTE);
  }
  
  BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_TransferDataIndication_c, 0, 0, pIndication);
  
#if gASL_ZclSETunnelingOptionals_d
  if (pTTE->flowControlSupport)
  {
    transferDataAck.cmdFrame.numberOfBytesLeft = gZclSETunnelRxBufferSize_c - pTTE->rxDataLength;
    transferDataAck.cmdFrame.tunnelId = pTTE->tunnelID;
    transferDataAck.addrInfo = pTTE->partnerAddrInfo;
    if(pTTE->isClient)
    {
      return ZCL_SETunnel_Client_AckTransferDataRsp(&transferDataAck);
    }
    else
    {
      return ZCL_SETunnel_Server_AckTransferDataRsp(&transferDataAck);
    }
  }
#endif
  return gZclSuccessDefaultRsp_c;
}

#if gASL_ZclSETunnelingOptionals_d

zbStatus_t ZCL_SETunnel_Process_AckTransferDataRsp(zbApsdeDataIndication_t *pIndication)
{
  zclCmdSETunneling_AckTransferDataRsp_t *pRsp;
  zclSETunneling_TransferDataReq_t *pTransferDataReq;
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  
  pRsp = (zclCmdSETunneling_AckTransferDataRsp_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
  iTTE = getTunnelTableEntry(pRsp->tunnelId);
  
  if(iTTE == gZclSETunnel_Fail_c)
  {
    return gZclSuccessDefaultRsp_c;
  }
  
  pTTE = &gaZclSETunnelingTable[iTTE];
  pTTE->numberOfOctetsLeft = OTA2Native16(pRsp->numberOfBytesLeft);
  
  if((OTA2Native16(pRsp->numberOfBytesLeft) > 0) && (pTTE->txDataLength > 0))
  {
    (void)ZCL_SETunnel_TransferDataReq(pTTE->tunnelID);
  }
  
  return gZclSuccessDefaultRsp_c;
}

zbStatus_t ZCL_SETunnel_Process_ReadyDataReq(zbApsdeDataIndication_t *pIndication)
{
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  zclCmdSETunneling_ReadyDataReq_t *pReq;
  zclSETunneling_TransferDataReq_t *pTransferDataReq;
  
  
  pReq = (zclCmdSETunneling_ReadyDataReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
  iTTE = getTunnelTableEntry(OTA2Native16(pReq->tunnelId));
  
  if(iTTE == gZclSETunnel_Fail_c)
  {
    return gZclSuccessDefaultRsp_c;
  }
  
  pTTE = &gaZclSETunnelingTable[iTTE];
  pTTE->numberOfOctetsLeft = OTA2Native16(pReq->numberOfBytesLeft);
  if(!pTTE->isClient)
  {
    ResetTunnelTimeout(iTTE);
  }
  
  if(pTTE->txDataLength > 0)
  {
    (void)ZCL_SETunnel_TransferDataReq(pTTE->tunnelID);
  }
  
  return gZclSuccessDefaultRsp_c;
}

#endif


#if gASL_ZclSETunneling_d
/*!
 * @fn 		zbStatus_t ZCL_SETunnel_LoadFragment(zclSETunneling_LoadFragment_t *pReq)
 *
 * @brief	Loads fragments of data to be tunneled.
 *
 */
zbStatus_t ZCL_SETunnel_LoadFragment(zclSETunneling_LoadFragment_t *pReq)
{
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  
  if(OTA2Native16(pReq->length) > ApsmeGetMaxFragBufferSize())
  {
    return gZclFailure_c;
  }
  
  iTTE = getTunnelTableEntry(pReq->tunnelId);
  pTTE = &gaZclSETunnelingTable[iTTE];
  
  if (pTTE->txDataLength == 0)
  {
    zbTxFragmentedHdr_t *pBuffer;
    
    /* This is the first buffer of the data packet */
    pBuffer = MSG_Alloc(ApsmeGetMaxFragBufferSize());
    
    if (!pBuffer)
      return gZclFailure_c;
    
    pTTE->pTxBuffers = pBuffer;
    pBuffer->iDataSize = pReq->length;
    pBuffer->iMsgType = gApsdeDataReqFragMsgType_d;
    pBuffer->pNextDataBlock = NULL;
    FLib_MemCpy((uint8_t *)pBuffer + sizeof(zbTxFragmentedHdr_t), &pReq->data, pReq->length);
  }
  else
  {
    zbTxFragmentedHdr_t *pCurrentBuffer = pTTE->pTxBuffers;
    
    while(pCurrentBuffer->pNextDataBlock)
      pCurrentBuffer = pCurrentBuffer->pNextDataBlock;
    
    if (pCurrentBuffer->iDataSize + pReq->length > ApsmeGetMaxFragBufferSize())
    {
      uint8_t splitPayloadLen = ApsmeGetMaxFragBufferSize() - pCurrentBuffer->iDataSize;
      
      zbTxFragmentedHdr_t *pBuffer = MSG_Alloc(ApsmeGetMaxFragBufferSize());
      
      
      if (!pBuffer)
        return gZclFailure_c;
      
      /* Copy first part of the payload*/
      FLib_MemCpy((uint8_t *)pCurrentBuffer + sizeof(zbTxFragmentedHdr_t) + pCurrentBuffer->iDataSize, &pReq->data, splitPayloadLen);
      pCurrentBuffer->iDataSize = ApsmeGetMaxFragBufferSize();
      
      /* Copy second part of the payload*/
      FLib_MemCpy((uint8_t *)pBuffer + sizeof(zbTxFragmentedHdr_t), &pReq->data[splitPayloadLen], pReq->length - splitPayloadLen);
      pBuffer->iDataSize = pReq->length - splitPayloadLen;
      pCurrentBuffer->pNextDataBlock = pBuffer;
      pBuffer->iMsgType = gApsdeDataReqFragMsgType_d;
      pBuffer->pNextDataBlock = NULL;
    }
    else
    {
      FLib_MemCpy((uint8_t *)pCurrentBuffer + sizeof(zbTxFragmentedHdr_t) + pCurrentBuffer->iDataSize, &pReq->data, pReq->length);
      pCurrentBuffer->iDataSize += pReq->length;
    }
  }
  pTTE->txDataLength += pReq->length;
    
  return gZclSuccess_c;
}

zbStatus_t ZCL_SETunnel_ZTCTransferDataReq(zclSETunneling_ZTCTransferDataReq_t *pReq)
{
  return ZCL_SETunnel_TransferDataReq(OTA2Native16(pReq->tunnelId));
}

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_TransferDataReq(uint16_t tunnelId)
 *
 * @brief	Sends the cached data to the tunnel partner.
 *
 */
zbStatus_t ZCL_SETunnel_TransferDataReq
(
  uint16_t tunnelId
)
{
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  uint16_t length, requestLength;
  afToApsdeMessage_t *pMsg;
  uint8_t frameCtl, cmd;
  zbStatus_t status;
  afAddrInfo_t addrInfo;
  
  iTTE = getTunnelTableEntry(tunnelId);
  
  if(iTTE == gZclSETunnel_TableFull_c)
  {
    return gZclFailure_c;
  }
  pTTE = &gaZclSETunnelingTable[iTTE];
  
  requestLength = pTTE->txDataLength;
  
  addrInfo = pTTE->partnerAddrInfo;
  
#if gASL_ZclSETunnelingOptionals_d
    
  if(requestLength > pTTE->numberOfOctetsLeft)
  {
    if(pTTE->flowControlSupport)
    {
      requestLength = pTTE->numberOfOctetsLeft;
    }
    else
    {
      return gZclFailure_c;
    }
  }
#else
  if(requestLength > pTTE->maximumIncomingTransferSize)
  {
    return gZclFailure_c;
  }
#endif
  
  frameCtl = gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp | 
                        (gZclFrameControl_DirectionRsp * !pTTE->isClient);
  cmd = gZclCmdSETunneling_Server_TransferData_c + pTTE->isClient;
  
  length = sizeof(uint16_t);
  
  /* Create the first frame with only */
  pMsg = ZCL_CreateFrame( &addrInfo, cmd, frameCtl, &ZbZclFoundationGlobals(gZclTransactionId), (uint8_t*)&length, &pTTE->tunnelID);
  
  if(!pMsg)
    return gZclNoMem_c;
  
  /* Build the Tunnel Data frame */
  if (requestLength > 0)
    length += AddTunnelData(pTTE, pMsg, requestLength); 
  
  
  /* Send response OTA */
  status = ZCL_DataRequestNoCopy(&addrInfo, (uint8_t)length , pMsg);
  
  if (status == gZclSuccess_c)
  {
    ZbZclFoundationGlobals(gZclTransactionId)++;
      
    /* Reset the tunnel timeout on the server because we are sending data*/
    if(!pTTE->isClient) 
    {
      ResetTunnelTimeout(iTTE);
    }
  }

  return status;
}
#endif

/*!
 * @fn 		zbStatus_t ZCL_SendTunnelClosureNotification(index_t tableIdx) 
 *
 * @brief	Sends a Tunnel Closure Notification from the server when the tunnel times out.
 *
 */
zbStatus_t ZCL_SendTunnelClosureNotification(index_t tableIdx) 
{
  zclSETunneling_TunnelClosureNotification_t req;    
  FLib_MemCpy(&req.addrInfo, &gaZclSETunnelingTable[tableIdx].partnerAddrInfo, sizeof(afAddrInfo_t));	
  req.zclTransactionId =  ZbZclFoundationGlobals(gZclTransactionId)++;
  req.cmdFrame.tunnelId = gaZclSETunnelingTable[tableIdx].tunnelID;
  return ZCL_SETunnel_Server_TunnelClosureNotificationReq(&req);
}

/*!
 * @fn 		static uint16_t AddTunnelData(tunnelingTable_t* pTTE, afToApsdeMessage_t * pMsg, uint16_t requestLength)
 *
 * @brief	Builds the tunneling data to be sent over-the-air
 *
 */
#if gASL_ZclSETunneling_d
static uint16_t AddTunnelData(tunnelingTable_t* pTTE, afToApsdeMessage_t * pMsg, uint16_t requestLength)
{
  uint8_t bufferLen;
  uint16_t len;
  zbTxFragmentedHdr_t *pFrag = &pMsg->msgData.dataReq.fragmentHdr;
  uint8_t *pPayload; 
  
  bufferLen = ApsmeGetMaxFragBufferSize() - (gAsduOffset_c + sizeof(zclFrame_t) + sizeof(uint16_t));
  
  /* Add octets to the first data request buffer*/
  if (bufferLen > requestLength)
  {
    pPayload = (uint8_t*)pTTE->pTxBuffers + sizeof(zbTxFragmentedHdr_t);
    FLib_MemCpy((uint8_t*) pMsg + gAsduOffset_c + sizeof(zclFrame_t) + sizeof(uint16_t), pPayload, requestLength);
    pFrag->iDataSize = requestLength + sizeof(zclFrame_t) + sizeof(uint16_t);
    pTTE->txDataLength -= requestLength;
    pTTE->pTxBuffers->iDataSize -= requestLength;
    /* Shift remaining payload */
    FLib_MemInPlaceCpy(pPayload, pPayload + requestLength, pTTE->pTxBuffers->iDataSize);
    
    /* Free the buffer if we empty it */
    if (!pTTE->pTxBuffers->iDataSize)
    {
      zbTxFragmentedHdr_t* pBuffer = pTTE->pTxBuffers->pNextDataBlock;
      MSG_Free(pTTE->pTxBuffers);
      pTTE->pTxBuffers = pBuffer;
    }
    
    return requestLength;
  }
  else
  {
    len = requestLength;
    
    pFrag->iDataSize = sizeof(uint16_t) + sizeof(zclFrame_t);

    /* Add remaining payload in subsequent buffers */
    while(len)
    {  
      /* Use the same buffer for sending the data*/
      if (pTTE->pTxBuffers->iDataSize <= len)
      {
        len -= pTTE->pTxBuffers->iDataSize;
        pTTE->txDataLength -= pTTE->pTxBuffers->iDataSize;
        
        /* Add message to the data request fragment list*/
        pFrag->pNextDataBlock = pTTE->pTxBuffers;
        
        /* Remove message from the buffer list*/  
        pTTE->pTxBuffers = pTTE->pTxBuffers->pNextDataBlock;
        
        /* Point to the newest fragment */
        pFrag = pFrag->pNextDataBlock;
        if (pFrag)
          pFrag->pNextDataBlock = NULL;
        
      }
      else
      {
        pPayload = (uint8_t*)pTTE->pTxBuffers + sizeof(zbTxFragmentedHdr_t);
        
        /* We need to create a new fragment for the remaining data*/
        if (!AF_MsgAppendData(pMsg, pPayload, len))
        {
          return requestLength - len;
        }
                    
        pTTE->txDataLength -= len;
        pTTE->pTxBuffers->iDataSize -= len;
        /* Shift remaining payload */
        FLib_MemInPlaceCpy(pPayload, pPayload + len,  pTTE->pTxBuffers->iDataSize);  
        len = 0;
      }
    }
   return requestLength;
  }
}
#endif
