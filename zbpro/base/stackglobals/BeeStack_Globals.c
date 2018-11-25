/******************************************************************************
* This file contains the declarations for different tables used in BeeStack.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/
#include "EmbeddedTypes.h"
#include "FunctionLib.h"
#include "NV_Data.h"
#include "TS_Interface.h"

#include "zigbee.h"
#include "BeeStackInterface.h"
#include "BeeStackConfiguration.h"
#include "Beestack_Globals.h"
#include "ZdoNwkInterface.h"
#include "nwkcommon.h"

#include "AppZdoInterface.h"
#include "ApsDataInterface.h"
#include "ZdoApsInterface.h"

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
  #if gStaticAddressingCapability_d
    /* =======================
         STATIC ADDRESSING
    ======================= */
    /* Home Control Lighhting and Test Profile */
    /*Default values of CSkip Table to Assign the Static addresses*/
    #include "CSkipCalc.h"
  #endif /* #endif for Static Addr */
#endif

#include "NwkVariables.h"
#include "ApsVariables.h"
#include "AfVariables.h"
#include "ZDOVariables.h"
#include  "BeeStackRamAlloc.h"
#include "BeeApp.h"
/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/
/***************************** For NWK Layer *********************************/

/****************************DEFINES*******************************************/

/* Which ZigBee device type is this node (only combo devices can change type) */
#if gCoordinatorCapability_d
  #define mZbDeviceType_c gCoordinator_c
#endif

#if gRouterCapability_d
  #define mZbDeviceType_c gRouter_c
#endif

#if gEndDevCapability_d
  #define mZbDeviceType_c gEndDevice_c
#endif

#if gComboDeviceCapability_d
  #define mZbDeviceType_c gInvalidDeviceType_c
#endif


/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Private type Definations
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/***************************** For NWK Layer *********************************/

/******************************CONST*******************************************/

const nvmNwkData_t gDefaultNwkData = 
{
  { mDefaultValueOfExtendedAddress_c },
  { mDefaultValueOfMacCapFlags_c },
  { mZbDeviceType_c },
  { mDefaultLogicalChannel_c },
  { mDefaultParentShortAddress_c },       // aParentShortAddress
  { mDefaultParentLongAddress_c },        // aParentLongAddress
  { 0x00 },                               // deviceDepth
  { 0x00 },                               // nwkUpdateId;
  { mDefaultValueOfPermitJoinDuration_c } // permitJoining
};

const uint16_t gNibPassiveAckTimeout = gNwkInfobasePassiveAckTimeout_c;

const bool_t   gNibUseTreeRouting = mNwkUseTreeRouting_c;

const uint8_t  gNibMaxBroadcastRetries = gNwkInfobaseMaxBroadcastRetries_c;

const uint8_t gNibReportConstantCost = mNwkRptConstantCost_c;

const bool_t gNibSymLink = mNwkSymmetryLink_c;

const uint8_t gNibAddrAlloc = mNwkAddressAlloc_c;

const uint8_t gNibUniqueAddr = mNwkUniqueAddr_c;

const uint8_t gScanDuration = gScanDuration_c;

/* Age limit in time for end devices using SP1*/
const uint8_t gNwkAgingTimeForEndDevice = gNwkAgingTimeForEndDevice_c;

/* Age limit in time for router devices using SP1*/
const uint8_t gNwkAgingTimeForRouter = gNwkAgingTimeForRouter_c;

/* Age limit in time for sibling router devices using SP1*/
const uint8_t gNwkAgingTimeForSiblingRouter = gNwkAgingTimeForSiblingRouter_c;

/* Age limit in time to expire route and source route table entries*/
const uint8_t gMinutesToExpireRoute = gMinutesToExpireRoute_c;

/* This global varible is used to change the PID upper Limit */
const zbPanId_t gaPIDUpperLimit = {0xFE, 0xFF};

#if gNwkSymLinkCapability_d
/* 053474r17 ZigBee Spec.
   The time in seconds between link status command frames. 
   Default 0x0f */

const uint8_t gNwkLinkStatusMultipleFramesPeriod = gNwkLinkStatusMultipleFramesPeriod_c;

const bool_t gNwkSendLinkStatusOnDevAnnc = gNwkSendLinkStatusOnDevAnnc_c;
#endif

const uint8_t gNibNwkRouterAgeLimit =  gNetworkRouterAgeLimit_c;

/* Energy Threshhold used when performing ED Scan */
const uint8_t  gNwkEnergyLevelThresHold = gNwkEnergyLevelThresHold_d;

/*  Default Transmit Failure Counter, indicates how many times have to fail a 
  transmition to a specific device before a route repair is initiated */
  
/* This variable is used to set the protocol ID uesd in Beacon Payload, This
  can be changed by the user with a proper value.
  Note:Two devices with diffrent protocol Id will never join*/
const uint8_t gNwkProtocolId = gDefaultValueOfNwkProtocolId_c;

/* This variable is used for setting the Radius Counter parameter to the
   default value which is 2 times the maximun depth of a network */
const zbCounter_t gDefaultRadiusCounter = 2 * gNwkMaximumDepth_c;

/* This variable is used to hold the max children, that a ZC/ZR can accept.
  Note: This has to be the same or greater than Nwk Max Routers */
const uint8_t gNibMaxChildren=gNwkMaximumChildren_c;

/* This variable is used to hold the max routers, that a ZC/ZR can accept.
  Note: This has to be the same or greater than Nwk Max Children */
const uint8_t gNibMaxRouter=gNwkMaximumRouters_c;

/* This variable is used to hold the max depth at which the device can be present
  Note: The ZR at Last depth cannot Accept children */
const uint8_t gNibMaxDepth=gNwkMaximumDepth_c;

/* Time duration in seconds that a broadcast message needs to encompass the
  entire network. Devices using the ZigBee stack profile must set: 
  nwkBroadcastDeliveryTime = 3 secs */
const uint32_t gNibNetworkBroadcastDeliveryTime	= gNwkInfobaseBroadcastDeliveryTime_c;

