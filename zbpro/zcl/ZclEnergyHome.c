/*! @file 	  ZclEnergyHome.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for the Power Profile and Appliance Control Clusters.
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
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ApsMgmtInterface.h"
#include "ASL_UserInterface.h"
#include "ASL_ZdpInterface.h"
#include "zcl.h"
#include "ZclFoundation.h"
#include "ZclEnergyHome.h"
#include "HaProfile.h"
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

#if gZclEnablePwrProfileClusterClient_d
zbStatus_t zclPwrProfile_GetPwrProfilePriceRsp(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t serverIndex);
zbStatus_t zclPwrProfile_GetOverallSchedulePriceRsp(afAddrInfo_t addrInfo, uint8_t transactionId);
zbStatus_t zclPwrProfile_GetPwrProfilePriceExtendedRsp(afAddrInfo_t addrInfo, uint8_t transactionId, zclCmdPwrProfile_GetPwrProfilePriceExtended_t cmdReqPayload, uint8_t serverIndex);
static uint8_t zclPwrProfileClient_GetIndexFromDeviceTable(zbNwkAddr_t aNwkAddr);
#endif

#if gZclEnablePwrProfileClusterServer_d
static void AppManagerTimerCallback(uint8_t tmr);
static void PwrProfile_EnPhsStateNotifTimerCallback(uint8_t tmr);
static uint8_t PwrProfile_CheckPwrProfileId(uint8_t pwrProfileId);
zbStatus_t gZclPowerProfile_EnPhsScheduleRspOrNotif_Handler(zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_t *command);
zbStatus_t zclPwrProfile_PwrProfileScheduleConstraintsRspOrNotif(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId);
#endif

#if gZclEnablePwrProfileClusterServer_d
static void ZCL_ApplianceControlTimer(uint8_t timerId);
#endif
zbStatus_t zclApplianceCtrl_ExecutionCommand_Handler( afAddrInfo_t addrInfo, uint8_t cmdId, uint8_t applianceStatus);


/* partition: private prototypes */
#if (gZclEnablePwrProfileClusterClient_d|| gZclEnablePwrProfileClusterServer_d)&& gZclEnablePartitionPwrProfile_d
static uint8_t*   PowerProfilePartitionCallback(zclPartitionAppEvent_t* pPartitionEvent, uint8_t* dataLen); 
static void       PowerProfilePartitionServer(zclPartitionedDataIndication_t* partitionDataIndication);
static zbStatus_t PowerProfile_TransferDataPtr(uint8_t commandID, bool_t isServer, uint8_t *pReq); 
static void PwrProfile_ProcessDataRcvUsingPartition(uint8_t *pData, uint8_t bufferIndex);
#endif



/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* Server information */
#if gZclEnablePwrProfileClusterServer_d
/* power profile server table */
zclPwrProfileInf_t gPwrProfileServerInf[gPwrProfile_MaxNumberOfPwrProfile_d];
uint8_t gPwrProfile_CurrentProfileId = 0;
bool_t gStartApplianceIfPrice = FALSE;
#endif
/* End  server information */

/*  Client information */
#if gZclEnablePwrProfileClusterClient_d
zclPwrProfileClientInf_t gPwrProfileClientInformation[gPwrProfileClient_MaxNoServerDevices_d];
#endif /* End client information */

/* Application inf.: */
uint8_t gApplianceControl_Status = gApplianceStatus_Idle_c;
zbTmrTimerID_t gPwrProfileTimer = gTmrInvalidTimerID_c; 
zbTmrTimerID_t gAppControlTimer = gTmrInvalidTimerID_c; 
extern zbTmrTimerID_t gApplicationTimer; 
//extern zbEndPoint_t appEndPoint;


#if (gZclEnablePwrProfileClusterClient_d || gZclEnablePwrProfileClusterServer_d)&&gZclEnablePartitionPwrProfile_d
static PwrPRofilePartitionTxFrameBuffer_t* gpPwrProfilePartitionTxPendingFrame;   /* used for partition cluster*/
static PwrProfilePartitionRxFrameBuffer_t gPwrProfilePartitionRxPendingFrame;     /* used for partition cluster*/
#endif

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/******************************
  Power Profile Cluster 
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.6
*******************************/
/* Power Profile Cluster Attribute Definitions */
const zclAttrDef_t gaZclPowerProfileClusterAttrDef[] = {
  { gZclAttrIdPowerProfile_TotalProfileNumId_c,    gZclDataTypeUint8_c,   gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclPowerProfileAttrsRAM_t, totalProfileNum)    },
  { gZclAttrIdPowerProfile_MultipleSchedulingId_c, gZclDataTypeBool_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(bool_t),  (void *)MbrOfs(zclPowerProfileAttrsRAM_t, multipleScheduling) },
  { gZclAttrIdPowerProfile_EnergyFormattingId_c,   gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclPowerProfileAttrsRAM_t, energyFormatting)   },
  { gZclAttrIdPowerProfile_EnergyRemoteId_c,       gZclDataTypeBool_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(bool_t),  (void *)MbrOfs(zclPowerProfileAttrsRAM_t, energyRemote) },
  { gZclAttrIdPowerProfile_ScheduleModeId_c,       gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(zclPowerProfileAttrsRAM_t, scheduleMode) }

};

/** Power Profile Attributes list definition */

const zclAttrSet_t gaZclPowerProfileClusterAttrSet[] = {
  {gZclAttrSetPowerProfileInformation_c, (void *)&gaZclPowerProfileClusterAttrDef, NumberOfElements(gaZclPowerProfileClusterAttrDef)}
};

const zclAttrSetList_t gZclPowerProfileClusterAttrSetList = {
  NumberOfElements(gaZclPowerProfileClusterAttrSet),
  gaZclPowerProfileClusterAttrSet
};

const zclCmd_t gaZclPowerProfileClusterCmdReceivedDef[]={
  /* commands received */
  gZclCmdPowerProfile_PowerProfileRequest_c,
  gZclCmdPowerProfile_PowerProfileStateRequest_c,
  gZclCmdPowerProfile_GetPowerProfilePriceResponse_c,
  gZclCmdPowerProfile_GetOverallSchedulePriceResponse_c,
  gZclCmdPowerProfile_EnergyPhasesScheduleNotification_c,
  gZclCmdPowerProfile_EnergyPhasesScheduleResponse_c,
  gZclCmdPowerProfile_PowerProfileScheduleConstraintsRequest_c,
  gZclCmdPowerProfile_EnergyPhasesScheduleStateRequest_c,
  gZclCmdPowerProfile_GetPwrProfilePriceExtendedRsp_c
};
const zclCmd_t gaZclPowerProfileClusterCmdGeneratedDef[]={
  /* commands generated */
  gZclCmdPowerProfile_PowerProfileNotification_c,
  gZclCmdPowerProfile_PowerProfileResponse_c,
  gZclCmdPowerProfile_PowerProfileStateResponse_c,
  gZclCmdPowerProfile_GetPowerProfilePrice_c,
  gZclCmdPowerProfile_PowerProfileStateNotification_c,
  gZclCmdPowerProfile_GetOverallSchedulePrice_c,
  gZclCmdPowerProfile_EnergyPhasesScheduleRequest_c,
  gZclCmdPowerProfile_EnergyPhasesScheduleStateResponse_c,
  gZclCmdPowerProfile_EnergyPhasesScheduleStateNotification_c,
  gZclCmdPowerProfile_PowerProfileScheduleConstraintsNotification_c,
  gZclCmdPowerProfile_PowerProfileScheduleConstraintsResponse_c,
  gZclCmdPowerProfile_GetPowerProfilePriceExtended_c
};

const zclCommandsDefList_t gZclPowerProfileClusterCommandsDefList =
{
   NumberOfElements(gaZclPowerProfileClusterCmdReceivedDef), gaZclPowerProfileClusterCmdReceivedDef,
   NumberOfElements(gaZclPowerProfileClusterCmdGeneratedDef), gaZclPowerProfileClusterCmdGeneratedDef
};

/******************************
  EN50523 Appliance Control Cluster 
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.7
*******************************/
/* Appliance Control Cluster Attribute Definitions */
const zclAttrDef_t gaZclApplianceControlClusterAttrDef[] = {
  { gZclAttrIdApplianceControl_StartTimeId_c,          gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsReportable_c, sizeof(uint16_t), (void *)MbrOfs(zclApplianceControlAttrsRAM_t, startTime)},
  { gZclAttrIdApplianceControl_FinishTimeId_c,         gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsReportable_c, sizeof(uint16_t), (void *)MbrOfs(zclApplianceControlAttrsRAM_t, finishTime)} 
#if (gZclClusterOptionals_d)
  ,{ gZclAttrIdApplianceControl_RemainingTimeId_c,     gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsReportable_c, sizeof(uint16_t), (void *)MbrOfs(zclApplianceControlAttrsRAM_t, remainingTime)}
#endif 
};

/** Appliance Control Attributes list definition */
const zclAttrSet_t gaZclApplianceControlClusterAttrSet[] = {
  {gZclApplianceControl_ApplianceFunctionsSet_c, (void *)&gaZclApplianceControlClusterAttrDef, NumberOfElements(gaZclApplianceControlClusterAttrDef)}
};

const zclAttrSetList_t gZclApplianceControlClusterAttrSetList = {
  NumberOfElements(gaZclApplianceControlClusterAttrSet),
  gaZclApplianceControlClusterAttrSet
};

const zclCmd_t gaZclApplianceControlClusterCmdReceivedDef[]={
  /* commands received */
  gZclCmdApplianceControl_ExecutionCommand_c,
  gZclCmdApplianceControl_SignalState_c,
  gZclCmdApplianceControl_WriteFunctions_c,
  gZclCmdApplianceControl_OverloadPauseResume_c,
  gZclCmdApplianceControl_OverloadPause_c,
  gZclCmdApplianceControl_OverloadWarning_c
};
const zclCmd_t gaZclApplianceControlClusterCmdGeneratedDef[]={
  /* commands generated */
  gZclCmdApplianceControl_SignalStateResponse_c,
  gZclCmdApplianceControl_SignalStateNotification_c
};

const zclCommandsDefList_t gZclApplianceControlClusterCommandsDefList =
{
   NumberOfElements(gaZclApplianceControlClusterCmdReceivedDef), gaZclApplianceControlClusterCmdReceivedDef,
   NumberOfElements(gaZclApplianceControlClusterCmdGeneratedDef), gaZclApplianceControlClusterCmdGeneratedDef
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************
  Power Profile Cluster
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.6
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_PowerProfileClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the PowerProfile Cluster Server. 
 *
 */
zbStatus_t ZCL_PowerProfileClusterServer
(
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
)
{
#if gZclEnablePwrProfileClusterServer_d	
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterPowerProfile_c}, 0, gApsTxOptionAckTx_c, 1};
  /* prevent compiler warning */
  (void)pDevice;

  pFrame = (void *)pIndication->pAsdu;
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	  status = gZclSuccess_c;
   
  //Create the destination address
  Copy2Bytes(&addrInfo.dstAddr,&pIndication->aSrcAddr);
  addrInfo.dstEndPoint =  pIndication->srcEndPoint;
  addrInfo.srcEndPoint =  pIndication->dstEndPoint;
  if (pIndication->fSecurityStatus & gApsSecurityStatus_Link_Key_c)
    addrInfo.txOptions = gApsTxOptionSecEnabled_c;
  addrInfo.radiusCounter = afDefaultRadius_c;
   
  /* handle the command */
  switch (pFrame->command) 
  {
    case gZclCmdPowerProfile_PowerProfileRequest_c:
    {
        uint8_t pwrProfileId = *((uint8_t *)pFrame + sizeof(zclFrame_t));
        return zclPwrProfile_PwrProfileRspOrNotification(addrInfo, pFrame->transactionId, pwrProfileId, gZclCmdPowerProfile_PowerProfileResponse_c);
    }
    
    case gZclCmdPowerProfile_PowerProfileStateRequest_c:
    {
        return zclPwrProfile_PwrProfileStateRspOrNotification(addrInfo, pFrame->transactionId, gZclCmdPowerProfile_PowerProfileStateResponse_c);  
    }
    
    case gZclCmdPowerProfile_GetPowerProfilePriceResponse_c:  
    case gZclCmdPowerProfile_GetPwrProfilePriceExtendedRsp_c: 
    {
        zclCmdPwrProfile_GetPwrProfilePriceRsp_t cmdRspPayload;
        uint32_t price;
        uint8_t  priceTrailingDigit;
        
        FLib_MemCpy(&cmdRspPayload ,(pFrame + 1), sizeof(zclCmdPwrProfile_GetPwrProfilePriceRsp_t));
        if(cmdRspPayload.pwrProfileId != gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileId)
          return gZclInvalidValue_c;
        BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, &cmdRspPayload);
        
        if((gStartApplianceIfPrice)&&(pFrame->command == gZclCmdPowerProfile_GetPwrProfilePriceExtendedRsp_c))
        {
          priceTrailingDigit = cmdRspPayload.priceInf.priceTrailingDigit;
          price = cmdRspPayload.priceInf.price;
          while(priceTrailingDigit > 0)
          {
              price = price/10;
              priceTrailingDigit--;
          }
          /* check Price Value */
          if((price < gPriceDefaultValueForStart_d) &&
               ((gApplianceControl_Status == gApplianceStatus_Idle_c)||(gApplianceControl_Status == gApplianceStatus_StandBy_c)))
          {
              (void)ApplianceControlProcessStateMachine(gApplianceStatus_ProgrammedWaitToStart_c);
          }   
        
        }
        break;
    }
    
    case gZclCmdPowerProfile_GetOverallSchedulePriceResponse_c:   
    {
        zclCmdPwrProfile_GetPwrProfilePriceRsp_t cmdRspPayload;
        FLib_MemCpy(&cmdRspPayload.priceInf ,(pFrame + 1), sizeof(zclCmdPwrProfile_GetPriceInfRsp_t));
        cmdRspPayload.pwrProfileId = gPwrProfile_AllAvailablePwrProfiles_d;
        break;
    }
    
    case gZclCmdPowerProfile_EnergyPhasesScheduleNotification_c:
    case gZclCmdPowerProfile_EnergyPhasesScheduleResponse_c:  
    {
        zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_t *pCmdPayload;
        pCmdPayload = (void *)(pFrame + 1);
        status = gZclPowerProfile_EnPhsScheduleRspOrNotif_Handler(pCmdPayload);
        if(status == gZclSuccess_c)
        	status = (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c;
        break;
    }
    
    case gZclCmdPowerProfile_PowerProfileScheduleConstraintsRequest_c:
    {
        uint8_t pwrProfileId = *((uint8_t *)pFrame + sizeof(zclFrame_t));
        return zclPwrProfile_PwrProfileScheduleConstraintsRspOrNotif(addrInfo, pFrame->transactionId, pwrProfileId, gZclCmdPowerProfile_PowerProfileScheduleConstraintsResponse_c);
    }
    
    case gZclCmdPowerProfile_EnergyPhasesScheduleStateRequest_c:
    {
        uint8_t pwrProfileId = *((uint8_t *)pFrame + sizeof(zclFrame_t));
        return zclPwrProfile_EnergyPhsScheduleStateRspOrNotif(addrInfo, pFrame->transactionId, pwrProfileId, gZclCmdPowerProfile_EnergyPhasesScheduleStateResponse_c);
    }
    
    default:
      return gZclUnsupportedClusterCommand_c;
  }
  return status;
