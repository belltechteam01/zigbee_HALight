/*! @file 	  ZclSE.c
 *
 * @brief	  This source file describes general functionality implemented
 *			  for in the ZigBee Smart Energy Profile.
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
 *			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   		contributors may be used to endorse or promote products derived from this
 *   		software without specific prior written permission.
 *
 *			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZdoApsInterface.h"
#include "HaProfile.h"
#include "ZCLGeneral.h"
#include "zcl.h"
#include "SEProfile.h"
#include "zclSE.h"
#include "display.h"
#include "eccapi.h"
#include "Led.h"
#include "ZdoApsInterface.h"
#include "ASL_ZdpInterface.h"
#include "ApsMgmtInterface.h"
#include "beeapp.h"
#include "zclSEPrice.h"
#include "zclSEMessaging.h"
#include "zclOTA.h"
#include "zclSEDRLC.h"
#include "zclSEKeyEstab.h"
#include "zclSEMetering.h"
#include "zclSECalendar.h"
#include "zclSEMDUPairing.h"
#include "EndpointConfig.h"
#include "ZdoNwkManager.h"
#include "BeeAppInit.h"
#include "ZclFoundation.h"
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
int ECC_GetRandomDataFunc(unsigned char *buffer, unsigned long sz);
int ECC_HashFunc(unsigned char *digest, unsigned long sz, unsigned char *data);

zbTmrTimerID_t gAutoJoinTimerID;
uint8_t mJoinAttempts;

#if !gCoordinatorCapability_d && gZclSeEnableAutoJoining_d
static void AutoJoinScanCallBack(zbTmrTimerID_t tmrid);
#endif

zbTmrTimerID_t gTimeSyncTimerID;
static void TimeUpdateCallBack(zbTmrTimerID_t tmrid);
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

#define gKeyEstabMaxRetries_c 3
#define mNoOfBoundClusters_c  4
  /* ZCL Time Cluster starts at 01 Jan 2000 00:00:00 GMT, hence -946702800 offset to unix Epox 01 Jan 1970 00:00:00 GMT */
#define mTimeClusterTimeEpoxOffset_c 946702800
  
  /* 28 August 2008 12:00:00 GMT */
#define mDefaultValueOfTimeClusterTime_c 1375444800 - mTimeClusterTimeEpoxOffset_c
  
  /* Time Zone GMT+1 - Aarhus, Denmark */
#define mDefaultValueOfTimeClusterTimeZone_c 3600
  /* Start daylight savings 2008 for Aarhus, Denmark 30 March 2008 */
#define mDefaultValueOfTimeClusterDstStart_c 1206849600 - mTimeClusterTimeEpoxOffset_c
  /* End daylight savings 2008 for Aarhus, Denmark 26 October 2008 */
#define mDefaultValueOfTimeClusterDstEnd_c 1224993600 - mTimeClusterTimeEpoxOffset_c
  /* Daylight Saving + 1h */
#define mDefaultValueOfClusterDstShift_c 3600
  /* Valid Until Time - mDefaultValueOfTimeClusterTime_c + aprox 5 years */
#define mDefaultValueOfTimeClusterValidUntilTime_c mDefaultValueOfTimeClusterTime_c + 31536000

#define mTsSETaskPriority_c (gTsAppTaskPriority_c + 1)

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/
#if (gStandardSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) 
ESPRegTable_t RegistrationTable[RegistrationTableEntries_c] = {0};
#endif

uint8_t gZclSENoOfFailedKeepAlive = 0;

/* Federated Trust Center EndPoint*/
zbEndPoint_t gFTCEndPoint;

const zbNwkAddr_t gTrustCenterAddr = {0x00,0x00};
seDeviceDescriptor_t gZclSeDeviceList[gApsMaxAddrMapEntries_c];

ESPRegisterDevFunc *pSE_ESPRegFunc = NULL;
ESPDeRegisterDevFunc *pSE_ESPDeRegFunc = NULL;
/******************************************************************************
*******************************************************************************
* Private memory definitions
*******************************************************************************
******************************************************************************/
index_t mCurrentBoundClusterIdx;
uint8_t mNoBoundClusters;
index_t mCurrentESIIdx;
index_t mAuthorativeTimeServerIdx = 0; // ZC is by default
zbClusterId_t maBoundClusterList[mNoOfBoundClusters_c];
const uint8_t maKeyEstabClusterList[2] = {gaKeyEstabCluster_c};
const uint8_t maMDUPairingClusterList[2] = {gaZclClusterMDUPairing_c};
const uint8_t maOtherClusterList[8] = {gaZclClusterPrice_c, gaZclClusterDmndRspLdCtrl_c, gaZclClusterMsg_c, gaZclClusterTouCalendar_c};

static uint32_t mSEClusterEvt = 0;
#if gZclSE_MDUPairing_d 
static uint8_t mCurrentHANDevIdx;
#endif
uint8_t gKeyEstabMaxRetries; 

#if gInterPanCommunicationEnabled_c
/*  Handle the InterPan messages  */
pfnInterPanIndication_t pfnInterPanServerInd = NULL;
pfnInterPanIndication_t pfnInterPanClientInd = NULL;
#endif

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
tsTaskID_t gSETaskID;
zclSEState_t gZclSEState;
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		void SE_Init(void)
 *
 * @brief	Initialize the SE task and functionality
 *
 */
void SE_Init(void)
{
  /* Create the Smart Energy task */
  gSETaskID = TS_CreateTask(mTsSETaskPriority_c, TS_SETask);
 
  {
    ZCLTimeConf_t defaultTimeConf;
    defaultTimeConf.Time = mDefaultValueOfTimeClusterTime_c;
    defaultTimeConf.TimeStatus = zclTimeStatusMaster;
    defaultTimeConf.TimeZone = mDefaultValueOfTimeClusterTimeZone_c;
    defaultTimeConf.DstStart = mDefaultValueOfTimeClusterDstStart_c;
    defaultTimeConf.DstEnd = mDefaultValueOfTimeClusterDstEnd_c;
    defaultTimeConf.DstShift = mDefaultValueOfClusterDstShift_c;
    defaultTimeConf.ValidUntilTime = mDefaultValueOfTimeClusterValidUntilTime_c;
    
    ZCL_TimeInit(BeeAppDataInit(appEndPoint),&defaultTimeConf);
    
    /* Allocate timer for time server synchronization*/
    gTimeSyncTimerID = ZbTMR_AllocateTimer();
    
    /* Allocate timer for the PAN Auto-Join state */
#if !gCoordinatorCapability_d && gZclSeEnableAutoJoining_d
    mJoinAttempts = 0;
    gAutoJoinTimerID = ZbTMR_AllocateTimer();    
#endif    
  }
  
#if gFragmentationCapability_d 
  {
    uint8_t aIncomingTransferSize[2] ={ gSEMaxIncomingTransferSize };
    FLib_MemCpy(&ZbBeeStackGlobalsParams(gBeeStackConfig.maxTransferSize[0]), &aIncomingTransferSize[0], 2);
    FLib_MemCpy(&ZbBeeStackGlobalsParams(gBeeStackConfig.aMaxOutTransferSize[0]), &aIncomingTransferSize[0], 2);
    ApsmeSetRequest(gApsMaxWindowSize_c, gSEMaxWindowSize_c);        // window can be 1 - 8
    ApsmeSetRequest(gApsInterframeDelay_c, gSEInterframeDelay_c);    // interframe delay can be 1 - 255ms
    ApsmeSetRequest(gApsMaxFragmentLength_c, gSEMaxFragmentLength_c);  // max len can be 1 - ApsmeGetMaxAsduLength()
  }
#endif /* #if gFragmentationCapability_d  */  
  
}


