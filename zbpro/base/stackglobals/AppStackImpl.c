/******************************************************************************
* This is the source file which implements the channel PAN ID selection logic.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*******************************************************************************/
#include "EmbeddedTypes.h"
#include "FunctionLib.h"
#include "NV_Data.h"
#if (defined(gHostApp_d) || defined(gBlackBox_d))  
#include "ZtcInterface.h"
#endif

#include "Zigbee.h"
#include "Beestack_Globals.h"
#include "BeeStackUtil.h"
#include "BeeStackConfiguration.h"
#include "BeeStackInterface.h"
#include "BeeCommon.h"
#include "NwkMacInterface.h"
#include "nwkcommon.h"
#include "NwkVariables.h"

#include "ZdoNwkInterface.h"
#include "ZdoNwkManager.h"
#include "ZdoApsInterface.h"
#include "ZdoMain.h"
#include "ZdoStateMachineHandler.h"
#include "ZdpManager.h"
#include "ZdoVariables.h"

#include "ApsMgmtInterface.h"

#include "ASL_UserInterface.h"
#include "ASL_ZdpInterface.h"

#include "ApsVariables.h"
#include "AfVariables.h"
#include "BeeStackRamAlloc.h"
#include "ZclSE.h"
/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

/* Tells the criteria used to search suitable router to join */
#define mExtendedPanId_c        0x00
#define mNwkPanId_c             0x01
#define mAnyPanId_c             0x02

#define gRouterCapacity_c       0x04

#define gEndDeviceCapacity_c    0x80

#define gDeviceDepthMask_c      0x78

#define gZigbeeProStackProfile  0x02

#define gZigbeeStackProfile1    0x01

#define mNotFound               0xFF

#define IsStackProfileCompatible( pPayload ) (( gNwkProtocolId == pPayload->protocolId ) && \
                                             ((( pPayload->Info & 0x0F ) == NlmeGetRequest(gNwkStackProfile_c))|| \
                                             ((( pPayload->Info & 0x0F ) == gZigbeeProStackProfile || ( pPayload->Info & 0x0F ) == gZigbeeStackProfile1) &&  NlmeGetRequest(gNwkStackProfile_c))))

/*  
  0 = no security
  1 = no encryption, 32-bit MIC (no encryption, but authentication)
  2 = no encryption, 64-bit MIC  
  3 = no encryption, 128-bit MIC 
  4 = encryption, no MIC (encrypted but no authentication)
  5 = encryption, 32-bit MIC (default) 
  6 = encryption, 64-bit MIC
  7 = encryption, 128-bit MIC 
*/
#if ((mDefaultValueOfNwkSecurityLevel_c == 0) || (mDefaultValueOfNwkSecurityLevel_c == 4))
  #define mMICSize_c    0
#elif ((mDefaultValueOfNwkSecurityLevel_c == 1) || (mDefaultValueOfNwkSecurityLevel_c == 5))
  #define mMICSize_c    4
#elif ((mDefaultValueOfNwkSecurityLevel_c == 2) || (mDefaultValueOfNwkSecurityLevel_c == 6))
  #define mMICSize_c    8
#else
  #define mMICSize_c    16
#endif

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

void ResetSourceRouteTable(void);

index_t GetFreeEntryInSourceRouteTable(void);

sourceRouteTable_t* NwkRetrieveSourceRoute(uint8_t * pDestinationAddress);

void NwkStoreSourceRoute(uint8_t * pDestinationAddress, routeRecord_t * pRouteRecord);

void NwkStartOrStopNwkConcentratorDiscoveryTime( bool_t startOrStop );

/******************************************************************************
* This function is used to aging the route and source route table entries
******************************************************************************/
#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) && gRnplusCapability_d

/* This function read both route and source route table entries and decrement the aging field value. */
void AgingRouteAndSourceTableEntries( void );

/* This is the custom call back timeout for the route and source route table aging timer*/
void CustomAgingRouteAndSourceTableTimeOutCallBack(zbTmrTimerID_t timerId );

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
#endif


#if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) && gRnplusCapability_d
void CleanUpPacketsOnHoldTable(zbNwkAddr_t aDestinationAddress);
#endif

#if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) && gRnplusCapability_d
void CleanUpPacketsOnHoldTable(zbNwkAddr_t aDestinationAddress);
void SetInactiveRotueEntry(index_t iIndex);
#endif
/******************************************************************************
*******************************************************************************
* Private Type Definations
*******************************************************************************
******************************************************************************/

/*None*/

/******************************************************************************
*******************************************************************************
* Private Memory Decleration
*******************************************************************************
******************************************************************************/

/*None*/

/******************************************************************************
*******************************************************************************
* Global variables
*******************************************************************************
******************************************************************************/
/********************* HighRamNwkConcentrator **********************/
#if gConcentratorFlag_d


  const NwkResetSrcRouteTable_t           pNwkResetSrcRouteTable           = ResetSourceRouteTable;
  const NwkGetFreeEntryInSrcRouteTable_t  pNwkGetFreeEntryInSrcRouteTable  = GetFreeEntryInSourceRouteTable;
  const NwkRetrieveSrcRoute_t             pNwkRetrieveSrcRoute             = NwkRetrieveSourceRoute;
  const NwkStoreSrcRoute_t                pNwkStoreSrcRoute                = NwkStoreSourceRoute;
  const NwkStopOrStartCtorDiscTime_t      pNwkStopOrStartCtorDiscTime      = NwkStartOrStopNwkConcentratorDiscoveryTime;
  const uint8_t                           gNibNwkMaxSourceRoute            = gNwkMaxHopsInSourceRoute_c;
#else
  const NwkResetSrcRouteTable_t           pNwkResetSrcRouteTable           = NULL;
  const NwkGetFreeEntryInSrcRouteTable_t  pNwkGetFreeEntryInSrcRouteTable  = NULL;
  const NwkRetrieveSrcRoute_t             pNwkRetrieveSrcRoute             = NULL;
  const NwkStoreSrcRoute_t                pNwkStoreSrcRoute                = NULL;
  const NwkStopOrStartCtorDiscTime_t      pNwkStopOrStartCtorDiscTime      = NULL;
  const uint8_t                           gNibNwkMaxSourceRoute            = gZero_c;
#endif

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/



#if gStandardSecurity_d || gHighSecurity_d
extern zbKeyType_t ZDO_SecGetDeviceKeyType
(
  zbIeeeAddr_t aDeviceAddress
);
#endif  /* gStandardSecurity_d || gHighSecurity_d */

extern uint8_t * APS_GetNwkAddress
(
  uint8_t * pNwkAddr, 
  uint8_t   aWereToCpyNwkAddr[]
);

extern uint8_t * APS_GetIeeeAddress
(
  uint8_t * pNwkAddr,
  uint8_t   aWereToCpyExtAddr[]
);

/*
  GetMaxApplicationPayload

  Returns the maximum application payload (ASDU) given the data request.  
*/
uint8_t GetMaxApplicationPayload
  (
  zbApsdeDataReq_t *pDataReq /* IN: potential data request */
  )
{
  /* Maximum Packet size */
  #define mPhyLen_c         127
  
  /* Default header sizes */
  #define mMacHdrSize_c     11 /* |FC(2) | seqNo(1) | dstPan(2) | dstAddr(2) | srcAddr(2) | fcs(2) */
  #define mNwkHdrSize_c     8  /* |FC(2) | dstAddr(2) | srcAddr(2) | radius(1) | seqNo(1) */
  #define mApsHdrSize_c     8  /* |FC(1) | dstEp(1) | clusId(2) | profId(2) | srcEp(1) | counter(1) */

  /* Optional extra overheads */
  #define mNwkSrcRouteSize_c (1 + 1 + gNwkMaxHopsInSourceRoute_c*2) /* relayCount(1) + relayIndex(1) +  gNwkMaxHopsInSourceRoute_c * zbNwkAddr_t(2)*/
  #define mNwkAuxSize_c     (14+mMICSize_c) /* |SC(1) | frameCtr(4) | srcAddr(8) | keySeq(1) | MIC(4) */
  #define mApsAuxSize_c     (5+mMICSize_c) /* |SC(1) | frameCtr(4) | MIC(4) */
  #define mApsFragSize_c    2  /* Overhead for fragmentation */
  #define mApsGroupSize_c   1  /* Overhead for groups */

  uint8_t maxPayloadLen;
  uint8_t * aNwkAddr=NULL;
  uint8_t * aIeeeAddr=NULL;
  
#ifndef gHostApp_d  
  uint8_t aNwkAddrLocalCpy[2], aExtAddr[8];
#endif  

#if ((gStandardSecurity_d || gHighSecurity_d) && gApsLinkKeySecurity_d)
  zbKeyType_t pKeyType=0xFF;
  (void)pKeyType;
#endif  /* ((gStandardSecurity_d || gHighSecurity_d) && gApsLinkKeySecurity_d) */

  (void)aNwkAddr;
  (void)aIeeeAddr;
  

  /* assume 100 bytes payload max */
  maxPayloadLen = mPhyLen_c - (mMacHdrSize_c + mNwkHdrSize_c + mApsHdrSize_c);
 #ifndef gHostApp_d
  /* reduce for various options (if specified) */
  if(pDataReq)
  {
    /* both group and indirect (which may have groups) are smaller */
    if(pDataReq->dstAddrMode <= gZbAddrModeGroup_c)
      maxPayloadLen -= mApsGroupSize_c;   /* group overhead is 1 */
    else 
    {
      if (pDataReq->dstAddrMode == gZbAddrMode16Bit_c) 
      {
        aNwkAddr = pDataReq->dstAddr.aNwkAddr;
        aIeeeAddr = APS_GetIeeeAddress(aNwkAddr, aExtAddr);
      }
      else 
      {
        aIeeeAddr = pDataReq->dstAddr.aIeeeAddr;
        aNwkAddr = APS_GetNwkAddress(aIeeeAddr, aNwkAddrLocalCpy);
      }
    }
                
    if ((pDataReq->txOptions & gApsTxOptionFragmentationRequested_c) && (pDataReq->dstAddrMode != gZbAddrModeGroup_c))
    {
      maxPayloadLen -= mApsFragSize_c;   /* up to 2 bytes for fragmentation header */
    }

  /* If we are in ZigBee Pro and RnPlus is activated */
#if (gZigbeeProIncluded_d && gRnplusCapability_d)
#if (gRouterCapability_d || gCoordinatorCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
    if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
      /* If is a unicast we need to verify if there is a source route */
      if (pDataReq->dstAddrMode != gZbAddrModeGroup_c) 
      {
#if (gConcentratorFlag_d)
        if(NlmeGetRequest(gNwkIsConcentrator_c))
        {
          /*
            Verify if a source route exists for this destination. If no source route
            is found the frame cannot be sent using source routing.
          */
          if (aNwkAddr)
          {
            maxPayloadLen -= NwkGetSourceRouteSubframeSize(aNwkAddr);
          }
        }
#endif  /* (gConcentratorFlag_d) */
      }
#endif /* (gRouterCapability_d || gCoordinatorCapability_d || gComboDeviceCapability_d) */
#endif  /* (gZigbeeProIncluded_d && gRnplusCapability_d) */

#if gStandardSecurity_d || gHighSecurity_d
    /* if security is enabled for this packet, subtract the NWK AUX size */
    
    
    if(!(pDataReq->txOptions & gApsTxOptionNoSecurity_c))
    {
      if (NlmeGetRequest(gNwkSecureAllFrames_c))
      {
        maxPayloadLen -= mNwkAuxSize_c;
      }
#if gApsLinkKeySecurity_d
      if((pDataReq->txOptions & gApsTxOptionSecEnabled_c))
      {
        /*  If we are using the active network key, and the nwkSecureAllFrames attribute
            of the NIB is TRUE, the APS layer will not apply security */
        if (!((pDataReq->txOptions & gApsTxOptionUseNwkKey_c) && (NlmeGetRequest(gNwkSecureAllFrames_c))))
        {
          /* We need the Ieee address to verify if there is a link key for the destination device */
          if (aIeeeAddr)
          {
            pKeyType = ZDO_SecGetDeviceKeyType(aIeeeAddr);
            if ((pKeyType == gApplicationLinkKey_c) || (pKeyType == gTrustCenterLinkKey_c))
            {
              maxPayloadLen -= mApsAuxSize_c;
            }
          }
        }
      }
#endif  /* gApsLinkKeySecurity_d */
    }
#endif  /* gStandardSecurity_d || gHighSecurity_d */
  }
  else
#else
(void)pDataReq;  
#endif  /*end gHostApp_d*/
  {
  /* Unknown what the data request is, so choose a "safe" size */
#if gFragmentationCapability_d
    maxPayloadLen -= mApsFragSize_c;   /* worst case is fragmentation */
#endif

#if (gZigbeeProIncluded_d)
#if (gRouterCapability_d || gCoordinatorCapability_d || gComboDeviceCapability_d)
#if (gConcentratorFlag_d)
#if gComboDeviceCapability_d
    if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
    {
      if(NlmeGetRequest(gNwkIsConcentrator_c))
      {
        maxPayloadLen -= mNwkSrcRouteSize_c;
      }
    }
#endif /* (gConcentratorFlag_d) */
#endif /* (gRouterCapability_d || gCoordinatorCapability_d || gComboDeviceCapability_d) */
#endif /* (gZigbeeProIncluded_d) */
    
#if gStandardSecurity_d || gHighSecurity_d
    /* normal case for stack profile 0x01, residential security */
    maxPayloadLen -= mNwkAuxSize_c;
#if gApsLinkKeySecurity_d
    maxPayloadLen -= mApsAuxSize_c;
#endif  /* gApsLinkKeySecurity_d */
#endif  /* gStandardSecurity_d || gHighSecurity_d */
  }

  /* return maximum size of payload */
  return maxPayloadLen;
}

