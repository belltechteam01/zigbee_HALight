/*! @file 	  ZllCommissioning.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for Zll Commissioning cluster (touchlink commissioning).
 *
 * @copyright Copyright(c) 2014, Freescale, Inc. All rights reserved.
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
 *  [R1] - docs-11-0037-10-0zll-zigbee-light-link-zll-profile-specification.pdf
 *  [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf 
 */
    
#include "ZllCommissioning.h"  
#include "HaProfile.h"
#include "ZclFoundation.h"
#include "ZdoApsInterface.h"
#include "ASL_ZdpInterface.h"
#include "SecLib.h"   
#include "ZdoApsInterface.h"
#include "BeeStackInterface.h"
#include "BeeStackUtil.h"
#include "ZdoNwkManager.h"   
#include "nwkcommon.h"
#include "ZdoSecurityManager.h"
#include "ZdpUtils.h"
#include "Beestack_Globals.h"
#include "Phy.h"
#include "EndpointConfig.h"
#include "ZDOVariables.h"
#include "ZdoApsInterface.h"
#include "BeeApp.h"

#if gASL_EnableEZCommissioning_d
#include "EzCommissioning.h"
#endif


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
#if gInterPanCommunicationEnabled_c
/* Zll commissioning Target command handler: */
static zbStatus_t ZllTouchlink_ScanReqHandler(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_IdentifyReqHandler(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_DeviceInformationReqHandler(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_ResetToFactoryNewReqHandler(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_NetworkStartReqHandler(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_NetworkJoinRouterReqHandler(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_NetworkJoinEndDeviceReqHandler(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_NetworkUpdateReqHandler(zbInterPanDataIndication_t *pIndication);

/* Zll touchlink target callbacks: */
static void ZllTouchlink_TargetSetTimeout(uint32_t duration);
static void ZllTouchlink_TargetTimeoutCallBack(uint8_t tmrId);

/* Zll commissioning target procedure: */
static bool_t ZllTouchlink_TargetNtwDiscoveryCnf(nlmeNetworkDiscoveryConf_t  *pNetworkDiscoveryConf);
static void   ZllTouchlink_TargetNtwStart(void);

/* Zll commissioning Initiator command handler: */
#if gASL_EnableZllCommissioning_Initiator_d
static zbStatus_t ZllTouchlink_ScanRspHandler(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_DeviceInformationRsp(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_NtwStartRspHandler(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_NtwJoinRouterRspHandler(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZllTouchlink_NtwJoinEndDeviceRspHandler(zbInterPanDataIndication_t *pIndication);

/* Zll commissioning initiator callbacks: */
static void ZllTouchlink_InitiatorSetTimeout(uint32_t duration);
static void ZllTouchlink_InitiatorTimeoutCallBack(uint8_t tmrId);
static void ZllTouchlink_InitiatorDeviceDiscoveryCallBack(void);
static void ZllTouchlink_RejoinCallBack(void);
static bool_t ZllTouchlink_AddToRemoteDeviceTable(zbIeeeAddr_t ieeeAddr, zbNwkAddr_t ntwAddr, 
                                                  uint8_t endpointCount, uint8_t *endPointList, 
                                                  zbProfileId_t profileId, zbProfileId_t *deviceIdList);
void ZllTouchlink_UpdateNtwkAddrRemoteDeviceTable(zbIeeeAddr_t ieeeAddr, zbNwkAddr_t ntwAddr);
#endif /* gASL_EnableZllCommissioning_Initiator_d */
/* Zll commissioning Security */
static void ZllCommissioningSecurity_GetZllKey(uint8_t keyIndex, uint8_t *outputZllKey);
#if gASL_EnableZllCommissioning_Initiator_d
static void ZllCommissioningSecurity_EncryptNetworkKey(uint32_t transactionIdReq, uint32_t transactionIdRsp, zbAESKey_t inZllkey, zbAESKey_t outputKey);
#endif
static void ZllCommissioningSecurity_DecryptNetworkKey(uint32_t transactionIdReq, uint32_t transactionIdRsp, zbAESKey_t inZllkey, zbAESKey_t encryptedNwkKey, zbAESKey_t outputKey);

/* Zll commissioning network params */
static void ZllTouchlink_SetReceiverOn(uint8_t deviceType);
#if gComboDeviceCapability_d || gEndDevCapability_d
void ZllTouchlink_SetReceiverOff(uint8_t deviceType);
#endif
static void ZllTouchlink_SetNtwParam(zbPanId_t panId, zbIeeeAddr_t extendedPanId, uint8_t logicalChannel, zbNwkAddr_t ntwAddress);
#if gASL_EnableZllCommissioning_Initiator_d
static void ZllTouchlink_GetAvailableGroupIds(uint16_t *pGroupRangeBegin, uint16_t *pGroupRangeEnd, uint8_t requiredGroupNo);
static void ZllTouchlink_GetAvailableFreeRanges(uint16_t *pGroupBegin, uint16_t *pGroupEnd, uint16_t *pNtwAddrBegin, uint16_t *pNtwAddrEnd, bool_t ntwAssign);
static void ZllTouchlink_InitiatorUpdateFreeRanges(bool_t ntwAssign, uint8_t groupNo, bool_t status);
#endif
static uint16_t ZllTouchlink_GetKeyBitmask(void);
static uint8_t ZllTouchlink_GetDefaultLogicalChannel(void);
void ZllTouchlink_InitVariables(void);
#endif /* gInterPanCommunicationEnabled_c */


/* ZLL Commissioning Utility Server commands handler*/
zbStatus_t ZllCommissioningUtility_GetEndpointListReqHandler(zbApsdeDataIndication_t *pIndication);
zbStatus_t ZllCommissioningUtility_GetGroupIdReqHandler(zbApsdeDataIndication_t *pIndication);

#if (gStandardSecurity_d || gHighSecurity_d)
extern void SSP_NwkResetSecurityMaterials(void);
#endif
extern void PWRLib_Reset(void);

#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
extern void  NWK_FRAMEHANDLE_SetBeaconPayloadRAM(void);
#endif

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
#if !gInstantiableStackEnabled_d

/* Zll commissioning touchlink setup: */
zllCommissioning_TouchlinkSetup_t gTouchLinkSetup; /* should be stored in nvm */

/* Zll commissioning touchlink static data */
static bool_t gZllCommissioningDeviceInNonZllNtw = FALSE;
uint8_t gZllCommissioningDefaultDeviceType = gZllCommissioningDefaultDeviceType_d;

#if gInterPanCommunicationEnabled_c
#if gASL_EnableZllCommissioning_Initiator_d
static bool_t mZllExtendedScanReq = FALSE;
#endif
static uint32_t mTouchLinkScanReqId  = 0x00;   
static uint32_t mTouchLinkScanRspId  = 0x00;
static zllCommissioning_TouchlinkSession_t mTouchLinkSession;

#if gComboDeviceCapability_d || gEndDevCapability_d 
uint16_t gZllOrgPollRate;
#if gASL_EnableZllCommissioning_Initiator_d
bool_t gEndDevForcedTouchlink = FALSE;
#endif
#endif 
#endif /* gInterPanCommunicationEnabled_c */

/* initiator remote device data */
#if gASL_EnableZllCommissioning_Initiator_d && gASL_EnableZllTouchlinkCommissioning_d
static uint8_t *mpTouchLinkTempRemoteTarget = NULL;
static zllCommissioning_TouchlinkInitiatorSession_t mTouchLinkInitiatorSession;
zllTouchlink_RemoteDeviceTable_t gZllRemoteDeviceTable[gZLLCommissioning_MaxRemoteDevices_c];
#endif /* gASL_EnableZllCommissioning_Initiator_d */


#else /* !gInstantiableStackEnabled_d */

zllTouchlinkData_t *pZllTouchlinkData;
zllTouchlinkData_t  ZllTouchlinkData[2]; 
#endif

#if gInterPanCommunicationEnabled_c
/* ZLL primary logical channels*/
static uint8_t gZllPrimaryLogicalChannels[4] = {gLogicalChannel11_c, gLogicalChannel15_c, 
                                                gLogicalChannel20_c, gLogicalChannel25_c};
#endif /* gInterPanCommunicationEnabled_c */

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/
const zclCmd_t gaZclZllCommissioningCmdReceivedDef[]={
  // commands received 
  gZclCmdZllCommissioning_ScanReq_c, gZclCmdZllCommissioning_DeviceInformationReq_c,
  gZclCmdZllCommissioning_IdentifyReq_c, gZclCmdZllCommissioning_ResetToFactoryNewReq_c,
  gZclCmdZllCommissioning_NetworkStartReq_c, gZclCmdZllCommissioning_NetworkJoinRouterReq_c,
  gZclCmdZllCommissioning_NetworkJoinEndDeviceReq_c, gZclCmdZllCommissioning_NetworkUpdateReq_c,
  gZclCmdZllCommissioning_GetGroupIdReq_c, gZclCmdZllCommissioning_GetEndpointListReq_c
};
const zclCmd_t gaZclZllCommissioningCmdGeneratedDef[]={
  // commands generated 
  gZclCmdZllCommissioning_ScanRsp_c, gZclCmdZllCommissioning_DeviceInformationRsp_c,
  gZclCmdZllCommissioning_NetworkStartRsp_c, gZclCmdZllCommissioning_NetworkJoinRouterRsp_c,
  gZclCmdZllCommissioning_NetworkJoinEndDeviceRsp_c, gZclCmdZllCommissioning_EndpointInformation_c,
  gZclCmdZllCommissioning_GetGroupIdRsp_c, gZclCmdZllCommissioning_GetEndpointListRsp_c
};

const zclCommandsDefList_t gZclZllCommissioningClusterCommandsDefList =
{
   NumberOfElements(gaZclZllCommissioningCmdReceivedDef),  gaZclZllCommissioningCmdReceivedDef,
   NumberOfElements(gaZclZllCommissioningCmdGeneratedDef), gaZclZllCommissioningCmdGeneratedDef
};


/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/**********************************************
    [R1] 7.1. ZLl commissioning Cluster 
**********************************************/
#if gInterPanCommunicationEnabled_c
/*!
 * @fn 		zbStatus_t ZCL_ZllTouchlinkClusterServer(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the InterPAN Zll Commisisoning (Touchlink)Cluster server. 
 *
 */
zbStatus_t ZCL_ZllTouchlinkClusterServer
(
   zbInterPanDataIndication_t *pIndication, /* IN: */
   afDeviceDef_t *pDevice                   /* IN: */
) 
{
   zclFrame_t *pFrame;
   zbStatus_t status = gZclSuccess_c;
   zbProfileId_t   zllProfileId = {gZllProfileIdentifier_d};

   /* to prevent compiler warning */
   (void)pDevice;

   /* check profile Id */
   if(!FLib_Cmp2Bytes(pIndication->aProfileId, zllProfileId))
     return gZclMalformedCommand_c;
   
   /* get frame information */
   pFrame = (void *)pIndication->pAsdu;
      
   /* handle incoming server commands */
   switch (pFrame->command)
   {
      case gZclCmdZllCommissioning_ScanReq_c:    
        status = ZllTouchlink_ScanReqHandler(pIndication);
        break; 
      case gZclCmdZllCommissioning_DeviceInformationReq_c:
        status = ZllTouchlink_DeviceInformationReqHandler(pIndication);
        break;     
      case gZclCmdZllCommissioning_IdentifyReq_c:
        status = ZllTouchlink_IdentifyReqHandler(pIndication);
        break;         
      case gZclCmdZllCommissioning_ResetToFactoryNewReq_c:   
        status = ZllTouchlink_ResetToFactoryNewReqHandler(pIndication);
        break;  
      case gZclCmdZllCommissioning_NetworkStartReq_c:
        status = ZllTouchlink_NetworkStartReqHandler(pIndication);
        break;          
      case gZclCmdZllCommissioning_NetworkJoinRouterReq_c:
        status = ZllTouchlink_NetworkJoinRouterReqHandler(pIndication);
        break;          
      case gZclCmdZllCommissioning_NetworkJoinEndDeviceReq_c:
        status = ZllTouchlink_NetworkJoinEndDeviceReqHandler(pIndication);
        break;            
      case gZclCmdZllCommissioning_NetworkUpdateReq_c:   
        status = ZllTouchlink_NetworkUpdateReqHandler(pIndication);
        break;   
      default:
        status = gZclUnsupportedClusterCommand_c;
        break;
   }
   return status;
}


/********************************************************************************************
 *                   Touchlink target commands:                                             *
 ********************************************************************************************/

/*!
 * @fn 		zbStatus_t zclZllTouchlink_ScanRsp(zllCommissioning_ScanResponse_t *pReq)
 *
 * @brief	Sends over-the-air a Scan Response from the Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllTouchlink_ScanRsp
(
  zllCommissioning_ScanResponse_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_ScanResponse_t);	
  if(pReq->cmdFrame.noOfSubdevices != 0x01)
  {
    /* The endpoint, profileId, device Id, Version, Group Identifier Count should not be present */
    payloadLen-= (3+2*sizeof(uint16_t)); 
  }
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanServerReqSeqPassed(gZclCmdZllCommissioning_ScanRsp_c, payloadLen, pReq);
} 

/*!
 * @fn 		zbStatus_t zclZllTouchlink_DeviceInfRsp(zllCommissioning_DeviceInfRsp_t *pReq)
 *
 * @brief	Sends over-the-air DeviceInf response from the Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllTouchlink_DeviceInfRsp
(
  zllCommissioning_DeviceInfRsp_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_DeviceInfRsp_t) + (pReq->cmdFrame.deviceInfRecordCount - 1)*sizeof(zllCommissioning_DeviceInfRecord_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanServerReqSeqPassed(gZclCmdZllCommissioning_DeviceInformationRsp_c, payloadLen, pReq);
} 

/*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkStartRsp(zllCommissioning_NtwStartRsp_t *pReq)
 *
 * @brief	Sends over-the-air NetworkStartResponse from the Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkStartRsp
(
  zllCommissioning_NtwStartRsp_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_NtwStartRsp_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanServerReqSeqPassed(gZclCmdZllCommissioning_NetworkStartRsp_c, payloadLen, pReq);
} 

 /*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkJoinRouterRsp(zllCommissioning_NtwJoinRouterRsp_t *pReq)
 *
 * @brief	Sends over-the-air NetworkJoinRouterResponse from the Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkJoinRouterRsp
(
  zllCommissioning_NtwJoinRouterRsp_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_NtwJoinRouterRsp_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanServerReqSeqPassed(gZclCmdZllCommissioning_NetworkJoinRouterRsp_c, payloadLen, pReq);
} 

 /*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkJoinEndDeviceRsp(zllCommissioning_NtwJoinEndDeviceRsp_t *pReq)
 *
 * @brief	Sends over-the-air NetworkJoinEndDeviceResponse from the Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkJoinEndDeviceRsp
(
  zllCommissioning_NtwJoinEndDeviceRsp_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_NtwJoinEndDeviceRsp_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanServerReqSeqPassed(gZclCmdZllCommissioning_NetworkJoinEndDeviceRsp_c, payloadLen, pReq);
} 


/********************************************************************************************
 *                   Touchlink target  command handler                                      *
 ********************************************************************************************/

/*!
 * @fn 		static zbStatus_t ZllTouchlink_ScanReqHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Scan Req Command received from Zll Initiator Device
 *
 */
static zbStatus_t ZllTouchlink_ScanReqHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCommissioning_ScanResponse_t   *pScanRspCmd;
  zllCmdCommissioning_ScanRequest_t scanReqPayload;
  zbPanId_t dstPanId = {0xFF, 0xFF};
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccess_c;
  
  /* verify RSSI */
  if(pIndication->linkQuality < ZllTouchlinkConfigData(mTouchLinkSession.minRssi))
    return gZclSuccess_c;
    
  pFrame = (void *)pIndication->pAsdu;
  scanReqPayload = *(zllCmdCommissioning_ScanRequest_t *)(pFrame+1);
  
  /* verify interpan transaction Id and initiator flag*/
  if((scanReqPayload.interPanTransactionId == 0x00) || (scanReqPayload.interPanTransactionId == ZllTouchlinkConfigData(mTouchLinkScanReqId)) ||
     (scanReqPayload.zllInf.initiator != 0x01))
    return gZclSuccess_c;
    
  /* verify Zigbee information */
  if(scanReqPayload.zbInf.logicalType > gEndDevice_c)
    return gZclSuccess_c;
  
#if gASL_EnableZllCommissioning_Initiator_d  
  /* verify if the target is not in the scan procedure */
  if(ZllTouchlinkConfigData(mTouchLinkScanReqId) != 0x00 && scanReqPayload.zllInf.factoryNew && ZbTMR_IsTimerActive(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr)))
    return gZclSuccess_c;
#endif    
  
  /* get initiator capability information */
  ZllTouchlinkConfigData(mTouchLinkSession.initiatorCapabilityInf) = (scanReqPayload.zbInf.rxOnIdle==0x01)?0x08:0x00; /* rxonIdle bit*/
  if(scanReqPayload.zbInf.logicalType == gRouter_c)
    ZllTouchlinkConfigData(mTouchLinkSession.initiatorCapabilityInf) |= 0x01; /* deviceType bit */
  
  /* reply to sender */
  pScanRspCmd = AF_MsgAlloc();
  if(!pScanRspCmd)
    return gZclNoMem_c;
  
  /* keep the interpan transaction Id*/
  ZllTouchlinkConfigData(mTouchLinkScanReqId) = scanReqPayload.interPanTransactionId;
  
  /* get address ready for reply */
  PrepareInterPanForReply(&pScanRspCmd->addrInfo, pIndication);
  pScanRspCmd->addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  Copy2Bytes(pScanRspCmd->addrInfo.dstPanId, dstPanId);
  pScanRspCmd->zclTransactionId = pFrame->transactionId;
    
  /* complete the command payload */
  pScanRspCmd->cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);
  pScanRspCmd->cmdFrame.rssiCorrection = gZllCommissioning_RssiCorrection_d; 
  
  /* set zigbee information */
  pScanRspCmd->cmdFrame.zbInf.logicalType = NlmeGetRequest(gDevType_c);
  pScanRspCmd->cmdFrame.zbInf.reserved = 0x00;
  pScanRspCmd->cmdFrame.zbInf.rxOnIdle = 0x01;
  if(pScanRspCmd->cmdFrame.zbInf.logicalType == gEndDevice_c)
    pScanRspCmd->cmdFrame.zbInf.rxOnIdle = 0x00;
  
  /* set Zll information */
  pScanRspCmd->cmdFrame.zllInf.priorityReq = (NlmeGetRequest(gDevType_c) == gEndDevice_c)?TRUE:FALSE;
  pScanRspCmd->cmdFrame.zllInf.factoryNew = ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus);   
#if gASL_EnableZllCommissioning_Initiator_d  
  pScanRspCmd->cmdFrame.zllInf.initiator = 0x01;
  pScanRspCmd->cmdFrame.zllInf.addrAssignment = 0x01;
#else 
  pScanRspCmd->cmdFrame.zllInf.initiator = 0x00;  
  pScanRspCmd->cmdFrame.zllInf.addrAssignment = 0x00;
#endif 
  pScanRspCmd->cmdFrame.zllInf.reserved1 = 0x00;
  pScanRspCmd->cmdFrame.zllInf.reserved2 = 0x00;
  
  pScanRspCmd->cmdFrame.keyBitmask = ZllTouchlink_GetKeyBitmask();
  pScanRspCmd->cmdFrame.responseId = GetRandomNumber();
  ZllTouchlinkConfigData(mTouchLinkScanRspId) = pScanRspCmd->cmdFrame.responseId;
  Copy8Bytes(pScanRspCmd->cmdFrame.extendedPanId, NlmeGetRequest(gNwkExtendedPanId_c));
  pScanRspCmd->cmdFrame.nwkUpdateId = NlmeGetRequest(gNwkUpdateId_c); 
  pScanRspCmd->cmdFrame.logicalChannel = NlmeGetRequest(gNwkLogicalChannel_c);
  Copy2Bytes(pScanRspCmd->cmdFrame.panId, NlmeGetRequest(gNwkPanId_c));  
  if(ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
  {
    pScanRspCmd->cmdFrame.nwkAddr[0] = pScanRspCmd->cmdFrame.nwkAddr[1] = 0xFF;
  }
  else
  {
    Copy2Bytes(pScanRspCmd->cmdFrame.nwkAddr, NlmeGetRequest(gNwkShortAddress_c));
  }
#if gASL_EnableZllCommissioning_Initiator_d   
  pScanRspCmd->cmdFrame.totalGroups = gZLLCommissioning_MaxNoOfGroups_c;
#else
  pScanRspCmd->cmdFrame.totalGroups = 0x00;
#endif

#if gInstantiableStackEnabled_d 
  pScanRspCmd->cmdFrame.noOfSubdevices = EndPointConfigData(gNum_EndPoints);
#else
  pScanRspCmd->cmdFrame.noOfSubdevices = gNum_EndPoints_c;
#endif 
  if(pScanRspCmd->cmdFrame.noOfSubdevices == 0x01)
  {
     pScanRspCmd->cmdFrame.endPoint = EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint);
     Copy2Bytes(pScanRspCmd->cmdFrame.profileId, pIndication->aProfileId);
     Copy2Bytes(pScanRspCmd->cmdFrame.deviceId, EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->aAppDeviceId));
     pScanRspCmd->cmdFrame.version = EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->appDevVerAndFlag);
     pScanRspCmd->cmdFrame.groupIdCount = 0x00;
  }
    
  /* send the scan response */
  status = zclZllTouchlink_ScanRsp(pScanRspCmd);
  /* timeout = gZllAplcInterPanTransIdLifeTime_d */
  ZllTouchlink_TargetSetTimeout(gZllAplcInterPanTransIdLifeTime_d);
  
  MSG_Free(pScanRspCmd);
  return  status;
}
/*!
 * @fn 		static zbStatus_t ZllTouchlink_DeviceInformationReqHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Device Information Command received from Zll Initiator Device
 *
 */
static zbStatus_t ZllTouchlink_DeviceInformationReqHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCommissioning_DeviceInfRsp_t *pDeviceInfRspCmd;
  zllCmdCommissioning_DeviceInformationReq_t deviceInfReqPayload;
  zbPanId_t dstPanId = {0xFF, 0xFF};
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccess_c;
  
  pFrame = (void *)pIndication->pAsdu;
  deviceInfReqPayload = *(zllCmdCommissioning_DeviceInformationReq_t *)(pFrame+1);
  
  /* test the interpan transaction Id*/
  if((deviceInfReqPayload.interPanTransactionId == 0x00)||
     (deviceInfReqPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId))||
       (ZllTouchlinkConfigData(mTouchLinkScanRspId) == 0x00))
  {
    /* discard the frame */
    return gZclMalformedCommand_c;
  }
  
  /* reply to sender */
  pDeviceInfRspCmd = AF_MsgAlloc();
  if(!pDeviceInfRspCmd)
    return gZclNoMem_c;
  
  /* get address ready for reply */
  PrepareInterPanForReply(&pDeviceInfRspCmd->addrInfo, pIndication);
  Copy2Bytes(pDeviceInfRspCmd->addrInfo.dstPanId, dstPanId);
  pDeviceInfRspCmd->addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  pDeviceInfRspCmd->zclTransactionId = pFrame->transactionId;
  
  /* complete the command payload */
  pDeviceInfRspCmd->cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);
#if gInstantiableStackEnabled_d 
  pDeviceInfRspCmd->cmdFrame.noOfSubDevices = EndPointConfigData(gNum_EndPoints);
#else
  pDeviceInfRspCmd->cmdFrame.noOfSubDevices = gNum_EndPoints_c;
#endif   
  
  pDeviceInfRspCmd->cmdFrame.startIndex = deviceInfReqPayload.startIndex;
  pDeviceInfRspCmd->cmdFrame.deviceInfRecordCount = 0x00;
  if(deviceInfReqPayload.startIndex < pDeviceInfRspCmd->cmdFrame.noOfSubDevices)
  {
    uint8_t i;
    /* get the max length for device information record */
    uint8_t maxDevInfRecordLen = (AF_MaxPayloadLen(NULL) - (sizeof(zclFrame_t) + sizeof(zllCmdCommissioning_DeviceInfRsp_t) - sizeof(zllCommissioning_DeviceInfRecord_t)));
    if(maxDevInfRecordLen > sizeof(zllCommissioning_DeviceInfRecord_t))
    {
      for(i = deviceInfReqPayload.startIndex; i<pDeviceInfRspCmd->cmdFrame.noOfSubDevices; i++)
      {
          pDeviceInfRspCmd->cmdFrame.deviceInfRecordCount++; 
          Copy8Bytes(pDeviceInfRspCmd->cmdFrame.deviceInfRecord[i-deviceInfReqPayload.startIndex].ieeeAddress, NlmeGetRequest(gNwkIeeeAddress_c));
          pDeviceInfRspCmd->cmdFrame.deviceInfRecord[i-deviceInfReqPayload.startIndex].endpointId = EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint);
          Copy2Bytes(pDeviceInfRspCmd->cmdFrame.deviceInfRecord[i-deviceInfReqPayload.startIndex].profileId, (EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->aAppProfId)));
          Copy2Bytes(pDeviceInfRspCmd->cmdFrame.deviceInfRecord[i-deviceInfReqPayload.startIndex].deviceId, (EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->aAppDeviceId)));
          pDeviceInfRspCmd->cmdFrame.deviceInfRecord[i-deviceInfReqPayload.startIndex].version = (EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->appDevVerAndFlag));
          pDeviceInfRspCmd->cmdFrame.deviceInfRecord[i-deviceInfReqPayload.startIndex].groupIdCount = 0x00;
          pDeviceInfRspCmd->cmdFrame.deviceInfRecord[i-deviceInfReqPayload.startIndex].sort = 0x00;
          maxDevInfRecordLen -= sizeof(zllCommissioning_DeviceInfRecord_t);
          if(maxDevInfRecordLen < sizeof(zllCommissioning_DeviceInfRecord_t))
            break;
      }
    }
  }
  /* send the device information response */
  status = zclZllTouchlink_DeviceInfRsp(pDeviceInfRspCmd);
  MSG_Free(pDeviceInfRspCmd);
 
  return  status;
}