/*!
 * @fn 		static void BindClusterOnESI(void)
 *
 * @brief	 Binds the Smart Energy clusters that have unsolicited messages on the server
 * 			(ESI).
 *
 */
static void BindClusterOnESI(void)
{
  zbBindUnbindRequest_t  bindRequest;
  zbAddressMap_t addrMap;
  
  if (gZclSeESIList[mCurrentESIIdx].EntryStatus != gEntryUsed_c)
    return;
  
  (void)AddrMap_SearchTableEntry(NULL, (zbNwkAddr_t*)gZclSeESIList[mCurrentESIIdx].NwkAddr, &addrMap);
  Copy2Bytes(bindRequest.aClusterId, &maBoundClusterList[mCurrentBoundClusterIdx]);
  Copy8Bytes(bindRequest.aSrcAddress, addrMap.aIeeeAddr);
  bindRequest.srcEndPoint = gZclSeESIList[mCurrentESIIdx].EndPoint;
  bindRequest.addressMode = gZbAddrMode64Bit_c;
  Copy8Bytes(bindRequest.destData.extendedMode.aDstAddress, NlmeGetRequest(gNwkIeeeAddress_c));
  bindRequest.destData.extendedMode.dstEndPoint = BeeAppDataInit(appEndPoint);
  
  APP_ZDP_BindUnbindRequest(NULL, (uint8_t*)gZclSeESIList[mCurrentESIIdx].NwkAddr, gBind_req_c, &bindRequest);
}

/*!
 * @fn 		static void ZCL_RequestTimeFromServer(void)
 *
 * @brief	Sends a read attribute command to Time server in order to read the time.
 *
 */
static void ZCL_RequestTimeFromServer(void)
{
  zclReadAttrReq_t *pReq;

  pReq = MSG_Alloc(sizeof(zclReadAttrReq_t) + 4 * sizeof(zclAttrId_t));
  
  if(!pReq)
  {
    return;
  }
  
  /* Direct Address for the ESI*/
  pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c; 
  Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, gZclSeESIList[mAuthorativeTimeServerIdx].NwkAddr);
  pReq->addrInfo.dstEndPoint = gZclSeESIList[mAuthorativeTimeServerIdx].EndPoint;
  
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterTime_c);

  pReq->addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  /*Set security options to aps security if link keys are enabled*/
  pReq->addrInfo.txOptions = afTxOptionsDefault_c | gApsTxOptionAckTx_c;
  pReq->addrInfo.radiusCounter = afDefaultRadius_c;
  /* Request Time, TimeZone and DST Start time attributes */
  /* Request DST End time, DST Shift time attributes*/
  pReq->count = 5;
  pReq->cmdFrame.aAttr[0]= gZclAttrTime_c;  
  pReq->cmdFrame.aAttr[1]= gZclAttrTimeZone_c;  
  pReq->cmdFrame.aAttr[2]= gZclAttrDstStart_c;      
  pReq->cmdFrame.aAttr[3]= gZclAttrDstEnd_c;  
  pReq->cmdFrame.aAttr[4]= gZclAttrDstShift_c;  
  (void)ZCL_ReadAttrReq(pReq);
  MSG_Free(pReq);
}

/*!
 * @fn 		static void TimeUpdateCallBack(zbTmrTimerID_t tmrid)
 *
 * @brief	Callback for triggering the Time update procedure.
 *
 */
static void TimeUpdateCallBack(zbTmrTimerID_t tmrid)
{
  /* make the request */
  if (tmrid != 0)
  {
    gZclSENoOfFailedKeepAlive += 1;
    ZCL_RequestTimeFromServer();
  }
  /*start timer for next request*/
  ZbTMR_StartTimer(gTimeSyncTimerID, gTmrLowPowerMinuteTimer_c, ZbTmrSeconds(gTimeSyncInterval_c), TimeUpdateCallBack);
}

#if !gCoordinatorCapability_d && gZclSeEnableAutoJoining_d
/*!
 * @fn 		static void AutoJoinScanCallBack(zbTmrTimerID_t tmrid)
 *
 * @brief	Callback for triggering the Auto-Join scan procedure.
 *
 */
static void AutoJoinScanCallBack(zbTmrTimerID_t tmrid)
{
  TS_SendEvent(gSETaskID, gZclEvtSEPeriodicScan_c);
}
#endif

/*!
 * @fn 		static void QueryTimeServerOnESI(void)
 *
 * @brief	Sends a Read Attribute command on the time server to get
 *			the Time Status attribute.
 *
 */
static void QueryTimeServerOnESI(void)
{
  zclReadAttrReq_t pReq;

  pReq.addrInfo.dstAddrMode = gZbAddrMode16Bit_c; 
  Copy2Bytes(pReq.addrInfo.dstAddr.aNwkAddr, gZclSeESIList[mCurrentESIIdx].NwkAddr);
  pReq.addrInfo.dstEndPoint = gZclSeESIList[mCurrentESIIdx].EndPoint;
  Set2Bytes(pReq.addrInfo.aClusterId, gZclClusterTime_c);
  pReq.addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  pReq.addrInfo.txOptions = afTxOptionsDefault_c;
  pReq.addrInfo.radiusCounter = afDefaultRadius_c;
 
  /* Request TimeStatus attribute */
  pReq.count = 1;
  pReq.cmdFrame.aAttr[0]= gZclAttrTimeStatus_c;  
  (void)ZCL_ReadAttrReq(&pReq);
}

/*!
 * @fn 		bool_t ZCL_CheckTimeServer(zbApsdeDataIndication_t *pIndication, uint8_t timeStatus)
 *
 * @brief	Checks whether the remote node is the authorative Time Server.
 *
 */
bool_t ZCL_CheckTimeServer(zbApsdeDataIndication_t *pIndication, uint8_t timeStatus)
{
  bool_t bIsMoreAuthorative = FALSE;
 
  /* A Time server with neither the Master nor Synchronized bits set should not be
    chosen as the network time server. */
  if (((timeStatus & zclTimeStatusSynchronized ) != zclTimeStatusSynchronized) &&
    ((timeStatus & zclTimeStatusMaster ) != zclTimeStatusMaster))
    return FALSE;
  
  if (gZclSeESIList[mAuthorativeTimeServerIdx].TimeStatus == 0x00)
    bIsMoreAuthorative = TRUE;
 
  if (((timeStatus & zclTimeStatusSuperseeded ) == zclTimeStatusSuperseeded) &&
   ((gZclSeESIList[mAuthorativeTimeServerIdx].TimeStatus & zclTimeStatusSuperseeded ) != zclTimeStatusSuperseeded))
     bIsMoreAuthorative = TRUE;
  
  if (((timeStatus & zclTimeStatusMaster ) == zclTimeStatusMaster) && 
    ((gZclSeESIList[mAuthorativeTimeServerIdx].TimeStatus & zclTimeStatusMaster ) != zclTimeStatusMaster))
      bIsMoreAuthorative = TRUE;
  
  if (((timeStatus & (zclTimeStatusSuperseeded | zclTimeStatusMaster) ) == 
      (gZclSeESIList[mAuthorativeTimeServerIdx].TimeStatus & (zclTimeStatusSuperseeded | zclTimeStatusMaster) )) &&
      (TwoBytesToUint16(gZclSeESIList[mAuthorativeTimeServerIdx].NwkAddr) > TwoBytesToUint16(pIndication->aSrcAddr)))
        bIsMoreAuthorative = TRUE;

  /* Update the Time Server */
  if (bIsMoreAuthorative)
  {
    index_t i;
    
    for (i = 0; i < gMaxNoOfESISupported_c; i++)
      if ((FLib_MemCmp(&gZclSeESIList[i].NwkAddr, &pIndication->aSrcAddr, sizeof(zbNwkAddr_t))) &&
           (gZclSeESIList[i].EntryStatus != gEntryNotUsed_c))
      {
        gZclSeESIList[i].TimeStatus = timeStatus;
        mAuthorativeTimeServerIdx = i;
        break;
      }
  }
  
  return bIsMoreAuthorative;
}