uint8_t GetMaxNsduSize(void)
{
  uint8_t maxPayloadLen;
  
  maxPayloadLen = mPhyLen_c - (mMacHdrSize_c + mNwkHdrSize_c);
#if gStandardSecurity_d || gHighSecurity_d
  maxPayloadLen -= mNwkAuxSize_c;
#endif  /* gStandardSecurity_d || gHighSecurity_d */
  
#if (gZigbeeProIncluded_d)
#if gRouterCapability_d || gCoordinatorCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
  {
    maxPayloadLen -= mNwkSrcRouteSize_c;
  }
#endif
#endif  /* (gZigbeeProIncluded_d) */
  return maxPayloadLen;
}

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
)
{
  uint8_t i;

  uint8_t aNetworkCount[gNumOfChannels_c];  /* On per each channel. */

  /* Initialize at the highest value to track the channel with less amount of networks. */
  uint8_t iMinNetworkCount = 0xFF;

  /*
    This will have values 3,2,1,0 as we traverse through for each Channel,
    8 Channels are bit mapped in 1 Byte. This helps to walk throught the
    4 byte channels list.
  */
  uint8_t iIndex = 0x03;
  uint8_t iChannelIndex = 0x00; 

  /*
    This is used to Right Shift and mask as we traverse each Channel,
    0x04 is for Channel 26
  */
  uint8_t bitMask = 0x04;

  uint8_t iListIndex = (gNumOfChannels_c -1);
  
  bool_t firstChannel = FALSE;

  /* Leave on Zero the unused channels. */
  BeeUtilZeroMemory(aNetworkCount, sizeof(aNetworkCount));

  /*
    Fills each channel position with the amount of networks existing on that particular
    channel.
  */
  for (i = 0; i < ZbBeeStackNwkGlobals(gMaxDiscoveryTableEntries); i++)
  {
    if (IsBroadcastAddress(ZbBeeStackNwkGlobals(gpDiscoveryTable[i].aNetworkAddress)))
    {
      continue;
    }

    /*
      Transform the channel number (11 to 26) into an array index (0 to 15).
      And keep track of the amount of networks per channel.
    */
    aNetworkCount[ZbBeeStackNwkGlobals(gpExtendedDiscoveryTable[i].logicalChannel) - 11]++;
  }

  for (i = 0; i < gNumOfChannels_c; i++)
  {
    /*
      pChannelList represents the Channels Specified by Upper layer (e.g Formation Request)
      Check if channel was scanned.
    */
    if ( pChannelList[ iIndex ] & bitMask )
    {
      if(firstChannel == FALSE)
      {
        iChannelIndex = iListIndex - i;
        firstChannel = TRUE;
      }
      
      if (aNetworkCount[iListIndex - i] <= iMinNetworkCount)
      {
        if(SelectPanIDFromTables(NlmeGetRequest(gNwkPanId_c), iListIndex - i + 11) == FALSE)
        {
          iMinNetworkCount = aNetworkCount[iListIndex - i];
          iChannelIndex = (iListIndex - i);
        }
      }
    }
    /* Shift the mask one bit to the right in order to process the next channel. */
    bitMask >>= 1;

    /* Check if 1 Byte (set of 8 channels ) is over */
    if( 0x00 == bitMask )
    {
      /* This is used to Right Shift and mask as we traverse each Channel,
      Reinitialise it to 0x80 for next channel  */
      bitMask = 0x80;
      iIndex--;
    }
  }

  /* Return the value of the selected channel (11 .. 26). */
  return (iChannelIndex + 11);
}

/******************************************************************************
* Only included if ZigBee Coordinator (ZC)
* Select logical channel to form network
******************************************************************************/
void SelectLogicalChannel
(
  const nwkMessage_t *pMsg, /* IN - Pointer to the ED Scan Confirm */
  uint8_t* pScanChannels,/* IN - List of Channels obtained after ED Scan */ 
  uint8_t* pSelectedLogicalChannel/* IN/OUT- To be updated after finding
                                             least number of Nwks */     
)
{
  uint8_t panDescListSize = pMsg->msgData.scanCnf.resultListSize;
  uint8_t cCounter = 0x00;   /* Counter used for Num. of Pan Descriptors */
  uint8_t iChannelIndex  = 0x00; /* Index used for Network Count */
  uint8_t logicalChannel = 0x00;    /* Denotes Logical Channel */
  uint8_t minNetworkCount = 0xFF;   /* Used to store the number of Nwks */
  uint8_t networkCount[ 16 ];  /* Denotes Num of Networks in each Channel */
  uint8_t iIndex = 0x00;   /* Used to Scan through Channels */  
  uint8_t bitMask = 0x08; /* This is used to Left Shift and mask as we traverse 
                           each Channel ,0x08 is for Channel 11 */
  panDescriptorBlock_t *block;
  
  /* No need to check for Memory fail because it is done by the calling layer */
  panDescriptor_t *pPanDescList;
  //block = pMsg->msgData.scanCnf.resList.pPanDescriptorBlocks;
  block = pMsg->msgData.scanCnf.resList.pPanDescriptorBlockList;
  
  //pPanDescList = block->descriptorList;
  pPanDescList = block->panDescriptorList;
  
  BeeUtilZeroMemory(networkCount, gNumOfChannels_c);/* Initialise to Zero */
  
  /* To find the Num of Networks specified in the Selected Channels */
   /*  Process the Active scan confirm. The NLME shall review
       the list of returned PAN descriptors and find the first
       channel with the lowest number of existing networks, 
       favoring a channel with no detected networks */ 
  for ( cCounter = 0x00; cCounter<panDescListSize; cCounter++) 
  {
    if (cCounter >= aScanResultsPerBlock)
    {        
      //pPanDescList = block->pNext->descriptorList;     
      pPanDescList = block->pNext->panDescriptorList;
      logicalChannel = pPanDescList[ cCounter - aScanResultsPerBlock ].logicalChannel;
    }
    else      
    {        
      logicalChannel = pPanDescList[ cCounter ].logicalChannel;
    }
    networkCount[ logicalChannel - gLogicalChannel11_c ]++;          
  }
  /* Shifting the bitMask bit towards left to Select Each of the Channels
     and update the Selected Logical Channel */
  /* 26 channels spread over 4 Bytes as Bit maps, Valid Channel Starts 
     from 11 to 26 
     Hence the iIndex will vary from 1 to 4 */ 
  for( iIndex = 1; iIndex < 4; iIndex++ )
  {
    while( bitMask > 0x00 ) /* Shifting will take it to 0 after few iterations */
    {
      if( pScanChannels[ iIndex ] & ( bitMask )) {    
        if ( networkCount[ iChannelIndex ] < minNetworkCount ) 
        {
          /* Make the Min Nwk Count as the one with Least Nwk Found */
          minNetworkCount = networkCount[ iChannelIndex ];
          *pSelectedLogicalChannel = iChannelIndex  + gLogicalChannel11_c;
          if ( 0x00 == minNetworkCount ) {
            break;
          }
        }/* If loop Ends */      
      } /*   if( pScanChannels[ iIndex ] & ( bitMask )) ENDS */
      iChannelIndex ++;   /* This is done in order to get the correct Channel
      Offset */
      bitMask <<= 1;
    }
    /* Check if 1 Byte (set of 8 channels ) is over */
    if ( 0x00 == bitMask ) {
      bitMask = 0x01;      /* Reinitialise for the Next Set of Channels */
    }
  }
}

/****************************************************************************/
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
)
{
  aPanId[0] = GetRandomRange(0x00, gaPIDUpperLimit[1]);
  aPanId[1] = GetRandomRange(0x00, gaPIDUpperLimit[0]);
}

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
)
{
  bool_t  useRandom;

  bool_t  panIdConflict = FALSE;

  uint8_t i;

  /* Is a the given Pan Id a valid one? or we need to generate one? */
  useRandom = (IsEqual2BytesInt(aPanId, 0xFFFF))? TRUE : FALSE;

  if (useRandom)
  {
    getRandomPanId(aPanId);
  }

  while (!panIdConflict)
  {
    /*  */
    for (i = 0; i < ZbBeeStackNwkGlobals(gMaxDiscoveryTableEntries); i++)
    {
      /* Skip the invalid entries. */
      if (IsBroadcastAddress(ZbBeeStackNwkGlobals(gpDiscoveryTable[i].aNetworkAddress)))
      {
        continue;
      }

      if (IsEqual2Bytes(aPanId, ZbBeeStackNwkGlobals(gpExtendedDiscoveryTable[i].aPanId)) &&
          channel == ZbBeeStackNwkGlobals(gpExtendedDiscoveryTable[i].logicalChannel))
      {
        panIdConflict = TRUE;
        break; /* Break only the for loop. */
      }
    }  /* End of For loop */

    /* Check if the PanId conflict was found and if the Pan Id is random. */
    if (panIdConflict && useRandom)
    {
      /*
        Ok, we got a random panId with conflict well..lets just pick another one a
        keep moving.
      */
      panIdConflict = FALSE;
      getRandomPanId(aPanId);
    }
    else
    {
      /* Ok, no conflicts or conflicts but no ra */
      break;
    }
  }  /* End of While loop */

  return panIdConflict;
}

/****************************************************************************
* Only on a ZigBee Coordinator (ZC)
* Select a random PAN ID if one not given by user.
*
* Returns TRUE if there is a PAN ID conflict.
*****************************************************************************/
bool_t SelectPanId
(
  const nwkMessage_t *pMsg,       /* IN -Pointer to Active Scan Confirm Paramters */
  uint8_t selectedLogicalChannel, /* IN - Log.Channel for determining PanId */
  uint8_t* pPanId                  /* IN/OUT - Pointer to the PanId */
)
{
  /* Get the Pan Descriptor List Size */
  uint8_t panDescListSize   = pMsg->msgData.scanCnf.resultListSize;
  uint8_t iIndex;
  uint8_t panIdConflictDetected;
  

  /* Point to the Pan Descriptors obtained */
  panDescriptorBlock_t *block;
  
  /* No need to check for Memory fail because it is done by the calling layer */
  panDescriptor_t *pPanDesc;
  //block = pMsg->msgData.scanCnf.resList.pPanDescriptorBlocks;
  block = pMsg->msgData.scanCnf.resList.pPanDescriptorBlockList;


  /* if there is another network in this channel with our extendedpanid there is a conflict */
  if(CheckForExtendedPanIdConflict(selectedLogicalChannel))
    return TRUE;

  /* select random pan ID */
  if(gUseRandomPanId) {
    getRandomPanId(pPanId);
  }

  /* PAN ID given by user */
  else {
    Copy2Bytes(pPanId,NlmeGetRequest(gNwkPanId_c));
  }

  /* Search for a PanId that does not conflict with the networks 
     existing in the 'selectedLogicalChannel' */  
  do {

    panIdConflictDetected = 0; 

    /* Scan through the Pan Desc. Found from the scan */
    for(iIndex = 0; iIndex<panDescListSize; iIndex++)
    {
    if (iIndex >= aScanResultsPerBlock)
    {        
      //pPanDesc = &block->pNext->descriptorList[ iIndex - aScanResultsPerBlock ];
      pPanDesc = &block->pNext->panDescriptorList[ iIndex - aScanResultsPerBlock ];     
    }
    else      
    {        
      //pPanDesc = &block->descriptorList[ iIndex ];
      pPanDesc = &block->panDescriptorList[ iIndex ];     
    }

      /* Check if the Channels are same */
      if( selectedLogicalChannel == pPanDesc->logicalChannel ) { 

        /* Check if the Pan ID is same */
        //if( IsEqual2Bytes(pPanId, pPanDesc->coordPanId )) {
        if( IsEqual2Bytes(pPanId, (void*)&pPanDesc->coordPanId )) {
          ++panIdConflictDetected;
          break;
        }
      }
    } /* for loop Ends */

    /* a PAN ID was provided and there is a conflict */
    if(!gUseRandomPanId) {
#if gSamePanIdOk_c
      panIdConflictDetected = FALSE;
#endif
      return panIdConflictDetected;
    }

    /* pick another PAN ID */
    getRandomPanId(pPanId);

  } while (panIdConflictDetected);
  
  return panIdConflictDetected;
}

/**********************************************************************************
*
* Cheks if in a particular channel exists another network using the same Extended
* Pan Id we are tryig to use to form our network.
* Returns True if there is a confilct and False if there is none
*
**********************************************************************************/
bool_t CheckForExtendedPanIdConflict
(
  uint8_t logicalChannel /* IN - Log.Channel for determining PanId */
)
{
  /* Index to traverse through the neighbor table*/
  uint8_t iIndex = 0;

  for(iIndex = 0; iIndex < ZbBeeStackNwkGlobals(gMaxDiscoveryTableEntries; iIndex++)) {

      if(IsEqual8Bytes( NlmeGetRequest(gNwkExtendedPanId_c),
                        ZbBeeStackNwkGlobals(gpExtendedDiscoveryTable[iIndex].aExtendedPanId )) &&
                        logicalChannel == ZbBeeStackNwkGlobals(gpExtendedDiscoveryTable[iIndex].logicalChannel ))
        return TRUE;
  }
  return FALSE;
}

/*
  Calculates the Link cost based on the following Table
  ===================
  | LQI    |LinkCost|
  ===================
  |>75     |    1   |
  -------------------
  |50-75   |    3   |
  -------------------
  |<50     |    7   |
  -------------------                                                    

  053474r20 ZigBee Pro spec
  Link cost calculation explanation 3.6.3.1 Routing Cost
*/
zbLinkCost_t LinkCostCalculator(uint8_t iLqi)
{
  if(iLqi > 75)
  {
    return 1;
  }
#ifdef PROCESSOR_KINETIS
  else if(iLqi > 30)
#else
  else if(iLqi > 50)
#endif
  {
    return 3;
  }
  /*
    The change is need to report onthe link status an expired parent, when a parent gets
    expired, we set the LQI to zero and the outgoing cost to zero, so inorder to report a
    link cost of zero we must set it here.

    See 075035r5, test 13.5 and test 13.4.
  */
  if (!iLqi)
  {
    return 0;
  }

  return 7;
}