/* The buffer used is as big as a small bufffer. */
const uint8_t gNwkRejoinStateMachineSize = gNwkRejoinStateMachineSize_c;
/* The size of the buffer divided by the amount of entries. */
const uint8_t gNwkRejoinStateMachineAmountOfEntries = gNwkRejoinStateMachineAmountOfEntries_c;
/* The amount of time in millisecodn to exppire the Nwk Rejoin state machine. */
const zbTmrTimeInMilliseconds_t  gNwkRejoinExpirationTime = gNwkRejoinExpirationTime_c;

/* Sets the maximum number of failures to tolarate in the network layer before
  it send a report up. FA module. */
const uint8_t gMaxNumberOfTxAttempts = (gMaxNumberOfTxAttempts_c+1);

const uint8_t gHttMaxIndirectEntries = gHttMaxIndirectEntries_c;

const bool_t gNwkSmartSiblingReplacement = gNwkSmartSiblingReplacement_c;

#if gStandardSecurity_d || gHighSecurity_d
  /* Set the defualt transmision options for SSP. */
  #if gApsLinkKeySecurity_d
    /* default to APS layer security. */
    const uint8_t gTxOptionsDefault = (gApsTxOptionSecEnabled_c);
  #else
    /* default to no APS layer security (both ZigBee and ZigBee Pro) */
    const uint8_t gTxOptionsDefault = gApsTxOptionNone_c;
  #endif
#endif
    
#if ( !gNwkBroadcastPassiveAckRetryCapability_d )
  const uint8_t gBroadcastDeliveryInterval_c = 250;
  const uint8_t gBroadcastJitterInterval_c = 10;
  /* This value determines the number of ticks before a broadcast transaction entry expires
  in the BroadcastTransactionTable. Changed to constant gNwkInfobaseBroadcastDeliveryTime_c*/
  const uint8_t gBroadcastDeliveryNumTicks = ( gNwkInfobaseBroadcastDeliveryTime_c / 250 /* = gBroadcastDeliveryInterval_c */) ;
#endif ( !gNwkBroadcastPassiveAckRetryCapability_d ) 

/*This variable hold the value of Maximum route table Entries supported, this
  can be changed by the user at compile time by updating
  gNwkInfobaseMaxRouteTableEntry_c macro by proper value */
const uint8_t gMaxRouteTableEntries = gNwkInfobaseMaxRouteTableEntry_c;

const uint8_t gRouteRequestRetryInterval = 8;

const uint8_t gRouteDiscoveryExpirationInterval = 150;

const uint8_t gRouteDiscoveryExpirationNumTicks = gNibNwkRouteDiscoveryTime_c / 150 /* = gRouteDiscoveryExpirationInterval*/;

const uint8_t gDefaultRetriesStochasticAddrLocalDevice = gDefaultRetriesStochasticAddrLocalDevice_c;

const bool_t gR20BroadcastLeaveOnZed = FALSE;  

/***************************** For Inter-Pan *********************************/
const uint8_t gInterPanCommunicationEnabled = gInterPanCommunicationEnabled_c;

#if gInterPanCommunicationEnabled_c
  /*NOTE: DO NOT CHANGE THESE POINTERS */
  const InterPanFuncInd_t pInterPanProcessIndication  = InterPan_ProcessDataIndication;
  const InterPanFuncCnf_t pInterPanProcessConfirm     = InterPan_ProcessConfirm;
#else
  const InterPanFuncInd_t pInterPanProcessIndication  = NULL;
  const InterPanFuncCnf_t pInterPanProcessConfirm     = NULL;
#endif
/***************************** For Pan Id Condflict **************************/

#if gConflictResolutionEnabled_d && (( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) && gNwkPanIdConflict_d)
  const PIDConflictDetect_t pPIDDetection           = ActualDetectPanIdConflict;
  const PIDConflictResol_t  pPIDResolution          = ActualProcessPanIdConflictDetected;
#else
  const PIDConflictDetect_t pPIDDetection           = NULL;
  const PIDConflictResol_t  pPIDResolution          = NULL;
#endif

/***************************** Stack Hooks **************************/
#if( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) && gNwkSymLinkCapability_d
  const NodeNotInParentLinkStatus_t   pfNodeNotInParentLinkStatus = NodeNotInParentLinkStatus;
#else
  const NodeNotInParentLinkStatus_t   pfNodeNotInParentLinkStatus = NULL;
#endif
const UpdateNtEntryLQI_t            pfUpdateNtEntryLQI  =  UpdateNtEntryLQI;   

#if(gInstantiableStackEnabled_d == 1)  
/* Initialization structure for gaZbBeeStackNwkGlobals */
const zbBeeStackNwkGlobalsInit_t gZbBeeStackNwkGlobalsInit=
{
  mDefaultValueOfNwkLeaveRequestAllowed_c,                       /* bool_t gNibNwkLeaveRequestAllowed;*/
  gNwkTransPersistenceTime_c,                                    /* gNibTransactionPersistenceTime */
  (gNwkTransPersistenceTime_c * gSuperFrameSpecLsbBO_c * 1.024), /* gNwkPurgeTimeout */
  mNwkRouteDiscRetriesPermitted_c,                               /* gNibRouteDiscoveryRetriesPermitted*/
  {mDefaultNwkAvailableAddress_c},                               /* gNibAvailableAddresses*/
  {mDefaultNwkAddressIncrement_c},                               /* gNibAddressIncrement*/
  gNwkAgingTickScale_c,                                          /* gNwkAgingTickScale*/
  #if gNwkSymLinkCapability_d
    gNetworkLinkStatusPeriod_c,                                  /* gNibNwkLinkStatusPeriod  */
  #endif
  mUseMultiCast_c,                                               /* gNibNwkUseMulticast */
  mDefaultUseMultiCastForApsGroupDataRequest_c,                  /* gNwkUseMulticastForApsGroupDataRequest*/
  0x07,                                                          /* gMulticastMaxNonMemberRadius_c*/
  gNwkHighRamConcentrator_d                                      /* gNwkHighRamConcentrator*/
};
#endif  
/********************************RAM*******************************************/

#if(gInstantiableStackEnabled_d == 1)

zbBeeStackNwkGlobals_t* pZbBeeStackNwkGlobals = NULL;

