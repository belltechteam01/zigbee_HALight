/***************************************************************************
* This is the header file for the NwkInternalVariables
*
* (c) Copyright 2013, Freescale, Inc. All rights reserved.
*
* Freescale Semiconductor Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*****************************************************************************/
#ifndef _NWK_INTERNAL_VARIABLES_H_
#define _NWK_INTERNAL_VARIABLES_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "NwkCommon.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

#if(gInstantiableStackEnabled_d == 0)
  #define ZbNwkPrivateData(val)          val
  #define ZbNwkPublicData(val)           val
#else
  #define ZbNwkPrivateData(val)            pZbNwkPrivateData->val
  #define ZbNwkPublicData(val)             pZbNwkPublicData->val
#endif

/* NwkDataServices*/
#define gDevRepPIDConflictTableSize_c 5


/*****************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
/* NwkLeave*/
typedef uint8_t leaveState_t;

#if(gInstantiableStackEnabled_d == 1)
/* Nwk module internal used ram global structure */
typedef struct zbNwkPrivateData_tag
{
#endif  
  
  /* NwkCommandFrameCreate */
  #if ( ( gCoordinatorCapability_d || gRouterCapability_d ) || gComboDeviceCapability_d )
  /* Route Request ID - This is incremented by one, each time, when a route
     request is created by the current device */
EXTERN  uint8_t gRouteRequestId;
  #endif /* (( gCoordinatorCapability_d || gRouterCapability_d ) || gComboDeviceCapability_d) */
  
  /*NwkDataFrameCreate.*/
  /* maybe is not necessary */
EXTERN  uint8_t gTypeOfDataFrameCreated;
  
    /*NwkDataServices*/
EXTERN  uint8_t gPanIdConflictDetectionInterval;
  /*
      Number of devices required to report the Pan Id conflict until de Nwk Manager
     changes the Pan Id.
    */
EXTERN  uint8_t gPIDConflictDevRepThershold;
  #if( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d )
  #if( gNwkSymLinkCapability_d )
EXTERN    uint8_t NumberOfFrames;
EXTERN    uint8_t FramesCounter;	
  #endif /* ( gNwkSymLinkCapability_d ) */  
  #endif /* ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) */ 
  /* NwkJoin */
EXTERN  bool_t StartAging;
   /* NwkLeave */
  /* By default Leave will be in initial state*/
EXTERN  leaveState_t mLeaveState;
EXTERN  uint8_t mLeaveStatus;
   /*NwkMain*/
  /*use post r20 certification fixes*/
EXTERN  bool_t gEnableFPR20;
  /* NwkMngServices */
  /* Parents BO Set after the device joins */
EXTERN  uint8_t gCoordinatorBeaconOrder; 
  /* Parents SO Set after the device joins */
EXTERN  uint8_t gCoordinatorSuperFrameOrder; 
  /***********************************************************************************************************/	
  /* NwkAddrAssinment*/
  #if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
  /* Holds the offset ( Cskip ) of the current device. In case of Distributed
     Addressing Mode, this is calculated, based on MaxDepth, MaxChildren,
     MaxRouter and current device depth */
EXTERN  uint16_t mCSkipAtThisLevel;
  #if gBlockAddressingCapability_d
  /* Holds the Available Address in case of Block Addressing Mode, which
      indicates the max number of children that can effectively join the
      current device */
EXTERN  uint16_t nwkAvailableAddress;
  /* Holds the Address Increment, which is the CSkip ( Offset ) in case of
      Block Addressing Mode */
EXTERN  uint16_t nwkAddressIncrement;
  /* Holds the nwkNextAddress, from NIB */
EXTERN  uint16_t nwkNextAddress;
  #endif
  #endif
  
  /*NwkBroadcast*/
  #if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
EXTERN  broadcastTransactionTable_t *gpBroadcastInProgress;
   /* module variables used to deliver broadcast to sleeping devices */
EXTERN  index_t mUnicastingIndex;
EXTERN  bool_t  mUnicastPending;
EXTERN  bool_t  mStartUnicast;
  #if( gNwkBroadcastPassiveAckRetryCapability_d )
  /* If Passive Ack & Retry are not supported, then Nwk Layer can handle 4
     broadcast at a time simultaneously. The limitation of four is due to timer
     availability */
  /* Holds the sequence number of the broadcast data which is under progress */
EXTERN  uint8_t mBroadcastSequenceNumberInProgress;
  /* Indicates the type of broadcast message that is buffered */
EXTERN  uint8_t gBroadcastBufferMsgType;
  #endif
  #endif
  
  /*NwkDataServices*/
  /*
    Table containing the devices that reported the Pan Id conflict. Used for
    duplicate reporting detection.
    */
EXTERN  zbNwkAddr_t gaDevRepPIDConflictTable[gDevRepPIDConflictTableSize_c];
  /* gDiscoverRoute global variable is used to avoid rom patching for mc13226 */
EXTERN  uint8_t gDiscoverRoute;
  /* remove remove */ 
EXTERN  bool_t gKeepInCommunicationWhenNotInLinkStatus;
  #if( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d )
    #if( gNwkSymLinkCapability_d )
      /* Indicate if this node has to send multiple frames */
EXTERN      bool_t multipleFrames;
EXTERN      index_t iIndexInNT;
EXTERN      index_t iIndex; 
      /* Flag to indicate if we need send multiple frames. Default is 1 frame */
EXTERN      uint8_t NumOFRouterPerList;
EXTERN      uint8_t RoutersJoined;
EXTERN      uint8_t RoutersAlreadySent;
    #endif /* ( gNwkSymLinkCapability_d ) */  
  #endif /* ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) */ 
EXTERN  uint8_t gMsduHandle; 
EXTERN  bool_t gKeepNeighborInformation;
  #if( gEndDevCapability_d || gComboDeviceCapability_d )
EXTERN  uint16_t previouspollTimeOut;
  #endif
EXTERN  uint8_t mSecurityRequired;
  #if( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d )
  #if( gRnplusCapability_d )  
  /* Holds the address of the source data, to which the */
EXTERN  zbNwkAddr_t gaAddressToSendRouteError;
  #endif /* ( gRnplusCapability_d ) */
  #endif /* ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) */
  /* This flag and the 3 next variablesis are set if the broadcast message arrived has a network update command as nwk payload*/
EXTERN  bool_t gIsNwkUpdateCmdIndication;
EXTERN  zbPanId_t gNewPanIdInUpdateInformation;
EXTERN  zbNwkAddr_t gTmpSrcAddr;
EXTERN  uint8_t gTmpSequenceNumber;
  /* This flag is to track the network update command broadcast message inside of the BTT */
EXTERN  bool_t gIsANwkUpdateCommandRequest;
EXTERN  uint8_t InterPanMsduHandle;
EXTERN  bool_t  InterPanInProgress;
  
  /*NwkInfoBase*/
EXTERN  zbNwkAddr_t aLastNwkAddrUsed;
  
  /*NwkJoin*/
  /* used during joining to keep track of potential parent from the discovery list*/
EXTERN  index_t gPotentialParentIndex;
  #if ( gCoordinatorCapability_d ) || ( gRouterCapability_d ) || (gComboDeviceCapability_d)
  /*mJoiningDevCapabilityInfo stores the capability information of the
     joining  device when sending association  response*/
EXTERN  uint8_t mJoiningDevCapabilityInfo;
  /*mAssociatedeviceType stores the device type(Router\EndDevice) of the
     associated device*/
EXTERN  uint8_t mAssociatedeviceType;
  /*maAssocRespShortAddr stores the Nwkaddress Allocated to the device on  Successful association*/
EXTERN  zbNwkAddr_t maAssocRespShortAddr;
  #endif
  #if( gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d )
  /*Ptr to Joinequest that is buffered in case join has to be retried */
EXTERN  zdoNlmeMessage_t *mpNwkJoinMsgBuf;
  #endif
  
  #if ( gRouterCapability_d || gComboDeviceCapability_d ) && ( gBeaconSupportCapability_d )
  /*Beacon TxOffset of the parent device */
EXTERN  uint8_t gaParentTxOffset[3];
  #endif
EXTERN  nwkDataBackup_t * gpNwkDataBackup;
  /* NwkLeave*/
  /* By default Leave will be in initial state*/
EXTERN  neighborTable_t *mpLeavingDeviceNTE;
EXTERN  index_t mLeaveIndex;
EXTERN  nlmeLeaveReq_t *mpCurrentLeaveRequest;
  
  /*NwkMain*/
  /* code reduction */
EXTERN  zbTmrTimerID_t gNwkTimerIDs[gMaxVectorTimerSize_c];
  
  /* NwkMngServices */
  #if ( gCoordinatorCapability_d ) || ( gRouterCapability_d ) || (gComboDeviceCapability_d)
  /* Below three variables are made as global to optimize from fetching
         from the NIB every time */
  /* Max EndDevice in the NIB */
EXTERN  uint8_t gNwkMaxEndDevice; 
  #endif /* ( gCoordinatorCapability_d ) || ( gRouterCapability_d ) || (gComboDeviceCapability_d) */ 
  
  /* NwkPermitJoin*/
EXTERN  bool_t gbPanIdConflictPending;
EXTERN  uint8_t gJoinLogicState;
  
  /* NwkRouting*/
  #if ( gCoordinatorCapability_d ) || ( gRouterCapability_d ) || (gComboDeviceCapability_d)
  #if ( gRnplusCapability_d )
  /* Holds the index of the current route discovery table entry */
EXTERN  uint8_t mCurrentRouteDiscoveryTableIndex;
  /* Holds the index for the current route table entry */
EXTERN  uint8_t mCurrentRouteTableIndex;
  #endif /* ( gRnplusCapability_d ) */
  #endif /* ( gCoordinatorCapability_d ) || ( gRouterCapability_d ) || (gComboDeviceCapability_d) */
  /* Dest. Addr for Mcps Data req */
EXTERN  zbNwkAddr_t aDestAddress;
  /* Indicates the value that the error code, in the route error comand frame will carry. */
EXTERN  nwkStatusCode_t gRouteErrorReason;
  
  /*NwkStartRouter*/
  #if (gBeaconSupportCapability_d )
  /* Beacon Offset for Current Device */
EXTERN  uint8_t gCurrentDeviceTxOffset[3]; 		
  #endif
  
  /*NwkSync*/
EXTERN  bool_t gConfimationPoll;

#if(gInstantiableStackEnabled_d == 1)  
}zbNwkPrivateData_t;
#endif