/*!
 * @fn 		void ServiceDiscoveryForKeyEstab(void)
 *
 * @brief	Initiates Service Discovery for the Key Establishment server.
 *
 */
#if gNum_EndPoints_c != 0
void ServiceDiscoveryForKeyEstab(void)
{
  zbSimpleDescriptor_t keyEstabSimpleDesc;
  
  /* Create the Key Estab Simple Descriptor based on the endpoint descriptor*/
  FLib_MemCpy(&keyEstabSimpleDesc, EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc), sizeof(zbSimpleDescriptor_t));
  keyEstabSimpleDesc.pAppInClusterList = (uint8_t* )maKeyEstabClusterList;  
  keyEstabSimpleDesc.appNumInClusters = 1;
  keyEstabSimpleDesc.pAppOutClusterList = NULL;
  keyEstabSimpleDesc.appNumOutClusters = 0;
  ASL_MatchDescriptor_req(NULL, (uint8_t *)gTrustCenterAddr, &keyEstabSimpleDesc);
}
#endif

/*!
 * @fn 		zbStatus_t SendPairRequest(void)
 *
 * @brief	Sends a Pair Request to the Federated Trust Center.
 *
 */
zbStatus_t SendPairRequest(void)
{
  zclMDUPairing_PairingReq_t *pReq;
  
  pReq = MSG_Alloc(sizeof(zclMDUPairing_PairingReq_t));
  
  if (!pReq)
    return gZclFailure_c;
  
  /* Fill in address info */
  pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
  Set2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, 0);
  pReq->addrInfo.dstEndPoint = gFTCEndPoint;
  Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterMDUPairing_c);
  pReq->addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  pReq->addrInfo.txOptions = afTxOptionsDefault_c;
  pReq->addrInfo.radiusCounter = afDefaultRadius_c;
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  
  /* Set command payload */
  pReq->cmdFrame.pairInfoVersion = 0;
  Copy8Bytes(pReq->cmdFrame.aExtAddress, NlmeGetRequest(gNwkIeeeAddress_c));

  return zclMDUPairing_PairingReq(pReq);
}

/*!
 * @fn 		static void DiscoverOtherServices(zbNwkAddr_t aDstAddr)
 *
 * @brief	Initiates service discovery for the Price, DRLC and Messaging server cluster.
 *
 */
static void DiscoverOtherServices(zbNwkAddr_t aDstAddr)
{
#if gNum_EndPoints_c != 0
  zbSimpleDescriptor_t simpleDesc;

  FLib_MemCpy(&simpleDesc, EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc), sizeof(zbSimpleDescriptor_t));
  simpleDesc.pAppInClusterList = (uint8_t* )maOtherClusterList;  
  simpleDesc.appNumInClusters = mNoOfBoundClusters_c;
  simpleDesc.pAppOutClusterList = NULL;
  simpleDesc.appNumOutClusters = 0;
  ASL_MatchDescriptor_req(NULL, aDstAddr, &simpleDesc);
#endif
}

#if gZclSE_MDUPairing_d 
/*!
 * @fn 		static void DiscoverOtherServicesOnVHAN()
 *
 * @brief	Initiates service discovery for the Price, DRLC and Messaging server cluster
 * 			on the Virtual HAN.
 *
 */
static void DiscoverOtherServicesOnVHAN()
{
  uint8_t *pNwkAddr, aNwkAddr[2];
  zclMDUPairing_MDUEntry_t* pDevices = zclMDUPairing_GetVHANDevices();
  
  /* Skip if it's our own address*/
  if (Cmp8Bytes(pDevices->aDeviceList[mCurrentHANDevIdx], NlmeGetRequest(gNwkIeeeAddress_c)) &&
      (mCurrentHANDevIdx < pDevices->devCount - 1))
    mCurrentHANDevIdx++;
  
  /* Find the nwk 16 bit address */
  pNwkAddr = APS_GetNwkAddress(pDevices->aDeviceList[mCurrentHANDevIdx], aNwkAddr);
  
  if (pNwkAddr)
  {
    DiscoverOtherServices(pNwkAddr);
    TS_SendEvent(gSETaskID, gZclEvtQueryNextPairing_c);
  }
  else
  {
    ASL_NWK_addr_req(NULL, (uint8_t *)gaBroadcastRxOnIdle, 
                    pDevices->aDeviceList[mCurrentHANDevIdx], 0x00, 0x00);
    /* Retry the discovery */
    mCurrentHANDevIdx--;
  }
}
#endif

/*!
 * @fn 		void DiscoverMDUPairingService(void)
 *
 * @brief	Initiates service discovery for the MDU Pairing server cluster.
 *
 */
void DiscoverMDUPairingService(void)
{
#if gNum_EndPoints_c != 0
  zbSimpleDescriptor_t simpleDesc;

  FLib_MemCpy(&simpleDesc, EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc), sizeof(zbSimpleDescriptor_t));
  simpleDesc.pAppInClusterList = (uint8_t* )maMDUPairingClusterList;  
  simpleDesc.appNumInClusters = 1;
  simpleDesc.pAppOutClusterList = NULL;
  simpleDesc.appNumOutClusters = 0;
  ASL_MatchDescriptor_req(NULL, (uint8_t *)gTrustCenterAddr, &simpleDesc);
#endif
}

/*!
 * @fn 		bool_t DeviceHasCBKEKey(void)
 *
 * @brief	Checks whether the node had previously established a key through CBKE
 * 			with the Trust Center.
 *
 */
bool_t DeviceHasCBKEKey(void)
{
  zbApsDeviceKeyPairSet_t whereToCpyEntry;
  zbApsDeviceKeyPairSet_t  *pSecurityMaterial;
  
  if (!gZclSE_KeyEstablished)
    return FALSE;

  pSecurityMaterial = APS_GetSecurityMaterilaEntry(ApsmeGetRequest(gApsTrustCenterAddress_c), &whereToCpyEntry);
  
  if (!pSecurityMaterial)
    return FALSE;
  
  /* The Preconfigured Link Key is the first in the Key Table */
  return (pSecurityMaterial->iKey != 0x00);
}

/*!
 * @fn 		static void SetClustersForBinding(void)
 *
 * @brief	Sets the clusters that will need binding to a server.
 *
 */