#else
  return gZclUnsupportedClusterCommand_c;
#endif  
}

/*!
 * @fn 		zbStatus_t ZCL_PowerProfileClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the PowerProfile Cluster Client. 
 *
 */
zbStatus_t ZCL_PowerProfileClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
#if gZclEnablePwrProfileClusterClient_d		
  zclFrame_t *pFrame;
  uint8_t index = 0;
  zbStatus_t status = gZclSuccessDefaultRsp_c;  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterPowerProfile_c}, 0, gApsTxOptionAckTx_c, 1};
  
  /* prevent compiler warning */
  (void)pDevice;

   pFrame = (void *)pIndication->pAsdu;
   if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;
   
   //Create the destination address
   Copy2Bytes(&addrInfo.dstAddr,&pIndication->aSrcAddr);
   addrInfo.dstEndPoint =  pIndication->srcEndPoint;
   addrInfo.srcEndPoint =  pIndication->dstEndPoint;
   if (pIndication->fSecurityStatus & gApsSecurityStatus_Link_Key_c)
    addrInfo.txOptions = gApsTxOptionSecEnabled_c;
   addrInfo.radiusCounter = afDefaultRadius_c;
   
   index = zclPwrProfileClient_GetIndexFromDeviceTable(addrInfo.dstAddr.aNwkAddr);
   if(index == gPwrProfileClient_MaxNoServerDevices_d)
   {
      /* search a free slot */
      for(index = 0; index<gPwrProfileClient_MaxNoServerDevices_d; index++)
        if(gPwrProfileClientInformation[index].slotState == FALSE)
          break;
      if(index != gPwrProfileClient_MaxNoServerDevices_d)
      {
        Copy2Bytes(gPwrProfileClientInformation[index].aNwkAddr, addrInfo.dstAddr.aNwkAddr);
        gPwrProfileClientInformation[index].slotState = TRUE;
      }else
        return gZclNoMem_c;
   } 
   
   
  /* handle the command */
  switch (pFrame->command) 
  {
    case gZclCmdPowerProfile_PowerProfileNotification_c:
    case gZclCmdPowerProfile_PowerProfileResponse_c:
    {
      zclCmdPwrProfile_PwrProfileRspOrNotification_t *pCmdPayload;
      pCmdPayload = (void *)(pFrame+1);
      PwrProfileClient_StoreRemoteServerEnPhsInf(index, pCmdPayload);
      break;
    }
    
    case gZclCmdPowerProfile_PowerProfileStateResponse_c:
    case gZclCmdPowerProfile_PowerProfileStateNotification_c:    
    {
      zclCmdPwrProfile_PwrProfileStateRspOrNotification_t *pCmdPayload;
      pCmdPayload = (void *)(pFrame+1);
      PwrProfileClient_StoreRemoteServerStateInf(index, pCmdPayload);
	  break;
    }
    case gZclCmdPowerProfile_EnergyPhasesScheduleStateResponse_c:
    case gZclCmdPowerProfile_EnergyPhasesScheduleStateNotification_c:
    case gZclCmdPowerProfile_PowerProfileScheduleConstraintsNotification_c:
    case gZclCmdPowerProfile_PowerProfileScheduleConstraintsResponse_c:
    {
        /* process packet received over the air */
        break;
    } 
    
    case gZclCmdPowerProfile_EnergyPhasesScheduleRequest_c:  
    { 
        uint8_t pwrProfileId = *((uint8_t *)pFrame + sizeof(zclFrame_t));
        status = zclPwrProfile_EnergyPhaseScheduleRspOrNotification(addrInfo, pFrame->transactionId, 
                                                                    pwrProfileId, gZclCmdPowerProfile_EnergyPhasesScheduleResponse_c);    
        if(status == 0xFF)
        	status = (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c;
        break;
    }
    
#if (gZclClusterOptionals_d)
    case gZclCmdPowerProfile_GetPowerProfilePrice_c:  
    {
        uint8_t pwrProfileId = *((uint8_t *)pFrame + sizeof(zclFrame_t));
        status = zclPwrProfile_GetPwrProfilePriceRsp(addrInfo, pFrame->transactionId, pwrProfileId, index);
        break;
    }
      
    case gZclCmdPowerProfile_GetOverallSchedulePrice_c:
    {
        status = zclPwrProfile_GetOverallSchedulePriceRsp(addrInfo, pFrame->transactionId);
        break;  
    }
    
    case gZclCmdPowerProfile_GetPowerProfilePriceExtended_c: 
    {
        zclCmdPwrProfile_GetPwrProfilePriceExtended_t cmdReqPayload;
        FLib_MemCpy(&cmdReqPayload ,(pFrame + 1), sizeof(zclCmdPwrProfile_GetPwrProfilePriceExtended_t));    
        status = zclPwrProfile_GetPwrProfilePriceExtendedRsp(addrInfo, pFrame->transactionId, cmdReqPayload, index);
        break; 
    }
    
#endif
    
    default:
      return gZclUnsupportedClusterCommand_c;
  }
  return status;
#else
  return gZclUnsupportedClusterCommand_c;
#endif  
}


#if gZclEnablePwrProfileClusterClient_d
/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileReq(zclPwrProfile_PwrProfileReq_t *pReq)
 *
 * @brief	Sends over-the-air a PowerProfileRequest from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_PwrProfileReq
(
  zclPwrProfile_PwrProfileReq_t *pReq
)  
{
  uint8_t payloadLen = sizeof(zclCmdPwrProfile_PwrProfileReq_t);
  
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPowerProfile_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdPowerProfile_PowerProfileRequest_c, payloadLen,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileStateReq(zclPwrProfile_PwrProfileStateReq_t *pReq)
 *
 * @brief	Sends over-the-air a PowerProfileStateRequest from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_PwrProfileStateReq
(
  zclPwrProfile_PwrProfileStateReq_t *pReq
)  
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPowerProfile_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdPowerProfile_PowerProfileStateRequest_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPwrProfile_GetPwrProfilePriceRsp(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId,  uint8_t serverIndex)
 *
 * @brief	Sends over-the-air a GetPowerProfilePriceResponse Command from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_GetPwrProfilePriceRsp
(
  afAddrInfo_t addrInfo, 
  uint8_t transactionId, 
  uint8_t pwrProfileId,
  uint8_t serverIndex
)
{
  afToApsdeMessage_t *pCommandRsp;
  zclCmdPwrProfile_GetPwrProfilePriceRsp_t cmdRspPayload;
  uint8_t len = 0;  

  cmdRspPayload.pwrProfileId = pwrProfileId;
  cmdRspPayload.priceInf = PwrProfileClient_GetPrice(serverIndex,  pwrProfileId);  

  len = sizeof(zclCmdPwrProfile_GetPwrProfilePriceRsp_t);
  pCommandRsp = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdPowerProfile_GetPowerProfilePriceResponse_c,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp, 
                          &transactionId, 
                          &len,
                          &cmdRspPayload);
  if(!pCommandRsp)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pCommandRsp);  
}

/*!
 * @fn 		zbStatus_t zclPwrProfile_GetOverallSchedulePriceRsp(afAddrInfo_t addrInfo, uint8_t transactionId)
 *
 * @brief	Sends over-the-air a GetOverallSchedulePriceResponse Command from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_GetOverallSchedulePriceRsp
(
  afAddrInfo_t addrInfo, 
  uint8_t transactionId
)
{
  afToApsdeMessage_t *pCommandRsp;
  zclCmdPwrProfile_GetPriceInfRsp_t cmdRspPayload;
  uint8_t len = 0;  

   //used for test:
   cmdRspPayload.currency = gISO4217Currency_EUR_c;
   cmdRspPayload.price = gPwrProfile_DefaulOverallPriceValue_d;   
   cmdRspPayload.priceTrailingDigit = 0x03;
    
  len = sizeof(cmdRspPayload);
  
  pCommandRsp = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdPowerProfile_GetOverallSchedulePriceResponse_c,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp, 
                          &transactionId, 
                          &len,
                          &cmdRspPayload);
  if(!pCommandRsp)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pCommandRsp);  
}

/*!
 * @fn 		zbStatus_t zclPwrProfile_EnergyPhaseScheduleRspOrNotification(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId)
 *
 * @brief	Sends over-the-air a EnergyPhaseScheduleResponse/EnergyPhaseScheduleNotification Command from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_EnergyPhaseScheduleRspOrNotification
(
  afAddrInfo_t addrInfo, 
  uint8_t transactionId,
  uint8_t pwrProfileId,
  uint8_t commandId
)
{
  zclPwrProfile_EnergyPhasesScheduleRspOrNotif_t *pCommandRsp;
  uint8_t payloadLen = 0, i = 0;
  zbStatus_t status = gZbFailed_c;
  uint8_t index = 0;
        
  index = zclPwrProfileClient_GetIndexFromDeviceTable(addrInfo.dstAddr.aNwkAddr);
  if(index == gPwrProfileClient_MaxNoServerDevices_d)
     return gZclNotAuthorized_c;

  if(gPwrProfileClientInformation[index].multipleSchedulingInf == 0xFF) /* not have this information*/
  {
     /*ignore the request -  shoul send a ReadAttrReq - for gZclAttrPowerProfile_MultipleSchedulingId_c */
     return 0xFF;
  }
  
  pCommandRsp = AF_MsgAlloc();  
  if(!pCommandRsp)
     return gZclNoMem_c;  

  FLib_MemCpy(&pCommandRsp->addrInfo, &addrInfo, sizeof(afAddrInfo_t));
  pCommandRsp->zclTransactionId = transactionId;
  pCommandRsp->cmdFrame.pwrProfileId = pwrProfileId;
   
  if(gPwrProfileClientInformation[index].multipleSchedulingInf == FALSE)
  {
    pCommandRsp->cmdFrame.numOfScheduledPhases = 0x01;
    /*  response shall carry the scheduled time of the next energy phase*/
    if(gPwrProfileClientInformation[index].currentEnergyId == 0xFF || 
       gPwrProfileClientInformation[index].currentEnergyId == gPwrProfile_MaxEnergyPhaseInf_d)
      pCommandRsp->cmdFrame.energyPhsScheduledTime[0].energyPhaseId = gPwrProfile_EnergyPhaseId1_c;
    else
      pCommandRsp->cmdFrame.energyPhsScheduledTime[0].energyPhaseId = gPwrProfileClientInformation[index].currentEnergyId++;
    pCommandRsp->cmdFrame.energyPhsScheduledTime[0].scheduledTime = 0x0A;
    
    payloadLen = sizeof(zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_t);
  }
  else
  {
    pCommandRsp->cmdFrame.numOfScheduledPhases = gPwrProfile_MaxEnergyPhaseInf_d;
    for(i=0;i<gPwrProfile_MaxEnergyPhaseInf_d;i++)
    {
      pCommandRsp->cmdFrame.energyPhsScheduledTime[i].energyPhaseId = i;
      pCommandRsp->cmdFrame.energyPhsScheduledTime[i].scheduledTime = 0x0A;  
    }
    payloadLen = sizeof(zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_t)+
          (gPwrProfile_MaxEnergyPhaseInf_d-1)*sizeof(zclCmdPwrProfile_EnegyPhsScheduledTime_t);
  }
  
  if (payloadLen > GetMaxApplicationPayload(NULL))   
  {
   /* Send Response using Partition Cluster */
#if gZclEnablePwrProfileClusterClient_d && gZclEnablePartitionPwrProfile_d    
      status = PowerProfile_TransferDataPtr(commandId, TRUE, (uint8_t *)pCommandRsp);
#endif  
   }
   else
      status = ZCL_SendClientRspSeqPassed(commandId, payloadLen,(zclGenericReq_t *)pCommandRsp);
   MSG_Free(pCommandRsp);
   return status;
}