/*!
 * @fn 		static zbStatus_t ZllTouchlink_IdentifyReqHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Identify Req Command received from Zll Initiator Device
 *
 */
static zbStatus_t ZllTouchlink_IdentifyReqHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCmdCommissioning_IdentifyReq_t identifyReqPayload;
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccess_c;
  uint16_t identifyDuration;
  zbClusterId_t clusterId = {gaZclClusterIdentify_c};
  uint8_t endpointIdentify = ZCL_GetEndPointForSpecificCluster(clusterId, TRUE, 0, NULL);
  
  if(endpointIdentify == gZclCluster_InvalidDataIndex_d)
    return status;

  pFrame = (void *)pIndication->pAsdu;
  identifyReqPayload = *(zllCmdCommissioning_IdentifyReq_t *)(pFrame+1);
  
  /* test the interpan transaction Id*/
  if((identifyReqPayload.interPanTransactionId == 0x00)||
     (identifyReqPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId))||
       (ZllTouchlinkConfigData(mTouchLinkScanRspId) == 0x00))
  {
    /* discard the frame */
    return status;
  }
  
  /* enter in the identify state */
  identifyDuration = (identifyReqPayload.identifyDuration == gZllCommissioning_KeepIdentifyMode_d)?ZllTouchlinkConfigData(mTouchLinkSession.identifyDuration):identifyReqPayload.identifyDuration;
  if(identifyDuration == gZllCommissioning_KeepIdentifyMode_d)
      identifyDuration = gZllCommissioningDefaultIdentifyDuration_d;
  ZCL_SetIdentifyMode(endpointIdentify, identifyDuration);
  return status;
}
/*!
 * @fn 		static zbStatus_t ZllTouchlink_ResetToFactoryNewReqHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Reset To Factory New Req Command received from Zll Initiator Device
 *
 */
static zbStatus_t ZllTouchlink_ResetToFactoryNewReqHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCmdCommissioning_ResetFactoryReq_t resetToFactoryNewReqPayload;
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccess_c;

  pFrame = (void *)pIndication->pAsdu;
  resetToFactoryNewReqPayload = *(zllCmdCommissioning_ResetFactoryReq_t *)(pFrame+1);
  
  /* test the interpan transaction Id*/
  if((resetToFactoryNewReqPayload.interPanTransactionId == 0x00)||
     (resetToFactoryNewReqPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId))||
       (ZllTouchlinkConfigData(mTouchLinkScanRspId) == 0x00) ||
        (ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus) == TRUE))
  {
    /* discard the frame */
    return status;
  }
  
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
  BeeApp_FactoryFresh();
  
  /* leave the network */
  ZDO_Leave();
 
  return status;
}

/*!
 * @fn 		static zbStatus_t ZllTouchlink_NetworkStartReqHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Network Start Req Command received from Zll Initiator Device
 *
 */
static zbStatus_t ZllTouchlink_NetworkStartReqHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCommissioning_NtwStartRsp_t ntwStartRspCmd;
  zllCmdCommissioning_NetworkStartReq_t ntwStartReqPayload;
  zbPanId_t dstPanId = {0xFF, 0xFF}; 
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccess_c;
  uint16_t keyBitmask = ZllTouchlink_GetKeyBitmask();
  
  pFrame = (void *)pIndication->pAsdu;
  ntwStartReqPayload = *(zllCmdCommissioning_NetworkStartReq_t *)(pFrame+1);
  
  /* verify the interpan transaction Id*/
  if((ntwStartReqPayload.interPanTransactionId == 0x00)||
     (ntwStartReqPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId))||
       (ZllTouchlinkConfigData(mTouchLinkScanRspId) == 0x00))
  {
    /* discard the frame */
    return gZclMalformedCommand_c;
  }

#if gInstantiableStackEnabled_d 
  if(EndPointConfigData(gNum_EndPoints) > 0x01)
#else
  if(gNum_EndPoints_c > 0x01)
#endif   
      if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_NetworkStart_d)
        return gZclSuccess_c;
  
  /* get address ready for reply */
  PrepareInterPanForReply(&ntwStartRspCmd.addrInfo, pIndication);
  Copy2Bytes(ntwStartRspCmd.addrInfo.dstPanId, dstPanId);
  ntwStartRspCmd.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  ntwStartRspCmd.zclTransactionId = pFrame->transactionId;
  
  /* verify the key index, target state and logical Channel */
  if((!(keyBitmask & (0x0001<<ntwStartReqPayload.keyIndex)))||
     (ZllTouchlinkConfigData(mTouchLinkSession.state) != gZllCommissioning_InitialState_d))
  {
    /* failure case: */
    ntwStartRspCmd.cmdFrame.status = gZllCommissioning_StatusFailed_d;
    ntwStartRspCmd.cmdFrame.interPanTransactionId = ntwStartReqPayload.interPanTransactionId;
    ntwStartRspCmd.cmdFrame.logicalChannel = ntwStartReqPayload.logicalChannel;
    ntwStartRspCmd.cmdFrame.nwkUpdateId = NlmeGetRequest(gNwkUpdateId_c);// 0x00; /* [R1] 7.1.2.3.3.4*/
    Copy8Bytes(ntwStartRspCmd.cmdFrame.extendedPanId, ntwStartReqPayload.extendedPanId);
    Copy2Bytes(ntwStartRspCmd.cmdFrame.panId, ntwStartReqPayload.panId);
    /* update target state */
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
    /* send the network start response */
    return zclZllTouchlink_NetworkStartRsp(&ntwStartRspCmd);
  }
  
  /* verify the PAN identifier, extended PAN identifier and logical channel. If the value of the field 
  is equal to zero, the target shall determine an appropriate value [R1] 8.4.3.2 */
  if(ntwStartReqPayload.logicalChannel == 0x00)
    ntwStartReqPayload.logicalChannel = ZllTouchlink_GetDefaultLogicalChannel();
  if(CmpToZero(ntwStartReqPayload.panId, sizeof(zbNwkAddr_t)))
  {
    /* generate random value */
    uint16_t tempPanID = (uint16_t)GetRandomNumber();
    Set2Bytes(ntwStartReqPayload.panId, tempPanID);
  }
  if(CmpToZero(ntwStartReqPayload.extendedPanId, sizeof(zbIeeeAddr_t)))
  {
    /* generate random value */
    uint8_t i;
    uint32_t tempData= GetRandomNumber();
    for(i = 0; i<2; ++i) 
    {
        FLib_MemCpy(ntwStartReqPayload.extendedPanId + (i<<1),&tempData, sizeof(uint32_t));
        tempData= GetRandomNumber();
    } 
  }
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningTarget_NetworkStart_d;
  /* store initiator informations: */
  ZllTouchlinkConfigData(mTouchLinkSession.keyIndex) = ntwStartReqPayload.keyIndex;
  ZllTouchlinkConfigData(mTouchLinkSession.ntwUpdateId) = 0x00;
  Copy8Bytes(ZllTouchlinkConfigData(mTouchLinkSession.extendedPanId), ntwStartReqPayload.extendedPanId);
  FLib_MemCpy(ZllTouchlinkConfigData(mTouchLinkSession.encryptedNtwKey), ntwStartReqPayload.encryptedNtwKey, sizeof(zbAESKey_t));
  ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel) = ntwStartReqPayload.logicalChannel;
  Copy2Bytes(ZllTouchlinkConfigData(mTouchLinkSession.panId), ntwStartReqPayload.panId);
  Copy2Bytes(ZllTouchlinkConfigData(mTouchLinkSession.ntwAddress), ntwStartReqPayload.ntwAddress);
  ZllTouchlinkConfigData(mTouchLinkSession.groupIdBegin) = ntwStartReqPayload.freeGroupIdRangeBegin;
  ZllTouchlinkConfigData(mTouchLinkSession.groupIdEnd) = ntwStartReqPayload.freeGroupIdRangeEnd; 
  Copy8Bytes(ZllTouchlinkConfigData(mTouchLinkSession.initiatorIeeeAddress), ntwStartReqPayload.initiatorIeeeAddress);
  Copy2Bytes(ZllTouchlinkConfigData(mTouchLinkSession.initiatorNtwAddress), ntwStartReqPayload.initiatorNtwAddress);
  FLib_MemCpy(&ZllTouchlinkConfigData(mTouchLinkSession.remoteDevAddrInfo), &ntwStartRspCmd.addrInfo, sizeof(InterPanAddrInfo_t));

  /* timeout = gZllAplcRxWindowDuration_d */
  ZllTouchlink_TargetSetTimeout(gZllAplcRxWindowDuration_d);

  /* verify that the PAN identifier and extended PAN identifier are unique => send NLME-NETWORK_DISCOVERY.request
  primitive to the NWK layer, over the primary ZLL channels,*/
  ASL_GenerateNwkDiscoveryReq(gZllCommissioningPrimaryChannelsMask_c, 0);
  
  return status;
}

/*!
 * @fn 		static zbStatus_t ZllTouchlink_NetworkJoinRouterReqHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Network Join Router Req Command received from Zll Initiator Device
 *
 */
static zbStatus_t ZllTouchlink_NetworkJoinRouterReqHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCommissioning_NtwJoinRouterRsp_t    ntwJoinRouterRspCmd;
  zllCmdCommissioning_NtwJoinRouterReq_t ntwJoinRouterReqPayload;
  zbPanId_t dstPanId = {0xFF, 0xFF}; 
  zclFrame_t *pFrame;
  uint16_t keyBitmask = ZllTouchlink_GetKeyBitmask();
  
  /* verify the device type */
  if(NlmeGetRequest(gDevType_c) != gRouter_c)
    return gZclSuccess_c;
  
#if gInstantiableStackEnabled_d 
  if(EndPointConfigData(gNum_EndPoints) > 0x01)
#else
  if(gNum_EndPoints_c > 0x01)
#endif 
    if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_JoinNetwork_d)
      return gZclSuccess_c;
 
  
  pFrame = (void *)pIndication->pAsdu;
  ntwJoinRouterReqPayload = *(zllCmdCommissioning_NtwJoinRouterReq_t *)(pFrame+1);
  
  /* verify the interpan transaction Id */
  if((ntwJoinRouterReqPayload.interPanTransactionId == 0x00)||
     (ntwJoinRouterReqPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId))||
       (ZllTouchlinkConfigData(mTouchLinkScanRspId) == 0x00))
  {
    /* discard the frame */
    return gZclMalformedCommand_c;
  }
  
  /* get address ready for reply */
  PrepareInterPanForReply(&ntwJoinRouterRspCmd.addrInfo, pIndication);
  Copy2Bytes(ntwJoinRouterRspCmd.addrInfo.dstPanId, dstPanId);
  ntwJoinRouterRspCmd.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  ntwJoinRouterRspCmd.zclTransactionId = pFrame->transactionId;
  
  ntwJoinRouterRspCmd.cmdFrame.status = gZllCommissioning_StatusSuccess_d;
  
  /* verify the key index, target state, logical Channel, panId and nwkAddr */
  if((!(keyBitmask & (0x0001<<ntwJoinRouterReqPayload.keyIndex)))||
     (ZllTouchlinkConfigData(mTouchLinkSession.state) != gZllCommissioning_InitialState_d) ||
       (!IsValidLogicalChannel(ntwJoinRouterReqPayload.logicalChannel))||
         (!IsValidExtendedPanId(ntwJoinRouterReqPayload.extendedPanId))||
           (!IsValidNwkAddr(ntwJoinRouterReqPayload.ntwAddress)))
  {
    /* failure case: */
    ntwJoinRouterRspCmd.cmdFrame.status = gZllCommissioning_StatusFailed_d;
    ntwJoinRouterRspCmd.cmdFrame.interPanTransactionId = ntwJoinRouterReqPayload.interPanTransactionId;
    /* update target state */
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
  }
  else
  {
    /* succes case: */
    ntwJoinRouterRspCmd.cmdFrame.status = gZllCommissioning_StatusSuccess_d;
    ntwJoinRouterRspCmd.cmdFrame.interPanTransactionId = ntwJoinRouterReqPayload.interPanTransactionId;
    /* update target state */
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningTarget_JoinNetwork_d;
    
  }
  
  /* store initiator informations: */
  ZllTouchlinkConfigData(mTouchLinkSession.keyIndex) = ntwJoinRouterReqPayload.keyIndex;
  ZllTouchlinkConfigData(mTouchLinkSession.ntwUpdateId) = ntwJoinRouterReqPayload.nwkUpdateId;
  Copy8Bytes(ZllTouchlinkConfigData(mTouchLinkSession.extendedPanId), ntwJoinRouterReqPayload.extendedPanId);
  FLib_MemCpy(ZllTouchlinkConfigData(mTouchLinkSession.encryptedNtwKey), ntwJoinRouterReqPayload.encryptedNtwKey, sizeof(zbAESKey_t));
  ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel) = ntwJoinRouterReqPayload.logicalChannel;
  Copy2Bytes(ZllTouchlinkConfigData(mTouchLinkSession.panId), ntwJoinRouterReqPayload.panId);
  Copy2Bytes(ZllTouchlinkConfigData(mTouchLinkSession.ntwAddress), ntwJoinRouterReqPayload.ntwAddress);
  ZllTouchlinkConfigData(mTouchLinkSession.groupIdBegin) = ntwJoinRouterReqPayload.freeGroupIdRangeBegin;
  ZllTouchlinkConfigData(mTouchLinkSession.groupIdEnd) = ntwJoinRouterReqPayload.freeGroupIdRangeEnd; 
  FLib_MemCpy(&ZllTouchlinkConfigData(mTouchLinkSession.remoteDevAddrInfo), &ntwJoinRouterRspCmd.addrInfo, sizeof(InterPanAddrInfo_t));
  
  /* timeout = gZllAplcRxWindowDuration_d */
  ZllTouchlink_TargetSetTimeout(gZllAplcRxWindowDuration_d);
  
  /* send the network start response */
  return zclZllTouchlink_NetworkJoinRouterRsp(&ntwJoinRouterRspCmd);
}

/*!
 * @fn 		static zbStatus_t ZllTouchlink_NetworkJoinEndDeviceReqHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Network Join EndDevice Req Command received from Zll Initiator Device
 *
 */
static zbStatus_t ZllTouchlink_NetworkJoinEndDeviceReqHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCommissioning_NtwJoinEndDeviceRsp_t    ntwJoinEndDeviceRspCmd;
  zllCmdCommissioning_NtwJoinEndDeviceReq_t ntwJoinEndDeviceReqPayload;
  zbPanId_t dstPanId = {0xFF, 0xFF}; 
  zclFrame_t *pFrame;
  uint16_t keyBitmask = ZllTouchlink_GetKeyBitmask();
  
  /* verify the device type */
  if(NlmeGetRequest(gDevType_c) != gEndDevice_c)
    return gZclSuccess_c;
  
#if gInstantiableStackEnabled_d 
  if(EndPointConfigData(gNum_EndPoints) > 0x01)
#else
  if(gNum_EndPoints_c > 0x01)
#endif  
    if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_JoinNetwork_d)
      return gZclSuccess_c;
 
  
  pFrame = (void *)pIndication->pAsdu;
  ntwJoinEndDeviceReqPayload = *(zllCmdCommissioning_NtwJoinEndDeviceReq_t *)(pFrame+1);
  
  /* verify the interpan transaction Id */
  if((ntwJoinEndDeviceReqPayload.interPanTransactionId == 0x00)||
     (ntwJoinEndDeviceReqPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId))||
       (ZllTouchlinkConfigData(mTouchLinkScanRspId) == 0x00))
  {
    /* discard the frame */
    return gZclMalformedCommand_c;
  }
  
  /* get address ready for reply */
  PrepareInterPanForReply(&ntwJoinEndDeviceRspCmd.addrInfo, pIndication);
  Copy2Bytes(ntwJoinEndDeviceRspCmd.addrInfo.dstPanId, dstPanId);
  ntwJoinEndDeviceRspCmd.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  ntwJoinEndDeviceRspCmd.zclTransactionId = pFrame->transactionId;
  
  ntwJoinEndDeviceRspCmd.cmdFrame.status = gZllCommissioning_StatusSuccess_d;
  
  /* verify the key index, target state, logical Channel, panId and nwkAddr */
  if((!(keyBitmask & (0x0001<<ntwJoinEndDeviceReqPayload.keyIndex)))||
     (ZllTouchlinkConfigData(mTouchLinkSession.state) != gZllCommissioning_InitialState_d) ||
       (!IsValidLogicalChannel(ntwJoinEndDeviceReqPayload.logicalChannel))||
         (!IsValidExtendedPanId(ntwJoinEndDeviceReqPayload.extendedPanId))||
           (!IsValidNwkAddr(ntwJoinEndDeviceReqPayload.ntwAddress)))
  {
    /* failure case: */
    ntwJoinEndDeviceRspCmd.cmdFrame.status = gZllCommissioning_StatusFailed_d;
    ntwJoinEndDeviceRspCmd.cmdFrame.interPanTransactionId = ntwJoinEndDeviceReqPayload.interPanTransactionId;
    /* update target state */
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
  }
  else
  {
    /* succes case: */
    ntwJoinEndDeviceRspCmd.cmdFrame.status = gZllCommissioning_StatusSuccess_d;
    ntwJoinEndDeviceRspCmd.cmdFrame.interPanTransactionId = ntwJoinEndDeviceReqPayload.interPanTransactionId;
    /* update target state */
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningTarget_JoinNetwork_d;
    
  }
  
  /* store initiator informations: */
  ZllTouchlinkConfigData(mTouchLinkSession.keyIndex) = ntwJoinEndDeviceReqPayload.keyIndex;
  ZllTouchlinkConfigData(mTouchLinkSession.ntwUpdateId) = ntwJoinEndDeviceReqPayload.nwkUpdateId;
  Copy8Bytes(ZllTouchlinkConfigData(mTouchLinkSession.extendedPanId), ntwJoinEndDeviceReqPayload.extendedPanId);
  FLib_MemCpy(ZllTouchlinkConfigData(mTouchLinkSession.encryptedNtwKey), ntwJoinEndDeviceReqPayload.encryptedNtwKey, sizeof(zbAESKey_t));
  ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel) = ntwJoinEndDeviceReqPayload.logicalChannel;
  Copy2Bytes(ZllTouchlinkConfigData(mTouchLinkSession.panId), ntwJoinEndDeviceReqPayload.panId);
  Copy2Bytes(ZllTouchlinkConfigData(mTouchLinkSession.ntwAddress), ntwJoinEndDeviceReqPayload.ntwAddress);
  ZllTouchlinkConfigData(mTouchLinkSession.groupIdBegin) = ntwJoinEndDeviceReqPayload.freeGroupIdRangeBegin;
  ZllTouchlinkConfigData(mTouchLinkSession.groupIdEnd) = ntwJoinEndDeviceReqPayload.freeGroupIdRangeEnd; 
  FLib_MemCpy(&ZllTouchlinkConfigData(mTouchLinkSession.remoteDevAddrInfo), &ntwJoinEndDeviceRspCmd.addrInfo, sizeof(InterPanAddrInfo_t));
  
  /* timeout = gZllAplcRxWindowDuration_d */
  ZllTouchlink_TargetSetTimeout(gZllAplcRxWindowDuration_d);
  
  /* send the network start response */
  return zclZllTouchlink_NetworkJoinEndDeviceRsp(&ntwJoinEndDeviceRspCmd);
}


