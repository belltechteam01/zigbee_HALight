/******************************************************************************
* In this file are the functions need it for the processing in 
*  frequency agility.
*
* (c) Copyright 2013, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
******************************************************************************/
#include "EmbeddedTypes.h"
#include "FunctionLib.h"
#include "TMR_Interface.h"
#include "MsgSystem.h"
#include "NV_Data.h"
#if (defined(gHostApp_d) || defined(gBlackBox_d))  
  #include "ZtcInterface.h"
#endif

#include "Zigbee.h"
#include "BeeCommon.h"
#include "Beestack_Globals.h"
#include "BeeStackUtil.h"
#include "BeeStackConfiguration.h"
#include "BeeStackInterface.h"

//#include "NwkCreateBeaconPayload.h"
#include "NwkMacInterface.h"
#include "nwkcommon.h"

#include "ZdoStateMachineHandler.h"
#include "ZdoNwkManager.h"
#include "ZdpManager.h"
#include "ZdoNwkInterface.h"
#include "ZdoApsInterface.h"
#include "ZdoVariables.h"
#include "ZdpFrequencyAgility.h"
#include "ASL_ZdpInterface.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/
  /* None */
/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
extern void  NWK_FRAMEHANDLE_SetBeaconPayloadRAM
(
  void
);
#endif
/******************************************************************************
*******************************************************************************
* Private type definitions
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
/* None */
/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)

void FA_ReportAnalysis
(
  zbEnergyValue_t  *pEnergyValues,
  zbChannels_t  ScannedChannels,
  uint8_t  scannedIndex
);

uint32_t ScanDurationToMilliseconds
(
  uint8_t scanDuration
);

void FA_SetPrivateChannelList(uint32_t *pChannelList);

#if (gMgmt_NWK_Update_req_d &&(gNetworkManagerCapability_d || gComboDeviceCapability_d))
void FA_SendEnergyScanRequestToRouters(void);
#endif

#endif
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
/************************************************************************************
* Given a 32 bit mask return a integer channel number. Or 0xFF if there is no valid
* channels. This fucntion is internal for the FA module.
*
* Interface assumptions:
*   The parameter pChannelList, is not a null pointer.
*   The parameter pChannelList, is a 32-bit mask.
*
* Return value:
*   The integer value between 11 and 26 of the first channel found in the list.
*
************************************************************************************/
uint8_t FA_GetChannelFromList
(
  uintn32_t *pChannelList  /* IN: the 32-bits channel list, the list shall have only
                                  one valid channel. */
)
{
  /* The counter to be use during the function. */
  uint8_t i;

#ifdef __IAR_SYSTEMS_ICC__
  uint32_t cl;

  /* Transform the data to be able to use the native value */
  FLib_MemCpy(&cl, pChannelList, sizeof(uint32_t));

  /* Eliminate the not in use bits on the channel list. */
  cl = (cl >> 11);

  /* check every one of the valid channels tha zigbee can use. */
  for (i = 0; i < 16; i++)
  {
    if (cl & 1)
    {
      return (i+11);
    }

    cl = (cl >> 1);
  }

  return 0xFF;
#else
  /* Transform the data to be able to use the native value */
  *pChannelList = OTA2Native32(*pChannelList);

  /*
    Verify if the list is invalid. If the list if empty or has an invalid
    channel set, then return an error code.
  */
  if (!(*pChannelList & 0x07fff800))
    return 0xFF;

  /* Eliminate the not in use bits on the channel list. */
  *pChannelList = (*pChannelList >> 11);

  /* check every one of the valid channels tha zigbee can use. */
  for (i = 0; i < 16; i++)
  {
    if (*pChannelList & 1)
    {
      return (i+11);
    }

    *pChannelList = (*pChannelList >>1);
  }

  return 0xFF;
#endif
}