/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileScheduleConstraintsReq(zclPwrProfile_PwrProfileScheduleConstraintsReq_t *pReq) 
 *
 * @brief	Sends over-the-air a PowerProfileScheduleConstraints Request from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_PwrProfileScheduleConstraintsReq
(
  zclPwrProfile_PwrProfileScheduleConstraintsReq_t *pReq
)  
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPowerProfile_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdPowerProfile_PowerProfileScheduleConstraintsRequest_c, sizeof(zclCmdPwrProfile_PwrProfileReq_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPwrProfile_EnergyPhsScheduleStateReq(zclPwrProfile_EnergyPhsScheduleStateReq_t *pReq) 
 *
 * @brief	Sends over-the-air a EnergyPhaseScheduleState Request from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_EnergyPhsScheduleStateReq
(
  zclPwrProfile_EnergyPhsScheduleStateReq_t *pReq
)  
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPowerProfile_c);	
  return ZCL_SendClientRspSeqPassed(gZclCmdPowerProfile_EnergyPhasesScheduleStateRequest_c, sizeof(zclCmdPwrProfile_PwrProfileReq_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPwrProfile_GetPwrProfilePriceExtendedRsp(afAddrInfo_t addrInfo, uint8_t transactionId, zclCmdPwrProfile_GetPwrProfilePriceExtended_t cmdReqPayload, uint8_t serverIndex)
 *
 * @brief	Sends over-the-air a GetPowerProfilePriceExtended Response from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_GetPwrProfilePriceExtendedRsp
(
  afAddrInfo_t addrInfo, 
  uint8_t transactionId,
  zclCmdPwrProfile_GetPwrProfilePriceExtended_t cmdReqPayload,
  uint8_t serverIndex
)
{
  
  afToApsdeMessage_t *pCommandRsp;
  zclCmdPwrProfile_GetPwrProfilePriceRsp_t cmdRspPayload;
  uint8_t len = 0;  

  cmdRspPayload.pwrProfileId = cmdReqPayload.pwrProfileId;
  cmdRspPayload.priceInf = PwrProfileClient_GetPrice(serverIndex,  cmdReqPayload.pwrProfileId);  

  len = sizeof(cmdRspPayload);
  
  pCommandRsp = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdPowerProfile_GetPwrProfilePriceExtendedRsp_c,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp, 
                          &transactionId, 
                          &len,
                          &cmdRspPayload);
  if(!pCommandRsp)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pCommandRsp); 
}

/*!
 * @fn 		void zclPwrProfile_InitClient(void)
 *
 * @brief	Init Power Profile Client Cluster 
 *
 */
void zclPwrProfile_InitClient(uint8_t endpoint)
{
  uint8_t i=0, j = 0;	
  for(i=0; i<gPwrProfileClient_MaxNoServerDevices_d; i++)	
  {
	  gPwrProfileClientInformation[i].slotState = FALSE;
	  gPwrProfileClientInformation[i].aNwkAddr[0] = 0xFF;
  	  gPwrProfileClientInformation[i].aNwkAddr[1] = 0xFF;
  	  gPwrProfileClientInformation[i].multipleSchedulingInf = 0xFF;
  	  gPwrProfileClientInformation[i].remoteControlInf = 0xFF;
  	  gPwrProfileClientInformation[i].currentEnergyId = 0xFF;
  	  for(j=0;j<gPwrProfileClient_MaxPwrProfileInfSupported_d; j++)
  	  {
  		gPwrProfileClientInformation[i].pwrProfileInf[j].pwrProfileId = 0xFF;
  		gPwrProfileClientInformation[i].pwrProfileInf[j].currentEnergyPhaseId = 0xFF;
  		gPwrProfileClientInformation[i].pwrProfileInf[j].numOfTransferredPhases = 0xFF;
  		gPwrProfileClientInformation[i].pwrProfileInf[j].pwrProfileState = 0xFF;
  		gPwrProfileClientInformation[i].pwrProfileInf[j].pwrProfileRemoteControl = 0xFF;
  	  }  	  
  }
  App_InitTimeCluster(endpoint);
}

/*!
 * @fn 		static uint8_t zclPwrProfileClient_GetIndexFromDeviceTable(zbNwkAddr_t aNwkAddr)
 *
 * @brief	Get corresponding server index from client Table
 *
 */
static uint8_t zclPwrProfileClient_GetIndexFromDeviceTable(zbNwkAddr_t aNwkAddr)
{
  uint8_t i = 0;
  for(i=0; i<gPwrProfileClient_MaxNoServerDevices_d; i++)
    if(IsEqual2Bytes(aNwkAddr, gPwrProfileClientInformation[i].aNwkAddr))
      return i;
  return gPwrProfileClient_MaxNoServerDevices_d;
}

/*!
 * @fn 		void zclPwrProfileClient_StoreRemoteServerMultipleSchedulingInf(zbNwkAddr_t aNwkAddr, uint16_t attrId, uint8_t attrValue)
 *
 * @brief	Update client table. Store multiple schedule and remote control server informations 
 *
 */
void zclPwrProfileClient_StoreRemoteServerMultipleSchedulingInf(zbNwkAddr_t aNwkAddr, uint16_t attrId, uint8_t attrValue)
{
  uint8_t index =  zclPwrProfileClient_GetIndexFromDeviceTable(aNwkAddr);
  if(index == gPwrProfileClient_MaxNoServerDevices_d)
  {
    /* search a free slot */
    for(index = 0; index<gPwrProfileClient_MaxNoServerDevices_d; index++)
      if(IsEqual2BytesInt(gPwrProfileClientInformation[index].aNwkAddr, 0xFFFF))
        break;
    if(index != gPwrProfileClient_MaxNoServerDevices_d)
      Copy2Bytes(gPwrProfileClientInformation[index].aNwkAddr, aNwkAddr);
    else
      return;
  }
  if(attrId == gZclAttrPowerProfile_MultipleSchedulingId_c)
    gPwrProfileClientInformation[index].multipleSchedulingInf = attrValue;
  else
  {
    if(attrId == gZclAttrPowerProfile_EnergyRemoteId_c)
      gPwrProfileClientInformation[index].remoteControlInf = attrValue;  
  }
}

/*!
 * @fn 		void PwrProfileClient_StoreRemoteServerEnPhsInf(uint8_t index,zclCmdPwrProfile_PwrProfileRspOrNotification_t *pData)
 *
 * @brief	Update client table. Store the server energy phase inf.
 *
 */
void PwrProfileClient_StoreRemoteServerEnPhsInf(uint8_t index,zclCmdPwrProfile_PwrProfileRspOrNotification_t *pData)
{
  uint8_t i=0;
  
  for(i=0;i<gPwrProfileClient_MaxPwrProfileInfSupported_d; i++)
  {
	  if((gPwrProfileClientInformation[index].pwrProfileInf[i].pwrProfileId == pData->powerProfileID)||
			 (gPwrProfileClientInformation[index].pwrProfileInf[i].pwrProfileId == 0xFF))
	  {
		 gPwrProfileClientInformation[index].pwrProfileInf[i].pwrProfileId = pData->powerProfileID;
		 if(pData->numOfTransferredPhases > gPwrProfile_MaxEnergyPhaseInf_d)
			 gPwrProfileClientInformation[index].pwrProfileInf[i].numOfTransferredPhases = gPwrProfile_MaxEnergyPhaseInf_d;
	     else
	    	 gPwrProfileClientInformation[index].pwrProfileInf[i].numOfTransferredPhases = pData->numOfTransferredPhases;
		 
		 FLib_MemCpy(gPwrProfileClientInformation[index].pwrProfileInf[i].energyPhaseInf, pData->energyPhaseInf, sizeof(zclPwrProfile_EnergyPhaseInf_t)*gPwrProfileClientInformation[index].pwrProfileInf[i].numOfTransferredPhases);
	  }
  }
}

/*!
 * @fn 		void PwrProfileClient_StoreRemoteServerStateInf(uint8_t index, zclCmdPwrProfile_PwrProfileStateRspOrNotification_t *pData)
 *
 * @brief	Update client table. Store the current server state.
 *
 */
void PwrProfileClient_StoreRemoteServerStateInf(uint8_t index, zclCmdPwrProfile_PwrProfileStateRspOrNotification_t *pData)
{
  uint8_t i=0;

  for(i=0;i<pData->pwrProfileCount; i++)
  {
	  if(gPwrProfileClientInformation[index].pwrProfileInf[i].pwrProfileId == pData->pwrProfileRecord[i].pwrProfileId)
	  {
		  gPwrProfileClientInformation[index].pwrProfileInf[i].pwrProfileRemoteControl = pData->pwrProfileRecord[i].remoteControl;
		  gPwrProfileClientInformation[index].pwrProfileInf[i].pwrProfileState =  pData->pwrProfileRecord[i].pwrProfileState;
		  gPwrProfileClientInformation[index].pwrProfileInf[i].currentEnergyPhaseId = pData->pwrProfileRecord[i].energyPhaseId;
	  }
  }
}

/*!
 * @fn 		zclCmdPwrProfile_GetPriceInfRsp_t PwrProfileClient_GetPrice(uint8_t serverIndex, uint8_t powerProfileID)
 *
 * @brief	Based on duration set a demo price value
 *
 */
zclCmdPwrProfile_GetPriceInfRsp_t PwrProfileClient_GetPrice(uint8_t serverIndex, uint8_t powerProfileID)
{
  uint16_t duration = 0;
  uint8_t i=0, j =0;
  zclCmdPwrProfile_GetPriceInfRsp_t priceInf;

  for(i=0;i<gPwrProfileClient_MaxPwrProfileInfSupported_d; i++)
  {
	  if(gPwrProfileClientInformation[serverIndex].pwrProfileInf[i].pwrProfileId == powerProfileID)
	  {
		  for(j=0;j<gPwrProfileClientInformation[serverIndex].pwrProfileInf[i].numOfTransferredPhases;j++)
		  {
			  duration+= gPwrProfileClientInformation[serverIndex].pwrProfileInf[i].energyPhaseInf[j].expectedDuration;
		  }
		  break;
	  }
  }
  
  duration = OTA2Native16(duration);
  priceInf.price = gPwrProfile_DefaulPriceValue_d; 
  
  priceInf.price = OTA2Native32(priceInf.price);
  priceInf.priceTrailingDigit = 0x01;
  priceInf.currency = gISO4217Currency_EUR_c;

  if(duration > 60)
	 priceInf.price += (duration-60);
  else 
	 priceInf.price -= ((60-duration) > priceInf.price)?(5):(60-duration);
  
  priceInf.price = Native2OTA32(priceInf.price);
  return priceInf;
}

#endif /* gZclEnablePwrProfileClusterClient_d */

#if gZclEnablePwrProfileClusterServer_d	
/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileRspOrNotification(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId)
 *
 * @brief	Sends over-the-air a PowerProfileResponse/PowerProfileNotification command from the PowerProfile Cluster Server. 
 *
 */