#else /*(gInstantiableStackEnabled_d == 1)*/

/* Nwk struct of globals */  
  bool_t   gNibNwkLeaveRequestAllowed = mDefaultValueOfNwkLeaveRequestAllowed_c;
  uint16_t gNibTransactionPersistenceTime = gNwkTransPersistenceTime_c;
  /*PurgeTime is : nwkPersistenceTime * SuperFrame  * 1/aBaseSuperFrameDuration */
  uint16_t gNwkPurgeTimeout = (gNwkTransPersistenceTime_c * gSuperFrameSpecLsbBO_c * 1.024);
  uint8_t gNibRouteDiscoveryRetriesPermitted = mNwkRouteDiscRetriesPermitted_c;
  zbNwkAddr_t gNibAvailableAddresses = {mDefaultNwkAvailableAddress_c};
  zbNwkAddr_t gNibAddressIncrement = {mDefaultNwkAddressIncrement_c};
  /* Scale used to define the resolution for expiring the entries on the NT */
  uint8_t gNwkAgingTickScale = gNwkAgingTickScale_c;
  #if gNwkSymLinkCapability_d
  /* 053474r17 ZigBee Spec.
     The time in seconds between link status command frames. 
                    Default 0x0f */
  uint8_t gNibNwkLinkStatusPeriod = gNetworkLinkStatusPeriod_c;
  #endif
  /* 
    Default:  TRUE
    A flag determining the layer where multicast messaging occurs.
    TRUE = multicast occurs at the network layer.
    FALSE= multicast occurs at the APS layer and using the APS header.*/
  /* Don't touch this line */
  bool_t gNibNwkUseMulticast = mUseMultiCast_c;
  bool_t gNwkUseMulticastForApsGroupDataRequest = mDefaultUseMultiCastForApsGroupDataRequest_c;
  uint8_t gMulticastMaxNonMemberRadius_c = 0x07;
  uint8_t gNwkHighRamConcentrator = gNwkHighRamConcentrator_d;
  /************************************unit************************************/
/* uninit part os structure */
  nvmNwkData_t gNwkData;
  zbCounter_t gNibSequenceNumber;
  /* Current state of network state machine */
  uint8_t gNwkState;
  uint16_t gNibTxTotal;
  uint16_t gNibTxTotalFailures;
  bool_t gDeviceAlreadySentRouteRecord;
  bool_t gActAsZed;
  #if gComboDeviceCapability_d
    bool_t gLpmIncluded;
  #endif
  /* This variable contains sorted the indexes of the neighbor table in ascending order.
       This array only is used when NwkSymLink is TRUE and is used by Link Status Command 
       to build the link status information list. This array has the same length of the gaNeighborTable */
  #if gNwkSymLinkCapability_d && (( gCoordinatorCapability_d || gRouterCapability_d ) || gComboDeviceCapability_d)
  uint8_t gaNeighborTableAscendantSorted[gNwkInfobaseMaxNeighborTableEntry_c];
  #endif
  /* This table will exist only if a discovery network has been performed 
          previously and it last only during joining or forming a network */
  neighborTable_t *gpDiscoveryTable;
  extendedNeighborTable_t *gpExtendedDiscoveryTable;
  /* This value will change depending on the size of memory allocated for
        gpDiscoveryTable and gpExtendedDiscoveryTable */
  index_t gMaxDiscoveryTableEntries;
  /* this table keeps the pointer to the messages that are waiting to be routed.
         waiting for a route discovery or device authentication */
  packetsOnHoldTable_t gaPacketsOnHoldTable[gPacketsOnHoldTableSize_c];
  /* this table keeps track of every unicast data request sent to the NLL */
  handleTrackingTable_t gaHandleTrackingTable[gHandleTrackingTableSize_c];
  /* keeps track of each rejoin reuqest until copleted. */
  nwkRejoinEntry_t *gpNwkRejoinStateMachine;
  /* indicates how many entries of the handleTrackingTable are occupied by sleeping devices */
  zbCounter_t gHttIndirectEntries;
  #if gStandardSecurity_d || gHighSecurity_d
    zbIncomingFrameCounterSet_t  gaIncomingFrameCounterSet1[gNwkInfobaseMaxNeighborTableEntry_c];
    zbIncomingFrameCounterSet_t  gaIncomingFrameCounterSet2[gNwkInfobaseMaxNeighborTableEntry_c]; 
    #if gComboDeviceCapability_d
      bool_t gTrustCenter;
    #endif
  #endif /*gStandardSecurity_d || gHighSecurity_d*/
  #if ( !gNwkBroadcastPassiveAckRetryCapability_d ) || (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
    /* Broadcast Transaction Table, size of this table is decided by the number
          of broadcast requests that handled by the network layer simultaneously */
    broadcastTransactionTable_t gaBroadcastTransactionTable[gMaxBroadcastTransactionTableEntries_c];
  #endif /* ( !gNwkBroadcastPassiveAckRetryCapability_d ) || (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) */
      
  #if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) && gRnplusCapability_d 
    routeTable_t gaRouteTable[gNwkInfobaseMaxNeighborTableEntry_c];
  #endif
  /* Route Discovery Table Entries, to update the number of route Discovery
          table entries, the value of gNwkRoutingMaxRouteDiscoveyTableEntry_c macro
         needs to be updated */
  routeDiscoveryTable_t gaRouteDiscoveryTable[gNwkRoutingMaxRouteDiscoveyTableEntry_c];
  /* This is the variable that contains our neighbors information */
  neighborTable_t gaNeighborTable[gNwkInfobaseMaxNeighborTableEntry_c];
  
  #if gNwkSmartSiblingReplacement_c
    uint8_t gNeighborRelationsTable[gNwkInfobaseMaxNeighborTableEntry_c * (gNwkInfobaseMaxNeighborTableEntry_c/8 + 1)];
  #else
    uint8_t gNeighborRelationsTable[1] ={0};
  #endif
  /*This variable is set to true when a device of the network detects a PanId conflict 
         after it joined the network.*/
  bool_t gNwkPanIdConflictDetected;