/*!
 * @fn 		static zbStatus_t ZllTouchlink_NetworkUpdateReqHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Network Update Req Command received from Zll Initiator Device
 *
 */
static zbStatus_t ZllTouchlink_NetworkUpdateReqHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCmdCommissioning_NtwUpdateReq_t ntwUpdateReqPayload;
  zclFrame_t *pFrame;
  
  pFrame = (void *)pIndication->pAsdu;
  ntwUpdateReqPayload = *(zllCmdCommissioning_NtwUpdateReq_t *)(pFrame+1);
  
  /* verify the interpan transaction Id */
  if((ntwUpdateReqPayload.interPanTransactionId == 0x00)||
     (ntwUpdateReqPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId))||
       (ZllTouchlinkConfigData(mTouchLinkScanRspId) == 0x00))
  {
    /* discard the frame */
    return gZclSuccess_c;
  }
   
  /* verify logical Channel */
  if(!IsValidLogicalChannel(ntwUpdateReqPayload.logicalChannel))
  {
    /* discard the frame */
    return gZclSuccess_c;
  }
  
  /* verify extended panId, ntwAddr, network update identifier */
  if((!Cmp8Bytes(ntwUpdateReqPayload.extendedPanId, NlmeGetRequest(gNwkExtendedPanId_c))) ||
     (!FLib_Cmp2Bytes(ntwUpdateReqPayload.ntwAddress, NlmeGetRequest(gNwkShortAddress_c)))||
       (ntwUpdateReqPayload.nwkUpdateId <= NlmeGetRequest(gNwkUpdateId_c)))
  {
    /* discard the frame */
    return gZclSuccess_c;    
  }
   
  /* update device */
  NlmeSetRequest(gNwkUpdateId_c, &ntwUpdateReqPayload.nwkUpdateId);
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
    NWK_FRAMEHANDLE_SetBeaconPayloadRAM();  
#endif  
  
  ASL_ChangeChannel(ntwUpdateReqPayload.logicalChannel);
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
  
  return gZclSuccess_c;
}


/********************************************************************************************
 *                   Touchlink target protocol functions                                     *
 ********************************************************************************************/
/*!
 * @fn 		static void ZllTouchlink_TargetSetTimeout(uint32_t duration  ) 
 *
 * @brief	set target timeout
 *
 */
static void ZllTouchlink_TargetSetTimeout(uint32_t duration) /* miliseconds */
{
  if(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) == gTmrInvalidTimerID_c)
  {
    ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) = ZbTMR_AllocateTimer(); 
    if(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) == gTmrInvalidTimerID_c)
      return;
  }
  ZbTMR_StartTimer(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr), gTmrSingleShotTimer_c|gTmrLowPowerTimer_c, duration, ZllTouchlink_TargetTimeoutCallBack);
}

/*!
 * @fn 		static void ZllTouchlink_TargetTimeoutCallBack(uint8_t tmrId)
 *
 * @brief	Zll device discovery callback
 *
 */
static void ZllTouchlink_TargetTimeoutCallBack(uint8_t tmrId)
{
 
  ZbTMR_FreeTimer(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr));
  ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) = gTmrInvalidTimerID_c;
  
  if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioning_InitialState_d)
  {
    ZllTouchlinkConfigData(mTouchLinkScanReqId) = 0x00;
    ZllTouchlinkConfigData(mTouchLinkScanRspId) = 0x00;
  }
  if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_NetworkStart_d)
  {
    /* failed, send the response to the initiator */
    zllCommissioning_NtwStartRsp_t ntwStartRspCmd;
    /* set the target state as initial state */
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
    
    /* send the response back to the sender */     
    FLib_MemCpy(&ntwStartRspCmd.addrInfo, &ZllTouchlinkConfigData(mTouchLinkSession.remoteDevAddrInfo), sizeof(InterPanAddrInfo_t));
    ntwStartRspCmd.cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);
    ntwStartRspCmd.cmdFrame.logicalChannel = ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel);
    ntwStartRspCmd.cmdFrame.nwkUpdateId = NlmeGetRequest(gNwkUpdateId_c);//0x00; /* [R1] 7.1.2.3.3.4*/
    Copy8Bytes(ntwStartRspCmd.cmdFrame.extendedPanId, ZllTouchlinkConfigData(mTouchLinkSession.extendedPanId));
    Copy2Bytes(ntwStartRspCmd.cmdFrame.panId, ZllTouchlinkConfigData(mTouchLinkSession.panId));
    ntwStartRspCmd.cmdFrame.status = gZllCommissioning_StatusFailed_d;
    
    /* send the network start response */
    (void)zclZllTouchlink_NetworkStartRsp(&ntwStartRspCmd);
  }
  
  if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_JoinNetwork_d)
  {
    if(!ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
    {
        /* leave the network if not factory new */
        ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus) = TRUE;
        ZDO_Leave();
        return;
    }
    /* ZLL ntw start procedure */
    ZllTouchlink_TargetNtwStart();
  }
  
  if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_FormNetwork_d)
  {
     /* ZLL ntw start procedure */
     ZllTouchlink_TargetNtwStart();
  }
  
}



/*!
 * @fn 		zbStatus_t ZCL_ZllTouchlinkClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the InterPAN Zll Commisisoning (Touchlink)Cluster Client. 
 *
 */
zbStatus_t ZCL_ZllTouchlinkClusterClient
(
   zbInterPanDataIndication_t *pIndication, /* IN: */
   afDeviceDef_t *pDevice                   /* IN: */
) 
{
#if gASL_EnableZllCommissioning_Initiator_d     
   zclFrame_t *pFrame;
   zbStatus_t status = gZclSuccessDefaultRsp_c;
   zbProfileId_t   zllProfileId = {gZllProfileIdentifier_d};
     
   /* to prevent compiler warning */
   (void)pDevice;
  
   /* check profile Id */
   if(!FLib_MemCmp(pIndication->aProfileId, zllProfileId, sizeof(zbProfileId_t)))
     return gZclMalformedCommand_c;
   
   /* get frame information */
   pFrame = (void *)pIndication->pAsdu;
   
   if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
      status = gZclSuccess_c;

   /* handle incoming server commands */
   switch (pFrame->command)
   {
      case gZclCmdZllCommissioning_ScanRsp_c:
        status = ZllTouchlink_ScanRspHandler(pIndication);
        break;         
      case gZclCmdZllCommissioning_DeviceInformationRsp_c:
        status = ZllTouchlink_DeviceInformationRsp(pIndication);
        break;
      case gZclCmdZllCommissioning_NetworkStartRsp_c:
        status = ZllTouchlink_NtwStartRspHandler(pIndication);
        break;         
      case gZclCmdZllCommissioning_NetworkJoinRouterRsp_c:   
        status = ZllTouchlink_NtwJoinRouterRspHandler(pIndication);
        break;          
      case gZclCmdZllCommissioning_NetworkJoinEndDeviceRsp_c:       
        status = ZllTouchlink_NtwJoinEndDeviceRspHandler(pIndication);
        break; 
      default:
          status = gZclUnsupportedClusterCommand_c;
          break;
   }
   return status;
#else
   return gZclUnsupportedClusterCommand_c;
#endif   
}


/********************************************************************************************
 *                   Touchlink initiator commands:                                          *
 ********************************************************************************************/

/*!
 * @fn 		zbStatus_t zclZllTouchlink_ScanReq(zllCommissioning_ScanRequest_t *pReq)
 *
 * @brief	Sends over-the-air a Scan Request command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_ScanReq
(
  zllCommissioning_ScanRequest_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_ScanRequest_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdZllCommissioning_ScanReq_c, payloadLen, pReq);
}

/*!
 * @fn 		zbStatus_t zclZllTouchlink_DeviceInfReq(zllCommissioning_DeviceInformationReq_t *pReq)
 *
 * @brief	Sends over-the-air a Device Information request command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_DeviceInfReq
(
  zllCommissioning_DeviceInformationReq_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_DeviceInformationReq_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdZllCommissioning_DeviceInformationReq_c, payloadLen, pReq);
}

/*!
 * @fn 		zbStatus_t zclZllTouchlink_IdentifyReq(zllCommissioning_DeviceInformationReq_t *pReq)
 *
 * @brief	Sends over-the-air a Identify request command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_IdentifyReq
(
  zllCommissioning_IdentifyReq_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_IdentifyReq_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdZllCommissioning_IdentifyReq_c, payloadLen, pReq);
}

/*!
 * @fn 		zbStatus_t zclZllTouchlink_ResetFactoryReq(zllCommissioning_ResetFactoryReq_t *pReq)
 *
 * @brief	Sends over-the-air a ResetFactoryNewRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_ResetFactoryReq
(
  zllCommissioning_ResetFactoryReq_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_ResetFactoryReq_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdZllCommissioning_ResetToFactoryNewReq_c, payloadLen, pReq);
}

/*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkStartReq(zllCommissioning_NetworkStartReq_t *pReq)
 *
 * @brief	Sends over-the-air a Network start Request from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkStartReq
(
  zllCommissioning_NetworkStartReq_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_NetworkStartReq_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdZllCommissioning_NetworkStartReq_c, payloadLen, pReq);
}  
 
/*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkJoinRouterReq(zllCommissioning_NtwJoinRouterReq_t *pReq)
 *
 * @brief	Sends over-the-air a Network join router Request from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkJoinRouterReq
(
  zllCommissioning_NtwJoinRouterReq_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_NtwJoinRouterReq_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdZllCommissioning_NetworkJoinRouterReq_c, payloadLen, pReq);
} 

/*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkJoinEndDeviceReq(zllCommissioning_NtwJoinEndDeviceReq_t *pReq)
 *
 * @brief	Sends over-the-air a Network join router Request from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkJoinEndDeviceReq
(
  zllCommissioning_NtwJoinEndDeviceReq_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_NtwJoinEndDeviceReq_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdZllCommissioning_NetworkJoinEndDeviceReq_c, payloadLen, pReq);
}  

/*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkUpdateReq(zllCommissioning_NtwUpdateReq_t *pReq)
 *
 * @brief	Sends over-the-air a Network join router Request from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkUpdateReq
(
  zllCommissioning_NtwUpdateReq_t *pReq
)
{
  uint8_t payloadLen = sizeof(zllCmdCommissioning_NtwUpdateReq_t);	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdZllCommissioning_NetworkUpdateReq_c, payloadLen, pReq);
}  

/************************************************************************************************/
/*                 Helper Functions to send ZLL Commissioning commands:                         */ 
/************************************************************************************************/
#if gASL_EnableZllCommissioning_Initiator_d
/*!
 * @fn 		zbStatus_t ZllTouchlink_SendScanReq(void)
 *
 * @brief	Helper functions: Sends over-the-air a ScanRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendScanReq(void)
{
  zbPanId_t     dstPanId = {0xFF, 0xFF};
  zbNwkAddr_t   dstAddr = {0xFF, 0xFF}; /* broadcast */
  zllCommissioning_ScanRequest_t scanReq;
  zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
  
  /* set destination informations: */
  scanReq.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  scanReq.addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
  Set2Bytes(scanReq.addrInfo.aClusterId, gZclClusterZllCommissioning_c);	
  Copy2Bytes(scanReq.addrInfo.aProfileId, zllProfileId);
  Copy2Bytes(scanReq.addrInfo.dstPanId, dstPanId);
  Copy2Bytes(scanReq.addrInfo.dstAddr.aNwkAddr, dstAddr);
  
  /* complete scan request command */
  scanReq.cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);  
  scanReq.cmdFrame.zbInf.logicalType = NlmeGetRequest(gDevType_c); 
  scanReq.cmdFrame.zbInf.reserved = 0x00;
  scanReq.cmdFrame.zbInf.rxOnIdle = 0x01;
  if(scanReq.cmdFrame.zbInf.logicalType == gEndDevice_c)
    scanReq.cmdFrame.zbInf.rxOnIdle = 0x00;

  
#if gASL_EnableZllCommissioning_Initiator_d
  scanReq.cmdFrame.zllInf.addrAssignment = 0x01;
  scanReq.cmdFrame.zllInf.initiator = 0x01;
#else
  scanReq.cmdFrame.zllInf.addrAssignment = 0x00;
  scanReq.cmdFrame.zllInf.initiator = 0x00;  
#endif  
  scanReq.cmdFrame.zllInf.factoryNew = ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus);   
  scanReq.cmdFrame.zllInf.reserved1 = 0x00;
  scanReq.cmdFrame.zllInf.reserved2 = 0x00;
  
  return zclZllTouchlink_ScanReq(&scanReq);
}
/*!
 * @fn 		zbStatus_t ZllTouchlink_SendDeviceInfReq(uint8_t dstAddrMode, zbApsAddr_t dstAddr, zbPanId_t dstPanId, uint8_t startIndex)
 *
 * @brief	Helper functions: Sends over-the-air a SendDeviceInfReq command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendDeviceInfReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId, uint8_t startIndex)
{
  zllCommissioning_DeviceInformationReq_t deviceInfReq;
  zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
  zbStatus_t status;
  
  /* set destination informations: */
  deviceInfReq.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  deviceInfReq.addrInfo.dstAddrMode = gZbAddrMode64Bit_c;
  Set2Bytes(deviceInfReq.addrInfo.aClusterId, gZclClusterZllCommissioning_c);	
  Copy2Bytes(deviceInfReq.addrInfo.aProfileId, zllProfileId);
  Copy2Bytes(deviceInfReq.addrInfo.dstPanId, dstPanId);
  Copy8Bytes(deviceInfReq.addrInfo.dstAddr.aIeeeAddr, dstAddr);
  
  /* complete device Inf request command */
  deviceInfReq.cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);
  deviceInfReq.cmdFrame.startIndex = startIndex;
   
  status = zclZllTouchlink_DeviceInfReq(&deviceInfReq);
  
  if(status == gZclSuccess_c)
  {
    /* set initiator next state */
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningInitiator_DeviceInfReq_d;
    /* set timeout */
    ZllTouchlink_InitiatorSetTimeout(gZllAplcScanTimeBaseDuration_d);
  }
  
  return status;
  
}
/*!
 * @fn 		zbStatus_t ZllTouchlink_SendIdentifyReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId, uint16_t identifyDuration)
 *
 * @brief	Helper functions: Sends over-the-air a SendIdentifyReq command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendIdentifyReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId, uint16_t identifyDuration)
{
  zllCommissioning_IdentifyReq_t identifyReq;
  zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
  zbStatus_t status;
  
  /* set destination informations: */
  identifyReq.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  identifyReq.addrInfo.dstAddrMode = gZbAddrMode64Bit_c;
  Set2Bytes(identifyReq.addrInfo.aClusterId, gZclClusterZllCommissioning_c);	
  Copy2Bytes(identifyReq.addrInfo.aProfileId, zllProfileId);
  Copy2Bytes(identifyReq.addrInfo.dstPanId, dstPanId);
  Copy8Bytes(identifyReq.addrInfo.dstAddr.aIeeeAddr, dstAddr);
  
  /* complete reset to factory new request command */
  identifyReq.cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);
  identifyReq.cmdFrame.identifyDuration = identifyDuration;
  status = zclZllTouchlink_IdentifyReq(&identifyReq);
  
  if(status == gZclSuccess_c)
  {
    /* set initiator next state */
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningInitiator_IdentifyReq_d;
    /* set timeout */
    ZllTouchlink_InitiatorSetTimeout(gZllAplcScanTimeBaseDuration_d);
  }
  
  return status;
}
/*!
 * @fn 		zbStatus_t ZllTouchlink_SendResetFactoryNewReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId)
 *
 * @brief	Helper functions: Sends over-the-air a ResetToFacory command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendResetFactoryNewReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId)
{
  zllCommissioning_ResetFactoryReq_t resetFactoryReq;
  zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
  
  /* set destination informations: */
  resetFactoryReq.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  resetFactoryReq.addrInfo.dstAddrMode = gZbAddrMode64Bit_c;
  Set2Bytes(resetFactoryReq.addrInfo.aClusterId, gZclClusterZllCommissioning_c);	
  Copy2Bytes(resetFactoryReq.addrInfo.aProfileId, zllProfileId);
  Copy2Bytes(resetFactoryReq.addrInfo.dstPanId, dstPanId);
  Copy8Bytes(resetFactoryReq.addrInfo.dstAddr.aIeeeAddr, dstAddr);
  
  /* complete reset to factory new request command */
  resetFactoryReq.cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);

  return zclZllTouchlink_ResetFactoryReq(&resetFactoryReq);
}
/*!
 * @fn 		zbStatus_t ZllTouchlink_SendNetworkStartReq(zllCommissioning_TouchlinkRemoteTarget_t remoteTarget)
 *
 * @brief	Helper functions: Sends over-the-air a Network Start Request command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendNetworkStartReq(zllCommissioning_TouchlinkRemoteTarget_t remoteTarget)
{
  zllCommissioning_NetworkStartReq_t ntwStartReq;
  zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
  zbAESKey_t zllkey;
  zbStatus_t status;
  uint16_t ntwAddress = (uint16_t)GetRandomNumber(); 
  uint16_t addrBegin, addrEnd, groupBegin, groupEnd;
  
  /* set destination informations: */
  ntwStartReq.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  ntwStartReq.addrInfo.dstAddrMode = remoteTarget.dstAddrMode;
  Set2Bytes(ntwStartReq.addrInfo.aClusterId, gZclClusterZllCommissioning_c);	
  Copy2Bytes(ntwStartReq.addrInfo.aProfileId, zllProfileId);
  Copy2Bytes(ntwStartReq.addrInfo.dstPanId, remoteTarget.destPanId);
  FLib_MemCpy(&ntwStartReq.addrInfo.dstAddr, &remoteTarget.dstAddr, sizeof(zbApsAddr_t));
  
  /* set initiator next state */
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningInitiator_NetworkStart_d;
  
  /* get zll key*/
  ZllCommissioningSecurity_GetZllKey(remoteTarget.keyIndex, zllkey);
  
  /* complete network start request command */
  ntwStartReq.cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);
  Copy8Bytes(ntwStartReq.cmdFrame.extendedPanId, NlmeGetRequest(gNwkExtendedPanId_c));
  ntwStartReq.cmdFrame.keyIndex = remoteTarget.keyIndex;
  ZllCommissioningSecurity_EncryptNetworkKey(ZllTouchlinkConfigData(mTouchLinkScanReqId), ZllTouchlinkConfigData(mTouchLinkScanRspId), zllkey, &ntwStartReq.cmdFrame.encryptedNtwKey[0]);
  /* keep a copy for the network key */
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.keyIndexSent) = remoteTarget.keyIndex;
  Copy16Bytes(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.tempEncrNtwKey), ntwStartReq.cmdFrame.encryptedNtwKey);
  
  ntwStartReq.cmdFrame.logicalChannel = ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel);
  if(ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
    ntwStartReq.cmdFrame.logicalChannel = 0x00;
  
  Copy2Bytes(ntwStartReq.cmdFrame.panId, NlmeGetRequest(gNwkPanId_c));  
  /* get an available short Address */
  if((ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) < ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd))&&
     (ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) > gZllAplFreeNwkAddRangeBegin_d))
  {
     ntwAddress = ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin);
     /* update ntw address range begin */
     ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin)++;
  }
  Set2Bytes(ntwStartReq.cmdFrame.ntwAddress, ntwAddress);
   
  /* get group and ntwAddr ranges */
  ZllTouchlink_GetAvailableGroupIds(&groupBegin, &groupEnd, remoteTarget.groupIdCount);
  ntwStartReq.cmdFrame.groupIdBegin = groupBegin;
  ntwStartReq.cmdFrame.groupIdEnd = groupEnd;
  ZllTouchlink_GetAvailableFreeRanges(&groupBegin, &groupEnd, &addrBegin, &addrEnd, remoteTarget.zllInf.addrAssignment);
  Set2Bytes(ntwStartReq.cmdFrame.freeNtwAddrRangeBegin, addrBegin);
  Set2Bytes(ntwStartReq.cmdFrame.freeNtwAddrRangeEnd, addrEnd);
  ntwStartReq.cmdFrame.freeGroupIdRangeBegin =  groupBegin;
  ntwStartReq.cmdFrame.freeGroupIdRangeEnd =  groupEnd;
   
  Copy8Bytes(ntwStartReq.cmdFrame.initiatorIeeeAddress, NlmeGetRequest(gNwkIeeeAddress_c));
  Copy2Bytes(ntwStartReq.cmdFrame.initiatorNtwAddress, NlmeGetRequest(gNwkShortAddress_c));
  /* send network start request */
  status = zclZllTouchlink_NetworkStartReq(&ntwStartReq);
  if(status == gZclSuccess_c)
  {
    ZllTouchlink_UpdateNtwkAddrRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr, ntwStartReq.cmdFrame.ntwAddress);
  }
  /* set timeout */
  ZllTouchlink_InitiatorSetTimeout(gZllAplcRxWindowDuration_d);
  return status;
}

