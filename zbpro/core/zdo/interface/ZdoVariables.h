/***************************************************************************
* This is the header file for the ZdoInternalVariables
*
* (c) Copyright 2013, Freescale, Inc. All rights reserved.
*
* Freescale Semiconductor Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*****************************************************************************/
#ifndef _ZDO_INTERNAL_VARIABLES_H_
#define _ZDO_INTERNAL_VARIABLES_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"
#include "zigbee.h"

#include "ZDOApsInterface.h"
#include "ZdpManager.h"
/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

#if(gInstantiableStackEnabled_d == 0)

  #define ZbZdoPrivateData(val)          val
  #define ZbZdoPublicData(val)           val

#else

  #define ZbZdoPrivateData(val)            pZbZdoPrivateData->val
  #define ZbZdoPublicData(val)             pZbZdoPublicData->val

#endif

/* various end-device bind constants */
#define mEdbMaxEntries_c              2             /* maximum EDB nodes at once */
#define mEdbMaxClusters_c             16            /* 16 total clusters in both input/output list */
#define mEdbTickInterval_c            100           /* 1/10 second between ticks */
#define mEdbTicksToExpire_c           (mDefaultValueOfEndDeviceBindTimeOut_c/mEdbTickInterval_c)  /* wait EDB up to xx seconds */
#define mEdbProcessTicksToExpire_c    (mDefaultValueOfEndDeviceBindProcessTimeOut_c/mEdbTickInterval_c)  /* wait EDB up to xx seconds */
#define mEdbTicksNoExpire_c           0xffff
#define mEdbTicksForReply_c           (uint8_t)((ZbBeeStackNwkGlobals(gNwkPurgeTimeout) + 1500)/mEdbTickInterval_c)   /* wait 1.5 seconds + NwkPersistenceTime for reply */
#define mEdbMaxClusterMatch_c         (8 +1)

/* Maximum size of AF data frame at AF level as specified by spec. But
application can send data frame more than 70 bytes. */
#if gStandardSecurity_d || gHighSecurity_d
  #define gAfMaxCommandSize_c 0x43
#else
  #define gAfMaxCommandSize_c 70
#endif

#define gEndDevBindTimerID               ZbZdoPrivateData(gZdoTimerIDs[0])
#define gZdoDiscoveryAttemptTimerID      ZbZdoPrivateData(gZdoTimerIDs[1])
#define gZdoFormationAttemptTimerID      ZbZdoPrivateData(gZdoTimerIDs[2]) /* forming and leaving can't happen at same time */
#define gZdoLeavingTimerID               ZbZdoPrivateData(gZdoTimerIDs[2]) /* forming and leaving can't happen at same time */
#define gZdoOrphanScanAttemptTimerID     ZbZdoPrivateData(gZdoTimerIDs[3])
#define gTimeBetScansTimerID             ZbZdoPrivateData(gZdoTimerIDs[4])
#define gZdoEstablishKeysTimerID         ZbZdoPrivateData(gZdoTimerIDs[5])
#define gFAChannelChangeTimerID          ZbZdoPrivateData(gZdoTimerIDs[6])
#define gNwkConcentratorDiscoveryTimerID ZbZdoPrivateData(gZdoTimerIDs[7])
/*****************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
/* how many ticks left before quitting end-device-bind? */
typedef uint32_t zdoEdbTicksLeft_t;

/* various states for this EDB call, numeric not bit mask */
typedef uint8_t zdoEdbState_t;

/* internal structure for end-device-bind */
typedef PACKED_STRUCT zdoEdbTable_tag
{
  zdoEdbState_t            state;           /* current state of this entry (also direction) */
  zbStatus_t               status;          /* what status should this report? */
  uint8_t                  iTransactionId;  /* record transaction ID */
  uint8_t                  iTimeout;        /* how long to wait before this next state time's out? */
  index_t                  iMatchIndex;     /* does the other side match? Record the index in the EDB table that matches */
  zbClusterId_t            aMatchCluster[mEdbMaxClusterMatch_c];   /* what cluster matched? */
  zbEndDeviceBindRequest_t request;         /* copy of bind request */
  zbClusterId_t            aClusterList[mEdbMaxClusters_c];  /* input and output clusters */
} zdoEdbTable_t;


/* Zdo module internal used ram global structure */

