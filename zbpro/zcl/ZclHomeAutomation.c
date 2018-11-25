/*! @file 	  ZclHomeAutomation.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for the Home Automation domain: Appliance Identification, Meter Identification,
 *			  Appliance Events and Alerts, Appliance Statistics.
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
 *			list of conditions and the following disclaimer in the documentation and/or
 *			other materials provided with the distribution.
 *
 *			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *			contributors may be used to endorse or promote products derived from this
 *			software without specific prior written permission.
 *
 *			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *			ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *			WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 *			ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *			(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *			LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *			ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *			(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *			SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/* 
 *  [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
 *  [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf
*/

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "BeeAppInit.h"
#include "ASL_ZdpInterface.h"
#include "ZdoApsInterface.h"
#include "HaProfile.h"
#include "ZclHomeAutomation.h"

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
#if gZclEnableApplianceStatistics_d
static void ApplianceStatistics_UpdateLogIdTable(void);
static void ApplianceStatistics_InsertDataInMemory(uint8_t *data, uint32_t address, uint32_t count);
static void ApplianceStatistics_GetLogPayloadData(uint8_t *payload, uint32_t offset, uint32_t count);

static bool_t ApplianceStatus_GetLogIdInformation(void);

static zbStatus_t zclApplianceStatistics_LogReq_Handler(afAddrInfo_t addrInfo, uint8_t  transactionId, uint8_t commandId);
static zbStatus_t zclApplianceStatistics_LogQueueReq_Handler(afAddrInfo_t addrInfo, uint8_t transactionId,uint8_t commandId);
static zbStatus_t zclApplianceStatistics_LogRspOrNotif_Handler(zclCmdApplianceStatistics_LogRspOrNotif_t *pCmdPayload,bool_t withPartition);

#if gZclEnablePartition_d  
static void ZCL_ApplianceStatisticsPartitionServer(zclPartitionedDataIndication_t* partitionDataIndication);
static uint8_t* ApplianceStatisticsPartitionCallback(zclPartitionAppEvent_t* pPartitionEvent, uint8_t* dataLen);
static zbStatus_t ApplianceStatistics_TransferDataPtr(zclApplianceStatistics_LogRspOrNotif_t *pReq); 
#endif /* gZclEnablePartition_d */

#endif /* gZclEnableApplianceStatistics_d */


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/
#if gZclEnableApplianceStatistics_d

#if gZclEnablePartition_d
uint8_t gCommandToSendUsingPartition;
static ApplianceStatisticsPartitionTxFrameBuffer_t* gpPartitionTxPendingFrame;   /* used for partition cluster*/
static ApplianceStatisticsPartitionRxFrameBuffer_t gPartitionRxPendingFrame;     /* used for partition cluster*/
#endif /* gZclEnablePartition_d */
zclApplianceStatistics_LogInf_t gApplianceStatistics_CurrentLogInf; /* used for ApplianceStatistics to store Current Log Information - Id, Length, Address */
uint32_t gAvailableLogIdTable[gApplianceStatistics_MaxLogId_c];
zclPairDeviceInf_t gApplianceStatisticsServerInf;

#endif /* gZclEnableApplianceStatistics_d */


#if gZclEnableApplianceEventsAlerts_d
uint8_t gCurrentNumberOfAlerts = 0;
zclApplAlertsEvents_AlertsStructure_t gApplAlertsEventsTable[gApplEventsAlerts_MaxAlertsSupported_c]; 
#endif /* gZclEnableApplianceEventsAlerts_d */


/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

#if gZclEnableApplianceStatistics_d
/******************************
  Appliance Statistics Cluster 
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.11 [R1]
*******************************/
/* Appliance Statistics Cluster Attribute Definitions */
const zclAttrDef_t gaZclApplianceStatisticsClusterAttrDef[] = {
  { gZclAttrApplianceStatisticsLogMaxSizeId_c,      gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t),(void *) MbrOfs(zclApplianceStatisticsAttrsRAM_t, logMaxSize)},
  { gZclAttrApplianceStatisticsLogQueueMaxSizeId_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) MbrOfs(zclApplianceStatisticsAttrsRAM_t, logQueueMaxSize)}
};

const zclAttrSet_t gaZclApplianceStatisticsClusterAttrSet[] = {
  {gZclAttrSetApplianceStatisticsInformation_c, (void *)&gaZclApplianceStatisticsClusterAttrDef, NumberOfElements(gaZclApplianceStatisticsClusterAttrDef)}
};

const zclAttrSetList_t gZclApplianceStatisticsClusterAttrSetList = {
  NumberOfElements(gaZclApplianceStatisticsClusterAttrSet),
  gaZclApplianceStatisticsClusterAttrSet
};

const zclCmd_t gaZclApplianceStatisticsClusterCmdReceivedDef[]={
  // commands received 
  gZclCmdApplianceStatistics_LogReq_c,
  gZclCmdApplianceStatistics_LogQueueReq_c
};
const zclCmd_t gaZclApplianceStatisticsClusterCmdGeneratedDef[]={
  // commands generated 
  gZclCmdApplianceStatistics_LogNotification_c,
  gZclCmdApplianceStatistics_LogResponse_c,
  gZclCmdApplianceStatistics_LogQueueResponse_c,
  gZclCmdApplianceStatistics_StatisticsAvailable_c
};

const zclCommandsDefList_t gZclApplianceStatisticsClusterCommandsDefList =
{
   NumberOfElements(gaZclApplianceStatisticsClusterCmdReceivedDef), gaZclApplianceStatisticsClusterCmdReceivedDef,
   NumberOfElements(gaZclApplianceStatisticsClusterCmdGeneratedDef), gaZclApplianceStatisticsClusterCmdGeneratedDef
};

/******************************
  Appliance Statistics Cluster 
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.11 [R1]
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_ApplianceStatisticsClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ApplianceStatistics Cluster Server. 
 *
 */