#if gConcentratorFlag_d    
  sourceRouteTable_t gaSourceRouteTable[gNwkInfobaseMaxSourceRouteTableEntry_c];
#else
  sourceRouteTable_t              gaSourceRouteTable[1];
#endif  
  rejoinConfigParams_t gRejoinConfigParams;
#endif  

//const zbBeeStackNwkGlobals_t zbBeeStackNwkGlobals ={0};


/***************************** For APS Layer *********************************/

/******************************CONST*******************************************/

/*
  APS Information Base (AIB) ID=0xc1, Binding Table

  The binding table contains a list of "bindings" from a single source endpoint to 
  endpoints on other nodes or groups. Use zbAddrMode_t gZbAddrModeIndirect_c in the 
  AF_DataRequest() to instruct BeeStack to use the binding table on the data request.

  This is the "internal" structure for a binding table. For the ZigBee-style, use the
  ZDP commands.
*/

const uint8_t gApsMaxClusterList = gMaximumApsBindingTableClusters_c;

/* You know the question Neo.... What is the matrix? */
const uint8_t gEpMaskSize = gEndPointsMaskSizeInBytes_c;


/* specifies the # of retries APS uses */
const uint8_t gApsMaxRetries = gApsMaxRetries_c;

/* specifies the wait duration on APS ACKs before retrying */
const uint16_t gApsAckWaitDuration = gApsAckWaitDuration_c;

/* specifies the wait for a nwk address response to complete binding information */
const uint32_t gAps64BitAddressResolutionTimeout = gAps64BitAddressResolutionTimeout_c;

const zbChannels_t gaFullChannelList = 
{
  (uint8_t)(gFullChannelList_c & 0xff),
  (uint8_t)((gFullChannelList_c>>8) & 0xff),
  (uint8_t)((gFullChannelList_c>>16) & 0xff),
  (uint8_t)((gFullChannelList_c>>24) &0xff)
};

const uint8_t  gApsChannelTimerDefault = gApsChannelTimerDefault_c;

/************************Fragmentation Capability CONST************************/
/* is fragmentation enabled? */
const bool_t gfFragmentationEnabled = gFragmentationCapability_d;

#if gEndDevCapability_d || gComboDeviceCapability_d
const uint16_t gApsFragmentationPollRate = gApsFragmentationPollTimeOut_c;
#endif
/*************************APS SECURITY CONST***********************************/

/*  Use this on the security functions to avoid having #if in the lib.*/
#if gSKKESupported_d
  const uint8_t  gSkkeCommand = sizeof(zbApsmeSKKECommand_t);
#else
  const uint8_t  gSkkeCommand = 0;
#endif

 /*************************APS SECURITY*****************************************/
  
#if gStandardSecurity_d || gHighSecurity_d

  const uint8_t gAllowNonSecurePacketsInSecureMode = gAllowNonSecure_d;

#endif
  
const uint16_t gDefaultApsSecurityTimeOutPeriod = mDefaultApsSecurityTimeOutPeriod_c;  
/******************************RAM*******************************************/

#if(gInstantiableStackEnabled_d == 1) 
/* APS global pointer to beestack structure */
zbBeeStackApsGlobals_t* pZbBeeStackApsGlobals = NULL;

#else

  uint8_t                         gApsLastChannelEnergy;
  uint8_t                         gApsChannelFailureRate;
  uint8_t                         gApsChannelTimer;
  addrMapIndex_t                  gGlobalAddressMapCounter;
  zbApsDeviceKeyPairSet_t         gaApsDeviceKeyPairSet[gApsMaxSecureMaterialEntries_c];
  zbAESKey_t                      gaApsKeySet[gApsMaxLinkKeys_c];
  
  #if (gApsAckCapability_d)
    zbTmrTimerID_t                 gaApsTxTimerIDTable[gApsMaxDataHandlingCapacity_c];
  #endif /*gApsAckCapability_d*/

  /* Memory Allocated for AddressMap table */
  zbTmrTimerID_t                    gaApsTxTimerIDTable[gApsMaxDataHandlingCapacity_c];
  uint8_t                         gaApsAddressMapBitMask[gApsAddressMapBitMaskInBits_c];
  zbAddressMap_t                  gaApsAddressMap[gApsMaxAddrMapEntries_c];

  /* ID=0xc1, Memory Allocated for Binding table */
  apsBindingTable_t               gaApsBindingTable[gMaximumApsBindingTableEntries_c];

  /* Memory allocated for GroupTable */
  zbGroupTable_t                 gaApsGroupTable[gApsMaxGroups_c];
  
  apsTxTable_t                   gaApsTxTable[gApsMaxDataHandlingCapacity_c];
  
  apsTxFragCtrlSettings_t        apsTxFragCtrlSettings[gApsMaxDataHandlingCapacity_c];

  apsRxFragmentationTable_t      gApsRxFragmentationTable[gApsRxFragmentationCapacity_c];

/* duplicate rejection table */
  zbApsDuplicateRejectionTable_t gaApsDuplicateRejectionTable[gApscMinDuplicationRejectionTableSize_c];
  
  beeStackParameters_t           gBeeStackParameters;
  /*************************APS SECURITY*****************************************/ 
  /* NOT saved in NVM */
  zbKeyEstablish_t*              apSKKEMaterial[gDefaulEntriesInSKKEStateMachine_c];
  
  #if gHighSecurity_d
  /*  This variable is an array of pointers, each entry keeps the state machine values for
      a Entity authentication process. */
  zbEntityAuthentication_t  *gapEntityAuthenticationMaterial[gDefaultEntriesInEAStateMachine_c];
  #endif
  /* Permissions Configuration Table */
  #if gApsMaxEntriesForPermissionsTable_c
  /* Permissions Configuration Table */
  permissionsTable_t  gaPermissionsTable[gApsMaxEntriesForPermissionsTable_c];
  uint8_t gPermissionsTableCount;
#endif  /* (gApsMaxEntriesForPermissionsTable_c) */
    
#endif


/***************************** BeeStack*********************************/

/******************************CONST*******************************************/

const uint8_t zdoTxOptionsDefault = zdoTxOptionsDefault_c;

