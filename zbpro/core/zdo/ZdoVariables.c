/******************************************************************************
* This file contains declaration of structures and pointers to structures
* used internal zigbee af module 
*  
*
* ( c ) Copyright 2013, Freescale, Inc. All rights reserved.
*
* Freescale Semiconductor Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#include "EmbeddedTypes.h"

#include "zigbee.h"
#include "BeeStack_Globals.h"
#include "nwkcommon.h"

#include "NwkVariables.h"
#include "ApsVariables.h"
#include "AfVariables.h"
#include "ZdoVariables.h"
#include "BeeStackRamAlloc.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/* Application Device Version and flag values */
#if ( gUser_Desc_rsp_d == 1 )&&( gComplex_Desc_rsp_d == 1 )
/* user and complex descriptor both supported */
#define gAppDeviceVersionAndFlag_c 0x30 
#elif ( gUser_Desc_rsp_d == 1 ) 
/* user descriptor supported */
#define gAppDeviceVersionAndFlag_c 0x20
#elif ( gComplex_Desc_rsp_d == 1 )
/* complex descriptor supported */
#define gAppDeviceVersionAndFlag_c 0x10
#else
/* user and complex descriptor both not supported */
#define gAppDeviceVersionAndFlag_c 0x00
#endif

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
#if(gInstantiableStackEnabled_d == 1)
/* pointer used to access instance id variables */
zbZdoPrivateData_t* pZbZdoPrivateData = NULL;

const zbZdoPrivateData_t zbZdoPrivateData ={0};

const zbZdoInitPrivateData_t gZbZdoInitPrivateData =
{
  /*ZDOstateMachineHandler*/
  /* gZdoState */
  gZdoInitialState_c,
  /* informs application layer which join mode (NLME-JOIN modes) */
  /* gZDOJoinMode */
  gAssociationJoin_c,
  /* running state can be looked up by device type (ZC=0, ZR=1, ZED=2) */
  /* maZdo2AppEvents[3] */
  { gZDOToAppMgmtZCRunning_c, gZDOToAppMgmtZRRunning_c, gZDOToAppMgmtZEDRunning_c },
  /* maZdoRunning[3]*/
  { gZdoCoordinatorRunningState_c, gZdoRouterRunningState_c, gZdoEndDeviceRunningState_c},
  /* ZdpFrequencyAgility */
  #if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
    /* gFA_DeviceState */
    gRunning_c,
    /* The Id of the task that will handle the Channel Master State machine. */
    /* gFATaskId */
    gTsInvalidTaskID_c,
    /* Initialize all channels with the middle energy value. */
    /* mChannels[16] */
    {0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F},
  #endif
  
  /* ZdpManager*/
  /*The internal counter for each ZDP message, they are mainly neede to keep sync of the request and responses.*/
  /* gZdpSequence */
  0x02,
  /* mbIsNotFree */
  TRUE
  
  /* ZdpResponses */
  #if gMgmt_Direct_Join_rsp_d
    /* aSrcAddress */
    ,{0xff,0xff}
  #endif
};

/* pointer used to access instance id variables */
zbZdoPublicData_t* pZbZdoPublicData = NULL;

const zbZdoPublicData_t gZbZdoInitPublicData =
{
  /* Initialized part of the structure */
  /* ZdpManager*/
  /* gZdpEp */
  {
  0x00,                        /* End Point (1Byte) */
  0x00,0x00,                   /* Device Description (2Bytes) */
  0x00,0x00,                   /* Profile ID (2Bytes) */
  gAppDeviceVersionAndFlag_c,  /* AppDeviceVersionAndFlag (1Byte) */
  0x00,                        /* NumOfInputClusters(1Byte) */
  NULL,                        /* PointerToInputClusterList (1Byte) */
  0x00,                        /* NumOfOutputClusters (1Byte) */
  NULL                         /* PointerToOutputClusterList (1Byte) */
  },
  
  /* ZdpResponses */
  /* gaServerMask */
  {gServerMask_c},
  0
};

#else /* gInstantiableStackEnabled_d*/