/*!
 * @fn 		zbStatus_t ZllTouchlink_SendNetworkJoinRouterReq(zllCommissioning_TouchlinkRemoteTarget_t remoteTarget)
 *
 * @brief	Helper functions: Sends over-the-air a Network Join Router Request command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendNetworkJoinRouterReq(zllCommissioning_TouchlinkRemoteTarget_t remoteTarget)
{
  zllCommissioning_NtwJoinRouterReq_t ntwJoinRouterReq;
  zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
  uint16_t ntwAddress = (uint16_t)GetRandomNumber(); 
  zbAESKey_t zllkey;
  zbStatus_t status;
  uint16_t addrBegin, addrEnd, groupBegin, groupEnd;
  
  /* set destination informations: */
  ntwJoinRouterReq.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  ntwJoinRouterReq.addrInfo.dstAddrMode = remoteTarget.dstAddrMode;
  Set2Bytes(ntwJoinRouterReq.addrInfo.aClusterId, gZclClusterZllCommissioning_c);	
  Copy2Bytes(ntwJoinRouterReq.addrInfo.aProfileId, zllProfileId);
  Copy2Bytes(ntwJoinRouterReq.addrInfo.dstPanId, remoteTarget.destPanId);
  FLib_MemCpy(&ntwJoinRouterReq.addrInfo.dstAddr, &remoteTarget.dstAddr, sizeof(zbApsAddr_t));
  
  /* set initiator next state */
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningInitiator_NetworkJoinReq_d;
  
  /* get zll key*/
  ZllCommissioningSecurity_GetZllKey(remoteTarget.keyIndex, zllkey);
    
  /* complete network start request command */
  ntwJoinRouterReq.cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);
  Copy8Bytes(ntwJoinRouterReq.cmdFrame.extendedPanId, ApsmeGetRequest(gApsUseExtendedPANID_c));

  ntwJoinRouterReq.cmdFrame.keyIndex = remoteTarget.keyIndex;
  ZllCommissioningSecurity_EncryptNetworkKey(ZllTouchlinkConfigData(mTouchLinkScanReqId), ZllTouchlinkConfigData(mTouchLinkScanRspId), zllkey, &ntwJoinRouterReq.cmdFrame.encryptedNtwKey[0]);
  ntwJoinRouterReq.cmdFrame.logicalChannel = ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel);
  Copy2Bytes(ntwJoinRouterReq.cmdFrame.panId, NlmeGetRequest(gNwkPanId_c));  
  ntwJoinRouterReq.cmdFrame.nwkUpdateId = NlmeGetRequest(gNwkUpdateId_c);
  /* get an available short Address */
  if((ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) < ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd))&&
     (ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) > gZllAplFreeNwkAddRangeBegin_d))
  {
     ntwAddress = ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin);
     /* update ntw address range begin */
     ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin)++;
  }
  Set2Bytes(ntwJoinRouterReq.cmdFrame.ntwAddress, ntwAddress);
  /* get group and ntwAddr ranges */
  ZllTouchlink_GetAvailableGroupIds(&groupBegin, &groupEnd, remoteTarget.groupIdCount);
  ntwJoinRouterReq.cmdFrame.groupIdBegin = groupBegin;
  ntwJoinRouterReq.cmdFrame.groupIdEnd = groupEnd;
  ZllTouchlink_GetAvailableFreeRanges(&groupBegin, &groupEnd, &addrBegin, &addrEnd, remoteTarget.zllInf.addrAssignment);
  Set2Bytes(ntwJoinRouterReq.cmdFrame.freeNtwAddrRangeBegin, addrBegin);
  Set2Bytes(ntwJoinRouterReq.cmdFrame.freeNtwAddrRangeEnd, addrEnd);
  ntwJoinRouterReq.cmdFrame.freeGroupIdRangeBegin =  groupBegin;
  ntwJoinRouterReq.cmdFrame.freeGroupIdRangeEnd =  groupEnd;
  /* send network join router request */
  status = zclZllTouchlink_NetworkJoinRouterReq(&ntwJoinRouterReq);
  if(status == gZclSuccess_c)
  {
    ZllTouchlink_UpdateNtwkAddrRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr, ntwJoinRouterReq.cmdFrame.ntwAddress);
  }
  /* set timeout */
  ZllTouchlink_InitiatorSetTimeout(gZllAplcRxWindowDuration_d);
  return status;
}

/*!
 * @fn 		zbStatus_t ZllTouchlink_SendNetworkJoinEndDeviceReq(zllCommissioning_TouchlinkRemoteTarget_t remoteTarget)
 *
 * @brief	Helper functions: Sends over-the-air a Network Join End Device Request command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendNetworkJoinEndDeviceReq(zllCommissioning_TouchlinkRemoteTarget_t remoteTarget)
{
  zllCommissioning_NtwJoinEndDeviceReq_t ntwJoinEndDeviceReq;
  zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
  uint16_t ntwAddress = (uint16_t)GetRandomNumber(); 
  zbAESKey_t zllkey;
  zbStatus_t status;
  uint16_t addrBegin, addrEnd, groupBegin, groupEnd;
  
  /* set destination informations: */
  ntwJoinEndDeviceReq.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  ntwJoinEndDeviceReq.addrInfo.dstAddrMode = remoteTarget.dstAddrMode;
  Set2Bytes(ntwJoinEndDeviceReq.addrInfo.aClusterId, gZclClusterZllCommissioning_c);	
  Copy2Bytes(ntwJoinEndDeviceReq.addrInfo.aProfileId, zllProfileId);
  Copy2Bytes(ntwJoinEndDeviceReq.addrInfo.dstPanId, remoteTarget.destPanId);
  FLib_MemCpy(&ntwJoinEndDeviceReq.addrInfo.dstAddr, &remoteTarget.dstAddr, sizeof(zbApsAddr_t));
  
  /* set initiator next state */
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningInitiator_NetworkJoinReq_d;
  
  /* get zll key*/
  ZllCommissioningSecurity_GetZllKey(remoteTarget.keyIndex, zllkey);
  
  /* complete network start request command */
  ntwJoinEndDeviceReq.cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);
  Copy8Bytes(ntwJoinEndDeviceReq.cmdFrame.extendedPanId, ApsmeGetRequest(gApsUseExtendedPANID_c));
  ntwJoinEndDeviceReq.cmdFrame.keyIndex = remoteTarget.keyIndex;
  ZllCommissioningSecurity_EncryptNetworkKey(ZllTouchlinkConfigData(mTouchLinkScanReqId), ZllTouchlinkConfigData(mTouchLinkScanRspId), zllkey, &ntwJoinEndDeviceReq.cmdFrame.encryptedNtwKey[0]);
  ntwJoinEndDeviceReq.cmdFrame.logicalChannel = ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel);
  Copy2Bytes(ntwJoinEndDeviceReq.cmdFrame.panId, NlmeGetRequest(gNwkPanId_c));  
  
  /* get an available short Address */
  if((ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) < ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd))&&
     (ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) > gZllAplFreeNwkAddRangeBegin_d))
  {
     ntwAddress = ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin);
     /* update ntw address range begin */
     ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin)++;
  }
  Set2Bytes(ntwJoinEndDeviceReq.cmdFrame.ntwAddress, ntwAddress);
  /* get group and ntwAddr ranges */
  ZllTouchlink_GetAvailableGroupIds(&groupBegin, &groupEnd, remoteTarget.groupIdCount);
  ntwJoinEndDeviceReq.cmdFrame.groupIdBegin = groupBegin;
  ntwJoinEndDeviceReq.cmdFrame.groupIdEnd = groupEnd;
  ZllTouchlink_GetAvailableFreeRanges(&groupBegin, &groupEnd, &addrBegin, &addrEnd, remoteTarget.zllInf.addrAssignment);
  Set2Bytes(ntwJoinEndDeviceReq.cmdFrame.freeNtwAddrRangeBegin, addrBegin);
  Set2Bytes(ntwJoinEndDeviceReq.cmdFrame.freeNtwAddrRangeEnd, addrEnd);
  ntwJoinEndDeviceReq.cmdFrame.freeGroupIdRangeBegin =  groupBegin;
  ntwJoinEndDeviceReq.cmdFrame.freeGroupIdRangeEnd =  groupEnd;
  ntwJoinEndDeviceReq.cmdFrame.nwkUpdateId = NlmeGetRequest(gNwkUpdateId_c);
  
  /* send network join end device request */
  status = zclZllTouchlink_NetworkJoinEndDeviceReq(&ntwJoinEndDeviceReq);
  if(status == gZclSuccess_c)
  {
    ZllTouchlink_UpdateNtwkAddrRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr, ntwJoinEndDeviceReq.cmdFrame.ntwAddress);
  }
  /* set timeout */
  ZllTouchlink_InitiatorSetTimeout(gZllAplcRxWindowDuration_d);
  return status;
}

/*!
 * @fn 		zbStatus_t ZllTouchlink_SendNetworkUpdateReq(zllCommissioning_TouchlinkRemoteTarget_t remoteTarget)
 *
 * @brief	Helper functions: Sends over-the-air a Network Update Request command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendNetworkUpdateReq(zllCommissioning_TouchlinkRemoteTarget_t remoteTarget)
{
  zllCommissioning_NtwUpdateReq_t ntwUpdateReq;
  zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
  zbStatus_t status; 
  
  /* set destination informations: */
  ntwUpdateReq.addrInfo.srcAddrMode = gZbAddrMode64Bit_c;
  ntwUpdateReq.addrInfo.dstAddrMode = remoteTarget.dstAddrMode;
  Set2Bytes(ntwUpdateReq.addrInfo.aClusterId, gZclClusterZllCommissioning_c);	
  Copy2Bytes(ntwUpdateReq.addrInfo.aProfileId, zllProfileId);
  Copy2Bytes(ntwUpdateReq.addrInfo.dstPanId, remoteTarget.destPanId);
  FLib_MemCpy(&ntwUpdateReq.addrInfo.dstAddr, &remoteTarget.dstAddr, sizeof(zbApsAddr_t));
  
  /* set initiator next state */
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningInitiator_NetworkUpdateReq_d;
   
  
  /* complete network update request command */
  ntwUpdateReq.cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);
  Copy8Bytes(ntwUpdateReq.cmdFrame.extendedPanId, NlmeGetRequest(gNwkExtendedPanId_c));
  ntwUpdateReq.cmdFrame.nwkUpdateId = NlmeGetRequest(gNwkUpdateId_c);
  ntwUpdateReq.cmdFrame.logicalChannel = ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel);
  Copy2Bytes(ntwUpdateReq.cmdFrame.panId, NlmeGetRequest(gNwkPanId_c));  
  Copy2Bytes(ntwUpdateReq.cmdFrame.ntwAddress, remoteTarget.ntwAddress); 
  
  /* send network update request */
  status = zclZllTouchlink_NetworkUpdateReq(&ntwUpdateReq);
  
  /* set timeout */
  ZllTouchlink_InitiatorSetTimeout(gZllAplcScanTimeBaseDuration_d);
  return status;
}

#endif /* gASL_EnableZllCommissioning_Initiator_d */
/*!
 * @fn 		static void ZllCommissioningSecurity_GetZllKey(uint8_t keyIndex, uint8_t *outputZllKey)
 *
 * @brief	Get the zll key
 *
 */
static void ZllCommissioningSecurity_GetZllKey(uint8_t keyIndex, uint8_t *outputZllKey)
{
  switch(keyIndex)
  {
    case gZllCommissioning_DevelopmentKeyIndex_d:
      {
        zbAESKey_t  zllKey = {gZllCommissioning_DevelopmentKey_d};
        Copy16Bytes(&outputZllKey[0], zllKey);
        FLib_MemCpy(&outputZllKey[4], &ZllTouchlinkConfigData(mTouchLinkScanReqId), sizeof(uint32_t));
        FLib_MemCpy(&outputZllKey[12], &ZllTouchlinkConfigData(mTouchLinkScanRspId), sizeof(uint32_t));
        break;
      }
    case gZllCommissioning_MasterKeyIndex_d:
      {
        zbAESKey_t  zllKey = {gZllCommissioning_MasterKey_d};
        Copy16Bytes(&outputZllKey[0], zllKey);
        break;
      }
    case gZllCommissioning_CertificationKeyIndex_d:
      {
        zbAESKey_t  zllKey = {gZllCommissioning_CertificationKey_d};
        Copy16Bytes(&outputZllKey[0], zllKey);
        break;
      }
    default:
      {
        zbAESKey_t  zllKey = {gZllCommissioning_InvalidKey_d};
        Copy16Bytes(&outputZllKey[0], zllKey);
        break;
      }
  } 
}
#if gASL_EnableZllCommissioning_Initiator_d
/*!
 * @fn 		static void ZllCommissioningSecurity_EncryptNetworkKey(uint32_t transactionIdReq, uint32_t transactionIdRsp, zbAESKey_t inZllkey, zbAESKey_t outputKey)
 *
 * @brief	Encrypt the network key algorithm (ZLl commissioning)
 *
 */
static void ZllCommissioningSecurity_EncryptNetworkKey(uint32_t transactionIdReq, uint32_t transactionIdRsp, zbAESKey_t inZllkey, zbAESKey_t outputKey)
{
  zbAESKey_t expandData, transportKey, nwkKey;
  uint32_t tempData= GetRandomNumber();
  
  transactionIdReq = Swap4Bytes(transactionIdReq);
  transactionIdRsp = Swap4Bytes(transactionIdRsp);
  /* step1: merge and expand the transaction identifier and response identifier */
  FLib_MemCpy(&expandData[0], &transactionIdReq, sizeof(uint32_t));
  FLib_MemCpy(&expandData[4], &transactionIdReq, sizeof(uint32_t));
  FLib_MemCpy(&expandData[8], &transactionIdRsp, sizeof(uint32_t));
  FLib_MemCpy(&expandData[12],&transactionIdRsp, sizeof(uint32_t));
  
  /* step 2: derive the ephemeral transport key: using expandKey and inKey(ZLL master or certification key) */
  AES_128_Encrypt(expandData, inZllkey, transportKey);
  
  if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningInitiator_NetworkJoinReq_d)
  {
    /* get network key from memory */
    zbNwkSecurityMaterialSet_t  *pSecurityMaterial;
    pSecurityMaterial = SSP_NwkGetSecurityMaterialSet(NlmeGetRequest(gNwkActiveKeySeqNumber_c));
    Copy16Bytes(nwkKey, pSecurityMaterial->nwkKey);
  }
  else
  {
    uint8_t i;
    /* generate random network key */
    for(i = 0; i<4; ++i) 
    {
      FLib_MemCpy(nwkKey + (i<<2),&tempData, sizeof(uint32_t));
      tempData= GetRandomNumber();
    } 
  }
  
  /* step 3: encrypt the Zll network key using AES ECB mode */
  AES_128_Encrypt(nwkKey, transportKey, outputKey);
}
#endif

/*!
 * @fn 		static void ZllCommissioningSecurity_DecryptNetworkKey(uint32_t transactionIdReq, uint32_t transactionIdRsp, zbAESKey_t inZllkey, zbAESKey_t encryptedNwkKey, zbAESKey_t outputKey)
 *
 * @brief	Decrypt the network key algorithm (ZLl commissioning)
 *
 */
static void ZllCommissioningSecurity_DecryptNetworkKey(uint32_t transactionIdReq, uint32_t transactionIdRsp, zbAESKey_t inZllkey, zbAESKey_t encryptedNwkKey, zbAESKey_t outputKey)
{
  zbAESKey_t    expandData, transportKey;
  
  transactionIdReq = Swap4Bytes(transactionIdReq);
  transactionIdRsp = Swap4Bytes(transactionIdRsp);
  /* step1: merge and expand the transaction identifier and response identifier */
  FLib_MemCpy(&expandData[0], &transactionIdReq, sizeof(uint32_t));
  FLib_MemCpy(&expandData[4], &transactionIdReq, sizeof(uint32_t));
  FLib_MemCpy(&expandData[8], &transactionIdRsp, sizeof(uint32_t));
  FLib_MemCpy(&expandData[12],&transactionIdRsp, sizeof(uint32_t));
  
  /* step 2: calculate the transport key: using expandKey and inKey(ZLL master or certification key) */
  AES_128_Encrypt(expandData, inZllkey, transportKey);
    
  /* step 3: decrypt the Zll network key */
  AES_128_Decrypt(encryptedNwkKey, transportKey, outputKey);
}

/*!
 * @fn 		void ZllTouchlink_StartDiscovery(bool_t initiator)
 *
 * @brief	start Zll device discovery procedure
 *
 */
void ZllTouchlink_StartDiscovery(bool_t initiator)
{  
  if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    ZllTouchlink_SetReceiverOn(gEndDevice_c); 
  }
  
  if(initiator)
  {
#if gASL_EnableZllCommissioning_Initiator_d    
    if(!ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
    {
      /* keep current channel */
      ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel) =  NlmeGetRequest(gNwkLogicalChannel_c);
    }
    
    ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw) = FALSE;
    if(IsValidExtendedAddress(ApsmeGetRequest(gApsTrustCenterAddress_c)))
    {
      ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw) = TRUE;
      ZllTouchlinkConfigData(mZllExtendedScanReq) = TRUE;
    }
    
    if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget) == NULL)
    {
      ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget) = AF_MsgAlloc();
      if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget) == NULL)
        return;
    }
    
    ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0] = 0x00;
    ZllTouchlinkConfigData(mTouchLinkInitiatorSession.zllDeviceDiscoveryCounter) = 0x00;
  
    ZllTouchlinkConfigData(mTouchLinkScanReqId) = GetRandomNumber();
    /* start from the first primary channel = 0x0b */
    ASL_ChangeChannel(gZllPrimaryLogicalChannels[0]);
  
    /* send scan Reguest */
    (void)ZllTouchlink_SendScanReq();
    ZllTouchlinkConfigData(mTouchLinkInitiatorSession.zllDeviceDiscoveryCounter)++;  
    /* set device state */
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningInitiator_ScanReq_d;
    /* start timer for the next scan request */
    ZllTouchlink_InitiatorSetTimeout(gZllAplcScanTimeBaseDuration_d);  
#endif  
  }
}
#if gASL_EnableZllCommissioning_Initiator_d
/********************************************************************************************
 *                   Touchlink initiator command handler                                    *
 ********************************************************************************************/
/*!
 * @fn 		static zbStatus_t ZllTouchlink_ScanRspHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Scan Rsp Command received from the Zll Target
 *
 */
static zbStatus_t ZllTouchlink_ScanRspHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCmdCommissioning_ScanResponse_t scanRspPayload;
  zllCommissioning_TouchlinkRemoteTarget_t remoteTarget;
  zclFrame_t *pFrame;
  uint8_t keyIndex = 0x00;
  uint16_t matchKeyBitmask;
  uint16_t keyBitmask = ZllTouchlink_GetKeyBitmask();
  bool_t requestDeviceInf = FALSE;
      
  /* verify RSSI */
  if(pIndication->linkQuality < ZllTouchlinkConfigData(mTouchLinkSession.minRssi))
    return gZclFailure_c;
  
  /* get payload data */
  pFrame = (void *)pIndication->pAsdu;
  scanRspPayload = *(zllCmdCommissioning_ScanResponse_t *)(pFrame+1);
  
  /* verify transaction Id */
  if((scanRspPayload.interPanTransactionId == 0x00)||
     (scanRspPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId)))
    return gZclFailure_c;
  
  /* verify key bitmask */
  matchKeyBitmask = scanRspPayload.keyBitmask & keyBitmask;
  if(!(matchKeyBitmask))
  {
    if(!ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw))
      return gZclFailure_c;
  }
  else
  {
    uint8_t i;
    /* the initiator shall set the key index to the bit position corresponding 
                       to the matching key with the highest index  */
    for(i=0; i<16; i++)
    {
      if(matchKeyBitmask & (0x8000>>i))
      {
        keyIndex = 15-i;
        break;
      }
    }
  }
 
  /* test device information with the last remote target */ 
  if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0])
  {
    FLib_MemCpy(&remoteTarget,&ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[1], sizeof(zllCommissioning_TouchlinkRemoteTarget_t));
    /* verify touchlink priority request */
    if(scanRspPayload.zllInf.priorityReq == remoteTarget.zllInf.priorityReq)
    {
      /* verify rssi informations */
      if(scanRspPayload.rssiCorrection + pIndication->linkQuality < remoteTarget.rssiValue)
        return gZclSuccess_c;
    }
    else
    {
      if(scanRspPayload.zllInf.priorityReq == 0x00)
        return gZclSuccess_c;
    } 
  }

  /* store current inf*/
  remoteTarget.rssiValue = scanRspPayload.rssiCorrection + pIndication->linkQuality;
  remoteTarget.dstAddrMode = pIndication->srcAddrMode;
  FLib_MemCpy(&remoteTarget.dstAddr, &pIndication ->aSrcAddr, sizeof(zbApsAddr_t));
  Copy2Bytes(remoteTarget.destPanId, pIndication->dstPanId); 
  Copy2Bytes(remoteTarget.ntwAddress, scanRspPayload.nwkAddr);
  Copy8Bytes(remoteTarget.extendedPanId, scanRspPayload.extendedPanId);
 
  if((scanRspPayload.noOfSubdevices == 0x01) &&
     (pIndication->asduLength - sizeof(zclFrame_t) == sizeof(zllCmdCommissioning_ScanResponse_t)))
  {
    /* The endpoint, profileId, device Id, Version, Group Identifier Count should  be present */
    Copy2Bytes(remoteTarget.profileId, scanRspPayload.profileId);
    Copy2Bytes(&remoteTarget.deviceId[0],  scanRspPayload.deviceId);
    remoteTarget.endPoint[0] = scanRspPayload.endPoint;
    remoteTarget.groupIdCount = scanRspPayload.groupIdCount;
    remoteTarget.version = scanRspPayload.version;
  }
  else
  {
    /* set to invalid endpoint. The right endpoint will be set when the 
        device receive Device Information Rsp comand */
    remoteTarget.endPoint[0] = 0xFF;
    requestDeviceInf = TRUE;
    
  }

  remoteTarget.keyIndex = keyIndex;
  remoteTarget.logicalChannel = scanRspPayload.logicalChannel;
  remoteTarget.noOfSubdevices = scanRspPayload.noOfSubdevices;
  remoteTarget.zbInf = scanRspPayload.zbInf;
  remoteTarget.zllInf = scanRspPayload.zllInf;
  remoteTarget.ntwUpdateId = scanRspPayload.nwkUpdateId;
  ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0] = 0xFF;
  FLib_MemCpy(&ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[1], &remoteTarget, sizeof(zllCommissioning_TouchlinkRemoteTarget_t));
  
  /* keep the interpan transaction Id*/
  ZllTouchlinkConfigData(mTouchLinkScanRspId) = scanRspPayload.responseId;
  
  /* verify no. of subdevices */
  if(requestDeviceInf)
  {
    /* send Device information Request */
    return ZllTouchlink_SendDeviceInfReq(remoteTarget.dstAddr.aIeeeAddr, remoteTarget.destPanId, ZllTouchlinkConfigData(mTouchLinkInitiatorSession.deviceConfigStartIndex));
  }
  
  return gZclSuccess_c;
}