#if(gInstantiableStackEnabled_d == 0)  
const zbCommissioningAttributes_t gSAS_Rom =
#else
const zbCommissioningAttributes_t gSAS_Rom[2] = {
#endif  
{
  /*** Startup Attribute Set (064699r12, section 6.2.2.1) ***/
  {mDefaultNwkShortAddress_c},        /* x shortAddress (default 0xff,0xff) */
  {mDefaultNwkExtendedPANID_c},       /* x nwkExtendedPANId */
  {mDefultApsUseExtendedPANID_c},     /* x apsUseExtendedPANId */
  {mDefaultValueOfPanId_c},           /* x panId */
  { (uint8_t)(mDefaultValueOfChannel_c & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>8) & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>16) & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>24) &0xff)
  },                                          /* x channelMask */
  {mNwkProtocolVersion_c},                    /* x protocolVersion, always 0x02=ZigBee 2006, 2007 */
  {gAppStackProfile_c},                       /* x stackProfile 0x01 or 0x02 */
  {gStartupControl_Associate_c},              /* startupControl */
  {mDefaultValueOfTrustCenterLongAddress_c},  /* x trustCenterAddress */
  {mDefaultValueOfTrustCenterMasterKey_c},    /* trustCenterMasterKey */
  {mDefaultValueOfNetworkKey_c},              /* x networkKey */
  {gApsUseInsecureJoinDefault_c},             /* x useInsecureJoin */
  {mDefaultValueOfTrustCenterLinkKey_c},      /* preconfiguredLinkKey (w/ trust center) */
  {mDefaultValueOfNwkActiveKeySeqNumber_c},   /* x networkKeySeqNum */
  {mDefaultValueOfNwkKeyType_c},              /* x networkKeyType */
  {gNwkManagerShortAddr_c},                   /* x networkManagerAddress, little endian */

  /*** Join Parameters Attribute Set (064699r12, section 6.2.2.2) ***/
  {mDefaultValueOfNwkScanAttempts_c}, /* x # of scan attempts */
  { (mDefaultValueOfNwkTimeBwnScans_c & 0xff),
    (mDefaultValueOfNwkTimeBwnScans_c >> 8)
  },                                  /* x time between scans(ms) */
  {(mDefaultValueOfRejoinInterval_c&0xff),
   (mDefaultValueOfRejoinInterval_c>>8)
  },                                  /* x rejoin interval (sec) */
  {(mDefaultValueOfMaxRejoinInterval_c & 0xff),
   (mDefaultValueOfMaxRejoinInterval_c >> 8)
  },                                  /* x maxRejoinInterval (sec) */

  /*** End-Device Parameters Attribute Set (064699r12, section 6.2.2.3) ***/
  {(mDefaultValueOfIndirectPollRate_c & 0xff),
   (mDefaultValueOfIndirectPollRate_c >> 8)
  },                                  /* x indirectPollRate(ms) */
  {gMaxNwkLinkRetryThreshold_c},      /* x parentRetryThreshold */

  /*** Concentrator Parameters Attribute Set (064699r12, section 6.2.2.4) ***/
  {gConcentratorFlag_d},              /* x concentratorFlag */
  {gConcentratorRadius_c},            /* x concentratorRadius */
  {gConcentratorDiscoveryTime_c}     /* x concentratorDiscoveryTime */
}
#if(gInstantiableStackEnabled_d == 0)  
;
#else /* Second pan gSAS_Rom */
,{
  /*** Startup Attribute Set (064699r12, section 6.2.2.1) ***/
  {mDefaultNwkShortAddress_c},        /* x shortAddress (default 0xff,0xff) */
  {mDefaultNwkExtendedPANID_c},       /* x nwkExtendedPANId */
  {mDefultApsUseExtendedPANID_c},     /* x apsUseExtendedPANId */
  {mDefaultValueOfPanId2_c},           /* x panId */
  { (uint8_t)(mDefaultValueOfChannelPan2_c & 0xff),
    (uint8_t)((mDefaultValueOfChannelPan2_c>>8) & 0xff),
    (uint8_t)((mDefaultValueOfChannelPan2_c>>16) & 0xff),
    (uint8_t)((mDefaultValueOfChannelPan2_c>>24) &0xff)
  },                                          /* x channelMask */
  {mNwkProtocolVersion_c},                    /* x protocolVersion, always 0x02=ZigBee 2006, 2007 */
  {gAppStackProfile_c},                       /* x stackProfile 0x01 or 0x02 */
  {gStartupControl_Associate_c},              /* startupControl */
  {mDefaultValueOfTrustCenterLongAddress_c},  /* x trustCenterAddress */
  {mDefaultValueOfTrustCenterMasterKey_c},    /* trustCenterMasterKey */
  {mDefaultValueOfNetworkKey_c},              /* x networkKey */
  {gApsUseInsecureJoinDefault_c},             /* x useInsecureJoin */  
  {mDefaultValueOfTrustCenterLinkKey_c},      /* preconfiguredLinkKey (w/ trust center) */
  {mDefaultValueOfNwkActiveKeySeqNumber_c},   /* x networkKeySeqNum */
  {mDefaultValueOfNwkKeyType_c},              /* x networkKeyType */
  {gNwkManagerShortAddr_c},                   /* x networkManagerAddress, little endian */

  /*** Join Parameters Attribute Set (064699r12, section 6.2.2.2) ***/
  {mDefaultValueOfNwkScanAttempts_c}, /* x # of scan attempts */
  { (mDefaultValueOfNwkTimeBwnScans_c & 0xff),
    (mDefaultValueOfNwkTimeBwnScans_c >> 8)
  },                                  /* x time between scans(ms) */
  {(mDefaultValueOfRejoinInterval_c&0xff),
   (mDefaultValueOfRejoinInterval_c>>8)
  },                                  /* x rejoin interval (sec) */
  {(mDefaultValueOfMaxRejoinInterval_c & 0xff),
   (mDefaultValueOfMaxRejoinInterval_c >> 8)
  },                                  /* x maxRejoinInterval (sec) */

  /*** End-Device Parameters Attribute Set (064699r12, section 6.2.2.3) ***/
  {(mDefaultValueOfIndirectPollRate_c & 0xff),
   (mDefaultValueOfIndirectPollRate_c >> 8)
  },                                  /* x indirectPollRate(ms) */
  {gMaxNwkLinkRetryThreshold_c},      /* x parentRetryThreshold */

  /*** Concentrator Parameters Attribute Set (064699r12, section 6.2.2.4) ***/
  {gConcentratorFlag_d},              /* x concentratorFlag */
  {gConcentratorRadius_c},            /* x concentratorRadius */
  {gConcentratorDiscoveryTime_c}     /* x concentratorDiscoveryTime */
}};
#endif  