zbStatus_t ZCL_ApplianceStatisticsClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 	/* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    				/* IN: MUST be set to the endpoint's device data */
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterApplianceStatistics_c}, 0, gApsTxOptionNone_c, 1};
  zbStatus_t status = gZbSuccess_c;
  
  /* to prevent compiler warning */
  (void)pDevice;
  
   pFrame = (void *)pIndication->pAsdu;
   
   /* Create the destination address */
   AF_PrepareForReply(&addrInfo, pIndication); 

   /* handle incoming server commands */
   switch (pFrame->command) 
   {
      case gZclCmdApplianceStatistics_LogReq_c:
      {
        FLib_MemCpy(&gApplianceStatistics_CurrentLogInf.logId,(pFrame + 1), sizeof(zclCmdApplianceStatistics_LogReq_t));
        gApplianceStatistics_CurrentLogInf.logId  = OTA2Native32(gApplianceStatistics_CurrentLogInf.logId);
        status = zclApplianceStatistics_LogReq_Handler(addrInfo, pFrame->transactionId, gZclCmdApplianceStatistics_LogResponse_c);
        break;
      }
      
      case gZclCmdApplianceStatistics_LogQueueReq_c:
      case gZclCmdDefaultRsp_c:  
      {
        uint8_t commandId = gZclCmdApplianceStatistics_LogQueueResponse_c;
        if(pFrame->command == gZclCmdDefaultRsp_c)
        {
            zclCmdDefaultRsp_t * pDefaultRsp;
            pDefaultRsp = ZCL_GetPayload(pIndication);
            /* when receive an DefaultResponse with an error status - for log notification command - notify the client that are statistics available */
            if(((uint8_t)(pDefaultRsp->status) == gZclSuccess_c)||(pDefaultRsp->cmd != gZclCmdApplianceStatistics_LogNotification_c))
            {
              return gZclSuccess_c;
            }
            commandId = gZclCmdApplianceStatistics_StatisticsAvailable_c;
        }
        status = zclApplianceStatistics_LogQueueReq_Handler(addrInfo, pFrame->transactionId, commandId);
        break;
      }
      
      default:
        return  gZclUnsupportedClusterCommand_c;                     
  }
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_ApplianceStatisticsClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ApplianceStatistics Cluster Client. 
 *
 */