zbStatus_t zclPwrProfile_PwrProfileRspOrNotification
(
  afAddrInfo_t addrInfo, 
  uint8_t transactionId, 
  uint8_t pwrProfileId,
  uint8_t commandId
)
{
   zclPwrProfile_PwrProfileRspOrNotification_t *pCommandRsp;
   uint8_t i = 0;
   zbStatus_t status = gZclFailure_c; 
   uint16_t payloadLen = 0;
   
   if(pwrProfileId!= gPwrProfile_AllAvailablePwrProfiles_d)
   {
     i = PwrProfile_CheckPwrProfileId(pwrProfileId);
     if(i == gZclInvalidValue_c)
       return gZclInvalidValue_c;
   }
   
   pCommandRsp = MSG_Alloc(sizeof(zclPwrProfile_PwrProfileRspOrNotification_t) + sizeof(zclPwrProfile_EnergyPhaseInf_t)*(gPwrProfileServerInf[i].numOfTransferredPhases-1));  
   if(!pCommandRsp)
     return gZclNoMem_c;
   FLib_MemCpy(&pCommandRsp->addrInfo, &addrInfo, sizeof(afAddrInfo_t));
   pCommandRsp->zclTransactionId = transactionId;
   pCommandRsp->cmdFrame.totalProfileNum = gPwrProfile_MaxNumberOfPwrProfile_d;
   FLib_MemCpy(&pCommandRsp->cmdFrame.powerProfileID, &gPwrProfileServerInf[i], 2 + gPwrProfileServerInf[i].numOfTransferredPhases*sizeof(zclPwrProfile_EnergyPhaseInf_t));
   payloadLen = sizeof(zclCmdPwrProfile_PwrProfileRspOrNotification_t)+ sizeof(zclPwrProfile_EnergyPhaseInf_t)*(gPwrProfileServerInf[i].numOfTransferredPhases-1);   
 
   if (payloadLen > GetMaxApplicationPayload(NULL))   
   {
   /* Send Response using Partition Cluster */
#if gZclEnablePwrProfileClusterServer_d && gZclEnablePartitionPwrProfile_d    
      status = PowerProfile_TransferDataPtr(commandId, TRUE, (uint8_t *)pCommandRsp);
#endif  
   }
   else
      status = ZCL_SendServerRspSeqPassed(commandId, (uint8_t)payloadLen,(zclGenericReq_t *)pCommandRsp);
   MSG_Free(pCommandRsp);
   return status;
}


/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileStateRspOrNotification(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t commandId)
 *
 * @brief	Sends over-the-air a PowerProfileStateResponse/PowerProfileStateNotification command from the PowerProfile Cluster Server. 
 *
 */                    
zbStatus_t zclPwrProfile_PwrProfileStateRspOrNotification
(
  afAddrInfo_t addrInfo, 
  uint8_t transactionId, 
  uint8_t commandId
)
{
  zbStatus_t status = gZclFailure_c;  
  zclPwrProfile_PwrProfileStateRspOrNotification_t *pCommandRsp;
  uint16_t payloadLen = 0;
  uint8_t i = 0;
  bool_t energyRemote;
   
  if(gPwrProfile_MaxNumberOfPwrProfile_d == 0)
    return gZclFailure_c;
  pCommandRsp = AF_MsgAlloc();//MSG_Alloc(sizeof(zclPwrProfile_PwrProfileStateRspOrNotification_t) + sizeof(zclPwrProfile_PwrProfileRecord_t)*(gPwrProfile_MaxNumberOfPwrProfile_d-1));  
  if(!pCommandRsp)
    return gZclNoMem_c;
  FLib_MemCpy(&pCommandRsp->addrInfo, &addrInfo, sizeof(afAddrInfo_t));
  pCommandRsp->zclTransactionId = transactionId;
  pCommandRsp->cmdFrame.pwrProfileCount = gPwrProfile_MaxNumberOfPwrProfile_d;

  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), pCommandRsp->addrInfo.aClusterId, gZclAttrPowerProfile_EnergyRemoteId_c, gZclServerAttr_c,&energyRemote, NULL);
     
  for(i=0;i<gPwrProfile_MaxNumberOfPwrProfile_d; i++)
  {
      /* update pwrProfile energyRemote value */
      gPwrProfileServerInf[i].pwrProfileRemoteControl = energyRemote;
      /* complete the command */
      pCommandRsp->cmdFrame.pwrProfileRecord[i].pwrProfileId = gPwrProfileServerInf[i].pwrProfileId;
      pCommandRsp->cmdFrame.pwrProfileRecord[i].energyPhaseId = gPwrProfileServerInf[i].currentEnergyPhaseId;
      pCommandRsp->cmdFrame.pwrProfileRecord[i].remoteControl = energyRemote; 
      gPwrProfileServerInf[i].pwrProfileRemoteControl = energyRemote;
      pCommandRsp->cmdFrame.pwrProfileRecord[i].pwrProfileState = gPwrProfileServerInf[i].pwrProfileState;
  }
  payloadLen = sizeof(zclCmdPwrProfile_PwrProfileStateRspOrNotification_t) + sizeof(zclPwrProfile_PwrProfileRecord_t)*(gPwrProfile_MaxNumberOfPwrProfile_d-1);

  if (payloadLen > GetMaxApplicationPayload(NULL))   
  {
	  /* Send Response using Partition Cluster */
#if gZclEnablePwrProfileClusterServer_d && gZclEnablePartitionPwrProfile_d    
      status = PowerProfile_TransferDataPtr(commandId, TRUE, (uint8_t *)pCommandRsp);
#endif  
  }
  else
      status = ZCL_SendServerRspSeqPassed(commandId, (uint8_t)payloadLen,(zclGenericReq_t *)pCommandRsp);

  MSG_Free(pCommandRsp);
  return status;
}

/*!
 * @fn 		zbStatus_t zclPwrProfile_GetPwrProfilePrice(zclPwrProfile_GetPwrProfilePrice_t *pReq)
 *
 * @brief	Sends over-the-air a GetPowerProfilePriceRequest command from the PowerProfile Cluster Server. 
 *
 */ 
#if (gZclClusterOptionals_d)
zbStatus_t zclPwrProfile_GetPwrProfilePrice
(
  zclPwrProfile_GetPwrProfilePrice_t *pReq
)
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPowerProfile_c);	
  return ZCL_SendServerReqSeqPassed(gZclCmdPowerProfile_GetPowerProfilePrice_c, sizeof(zclCmdPwrProfile_PwrProfileReq_t),(zclGenericReq_t *)pReq);
}


/*!
 * @fn 		zbStatus_t zclPwrProfile_GetOverallSchedulePrice(zclPwrProfile_GetOverallSchedulePrice_t *pReq)
 *
 * @brief	Sends over-the-air a GetOverallSchedulePriceRequest command from the PowerProfile Cluster Server. 
 *
 */ 
zbStatus_t zclPwrProfile_GetOverallSchedulePrice
(
  zclPwrProfile_GetOverallSchedulePrice_t *pReq
)
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPowerProfile_c);	
  return ZCL_SendServerRspSeqPassed(gZclCmdPowerProfile_GetOverallSchedulePrice_c, 0,(zclGenericReq_t *)pReq);
}
#endif

/*!
 * @fn 		zbStatus_t zclPwrProfile_EnergyPhsScheduleReq(zclPwrProfile_EnergyPhsScheduleReq_t *pReq)
 *
 * @brief	Sends over-the-air a EnergyPhaseScheduleRequest command from the PowerProfile Cluster Server. 
 *
 */
zbStatus_t zclPwrProfile_EnergyPhsScheduleReq
(
  zclPwrProfile_EnergyPhsScheduleReq_t *pReq
)
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPowerProfile_c);	
  return ZCL_SendServerReqSeqPassed(gZclCmdPowerProfile_EnergyPhasesScheduleRequest_c, sizeof(zclCmdPwrProfile_PwrProfileReq_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPwrProfile_EnergyPhsScheduleStateRspOrNotif(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId)
 *
 * @brief	Sends over-the-air a EnergyPhaseScheduleStateResponse/EnergyPhaseScheduleStateNotification command from the PowerProfile Cluster Server. 
 *
 */
zbStatus_t zclPwrProfile_EnergyPhsScheduleStateRspOrNotif
(
  afAddrInfo_t addrInfo, 
  uint8_t transactionId, 
  uint8_t pwrProfileId,
  uint8_t commandId
)
{
  zclPwrProfile_EnergyPhsScheduleStateRspOrNotif_t *pCommandRsp;
  uint8_t payloadLen = 0, i=0, index;  
  zbStatus_t status = gZbFailed_c;
  bool_t transmitScheduledTime = FALSE;
  
  /* check power profile Id */
  if (!pwrProfileId)
    return gZclInvalidValue_c;
  index = PwrProfile_CheckPwrProfileId(pwrProfileId);
  if(index == gZclInvalidValue_c)
    return gZclInvalidValue_c;
  
  pCommandRsp = AF_MsgAlloc();  
  if(!pCommandRsp)
     return gZclNoMem_c;  
  
  FLib_MemCpy(&pCommandRsp->addrInfo, &addrInfo, sizeof(afAddrInfo_t));
  pCommandRsp->zclTransactionId = transactionId;
  
  if ((gPwrProfileServerInf[index].pwrProfileState == gPwrProfileState_EnergyPhsWaitingToStart_c) ||
        (gPwrProfileServerInf[index].pwrProfileState == gPwrProfileState_EnergyPhsRunning_c))
    transmitScheduledTime = TRUE;
  
  /* verify remote control status for selected power profile */
  if((gPwrProfileServerInf[index].pwrProfileRemoteControl == FALSE)&&(transmitScheduledTime == FALSE))
  {
    pCommandRsp->cmdFrame.numOfScheduledPhases = 0x00;
    payloadLen = 2;
  }
  else
  { 
      pCommandRsp->cmdFrame.numOfScheduledPhases = gPwrProfileServerInf[index].numOfTransferredPhases;
   
      for(i=0;i<gPwrProfileServerInf[index].numOfTransferredPhases;i++)
      {
        pCommandRsp->cmdFrame.energyPhsScheduledTime[i].energyPhaseId = gPwrProfileServerInf[index].energyPhaseInf[i].energyPhaseID;
        pCommandRsp->cmdFrame.energyPhsScheduledTime[i].scheduledTime = gPwrProfileServerInf[index].energyPhaseInf[i].expectedDuration;  
      }
      
      payloadLen = sizeof(zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_t)+
              (pCommandRsp->cmdFrame.numOfScheduledPhases-1)*sizeof(zclCmdPwrProfile_EnegyPhsScheduledTime_t);
  }
  
  pCommandRsp->cmdFrame.pwrProfileId = pwrProfileId;   
  
  if (payloadLen > GetMaxApplicationPayload(NULL))   
  {
   /* Send Response using Partition Cluster */
#if gZclEnablePwrProfileClusterServer_d && gZclEnablePartitionPwrProfile_d    
      status = PowerProfile_TransferDataPtr(commandId, TRUE, (uint8_t *)pCommandRsp);
#endif  
   }
   else
      status = ZCL_SendServerRspSeqPassed(commandId, payloadLen,(zclGenericReq_t *)pCommandRsp);
   MSG_Free(pCommandRsp);
   return status;
}


/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileScheduleConstraintsRspOrNotif(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId)
 *
 * @brief	Sends over-the-air a PowerProfileScheduleConstraintsResponse/PowerProfileScheduleConstraintsNotification command from the PowerProfile Cluster Server. 
 *
 */
zbStatus_t zclPwrProfile_PwrProfileScheduleConstraintsRspOrNotif
(
  afAddrInfo_t addrInfo, 
  uint8_t transactionId, 
  uint8_t pwrProfileId,
  uint8_t commandId
)
{
  afToApsdeMessage_t *pCommandRsp;
  zclCmdPwrProfile_PwrProfileScheduleConstraintsRspOrNotif_t cmdRspPayload;
  uint8_t len = 0;  

  cmdRspPayload.powerProfileId = pwrProfileId;
  cmdRspPayload.startAfter = 0x0000;
  cmdRspPayload.stopBefore = 0xFFFF;
  
  len = sizeof(cmdRspPayload);
  
  pCommandRsp = ZCL_CreateFrame( &addrInfo, 
                          commandId,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp | gZclFrameControl_DirectionRsp, 
                          &transactionId, 
                          &len,
                          &cmdRspPayload);
  if(!pCommandRsp)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pCommandRsp);  
}


#if (gZclClusterOptionals_d)
/*!
 * @fn 		zbStatus_t zclPwrProfile_GetPwrProfilePriceExtended(zclPwrProfile_GetPwrProfilePriceExtended_t *pReq)
 *
 * @brief	Sends over-the-air a GetPowerProfilePriceExtended command from the PowerProfile Cluster Server. 
 *
 */
zbStatus_t zclPwrProfile_GetPwrProfilePriceExtended
(
  zclPwrProfile_GetPwrProfilePriceExtended_t *pReq
)
{
  uint8_t payloadLen = 0;
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPowerProfile_c);	
  if((pReq->cmdFrame.options & gPwrProfile_StartTimeFieldPresent_d)== gPwrProfile_StartTimeFieldPresent_d)
     payloadLen = sizeof(zclCmdPwrProfile_GetPwrProfilePriceExtended_t);
  else
     payloadLen = sizeof(zclCmdPwrProfile_GetPwrProfilePriceExtended_t)-sizeof(uint16_t);
  return ZCL_SendServerRspSeqPassed(gZclCmdPowerProfile_GetPowerProfilePriceExtended_c, payloadLen,(zclGenericReq_t *)pReq);
}
#endif