#if gFrequencyAgilityCapability_d
/*-------------------- FA_Process_Mgmt_NWK_Update_request ----------------------
  This function is available for any RxOnWhenIdle device, to process the incomming
  Mgmt_NWK_Update_Request (2.4.3.3.9 Mgmt_NWK_Update_req). 
  Upon receipt, the Remote Device shall determine from the contents of the
  ScanDuration parameter whether this request is an update to the apsChannelMask
  and nwkManagerAddr parameters, a channel change command, or a request to
  scan channels and report the results.

  IN: The package with the request information.
  IN/OUT: The buffer where the response will be build.
*/
zbSize_t FA_Process_Mgmt_NWK_Update_request
(
  zbMgmtNwkUpdateRequest_t  *pMessageComingIn,  /* IN: The package with the request information. */
  void                      *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be build. */
  zbNwkAddr_t               aSrcAddrr
)
{
  /*
    Get the value to determine how the packet will intepretated.
  */
  uint8_t ScanDuration = pMessageComingIn->ScanDuration;
  uint8_t NewChannel;


  /*
    Free the big buffer being allocated by ZDP, we are not going to use it.
  */
  MSG_Free(pMessageGoingOut);

  /*
    Tell ZDP that the buffer is already free.
  */
  ZbZdoPrivateData(mbIsNotFree) = FALSE;

#if !gASL_EnableZllTouchlinkCommissioning_d    
  /*
    If the incomming request is not from the current Nwk Manager, we should ignore
    the request.
  */
  if (!IsEqual2Bytes(aSrcAddrr, NlmeGetRequest(gNwkManagerAddr_c)))
  {
    return gZero_c;
  }
#endif /* !gASL_EnableZllTouchlinkCommissioning_d */  

#if( gCoordinatorCapability_d || gRouterCapability_d  || gComboDeviceCapability_d )
#if gComboDeviceCapability_d
  if ((NlmeGetRequest(gDevType_c) == gEndDevice_c)&&
      (ScanDuration < 0xfe))
  {
    return gZero_c;
  }

#endif
  if ((ScanDuration <= 0x05) && (pMessageComingIn->ExtraData.ScanCount <= 0x05))
  {
    /*
      If the request indicates an energy scan of 0 scans, leave there is
      nothing to do.
    */
    if (!pMessageComingIn->ExtraData.ScanCount)
    {
      return gZero_c;
    }
    
    ZbZdoPrivateData(gScanCount) = pMessageComingIn->ExtraData.ScanCount;
    
    /*
    An Energy Scan was requested by the Network Manager, stop the gFA_NotifyTimer
    if started to make sure the Update Notify with the scan results can be sent.
    */
    ZbTMR_StopTimer(ZbZdoPrivateData(gFA_NotifyTimer));
    
    /*
      Catch the scan duration in the case that the command needs to be resent.
    */
    ZbZdoPrivateData(mFAScanDuration) = pMessageComingIn->ScanDuration;

    /*
      Get the channellist to be use during the FA procedure.
    */
    FA_SetPrivateChannelList((void *)pMessageComingIn->aScanChannels);

    /*
      Send the request out the NWK layer.
    */
    ZdoNwkMng_EnergyScanRequest(ZbZdoPrivateData(gaScanChannelList), ZbZdoPrivateData(mFAScanDuration));
  }
  else
#endif
    /*
      Requesting a Change channel with out Energy Scan.
    */
    if (ScanDuration == 0xfe)
    {
      /*
        Send the Set Channel request to the nwk layer, through the State machine.
      */
      NlmeSetRequest(gNwkUpdateId_c, &pMessageComingIn->ExtraData.NwkManagerData.nwkUpdateId);

      /*
        We need to know which channel (number) to use, before we actually change to it.
      */
      NewChannel = FA_GetChannelFromList((void *)pMessageComingIn->aScanChannels);
#if (gEndDevCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
      if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
        /*
          The ZEDs only update the update id and then, change the channel (Broadcast or unicast).
        */
        ASL_ChangeChannel(NewChannel);
#endif

#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
      /*
        The beacon information needs to be update as soon as possible to respond to the
        beacons request with the updated info as fast a possible.
      */
#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
      {
        NWK_FRAMEHANDLE_SetBeaconPayloadRAM();

        /*
          Routers and coordinators, if the packet was unicast then, just update the update id
          and change the channel.
        */
        if( !ZbZdoPrivateData(gIsABroadcast) && !IsSelfNwkAddress(aSrcAddrr) )
        {
          /*
            The ZEDs only update the update id and then, change the channel (Broadcast or unicast).
          */
          ASL_ChangeChannel(NewChannel);
        }
        else
        {
          NlmeSetRequest(gNwkLogicalChannel_c, &NewChannel);
          FA_SetNwkManagerState(gChangeChannelEnd_c);
          ZbTMR_StartSingleShotTimer(ZbZdoPrivateData(mGenericFATimer),
                                   NlmeGetRequest(gNwkNetworkBroadcastDeliveryTime_c),
                                   FA_TimeoutHandler);
        }
      }
#endif

    }
    /*
      Requesting to change the NwkUpdateId, ApsChannelMask and the NwkManagerAdddr.
    */
    else  if (ScanDuration == 0xff)
    {
      /*
        When is 0xff, is a synchronous call, it will never leave ZDP.
      */
      NlmeSetRequest(gNwkUpdateId_c,&pMessageComingIn->ExtraData.NwkManagerData.nwkUpdateId);
      NlmeSetRequest(gNwkManagerAddr_c, pMessageComingIn->ExtraData.NwkManagerData.aNwkManagerAddr);
      ApsmeSetRequest(gApsChannelMask_c, pMessageComingIn->aScanChannels);
    }
#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
    /*
      If for any reason the request is invalid, we should only answer if the
      it was unicast.
    */
    else if (!ZbZdoPrivateData(gIsABroadcast))
#if gComboDeviceCapability_d
    if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
    {
      /*
        Set the Scan Channels field
      */
      pMessageComingIn->aScanChannels[0] = 0;
      pMessageComingIn->aScanChannels[1] = 0;
      pMessageComingIn->aScanChannels[2] = 0;
      pMessageComingIn->aScanChannels[3] = 0;
      /*
        Just set the right status, the rest of the payload is filled with Zeros
        by default.
      */   
   ZDP_Mgmt_NWK_Update_Notify(NlmeGetRequest(gNwkManagerAddr_c),
                                 pMessageComingIn->aScanChannels,
                                 0, 0, 0, NULL, gZdoInvalidRequestType_c);
    }
#endif
  /*
    This will force ZDP to free the message and do not send anythign to next layer.
  */
  return gZero_c;
}
#endif

#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
/************************************************************************************
* Given a 32 bit mask representing the channel list, changes it for OTA to
* native, and set it into the local FA channel list.
*
* Interface assumptions:
*   The parameter pChannelList, if is not a null pointer and it should be a valid
*   OTA channel list.
*
* Return value:
*   NONE
*
************************************************************************************/
void FA_SetPrivateChannelList
(
  uint32_t *pChannelList  /* IN: The list to be set as the current channel list. */
)
{
  if (!pChannelList)
  {
    *((uint32_t *)ZbZdoPrivateData(gaScanChannelList)) = 0x07fff800;
  }
  else
  {
#ifdef __IAR_SYSTEMS_ICC__
    FLib_MemCpy(ZbZdoPrivateData(gaScanChannelList), pChannelList, sizeof(uint32_t));
#else
    *((uint32_t *)ZbZdoPrivateData(gaScanChannelList)) = OTA2Native32(*((uint32_t *)pChannelList));
#endif
  }

}