#if gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d
/**********************************************************************************
* Valid for ZR, ZED or Combo devices
*
* Look for a router (ZC or ZR) to join. Return the index in the discover table for
* the potential parent. Return 0XFF if there were no match found. The criteria used 
* is as follows:
*
* 1. First check the extended pan id in the nib, if it's different than zero the 
*    algoritm will try to find a suitable router based on that extended pan id.
* 2. If extended pan id is equal to zero check the nwk pan id in the nib, if it's
*    diffferent than 0xFFFF the algoritm will try to find a suitable router based
*    on the nwk pan id.
* 3. If nwk pan id is equal to 0xFFFF (actually, anything 0x4000-0xffff) then the 
*    algoritm will try to find a suitable router withot paying attention to either 
*    extended or nwk pan id.
*
* Note: also takes into a count the permit joining flag, link cost, maxDepth and 
* router or end device capacity.
*
**********************************************************************************/
index_t SearchForSuitableParentToJoin ( void )
{
  /* Index to traverse through the neighbor table*/
  uint8_t iIndex = 0;

  /* To check whether the link cost in valid range */
  uint8_t linkCost = 0;

  /* The MaxDepth that can be possible is 0xff*/
  uint8_t minDepth=0xff;
  
  /* Index in neighbor table once the sutible coordinator is selected */
  uint8_t minimumDepthIndex =0;
  
  /* by default, search will be based on Extended Pan Id*/
  uint8_t lookFor;
  
  /* flag used to indicated wheter or not a particular coord is suitable*/
  bool_t panIdMatches=FALSE;

  /* value of the Highest NwkUpdateId to join to */
  uint8_t highestNwkUpdateId = NlmeGetRequest(gNwkUpdateId_c);

  /* flag used to indicate whether or not a coord has been found*/
  bool_t coordinatorFound = FALSE;

  /*Ptr to a Discovery Table entry containing Vendor specific ,Mandatory and
  Optional Attributes*/
  neighborTable_t *pDTE;

  /* Pointer to an Extended Discovery Table Entry */
  extendedNeighborTable_t *pEDTE;

  /* The right extended PAN ID to use. */
  uint8_t *pExtendedPanId = NULL;

  /*
    If the device is a combo device acting like coordinator should not proceed
    processing further.
  */
#if gComboDeviceCapability_d
  if ((NlmeGetRequest(gDevType_c) == gCoordinator_c))
  {
    return gNwkEntryNotFound_c;
  }
#endif


  /* if discovery tables are not even allocated return gNwkEntryNotFound_c(0xFF) */
  if(!ZbBeeStackNwkGlobals(gpDiscoveryTable))
    return gNwkEntryNotFound_c;

  /* By default use Network Extended PAN Id. */
  lookFor = mExtendedPanId_c;

  /* Se if the stack has a valid Nwk Extended PAN Id. If so, use it to match the desired Network. */
  if (!Cmp8BytesToZero(NlmeGetRequest(gNwkExtendedPanId_c)))
  {
    pExtendedPanId = NlmeGetRequest(gNwkExtendedPanId_c);
  }
  else if (!Cmp8BytesToZero(ApsmeGetRequest(gApsUseExtendedPANID_c)))
  {
    /*
      Ok, We don't have a Nwk Extended Pan Id, we must then have an APS extended Pan Id,
      lets try to use it (we may be doing rejoin, See section 2.5.5.5.6.2 of 053474r20 ZigBee Pro spec.).
    */
    pExtendedPanId = ApsmeGetRequest(gApsUseExtendedPANID_c);
  }
  /* if it's not a valid extended Pan Id, check if the nwk Pan Id is a valid one */
  else if (TwoBytesToUint16(NlmeGetRequest(gNwkPanId_c)) == 0xffff)
  {
    /* if the nwk Pan Id is also invalid, search will be based on any pan id. */
    lookFor = mAnyPanId_c;
  }
  else
  {
    /* If nwk Pan Id is a valid one, search will be base on it */
    lookFor = mNwkPanId_c;
  }

  /* Concider all of the cases found.! */
  for (iIndex = 0; iIndex < ZbBeeStackNwkGlobals(gMaxDiscoveryTableEntries); iIndex++)
  {
    panIdMatches=FALSE; // Reset PanId match flag
    pDTE = &( ZbBeeStackNwkGlobals(gpDiscoveryTable [ iIndex ] ));
    pEDTE = &( ZbBeeStackNwkGlobals(gpExtendedDiscoveryTable [ iIndex ] ));

    linkCost = LinkCostCalculator ( pDTE->lqi );

    if (lookFor == mExtendedPanId_c)
    {
      /* Compare the extended PAN Id's fond to the selected one (Nwk or APS). */
      if( IsEqual8Bytes( pEDTE->aExtendedPanId, pExtendedPanId))
        panIdMatches = TRUE;
    }
    else if (lookFor == mNwkPanId_c)
    {
      /* We don't have extended Pan Id, lets try the configured Short Pan Id. */
      if(IsEqual2Bytes(pEDTE->aPanId,NlmeGetRequest(gNwkPanId_c)))
        panIdMatches = TRUE;
    }
    else if (lookFor == mAnyPanId_c)
    {
      /* We dont have an Extended Pan Id or a short Pan Id, lets try Any short Pan Id. */
      panIdMatches =TRUE;
    }

    if ((panIdMatches) && ( NWK_DeviceIsPotentialParent(pEDTE->joiningFeatures) ) && ( linkCost <= maxLinkCost ))
    {
      if( NWK_DevAllowsAssociation( pEDTE->joiningFeatures) || ZbZdoPrivateData(gZDOJoinMode) == gNwkRejoin_c || ZbZdoPrivateData(gZDOJoinMode) == gSilentNwkRejoin_c )
      {
        if ((((ZbBeeStackNwkGlobals(gRejoinConfigParams.paramFlags) & gRejoinConfigParamsMask_EnableAddrFiltering) != gRejoinConfigParamsMask_EnableAddrFiltering)
             || !IsEqual2Bytes(pDTE->aNetworkAddress, ZbBeeStackNwkGlobals(gRejoinConfigParams.filterDeviceNwkAddress))) && 
            (((ZbBeeStackNwkGlobals(gRejoinConfigParams.paramFlags) & gRejoinConfigParamsMask_EnableLqiFiltering) != gRejoinConfigParamsMask_EnableLqiFiltering)
             || pDTE->lqi >= ZbBeeStackNwkGlobals(gRejoinConfigParams.filterDeviceLqi)))           
        {
          if ((pEDTE->nwkUpdateId >= highestNwkUpdateId) || ((highestNwkUpdateId >= 0xFD) && ((uint8_t)(highestNwkUpdateId + 0x05) >= pEDTE->nwkUpdateId)))
          {
            if((pEDTE->nwkUpdateId > highestNwkUpdateId) || ((highestNwkUpdateId >= 0xFD) && ((uint8_t)(highestNwkUpdateId + 0x05) >= pEDTE->nwkUpdateId)))
            {
              /*
                The previous potential parent is no longer valid since a potential
                parent with a higher Nwk Update Id was found.
              */
              highestNwkUpdateId = pEDTE->nwkUpdateId;
              minimumDepthIndex = 0;
              minDepth = 0xFF;
              coordinatorFound = FALSE;
              panIdMatches = FALSE;  /* reset flag */
            }
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
            if ( NlmeGetRequest( gDevType_c ) == gEndDevice_c )
#endif
            {
              if ((ZbBeeStackNwkGlobals(gRejoinConfigParams.paramFlags) & gRejoinConfigParamsMask_EnablePreviousParent) == gRejoinConfigParamsMask_EnablePreviousParent)
              {
                /* At this point the link costis below 3 which makes it acceptable, lets see ti it wasnt our parent before. */
                if (IsEqual2Bytes(pDTE->aNetworkAddress, NlmeGetRequest(gParentShortAddress_c)))
                {
                  //gSuitableParentFound = TRUE;
                  
                  /* Ok, it was our parent before and is between the acceptable limits, use this guy. */
                  return iIndex;
                }
              }
            }
#endif
            /* to choose a potential parent at the minimum depth. */
            if( pEDTE->depth < minDepth )
            {
              minimumDepthIndex = iIndex;
              minDepth = pEDTE->depth;
              coordinatorFound = TRUE;
              panIdMatches = FALSE;  /* reset flag */
            }
          }
        }
      }
    }
  } /* for (iIndex = 0;... */

  if( coordinatorFound )
  {
    //gSuitableParentFound = TRUE;
    
    return minimumDepthIndex;
  }

  /* Giveup, theres no one out there.! */
  return gNwkEntryNotFound_c;
}
#endif


/****************************************************************************
* Look for an empty entry in Discovery Tables
*****************************************************************************/ 
index_t GetFreeEntryInDiscoveryTable
( 
  zbNwkAddr_t aNwkAddr,
  zbPanId_t aPanId, 
  uint8_t channel,
  uint8_t stackProfile,
  bool_t reuse     /* indicate that it will be consider as empty any device at capacity */
)
{
  index_t iIndex=0;

  /* Pointer to the DiscoveryTable entry containing Vendor specific ,Mandatory and
  Optional Attributes*/
  neighborTable_t *pDTE;

  /* Pointer to the Extended Discovery Table Entry containing information needed
  to form or join a network */
  extendedNeighborTable_t *pEDTE;

  /* look for an available entry for the beacon notify sender */
  for(iIndex=0; iIndex < ZbBeeStackNwkGlobals(gMaxDiscoveryTableEntries); iIndex++) {

     pDTE = &( ZbBeeStackNwkGlobals(gpDiscoveryTable [ iIndex ] ));
    pEDTE = &( ZbBeeStackNwkGlobals(gpExtendedDiscoveryTable [ iIndex ] ));

    if(reuse)
      if( !NWK_DeviceIsPotentialParent(pEDTE->joiningFeatures) ||
          !NWK_DevAllowsAssociation( pEDTE->joiningFeatures) )
        return iIndex;

    /* if an empty entry has been found return index. */
    if( IsEqual2BytesInt( pDTE->aNetworkAddress, 0xFFFF ) ||
      /* or if we got already a becacon notify from the sender reuse the entry */
      ( IsEqual2Bytes( pEDTE->aPanId, aPanId) &&
        IsEqual2Bytes(pDTE->aNetworkAddress, aNwkAddr ) &&
        pEDTE->logicalChannel == channel &&
        ((pEDTE->joiningFeatures & 0xF0)>>4) == stackProfile ) ) {
      return iIndex;
    }
  }
  return mNotFound;
}

/******************************************************************************
* This function gets the pointer to the nwkBeaconNotifyInd_t structure.
* The NeighborTable is updated with the information obtained from the 
* panDescriptor of BeaconIndication. 
*
* Interface assumptions: 
*   1)rxonwhenidle == TRUE for devices sending the beacon for which beacon 
*   indication is received.
*   2)BeaconIndication will be obtained only if the Beacon contains the  
*     BeaconPayload. 
******************************************************************************/
void ParseBeaconNotifyIndication
  (
  nwkBeaconNotifyInd_t *pBeaconIndication  /*IN: Ptr to BeaconIndication sent by Mac */
  )
{
  index_t iIndex=0;

  /* Pointer to the DiscoveryTable entry containing Vendor specific ,Mandatory and
  Optional Attributes*/
  neighborTable_t *pDTE;

  /* Pointer to the Extended Discovery Table Entry containing information needed
  to form or join a network */
  extendedNeighborTable_t *pEDTE;

  /* Pointer to the pan descriptor contained in the beacon indication */
  panDescriptor_t *pPanDescriptor = pBeaconIndication->pPanDescriptor;

  /* Pointer to the beacon payload contained in the beacon indication */
  beaconPayloadData_t *pPayload = ( beaconPayloadData_t * )pBeaconIndication->pSdu;

 /* if packet is null */
  if(!pPayload || !pPanDescriptor)
    return;

  /* Check if beacon indication has a compatible protocol id and stack profile, if not ignore the beacon */
  if( !IsStackProfileCompatible( pPayload ) )
    return;

  /* look for an empty slot */
  iIndex = GetFreeEntryInDiscoveryTable(//pPanDescriptor->coordAddress,
                                        (uint8_t*)&pPanDescriptor->coordAddress,
										//pPanDescriptor->coordPanId,
                                        (uint8_t*)&pPanDescriptor->coordPanId,
                                        pPanDescriptor->logicalChannel,
                                        pPayload->Info & 0x0F, //stack profile
                                        FALSE);

  /* if there is no empty slots then looks for an entry with less priority that can be replace */
  if(iIndex ==  mNotFound) 
  {
    iIndex = GetFreeEntryInDiscoveryTable(//pPanDescriptor->coordAddress,
	                                      (uint8_t*)&pPanDescriptor->coordAddress,
                                          //pPanDescriptor->coordPanId,
										  (uint8_t*)&pPanDescriptor->coordPanId,
                                          pPanDescriptor->logicalChannel,
                                          pPayload->Info & 0x0F, //stack profile
                                          TRUE);
  }

  /* if there is no room for another entry then discard the beacon */
  if(iIndex ==  mNotFound)
    return;
  
  /* if an empty entry has been found add new data to that entry, else ignore the beacon.
  to check if an entry is empty check extended address, this never will be zero if entry is being used */

  pDTE = &( ZbBeeStackNwkGlobals(gpDiscoveryTable [ iIndex ] ));
  pEDTE = &( ZbBeeStackNwkGlobals(gpExtendedDiscoveryTable [ iIndex ] ));


  /* initialize transmit failures counter to zero */
  pDTE->transmitFailure=0;

  /* initialize device property field*/
  pDTE->deviceProperty.allBits = 0;

  /*update the Discovery Table Entry with the Nwk Address of the Router (aka coord in MAC speak) */
  //Copy2Bytes(pDTE->aNetworkAddress, pPanDescriptor->coordAddress);
  Copy2Bytes(pDTE->aNetworkAddress, (uint8_t*)&pPanDescriptor->coordAddress);

  /*Update the Extended Discovery Table Entry with the PanId of the Coordinator */
  //Copy2Bytes(pEDTE->aPanId,pPanDescriptor->coordPanId);
  Copy2Bytes(pEDTE->aPanId,(void*)&pPanDescriptor->coordPanId);

  /*Update the Extended Discovery Table Entry with the Extended PanId contained in payload */
  Copy8Bytes(pEDTE->aExtendedPanId, pPayload->aNwkExtPANId) ;

  /*Update the Extended Discovery Table Entry with the logical channel*/
  pEDTE->logicalChannel = pPanDescriptor->logicalChannel;

#if ( gBeaconSupportCapability_d )
  /* Update the txOffset of the potential parent */
  FLib_MemCpy( pDTE->aBeaconTransmissionTimeOffset, pPayload->aTxOffset, 3 );

  /* Update the SuperFrame Specifications */
  pEDTE->beaconOrder = ( pPanDescriptor->superFrameSpec[ 0 ] ) & gSuperFrameSpecLsbBO_c;

#endif /*( gBeaconSupportCapability_d )*/

  /* Beacons can be received only from a Zigbee Coordinator or from a Zigbee Router
  and hence based on the Pan Coordinator Bit set the deviceType to Coordinator or Router */
  //pDTE->deviceProperty.bits.deviceType = ( ( ( pPanDescriptor->superFrameSpec[ 1 ] ) & gSuperFrameSpecMsbPanCoord_c)? gCoordinator_c : gRouter_c );
  pDTE->deviceProperty.bits.deviceType = ((pPanDescriptor->superframeSpec.panCoordinator)? gCoordinator_c : gRouter_c );
  /* Update Neighbor Table entry with the LinkQuality obtained from Mac */
  pDTE->lqi = pPanDescriptor->linkQuality;

  /* Assumption: There is no way to receive the rxonwhenidle while performing
  discovery and  the assumption is that any device sending the beacon will
  have its receiver turned on */
  pDTE->deviceProperty.bits.rxOnWhenIdle = gRxOn_c;

  /* If the Info about the device is already present in the neighbor table then do not
  update the relationship info else  set it to none */

  if( pDTE->deviceProperty.bits.relationship != gNeighborIsParent_c && pDTE->deviceProperty.bits.relationship != gNeighborIsChild_c )
  {
    pDTE->deviceProperty.bits.relationship = gNone_c;
  }

  pEDTE->depth = ( ( pPayload->DeviceInfo & gDeviceDepthMask_c ) >> 3 ) ;

  pEDTE->nwkUpdateId = pPayload->nwkUpdateId;

  /* Update the Permit Join subfield for this entry */
  //#warning join nt permit - check how panDescriptor block is free
  //pEDTE->joiningFeatures = 0 | ( ( ( pPanDescriptor->superFrameSpec[1] ) & gSuperFrameSpecMsbAssocPermit_c ) >> 7  );
  pEDTE->joiningFeatures = 0 | pPanDescriptor->superframeSpec.associationPermit;
  /* If End Device capacity or Router capacity (respectively) is True then the device is a
  potential parent, so update potential parent subfield */
  if( ( IsLocalDeviceTypeARouter() && ( pPayload->DeviceInfo & gRouterCapacity_c ) ) ||
      ( !IsLocalDeviceTypeARouter() && ( pPayload->DeviceInfo & gEndDeviceCapacity_c ) ) ) {
    pEDTE->joiningFeatures |= gMaskPotentialParent_c;
  }
  else
  {
    pEDTE->joiningFeatures &=  ~gMaskPotentialParent_c;
  }

  /* Update stack profile subfield for this entry */
  pEDTE->joiningFeatures |= ( ( pPayload->Info & 0x0F ) << 4 );

  return;
}