/*!
 * @fn 		gZclPowerProfile_EnPhsScheduleRspOrNotif_Handler(zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_t *command)
 *
 * @brief	Process EnergyPhaseScheduleResponse/EnergyPhaseScheduleNotification command received from PowerProfile Cluster Client 
 *
 */
zbStatus_t gZclPowerProfile_EnPhsScheduleRspOrNotif_Handler(zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_t *command)
{
#if (gZclEnablePwrProfileClusterServer_d) 
  uint8_t i = 0, j = 0, k = 0;
  zbClusterId_t aClusterId;  
  bool_t energyRemote = FALSE;
  bool_t multipleScheduling = FALSE;
  
  Set2Bytes(aClusterId, gZclClusterPowerProfile_c);
  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), aClusterId, gZclAttrPowerProfile_EnergyRemoteId_c, gZclServerAttr_c,&energyRemote, NULL);
  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), aClusterId, gZclAttrPowerProfile_MultipleSchedulingId_c, gZclServerAttr_c,&multipleScheduling, NULL);
   
  /* check the power profile */ 
  i = PwrProfile_CheckPwrProfileId(command->pwrProfileId);
  
  /* update energy remote information */
  gPwrProfileServerInf[i].pwrProfileRemoteControl = energyRemote;
  
  /* verify remote control status for selected power profile */
  if((i == gZclInvalidValue_c)||(gPwrProfileServerInf[i].pwrProfileRemoteControl == FALSE))
  {
    return gZclNotAuthorized_c;
  }
  command->energyPhsScheduledTime[0].scheduledTime = OTA2Native16(command->energyPhsScheduledTime[0].scheduledTime);
  if(!multipleScheduling)
  {
    for(j=0;j<gPwrProfileServerInf[i].numOfTransferredPhases;j++)
      if(command->energyPhsScheduledTime[0].energyPhaseId == gPwrProfileServerInf[i].energyPhaseInf[j].energyPhaseID)
      {
        if((gPwrProfileServerInf[i].energyPhaseInf[j].maxActivationDelay != 0x00)&&
           (gPwrProfileServerInf[i].energyPhaseInf[j].maxActivationDelay > command->energyPhsScheduledTime[0].scheduledTime))
        {
          gPwrProfileServerInf[i].energyPhaseInf[j].expectedDuration = Native2OTA16(command->energyPhsScheduledTime[0].scheduledTime);
          gPwrProfileServerInf[i].currentEnergyPhaseId = command->energyPhsScheduledTime[0].energyPhaseId;
        }  
        else
          return gZclNotAuthorized_c;
      }
  }
  else
  { 
    k=0;
    while(k < command->numOfScheduledPhases)  
    {
      for(j=0;j<gPwrProfileServerInf[i].numOfTransferredPhases;j++)
      {
        if(command->energyPhsScheduledTime[k].energyPhaseId == gPwrProfileServerInf[i].energyPhaseInf[j].energyPhaseID)
        {
          if((gPwrProfileServerInf[i].energyPhaseInf[j].maxActivationDelay != 0x00)&&
            (gPwrProfileServerInf[i].energyPhaseInf[j].maxActivationDelay > command->energyPhsScheduledTime[k].scheduledTime))
            gPwrProfileServerInf[i].energyPhaseInf[j].expectedDuration =  Native2OTA16(command->energyPhsScheduledTime[k].scheduledTime);
          else
            return gZclNotAuthorized_c;
        }
      }
      k++;
    }
  }

  ZbTMR_StartSecondTimer(gApplicationTimer, 0x03, AppManagerTimerCallback); 
#endif
  return gZclSuccess_c;
  
}

/*!
 * @fn 		static uint8_t PwrProfile_CheckPwrProfileId(uint8_t pwrProfileId)
 *
 * @brief	Check the powerProfileId. If valid return the index from powerProfileInf Table, otherwise return gZclInvalidValue_c
 *
 */
static uint8_t PwrProfile_CheckPwrProfileId(uint8_t pwrProfileId)
{
  zbClusterId_t aClusterId;
  uint8_t totalProfileNum = 0, i = 0;  
  
  Set2Bytes(aClusterId, gZclClusterPowerProfile_c);	  
  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), aClusterId, gZclAttrPowerProfile_TotalProfileNumId_c, gZclServerAttr_c,&totalProfileNum, NULL); 
    
  if(totalProfileNum == 0)
     return gZclInvalidValue_c;
  for(i=0;i<totalProfileNum;i++)
  {
     if(gPwrProfileServerInf[i].pwrProfileId == pwrProfileId)
         break;
     else
         if(i == totalProfileNum-1)
           return gZclInvalidValue_c;
  }
  return i;
}

#if (gZclClusterOptionals_d)
/*!
 * @fn 		zbStatus_t ZtcPwrProfileServer_SetCurrentPwrProfileInformation(zclPwrProfileInf_t* pPwrProfileInf)
 *
 * @brief	Set current power profile information
 *
 */
zbStatus_t ZtcPwrProfileServer_SetCurrentPwrProfileInformation(zclPwrProfileInf_t* pPwrProfileInf)
{
  /* Note: current power profile = first position of the gPwrProfileServerInf table */ 
  FLib_MemCpy(&gPwrProfileServerInf[gPwrProfile_CurrentProfileId], pPwrProfileInf, sizeof(zclPwrProfileInf_t));
  ZbTMR_StopTimer(gApplicationTimer);
  //BeeAppUpdateDevice(0, gZclUI_Off_c, 0, 0, NULL);
  (void)ApplianceControlProcessStateMachine(gApplianceStatus_Idle_c);
  return gZclSuccess_c;

}
#endif

/*!
 * @fn 		void PwrProfile_InitPwrProfileServer(void)
 *
 * @brief	Init Power Profile Server
 *
 */
void PwrProfile_InitPwrProfileServer(uint8_t endpoint)
{
  uint8_t i=0, j=0;
  
  /* set the current power profile */
  gPwrProfile_CurrentProfileId = 0;	
  
  /* init power profile server table */
  for(i=0; i<gPwrProfile_MaxNumberOfPwrProfile_d; i++)
  {
	  gPwrProfileServerInf[i].pwrProfileId = i+1; /* 0x00 = reserved -  match all power profiles */
	  gPwrProfileServerInf[i].currentEnergyPhaseId = gPwrProfile_MaxEnergyPhaseInf_d; /* set the current energy phase to the max energy phase available */
	  gPwrProfileServerInf[i].numOfTransferredPhases = gPwrProfile_MaxEnergyPhaseInf_d;
	  gPwrProfileServerInf[i].pwrProfileRemoteControl = FALSE; /* default - remote control is set to FALSE */
	  gPwrProfileServerInf[i].pwrProfileState = gPwrProfileState_PwrProfileProgrammed_c;
	  gPwrProfileServerInf[i].currentDuration = 0x00;
	  for(j=0;j<gPwrProfile_MaxEnergyPhaseInf_d; j++)
	  {
		  gPwrProfileServerInf[i].energyPhaseInf[j].energyPhaseID = j+1; 
		  gPwrProfileServerInf[i].energyPhaseInf[j].macroPhaseID = j+1;
		  gPwrProfileServerInf[i].energyPhaseInf[j].expectedDuration = gPwrProfile_DefaultExpectedDuration_d;
		  gPwrProfileServerInf[i].energyPhaseInf[j].peakPower = gPwrProfile_DefaultPeakPower_d;
		  gPwrProfileServerInf[i].energyPhaseInf[j].maxActivationDelay = gPwrProfile_DefaultMaxActivationDelay_d;
		  gPwrProfileServerInf[i].energyPhaseInf[j].energy = gPwrProfileServerInf[i].energyPhaseInf[j].peakPower * gPwrProfileServerInf[i].energyPhaseInf[j].expectedDuration;
	  }
  }
  
  App_InitTimeCluster(endpoint);
}


#endif /* gZclEnablePwrProfileClusterServer_d */




/******************************
  EN50523 Appliance Control Cluster 
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.7
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_ApplianceControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ApplianceControl Cluster Server. 
 *
 */
zbStatus_t ZCL_ApplianceControlClusterServer
(
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
)
{
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccess_c;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterApplianceControl_c}, 0, gApsTxOptionAckTx_c, 1};
  uint8_t applianceStatus = gApplianceStatus_ProgrammedWaitToStart_c;
  
  /* prevent compiler warning */
  (void)pDevice;
      
  /* Create the destination address */
  AF_PrepareForReply(&addrInfo, pIndication);  
   
  pFrame = (void *)pIndication->pAsdu;
  applianceStatus = gApplianceControl_Status;   
   
  /* handle the server commands */
  switch (pFrame->command) 
  {
    case gZclCmdApplianceControl_ExecutionCommand_c:
    {
        uint8_t cmdId = *((uint8_t *)pFrame + sizeof(zclFrame_t));
        status = zclApplianceCtrl_ExecutionCommand_Handler(addrInfo, cmdId, applianceStatus);
        break;
    }
      
    case gZclCmdApplianceControl_SignalState_c:  
    {
      return zclApplianceCtrl_SignalStateRspOrNotif(addrInfo, pFrame->transactionId, gZclCmdApplianceControl_SignalStateResponse_c);
    }
#if (gZclClusterOptionals_d)    
    case gZclCmdApplianceControl_WriteFunctions_c: 
    {
        zclCmdApplCtrl_WriteFunction_t *pCmdPayload;
        
        pCmdPayload = (zclCmdApplCtrl_WriteFunction_t *)(pFrame+1);
        if((pCmdPayload->functionId == gZclAttrApplianceControl_StartTimeId_c)||
            (pCmdPayload->functionId == gZclAttrApplianceControl_FinishTimeId_c)||
             (pCmdPayload->functionId == gZclAttrApplianceControl_RemainingTimeId_c))
           status = ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, pCmdPayload->functionId, gZclServerAttr_c, &pCmdPayload->functionData);
       
        if((pCmdPayload->functionId == gZclAttrApplianceControl_StartTimeId_c)&&(pCmdPayload->functionData !=0x0000))
        {
#if gZclEnablePwrProfileClusterServer_d          
          if(gAppControlTimer == gTmrInvalidTimerID_c)
            gAppControlTimer = ZbTMR_AllocateTimer(); 
          ZbTMR_StartSecondTimer(gAppControlTimer, 60, ZCL_ApplianceControlTimer);  
#endif          
        }
        break;
    }
    
    case gZclCmdApplianceControl_OverloadPauseResume_c: 
    {
      if(applianceStatus == gApplianceStatus_Pause_c)
        (void)ApplianceControlProcessStateMachine(gApplianceStatus_Running_c);
      break;
    }
    
    case gZclCmdApplianceControl_OverloadPause_c:
    {
      (void)ApplianceControlProcessStateMachine(gApplianceStatus_Pause_c);
      break;
    }
    
    case gZclCmdApplianceControl_OverloadWarning_c:
    {
        uint8_t warningEvent = *((uint8_t *)pFrame + sizeof(zclFrame_t));
        /* print warning event */
      	BeeAppUpdateDevice(BeeAppDataInit(appEndPoint), gZclUI_StartWarning_c, 0, 0, &warningEvent);
        break;
    }