/************************************************************************************
* Given a 8 bit number sets a bit into a 32 bit mask and returns the value.
*
* Interface assumptions:
*   The parameter channelNumber, is a number between 11 and 26.
*
* Return value:
*   The 32-bit mask channel list.
*
************************************************************************************/
uint32_t FA_BuildListFromChannel
(
  uint8_t  channelNumber  /* IN: The channel number tio set a bit. */
)
{
  uint32_t  channelList=0;

  /*
    Set the right bit in to the list representing the given channel.
  */
  channelList |= ((uint32_t)1 << channelNumber);

  /*
    Change from the native format to OTA, swaping the bytes.
  */
  channelList = Native2OTA32(channelList);

  return (channelList);
}


/************************************************************************************
* This fucntion at most selects iNUmberOfRouters from the neighbor table.
*
* Interface assumptions:
*   The parameter pListOfRouters, is not a null pointer.
*   The array pointed by the parameter pListOfRouters is at least as big as the
*   number passed in the iNumberOfRouters parameter.
*
* Return value:
*   The amount of entries in the pListOfRouters array.
*   The array of Nwk Address pointed in the pListOfRouters parameter.
*
************************************************************************************/
zbSize_t FA_GetRoutersForScanningChannels
(
  zbNwkAddr_t *pListOfRouters,  /* IN/OUT: The buffer list of routers is returned. */
  zbSize_t iNumberOfRouters     /* IN: The actual number of router extracted from the NT.*/
)
{
  neighborTable_t  *pNeighborTable;
  index_t           iIndex;
  uint8_t           iRealListSize = 0;

  /*
    If there amount of routers to get from the NT is zero, no further processign should
    be done.
  */
  if (!iNumberOfRouters)
    return iRealListSize;

  /*
    For each device in the Neighbor table....
  */
  for(iIndex = 0 ; iIndex < gNwkInfobaseMaxNeighborTableEntry_c ; iIndex++)
  {
    pNeighborTable = NlmeGetRequestTableEntry(gNwkNeighborTable_c, iIndex);

    /*
      Get only the ones with router capability to process in the FA module.
    */
    if ( pNeighborTable->deviceProperty.bits.deviceType != gRouter_c
        || !IsValidNwkUnicastAddr(pNeighborTable->aNetworkAddress)
          || pNeighborTable->deviceProperty.bits.linkStatus != gInCommunication_c)
    {
      continue;
    }
    /*
      If we already have all the amoutn required, then we are done.
    */
    if (!iNumberOfRouters)
      break;

    /*
      Got one, one less to retrive.
    */
    iNumberOfRouters--;

    /*
      Fill the array with the current info.
    */
    Copy2Bytes(pListOfRouters[iRealListSize], pNeighborTable->aNetworkAddress);

    /*
      Report the real amount of address the deliver.
    */
    iRealListSize++;
  }

  /*
    At this point the amount of router should be at least one, if there is at
    least one in the NT.
  */
  return iRealListSize;
}

/************************************************************************************
* This function is a callback to be use for the timers needed, this timers
* handle the amount of error reports that arrive on a fixed period of time. The
* neighbor scan time out and the wait before the Nwk Manager switch channels on a
* channel change.
*
* Interface assumptions:
*   The parameter tmrId, is a previously register timer ID.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_TimeoutHandler
(
  zbTmrTimerID_t tmrId  /* IN: The timer Id to process. */
)
{
  /*
    The time in minutes to wait for the error reports to arrive into the Channel Master.
  */
  if (tmrId == ZbZdoPrivateData(mErrorReportTimer))
  {
      /*
      If the timer expired it means that the required number of reports was not
      received, so reset the relevant data
      */
      // Reset the number of reports received
      ZbZdoPrivateData(mErrorReportsReceived) = 0;
      // Reset the energy values for all channels to the default value.
      FLib_MemSet(ZbZdoPrivateData(mChannels), 0x7F, 16);
  }
  /*
    If the state machine is almost about to finish and the neighbor scan is done
    send the event to the state machine so it can move forward.
  */
  else if ((tmrId == ZbZdoPrivateData(mGenericFATimer)) && (FA_GetNwkManagerState() == gReportToTheApplication_c))
  {
    TS_SendEvent(ZbZdoPrivateData(gFATaskId), gNeighborScanTimeout_c);
  }
  /*
    This time expires once the message has being send as a broadcast and we have done
    waiting for the Broadcast delivery time, so we jump into the final state.
  */
  else if ((tmrId == ZbZdoPrivateData(mGenericFATimer)) && (FA_GetNwkManagerState() == gChangeChannelEnd_c) )
  {
    TS_SendEvent(ZbZdoPrivateData(gFATaskId), gDeliveryTimeOutExpires_c);
  }
#if (gMgmt_NWK_Update_req_d &&(gNetworkManagerCapability_d || gComboDeviceCapability_d))
  else if(tmrId == ZbZdoPrivateData(gFA_ReqEnergyScanToRoutersTimer))
  {
    FA_SendEnergyScanRequestToRouters();
  }
#endif
}