#if(gInstantiableStackEnabled_d == 1)
typedef struct zbZdoPrivateData_tag
{
#endif
  
  /* Initialized part of the structure */
  /*ZDOstateMachineHandler*/
EXTERN  ZdoState_t      gZdoState;
  /* informs application layer which join mode (NLME-JOIN modes) */
EXTERN  zbNwkJoinMode_t gZDOJoinMode;
  /* running state can be looked up by device type (ZC=0, ZR=1, ZED=2) */
EXTERN  uint8_t maZdo2AppEvents[3];
EXTERN  ZdoState_t maZdoRunning[3];
  /* ZdpFrequencyAgility */
  #if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
EXTERN  uint8_t  gFA_DeviceState;
    /* The Id of the task that will handle the Channel Master State machine. */
EXTERN  tsTaskID_t gFATaskId;
    /* Initialize all channels with the middle energy value. */
EXTERN  uint8_t mChannels[16];
  #endif
  
  /* ZdpManager*/
  /*The internal counter for each ZDP message, they are mainly neede to keep sync of the request and responses.*/
EXTERN  zbCounter_t  gZdpSequence;
EXTERN  bool_t  mbIsNotFree;
  
  /* ZdpResponses */
  #if gMgmt_Direct_Join_rsp_d
EXTERN  zbNwkAddr_t  aSrcAddress;
  #endif
  
  /******************************************************************************/
  /* Unitialized part of the structure */
  /* ZdoBindManager */
EXTERN  zbClusterId_t gaZdpMatchedOutCluster[mEdbMaxClusterMatch_c];
EXTERN  index_t giMatchIndex;
  #if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d
    /* # of nodes that have requested EDB */
EXTERN  bool_t gZdoInEndDeviceBind;
    /* only processes one side at a time (code reduction) */
EXTERN  index_t mEdbCurrentIndex;
EXTERN  bool_t mEdbError;
	/* time to wait for the 2nd end-device-bind req */
EXTERN  zdoEdbTicksLeft_t mEdbTicksLeft;
    /* time to wait for the end device bind process  to complete */	
EXTERN  zdoEdbTicksLeft_t mEdbProcessTicksLeft;
    /* each entry contains a bind-state-machine */
#if(gInstantiableStackEnabled_d == 1)
EXTERN  zdoEdbTable_t* maEdbTable;
#else
extern  zdoEdbTable_t  maEdbTable[mEdbMaxEntries_c];
#endif
  #endif
  
  /* ZdoMain */  
EXTERN  anchor_t mNlmeZdoInputQueue;
EXTERN  anchor_t mAppZdpInputQueue; 
  #if gStandardSecurity_d || gHighSecurity_d
EXTERN  msgQueue_t mApsmeZdoInputQueue;  
    #if gTrustCenter_d || gComboDeviceCapability_d
      /* The State holder for the Trust center state machine. */
EXTERN  uint8_t mTrustCenterState;
      /* This two pointers are use to handle the queues fro the trust center state machine.*/
EXTERN  nlmeZdoMessage_t          *pJoinMsg;
EXTERN  zbApsmeUpdateDeviceInd_t  *pUpdateMsg;
EXTERN  msgQueue_t                gJoinIndicationQueue;
EXTERN  msgQueue_t                gUpdateDeviceQueue;
    #endif/* gTrustCenter_d || gComboDeviceCapability_d */
  #endif/* gStandardSecurity_d || gHighSecurity_d */
  /* counter for stack sleep... app must manage its own. Makes it so reset allows sleep properly */
EXTERN  zbCounter_t gZdoAllowToSleepCounter;
  /* Various ZDO timers. See ZdoCommon.h for definitions. */
EXTERN  zbTmrTimerID_t gZdoTimerIDs[8];
  
  /* ZdoNwkManager */
EXTERN  bool_t gMemoryFreedByApplication;
  
  /*ZDOstateMachineHandler*/
  /* used for both forming and joining in new state machine */
  /* a network discovery results in a list of networks */
EXTERN  uint8_t     mcDiscoveryRetries;
  /* each network discovery may involve 1 or more active scans */
EXTERN  uint8_t     mcScanRetries;
EXTERN  ZdoEvent_t  gZdoEvent;
EXTERN  bool_t      mDiscoveryFirstTime;
  /* Keeps track of the current rejoin, to always remeber if ti was Seucre or not. */
EXTERN  bool_t      gZDO_SecureRejoin;
  
  /* ZdpFrequencyAgility */
  #if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
    /* List of routers to send the Energy Scan request */
EXTERN  nwkAddrList_t *mpFA_ListOfRouters;
    /* This variable tells the current state of the nwkmanager state machine.*/
EXTERN  uint8_t       gChannelMasterState;
    /*  The amount of incoming report errors to keep track*/
EXTERN  uint8_t       mErrorReportsReceived;
    /* The amount of Scan reports received during the run of the state machine. */
EXTERN  uint8_t       mNumScansReceived;
    /* The amount of node doing Scans, this are the routers request to do the scan. */
EXTERN  uint8_t       mNumScansSent;
    /*   The timer to be use during the error report processing. */
EXTERN  zbTmrTimerID_t  mErrorReportTimer;
     /* FA Generic millisecond timer. */
EXTERN  zbTmrTimerID_t  mGenericFATimer;
    #if (gMgmt_NWK_Update_req_d &&(gNetworkManagerCapability_d || gComboDeviceCapability_d))
      /* The timer to be used during to request the  Energy Scan Req to Routers. Used only by the Network Manager */
EXTERN  zbTmrTimerID_t gFA_ReqEnergyScanToRoutersTimer;
    #endif
    /* Update notify report timer (minute timer). */
EXTERN  zbTmrTimerID_t  gFA_NotifyTimer;
    /* Keeps track of the scannign reason in the state machone.*/
EXTERN  uint8_t  mScanReason;
    /* ScanCount is a counter to keep track of the number of scan requests and reports that needs to be done.*/
EXTERN  uint8_t gScanCount;
    /* Scan channel list in native format. */
EXTERN  zbChannels_t gaScanChannelList;
    /* For the devices that need to send reports back to the Nwk Mngr, we need to keep around the scanduration.*/
EXTERN  uint8_t  mFAScanDuration;
  #endif
    
  /* ZdpManager*/
  /*The internal counter for each ZDP message, they are mainly neede to keep sync of the request and responses.*/
EXTERN  zbCounter_t  gZdpResponseCounter;
  #if gDiscovery_Cache_req_d || gNode_Desc_store_req_d || gPower_Desc_store_req_d || gActive_EP_store_req_d || gSimple_Desc_store_req_d
    /* ZigBee 1.1 Discovery Chache table, Store the address of Discovery Cache Succcess server */
EXTERN  zdpRecordDiscCacheSuccess_t  gDiscoveryCacheSuccess;
EXTERN  bool_t  gDiscoveryCacheResponse;
  #endif
  #if gFind_node_cache_req_d
EXTERN  zdpRecordFindNodeSuccess_t  gFindNodeCacheSuccess;
  #endif /* gFind_node_cache_req_d */
EXTERN  bool_t gIsABroadcast;
    
  /* ZdpResponses */
  #if gMgmt_Leave_rsp_d
EXTERN  zbMgmtOptions_t gMgmtSelfLeaveMask;	
EXTERN  zbNwkAddr_t  mgmtLeaveResponseInfo;
  #endif /* gMgmt_Leave_rsp_d */
  #if gMgmt_NWK_Disc_rsp_d
EXTERN  index_t  gIndex;
EXTERN  bool_t   gMgmtNwkDiscReq;
EXTERN  index_t  gStartIndexforNwkDisc;
  #endif /*gMgmt_NWK_Disc_rsp_d */
  #if gMgmt_NWK_Disc_rsp_d
#if(gInstantiableStackEnabled_d == 1) 
EXTERN  strAddrClusterId_t*  strAddrClusterId;
#else
extern  strAddrClusterId_t   strAddrClusterId[];
#endif
  #endif
  #if (gDiscovery_store_rsp_d || gRemove_node_cache_rsp_d || gMgmt_Cache_rsp_d)
EXTERN  zbCounter_t  mDiscoveryStoreTableEntrys;
  #endif
  #if (gBindCapability_d && (gBind_rsp_d || gUnbind_rsp_d)) || gReplace_Device_rsp_d || gBind_Register_rsp_d
EXTERN  zbEventProcessQueue_t  maEventProcessQueue[gMaximumEventsToProcess_c];
  #endif
  #if gBkup_Discovery_cache_d || gDiscovery_store_rsp_d || gMgmt_Cache_rsp_d || gRemove_node_cache_rsp_d || gFind_node_cache_rsp_d|| gActive_EP_store_rsp_d || gPower_Desc_store_rsp_d || gNode_Desc_store_rsp_d || gSimple_Desc_store_rsp_d
#if(gInstantiableStackEnabled_d == 1) 
EXTERN  discoveryStoreTable_t*  gaDiscoveryStoreTable;
#else
extern  discoveryStoreTable_t  gaDiscoveryStoreTable[];
#endif
  #endif 
  #if gBind_Register_rsp_d || gRecover_Source_Bind_rsp_d || gBackup_Source_Bind_rsp_d || (gBindCapability_d && gBind_rsp_d)
#if(gInstantiableStackEnabled_d == 1)
EXTERN  zbIeeeAddr_t*  aDevicesHoldingItsOwnBindingInfo;
#else
extern  zbIeeeAddr_t  aDevicesHoldingItsOwnBindingInfo[];
#endif
EXTERN  zbCounter_t    gDeviceHoldingItsOwnBindingInfo_Counter;
  #endif
  #if (gReplace_Device_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRecover_Bind_Table_rsp_d || gBind_Register_rsp_d || gMgmt_Bind_rsp_d)
#if(gInstantiableStackEnabled_d == 1)  
EXTERN  zbApsmeBindEntry_t*  gaBindingCacheTable;
#else
extern  zbApsmeBindEntry_t   gaBindingCacheTable[];
#endif
EXTERN  zbSize_t  gBindingCacheTableCounter;
  #endif
  
  /* ZdpUtils  */
  /* ZigBee 1.1 Required response structure (system Server Dsicovery Response) */
  #if gSystem_Server_Discovery_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gRecover_Bind_Table_rsp_d || gBackup_Source_Bind_rsp_d || gRecover_Source_Bind_rsp_d || gReplace_Device_rsp_d
#if(gInstantiableStackEnabled_d == 1)  
EXTERN  zbSystemServerDiscoveryStore_t*  mSystemServerDiscResponse;
#else
extern  zbSystemServerDiscoveryStore_t   mSystemServerDiscResponse[];
#endif

EXTERN  index_t  mSystemServerDiscoveryResponsesCounter;
  #endif

#if(gInstantiableStackEnabled_d == 1)
}zbZdoPrivateData_t;
#endif