/*****************************zbZdoPrivateData**********************************/  
/* Zdo module internal used ram global data */
  /* Initialized part of the structure */
  /*ZDOstateMachineHandler*/
  ZdoState_t      gZdoState = gZdoInitialState_c;
  /* informs application layer which join mode (NLME-JOIN modes) */
  zbNwkJoinMode_t gZDOJoinMode = gAssociationJoin_c;
  /* running state can be looked up by device type (ZC=0, ZR=1, ZED=2) */
  uint8_t maZdo2AppEvents[3] = { gZDOToAppMgmtZCRunning_c, gZDOToAppMgmtZRRunning_c, gZDOToAppMgmtZEDRunning_c };
  ZdoState_t maZdoRunning[3] = { gZdoCoordinatorRunningState_c, gZdoRouterRunningState_c, gZdoEndDeviceRunningState_c};
  /* ZdpFrequencyAgility */
  #if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
    uint8_t  gFA_DeviceState = gRunning_c;
    /* The Id of the task that will handle the Channel Master State machine. */
    tsTaskID_t gFATaskId = gTsInvalidTaskID_c;
    /* Initialize all channels with the middle energy value. */
    uint8_t mChannels[16] = {0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F};
  #endif
  
  /* ZdpManager*/
  /*The internal counter for each ZDP message, they are mainly neede to keep sync of the request and responses.*/
  zbCounter_t  gZdpSequence = 0x02;
  bool_t  mbIsNotFree = TRUE;
  
  /* ZdpResponses */
  #if gMgmt_Direct_Join_rsp_d
    zbNwkAddr_t  aSrcAddress = {0xff,0xff};
  #endif
  
  /******************************************************************************/
  /* Unitialized part of the structure */
  /* ZdoBindManager */
  zbClusterId_t gaZdpMatchedOutCluster[mEdbMaxClusterMatch_c];
  index_t giMatchIndex;
  #if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d
    /* # of nodes that have requested EDB */
    bool_t gZdoInEndDeviceBind;
    /* only processes one side at a time (code reduction) */
    index_t mEdbCurrentIndex;
    bool_t mEdbError;
	/* time to wait for the 2nd end-device-bind req */
    zdoEdbTicksLeft_t mEdbTicksLeft;
    /* time to wait for the end device bind process  to complete */	
    zdoEdbTicksLeft_t mEdbProcessTicksLeft;
    /* each entry contains a bind-state-machine */	
    zdoEdbTable_t maEdbTable[mEdbMaxEntries_c]; 	
  #endif
  
  /* ZdoMain */  
  anchor_t mNlmeZdoInputQueue;
  anchor_t mAppZdpInputQueue; 
  #if gStandardSecurity_d || gHighSecurity_d
    msgQueue_t mApsmeZdoInputQueue;  
    #if gTrustCenter_d || gComboDeviceCapability_d
      /* The State holder for the Trust center state machine. */
      uint8_t mTrustCenterState;
      /* This two pointers are use to handle the queues fro the trust center state machine.*/
      nlmeZdoMessage_t          *pJoinMsg;
      zbApsmeUpdateDeviceInd_t  *pUpdateMsg;
      msgQueue_t                gJoinIndicationQueue;
      msgQueue_t                gUpdateDeviceQueue;
    #endif/* gTrustCenter_d || gComboDeviceCapability_d */
  #endif/* gStandardSecurity_d || gHighSecurity_d */
  /* counter for stack sleep... app must manage its own. Makes it so reset allows sleep properly */
  zbCounter_t gZdoAllowToSleepCounter;
  /* Various ZDO timers. See ZdoCommon.h for definitions. */
  zbTmrTimerID_t gZdoTimerIDs[8];
  
  /* ZdoNwkManager */
  bool_t gMemoryFreedByApplication;
  
  /*ZDOstateMachineHandler*/
  /* used for both forming and joining in new state machine */
  /* a network discovery results in a list of networks */
  uint8_t     mcDiscoveryRetries;
  /* each network discovery may involve 1 or more active scans */
  uint8_t     mcScanRetries;
  ZdoEvent_t  gZdoEvent;
  bool_t      mDiscoveryFirstTime;
  /* Keeps track of the current rejoin, to always remeber if ti was Seucre or not. */
  bool_t      gZDO_SecureRejoin;
  
  /* ZdpFrequencyAgility */
  #if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
    /* List of routers to send the Energy Scan request */
    nwkAddrList_t *mpFA_ListOfRouters;
    /* This variable tells the current state of the nwkmanager state machine.*/
    uint8_t       gChannelMasterState;
    /*  The amount of incoming report errors to keep track*/
    uint8_t       mErrorReportsReceived;
    /* The amount of Scan reports received during the run of the state machine. */
    uint8_t       mNumScansReceived;
    /* The amount of node doing Scans, this are the routers request to do the scan. */
    uint8_t       mNumScansSent;
    /*   The timer to be use during the error report processing. */
    zbTmrTimerID_t  mErrorReportTimer;
     /* FA Generic millisecond timer. */
    zbTmrTimerID_t  mGenericFATimer;
    #if (gMgmt_NWK_Update_req_d &&(gNetworkManagerCapability_d || gComboDeviceCapability_d))
      /* The timer to be used during to request the  Energy Scan Req to Routers. Used only by the Network Manager */
      zbTmrTimerID_t gFA_ReqEnergyScanToRoutersTimer;
    #endif
    /* Update notify report timer (minute timer). */
    zbTmrTimerID_t  gFA_NotifyTimer;
    /* Keeps track of the scannign reason in the state machone.*/
    uint8_t  mScanReason;
    /* ScanCount is a counter to keep track of the number of scan requests and reports that needs to be done.*/
    uint8_t gScanCount;
    /* Scan channel list in native format. */
    zbChannels_t gaScanChannelList;
    /* For the devices that need to send reports back to the Nwk Mngr, we need to keep around the scanduration.*/
    uint8_t  mFAScanDuration;
  #endif
    
  /* ZdpManager*/
  /*The internal counter for each ZDP message, they are mainly neede to keep sync of the request and responses.*/
  zbCounter_t  gZdpResponseCounter;
  #if gDiscovery_Cache_req_d || gNode_Desc_store_req_d || gPower_Desc_store_req_d || gActive_EP_store_req_d || gSimple_Desc_store_req_d
    /* ZigBee 1.1 Discovery Chache table, Store the address of Discovery Cache Succcess server */
    zdpRecordDiscCacheSuccess_t  gDiscoveryCacheSuccess;
    bool_t  gDiscoveryCacheResponse;
  #endif
  #if gFind_node_cache_req_d
    zdpRecordFindNodeSuccess_t  gFindNodeCacheSuccess;
  #endif /* gFind_node_cache_req_d */
  bool_t gIsABroadcast;
    
  /* ZdpResponses */
  #if gMgmt_Leave_rsp_d
    zbMgmtOptions_t gMgmtSelfLeaveMask;	
    zbNwkAddr_t  mgmtLeaveResponseInfo;
  #endif /* gMgmt_Leave_rsp_d */
  #if gMgmt_NWK_Disc_rsp_d
    index_t  gIndex;
    bool_t   gMgmtNwkDiscReq;
    index_t  gStartIndexforNwkDisc;
  #endif /*gMgmt_NWK_Disc_rsp_d */
  #if gMgmt_NWK_Disc_rsp_d
    strAddrClusterId_t  strAddrClusterId[gMaxArray_d];
  #endif
  #if (gDiscovery_store_rsp_d || gRemove_node_cache_rsp_d || gMgmt_Cache_rsp_d)
    zbCounter_t  mDiscoveryStoreTableEntrys;
  #endif
  #if (gBindCapability_d && (gBind_rsp_d || gUnbind_rsp_d)) || gReplace_Device_rsp_d || gBind_Register_rsp_d
    zbEventProcessQueue_t  maEventProcessQueue[gMaximumEventsToProcess_c];
  #endif
  #if gBkup_Discovery_cache_d || gDiscovery_store_rsp_d || gMgmt_Cache_rsp_d || gRemove_node_cache_rsp_d || gFind_node_cache_rsp_d|| gActive_EP_store_rsp_d || gPower_Desc_store_rsp_d || gNode_Desc_store_rsp_d || gSimple_Desc_store_rsp_d
    discoveryStoreTable_t  gaDiscoveryStoreTable[gMaximumDiscoveryStoreTableSize_c];
  #endif 
  #if gBind_Register_rsp_d || gRecover_Source_Bind_rsp_d || gBackup_Source_Bind_rsp_d || (gBindCapability_d && gBind_rsp_d)
    zbIeeeAddr_t  aDevicesHoldingItsOwnBindingInfo[gMaximumDevicesHoldingBindingInfo_c];
    zbCounter_t    gDeviceHoldingItsOwnBindingInfo_Counter;
  #endif
  #if (gReplace_Device_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRecover_Bind_Table_rsp_d || gBind_Register_rsp_d || gMgmt_Bind_rsp_d)
    zbApsmeBindEntry_t  gaBindingCacheTable[gMaximumBindingCacheTableList_c] = {0};
    zbSize_t  gBindingCacheTableCounter;
  #endif
  
  /* ZdpUtils  */
  /* ZigBee 1.1 Required response structure (system Server Dsicovery Response) */
  #if gSystem_Server_Discovery_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gRecover_Bind_Table_rsp_d || gBackup_Source_Bind_rsp_d || gRecover_Source_Bind_rsp_d || gReplace_Device_rsp_d
    zbSystemServerDiscoveryStore_t  mSystemServerDiscResponse[gMaximumSystemServerciesResponses_c];
    index_t  mSystemServerDiscoveryResponsesCounter;
  #endif
    