/************************************************************************************
* This function is only available for the Network Manager (Channel Master), to 
* process the incomming Mgmt_NWK_Update_Notify.
*
* Upon receipt of an unsolicited Mgmt_NWK_Update_notify, the network manager
* must evaluate if a channel change is required in the network. For this there
* are several steps to follow:
* Request other interference reports using the Mgmt_NWK_Update_req command.The
* network manager may request data from randomly selected routers in the network.
* (as indicated in the Zigbee spec R17- Annex E, for the purpose of giving the
* user a simple example of how and where this is done and it can be adjusted
* as need it).
*
* Interface assumptions:
*   The parameter pMessageComingIn, is not a null pointer.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_Process_Mgmt_NWK_Update_notify
(
  zbMgmtNwkUpdateNotify_t  *pMessageComingIn  /* IN: The package with the request information. */
)
{
  /* If the node receiving the MgmtNwkUpdateNotify is not a Network Manager then
     then do nothing and end the function. */
  if (!ZdoGetNwkManagerBitMask( ZbZdoPublicData(gaServerMask)) || pMessageComingIn->Status != gZbSuccess_c){
    return;
  }

  /*
    It does not matter if the current report was requested or not. Process it anyway.
  */
  FA_ReportAnalysis(pMessageComingIn->aEnergyValues,
                    pMessageComingIn->ScannedChannels,
                    pMessageComingIn->ScannedChannelsListCount);

  /*
    If the State machine is on init, this is an unrequested update notify.
  */
  if (FA_GetNwkManagerState() == gInitState_c)
  {
    /*
      If this is the first unrequested report received, then start the timer.
    */
    if (!ZbZdoPrivateData(mErrorReportsReceived))
    {
      /*
        This is also a valid report so consider it in the incoming amount.
      */
      ZbZdoPrivateData(mErrorReportsReceived)++;

      /*
        Start a timer with the maximumm amoutn of minutes to wait for the
        incomming error reports.
      */
      ZbTMR_StartMinuteTimer(ZbZdoPrivateData(mErrorReportTimer),
                           (zbTmrTimeInMinutes_t)gMaxTimeoutForIncomingErrorReports_c,
                           FA_TimeoutHandler);
    }

    /*
      If this is not the first report that arrives and timer has not expire,
      but we reach the limit of reports, then, stop the timer, clear the amount
      of incoming erros and send the event to the FA State machine.
    */
    else
    {
      if ((++ZbZdoPrivateData(mErrorReportsReceived)) >= gMaxIncomingErrorReports_c)
      {
        ZbTMR_StopTimer(ZbZdoPrivateData(mErrorReportTimer));
        TS_SendEvent(ZbZdoPrivateData(gFATaskId), gMaxErrorReports_c);
      }
    }  
  }

  /*
    If the repors arrive becasue the Nwk Manager request them then, 
  */
  if (FA_GetNwkManagerState() == gNeighborEnergyScan_c)
  {
    ZbZdoPrivateData(mNumScansReceived)++;
  }
}

/************************************************************************************
* Gets the amount of active channels in the apsCheannel mask and use it for the time
* out period to wait before the Nwk manager changes it's own current channel.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_SetWaitingForReportsTimer
(
  void
)
{
  /*
    The time in millisecond for the current scanduration.
  */
  uint32_t scanMilliseconds = 0;

  uint8_t i, numberOfRouters = 1;

  uintn32_t  channelList;

  /*
    Get the current channel list, we will use it to count the amount of channels set into it.
  */
  channelList = *((uint32_t *)ZbZdoPrivateData(gaScanChannelList));

  /*
    Eliminate the un used bits.
  */
  channelList = (channelList >> 11);

  for (i = 0; i < 16; i++)
  {
    if (channelList & ((uint32_t)(1<<i)))
      /*Increment scan duration for each channel.*/
      scanMilliseconds += ScanDurationToMilliseconds(gDefaultScanDuration_c);
  }
  
  if(ZbZdoPrivateData(mpFA_ListOfRouters) != NULL){
    numberOfRouters = ZbZdoPrivateData(mpFA_ListOfRouters->cDestAddrListLength);
  }

  ZbTMR_StartSingleShotTimer(ZbZdoPrivateData(mGenericFATimer),
                          gExtraTimeWindow_c + (scanMilliseconds + gApsAckWaitDuration_c * (gApsMaxRetries_c + 1)) * numberOfRouters,
                          FA_TimeoutHandler);
}