#endif   
    
    default:
      return gZclUnsupportedClusterCommand_c;
  }
  if (status == gZbSuccess_c)
    status =(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZbSuccess_c:gZclSuccessDefaultRsp_c;
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_ApplianceControlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t ZCL_ApplianceControlClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  //zbStatus_t status = gZclSuccess_c; 
  /* prevent compiler warning */
  (void)pDevice;

   pFrame = (void *)pIndication->pAsdu;
   
  /* handle the incoming client commands */
  switch (pFrame->command) 
  {
    case gZclCmdApplianceControl_SignalStateResponse_c :
    case gZclCmdApplianceControl_SignalStateNotification_c:
      return ((pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c);
    default:
      return gZclUnsupportedClusterCommand_c;
  }
}

/*!
 * @fn 		zbStatus_t zclApplianceCtrl_ExecutionCommand(zclApplCtrl_ExecutionCommand_t *pReq) 
 *
 * @brief	Sends over-the-air an ExecutionOfCommand request from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_ExecutionCommand
(
    zclApplCtrl_ExecutionCommand_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterApplianceControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdApplianceControl_ExecutionCommand_c,sizeof(zclCmdApplCtrl_ExecutionCommand_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclApplianceCtrl_ExecutionCommand_Handler(  afAddrInfo_t addrInfo, uint8_t commandId,  uint8_t applianceStatus) 
 *
 * @brief	Process ExecutionOfCommand Request received from the Appliance Control Client 
 *
 */
zbStatus_t zclApplianceCtrl_ExecutionCommand_Handler
(
  afAddrInfo_t addrInfo, 
  uint8_t commandId, 
  uint8_t applianceStatus
)
{   
  switch(commandId)
  {
    case gApplCtrl_CommandId_Start:                /* start appliance cycle */
    case gApplCtrl_CommandId_StartSuperFreezing:   /* start superfreezing cycle */  
    case gApplCtrl_CommandId_StartSuperCooling:    /* start supercooling cycle */  
    {
        /* check if an other process is started */
        if(applianceStatus != gApplianceStatus_Running_c)
        {
           applianceStatus = gApplianceStatus_Running_c;
#if (gZclEnablePwrProfileClusterServer_d)            
           gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentEnergyPhaseId = gPwrProfile_EnergyPhaseId1_c;
#endif           
        }
        else
          return gZbFailed_c;
        //BeeAppUpdateDevice(0, gZclUI_On_c, 0, 0, NULL);
        break;
    }
    
    case gApplCtrl_CommandId_Pause:                /* pause appliance cycle */  
    {
      applianceStatus = gApplianceStatus_Pause_c;
      //BeeAppUpdateDevice(0, gZclUI_Off_c, 0, 0, NULL);
      break;
    }
    
    case gApplCtrl_CommandId_Stop:                 /* stop appliance cycle */
    case gApplCtrl_CommandId_StopSuperFreezing:    /* stop superfreezing cycle */
    case gApplCtrl_CommandId_StopSuperCooling:     /* stop supercooling cycle */  
    {
      applianceStatus = gApplianceStatus_StandBy_c;//ApplianceStatus_EndProgrammed_c;
      //BeeAppUpdateDevice(0, gZclUI_Off_c, 0, 0, NULL);
      break;
    }
    
    case gApplCtrl_CommandId_DisableGas:           /* disable gas */
    case gApplCtrl_CommandId_EnableGas:            /* enable gas */
    {
      break;
    }
    
    default :  
       return gZbFailed_c;
  }
  if(ApplianceControlProcessStateMachine(applianceStatus) == gZbSuccess_c)
  { 
     return zclApplianceCtrl_SignalStateRspOrNotif(addrInfo, ZbZclFoundationGlobals(gZclTransactionId), gZclCmdApplianceControl_SignalStateNotification_c);
  }
  
  return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t zclApplianceCtrl_SignalState(zclApplCtrl_CommandWithNoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air an SignalState request from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_SignalState
(
  zclApplCtrl_CommandWithNoPayload_t *pReq
      
)  
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterApplianceControl_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdApplianceControl_SignalState_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclApplianceCtrl_SignalStateRspOrNotif(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t commandId)  
 *
 * @brief	Sends over-the-air an SignalStateResponse/SignalState Notification from the ApplianceControl Cluster Server. 
 *
 */
zbStatus_t zclApplianceCtrl_SignalStateRspOrNotif
(
  afAddrInfo_t addrInfo, 
  uint8_t transactionId,
  uint8_t commandId
)
{
  afToApsdeMessage_t *pCommandRsp;
  zclCmdApplCtrl_SignalStateRspOrNotif_t cmdRspPayload;
  bool_t energyRemote;
  uint8_t len = 0;
  
  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), addrInfo.aClusterId, gZclAttrPowerProfile_EnergyRemoteId_c, gZclServerAttr_c,&energyRemote, NULL);
  
  cmdRspPayload.applianceStatus = gApplianceControl_Status;
  cmdRspPayload.applianceStatus2[0] = 0x00;
  cmdRspPayload.applianceStatus2[1] = 0x00;  
  cmdRspPayload.applianceStatus2[2] = 0x00;
  cmdRspPayload.RemoteEnFlagsDevStatus2 = (0x0F&energyRemote);
  
  len = sizeof(cmdRspPayload);
  pCommandRsp = ZCL_CreateFrame( &addrInfo, 
                          commandId,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &transactionId, 
                          &len,
                          &cmdRspPayload);
  if(!pCommandRsp)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pCommandRsp); 
}

/*!
 * @fn 		zbStatus_t zclApplianceCtrl_WriteFunctions(zclApplCtrl_WriteFunction_t *pReq) 
 *
 * @brief	Sends over-the-air a WriteFunctions command from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_WriteFunctions
(
  zclApplCtrl_WriteFunction_t *pReq    
)  
{
  uint8_t payloadLen;
  
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterApplianceControl_c);	
  payloadLen = sizeof(zclCmdApplCtrl_WriteFunction_t)-1 + ZCL_GetDataTypeLen(pReq->cmdFrame.functionDataType, pReq->cmdFrame.functionData, NULL);
  return ZCL_SendClientReqSeqPassed(gZclCmdApplianceControl_WriteFunctions_c, payloadLen, (zclGenericReq_t *)pReq);
}
 
/*!
 * @fn 		zbStatus_t zclApplianceCtrl_OverloadPauseResume(zclApplCtrl_CommandWithNoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a OverloadPauseResume command from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_OverloadPauseResume
(
  zclApplCtrl_CommandWithNoPayload_t *pReq    
)  
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterApplianceControl_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdApplianceControl_OverloadPauseResume_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclApplianceCtrl_OverloadPause(zclApplCtrl_CommandWithNoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a OverloadPause command from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_OverloadPause
(
  zclApplCtrl_CommandWithNoPayload_t *pReq     
)  
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterApplianceControl_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdApplianceControl_OverloadPause_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclApplianceCtrl_OverloadWarning(zclApplCtrl_OverloadWarning_t *pReq) 
 *
 * @brief	Sends over-the-air a OverloadWarning command from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_OverloadWarning
(
  zclApplCtrl_OverloadWarning_t *pReq   
)  
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterApplianceControl_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdApplianceControl_OverloadWarning_c, sizeof(zclCmdApplCtrl_OverloadWarning_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclApplianceCtrl_OverloadCommand(zclApplCtrl_OverloadCommand_t *pReq)
 *
 * @brief	Sends over-the-air an Overload Command(OverloadWarning/OverloadPause/OverloadPauseResume) from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_OverloadCommand(zclApplCtrl_OverloadCommand_t *pReq)
{ 
  zbStatus_t	 status = gZclUnsupportedClusterCommand_c;
  if(pReq->commandId == gZclCmdApplianceControl_OverloadWarning_c)
  {
	  zclApplCtrl_OverloadWarning_t req;
	  req.zclTransactionId = pReq->zclTransactionId;
	  FLib_MemCpy(&req.addrInfo, &pReq->addrInfo, sizeof(afAddrInfo_t));
	  req.cmdFrame.warningEvent = pReq->cmdData;
	  status = zclApplianceCtrl_OverloadWarning(&req);
  }
  else
  {
	  zclApplCtrl_CommandWithNoPayload_t req;
	  req.zclTransactionId = pReq->zclTransactionId;
	  FLib_MemCpy(&req.addrInfo, &pReq->addrInfo, sizeof(afAddrInfo_t));
	  if(pReq->commandId == gZclCmdApplianceControl_OverloadPauseResume_c)
		  status = zclApplianceCtrl_OverloadPauseResume(&req);	 
	  if(pReq->commandId == gZclCmdApplianceControl_OverloadPause_c)
		  status = zclApplianceCtrl_OverloadPause(&req);	 
  }
  return status;
}


/*!
 * @fn 		zbStatus_t ApplianceControlProcessStateMachine(uint8_t applianceControlStatus)
 *
 * @brief	Process Appliance Control events 
 *
 */
zbStatus_t ApplianceControlProcessStateMachine(uint8_t applianceControlStatus)
{
#if (gZclEnablePwrProfileClusterServer_d)  
  bool_t energyRemote = FALSE;
  zbClusterId_t aClusterId;
  bool_t sendEnPhsNotification = FALSE; 
  afAddrInfo_t addrInfo = {gZbAddrModeIndirect_c, {0x00, 0x00}, 0, {gaZclClusterPowerProfile_c}, 0, gApsTxOptionAckTx_c, 1};
  uint16_t timeToNextState = 0x00;
  uint8_t mLastAppliancCtrlStatus = gApplianceStatus_Idle_c;
  uint8_t j = 0;
  zbClusterId_t clusterId;
  uint8_t endPoint;
  
  Set2Bytes(clusterId, gZclClusterApplianceControl_c);	   
  endPoint = ZCL_GetEndPointForSpecificCluster(clusterId, TRUE, 0, NULL);
   
  /* Create the destination address -  use binding table*/
  addrInfo.radiusCounter = afDefaultRadius_c;  
  Set2Bytes(aClusterId, gZclClusterPowerProfile_c);	
  addrInfo.srcEndPoint = ZCL_GetEndPointForSpecificCluster(addrInfo.aClusterId, TRUE, 0, NULL);
  /* get attribute values */ 
  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), aClusterId, gZclAttrPowerProfile_EnergyRemoteId_c, gZclServerAttr_c, &energyRemote, NULL); 
	
  /* obtain the last applianceControlStatus */
  mLastAppliancCtrlStatus = gApplianceControl_Status;
  /* set the next applianceControlStatus */ 
  gApplianceControl_Status = applianceControlStatus;
   
  /* Note: current power profile = first position of the gPwrProfileServerInf table */ 
  gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileRemoteControl = energyRemote;
  
  switch(applianceControlStatus)
  {
    case gApplianceStatus_Off_c:
    case gApplianceStatus_EndProgrammed_c:  
    {
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentEnergyPhaseId = gPwrProfileServerInf[gPwrProfile_CurrentProfileId].numOfTransferredPhases;
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileState = gPwrProfileState_PwrProfileEnded_c;
      //BeeAppUpdateDevice(0, gZclUI_Off_c, 0, 0, NULL);
      ZbTMR_StopTimer(gApplicationTimer);
      break;
    }
      
    case gApplianceStatus_Idle_c:    
    case gApplianceStatus_StandBy_c:
    {
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentEnergyPhaseId = gPwrProfileServerInf[gPwrProfile_CurrentProfileId].numOfTransferredPhases;
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileState = gPwrProfileState_PwrProfileIdle_c;
      (void)PwrProfile_GetProfileExpectedDuration();
      ZbTMR_StopTimer(gApplicationTimer);
      break;
    }
    
    case gApplianceStatus_Programmed_c:
    {
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentEnergyPhaseId = gPwrProfileServerInf[gPwrProfile_CurrentProfileId].numOfTransferredPhases;
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileState = gPwrProfileState_PwrProfileProgrammed_c;
      ZbTMR_StopTimer(gApplicationTimer);
      break;
    }
    
    case gApplianceStatus_ProgrammedWaitToStart_c:  
    {
      sendEnPhsNotification = TRUE;
      if(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentEnergyPhaseId == gPwrProfileServerInf[gPwrProfile_CurrentProfileId].numOfTransferredPhases)
      {
    	uint16_t finishDuration = Native2OTA16(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentDuration);
        gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentEnergyPhaseId = gPwrProfile_EnergyPhaseId1_c;
        (void)ZCL_SetAttribute(endPoint, clusterId, gZclAttrApplianceControl_FinishTimeId_c, gZclServerAttr_c,&finishDuration);  
      }
      else
        gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentEnergyPhaseId++;
      for(j= 0;j<gPwrProfileServerInf[gPwrProfile_CurrentProfileId].numOfTransferredPhases;j++)
      {
        if(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentEnergyPhaseId == gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[j].energyPhaseID)
        {
         if(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[j].maxActivationDelay != 0x0000)	
        	 timeToNextState = gPwrProfile_DefaultActivationDelay_d;   /* seconds */
         else
        	 timeToNextState = 1; /* 1 seconds */
         break;
        }
      }
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileState = gPwrProfileState_EnergyPhsWaitingToStart_c;
      break;
    }
    
    case gApplianceStatus_Running_c:
    {
      for(j= 0;j<gPwrProfile_MaxEnergyPhaseInf_d;j++)
        if(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentEnergyPhaseId == gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[j].energyPhaseID)
        {
          timeToNextState = OTA2Native16(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[j].expectedDuration)*60;   /* minute */
          break;
        }
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileState = gPwrProfileState_EnergyPhsRunning_c;
      //BeeAppUpdateDevice(0, gZclUI_On_c, 0, 0, NULL);
      break;
    }
    
    case gApplianceStatus_ProgrammedInterrupted_c:
    case gApplianceStatus_Pause_c:   
    {
      timeToNextState = gPwrProfile_DefaultActivationDelay_d; //!=0x00 
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileState = gPwrProfileState_EnergyPhsPaused_c;
      break;
    }
    
    case gApplianceStatus_Service_c:   
    case gApplianceStatus_Failure_c:   
      break;  
    
    case gApplianceStatus_RinseHold_c:    
    case gApplianceStatus_SuperFreezing_c:  
    case gApplianceStatus_SuperCooling_c:
    case gApplianceStatus_SuperHeating_c:  
      break;
    
    default:
      return gZbFailed_c;
  }  
  
  if(sendEnPhsNotification == TRUE)
  {
    gPwrProfileTimer = ZbTMR_AllocateTimer(); 
    if(gPwrProfileTimer == gTmrInvalidTimerID_c)
	return gZbNoMem_c;	
    ZbTMR_StartSecondTimer(gPwrProfileTimer, 0x02, PwrProfile_EnPhsStateNotifTimerCallback);  
  }
  if(timeToNextState != 0x00)
  {
    if((applianceControlStatus == gApplianceStatus_Pause_c)&&(mLastAppliancCtrlStatus == gApplianceStatus_Running_c))
        ZbTMR_StopTimer(gApplicationTimer);
    else
    {
      if((applianceControlStatus ==  gApplianceStatus_Running_c)&&(mLastAppliancCtrlStatus == gApplianceStatus_Pause_c))
        ZbTMR_EnableTimer(gApplicationTimer);
      else
      {
#if (gZclClusterOptionals_d)         
        if(applianceControlStatus == gApplianceStatus_Running_c)
        {
          (void)ZCL_SetAttribute(endPoint, clusterId, gZclAttrApplianceControl_RemainingTimeId_c, gZclServerAttr_c,&gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[j].expectedDuration); 
           timeToNextState = 60;
        }
#endif        
        ZbTMR_StartSecondTimer(gApplicationTimer, timeToNextState, AppManagerTimerCallback);  
      }
    }
  }
  
  BeeAppUpdateDevice(0, gZclUI_ChangeNotification, 0, 0, NULL);
  /* send power profile state notification command*/
  return zclPwrProfile_PwrProfileStateRspOrNotification(addrInfo, ZbZclFoundationGlobals(gZclTransactionId), gZclCmdPowerProfile_PowerProfileStateNotification_c);  
