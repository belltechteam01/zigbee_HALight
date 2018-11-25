/*! @file	ZclSEDRLC.c
 *
 * @brief	This source file describes specific functionality implemented
 *			for the Demand Response and Load Control cluster.
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
 * 			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   		contributors may be used to endorse or promote products derived from this
 *   		software without specific prior written permission.
 *
 * 			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"

#include "SEProfile.h"
#include "ZclSE.h"
#include "ZclSEDRLC.h"
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
static uint8_t FindEventInEventsTable(uint8_t *pEvtId);
static uint8_t FindEmptyEntryInEventsTable(void);
static uint8_t FindEmptyEntryInServerEventsTable(void);
static void AddNewEntry(zclLdCtrl_EventsTableEntry_t *pDst, zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pSrc, afAddrInfo_t *pAddrInfo);
static void CheckForSuccessiveEvents(uint8_t msgIndex);
static uint8_t ZCL_ScheduleEvents(afAddrInfo_t *pAddrInfo, zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg);
static zbStatus_t ZCL_ProcessCancelLdCtrlEvt(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_ProcessCancelAllLdCtrlEvtst(zbApsdeDataIndication_t * pIndication);
static zbStatus_t ZCL_ProcessLdCtrlEvt(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_ProcessGetScheduledEvtsReq(addrInfoType_t *pAddrInfo, zclCmdDmndRspLdCtrl_GetScheduledEvts_t * pGetScheduledEvts);
static void LdCtrlEvtTimerCallback(zbTmrTimerID_t timerID);
static void LdCtrlJitterTimerCallBack(zbTmrTimerID_t timerID);
static zbStatus_t ZCL_SendLdCtlEvt(addrInfoType_t *pAddrInfo,zclLdCtrl_EventsTableEntry_t * pMsg);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/* Timers ID for DmndRspLdCtrl  */
static zbTmrTimerID_t gLdLdCtrlTimerID, mJitterTimerID;

/* Events Table Information */
static zclLdCtrl_EventsTableEntry_t gaEventsTable[gNumOfEventsTableEntry_c];
static zclLdCtrl_EventsTableEntry_t gaServerEventsTable[gNumOfServerEventsTableEntry_c];
static uint32_t mGetLdCtlEvtStartTime;
static uint8_t mGetNumOfLdCtlEvts;
static uint8_t mIndexLdCtl;

/*Used to keep the new message(event)*/
static zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t mNewLDCtrlEvt;
static afAddrInfo_t mNewAddrInfo;
static addrInfoType_t mAddrInfo;
static uint8_t mNewEntryIndex = 0xff, mReportStatusEntryIndex, mGetRandomFlag = TRUE;
static uint8_t mAcceptVoluntaryLdCrtlEvt = TRUE; /* Default Voluntary Load Control events are accepted */

/* Client Attributes */

const zclAttrDef_t gaZclDRLCClientAttrDef[] = {
  { gZclAttrIdDRLCClientUtilityGroup_c, gZclDataTypeUint8_c,  gZclAttrFlagsReportable_c | gZclAttrFlagsInRAMClient_c, sizeof(uint8_t) * zclReportableCopies_c, (void *) MbrOfs(zclDRLCClientAttr_t, UtilityGroup)},
  { gZclAttrIdDRLCClientStartRandomizeMin_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMClient_c, sizeof(uint8_t), (void *)MbrOfs(zclDRLCClientAttr_t, StartRandomizeMin)},
  { gZclAttrIdDRLCClientStopRandomizeMin_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAMClient_c, sizeof(uint8_t), (void *)MbrOfs(zclDRLCClientAttr_t, StopRandomizeMin)},
  { gZclAttrIdDRLCClientDevCls_c, gZclDataTypeUint16_c,  gZclAttrFlagsInRAMClientRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zclDRLCClientAttr_t, DevCls)}
};
const zclAttrSet_t gaZclDRLCClientAttrSet[] = {
  {gZclAttrSetDRLCClientSet_c, (void *)&gaZclDRLCClientAttrDef, NumberOfElements(gaZclDRLCClientAttrDef)}
};