static void SetClustersForBinding(void)
{
#if gNum_EndPoints_c != 0
  zbClusterId_t aClusterList[mNoOfBoundClusters_c] = {gaZclClusterPrice_c, gaZclClusterDmndRspLdCtrl_c, gaZclClusterMsg_c, gaZclClusterTouCalendar_c};
  uint8_t  noOutputClusters = EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->appNumOutClusters);
  zbClusterId_t *pOutputClusterList = (zbClusterId_t *)EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->pAppOutClusterList);
  uint8_t i, j;
  
  mNoBoundClusters = 0;
  
  for (i = 0; i < noOutputClusters; i++)
  {
    for (j = 0; j < mNoOfBoundClusters_c; j++)
    {
      if (IsEqual2Bytes(pOutputClusterList[i], aClusterList[j]))
      {
        Copy2Bytes(maBoundClusterList[mNoBoundClusters], aClusterList[j]);
        mNoBoundClusters++;
      }
    }
  }
#endif
}

/*!
 * @fn 		void TS_SETask(uint16_t events)
 *
 * @brief	Task for the Smart Energy Profile. Used to signal ourselves of events.
 *
 */
void TS_SETask
(
  uint16_t events  /* IN: the ID to shoose from which to which sap handler the message should flow */
)
{
  switch(SE_GetState())
  {

    /* 5.5.5.1 PAN Auto-Joining State */
#if !gCoordinatorCapability_d && gZclSeEnableAutoJoining_d  
    case gZclSE_AutoJoiningState_c:
    
      if (events & gZclEvtSEPeriodicScan_c)
      {
        uint8_t randomTime;
        if (++mJoinAttempts < 15)
        {
          randomTime = (uint8_t)GetRandomRange(45, 75);
          ZbTMR_StartTimer(gAutoJoinTimerID, gTmrLowPowerSecondTimer_c, ZbTmrSeconds(randomTime), AutoJoinScanCallBack);
        }
        else
        {
          randomTime = (uint8_t)GetRandomRange(30, 90);
          ZbTMR_StartTimer(gAutoJoinTimerID, gTmrLowPowerSecondTimer_c, ZbTmrMinutes(randomTime), AutoJoinScanCallBack);
        }
        ZDO_Start(gStartWithOutNvm_c);
      }
      break;
#endif
      
    case gZclSE_InitState_c:
      
      if (events & gZclEvtSEDeviceRunning_c)
      {
#if !gFullEcc_d
        FLib_MemCpyReverseOrder(DeviceImplicitCert.Subject, NlmeGetRequest(gNwkIeeeAddress_c), sizeof(zbIeeeAddr_t));
#endif 	  
        
#if gStandardSecurity_d || gHighSecurity_d     
        
#if gTrustCenter_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
        if (ZbBeeStackNwkGlobals(gTrustCenter))
#endif  
        {
          SE_SetState(gZclSE_ReadyState_c);
        }
#endif

#if !gTrustCenter_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
        if (!ZbBeeStackNwkGlobals(gTrustCenter)) 
#endif         
        if (!DeviceHasCBKEKey())
        {
          SE_SetState(gZclSE_ServiceDiscoveryForKEState_c);
#if (!gZclKeyEstabDebugMode_d  && (gNum_EndPoints_c != 0))
          ServiceDiscoveryForKeyEstab();
#endif
        } 
        else
        {
          SE_SetState(gZclSE_KeyEstabState_c);
          TS_SendEvent(gSETaskID, gZclEvtKeyEstablishmentSuccess_c);
        }
         
#endif         
#endif /* gStandardSecurity_d || gHighSecurity_d */
      }
      break;
      
    case gZclSE_ServiceDiscoveryForKEState_c:
      if (events & gZclEvtServiceDiscoveryFinished_c)
      {
        if (gTCEndPoint != gZdoInvalidEndPoint_c)
        {
          SE_SetState(gZclSE_KeyEstabState_c);
          gKeyEstabMaxRetries = gKeyEstabMaxRetries_c;
          ZCL_InitiateKeyEstab(gTCEndPoint, BeeAppDataInit(appEndPoint),(uint8_t*) gTrustCenterAddr);
        }
      }
      break;

    case gZclSE_KeyEstabState_c:
      if (events & gZclEvtKeyEstablishmentFailed_c)
      {
        gKeyEstabMaxRetries--;
        if (gKeyEstabMaxRetries)
          ZCL_InitiateKeyEstab(gTCEndPoint, BeeAppDataInit(appEndPoint),(uint8_t*) gTrustCenterAddr);
      }
      
      if (events & gZclEvtKeyEstablishmentSuccess_c)
      {
#if gZclSE_MDUPairing_d        
        SE_SetState(gZclSE_ServiceDiscoveryForMDUPairingState_c);
        gFTCEndPoint = gZdoInvalidEndPoint_c;
        DiscoverMDUPairingService();
#else
        SE_SetState(gZclSE_ServiceDiscoveryState_c);
        DiscoverOtherServices((uint8_t*)gaBroadcastRxOnIdle);
#endif        
      }
      break;

#if gZclSE_MDUPairing_d       
    case gZclSE_ServiceDiscoveryForMDUPairingState_c:
      
      if (events & gZclEvtServiceDiscoveryFinished_c)
      {
        if (gFTCEndPoint != gZdoInvalidEndPoint_c)
        {
          zclMDUPairing_MDUEntry_t* pDevices = zclMDUPairing_GetVHANDevices();
          
          SE_SetState(gZclSE_MDUPairingState_c);
          FLib_MemSet(pDevices, 0, sizeof(zclMDUPairing_MDUEntry_t));
          SendPairRequest();
        }
        else
        {
          /* MDU Pairing is not supported. Revert to classic SD */
          SE_SetState(gZclSE_ServiceDiscoveryState_c);
          DiscoverOtherServices((uint8_t*)gaBroadcastRxOnIdle);
        }
      }
      break;
      
    case gZclSE_MDUPairingState_c:
      
      if (events & gZclEvtPairingFinished_c)
      {
        zclMDUPairing_MDUEntry_t* pDevices = zclMDUPairing_GetVHANDevices();
          
        if (pDevices->devCount)
        {
          mCurrentHANDevIdx = 0;
          DiscoverOtherServicesOnVHAN();
        }
        else
        {
          /* No Pairings found. Revert to classic SD */
          SE_SetState(gZclSE_ServiceDiscoveryState_c);
          DiscoverOtherServices((uint8_t*)gaBroadcastRxOnIdle);
        }
      }
      
      if (events & gZclEvtQueryNextPairing_c)
      {
        zclMDUPairing_MDUEntry_t* pDevices = zclMDUPairing_GetVHANDevices();
        
        mCurrentHANDevIdx++;
        if (mCurrentHANDevIdx < pDevices->devCount)
          DiscoverOtherServicesOnVHAN();
        else
          SE_SetState(gZclSE_ServiceDiscoveryState_c);
      }
      break;
#endif      

  case gZclSE_ServiceDiscoveryState_c:
      if (events & gZclEvtServiceDiscoveryFinished_c)
      {
        if (IsLocalDeviceReceiverOnWhenIdle())
        {
          SetClustersForBinding();
          
          if (mNoBoundClusters != 0)
          {
            SE_SetState(gZclSE_BindServiceState_c);
            mCurrentBoundClusterIdx = 0;
            mCurrentESIIdx = 0;
            BindClusterOnESI();
          }
          else
          {
#if gSeEnergyServicePortal_d
          SE_SetState(gZclSE_ReadyState_c); 
#else          
          SE_SetState(gZclSE_TimeServerDiscoveryState_c);
          BeeAppUpdateDevice(BeeAppDataInit(appEndPoint), gZclUI_SEServiceDiscoveryCompleted_c, 0, NULL, NULL);
          mCurrentESIIdx = 0;
          QueryTimeServerOnESI();
#endif /* gSeEnergyServicePortal_d */       
          }
        }
        else
        {
          SE_SetState(gZclSE_TimeServerDiscoveryState_c);
          BeeAppUpdateDevice(BeeAppDataInit(appEndPoint), gZclUI_SEServiceDiscoveryCompleted_c, 0, NULL, NULL);
          mCurrentESIIdx = 0;
          QueryTimeServerOnESI();
        }
      }
      break;
     
    case gZclSE_BindServiceState_c:
      if (events & gZclEvtServiceBindFinished_c)
      {
        if (mCurrentBoundClusterIdx == mNoBoundClusters - 1)
        {
          mCurrentESIIdx++;
          mCurrentBoundClusterIdx = 0;
                    
          if ((mCurrentESIIdx < gMaxNoOfESISupported_c) &&
              (gZclSeESIList[mCurrentESIIdx].EntryStatus == gEntryUsed_c))
          {
             BindClusterOnESI();
          }
          else
          {
            SE_SetState(gZclSE_TimeServerDiscoveryState_c);  
            BeeAppUpdateDevice(BeeAppDataInit(appEndPoint), gZclUI_SEServiceDiscoveryCompleted_c, 0, NULL, NULL);
            mCurrentESIIdx = 0;
            QueryTimeServerOnESI();
          }
        }
        else
        {
          mCurrentBoundClusterIdx++;
          BindClusterOnESI();
        }
      }
      break;
      
    case gZclSE_TimeServerDiscoveryState_c:
      if (events & gZclEvtTimeServerQueried_c)
      {
        mCurrentESIIdx++;
        if ((mCurrentESIIdx < gMaxNoOfESISupported_c) &&
            (gZclSeESIList[mCurrentESIIdx].EntryStatus == gEntryUsed_c))
          QueryTimeServerOnESI();    
        else
        {
          /* Sync time with server*/
          ZbTMR_StartTimer(gTimeSyncTimerID, gTmrLowPowerMinuteTimer_c, 100, TimeUpdateCallBack);
          
          SE_SetState(gZclSE_ReadyState_c);  
          BeeAppUpdateDevice(BeeAppDataInit(appEndPoint), gZclUI_SEReadyState, 0, NULL, NULL);
        }       
        
#if gZclEnableOTAClient_d  &&   gZclOTADiscoveryServerProcess_d   
      (void)OtaClient_StartServerDiscoveryProcess(gNoOfOtaClusterInstances_d); 
#endif	//gZclEnableOTAClient_d	&& gZclOTADiscoveryServerProcess_d        
      
      }
      break;
        
    case gZclSE_ReadyState_c:
      if (events & gZclEvtESIDiscoveryTimeOut_c)
      {
        SE_SetState(gZclSE_ServiceDiscoveryState_c);
        DiscoverOtherServices((uint8_t *)gaBroadcastRxOnIdle);
      }
      
      if (events & gZclEvtKeepAliveFailure_c)
      {        
        if (gZclSENoOfFailedKeepAlive >= gZclSEMaxNoFailedKeepAlives_c)
        {
          /* Reset Keep Alive Counter */
          gZclSENoOfFailedKeepAlive = 0;
          
          /* Enter Rejoin & recovery state*/
          SE_SetState(gZclSE_RejoinAndRecoveryState_c);
          ZDO_StopEx(gZdoStopMode_StopAndRestart_c | gZdoStopMode_ResetNvm_c);
        }
       }
      
      if (events & gZclEvtHandleSEClusterEvt_c)
        ZCL_HandleSEClusterEvt();

      break;
      
    case gZclSE_RejoinAndRecoveryState_c:
      {
        if(events & gZclEvtRejoinFailure_c)
        {
           ApsmeSetRequest(gApsUseInsecureJoin_c, TRUE);
        }
      }
      break;
      
    default:
      break;    
  }
}