/*!
 * @fn 		static zbStatus_t ZllTouchlink_DeviceInformationRsp(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Device Information Rsp Command received from the Zll Target
 *
 */
static zbStatus_t ZllTouchlink_DeviceInformationRsp(zbInterPanDataIndication_t *pIndication)
{
  zllCmdCommissioning_DeviceInfRsp_t *pDeviceInfRsp;
  zclFrame_t *pFrame;
  
  /* verify RSSI */
  if(pIndication->linkQuality < ZllTouchlinkConfigData(mTouchLinkSession.minRssi))
    return gZclFailure_c;
  
  /* get payload data */
  pFrame = (void *)pIndication->pAsdu;
  pDeviceInfRsp = (zllCmdCommissioning_DeviceInfRsp_t *)(pFrame+1);
  
  /* verify transaction Id */
  if((pDeviceInfRsp->interPanTransactionId == 0x00)||
     (pDeviceInfRsp->interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId)))
    return gZclFailure_c;
  
  if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0] && 
     pDeviceInfRsp->noOfSubDevices > 0x01)
  {
    zllCommissioning_TouchlinkRemoteTarget_t remoteTarget; 
    uint8_t i;
    FLib_MemCpy(&remoteTarget,&ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[1], sizeof(zllCommissioning_TouchlinkRemoteTarget_t));
    
    if(remoteTarget.endPoint[0] == 0xFF)
    {
      /* update remote target information */
      for(i=0; i< pDeviceInfRsp->deviceInfRecordCount; i++)
      {
        if(Cmp8Bytes(remoteTarget.dstAddr.aIeeeAddr, pDeviceInfRsp->deviceInfRecord[i].ieeeAddress) &&
           i < gZllCommissioningMaxEndpointCount_c)
        {
          /* set endpoint, profile Id, goupIdcount */
          remoteTarget.endPoint[i] = pDeviceInfRsp->deviceInfRecord[i].endpointId;
          Copy2Bytes(remoteTarget.profileId, pDeviceInfRsp->deviceInfRecord[i].profileId);
          Copy2Bytes(&remoteTarget.deviceId[i],  pDeviceInfRsp->deviceInfRecord[i].deviceId);
          remoteTarget.groupIdCount = pDeviceInfRsp->deviceInfRecord[i].groupIdCount;
          remoteTarget.version = pDeviceInfRsp->deviceInfRecord[i].version;
        }
      }
      FLib_MemCpy(&ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[1], &remoteTarget, sizeof(zllCommissioning_TouchlinkRemoteTarget_t));

    }
  }
  
  return gZclSuccess_c;
}

/*!
 * @fn 		static zbStatus_t ZllTouchlink_NtwStartRspHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Network Start Rsp Command received from the Zll Target
 *
 */
static zbStatus_t ZllTouchlink_NtwStartRspHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCmdCommissioning_NtwStartRsp_t ntwStartRspPayload;
  zclFrame_t *pFrame;
  zbAESKey_t zllkey, ntwKey;
  zllCommissioning_TouchlinkRemoteTarget_t remoteTarget; 
  uint32_t channelMask  = 0x00000000;
  
  
  if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0])
    FLib_MemCpy(&remoteTarget,&ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[1], sizeof(zllCommissioning_TouchlinkRemoteTarget_t));
  else
    return gZclFailure_c;
  
  MSG_Free(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget));
  ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget) = NULL;    
  
  pFrame = (void *)pIndication->pAsdu;
  ntwStartRspPayload = *(zllCmdCommissioning_NtwStartRsp_t *)(pFrame+1);
   
  /* verify transaction Id */
  if((ntwStartRspPayload.interPanTransactionId == 0x00)||
     (ntwStartRspPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId)))
  {
    ZllTouchlink_RemoveEntryFromRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr);
    return gZclFailure_c; /* discard the frame*/
  }
  
  /* verify command status */
  if(ntwStartRspPayload.status != gZllCommissioning_StatusSuccess_d)
  {   
    ZllTouchlink_RemoveEntryFromRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr);
    ZllTouchlink_InitiatorUpdateFreeRanges(remoteTarget.zllInf.addrAssignment, remoteTarget.groupIdCount, ntwStartRspPayload.status);
    return gZclFailure_c; /* discard the frame*/
  }
  
  /* set network data */
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel) = ntwStartRspPayload.logicalChannel;
  channelMask = (0x00000800<<(ntwStartRspPayload.logicalChannel-11));
  ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask[0]) = (( channelMask ) & 0xFF );
  ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask[1]) = (( channelMask >>  8 ) & 0xFF );
  ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask[2]) = (( channelMask >> 16 ) & 0xFF );
  ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask[3]) = (( channelMask >> 24 ) & 0xFF );  
  ApsmeSetRequest(gApsChannelMask_c, ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask)); 
  ZllTouchlink_SetNtwParam(ntwStartRspPayload.panId , ntwStartRspPayload.extendedPanId,
                                       ntwStartRspPayload.logicalChannel, NlmeGetRequest(gNwkShortAddress_c));
  
  /* set the network key */
  ZllCommissioningSecurity_GetZllKey(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.keyIndexSent), zllkey);
  ZllCommissioningSecurity_DecryptNetworkKey(ZllTouchlinkConfigData(mTouchLinkScanReqId), ZllTouchlinkConfigData(mTouchLinkScanRspId), 
                                               zllkey, ZllTouchlinkConfigData(mTouchLinkInitiatorSession.tempEncrNtwKey), ntwKey);

  ZbBeeStackGlobalsParams(gSAS_Ram.networkKeyType) = gStandardNetworkKey_c;
  Copy16Bytes((void *)(ZbBeeStackGlobalsParams(gSAS_Ram.aNetworkKey)), ntwKey);  
  
  /* rejoin after gZllAplcMinStartupDelayTime_d */  
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningInitiator_NetworkRejoin_d;
  ZllTouchlink_InitiatorSetTimeout(gZllAplcMinStartupDelayTime_d);
 
  return gZclSuccess_c;
}

/*!
 * @fn 		static zbStatus_t ZllTouchlink_NtwJoinRouterRspHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Network Join Router Rsp Command received from the Zll Target
 *
 */
static zbStatus_t ZllTouchlink_NtwJoinRouterRspHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCmdCommissioning_NtwJoinRouterRsp_t ntwJoinRouterRspPayload;
  zclFrame_t *pFrame;
  zllCommissioning_TouchlinkRemoteTarget_t remoteTarget; 
 
  if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0])
    FLib_MemCpy(&remoteTarget,&ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[1], sizeof(zllCommissioning_TouchlinkRemoteTarget_t));
  else
    return gZclSuccess_c; 
  
  MSG_Free(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget));
  ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget) = NULL;    
  
  pFrame = (void *)pIndication->pAsdu;
  ntwJoinRouterRspPayload = *(zllCmdCommissioning_NtwJoinRouterRsp_t *)(pFrame+1);
  
  /* verify transaction Id */
  if((ntwJoinRouterRspPayload.interPanTransactionId == 0x00)||
     (ntwJoinRouterRspPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId)))
  {
    ZllTouchlink_RemoveEntryFromRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr);
    return gZclFailure_c; /* discard the frame*/
  }
  /* verify command status */
  if(ntwJoinRouterRspPayload.status != gZllCommissioning_StatusSuccess_d)
  {
    ZllTouchlink_RemoveEntryFromRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr);
    ZllTouchlink_InitiatorUpdateFreeRanges(remoteTarget.zllInf.addrAssignment, remoteTarget.groupIdCount, ntwJoinRouterRspPayload.status);
    return gZclFailure_c; /* discard the frame*/
  } 
  /* set timeout */
  ZllTouchlink_InitiatorSetTimeout(gZllAplcMinStartupDelayTime_d);
  return gZclSuccess_c;
}


/*!
 * @fn 		static zbStatus_t ZllTouchlink_NtwJoinEndDeviceRspHandler(zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Handle the Network Join EndDevice Rsp Command received from the Zll Target
 *
 */
static zbStatus_t ZllTouchlink_NtwJoinEndDeviceRspHandler(zbInterPanDataIndication_t *pIndication)
{
  zllCmdCommissioning_NtwJoinEndDeviceRsp_t ntwJoinEndDeviceRspPayload;
  zclFrame_t *pFrame;
  zllCommissioning_TouchlinkRemoteTarget_t remoteTarget; 
    
  if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0])
    FLib_MemCpy(&remoteTarget,&ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[1], sizeof(zllCommissioning_TouchlinkRemoteTarget_t));
  else
    return gZclFailure_c; 
  
  MSG_Free(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget));
  ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget) = NULL;    
  
  pFrame = (void *)pIndication->pAsdu;
  ntwJoinEndDeviceRspPayload = *(zllCmdCommissioning_NtwJoinEndDeviceRsp_t *)(pFrame+1);
  
  /* verify transaction Id */
  if((ntwJoinEndDeviceRspPayload.interPanTransactionId == 0x00)||
     (ntwJoinEndDeviceRspPayload.interPanTransactionId != ZllTouchlinkConfigData(mTouchLinkScanReqId)))
  {
    ZllTouchlink_RemoveEntryFromRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr);
    return gZclFailure_c; /* discard the frame*/
  }
  
  /* verify command status */
  if(ntwJoinEndDeviceRspPayload.status != gZllCommissioning_StatusSuccess_d)
  {
    ZllTouchlink_RemoveEntryFromRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr);
    ZllTouchlink_InitiatorUpdateFreeRanges(remoteTarget.zllInf.addrAssignment, remoteTarget.groupIdCount, ntwJoinEndDeviceRspPayload.status);
    return gZclFailure_c; /* discard the frame*/
  }
  
  /* set timeout */
  ZllTouchlink_InitiatorSetTimeout(gZllAplcMinStartupDelayTime_d);
  
  return gZclSuccess_c;
}
#endif


/*!
 * @fn 		bool_t ZllTouchlink_ProcessEvent(uint8_t event, uint8_t *pData)
 *
 * @brief	Process Zll commissioning Touchlink events
 *Cologne
 */
bool_t ZllTouchlink_ProcessEvent(uint8_t event, uint8_t *pData)
{
  bool_t result = FALSE;
  
  if(event == gZDOToAppMgmtZRRunning_c || event == gZDOToAppMgmtZEDRunning_c ||
     (event == gZDOToAppMgmtZCRunning_c)) 
  {
#if gASL_EnableZllCommissioning_Initiator_d   
      /* keep current channel */
      ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel) =  NlmeGetRequest(gNwkLogicalChannel_c);
#endif
        
      /* update device status */
      if(ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus) != FALSE)
      {
        ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus) = FALSE;
        NvSyncSave(&ZllTouchlinkConfigData(gTouchLinkSetup), TRUE, TRUE);
      }
      
      if(!((ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioning_JoinFormNtwState_d)||
         (ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_JoinNetwork_d)||
         (ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_CreateNeighTableEntry_d)||
         (ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningInitiator_SendNetworkRejoin_d)))
      {
        if(IsValidExtendedAddress(ApsmeGetRequest(gApsTrustCenterAddress_c)))
          ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw) = TRUE;   
        return FALSE;
      }
      
      if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_JoinNetwork_d)
      {
        uint8_t aInvalidAddr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        /* set apsTrustCenterAddress to 0xff's */
        ApsmeSetRequest(gApsTrustCenterAddress_c, aInvalidAddr);       
      }
      
      if(IsValidExtendedAddress(ApsmeGetRequest(gApsTrustCenterAddress_c)))
          ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw) = TRUE;     
      
      /* Announce the capabilities and the device's info to everybody in range. 
        For rejoin the stack will automatically send deviceAnnounce */
      if((NlmeGetRequest(gDevType_c) != gEndDevice_c) && 
         (ZllTouchlinkConfigData(mTouchLinkSession.state) != gZllCommissioningInitiator_SendNetworkRejoin_d))
        DeviceAnnounce((uint8_t *)gaBroadcastRxOnIdle);

      /* verify target state */
      if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_CreateNeighTableEntry_d)
      {
        /* NLME-Direct-Join between target and initiator */
        ASL_GenerateDirectJoinReq(ZllTouchlinkConfigData(mTouchLinkSession.initiatorIeeeAddress), ZllTouchlinkConfigData(mTouchLinkSession.initiatorCapabilityInf));
      }
      /* set target state */
      ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
      ApsmeSetRequest(gApsUseInsecureJoin_c, gApsUseInsecureJoinDefault_c);
#if gComboDeviceCapability_d      
      ZllTouchlinkConfigData(gTouchLinkSetup.deviceType) = NlmeGetRequest(gDevType_c);
#endif      
  }  
  if(event == gNlmeNetworkDiscoveryConfirm_c)
  {
      /* process discovery confirm */
      nlmeNetworkDiscoveryConf_t  *pNetworkDiscoveryConf;
      pNetworkDiscoveryConf = (void *)pData;
      return ZllTouchlink_TargetNtwDiscoveryCnf(pNetworkDiscoveryConf);
  }
  if(event == gZDOToAppMgmtStopped_c)
  {
       if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_FormNetwork_d ||
          ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_JoinNetwork_d)
       {
          /* ZLL starting procedure */
          //ZllTouchlink_TargetNtwStart();
         ZllTouchlink_TargetSetTimeout(gZllAplcMinStartupDelayTime_d);
       }
       if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioning_InitialState_d)
       {
         
#if gComboDeviceCapability_d && gASL_EnableEZCommissioning_d
        if(EZCommissioning_GetState() !=  EZCommissioning_NetworkSteering_c)
          
#endif         
         /* reset Device*/
         PWRLib_Reset(); 
       }
       if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioning_ClassicalCommissioning_d)
       {
          /* go to secondary scan channels */
          zbChannels_t aSecondaryChannelMask = {gaZllCommissioningSecondaryChannelsMask_c}; 
          /* Set the channel mask */
          ApsmeSetRequest(gApsChannelMask_c, aSecondaryChannelMask);
          ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
#if gASL_EnableEZCommissioning_d
          EZComissioning_Start(gEzCommissioning_NetworkSteering_c);
#else
          ZDO_Start(gZdoStartMode_RamSet_c);  
#endif      
          return TRUE;
       }
  }
  
#if gComboDeviceCapability_d || gEndDevCapability_d
  if(event == gZDOToAppMgmtJoinNwkFailed_c)
  {   
    if(!ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
#if gComboDeviceCapability_d      
      if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif        
      {
        /* go to full scan channels */
        zbChannels_t aFullChannelMask = {0x00, 0xf8, 0xff, 0x07};
        /* Set the channel mask */
        ApsmeSetRequest(gApsChannelMask_c, aFullChannelMask);

        ZDO_Start(gZdoStartMode_FindAndRejoin_c | gZdoStartMode_RamSet_c);   
        return TRUE;
      }
  }
#endif  
  return result;
}

/*!
 * @fn 		static bool_t ZllTouchlink_TargetNtwDiscoveryCnf(nlmeNetworkDiscoveryConf_t  *pNetworkDiscoveryConf)
 *
 * @brief	Process Network discovery Confirm 
 *
 */
static bool_t ZllTouchlink_TargetNtwDiscoveryCnf(nlmeNetworkDiscoveryConf_t  *pNetworkDiscoveryConf)
{
  bool_t status = FALSE;
  
  if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_NetworkStart_d && 
     ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) != gTmrInvalidTimerID_c)
  {
    zllCommissioning_NtwStartRsp_t ntwStartRspCmd;
    
    ZbTMR_StopTimer(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr));
    
    FLib_MemCpy(&ntwStartRspCmd.addrInfo, &ZllTouchlinkConfigData(mTouchLinkSession.remoteDevAddrInfo), sizeof(InterPanAddrInfo_t));
    ntwStartRspCmd.cmdFrame.interPanTransactionId = ZllTouchlinkConfigData(mTouchLinkScanReqId);
    ntwStartRspCmd.cmdFrame.logicalChannel = ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel);
    ntwStartRspCmd.cmdFrame.nwkUpdateId = NlmeGetRequest(gNwkUpdateId_c);// 0x00; /* [R1] 7.1.2.3.3.4*/
    Copy8Bytes(ntwStartRspCmd.cmdFrame.extendedPanId, ZllTouchlinkConfigData(mTouchLinkSession.extendedPanId));
    Copy2Bytes(ntwStartRspCmd.cmdFrame.panId, ZllTouchlinkConfigData(mTouchLinkSession.panId));
    /* complete the response status */
    ntwStartRspCmd.cmdFrame.status = gZllCommissioning_StatusSuccess_d;
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningTarget_FormNetwork_d;
    
    if(pNetworkDiscoveryConf->networkCount != 0x00)
    {
      if(pNetworkDiscoveryConf->status != gZclSuccess_c)
      {
        ntwStartRspCmd.cmdFrame.status = gZllCommissioning_StatusFailed_d;
        /* set the target state */
        ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
      }
      else
      {
        uint8_t i;
        /* verify that PanId and ExtendedPandId are unique - [R1] 8.4.3.2 */
        for(i=0; i<pNetworkDiscoveryConf->networkCount; i++)
        {
          if(FLib_MemCmp(ZllTouchlinkConfigData(mTouchLinkSession.extendedPanId), pNetworkDiscoveryConf->pNetworkDescriptor[i].aExtendedPanId, sizeof(zbIeeeAddr_t)))
          {
            ntwStartRspCmd.cmdFrame.status = gZllCommissioning_StatusFailed_d;
            /* set the target state as Initial State state */
            ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
            break;
          }
        }
        if( ntwStartRspCmd.cmdFrame.status == gZllCommissioning_StatusSuccess_d)
        {
          uint8_t i;
          for(i=0; i<pNetworkDiscoveryConf->networkCount; i++)
          {
            if(FLib_Cmp2Bytes(ZllTouchlinkConfigData(mTouchLinkSession.panId), pNetworkDiscoveryConf->pNetworkDescriptor[i].aPanId))
            {
              ntwStartRspCmd.cmdFrame.status = gZllCommissioning_StatusFailed_d;
              /* set the target state as Initial State state */
              ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
              break;
            }
          }
        }
      }
    }
    
    if(ntwStartRspCmd.cmdFrame.status == gZllCommissioning_StatusSuccess_d)
    {
      uint8_t aInvalidAddr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      
      /* set apsTrustCenterAddress to 0xff's */
      ApsmeSetRequest(gApsTrustCenterAddress_c, aInvalidAddr);
      if(!ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
      {
        /* leave the network if not factory new */
         ZDO_Leave();
      }
      else
      {
        /* start operating in the new network */
        ZllTouchlink_TargetSetTimeout(gZllAplcScanTimeBaseDuration_d);
      }
    }
    
    /* send the network start response */
    (void)zclZllTouchlink_NetworkStartRsp(&ntwStartRspCmd);
    status = TRUE;
  }
  
  return status;
}
#if gASL_EnableZllCommissioning_Initiator_d
/*!
 * @fn 		zbStatus_t ZllTouchlink_CommonDiscoveryProcedure(void)
 *
 * @brief	Common Discovery procedure based on Primary, secondary channels
 *
 */