#ifdef gHostApp_d
  /* Denotes the current Software Version */
  const uint8_t gaSoftwareVersion[ 2 ] = { gBeeStackVerMajor_c, gBeeStackVerMinor_c };
#endif

/* the NVM set is stored in NVM. See gaNvNwkDataSet in NV_Data.c */
#if(gInstantiableStackEnabled_d == 1) 
const beeStackConfigParams_t  gBeeStackInitConfig =
#else
beeStackConfigParams_t gBeeStackConfig = 
#endif
{
  mDefaultValueOfBeaconOrder_c,                             // uint8_t beaconOrder;
  mDefaultValueOfBatteryLifeExtension_c,                    // bool_t batteryLifeExtn;
  mDefaultValueOfApsFlagsAndFreqBand_c,                     // uint8_t frequency band used by the radio
  mDefaultValueOfManfCodeFlags_c,                           // uint8_t array[2] default Value of Manufacturer Code Flag
  (gMaxRxTxDataLength_c - gAsduOffset_c),                   // uint8_t default value of maximum buffer size
  mDefaultValueOfMaxTransferSize_c,                         // uint8_t array[2] default Value od Maximum Transfer Size
  gServerMask_c,                                            // uint8_t array[2] the services server mask
  mDefaultValueOfMaxTransferSize_c,                         // uint8_t array[2] default Value of Maximum Outgoing Transfer Size
  dummyDescCapField,                                        // uint8_t descriptor capability field
  mDefaultValueOfCurrModeAndAvailSources_c,                 // uint8_t currModeAndAvailSources;
  mDefaultValueOfCurrPowerSourceAndLevel_c,                 // uint8_t currPowerSourceAndLevel;
  mDefaultValueOfNwkDiscoveryAttempts_c,                    // uint8_t gNwkDiscoveryAttempts;
  mDefaultValueOfNwkFormationAttempts_c,                    // uint8_t gNwkFormationAttempts;
  mDefaultValueOfNwkOrphanScanAttempts_c,                   // uint8_t gNwkOrphanScanAttempts;
  mDefaultValueOfDiscoveryAttemptsTimeOut_c,                // uint32_t gDiscoveryAttemptsTimeOut;
  mDefaultValueOfFormationAttemptsTimeOut_c,                // uint32_t gFormationAttemptsTimeOut;
  mDefaultValueOfOrphanScanAttemptsTimeOut_c,               // uint32_t gOrphanScanAttemptsTimeOut;
#if( gEndDevCapability_d || gComboDeviceCapability_d)
  mDefaultValueOfAuthenticationPollTimeOut_c,               // uint16_t gAuthenticationPollTimeOut;
  mDefaultValueOfConfirmationPollTimeOut_c,                 // uint16_t gConfirmationPollTimeOut;
#endif
  mDefaultValueOfAuthTimeOutDuration_c,                     // uint16_t gAuthenticationTimeOut;
  FALSE,//mDefaultValueOfNwkKeyPreconfigured_c,             // bool_t gNwkKeyPreconfigured;
  mDefaultValueOfNwkSecurityLevel_c,                        // uint8_t gNwkSecurityLevel;
  mDefaultValueOfNwkSecureAllFrames_c,                      // bool_t gNwkSecureAllFrames;
  0xFF,                                                     // zbKeyType_t
  mDefaultValueOfLpmStatus_c                                // bool_t lpmStatus;
};

#if gUser_Desc_rsp_d || gUser_Desc_conf_d
const zbUserDescriptorPkg_t gUserDescInit =
{
  0x09,
  'F','r','e','e','s','c','a','l','e',' ',' ',' ',' ',' ',' ',' '
};
#endif

#if gComplex_Desc_rsp_d
const configComplexDescriptor_t gComplexDescInit =
{
  mDefaultValueOfComplexDescFieldCount_c ,
  mDefaultValueOfComplexDescLangCharSet_c,
  mDefaultValueOfComplexDescManufactureName_c,
  mDefaultValueOfComplexDescModelName_c,
  mDefaultValueOfComplexDescSerialNumber_c,
  mDefaultValueOfComplexDescDeviceUrl_c,
  mDefaultValueOfComplexDescIcon_c,
  mDefaultValueOfComplexDescIconUrl_c
};
#endif

#if(gInstantiableStackEnabled_d == 0)     
  /* nwk */
  const  uint8_t gMaxNeighborTableEntries = gNwkInfobaseMaxNeighborTableEntry_c;
  #if gNwkSmartSiblingReplacement_c
  const  uint16_t gMaxNeighborRelationsTableEntries = gNwkInfobaseMaxNeighborTableEntry_c * (gNwkInfobaseMaxNeighborTableEntry_c/8 + 1);
  #else
  const  uint16_t gMaxNeighborRelationsTableEntries = 1;
  #endif
  const  uint8_t gIncomingFrameCounterSetLimit = gNwkInfobaseMaxNeighborTableEntry_c;
  const  uint8_t gPacketsOnHoldTableSize = gPacketsOnHoldTableSize_c;
  const  uint8_t gHandleTrackingTableSize = gHandleTrackingTableSize_c;
#if ( !gNwkBroadcastPassiveAckRetryCapability_d ) || (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)   
  const  uint8_t gMaxBroadcastTransactionTableEntries = gMaxBroadcastTransactionTableEntries_c;
#endif  
  const  uint8_t gMaxRouteDiscoveryTableEntries = gNwkRoutingMaxRouteDiscoveyTableEntry_c;