/*!
 * @fn 		void ZCL_Register_EspRegisterDeviceFunctions(ESPRegisterDevFunc *RegFunc, ESPDeRegisterDevFunc *DeRegFunc)
 *
 * @brief	Assigns Register Device Functions.
 *
 */
void ZCL_Register_EspRegisterDeviceFunctions(ESPRegisterDevFunc *RegFunc, ESPDeRegisterDevFunc *DeRegFunc)
{
  pSE_ESPRegFunc = RegFunc;
  pSE_ESPDeRegFunc = DeRegFunc;
}

/*!
 * @fn 		int ECC_GetRandomDataFunc(unsigned char *buffer, unsigned long sz)
 *
 * @brief	Assigns random values to a supplied buffer
 *
 */
int ECC_GetRandomDataFunc(unsigned char *buffer, unsigned long sz)
{
        unsigned long i;
        for(i=0;i<sz;i++){
                ((unsigned char *)buffer)[i] = (uint8_t) GetRandomNumber();
        }
        return 0x00; //success
} 

/*!
 * @fn 		int ECC_HashFunc(unsigned char *digest, unsigned long sz, unsigned char *data)
 *
 * @brief	Applies MatyasMeyerOseasHash on the supplied data.
 *
 */
int ECC_HashFunc(unsigned char *digest, unsigned long sz, unsigned char *data)
{
/* Insert to use Certicom's AES MMO function*/
/*  aesMmoHash(digest, sz, data);  */
  /*zero initialize digest before generating hash*/
  BeeUtilZeroMemory(digest,16);
  SSP_MatyasMeyerOseasHash(data, (uint8_t) sz, digest);  
  return 0x00; //success
}

/*!
 * @fn 		void ZCL_ApplyECDSASign(uint8_t *pBufIn, uint8_t lenIn, uint8_t *pBufOut)
 *
 * @brief	Applies ECDSA Signing on the supplied input buffer.
 *
 */
void ZCL_ApplyECDSASign(uint8_t *pBufIn, uint8_t lenIn, uint8_t *pBufOut)
{
  uint8_t digest[16];
  /* Hash the pBufIn */
  BeeUtilZeroMemory(digest, 16);
  SSP_MatyasMeyerOseasHash(pBufIn, lenIn, digest);

  (void)ZSE_ECDSASign( DevicePrivateKey, &digest[0],
                ECC_GetRandomDataFunc, pBufOut, (pBufOut + 21), NULL, 0 );

}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		static uint16_t crc_reflect(uint16_t data, uint8_t data_len)
 *
 * @brief	Applies CRC reflect on the input data.
 *
 */
static uint16_t crc_reflect(uint16_t data, uint8_t data_len)
{
    uint8_t i;
    uint16_t ret;
 
    ret = 0;
    for (i = 0; i < data_len; i++)
    {
        if (data & 0x01) {
            ret = (ret << 1) | 1;
        } else {
            ret = ret << 1;
        }
        data >>= 1;
    }
    return ret;
}
 