#if (gMgmt_NWK_Update_req_d &&(gNetworkManagerCapability_d || gComboDeviceCapability_d))
/************************************************************************************
* Selects an specific amount of router from the neighbor table and sends a unicast
* message to every single one, this is how the Nwk Manager requests a energy scan
* to the routers.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_SendEnergyScanRequestToRouters
(
void
)
{
  zbChannels_t channelList;
  
    /* If no list of network destination addresses exists, it means this is the
    first time this function is called during the current FA process, so create
    one. */
  if( !ZbZdoPrivateData(mpFA_ListOfRouters) ) {
      // Allocate memory for the list of destination network addresses.
    ZbZdoPrivateData(mpFA_ListOfRouters) = MSG_Alloc(sizeof(nwkAddrList_t) + (gMinNumberOfRouters_c - 1) * sizeof(zbNwkAddr_t));
    
    if( ZbZdoPrivateData(mpFA_ListOfRouters) == NULL ) {
      return;
    }
    
      // Initialize the index of the router to which the Energy Scan request is sent
    ZbZdoPrivateData(mpFA_ListOfRouters->iCurrentRouter) = 0;
      // Populate the list of destination addresses.
    ZbZdoPrivateData(mpFA_ListOfRouters->cDestAddrListLength) = FA_GetRoutersForScanningChannels( ZbZdoPrivateData(mpFA_ListOfRouters->aDestAddrList), gMinNumberOfRouters_c );
  }
  
    /* Send the Energy Scan request to the current router. */
  if(ZbZdoPrivateData(mpFA_ListOfRouters->iCurrentRouter) < ZbZdoPrivateData(mpFA_ListOfRouters->cDestAddrListLength)) {
    *((uint32_t *)channelList) = Native2OTA32(*((uint32_t *)ZbZdoPrivateData(gaScanChannelList)));
      // Send the energy request to the current router
    #if(gInstantiableStackEnabled_d == 1)  
    ZDP_Mgmt_NWK_Update_req(ZbZdoPrivateData(mpFA_ListOfRouters->aDestAddrList[pZbZdoPrivateData->mpFA_ListOfRouters->iCurrentRouter]),
                            channelList,
                            gDefaultScanDuration_c,
                            gDefaultScanCount_c);
   #else
    ZDP_Mgmt_NWK_Update_req(ZbZdoPrivateData(mpFA_ListOfRouters->aDestAddrList[mpFA_ListOfRouters->iCurrentRouter]),
                            channelList,
                            gDefaultScanDuration_c,
                            gDefaultScanCount_c);
    #endif
    // Increment the current router index in the mpFA_ListOfRouters
    ZbZdoPrivateData(mpFA_ListOfRouters->iCurrentRouter)++;
      // Increment the number of Energy Scan requests sent
    ZbZdoPrivateData(mNumScansSent)++;
  }
  
    /* If Energy Scan requests were sent to all the routers, clear the relevant data. */
  if(ZbZdoPrivateData(mpFA_ListOfRouters->iCurrentRouter) >= ZbZdoPrivateData(mpFA_ListOfRouters->cDestAddrListLength)) {
      /* All Energy Scan requests were sent, free the buffer containing the list
      of nwk destination addresses. */
    if(ZbZdoPrivateData(mpFA_ListOfRouters) != NULL)
    {
      MSG_Free(ZbZdoPrivateData(mpFA_ListOfRouters));
      ZbZdoPrivateData(mpFA_ListOfRouters) = NULL;
    }
  }
    /* If there are still routers to send the Energy Scan request start the timer
    for the next request. */
  else {
    ZbTMR_StartSingleShotTimer(ZbZdoPrivateData(gFA_ReqEnergyScanToRoutersTimer), gApsAckWaitDuration_c * (gApsMaxRetries_c + 1), FA_TimeoutHandler);
  }
}
#endif

/************************************************************************************
* Choose from the global array that keeps tract of the energy info of the cahnnels
* the less noicy one. Use a simple algorithm to select it. Asumes that the current
* channel is the best one and compares it, one to one with whole array.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   TRUE if a new channel was found. FALSE if it keep the current channel as the choosen one.
*
************************************************************************************/
bool_t FA_SelectChannelFromList
(
  void
)
{
  uint8_t i;
  uint8_t theChosenOne;
  uint8_t theOneIndex;

  theOneIndex = (NlmeGetRequest(gNwkLogicalChannel_c)-11);
  theChosenOne = ZbZdoPrivateData(mChannels[theOneIndex]);

  for (i = 0; i < 16; i++)
  {
    
    if (theChosenOne <= ZbZdoPrivateData(mChannels[i]))
      continue;

    theChosenOne = ZbZdoPrivateData(mChannels[i]);
    theOneIndex = i;
  }
  /*
    Clear the FA globals
  */
  ZbZdoPrivateData(mErrorReportsReceived) = 0;
  ZbZdoPrivateData(mNumScansReceived) = 0;
  ZbZdoPrivateData(mNumScansSent) = 0;
  FLib_MemSet(ZbZdoPrivateData(mChannels), 0x7F, 16);
  
  theOneIndex +=11;
  if (theOneIndex == NlmeGetRequest(gNwkLogicalChannel_c))
    return FALSE;

  NlmeSetRequest(gNwkLogicalChannel_c, &theOneIndex );
  return TRUE;
}

/************************************************************************************
* All channels are initialized with the middle energy value(0x7f) (most noisy
* channel has the value 0xff).
* This function updates each channel up/down based on the energy value from the
* scanned channel reported in the notify information packet.
*
* Interface assumptions:
*   The parameter pMessageComingIn, is not a null pointer.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_ReportAnalysis
(
  zbEnergyValue_t  *pEnergyValues,
  zbChannels_t  ScannedChannels,
  uint8_t  scannedIndex
)
{
  uint8_t i, j = 0;

  /*
    The incoming channel list on native format.
  */
  uint32_t channelList;

  /*
    Transform the data to be able to use the native value.
  */
  FLib_MemCpy(&channelList, ScannedChannels, sizeof(uint32_t));
  channelList = OTA2Native32(channelList);
  
  /* Verify if the channel list is invalid.*/
  if (!(channelList & 0x07fff800))
    return;

  /*
    Eliminate the bits that are not used.
  */
  channelList = (channelList >> 11);

  /* Check every channel... update up/down the channel[] for report analysis */
  for (i = 0; i < 16; i++)
  {
    /* Check if channel was scanned */
    if (!((channelList >> i) & 1)){
      continue;
    }
    /*
      Update up/down the mChannels array, based on the energy value from the
      scanned channel reported by the notify information packet.
    */
    if (pEnergyValues[j] < 0x7f)   //scannedIndex
    {
      if (pEnergyValues[j] <= 32)   //scannedIndex
        ZbZdoPrivateData(mChannels[i])-=7;
      else if (pEnergyValues[j] <= 64) //scannedIndex
        ZbZdoPrivateData(mChannels[i])-=5;
      else if (pEnergyValues[j] <= 96)  //scannedIndex
        ZbZdoPrivateData(mChannels[i])-=3;
      else
        ZbZdoPrivateData(mChannels[i])-=1;
    }
    else if(pEnergyValues[j] <= 159)   //scannedIndex
      ZbZdoPrivateData(mChannels[i])+=1;
    else if(pEnergyValues[j] <= 191)   //scannedIndex
      ZbZdoPrivateData(mChannels[i])+=3;
    else if(pEnergyValues[j] <= 223)   //scannedIndex
      ZbZdoPrivateData(mChannels[i])+=5;
    else
      ZbZdoPrivateData(mChannels[i])+=7;
   
    j++;
    /* Verify if there are more Energy reported values  */
    if (j == scannedIndex)
      return;

  } /* for (i = 0; i < 16; i++) */

}