#else
   return gZclSuccess_c;
#endif
}


#if gZclEnablePwrProfileClusterServer_d
/*!
 * @fn 		uint16_t PwrProfile_GetProfileExpectedDuration(void)
 *
 * @brief	Get Power Profile Expected Duration for the current profile
 *
 */
uint16_t PwrProfile_GetProfileExpectedDuration(void)
{
  uint8_t i = 0;
  uint16_t duration = 0;
  zbClusterId_t clusterId;
  //uint8_t endPoint;
  
  Set2Bytes(clusterId, gZclClusterApplianceControl_c);	   
  //endPoint = ZCL_GetEndPointForSpecificCluster(clusterId, TRUE, 0, NULL);
 
  for(i=0; i< gPwrProfileServerInf[gPwrProfile_CurrentProfileId].numOfTransferredPhases; i++)
  {
    if(gPwrProfile_DefaultActivationDelay_d > 60)  
	duration += gPwrProfile_DefaultActivationDelay_d/60;
    duration += OTA2Native16(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].expectedDuration);
  }
  
  gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentDuration = duration;
  
  return duration;
}

/*!
 * @fn 		uint16_t PwrProfile_UpdateProfileDuration(uint8_t stateDuration)
 *
 * @brief   Update Power Profile Duration according with the device running state
 *
 */
uint16_t PwrProfile_UpdateProfileDuration(uint8_t stateDuration)
{
  gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentDuration -= stateDuration;
  return gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentDuration;
}
  
/*!
 * @fn 		void PwrProfile_ChangeEnergyPhaseInformation(bool_t increaseEnPhsInf) 
 *
 * @brief   Change default Configuration by increased or decreased energy phase information
 *
 */
void PwrProfile_ChangeEnergyPhaseInformation(bool_t increaseEnPhsInf) 
{
  uint8_t i=0;
  
  for(i= 0;i<gPwrProfileServerInf[gPwrProfile_CurrentProfileId].numOfTransferredPhases;i++)
  {
	gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].expectedDuration = OTA2Native16(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].expectedDuration);	
    if(increaseEnPhsInf)
    {
     
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].expectedDuration++;
      gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentDuration++;
    }
    else
    {
      if(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].expectedDuration > 1)
      {
        gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].expectedDuration--;
        if(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentDuration)
        {
          gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentDuration--;
        }
      }
    }
    gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].expectedDuration = Native2OTA16(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].expectedDuration);	
    gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].energy = 
    		(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].peakPower >1)?
    				(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].expectedDuration * gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].peakPower):
    					gPwrProfileServerInf[gPwrProfile_CurrentProfileId].energyPhaseInf[i].expectedDuration;
  }
  
  (void)Zcl_PowerProfile_UnsolicitedCommandHandler(gZclCmdPowerProfile_PowerProfileNotification_c, gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileId);  
}

/*!
 * @fn 		static void PwrProfile_EnPhsStateNotifTimerCallback(uint8_t tmr) 
 *
 * @brief   callback for the energy Phase state notification
 *
 */
static void PwrProfile_EnPhsStateNotifTimerCallback(uint8_t tmr) 
{
  /* Create the destination address -  use binding table*/
  afAddrInfo_t addrInfo = {gZbAddrModeIndirect_c, {0x00, 0x00}, 0, {gaZclClusterPowerProfile_c}, 0, gApsTxOptionAckTx_c, 1}; 
  
  (void)tmr;
   addrInfo.radiusCounter = afDefaultRadius_c;
   addrInfo.srcEndPoint =  ZCL_GetEndPointForSpecificCluster(addrInfo.aClusterId, TRUE, 0, NULL);
  /* Note: current power profile = first position of the gPwrProfileServerInf table */  
  (void)zclPwrProfile_EnergyPhsScheduleStateRspOrNotif(addrInfo, ZbZclFoundationGlobals(gZclTransactionId), gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileId , gZclCmdPowerProfile_EnergyPhasesScheduleStateNotification_c);
  ZbTMR_FreeTimer(gPwrProfileTimer);
}
#endif

/*!
 * @fn 		static void ZCL_ApplianceControlTimer( uint8_t timerId )
 *
 * @brief   callback used to update the current configuration (power profile state, duration, appliance status)
 *
 */
#if gZclEnablePwrProfileClusterServer_d 
static void ZCL_ApplianceControlTimer( uint8_t timerId )
{    
 
  uint16_t time;
  zbClusterId_t aClusterId;
  
  /* check start time attribute */
  Set2Bytes(aClusterId, gZclClusterApplianceControl_c);	  
  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), aClusterId, gZclAttrApplianceControl_StartTimeId_c, gZclServerAttr_c,&time, NULL); 
  time = OTA2Native16(time);
  if(time&gApplianceControlTimeValueMask_d)
  {
    /* decrement the time */
    --time;
    /* Note: current power profile =  first position of the gPwrProfileServerInf table */ 
    gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileState = gPwrProfileState_EnergyPhsWaitingToStart_c;
    //BeeAppUpdateDevice(0, gZclUI_Off_c, 0, 0, NULL);
    time = Native2OTA16(time);
    (void)ZCL_SetAttribute(BeeAppDataInit(appEndPoint), aClusterId, gZclAttrApplianceControl_StartTimeId_c, gZclServerAttr_c, &time);
  }
  else
  {
    (void)ApplianceControlProcessStateMachine(gApplianceStatus_Running_c);
    ZbTMR_FreeTimer(gAppControlTimer);
    return;
  }   
  ZbTMR_StartSecondTimer(gAppControlTimer, 60, ZCL_ApplianceControlTimer);
 
}
#endif 

#if gZclEnablePwrProfileClusterServer_d
/*!
 * @fn 		static void AppManagerTimerCallback(uint8_t tmr) 
 *
 * @brief   Callback used to set the Apliance next state. If applianceState = gApplianceStatus_Running_c => nextApplicationStates = gApplianceStatus_EndProgrammed_c. 
 *			If applianceState = gApplianceStatus_ProgrammedWaitToStart_c => nextApplicationStates = gApplianceStatus_Running_c
 *
 */
static void AppManagerTimerCallback(uint8_t tmr) 
{
  uint8_t nextApplianceStates;
  zbClusterId_t clusterId;
  uint8_t endPoint;
  uint16_t remainingTime;
  (void)tmr;
  
#if (gZclClusterOptionals_d)    
  if(gApplianceControl_Status == gApplianceStatus_Running_c)
  {
    Set2Bytes(clusterId, gZclClusterApplianceControl_c);	   
    endPoint = ZCL_GetEndPointForSpecificCluster(clusterId, TRUE, 0, NULL);
    (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrApplianceControl_RemainingTimeId_c, gZclServerAttr_c, &remainingTime, NULL); 
    remainingTime = OTA2Native16(remainingTime);
    remainingTime--;
    remainingTime = Native2OTA16(remainingTime);
    (void)ZCL_SetAttribute(endPoint, clusterId, gZclAttrApplianceControl_RemainingTimeId_c, gZclServerAttr_c, &remainingTime); 
    (void)PwrProfile_UpdateProfileDuration(1);
     BeeAppUpdateDevice(0, gZclUI_ChangeNotification, 0, 0, NULL);
    if(remainingTime)
    {
      ZbTMR_StartSecondTimer(gApplicationTimer, 60, AppManagerTimerCallback);  //timeToNextState
      return;
    }
  }
  else
  {
    (void)PwrProfile_UpdateProfileDuration(gPwrProfile_DefaultActivationDelay_d/60);
    BeeAppUpdateDevice(0, gZclUI_ChangeNotification, 0, 0, NULL);
  }
#endif
  
  nextApplianceStates = gApplianceStatus_ProgrammedWaitToStart_c;
  if(gApplianceControl_Status == gApplianceStatus_Running_c)
  {
    if(gPwrProfileServerInf[gPwrProfile_CurrentProfileId].currentEnergyPhaseId == gPwrProfileServerInf[gPwrProfile_CurrentProfileId].numOfTransferredPhases)
      nextApplianceStates = gApplianceStatus_EndProgrammed_c;
    else
      nextApplianceStates = gApplianceStatus_ProgrammedWaitToStart_c;
  }
  else
  {
    if(gApplianceControl_Status == gApplianceStatus_ProgrammedWaitToStart_c)
      nextApplianceStates = gApplianceStatus_Running_c;
  }
  
  (void)ApplianceControlProcessStateMachine(nextApplianceStates);
}

/*!
 * @fn 		zbStatus_t Zcl_PowerProfile_UnsolicitedCommandHandler(uint8_t commandId, uint32_t data)
 *
 * @brief   Process Power Profile Unsolicited command received from external/internal app.
 *
 */
zbStatus_t Zcl_PowerProfile_UnsolicitedCommandHandler(uint8_t commandId, uint32_t data)
{
  zbStatus_t status = gZclUnsupportedClusterCommand_c;
  /* Create the destination address -  use binding table*/
  afAddrInfo_t addrInfo = {gZbAddrModeIndirect_c, {0x00, 0x00}, 0, {gaZclClusterPowerProfile_c}, 0, gApsTxOptionAckTx_c, 1};
   
  addrInfo.srcEndPoint = ZCL_GetEndPointForSpecificCluster(addrInfo.aClusterId, TRUE, 0, NULL);
  addrInfo.radiusCounter = afDefaultRadius_c;
  data= OTA2Native32(data);  
  /* server unsolicited commands */	 
  if(commandId == gZclCmdPowerProfile_PowerProfileNotification_c)
  {
    status = zclPwrProfile_PwrProfileRspOrNotification(addrInfo, ZbZclFoundationGlobals(gZclTransactionId), (uint8_t )(data&0x000000FF), gZclCmdPowerProfile_PowerProfileNotification_c);
  }
  
  if(commandId == gZclCmdPowerProfile_PowerProfileStateNotification_c)
  {
    /* Note: current power profile: first position of the gPwrProfileServerInf table */    
	   
    gPwrProfileServerInf[gPwrProfile_CurrentProfileId].pwrProfileState = (uint8_t)(data);
 
    status = zclPwrProfile_PwrProfileStateRspOrNotification(addrInfo, gZclTransactionId, gZclCmdPowerProfile_PowerProfileStateNotification_c);
  } 
      
  if(commandId == gZclCmdPowerProfile_PowerProfileScheduleConstraintsNotification_c)
  {
    status = zclPwrProfile_PwrProfileScheduleConstraintsRspOrNotif(addrInfo, ZbZclFoundationGlobals(gZclTransactionId), (uint8_t )data, gZclCmdPowerProfile_PowerProfileScheduleConstraintsNotification_c);
  }
      
  if(commandId == gZclCmdPowerProfile_EnergyPhasesScheduleStateNotification_c)
  {	  	   
    status = zclPwrProfile_EnergyPhsScheduleStateRspOrNotif(addrInfo, ZbZclFoundationGlobals(gZclTransactionId), (uint8_t)data , gZclCmdPowerProfile_EnergyPhasesScheduleStateNotification_c);
  }
      
  return status;
}
#endif