zbStatus_t ZllTouchlink_CommonDiscoveryProcedure(zllCommissioning_TouchlinkRemoteTarget_t remoteTarget)
{
   zbStatus_t status = 0xFF;
     
   if(remoteTarget.logicalChannel != NlmeGetRequest(gNwkLogicalChannel_c))
      ASL_ChangeChannel(remoteTarget.logicalChannel);

    if(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) & gZllTouchlink_FactoryNewReq_c)
    {
      status = ZllTouchlink_SendResetFactoryNewReq(remoteTarget.dstAddr.aIeeeAddr, remoteTarget.destPanId);
      ZllTouchlink_RemoveEntryFromRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr);
      MSG_Free(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget));
      ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget) = NULL; 
      ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningInitiator_ResetToFactoryNew_d;
      ZllTouchlink_InitiatorSetTimeout(gZllAplcScanTimeBaseDuration_d);
      return status;
    }   
    
     /* verify: ntwUpdateId, panID short and extended */
    if((remoteTarget.ntwUpdateId < NlmeGetRequest(gNwkUpdateId_c)) &&
       Cmp8Bytes(remoteTarget.extendedPanId, NlmeGetRequest(gNwkExtendedPanId_c)))
    {
      /* send Network Update Req */
      status = ZllTouchlink_SendNetworkUpdateReq(remoteTarget); 
      ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) = gZllTouchlink_DefaultFunctionality_c;
      return status;
    }
  
    if(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) & gZllTouchlink_DeviceInformationReq_c)
    {     
      ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) &= (~gZllTouchlink_DeviceInformationReq_c);
      status = ZllTouchlink_SendDeviceInfReq(remoteTarget.dstAddr.aIeeeAddr, remoteTarget.destPanId, ZllTouchlinkConfigData(mTouchLinkInitiatorSession.deviceConfigStartIndex));
      return status;
    }
      
    if(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) & gZllTouchlink_IdentifyReq_c)
    {
      ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) &= (~gZllTouchlink_IdentifyReq_c);
      status = ZllTouchlink_SendIdentifyReq(remoteTarget.dstAddr.aIeeeAddr, remoteTarget.destPanId,  ZllTouchlinkConfigData(mTouchLinkSession.identifyDuration)); 
      return status;
    }  
    
    return status;
}

/*!
 * @fn 		void ZllTouchlink_ClosePrimaryScanProcedure(void)
 *
 * @brief	close primary scan procedure 
 *
 */
void ZllTouchlink_ClosePrimaryScanProcedure(void)
{  
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.zllDeviceDiscoveryCounter) = 0x00;
  
  /* verify remote target informations */
  if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0])
  {
    zllCommissioning_TouchlinkRemoteTarget_t remoteTarget;
    FLib_MemCpy(&remoteTarget, &ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[1], sizeof(zllCommissioning_TouchlinkRemoteTarget_t));
    
    /* used to send identify, deviceDiscovery, ntwUpdate, 
      resetFactoryReq */
    if(ZllTouchlink_CommonDiscoveryProcedure(remoteTarget) !=  0xFF)
      return;
        
    if(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) & gZllTouchlink_StartCommissioning_c) 
    {
      ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) = gZllTouchlink_DefaultFunctionality_c;
      
      /* add the target candidate to the remote table */
      if(ZllTouchlink_AddToRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr, remoteTarget.ntwAddress,remoteTarget.noOfSubdevices,
                                              remoteTarget.endPoint, remoteTarget.profileId, remoteTarget.deviceId))
      {
        if(ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
        {
          /* send start network */
          (void)ZllTouchlink_SendNetworkStartReq(remoteTarget);
          return;
        }
        else
        {
          /* verify remote device Type */
          if(remoteTarget.zbInf.logicalType == gRouter_c)
          {
            (void)ZllTouchlink_SendNetworkJoinRouterReq(remoteTarget);
            return;
          }
          else
          {
            if(remoteTarget.zbInf.logicalType == gEndDevice_c)
            {
              (void)ZllTouchlink_SendNetworkJoinEndDeviceReq(remoteTarget);
              return;
            }
          }
        }
      }
      ASL_ChangeChannel(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel));
      ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
      MSG_Free(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget));
      ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget) = NULL; 
    }

  }
  else   
  {
    /* update logical channel */
    ASL_ChangeChannel(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel));
  }
  
  if(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) != gTmrInvalidTimerID_c)
  {
    ZbTMR_FreeTimer(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr));
    ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) = gTmrInvalidTimerID_c;
  }
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d; 
  
#if gComboDeviceCapability_d || gEndDevCapability_d        
  if(((ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) == 0x00) || 
     (!(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0])))&&
     (NlmeGetRequest(gDevType_c) == gEndDevice_c))
  {     
     /* set receiver off */
     ZllTouchlink_SetReceiverOff(gEndDevice_c);
     if(!ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
     {
        /* change Poll rate */
        (void)ZDO_NLME_ChangePollRate(ZllTouchlinkConfigData(gZllOrgPollRate)); 
     }
     else
     {
        NlmeSetRequest(gNwkState_c, gOffTheNetwork_c);
        ZDO_SetState(gZdoInitialState_c);
     }  
  }
#endif  
}

/*!
 * @fn 		void ZllTouchlink_CloseSecondaryScanProcedure(void)
 *
 * @brief	close secondary scan procedure 
 *
 */
void ZllTouchlink_CloseSecondaryScanProcedure(void)
{
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.zllDeviceDiscoveryCounter) = 0x00;
  
#if gComboDeviceCapability_d || gEndDevCapability_d   
  if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    /* set receiver off */
    ZllTouchlink_SetReceiverOff(gEndDevice_c);
    /* change Poll rate */
    (void)ZDO_NLME_ChangePollRate(ZllTouchlinkConfigData(gZllOrgPollRate)); 
  }  
#endif  
  
  /* verify remote target informations */
  if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0])
  {
    zllCommissioning_TouchlinkRemoteTarget_t remoteTarget;
    FLib_MemCpy(&remoteTarget, &ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[1], sizeof(zllCommissioning_TouchlinkRemoteTarget_t));
    
    /* used to send identify, deviceDiscovery, ntwUpdate, 
      resetFactoryReq */
    if(ZllTouchlink_CommonDiscoveryProcedure(remoteTarget) !=  0xFF)
      return;
       
    if((ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) & gZllTouchlink_StartCommissioning_c) && 
            (ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw)) && (ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel) == remoteTarget.logicalChannel)&&
            (Cmp8Bytes(remoteTarget.extendedPanId, NlmeGetRequest(gNwkExtendedPanId_c))))
    {
        /* add the target candidate to the remote table */
         (void)ZllTouchlink_AddToRemoteDeviceTable(remoteTarget.dstAddr.aIeeeAddr, remoteTarget.ntwAddress,remoteTarget.noOfSubdevices,
                                              remoteTarget.endPoint, remoteTarget.profileId, remoteTarget.deviceId);
         BeeAppUpdateDevice(BeeAppDataInit(appEndPoint), gZclUI_EZCommissioning_Succesfull_c, 0, 0, NULL);
         
    }
    else
    {
        ASL_ChangeChannel(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel));
    }
    
    MSG_Free(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget));
    ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget) = NULL; 
  }  
  else
  {
    /* update logical channel */
    ASL_ChangeChannel(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel));
  }
  
  if(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) != gTmrInvalidTimerID_c)
  {
    ZbTMR_FreeTimer(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr));
    ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) = gTmrInvalidTimerID_c;
  }
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;  
}
/*!
 * @fn 		bool_t  ZllTouchlink_IsPrimaryChannel(uint8_t logicalChannel)
 *
 * @brief	return TRUE if the device is in the primary Channel list
 *
 */
bool_t  ZllTouchlink_IsPrimaryChannel(uint8_t logicalChannel)
{
  return (logicalChannel == gZllPrimaryLogicalChannels[0] || logicalChannel == gZllPrimaryLogicalChannels[1] || 
           logicalChannel == gZllPrimaryLogicalChannels[2] || logicalChannel == gZllPrimaryLogicalChannels[3]);
}
/*!
 * @fn 		uint8_t ZllTouchlink_GetNextPrimaryChannel(uint8_t counter)
 *
 * @brief	Get next primary channel
 *
 */
uint8_t ZllTouchlink_GetNextPrimaryChannel(uint8_t counter)
{
  uint8_t logicalChannel;
  
  if(counter == gZllCommissioningDiscoveryCh11ScanReq_d)
    logicalChannel = gZllPrimaryLogicalChannels[1];
  else if(counter == gZllCommissioningDiscoveryCh11ScanReq_d+1)
    logicalChannel = gZllPrimaryLogicalChannels[2];
  else if(counter == gZllCommissioningDiscoveryCh11ScanReq_d+2)
    logicalChannel = gZllPrimaryLogicalChannels[3];
  else
    logicalChannel = 0xFF;
  
  return logicalChannel;
}


/*!
 * @fn 		static void ZllTouchlink_InitiatorDeviceDiscoveryCallBack(void)
 *
 * @brief	Zll device discovery callback
 *
 */
static void ZllTouchlink_InitiatorDeviceDiscoveryCallBack(void)
{ 
  /* do not send network start, network join router or network join end device 
  if the initiator is connected to a trust centre protected network*/
  if((ZllTouchlinkConfigData(mTouchLinkInitiatorSession.zllDeviceDiscoveryCounter) == gZllCommissioningDiscoveryPrimaryChScanReq_d)&&
     (!ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw)) && (!ZllTouchlinkConfigData(mZllExtendedScanReq)))
  {
    /* verify remote target informations */
    if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget)[0])
    {
      ZllTouchlink_ClosePrimaryScanProcedure();
      ZllTouchlinkConfigData(mZllExtendedScanReq) = FALSE;
      return;
    }  
    else
    {
      /* enable extra scan for secondary channels if NtwUpdateId != 0x00 or 
        functionality = FactoryNewReq */
      if((NlmeGetRequest(gNwkUpdateId_c) != 0x00) ||
           (ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) & gZllTouchlink_FactoryNewReq_c))
      {
        ZllTouchlinkConfigData(mZllExtendedScanReq) = TRUE;
      }
    }
  }
  
  if(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.zllDeviceDiscoveryCounter) > gZllCommissioningDiscoveryCh11ScanReq_d-1)
  {
    uint8_t  logicalChannel = ZllTouchlink_GetNextPrimaryChannel(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.zllDeviceDiscoveryCounter));
    
    if(logicalChannel == 0xFF)
    {
      logicalChannel =  NlmeGetRequest(gNwkLogicalChannel_c); 
      if(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.zllDeviceDiscoveryCounter) == gZllCommissioningDiscoveryPrimaryChScanReq_d)
      {
        if(ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw) || ZllTouchlinkConfigData(mZllExtendedScanReq))
          logicalChannel = 12; /* first secondary channel*/
        else
        {
          ZllTouchlink_ClosePrimaryScanProcedure();
          return;
        }
      }
      else
      {
        /* secondary channels: 12,13,14,16,17,18,19,21,22,23,24,26*/
        if(logicalChannel == 26)
        {
          if(ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw))
            ZllTouchlink_CloseSecondaryScanProcedure();     
          else
            ZllTouchlink_ClosePrimaryScanProcedure();
          
          return;
        }
        else
        {
          logicalChannel++;
          if(ZllTouchlink_IsPrimaryChannel(logicalChannel))
            logicalChannel++;
        }
      }
    }
      
    /* change the channel */
    ASL_ChangeChannel(logicalChannel);
  }   
  
  /* send scan Reguest */   
  (void)ZllTouchlink_SendScanReq();
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.zllDeviceDiscoveryCounter)++;  
  
  /* start timer for the next scan request */
  ZllTouchlink_InitiatorSetTimeout(gZllAplcScanTimeBaseDuration_d);
}

/*!
 * @fn 		static void ZllTouchlink_InitiatorSetTimeout(uint32_t duration  ) 
 *
 * @brief	set timeout
 *
 */
static void ZllTouchlink_InitiatorSetTimeout(uint32_t duration) /* miliseconds */
{
  if(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) == gTmrInvalidTimerID_c)
  {
    ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) = ZbTMR_AllocateTimer(); 
    if(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) == gTmrInvalidTimerID_c)
      return;
  }
  ZbTMR_StartTimer(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr), gTmrSingleShotTimer_c|gTmrLowPowerTimer_c, duration, ZllTouchlink_InitiatorTimeoutCallBack);
}
/*!
 * @fn 		static void ZllTouchlink_InitiatorTimeoutCallBack(uint8_t tmrId)
 *
 * @brief	process Initiator Timeout
 *
 */
static void ZllTouchlink_InitiatorTimeoutCallBack(uint8_t tmrId)
{
   uint8_t freeTimer = FALSE;
   if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningInitiator_ScanReq_d)
   {
     ZllTouchlink_InitiatorDeviceDiscoveryCallBack();
     return;
   }
   if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningInitiator_DeviceInfReq_d ||
      ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningInitiator_IdentifyReq_d)
   {
      if(!ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw))
        ZllTouchlink_ClosePrimaryScanProcedure();
      else
        ZllTouchlink_CloseSecondaryScanProcedure();
     return;
   }
   
   if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningInitiator_NetworkStart_d ||
      ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningInitiator_NetworkJoinReq_d ||
       ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningInitiator_NetworkUpdateReq_d)
   {
     freeTimer = TRUE;
     ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
     /* update logical channel */
     ASL_ChangeChannel(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel));
#if gComboDeviceCapability_d || gEndDevCapability_d    
     if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
     {
        /* set receiver off */
        ZllTouchlink_SetReceiverOff(gEndDevice_c);
        if(!ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
        {
          /* change Poll rate */
          (void)ZDO_NLME_ChangePollRate(ZllTouchlinkConfigData(gZllOrgPollRate)); 
        }
     }
#endif     
     if(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget))
     {
        MSG_Free(ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget));
        ZllTouchlinkConfigData(mpTouchLinkTempRemoteTarget) = NULL; 
     }
   } 
   if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningInitiator_NetworkRejoin_d)
   {
     ZllTouchlink_RejoinCallBack();
     freeTimer = TRUE;
   }
   if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningInitiator_ResetToFactoryNew_d)
   {
     ASL_ChangeChannel(ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel));
     ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d; 
     freeTimer = TRUE;
   }
      
   /* free the timer if required */
   if(freeTimer)
   {
     ZbTMR_FreeTimer(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr));
     ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) = gTmrInvalidTimerID_c;
   }
}

/*!
 * @fn 		static void ZllTouchlink_RejoinCallBack(void)
 *
 * @brief	Zll device rejoin callback
 *
 */
static void ZllTouchlink_RejoinCallBack(void)
{  
  uint8_t deviceType = NlmeGetRequest(gDevType_c);
  uint8_t aInvalidAddr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  
  /* start operating in the network */
  if(ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
  {
    NlmeSetRequest(gNwkState_c, gOffTheNetwork_c);
    ZDO_SetState(gZdoInitialState_c);
  }

  /* verify device type */
  if(deviceType == gEndDevice_c)
  {
    deviceType = gZdoStartMode_Zed_c;
#if gComboDeviceCapability_d || gEndDevCapability_d    
    ZllTouchlink_SetReceiverOff(gEndDevice_c);
#endif    
  }
  else
  {
    if(deviceType == gRouter_c)
    {
      deviceType = gZdoStartMode_Zr_c;
    }
    else
    {
      return;
    }
  }
  ApsmeSetRequest(gApsUseInsecureJoin_c, FALSE);
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningInitiator_SendNetworkRejoin_d;
  
  /* set apsTrustCenterAddress to 0xff's */
  ApsmeSetRequest(gApsTrustCenterAddress_c, aInvalidAddr);
  
  /* start using ram informations */
  ZDO_Start(deviceType | gZdoStartMode_FindAndRejoin_c | gZdoStartMode_RamSet_c);  
}
#endif /* gASL_EnableZllCommissioning_Initiator_d */

/*!
 * @fn 		static void ZllTouchlink_TargetNtwStart(void)
 *
 * @brief	Zll target network start
 *
 */
static void ZllTouchlink_TargetNtwStart(void)
{
  zbAESKey_t zllkey, ntwKey;
  uint32_t channelMask  = 0x00000000;
  uint8_t startMode;
  if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    startMode = gZdoStartMode_Zed_c;
  }
  else
  {
    if(NlmeGetRequest(gDevType_c) == gRouter_c)
      startMode = gZdoStartMode_Zr_c;
    else
      return;
  }
  
  channelMask = (0x00000800<<(ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel)-11));
  ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask[0]) = (( channelMask ) & 0xFF );
  ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask[1]) = (( channelMask >>  8 ) & 0xFF );
  ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask[2]) = (( channelMask >> 16 ) & 0xFF );
  ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask[3]) = (( channelMask >> 24 ) & 0xFF );  
  ApsmeSetRequest(gApsChannelMask_c, ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask)); 
  ZllTouchlink_SetNtwParam(ZllTouchlinkConfigData(mTouchLinkSession.panId), ZllTouchlinkConfigData(mTouchLinkSession.extendedPanId),
                                       ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel), ZllTouchlinkConfigData(mTouchLinkSession.ntwAddress));
  ZDO_SetState(gZdoInitialState_c);
    
  /* update the network key */
  ZllCommissioningSecurity_GetZllKey(ZllTouchlinkConfigData(mTouchLinkSession.keyIndex), zllkey);
  ZllCommissioningSecurity_DecryptNetworkKey(ZllTouchlinkConfigData(mTouchLinkScanReqId), ZllTouchlinkConfigData(mTouchLinkScanRspId), 
                                               zllkey, ZllTouchlinkConfigData(mTouchLinkSession.encryptedNtwKey), ntwKey);

  ZbBeeStackGlobalsParams(gSAS_Ram.networkKeyType) = gStandardNetworkKey_c;
  Copy16Bytes((void *)(ZbBeeStackGlobalsParams(gSAS_Ram.aNetworkKey)), ntwKey);
    
  /* set networkUpdate Id */
  NlmeSetRequest(gNwkUpdateId_c, &ZllTouchlinkConfigData(mTouchLinkSession.ntwUpdateId)); 
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
    NWK_FRAMEHANDLE_SetBeaconPayloadRAM();  
#endif
  
  /* set the next state */
  if(ZllTouchlinkConfigData(mTouchLinkSession.state) == gZllCommissioningTarget_NetworkStart_d)
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioningTarget_CreateNeighTableEntry_d;
  else
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_JoinFormNtwState_d;
  
  /* start operating in the network */
  ZDO_Start(startMode | gZdoStartMode_SilentStart_c | gZdoStartMode_RamSet_c); 
}

/************************************************************************************************/
/*               ZLL Commissioning procedure: Init functions                                    */ 
/************************************************************************************************/
/*!
 * @fn 		void ZllTouchlink_Init(bool_t startWithNvm)
 *
 * @brief	init Zll commissioning procedure 
 *
 */
bool_t ZllTouchlink_Init(bool_t startWithNvm)
{
  /* init touchlink variables */
  ZllTouchlink_InitVariables();
  
  /* restore touchlink dataSet */  
  if(gNVM_OK_c != NvRestoreDataSet(&ZllTouchlinkConfigData(gTouchLinkSetup), TRUE))  
  {
      /* failed case: */
      ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus) = TRUE;
  }
  
  if(!ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
  {
#if gASL_EnableZllCommissioning_Initiator_d    
      NvRestoreDataSet(&ZllTouchlinkConfigData(gZllRemoteDeviceTable), TRUE);
#endif      
      if(startWithNvm)
      {
        uint8_t deviceType = NlmeGetRequest(gDevType_c);
        uint8_t startMode = gStartSilentRejoinWithNvm_c;
        ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_JoinFormNtwState_d; 
#if gComboDeviceCapability_d      
        deviceType = ZllTouchlinkConfigData(gTouchLinkSetup.deviceType);
#endif  
        startMode = (deviceType == gEndDevice_c)?gStartNwkRejoinWithNvm_c:gStartSilentRejoinWithNvm_c;
        ZDO_Start(startMode);
        return TRUE;
      }
  }
  else
  {
#if !gCoordinatorCapability_d      
     ZllTouchlink_InitDevice();
#endif     
  }
  return FALSE;
}
/*
 * @fn 		void ZllTouchlink_InitVariables(void)
 *
 * @brief	Init the ZLL Commissioning TouchLink Procedure  
 *
 */
void ZllTouchlink_InitVariables(void)
{   
  /* common data: */ 
  ZllTouchlinkConfigData(mTouchLinkScanReqId) = 0x00;
  ZllTouchlinkConfigData(mTouchLinkScanRspId) = 0x00;
  ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;
  ZllTouchlinkConfigData(mTouchLinkSession.ntwUpdateId) = 0x00; 
  ZllTouchlinkConfigData(mTouchLinkSession.minRssi) = gZllCommissioning_MinRssiValue_d;
  ZllTouchlinkConfigData(mTouchLinkSession.identifyDuration) = gZllCommissioning_KeepIdentifyMode_d;
  if(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr))
    ZbTMR_FreeTimer(ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr));
  ZllTouchlinkConfigData(mTouchLinkSession.zllTouchlinkTmr) = gTmrInvalidTimerID_c;
  
  /* initiator: */
#if gASL_EnableZllCommissioning_Initiator_d     
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.zllDeviceDiscoveryCounter) = 0x00;
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.deviceConfigStartIndex) = 0x00;
#endif
}
/*
 * @fn 		static void ZllTouchlink_InitNtwAddrGroupRanges(void)
 *
 * @brief	Init the ZLL ntw address and group ranges
 *
 */
static void ZllTouchlink_InitNtwAddrGroupRanges(void)
{
#if gASL_EnableZllCommissioning_Initiator_d
  if(ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
  {
    ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) = gZllAplFreeNwkAddRangeBegin_d;
    ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd) = gZllAplFreeNwkAddRangeEnd_d;
    ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) = gZllAplGroupIdRangeBegin_d;
    ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd) = gZllAplGroupIdRangeEnd_d;
  }
#else
  ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) = ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd) = 0x00;
  ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) = ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd) = 0x00;
#endif 
}
/*
 * @fn 		void ZllTouchlink_GetDefaultLogicalChannel(void)
 *
 * @brief	Get default logical channel  
 *
 */