/*!
 * @fn 		static uint16_t crc_update(uint16_t crc16, uint8_t *pInput, uint8_t length)
 *
 * @brief	Updates CRC16 on the input data.
 *
 */
static uint16_t crc_update(uint16_t crc16, uint8_t *pInput, uint8_t length)
{
uint8_t data;
  while (length--)
  {
 data = (uint8_t)crc_reflect(*pInput++, 8); // pInput may need to be shifted 8 bits up if running code on a big endian machine.
 
    crc16 = (uint8_t)(crc16 >> 8) | (crc16 << 8);
    crc16 ^= data; // x^1
    crc16 ^= (uint8_t)(crc16 & 0xff) >> 4; // ???
    crc16 ^= (crc16 << 8) << 4;    // x^12
    crc16 ^= ((crc16 & 0xff) << 4) << 1;    // X^5
  }
  return (0xFFFF^crc_reflect(crc16, 16));
}

/*!
 * @fn 		bool_t GenerateKeyFromInstallCode(uint8_t length,uint8_t* pInstallationCode, uint8_t *pKey)
 *
 * @brief	This function computes an initial key based on an installation code.
 *
 * @param   [in]  		pInstallationCode	pointer to an installation code (max 144bit incl crc16)
 * @param   [in]  		length				length of installation key (in bytes incl CRC)
 * @param   [in,out]  	pKey				pointer to key (or hash)
 */
bool_t GenerateKeyFromInstallCode(uint8_t length,uint8_t* pInstallationCode, uint8_t *pKey)
{
  // 1. Validate InstallationCode (CRC16)
  // 2. Pad installation code (M) to obtain M' 
  // 3. Compute hash using AES128bit engine
  uint16_t mCrc16;
  uint16_t vCrc16 = 0xffff;
  mCrc16=(pInstallationCode[length-1]<<8 | pInstallationCode[length-2]);
  vCrc16 = crc_update(vCrc16,pInstallationCode, length-2);
  // Check CRC
  if(mCrc16!=vCrc16){
    return FALSE;
  } 
  else{
    SSP_MatyasMeyerOseasHash(pInstallationCode,length ,pKey);
    return TRUE;  
  }
}

#if (gStandardSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d)
/*!
 * @fn 		zbStatus_t ZCL_ESPRegisterDevice(EspRegisterDevice_t *Devinfo)
 *
 * @brief	Registers an SE device locally on the Trust Center.
 *
 */
zbStatus_t ZCL_ESPRegisterDevice(EspRegisterDevice_t *Devinfo)
{
  zbAESKey_t key = {0};
  index_t    i;
  zbNwkAddr_t unknownAddr = {0xFF,0xFF};
  i = ZCL_AddToRegTable(Devinfo);
  if (i != RegTable_InvalidIndex_c)
  {
    ESPRegTable_t* pRegEntry = &RegistrationTable[i];
    if (GenerateKeyFromInstallCode(pRegEntry->DevInfo.InstallCodeLength, pRegEntry->DevInfo.InstallCode,key)) {    
      if (APS_AddToAddressMapPermanent(Devinfo->Addr,unknownAddr) != gAddressMapFull_c)
        APS_RegisterLinkKeyData(pRegEntry->DevInfo.Addr, gTrustCenterLinkKey_c, (uint8_t *) key);        
      else
        return gZclNoMem_c;    
    return gZclSuccess_c;  
    } else
     return gZclFailure_c;
  }
  return gZclNoMem_c;
    
}

/*!
 * @fn 		zbStatus_t ZCL_ESPDeRegisterDevice(EspDeRegisterDevice_t *Devinfo)
 *
 * @brief	Deregisters an SE device locally on the Trust Center.
 *
 */  
zbStatus_t ZCL_ESPDeRegisterDevice(EspDeRegisterDevice_t *Devinfo)
{
  index_t Index;
  Index = ZCL_FindIeeeInRegTable(Devinfo->Addr);
  if (Index != RegTable_InvalidIndex_c)
  {
    ESPRegTable_t* pRegEntry = &RegistrationTable[Index];
    ASL_Mgmt_Leave_req(NULL, (uint8_t *)gaBroadcastRxOnIdle, pRegEntry->DevInfo.Addr, 0);
    APS_RemoveSecurityMaterialEntry(pRegEntry->DevInfo.Addr);
    APS_RemoveFromAddressMap(Devinfo->Addr);
    Fill8BytesToZero(pRegEntry->DevInfo.Addr); 
    return gZclSuccess_c;
  } else
    return gZclNoMem_c;   
}

/*!
 * @fn 		zbStatus_t ZCL_ESIReRegisterDevice(EspReRegisterDevice_t *Devinfo)
 *
 * @brief	Re-Registers an SE device locally on the Trust Center.
 *
 */  
zbStatus_t ZCL_ESIReRegisterDevice(EspReRegisterDevice_t *DevInfo)
{
  index_t Index;
  Index = ZCL_FindIeeeInRegTable(DevInfo->Addr);
  if (Index != RegTable_InvalidIndex_c)
  {
    ESPRegTable_t* pRegEntry = &RegistrationTable[Index];
    APS_RemoveSecurityMaterialEntry(pRegEntry->DevInfo.Addr);
    return gZclSuccess_c;
  } else
    return gZclNoMem_c;   
}
#endif

#if (gStandardSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d)
/*!
 * @fn 		uint8_t ZclSE_GetNumOfRegisteredDevices(void)
 *
 * @brief	Returns the number of SE registered devices on the Trust Center.
 *
 */  
uint8_t ZclSE_GetNumOfRegisteredDevices(void)
{
  uint8_t i, count = 0;
  ESPRegTable_t* pRegEntry = &RegistrationTable[0];
  
  for(i=0; i<RegistrationTableEntries_c; ++i)
  {
    /* found the entry */
    if(pRegEntry[i].DevStatus == RegTable_DevStatusKeyEstablishedState_c)
      count ++;
  }
  
  return count;
}

/*!
 * @fn 		index_t ZCL_FindIeeeInRegTable(zbIeeeAddr_t aExtAddr)
 *
 * @brief	Finds the index of an SE device in the Registration Table on the 
 *			Trust Center.
 *
 */
index_t ZCL_FindIeeeInRegTable(zbIeeeAddr_t aExtAddr)
{
  index_t i;

  for(i=0; i<RegistrationTableEntries_c; ++i)
  {
    ESPRegTable_t* pRegEntry = &RegistrationTable[i];
    /* found the entry */
    if(IsEqual8Bytes(pRegEntry->DevInfo.Addr, aExtAddr))
      return i;
  }
  return RegTable_InvalidIndex_c;
}

/*!
 * @fn 		index_t ZCL_AddToRegTable(EspRegisterDevice_t *Devinfo)
 *
 * @brief	Adds an SE device in the Registration Table on the 
 *			Trust Center.
 *
 */