#if gNwkPanIdConflict_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
/*************************************************************************************************/
void ParseBeaconNotifyIndicationAndDetectPanIdConflict
(
  nwkBeaconNotifyInd_t *pBeaconIndication  /*IN: Ptr to BeaconIndication sent by Mac */
)
{
  /* Pointer to the pan descriptor contained in the beacon indication */
  panDescriptor_t *pPanDescriptor = pBeaconIndication->pPanDescriptor;

  /* Pointer to the beacon payload contained in the beacon indication */
  beaconPayloadData_t *pPayload = ( beaconPayloadData_t * )pBeaconIndication->pSdu;

  /* if packet is null */
  if(!pPayload || !pPanDescriptor)
    return;
 
  /* Check if beacon indication has a compatible protocol id and stack profile, if not ignore the beacon */
  if( !IsStackProfileCompatible( pPayload ) )
    return;

#if gCorruptBeaconDetection_c
  /* check if reserved bits are zero*/
  if(pPayload->DeviceInfo & gDeviceInfoReservedBits_c)
    return;
  
  /* verify tx Offset is all Fs*/
  if((pPayload->aTxOffset[0] != 0xFF) || (pPayload->aTxOffset[1] != 0xFF) || (pPayload->aTxOffset[2] != 0xFF))
    return;
#endif
  
/* This Pan Identifier detection isn't realized when the network is forming. This detection is realized 
   during the life of the device on the network. In BeeStack 2006 already exists an implementation to 
   detect a PanId conflict when the network is forming.
   */ 

  if( !Cmp8BytesToZero(NlmeGetRequest(gNwkExtendedPanId_c)) && 
      //( IsEqual2Bytes( pPanDescriptor->coordPanId ,NlmeGetRequest( gNwkPanId_c ) ) &&
      ( IsEqual2Bytes((void*)&pPanDescriptor->coordPanId ,NlmeGetRequest( gNwkPanId_c ) ) &&
      !IsEqual8Bytes( pPayload->aNwkExtPANId, NlmeGetRequest(gNwkExtendedPanId_c) ) ) ){

    /* If it is detected then the device has to create a PandId list but it first has to do a 
       MLME-SCAN.request of type scan ACTIVE. To can to do this scan the network already is formed and the
       device is ready and running on the network.

       gNwkPanIdConflictDetected indicate to the Network Discovery state 
       that it has to create a network report information pan id list 
    */
    if (ZDO_IsRunningState())
      ZbBeeStackNwkGlobals(gNwkPanIdConflictDetected) = TRUE;
  }
}
#endif
#endif
/******************************************************************************
* This function is called by the network layer when a ZeD device increments
* its parent transmit failure counter.
* The function initiates a network rejoin if the transmit failure is met.
* 
* NOTE! only the transmit failure limit (gMaxDataTransmitFailure_c) should
* be changed, or alternativly the ZDO start command can be removed if a rejoin
* is not wanted.
******************************************************************************/
bool_t ZeDTransmitFailureCounterCheck(uint16_t FailureCounter)
{
  /* Polling devices should base their failures on the poll confirm. */
  if (!IsLocalDeviceReceiverOnWhenIdle())
    return FALSE;

  if (FailureCounter >= NlmeGetRequest(gNwkLinkRetryThreshold_c))
  {
    /* Commands the rejoin. */
#if gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d
    ZDO_ProcessDeviceNwkRejoin();
#endif
    return TRUE; /* counter limit reached return true */
  }

  return FALSE; /* counter limit not reached return false*/
}

/*-------------------- ScanDurationToMilliseconds ----------------------
Calculate the length
of time to spend scanning each
channel. The time spent scanning
each channel is (aBaseSuperframeDuration * (2^n + 1)) symbols, where n is the value of
the ScanDuration parameter(0x00-0x05)

IN: ScanDuration value between 0x00..0x05.
OUT: milliseconds corresponding for ScanDuration
*/
uint32_t ScanDurationToMilliseconds
(
  uint8_t scanDuration
)
{
/* constValue is used to calc. the time in microseconds...
   constValue = (aBaseSuperframeDuration(16*60)) * (time for a symbol(16microsec))=15360 */
#define constValue (16*60*16)

/*Verify if wrong parameter*/
if (scanDuration > 0x05)
    return 0;

/* (constValue * (2^n + 1))\1000 miliseconds */
return (  ( ((uint32_t)constValue * (uint32_t)((0x01<<scanDuration)+1)) )/1000 );
}


/*****************************************************************************************
                     SECURITY SPECIFIC AND EXPOSED APIs
*****************************************************************************************/
#if gStandardSecurity_d ||gHighSecurity_d
#if gTrustCenter_d || gComboDeviceCapability_d
void AppAuthenticateDevice
(
  zbIeeeAddr_t  aIeeeAddress
)
{
  bool_t DeviceAllow = TRUE;
  uint8_t i;
  

  for (i =0; i < ZbStackTablesSizes(gExclusionMax); i++)
  {
    if (IsEqual8Bytes(aIeeeAddress, ZbBeeStackGlobalsParams(gaExclusionTable[i])))
    {
      DeviceAllow = FALSE;
      break;
    }
  }

#ifdef SmartEnergyApplication_d 
#if gZclSeUseInstallCodes_d
   /* 5.4.2.2 If the trust center determines that the device is not authorized 
    to be on the network, it shall send an APS Remove Device command to the parent 
    of the rejoining device, with the target address of the rejoining device's IEEE address. */
  if (ZCL_FindIeeeInRegTable(aIeeeAddress) == RegTable_InvalidIndex_c)
  {
    DeviceAllow = FALSE;
  }
#endif  
#endif  
  
  ZDO_SendAuthenticationToTrustCenter(DeviceAllow);  
}
#endif
#endif

/*
  given an Ieee Address this function will copmpare its OUI bytes against the aCompanyId, 
  which is the permitted group of addressesses in this network. it will return true if 
  the given address is valid otherwise it will return false.
*/
bool_t ValidateIeeeAddress( zbIeeeAddr_t aIeeeAddress ) 
{
  static const zbIeeeAddr_t aFilterMask = { gIeeeFilterMask_c };
  static const zbIeeeAddr_t aFilterValue = { gIeeeFilterValue_c };
  index_t i;

  /* no filter, always is valid IEEE address */
  if(Cmp8BytesToZero((uint8_t *)aFilterMask))
    return TRUE;

  /* check each byte in filter */
  for(i=0; i<sizeof(zbIeeeAddr_t); ++i)
  {
    if((aIeeeAddress[i] & aFilterMask[i]) != (aFilterValue[i] & aFilterMask[i]))
      return FALSE;
  }
  return TRUE;
}

/*****************************************************************************************
                     PERMISSIONS CONFIGURATION TABLE
*****************************************************************************************/
#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)

extern uint8_t * APS_GetIeeeAddress
(
  uint8_t * pNwkAddr,
  uint8_t   aWereToCpyExtAddr[]
);

extern zbKeyType_t ZDO_SecGetDeviceKeyType
(
  zbIeeeAddr_t aDeviceAddress
);

extern addrMapIndex_t APS_FindIeeeInAddressMap
(
  zbIeeeAddr_t aExtAddr
);

extern addrMapIndex_t APS_AddIeeeToAddressMap
(
  zbIeeeAddr_t aExtAddr
);

/**********************************************************************************
* Verify if the requesting device have authorization to carry out the incoming command, where
* aSrcAddr is the address of the requesting device, and permissionsCategory is the bit mask
* representing the incoming command.
*
* Return value:
*   Authorization flag, TRUE if the command was authorized, and FALSE otherwise.
*
**********************************************************************************/
bool_t CommandHasPermission
(
  zbNwkAddr_t aSrcAddr,
  permissionsFlags_t  permissionsCategory
)
{
  uint8_t i;
  uint8_t *pIeeeSrcAddr, aExtAddr[8];
  zbKeyType_t pKeyType=0xFF;
  
  /* If Permissions Table is in its default state authorize always the command */
  if (!ZbBeeStackApsGlobals(gPermissionsTableCount))
    return TRUE;

  /* Obtain the Ieee address from the neighborhood table or the address map */
  pIeeeSrcAddr = APS_GetIeeeAddress(aSrcAddr, aExtAddr);
  if (!pIeeeSrcAddr)
  {
    /* We dont know anything sbout this device, fail to authorize. */
    return FALSE;
  }

  /* We need to know the key type being in, use */
  pKeyType = ZDO_SecGetDeviceKeyType(pIeeeSrcAddr);

  /* If the command is secured with the Trust Center Link Key then authorize it without checking */
  if ((pKeyType == gTrustCenterLinkKey_c) && IsEqual8Bytes(pIeeeSrcAddr, ApsmeGetRequest(gApsTrustCenterAddress_c)))
  {
    /* The packet was secured with TC link key. */
    return TRUE;
  }

  /* Look if there is an entry in the table authorizing the device to perform the command */
  for (i = 0; i < ZbStackTablesSizes(gApsMaxEntriesForPermissionsTable); i++)
  {
    if (!IsEqual8Bytes(pIeeeSrcAddr, ZbBeeStackApsGlobals(gaPermissionsTable[i].authorizedDevice)))
    {
      /* Skip all the entries with a different IEEE address. */
      continue;
    }

    /* Verify if a link key is required and if the device has it */
    if ((gLinkKeyRequired_c & ZbBeeStackApsGlobals(gaPermissionsTable[i].permissionsFlags)) && (pKeyType != gApplicationLinkKey_c))
    {
      return FALSE;
    }

    /* Verify if device has permission */
    if (permissionsCategory & ZbBeeStackApsGlobals(gaPermissionsTable[i].permissionsFlags))
    {
      return TRUE;
    }

    /* Device found in the permissions table but is not authorized. */
    break;
  }

  return FALSE;
}

/**********************************************************************************
* Add a device to the Permissions Configuration Table, where aDevAddr is the IEEE
* device address, and permissionCategory is the bit mask representing the device
* permissions.
* Adding a device in this table include to adding it also to the address map.
* 
* Return values:
*   gZbSuccess_c
*   gZdoTableFull_c
*
**********************************************************************************/
uint8_t  AddDeviceToPermissionsTable
(
  zbIeeeAddr_t aDevAddr,
  permissionsFlags_t  permissionsCategory
)
{
  uint8_t i;
  int8_t iAvailable = -1;
  
  /* Look if the device is already in the table  */
  for (i = 0; i < ZbStackTablesSizes(gApsMaxEntriesForPermissionsTable); i++)
  {
    if (IsEqual8Bytes(&aDevAddr[0], &ZbBeeStackApsGlobals(gaPermissionsTable[i].authorizedDevice[0])))
    {
      /* Save the entry index to update */
      iAvailable = i;
      break;
    }
    /* Save the free entry index */
    if (!(ZbBeeStackApsGlobals(gaPermissionsTable[i].permissionsFlags & gPermissionsEntryActive)))
      iAvailable = i;
  }
  if (iAvailable > -1)
  {    
    /* Copy the device IEEE address */
    FLib_MemCpy(&ZbBeeStackApsGlobals(gaPermissionsTable[iAvailable].authorizedDevice[0]), aDevAddr, sizeof(zbIeeeAddr_t));
    /* Set the device permissions */
    ZbBeeStackApsGlobals(gaPermissionsTable[iAvailable].permissionsFlags) = (permissionsCategory | gPermissionsEntryActive);
    ZbBeeStackApsGlobals(gPermissionsTableCount)++;
    return gZbSuccess_c;
  }
  return gZdoTableFull_c;
}