static uint8_t ZllTouchlink_GetDefaultLogicalChannel(void)
{
  uint8_t randomData = (uint8_t)GetRandomNumber();
  uint8_t logicalChannel = gZllPrimaryLogicalChannels[3];
  
  if(randomData < 64)
    logicalChannel =  gZllPrimaryLogicalChannels[0];
  else 
  {
    if(randomData < 128)
      logicalChannel =  gZllPrimaryLogicalChannels[1];
    else if (randomData < 192)
      logicalChannel =  gZllPrimaryLogicalChannels[2];
  }
  return logicalChannel;
}
/*
 * @fn 		void ZllTouchlink_DefaultNetworkData(void)
 *
 * @brief	Default ZLL network data  
 *
 */
void ZllTouchlink_DefaultNetworkData(void)
{
  zbPanId_t panId = {mDefaultValueOfPanId_c};
  zbIeeeAddr_t extendedPanId = {mDefaultNwkExtendedPANID_c}; 
  uint16_t ntwAddress = GetRandomNumber();
  
  if(!IsValidPanId(panId))
    Set2Bytes(panId, GetRandomNumber());
  Copy2Bytes(ZllTouchlinkConfigData(mTouchLinkSession.panId), panId);
  
  if(!IsValidExtendedPanId(extendedPanId))
  {
    uint8_t i;
    uint32_t tempData= GetRandomNumber();
    for(i = 0; i<2; ++i) 
    {
        FLib_MemCpy(extendedPanId + (i<<2),&tempData, sizeof(uint32_t));
        tempData= GetRandomNumber();
    } 
  }
  Copy8Bytes(ZllTouchlinkConfigData(mTouchLinkSession.extendedPanId), extendedPanId);
  
  if(ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) != 0x0000)
  {
    ntwAddress = ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin);
    ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin)++;
  }
  Set2Bytes(ZllTouchlinkConfigData(mTouchLinkSession.ntwAddress), ntwAddress);
  BUtl_CreateExtendedAddress();
  
  ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel) = gLogicalChannel11_c; 
  if(gZllCommissioningDefaultDeviceType == gZdoStartMode_Zr_c)
    ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel) = ZllTouchlink_GetDefaultLogicalChannel();
#if gASL_EnableZllCommissioning_Initiator_d   
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.ntwLogicalChannel) = ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel);
#endif  
  ZllTouchlink_SetNtwParam(ZllTouchlinkConfigData(mTouchLinkSession.panId), ZllTouchlinkConfigData(mTouchLinkSession.extendedPanId),
                                       ZllTouchlinkConfigData(mTouchLinkSession.logicalChannel), ZllTouchlinkConfigData(mTouchLinkSession.ntwAddress));
}

/*
 * @fn 		void ZllTouchlink_DefaultNonZllNetworkData(zbChannels_t channelMask)
 *
 * @brief	Default network data
 *
 */
void ZllTouchlink_DefaultNonZllNetworkData(zbChannels_t channelMask)
{
#if !gCoordinatorCapability_d
  zbPanId_t panId = {0xFF, 0xFF};
  zbIeeeAddr_t extendedPanId = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
  zbNwkAddr_t ntwAddress = {0xFF, 0xFF};
#else
  zbPanId_t panId = {mDefaultValueOfPanId_c};
  zbIeeeAddr_t extendedPanId = {mDefaultNwkExtendedPANID_c}; 
  zbNwkAddr_t ntwAddress = {0x00, 0x00};
#endif

  NlmeSetRequest(gNwkPanId_c, panId); 
  NlmeSetRequest(gNwkExtendedPanId_c, extendedPanId); 
  ApsmeSetRequest(gApsUseExtendedPANID_c, extendedPanId);
  NlmeSetRequest(gNwkShortAddress_c, ntwAddress); 
  
  NlmeSetRequest(gNwkState_c, gOffTheNetwork_c);
  ZDO_SetState(gZdoInitialState_c);
  
  /* Set the channel mask */
  ApsmeSetRequest(gApsChannelMask_c, channelMask);
}
/*
 * @fn 		void ZllTouchlink_InitDevice(void)
 *
 * @brief	Init the ZLL Commissioning TouchLink Procedure  
 *
 */
void ZllTouchlink_InitDevice(void)
{ 
  if(!((gZllCommissioningDefaultDeviceType == gZdoStartMode_Zr_c) ||
      (gZllCommissioningDefaultDeviceType == gZdoStartMode_Zed_c)))
      return;
    
#if gASL_EnableZllCommissioning_Initiator_d  
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) = gZllTouchlink_DefaultFunctionality_c;
#endif      
    
  ZllTouchlink_InitNtwAddrGroupRanges();  

  /* set to default */
  ZllTouchlink_DefaultNetworkData();
 
#if gComboDeviceCapability_d  
  ZllTouchlinkConfigData(gTouchLinkSetup.deviceType) = gEndDevice_c;
#endif     
  
#if gASL_EnableZllCommissioning_Initiator_d  
  FLib_MemSet(&ZllTouchlinkConfigData(gTouchLinkSetup.groupTable[0]), 0xFF, sizeof(zllTouchlink_GroupTable_t)*gZLLCommissioning_MaxNoOfGroups_c);
#endif  
  
#if gComboDeviceCapability_d || gRouterCapability_d  
 /* set receiver On if router; end device wait for an application begin to start touchlink procedure */
 if(gZllCommissioningDefaultDeviceType == gZdoStartMode_Zr_c)
 {
      NlmeSetRequest(gDevType_c, gRouter_c);
#if gComboDeviceCapability_d     
      ZllTouchlinkConfigData(gTouchLinkSetup.deviceType) = gRouter_c;
#endif      
      ZllTouchlink_SetReceiverOn(gRouter_c);  
 }
#endif
 
#if gComboDeviceCapability_d || gEndDevCapability_d
 if(gZllCommissioningDefaultDeviceType == gZdoStartMode_Zed_c)
 {
#if gComboDeviceCapability_d    
      ZllTouchlinkConfigData(gTouchLinkSetup.deviceType) = gEndDevice_c;
#endif      
      NlmeSetRequest(gDevType_c, gEndDevice_c);
 }
#endif 
}
/*
 * @fn 		static void ZllTouchlink_SetNtwParam(zbPanId_t panId, zbIeeeAddr_t extendedPanId, uint8_t logicalChannel, zbNwkAddr_t ntwAddress)
 *
 * @brief	Set target ntw parameter 
 *
 */
static void ZllTouchlink_SetNtwParam(zbPanId_t panId, zbIeeeAddr_t extendedPanId, uint8_t logicalChannel, zbNwkAddr_t ntwAddress)
{
  NlmeSetRequest(gNwkState_c, gOnTheNetwork_c);
  NlmeSetRequest(gNwkPanId_c, panId); 
  NlmeSetRequest(gNwkExtendedPanId_c, extendedPanId); 
  ApsmeSetRequest(gApsUseExtendedPANID_c, extendedPanId);
  NlmeSetRequest(gNwkLogicalChannel_c, &logicalChannel);  
  ASL_ChangeChannel(logicalChannel);
  NlmeSetRequest(gNwkShortAddress_c, ntwAddress);   
}
/*
 * @fn 		static uint16_t ZllTouchlink_GetKeyBitmask(void)
 *
 * @brief	Get key bitmask 
 *
 */
static uint16_t ZllTouchlink_GetKeyBitmask(void)
{
  uint16_t keyBitmask = 0x00;
  zbAESKey_t zllMasterKey = {gZllCommissioning_MasterKey_d};
  
  keyBitmask = (0x0001<<gZllCommissioning_DevelopmentKeyIndex_d) | (0x0001<<gZllCommissioning_CertificationKeyIndex_d);
  if(!CmpToFs(zllMasterKey, sizeof(zbAESKey_t)))
    keyBitmask = (0x0001 << gZllCommissioning_MasterKeyIndex_d); 
  
  return keyBitmask;
}
#if gASL_EnableZllCommissioning_Initiator_d
/*
 * @fn 		static void ZllTouchlink_GetAvailableGroupIds(uint16_t *pGroupIdBegin, uint16_t *pGroupIdEnd, uint8_t requiredGroupNo)
 *
 * @brief	Get available group Ids
 *
 */
static void ZllTouchlink_GetAvailableGroupIds(uint16_t *pGroupBegin, uint16_t *pGroupEnd, uint8_t requiredGroupNo)
{
  if((requiredGroupNo == 0x00)||
     (ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) < gZllAplGroupIdRangeBegin_d)||
     (ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) > ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd))||
      (ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd) - ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) > requiredGroupNo))
  {
    /* not available groups */
    *pGroupBegin = *pGroupEnd  = 0x00;
  }
  else
  {
    *pGroupBegin = ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin);
    *pGroupEnd = ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) + requiredGroupNo - 1;
    /* update available group begin */
    ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) += requiredGroupNo;
  }
}
/*
 * @fn 		static void ZllTouchlink_GetAvailableFreeRanges(uint16_t *pGroupBegin, uint16_t *pGroupEnd, uint16_t *pNtwAddrBegin, uint16_t *pNtwAddrEnd, bool_t ntwAssign)

 *
 * @brief	Get available free ntwAddr and group ranges. Split the range and assign the upper range to the new device
 *
 */
static void ZllTouchlink_GetAvailableFreeRanges(uint16_t *pGroupBegin, uint16_t *pGroupEnd, uint16_t *pNtwAddrBegin, uint16_t *pNtwAddrEnd, bool_t ntwAssign)
{
  if((!ntwAssign)||(ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) >= ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd))||
    (ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) >= ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd)))
  {
    *pGroupBegin = *pGroupEnd = *pNtwAddrBegin = *pNtwAddrEnd = 0x00;
  }
  else
  {
    *pGroupBegin = (uint16_t)((uint32_t)(ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd) + ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) + 1)/2);
    *pGroupEnd = ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd);
    ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd)= *pGroupBegin-1;
    
    *pNtwAddrBegin = (uint16_t)((uint32_t)(ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd) + ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) + 1)/2);;
    *pNtwAddrEnd = ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd);
    ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd) = *pNtwAddrBegin-1;
  }
}


/*
 * @fn 		static void ZllTouchlink_InitiatorUpdateFreeRanges(bool_t ntwAssign, uint8_t groupNo, uint8_t status)
 *
 * @brief	update initiator free ranges
 *
 */
static void ZllTouchlink_InitiatorUpdateFreeRanges(bool_t ntwAssign, uint8_t groupNo, bool_t status)
{  
  if(status == gZllCommissioning_StatusFailed_d)
  {
    if(ntwAssign)
    {
      uint32_t tempData = ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd);
      ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrEnd) = (uint16_t)(2*tempData-ZllTouchlinkConfigData(gTouchLinkSetup.freeNwkAddrBegin) + 1);
      tempData = ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd);
      ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd) = (uint16_t)(2*tempData-ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) + 1);
    }
    if(groupNo)
    {
      ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) -= groupNo;
    }
  }
} 
#endif /* gASL_EnableZllCommissioning_Initiator_d */

/*
 * @fn 		static void ZllTouchlink_SetReceiverOn(uint8_t deviceType)
 *
 * @brief	Commissionin touchlink Set Receiver On
 *
 */
static void ZllTouchlink_SetReceiverOn(uint8_t deviceType)
{ 
  bool_t status = TRUE;
  
#if (gComboDeviceCapability_d ||  gEndDevCapability_d) && gASL_EnableZllCommissioning_Initiator_d 
  /* ZED: remember original polling rate */
  ZllTouchlinkConfigData(gZllOrgPollRate) = NlmeGetRequest(gNwkIndirectPollRate_c);  
  
  if((!ZDO_IsRunningState()) && (!ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus)))
  {
    if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
    {
      ZllTouchlinkConfigData(gEndDevForcedTouchlink) = TRUE;
    }
  }  
  if(ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus) ||
     ZllTouchlinkConfigData(gEndDevForcedTouchlink))
#else
    
  if(ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus))
#endif      
  {
    uint8_t runningState;
#if gComboDeviceCapability_d      
    uint8_t iCapInfo = NlmeGetRequest(gNwkCapabilityInformation_c);
#endif    
    
    NlmeSetRequest(gNwkState_c, gOnTheNetwork_c);
    if(!(deviceType == gEndDevice_c || deviceType == gRouter_c))
      return;
    
    runningState = (deviceType == gEndDevice_c)?gZdoEndDeviceRunningState_c:gZdoRouterRunningState_c;
    ZDO_SetState(runningState);
#if gComboDeviceCapability_d  
    iCapInfo |= gReceiverOnwhenIdle_c;
    iCapInfo |= (deviceType == gEndDevice_c)?( gZdoStartMode_Zed_c):(gZdoStartMode_Zr_c);    
    NlmeSetRequest(gNwkCapabilityInformation_c, &iCapInfo);
#endif    
  }
#if gComboDeviceCapability_d || gEndDevCapability_d
  else
  {    
    if(deviceType == gEndDevice_c)
    {
      (void)ZDO_NLME_ChangePollRate(0);
    }
  }
#endif  
  
 (void)SetPibAttributeValue( gMPibRxOnWhenIdle_c, &status);
}

#if gComboDeviceCapability_d || gEndDevCapability_d
/*
 * @fn 		void ZllTouchlink_SetReceiverOff(uint8_t deviceType)
 *
 * @brief	Commissionin touchlink Set Receiver On
 *
 */
void ZllTouchlink_SetReceiverOff(uint8_t deviceType)
{ 
  uint8_t iCapInfo = NlmeGetRequest(gNwkCapabilityInformation_c);
  
  if(deviceType == gEndDevice_c)
  {
    iCapInfo &= ~(gReceiverOnwhenIdle_c | gDeviceType_c);
    NlmeSetRequest(gNwkCapabilityInformation_c, &iCapInfo);
  }
  BUtl_SetReceiverOff(); 
  
#if (gComboDeviceCapability_d || gEndDevCapability_d) && gASL_EnableZllCommissioning_Initiator_d   
  /* reset the device if a Touchlink Procedure was forced when the 
     device state != running */
  if(ZllTouchlinkConfigData(gEndDevForcedTouchlink))
  {
    if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
    {
      ZllTouchlinkConfigData(gEndDevForcedTouchlink) = FALSE;
      PWRLib_Reset();
    }
  }
#endif  
  
}
#endif

#if gASL_EnableZllCommissioning_Initiator_d
/*
 * @fn 		static bool_t ZllTouchlink_AddToRemoteDeviceTable(zbIeeeAddr_t ieeeAddr, zbNwkAddr_t ntwAddr, 
 *                                                 uint8_t endpointCount, uint8_t *endPointList, 
 *                                                 zbProfileId_t profileId, zbProfileId_t deviceId)
 * @brief	Add to remote device table 
 *
 */
static bool_t ZllTouchlink_AddToRemoteDeviceTable(zbIeeeAddr_t ieeeAddr, zbNwkAddr_t ntwAddr, 
                                                  uint8_t endpointCount, uint8_t *endPointList, 
                                                  zbProfileId_t profileId, zbProfileId_t *deviceIdList)
{ 
  uint8_t i, index = 0xFF;
  
  for(i=0;i<gZLLCommissioning_MaxRemoteDevices_c; i++)
  {
    /* verify existing informations */
    if(Cmp8Bytes(ieeeAddr, ZllTouchlinkConfigData(gZllRemoteDeviceTable[i].ieeeAddress)) &&
       endPointList[0] == ZllTouchlinkConfigData(gZllRemoteDeviceTable[i].endpointId[0]))
    {
      /* update entry */
      Copy2Bytes(ZllTouchlinkConfigData(gZllRemoteDeviceTable[i].profileId), profileId);
      Copy2Bytes(ZllTouchlinkConfigData(gZllRemoteDeviceTable[i].ntwAddress), ntwAddr);
      return TRUE;
    }
    
    /* get the first free entry */
    if((!IsValidExtendedAddress(ZllTouchlinkConfigData(gZllRemoteDeviceTable[i].ieeeAddress)))&& (index == 0xFF))
    {
      index = i;
    }
  }
    
  if(index !=0xFF)  
  {
     /* update entry */
      for(i = 0; i< endpointCount && i< gZllCommissioningMaxEndpointCount_c; i++)
      {
        ZllTouchlinkConfigData(gZllRemoteDeviceTable[index].endpointId[i]) =  endPointList[i];
        Copy2Bytes(&ZllTouchlinkConfigData(gZllRemoteDeviceTable[index].deviceId[i]), &deviceIdList[i]); 
      }
      Copy8Bytes(ZllTouchlinkConfigData(gZllRemoteDeviceTable[index].ieeeAddress), ieeeAddr);
      Copy2Bytes(ZllTouchlinkConfigData(gZllRemoteDeviceTable[index].profileId), profileId);
      Copy2Bytes(ZllTouchlinkConfigData(gZllRemoteDeviceTable[index].ntwAddress), ntwAddr);
      
      /* store the entry: */
      NvSyncSave(&ZllTouchlinkConfigData(gZllRemoteDeviceTable), TRUE, TRUE);
      return TRUE;
  }
  return FALSE;
}
/*
 * @fn 		void ZllTouchlink_UpdateNtwkAddrRemoteDeviceTable(zbIeeeAddr_t ieeeAddr, zbNwkAddr_t ntwAddr)
 *
 * @brief	Update network address in remote device table 
 *
 */
void ZllTouchlink_UpdateNtwkAddrRemoteDeviceTable(zbIeeeAddr_t ieeeAddr, zbNwkAddr_t ntwAddr)
{
  uint8_t i;
  for(i=0;i<gZLLCommissioning_MaxRemoteDevices_c; i++)
  {
    /* verify existing informations */
    if(Cmp8Bytes(ieeeAddr, ZllTouchlinkConfigData(gZllRemoteDeviceTable[i].ieeeAddress)))
    {
      Copy2Bytes(ZllTouchlinkConfigData(gZllRemoteDeviceTable[i].ntwAddress), ntwAddr);
      /* store the update: */
      NvSyncSave(&ZllTouchlinkConfigData(gZllRemoteDeviceTable), TRUE, TRUE);     
      return;
    }
  }
}
/*
 * @fn 		void ZllTouchlink_RemoveEntryFromRemoteDeviceTable(zbIeeeAddr_t ieeeAddr)
 *
 * @brief	Update network address in remote device table 
 *
 */
void ZllTouchlink_RemoveEntryFromRemoteDeviceTable(zbIeeeAddr_t ieeeAddr)
{
  uint8_t i;
  for(i=0;i<gZLLCommissioning_MaxRemoteDevices_c; i++)
  {
    /* verify existing informations */
    if(Cmp8Bytes(ieeeAddr, ZllTouchlinkConfigData(gZllRemoteDeviceTable[i].ieeeAddress)))
    {
      FLib_MemSet(&ZllTouchlinkConfigData(gZllRemoteDeviceTable[i]), 0x00, sizeof(zllTouchlink_RemoteDeviceTable_t));
      /* store the update: */
      NvSyncSave(&ZllTouchlinkConfigData(gZllRemoteDeviceTable), TRUE, TRUE);     
    }
  }
}

#endif /* gASL_EnableZllCommissioning_Initiator_d */
#endif /* gInterPanCommunicationEnabled_c */


/*!
 * @fn 		zbStatus_t ZllTouchlinkComissioning_Start(zllTouchlink_Start_t *pTouchlinkStart)     
 *
 * @brief	start Zll Touchlink procedure command
 *
 */
zbStatus_t ZllTouchlinkComissioning_Start(zllTouchlink_Start_t *pTouchlinkStart)      
{
#if gInterPanCommunicationEnabled_c    
#if gComboDeviceCapability_d
  /* verify factory new status */
  if(ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus) )
  {
    if(!(pTouchlinkStart->deviceType == gZdoStartMode_ComboMask_c || 
         pTouchlinkStart->deviceType == gZdoStartMode_Zc_c))
    {
      ZllTouchlinkConfigData(gTouchLinkSetup.deviceType) = (pTouchlinkStart->deviceType == gZdoStartMode_Zr_c)? gRouter_c:gEndDevice_c; 
      NlmeSetRequest(gDevType_c, ZllTouchlinkConfigData(gTouchLinkSetup.deviceType));
    }
  }
#endif
  
  /* verify functionality for combo devices */
  if(pTouchlinkStart->deviceType == gZdoStartMode_ComboMask_c || pTouchlinkStart->deviceType == gZdoStartMode_Zc_c)
  {
    /* for Combo device (Coordonator/Router) should avoid the start 
    touchlink commissioning if the device is not into a HA network */
    if((pTouchlinkStart->functionality & gZllTouchlink_StartCommissioning_c)&&
       (ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw) == FALSE))
    {
      return gZclFailure_c;
    }
  }
  
#if gASL_EnableZllCommissioning_Initiator_d
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.functionality) = pTouchlinkStart->functionality;
#endif 
  
  /* verify functionality */
  if(pTouchlinkStart->functionality & gZllTouchlink_FactoryFresh_c)
  {
    ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_InitialState_d;   
#if gASL_EnableEZCommissioning_d 
    if(ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw))
    {
      /* EZ mode factory fresh */
      EZComissioning_Start(gEzCommissioning_FactoryFresh_c);
    }
    else
    {
      BeeApp_FactoryFresh();
      /* leave the network */
      ZDO_Leave();
    }
#else  
    BeeApp_FactoryFresh();
    /* leave the network */
    ZDO_Leave();