zbStatus_t ZCL_ApplianceStatisticsClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterApplianceStatistics_c}, 0, gApsTxOptionNone_c, 1};
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  zbStatus_t statusCmd = gZclSuccess_c;
 
  /* prevent compiler warning */
  (void)pDevice;

  /* Create the destination address */
  AF_PrepareForReply(&addrInfo, pIndication);  
  
  pFrame = (void *)pIndication->pAsdu;
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	  status = gZclSuccess_c;
    
  pFrame = (void *)pIndication->pAsdu;
  
  /* handle incoming client commands */
  switch(pFrame->command)
  {
     case gZclCmdApplianceStatistics_LogNotification_c:
     case gZclCmdApplianceStatistics_LogResponse_c:
     {
         zclCmdApplianceStatistics_LogRspOrNotif_t *pCmdPayload;
         pCmdPayload = (void *)(pFrame+1);
         (void)zclApplianceStatistics_LogRspOrNotif_Handler(pCmdPayload, FALSE);
         
         /* check for next LogId available */
         if(gAvailableLogIdTable[0]!=0x00)
         {        
           zclApplianceStatistics_LogReq_t logReqCmd;
           /* generate a logRequest */
           FLib_MemCpy(&logReqCmd.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
           logReqCmd.cmdFrame.LogID = gAvailableLogIdTable[0];
           statusCmd = zclApplianceStatistics_LogRequest(&logReqCmd);  
           if(statusCmd == gZbSuccess_c)
              ApplianceStatistics_UpdateLogIdTable();
         }
         break;
     }
       
     case gZclCmdApplianceStatistics_LogQueueResponse_c:
     case gZclCmdApplianceStatistics_StatisticsAvailable_c:  
     {
         zclCmdApplianceStatistics_LogQueueResponse_t *pCmdPayload;
         zclApplianceStatistics_LogReq_t logReqCmd;

         pCmdPayload = (void *)(pFrame+1); 
         if(pCmdPayload->LoqQueueSize == 0)
         {
            return status;
         }
         /* store loqIDs */ 
         FLib_MemCpy(&gAvailableLogIdTable[0], &pCmdPayload->LogID[0], sizeof(uint32_t)*pCmdPayload->LoqQueueSize);

         /* store server information */
         Copy2Bytes(gApplianceStatisticsServerInf.aDestAddress, pIndication->aSrcAddr);
         gApplianceStatisticsServerInf.endPoint =  pIndication->srcEndPoint;
         
         /* generate a logRequest */
         FLib_MemCpy(&logReqCmd.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
         logReqCmd.cmdFrame.LogID = gAvailableLogIdTable[0];
         statusCmd = zclApplianceStatistics_LogRequest(&logReqCmd);
         if(statusCmd == gZbSuccess_c)
        	 ApplianceStatistics_UpdateLogIdTable();
         break;
     }
       
     default:
         status = gZclUnsupportedClusterCommand_c;    
  }
    
  return status;
}

/*!
 * @fn 		zbStatus_t zclApplianceStatistics_LogRequest(zclApplianceStatistics_LogReq_t *pReq) 
 *
 * @brief	Sends over-the-air a LogRequest command from the Appliance Statistics Cluster Client. 
 *
 */
zbStatus_t zclApplianceStatistics_LogRequest
( 
  zclApplianceStatistics_LogReq_t *pReq
) 
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterApplianceStatistics_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdApplianceStatistics_LogReq_c,sizeof(zclCmdApplianceStatistics_LogReq_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclApplianceStatistics_LogRequest(zclApplianceStatistics_LogReq_t *pReq) 
 *
 * @brief	Process LogRequest command received from the Appliance Statistics Cluster Client. 
 *
 */
static zbStatus_t zclApplianceStatistics_LogReq_Handler
(
  afAddrInfo_t addrInfo, 
  uint8_t      transactionId,
  uint8_t      commandId  
)
{
  zclApplianceStatistics_LogRspOrNotif_t *pCommandRsp;
  zbStatus_t status = gZbSuccess_c;
  uint32_t payloadLen;
  uint32_t  maxLogSizeAttr = 0x00;
  /* check if the logId is available and obtain logId information - length, address*/
  if(!ApplianceStatus_GetLogIdInformation())
    return gZclNotFound_c;  
  (void)ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrApplianceStatisticsLogMaxSize_c, gZclServerAttr_c,  &maxLogSizeAttr, NULL);
  /* compare the logLength with maxLogSize attribute */
  if(maxLogSizeAttr < gApplianceStatistics_CurrentLogInf.length)
    return gZclNotAuthorized_c;
  /* prepare for Log response command */
  pCommandRsp = AF_MsgAlloc(); 
  if(!pCommandRsp)
    return gZclNoMem_c;
  FLib_MemCpy(&pCommandRsp->addrInfo, &addrInfo, sizeof(afAddrInfo_t));
  pCommandRsp->zclTransactionId = transactionId;
#if (!gZclClusterOptionals_d)  
  pCommandRsp->cmdFrame.TimeStamp = 0xFFFFFFFF; 
#else  
  pCommandRsp->cmdFrame.TimeStamp = Native2OTA32(ZCL_GetUTCTime(addrInfo.srcEndPoint)); 
#endif  
  pCommandRsp->cmdFrame.LogID = Native2OTA32(gApplianceStatistics_CurrentLogInf.logId);
  pCommandRsp->cmdFrame.LogLength = Native2OTA32(gApplianceStatistics_CurrentLogInf.length);
  payloadLen = gApplianceStatistics_CurrentLogInf.length + 3*sizeof(uint32_t);
  
  if(payloadLen < gApplianceStatistics_PayloadLogMaxSize_c)
  {
    /* don't use the partition cluster to transfer the data */
    ApplianceStatistics_GetLogPayloadData(&pCommandRsp->cmdFrame.LogPayload[0], 0x00, gApplianceStatistics_CurrentLogInf.length);
    status = zclApplianceStatistics_LogRspOrNotif(pCommandRsp, commandId);
  }
#if gZclEnablePartition_d   
  else /* use the partition cluster to transfer the data */     
  {
	gCommandToSendUsingPartition = commandId;
    status = (ApplianceStatistics_TransferDataPtr(pCommandRsp)==gZbSuccess_c)?gZbSuccess_c:gZbFailed_c;
  }
#endif  
  MSG_Free(pCommandRsp);
  return status;
}

/*!
 * @fn 		zbStatus_t zclApplianceStatistics_LogRspOrNotif(zclApplianceStatistics_LogRspOrNotif_t *pCommandRsp, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a LogResponse/LogNotification command from the Appliance Statistics Cluster Server.
 *
 */
zbStatus_t zclApplianceStatistics_LogRspOrNotif
( 
  zclApplianceStatistics_LogRspOrNotif_t *pCommandRsp, 
  uint8_t commandId
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = OTA2Native32(pCommandRsp->cmdFrame.LogLength)  + 3*sizeof(uint32_t);
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          commandId,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &pCommandRsp->zclTransactionId, 
                          &iPayloadLen,
                          (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
 
 return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t zclApplianceStatistics_LogRspOrNotif_Handler(zclCmdApplianceStatistics_LogRspOrNotif_t *pCmdPayload, bool_t withPartition) 
 *
 * @brief	Process LogResponse/LogNotification command received from the Appliance Statistics Cluster Server. 
 *
 */
static zbStatus_t zclApplianceStatistics_LogRspOrNotif_Handler
(
  zclCmdApplianceStatistics_LogRspOrNotif_t *pCmdPayload,
  bool_t withPartition
)
{
  /* check command Payload */
  pCmdPayload->LogID = OTA2Native32(pCmdPayload->LogID);
  
  /* process command - TO DO: */
  if(withPartition == FALSE)
    ApplianceStatistics_InsertDataInMemory(pCmdPayload->LogPayload, 0x0000, pCmdPayload->LogLength);
  return gZclSuccess_c;  
}

/*!
 * @fn 		zbStatus_t zclApplianceStatistics_LogQueueReq(zclApplianceStatistics_LogQueueReq_t *pReq) 
 *
 * @brief	Sends over-the-air a LogQueueRequest command from the Appliance Statistics Cluster Client.
 *
 */
zbStatus_t zclApplianceStatistics_LogQueueReq
(
  zclApplianceStatistics_LogQueueReq_t *pReq
)
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterApplianceStatistics_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdApplianceStatistics_LogQueueReq_c, 0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclApplianceStatistics_LogQueueReq(zclApplianceStatistics_LogQueueReq_t *pReq) 
 *
 * @brief	Process a LogQueueRequest command received from the Appliance Statistics Cluster Client.
 *
 */
static zbStatus_t zclApplianceStatistics_LogQueueReq_Handler
(
  afAddrInfo_t addrInfo, 
  uint8_t      transactionId,
  uint8_t      commandId  
)
{
  uint8_t maxLogQueueSize = 0, i=0;
  zbStatus_t status;
  /* prepare to generate a Log queue response or StatisticsAvailable command */
  zclApplianceStatistics_LogQueueResponse_t *pCommandRsp;
  pCommandRsp = AF_MsgAlloc(); 
  if(!pCommandRsp)
    return gZclNoMem_c;
  /* get loqMaxQueueMax attribute */
  (void)ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrApplianceStatisticsLogQueueMaxSize_c, gZclServerAttr_c, &maxLogQueueSize, NULL);
  if (gApplianceStatistics_MaxLogId_c > maxLogQueueSize)
     status = gZclFailure_c;
  else
  {
     FLib_MemCpy(&pCommandRsp->addrInfo, &addrInfo, sizeof(afAddrInfo_t));
     pCommandRsp->zclTransactionId = transactionId;
     pCommandRsp->cmdFrame.LoqQueueSize = gApplianceStatistics_MaxLogId_c;
     if(gApplianceStatistics_MaxLogId_c > 0)
       for(i=1;i<gApplianceStatistics_MaxLogId_c+1;i++)
         pCommandRsp->cmdFrame.LogID[i-1] = Native2OTA32(i);
     status = ApplianceStatistics_StatisticsAvailableOrLogQueueResponse(pCommandRsp, commandId);
  }
  MSG_Free(pCommandRsp);
  return status;
}

/*!
 * @fn 		zbStatus_t ApplianceStatistics_StatisticsAvailableOrLogQueueResponse(zclApplianceStatistics_LogQueueResponse_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a StatisticAvailable/LogQueueResponse command from the Appliance Statistics Cluster Server.
 *
 */
zbStatus_t ApplianceStatistics_StatisticsAvailableOrLogQueueResponse
( 
  zclApplianceStatistics_LogQueueResponse_t *pReq, 
  uint8_t commandId
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = 4*pReq->cmdFrame.LoqQueueSize  + 1;
  pMsg = ZCL_CreateFrame( &(pReq->addrInfo), 
                          commandId,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp, 
                          &pReq->zclTransactionId, 
                          &iPayloadLen,
                          (uint8_t*)&(pReq->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
 
 return ZCL_DataRequestNoCopy(&(pReq->addrInfo),  iPayloadLen, pMsg);  
}

 /*!
 * @fn 		static bool_t ApplianceStatus_GetLogIdInformation(void)
 *
 * @brief	Get current LogID information. Return - TRUE if the LogId information are available or FALSE otherwise
 *
 */
static bool_t ApplianceStatus_GetLogIdInformation(void)
{
  bool_t status = FALSE;
  
  /* To do: check for LogId in the Memory and set log information length and start address */   
  if ((gApplianceStatistics_MaxLogId_c==0)||(gApplianceStatistics_CurrentLogInf.logId == 0x00))
    return FALSE;
  
  /* for demo only */
  if(gApplianceStatistics_CurrentLogInf.logId < gApplianceStatistics_MaxLogId_c/2)
  {
    gApplianceStatistics_CurrentLogInf.length  = (gApplianceStatistics_CurrentLogInf.logId == 0x01)?gApplianceStatistics_LogsWithPartitionLength2_c:gApplianceStatistics_LogsWithPartitionLength1_c;  
    gApplianceStatistics_CurrentLogInf.address =0;     
    status = TRUE;    
  }
  else
    if(gApplianceStatistics_CurrentLogInf.logId <=  gApplianceStatistics_MaxLogId_c)
    {
      gApplianceStatistics_CurrentLogInf.length  = gApplianceStatistics_LogsFittingInSinglePacketLength_c;  
      gApplianceStatistics_CurrentLogInf.address =0;     
      status = TRUE;  
    }
    else
      status = FALSE;
      
  return status;
}

 /*!
 * @fn 		static void ApplianceStatistics_GetLogPayloadData(uint8_t *payload, uint32_t offset, uint32_t count)
 *
 * @brief	Get Log Payload data
 *
 */
static void ApplianceStatistics_GetLogPayloadData(uint8_t *payload, uint32_t offset, uint32_t count)
{
#if gZclEnableApplianceStatisticsServerOptionals_d 
  /* for Demo - all logPayload Data is set to value 0x02 */  
  uint32_t i = 0;
  for(i=offset+gApplianceStatistics_CurrentLogInf.address+1; i < (count + offset + gApplianceStatistics_CurrentLogInf.address); i++)
     payload[i - offset - gApplianceStatistics_CurrentLogInf.address] = 0x02;
  payload[0] = (uint8_t)(offset/gDefaultPartitionAttrs.partitionedFrameSize);
#endif
}

 /*!
 * @fn 		static void ApplianceStatistics_InsertDataInMemory(uint8_t *data, uint32_t address, uint32_t count)
 *
 * @brief	Keep Data received using Partition Cluster
 *
 */
static void ApplianceStatistics_InsertDataInMemory(uint8_t *data, uint32_t address, uint32_t count)
{
  /* To Do */
}

 /*!
 * @fn 		static void ApplianceStatistics_UpdateLogIdTable(void)
 *
 * @brief	update LogId information
 *
 */
static void ApplianceStatistics_UpdateLogIdTable(void)
{
  uint8_t i= 0;
  for(i=0;i<gApplianceStatistics_MaxLogId_c-1;i++)
    gAvailableLogIdTable[i] = gAvailableLogIdTable[i+1];
  gAvailableLogIdTable[gApplianceStatistics_MaxLogId_c-1] = 0x00;
}

#if gZclEnablePartition_d   

 /*!
 * @fn 		void ZCL_ApplianceStatisticsInit(void)
 *
 * @brief   Init ApplianceStatistic - Partition configuration. In case of messages that will not fit into a single Zigbee payload, 
 *			the Partition Cluster will be used (managed by the application). This function will register the ApllianceStatistics to Partition Cluster.
 *
 */
void ZCL_ApplianceStatisticsInit(void)
{
  zbClusterId_t aClusterId ={gaZclClusterApplianceStatistics_c};
  zclPartitionedClusterInfo_t clusterReg;
  Copy2Bytes(&clusterReg.clusterId, aClusterId);
  clusterReg.dataIndHandler = ZCL_ApplianceStatisticsPartitionServer;
  clusterReg.dataReqHandler = ApplianceStatisticsPartitionCallback;
  clusterReg.clusterBufferSize = gMaxPartitionBuffer;
  
  gpPartitionTxPendingFrame = NULL;
  gPartitionRxPendingFrame.framePending = FALSE;  
  
  (void)ZCL_PartitionRegisterCluster(&clusterReg);
}

/*!
 * @fn 		static uint8_t* ApplianceStatisticsPartitionCallback(zclPartitionAppEvent_t* pPartitionEvent, uint8_t* dataLen) 
 *
 * @brief   ApplianceStatistics-Partition configuration callback. 
 *
 */
static uint8_t* ApplianceStatisticsPartitionCallback
(
  zclPartitionAppEvent_t* pPartitionEvent,
  uint8_t* dataLen 
) 
{
  zclFrame_t zclFrame;
  if (pPartitionEvent->eventType == gZclPartitionAppEventType_DataRequest) 
  {
    uint8_t partitionSize = pPartitionEvent->frameInfo->partitionAttrs.partitionedFrameSize;
    uint8_t* msg;
    uint8_t dataLength = partitionSize;
    uint16_t bufferIndex = pPartitionEvent->partitionedFrameId * partitionSize;
    if (gPartitionRxPendingFrame.totalLen - bufferIndex < partitionSize)
      dataLength = gPartitionRxPendingFrame.totalLen - bufferIndex;
    
    msg =  AF_MsgAlloc();/*MSG_Alloc(partitionSize);*/
    if(!msg)
      return NULL;
    if(bufferIndex == 0)
    {
      zclFrame.frameControl = gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp|gZclFrameControl_DirectionRsp; 
      zclFrame.transactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
      zclFrame.command = gCommandToSendUsingPartition;  
      FLib_MemCpy(&msg[0], &zclFrame, sizeof(zclFrame_t)); 
      FLib_MemCpy(&msg[sizeof(zclFrame_t)], &gPartitionRxPendingFrame.data, 3*sizeof(uint32_t));   
       /* read data from memory */
      ApplianceStatistics_GetLogPayloadData(&msg[sizeof(zclFrame_t)+ 3*sizeof(uint32_t)], bufferIndex, partitionSize - sizeof(zclFrame_t)- 3*sizeof(uint32_t));
    }
    else
      /* read data from memory */
       ApplianceStatistics_GetLogPayloadData(&msg[0], bufferIndex, partitionSize);
    *dataLen = dataLength;
    return msg;        
    
  } 
  else if  (pPartitionEvent->eventType == gZclPartitionAppEventType_FrameComplete)
  {
      /* data is received, now you can read */
      gPartitionRxPendingFrame.framePending = FALSE;
      /* check for next LogId available */
      if(gAvailableLogIdTable[0]!=0x00)
      {        
        zclApplianceStatistics_LogReq_t logReqCmd;
        /* generate a logRequest */
        logReqCmd.addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(&logReqCmd.addrInfo.dstAddr, &gApplianceStatisticsServerInf.aDestAddress);
        logReqCmd.addrInfo.dstEndPoint = gApplianceStatisticsServerInf.endPoint;
        Set2Bytes(logReqCmd.addrInfo.aClusterId, gZclClusterApplianceStatistics_c);
        logReqCmd.addrInfo.srcEndPoint = ZCL_GetEndPointForSpecificCluster(logReqCmd.addrInfo.aClusterId ,FALSE, 0, NULL);
        logReqCmd.addrInfo.txOptions = gApsTxOptionNone_c;
        logReqCmd.addrInfo.radiusCounter = afDefaultRadius_c;
        logReqCmd.cmdFrame.LogID = gAvailableLogIdTable[0];
        (void)zclApplianceStatistics_LogRequest(&logReqCmd);  
        ApplianceStatistics_UpdateLogIdTable();
       }      
  } 
  else if  (pPartitionEvent->eventType == gZclPartitionAppEventType_TxFrameComplete) 
  {
      ApplianceStatisticsPartitionTxFrameBuffer_t* currentBuffer = gpPartitionTxPendingFrame;
      while (currentBuffer != NULL) {
        ApplianceStatisticsPartitionTxFrameBuffer_t* nextBuffer = 
                (ApplianceStatisticsPartitionTxFrameBuffer_t*)currentBuffer->nextBuffer;
        MSG_Free(currentBuffer);
        currentBuffer = nextBuffer;
      } 
      gpPartitionTxPendingFrame = NULL;
  }  
  return NULL;
}       

/*!
 * @fn 		static void ZCL_ApplianceStatisticsPartitionServer(zclPartitionedDataIndication_t* partitionDataIndication) 
 *
 * @brief   Manage Data Received Using Partition Cluster.
 *
 */
static void ZCL_ApplianceStatisticsPartitionServer(zclPartitionedDataIndication_t* partitionDataIndication) 
{
  static bool_t firstPacket = TRUE;
  uint8_t offset = 1;
  if (!gPartitionRxPendingFrame.framePending) 
  {
    gPartitionRxPendingFrame.framePending = TRUE;
    gPartitionRxPendingFrame.zclTransactionId = partitionDataIndication->zclTransactionId;
    firstPacket = TRUE;
  }
  /* for receiving data using partition (not used in transmition) */
  if (gPartitionRxPendingFrame.zclTransactionId == partitionDataIndication->zclTransactionId)
  {
    /* compute buffer offset */
    uint16_t bufferIndex = partitionDataIndication->partitionId * partitionDataIndication->partitionLength;
    /* update stored data length */
    gPartitionRxPendingFrame.totalLen = bufferIndex + partitionDataIndication->dataLength;
    /* store the data comming using partition in the memory */
    if(firstPacket == TRUE)
    {   	
      zclCmdApplianceStatistics_LogRspOrNotif_t cmdPayload;
      offset = 4;
      FLib_MemCpy(&cmdPayload ,&partitionDataIndication->partitionedFrame + offset, 3*sizeof(uint32_t));
      (void)zclApplianceStatistics_LogRspOrNotif_Handler(&cmdPayload, TRUE);
      offset+= 3*sizeof(uint32_t);
      firstPacket = FALSE;     
    } 
    ApplianceStatistics_InsertDataInMemory(partitionDataIndication->partitionedFrame + offset, bufferIndex, partitionDataIndication->dataLength-offset+1);
   }
}

 /*!
 * @fn 		static zbStatus_t PowerProfile_TransferDataPtr(uint8_t commandID, bool_t isServer, uint8_t *pReq) 
 *
 * @brief   Use this function to start an Appliance Statistics Transfer using Partition Cluster
 *
 */
static zbStatus_t ApplianceStatistics_TransferDataPtr
(
  zclApplianceStatistics_LogRspOrNotif_t *pReq
) 
{
  uint32_t reqPayloadLen = OTA2Native32(pReq->cmdFrame.LogLength)  +  3*sizeof(uint32_t) + sizeof(zclFrame_t) ;
  zclPartitionedClusterFrameInfo_t partitionTxFrameInfo;
  
  gPartitionRxPendingFrame.totalLen = reqPayloadLen;
  FLib_MemCpy(&gPartitionRxPendingFrame.data, &pReq->cmdFrame, 3*sizeof(uint32_t));
  partitionTxFrameInfo.zclTransactionId = pReq->zclTransactionId; //gpPartitionTxPendingFrame->payload.zclHeader.transactionId;
  
  FLib_MemCpy(&partitionTxFrameInfo.addrInfo, &pReq->addrInfo, sizeof(pReq->addrInfo));
  partitionTxFrameInfo.addrInfo.txOptions &= ~gApsTxOptionFragmentationRequested_c;
  
  /*set partition Frame Size atribute */
  gDefaultPartitionAttrs.partitionedFrameSize = 0x40;
  
  partitionTxFrameInfo.frameCount = (uint16_t)(reqPayloadLen / gDefaultPartitionAttrs.partitionedFrameSize); 
  if (reqPayloadLen % gDefaultPartitionAttrs.partitionedFrameSize != 0)
    partitionTxFrameInfo.frameCount++;
   
  partitionTxFrameInfo.frameCount = Native2OTA16(partitionTxFrameInfo.frameCount);
  FLib_MemCpy((void *)&partitionTxFrameInfo.partitionAttrs, (void *)&gDefaultPartitionAttrs, sizeof(zclPartitionAttrs_t));
  partitionTxFrameInfo.partitionAttrs.partitionedFrameSize = gDefaultPartitionAttrs.partitionedFrameSize;
  return ZCL_PartitionRegisterTxFrame(&partitionTxFrameInfo);
}
#endif //gZclEnablePartition_d

/*!
 * @fn 		zbStatus_t Zcl_ApplianceStatistics_UnsolicitedCommandHandler(uint8_t commandId, uint32_t data)
 *
 * @brief   Process ApplianceStatistics Unsolicited command received from external/internal app.
 *
 */
zbStatus_t Zcl_ApplianceStatistics_UnsolicitedCommandHandler(uint8_t commandId, uint32_t data)
{
#if gZclEnableApplianceStatisticsServerOptionals_d	
 /* Create the destination address -  use binding table*/
 afAddrInfo_t addrInfo = {gZbAddrModeIndirect_c, {0x00, 0x00}, 0, {gaZclClusterApplianceStatistics_c}, 0, gApsTxOptionNone_c, 1};
 
 addrInfo.radiusCounter = afDefaultRadius_c;
 addrInfo.srcEndPoint =  ZCL_GetEndPointForSpecificCluster(addrInfo.aClusterId, TRUE, 0, NULL);
 if(commandId == gZclCmdApplianceStatistics_LogNotification_c)
 {
   /* send a log notification command */
  
  gApplianceStatistics_CurrentLogInf.logId  = OTA2Native32(data);
  return zclApplianceStatistics_LogReq_Handler(addrInfo, ZbZclFoundationGlobals(gZclTransactionId), gZclCmdApplianceStatistics_LogNotification_c);
 }
 if(commandId == gZclCmdApplianceStatistics_StatisticsAvailable_c)
 {
   return zclApplianceStatistics_LogQueueReq_Handler(addrInfo, ZbZclFoundationGlobals(gZclTransactionId), commandId);
 }
#endif 
  return gZbFailed_c;
}

#endif //gZclEnableApplianceStatistics_d



#if gZclEnableApplianceEventsAlerts_d 

const zclCmd_t gaZclApplianceEventsAlertClusterCmdReceivedDef[]={
  /* commands received */
  gZclCmdApplianceEventsAlerts_GetAlerts_c
};
const zclCmd_t gaZclApplianceEventsAlertClusterCmdGeneratedDef[]={
  /* commands generated */
  gZclCmdApplianceEventsAlerts_GetAlertsResponse_c,
  gZclCmdApplianceEventsAlerts_AlertsNotification_c,
  gZclCmdApplianceEventsAlerts_EventNotification_c
};

const zclCommandsDefList_t gZclApplianceEventsAlertClusterCommandsDefList =
{
   NumberOfElements(gaZclApplianceEventsAlertClusterCmdReceivedDef), gaZclApplianceEventsAlertClusterCmdReceivedDef,
   NumberOfElements(gaZclApplianceEventsAlertClusterCmdGeneratedDef), gaZclApplianceEventsAlertClusterCmdGeneratedDef
};


/*!
 * @fn 		zbStatus_t ZCL_ApplianceEventsAlertsClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Appliance Events and Alerts  Cluster Server. 
 *
 */
zbStatus_t ZCL_ApplianceEventsAlertsClusterServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterApplianceEventsAlerts_c}, 0, gApsTxOptionNone_c, 1};
  zbStatus_t status = gZbSuccess_c;
  
  /* to prevent compiler warning */
  (void)pDevice;
  
  /* Create the destination address */
  AF_PrepareForReply(&addrInfo, pIndication); 
  
  pFrame = (void *)pIndication->pAsdu;
   
  /* handle the incoming server commands */
  switch (pFrame->command) 
  {
     case gZclCmdApplianceEventsAlerts_GetAlerts_c:
     {
        zclApplianceEventsAlerts_GetAlertsRsp_t *pCommandRsp; 
        pCommandRsp = AF_MsgAlloc();
        if(!pCommandRsp)
          return gZclNoMem_c;
        
        FLib_MemCpy(&pCommandRsp->addrInfo, &addrInfo, sizeof(addrInfo));
        pCommandRsp->zclTransactionId = pFrame->transactionId;
        pCommandRsp->cmdFrame.AlertsCount.AlertsNumber = (gCurrentNumberOfAlerts&0x0F);
        pCommandRsp->cmdFrame.AlertsCount.AlertType = 0x00; /* unstructured */
        FLib_MemCpy(pCommandRsp->cmdFrame.AlertsStructure, gApplAlertsEventsTable, gCurrentNumberOfAlerts*sizeof(zclApplAlertsEvents_AlertsStructure_t));       
        status = zclApplianceEventsAlerts_GetAlertsRspOrNotif(pCommandRsp, gZclCmdApplianceEventsAlerts_GetAlertsResponse_c); 
        MSG_Free(pCommandRsp);
        break;
     }
     
     default:
        return  gZclUnsupportedClusterCommand_c;                     
  }
  return status;
}


/*!
 * @fn 		zbStatus_t ZCL_ApplianceEventsAlertsClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Appliance Events and Alerts  Cluster Client. 
 *
 */
zbStatus_t ZCL_ApplianceEventsAlertsClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;

  /* prevent compiler warning */
  (void)pDevice;
  
  pFrame = (void *)pIndication->pAsdu;
  
  /* handle the incoming client commands */
  switch(pFrame->command)
  {
     case gZclCmdApplianceEventsAlerts_GetAlertsResponse_c:
     case gZclCmdApplianceEventsAlerts_AlertsNotification_c:
     case gZclCmdApplianceEventsAlerts_EventNotification_c:  
    	 return ((pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c);
    
     default:
         return gZclUnsupportedClusterCommand_c;    
  }
}

/*!
 * @fn 		zbStatus_t zclApplianceEventsAlerts_GetAlerts(zclApplianceEventsAlerts_GetAlerts_t *pReq) 
 *
 * @brief	Sends over-the-air a GetAlerts command  from the Appliance Events and Alerts Cluster Client. 
 *
 */
zbStatus_t zclApplianceEventsAlerts_GetAlerts
(
  zclApplianceEventsAlerts_GetAlerts_t *pReq
)
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterApplianceEventsAlerts_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdApplianceEventsAlerts_GetAlerts_c, 0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclApplianceEventsAlerts_GetAlertsRspOrNotif(zclApplianceEventsAlerts_GetAlertsRsp_t *pCommandRsp, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a GetAlertsRespone/AlertNotification command  from the Appliance Events and Alerts Cluster Server. 
 *
 */
zbStatus_t zclApplianceEventsAlerts_GetAlertsRspOrNotif
( 
  zclApplianceEventsAlerts_GetAlertsRsp_t *pCommandRsp, 
  uint8_t commandId
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen =  1 + OTA2Native32(pCommandRsp->cmdFrame.AlertsCount.AlertsNumber)*3; /* 3 = sizeof(unsigned 24-bit integer) */
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          commandId,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &pCommandRsp->zclTransactionId, 
                          &iPayloadLen,
                          (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
 
 return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t zclApplianceEventsAlerts_EventNotif(zclApplianceEventsAlerts_EventNotif_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air an EventNotification command  from the Appliance Events and Alerts Cluster Server. 
 *
 */
zbStatus_t zclApplianceEventsAlerts_EventNotif
( 
  zclApplianceEventsAlerts_EventNotif_t *pCommandRsp
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = sizeof(zclCmdApplianceEventsAlerts_EventNotif_t);  
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          gZclCmdApplianceEventsAlerts_EventNotification_c,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &pCommandRsp->zclTransactionId, 
                          &iPayloadLen,
                          (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
 
 return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}


#if gZclEnableApplianceEventsAlertsUnsolicitedCmd_d
/*!
 * @fn 		zbStatus_t Zcl_ApplianceEventsAlerts_UnsolicitedCommandHandler(uint8_t commandId, bool_t state, uint32_t data)
 *
 * @brief   Process ApplianceEvents and Alerts Unsolicited command received from external/internal app.
 *
 */
zbStatus_t Zcl_ApplianceEventsAlerts_UnsolicitedCommandHandler(uint8_t commandId, bool_t state, uint32_t data)
{
  zbStatus_t status = gZclUnsupportedClusterCommand_c;
  
  /* Create the destination address -  use binding table*/
  afAddrInfo_t addrInfo = {gZbAddrModeIndirect_c, {0x00, 0x00}, 0, {gaZclClusterApplianceEventsAlerts_c}, 0, gApsTxOptionAckTx_c, 1};
   
  addrInfo.srcEndPoint = ZCL_GetEndPointForSpecificCluster(addrInfo.aClusterId, TRUE, 0, NULL);
  addrInfo.radiusCounter = afDefaultRadius_c;
    
  if(commandId == gZclCmdApplianceEventsAlerts_AlertsNotification_c)
  {
    zclApplianceEventsAlerts_GetAlertsRsp_t command; 
    FLib_MemCpy(&command.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
    command.zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId);
    if(state == TRUE)
    {
        if(gCurrentNumberOfAlerts != gApplEventsAlerts_MaxAlertsSupported_c)
        {
          zclApplAlertsEvents_AlertsStructure_t	*pCurrentData;
          pCurrentData = (zclApplAlertsEvents_AlertsStructure_t *)data;
          
          gApplAlertsEventsTable[gCurrentNumberOfAlerts].AlertsId = pCurrentData->AlertsId; 
          gApplAlertsEventsTable[gCurrentNumberOfAlerts].Category = pCurrentData->Category; 
          gApplAlertsEventsTable[gCurrentNumberOfAlerts].PresenceRecovery = pCurrentData->PresenceRecovery;
          gCurrentNumberOfAlerts++;
        }
      }
      else
      {
        if(gCurrentNumberOfAlerts > 0)
          gCurrentNumberOfAlerts--; 
      }
      command.cmdFrame.AlertsCount.AlertsNumber = (gCurrentNumberOfAlerts&0x0F);
      command.cmdFrame.AlertsCount.AlertType = 0x00; /* unstructured */
      FLib_MemCpy(command.cmdFrame.AlertsStructure, gApplAlertsEventsTable, gCurrentNumberOfAlerts*sizeof(zclApplAlertsEvents_AlertsStructure_t));        
      status = zclApplianceEventsAlerts_GetAlertsRspOrNotif(&command, gZclCmdApplianceEventsAlerts_AlertsNotification_c); 
  }
  if(commandId == gZclCmdApplianceEventsAlerts_EventNotification_c)
  {
     zclApplianceEventsAlerts_EventNotif_t command;
     
     FLib_MemCpy(&command.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
     command.zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId);
     command.cmdFrame.EventHeader = 0x00; /* reserved spec */
     data = OTA2Native32(data);  
     command.cmdFrame.EventIdentification = (uint8_t)(data&0x000000FF);   
     status = zclApplianceEventsAlerts_EventNotif(&command);
  }
  return status;
}
#endif
#endif /* gZclEnableApplianceEventsAlerts_d */




#if gZclEnableApplianceIdentification_d
/******************************
  Appliance Identification Cluster 
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.8 [R1]
*******************************/
/* Appliance Identification Cluster Attribute Definitions */
const zclAttrDef_t gaZclApplianceIdentificationClusterAttrDef[] = {
  { gZclAttrIdApplianceIdentification_BasicIdentification_c, gZclDataTypeUint56_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclApplIdentification_BasicIdentif_t),(void *)MbrOfs(zclApplIdentificationAttrsRAM_t,basicIdentification)}
#if gZclClusterOptionals_d
  ,{ gZclAttrIdApplianceIdentification_CompanyName_c,       gZclDataTypeStr_c,      gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr16_t),    (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,companyName)},
   { gZclAttrIdApplianceIdentification_CompanyId_c,         gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),      (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,companyId)},
   { gZclAttrIdApplianceIdentification_BrandName_c,         gZclDataTypeStr_c,      gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr16_t),    (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,brandName)},
   { gZclAttrIdApplianceIdentification_BrandId_c,           gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),      (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,brandId)},
   { gZclAttrIdApplianceIdentification_Model_c,             gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr16Oct_t), (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,model)},
   { gZclAttrIdApplianceIdentification_PartNumber_c,        gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr16Oct_t), (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,partNumber)},
   { gZclAttrIdApplianceIdentification_ProductRevision_c,   gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr6Oct_t),  (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,productRevision)},
   { gZclAttrIdApplianceIdentification_SoftwareRevision_c,  gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr6Oct_t),  (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,softwareRevision)},
   { gZclAttrIdApplianceIdentification_ProductTypeName_c,   gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr2Oct_t),  (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,productTypeName)},
   { gZclAttrIdApplianceIdentification_ProductTypeId_c,     gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),      (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,productTypeId)},
   { gZclAttrIdApplianceIdentification_CECEDSpecVersion_c,  gZclDataTypeUint8_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),       (void *)MbrOfs(zclApplIdentificationAttrsRAM_t,cecedSpecVersion)}