/************************************************************************************
* Catch every time that the nwk layr sends a Tx report,inorder to keep track of
* the Trasnmitions attempts and the transmitions failures.
*
* Interface assumptions:
*   The parameter pNlmeNwkTxReport, is not a null pointer.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_ProcessNlmeTxReport
(
  nlmeNwkTxReport_t  *pNlmeNwkTxReport  /* IN: The report from the Nwk Layer. */
)
{
  /*
    The current percentage of failures, of the total of transmitions attempts.
  */
  uint16_t percentageOfFailure;

  if (!ZDO_IsRunningState())
  {
    return;
  }

  /*
    If TxAttempts is the total of transmitions, then, that is our 100%.
    So using a 3 value mechanism (3 simple rule) we calculate at what
    porcentage is equal the TxFailures corresponding to the current TxAttempts.
  */
  percentageOfFailure = (uint16_t)((pNlmeNwkTxReport->TxFailures * 100)/pNlmeNwkTxReport->TxAttempts);

  /*
    If this value exceed the porcentage that is the limit of tolerance, then,
    send the proper message.
  */
  if (percentageOfFailure >= gMaxTxFailuresPercentage_c)
  {
    /*
      Let the state machine know about the current event.
    */
    TS_SendEvent(ZbZdoPrivateData(gFATaskId), gMaxTransmitionsFailure_c);
  }
}

/************************************************************************************
* Initialize all the values needed to have the FA state machine running on the
* Nwk Manager device.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_StateMachineInit
(
  void
)
{
  if(ZbZdoPrivateData(gFATaskId) != gTsInvalidTaskID_c)
  {
    /*
      The FA state machine was already initialized. FA_StateMachineInit() can be
      called multiple times on combo devices.
    */
    return;
  }
  /*
    Create the task to use for the State machine.
  */
  ZbZdoPrivateData(gFATaskId) = TS_CreateTask(gTsFirstApplicationTaskPriority_c +1, FA_StateMachine);

  /*
    Set the state machine ready to run, before this the state machine is not working at all.
  */
  FA_SetNwkManagerState(gInitState_c);

  /*
    Clear the error globals.
  */
  ZbZdoPrivateData(mErrorReportsReceived) = 0;
  ZbZdoPrivateData(mNumScansReceived) = 0;
  ZbZdoPrivateData(mNumScansSent) = 0;

  /*
    Get the timers needed for the state machine.
  */
  ZbZdoPrivateData(mErrorReportTimer) = ZbTMR_AllocateTimer();

  /*
    Allocate a  multipropouse timer.
  */
  ZbZdoPrivateData(mGenericFATimer) = ZbTMR_AllocateTimer();
#if (gMgmt_NWK_Update_req_d &&(gNetworkManagerCapability_d || gComboDeviceCapability_d))
  /*
    Timer used to send the Energy scan requests. Only used by the Network Manager
  */
  ZbZdoPrivateData(gFA_ReqEnergyScanToRoutersTimer) = ZbTMR_AllocateTimer();
#endif

  /*
    Allocate the minute timer for the update notify.
  */
  ZbZdoPrivateData(gFA_NotifyTimer) = ZbTMR_AllocateTimer();
}

/************************************************************************************
* Time out function used to generate a Energy scan after a fixed period of time.
* Normally is used when with the scan count to avoid memory problems or fails on
* the notify delivery.
*
* Interface assumptions:
*   The parameter Timer is a vlid timer Id.
*
* Return value:
*   NONE.
*
************************************************************************************/
void GenerateEnergyRequest
(
  zbTmrTimerID_t Timer
)
{
  (void) Timer;
  ZdoNwkMng_EnergyScanRequest(ZbZdoPrivateData(gaScanChannelList), ZbZdoPrivateData(mFAScanDuration));
}

/************************************************************************************
* The function is use either by the Network manager or by any other device
* participating on the FA porcess. This function catchs every single energy scan
* confirm from the Nwk layer and process it.
*
* Interface assumptions:
*   The paramter pScanCnf is not a null pointer.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_ProcessEnergyScanCnf
(
  nlmeEnergyScanCnf_t *pScanCnf  /* The Nlme scan confirm. */
)
{
#if (gMgmt_NWK_Update_notify_d &&(!gNetworkManagerCapability_d || gComboDeviceCapability_d))
  zbChannels_t channelList;
#endif

#if gComboDeviceCapability_d
   /* Combo device acting like ZEd should not go further. */
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    return;
  }