/**********************************************************************************
* Remove a device from the Permissions Configuration Table. This function only removes the device
* from the Permissions Configuration Table, not from the Address Map.
* 
* Return values:
*   gZbSuccess_c
*   gZdoNoEntry_c
*
**********************************************************************************/
uint8_t RemoveDeviceFromPermissionsTable
(
  zbIeeeAddr_t  aDevAddr
)
{
  uint8_t i;
  
  /* Search in table only when there are active entries */
  if (ZbBeeStackApsGlobals(gPermissionsTableCount)) 
  {  
    /* Search for the entry to delete */
    for (i = 0; i < ZbStackTablesSizes(gApsMaxEntriesForPermissionsTable); i++)
    {
      if (IsEqual8Bytes(aDevAddr, ZbBeeStackApsGlobals(gaPermissionsTable[i].authorizedDevice)))
      {
        /* The entry must have the Ieee Address to delete and be an active one */
        if (ZbBeeStackApsGlobals(gaPermissionsTable[i].permissionsFlags) & gPermissionsEntryActive)
        {
          BeeUtilZeroMemory(&ZbBeeStackApsGlobals(gaPermissionsTable[i]), sizeof(permissionsTable_t));
          ZbBeeStackApsGlobals(gPermissionsTableCount)--;
          return gZbSuccess_c;
        }
        return gZdoNoEntry_c;
      }
    }
  }
  return gZdoNoEntry_c;
}

/**********************************************************************************
* Remove all devices from the Permissions Configuration Table and leave it in its default state.
* 
* Return value:
*
**********************************************************************************/
void RemoveAllFromPermissionsTable(void)
{
  /* Just set all Permissions Table entries to zero */
  BeeUtilZeroMemory(ZbBeeStackApsGlobals(gaPermissionsTable), sizeof(permissionsTable_t) * ZbStackTablesSizes(gApsMaxEntriesForPermissionsTable));
  ZbBeeStackApsGlobals(gPermissionsTableCount) = 0;
}

/**********************************************************************************
* Get the Permissions Configuration Table.
* 
* Return value:
*
**********************************************************************************/
index_t GetPermissionsTable
(
  uint8_t * pDstTable
)
{
  uint8_t i, count=0;
  
  /* No need to search in table when there is no active entry */
  if (!ZbBeeStackApsGlobals(gPermissionsTableCount))
    return count;
    
  /* Search in Permissions Table */
  for (i = 0; i < ZbStackTablesSizes(gApsMaxEntriesForPermissionsTable); i++)
  {
    /* Retrieve only the active entries */
    if (ZbBeeStackApsGlobals(gaPermissionsTable[i].permissionsFlags) & gPermissionsEntryActive)
    {
      FLib_MemCpy(&pDstTable[sizeof(permissionsTable_t) * count], &ZbBeeStackApsGlobals(gaPermissionsTable[i]), sizeof(permissionsTable_t));
      count++;
    }
  }
  /* Return the number of active entries in Permissions Table */
  return count;
}

/**********************************************************************************
* Set the Permissions Configuration Table.
* 
* Return value:
*
**********************************************************************************/
void SetPermissionsTable
(
  index_t entryCounter,
  uint8_t * payload
)
{
  /* Verification to don't overrun the array */
  if(entryCounter > ZbStackTablesSizes(gApsMaxEntriesForPermissionsTable))
    entryCounter = ZbStackTablesSizes(gApsMaxEntriesForPermissionsTable);
  
  /* Fill the non set entries with zeros */
  BeeUtilZeroMemory(&payload[(sizeof(permissionsTable_t) * entryCounter)], sizeof(permissionsTable_t) * (ZbStackTablesSizes(gApsMaxEntriesForPermissionsTable) - entryCounter));
  
  /* Set Permissions Table */
  FLib_MemCpy(ZbBeeStackApsGlobals(gaPermissionsTable), payload, sizeof(permissionsTable_t) * ZbStackTablesSizes(gApsMaxEntriesForPermissionsTable));
  ZbBeeStackApsGlobals(gPermissionsTableCount) = entryCounter;
}

#endif  /* (gZtcIncluded_d && (gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */


/******************************************************************************
* This function looks for a free entry in the source route table.  
*
* Return: The index of that entry, if it's free, otherwise return gNwkEntryNotFound_c
*         (0xFF)
******************************************************************************/ 
index_t GetFreeEntryInSourceRouteTable( void )
{
  index_t iIndex;
  for( iIndex = 0; iIndex < ZbStackTablesSizes(gMaxSourceRouteEntries); iIndex++ ) {
    if( IsEqual2Bytes(ZbBeeStackNwkGlobals(gaSourceRouteTable[iIndex].aDestinationAddress), gaBroadcastAddress) ){
      return iIndex;
    }
  }
 return gNwkEntryNotFound_c; 
}
/******************************************************************************
* This function look for an specific entry in source route Table with specific 
* find type.
*
* Returns:  Source route entry if destination address is found, otherwise return 
*           NULL
******************************************************************************/
sourceRouteTable_t* NwkRetrieveSourceRoute(uint8_t *pDestinationAddress){

  /* Index to traverse throught the source route table*/
  uint8_t iIndex;

  /* pointer to source route table entry */
  sourceRouteTable_t*  pSourceRouteTable;

  for (iIndex = 0; iIndex < ZbStackTablesSizes(gMaxSourceRouteEntries); iIndex++) {
    pSourceRouteTable = &( ZbBeeStackNwkGlobals(gaSourceRouteTable[ iIndex ] ));

    /* if address is found then return the the pointer to this entry*/
    if( IsEqual2Bytes( pSourceRouteTable->aDestinationAddress,pDestinationAddress))
    {return( pSourceRouteTable );}

  }
  return NULL;
}

/******************************************************************************
* This function creates and entry if it doesn't exists in the source route table 
* or update it if it already exist.
* 
* Return: void
******************************************************************************/ 
extern void GenerateNlmeNwkStatusIndication(zbStatus_t pNwkStatus, uint8_t * pShortAddress);

void NwkStoreSourceRoute(uint8_t* pDestinationAddress, routeRecord_t*  pRouteRecord){

  sourceRouteTable_t *pSRTE = NULL;

  index_t indexOfFreeEntryInSourceRouteTable;

  RouteRecordRelayList_t *pRouteRecordList;
  
  /* Verify if already exists an entry for that destination address */
  if( pNwkRetrieveSrcRoute )
    pSRTE = pNwkRetrieveSrcRoute(pDestinationAddress);

  
  /* If the entry doesn't exist then get a free entry */
  if(!pSRTE){

    if( pNwkGetFreeEntryInSrcRouteTable ){
      indexOfFreeEntryInSourceRouteTable = pNwkGetFreeEntryInSrcRouteTable();

      if(indexOfFreeEntryInSourceRouteTable !=  gNwkEntryNotFound_c){
        pSRTE =  &ZbBeeStackNwkGlobals(gaSourceRouteTable[ indexOfFreeEntryInSourceRouteTable ]);
      }
    }
  }
  /* if the entry exists or is new then fill it */
  if(pSRTE){
    /* Free the previous information and re-fill with the new. We assume that this entry
      has the path info empty - NULL - when it is free. */
    if ( pSRTE->path )
      FreeSourceRouteEntry( (uint8_t)(pSRTE - ZbBeeStackNwkGlobals(gaSourceRouteTable)) );
    
    /* First of all, we need to allocate the relay list */
    pRouteRecordList = MSG_Alloc( (pRouteRecord->relayCount << 1) );

    /* If there is no memeory then do not copy the source route entry */
    if(!pRouteRecordList){
      (void)GenerateNlmeNwkStatusIndication(gZbNoMem_c, pDestinationAddress);
      return;
    }
    /* Copy the destination address */
    Copy2Bytes(pSRTE->aDestinationAddress, pDestinationAddress);

    pSRTE->relayCount = pRouteRecord->relayCount;

    FLib_MemCpy(pRouteRecordList, pRouteRecord->relayList, (pRouteRecord->relayCount << 1) );

    /* Store the list */
    pSRTE->path = pRouteRecordList;

    /* Set the age field */
    pSRTE->age = gMinutesToExpireRoute;

  }

}

/* Clears the NIB counters */
void ResetTxCounters(void)
{
  ZbBeeStackNwkGlobals(gNibTxTotal)=0;
  ZbBeeStackNwkGlobals(gNibTxTotalFailures)=0;
}


/*****************************************************************************
* This function reset the source route table, on ZC and ZR 
*****************************************************************************/
void ResetSourceRouteTable(void){

  /* Index to traverse throught the source route table*/
  index_t iIndex;

  /* Set to NULL to the pointer */  
  for(iIndex = 0; iIndex < ZbStackTablesSizes(gMaxSourceRouteEntries); iIndex++){
    FreeSourceRouteEntry(iIndex );
  }
}

/*****************************************************************************
* This function free or remove a specific entry in the source route table 
*****************************************************************************/
void FreeSourceRouteEntry(index_t iIndex )
{
  BeeUtilSetToF(&ZbBeeStackNwkGlobals(gaSourceRouteTable[iIndex]), sizeof( sourceRouteTable_t ) - sizeof(ZbBeeStackNwkGlobals(gaSourceRouteTable[iIndex].path)));

  if( ZbBeeStackNwkGlobals(gaSourceRouteTable[iIndex].path )){
    /* Free the path list */
    MSG_Free(ZbBeeStackNwkGlobals(gaSourceRouteTable[iIndex].path ));

    /* Point to NULL to be reused againg */
    ZbBeeStackNwkGlobals(gaSourceRouteTable[iIndex].path) = NULL;
   }
}

#ifndef gHostApp_d
/****************************************************************************/
extern void ZDO_StateMachineTimerCallBack(uint8_t timerId);
void NwkStartOrStopNwkConcentratorDiscoveryTime(bool_t startOrStop)
{
  /* if nwkConcentratorDiscoveryTime is 0x00 it means it will be maanage by the application*/
  if( !startOrStop )
  {
    ZbTMR_StopTimer(gNwkConcentratorDiscoveryTimerID);
  }
  else
  {
    /* Otherwise, start again the timer */
    ZbTMR_StartSecondTimer(gNwkConcentratorDiscoveryTimerID, NlmeGetRequest(gNwkConcentratorDiscoveryTime_c), ZDO_StateMachineTimerCallBack);
  }
} 
#endif

#if( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) && gNwkSymLinkCapability_d
void NodeNotInParentLinkStatus()
{

}
#endif

void UpdateNtEntryLQI(neighborTable_t* pNTE, uint8_t newLQI)
{
  pNTE->lqi = newLQI;
}

bool_t IsProfileIdMatched(zbSimpleDescriptor_t *pSimpleDescriptor, zbProfileId_t aProfileId)
{
  return  
  (
    IsEqual2Bytes(pSimpleDescriptor->aAppProfId, aProfileId) || 
    (
      /* R20 Profile wildcard */
      (aProfileId[0] == 0xFF && aProfileId[1] == 0xFF) && 
      (
        /* Only public profiles are checked */
        (pSimpleDescriptor->aAppProfId[1] < 0x7F) || 
        /* ZigBee Profiled IDs 0x7Fxx are reserved except for 
           test profile 0x7F01 and Gateway 0x7F02 */
        (
          pSimpleDescriptor->aAppProfId[1] == 0x7F && 
          (
            pSimpleDescriptor->aAppProfId[0] == 0x01 || 
            pSimpleDescriptor->aAppProfId[0] == 0x02
          )
        )
      )
    )
  );
}   

#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) && gRnplusCapability_d 
/*****************************************************************************/
void StartRouteAndSourceRouteTable( void )
{
  /* Start aging route and source route table entries if and only if the gMinutesToExpireRoute is not zero */
  if(gMinutesToExpireRoute)
  {
    /* 
      This timer function require milliseconds so you need to convert gMinutesToExpireRoute in milliseconds.
      So, gMinutesToExpireRoute * 1000 milliseconds ( = 1 second ) * 60 seconds */
    ZbTMR_StartMinuteTimer(gExpireRouteEntriesTimerID, 1, CustomAgingRouteAndSourceTableTimeOutCallBack);
  }
}
/*****************************************************************************/
void AgingRouteAndSourceTableEntries( void )
{
  /* Index of the route table entry */  
  index_t iIndex = 0;

  /* Points to route table entry */
  routeTable_t *pRTE = NULL;
  
#if gConcentratorFlag_d
  /* Points to source route table entry */
  sourceRouteTable_t *pSRTE = NULL;
#endif
  /* Used to notify which device was expired */
  zbNwkAddr_t aDstNwkAddress;

  /* Used to notify or not to app that one entry has expired */
  bool_t NotifyToApp = FALSE;

  /* Indicate the expiration type */
  zbStatus_t status;

  /* ----------------- Aging Route Table ----------------- */
  for( iIndex = 0; iIndex < gMaxRouteTableEntries; iIndex++ ){

    pRTE = &ZbBeeStackNwkGlobals(gaRouteTable[iIndex]);

    /* Only valid entries can be aging */
    if(pRTE && !IsEqual2Bytes(pRTE->aDestinationAddress, gaBroadcastAddress) && pRTE->properties.status != gInactive_c)
    {
      /* aging the route */
      if( pRTE->age )
      {
        pRTE->age--;
      }

      if( !pRTE->age )
      {
        /* Notify to App */
        NotifyToApp = TRUE;

        status = gZbRouteExpided_c;

        Copy2Bytes(aDstNwkAddress, pRTE->aDestinationAddress );
        
        /*
          Remove the route entry and clean up any packets pending for this route
          to be resolved.
        */
        CleanUpPacketsOnHoldTable(pRTE->aDestinationAddress);
        SetInactiveRotueEntry(iIndex);
      }
    }
  }/* for.. route entries */

  /* ----------------- Aging Source Route Table ----------------- */

#if gConcentratorFlag_d
  
  if(ZbBeeStackNwkGlobals(gNwkHighRamConcentrator))
  {
    /* 
    Particulary to aging the source route entries must be when the device is a nwk concentrator and when it has the
    high ram enabled.
    */
    for( iIndex = 0; iIndex < ZbStackTablesSizes(gMaxSourceRouteEntries); iIndex++ ){
      
      pSRTE = &ZbBeeStackNwkGlobals(gaSourceRouteTable[iIndex]);
      
      /* Only valid entries can be aging */
      if( pSRTE ) 
      {
        if( !IsBroadcastAddress(pSRTE->aDestinationAddress) && pSRTE->age )
        {
          pSRTE->age--;
        }
        
        if( !pSRTE->age ){
          
          /* Notify to App */
          NotifyToApp = TRUE;
          
          status = gZbSourceRouteExpided_c;
          
          Copy2Bytes(aDstNwkAddress, pSRTE->aDestinationAddress);
          
          /* Remove the source route entry */
          FreeSourceRouteEntry( (uint8_t)(pSRTE - ZbBeeStackNwkGlobals(gaSourceRouteTable)));
        }
      }
    }/* for.. route entries */
  }
  
#endif

  if( NotifyToApp )
  {
    GenerateNlmeNwkStatusIndication(status, aDstNwkAddress);
  }
}
/****************************************************************************/
void CustomAgingRouteAndSourceTableTimeOutCallBack
(
  zbTmrTimerID_t timerId  /* IN: */
)
{
  (void)timerId;  /* to prevent compiler warnings */

  /* Aging both route and source table entries */
  AgingRouteAndSourceTableEntries();

  ZbTMR_StartMinuteTimer(gExpireRouteEntriesTimerID, 1, CustomAgingRouteAndSourceTableTimeOutCallBack);
}