index_t ZCL_AddToRegTable(EspRegisterDevice_t *Devinfo)
{
  index_t i;
  index_t iFree;
  ESPRegTable_t* pRegEntry;

  /* indicate we haven't found a free one yet */
  iFree = RegTable_InvalidIndex_c;

  for(i=0; i<RegistrationTableEntries_c; ++i)
  {
    pRegEntry = &RegistrationTable[i];
    
    /* found the entry */
    if(IsEqual8Bytes(pRegEntry->DevInfo.Addr, Devinfo->Addr))
    {
      /*Ieee address already exist in table, exist and do nothing*/
      return RegTable_InvalidIndex_c;
    }

    /* record first free entry */
    if(iFree == RegTable_InvalidIndex_c && Cmp8BytesToZero(pRegEntry->DevInfo.Addr))
      iFree = i;
  }
  /* return indicating full */
  if(iFree == RegTable_InvalidIndex_c)
    return iFree;
  
  pRegEntry = &RegistrationTable[iFree];

  /* add in new entry */
  FLib_MemCpy(&pRegEntry->DevInfo, Devinfo, sizeof(EspRegisterDevice_t));
#if gEccIncluded_d == 1
  pRegEntry->DevStatus = RegTable_DevStatusIntialState_c;
#else
  // If Ecc library is not included, then set that device has done key estab.
  pRegEntry->DevStatus = RegTable_DevStatusKeyEstablishedState_c; 
  pRegEntry->DateTimeStamp = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint)); 
#endif  
  return iFree;
}
#endif

/*!
 * @fn 		void ZCL_ESPInit(void)
 *
 * @brief	Initializes device registration on the ESI Trust Center.
 *
 */
void ZCL_ESPInit(void)
{
#if (gStandardSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d)  
    ZCL_Register_EspRegisterDeviceFunctions(ZCL_ESPRegisterDevice, ZCL_ESPDeRegisterDevice);
#endif    
}

/*!
 * @fn 		void ZCL_SetKeyEstabComplete(zbEndPoint_t remoteEndPoint, zbIeeeAddr_t aExtAddr)
 *
 * @brief	Change status of new registered device to KeyEstablished.
 *
 */
void ZCL_SetKeyEstabComplete(zbEndPoint_t remoteEndPoint, zbIeeeAddr_t aExtAddr)
{
#if (gStandardSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d)  
#if (gComboDeviceCapability_d)
  if (ZbBeeStackNwkGlobals(gTrustCenter))
#endif  
  {
    index_t Index;
    Index = ZCL_FindIeeeInRegTable(aExtAddr); 
    
    /* Device was not commissioned using install codes and used the global 
     link key. We must add it into the registration table */
    if (Index == RegTable_InvalidIndex_c)
    {
      EspRegisterDevice_t devInfo;
      Copy8Bytes(devInfo.Addr, aExtAddr);
      devInfo.InstallCodeLength = 0;
      Index = ZCL_AddToRegTable(&devInfo);
    }
        
    if (Index != RegTable_InvalidIndex_c)
    {
      ESPRegTable_t* pRegEntry = &RegistrationTable[Index];
      pRegEntry->DevStatus = RegTable_DevStatusKeyEstablishedState_c;
      pRegEntry->DateTimeStamp = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint)); 
      pRegEntry->KeyEstabEndPoint = remoteEndPoint;
    }
  }
#endif    
}  

#if (gStandardSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) 
/*!
 * @fn 		bool_t ZCL_CreateRegDevBackupEntry(uint8_t index, seRegDev_t* pEntry)
 *
 * @brief	Creates a backup of the registration table entry.
 *
 */
bool_t ZCL_CreateRegDevBackupEntry(uint8_t index, seRegDev_t* pEntry)
{
  zbApsDeviceKeyPairSet_t whereToCpyEntry;
  ESPRegTable_t* pRegEntry = &RegistrationTable[index];
  zbApsDeviceKeyPairSet_t  *pSecurityMaterial;
  zbAESKey_t tcLinkKey;
  
  if (Cmp8BytesToZero(pRegEntry->DevInfo.Addr))
   return FALSE;
  
  pSecurityMaterial = APS_GetSecurityMaterilaEntry(pRegEntry->DevInfo.Addr, &whereToCpyEntry);
  
  if (!pSecurityMaterial)
    return FALSE;
  
  KeySet_GetTableEntry(pSecurityMaterial->iKey, &tcLinkKey);
  (void)ECC_HashFunc(pEntry->aHashedKey, sizeof(zbAESKey_t), (uint8_t*)&tcLinkKey);
  Copy8Bytes(pEntry->aExtAddr, pRegEntry->DevInfo.Addr);
  FLib_MemCpy(pEntry->aInstallCode, pRegEntry->DevInfo.InstallCode, sizeof(zbAESKey_t));
  
  return TRUE;
}
#endif

/*!
 * @fn 		bool_t ZCl_SEClusterSecuritycheck(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Checks cluster security level for received data indication
 *
 */
bool_t ZCl_SEClusterSecuritycheck(zbApsdeDataIndication_t *pIndication)
{
  uint16_t cluster;

   /*if packets are from our self then do not check security level */
  if (IsSelfNwkAddress(pIndication->aSrcAddr)) 
  {  
    return TRUE;
  }

  Copy2Bytes(&cluster, pIndication->aClusterId);
  
#if gStandardSecurity_d
  /* NWK Security required */
  if((cluster == gZclClusterBasic_c)    ||
     (cluster == gZclClusterIdentify_c) ||
     (cluster == gZclClusterPowerCfg_c) ||
     (cluster == gZclClusterALarms_c) ||
     (cluster == gKeyEstabCluster_c)) 
  {
    if((pIndication->fSecurityStatus & gApsSecurityStatus_Nwk_Key_c) == gApsSecurityStatus_Nwk_Key_c)
      return TRUE;   
    else
      return FALSE;
  }
  
  /* APS Security required */
  if((cluster == gZclClusterSmplMet_c) ||
     (cluster == gZclClusterMsg_c)      ||
     (cluster == gZclClusterPrice_c)    ||   
     (cluster == gZclClusterDmndRspLdCtrl_c) ||
     (cluster == gZclClusterTime_c)     ||
     (cluster == gZclClusterSETunneling_c)   ||
     (cluster == gZclClusterPrepayment_c)    ||
     (cluster == gZclClusterTouCalendar_c)     ||
     (cluster == gZclClusterDevMgmt_c)     ||
     (cluster == gZclClusterMDUPairing_c)     ||
     (cluster == gZclClusterCommissioning_c))
   {
#if gApsLinkKeySecurity_d  
    if((pIndication->fSecurityStatus & gApsSecurityStatus_Link_Key_c) == gApsSecurityStatus_Link_Key_c)
      return TRUE;
    else
      return FALSE;
#else
   return TRUE;
#endif    
   }
#endif
  return TRUE; 
}

/*!
 * @fn 		zbStatus_t ZCL_SE_TCSwapOut_SetDeviceData(seRegDev_t *pReq)
 *
 * @brief	Restores Trust Center data after Swap-Out
 *
 */
