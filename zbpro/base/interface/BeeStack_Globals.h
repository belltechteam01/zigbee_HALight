/******************************************************************************
* This public header file provides structure definition for different table
* structure whose size can be configured by the application. To configure the
* the table size the user can use BeeStackConfiguration.h file
*
* Copyright (c) 2013, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*******************************************************************************/

#ifndef _BEESTACK_GLOBALS_H_
#define _BEESTACK_GLOBALS_H_

#ifdef __cplusplus
    extern "C" {
#endif


#include "EmbeddedTypes.h"
//#include "AppToPlatformConfig.h"
#include "TS_Interface.h"
#include "TMR_Interface.h"
#include "MsgSystem.h"
#include "NVM_Interface.h"
#include "NV_Data.h"

#include "zigbee.h"
#include "BeeStackFunctionality.h"
#include "BeeStackConfiguration.h"
#include "BeeCommon.h"
#include "NwkMacInterface.h"


#include "AfApsInterface.h"
/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/
#define gZbActiveInstance_d   0
#define gZbMaxInstanceCount_c 2


/* Number of Channel available (do not redefined 802.15.4 standard) */
#define gNumOfChannels_c                  16


/* ZDO NVM address offset for Pan ID */
#define gPanIdNVMOffset_c                 60

/*
  Default Value of Beacon Order. Not used presently in ZigBee. Set to 0x0F (no
  beacons) always.
*/
#ifndef mDefaultValueOfBeaconOrder_c
  #define mDefaultValueOfBeaconOrder_c    0x0F
#endif

#define   dummyDescCapField   0x00

#if gNwkMulticastCapability_d
 #define mUseMultiCast_c                       mDefaultValueOfNwkUseMulticast_c
 #define mUseMultiCastForApsGroupDataRequest_c mDefaultUseMultiCastForApsGroupDataRequest_c
#else
 #define mUseMultiCast_c                       FALSE
 #define mUseMultiCastForApsGroupDataRequest_c FALSE
#endif

/****************************************************************************/

/* Task / kernel identifiers. */

/* Reserved task priority ranges.
 * Task priority   0 is reserved by the kernel for the idle task.
 * Task priority 255 is reserved by the kernel as an invalid priority value.
 *
 * Higher number tasks run first.
 */

/* Reserve some IDs for low priority BeeStack tasks. */
#define gTsFirstBeeStackTaskPriorityLow_c   0x01
#define gTsZTCTaskPriority_c                0x02

/* Reserved for application tasks. */
#define gTsFirstApplicationTaskPriority_c   0x40

/* Application main task. */
#define gTsAppTaskPriority_c                0x80


#define gTsTimerTaskPriority_c	            0xFE


/* Reserved for high priority BeeStack tasks. */
#define gTsFirstBeeStackTaskPriorityHigh    0xC0
#define gTsZclTaskPriority_c                gTsFirstBeeStackTaskPriorityHigh
#define gTsAfTaskPriority_c                 0xC1
#define gTsApsTaskPriority_c                0xC2
#define gTsZdoTaskPriority_c                0xC3
#define gTsZdoStateMachineTaskPriority_c    0xC4
#define gTsNwkTaskPriority_c                0xC5
#define gTsMlmeTaskPriority_c               0xC6
#define gTsHighestTaskPriorityPlusOne_c     0xC7

#if gTsTimerTaskPriority_c < gTsHighestTaskPriorityPlusOne_c
#error The Timer Task must be the highest priority task.
#endif

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
  extern tsTaskID_t taskIdGlobal; \
  extern void taskInitFunc( void ); \
  extern void taskMainFunc( tsEvent_t );
/*This will expand macro in task extern declaration */
#include "BeeStackTasksTbl.h" 
/*****************************************************************************/  
  
  
/**********************************************************************
***********************************************************************
*                             NWK Macros
***********************************************************************
***********************************************************************/
#if(gInstantiableStackEnabled_d == 0)
  #define ZbBeeStackNwkGlobals(val)      val
#else
  #define ZbBeeStackNwkGlobals(val)        pZbBeeStackNwkGlobals->val
#endif
  
#define gZbDeviceTypeMask_c  (0b11)

/* handleTrackingTable_t status codes */
#define mHttFreeEntry_c             0x00
#define mHttDataImSending_c         0x01
#define mHttDataImRouting_c         0x02
#define mHttBroadcastForRxOff_c     0x03

#define mHttSuppressRouteDiscovery  0x80
#define mHttStatusMask              0x7F

/* packetsOnHoldTable_t status codes */
#define mPohtFreeEntry_c          0x00
#define mPohtDataFromNhl_c        0x01
#define mPohtDataFromNll_c        0x02
#define mReadyToSend_c            0x03
/**********************************************************************
***********************************************************************
*                             APS Macros
***********************************************************************
***********************************************************************/

#if(gInstantiableStackEnabled_d == 0)
  #define ZbBeeStackApsGlobals(val)      val
#else
  #define ZbBeeStackApsGlobals(val)        pZbBeeStackApsGlobals->val
#endif
  
/**** Fragmentation Capability Types **********************************/
/* type returned by retry queue */
#define gApsTxTableFull_c         gMaxIndex_c
#define gApsTxEntryNotFound_c     gMaxIndex_c

#define gApsTxFlags_Fragmented_c  0x01   /* fragmented transmission */
#define gApsTxFlags_Indirect_c    0x02   /* indirect transmission */


#define gApsTxResult_DontContinue_c   FALSE
#define gApsTxResult_Continue_c       TRUE

#define frmEnabledAndReq      0x03
#define frmEnabledAndNotReq   0x02
#define frmDisabledAndReq     0x01
#define frmDisabledAndNotReq  0x00

/* address map index type */
#define gNotInAddressMap_c  (gMaxIndex_c-1)
#define gAddressMapFull_c   (gMaxIndex_c-1)

/* indicate destination is group, not IEEE address (flag multiplexed on dstEndPoint field) */
/* otherwise, dstEndPoint will be 0x01 - 0xfE */
#define gApsBindAddrModeGroup_c 0x00

/*
  calculating bytes needed to represent the end points bit mask for groups 
  1+ because at least one is needed
  +2 because 0xFF and 0x00 are hardcoded in index 0 and 1 then
  gNumberOfEndPoints_c only represents application specific end points
*/

#define gEndPointsMaskSizeInBytes_c  (1 + ((gNumberOfEndPoints_c + 2)>> 3))

/*
  defines the a bit mask (in bytes) to be used withthe address map, each bit in this mask
  represents an entry in the addess map table.
  1+ because at least we need a byte no matter if address map is only one entry long.
  (gApsMaxAddrMapEntries_c >> 3) is the amount of bytes need according to amount of entries
  in the address map table.

  (*) The smalles amount of bytes is two.
*/
#define gApsAddressMapBitMaskInBits_c  (1 + (gApsMaxAddrMapEntries_c >> 3))

#define gpBindingTable ZbBeeStackApsGlobals(gaApsBindingTable)

#define gpAddressMapTable gaApsAddressMap
  
  
/**********************************************************************
***********************************************************************
*                             SSP Macros
***********************************************************************
***********************************************************************/

#define gSSPNoKeyFound_c (zbKeyTableIndex_t)(~((zbKeyTableIndex_t)0))

/**********************************************************************
***********************************************************************
*                             Beestack Macros
***********************************************************************
***********************************************************************/
#if(gInstantiableStackEnabled_d == 0)

  #define ZbBeeStackGlobalsParams(val)   val  
  #define ZbStackTablesSizes(val)        val

#else

  #define ZbBeeStackGlobalsParams(val)     pBeeStackGlobalsParams->val
  #define ZbStackTablesSizes(val)          pZbStackTablesSizes->val

#endif
    
/* from commissioning cluster (064699r12) */
// see ZdoStartMode_t in 
#define gStartupControl_SilentStart_c   0x00  // already part of the network (no join needed)
#define gStartupControl_Form_c          0x01  // form a network with exteneded Pan Id described in the attribute "Extended Pan Id"(ZC only)
#define gStartupControl_NwkRejoin_c     0x02  // use NWK rejoin (ZR, ZED only)
#define gStartupControl_Associate_c     0x03  // use association (ZR, ZED only)

/* FS Specific Startup control modes. */
#define gStartupControl_OrphanRejoin_c  0x04  // FS specific: use orphan rejoin (ZR, ZED only)
#define gStartupControl_ChannelRejoin_c 0x05  // FS specific: use orphan rejoin (ZR, ZED only)

/* define function to set commissioning cluster SAS */
#define ASL_GetCommissioningSAS()     (ZbBeeStackGlobalsParams(gpSAS_Commissioning))
#define ASL_SetCommissioningSAS(pPtr) (ZbBeeStackGlobalsParams(gpSAS_Commissioning) = (pPtr))
  
  
/* Permissions Configuration Table */
#if gApsMaxEntriesForPermissionsTable_c
/*
  Set of bit masks representing the different permission categories.
  The gLinkKeyRequired_c mask is used to verify if a link key is required to carry out the commands.
*/
#define	gPermissionsEntryInActive                 0x00
#define	gNetworkSettingsPermission_c              0x02
#define	gApplicationSettingsPermission_c          0x04
#define	gSecuritySettingsPermission_c             0x08
#define	gApplicationCommandsPermission_c          0x10
#define	gLinkKeyRequired_c                        0x20
#define	gPermissionsEntryActive                   0x40

#endif  
  
  
/**********************************************************************
***********************************************************************
*                             Endpoints Macros
***********************************************************************
***********************************************************************/

#if(gInstantiableStackEnabled_d == 0)
  #define EndPointConfigData(val)     val
#else
  #define EndPointConfigData(val)      pEndPointData->val
#endif
  
/**********************************************************************
***********************************************************************
* Public type definitions
***********************************************************************
***********************************************************************/

/**********************************************************************
***********************************************************************
*                             NWK Types
***********************************************************************
***********************************************************************/
typedef struct nldeDataCnf_tag {
  uint8_t nsduHandle;
  uint8_t status;
} nldeDataCnf_t;

/* NLDE to APS message */
typedef struct nldeApsMessage_tag {
  uint8_t msgType;
  union {
    nldeDataCnf_t dataConf;
    nldeDataIndication_t dataIndication;
  } msgData;
} nldeApsMessage_t;
/* Network Layer Tables */

typedef PACKED_STRUCT rteProperties_tag
{  
    /*The status of the route. See Table 3.52 for values (053474r20 ZigBee Pro spec)*/
  uint8_t status              :3;
  /*A flag indicating that the destination indicated by this address does not store source routes.*/
  uint8_t noRouteCache        :1;
  /*A flag indicating that the destination is a concentrator that issued a many-to-one route request.*/
  uint8_t manyToOne           :1;
  /*A flag indicating that a route record command frame should be sent to the destination prior to the next data packet.*/
  uint8_t routeRecordRequired :1;
  /*A flag indicating that the destination address is a Group ID.*/
  uint8_t groupIdFlag         :1;
  /* This bit will be used to control the sent of the route record to a network concentrator */
  uint8_t AllAlreadySentItsRouteRecord  :1;   
} rteProperties_t;

/**** Backup Nwk Info ************************************************************/
/* This structure saves the nwk information in case mac association fails and */ 
typedef struct nwkDataBackup_tag{
  /*The 16-bit PAN identifier of the neighboring device.*/
  zbPanId_t aPanId;

    /*The 64-bit Extended PAN identifier of the neighboring device.*/
  zbIeeeAddr_t aExtendedPanId;

  /*The 16-bit network address of the neighboring device.*/
  zbNwkAddr_t aNetworkAddress;
}nwkDataBackup_t;


/**** Routing table **************************************************************/
/* Stucture for Route table Elements ( r13 - Table 3.46 ) */
typedef struct routeTable_tag
{
  /*Destination Address for which the route is discovered*/
  zbNwkAddr_t aDestinationAddress;

  /*Next hope Address to the discovered Destination*/
  zbNwkAddr_t aNextHopAddress;

  rteProperties_t properties;

  /* Used to reuse the route table entry */
  uint8_t age;

} routeTable_t;

/**** RouteDiscoveryTable ********************************************************/
/* Structure of route discovery table. ( r13 - Table 3.48 ) */
typedef struct routeDiscoveryTable_tag
{
  /* A sequence number for a route request command frame that is incremented
  each time a device initiates a route request.*/
  uint8_t  routeRequestId;

  /*The 16-bit network address of the route request’s initiator.*/
  zbNwkAddr_t aSourceAddress;

  /*The 16-bit network address of the device that has sent the most recent
  lowest cost route request command frame corresponding to this entry’s
  Route request identifier and Source address. This field is used to determine
  the path that an eventual route reply command frame should follow.*/
  zbNwkAddr_t aSenderAddress;

  /*
    053474r20 ZigBee Pro spec
    3.4.1.2 NWK Header Fields 
    The source IEEE address sub-field of the frame control field shall be set to 1
    and the source IEEE address field of the NWK header shall be present and shall
    contain the 64-bit IEEE address of the originator of the frame.
  */
  zbIeeeAddr_t aIEEESrcAddr;

  /* Destination IEEE address in command options */
  zbIeeeAddr_t aIEEEDstAddrCmdOptions;
  
	/*The accumulated path cost from source of the route request to the current
  device.*/
  uint8_t  forwardCost;

	/*The accumulated path cost from the current device to the destination
  device.*/
  uint8_t  residualCost;

	/*A countdown timer indicating the number of ticks until route discovery expires.*/
  uint8_t  expirationTicks;

	/*A countdown timer indicating the number of ticks until route discovery is retransmited */
  uint8_t  retriesTicks;

	/* Holds the number of retries pending for this route request */
  uint8_t  retriesLeft;

  index_t indexInRouteTable;

  uint8_t sequenceNumber;

  uint8_t radius;

} routeDiscoveryTable_t;

/************* Source Route Table  ***********************************************/
/* Structure for source route table 
   053474r20 ZigBee Pro spec
   Table 3.45 Route Record Table Entry Format
*/ 
typedef struct sourceRouteTable_tag{

  /* The destination network address for this route record. */
  zbNwkAddr_t aDestinationAddress;

  /* The count of relay nodes from concentrator to the destination.*/
  uint8_t relayCount;

  /* Age field. Is settable option */
  uint8_t age;

  /* The set of network addresses that represent the route  from  concentrator to destination.*/
  RouteRecordRelayList_t *path;

} sourceRouteTable_t;

/**** Broadcast Transaction Buffer ***********************************************/
typedef struct broadcastBuffer_tag
{
  /* Holds the length of the stored NPDU */
  uint8_t npduLength;

  /* Holds the pointer to the stored NPDU */
  npduFrame_t *pNpdu;

}broadcastBuffer_t;


/**** Broadcast Transaction Table ************************************************/
typedef struct broadcastTransactionTable_tag
{

  /* Holds the Source Short Address of the broadcast transaction */
  zbNwkAddr_t aSourceAddress;

  /* Holds the Sequence number of the broadcast transaction */
  uint8_t sequenceNumber;

  /* Holds the handle to identify mac confirmation for this broadcast instance*/
  uint8_t msduHandle;

  bool_t confirmPending;

  /* number of ticks to expire this broadcast transaction */
  uint8_t deliveryTicks;

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d

  /* Holds the status whether broadcast is in progress or not */
  uint8_t retriesTicks;

  /* Holds the number of retries pending for this broadcast transaction */
  uint8_t retriesLeft;

  /* Data needed for routers or coordinator */
  broadcastBuffer_t  bufferedData;

#endif

}broadcastTransactionTable_t;


/*********************************************************************************/
typedef PACKED_UNION {
  uint8_t allBits;
  struct {
#ifdef __IAR_SYSTEMS_ICC__    
    /*
      deviceType 
         0b00 -- ZigBee Coordinator.
         0b01 -- ZigBee Router.
         0b10 -- ZigBee EndDevice.
         0b11 -- Device Mask.
    */
    uint8_t deviceType     :2;
    /**/
    uint8_t rxOnWhenIdle   :1;

    /*
      (0) = 0b000 - Is a parent
      (1) = 0b001 - Is a children
      (2) = 0b010 - Is a silbing
      (3) = 0b011 - Is not related.
      (4) = 0b100 - Is previous child
      (5) = 0b101 - Is un-authenticated
      (6) = 0b110 - Is an expired children.
    */
    uint8_t relationship   :3;
    /**/
    uint8_t linkStatus     :1;
    /**/
    uint8_t reuseAddress   :1;
#else
     /*
      deviceType 
         0b00 -- ZigBee Coordinator.
         0b01 -- ZigBee Router.
         0b10 -- ZigBee EndDevice.
         0b11 -- Device Mask.
    */
    unsigned int deviceType     :2;
    /**/
    unsigned int rxOnWhenIdle   :1;

    /*
      (0) = 0b000 - Is a parent
      (1) = 0b001 - Is a children
      (2) = 0b010 - Is a silbing
      (3) = 0b011 - Is not related.
      (4) = 0b100 - Is previous child
      (5) = 0b101 - Is un-authenticated
      (6) = 0b110 - Is an expired children.
    */
    unsigned int relationship   :3;
    /**/
    unsigned int linkStatus     :1;
    /**/
    unsigned int reuseAddress   :1;
#endif    
  } bits;
} nteProperties_t;


/**** Neighbor Table *************************************************************/
/*This structure holds the information of the Additional Neighbour Table
	r13 - Table 3.44 Additional Neighbor Table Fields */
typedef PACKED_STRUCT neighborTable_tag{

  /*64-bit IEEE address that is unique to every device.*/
  zbIeeeAddr_t aExtendedAddress;

  /*The 16-bit network address of the neighboring device.*/
  zbNwkAddr_t aNetworkAddress;

  /* a set of sub-fields associated to this entry */
  nteProperties_t deviceProperty;

  /*A value indicating if previous transmissions to the device were
  successful or not. Higher values indicate more failures.*/
  uint8_t transmitFailure;

  /*The estimated link quality for RF transmissions from this device.*/
  uint8_t lqi;

  /* 053474r20 ZigBee Pro spec 
     Sec. Table 3.48 Neighbor Table Entry Format
     The cost of an outgoing link as measured by the neighbor. A valueof 0 indicates no 
     outgoing cost is available.

     This field is mandatory if nwkSymLink = TRUE.*/
  zbCounter_t outgoingCost;

    /* 053474r20 ZigBee Pro spec. 
     Sec. Table 3.48 Neighbor Table Entry Format
     The number of nwkLinkStatusPeriod intervals since a link status command was received. 

     This field is mandatory if nwkSymLink = TRUE.*/
  zbCounter_t age;

#if ( gBeaconSupportCapability_d )

  /*The time, in symbols, at which the last beacon frame was received from
  the neighbor. This value is equal to the timestamp taken when the beacon
  frame was received,*/
  uint8_t aIncomingBeaconTimeStamp[3];

  /*The transmission time difference,in symbols, between the neighbor’s
  beacon and its parent’s beacon. This difference may be subtracted from the
  corresponding incoming beacon timestamp to calculate the beacon transmission
  time of the neighbor’s parent.*/
  uint8_t aBeaconTransmissionTimeOffset[3];

#endif /* ( gBeaconSupportCapability_d ) */

}neighborTable_t;


/**** Extended Neighbor Table ****************************************************/
typedef PACKED_STRUCT extendedNeighborTable_tag
{
  /*The 16-bit PAN identifier of the neighboring device.*/
  zbPanId_t aPanId;

	/*The 64-bit Extended PAN identifier of the neighboring device.*/
  zbIeeeAddr_t aExtendedPanId;

  /*The logical channel on which the neighbor is operating.*/
  uint8_t logicalChannel;

  /*The tree depth of the neighbor device. A value of 0x00 indicates that
  the device is the ZigBee coordinator for the network.*/
  uint8_t depth;

  zbCounter_t nwkUpdateId;
  
#if ( gBeaconSupportCapability_d )
  /*This specifies how often the beacon is to be transmitted.*/
  uint8_t beaconOrder ;
#endif /* ( gBeaconSupportCapability_d ) */

  /*joiningFeatures is a bit field containing the following bits
  Bit 0   -> permitJoining
  Bit 1   -> potential parent
  Bit 2-3 -> not used
  Bit 4-7 -> stack profile
  Note: This is a slight deviation from spec where these attributes are one byte*/
  uint8_t joiningFeatures;

}extendedNeighborTable_t;

/* handleTrackingTable_t status codes */
typedef uint8_t httStatus_t;

/**** HandleTrackingTable ************************************************************/
typedef PACKED_STRUCT handleTrackingTable_tag{
  /* entry status */
  httStatus_t status;
  /* Indicate if the msg is a data frame - nldedataReq, indicat, source route.-*/
  uint8_t msgType;
  /* msduHandle awaiting Confirmation */
  zbCounter_t handle;
  /* index pointing to a neighbor table entry */
  index_t iNteIndex;
  /* Mac src addres used to control the many-to-one route error */
  zbNwkAddr_t aMacSourceAddress;
  /* Source Address of this data request - NwkHeader.aSourceAddress- */
  zbNwkAddr_t aSourceAddress;
  /* final destination address of this data request */
  zbNwkAddr_t aDestinationAddress;
} handleTrackingTable_t;


/* packetsOnHoldTable_t status codes */
typedef uint8_t pohtStatus_t;

/**** PacketsOnHoldTable *************************************************************/
typedef PACKED_STRUCT packetsOnHoldTable_tag{
  /* entry status */
  pohtStatus_t status;

  uint8_t msgType;

  /* short address to where the packet should be send */
  zbNwkAddr_t aDestinationAddress;

  /* packet waiting to be routed */
  uint8_t *pPacketOnHold;
  uint8_t age;
} packetsOnHoldTable_t;


/*
  This data type is used to handle each rejoin request to be send out, keeps aunique handler
  in order to respond to more than one rejoin reqwuest at the same time
*/
typedef PACKED_STRUCT nwkRejoinEntry_tag
{
  zbCounter_t  nsduHanlde;
  index_t      iIndex;
  zbNwkAddr_t  aNewAddress;
  bool_t       wasSecure;
}nwkRejoinEntry_t;

/* this structure defines general network data, much of it assigned at run-time */
typedef PACKED_STRUCT nvmNwkData_tag
{
  /* Extended Address of the Device */
  zbIeeeAddr_t aExtendedAddress;

  /* Capability information of this device */
  macCapabilityInfo_t capabilityInformation;

  /* is this device started as a ZC, ZR or ZED? */
  zbDeviceType_t        deviceType;

  /* Logical channel on which the current device has associated */
  uint8_t logicalChannel;

  /* Short address of current device's parent */
  zbNwkAddr_t aParentShortAddress;

  /* Long address of current device's parent */
  zbIeeeAddr_t aParentLongAddress;

  uint8_t deviceDepth;

  uint8_t nwkUpdateId;

  uint8_t permitJoining;

  /*
    NIB Security Attributes
  */
#if gStandardSecurity_d || gHighSecurity_d
  /*
    053474r17 Table 4.1
    Name: nwkSecurityLevel
    Type: Octet
    Range: 0x00 - 0x07
    Default: 0x05
    NIB Id: 0xa0
  */
  zbNwkSecurityLevel_t  nwkSecurityLevel;

  /*
    053474r17 Table 4.1
    Name: nwkSecurityMaterialSet
    Type: A set of 0, 1 or 2 nwk security material descriptor (Table 4.2)
    Range: Variable
    Default: --
    NIB Id: 0xa1
  */
  zbNwkSecurityMaterialSet_t  aNwkSecurityMaterialSet[2];

  /*
    053474r17 Table 4.1
    Name: nwkAllFresh
    Type: Boolean
    Range: TRUE | FALSE
    Default: TRUE
    NIB Id: 0xa3
  */
  bool_t nwkAllFresh;

  /*
    053474r17 Table 4.1
    Name: nwkSecureAllFrames
    Type: Boolean
    Range: TRUE | FALSE
    Default: TRUE
    NIB Id: 0xa5
  */
  bool_t nwkSecureAllFrames;
#endif
}nvmNwkData_t;

typedef struct rejoinConfigParams_tag
{
  uint8_t   paramFlags;
  uint8_t   filterDeviceLqi;
  zbNwkAddr_t   filterDeviceNwkAddress;  
} rejoinConfigParams_t;

#if(gInstantiableStackEnabled_d == 1)
/* Nwk struct of globals */  
typedef struct zbBeeStackNwkGlobals_tag
{
#endif  

EXTERN  bool_t   gNibNwkLeaveRequestAllowed;
EXTERN  uint16_t gNibTransactionPersistenceTime;
  /*PurgeTime is : nwkPersistenceTime * SuperFrame  * 1/aBaseSuperFrameDuration */
EXTERN  uint16_t gNwkPurgeTimeout;
EXTERN  uint8_t gNibRouteDiscoveryRetriesPermitted;
EXTERN  zbNwkAddr_t gNibAvailableAddresses;
EXTERN  zbNwkAddr_t gNibAddressIncrement;
  /* Scale used to define the resolution for expiring the entries on the NT */
EXTERN  uint8_t gNwkAgingTickScale;
  #if gNwkSymLinkCapability_d
  /* 053474r17 ZigBee Spec.
     The time in seconds between link status command frames. 
                    Default 0x0f */
EXTERN  uint8_t gNibNwkLinkStatusPeriod;
  #endif
  /* 
    Default:  TRUE
    A flag determining the layer where multicast messaging occurs.
    TRUE = multicast occurs at the network layer.
    FALSE= multicast occurs at the APS layer and using the APS header.*/
  /* Don't touch this line */
EXTERN  bool_t gNibNwkUseMulticast;
EXTERN  bool_t gNwkUseMulticastForApsGroupDataRequest;
EXTERN  uint8_t gMulticastMaxNonMemberRadius_c;
EXTERN  uint8_t gNwkHighRamConcentrator;
/************************** uninit elements ********* *************************/
EXTERN  nvmNwkData_t gNwkData;
EXTERN  zbCounter_t gNibSequenceNumber;
        /* Current state of network state machine */
EXTERN  uint8_t gNwkState;
EXTERN  uint16_t gNibTxTotal;
EXTERN  uint16_t gNibTxTotalFailures;
EXTERN  bool_t gDeviceAlreadySentRouteRecord;
EXTERN  bool_t gActAsZed;
  #if gComboDeviceCapability_d
EXTERN    bool_t gLpmIncluded;
  #endif
  /* This variable contains sorted the indexes of the neighbor table in ascending order.
       This array only is used when NwkSymLink is TRUE and is used by Link Status Command 
       to build the link status information list. This array has the same length of the gaNeighborTable */
  #if gNwkSymLinkCapability_d && (( gCoordinatorCapability_d || gRouterCapability_d ) || gComboDeviceCapability_d)
#if(gInstantiableStackEnabled_d == 1)    
  uint8_t* gaNeighborTableAscendantSorted;
#else
  extern uint8_t gaNeighborTableAscendantSorted[];
#endif
  #endif
  /* This table will exist only if a discovery network has been performed 
          previously and it last only during joining or forming a network */
EXTERN  neighborTable_t *gpDiscoveryTable;
EXTERN  extendedNeighborTable_t *gpExtendedDiscoveryTable;
  /* This value will change depending on the size of memory allocated for
        gpDiscoveryTable and gpExtendedDiscoveryTable */
EXTERN  index_t gMaxDiscoveryTableEntries;
  /* this table keeps the pointer to the messages that are waiting to be routed.
         waiting for a route discovery or device authentication */
#if(gInstantiableStackEnabled_d == 1)  
  packetsOnHoldTable_t* gaPacketsOnHoldTable;
#else
  extern packetsOnHoldTable_t gaPacketsOnHoldTable[]; 
#endif
  /* this table keeps track of every unicast data request sent to the NLL */
#if(gInstantiableStackEnabled_d == 1)    
  handleTrackingTable_t* gaHandleTrackingTable;
#else
  extern handleTrackingTable_t gaHandleTrackingTable[];
#endif
  /* keeps track of each rejoin reuqest until copleted. */
EXTERN  nwkRejoinEntry_t *gpNwkRejoinStateMachine;
  /* indicates how many entries of the handleTrackingTable are occupied by sleeping devices */
EXTERN  zbCounter_t gHttIndirectEntries;
  #if gStandardSecurity_d || gHighSecurity_d
#if(gInstantiableStackEnabled_d == 1) 
          zbIncomingFrameCounterSet_t*  gaIncomingFrameCounterSet1;
         zbIncomingFrameCounterSet_t*  gaIncomingFrameCounterSet2; 
#else
 extern         zbIncomingFrameCounterSet_t  gaIncomingFrameCounterSet1[];
 extern         zbIncomingFrameCounterSet_t  gaIncomingFrameCounterSet2[]; 
#endif
    #if gComboDeviceCapability_d
EXTERN      bool_t gTrustCenter;
    #endif
  #endif /*gStandardSecurity_d || gHighSecurity_d*/
  #if ( !gNwkBroadcastPassiveAckRetryCapability_d ) || (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
    /* Broadcast Transaction Table, size of this table is decided by the number
          of broadcast requests that handled by the network layer simultaneously */
#if(gInstantiableStackEnabled_d == 1) 
EXTERN    broadcastTransactionTable_t* gaBroadcastTransactionTable;
#else
   extern broadcastTransactionTable_t gaBroadcastTransactionTable[];
#endif
  #endif /* ( !gNwkBroadcastPassiveAckRetryCapability_d ) || (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) */
      
  #if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)// && gRnplusCapability_d 
#if(gInstantiableStackEnabled_d == 1) 
EXTERN    routeTable_t* gaRouteTable;
#else
   extern  routeTable_t  gaRouteTable[];
#endif
  #endif
  /* Route Discovery Table Entries, to update the number of route Discovery
          table entries, the value of gNwkRoutingMaxRouteDiscoveyTableEntry_c macro
         needs to be updated */
#if(gInstantiableStackEnabled_d == 1)           
    routeDiscoveryTable_t* gaRouteDiscoveryTable;
#else
    extern routeDiscoveryTable_t gaRouteDiscoveryTable[];
#endif    
  /* This is the variable that contains our neighbors information */
#if(gInstantiableStackEnabled_d == 1)
     neighborTable_t* gaNeighborTable;
#else
     extern neighborTable_t  gaNeighborTable[];
#endif
#if(gInstantiableStackEnabled_d == 1)
     uint8_t* gNeighborRelationsTable;
#else
     extern uint8_t gNeighborRelationsTable[];
#endif        
  /*This variable is set to true when a device of the network detects a PanId conflict 
         after it joined the network.*/
EXTERN  bool_t gNwkPanIdConflictDetected;
#if(gInstantiableStackEnabled_d == 1) 
        sourceRouteTable_t* gaSourceRouteTable;
#else
 extern sourceRouteTable_t gaSourceRouteTable[];
#endif
        
EXTERN  rejoinConfigParams_t gRejoinConfigParams;

#if(gInstantiableStackEnabled_d == 1)  
}zbBeeStackNwkGlobals_t;
#endif

#if(gInstantiableStackEnabled_d == 1)
/* Nwk struct of globals */  
typedef struct zbBeeStackNwkGlobalsInit_tag
{
  bool_t   gNibNwkLeaveRequestAllowed;
  uint16_t gNibTransactionPersistenceTime;
  /*PurgeTime is : nwkPersistenceTime * SuperFrame  * 1/aBaseSuperFrameDuration */
  uint16_t gNwkPurgeTimeout;
  uint8_t gNibRouteDiscoveryRetriesPermitted;
  zbNwkAddr_t gNibAvailableAddresses;
  zbNwkAddr_t gNibAddressIncrement;
  /* Scale used to define the resolution for expiring the entries on the NT */
  uint8_t gNwkAgingTickScale;
  #if gNwkSymLinkCapability_d
  /* 053474r17 ZigBee Spec.
     The time in seconds between link status command frames. 
                    Default 0x0f */
  uint8_t gNibNwkLinkStatusPeriod;
  #endif
  /* 
    Default:  TRUE
    A flag determining the layer where multicast messaging occurs.
    TRUE = multicast occurs at the network layer.
    FALSE= multicast occurs at the APS layer and using the APS header.*/
  /* Don't touch this line */
  bool_t gNibNwkUseMulticast;
  bool_t gNwkUseMulticastForApsGroupDataRequest;
  uint8_t gMulticastMaxNonMemberRadius_c;
  uint8_t gNwkHighRamConcentrator;
}zbBeeStackNwkGlobalsInit_t;
#endif
/***************************** Conflict Resolution **************************/
typedef void (*PIDConflictDetect_t) ( nwkBeaconNotifyInd_t *pBeaconIndication );
typedef void (*PIDConflictResol_t) ( void );

/***************************** Stack Hooks **************************/
typedef void (*NodeNotInParentLinkStatus_t) ( void );
typedef void (*UpdateNtEntryLQI_t) ( neighborTable_t* pNTE, uint8_t newLQI );

/**************** HighRamConcentrator function pointers *********************/
typedef sourceRouteTable_t*  (*NwkRetrieveSrcRoute_t) ( uint8_t *pDestinationAddress );
typedef void     (*NwkStoreSrcRoute_t) (uint8_t* pDestinationAddress, routeRecord_t*  pRouteRecord);
typedef void     (*NwkResetSrcRouteTable_t) ( void );
typedef index_t  (*NwkGetFreeEntryInSrcRouteTable_t) (void);
typedef void     (*NwkStopOrStartCtorDiscTime_t)(bool_t startOrStop); 

/***************************** For Inter-Pan *********************************/
typedef void (*InterPanFunc_t) ( void *msg );
typedef void (*InterPanFuncInd_t) ( mcpsToNwkMessage_t *pMsg );
typedef void (*InterPanFuncCnf_t) (uint8_t msduHandle, uint8_t status);

/**********************************************************************
***********************************************************************
*                             APS Types
***********************************************************************
***********************************************************************/
/**** Fragmentation Capability Types ***********************************************/
/* type returned by retry queue */
typedef index_t apsTxIndex_t;

/* define a set of states */
typedef uint8_t apsTxState_t;

enum {
  gApsTxState_Unused = 0,               /* entry not used */
  gApsTxState_WaitingToTx_c = 1,        /* have big buffer, waiting to Tx (waiting for lock) */
  gApsTxState_WaitingToAllocMsg_c,      /* ready to retry, waiting for buffer */
  gApsTxState_WaitingForNldeConfirm_c,  /* sent, waiting for confirm */
  gApsTxState_GotNldeConfirm_c,         /* got the NLDE confirm */
  gApsTxState_WaitingForAck_c,          /* sent, waiting for APS ACK (indication) */
  gApsTxState_WaitingInterframeDelay_c, /* fragmentation: waiting to send next block this window */
  gApsTxState_PrepareNextBlock_c,       /* fragmentation: create next OTA fragment */
  gApsTxState_GotAck_c,                 /* got APS ACK (indication) */
  gApsTxState_GotAckTimeout_c,          /* ACK timeout */
  gApsTxState_SendConfirm_c,            /* got confirm, waiting to alloc/send it */
  gApsTxState_FindNextBindEntry_c,      /* done with last entry, look for next bind entry */
  gApsTxState_WaitingForIndirect_c,     /* need to wait between indirect msgs */
  gApsTxState_InitFrag_c,               /* initialize the fragmentation packets */
  gApsTxState_Done_c,                   /* done with entry, clean it up */
  gApsTxState_SendNwkAddrReq_c,
  gApsTxState_SearchingNwkAddr_c,        /* searching Nwk address request for an invalid binding*/
  gApsTxState_NwkAddrRspTimeOut_c
};

/* define a set of states */
typedef uint8_t apsRxState_t;
enum {
  gApsRxState_Unused_c =0,          /* not in use */
  gApsRxState_GotFirstBlock_c,      /* received first block */
  gApsRxState_WaitingBetweenAcks_c, /* waiting for another block after we sent an ACK (long wait) */
  gApsRxState_WaitingForBlock_c,    /* waiting for another block in this window (short wait) */
  gApsRxState_GotBlock_c,           /* got a block. Time to move on to a new window? */
  gApsRxState_SendAck_c,            /* time to send ACK to Tx side */
  gApsRxState_SendIndication_c      /* time to send indication to app */
};

typedef uint8_t apsTxFlags_t;

/* Type to know if a continue is necessary after calling a fragmentation state */
typedef uint8_t apsTxResult_t;

typedef uint8_t frmCapAndReqStatus;

/* APS Layer Tables */
/* address map index type */
typedef index_t addrMapIndex_t;

typedef PACKED_UNION apsBindingDst_tag
{
  addrMapIndex_t  index;
  zbGroupId_t     aGroupId;
} apsBindingDst_t;

/* BeeStack in-memory representation of a binding table entry */
typedef PACKED_STRUCT apsBindingTable_tag
{
  zbEndPoint_t    srcEndPoint;      /* always 0x01-0xf0 */
  apsBindingDst_t dstAddr;          /* either group id or address map index */
  zbEndPoint_t    dstEndPoint;      /* multiplex dst endpoint or group  */
  index_t         iClusterCount;    /* count of (packed) clusters in list */
  zbClusterId_t   aClusterList[gMaximumApsBindingTableClusters_c];   /* list of clusters bound for this destination */
}apsBindingTable_t;


/* Pointer type for the binding table to be used in the library. */
typedef PACKED_STRUCT apsBindingTable_Ptr_tag
{
  zbEndPoint_t    srcEndPoint;      /* always 0x01-0xf0 */
  apsBindingDst_t dstAddr;          /* either group id or address map index */
  zbEndPoint_t    dstEndPoint;      /* multiplex dst endpoint or group  */
  index_t         iClusterCount;    /* count of (packed) clusters in list */
  zbClusterId_t   aClusterList[1];  /* list of clusters bound for this destination */
}apsBindingTable_Ptr_t;

typedef uint8_t  zbEpMask_t[gEndPointsMaskSizeInBytes_c];

/*
  Each group entry keeps track of which endpoints belong to this group. Used
  by APSDE_DataRequest to determine if the data indication will be propogated
  to the endpoint. Used by the APSME_xxx management functions.

  Note: gaEndPointDesc describes endpoints. It's an array that contains the
  following:
  [0] = ZDP
  [1] = broadcast endpoint
  [2-n] = user endpoints by # (1-240)

  References
  [R1] sections 2.2.4.5.1 thru 2.2.4.5.6
*/
typedef PACKED_STRUCT zbGroupTable_tag
{
  zbGroupId_t aGroupId;
  uint8_t  aEpMask[gEndPointsMaskSizeInBytes_c];  /* variable length array - 1 bit per endpoint */
}zbGroupTable_t;

typedef PACKED_STRUCT zbGroupTableStackPtr_tag
{
  zbGroupId_t aGroupId;
  uint8_t  aEpMask[1];  /* dummy array entry, but is the size of gEndPointsMaskSizeInBytes_c
                             used inside the stack lib */
}zbGroupTableStackPtr_t;

/* includes the APS Information Base (AIB) Table 2.24 */
typedef struct beeStackParameters_tag 
{
  /* Memory Allocated for AddressMapIndex of ZdoNVM Section */
  uint8_t iAddrMapIndex;

  /* this window size must be the same on both sending and receiving nodes (1-8) */
  uint8_t gApsMaxWindowSize;

  /* ID=0xc9 APS Fragmentation: 0-255, delay in ms between frames */
  uint8_t gApsInterframeDelay;

  /* ID=APS fragmentation, maximum size of fragment for each OTA frame (1-0x4e) */
  uint8_t gApsMaxFragmentLength;

  /* ID=0xc2, should we start up as a coordinator or router? */
  bool_t gfApsDesignatedCoordinator;

  /* ID=0xc6, For Multicast, stack profile 0x02 only */
  uint8_t  gApsNonmemberRadius;

  /* ID=0xc8, should we start up using insecure join? */
  bool_t gfApsUseInsecureJoinDefault;

#if gStandardSecurity_d || gHighSecurity_d
  /*
    053474r17 Table 4.36
    Name: apsTrustCenterAddress
    Type: Device Address
    Range: Any valid 64-bt address
    Default: --
    AIB Id: 0xab
  */

  /* The short addres must be save on NVM as well. */
  zbNwkAddr_t  apsTrustCenterNwkAddress;

  /*
    053474r17 Table 4.36
    Name: The period of time a device will wait for an expected security protocol
          frame (in milliseconds)
    Type: Integer
    Range: 0x0000 - 0xffff
    Default: 1000
    AIB Id: 0xac
  */
  uint16_t apsSecurityTimeOutPeriod;
#endif
} beeStackParameters_t;

typedef index_t bindIndex_t;

typedef struct apsRxFragCtrlSettings_tag
{
  uint8_t iRxWindowStart;
  uint8_t iRxApsMaxWindowSize;
  uint8_t iRxTotalBlocks;
  uint8_t iRxFragmentationGiveUpCounter;
  uint8_t iRxAckBitField;
}apsRxFragCtrlSettings_t;

typedef struct apsTxFragCtrlSettings_tag
{
  uint8_t iTxWindowStart;
  uint8_t iTxApsMaxWindowSize;
  zbApsCounter_t iTxBlockCounter;
  zbApsCounter_t  iTxTotalBlocks;
  uint16_t iTxTotalLength;
}apsTxFragCtrlSettings_t;


/* Used for APS transmit (Tx) state machine. */
/* See also gApsMaxDataHandlingCapacity_c in BeeStackConfiguration.h */
typedef struct apsTxTable_tag
{
  apsTxState_t        state;          /* what state are we in? */
  apsTxFlags_t        flags;          /* what type of transmission is this? */
  uint8_t             cRetries;       /* # of left retries for ACK */
  zbApsCounter_t      orgApsCounter;  /* remember the original APS counter */ 
  zbApsConfirmId_t    apsConfirmId;   /* confirm Id */
  uint8_t             nsduHandle;     /* handle received from NWK layer */
  zbApsCounter_t      apsCounter;     /* APS counter for this transmission */
  zbStatus_t          status;         /* remember error status */
#if (gBindCapability_d)
  bindIndex_t         iBindIndex;     /* last found binding index */
#endif
  zbExtHdr_t          iAckBitField;   /* for fragmentation */
  zbApsdeDataReq_t    *pDataReq;      /* ptr to a "packed" copy of the original data request */
  afToApsdeMessage_t  *pMsg;          /* ptr to current msg, possibly built in Tx state machine */
} apsTxTable_t;

/* Fragmentation Rx state machine variables. all pointers are NULL if unused */
typedef struct apsRxFragmentationTable_tag
{
  apsRxState_t        state;          /* what state are we in? init*/
  uint8_t             keyUsed;        /* remember key used for security init*/
  uint8_t             blockNumber;    /* the block # currently being processed */
  zbApsCounter_t      apsCounter;     /* counter ID of this transaction */
  zbNwkAddr_t         aSrcAddr;       /* source addr for this transaction */  
  zbExtHdr_t          iAckBitField;   /* receive ACK bits */
  apsdeToAfMessage_t *pDataIndMsg;    /* ptr to data indication we're building */
  nldeApsMessage_t   *pMsg;           /* pointer to new message from NWK layer */
  nldeDataIndication_t *pOrgNldeReq;  /* pointer to original nlde-data.req from NWK layer, in packed form, for ACKs */
  zbTmrTimerID_t iRxTimerId;            /* timer id used to wait the next block*/
  apsRxFragCtrlSettings_t apsRxFragCtrlSettings;
} apsRxFragmentationTable_t;

/**********************Permissions Configuration Table*********************/
#if gApsMaxEntriesForPermissionsTable_c
/* Type representing the permissions bit mask */
typedef	uint8_t permissionsFlags_t;
/* Permissions Configuration Table structure */
typedef	struct permissionsTable_tag 
{
  zbIeeeAddr_t authorizedDevice;
  permissionsFlags_t permissionsFlags;
}permissionsTable_t;
#endif 

#if(gInstantiableStackEnabled_d == 1)  
/* Aps struct of globals */  
typedef struct zbBeeStackApsGlobals_tag
{
#endif  

EXTERN  uint8_t                         gApsLastChannelEnergy;
EXTERN  uint8_t                         gApsChannelFailureRate;
EXTERN  uint8_t                         gApsChannelTimer;
EXTERN  addrMapIndex_t                  gGlobalAddressMapCounter;
#if(gInstantiableStackEnabled_d == 1) 
       zbApsDeviceKeyPairSet_t*        gaApsDeviceKeyPairSet;
#else
extern zbApsDeviceKeyPairSet_t         gaApsDeviceKeyPairSet[];
#endif
#if(gInstantiableStackEnabled_d == 1)        
       zbAESKey_t*                     gaApsKeySet;
#else
extern zbAESKey_t                      gaApsKeySet[];
#endif
  #if (gApsAckCapability_d)
#if(gInstantiableStackEnabled_d == 1)       
       zbTmrTimerID_t*                    gaApsTxTimerIDTable;
#else
extern zbTmrTimerID_t                     gaApsTxTimerIDTable[];
#endif
  #endif /*gApsAckCapability_d*/

  /* Memory Allocated for AddressMap table */
#if(gInstantiableStackEnabled_d == 1)        
EXTERN  uint8_t*                        gaApsAddressMapBitMask;
EXTERN  zbAddressMap_t*                 gaApsAddressMap;
#else
extern  uint8_t                         gaApsAddressMapBitMask[];
extern  zbAddressMap_t                  gaApsAddressMap[];
#endif

  /* ID=0xc1, Memory Allocated for Binding table */
#if(gInstantiableStackEnabled_d == 1)        
EXTERN  apsBindingTable_t*              gaApsBindingTable;
#else
extern  apsBindingTable_t               gaApsBindingTable[];
#endif

  /* Memory allocated for GroupTable */
#if(gInstantiableStackEnabled_d == 1)        
EXTERN  zbGroupTable_t*                 gaApsGroupTable;
  
EXTERN  apsTxTable_t*                   gaApsTxTable;
  
EXTERN  apsTxFragCtrlSettings_t*        apsTxFragCtrlSettings;

EXTERN  apsRxFragmentationTable_t*      gApsRxFragmentationTable;

/* duplicate rejection table */
EXTERN  zbApsDuplicateRejectionTable_t* gaApsDuplicateRejectionTable;
#else
extern  zbGroupTable_t                  gaApsGroupTable[];
  
extern  apsTxTable_t                    gaApsTxTable[];
  
extern  apsTxFragCtrlSettings_t         apsTxFragCtrlSettings[];

extern  apsRxFragmentationTable_t       gApsRxFragmentationTable[];

        /* duplicate rejection table */
extern  zbApsDuplicateRejectionTable_t  gaApsDuplicateRejectionTable[];

#endif

EXTERN  beeStackParameters_t           gBeeStackParameters;
  /*************************APS SECURITY*****************************************/ 
  /* NOT saved in NVM */
#if(gInstantiableStackEnabled_d == 1) 
EXTERN  zbKeyEstablish_t**              apSKKEMaterial;
#else
extern  zbKeyEstablish_t*               apSKKEMaterial[];
#endif
  
  #if gHighSecurity_d
  /*  This variable is an array of pointers, each entry keeps the state machine values for
      a Entity authentication process. */
#if(gInstantiableStackEnabled_d == 1)
EXTERN  zbEntityAuthentication_t  **gapEntityAuthenticationMaterial;
#else
extern  zbEntityAuthentication_t  *gapEntityAuthenticationMaterial[];
#endif
  #endif
  /* Permissions Configuration Table */
  #if gApsMaxEntriesForPermissionsTable_c
  /* Permissions Configuration Table */
#if(gInstantiableStackEnabled_d == 1)
EXTERN  permissionsTable_t*  gaPermissionsTable;
#else
extern  permissionsTable_t  gaPermissionsTable[];
#endif

EXTERN  uint8_t gPermissionsTableCount;
#endif  /* (gApsMaxEntriesForPermissionsTable_c) */
#if(gInstantiableStackEnabled_d == 1)    
}zbBeeStackApsGlobals_t;
#endif

/**********************************************************************
***********************************************************************
*                            ZDO Types
***********************************************************************
***********************************************************************/
/************* For ZDO Layer ************/

/*union for address type*/

typedef PACKED_UNION zdoAddress_tag {
  zbNwkAddr_t aShortAddress;   /* Short Address */
  zbIeeeAddr_t aLongAddress;  /* Ieee Address */
} zdoAddress_t;

/*Structure for addreass information of an End device*/
typedef PACKED_STRUCT zdoAddressInfo_tag {
  zbAddrMode_t	addressMode;
  zdoAddress_t zbAddress;
  zbEndPoint_t epAddr;    /*Endpoint Address*/
} zdoAddressInfo_t;

/* Typedef for Semiprecision data */
typedef uint16_t afSemiPrecision_t;

/*
  Max window size type, no  more than a byte according to the docuemtn 085023r05.
  Values: 1 - 8 valid window sizes.
  Zero means not supported.
  Values: 9 - 0xFF are invalid.
*/
typedef uint8_t zbMaxWindowSize_t;

/**********************************************************************
***********************************************************************
*                            Endpoints Types
***********************************************************************
***********************************************************************/

/*
  endPointDesc_t

  this structure points not only to the simple descriptor (ZigBee endpoint
  structure), but also to callbacks for the indications.
*/
typedef PACKED_STRUCT endPointDesc_tag
{
  zbSimpleDescriptor_t *pSimpleDesc;
  dataMsgCallback_t  pDataMsgCallBackFuncPtr;
  dataConfCallback_t  pDataConfCallBackFuncPtr;
  /*
    apsMaxWindowSize for the Endpoint Number.
    Value of 1-8, and 0 for not supporting fragmentation.
  */
  zbMaxWindowSize_t  maxWindowSize;
} endPointDesc_t;

/* For run-time endpoints, there is an array of ptrs to the descriptions */
typedef PACKED_STRUCT endPointPtrArray_tag {
  //const endPointDesc_t  *pDescription; /* Variable pointer to a const endPointDesc_t, the content of the pointer is constant. */
  endPointDesc_t  *pDescription;
} endPointPtrArray_t;


/*Power Descriptor Structure*/
typedef PACKED_STRUCT configPowerDescriptor_tag
{
  /*Current Mode and Available Source*/
  uint8_t     currModeAndAvailSources;
  /*Current Power Source And Lavel*/
  uint8_t     currPowerSourceAndLevel;
}configPowerDescriptor_t;

/*Simple Descriptor Structure*/
typedef PACKED_STRUCT configSimpleDescriptors_tag
{
  /*End point ID */
  uint8_t       endPoint;
  /*Application Profile ID*/
  uint8_t       aAppProfId[ 2 ];
  /*Appliacation Device ID*/
  uint8_t       aAppDeviceId[ 2 ];
  /*Application Device Version And APS Flag*/
  uint8_t       appDevVerAndFlag;
  /*Number of Input Cluster ID Supported by the End Point*/
  uint8_t       appNumInClusters;
  /*Place Holder for the list of Input Cluster ID*/
  uint8_t       aAppInClusterList[1];
  /*Number of Output Cluster ID Supported by the End Point*/
  uint8_t       appNumOutClusters;
  /*Place Holder for the list of Output Cluster ID*/
  uint8_t       aAppOutClusterList[1];
}configSimpleDescriptors_t;

/*Complex Descriptor Structure*/
typedef PACKED_STRUCT configComplexDescriptor_tag
{
    /*Field Count*/
    uint8_t fieldCount;
    /*Language and Charecter Set*/
    uint8_t aLanguageAndCharSet[4];
    /*Manufacturer Name*/
    uint8_t aManufacturerName[6];
    /*Model Name*/
    uint8_t aModelName[6];
    /*Serial Number*/
    uint8_t aSerialNumber[6];
    /*Device URL*/
    uint8_t aDeviceUrl[17];
    /*ICON*/
    uint8_t aIcon[4];
    /*ICON URL*/
    uint8_t aIconUrl[9];
}configComplexDescriptor_t;

/*User Descriptor Structure*/
typedef PACKED_STRUCT configUserDescriptor_tag
{
  /*user Descriptor*/
  zbUserDescriptor_t  aUserDescriptor;
}configUserDescriptor_t;



typedef uint8_t zbTimeType_t[2];

/**********************************************************************
***********************************************************************
*            BEESTACK
***********************************************************************
***********************************************************************/

/* Commissioning Parameters structure, aka Startup Attribute Set or SAS (91 bytes) */
/* note: this structure is cross-layer */
typedef PACKED_STRUCT zbCommissioningAttributes_tag
{
  /*** Startup Attribute Set (064699r12, section 6.2.2.1) ***/
  zbNwkAddr_t aShortAddress;          /* 16bit network address of the device*/
  zbIeeeAddr_t aNwkExtendedPanId;     /* 64-bit extended PAN ID (nwkExtendedPanId) */
  zbIeeeAddr_t aApsUseExtendedPanId;  /* 64-bit extended PAN ID (apsUseExtendedPanId) */
  zbPanId_t aPanId;                   /* 16-bit PAN ID */
  zbChannels_t aChannelMask;          /* set of channels for */
  uint8_t protocolVersion;            /* selects the current protocol version (must be 0x02 for ZigBee 2006+) */
  uint8_t stackProfile;               /* 0x01 or 0x02 */
  uint8_t startupControl;             /* determines how certain other parameters will be used */  
  zbIeeeAddr_t aTrustCenterAddress;   /* used in security as extendedPanId */
  zbAESKey_t aTrustCenterMasterKey;   /* used during key stablishment with TC */
  zbAESKey_t aNetworkKey;             /* network key */
  bool_t fUseInsecureJoin;            /* use insecure join mechanism */
  zbAESKey_t aPreconfiguredTrustCenterLinkKey;   /* key between the device and the TC */
  zbKeySeqNumber_t activeNetworkKeySeqNum;  /* key sequence # of network key */
  zbKeyType_t networkKeyType;         /* key type for network key */
  zbNwkAddr_t aNetworkManagerAddress; /* where to find network manager (for FA) */

  /*** Join Parameters Attribute Set (064699r12, section 6.2.2.2) ***/
  uint8_t      scanAttempts;               /* # of scan attempts (default 5) */
  zbTimeType_t aTimeBetweenScans;     /* time between scans (default 100ms), little endian */
  zbTimeType_t aRejoinInterval;       /* rejoin interval in seconds, little endian */
  zbTimeType_t aMaxRejoinInterval;    /* in seconds, little endian */

  /*** End-Device Parameters Attribute Set (064699r12, section 6.2.2.3) ***/
  zbTimeType_t aIndirectPollRate;     /* poll time for messages from its parent */
  uint8_t parentLinkRetryThreshold;   /* # of missed parent access before finding new parent */

  /*** Concentrator Parameters Attribute Set (064699r12, section 6.2.2.4) ***/
  bool_t fConcentratorFlag;           /* is this a concentrator or not? */
  uint8_t concentratorRadius;         /* radius for many-to-one route discovery */
  uint8_t concentratorDiscoveryTime;  /* how often to send out many-to-one route discovery */
}zbCommissioningAttributes_t;

/*
  BeeStack Configurable Parameter Structure
  Warning: If you modify this structure from logicalType field to descCapField, need to modify
           also zbNodeDescriptor_tag in zigbee.h to make them match
*/
typedef PACKED_STRUCT beeStackConfigParams_tag
{
  /*Beacon Order*/
  uint8_t beaconOrder;                        

  /*Batter Life Extension is Enabled or not*/
  bool_t batteryLifeExtn;                     

  /* Frequency band used by the radio */
  uint8_t apsFlagsAndFreqBand;                

	
  /* Default Value of Manufacturer Code Flag */
  uint8_t manfCodeFlags[2];                   

  /* Default value of maximum buffer size */
  uint8_t maxBufferSize;                      

  /* default Value od Maximum Transfer Size */
  uint8_t maxTransferSize[2];                 

  /* The services server mask */
  uint8_t serverMask[2];

   /*Maximum Outgoing Transfer Size*/
  uint8_t	aMaxOutTransferSize[2];

  /* The descriptor capability field.	*/
  uint8_t descCapField;	

  /*Current Mode and Available Source*/
  uint8_t currModeAndAvailSources;

  /*Current Power Source And Lavel*/
  uint8_t currPowerSourceAndLevel;

  /* Number of Scan Attempts */
  uint8_t gNwkDiscoveryAttempts;              

  /*Nuber of Scan Attemp*/
  uint8_t gNwkFormationAttempts;              

  /*Nuber of Scan Attemp*/
  uint8_t gNwkOrphanScanAttempts;             

  /* timer value for time between the Discovery */
  zbTmrTimeInSeconds_t gDiscoveryAttemptsTimeOut;         

  /*timer value for time between the Formation*/
  zbTmrTimeInSeconds_t gFormationAttemptsTimeOut;         

  /*timer value for time between the Orphan scan*/
  zbTmrTimeInSeconds_t gOrphanScanAttemptsTimeOut;        

#if( gEndDevCapability_d || gComboDeviceCapability_d)
  /* FS:SAS? Timer value for a device to poll for Authentication*/
  uint16_t gAuthenticationPollTimeOut;       

  /* FS:SAS? Timer value for a device to poll*/
  uint16_t gConfirmationPollTimeOut;         
#endif

  /* FS:SAS? Timeout value for authentication process*/
  uint32_t gAuthenticationTimeOut;            

  /* FS:SAS? Preconfigured Network key is used or not*/
  bool_t gNwkKeyPreconfigured;                

  /*Security Level*/
  uint8_t gNwkSecurityLevel;                  

  /*Networ Secure all Frame is Enable d or not: Ignored in No Security*/
  bool_t gNwkSecureAllFrames;                 

  /*
    FS, internal parameter.
    Name: defaultPreconfigureTCKey
    Type: zbKeyType_t
    Range: 0x00 or 0x04
    Default: 0x00
  */
  zbKeyType_t  defaultPreconfigureTCKey;

  /*enable/disable of low power mode*/
  bool_t lpmStatus;

} beeStackConfigParams_t;

#if(gInstantiableStackEnabled_d == 1)    
typedef struct beeStackGlobalsParams_tag
{
#endif  
EXTERN  uint8_t                      aExtendedAddress[8];
EXTERN  zbCommissioningAttributes_t  gSAS_Ram;
   /* commissioning cluster (optional) */
EXTERN  zbCommissioningAttributes_t  *gpSAS_Commissioning;
EXTERN  beeStackConfigParams_t        gBeeStackConfig;
  #if gUser_Desc_rsp_d || gUser_Desc_conf_d
    /* User Descriptor */
EXTERN    zbUserDescriptorPkg_t gUserDesc;
  #endif
  #if gComplex_Desc_rsp_d
    /* Complex Descriptor */
EXTERN    configComplexDescriptor_t gComplexDesc;
  #endif
  
  /* array of pointers to endpoint descriptions, set at runtime */
  /* allow 2 extra ptrs for ZDP and broadcast endpoints */
#if(gInstantiableStackEnabled_d == 1) 
   endPointPtrArray_t* gaEndPointDesc;
  
  /* array of active endpoints, + 2 means that the array will include the ZDO endpoint and the Bradcast Endpoint */
   zbEndPoint_t* gaActiveEndPointList;
#else
    extern endPointPtrArray_t gaEndPointDesc[];
  
  /* array of active endpoints, + 2 means that the array will include the ZDO endpoint and the Bradcast Endpoint */
    extern zbEndPoint_t gaActiveEndPointList[];
#endif
  /* Private debug stuff */
  #if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) && gDefaultValueOfMaxEntriesForExclusionTable_c
#if(gInstantiableStackEnabled_d == 1)  
  zbIeeeAddr_t *gaExclusionTable;
#else
  extern zbIeeeAddr_t gaExclusionTable[];
#endif  
  #endif
  #if gICanHearYouTableCapability_d
    /* 
    ICanHearYou Table Entries = NWK Short Addresses that this device can hear, All packets received 
    via McpsDataIndication with MAC SourceAddress not listed in this table will be discarded by NWK 
    layer. Use for table-top demonstrations and debugging. Not saved to NVM.
    */
#if(gInstantiableStackEnabled_d == 1)  
   zbNwkAddr_t* gaICanHearYouTable;
   uint8_t* gaICanHearYouLqi;
#else
   extern zbNwkAddr_t  gaICanHearYouTable[];
   extern uint8_t      gaICanHearYouLqi[];
#endif   
EXTERN    uint8_t gICanHearYouCounter;
  #endif
  /* note: 0 means use real Link Cost. */
EXTERN  uint8_t giIcanHearLinkCost;
  
EXTERN  bool_t mBUtl_ReceiverIsTurnedOn;
#if(gInstantiableStackEnabled_d == 1)      
}beeStackGlobalsParams_t;
#endif

#ifdef gHostApp_d
  extern  uint8_t  aExtendedAddress[8]; 
  extern  uint8_t giFragmentedPreAcked;
#endif /* gHostApp_d */

#if(gInstantiableStackEnabled_d == 1)     
typedef struct zbStackTablesSizes_tag
{
#endif  
  /* nwk */
EXTERN_CONST  uint8_t gMaxNeighborTableEntries;
EXTERN_CONST  uint16_t gMaxNeighborRelationsTableEntries;
EXTERN_CONST  uint8_t gIncomingFrameCounterSetLimit;
EXTERN_CONST  uint8_t gPacketsOnHoldTableSize;
EXTERN_CONST  uint8_t gHandleTrackingTableSize;
#if ( !gNwkBroadcastPassiveAckRetryCapability_d ) || (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)   
EXTERN_CONST  uint8_t gMaxBroadcastTransactionTableEntries;
#endif  
EXTERN_CONST  uint8_t gMaxRouteDiscoveryTableEntries;
EXTERN_CONST  uint8_t gMaxSourceRouteEntries;
  /* aps */
EXTERN_CONST  uint8_t giApsDeviceKeyPairCount;
EXTERN_CONST  uint8_t gApsKeySetCount;
EXTERN_CONST  uint8_t gApsAddressMapBitMaskInBits;
EXTERN_CONST  uint8_t gApsMaxAddressMapEntries;
EXTERN_CONST  uint8_t gApsMaxBindingEntries;
EXTERN_CONST  uint8_t gApsMaxGroups;
EXTERN_CONST  uint8_t gMaxApsTxTableEntries;
EXTERN_CONST  uint8_t gMaxApsRxFragmentationTableEntries;
EXTERN_CONST  uint8_t giApsDuplicationRejectionTableSize;
EXTERN_CONST  uint8_t gSKKEStateMachineSize;
  #if gHighSecurity_d 
EXTERN_CONST  uint8_t gEAMaterialSize;
  #endif
  #if gApsMaxEntriesForPermissionsTable_c
EXTERN_CONST  uint8_t gApsMaxEntriesForPermissionsTable;
  #endif /* gApsMaxEntriesForPermissionsTable_c */
  #if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) && gDefaultValueOfMaxEntriesForExclusionTable_c
EXTERN_CONST  uint8_t gExclusionMax;
  #endif
  #if gICanHearYouTableCapability_d
EXTERN_CONST  uint8_t gICanHearYouMax;
  #endif
EXTERN_CONST  uint8_t gNoOfEndPoints;
  #if gBkup_Discovery_cache_d || gDiscovery_store_rsp_d || gMgmt_Cache_rsp_d || gRemove_node_cache_rsp_d || gFind_node_cache_rsp_d|| gActive_EP_store_rsp_d || gPower_Desc_store_rsp_d || gNode_Desc_store_rsp_d || gSimple_Desc_store_rsp_d  
EXTERN_CONST  uint8_t gMaximumDiscoveryStoreTableSize;
  #endif
  #if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d  
EXTERN_CONST  uint8_t mEdbMaxEntries;
  #endif
  #if gMgmt_NWK_Disc_rsp_d
EXTERN_CONST  uint8_t gMaxArray;
  #endif
  #if gBind_Register_rsp_d || gRecover_Source_Bind_rsp_d || gBackup_Source_Bind_rsp_d || (gBindCapability_d && gBind_rsp_d)
EXTERN_CONST  uint8_t gMaximumDevicesHoldingBindingInfo;
  #endif
  #if (gReplace_Device_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRecover_Bind_Table_rsp_d || gBind_Register_rsp_d || gMgmt_Bind_rsp_d)
EXTERN_CONST  uint8_t gMaximumBindingCacheTableList;  
  #endif
  #if gSystem_Server_Discovery_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gRecover_Bind_Table_rsp_d || gBackup_Source_Bind_rsp_d || gRecover_Source_Bind_rsp_d || gReplace_Device_rsp_d
EXTERN_CONST  uint8_t gMaximumSystemServerciesResponses;
  #endif
#if(gInstantiableStackEnabled_d == 1)       
}zbStackTablesSizes_t;
#endif 
/****************************************************************************
*****************************************************************************
* Public memory declarations
*****************************************************************************
****************************************************************************/

/**********************************************************************
***********************************************************************
*                             NWK memory declarations
***********************************************************************
***********************************************************************/

/********CONST******/

extern const uint8_t gMaxRouteTableEntries;

/*
  The failure tolerance for the Nwk layer transmitions.
*/
extern const uint8_t gMaxNumberOfTxAttempts;

/* The buffer used is as big as a small bufffer. */
extern const uint8_t gNwkRejoinStateMachineSize;
/* The size of the buffer divided by the amount of entries. */
extern const uint8_t gNwkRejoinStateMachineAmountOfEntries;
/* The amount of time in millisecodn to exppire the Nwk Rejoin state machine. */
extern const zbTmrTimeInMilliseconds_t  gNwkRejoinExpirationTime;

#if ( gRnplusCapability_d )
extern const uint8_t gRouteRequestRetryInterval;
extern const uint8_t gRouteDiscoveryExpirationNumTicks;
extern const uint8_t gRouteDiscoveryExpirationInterval;
#endif /* ( gRnplusCapability_d ) */

extern const bool_t gR20BroadcastLeaveOnZed;

#if gNwkSymLinkCapability_d
#if( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) 
  extern const uint8_t          gNwkLinkStatusMultipleFramesPeriod;
  extern const bool_t           gNwkSendLinkStatusOnDevAnnc;
#endif
#endif

#if ( gRnplusCapability_d )
extern const uint8_t gRouteRequestRetryInterval;
extern const uint8_t gRouteDiscoveryExpirationNumTicks;
extern const uint8_t gRouteDiscoveryExpirationInterval;
#endif /* ( gRnplusCapability_d ) */  
  
extern const bool_t gR20BroadcastLeaveOnZed;

extern bool_t gNwkPanIdConflictDetected;

/* variables used to set broadcast timers */
extern const uint8_t gBroadcastDeliveryNumTicks;
extern const uint8_t gBroadcastDeliveryInterval_c;
extern const uint8_t gBroadcastJitterInterval_c;

/* Nwk nibs */
extern const uint8_t          gNibNwkRouterAgeLimit;
extern const uint8_t          gNwkRouteTableEntryMaxAge; 	
extern const uint8_t          gNibReportConstantCost;
extern const uint8_t          gNibAddrAlloc;
extern const uint8_t          gNwkEnergyLevelThresHold;
extern const uint8_t          gDefaultRetriesStochasticAddrLocalDevice;
extern const uint8_t          gHttMaxIndirectEntries;
extern const zbChannels_t     gaFullChannelList;

extern const uint8_t          gNwkAgingTimeForEndDevice;
extern const uint8_t          gScanDuration;
extern const uint8_t          gNwkAgingTimeForRouter;
extern const uint8_t          gMinutesToExpireRoute; 

extern const uint8_t          gNibMaxBroadcastRetries;
extern const bool_t           gNibSymLink;
extern const zbCounter_t      gDefaultRadiusCounter;
extern const uint8_t          gNwkAgingTimeForSiblingRouter;
  
extern const nvmNwkData_t     gDefaultNwkData;

extern const bool_t gNwkSmartSiblingReplacement;
/***************************** For Inter-Pan *********************************/
extern const uint8_t gInterPanCommunicationEnabled;
extern const InterPanFuncInd_t pInterPanProcessIndication;
extern const InterPanFuncCnf_t pInterPanProcessConfirm;

/***************************** Conflict Resolution NWK**************************/
extern const PIDConflictDetect_t pPIDDetection;
extern const PIDConflictResol_t  pPIDResolution;

/***************************** Stack Hooks **************************/
extern const NodeNotInParentLinkStatus_t  pfNodeNotInParentLinkStatus;
extern const UpdateNtEntryLQI_t  pfUpdateNtEntryLQI;

/**************** HighRamConcentrator function pointers *********************/
extern const NwkResetSrcRouteTable_t           pNwkResetSrcRouteTable;
extern const NwkGetFreeEntryInSrcRouteTable_t  pNwkGetFreeEntryInSrcRouteTable;
extern const NwkRetrieveSrcRoute_t             pNwkRetrieveSrcRoute;
extern const NwkStoreSrcRoute_t                pNwkStoreSrcRoute;
extern const NwkStopOrStartCtorDiscTime_t      pNwkStopOrStartCtorDiscTime;
extern const uint8_t                           gNibNwkMaxSourceRoute;   

/* DATA */
extern secClearDeviceDataEvent_t gSecClearDeviceDataEvent;

#if(gInstantiableStackEnabled_d == 1)  
extern zbBeeStackNwkGlobals_t* pZbBeeStackNwkGlobals;
#endif
/**********************************************************************
***********************************************************************
*                             APS memory declarations
***********************************************************************
***********************************************************************/
/* CONST */
/* specifies the # of retries APS uses */
extern const uint8_t          gApsMaxRetries;  
/* specifies the wait duration on APS ACKs before retrying */
extern const uint16_t         gApsAckWaitDuration;  
extern const bool_t           gfFragmentationEnabled;  

#if gStandardSecurity_d || gHighSecurity_d
  /*
    allows non-secure APS data packets on a secure network. 
    If this option is true, the network is not ZigBee compliant.
    Has no effect if security is disabled.
  */
  extern const uint8_t gAllowNonSecurePacketsInSecureMode;
  
#endif

extern const uint8_t gApsMaxClusterList;

extern const uint32_t gAps64BitAddressResolutionTimeout;

extern const uint16_t gDefaultApsSecurityTimeOutPeriod;

#if gEndDevCapability_d || gComboDeviceCapability_d
extern const uint16_t gApsFragmentationPollRate;
#endif

/* for APS Rx Fragmentation State Machine */
//extern const uint8_t gMaxApsRxFragmentationTableEntries;

//extern zbAESKey_t  gaApsKeySet[][gApsMaxLinkKeys_c];

/* ID=0xc1, Memory Allocated for Binding table */
//extern apsBindingTable_t gaApsBindingTable[][gMaximumApsBindingTableEntries_c];

/* APS global ram structure */
//extern zbBeeStackApsGlobals_t gaZbBeeStackApsGlobals[];

#if(gInstantiableStackEnabled_d == 1)  
/* APS pointer to global ram structure */
extern zbBeeStackApsGlobals_t* pZbBeeStackApsGlobals;
#endif
/**********************************************************************
***********************************************************************
*                             ZDO memory declarations
***********************************************************************
***********************************************************************/
extern const uint8_t zdoTxOptionsDefault;
/* CONST */
#if gStandardSecurity_d || gHighSecurity_d
/* Set the tx options by default. */
extern const uint8_t gTxOptionsDefault;
#endif /* gStandardSecurity_d || gHighSecurity_d */

#if(gInstantiableStackEnabled_d == 0)    
/* There are 4 copies of the startup attributes: ROM, RAM(working copy), NVM and commissioning cluster */
extern const zbCommissioningAttributes_t gSAS_Rom;        /* factory set */
#else
extern const zbCommissioningAttributes_t gSAS_Rom[2];
#endif

extern const zbPanId_t          gaPIDUpperLimit;

/* RAM */

/***************************BeeStack config **********************************/

#ifdef gHostApp_d
  extern uint8_t  aExtendedAddress[];
#endif /* gHostApp_d */

#if gApsLinkKeySecurity_d
extern const uint8_t mMICLength[8];
#endif 


//extern beeStackGlobalsParams_t beeStackGlobalsParams[];
#if(gInstantiableStackEnabled_d == 1)  
extern beeStackGlobalsParams_t* pBeeStackGlobalsParams;
#endif

/*******************************************************************************
********************************************************************************
* Public prototypes
********************************************************************************
*******************************************************************************/

/*******************************************************************************
* This function is invoked when the Mac layer sends Active scan
* confirmation. Here a logical channel to form a Network is selected
* The criteria for Selecting the Channel is the first one with Zero
* networks or the one containing the least number of PAN's"
*
* Interface assumptions:  This function is called only if ED Scan Confirm is
* obtained from the MAC.
* Return value:  void
*
* Effects on global data:
*  gSelectedLogicalChannel  - This will select the Channel based on Active Scan
*                             confirm
******************************************************************************/
void SelectLogicalChannel
(
  const nwkMessage_t *pMsg, /* IN - Pointer to the Active Scan Confirm */
  uint8_t* pScanChannels,
  uint8_t* pSelectedLogicalChannel
);

/***************************************************************************
* Using the already collected information onthe discovery process and the
* Energy scan, gathers the information and choose which channel is the one
* with less networks on it, using the channel list as a reference and then
* returns the channel number (11 .. 26).
*
* Interface assumptions:
*   The parameter pChannelList is not a null pointer.
*
* Return value:
*   The number id of the channel with less network available (11 .. 26).
****************************************************************************/
uint8_t SelectChannelFromTables
(
  uint8_t  *pChannelList  /* IN: The list of channel to be consider during the forming. */
);

/******************************************************************************
* This fuction is used to Select the PanId for the device
* that wants to form a Network when Upper layer specifies NULL as PanId.
*
* Interface assumptions:
*   None    Called only when the Pan Id specified by Upper layer is NULL.
* Return value:   void
*
* Effects on global data:
*   gaNibPanId Pan Id is generated.
*******************************************************************************/
bool_t SelectPanId
(
  const nwkMessage_t *pMsg,  /* IN/OUT -Pointer to Active Scan Confirm Paramters */
  uint8_t SelectedLogicalChannel,
  zbPanId_t aPanId
);

/***************************************************************************
* Using the Pan Id provided in the formation request, search on the
* discovery tables to see if the given pan Id is not on conflict. Or if the
* Given Pan Id is not a valid one then generate one and check for conflicts.
*
* Interface assumptions:
*   The parameter aPanId is a valid memory space and an input and output
*   parameter.
*   The parameter channel is avalid cahnnel number and is an input parameter
*   only.
*
* Return value:
*   TRUE if there is a conflict. FALSE other wise.
****************************************************************************/
bool_t SelectPanIDFromTables
(
  zbPanId_t  aPanId,  /* IN/OUT */
  uint8_t  channel    /* IN */
);

/***************************************************************************
* Generates a random and valid ZigBee PanId.
*
* Interface assumptions:
*   The parameter aPanId is a valid memory location. aPanI is an output
*   parameter.
*
* Return value:
*   NONE.
****************************************************************************/
void getRandomPanId
(
  zbPanId_t  aPanId  /* OUT: This parameter is used as an output. */
);

/**********************************************************************************
*
* Cheks if in a particular channel exists another network using the same Extended
* Pan Id we are tryig to use to form our network.
* Returns True if there is a confilct and False if there
*
**********************************************************************************/
bool_t CheckForExtendedPanIdConflict
(
  uint8_t logicalChannel /* IN - Log.Channel for determining PanId */
);

/****************************************************************************
 Look for a router (ZC or ZR) to join.
****************************************************************************/
index_t SearchForSuitableParentToJoin ( void );

/******************************************************************************
 Init the Address Map Table (fill it with zeros).
*******************************************************************************/
void AddrMap_InitTable(void); 

/******************************************************************************
 Get an entry from the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_GetTableEntry
(
  uint8_t index, 
  void *pEntry
);

/******************************************************************************
  Search IEEE address OR short address in the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_SearchTableEntry
(
  zbIeeeAddr_t   *pIeeeAddr,
  zbNwkAddr_t    *pNwkAddr, 
  void           *pEntry
);
                                 
/******************************************************************************
 Set an entry in the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_SetTableEntry(uint8_t index, void *pEntry);

/******************************************************************************
 Remove an entry from the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_RemoveEntry(uint8_t index);

/*******************************************************************************
Save an Address Map Table entry to NVM
*******************************************************************************/
void AddrMap_SaveEntryToNvm(uint8_t iNvmObject, uint8_t index);

 /******************************************************************************
 Init the Device Key Pair Set table (fill it with zeros).
*******************************************************************************/
void DevKeyPairSet_InitTable(void);

/******************************************************************************
 Get an entry from the Device Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_GetTableEntry(uint8_t index, void *pEntry);

/******************************************************************************
 Set an entry in the evice Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_SetTableEntry(uint8_t index, void *pEntry);

/******************************************************************************
 Remove an entry from the Device Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_RemoveEntry(uint8_t index);

/******************************************************************************
 Save an entry from the Device Key Pair Set table to NVM.
*******************************************************************************/
void DevKeyPairSet_SaveEntryToNvm(uint8_t iNvmObject, uint8_t index);

/******************************************************************************
 Init the Key Set table (fill it with zeros).
*******************************************************************************/
void KeySet_InitTable(void);

/******************************************************************************
 Get an entry(a key) from the Key Set table.
*******************************************************************************/
uint8_t KeySet_GetTableEntry(uint8_t index, void *pKey);

/******************************************************************************
 Set an entry (a key) in the Key Set table.
*******************************************************************************/
uint8_t KeySet_SetTableEntry(uint8_t index, void *pKey);

/******************************************************************************
 Remove an entry (a key) from the Key Set table.
*******************************************************************************/
uint8_t KeySet_RemoveEntry(uint8_t index);

/******************************************************************************
  Search a key in the Key Set table.
*******************************************************************************/
uint8_t KeySet_SearchTableEntry(zbAESKey_t *pKey);

/******************************************************************************
  Verify if the APS profileID in a profile descriptor match frame value or wildcard
*******************************************************************************/
bool_t IsProfileIdMatched(zbSimpleDescriptor_t *pSimpleDescriptor, 
                          zbProfileId_t aProfileId);

/******************************************************************************
* This function is called by the network layer when the Routing table
* is full. Default behaviour is not to free a route entry.
* The example function finds an expired entry and passes the index
* to the network layer. A counter is used to avoid the same entry is not freed
* every time.
* Note the entry must be marked as gActive_c for it to able to get freed.
*
******************************************************************************/
index_t ExpireAndGetEntryInRouteTable(void);


void CleanUpPacketsOnHoldTable(zbNwkAddr_t aDestinationAddress);

/******************************************************************************
* This function is called by the network layer when a ZeD device increments
* its parent transmit failure counter.
* The function initiates a network rejoin if the transmit failure is met.
* 
* NOTE! only the transmit failure limit (gMaxDataTransmitFailure_c) should
* be changed, or alternativly the ZDO start command can be removed if a rejoin
* is not wanted.
******************************************************************************/
bool_t ZeDTransmitFailureCounterCheck(uint16_t FailureCounter);

/******************************************************************************
* This function is called everytime a child joins the device.
* This method generates an stochastic short address for the device. 
* This module returns the address of the device that has joined the parent.
*
* Interface assumptions: NONE
*      
* Return value:    uint8_t*   
* Effects on global data:
* 
******************************************************************************/
void NWK_ADDRESSASSIGN_GenerateAddressStochasticMode
(
  uint8_t*  pShortAddrGenerated 
);

void FreeSourceRouteEntry(index_t iIndex );

#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) && gRnplusCapability_d
/* This function start the aging of the route and source route tables*/
void StartRouteAndSourceRouteTable(void);
#endif

extern void SSP_NwkResetSecurityMaterials(void);
/******************************************************************************
* This function initialize structures from beestack
******************************************************************************/
void BeeStackGlobalsInit(uint8_t instId);
/******************************************************************************
* This function set pointers for current instances structures 
******************************************************************************/
void BeeStackGlobalsSetCurrentInstance(uint8_t instId);


#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
bool_t CommandHasPermission
(
  zbNwkAddr_t aSrcAddr,
  permissionsFlags_t  permissionsCategory
);
#endif /* ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */
/****************************************************************************
*****************************************************************************
* Fragmentation Capability Headers
*****************************************************************************
****************************************************************************/

/* state machine that handles fragmented Tx (transmit). May be disabled at compile-time. */
apsTxResult_t APS_ActualFragmentationTxStateMachine(apsTxIndex_t iTxIndex, apsTxState_t state);
apsTxResult_t APS_StubFragmentationTxStateMachine(apsTxIndex_t iTxIndex, apsTxState_t state);
bool_t APS_FragmentationTxStateMachine(apsTxIndex_t iTxIndex, apsTxState_t txState);
zbStatus_t Aps_StubSaveFragmentedData(void);
zbStatus_t Aps_ActualSaveFragmentedData(index_t iIndexRxTable);

/* state machine that handles fragmented Rx (receive).  May be disabled at compile-time  */
void APS_ActualFragmentationRxStateMachine(void);
void APS_StubFragmentationRxStateMachine(void);
void APS_FragmentationRxStateMachine(void);

#ifdef __cplusplus
}
#endif

#endif /* _BEESTACK_GLOBALS_H_ */