#endif

  if (!ZDO_IsRunningState())
  {
    return;
  }
  
  if(!ZbZdoPrivateData(gScanCount))
  {
    /*
      Energy Scan is not related to the FA process
    */
    return;
  }
  
  /*
    Keep track of how many energy scan are left to do.
  */
  ZbZdoPrivateData(gScanCount)--;

  /*
    If the current node is the network manager process the energy scan as a local
    energy scan and inform the state amchine about it.
  */
  if (ZdoGetNwkManagerBitMask( ZbZdoPublicData(gaServerMask)))
  {
    /*
      Only the Network manager keeps track of every energy scan report.
    */
    FA_ReportAnalysis(pScanCnf->resList.pEnergyDetectList,
                      ApsmeGetRequest(gApsChannelMask_c),
                      pScanCnf->resultListSize);

    /*
      Change the values of the Nlme counters.
    */
    ResetTxCounters();

    /*
      Let the state machine know that the local energy scan is done.
    */
    TS_SendEvent(ZbZdoPrivateData(gFATaskId), gLocalScanArrive_c);
    return;
  }
#if (gMgmt_NWK_Update_notify_d &&(!gNetworkManagerCapability_d || gComboDeviceCapability_d))
  if(!ZbZdoPrivateData(gScanCount))
  {
    FA_SetNwkManagerState(gInitState_c);
  }

  /*
    If node is not the Network manager and the current energy scan fails,
    there is nothing to do.
  */
  if (pScanCnf->status)
    return;

  if (ZbTMR_IsTimerActive(ZbZdoPrivateData(gFA_NotifyTimer)))
    return;
  
  /*
    Send a Notify Message to the NwkManager for each energy scan received, only
    if the Timer is not active, to avoid reporting errors to often.
  */
  *((uint32_t *)channelList) = Native2OTA32(*((uint32_t *)ZbZdoPrivateData(gaScanChannelList)));

  ZDP_Mgmt_NWK_Update_Notify(NlmeGetRequest(gNwkManagerAddr_c),
                             channelList,
                             NlmeGetRequest(gNwkTxTotal_c),
                             NlmeGetRequest(gNwkTxTotalFailures_c),
                             pScanCnf->resultListSize,
                             pScanCnf->resList.pEnergyDetectList,
                             gZbSuccess_c);
  /*
    If there is more energy scans left ot do, process the next one, but not too often
    to avoid memory and delivery problems.
  */
  if (ZbZdoPrivateData(gScanCount))
  {
    ZbTMR_StartSingleShotTimer(ZbZdoPrivateData(mGenericFATimer), 250, GenerateEnergyRequest);
    return;
  }
  else
  {
    /*
      All the requested Energy Scans have been sent, start the gFA_NotifyTimer to
      limit the number of Update Notifies sent by the device and reset the counters.
    */
    ZbTMR_StartMinuteTimer(ZbZdoPrivateData(gFA_NotifyTimer), gTimeBeforeNextNwkUpdateNotify_c, NULL);
    ResetTxCounters();
  }
#endif
}

/************************************************************************************
* Use the information generated during the FA process and build a specific message,
* a notify in the case of the none Nwk Manager nodes, and an event to the state
* machine in the case of the Network Manager.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_SendReportToApplication
(
  void
)
{
  zdpToAppMessage_t  *pMsg;
  sChannelMasterStatistics_t  *pChannelReport;

#if gComboDeviceCapability_d
   /* Combo device acting like ZEd should not go further. */
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    return;
  }
#endif

  pMsg = MSG_Alloc(sizeof(sChannelMasterStatistics_t) + sizeof(zbMsgId_t));

  if (!pMsg)
    return;

  /*
    Build the error tobe send out to the application.
  */
  pMsg->msgType = gChannelMasterReport_c;
  pChannelReport = &pMsg->msgData.channelMasterReport;

  pChannelReport->reason = ZbZdoPrivateData(mScanReason);
  pChannelReport->numErrorReports  = ZbZdoPrivateData(mErrorReportsReceived);
  pChannelReport->numScansReceived = ZbZdoPrivateData(mNumScansReceived);
  pChannelReport->numScansRequest  = ZbZdoPrivateData(mNumScansSent);

  FLib_MemCpy(pChannelReport->aChannels, ZbZdoPrivateData(mChannels), sizeof(ZbZdoPrivateData(mChannels)));

  /*
    Use the ZDP Sap Handler to be able to send the report and see it thourgh test tool.
  */
  if (ZDP_APP_SapHandler(pMsg))
  {
    /*
      Catch Error here.
    */
  }
}

/************************************************************************************
* The state machine used by the network manager to keep track of the FA procedure
* described in 053474r17 Annex E, uses a task and events. This state machine is also
* used by the routers particiapting in the FA procedure.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_StateMachine
(
  tsEvent_t events
)
{
/* to avoid IAR compiler warning */  
#if (gMgmt_NWK_Update_req_d && (gNetworkManagerCapability_d || gComboDeviceCapability_d))
  zbChannels_t  aChannels;
#endif  

#if gComboDeviceCapability_d
   /* Combo device acting like ZEd should not go further. */
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    return;
  }