/*****************************zbZdoPublicData**********************************/  

  
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
  zbServerMask_t  gaServerMask    = {gServerMask_c};
  
  /******************************************************************************/
  /* Unitialized part of the structure */
  /*ZDOstateMachineHandler*/
  ZdoStartMode_t  gZdoStartMode = 0;
  ZdoStopMode_t   gZdoStopMode = 0;
  #if gEndDevCapability_d || gComboDeviceCapability_d
  /* original poll rate */
  uint16_t        gZdoOrgPollRate = 0;  
  #endif
  /* The pointer used to register the callback function used by the application to get the response form ZDP. */
  ZDPCallBack_t  gpZdpAppCallBackPtr = NULL;

#endif /* gInstantiableStackEnabled_d */ 
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
void ZdoInitVariables(uint8_t instId)
{
  #if(gInstantiableStackEnabled_d == 1)
  /* Init private data structure */
  FLib_MemCpy((void*)pZbStackStuctPointersTable[instId]->pZbZdoPrivateData,
                (void*)&gZbZdoInitPrivateData, 
                (uint8_t)sizeof(zbZdoInitPrivateData_t));
  /* Init public data structure */ 
  FLib_MemCpy((void*)pZbStackStuctPointersTable[instId]->pZbZdoPublicData ,
                (void*)&gZbZdoInitPublicData,
                (uint8_t)sizeof(gZbZdoInitPublicData));
  #else
  (void)instId;
  #endif
}
