/******************************************************************************
* BeeAppInit.c
*
* Initialization common to all applications. The very start of the program,
* main(), is found here.
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor
.
*******************************************************************************/
#include "EmbeddedTypes.h"
#include "MsgSystem.h"

#include "ZtcInterface.h"

#include "BeestackFunctionality.h"
#include "BeeStackInterface.h"

#if gBeeStackIncluded_d
  #include "BeeStackInit.h"
  #include "BeeStackUtil.h"
  #include "BeeStackConfiguration.h"
  #include "ZdoApsInterface.h"
  #include "ZdoMain.h"
#endif /* gBeeStackIncluded_d */

#include "BeeApp.h"

#ifndef gHostApp_d
  //#include "AppAspInterface.h"
#endif

#include "NwkMacInterface.h"

#include "BeeAppInit.h"

#include "BeeApp.h"
#include "EndPointConfig.h"

#include "ZdoNwkInterface.h"

/* For the Nlme_Sync_req */
#include "ASL_ZdpInterface.h"
#include "BeeStackInit.h"
#if gLpmIncluded_d 
//#include "pwr_interface.h"
//#include "pwr_configuration.h"
#endif

#include "Nwkcommon.h"

#include "BeeStack_Globals.h"
#include "NwkVariables.h"
#include "ApsVariables.h"
#include "AfVariables.h"
#include "ZDOVariables.h"
#include "BeeStackRamAlloc.h" 

#ifdef gHostApp_d
#include "ZtcHandler.h"
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define mMsgTypeForMSGInd                       0xFC

#ifdef PROCESSOR_KINETIS
  #define InterruptInit()
#endif

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

void APP_ZDPJoinPermitReq(uint8_t);
void BeeAppInit(void);
void AppResetApplicationQueues(void);

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

void Include_Symbols(void);

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

#if(gInstantiableStackEnabled_d == 1)
  beeAppDataInit_t* pBeeAppInitData;
#else
  anchor_t gAppDataConfirmQueue;
  anchor_t gAppDataIndicationQueue;
  anchor_t gInterPanAppDataConfirmQueue;
  anchor_t gInterPanAppDataIndicationQueue;
  
  #if (gLpmIncluded_d || gComboDeviceCapability_d)
    uint16_t PollTimeoutBackup; /*Stores orginal pollrate during binding.*/
  #endif
   
  zbEndPoint_t appEndPoint;
  zbTmrTimerID_t gAppGenericTimerId; 
#endif /* gInstantiableStackEnabled_d */ 


tsTaskID_t gAppTaskID;

 	

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* Permit Join Request through ZDO
*****************************************************************************/


/*****************************************************************************
* Callback for Idle timer
*
*****************************************************************************/
#define gIdleTaskNVIntervalEvent_c  ( 1 << 0 )
void IdleTaskNvTimerCallback(zbTmrTimerID_t timerID) {
  (void) timerID;
  //TS_SendEvent(gIdleTaskID, gIdleTaskNVIntervalEvent_c);
}