#endif  
};

const zclAttrSet_t gaZclApplianceIdentificationClusterAttrSet[] = {
  {gZclAttrApplianceIdentification_BasicSet_c, (void *)&gaZclApplianceIdentificationClusterAttrDef, NumberOfElements(gaZclApplianceIdentificationClusterAttrDef)}
};

const zclAttrSetList_t gZclApplianceIdentificationClusterAttrSetList = {
  NumberOfElements(gaZclApplianceIdentificationClusterAttrSet),
  gaZclApplianceIdentificationClusterAttrSet
};

/*!
 * @fn 		zbStatus_t ZCL_ApplianceIdentificationClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Appliance Identification Cluster Server. 
 *
 */
zbStatus_t ZCL_ApplianceIdentificationClusterServer
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
)
{
  (void)pIndication;
  (void)pDevice;
  
  return gZclUnsupportedClusterCommand_c;    
}

/*!
 * @fn 		zbStatus_t ZCL_ApplianceIdentificationClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Appliance Identification Cluster Client. 
 *
 */
zbStatus_t ZCL_ApplianceIdentificationClusterClient
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
)
{
  (void)pIndication;
  (void)pDevice;
  
  return gZclUnsupportedClusterCommand_c;    
}

#endif //gZclEnableApplianceIdentification_d