#endif
    return gZclSuccess_c;
  } 
 
  if(!(pTouchlinkStart->functionality & gZllTouchlink_ClasicalCommissioning_c))
  {   
    ZllTouchlink_StartDiscovery(pTouchlinkStart->deviceRole);
  }
  else
  {
    zbChannels_t aChannelMask = {gaZllCommissioningPrimaryChannelsMask_c}; 
    
    if(!(pTouchlinkStart->deviceType == gZdoStartMode_ComboMask_c ||
         pTouchlinkStart->deviceType == gZdoStartMode_Zc_c))
    {
      uint16_t aTimeBetweenScans = 240;
      uint8_t scanAttempts = 1;
      
      NlmeSetRequest(gNwkTimeBtwnScans_c, aTimeBetweenScans);
      NlmeSetRequest(gNwkScanAttempts_c, scanAttempts);
      ZbBeeStackGlobalsParams(gBeeStackConfig.gNwkDiscoveryAttempts) = 0x01;  
          
      ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_ClassicalCommissioning_d;
#if gASL_EnableEZCommissioning_d      
      EZCommissioningConfigData(gEZCommissioningPrimaryDeviceType) = pTouchlinkStart->deviceType;
#endif      
    }
    else
    {
#if gASL_EnableEZCommissioning_d      
      if(pTouchlinkStart->deviceType == gZdoStartMode_ComboMask_c)
        EZCommissioningConfigData(gEZCommissioningPrimaryDeviceType) = gZdoStartMode_Zr_c;
      else
        EZCommissioningConfigData(gEZCommissioningPrimaryDeviceType) = gZdoStartMode_Zc_c;
#else
      if(pTouchlinkStart->deviceType == gZdoStartMode_ComboMask_c)
        pTouchlinkStart->deviceType = gZdoStartMode_Zr_c;
      else
        pTouchlinkStart->deviceType = gZdoStartMode_Zc_c;
#endif      
      FLib_MemCpy(aChannelMask,
              ApsmeGetRequest(gApsChannelMask_c),
              sizeof(ApsmeGetRequest(gApsChannelMask_c)));
      ZllTouchlinkConfigData(mTouchLinkSession.state) = gZllCommissioning_ClassicalCommissioningCombo_d;
    }
    /* verify factory new status */
    if(ZllTouchlinkConfigData(gTouchLinkSetup.factoryNewStatus) )
      ZllTouchlink_DefaultNonZllNetworkData(aChannelMask);
    
#if gASL_EnableEZCommissioning_d
    EZComissioning_Start(gEzCommissioning_NetworkSteering_c);
#else
    ZDO_Start(pTouchlinkStart->deviceType | gZdoStartMode_RamSet_c);  
#endif    
  }
  return gZclSuccess_c;
#else
  return gZclUnsupportedClusterCommand_c;
#endif    
}        
     
/*!
 * @fn 		zbStatus_t ZllTouchlinkComissioning_Configure(zllTouchlink_Configure_t *pTouchlinkConfigure)     
 *
 * @brief	configure Zll Touchlink procedure 
 *
 */
zbStatus_t ZllTouchlinkComissioning_Configure(zllTouchlink_Configure_t *pTouchlinkConfigure)      
{
#if gInterPanCommunicationEnabled_c    
  ZllTouchlinkConfigData(mTouchLinkSession.minRssi) = pTouchlinkConfigure->minRssi;
  ZllTouchlinkConfigData(mTouchLinkSession.identifyDuration) = pTouchlinkConfigure->idenfityDuration;
#if gASL_EnableZllCommissioning_Initiator_d
  ZllTouchlinkConfigData(mTouchLinkInitiatorSession.deviceConfigStartIndex) = pTouchlinkConfigure->deviceConfigStartIndex;
#endif 
  return gZclSuccess_c;
#else
  return gZclUnsupportedClusterCommand_c;
#endif  
}

/*!
 * @fn 		zbStatus_t ZllTouchlinkComissioning_GetListOfCommDevices(zllTouchlink_GetCommDevices_t *pReq, uint8_t *plistOfDevices)     
 *
 * @brief	Return the  list of commissioned Devices (pListOfDevices[0] = totalNoOfDevice, pListOfDevices[1] = count,  pListOfDevices[2....] = deviceInf 
 *
 */
zbStatus_t ZllTouchlinkComissioning_GetListOfCommDevices(zllTouchlink_GetCommDevices_t *pReq, uint8_t *plistOfDevices)
{

  *plistOfDevices = 0x00; 
#if gASL_EnableZllCommissioning_Initiator_d  && gASL_EnableZllTouchlinkCommissioning_d
  {
    uint8_t i;
    
    plistOfDevices[1] = 0x00;
    for(i=0; i< gZLLCommissioning_MaxRemoteDevices_c; i++)
    {     
      if((IsValidNwkUnicastAddr(ZllTouchlinkConfigData(gZllRemoteDeviceTable[i].ntwAddress))) &&
          (IsValidExtendedPanId(ZllTouchlinkConfigData(gZllRemoteDeviceTable[i].ieeeAddress))))
      {
        if((plistOfDevices[0] >=  pReq->startIndex) && (plistOfDevices[1] < pReq->count))
        {
          /* add entry in response */
          FLib_MemCpy(&plistOfDevices[2+plistOfDevices[1]*sizeof(zllTouchlink_RemoteDeviceTable_t)], &ZllTouchlinkConfigData(gZllRemoteDeviceTable[i]), sizeof(zllTouchlink_RemoteDeviceTable_t));
          plistOfDevices[1]++;
        }
        plistOfDevices[0]++;        
      }       
    }
    return gZclSuccess_c;
  }
#else
  return gZclUnsupportedClusterCommand_c;
#endif  
}

/*!
 * @fn 		zbStatus_t ZllTouchlinkComissioning_AddGroup(zbGroupId_t aGroupId)     
 *
 * @brief	Add a group in the touchlink commissioning table  
 *
 */
zbStatus_t ZllTouchlinkComissioning_AddGroup(zbGroupId_t aGroupId)
{
  
#if gASL_EnableZllCommissioning_Initiator_d   
  uint8_t i, index = 0xFF;
  uint16_t groupId;
  
  Copy2Bytes(&groupId, aGroupId);
  if(groupId < ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdBegin) ||
     groupId > ZllTouchlinkConfigData(gTouchLinkSetup.freeGroupIdEnd))
    return gZclFailure_c;
  
  /* verify groupId */
  for(i=0; i<gZLLCommissioning_MaxNoOfGroups_c; i++)
  {
    if(ZllTouchlinkConfigData(gTouchLinkSetup.groupTable[i].groupId) == groupId)
    {
      return gZclSuccess_c;
    }
    
    /* keep the first free entry */
    if((ZllTouchlinkConfigData(gTouchLinkSetup.groupTable[i].groupId) == 0xFFFF) && (index == 0xFF))
    {
      index = i;
    }
  }
  
  if(index != 0xFF)
  {
    ZllTouchlinkConfigData(gTouchLinkSetup.groupTable[index].groupId) = groupId;
    return gZclSuccess_c;
  }
  
  return gZclNoMem_c;
#else
  return gZclSuccess_c;
#endif  
}
/*!
 * @fn 		zbStatus_t ZllTouchlinkComissioning_RemoveGroup(zbGroupId_t aGroupId)     
 *
 * @brief	Remove a group in the touchlink commissioning table  
 *
 */
zbStatus_t ZllTouchlinkComissioning_RemoveGroup(zbGroupId_t aGroupId)
{
#if gASL_EnableZllCommissioning_Initiator_d  
  uint8_t i;
  uint16_t groupId;
  
  Copy2Bytes(&groupId, aGroupId);

  if(groupId != 0xFFFF)
  {
    /* verify groupId */
    for(i=0; i<gZLLCommissioning_MaxNoOfGroups_c; i++)
    {
      if(ZllTouchlinkConfigData(gTouchLinkSetup.groupTable[i].groupId) == groupId)
      {
        ZllTouchlinkConfigData(gTouchLinkSetup.groupTable[i].groupId) = 0xFFFF;
        break;
      }
    }
  }
  else
  {
    FLib_MemSet(&ZllTouchlinkConfigData(gTouchLinkSetup.groupTable[0]), 0xFF, sizeof(zllTouchlink_GroupTable_t)*gZLLCommissioning_MaxNoOfGroups_c);
  }
#endif
  return gZclSuccess_c;
}

/************************************************************************************************/
/*                         ZLL Commissioning Utility                                            */ 
/************************************************************************************************/

/*!
 * @fn 		zbStatus_t ZCL_ZllCommissioningUtilityClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t ZCL_ZllCommissioningUtilityClusterServer
(
   zbApsdeDataIndication_t *pIndication,
   afDeviceDef_t *pDevice
)
{
   zclFrame_t *pFrame;
   zbStatus_t status = gZclSuccessDefaultRsp_c;
   zbProfileId_t   haProfileId = {0x04, 0x01};
   
   /* to prevent compiler warning */
   (void)pDevice;
  
   /* check profile Id */
   if(!FLib_Cmp2Bytes(pIndication->aProfileId, haProfileId))
     return gZclMalformedCommand_c;
  
   /* get frame information */
   pFrame = (void *)pIndication->pAsdu;
     
   /* handle incoming server commands */
   switch (pFrame->command) 
   {
      case gZclCmdZllCommissioning_GetGroupIdReq_c:
        status = ZllCommissioningUtility_GetGroupIdReqHandler(pIndication);
        break;
      case gZclCmdZllCommissioning_GetEndpointListReq_c:  
        status = ZllCommissioningUtility_GetEndpointListReqHandler(pIndication);
        break;
      default:
        status = gZclUnsupportedClusterCommand_c;
        break;
   }
   if((pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) && 
      (status == gZclSuccess_c || status == gZclSuccessDefaultRsp_c))
      status = gZclSuccess_c;
   
   return status;
}

/*!
 * @fn 		zbStatus_t ZCL_ZllCommissioningUtilityClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Zll commissioning Cluster Client. 
 *
 */
zbStatus_t ZCL_ZllCommissioningUtilityClusterClient
(
   zbApsdeDataIndication_t *pIndication,
   afDeviceDef_t *pDevice
)
{
   zclFrame_t *pFrame;
   zbStatus_t status = gZclSuccessDefaultRsp_c;
   zbProfileId_t   haProfileId = {0x04, 0x01};
   
   /* to prevent compiler warning */
   (void)pDevice;
  
   /* check profile Id */
   if(!FLib_Cmp2Bytes(pIndication->aProfileId, haProfileId))
     return gZclMalformedCommand_c;
  
   /* get frame information */
   pFrame = (void *)pIndication->pAsdu;
      
   /* handle incoming client commands */
   switch (pFrame->command) 
   {
      case gZclCmdZllCommissioning_EndpointInformation_c:
      case gZclCmdZllCommissioning_GetGroupIdRsp_c:
        break;
      case gZclCmdZllCommissioning_GetEndpointListRsp_c:   
        break;
      default:
        status = gZclUnsupportedClusterCommand_c;
        break;
   }
   
   if((pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) && 
      (status == gZclSuccess_c || status == gZclSuccessDefaultRsp_c))
      status = gZclSuccess_c;
   
   return status;
} 

/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_GetGroupIdReq(zllCommissioning_GetGroupIdReq_t *pReq) 
 *
 * @brief	Sends over-the-air a GetGroupId request from the Zll Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllCommissioningUtility_GetGroupIdReq
( 
  zllCommissioning_GetGroupIdReq_t *pReq
) 
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterZllCommissioning_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdZllCommissioning_GetGroupIdReq_c, sizeof(zllCmdCommissioning_GetGroupIdReq_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_GetEndpointListReq(zllCommissioning_GetEndpointListReq_t *pReq) 
 *
 * @brief	Sends over-the-air a GetEndpointList request from the Zll Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllCommissioningUtility_GetEndpointListReq
( 
  zllCommissioning_GetEndpointListReq_t *pReq
) 
{
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterZllCommissioning_c);	
  return ZCL_SendClientReqSeqPassed(gZclCmdZllCommissioning_GetEndpointListReq_c,sizeof(zllCmdCommissioning_GetEndpointListReq_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_EndpointInformation(zllCommissioning_EndpointInformation_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air an Endpoint Information command from the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllCommissioningUtility_EndpointInformation
( 
  zllCommissioning_EndpointInformation_t *pCommandRsp
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = sizeof(zllCmdCommissioning_EndpointInformation_t);
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          gZclCmdZllCommissioning_EndpointInformation_c,
	                  gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp, 
	                  &pCommandRsp->zclTransactionId, 
	                  &iPayloadLen,
	                  (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
	 
  return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}


/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_SendEndpointInformation(zllCommissioning_SendEndpointInf_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air an Endpoint Information command from the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllCommissioningUtility_SendEndpointInformation
( 
  zllCommissioning_SendEndpointInf_t *pCommandRsp
) 
{
  zllCommissioning_EndpointInformation_t *pRsp;
  zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
  zbStatus_t status;
  uint8_t i;
  
  pRsp = AF_MsgAlloc();
  if(!pRsp)
    return gZclNoMem_c; 
  
  FLib_MemCpy(&pRsp->addrInfo, &pCommandRsp->addrInfo, sizeof(afAddrInfo_t));
  pRsp->zclTransactionId = pCommandRsp->zclTransactionId;
  
  
  pRsp->cmdFrame.endpointId = pCommandRsp->addrInfo.srcEndPoint;
  
  /* complete the command */
#if gInstantiableStackEnabled_d 
  for(i=0; i<EndPointConfigData(gNum_EndPoints); i++)
#else
  for(i=0; i<gNum_EndPoints_c; i++)
#endif    
  {
    if(EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint) ==  pRsp->cmdFrame.endpointId)
    {
      if(ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw))
        Copy2Bytes(pRsp->cmdFrame.profileId, EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->aAppProfId));
      else
        Copy2Bytes(pRsp->cmdFrame.profileId, zllProfileId);
      Copy2Bytes(pRsp->cmdFrame.deviceId, EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->aAppDeviceId));
      pRsp->cmdFrame.version = EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->appDevVerAndFlag);
      Copy2Bytes(pRsp->cmdFrame.nwkAddress,  NlmeGetRequest(gNwkShortAddress_c));
      Copy8Bytes(pRsp->cmdFrame.ieeeAddress, NlmeGetRequest(gNwkIeeeAddress_c));
      break;
    }
    else
    {
#if gInstantiableStackEnabled_d 
      if( i== EndPointConfigData(gNum_EndPoints)-1)
#else
      if( i== gNum_EndPoints_c - 1)
#endif        
      {
          MSG_Free(pRsp);
          return gZclFailure_c;
      }
    }
  }
  
  status = zclZllCommissioningUtility_EndpointInformation(pRsp);
  MSG_Free(pRsp);
  
  return status;
}

/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_GetGroupIdRsp(zllCommissioning_GetGroupIdRsp_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air a GetGroupId response from the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllCommissioningUtility_GetGroupIdRsp
( 
  zllCommissioning_GetGroupIdRsp_t *pCommandRsp
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = sizeof(zllCmdCommissioning_GetGroupIdRsp_t)+ (pCommandRsp->cmdFrame.count-1)*sizeof(zllCommissioning_GroupInfRecord_t);
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          gZclCmdZllCommissioning_GetGroupIdRsp_c,
	                  gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
	                  &pCommandRsp->zclTransactionId, 
	                  &iPayloadLen,
	                  (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
	 
  return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_GetEndpointListRsp(zllCommissioning_GetEndpointListRsp_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air a GetEndpointList response from the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllCommissioningUtility_GetEndpointListRsp
( 
  zllCommissioning_GetEndpointListRsp_t *pCommandRsp
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = sizeof(zllCmdCommissioning_GetEndpointListRsp_t)+ (pCommandRsp->cmdFrame.count-1)*sizeof(zllCommissioning_EndpointInfRecord_t);
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          gZclCmdZllCommissioning_GetEndpointListRsp_c,
	                  gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
	                  &pCommandRsp->zclTransactionId, 
	                  &iPayloadLen,
	                  (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
	 
  return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t ZllCommissioningUtility_GetEndpointListReqHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle the GetEndpointListReq from the Zll Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllCommissioningUtility_GetEndpointListReqHandler(zbApsdeDataIndication_t *pIndication)
{
  zclFrame_t *pFrame;
  zllCommissioning_GetEndpointListRsp_t *pRsp;
  zllCmdCommissioning_GetEndpointListReq_t *pReqPayload;
  zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
  zbStatus_t status;
  uint8_t i;
  
  pRsp = AF_MsgAlloc();
  
  if(!pRsp)
    return gZclNoMem_c;
  
  /* get frame information */
  pFrame = (void *)pIndication->pAsdu;
  pReqPayload = (void *)(pFrame+1);
 
  /* Create the destination address */
  AF_PrepareForReply(&pRsp->addrInfo, pIndication); 
  pRsp->zclTransactionId = pFrame->transactionId;
#if gInstantiableStackEnabled_d 
  pRsp->cmdFrame.total = EndPointConfigData(gNum_EndPoints);
#else
  pRsp->cmdFrame.total = gNum_EndPoints_c;
#endif   
  pRsp->cmdFrame.startIndex = pReqPayload->startIndex;
  if(pReqPayload->startIndex >=  pRsp->cmdFrame.total)
    pRsp->cmdFrame.count = 0;
  else
  {
    pRsp->cmdFrame.count = pRsp->cmdFrame.total - pReqPayload->startIndex;
    for(i=pReqPayload->startIndex; i<pRsp->cmdFrame.total; i++)
    {
      pRsp->cmdFrame.endpointInfRecord[i-pReqPayload->startIndex].endpointId = EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint);
      pRsp->cmdFrame.endpointInfRecord[i-pReqPayload->startIndex].version = EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->appDevVerAndFlag);
      Copy2Bytes(pRsp->cmdFrame.endpointInfRecord[i-pReqPayload->startIndex].nwkAddress,  NlmeGetRequest(gNwkShortAddress_c));
      if(ZllTouchlinkConfigData(gZllCommissioningDeviceInNonZllNtw))
        Copy2Bytes(pRsp->cmdFrame.endpointInfRecord[i-pReqPayload->startIndex].profileId, EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->aAppProfId));
      else
        Copy2Bytes(pRsp->cmdFrame.endpointInfRecord[i-pReqPayload->startIndex].profileId, zllProfileId);
      Copy2Bytes(pRsp->cmdFrame.endpointInfRecord[i-pReqPayload->startIndex].deviceId, EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->aAppDeviceId));
    }
  }
  status = zclZllCommissioningUtility_GetEndpointListRsp(pRsp);
  MSG_Free(pRsp);
  
  return status;
}

 /*!
 * @fn 		zbStatus_t ZllCommissioningUtility_GetGroupIdReqHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle the GetGroupIdReq from the Zll Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllCommissioningUtility_GetGroupIdReqHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclUnsupportedClusterCommand_c;
#if gASL_EnableZllCommissioning_Initiator_d 
  zclFrame_t *pFrame;
  zllCommissioning_GetGroupIdRsp_t *pRsp;
  zllCmdCommissioning_GetGroupIdReq_t *pReqPayload;
  uint8_t i;
  
  pRsp = AF_MsgAlloc();
  
  if(!pRsp)
    return gZclNoMem_c;
  
  /* get frame information */
  pFrame = (void *)pIndication->pAsdu;
  pReqPayload = (zllCmdCommissioning_GetGroupIdReq_t *)(pFrame+1);
 
  /* Create the destination address */
  AF_PrepareForReply(&pRsp->addrInfo, pIndication); 
  pRsp->zclTransactionId = pFrame->transactionId;
  
  /* complete the command */
  pRsp->cmdFrame.startIndex = pReqPayload->startIndex;
  pRsp->cmdFrame.count = 0x00;
  pRsp->cmdFrame.total = 0x00;
  for(i=0; i<gZLLCommissioning_MaxNoOfGroups_c; i++)
  {
    if(ZllTouchlinkConfigData(gTouchLinkSetup.groupTable[i].groupId) != 0xFFFF)
    {
      pRsp->cmdFrame.total++;
      if(pRsp->cmdFrame.total >= pReqPayload->startIndex)
      {
        pRsp->cmdFrame.groupInfRecord[pRsp->cmdFrame.count].groupId = ZllTouchlinkConfigData(gTouchLinkSetup.groupTable[i].groupId);
        pRsp->cmdFrame.groupInfRecord[pRsp->cmdFrame.count].groupType = 0x00; /* ZLL Spec 7.1.2.3.7.4 */
        pRsp->cmdFrame.count++;
      }
    }
  }
  
  status = zclZllCommissioningUtility_GetGroupIdRsp(pRsp);
  MSG_Free(pRsp);
#endif  
  return status;
}

#if(gInstantiableStackEnabled_d == 1)
 /*!
 * @fn 		void ZllTouchlinkInitVariables(uint8_t instId) 
 * 
 * @brief	Init touchlink variables based on Network/Mac Instance Id
 *
 */
void ZllTouchlinkInitVariables(uint8_t instId)
{
  pZllTouchlinkData->gZllCommissioningDeviceInNonZllNtw = FALSE;
  if(instId == 0x00)  
    pZllTouchlinkData->gZllCommissioningDefaultDeviceType = gZllCommissioningDefaultDeviceType_d;
  else
    pZllTouchlinkData->gZllCommissioningDefaultDeviceType = gZllCommissioningDefaultDeviceTypePan1_d;
#if gInterPanCommunicationEnabled_c  
#if gASL_EnableZllCommissioning_Initiator_d  
  pZllTouchlinkData->mZllExtendedScanReq = FALSE;
#endif   
  pZllTouchlinkData->mTouchLinkScanReqId = 0x00;   
  pZllTouchlinkData->mTouchLinkScanRspId  = 0x00;
#if gComboDeviceCapability_d || gEndDevCapability_d 
  pZllTouchlinkData->gZllOrgPollRate = 0x00;
#if gASL_EnableZllCommissioning_Initiator_d
  pZllTouchlinkData->gEndDevForcedTouchlink = FALSE;
#endif  
#endif 
#endif /* gInterPanCommunicationEnabled_c */

/* initiator remote device data */
#if gASL_EnableZllCommissioning_Initiator_d
  pZllTouchlinkData->mpTouchLinkTempRemoteTarget = NULL;
#endif /* gASL_EnableZllCommissioning_Initiator_d */
}
 /*!
 * @fn 		void ZllTouchlinkContextSwitch(uint8_t instId)
 * 
 * @brief	Switch btw Network/Mac Instance Ids
 *
 */
void ZllTouchlinkContextSwitch(uint8_t instId)
{
  pZllTouchlinkData = &ZllTouchlinkData[instId];
}     
#endif  