/***************************************************************************/
/* NOTE: NEED ALOT OF COMMENTS */
uint8_t InterPan_APP_SapHandler
(
  InterPanMessage_t *pMsg /*pointer from Intra Pan to APP*/
)
{

  zbInterPanDataIndication_t *pDataInd;
  uint8_t index;
  zbSimpleDescriptor_t *pSimpleDescriptor;
  uint8_t fMatch = FALSE;
  
  /* For Host application , all received SAP messages are forward 
     to Host(if Host uart communication is enabled) by ZTC */  
#ifndef gHostApp_d
  ZTC_TaskEventMonitor(gInterPanApp_SAPHandlerId_c, (uint8_t *) pMsg, gZbSuccess_c);
#endif
  if(pMsg->msgType == gInterPanDataCnf_c)
  {
    MSG_Queue( &BeeAppDataInit(gInterPanAppDataConfirmQueue), pMsg );
    TS_SendEvent(gAppTaskID, gInterPanAppEvtDataConfirm_c);
  }
  else if(pMsg->msgType == gInterPanDataInd_c)
  {
    /* filter the profile */
    pDataInd =(zbInterPanDataIndication_t *)(&pMsg->msgData.InterPandataIndication);
/* look for matching endpoint
            iIndex starts as 2 in order to ignore broadcast and ZDP endpoint. */
    #if TestProfileApp_d
	for(index = 0; index < ZbStackTablesSizes(gNoOfEndPoints) ; ++index)
	#else		    
    for(index = 2; index < ZbStackTablesSizes(gNoOfEndPoints) ; ++index) 
    #endif
	{
      /* is this a registered endpoint? */
      if(ZbBeeStackGlobalsParams(gaEndPointDesc[index].pDescription))
      {
        zbProfileId_t zllProfileId = {gZllProfileIdentifier_d};
        /* does profile ID match? */
        pSimpleDescriptor = (zbSimpleDescriptor_t *)ZbBeeStackGlobalsParams(gaEndPointDesc[index].pDescription->pSimpleDesc);
        if(IsEqual2Bytes(pSimpleDescriptor->aAppProfId, pDataInd->aProfileId) ||
           IsEqual2Bytes(zllProfileId, pDataInd->aProfileId))
        {
          fMatch = TRUE;
          break;
        }
      }
    } /* end for(...) */
    if(fMatch){
      MSG_Queue( &BeeAppDataInit(gInterPanAppDataIndicationQueue), pMsg );
      TS_SendEvent(gAppTaskID, gInterPanAppEvtDataIndication_c);
    }
    else{
      MSG_Free(pMsg);
    }
  }

  return gZbSuccess_c;
}
/***************************************************************************/

/*
  AppMsgCallBack

  Received a message.
*/
void AppMsgCallBack(apsdeToAfMessage_t *pMsg)
{
  MSG_Queue( &BeeAppDataInit(gAppDataIndicationQueue), pMsg );
  TS_SendEvent(gAppTaskID, gAppEvtDataIndication_c);
}

/***************************************************************************/

/*
  AppCnfCallBack

  Received a confirm.
*/
void AppCnfCallBack(apsdeToAfMessage_t *pMsg)
{
  MSG_Queue( &BeeAppDataInit(gAppDataConfirmQueue), pMsg );
  TS_SendEvent(gAppTaskID, gAppEvtDataConfirm_c);
}

/*
  DeQueue (but don't free) the application message queues.
  
*/
void AppResetApplicationQueues(void)
{
  List_ClearAnchor( &BeeAppDataInit(gAppDataIndicationQueue) );

  List_ClearAnchor( &BeeAppDataInit(gAppDataConfirmQueue) );

  /* Also Cleat all the events to the application. */
  TS_ClearEvent(gAppTaskID, 0xFFFF);
}
/*****************************************************************************
  AF_GetEndPointDevice

  Based on an endpoint number, get the pointer to the device data. Device data
  is unique per endpoint, and only for application endpoints (not ZDO or
  Broadcast).

  Returns pointer to endpoint device or NULL if doesn't exist.
*****************************************************************************/
afDeviceDef_t *AF_GetEndPointDevice
  (
  zbEndPoint_t endPoint /* IN: endpoint # (1-240) */
  )
{
#if gNum_EndPoints_c != 0
  index_t i;
  zbSimpleDescriptor_t *pSimpleDesc;

  if(!endPoint)
    return NULL;
  #if gInstantiableStackEnabled_d 
  for(i=0; i < EndPointConfigData(gNum_EndPoints); ++i) {
  #else
  for(i=0; i < gNum_EndPoints_c; ++i) {
  #endif  
    if(EndPointConfigData(endPointList[i].pEndpointDesc)) {
      pSimpleDesc = EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc);
      if(pSimpleDesc->endPoint == endPoint)
        return (afDeviceDef_t *)EndPointConfigData(endPointList[i].pDevice);
    }
  }
  return NULL;
#else
  (void) endPoint;
  return NULL;
#endif  
}