/******************************************************************************/
index_t ExpireAndGetEntryInRouteTable(void)
{
  /* Actual index */
  index_t iIndex;

  /* index of the field to compare */
  index_t iIndexCmp;

  /* index of the field to compare */
  index_t iIndexEntryExpired = gNwkEntryNotFound_c;

  /* First search for expired entry and if none is found then search the minor age */
  for( iIndex = 0; iIndex < gMaxRouteTableEntries; iIndex++ )
  { 
    if( !ZbBeeStackNwkGlobals(gaRouteTable[iIndex].age ))
      return iIndex;
    
    for( iIndexCmp = 0; iIndexCmp < gMaxRouteTableEntries; iIndexCmp++ )
    {
      if( ZbBeeStackNwkGlobals(gaRouteTable[iIndex].age) < ZbBeeStackNwkGlobals(gaRouteTable[iIndexCmp].age))
      {
        iIndexEntryExpired = iIndex;

      }
    }
  }
  if (iIndexEntryExpired != gNwkEntryNotFound_c)
  {
    // clean up any packets pending for this route to be resolved
    CleanUpPacketsOnHoldTable(ZbBeeStackNwkGlobals(gaRouteTable[iIndexEntryExpired].aDestinationAddress));
    // set route as inactive and return index.
    ZbBeeStackNwkGlobals(gaRouteTable[iIndexEntryExpired].properties.status) = gInactive_c;  
  }
  return iIndexEntryExpired;
}
#endif /*( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) && gRnplusCapability_d*/

#ifndef gHostApp_d
/******************************************************************************
* This function starts the aging timer

* 
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
void StartCustomAgingNeighborTableTimer(void)
{
    ZbTMR_StartMinuteTimer(gAgingNTEntriesTimerID, 1 , CustomAgingNeighborTableTimeOutCallBack); 
}
/******************************************************************************
* This function gets controll once the aging timeout is triggered, and ages
* the ticks on the NT once per each minute defined on the aging scale.
* 
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
void CustomAgingNeighborTableTimeOutCallBack
(
  zbTmrTimerID_t timerId  /* IN: The Id of the timer to be expired, may be needed or not.*/
)
{
  /* The timer Id is not needed, this function is linked to only one timer. */
  (void)timerId;  /* to prevent compiler warnings */

  /* Handle the resoltuin minutes. */
  if (ZbBeeStackNwkGlobals(gNwkAgingTickScale))
    --ZbBeeStackNwkGlobals(gNwkAgingTickScale);

  /* On Zero value we will eage by one, each time the resolutions minutes reach zero. */
  if (!ZbBeeStackNwkGlobals(gNwkAgingTickScale))
  {
    ZbBeeStackNwkGlobals(gNwkAgingTickScale) = gNwkAgingTickScale_c;
    /* Age by one tick each ZED on NT; also includes ZR for SP1, see inside the function.. */
#if !gZigbeeProIncluded_d
    AgingNeighborTable( gRouter_c | gEndDevice_c );
#else
    AgingNeighborTable( gEndDevice_c );
#endif
  }

  /* As the timer is a single shot we need to start again*/  
  StartCustomAgingNeighborTableTimer();
}
#endif
/******************************************************************************
* This function sets back to default the scale value, and the function is
* needed, on the library, nwk layer  portion to reset this value whe Nwk gets
* restarted. Remember library code can not use define macros from the
* application portion.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
void Nwk_ResetAgingTickScale(void)
{
  ZbBeeStackNwkGlobals(gNwkAgingTickScale) = gNwkAgingTickScale_c;
}

/*****************************************************************************
* This function must return false, it is implemented for future enchancements
*****************************************************************************/
bool_t SecurityTypeCheck( zbIeeeAddr_t aIeeeAddress )
{
  (void) aIeeeAddress;
  return FALSE;
}
/*****************************************************************************
* This function returns TRUE if a SE packet that doesn't belong to the Key
* Establishment cluester is encrypted with the preconfigured TC link key.
*****************************************************************************/
bool_t SeSecurityCheck
(
  uint8_t   frameControl,
  uint8_t   *pProfileId,
  uint8_t   *pSrcAddress
)
{
#if gZclSeSecurityCheck_d
  uint8_t profile[2] = {gSEProfIdb_c};
  /* Check if the packet is a aps data message or aps ack.*/
  if(((frameControl & gApsFrameControlTypeMask)==gApsFrameControlTypeData) || ((frameControl & gApsFrameControlTypeMask) == gApsFrameControlTypeAck))
  {
    if(IsEqual2Bytes(pProfileId, &profile))
    {
      return !ZclSE_ValidateKey(pSrcAddress);
    }
  }
#else
  (void)frameControl;
  (void)pProfileId;
  (void)pSrcAddress;
#endif
  return FALSE;
}
/*****************************************************************************/
#if gStochasticAddressingCapability_d
/*
  This function generates an stochastic short address for the device
  Params: 
  uint8_t*  pShortAddrGenerated: Variable where this function stores the short address.
*/
extern neighborTable_t* IsInNeighborTable(zbAddrMode_t addressMode, uint8_t *pAddress);
extern index_t IsInRouteTable(uint8_t * pDestinationAddressToCheck, zbAddrMode_t addrMode);
void NWK_ADDRESSASSIGN_GenerateAddressStochasticMode
(
  uint8_t* pShortAddrGenerated
)
{
  uint16_t retries = 0;
  uint16_t StochasticAddrGenerated16 = 0;

  /* used to check if the originator is one of our children */
  neighborTable_t *pNTE;

  /*
    This loop try of generates a random address for child device. 
    It retries giRetriesStochasticAddrLocalDevive.
  */
  do
  {
    /* Generate the short address */
    StochasticAddrGenerated16 = (((uint16_t)GetRandomNumber()) & 0xfff7);

    /*
      if the Stochastic address genereted is in the valid range 0x0001 and 0xFFF7 then proccess it.
    */
    if (StochasticAddrGenerated16)
    {
      /* store the address */
      Set2Bytes(pShortAddrGenerated, StochasticAddrGenerated16); 

      /*
        If I am the parent (Coordinator or Router), then I have to search the
        stochastic address generated in all NIB -neighbor and route- tables entries
        to can assign it to the device.
      */

      /* Check the Neighbor and Route table */
      pNTE = IsInNeighborTable(gZbAddrMode16Bit_c, pShortAddrGenerated);

      /*
        If the address generated doesn't exists in neighbor or route or both talbes or
        if the stochastic address generated is not my address then it must be generated again.
      */
      if ((!pNTE) 
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
#if gRnplusCapability_d
       && (IsInRouteTable(pShortAddrGenerated, gZbAddrMode16Bit_c) == gNwkEntryNotFound_c) 
#endif
#endif
      && (!IsEqual2Bytes(pShortAddrGenerated, NlmeGetRequest(gNwkShortAddress_c))) )
      {
        /* Finish the loop */
        retries = gDefaultRetriesStochasticAddrLocalDevice;
      }

      /* make it invalid */
      else
      {
        Copy2Bytes(pShortAddrGenerated, gaBroadcastAddress);
      }

      /* Increment the number of retries.*/
      ++retries;
    }

  }while(retries < gDefaultRetriesStochasticAddrLocalDevice);

}
#endif

#if gStandardSecurity_d
/******************************************************************************
* This funcion return the size of IEEE addresses contained in the nwk header.
*
* Interface assumptions: pNpdu is not null.
* 
* Return value: uint8_t - size of the IEEE addresses     
*   
* Effects on global data: NONE
* 
******************************************************************************/
extern uint8_t NwkReturnIeeeAddrLen(npduFrame_t *pNpdu);

/******************************************************************************
* This function is only called for all the packets with security off on a
* secure network, Depends on a global to allow packets, the relationship of
* the sending device and out own state, Data packet must be encrypted unless
* we allow for unsecure packet or come from a node getting authenticated or
* to us when we are in the way to be authenticated.
*
* Interface assumption:
*   The parameters pNTE and pNpduare not null pointers.
*
* Return vlue:
*   Success if the packet will be allow to reach the next higher layer.
*   Failure if the packet must be freed and will not reach the next higher layer.
*
*******************************************************************************/
zbStatus_t SSP_NwkVerifyUnsecurePacket
(
  neighborTable_t *pNTE,  /* IN: Entry on the NT where the node's info is stored, NULL if unknown. */
  npduFrame_t *pNpdu      /* IN: The payload of the incomming indication. */
)
{
  uint8_t *pCommandFrame;  /* Direct access to the command Id, if the packet is a Network command. */

  /* If we allow unsecure packets then let it go. */
  if (gAllowNonSecurePacketsInSecureMode)
    return gZbSuccess_c;  /* Success means let the packet go to the next higher layer. */

  /* If we dont know the device must some other type of packet. */
  if (pNTE)
  {
    /*
      Allow devices that aren't on our NT, just becuase thye may be ZED, from some other Depth.
      Or ZR from far way, and this may be the first time we hear then, now the next conditions
      may fial if this is not one of the allow packets.
    */
    /* If the sender is a device trying tobe authenticated,let it pass. */
    if (pNTE->deviceProperty.bits.relationship == gUnAuthenticatedChild_c)
      return gZbSuccess_c;  /* Success means let the packet go to the next higher layer. */
  }

  /* If we (local node receiving a packet) are not authenticated yet, we need to receive everything. */
  if (!ZDO_IsRunningState())
    return gZbSuccess_c;  /* Success means let the packet go to the next higher layer. */

  /*
    If it is a data frame (Not equal to command) and we don't allow unsecure packets then
    stop processing.
  */
  if (!IsACommandFrame(pNpdu))
  {
    /* This will return the security failure and will free the message. */
    return gZbSecurityFail_c;  /* Security fail means stop processing the packet and free the buffer. */
  }

  /* Get the command Id to be able to know if ti is one of the allow packets. */
  pCommandFrame = ((uint8_t *)pNpdu->pNwkPayload + NwkReturnIeeeAddrLen(pNpdu));

  /* Ok we have a command so far we just allow two command to be unsecure. Leave and rejoin (req and rsp). */
  if (pCommandFrame[0] != gRejoinRequest_c)
  {
    /* Invalid command return failure */
    return gZbSecurityFail_c;  /* Security fail means stop processing the packet and free the buffer. */
  }

  /* We mande it everyhitng is ok.. */
  return gZbSuccess_c;  /* Success means let the packet go to the next higher layer. */
}
#endif

 /******************************************************************************
 Set an IB attribute external(using UART, I2C etc)
*******************************************************************************/
void SetIBReqExternal(
            uint8_t iBReqType,   
            uint8_t attrId,
            uint8_t index,
            uint8_t *pValue
            ) 
{
#if defined(gHostApp_d)
  if(iBReqType == gApsmeReqType_c)
  {
  if((attrId == gApsChannelMask_c) ||
     (attrId == gApsAddressMap_c)  ||
     (attrId == gApsMaxWindowSize_c)  ||
     (attrId == gApsInterframeDelay_c)  ||
     (attrId == gApsMaxFragmentLength_c) 
    )
      ZtcApsmeSetRequest(attrId, index, pValue);
  }
  else if(iBReqType == gNLMEReqType_c)
  {
    if((attrId == gNwkPanId_c) ||
     (attrId == gNwkShortAddress_c)  ||
     (attrId == gNwkExtendedPanId_c)  ||
     (attrId == gNwkLogicalChannel_c)  ||
     (attrId == gNwkIndirectPollRate_c) 
    )
     ZtcNlmeSetRequest(attrId, index, pValue);
  } 
#elif defined(gBlackBox_d)
  if(iBReqType == gApsmeReqType_c)
  {
  if((attrId == gApsChannelMask_c) ||
     (attrId == gApsAddressMap_c) 
    )
      ZtcApsmeSetRequest(attrId, index, pValue);
  }
  else if(iBReqType == gNLMEReqType_c)
  {
    if((attrId == gNwkPanId_c) ||
     (attrId == gNwkShortAddress_c)  ||
     (attrId == gNwkExtendedPanId_c)  ||
     (attrId == gNwkLogicalChannel_c) 
    )
     ZtcNlmeSetRequest(attrId, index, pValue);
  } 
 
#else
  (void)iBReqType; 
  (void)attrId;
  (void)index;
  (void)pValue;
#endif 
}
/******************************************************************************
 Init the Address Map Table (fill it with 0xFF).
*******************************************************************************/
void AddrMap_InitTable(void)
{
  uint8_t i;
  
  /* Clear address map */
  BeeUtilLargeZeroMemory(ZbBeeStackApsGlobals(gaApsAddressMap), 
                        (uint16_t)ZbStackTablesSizes(gApsMaxAddressMapEntries) * sizeof(ZbBeeStackApsGlobals(gaApsAddressMap[0])));
  
  /* Initialize the short address with 0xffff */
  for(i=0; i< ZbStackTablesSizes(gApsMaxAddressMapEntries); i++)
  {
    Set2Bytes(ZbBeeStackApsGlobals(gaApsAddressMap[i].aNwkAddr), 0xFFFF);
  }
  
  /* Clear the gaApsAddressMapBitMask */
  BeeUtilZeroMemory(ZbBeeStackApsGlobals(gaApsAddressMapBitMask), ZbStackTablesSizes(gApsAddressMapBitMaskInBits));
}