zbStatus_t ZCL_SE_TCSwapOut_SetDeviceData(seRegDev_t *pReq)
{
  zbApsDeviceKeyPairSet_t  *pSecurityMaterial;
  zbStatus_t status = gZbSuccess_c;
  zbApsDeviceKeyPairSet_t whereToCpyEntry;
  
  addrMapIndex_t index;
  /*
    Check if the current device is already on our address map, just to avoids re-adding it
    with an invalid short address.
  */
  index = APS_FindIeeeInAddressMap(pReq->aExtAddr);
  if (index == gNotInAddressMap_c)
  {
    /*
      NOTE: we can not do as the function SSP_ApsRegisterLinkKeyData does, and serach on NT
      for the current device, because this node may be the TC, or the network may be too big,
      if we dont know the node the short address will be null and we will hope to update
      it later.
    */
    index = APS_AddToAddressMapPermanent(pReq->aExtAddr, (void *)gaBroadcastAddress);
    if (index == gAddressMapFull_c)
      status = gZbTableFull_c;
  }

  /* At this point we have an index in the address map, lets start filling the info. */
  APS_RegisterLinkKeyData(pReq->aExtAddr, gApplicationLinkKey_c, pReq->aHashedKey);

  /* The function above may fail but it does not return any errors, so we just check.. just in case. */
  pSecurityMaterial = SSP_ApsGetSecurityMaterilaEntry(pReq->aExtAddr, &whereToCpyEntry);

  /* Unable to read the secure material, report table full any way.! */
  if (!pSecurityMaterial)
  {
    status = gZbTableFull_c;
  }
  
  return status;
}

/*!
 * @fn 		bool_t ZclSE_ValidateKey(zbIeeeAddr_t aExtAddr)
 *
 * @brief	Checks if the application key used to encrypt frames from the 
 *			input address is stale or preconfigured. 
 *
 */
bool_t ZclSE_ValidateKey(zbIeeeAddr_t aExtAddr)
{
  bool_t status = TRUE;
#if gStandardSecurity_d || gHighSecurity_d     
  
  /* This is a coordinator which is also the Trust Center */  
#if gTrustCenter_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
  if (ZbBeeStackNwkGlobals(gTrustCenter))
#endif
  {  
    uint8_t devIdx;
  
    devIdx = ZCL_FindIeeeInRegTable(aExtAddr);
    
    if (devIdx == RegTable_InvalidIndex_c)
    {
      return FALSE;
    }
    else
    {
      ESPRegTable_t* pRegEntry = &RegistrationTable[devIdx] ;
      if (pRegEntry->DevStatus != RegTable_DevStatusKeyEstablishedState_c)
      {
        status = FALSE;
      }
      
      /* 5.4.5 When the trust center receives a message encrypted with a 
      stale link key, it shall initiate the key establishment procedure 
      to negotiate a new link key */

      if (pRegEntry->DevStatus == RegTable_DevStatusStaleKeyState_c) 
      {
        zbAddressMap_t addrMap;
        (void)AddrMap_SearchTableEntry((zbIeeeAddr_t*)aExtAddr, NULL, &addrMap);
        ZCL_InitiateKeyEstab(pRegEntry->KeyEstabEndPoint, BeeAppDataInit(appEndPoint), addrMap.aNwkAddr);
      }
    }
  }
#endif

  /* This is a router or end device */  
#if !gTrustCenter_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
  if (!ZbBeeStackNwkGlobals(gTrustCenter))
#endif
  {
    if (IsEqual8Bytes(aExtAddr, ApsmeGetRequest(gApsTrustCenterAddress_c)))
    {
      /* Message was sent by the Trust Center */
      status =  DeviceHasCBKEKey();
    }
    else
    {
      /* Message was not sent by the Trust Center */
      status = SSP_ApsIsLinkEstablished(aExtAddr);
    }
  }
#endif
#endif
    return status;  
}

/*!
 * @fn 		void ZCL_HandleSEClusterEvt(void)
 *
 * @brief	Handles any of the SE Clusters Events
 *
 */
void ZCL_HandleSEClusterEvt(void)
{
  uint32_t events = mSEClusterEvt;  

#if gASL_ZclDmndRspLdCtrl_ReportEvtStatus_d  
  if(events & gZclEvtHandleLdCtrl_c)
    ZCL_HandleScheduledEventNow();
  if(events & gzclEvtHandleReportEventsStatus_c)
    ZCL_HandleReportEventsStatus();
#endif 

#if gASL_ZclDmndRspLdCtrl_GetScheduledEvtsReq_d   
  if(events & gzclEvtHandleGetScheduledLdCtlEvts_c)
    ZCL_HandleGetScheduledLdCtlEvts();
#endif

#if gASL_ZclSE_12_Features_d      
  if(events & gZclEvtHandleGetSnapshot_c)
    ZCL_HandleSEMetGetSnapshot();  
#endif      
  if(events & gzclEvtHandlePriceClusterEvt_c)
    ZCL_HandleSEPriceClusterEvt();  

#if gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d  
  if(events & gzclEvtHandleTouCalendarClusterEvt_c)
    ZCL_HandleSETouCalendarClusterEvt();
#endif   

#if gZclSE_MDUPairing_d 
  if(events & gZclEvtHandleMDUPairingRspEvt_c)
    ZCL_HandlePairingRsp();
#endif     
}

/*!
 * @fn 		void ZclSE_SendClusterEvt(uint32_t evtId)
 *
 * @brief	Sends the Price Cluster Event to the Task Scheduler 
 *
 */
void ZclSE_SendClusterEvt(uint32_t evtId)
{
  mSEClusterEvt = evtId;
  TS_SendEvent(gSETaskID, gZclEvtHandleSEClusterEvt_c);
}

/*!
 * @fn 		void AddUpdateDeviceDescriptor(zbNodeDescriptorResponse_t *pNodeDesc)
 *
 * @brief	Adds or updates the remote device descriptor information
 *
 */
void AddUpdateDeviceDescriptor(zbNodeDescriptorResponse_t *pNodeDesc)
{
  index_t i, iNewEntry = gInvalidTableIndex_c;
  seDeviceDescriptor_t *pEntry;
  
  for (i = 0; i < gApsMaxAddrMapEntries_c; i++)
  {
    pEntry = &gZclSeDeviceList[i];
    
    if (pEntry->EntryStatus == gEntryNotUsed_c)
    {
      if (iNewEntry == gInvalidTableIndex_c)
        iNewEntry = i;
    }
    else if (FLib_Cmp2Bytes(pNodeDesc->aNwkAddrOfInterest, pEntry->aNwkAddr)){
      iNewEntry = i;
      break;
    }
  }
  
  /* Add the information */
  if (iNewEntry != gInvalidTableIndex_c)
  {
    pEntry = &gZclSeDeviceList[iNewEntry];
    pEntry->EntryStatus = gEntryUsed_c;
    pEntry->MaxOutTransferSize = GetNative16BitInt(pNodeDesc->nodeDescriptor.aMaxTransferSize);
    Copy2Bytes(pEntry->aNwkAddr, pNodeDesc->aNwkAddrOfInterest);
  }
}

/*!
 * @fn 		uint16_t GetMaxOutTransferSize(zbNwkAddr_t  aNwkAddr)
 *
 * @brief	Gets the Maximum Outgoing Transfer Size to a remote node
 *
 */
uint16_t GetMaxOutTransferSize(zbNwkAddr_t  aNwkAddr)
{
  index_t i;
  seDeviceDescriptor_t *pEntry;
  
  /* Find the entry */
  for (i = 0; i < gApsMaxAddrMapEntries_c; i++)
  {
    pEntry = &gZclSeDeviceList[i];
    
    if (FLib_Cmp2Bytes(aNwkAddr, pEntry->aNwkAddr) &&
             (pEntry->EntryStatus != gEntryNotUsed_c)){
      break;
    }
  }
  
  if (i < gApsMaxAddrMapEntries_c)
    return pEntry->MaxOutTransferSize;
  
  return TwoBytesToUint16(ZbBeeStackGlobalsParams(gBeeStackConfig.aMaxOutTransferSize));
}