/*****************************************************************************
* AF_DeviceDefToEndPoint
*
* Look through endpoint descriptors for this device definition. 
* Returns endpoint #.
*****************************************************************************/
zbEndPoint_t AF_DeviceDefToEndPoint
  (
  afDeviceDef_t *pDeviceDef
  )
{
#if gNum_EndPoints_c != 0  
  index_t i;

    #if gInstantiableStackEnabled_d 
    for(i=0; i < EndPointConfigData(gNum_EndPoints); ++i) {
    #else
    for(i=0; i < gNum_EndPoints_c; ++i) {
    #endif 
    if(EndPointConfigData(endPointList[i].pDevice) == pDeviceDef)
      return EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint);
  }

  return gInvalidAppEndPoint_c; /* not found */
#else
  (void) pDeviceDef;
  return gInvalidAppEndPoint_c;
#endif  
  
}

/*****************************************************************************
* Get the simple descriptor for this index #
*****************************************************************************/
static zbSimpleDescriptor_t *AF_GetSimpleDescriptor
  (
  uint8_t index
  )
{  
  return (ZbBeeStackGlobalsParams(gaEndPointDesc[index].pDescription) ? ZbBeeStackGlobalsParams(gaEndPointDesc[index].pDescription->pSimpleDesc) : NULL);
}

/*****************************************************************************
* TS_AppBroadcastMsgCallBack
*
* Common routine called whenever a message on the broadcast endpoint (0xff)
* is received.
*
*****************************************************************************/
void AppBroadcastMsgCallBack
  (
  apsdeToAfMessage_t *pMsg  /* IN: broadcast message coming in */
  )
{
  zbSimpleDescriptor_t *pSimpleDescriptor;
  zbApsdeDataIndication_t *pIndication;
  zbEndPoint_t endPoint;
	index_t iIndex;
  apsdeToAfMessage_t *pPrevEpMsg;
  uint8_t prevEndPoint=gZbBroadcastEndPoint_c;

  /* get indication */
  pIndication = &(pMsg->msgData.dataIndication);

	/* look for matching endpoint
            iIndex starts as 2 in order to ignore broadcast and ZDP endpoint. */
	for(iIndex = 2; iIndex < ZbStackTablesSizes(gNoOfEndPoints); ++iIndex) {

	  /* for now, just try the first application endpoint */
  	pSimpleDescriptor = AF_GetSimpleDescriptor(iIndex);
  	if(!pSimpleDescriptor)
			continue;
	  endPoint = pSimpleDescriptor->endPoint;

	  /* profile filter */
  	if(!IsEqual2Bytes(pIndication->aProfileId, pSimpleDescriptor->aAppProfId))
			continue;

	  /* group filter  */
  	if(pIndication->dstAddrMode == gZbAddrModeGroup_c) {
    	if(!ApsGroupIsMemberOfEndpoint(pIndication->aDstAddr,endPoint))
				continue;
			}

    if(prevEndPoint  !=  gZbBroadcastEndPoint_c)
    {
      /* copy all message in order to queue it */
      pPrevEpMsg = AF_MsgAlloc();
      if (pPrevEpMsg)
      {
        FLib_MemCpy(pPrevEpMsg, pMsg, gMaxRxTxDataLength_c);
        pPrevEpMsg->msgData.dataIndication.pAsdu = ((uint8_t *)pMsg->msgData.dataIndication.pAsdu - (uint8_t *)pMsg) + (uint8_t *)pPrevEpMsg;
        /* Copy EndPoint number */
        pPrevEpMsg->msgData.dataIndication.dstEndPoint = prevEndPoint;    /* set endpoint to found application endpoint */
        /* tell ZTC about the message */
#ifndef gHostApp_d        
        ZTC_TaskEventMonitor(gAFDEAppSAPHandlerId_c, (uint8_t *) pPrevEpMsg, mMsgTypeForMSGInd);
#else
        ZTC_TaskEventMonitor(gpHostAppUart, gAFDEAppSAPHandlerId_c, (uint8_t *) pPrevEpMsg, mMsgTypeForMSGInd);
#endif 
        /* pass it on to the app */
        MSG_Queue( &BeeAppDataInit(gAppDataIndicationQueue), pPrevEpMsg);
        TS_SendEvent(gAppTaskID, gAppEvtDataIndication_c);
      }
    }
    /* found a new end point save its number to send it later */
    prevEndPoint = endPoint;
	} /* end of for loop */

	/* no endpoints matched, throw out msg */
	if(prevEndPoint == gZbBroadcastEndPoint_c)
  {
    MSG_Free(pMsg);
		return;
	}

  /* Copy EndPoint Number*/
  pIndication->dstEndPoint = prevEndPoint;    /* set endpoint to found application endpoint */
  /* tell ZTC about the message */
#ifndef gHostApp_d   
  ZTC_TaskEventMonitor(gAFDEAppSAPHandlerId_c, (uint8_t *) pMsg, mMsgTypeForMSGInd);
#else
  ZTC_TaskEventMonitor(gpHostAppUart, gAFDEAppSAPHandlerId_c, (uint8_t *) pMsg, mMsgTypeForMSGInd);
#endif
  /* pass it on to the app */
  MSG_Queue( &BeeAppDataInit(gAppDataIndicationQueue), pMsg );
  TS_SendEvent(gAppTaskID, gAppEvtDataIndication_c);
}