/******************************************************************************
 Get an entry from the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_GetTableEntry(uint8_t index, void *pEntry)
{
 if((index >= ZbStackTablesSizes(gApsMaxAddressMapEntries)) || !pEntry) 
  return FALSE;
 
 FLib_MemCpy(pEntry, &ZbBeeStackApsGlobals(gpAddressMapTable[index]), sizeof(zbAddressMap_t));
 return TRUE;
}

/******************************************************************************
  Search IEEE address OR short address in the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_SearchTableEntry(zbIeeeAddr_t   *pIeeeAddr,
                                 zbNwkAddr_t    *pNwkAddr, 
                                 void           *pEntry)
{
  uint8_t i; 

  if(!pEntry || (!pIeeeAddr && !pNwkAddr)) 
    return 0xff;
      
  for(i = 0; i < ZbStackTablesSizes(gApsMaxAddressMapEntries); i++)
  {
    
    if( (pIeeeAddr && IsEqual8Bytes(ZbBeeStackApsGlobals(gpAddressMapTable[i].aIeeeAddr), (uint8_t*)pIeeeAddr)) ||
        (pNwkAddr && IsEqual2Bytes(ZbBeeStackApsGlobals(gpAddressMapTable[i].aNwkAddr), (uint8_t*)pNwkAddr)) ) 
    {
      FLib_MemCpy(pEntry, &ZbBeeStackApsGlobals(gpAddressMapTable[i]), sizeof(zbAddressMap_t));
      return i;
    } 
  }
  return 0xff;
}

 /******************************************************************************
 Set an entry in the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_SetTableEntry(uint8_t index, void *pEntry)
{
  if(index >= ZbStackTablesSizes(gApsMaxAddressMapEntries)) 
    return FALSE; 
  FLib_MemCpy(&ZbBeeStackApsGlobals(gpAddressMapTable[index]), pEntry, sizeof(zbAddressMap_t));
  return TRUE;
}

 /******************************************************************************
 Remove an entry from the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_RemoveEntry(uint8_t index)
{
  if(index >= ZbStackTablesSizes(gApsMaxAddressMapEntries)) 
    return FALSE; 
  
  BeeUtilZeroMemory(&ZbBeeStackApsGlobals(gpAddressMapTable[index]), sizeof(zbAddressMap_t));
  return TRUE;
}
/*******************************************************************************
Save an Address Map Table entry to NVM
*******************************************************************************/
void AddrMap_SaveEntryToNvm(uint8_t iNvmObject, uint8_t index)
{
  ZdoNwkMng_SaveToNvm(iNvmObject, &ZbBeeStackApsGlobals(gpAddressMapTable[index]));
}

 /******************************************************************************
 Init the Device Key Pair Set table (fill it with zeros).
*******************************************************************************/
void DevKeyPairSet_InitTable(void)
{
  uint8_t i;
  /* Clear gaApsDeviceKeyPairSet */
  BeeUtilLargeZeroMemory(ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet), 
                        (uint16_t)ZbStackTablesSizes(giApsDeviceKeyPairCount) * sizeof(ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet[0])));   
    /* Initialize each entry of the secure material. */
  for (i = 0; i < ApsmeGetRequest(gApsDeviceKeyPairCount_c); i++)
  {
    /* Device that are not registered yet.!! */
    ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet[i].iDeviceAddress) = gNotInAddressMap_c;

    /* Set the default TC key. */
    ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet[i].keyType) = ApsmeGetRequest(gApsDefaultTCKeyType_c);
  } 
}

 /******************************************************************************
 Get an entry from the Device Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_GetTableEntry(uint8_t index, void *pEntry)
{
 if((index >= ZbStackTablesSizes(giApsDeviceKeyPairCount)) || !pEntry) 
  return FALSE;
 
 FLib_MemCpy(pEntry, &ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet[index]), sizeof(zbApsDeviceKeyPairSet_t));
 return TRUE;
}


/******************************************************************************
 Set an entry in the evice Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_SetTableEntry(uint8_t index, void *pEntry)
{
  if(index >= ZbStackTablesSizes(giApsDeviceKeyPairCount)) 
    return FALSE; 
  FLib_MemCpy(&ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet[index]), pEntry, sizeof(zbApsDeviceKeyPairSet_t));
  return TRUE;
}

 /******************************************************************************
 Remove an entry from the Device Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_RemoveEntry(uint8_t index)
{
  if(index >= ZbStackTablesSizes(giApsDeviceKeyPairCount)) 
    return FALSE; 
  
  BeeUtilZeroMemory(&ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet[index]), sizeof(zbApsDeviceKeyPairSet_t));
  return TRUE;
}
 
 /******************************************************************************
 Save an entry from the Device Key Pair Set table to NVM.
*******************************************************************************/
void DevKeyPairSet_SaveEntryToNvm(uint8_t iNvmObject, uint8_t index)
{
  ZdoNwkMng_SaveToNvm(iNvmObject, &ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet[index]));
}

/******************************************************************************
 Init the Key Set table (fill it with zeros).
*******************************************************************************/
void KeySet_InitTable(void)
{
  /* Clear gaApsKeySet */
  BeeUtilLargeZeroMemory(ZbBeeStackApsGlobals(gaApsKeySet), 
                        (uint16_t)ZbStackTablesSizes(gApsKeySetCount) * sizeof(ZbBeeStackApsGlobals(gaApsKeySet[0])));    
}

 /******************************************************************************
 Get an entry(a key) from the Key Set table.
*******************************************************************************/
uint8_t KeySet_GetTableEntry(uint8_t index, void *pKey)
{
 if((index >= ZbStackTablesSizes(gApsKeySetCount)) || !pKey) 
  return FALSE;
 
 FLib_MemCpy(pKey, &ZbBeeStackApsGlobals(gaApsKeySet[index]), sizeof(zbAESKey_t));
 return TRUE;
}


 /******************************************************************************
 Set an entry (a key) in the Key Set table.
*******************************************************************************/
uint8_t KeySet_SetTableEntry(uint8_t index, void *pKey)
{
  if((index >= ZbStackTablesSizes(gApsKeySetCount)) || !pKey) 
    return FALSE; 
  FLib_MemCpy(&ZbBeeStackApsGlobals(gaApsKeySet[index]), pKey, sizeof(zbAESKey_t));
  return TRUE;
}

 /******************************************************************************
 Remove an entry (a key) from the Key Set table.
*******************************************************************************/
uint8_t KeySet_RemoveEntry(uint8_t index)
{
  if(index >= ZbStackTablesSizes(gApsKeySetCount)) 
    return FALSE; 
  
  BeeUtilZeroMemory(&ZbBeeStackApsGlobals(gaApsKeySet[index]), sizeof(zbAESKey_t));
  return TRUE;
}
 
 /******************************************************************************
  Search a key in the Key Set table.
*******************************************************************************/
uint8_t KeySet_SearchTableEntry(zbAESKey_t *pKey)
{
  uint8_t i; 
  
  if(!pKey)
    return 0xff;
  for(i = 0; i < ZbStackTablesSizes(gApsKeySetCount); i++)
  {
    if(FLib_MemCmp(ZbBeeStackApsGlobals(gaApsKeySet[i]), pKey, 16))
      return i;
  }
  return 0xff;
}


/*!
 * @fn 		  void GetMaxApplicationPayloadByAddrInfo(afAddrInfo_t *pAddrInfo)
 *
 * @brief	  Returns the maximum application payload (ASDU) that can fit
 *          into one single ZigBee packet, given the address information.  
 *
 * @param   [in] pAddrInfo   Pointer to the addressing info
 *
 * @return  uint8_t          Payload length     
 */
uint8_t GetMaxApplicationPayloadByAddrInfo
  (
  afAddrInfo_t *pAddrInfo /* IN: address info */
  )
{
  zbApsdeDataReq_t dataReq;
  
  if (pAddrInfo == NULL)
    return GetMaxApplicationPayload(NULL);
  
  /* Convert from the afAddrInfo_t structure to a zbApsdeDataReq_t structure */
  FLib_MemCpy(&dataReq.dstAddrMode, pAddrInfo, 	
    sizeof(zbAddrMode_t) + sizeof(zbApsAddr_t) + sizeof(zbEndPoint_t));
  Copy2Bytes(dataReq.aClusterId, pAddrInfo->aClusterId);
  dataReq.srcEndPoint = pAddrInfo->srcEndPoint;
  dataReq.txOptions = pAddrInfo->txOptions;
  dataReq.radiusCounter = pAddrInfo->radiusCounter;
  
  return GetMaxApplicationPayload(&dataReq);
}


 /******************************************************************************
*******************************************************************************
* Fragmentation Capability 
*******************************************************************************
******************************************************************************/

/*
  This function is called from within the BeeStack ZigBee library to handle a fragmented 
  transmission (Tx). If fragmentation is disabled at compile-time, then only the (small) 
  stub function is used to save code.
*/
bool_t APS_FragmentationTxStateMachine(apsTxIndex_t iTxIndex, apsTxState_t txState)
{
#if(gFragmentationCapability_d)
  return (APS_ActualFragmentationTxStateMachine(iTxIndex, txState));
#else
  return (APS_StubFragmentationTxStateMachine(iTxIndex, txState));
#endif
}

/*
  This function is called from within the BeeStack ZigBee library to handle a fragmented 
  reception (Rx). If fragmentation is disabled at compile-time, then only the (small) 
  stub function is used to save code.
*/
void APS_FragmentationRxStateMachine(void)
{
#if(gFragmentationCapability_d)
  APS_ActualFragmentationRxStateMachine();
#else
  APS_StubFragmentationRxStateMachine();
#endif
}

zbStatus_t Aps_SaveFragmentedData(index_t iIndexRxTable)
{
#if(gFragmentationCapability_d)
  return Aps_ActualSaveFragmentedData(iIndexRxTable);
#else
  (void)iIndexRxTable;
  return Aps_StubSaveFragmentedData();
#endif
}


/*****************************************************************************************
                          APS SECURITY COMPILE TIME CODE.
******************************************************************************************/

/************************************************************************************
* Reset the APS security materila to get the defualts.
*
* On a trust center, establish the predefined key (Master or Application) into each
* each entry of the security material set, given the possibility to every joining
* device the chance to share at least the key needed to communicate with the trust center.
* On a regular device (Non trust center), Stores the TC information into the entry zero
* of the set.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
************************************************************************************/
void APS_ResetSecurityMaterial
(
  void
)
{
#if gApsLinkKeySecurity_d
  SSP_ApsResetSecurityMaterial();
#endif
}

#ifndef gHostApp_d
/* This clear the APS security material, must be use when a node leaves. */
void APS_RemoveSecurityMaterialEntry
(
  uint8_t *pExtendedAddress
)
{
#if gApsLinkKeySecurity_d
  SSP_ApsRemoveSecurityMaterialEntry(pExtendedAddress); 
  /*Make sure device is removed from address map if it is not used for bindings*/
  (void)APS_RemoveDeviceFromAddressMap(pExtendedAddress);
#else
  (void)pExtendedAddress;
#endif
}
#endif

void APS_ResetDeviceCounters(uint8_t *pIEEEAddress)
{
#if gApsLinkKeySecurity_d
  SSP_ResetDeviceCounters(pIEEEAddress);
#else
  (void)pIEEEAddress;
#endif
}

uint8_t APS_TunnelCommandSize(void)
{
#if (gApsLinkKeySecurity_d && (gHighSecurity_d || gStandardSecurity_d))
  uint8_t size = 0;
  size = MbrOfs(zbApsmeTunnelCommand_t, tunnelCommand);
  /* By default we just support the Trasnport key more considerations can be added later. */
  size += sizeof(zbApsmeTransportKeyCommand_t);
  size += mMICLength[NlmeGetRequest(gNwkSecurityLevel_c)];
  return size;
#else
  return 0;
#endif

}

zbSize_t  APS_GenerateTunnelCommand
(
  zbApsmeTunnelCommand_t  *pTunnelCommand,
  zbApsmeTunnelReq_t  *pTunnelReq,
  uint8_t *pDestAddress,
  bool_t *pSecurityEnable
)
{
#if gApsLinkKeySecurity_d && (gHighSecurity_d || gStandardSecurity_d)
  return SSP_GenerateTunnelCommand(pTunnelCommand, pTunnelReq, pDestAddress, pSecurityEnable);
#else
  (void)pTunnelCommand;
  (void)pTunnelReq;
  (void)pDestAddress;
  (void)pSecurityEnable;
  return 0;
#endif
}

void APS_SetSecureTxOptions
(
  zbApsTxOption_t  *pTxOptions,
  zbApsCommandID_t  cmdID,
  uint8_t *pDestAddress
)
{
#if gApsLinkKeySecurity_d
  SSP_ApsSetTxOptions(pTxOptions, cmdID, pDestAddress);
#else
  (void)pTxOptions;
  (void)cmdID;
  (void)pDestAddress;
#endif
}