#if gZclEnableMeterIdentification_d
/******************************
  Meter Identification Cluster 
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.9 [R1]
*******************************/
/* Meter Identification Cluster Attribute Definitions */
const zclAttrDef_t gaZclMeterIdentificationClusterAttrDef[] = {
  { gZclAttrIdMeterIdentification_CompanyName_c,       gZclDataTypeStr_c,      gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr16_t),    (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,companyName)},
  { gZclAttrIdMeterIdentification_MeterTypeId_c,       gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),      (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,meterTypeId)},
  { gZclAttrIdMeterIdentification_DataQualityId_c,     gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),      (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,dataQualityId)},
#if gZclClusterOptionals_d
  { gZclAttrIdMeterIdentification_CustomerName_c,      gZclDataTypeStr_c,      gZclAttrFlagsInRAM_c, sizeof(zclStr16_t),    (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,customerName)},
  { gZclAttrIdMeterIdentification_Model_c,             gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr16Oct_t), (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,model)},
  { gZclAttrIdMeterIdentification_PartNumber_c,        gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr16Oct_t), (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,partNumber)},
  { gZclAttrIdMeterIdentification_ProductRevision_c,   gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr6Oct_t),  (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,productRevision)},
  { gZclAttrIdMeterIdentification_SoftwareRevision_c,  gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr6Oct_t),  (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,softwareRevision)},
  { gZclAttrIdMeterIdentification_UtilityName_c,       gZclDataTypeStr_c,      gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr16_t),    (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,utilityName)},