#if gConcentratorFlag_d
  const  uint8_t gMaxSourceRouteEntries = gNwkInfobaseMaxSourceRouteTableEntry_c;
#else
  const  uint8_t gMaxSourceRouteEntries = 1;
#endif
  /* aps */
const  uint8_t giApsDeviceKeyPairCount = gApsMaxSecureMaterialEntries_c;
const  uint8_t gApsKeySetCount = gApsMaxLinkKeys_c;
const  uint8_t gApsAddressMapBitMaskInBits = gApsAddressMapBitMaskInBits_c;
const  uint8_t gApsMaxAddressMapEntries = gApsMaxAddrMapEntries_c;
const  uint8_t gApsMaxBindingEntries = gMaximumApsBindingTableEntries_c;
const  uint8_t gApsMaxGroups = gApsMaxGroups_c;
const  uint8_t gMaxApsTxTableEntries = gApsMaxDataHandlingCapacity_c;
const  uint8_t gMaxApsRxFragmentationTableEntries = gApsRxFragmentationCapacity_c;
const  uint8_t giApsDuplicationRejectionTableSize = gApscMinDuplicationRejectionTableSize_c;
const  uint8_t gSKKEStateMachineSize = gDefaulEntriesInSKKEStateMachine_c;
  #if gHighSecurity_d 
const  uint8_t gEAMaterialSize = gDefaultEntriesInEAStateMachine_c;
  #endif
  #if gApsMaxEntriesForPermissionsTable_c
const    uint8_t gApsMaxEntriesForPermissionsTable = gApsMaxEntriesForPermissionsTable_c;
  #endif /* gApsMaxEntriesForPermissionsTable_c */
  #if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) && gDefaultValueOfMaxEntriesForExclusionTable_c
const    uint8_t gExclusionMax = gDefaultValueOfMaxEntriesForExclusionTable_c;
  #endif
  #if gICanHearYouTableCapability_d
const    uint8_t gICanHearYouMax = gDefaultValueOfMaxEntriesForICanHearYouTable_c;
  #endif
const    uint8_t gNoOfEndPoints = (gNumberOfEndPoints_c + 2);
  #if gBkup_Discovery_cache_d || gDiscovery_store_rsp_d || gMgmt_Cache_rsp_d || gRemove_node_cache_rsp_d || gFind_node_cache_rsp_d|| gActive_EP_store_rsp_d || gPower_Desc_store_rsp_d || gNode_Desc_store_rsp_d || gSimple_Desc_store_rsp_d  
const    uint8_t gMaximumDiscoveryStoreTableSize = gMaximumDiscoveryStoreTableSize_c;
  #endif
  #if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d  
const    uint8_t mEdbMaxEntries = mEdbMaxEntries_c;
  #endif
  #if gMgmt_NWK_Disc_rsp_d
const    uint8_t gMaxArray = gMaxArray_d;
  #endif
  #if gBind_Register_rsp_d || gRecover_Source_Bind_rsp_d || gBackup_Source_Bind_rsp_d || (gBindCapability_d && gBind_rsp_d)
const   uint8_t gMaximumDevicesHoldingBindingInfo = gMaximumDevicesHoldingBindingInfo_c;
  #endif
  #if (gReplace_Device_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRecover_Bind_Table_rsp_d || gBind_Register_rsp_d || gMgmt_Bind_rsp_d)
const uint8_t gMaximumBindingCacheTableList = gMaximumBindingCacheTableList_c;  
  #endif
  #if gSystem_Server_Discovery_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gRecover_Bind_Table_rsp_d || gBackup_Source_Bind_rsp_d || gRecover_Source_Bind_rsp_d || gReplace_Device_rsp_d
const   uint8_t gMaximumSystemServerciesResponses = gMaximumSystemServerciesResponses_c;
  #endif
#endif    
/******************************RAM*******************************************/

#if(gInstantiableStackEnabled_d == 1)
beeStackGlobalsParams_t* pBeeStackGlobalsParams = NULL;
#else

  uint8_t  aExtendedAddress[8]= { 0xff, 0xff,0xff,0xff, 0xff, 0xff,0xff,0xff };
  zbCommissioningAttributes_t  gSAS_Ram;
   /* commissioning cluster (optional) */
  zbCommissioningAttributes_t  *gpSAS_Commissioning;
  /* It is declared already */
  //beeStackConfigParams_t        gBeeStackConfig;
  
  #if gUser_Desc_rsp_d || gUser_Desc_conf_d
    /* User Descriptor */
    zbUserDescriptorPkg_t gUserDesc;
  #endif
  #if gComplex_Desc_rsp_d
    /* Complex Descriptor */
    configComplexDescriptor_t gComplexDesc = 
    {
      mDefaultValueOfComplexDescFieldCount_c ,
      mDefaultValueOfComplexDescLangCharSet_c,
      mDefaultValueOfComplexDescManufactureName_c,
      mDefaultValueOfComplexDescModelName_c,
      mDefaultValueOfComplexDescSerialNumber_c,
      mDefaultValueOfComplexDescDeviceUrl_c,
      mDefaultValueOfComplexDescIcon_c,
      mDefaultValueOfComplexDescIconUrl_c
    };
  #endif
  
  /* array of pointers to endpoint descriptions, set at runtime */
  /* allow 2 extra ptrs for ZDP and broadcast endpoints */
  endPointPtrArray_t  gaEndPointDesc[(gNumberOfEndPoints_c + 2)];
  
  /* array of active endpoints, + 2 means that the array will include the ZDO endpoint and the Bradcast Endpoint */
  zbEndPoint_t        gaActiveEndPointList[(gNumberOfEndPoints_c + 2)];
  
  /* Private debug stuff */
  #if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) && gDefaultValueOfMaxEntriesForExclusionTable_c
    zbIeeeAddr_t  gaExclusionTable[gDefaultValueOfMaxEntriesForExclusionTable_c];
  #endif
  #if gICanHearYouTableCapability_d
    /* 
    ICanHearYou Table Entries = NWK Short Addresses that this device can hear, All packets received 
    via McpsDataIndication with MAC SourceAddress not listed in this table will be discarded by NWK 
    layer. Use for table-top demonstrations and debugging. Not saved to NVM.
    */
    zbNwkAddr_t                     gaICanHearYouTable[gDefaultValueOfMaxEntriesForICanHearYouTable_c];
    uint8_t                         gaICanHearYouLqi[gDefaultValueOfMaxEntriesForICanHearYouTable_c];
    uint8_t gICanHearYouCounter;
  #endif
  /* note: 0 means use real Link Cost. */
  uint8_t giIcanHearLinkCost;
  
  bool_t mBUtl_ReceiverIsTurnedOn;