#if(gInstantiableStackEnabled_d == 1)
/* Nwk module public ram global structure */
typedef struct zbNwkPublicData_tag
{
#endif
  
  /* NwkDataServices */ 
  #if ( gCoordinatorCapability_d ) || ( gRouterCapability_d ) || (gComboDeviceCapability_d)
EXTERN  uint8_t iHtteIndexActualEntry;
  #endif /* ( gCoordinatorCapability_d ) || ( gRouterCapability_d )  || (gComboDeviceCapability_d) */
  
  /* NwkJoin */
  #if gStandardSecurity_d || gHighSecurity_d
EXTERN  secClearDeviceDataEvent_t gSecClearDeviceDataEvent;
  #endif
  
  /***********************************************************************************************************/	
  /* NwkDataServices */
EXTERN  anchor_t gNwkEaReuDataQueue;
  #if ( gCoordinatorCapability_d ) || ( gRouterCapability_d ) || (gComboDeviceCapability_d)
  #if( gRnplusCapability_d )
    /* Holds the address for which the route is to be repaired */
EXTERN    zbNwkAddr_t gaAddressForRouteRepair;
  #endif /* ( gRnplusCapability_d ) */	
  #endif /* ( gCoordinatorCapability_d ) || ( gRouterCapability_d )  || (gComboDeviceCapability_d) */
  
  /*NwkFormation*/
  #if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d )
EXTERN    zbChannels_t gaScanChannels;  /* ScanChannel Specified by Upperlayer */
  #endif
  
  
  /*NwkMain */
  /* Queue for the messages FROM and TO NWK  Layer */
EXTERN  anchor_t mZdoNlmeInputQueue;
EXTERN  anchor_t mApsNldeInputQueue;
EXTERN  anchor_t mMcpsNwkInputQueue;
EXTERN  anchor_t mMlmeNwkInputQueue;
  
  /* NwkMngServices */
  /* Denotes whether Zdo has given NULL PanId */
EXTERN  bool_t  gUseRandomPanId;

#if(gInstantiableStackEnabled_d == 1)  
}zbNwkPublicData_t;
#endif
/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
#if(gInstantiableStackEnabled_d == 1)  
/* pointer used to access instance id variables */
extern zbNwkPrivateData_t* pZbNwkPrivateData; 

/* pointer used to access instance id variables */
extern zbNwkPublicData_t* pZbNwkPublicData;
#endif
/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/
/******************************************************************************
* Initialize zigbee network variables with the default values.
*
*
* Return value:
*   VOID
*******************************************************************************/
void NwkInitVariables(uint8_t instId);
#ifdef __cplusplus
}
#endif
#endif /* _NWK_INTERNAL_VARIABLES_H_ */