#endif
  { gZclAttrIdMeterIdentification_POD_c,               gZclDataTypeStr_c,      gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr16_t),    (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,pointOfDelivery)},
  { gZclAttrIdMeterIdentification_AvailablePower_c,    gZclDataTypeInt24_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(powerType_t),   (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,availablePower)},
  { gZclAttrIdMeterIdentification_PowerThreshold_c,    gZclDataTypeInt24_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(powerType_t),   (void *)MbrOfs(zclMeterIdentificationAttrsRAM_t,powerThreshold)}
};


const zclAttrSet_t gaZclMeterIdentificationClusterAttrSet[] = {
  {gZclAttMeterIdentification_Set_c, (void *)&gaZclMeterIdentificationClusterAttrDef, NumberOfElements(gaZclMeterIdentificationClusterAttrDef)}
};

const zclAttrSetList_t gZclMeterIdentificationClusterAttrSetList = {
  NumberOfElements(gaZclMeterIdentificationClusterAttrSet),
  gaZclMeterIdentificationClusterAttrSet
};

/*!
 * @fn 		zbStatus_t ZCL_MeterIdentificationClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Meter Identification Cluster Server. 
 *
 */
zbStatus_t ZCL_MeterIdentificationClusterServer
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
)
{
  (void)pIndication;
  (void)pDevice;
  
  return gZclUnsupportedClusterCommand_c;    
}

/*!
 * @fn 		zbStatus_t ZCL_MeterIdentificationClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Meter Identification Cluster Client. 
 *
 */
zbStatus_t ZCL_MeterIdentificationClusterClient
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
)
{
  (void)pIndication;
  (void)pDevice;
  
  return gZclUnsupportedClusterCommand_c;    
}

#endif //gZclEnableMeterIdentification_d