/*****************************************************************************
* AppStartPolling
* 
* This fucntion change the Poll rate to 1 sec and start the interval timer to 
* make polling every 1sec until it is stop calling function AppStopPolling.
*
******************************************************************************/
void AppStartPolling
  (
  void
  )
{  
#if (gLpmIncluded_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if(ZbBeeStackNwkGlobals(gLpmIncluded))
#endif
  {
 	  uint16_t BindingPollTimeout = 1000; /* 1sec */
 	  /*Only set the polltimeout if lowpower is enabled*/
    if (ZbBeeStackGlobalsParams(gBeeStackConfig.lpmStatus) == 1) {      
      BeeAppDataInit(PollTimeoutBackup) = NlmeGetRequest(gNwkIndirectPollRate_c);
      NlmeSetRequest(gNwkIndirectPollRate_c, BindingPollTimeout);
#ifndef gHostApp_d    
 	    NWK_MNGTSync_ChangePollRate(BindingPollTimeout); 
#endif    
    }
  }
#endif
}
  
/*****************************************************************************
* AppStopPolling
* 
* This fucntion change the Poll rate to 0 so it just make polling when need it.
*
******************************************************************************/
void AppStopPolling
  (
  void
  )
{
#if (gLpmIncluded_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if(ZbBeeStackNwkGlobals(gLpmIncluded))
#endif
  {
    /*Only restore orginal poll timeout if low power is enabled*/
    if (ZbBeeStackGlobalsParams(gBeeStackConfig.lpmStatus) == 1) {      
     NlmeSetRequest(gNwkIndirectPollRate_c, BeeAppDataInit(PollTimeoutBackup));
#ifndef gHostApp_d    
     NWK_MNGTSync_ChangePollRate(NlmeGetRequest(gNwkIndirectPollRate_c));
#endif     
    }
  }
#endif
}

/*****************************************************************************
* BeeAppDataInitVariables
* 
* 
*
******************************************************************************/
#if(gInstantiableStackEnabled_d == 1)
void BeeAppDataInitVariables(void)
{
  FLib_MemSet(pBeeAppInitData,0,sizeof(beeAppDataInit_t));
}
#endif

#ifdef FRDM_KW24D512
/* Protection for FRDM_KW24D512 at start-up.
The combo sensor is connected to KW24D512 NMI pin trough R117 */
void NMI_Handler(void)
{
    __asm volatile("NOP");
}
#endif
void DBG_VECT_HWfaultISR
(
    void
)
{
   __asm("BKPT #0\n") ; /* cause the debugger to stop */
}