#if(gInstantiableStackEnabled_d == 1)
/* Zdo module init internal used ram global structure */
typedef struct zbZdoInitPrivateData_tag
{
  /* Initialized part of the structure */
  /*ZDOstateMachineHandler*/
  ZdoState_t      gZdoState;
  /* informs application layer which join mode (NLME-JOIN modes) */
  zbNwkJoinMode_t gZDOJoinMode;
  /* running state can be looked up by device type (ZC=0, ZR=1, ZED=2) */
  uint8_t maZdo2AppEvents[3];
  ZdoState_t maZdoRunning[3];
  /* ZdpFrequencyAgility */
  #if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
    uint8_t  gFA_DeviceState;
    /* The Id of the task that will handle the Channel Master State machine. */
    tsTaskID_t gFATaskId;
    /* Initialize all channels with the middle energy value. */
    uint8_t mChannels[16];
  #endif
  
  /* ZdpManager*/
  /*The internal counter for each ZDP message, they are mainly neede to keep sync of the request and responses.*/
  zbCounter_t  gZdpSequence;
  bool_t  mbIsNotFree;
  
  /* ZdpResponses */
  #if gMgmt_Direct_Join_rsp_d
    zbNwkAddr_t  aSrcAddress;
#endif
}zbZdoInitPrivateData_t;
#endif