#endif

  switch (FA_GetNwkManagerState())
  {
    /*
      S1: The state machine is on the initial state
    */
    case gInitState_c:
      /*
        If the event Ev1 (ChangeChannel) arrives, change state to S5 (ChangeChannelStart).
      */
      if (events & gChangeChannel_c)
      {
        FA_SetNwkManagerState(gChangeChannelStart_c);
        TS_SendEvent(ZbZdoPrivateData(gFATaskId), gChangeChannel_c);
      }

      /*
        If the number maximum of error repots (Ev2) has arrive the change to S2
        (LocalEnergyScan).
      */
      if (events & gMaxErrorReports_c)
      {
        FA_SetNwkManagerState(gLocalEnergyScan_c);
        TS_SendEvent(ZbZdoPrivateData(gFATaskId), gExecuteState_c);
        ZbZdoPrivateData(mScanReason) = gScanReasonMaxErrorReports_c;
      }

      /*
        If The number of maximum transmitions failures has being reached ()Ev3,
        we need to do a Local energy Scan (S2).
      */
      if (events & gMaxTransmitionsFailure_c)
      {
        FA_SetNwkManagerState(gLocalEnergyScan_c);
        TS_SendEvent(ZbZdoPrivateData(gFATaskId), gExecuteState_c);
        ZbZdoPrivateData(mScanReason) = gScanReasonTxFailureCount_c;
      }

      /*
        If the application command to do an energy scan ,then, change the current
        state and proceed.
      */
      if (events & gInitiateEnergyScan_c)
      {
        FA_SetNwkManagerState(gLocalEnergyScan_c);
        TS_SendEvent(ZbZdoPrivateData(gFATaskId), gExecuteState_c);
        ZbZdoPrivateData(mScanReason) = gScanReasonUserInitiated_c;
      }
      break;

    /*
      S2: Do a local energy scan.
    */
    case gLocalEnergyScan_c:
      /*
        To avoid errors, just execute the code deliberately, with an event.
      */
      if (events & gExecuteState_c)
      {
        /*
          To avoid wrong execution only change the state inside the state machine.
        */
        FA_SetNwkManagerState(gNeighborEnergyScan_c);
        FA_SetPrivateChannelList(NULL);
        ZbZdoPrivateData(gScanCount) = 1;
        (void)ZdoNwkMng_EnergyScanRequest(ZbZdoPrivateData(gaScanChannelList), gDefaultScanDuration_c);
      }
      break;

    /*
      S3: Do a Neighbor energy scan.
    */
    case gNeighborEnergyScan_c:
#if (gMgmt_NWK_Update_req_d &&(gNetworkManagerCapability_d || gComboDeviceCapability_d))
      /*
        The incoming event is "Local Energy scan results arrive" (Ev5), we most start
        telling the neighbors to do the same.
      */
      if (events & gLocalScanArrive_c)
      {
        FA_SetNwkManagerState(gReportToTheApplication_c);
        FA_SendEnergyScanRequestToRouters();
        FA_SetWaitingForReportsTimer();
      }
#endif
      break;

    /*
      S4: Build the report to be send up to the application.
    */
    case gReportToTheApplication_c:
        if (events & gNeighborScanTimeout_c)
        {
          FA_SetNwkManagerState(gInitState_c);
          /*
            Don't send report to the application, move on.
          FA_SendReportToApplication();
          */
          FA_SelectChannelAndChange();
        }
      break;

    /*
      S5: The procedure to start the change of a channel gets started.
    */
    case gChangeChannelStart_c:
      if (events & gChangeChannel_c)
      {
        FA_SetNwkManagerState(gChangeChannelEnd_c);
/* to avoid warning on IAR */        
#if (gMgmt_NWK_Update_req_d &&(gNetworkManagerCapability_d || gComboDeviceCapability_d))
#if gComboDeviceCapability_d
      if ((NlmeGetRequest(gDevType_c) != gEndDevice_c)&& ZdoGetNwkManagerBitMask( ZbZdoPublicData(gaServerMask)))
#endif
        if (ZdoGetNwkManagerBitMask( ZbZdoPublicData(gaServerMask))){
          /*
          Increment the Network Update ID
          */
          NlmeGetRequest(gNwkUpdateId_c)++;
          *((uint32_t *)aChannels) = FA_BuildListFromChannel(NlmeGetRequest(gNwkLogicalChannel_c));
		  
          ZDP_Mgmt_NWK_Update_req((uint8_t *)gaBroadcastRxOnIdle, aChannels, 0xfe, NlmeGetRequest(gNwkUpdateId_c));
        }
#endif
        ZbTMR_StartSingleShotTimer(ZbZdoPrivateData(mGenericFATimer),
                                 NlmeGetRequest(gNwkNetworkBroadcastDeliveryTime_c),
                                 FA_TimeoutHandler);
      }
      break;

    /*
      S6: The final state here the onlything that we can do is change to current
      logical channel.
    */
    case gChangeChannelEnd_c:
      if (events & gDeliveryTimeOutExpires_c)
      {
        FA_SetNwkManagerState(gInitState_c);
        ASL_ChangeChannel(NlmeGetRequest(gNwkLogicalChannel_c));
        if(ZdoGetNwkManagerBitMask( ZbZdoPublicData(gaServerMask)))
        {
          ApsmeSetRequest(gApsChannelTimer_c, gChannelChangeTimeout_c);
        }
      }
      break;
  }
}

/************************************************************************************
* Used byt the application to select and change to a different channel if is requiered.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
************************************************************************************/
void FA_SelectChannelAndChange
(
  void
)
{
#if gComboDeviceCapability_d
   /* Combo device acting like ZEd should not go further. */
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    return;
  }
#endif

  if (!ZDO_IsRunningState())
  {
    return;
  }

  if (FA_SelectChannelFromList())
  {
    if(ApsmeGetRequest(gApsChannelTimer_c))
    {
      /*
        The device is not allowed to change the channel. Notify the application
        instead
      */
      FA_SendReportToApplication();
    }
    else
    {
      /*
        Enough time has passed since the last FA channel change. Change the channel.
      */
      FA_SendChangeChannelEvt();
    }
  }
}
#endif
/*gFrequencyAgilityCapability_d*/