const zclAttrSetList_t gZclDRLCClientClusterAttrSetList = {
  NumberOfElements(gaZclDRLCClientAttrSet),
  gaZclDRLCClientAttrSet
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		void ZCL_LdCtrlClientInit(void)
 *
 * @brief	Initalize the DRLC Client functionality
 *
 */
void ZCL_LdCtrlClientInit(void)
{
  
  gLdLdCtrlTimerID = ZbTMR_AllocateTimer();
  mJitterTimerID = ZbTMR_AllocateTimer();
} 

/*!
 * @fn 		zbStatus_t ZCL_DmndRspLdCtrlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev)
 *
 * @brief	Processes the requests received on the DRLC client.
 *
 */
zbStatus_t ZCL_DmndRspLdCtrlClusterClient
  (
  zbApsdeDataIndication_t *pIndication, /* IN: */
  afDeviceDef_t *pDev                /* IN: */
  ) 
{
  zclCmd_t Cmd;
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  (void) pDev;
  pFrame = (void *)pIndication->pAsdu;
  
  /* Get the cmd and the SE message */
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  
  /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    status = gZclSuccess_c;
  
  switch(Cmd) {
  case gZclCmdDmndRspLdCtrl_LdCtrlEvtReq_c:
    status = ZCL_ProcessLdCtrlEvt(pIndication);
    if (status == gZclInvalidField_c) {      
    /* If there is a invalid field in the request, the request will be ignored and no
    response sent */
      if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
        status = gZclSuccess_c;
      else
        status = gZclSuccessDefaultRsp_c;        
    }
    break;
    
  case gZclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_c:
     status = ZCL_ProcessCancelLdCtrlEvt(pIndication);
    break;  
    
  case gZclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_c:
     status = ZCL_ProcessCancelAllLdCtrlEvtst(pIndication);
    break;
    
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
  }
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_DmndRspLdCtrlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev)
 *
 * @brief	Processes the requests received on the DRLC server.
 *
 */
zbStatus_t ZCL_DmndRspLdCtrlClusterServer
(
zbApsdeDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zclFrame_t *pFrame;
  addrInfoType_t replyAddrInfo;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  zclCmdDmndRspLdCtrl_GetScheduledEvts_t * pGetScheduledLdCtlEvt;
  
  (void) pDev;
  pFrame = (void *)pIndication->pAsdu;
  
  /* Get the cmd and the SE message */
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  
  /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    status = gZclSuccess_c;
  
  /* get address ready for reply */
  AF_PrepareForReply((afAddrInfo_t*)&replyAddrInfo, pIndication);
  switch(Cmd) {
  case gZclCmdDmndRspLdCtrl_GetScheduledEvts_c:
    pGetScheduledLdCtlEvt = (zclCmdDmndRspLdCtrl_GetScheduledEvts_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetScheduledEvtsReq(&replyAddrInfo, pGetScheduledLdCtlEvt);
    break;
  case gZclCmdDmndRspLdCtrl_ReportEvtStatus_c:
    break;
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
  }
  return status;
}

/*!
 * @fn 		void ZCL_DeleteServerScheduledEvents(void)
 *
 * @brief	Deletes all scheduled events on the DRLC server.
 *
 */
void ZCL_DeleteServerScheduledEvents(void)
{
  uint8_t i;
  for(i = 0; i < gNumOfServerEventsTableEntry_c; i++)
    gaServerEventsTable[i].EntryStatus = 0x00;
}

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerLdCtrlEvents(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg)
 *
 * @brief	Store Load Control information received from the Provider.
 *    		The server side doesn't keep track of the  status, only stores the 
 *    		received events and take care that Nested and overlapping  commands not to occur.
 */	 
zbStatus_t ZCL_ScheduleServerLdCtrlEvents (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg)
{
  uint8_t i;
  uint32_t currentTime, startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime; 
  uint16_t duration;

  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  msgStartTime = OTA2Native32(pMsg->StartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  duration = OTA2Native16(pMsg->DurationInMinutes);
  msgStopTime = msgStartTime + (60*(uint32_t)duration);
  
  //if(msgStopTime <= currentTime)
  //	return status;
  
  /* Nested and overlapping Load Control Event commands are not allowed */
  for(i = 0; i < gNumOfServerEventsTableEntry_c; i++)
  {
    if(gaServerEventsTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerEventsTable[i].cmdFrame.StartTime);
    duration = OTA2Native16(gaServerEventsTable[i].cmdFrame.DurationInMinutes);
    stopTime = startTime + (60*(uint32_t)duration);
    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
      continue;
    return gZclFailure_c;
  }
  
  mNewEntryIndex = FindEmptyEntryInServerEventsTable();
  
  if(mNewEntryIndex != 0xff)
  {
    /* add the new event and check it for succesive events */
    zclLdCtrl_EventsTableEntry_t *pNewEntry = &gaServerEventsTable[mNewEntryIndex];
    AddNewEntry(pNewEntry, pMsg, &mNewAddrInfo);
  }
              
  if(mNewEntryIndex == 0xff)
    return gZclFailure_c;
  return gZclSuccess_c;
} 

/*!
 * @fn 		void ZCL_HandleReportEventsStatus(void)
 *
 * @brief	Handles the ReportEventsStatus command
 *    		
 */	
void ZCL_HandleReportEventsStatus(void)
{
  uint16_t randomTime;
  /* The timer should be allocated */
  if(!mJitterTimerID)
    return;
  
  if(!ZbTMR_IsTimerActive(mJitterTimerID))
  { 
    /* Delayed the Report Status Event after a random delay between 0 and 5 seconds, to avoid a potential storm of
    packets; this is done if a timer can be allocated */ 
    /* Get a random time and send the Report Status Event */
    randomTime = (uint16_t)GetRandomRange(10, 50);
    ZbTMR_StartSingleShotTimer(mJitterTimerID, randomTime, LdCtrlJitterTimerCallBack); 
  }
  else
  {
    /* handle next time */
   ZclSE_SendClusterEvt(gZclEvtHandleLdCtrl_c); 
  }
}

/*!
 * @fn 		void ZCL_HandleScheduledEventNow(void)
 *
 * @brief	Handles a current scheduled DRLC event
 *    		
 */	
void ZCL_HandleScheduledEventNow(void)
{
  static uint8_t startRandomize, stopRandomize;
  uint32_t currentTime;
  uint16_t cancelClass, notSupersededClass;
  uint32_t entryStartTime, entryStopTime, cancelTime, minTiming = 0xffffffff, supersededTime;
  uint16_t entryDuration;  
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pEvent;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  uint8_t noEntryFlag = FALSE;
  uint8_t index, randomTime;
  uint8_t attrLen;
  
  /* If a new entry was received, add it in the table if the entry is not used; 
     if it is used then supersede it first  */
  if(mNewEntryIndex != 0xff)
  {
    zclLdCtrl_EventsTableEntry_t *pNewEntry = &gaEventsTable[mNewEntryIndex];
    if (pNewEntry->EntryStatus == gEntryNotUsed_c)
    {
      /* add the new event and check it for succesive events */
      AddNewEntry(pNewEntry, &mNewLDCtrlEvt, &mNewAddrInfo);
      CheckForSuccessiveEvents(mNewEntryIndex);
      /* Call the App to signal that an Load Control Event was received. The user should check the Current Status */
      BeeAppUpdateDevice(mNewAddrInfo.srcEndPoint, gZclUI_LdCtrlEvt_c, 0, mNewAddrInfo.aClusterId, pNewEntry); 
      /* if a new event is received get the randomize timing */
      mGetRandomFlag = TRUE;
      /* New events can be received */
      mNewEntryIndex = 0xff;
      /* Handle the next Load Control Event*/             
      ZclSE_SendClusterEvt(gZclEvtHandleLdCtrl_c); 
      return;

    }
    else
    {
      /*all Class bits are overlapped; supersede the entry */
      pNewEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtSuperseded_c;
      mReportStatusEntryIndex =mNewEntryIndex;
      /* send the RES */
      ZclSE_SendClusterEvt(gzclEvtHandleReportEventsStatus_c);
      return;
    }
    
  }
    /*
    Randomization:
    - maintain its current state in the random period...
    - Check Effective End Time overlaps the Effective Start Time of a Successive Event, the Effective Start Time takes precedence
    - Devices shall not artificially create a gap between Successive Events
    ( use Start < Stop Randomization to prevent this)
    - It is permissible to have gaps when events are not Successive Events or Overlapping Events
    */
    if(mGetRandomFlag)
    {
      mGetRandomFlag = FALSE;
      (void)ZCL_GetAttribute(mNewAddrInfo.srcEndPoint, mNewAddrInfo.aClusterId, gZclAttrDRLCClientStartRandomizeMin_c, gZclClientAttr_c, &startRandomize, &attrLen);
      (void)ZCL_GetAttribute(mNewAddrInfo.srcEndPoint, mNewAddrInfo.aClusterId, gZclAttrDRLCClientStopRandomizeMin_c, gZclClientAttr_c, &stopRandomize, &attrLen);
      if(startRandomize)
        startRandomize = GetRandomRange(0, startRandomize);
      if(stopRandomize)
        stopRandomize = GetRandomRange(0, stopRandomize);
    }
  
    /* here get the currentTime  */ 
    currentTime = ZCL_GetUTCTime(mNewAddrInfo.srcEndPoint);
    for(index = 0; index < gNumOfEventsTableEntry_c; index++)
    { 
      pEntry = &gaEventsTable[index];
      /* if not used go to next entry*/  
      if(pEntry->EntryStatus == gEntryNotUsed_c)
        continue;
	  /* there are entries in table(not all entries had expired) */
      noEntryFlag = TRUE;
      /* point to event */
      pEvent = (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)&pEntry->cmdFrame;

/* Handle the supersed time */
      /*get the NotSupersededClass */
      notSupersededClass = pEntry->NotSupersededClass;
      /* Event is superseded???*/
      if(notSupersededClass != 0xffff)
      {
        /*All class bits are superseded???*/
        if(notSupersededClass == 0x0000)
        { 
          supersededTime = OTA2Native32(pEntry->SupersededTime);
          if(supersededTime <= currentTime)
          {
            /*all Class bits are overlapped; supersede the entry */
            pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtSuperseded_c;
            mReportStatusEntryIndex =index;
            /* send the RES */
            ZclSE_SendClusterEvt(gzclEvtHandleReportEventsStatus_c);
            return;
          }
        }          
        else 
        {
          /*Keep the entry with the class bits that was not superseded */  
          pEntry->cmdFrame.DevGroupClass = notSupersededClass;
          pEntry->NotSupersededClass = 0xffff;
        }  
      }

/* Handle the cancel time */
      /* get the timing */
      cancelTime = OTA2Native32(pEntry->CancelTime);
      /* Event is Canceled??? */
      if (cancelTime != 0xffffffff)
      {
        /*Is it using randomization???*/
        if(pEntry->CancelCtrl & gCancelCtrlEndRandomization_c)
          randomTime = stopRandomize;
        else
          randomTime = 0;
        /* Check if the cmd have to execute */
        cancelTime = cancelTime + (60 * (uint32_t)randomTime); /*random time is in minutes*/
        if(cancelTime <= currentTime)
        {
          cancelClass= pEntry->CancelClass;
          /*Cancel only the specified class*/
          notSupersededClass = pEvent->DevGroupClass & (cancelClass^pEvent->DevGroupClass);
          /*check if the Class is specified (CancelAll cmd don't specifies the class)*/
          if((cancelClass != 0xffff) && notSupersededClass)
          {           
            /*Keep the entry that wasn't Overlapped */
            pEvent->DevGroupClass = notSupersededClass;
            /*Set Cancel fields as beeing invalid... not used yet*/
            BeeUtilSetToF(&(pEntry->CancelTime), (sizeof(ZCLTime_t) + sizeof(LCDRDevCls_t) +1));
          }
          else
          {
            mReportStatusEntryIndex =index;
            pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtCancelled_c;
            ZclSE_SendClusterEvt(gzclEvtHandleReportEventsStatus_c);
            return;
          }          
        }/* if(cancelTime  <= currentTime)...*/
        else /* else      if(cancelTime <= currentTime)...*/
        {
          if( minTiming > cancelTime)
            minTiming = cancelTime;
        } 
      }/* if (cancelTime != 0xffffffff)...*/

/* Handle the start time and stop time */
      entryDuration = OTA2Native16(pEvent->DurationInMinutes);
      entryStartTime = OTA2Native32(pEvent->StartTime);
      entryStopTime = entryStartTime + ((uint32_t)entryDuration * 60);
      
      /*Is it using randomization???*/
      if(pEvent->EvtCtrl & 0x01)
      {
        /* If it is an succesive event, use the shortest random time for entryStartTime (to avoid gaps)*/
        if(pEntry->IsSuccessiveEvent)
          randomTime = (startRandomize <= stopRandomize)?startRandomize:stopRandomize;
        else
          randomTime = startRandomize;
      }
      else
        randomTime = 0;
      entryStartTime = entryStartTime + (60 * (uint32_t)randomTime); /*random time is in minutes*/
      
      if(pEvent->EvtCtrl & 0x02)
        randomTime = stopRandomize;
      else
        randomTime = 0;    
      entryStopTime = entryStopTime + (60 * (uint32_t)randomTime); /*random time is in minutes*/
      /* Check if the cmd have to execute */
      if( ((entryStartTime <= currentTime)&&(pEntry->NextStatus == gSELCDR_LdCtrlEvtCode_Started_c))||
         (entryStopTime <= currentTime) )
      {
        /* Send the RES */
        mReportStatusEntryIndex = index;
        ZclSE_SendClusterEvt(gzclEvtHandleReportEventsStatus_c);
        return; 
      }
      
      if(entryStartTime > currentTime)
      {
        if( minTiming > entryStartTime)
          minTiming = entryStartTime;
      }
      else
        if(entryStopTime > currentTime)
        {
          if( minTiming > entryStopTime)
            minTiming = entryStopTime;
        } 
    } /* end  for(index = 0; index < gNumOfEventsTableEntry_c; */
    
    /* Get Random Timing... next time when ZCL_HandleScheduledEventNow() is called  */
    mGetRandomFlag = TRUE;
    if(!noEntryFlag || (minTiming == 0xffffffff))
      ZbTMR_StopTimer (gLdLdCtrlTimerID);
    else{
      ZbTMR_StartSecondTimer (gLdLdCtrlTimerID, (uint16_t)(minTiming - currentTime), LdCtrlEvtTimerCallback);
    }
}

/*!
 * @fn 		uint8_t ZCL_SetOptStatusOnEvent(uint8_t *pEvtId, uint8_t optStatus) 
 *
 * @brief	Set "Opt In" or "Opt Out" Status for an event Id.
 *
 */
uint8_t ZCL_SetOptStatusOnEvent(uint8_t *pEvtId, uint8_t optStatus) 
{
  uint8_t index;
  afAddrInfo_t *pAddrInfo;
  
  if(!pEvtId) 
  {
    return 0xff;
  }
  
  index = FindEventInEventsTable(pEvtId);
  if(index == 0xff) { 
    return 0xff;
  }
  
  pAddrInfo =  (afAddrInfo_t *)(&gaEventsTable[index]);
  /* Set the CurrentStatus */
  gaEventsTable[index].CurrentStatus =  optStatus;
  (void)ZCL_SendReportEvtStatus(pAddrInfo, (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)&(gaEventsTable[index].cmdFrame), optStatus, FALSE);
  BeeAppUpdateDevice(pAddrInfo->srcEndPoint, gZclUI_LdCtrlEvt_c, 0, pAddrInfo->aClusterId, &gaEventsTable[index]);
  return index;
}

/*!
 * @fn 		uint8_t ZCL_DeleteLdCtrlEvent(uint8_t *pEvtId) 
 *
 * @brief	Deletes a scheduled DRLC server event for an event Id.
 *
 */
uint8_t ZCL_DeleteLdCtrlEvent(uint8_t *pEvtId) 
{
  uint8_t index;
  
  if(!pEvtId) 
  {
    return 0xff;
  }
  
  index = FindEventInEventsTable(pEvtId);
  if(index == 0xff) { 
    return 0xff;
  }
  
  gaEventsTable[index].EntryStatus = gEntryNotUsed_c;
  return index;
}

/*!
 * @fn 		void ZCL_AcceptVoluntaryLdCrtlEvt(bool_t flag)
 *
 * @brief	Accepting voluntary load control events.
 *
 */
void ZCL_AcceptVoluntaryLdCrtlEvt(bool_t flag)
{
  mAcceptVoluntaryLdCrtlEvt = flag;
}

/*!
 * @fn 		zbStatus_t ZCL_SendReportEvtStatus(afAddrInfo_t *pAddrInfo, zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg, 
 *											uint8_t eventStatus, bool_t invalidValueFlag)
 *
 * @brief	Create and Send the Report Event Status OTA.
 *
 */
zbStatus_t ZCL_SendReportEvtStatus
(
afAddrInfo_t *pAddrInfo, 
zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg, 
uint8_t eventStatus, 
bool_t invalidValueFlag /* if TRUE sent RES with invalid values for fields */
)
{
  zclDmndRspLdCtrl_ReportEvtStatus_t req;
  zclCmdDmndRspLdCtrl_ReportEvtStatus_t *pCmdFrame;
  uint32_t currentTime;
  Signature_t signature;
  
  pCmdFrame = &(req.cmdFrame);
  /* Prepare the RES */
  FLib_MemCpy(&req, pAddrInfo, sizeof(afAddrInfo_t));
  req.zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  FLib_MemCpy(pCmdFrame->IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  pCmdFrame->EvtStatus = eventStatus; 
  /* here get the currentTime  */
  currentTime = ZCL_GetUTCTime(pAddrInfo->srcEndPoint);

  currentTime = Native2OTA32(currentTime);
  pCmdFrame->EvtStatusTime = currentTime;
  if(!invalidValueFlag)
  {
    pCmdFrame->CritLevApplied = pMsg->CritLev;
    FLib_MemCpy(&(pCmdFrame->CoolTempSetPntApplied), 
                &(pMsg->CoolingTempSetPoint), (2 + sizeof(int8_t) + 2*sizeof(LCDRSetPoint_t)) );
    
  }
  else
  {
    /* set invalid values */
    pCmdFrame->CritLevApplied = 0x00; 
    pCmdFrame->CoolTempSetPntApplied = Native2OTA16(gZclCmdDmndRspLdCtrl_OptionalTempSetPoint_c); 	
    pCmdFrame->HeatTempSetPntApplied = Native2OTA16(gZclCmdDmndRspLdCtrl_OptionalTempSetPoint_c);
    pCmdFrame->AverageLdAdjustmentPercentage = gZclCmdDmndRspLdCtrl_OptionalAverageLdAdjustmentPercentage_c;
    pCmdFrame->DutyCycle = gZclCmdDmndRspLdCtrl_OptionalDutyCycle_c;
    pCmdFrame->EvtCtrl = 0x00;
  }
  pCmdFrame->SignatureType = gSELCDR_SignatureType_c;
  ZCL_ApplyECDSASign((uint8_t*)pCmdFrame, (sizeof(zclCmdDmndRspLdCtrl_ReportEvtStatus_t)-sizeof(Signature_t)), (uint8_t*)&signature);
  FLib_MemCpy(&(pCmdFrame->Signature[0]), &signature, sizeof(Signature_t));
  /* Send the RES over the air */
 return ZclDmndRspLdCtrl_ReportEvtStatus(&req);
}

/*!
 * @fn 		void ZCL_HandleGetScheduledLdCtlEvts(void)
 *
 * @brief	Handles the processing of the GetScheduledLoadControlEvent command.
 *
 */
void ZCL_HandleGetScheduledLdCtlEvts(void)
{
  uint32_t  startTime, stopTime;
  uint8_t status;
  
  if(mIndexLdCtl < gNumOfServerEventsTableEntry_c)
  {
    /* Check if the entry is used and if there are more scheduled events to be send*/
    if((gaServerEventsTable[mIndexLdCtl].EntryStatus != 0x00) && mGetNumOfLdCtlEvts)
    {
      startTime = OTA2Native32(gaServerEventsTable[mIndexLdCtl].cmdFrame.StartTime);
      stopTime = startTime + (uint32_t)gaServerEventsTable[mIndexLdCtl].cmdFrame.DurationInMinutes * 60;
     
      if ((mGetLdCtlEvtStartTime <= startTime) ||
          ((mGetLdCtlEvtStartTime > startTime) && (mGetLdCtlEvtStartTime < stopTime)))
      {
        /* Send This Load Control Event */
       	status = ZCL_SendLdCtlEvt(&mAddrInfo, &gaServerEventsTable[mIndexLdCtl]);
        if(status == gZclSuccess_c)
      	{
          --mGetNumOfLdCtlEvts;
      	}
        
      }
      /* GO and send the next Event */
      mIndexLdCtl++;
      ZclSE_SendClusterEvt(gzclEvtHandleGetScheduledLdCtlEvts_c);
    }
    
  }
}

/*!
 * @fn 		zbStatus_t ZclDmndRspLdCtrl_ReportEvtStatus(zclDmndRspLdCtrl_ReportEvtStatus_t *pReq)
 *
 * @brief	Sends the ReportEventStatus command over-the-air from the DRLC client.
 *
 */
zbStatus_t ZclDmndRspLdCtrl_ReportEvtStatus
(
zclDmndRspLdCtrl_ReportEvtStatus_t *pReq
)
{
  uint8_t length;
#if gASL_ZclDmndRspLdCtrl_ReportEvtStatus_d
  uint8_t entryIndex;
  
  if (pReq->cmdFrame.EvtStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptOut_c ||
      pReq->cmdFrame.EvtStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptIn_c)
  {
    entryIndex = FindEventInEventsTable((uint8_t *)&(pReq->cmdFrame.IssuerEvtID));
    if(entryIndex != 0xff)
      gaEventsTable[entryIndex].CurrentStatus = pReq->cmdFrame.EvtStatus;
  }
#endif  
  length = sizeof(zclCmdDmndRspLdCtrl_ReportEvtStatus_t);
  return ZCL_SendClientRspSeqPassed(gZclCmdDmndRspLdCtrl_ReportEvtStatus_c, length ,(zclGenericReq_t *)pReq); 
  
}

/*!
 * @fn 		zbStatus_t ZclDmndRspLdCtrl_LdCtrlEvtReq(zclDmndRspLdCtrl_LdCtrlEvtReq_t *pReq)
 *
 * @brief	Sends the LoadControlEvent command over-the-air from the DRLC server.
 *
 */
zbStatus_t ZclDmndRspLdCtrl_LdCtrlEvtReq
(
zclDmndRspLdCtrl_LdCtrlEvtReq_t *pReq
)
{
  
  return ZCL_SendServerReqSeqPassed(gZclCmdDmndRspLdCtrl_LdCtrlEvtReq_c, sizeof(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t ZclDmndRspLdCtrl_CancelLdCtrlEvtReq(zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t *pReq)
 *
 * @brief	Sends the CancelLoadControlEvent command over-the-air from the DRLC server.
 *
 */
zbStatus_t ZclDmndRspLdCtrl_CancelLdCtrlEvtReq
(
zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t *pReq
)
{ 
  return ZCL_SendServerReqSeqPassed(gZclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_c, sizeof(zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t ZclDmndRspLdCtrl_CancelAllLdCtrlEvtReq(zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *pReq)
 *
 * @brief	Sends the CancelAllLoadControlEvents command over-the-air from the DRLC server.
 *
 */
zbStatus_t ZclDmndRspLdCtrl_CancelAllLdCtrlEvtReq
(
zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_c, sizeof(zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t ZclDmndRspLdCtrl_GetScheduledEvtsReq(zclDmndRspLdCtrl_GetScheduledEvts_t *pReq)
 *
 * @brief	Sends the GetScheduledEvents command over-the-air from the DRLC client.
 *
 */
zbStatus_t ZclDmndRspLdCtrl_GetScheduledEvtsReq
(
zclDmndRspLdCtrl_GetScheduledEvts_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdDmndRspLdCtrl_GetScheduledEvts_c, sizeof(zclCmdDmndRspLdCtrl_GetScheduledEvts_t),(zclGenericReq_t *)pReq);	
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/******************************************************************************/
/* Process the Received Load Control Event....                                */ 
/*****************************************************************************/
static zbStatus_t ZCL_ProcessLdCtrlEvt(zbApsdeDataIndication_t *pIndication)
{
  uint8_t status = gZclSuccess_c;
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg;
  afAddrInfo_t addrInfo;
  uint16_t durationInMinutes;
  uint16_t cooling, heating;
  
  /* get the load control event request */
  pMsg = (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  /* get address ready for reply */
  AF_PrepareForReply(&addrInfo, pIndication); 
  /* check the fields ranges */
  durationInMinutes = OTA2Native16(pMsg->DurationInMinutes);
  if( durationInMinutes > gZclCmdDmndRspLdCtrl_MaxDurationInMinutes_c  )
    status = gZclInvalidField_c;
  
  cooling = OTA2Native16(pMsg->CoolingTempSetPoint);
  heating = OTA2Native16(pMsg->HeatingTempSetPoint); 
  if( (cooling != gZclCmdDmndRspLdCtrl_OptionalTempSetPoint_c) &&
     (heating != gZclCmdDmndRspLdCtrl_OptionalTempSetPoint_c) &&
       (pMsg->CoolingTempOffset != gZclCmdDmndRspLdCtrl_OptionalTempOffset_c) &&
         (pMsg->HeatingTempOffset != gZclCmdDmndRspLdCtrl_OptionalTempOffset_c) )
  {
    if(cooling > gZclCmdDmndRspLdCtrl_MaxTempSetPoint_c||
       heating > gZclCmdDmndRspLdCtrl_MaxTempSetPoint_c )
      status = gZclInvalidField_c;
  }
  else
  {
    if( (cooling == 0x8000 &&  heating != 0x8000)||
       (cooling != 0x8000 &&  heating == 0x8000)||
         (pMsg->CoolingTempOffset != 0xff && pMsg->HeatingTempOffset == 0xff)||
           (pMsg->CoolingTempOffset == 0xff && pMsg->HeatingTempOffset != 0xff))
      status = gZclInvalidField_c;
  }
  
  if( (pMsg->AverageLdAdjustmentPercentage > gZclCmdDmndRspLdCtrl_MaxAverageLdAdjustmentPercentage_c ||
       pMsg->AverageLdAdjustmentPercentage < gZclCmdDmndRspLdCtrl_MinAverageLdAdjustmentPercentage_c )&&
     pMsg->AverageLdAdjustmentPercentage != gZclCmdDmndRspLdCtrl_OptionalAverageLdAdjustmentPercentage_c )
    status = gZclInvalidField_c;
  
  if( pMsg->DutyCycle > gZclCmdDmndRspLdCtrl_MaxDutyCycle_c &&
     pMsg->DutyCycle != gZclCmdDmndRspLdCtrl_OptionalDutyCycle_c )
    status = gZclInvalidField_c;
  
  if(status == gZclInvalidField_c)
  {
    /* Send the Event status rejected */ 
    status = ZCL_SendReportEvtStatus(&addrInfo, pMsg, gSELCDR_LdCtrlEvtCode_LdCtrlEvtCmdRjctd_c, FALSE);  
  }
  else
  {
    status = ZCL_ScheduleEvents(&addrInfo, pMsg); 
  }
  return status;
}

/******************************************************************************/
/* Update the load contol table of events...
Check if the event can be handled(to set it having received status). 
An event is considered received if it is added in the EventsTable[], superseding or Not one or more events.
the received event should be filtered (Class, Utility) and the overlapping rules should be checked
*/ 
/*****************************************************************************/
static uint8_t ZCL_ScheduleEvents(afAddrInfo_t *pAddrInfo, zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg)
{
  uint8_t status = gZclSuccess_c;
  uint8_t utilityGroup;
  uint16_t devGroupClass, entryClass;
  uint32_t currentTime;
  uint32_t msgStartTime, entryStartTime ;
  uint32_t msgStopTime, entryStopTime, OTATime;
  uint16_t msgDuration, entryDuration;  
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pEvent;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  uint8_t index;
  //uint8_t attrData[4];
  uint8_t attrLen;
  
  /* the previous received event is still not added in the Table Entry */
  if (mNewEntryIndex != 0xff)
    return status;
  (void)ZCL_GetAttribute(pAddrInfo->srcEndPoint,pAddrInfo->aClusterId, gZclAttrDRLCClientUtilityGroup_c, gZclClientAttr_c, &utilityGroup, &attrLen);
  (void)ZCL_GetAttribute(pAddrInfo->srcEndPoint,pAddrInfo->aClusterId, gZclAttrDRLCClientDevCls_c, gZclClientAttr_c, &devGroupClass, &attrLen);
  
  /* Filter the device Class and Utility (if no match, no RES is send)*/
  pMsg->DevGroupClass = pMsg->DevGroupClass & devGroupClass;
  if((!pMsg->DevGroupClass) || ((pMsg->UtilityGroup != utilityGroup) && (pMsg->UtilityGroup != 0)))
    return gZclInvalidField_c; /* NO RES is send */

  /* Check the criticality Level */
  if((pMsg->CritLev > gZclCmdDmndRspLdCtrl_MaxCritLev_c )||
     (pMsg->CritLev >= gZclSECritLev_Green_c && pMsg->CritLev <= gZclSECritLev_5_c && !mAcceptVoluntaryLdCrtlEvt))
       return gZclInvalidField_c; /* NO RES is send */
  
  /* Check for duplicate event */
  if((FindEventInEventsTable((uint8_t*)&(pMsg->IssuerEvtID)) != 0xff))
    return gZclDuplicateExists_c; 
  
  /* Check event with the End Time in the past (Event Status set to 0xFB) */
  msgDuration = OTA2Native16(pMsg->DurationInMinutes);
  msgStartTime = OTA2Native32(pMsg->StartTime);
  
 /* here get the currentTime  */ 
  currentTime = ZCL_GetUTCTime(pAddrInfo->srcEndPoint);
  /*Event is started now???*/
  if(msgStartTime == 0x00000000)
  {
    /*Set the start time for the new event(keep the current time) */
    msgStartTime = currentTime;
    /*set it OTA value */
    OTATime = Native2OTA32(currentTime);
    pMsg->StartTime = OTATime;
  }
  msgStopTime = msgStartTime + ((uint32_t)msgDuration * 60);
  /* Event had expired?? */
  if (msgStopTime <= currentTime)
  {
    status = ZCL_SendReportEvtStatus(pAddrInfo, pMsg, gSELCDR_LdCtrlEvtCode_EvtHadExpired_c, FALSE);
    return status; 
  }
  
  /* Check the overlapping rules for scheduled and not executing events. */
  /* After the Report Event Status command is successfully sent, the End Device can remove the
  previous event schedule.(  Event can be received??...Can be added in the Events Table??... table is not full??).
  No RES will be send if table is full )
  */
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  {  
    pEntry = &gaEventsTable[index];
    /* if not used go to next entry*/ 
    if(pEntry->EntryStatus == gEntryNotUsed_c)
      continue;
    /* point to entry */
    pEvent = (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)&pEntry->cmdFrame;
    /* check if the class is overlaping */
    entryClass = pEvent->DevGroupClass; 
    if (!(entryClass & pMsg->DevGroupClass))
      continue; 
    /* get the entry timing */
    entryDuration = OTA2Native16(pEvent->DurationInMinutes);
    entryStartTime = OTA2Native32(pEvent->StartTime);
    entryStopTime = entryStartTime + ((uint32_t)entryDuration * 60);
    
    /*Check if the new event and the entry event are overlaping */
    if ((msgStartTime >= entryStopTime)||(entryStartTime >= msgStopTime))
      continue;
    
    /* Here the entry of the table is Overlapped */
    /* overlap only the specified class */
    /* supersede the original event strictly for that device class */
    entryClass = entryClass & (entryClass^pMsg->DevGroupClass);
    /*if NotSupersededClass is 0x0000, all the class bits was overlapped (event will be superseded)*/
    pEntry->NotSupersededClass = entryClass;
    
    /* resolve the case when the running event is superseded with a event that will start later*/
    if((entryStartTime <= currentTime) && (msgStartTime > currentTime))
      OTATime = Native2OTA32(msgStartTime);
    else
      OTATime = 0x00000000;
    pEntry->SupersededTime =OTATime;
    
  } /* end for(index = 0; index...*/
  
  /* Find an empty entry to schedule it */  
  mNewEntryIndex = FindEmptyEntryInEventsTable();
  if(mNewEntryIndex != 0xff)
  { /* keep the new event info */
    FLib_MemCpy(&mNewLDCtrlEvt, pMsg, sizeof(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t));
    FLib_MemCpy(&mNewAddrInfo,pAddrInfo, sizeof(mNewAddrInfo));
    /* Send RES with Receiving Status, but the event is not added in the Events Table yet */
    status = ZCL_SendReportEvtStatus(&mNewAddrInfo, &mNewLDCtrlEvt, gSELCDR_LdCtrlEvtCode_CmdRcvd_c, FALSE);
  }
  else	
    return gZclFailure_c; /* event can't be load (or added in table) */
  
  
  if(status == gZclSuccess_c)
    ZclSE_SendClusterEvt(gZclEvtHandleLdCtrl_c);  
  return status;
}

/******************************************************************************/
/* Process the Cancel Load Control Event command                                                            */ 
/*****************************************************************************/
static zbStatus_t ZCL_ProcessCancelLdCtrlEvt(zbApsdeDataIndication_t *pIndication)
{

  uint8_t status = gZclSuccess_c;
  zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_t *pMsg;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  uint8_t index;
  afAddrInfo_t addrInfo;
  uint32_t msgCancelTime, entryStopTime, currentTime;
  uint16_t duration;
  uint8_t utilityGroup;
  uint8_t attrLen;

  /* get address ready for reply */
  AF_PrepareForReply(&addrInfo, pIndication); 
  pMsg = (zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_t *)((uint8_t*)pIndication->pAsdu +sizeof(zclFrame_t));
  (void)ZCL_GetAttribute(addrInfo.srcEndPoint,addrInfo.aClusterId, gZclAttrDRLCClientUtilityGroup_c, gZclClientAttr_c, &utilityGroup, &attrLen);

  if((pMsg->UtilityGroup != utilityGroup) && (pMsg->UtilityGroup != 0))
  {
    /* send RES (InvalidCancelCmdDefault) with invalid values*/
    status = ZCL_SendReportEvtStatus(&addrInfo,(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)pMsg, gSELCDR_LdCtrlEvtCode_EvtInvalidCancelCmdDefault_c, TRUE);
    return status; 
  }
  /* Find the entry table event */
  index = FindEventInEventsTable(pMsg->IssuerEvtID);
  if(index == 0xff)
  {
    /* send RES (undef status ) with invalid values*/
    status = ZCL_SendReportEvtStatus(&addrInfo,(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)pMsg, gSELCDR_LdCtrlEvtCode_EvtUndef_c, TRUE);
    return status; 
  }
  else
  {
  pEntry = &gaEventsTable[index];
  pMsg->DevGroupClass &= pEntry->cmdFrame.DevGroupClass;
  }
  
  /* class is not matching; take NO action */
  if(!pMsg->DevGroupClass)
    return status;
  
  msgCancelTime = OTA2Native32(pMsg->EffectiveTime);
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime(addrInfo.srcEndPoint);
  
  /*Event is Canceled Now???*/
  if(msgCancelTime == 0x00000000)
  {
    msgCancelTime = currentTime;
    currentTime = Native2OTA32(currentTime);
    pMsg->EffectiveTime = currentTime;
  }
  
  entryStopTime = OTA2Native32(pEntry->cmdFrame.StartTime);
  duration = OTA2Native16(pEntry->cmdFrame.DurationInMinutes);
  entryStopTime = entryStopTime + ((uint32_t)duration * 60);
  
  if(entryStopTime <= msgCancelTime)
  {
    /* send RES (invalid effective time) with invalid values */
    status = ZCL_SendReportEvtStatus(&addrInfo,(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)pMsg, gSELCDR_LdCtrlEvtCode_EvtInvalidEffectiveTime_c, TRUE);
	return status; 
  }
  /* Set the fields information and let the ZCL_HandleScheduledEventNow() to  handle it */
  pEntry->CancelCtrl = pMsg->CancelCtrl; /* Set the Cancel Control */
  pEntry->CancelClass = pMsg->DevGroupClass; /*for what class the event is canceled*/  
  pEntry->CancelTime = pMsg->EffectiveTime; /* set cancel time */
  
  if (status == gZclSuccess_c)
       ZclSE_SendClusterEvt(gZclEvtHandleLdCtrl_c);  
  
  return status;
  
}



/******************************************************************************/
/* Process the Cancel All Load Control Event command...                                */ 
/*****************************************************************************/
static zbStatus_t ZCL_ProcessCancelAllLdCtrlEvtst(zbApsdeDataIndication_t * pIndication)
{
  
  uint8_t status;
  zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *pMsg;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  uint8_t index;
  
  pMsg = (zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *)((uint8_t*)pIndication->pAsdu +sizeof(zclFrame_t));
  status = gZclFailure_c;
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  {  
    pEntry = &gaEventsTable[index];
    /* if not used go to next entry*/  
    if(pEntry->EntryStatus == gEntryUsed_c)
    {
      pEntry->CancelCtrl = pMsg->CancelCtrl; 
      pEntry->CancelClass = 0xffff; /*invalid... not used */ 
      pEntry->CancelTime = 0x00000000; /* cancel now */
      status = gZclSuccess_c; 
    }
  }
  
  if (status == gZclSuccess_c)
        ZclSE_SendClusterEvt(gZclEvtHandleLdCtrl_c);  
  
  return status;

}

/******************************************************************************/
/*  Timer Callback that handles the scheduled Events */ 
static void LdCtrlEvtTimerCallback(zbTmrTimerID_t timerID)
{
   (void) timerID;
   ZclSE_SendClusterEvt(gZclEvtHandleLdCtrl_c);  
}


/******************************************************************************/
/* LdCtrlJitterTimerCallBack()     */ 
/*****************************************************************************/
static void LdCtrlJitterTimerCallBack(zbTmrTimerID_t timerID)
{
  zclLdCtrl_EventsTableEntry_t *pEntry;
  uint8_t status, previousEntryStatus;
  
  pEntry = (zclLdCtrl_EventsTableEntry_t*)&gaEventsTable[mReportStatusEntryIndex];
  if (pEntry->EntryStatus == gEntryNotUsed_c)
    return;
  
  /*save the previuos status */
  previousEntryStatus = pEntry->CurrentStatus; 
  
  /* Check if the event is already started and an "OptOUT" status was sent */
  if((previousEntryStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptOut_c)&&
     ((pEntry->NextStatus == gSELCDR_LdCtrlEvtCode_Completed_c)||
      (pEntry->NextStatus == gSELCDR_LdCtrlEvtCode_EvtCompletedWithNoUser_c)) )
    pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptOut_c;
  /* check if an "OptIn" status was sent previous
  and the event was started or already had been started */
  if((previousEntryStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptIn_c)&&
     (pEntry->NextStatus == gSELCDR_LdCtrlEvtCode_Completed_c))
    pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptIn_c;
  
  /*check if the event was started and before it was sent an "OptOut" status (to set the next status EvtCompletedWithNoUser) */
  if((previousEntryStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptOut_c)&&
     (pEntry->NextStatus == gSELCDR_LdCtrlEvtCode_Started_c))
    pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtCompletedWithNoUser_c;
  
  status = ZCL_SendReportEvtStatus((afAddrInfo_t *)pEntry, &pEntry->cmdFrame, pEntry->NextStatus, FALSE);
  if (status == gZclSuccess_c)
  {
    /* set the current status (the Report Evt Status was sent)*/
    pEntry->CurrentStatus = pEntry->NextStatus;
    
    /* Call the BeeAppUpdateDevice */
    /* user should check the CurrentStatus of the Event */
    BeeAppUpdateDevice(((afAddrInfo_t *)pEntry)->srcEndPoint, gZclUI_LdCtrlEvt_c, 0, ((afAddrInfo_t *)pEntry)->aClusterId, pEntry); 
    
    if(pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_Started_c)
      pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_Completed_c;
    else
      if((pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_Completed_c)||
         (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_EvtCompletedWithNoUser_c)||
           (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptIn_c)||
             (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptOut_c)||
               (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_EvtCancelled_c)||
                 (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_EvtSuperseded_c) )
        pEntry->EntryStatus = gEntryNotUsed_c;
    
    /* check if an "OptIn" status was sent previous
    and the event was started or already had been started */
    if((previousEntryStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptIn_c)&&
       (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_Started_c) )
      pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptIn_c;
    
  }/* end  if (status == gZclSuccess_c) */
  else
  {
    /* Try again later */
    ZbTMR_StartSingleShotTimer (timerID, 10, LdCtrlJitterTimerCallBack);
    return;
  }
  /* Handle the next Load Control Event*/             
  ZclSE_SendClusterEvt(gZclEvtHandleLdCtrl_c); 
}

/******************************************************************************/
/* Find an Event in the Events Table...                                                                                   */
static uint8_t FindEventInEventsTable(uint8_t *pEvtId)
{
  uint8_t index; 
  zclLdCtrl_EventsTableEntry_t *pEntry = &gaEventsTable[0];
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  {  
    /* if not used go to next entry*/  
    if(((zclLdCtrl_EventsTableEntry_t *)(pEntry+index))->EntryStatus == gEntryUsed_c)
    {
      if(FLib_MemCmp(((zclLdCtrl_EventsTableEntry_t *)(pEntry+index))->cmdFrame.IssuerEvtID, pEvtId, sizeof(SEEvtId_t)))
        return index; 
    }
  }
  return 0xFF; 
  
}   

/******************************************************************************/
/* An Entry is empty if the EntryStatus is NOT Used or NotSupersededClass is 0x0000 (all class bits was overlapped)*/
static uint8_t FindEmptyEntryInEventsTable(void)
{
  uint8_t index;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  {  
    pEntry = &gaEventsTable[index];
    /* if not used go to next entry*/  
    if(pEntry->EntryStatus == gEntryNotUsed_c)
      return index;
    else /*if class is superseded, use that entry*/
      if (pEntry->NotSupersededClass == 0x0000 &&
          pEntry->SupersededTime == 0x00000000 )
        return index;
  }
        
  /*if all entries are used and the class bits wasn't overlaped, we have NO free entry in table*/
  /* keep the same entries */
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  { 
    pEntry = &gaEventsTable[index];
    if(pEntry->EntryStatus == gEntryUsed_c)
    {
      pEntry->NotSupersededClass = 0xffff;
      pEntry->SupersededTime = 0xffffffff;
    }
  }
  return 0xff;
}  

/******************************************************************************/
/* An Entry is empty if the EntryStatus is NOT Used*/
static uint8_t FindEmptyEntryInServerEventsTable(void)
{
  uint8_t index;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  for(index = 0; index < gNumOfServerEventsTableEntry_c; index++)
  {  
    pEntry = &gaServerEventsTable[index];
    /* if not used go to next entry*/  
    if(pEntry->EntryStatus == gEntryNotUsed_c)
      return index;
   }
  return 0xff;
}  

/******************************************************************************/
/* Add a new Event in the EventsTable */
static void AddNewEntry
(
zclLdCtrl_EventsTableEntry_t *pDst, 
zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pSrc,
afAddrInfo_t *pAddrInfo
)
{
  FLib_MemCpy(&(pDst->addrInfo), pAddrInfo, sizeof(afAddrInfo_t));
  FLib_MemCpy(&(pDst->cmdFrame), pSrc, sizeof(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t));
  pDst->CurrentStatus = gSELCDR_LdCtrlEvtCode_CmdRcvd_c;
  pDst->NextStatus = gSELCDR_LdCtrlEvtCode_Started_c;
  pDst->IsSuccessiveEvent = FALSE;
  /*Set  fields as beeing invalid... not used yet*/
  BeeUtilSetToF(&(pDst->CancelTime), (2*sizeof(ZCLTime_t) + 2*sizeof(LCDRDevCls_t) +1));
  pDst->EntryStatus = gEntryUsed_c;
}

/******************************************************************************/
static void CheckForSuccessiveEvents(uint8_t msgIndex)
{
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pEvent;
  zclLdCtrl_EventsTableEntry_t *pEntry,*pMsg;
  uint32_t msgStartTime, entryStartTime ;
  uint32_t msgStopTime, entryStopTime;
  uint16_t msgDuration, entryDuration; 
  uint8_t index;
  /* point to message */
  pMsg = &gaEventsTable[msgIndex];
  msgDuration = OTA2Native16(pMsg->cmdFrame.DurationInMinutes);
  msgStartTime = OTA2Native32(pMsg->cmdFrame.StartTime);
  msgStopTime = msgStartTime + ((uint32_t)msgDuration * 60);
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  {  
    pEntry = &gaEventsTable[index];
    /* if not used go to next entry*/  
    if((pEntry->EntryStatus == gEntryNotUsed_c)||(index == msgIndex) )
      continue;
    /* point to entry */
    pEvent = (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)&pEntry->cmdFrame;
    /* check if the class is overlaping */
    if (!(pEvent->DevGroupClass & pMsg->cmdFrame.DevGroupClass))
      continue;	
    /* get the entry timing */
    entryDuration = OTA2Native16(pEvent->DurationInMinutes);
    entryStartTime = OTA2Native32(pEvent->StartTime);
    entryStopTime = entryStartTime + ((uint32_t)entryDuration * 60);
    /*Check if the new event is a succesive event */
    if ((msgStartTime == entryStopTime)||(entryStartTime == msgStopTime))
    {
      if(!pEntry->IsSuccessiveEvent)
        pEntry->IsSuccessiveEvent = TRUE;
      if(!pMsg->IsSuccessiveEvent)
        pMsg->IsSuccessiveEvent = TRUE;
    } 
  }/* end for(...) */
  
}

static zbStatus_t ZCL_ProcessGetScheduledEvtsReq
(
addrInfoType_t *pAddrInfo, 
zclCmdDmndRspLdCtrl_GetScheduledEvts_t * pGetScheduledEvts
)
{
  FLib_MemCpy(&mAddrInfo, pAddrInfo, sizeof(addrInfoType_t));

  mGetLdCtlEvtStartTime = OTA2Native32(pGetScheduledEvts->EvtStartTime);
  mGetNumOfLdCtlEvts = pGetScheduledEvts->NumOfEvts;
  if (!mGetNumOfLdCtlEvts)
  {
    mGetNumOfLdCtlEvts = gNumOfServerEventsTableEntry_c;
    mGetLdCtlEvtStartTime = 0x00000000; /*all information*/
  }
  mIndexLdCtl = 0;
  ZclSE_SendClusterEvt(gzclEvtHandleGetScheduledLdCtlEvts_c);
  return gZclSuccess_c;
}

/******************************************************************************/
/* The LC Event command is generated in response to receiving a Get Scheduled Events
command */
static zbStatus_t ZCL_SendLdCtlEvt(addrInfoType_t *pAddrInfo,zclLdCtrl_EventsTableEntry_t * pMsg) 
{
  zclDmndRspLdCtrl_LdCtrlEvtReq_t req;
  
  FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  req.zclTransactionId =  ZbZclFoundationGlobals(gZclTransactionId)++;
  FLib_MemCpy(&req.cmdFrame, (uint8_t *)pMsg + MbrOfs(zclLdCtrl_EventsTableEntry_t, cmdFrame), sizeof(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t));
  return ZclDmndRspLdCtrl_LdCtrlEvtReq(&req);
}