#if(gInstantiableStackEnabled_d == 1)
/* Zdo module public ram global structure */
typedef struct zbZdoPublicData_tag
{
#endif
  
  /* Initialized part of the structure */
  /* ZdpManager*/
EXTERN  zbSimpleDescriptor_t gZdpEp;
  
  /* ZdpResponses */
  /*-------------------- gaServeMask --------------------
  Table 2.29 Server Mask Bit Assignments

  Bit number:
    0->Primary Trust Center.
    1->Backup  Trust Center.
    2->Primary Binding table cache.
    3->Backup Binding table cache.
    4->Primary Discovery Cache.
    5->Backup Discovery Cache.
    6->Network Manager.
    7-15->Reserved.
*/
EXTERN  zbServerMask_t  gaServerMask;
  
  /******************************************************************************/
  /* Unitialized part of the structure */
  /*ZDOstateMachineHandler*/
EXTERN  ZdoStartMode_t  gZdoStartMode;
EXTERN  ZdoStopMode_t   gZdoStopMode;
  #if gEndDevCapability_d || gComboDeviceCapability_d
  /* original poll rate */
EXTERN  uint16_t        gZdoOrgPollRate;  
  #endif
  /* The pointer used to register the callback function used by the application to get the response form ZDP. */
EXTERN  ZDPCallBack_t  gpZdpAppCallBackPtr;
#if(gInstantiableStackEnabled_d == 1)
}zbZdoPublicData_t;
#endif
/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
#if(gInstantiableStackEnabled_d == 1)
/* pointer used to access instance id variables */
extern zbZdoPrivateData_t* pZbZdoPrivateData; 

/* pointer used to access instance id variables */
extern zbZdoPublicData_t* pZbZdoPublicData;
#endif
/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/
void ZdoInitVariables(uint8_t instId);
#ifdef __cplusplus
}
#endif
#endif /* _ZDO_INTERNAL_VARIABLES_H_ */