/************************************************************************************
* Get the status of the security procedure, as well as the secure payload when the
* TxOptions requires it. If the APS layer has a frame, consisting of a header
* ApsHeader and Payload, that needs security protection and nwkSecurityLevel > 0,
* it shall apply security as descibed in this function.
*
* Interface assumptions:
*   The Aps header and the Aps payload are a contiguos set of bytes, in the way of
*   [ ApsHeader||ApsPayload ].
*   The incoming parameters pApsHeader, pApsFrameLength are not null pointers.
*   The memory buffer that holds the ApsHeader and Aps Payload is big enough to hold
*   the corresponding apsAuxiliaryFrame and MIC.
*
* Return value:
*   The status of the security procedure, the secure payload and the size of the
*   full payload after the security procedure.
*
************************************************************************************/
zbStatus_t APS_ProcessOutgoingFrame
(
  uint8_t  *pApsHeader,         /* IN: The pointer where the ApsDataRequest header is. */
  uint8_t  apsHeaderLength,     /* IN: Size in bytes of the Aps Header. */
  uint8_t  *pApsFrameLength,    /* IN/OUT: The size in bytes of the complete APS Frame
                                           [ header||payload ].*/
  zbNwkAddr_t  aDestAddress,    /* IN: The device who will receive the packet. */
  zbApsTxOption_t    txOptions, /*  */
  zbApsCommandID_t  apsCommandID
)
{
#if gApsLinkKeySecurity_d
  return SSP_ApsProcessOutgoingFrame(pApsHeader,
                                     apsHeaderLength,
                                     pApsFrameLength,
                                     aDestAddress,
                                     txOptions,
                                     apsCommandID);
#else
  (void)pApsHeader;
  (void)apsHeaderLength;
  (void)pApsFrameLength;
  (void)aDestAddress;
  (void)txOptions;
  (void)apsCommandID;
  return gZbSuccess_c;
#endif
}

bool_t ApsEncryptACK(zbIeeeAddr_t aDestAddress)
{
#if gStandardSecurity_d || gHighSecurity_d
#if gTrustCenter_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
  if (ZbBeeStackNwkGlobals(gTrustCenter))
#endif
  {
    uint8_t aExtAddr[8];
    if (!SecurityTypeCheck(APS_GetIeeeAddress(aDestAddress, aExtAddr)))
    {
      return TRUE;
    }
  }
#else
  (void)aDestAddress;
#endif
#else
  (void)aDestAddress;
#endif
  return TRUE;
}

                                           
zbStatus_t  APS_ProcessIncomingFrames
(
  uint8_t  *pApsHeader,
  uint8_t  *pApsFrameLength,
  zbNwkAddr_t  aSrcAddress
)
{
#if gApsLinkKeySecurity_d
  zbStatus_t status; 
#if gZclSETrustCenterSwapOutClient_d || gASL_EnableZllTouchlinkCommissioning_d
  uint8_t *pMsg;
#endif
  
#if gZclSETrustCenterSwapOutClient_d || gASL_EnableZllTouchlinkCommissioning_d
  /* save payload */
  /* 0x05 is the MIC length. we can;t access it from here. TODO*/
  pMsg = MSG_Alloc(*pApsFrameLength + 0x05);
  
  if(!pMsg)
  {
    return gZbNoMem_c;
  }
  
  FLib_MemCpy(pMsg, pApsHeader, *pApsFrameLength + 0x05);
#endif  
  
status =  SSP_ApsProcessIncomingFrames(pApsHeader, pApsFrameLength, aSrcAddress);

#if gZclSETrustCenterSwapOutClient_d 
  /* If we failed to decrypt the Transport key with the original link key, 
	we can retry using a hashed link key in the case of a TC swap out */
  if (status == gZbSecurityFail_c)
  {
    if ((ZDO_GetState() == gZdoDeviceWaitingForKeyState_c) || (ZDO_GetState() == gZdoDeviceAuthenticationState_c))
    {
      zbApsDeviceKeyPairSet_t devKeyPairSetEntry;
      zbAESKey_t hashedLinkKey, linkKey;
      zbIeeeAddr_t aExtAddr; 
      zbAddressMap_t addrMap;
      
      /* Restore payload */
      FLib_MemCpy(pApsHeader, pMsg, *pApsFrameLength + 0x05);
      
      (void)DevKeyPairSet_GetTableEntry(0, &devKeyPairSetEntry);
      KeySet_GetTableEntry(devKeyPairSetEntry.iKey, &linkKey);
      BeeUtilZeroMemory(hashedLinkKey, sizeof(zbAESKey_t));
      SSP_MatyasMeyerOseasHash((uint8_t *)&linkKey, sizeof(zbAESKey_t), hashedLinkKey);
      KeySet_SetTableEntry(devKeyPairSetEntry.iKey, &hashedLinkKey);
      
      (void)AddrMap_RemoveEntry(devKeyPairSetEntry.iDeviceAddress);
      devKeyPairSetEntry.iDeviceAddress = AddrMap_SearchTableEntry(NULL, (zbNwkAddr_t*)aSrcAddress, &addrMap);
      (void)DevKeyPairSet_SetTableEntry(0, &devKeyPairSetEntry);
      (void)addrMap;
      
      /* Retry decrypting the packet with the hashed TC Link Key (TC was swapped)*/
      status =  SSP_ApsProcessIncomingFrames(pApsHeader, pApsFrameLength, aSrcAddress);
      
    }
  }   
#endif
  
#if gASL_EnableZllTouchlinkCommissioning_d
  if (status == gZbSecurityFail_c)
  {
    if ((ZDO_GetState() == gZdoDeviceWaitingForKeyState_c) || (ZDO_GetState() == gZdoDeviceAuthenticationState_c))
    {
      zbApsDeviceKeyPairSet_t devKeyPairSetEntry;
  
      /* Get the parent aps sec entry*/
      (void)DevKeyPairSet_GetTableEntry(0, &devKeyPairSetEntry);
      
      if(devKeyPairSetEntry.iKey == 0)
      {
      
        /* Restore payload */
        FLib_MemCpy(pApsHeader, pMsg, *pApsFrameLength + 0x05);
        
        /* set the other key */
        devKeyPairSetEntry.iKey = 1;
        (void)DevKeyPairSet_SetTableEntry(0, &devKeyPairSetEntry);
      
        /* Retry decrypting the packet with the hashed TC Link Key (TC was swapped)*/
        status =  SSP_ApsProcessIncomingFrames(pApsHeader, pApsFrameLength, aSrcAddress);
      }
    }
  }
#endif
  
#if gZclSETrustCenterSwapOutClient_d || gASL_EnableZllTouchlinkCommissioning_d
  MSG_Free(pMsg);
#endif  
  
  return status;
#else
  (void)pApsHeader;
  (void)pApsFrameLength;
  (void)aSrcAddress;
  return gZbSuccess_c;
#endif
}

bool_t IsApsLinkKeySecurityEnabled(void)
{
#if gApsLinkKeySecurity_d
  return TRUE;
#else
  return FALSE;
#endif
}

#ifndef gHostApp_d 
void APS_RegisterLinkKeyData
(
  zbIeeeAddr_t  aDeviceAddress,
  zbKeyType_t  keyType,
  uint8_t  *pKey
)
{
#if gApsLinkKeySecurity_d
  /*
    Whe a node wishes to register a link key (either is a Master key or not)
    uses the field parent address as the device with whom is sharing the key.
  */
 
  SSP_ApsRegisterLinkKeyData(aDeviceAddress, keyType, pKey);
#else
  (void)aDeviceAddress;
  (void)keyType;
  (void)pKey;
#endif
}
#endif

/************************************************************************************
* 053474r17 Sec. - 4.4.2.6.1 Generating and Sending the Initial SKKE-1 Frame
* The SKKE protocol begins with the initiator device sending an SKKE-1 frame.
* The SKKE-1 command frame shall be constructed as specified in subclause
* 4.4.9.1.
*
* Interface assumptions:
*   The parameters pSKKE, pEstablishKeyReq and pDestAddress are not null pointers.
*
* Return value:
*   The size in bytes of the APSME command to be send out.
*
************************************************************************************/
uint8_t APS_SKKE1
(
  zbApsmeSKKECommand_t       *pSKKE,             /* OUT: The place in memory where the
                                                         command will be build. */
  zbApsmeEstablishKeyReq_t   *pEstablishKeyReq,  /* IN: Establish key request form ZDO. */
  uint8_t                    *pDestAddress,      /* OUT: The address of the destination
                                                         node. */
  bool_t                     *pSecurityEnable
)
{
#if gSKKESupported_d
  return  SSP_SKKE1(pSKKE,
                    pEstablishKeyReq,
                    pDestAddress,
                    pSecurityEnable);
#else
  (void)pSKKE;
  (void)pEstablishKeyReq;
  (void)pDestAddress;
  (void)pSecurityEnable;
  return 0;
#endif
}

/************************************************************************************
* 053474r17 - Sec. 4.4.2.4 APSME-ESTABLISH-KEY.response
* 4.4.2.4.3 Effect on Receipt.
* If the Accept parameter is TRUE, then the APSME of the responder will attempt to
* execute the key establishment protocol indicated by the KeyEstablishmentMethod
* parameter. If KeyEstablishmentMethod is equal to SKKE, the APSME shall execute the
* SKKE protocol, described in subclause 4.4.2.6.
*
* Interface assumptions:
*   The parameters pSKKE2, pEstablishKeyRsp and pDestAddress are not null pointers.
*
* Return value:
*   The size in bytes of the APSME command to be send out.
*
************************************************************************************/
uint8_t APS_SKKE2
(
  zbApsmeSKKECommand_t     *pSKKE2,
  zbApsmeEstablisKeyRsp_t  *pEstablishKeyRsp,
  uint8_t                  *pDestAddress,
  bool_t                   *pSecurityEnable
)
{
#if gSKKESupported_d
  return SSP_SKKE2(pSKKE2,
                   pEstablishKeyRsp,
                   pDestAddress,
                   pSecurityEnable);
#else
  (void)pSKKE2;
  (void)pEstablishKeyRsp;
  (void)pDestAddress;
  (void)pSecurityEnable;
  return 0;
#endif
}

zbApsmeZdoIndication_t *APSME_SKKE_indication
(
  uint8_t                commandId,
  zbApsmeSKKECommand_t  *pSKKECmd,
  zbIeeeAddr_t           aSrcAddress
)
{
#if gSKKESupported_d
  zbApsmeZdoIndication_t *pZdoInd;

  switch (commandId)
  {
    case gAPS_CMD_SKKE_1_c:
      pZdoInd = APSME_SKKE1_indication((void *)pSKKECmd, aSrcAddress);
      break;

    case gAPS_CMD_SKKE_2_c:
      pZdoInd = APSME_SKKE2_indication((void *)pSKKECmd);
      break;

    case gAPS_CMD_SKKE_3_c:
      pZdoInd = APSME_SKKE3_indication((void *)pSKKECmd);
      break;

    case gAPS_CMD_SKKE_4_c:
      pZdoInd = APSME_SKKE4_indication((void *)pSKKECmd);
      break;

    default:
      pZdoInd = NULL;
      break;
  }

  return pZdoInd;
#else
  (void)commandId;
  (void)pSKKECmd;
  (void)aSrcAddress;
  return NULL;
#endif
}

zbApsmeZdoIndication_t *APSME_TUNNEL_CMMD_indication
(
  zbApsmeTunnelCommand_t  *pTunnel
)
{
#if gApsLinkKeySecurity_d
  return APSME_TUNNEL_indication((void *)pTunnel);
#else
  (void)pTunnel;
  return NULL;
#endif
}

zbApsDeviceKeyPairSet_t * APS_GetSecurityMaterilaEntry
(
  uint8_t *pDeviceAddress,
  zbApsDeviceKeyPairSet_t *pWhereToCpyEntry
)
{
#if gApsLinkKeySecurity_d
  return SSP_ApsGetSecurityMaterilaEntry(pDeviceAddress, pWhereToCpyEntry);
#else
  (void)pDeviceAddress;
  (void)pWhereToCpyEntry;
  return NULL;
#endif
}

/*
  Set the default AIB variables.
*/
void SetupAIBDefaults(void)
{
  uint32_t  channelMask;

  /* window size (1-8)  */
  ApsmeSetRequest(gApsMaxWindowSize_c, gApsWindowSizeDefault_c);

  /* APS Fragmentation: 0-255, delay in ms between frames */
  ApsmeSetRequest(gApsInterframeDelay_c, gApsInterframeDelayDefault_c);

  /* APS fragmentation, maximum size of fragment for each OTA frame */
  ApsmeSetRequest(gApsMaxFragmentLength_c, gApsMaxFragmentLengthDefault_c);

  /* List of allowed channels for the node to work in, used specially for freq.agility */
  channelMask = mDefaultValueOfChannel_c;
  
  #if(gInstantiableStackEnabled_d == 1)
  {
    extern uint32_t zbProInstance;
    if(zbProInstance > 0)
    {
      channelMask = mDefaultValueOfChannelPan2_c;
    }
  }
  #endif
    
  ApsmeSetRequest(gApsChannelMask_c, (uint8_t*)&channelMask);

  /* for multicast */
  ApsmeSetRequest(gApsNonmemberRadius_c, gApsNonMemberRadiusDefault_c);

  /* will this node boot as a ZC? */
  ApsmeSetRequest(gApsDesignatedCoordinator_c, gApsDesignatedCoordinatorDefault_c);

  /* join securly or not */
  ApsmeSetRequest(gApsUseInsecureJoin_c, gApsUseInsecureJoinDefault_c);
}
 /******************************************************************************
*******************************************************************************/

void UpdateNeighborRelationsTable(uint8_t iSrcNteIndex, linkStatusCommand_t * pLinkStatusCommand)
{
#if gNwkSmartSiblingReplacement_c
  NWK_UpdateNeighborRelationsTable(iSrcNteIndex, pLinkStatusCommand);
#else
  (void)iSrcNteIndex;
  (void)pLinkStatusCommand;
#endif
}

void ResetNeighborRelationsTable(void)
{
#if gNwkSmartSiblingReplacement_c
  NWK_ResetNeighborRelationsTable();
#endif
}

uint8_t GetNumberOfCommonNeighbors(uint8_t iNTIndex)
{
#if gNwkSmartSiblingReplacement_c
  return NWK_GetNumberOfCommonNeighbors(iNTIndex);
#else
  (void)iNTIndex;
  return 0;
#endif
}
/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/
/*None*/
/******************************************************************************
*******************************************************************************
* Private Debug Stuff
*******************************************************************************
******************************************************************************/