#endif  
  

//uint8_t  aExtendedAddress[8] = { 0xff, 0xff,0xff,0xff, 0xff, 0xff,0xff,0xff };

#ifdef gHostApp_d
//check this for a host
uint8_t  aExtendedAddress[8] = { 0xff, 0xff,0xff,0xff, 0xff, 0xff,0xff,0xff };
uint8_t giFragmentedPreAcked;
#endif /* gHostApp_d */
/****************************************************************************/

/* TaskIDs. The MAC library needs to know the IDs for some of the tasks, so
 * it can set events for them. But it is supplied as a binary library, so it
 * can't use #define or enum constants, and the TaskIDs might change. TaskIDs
 * are assigned dynamically by the kernel.
 * Instead, there is integer in RAM for each task that contains that task's
 * ID. This costs some RAM, but the MAC library only needs to know the names
 * of the integers, not the TaskIDs, at link time.
 */

#define Task( taskIdGlobal, taskInitFunc, taskMainFunc, priority ) \
  tsTaskID_t taskIdGlobal;
#include "BeeStackTasksTbl.h"
  
/****************************************************************************/

/************************************************************************************
*
* Initialize beestack network global structure 
*
************************************************************************************/
void BeeStackNwkGlobalsInit(uint8_t instId)
{
#if(gInstantiableStackEnabled_d == 1)
  /* Init network global structure */
  FLib_MemCpy((void*)pZbStackStuctPointersTable[instId]->pZbBeeStackNwkGlobals,
                (void*)&gZbBeeStackNwkGlobalsInit,
                sizeof(gZbBeeStackNwkGlobalsInit));
#else
  (void)instId;
#endif  
}

/************************************************************************************
*
* Initialize beestack  Global structure ( ZDO, AF ..)
*
************************************************************************************/
void BeeStackGlobalsParamInit(uint8_t instId)
{
#if(gInstantiableStackEnabled_d == 1)
  uint8_t index;
  
  beeStackGlobalsParams_t* pBeeStackGlobalsParams;
  for(index = 0; index < gZbMaxInstanceCount_c; index++)
  {
    pBeeStackGlobalsParams = pZbStackStuctPointersTable[instId]->pBeeStackGlobalsParams;
    
    FLib_MemCpy((void*)&ZbBeeStackGlobalsParams(gBeeStackConfig),
                 (void*)&gBeeStackInitConfig,sizeof(beeStackConfigParams_t));
    
    #if gUser_Desc_rsp_d || gUser_Desc_conf_d
      FLib_MemCpy((void*)&ZbBeeStackGlobalsParams(gUserDesc),
                  (void*)&gUserDescInit,sizeof(zbUserDescriptorPkg_t));
    #endif
    #if gComplex_Desc_rsp_d 
      FLib_MemCpy((void*)&ZbBeeStackGlobalsParams(gComplexDesc),
                  (void*)&gComplexDescInit,sizeof(configComplexDescriptor_t));
    #endif 
      //check this in multi instances situation      
      ZbBeeStackGlobalsParams(mBUtl_ReceiverIsTurnedOn) = TRUE;
    
     FLib_MemSet(ZbBeeStackGlobalsParams(aExtendedAddress),
                 0xFF,sizeof(ZbBeeStackGlobalsParams(aExtendedAddress)));
  }
#else
  (void)instId;
#endif  
}

/************************************************************************************
*
* Initialize all beestack structures
*
************************************************************************************/
void BeeStackGlobalsInit(uint8_t instId)
{  
#if(gInstantiableStackEnabled_d == 1)
  /* Init Network variables */
  NwkInitVariables(instId);
  ApsInitVariables(instId);
  ZdoInitVariables(instId);
  BeeStackNwkGlobalsInit(instId);
  BeeStackGlobalsParamInit(instId);
#else
  (void)instId;  
#endif
}

/************************************************************************************
*
* Set beestack pointers to active instance
*
************************************************************************************/
void BeeStackGlobalsSetCurrentInstance(uint8_t instId)
{
  
#if(gInstantiableStackEnabled_d == 1)
  if (instId > gZbMaxInstanceCount_c) 
  {
    return;
  }
    
  /* Init beestack pointers structure */
  /* Nwk */
  pZbNwkPrivateData      = pZbStackStuctPointersTable[instId]->pZbNwkPrivateData;
  pZbNwkPublicData       = pZbStackStuctPointersTable[instId]->pZbNwkPublicData; 
  pZbBeeStackNwkGlobals  = pZbStackStuctPointersTable[instId]->pZbBeeStackNwkGlobals;
  /* APS */
  pZbApsPrivateData      = pZbStackStuctPointersTable[instId]->pZbApsPrivateData;
  pZbApsPublicData       = pZbStackStuctPointersTable[instId]->pZbApsPublicData;
  pZbBeeStackApsGlobals  = pZbStackStuctPointersTable[instId]->pZbBeeStackApsGlobals;
  /* AF */
  pZbAfPrivateData       = pZbStackStuctPointersTable[instId]->pZbAfPrivateData;
  /* ZDO */
  pZbZdoPrivateData      = pZbStackStuctPointersTable[instId]->pZbZdoPrivateData;
  pZbZdoPublicData       = pZbStackStuctPointersTable[instId]->pZbZdoPublicData;
  /* General */
  pBeeStackGlobalsParams = pZbStackStuctPointersTable[instId]->pBeeStackGlobalsParams;
  
  /* table sizes structures */
  pZbStackTablesSizes    = pZbStackStuctPointersTable[instId]->pZbStackTablesSizes;
#else
  (void)instId;
#endif
}