#if (gZclEnablePwrProfileClusterServer_d || gZclEnablePwrProfileClusterClient_d) &&  gZclEnablePartitionPwrProfile_d   
/*!
 * @fn 		void ZCL_PowerProfile_PartitionInit(void)
 *
 * @brief   Init PowerProfile - Partition configuration. In case of messages that will not fit into a single Zigbee payload, 
 *			the Partition Cluster will be used (managed by the application). This function will register the PowerProfile to Partition Cluster.
 *
 */
void ZCL_PowerProfile_PartitionInit(void)
{
  zbClusterId_t aClusterId ={gaZclClusterPowerProfile_c};
  zclPartitionedClusterInfo_t clusterReg;
  Copy2Bytes(&clusterReg.clusterId, aClusterId);
  clusterReg.dataIndHandler = PowerProfilePartitionServer;
  clusterReg.dataReqHandler = PowerProfilePartitionCallback;
  clusterReg.clusterBufferSize = gMaxPwrProfilePartitionBuffer;
  
  gpPwrProfilePartitionTxPendingFrame = NULL;
  gPwrProfilePartitionRxPendingFrame.framePending = FALSE;  
  
  (void)ZCL_PartitionRegisterCluster(&clusterReg);
}

/*!
 * @fn 		static uint8_t* PowerProfilePartitionCallback(zclPartitionAppEvent_t* pPartitionEvent, uint8_t* dataLen) 
 *
 * @brief   PowerProfile-Partition configuration callback. 
 *
 */
static uint8_t* PowerProfilePartitionCallback
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
    
    if (gPwrProfilePartitionRxPendingFrame.totalLen - bufferIndex < partitionSize)
      dataLength = gPwrProfilePartitionRxPendingFrame.totalLen - bufferIndex;
    
    msg = MSG_Alloc(partitionSize);
    if(bufferIndex == 0)
    {
      zclFrame.frameControl = gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp; 
      zclFrame.transactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
      zclFrame.command = gPwrProfilePartitionRxPendingFrame.commandId;
      FLib_MemCpy(&msg[0], &zclFrame, sizeof(zclFrame_t)); 
      /* added requested data into the frame */
      FLib_MemCpy(&msg[sizeof(zclFrame_t)], &gPwrProfilePartitionRxPendingFrame.data[bufferIndex], partitionSize - sizeof(zclFrame_t));   
    }
    else
    {
      /* added requested data into the frame*/
      if(bufferIndex + partitionSize > gPwrProfilePartitionRxPendingFrame.totalLen)
      {
        FLib_MemCpy(&msg[0], &gPwrProfilePartitionRxPendingFrame.data[bufferIndex], gPwrProfilePartitionRxPendingFrame.totalLen-bufferIndex);   
        MSG_Free(gPwrProfilePartitionRxPendingFrame.data);
      }
      else
        FLib_MemCpy(&msg[0], &gPwrProfilePartitionRxPendingFrame.data[bufferIndex], partitionSize);   
    }
      
    *dataLen = dataLength;
    return msg;        
    
  } 
  else if  (pPartitionEvent->eventType == gZclPartitionAppEventType_FrameComplete) 
  {
      /* data is received, now you can read */
      gPwrProfilePartitionRxPendingFrame.framePending = FALSE;       
      
  }
  else if  (pPartitionEvent->eventType == gZclPartitionAppEventType_TxFrameComplete) 
  {
      PwrPRofilePartitionTxFrameBuffer_t* currentBuffer = gpPwrProfilePartitionTxPendingFrame;
      while (currentBuffer != NULL) {
        PwrPRofilePartitionTxFrameBuffer_t* nextBuffer = 
                (PwrPRofilePartitionTxFrameBuffer_t*)currentBuffer->nextBuffer;
        MSG_Free(currentBuffer);
        currentBuffer = nextBuffer;
      } 
      gpPwrProfilePartitionTxPendingFrame = NULL;
  }  
  return NULL;
}       

/*!
 * @fn 		static void PowerProfilePartitionServer(zclPartitionedDataIndication_t* partitionDataIndication) 
 *
 * @brief   Manage Data Received Using Partition Cluster.
 *
 */
static void PowerProfilePartitionServer(zclPartitionedDataIndication_t* partitionDataIndication) 
{
  if (!gPwrProfilePartitionRxPendingFrame.framePending)
  {
    gPwrProfilePartitionRxPendingFrame.framePending = TRUE;
    gPwrProfilePartitionRxPendingFrame.zclTransactionId = partitionDataIndication->zclTransactionId;
  }
  /* for receiving data using partition (not used in transmition) */
  if (gPwrProfilePartitionRxPendingFrame.zclTransactionId == partitionDataIndication->zclTransactionId)
  {
    /* compute buffer offset */
    uint16_t bufferIndex = partitionDataIndication->partitionId * partitionDataIndication->partitionLength;
    /* update stored data length */
    gPwrProfilePartitionRxPendingFrame.totalLen = bufferIndex + partitionDataIndication->dataLength;
    /* store the data comming using partition in the memory */
    PwrProfile_ProcessDataRcvUsingPartition(partitionDataIndication->partitionedFrame, (uint8_t)bufferIndex);
  }
}

 /*!
 * @fn 		static zbStatus_t PowerProfile_TransferDataPtr(uint8_t commandID, bool_t isServer, uint8_t *pReq) 
 *
 * @brief   Use this function to start a PowerProfile Transfer using Partition Cluster
 *
 */
static zbStatus_t PowerProfile_TransferDataPtr
(
    uint8_t commandID, bool_t isServer, uint8_t *pReq
) 
{
  uint32_t reqPayloadLen;
  uint8_t  fixedDataLen = 0; 
  zclPartitionedClusterFrameInfo_t partitionTxFrameInfo;
  
  if(isServer == TRUE)
  {
    /* Pwr Profile server commands: */
    switch(commandID)
    {
      case gZclCmdPowerProfile_PowerProfileNotification_c:
      case gZclCmdPowerProfile_PowerProfileResponse_c:  
      {
          zclPwrProfile_PwrProfileRspOrNotification_t *pCmd;
          pCmd = (zclPwrProfile_PwrProfileRspOrNotification_t *)pReq;
          fixedDataLen = 3; /* totalProfileNum, powerProfileID, numOfTransferredPhases */
          reqPayloadLen = fixedDataLen + pCmd->cmdFrame.numOfTransferredPhases*sizeof(zclPwrProfile_EnergyPhaseInf_t)+  sizeof(zclFrame_t);
          break;
      }
      
      case gZclCmdPowerProfile_PowerProfileStateResponse_c:
      case gZclCmdPowerProfile_PowerProfileStateNotification_c:
      {
        zclPwrProfile_PwrProfileStateRspOrNotification_t *pCmd;
        pCmd = (zclPwrProfile_PwrProfileStateRspOrNotification_t *)pReq;
        fixedDataLen = 1; /* pwrProfileCount*/
        reqPayloadLen = fixedDataLen + pCmd->cmdFrame.pwrProfileCount*sizeof(zclPwrProfile_PwrProfileRecord_t)+  sizeof(zclFrame_t);      
        break;
      }
      
      case gZclCmdPowerProfile_EnergyPhasesScheduleStateResponse_c:
      case gZclCmdPowerProfile_EnergyPhasesScheduleStateNotification_c:
      {
        zclPwrProfile_EnergyPhsScheduleStateRspOrNotif_t *pCmd;
        pCmd = (zclPwrProfile_EnergyPhsScheduleStateRspOrNotif_t *)pReq;
        fixedDataLen = 2; /* pwrProfileId, numOfScheduledPhases*/
        reqPayloadLen = fixedDataLen + pCmd->cmdFrame.numOfScheduledPhases*sizeof(zclCmdPwrProfile_EnegyPhsScheduledTime_t)+  sizeof(zclFrame_t);      
        break;    
      }
      
      default:
        return gZbFailed_c;
    }
  }
  else
  {
    /* Pwr Profile client commands: */  
    if(commandID == gZclCmdPowerProfile_EnergyPhasesScheduleNotification_c ||
       commandID == gZclCmdPowerProfile_EnergyPhasesScheduleResponse_c)  
      {
        zclPwrProfile_EnergyPhsScheduleStateRspOrNotif_t *pCmd;
        pCmd = (zclPwrProfile_EnergyPhsScheduleStateRspOrNotif_t *)pReq;
        fixedDataLen = 2; // pwrProfileId, numOfScheduledPhases
        reqPayloadLen = fixedDataLen + pCmd->cmdFrame.numOfScheduledPhases*sizeof(zclCmdPwrProfile_EnegyPhsScheduledTime_t)+  sizeof(zclFrame_t);       
      }
    else
      return gZbFailed_c;
  }
  
  if(gPwrProfilePartitionRxPendingFrame.data)
  {
    return gZbBusy_c;
  }
  gPwrProfilePartitionRxPendingFrame.data = AF_MsgAlloc();
    if(!gPwrProfilePartitionRxPendingFrame.data)
      return gZbNoMem_c;  
  gPwrProfilePartitionRxPendingFrame.commandId = commandID;
  gPwrProfilePartitionRxPendingFrame.totalLen = reqPayloadLen;
  FLib_MemCpy(&gPwrProfilePartitionRxPendingFrame.data, &pReq[sizeof(afAddrInfo_t)+1], reqPayloadLen - sizeof(zclFrame_t));
  partitionTxFrameInfo.zclTransactionId = gpPwrProfilePartitionTxPendingFrame->payload.zclHeader.transactionId;
  FLib_MemCpy(&partitionTxFrameInfo.addrInfo, &pReq[0], sizeof(afAddrInfo_t));
  partitionTxFrameInfo.addrInfo.txOptions &= ~gApsTxOptionFragmentationRequested_c;
  
  /*set partition Frame Size atribute */
  gDefaultPartitionAttrs.partitionedFrameSize = 0x40;
  
  partitionTxFrameInfo.frameCount = (uint16_t)(reqPayloadLen / gDefaultPartitionAttrs.partitionedFrameSize); 
  if (reqPayloadLen % gDefaultPartitionAttrs.partitionedFrameSize != 0)
    partitionTxFrameInfo.frameCount++;
   
  partitionTxFrameInfo.frameCount = Native2OTA16(partitionTxFrameInfo.frameCount);
  FLib_MemCpy((void *)&partitionTxFrameInfo.partitionAttrs, (void *)&gDefaultPartitionAttrs, sizeof(zclPartitionAttrs_t));
  partitionTxFrameInfo.partitionAttrs.partitionedFrameSize = gDefaultPartitionAttrs.partitionedFrameSize;
  (void)ZCL_PartitionRegisterTxFrame(&partitionTxFrameInfo);
  return gZbSuccess_c;

}

 /*!
 * @fn 		static void PwrProfile_ProcessDataRcvUsingPartition(uint8_t *pData, uint8_t bufferIndex)
 *
 * @brief   Process Data Received via Partition Cluster
 *
 */
static void PwrProfile_ProcessDataRcvUsingPartition(uint8_t *pData, uint8_t bufferIndex)
{
  zclFrame_t zclFrame;

  if(bufferIndex == 0)
    FLib_MemCpy(&zclFrame, &pData[1], sizeof(zclFrame_t)); 
  
  if(zclFrame.frameControl | gZclFrameControl_DirectionRsp == 0)
  {
    /* Pwr Profile server commands: */
    switch(zclFrame.command)
    {
      case gZclCmdPowerProfile_PowerProfileNotification_c:
      case gZclCmdPowerProfile_PowerProfileResponse_c:  
        /* check payload */
        break;
      case gZclCmdPowerProfile_PowerProfileStateResponse_c:
      case gZclCmdPowerProfile_PowerProfileStateNotification_c:
        /* check payload */
        break;
      case gZclCmdPowerProfile_EnergyPhasesScheduleStateResponse_c:
      case gZclCmdPowerProfile_EnergyPhasesScheduleStateNotification_c:
        /* check payload */
        break;      
      default:
        return;  
    }
  }
  else
  {
    /* Pwr Profile client commands: */  
    if(zclFrame.command == gZclCmdPowerProfile_EnergyPhasesScheduleNotification_c ||
       zclFrame.command == gZclCmdPowerProfile_EnergyPhasesScheduleResponse_c)
    {
       /* check payload */
    }     
  }
}
#endif  /* (gZclEnablePwrProfileClusterServer_d || gZclEnablePwrProfileClusterClient_d) && gZclEnablePartitionPwrProfile_d*/
