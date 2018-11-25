/*! @file	ZclSECalendar.c
 *
 * @brief	This source file describes specific functionality implemented
 *			for the TOU Calendar cluster.
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
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"

#include "SEProfile.h"
#include "ZclSe.h"
#include "ZclSECalendar.h"
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
#if gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d     
static zbStatus_t zclTouCalendar_ProcessGetCalendar(afAddrInfo_t addrInfo, uint8_t  transactionId, zclCmdTouCalendar_GetCalendar_t  cmdPayload);
static zbStatus_t zclTouCalendar_ProcessGetCommand(afAddrInfo_t addrInfo, uint8_t transactionId, zclTouCalendar_GetInfCommands_t  cmdPayload, uint8_t commandId);
static zbStatus_t zclTouCalendar_ProcessGetSpecialDays(afAddrInfo_t addrInfo, uint8_t transactionId, zclCmdTouCalendar_GetSpecialDays_t cmdPayload);
static zbStatus_t zclTouCalendar_ProcessPublishCalendar(zclCmdTouCalendar_PublishCalendar_t  *pMsg);
static zbStatus_t zclTouCalendar_ProcessPublishDayProfile(zclCmdTouCalendar_PublishDayProfile_t *pMsg, uint8_t payloadLen);  
static zbStatus_t zclTouCalendar_ProcessPublishWeekProfile(zclCmdTouCalendar_PublishWeekProfile_t *pMsg);
static zbStatus_t zclTouCalendar_ProcessPublishSeasons(zclCmdTouCalendar_PublishSeasons_t *pMsg, uint8_t payloadLen);    
static zbStatus_t zclTouCalendar_ProcessPublishSpecialDays(zclCmdTouCalendar_PublishSpecialDays_t  *pMsg, uint8_t payloadLen);
static uint8_t zclTouCalendar_FindNextCalendarEvent(void);
static uint8_t zclTouCalendar_FindNextSpecialDayEvent(void);
static uint8_t zclTouCalendar_FindIssuerCalendarId(uint32_t issuerCalendarId, zclTouCalendar_ActivityCalendarInfo_t *pCalendarInfo);
static uint8_t zclTouCalendar_FindIssuerEventId(uint32_t issuerEventId, zclTouCalendar_SpecialDayInfo_t *pSpecialDayInfo);
static void zclTouCalendar_HandleGetCalendar(void);
static void zclTouCalendar_HandleGetDayProfiles(void);
static void zclTouCalendar_HandleGetWeekProfiles(void);
static void zclTouCalendar_HandleGetSeasons(void);
static void zclTouCalendar_HandleGetSpecialDays(void);
#endif
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

#if gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d   

/* Client and Server shall be able to store at least two instances of
*   the calendar, the current and the next one                      */

/* TOU Calendar data structures - Server */
zclTouCalendar_ActivityCalendarInfo_t    gServerActivityCalendarInfo[gTouCalendar_NoOfInstances_c];
zclTouCalendar_SpecialDayInfo_t          gServerSpecialDayInfo[gTouCalendar_NoOfInstances_c];

/* TOU Calendar data structures - Client */
zclTouCalendar_ActivityCalendarInfo_t    gClientActivityCalendarInfo[gTouCalendar_NoOfInstances_c];
zclTouCalendar_SpecialDayInfo_t          gClientSpecialDayInfo[gTouCalendar_NoOfInstances_c];

static zclTouCalendar_GetCalendarInf_t          mServerGetCalendarInf;
static zclTouCalendar_GetDayProfilesInf_t       mServerGetDayProfilesInf;
static zclTouCalendar_GetWeekProfilesInf_t      mServerGetWeekProfilesInf;
static zclTouCalendar_GetSeasonsInf_t           mServerGetSeasonsInf;
static zclTouCalendar_GetSpecialDayInf_t        mServerGetSpeciaDayInf;
#endif

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
#if gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d 
/*Used to keep the new message(event)*/
static uint32_t mTouCalendarClusterEvt = 0x00;
#endif


/*!
 * @fn 		zbStatus_t ZCL_TouCalendarClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the TOU Calendar Cluster server. 
 *
 */
zbStatus_t ZCL_TouCalendarClusterServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo;
  zbStatus_t status = gZbSuccess_c;
  /* to prevent compiler warning */
  (void)pDevice;
  
   pFrame = (void *)pIndication->pAsdu;
  /*Create the destination address*/
  AF_PrepareForReply(&addrInfo, pIndication); 

   switch (pFrame->command) 
   {
#if gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d     
      case gZclCmdTouCalendar_GetCalendar_c:
        {
          zclCmdTouCalendar_GetCalendar_t cmdPayload;
          FLib_MemCpy(&cmdPayload,(pFrame + 1),  pIndication->asduLength - sizeof(zclFrame_t));
          /* check if the calendar type field is present in command payload*/
          if(pIndication->asduLength - sizeof(zclFrame_t) == sizeof(zclCmdTouCalendar_GetCalendar_t)-1)
            cmdPayload.calendarType = gTouCalendar_Type_NotSpecified_c;
          status = zclTouCalendar_ProcessGetCalendar(addrInfo, pFrame->transactionId, cmdPayload);
          break;
        }
      case gZclCmdTouCalendar_GetDayProfiles_c:
      case gZclCmdTouCalendar_GetWeekProfiles_c:
      case gZclCmdTouCalendar_GetSeasons_c:
        {
          zclTouCalendar_GetInfCommands_t cmdPayload = *(zclTouCalendar_GetInfCommands_t *)(pFrame+1);
          status = zclTouCalendar_ProcessGetCommand(addrInfo, pFrame->transactionId, cmdPayload, pFrame->command);
          break;
        }
      case gZclCmdTouCalendar_GetSpecialDays_c:
        {
          zclCmdTouCalendar_GetSpecialDays_t cmdPayload;
          FLib_MemCpy(&cmdPayload,(pFrame + 1),  pIndication->asduLength - sizeof(zclFrame_t));
          /* check if the calendar type field is present in command payload*/
          if(pIndication->asduLength - sizeof(zclFrame_t) == sizeof(zclCmdTouCalendar_GetSpecialDays_t)-1)
            cmdPayload.calendarType = gTouCalendar_Type_NotSpecified_c;
          status = zclTouCalendar_ProcessGetSpecialDays(addrInfo, pFrame->transactionId, cmdPayload);
          break;
        }
#endif /* gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d   */       
      default:
        status = gZclUnsupportedClusterCommand_c;    
  }
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_TouCalendarClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the TOU Calendar Cluster client. 
 *
 */
zbStatus_t ZCL_TouCalendarClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo;
  zbStatus_t status = gZbSuccess_c;
 
  pFrame = (void *)pIndication->pAsdu;
  /*Create the destination address*/
  AF_PrepareForReply(&addrInfo, pIndication); 
  
  /* prevent compiler warning */
  (void)pDevice;
  
  pFrame = (void *)pIndication->pAsdu;
  /* handle the command */
  switch(pFrame->command){
#if gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d       
     case gZclCmdTouCalendar_PublishCalendar_c:
       {
         status =  zclTouCalendar_ProcessPublishCalendar((zclCmdTouCalendar_PublishCalendar_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
         break;
       }
     case gZclCmdTouCalendar_PublishDayProfile_c:
       {
         status =  zclTouCalendar_ProcessPublishDayProfile((zclCmdTouCalendar_PublishDayProfile_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))), pIndication->asduLength - sizeof(zclFrame_t));
         break;
       }
     case gZclCmdTouCalendar_PublishWeekProfile_c:
       {
         status =  zclTouCalendar_ProcessPublishWeekProfile((zclCmdTouCalendar_PublishWeekProfile_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
         break;
       }
     case gZclCmdTouCalendar_PublishSeasons_c:  
       {
         status =  zclTouCalendar_ProcessPublishSeasons((zclCmdTouCalendar_PublishSeasons_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))), pIndication->asduLength - sizeof(zclFrame_t));
         break;
       }
     case gZclCmdTouCalendar_PublishSpecialDays_c:  
       {
         status =  zclTouCalendar_ProcessPublishSpecialDays((zclCmdTouCalendar_PublishSpecialDays_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))), pIndication->asduLength - sizeof(zclFrame_t));
         break;
       }
#endif /* gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d  */       
     default:
        status = gZclUnsupportedClusterCommand_c;    
  }
  return status;
}

#if gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d   
/*!
 * @fn 		zbStatus_t zclTouCalendar_GetCalendar(zclTouCalendar_GetCalendar_t *pReq) 
 *
 * @brief	Sends over-the-air a Get Calendar frame from the TOU Calendar client. 
 *
 */
zbStatus_t zclTouCalendar_GetCalendar
( 
    zclTouCalendar_GetCalendar_t *pReq
) 
{
    uint8_t len = 0;
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterTouCalendar_c);
    len = (gTouCalendar_CalendarTypePresent_c == TRUE)?sizeof(zclCmdTouCalendar_GetCalendar_t):sizeof(zclCmdTouCalendar_GetCalendar_t)-1;
    return ZCL_SendClientReqSeqPassed(gZclCmdTouCalendar_GetCalendar_c, len, (zclGenericReq_t *)pReq);
}

/*!***************************************************************************
*  TOU Calendar - ProcessGetCalendar       
*****************************************************************************/
static zbStatus_t zclTouCalendar_ProcessGetCalendar
(
  afAddrInfo_t                         addrInfo, 
  uint8_t                              transactionId,
  zclCmdTouCalendar_GetCalendar_t      cmdPayload
 )
{ 
  uint8_t entryIndex;
  
  /* Copy the calendar information*/
  FLib_MemCpy(&mServerGetCalendarInf.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
  mServerGetCalendarInf.zclTransactionId = transactionId;
  mServerGetCalendarInf.calendarType = cmdPayload.calendarType;
  mServerGetCalendarInf.noOfCalendarsRemaining = cmdPayload.numberOfCalendars;
  mServerGetCalendarInf.index.cmdIndex = 0x00;
  mServerGetCalendarInf.index.TotalNumberOfCmds = 0x00;
  mServerGetCalendarInf.issuerCalendarId = cmdPayload.issuerCalendarId;
    
  /* Check valid value for IssuerCalendarId */
  entryIndex = zclTouCalendar_FindIssuerCalendarId(mServerGetCalendarInf.issuerCalendarId, gServerActivityCalendarInfo);
  if((entryIndex == gTouCalendar_NoOfInstances_c) &&  (mServerGetCalendarInf.issuerCalendarId!=0x00))  
  {
	  return gZclNotFound_c;
  }
	  
  /* Check valid value for Provider ID */
  if ( OTA2Native32(cmdPayload.providerId) != gServerActivityCalendarInfo[entryIndex].providerId)
  {
    return gZclNotAuthorized_c;
  }  
  
  /* Check valid value for calendar type*/
  if (( mServerGetCalendarInf.calendarType > gTouCalendar_CalendarType_AuxillaryLoadSwitchCalendar_c) &&
      ( mServerGetCalendarInf.calendarType != gTouCalendar_Type_NotSpecified_c))
  {
    return gZclFailure_c;
  }
  
  /* Get the start time */
  cmdPayload.startTime = OTA2Native32(cmdPayload.startTime);
  
  /* If start time is 0x00000000 replace with current start time */
  mServerGetCalendarInf.startTime = (cmdPayload.startTime == 0x00000000)?ZCL_GetUTCTime(addrInfo.srcEndPoint):cmdPayload.startTime;
  
  /* If number of calendars is 0 we will return all valid entries*/
  if(mServerGetCalendarInf.noOfCalendarsRemaining == 0)
  {
    mServerGetCalendarInf.noOfCalendarsRemaining = gTouCalendar_NoOfInstances_c;
  }
   
      
  /* Find next valid entry */
  entryIndex = zclTouCalendar_FindNextCalendarEvent();
  if(entryIndex != gTouCalendar_NoOfInstances_c)
  {
    mServerGetCalendarInf.index.cmdIndex = entryIndex;
    SendTouCalendarClusterEvt(gzclEvtHandleTouCalendar_GetCalendar_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}

/*!
 * @fn 		zbStatus_t zclTouCalendar_PublishCalendar(zclTouCalendar_PublishCalendar_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air a Publish Calendar frame from the TOU Calendar server. 
 *
 */
zbStatus_t zclTouCalendar_PublishCalendar
( 
  zclTouCalendar_PublishCalendar_t *pCommandRsp
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = sizeof(zclCmdTouCalendar_PublishCalendar_t)-sizeof(zclStr12Oct_t)+pCommandRsp->cmdFrame.calendarName.length + 1;
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          gZclCmdTouCalendar_PublishCalendar_c,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &pCommandRsp->zclTransactionId, 
                          &iPayloadLen,
                          (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
 
 return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}


/*****************************************
*  TOU Calendar - ProcessPublishCalendar */
static zbStatus_t zclTouCalendar_ProcessPublishCalendar
(
  zclCmdTouCalendar_PublishCalendar_t      *pMsg
 )
{ 
  uint8_t i = 0, calendarIndex = 0;
  ZCLTime_t duration;
  ZCLTime_t startTime = OTA2Native32(pMsg->startTime);
  if(gTouCalendar_NoOfInstances_c == 0)
    return gZclInsufficientSpace_c;
 
  /* Search for an existing entry */
  calendarIndex = zclTouCalendar_FindIssuerCalendarId(OTA2Native32(pMsg->issuerCalendarId), gClientActivityCalendarInfo);
  if(calendarIndex == gTouCalendar_NoOfInstances_c)  
  {
    calendarIndex = 0;
    for(i = 0; i < gTouCalendar_NoOfInstances_c-1; i++)
    {
        /* check the entry status */
        if(gClientActivityCalendarInfo[i].entryStatus == gEntryNotUsed_c)
        {
          calendarIndex = i;
          break;
        }
        
        /* get calendar duration */
        if(gClientActivityCalendarInfo[i+1].entryStatus == gEntryNotUsed_c)
          duration = 0xFFFFFFFF;
        else
          duration = gClientActivityCalendarInfo[i+1].calendar.startTime - gClientActivityCalendarInfo[i].calendar.startTime;
        
        /* check overlapping callendars */
        if((duration!= 0xFFFFFFFF)&&
           (gClientActivityCalendarInfo[i].calendar.startTime + duration > startTime))
        {
          /* overlaping */
          calendarIndex = i;
          break;
        }
        if(i == gTouCalendar_NoOfInstances_c-2)
           calendarIndex = gTouCalendar_NoOfInstances_c-1;
    }
  }
  
  /* store the calendar information */
  gClientActivityCalendarInfo[calendarIndex].entryStatus = gEntryUsed_c;
  gClientActivityCalendarInfo[calendarIndex].calendar.issuerId = OTA2Native32(pMsg->issuerCalendarId);
  gClientActivityCalendarInfo[calendarIndex].calendar.startTime = startTime;
  gClientActivityCalendarInfo[calendarIndex].calendar.type = pMsg->calendarType;

  gClientActivityCalendarInfo[calendarIndex].calendar.noOfSeasonEntries = pMsg->numberOfSeasons;
  gClientActivityCalendarInfo[calendarIndex].calendar.noOfDayProfileEntries = pMsg->numberOfDayProfiles;
  gClientActivityCalendarInfo[calendarIndex].calendar.noOfWeekProfileEntries = pMsg->numberOfWeekProfiles;
  FLib_MemCpy(&gClientActivityCalendarInfo[calendarIndex].calendar.name, &pMsg->calendarName,  pMsg->calendarName.length+1);  
         
  return gZclSuccess_c;
}

/*!
 * @fn 		zbStatus_t zclTouCalendar_GetDayProfiles(zclTouCalendar_GetDayProfiles_t *pReq) 
 *
 * @brief	Sends over-the-air a Get Day Profiles frame from the TOU Calendar client. 
 *
 */
zbStatus_t zclTouCalendar_GetDayProfiles
( 
    zclTouCalendar_GetDayProfiles_t *pReq
) 
{
    uint8_t len = 0;
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterTouCalendar_c);	
    len = sizeof(zclCmdTouCalendar_GetDayProfiles_t);
    return ZCL_SendClientReqSeqPassed(gZclCmdTouCalendar_GetDayProfiles_c, len, (zclGenericReq_t *)pReq);
}


/************************************************************************
*  TOU Calendar - ProcessGetCommand - DayProfiles, WeekProfiles, Seasons*/
static zbStatus_t zclTouCalendar_ProcessGetCommand
(
  afAddrInfo_t                         addrInfo, 
  uint8_t                              transactionId,
  zclTouCalendar_GetInfCommands_t      cmdPayload,
  uint8_t                              commandId
 )
{
    uint8_t index;
    uint8_t event;
       
    switch(commandId)
    {
      case gZclCmdTouCalendar_GetDayProfiles_c:
        {            	
          cmdPayload.getDay.issuerCalendarId = OTA2Native32(cmdPayload.getDay.issuerCalendarId);
          index = zclTouCalendar_FindIssuerCalendarId(cmdPayload.getDay.issuerCalendarId, gServerActivityCalendarInfo);

          if((index == gTouCalendar_NoOfInstances_c)||(gServerActivityCalendarInfo[index].calendar.noOfDayProfileEntries == 0))
              return gZclNotFound_c;        
          
          /* Check valid value for Provider ID */
          if ( OTA2Native32(OTA2Native16(cmdPayload.getDay.providerId)) != gServerActivityCalendarInfo[index].providerId)
          {
            return gZclNotAuthorized_c;
          }          
          
            /* set the day profiles information */
          FLib_MemCpy(&mServerGetDayProfilesInf.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
          mServerGetDayProfilesInf.zclTransactionId = transactionId;
          mServerGetDayProfilesInf.issuerCalendarId = Native2OTA32(cmdPayload.getDay.issuerCalendarId);
          mServerGetDayProfilesInf.NumberOfDays = cmdPayload.getDay.numberOfDays;
          if((mServerGetDayProfilesInf.NumberOfDays == 0x00)||
        		  (mServerGetDayProfilesInf.NumberOfDays > gServerActivityCalendarInfo[index].calendar.noOfDayProfileEntries))
        	  mServerGetDayProfilesInf.NumberOfDays = gServerActivityCalendarInfo[index].calendar.noOfDayProfileEntries;
          
          mServerGetDayProfilesInf.calendarIndex = index;
          mServerGetDayProfilesInf.dayIndex = cmdPayload.getDay.dayId;
          mServerGetDayProfilesInf.commandIndex.cmdIndex = 0;
          event = gzclEvtHandleTouCalendar_GetDayProfiles_c;
          break;
        }
      case gZclCmdTouCalendar_GetWeekProfiles_c:
        {
          /* Check valid value for Provider ID */	
          cmdPayload.getWeek.issuerCalendarId = OTA2Native32(cmdPayload.getWeek.issuerCalendarId);
          index = zclTouCalendar_FindIssuerCalendarId(cmdPayload.getWeek.issuerCalendarId, gServerActivityCalendarInfo);

          if((index == gTouCalendar_NoOfInstances_c)||(gServerActivityCalendarInfo[index].calendar.noOfWeekProfileEntries == 0))
            return gZclNotFound_c;         

          if ( OTA2Native32(OTA2Native16(cmdPayload.getWeek.providerId)) != gServerActivityCalendarInfo[index].providerId)
          {
            return gZclNotAuthorized_c;
          }
                       
          
          /* set the week profiles information */
          FLib_MemCpy(&mServerGetWeekProfilesInf.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
          mServerGetWeekProfilesInf.zclTransactionId = transactionId;
          mServerGetWeekProfilesInf.issuerCalendarId = Native2OTA32(cmdPayload.getWeek.issuerCalendarId);
          
          mServerGetWeekProfilesInf.noOfweeks = cmdPayload.getWeek.numberOfWeeks;
          if(( mServerGetWeekProfilesInf.noOfweeks == 0x00)||
        		  ( mServerGetWeekProfilesInf.noOfweeks > gServerActivityCalendarInfo[index].calendar.noOfWeekProfileEntries))
        	  mServerGetWeekProfilesInf.noOfweeks = gServerActivityCalendarInfo[index].calendar.noOfWeekProfileEntries;
                    
          
          mServerGetWeekProfilesInf.calendarIndex = index;
          mServerGetWeekProfilesInf.weekIndex = cmdPayload.getWeek.weekId;
          event = gzclEvtHandleTouCalendar_GetWeekProfiles_c;
          break;
        }
      case  gZclCmdTouCalendar_GetSeasons_c:
      {
    	      	
          cmdPayload.getSeason.issuerCalendarId = OTA2Native32(cmdPayload.getSeason.issuerCalendarId);
          index = zclTouCalendar_FindIssuerCalendarId(cmdPayload.getSeason.issuerCalendarId, gServerActivityCalendarInfo);

          if((index == gTouCalendar_NoOfInstances_c)||(gServerActivityCalendarInfo[index].calendar.noOfSeasonEntries == 0))
            return gZclNotFound_c;  

          /* Check valid value for Provider ID */
          if ( OTA2Native32(OTA2Native16(cmdPayload.getWeek.providerId)) != gServerActivityCalendarInfo[index].providerId)
          {
            return gZclNotAuthorized_c;
          }          
          
          /* set the week profiles information */
          FLib_MemCpy(&mServerGetSeasonsInf.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
          mServerGetSeasonsInf.zclTransactionId = transactionId;
          mServerGetSeasonsInf.issuerCalendarId = Native2OTA32(cmdPayload.getSeason.issuerCalendarId);
          
          mServerGetSeasonsInf.calendarIndex = index;
          mServerGetSeasonsInf.commandIndex.cmdIndex = 0;
          event = gzclEvtHandleTouCalendar_GetSeasons_c;
          break;
      }
      default:
        return gZclUnsupportedClusterCommand_c;
    }
    /* send event to TS */  
    SendTouCalendarClusterEvt(event);
    return gZclSuccess_c;
}


/*!
 * @fn 		zbStatus_t zclTouCalendar_PublishDayProfile(zclTouCalendar_PublishDayProfile_t *pCommandRsp, uint8_t payloadLen) 
 *
 * @brief	Sends over-the-air a Publish Day Profile frame from the TOU Calendar server. 
 *
 */
zbStatus_t zclTouCalendar_PublishDayProfile
( 
  zclTouCalendar_PublishDayProfile_t *pCommandRsp,
  uint8_t payloadLen
) 
{
  afToApsdeMessage_t *pMsg;
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          gZclCmdTouCalendar_PublishDayProfile_c,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &pCommandRsp->zclTransactionId, 
                          &payloadLen,
                          (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  payloadLen, pMsg);  
}

/********************************************
*  TOU Calendar - ProcessPublishDayProfile */
static zbStatus_t zclTouCalendar_ProcessPublishDayProfile
(
  zclCmdTouCalendar_PublishDayProfile_t *pMsg,
  uint8_t                               payloadLen
)    
{
  uint8_t calendarIndex = 0, dayIndex = 0,  i=0;
  uint8_t maxScheduleEntries = 0, frameScheduleEntries = 0;
    
  /* check issuerCalendarId */
  calendarIndex = zclTouCalendar_FindIssuerCalendarId(OTA2Native32(pMsg->issuerCalendarId), gClientActivityCalendarInfo);;
  if(calendarIndex == gTouCalendar_NoOfInstances_c)  
    return gZclNotFound_c;
  /* check the number of schedule entries */
  if(pMsg->totalNoOfScheduleEntries > gTouCalendar_NoOfDayScheduleEntries_c)
    return gZclInsufficientSpace_c;
  
  maxScheduleEntries = (uint8_t)(payloadLen - 3) / sizeof(zclTouCalendar_DayScheduleEntries_t);

  if((pMsg->commandIndex.cmdIndex +1)*maxScheduleEntries >= pMsg->totalNoOfScheduleEntries)
    frameScheduleEntries = pMsg->totalNoOfScheduleEntries - pMsg->commandIndex.cmdIndex*maxScheduleEntries;
  else
    frameScheduleEntries = maxScheduleEntries;

  for(i=0; i<gClientActivityCalendarInfo[calendarIndex].calendar.noOfDayProfileEntries; i++)  
  {
    if((gClientActivityCalendarInfo[calendarIndex].calendar.dayProfileTable[i].dayId == pMsg->dayId)||
          (gClientActivityCalendarInfo[calendarIndex].calendar.dayProfileTable[i].noOfDayScheduleEntries == 0x00))
    {
      dayIndex = i;
      break;
    }
    if(i == gClientActivityCalendarInfo[calendarIndex].calendar.noOfDayProfileEntries-1)
      return gZclInsufficientSpace_c;
  }
  
  /* store day profile data */
  gClientActivityCalendarInfo[calendarIndex].calendar.dayProfileTable[dayIndex].dayId = pMsg->dayId;
  gClientActivityCalendarInfo[calendarIndex].calendar.dayProfileTable[dayIndex].noOfDayScheduleEntries+=frameScheduleEntries;
  FLib_MemCpy(&gClientActivityCalendarInfo[calendarIndex].calendar.dayProfileTable[dayIndex].dayScheduleEntry[maxScheduleEntries*pMsg->commandIndex.cmdIndex], 
                    pMsg->dayScheduleEntries, frameScheduleEntries);
  
  
  return gZclSuccess_c;
}

/*!
 * @fn 		zbStatus_t zclTouCalendar_GetWeekProfiles(zclTouCalendar_GetWeekProfiles_t *pReq) 
 *
 * @brief	Sends over-the-air a Get Week Profiles frame from the TOU Calendar client. 
 *
 */
zbStatus_t zclTouCalendar_GetWeekProfiles
( 
    zclTouCalendar_GetWeekProfiles_t *pReq
) 
{
    uint8_t len = 0;
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterTouCalendar_c);	
    len = sizeof(zclCmdTouCalendar_GetWeekProfiles_t);
    return ZCL_SendClientReqSeqPassed(gZclCmdTouCalendar_GetWeekProfiles_c, len, (zclGenericReq_t *)pReq);
}


/*!
 * @fn 		zbStatus_t zclTouCalendar_PublishWeekProfile(zclTouCalendar_PublishWeekProfile_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air a Publish Week Profile frame from the TOU Calendar server. 
 *
 */
zbStatus_t zclTouCalendar_PublishWeekProfile
( 
  zclTouCalendar_PublishWeekProfile_t *pCommandRsp
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = sizeof(zclCmdTouCalendar_PublishWeekProfile_t);
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          gZclCmdTouCalendar_PublishWeekProfile_c,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &pCommandRsp->zclTransactionId, 
                          &iPayloadLen,
                          (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
 
 return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/********************************************
*  TOU Calendar - ProcessPublishWeekProfile */
static zbStatus_t zclTouCalendar_ProcessPublishWeekProfile
(
  zclCmdTouCalendar_PublishWeekProfile_t *pMsg
)    
{
  uint8_t calendarIndex = 0, weekIndex = 0,  i=0;
  
  /* check issuerCalendarId */
  calendarIndex = zclTouCalendar_FindIssuerCalendarId(OTA2Native32(pMsg->issuerCalendarId), gClientActivityCalendarInfo);;
  if(calendarIndex == gTouCalendar_NoOfInstances_c)  
    return gZclNotFound_c;
  
  for(i=0; i<gClientActivityCalendarInfo[calendarIndex].calendar.noOfWeekProfileEntries; i++)  
  {
    if((gClientActivityCalendarInfo[calendarIndex].calendar.weekProfileTable[i].weekId == pMsg->weekProfile.weekId)||
          (gClientActivityCalendarInfo[calendarIndex].calendar.weekProfileTable[i].weekId == gTouCalendar_NotWeekProfileDefined_c))
    {
      weekIndex = i;
      break;
    }
    if(i == gClientActivityCalendarInfo[calendarIndex].calendar.noOfWeekProfileEntries-1)
      return gZclInsufficientSpace_c;
  }
  
  /* store week profile data */
  FLib_MemCpy(&gClientActivityCalendarInfo[calendarIndex].calendar.weekProfileTable[weekIndex], &pMsg->weekProfile, sizeof(zclTouCalendar_WeekProfile_t));
  
  return gZclSuccess_c;
}

/*!
 * @fn 		zbStatus_t zclTouCalendar_GetSeasons(zclTouCalendar_GetSeasons_t *pReq) 
 *
 * @brief	Sends over-the-air a Get Seasons frame from the TOU Calendar client. 
 *
 */
zbStatus_t zclTouCalendar_GetSeasons
( 
    zclTouCalendar_GetSeasons_t *pReq
) 
{
    uint8_t len = 0;
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterTouCalendar_c);	
    len = sizeof(zclCmdTouCalendar_GetSeasons_t);
    return ZCL_SendClientReqSeqPassed(gZclCmdTouCalendar_GetSeasons_c, len, (zclGenericReq_t *)pReq);
}


/*!
 * @fn 		zbStatus_t zclTouCalendar_PublishSeasons(zclTouCalendar_PublishSeasons_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air a Publish Seasons frame from the TOU Calendar server. 
 *
 */
zbStatus_t zclTouCalendar_PublishSeasons
( 
  zclTouCalendar_PublishSeasons_t *pCommandRsp
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = sizeof(zclCmdTouCalendar_PublishSeasons_t);
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          gZclCmdTouCalendar_PublishSeasons_c,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &pCommandRsp->zclTransactionId, 
                          &iPayloadLen,
                          (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
 
 return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*****************************************
*  TOU Calendar - ProcessPublishSeasons */                                         
static zbStatus_t zclTouCalendar_ProcessPublishSeasons
(
  zclCmdTouCalendar_PublishSeasons_t *pMsg,
  uint8_t payloadLen
)    
{
  uint8_t calendarIndex = 0;
  uint8_t maxSeasonEntries = 0;
      
  /* check issuerCalendarId */
  calendarIndex = zclTouCalendar_FindIssuerCalendarId(OTA2Native32(pMsg->issuerCalendarId), gClientActivityCalendarInfo);;
  if(calendarIndex == gTouCalendar_NoOfInstances_c)  
    return gZclNotFound_c;
 
  /* Find how much Season Entries can fit into the packet */
  maxSeasonEntries = (uint8_t)(payloadLen - sizeof(uint32_t) - 1) / sizeof(zclTouCalendar_SeasonEntry_t);
  
  /* store season data */
  FLib_MemCpy(&gClientActivityCalendarInfo[calendarIndex].calendar.weekProfileTable[maxSeasonEntries*pMsg->commandIndex.cmdIndex], 
                    pMsg->seasonEntry, payloadLen - sizeof(uint32_t)-1);
  
  
  return gZclSuccess_c;
}

/*!
 * @fn 		zbStatus_t zclTouCalendar_GetSpecialDays(zclTouCalendar_GetSpecialDays_t *pReq)
 *
 * @brief	Sends over-the-air a Get Special Days frame from the TOU Calendar client. 
 *
 */
zbStatus_t zclTouCalendar_GetSpecialDays
( 
    zclTouCalendar_GetSpecialDays_t *pReq
) 
{
    uint8_t len = 0;
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterTouCalendar_c);	
    len = (gTouCalendar_CalendarTypePresent_c == TRUE)?sizeof(zclCmdTouCalendar_GetSpecialDays_t):sizeof(zclCmdTouCalendar_GetSpecialDays_t)-1;
    return ZCL_SendClientReqSeqPassed(gZclCmdTouCalendar_GetSpecialDays_c, len, (zclGenericReq_t *)pReq);
}

/*****************************************
*  TOU Calendar - ProcessGetSpecialDays */
static zbStatus_t zclTouCalendar_ProcessGetSpecialDays
(
  afAddrInfo_t                            addrInfo, 
  uint8_t                                 transactionId,
  zclCmdTouCalendar_GetSpecialDays_t      cmdPayload
 )
{ 
  uint8_t entryIndex;
  
  /* Copy the calendar information*/
  FLib_MemCpy(&mServerGetSpeciaDayInf.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
  mServerGetSpeciaDayInf.zclTransactionId = transactionId;
  mServerGetSpeciaDayInf.calendarType = cmdPayload.calendarType;
  mServerGetSpeciaDayInf.noOfEvents= cmdPayload.numberOfEvents;
  mServerGetSpeciaDayInf.index = 0x00;
  mServerGetSpeciaDayInf.commandIndex = 0x00;
  mServerGetSpeciaDayInf.issuerCalendarId = cmdPayload.issuerCalendarId;
    
  /* Check valid value for IssuerCalendarId */
  entryIndex = zclTouCalendar_FindIssuerCalendarId(mServerGetSpeciaDayInf.issuerCalendarId, gServerActivityCalendarInfo);
  if((entryIndex == gTouCalendar_NoOfInstances_c) &&  (mServerGetSpeciaDayInf.issuerCalendarId!=0x00))  
  {
	  return gZclNotFound_c;
  } 
  
  /* Check valid value for Provider ID */
  if ( OTA2Native32(cmdPayload.providerId) != gServerActivityCalendarInfo[entryIndex].providerId)
  {
    return gZclNotAuthorized_c;
  }  
  
  /* Check valid value for calendar type*/
  if (( mServerGetSpeciaDayInf.calendarType > gTouCalendar_CalendarType_AuxillaryLoadSwitchCalendar_c) &&
      ( mServerGetSpeciaDayInf.calendarType != gTouCalendar_Type_NotSpecified_c))
  {
    return gZclFailure_c;
  }
  
  /* Get the start time */
  cmdPayload.startTime = OTA2Native32(cmdPayload.startTime);
  
  /* If start time is 0x00000000 replace with current start time */
  mServerGetSpeciaDayInf.startTime = Native2OTA32((cmdPayload.startTime == 0x00000000)?ZCL_GetUTCTime(addrInfo.srcEndPoint):cmdPayload.startTime);
  
  /* If number of events is 0 we will return all valid entries*/
  if(mServerGetSpeciaDayInf.noOfEvents == 0)
  {
    mServerGetSpeciaDayInf.noOfEvents = gTouCalendar_NoOfInstances_c;
  }
      
  /* Find next valid entry */
  entryIndex = zclTouCalendar_FindNextSpecialDayEvent();
  if(entryIndex != gTouCalendar_NoOfInstances_c)
  {
    mServerGetCalendarInf.index.cmdIndex = entryIndex;
    SendTouCalendarClusterEvt(gzclEvtHandleTouCalendar_GetSpecialDays_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}

/*!
 * @fn 		zbStatus_t zclTouCalendar_PublishSpecialDays(zclTouCalendar_PublishSpecialDays_t *pCommandRsp, uint8_t payloadLen)
 *
 * @brief	Sends over-the-air a Publish Special Days frame from the TOU Calendar client. 
 *
 */
zbStatus_t zclTouCalendar_PublishSpecialDays
( 
  zclTouCalendar_PublishSpecialDays_t *pCommandRsp,
  uint8_t payloadLen
) 
{
  afToApsdeMessage_t *pMsg;
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          gZclCmdTouCalendar_PublishSpecialDays_c,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &pCommandRsp->zclTransactionId, 
                          &payloadLen,
                          (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
 
 return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  payloadLen, pMsg);  
}


/********************************************
*  TOU Calendar - ProcessPublishSpecialDays */
static zbStatus_t zclTouCalendar_ProcessPublishSpecialDays
(
  zclCmdTouCalendar_PublishSpecialDays_t      *pMsg,
  uint8_t       payloadLen
 )
{ 
  uint8_t i = 0, eventIndex = 0;
  ZCLTime_t duration;
  ZCLTime_t startTime = OTA2Native32(pMsg->startTime);
  uint8_t maxSpecialDayEntries, frameSpecDayEntries;
  
  
  if((gTouCalendar_NoOfInstances_c == 0)||(pMsg->totalNoSpecialDays > gTouCalendar_NoOfSpecialDayEntries_c))
    return gZclInsufficientSpace_c;

  /* Search for an existing entry */
  eventIndex = zclTouCalendar_FindIssuerEventId(OTA2Native32(pMsg->issuerEventId), gClientSpecialDayInfo);
  
  if(eventIndex == gTouCalendar_NoOfInstances_c)  
  {
    eventIndex = 0;
    /*If it is a new entry find an empty slot for it*/
    for(i = 0; i < gTouCalendar_NoOfInstances_c-1; i++)
    {        
        /* check the entry status */
        if(gClientSpecialDayInfo[i].entryStatus == gEntryNotUsed_c)
        {
          eventIndex = i;
          break;
        }
              
        /* get calendar duration */
        if(gClientSpecialDayInfo[i+1].entryStatus == gEntryNotUsed_c)
          duration = 0xFFFFFFFF;
        else
          duration = gClientSpecialDayInfo[i+1].specialDay.startTime - gClientSpecialDayInfo[i].specialDay.startTime;
        
        /* check overlaping callendars */
        if((duration!= 0xFFFFFFFF)&&
           (gClientSpecialDayInfo[i].specialDay.startTime + duration > startTime))
        {
          /* overlaping */
          eventIndex = i;
          break;
        }
        if(i == gTouCalendar_NoOfInstances_c-2)
           eventIndex = gTouCalendar_NoOfInstances_c-1;
    }
  }
  
  /* store the special day information */
  gClientSpecialDayInfo[eventIndex].entryStatus = gEntryUsed_c;
  gClientSpecialDayInfo[eventIndex].specialDay.issuerEventId = OTA2Native32(pMsg->issuerEventId);
  gClientSpecialDayInfo[eventIndex].specialDay.startTime = startTime;
  gClientSpecialDayInfo[eventIndex].specialDay.calendarType = pMsg->calendarType;

  /* Find how much Season Entries can fit into the packet */
  maxSpecialDayEntries = (uint8_t)(payloadLen - 2*sizeof(uint32_t) - 3) / sizeof(zclTouCalendar_SpecialDayEntry_t);
  
  if((pMsg->commandIndex.cmdIndex+1)*maxSpecialDayEntries >= pMsg->totalNoSpecialDays)
    frameSpecDayEntries = pMsg->totalNoSpecialDays - pMsg->commandIndex.cmdIndex*maxSpecialDayEntries;
  else
    frameSpecDayEntries = maxSpecialDayEntries;
 
  gClientSpecialDayInfo[eventIndex].specialDay.noOfSpecialDayEntries += frameSpecDayEntries;
  
  //FLib_MemCpy(&gClientSpecialDayInfo[eventIndex].specialDay.specialDayEntries[maxSpecialDayEntries*pMsg->commandIndex.cmdIndex], 
  //                  &pMsg->specialDayEntry, frameSpecDayEntries);
  
         
  return gZclSuccess_c;
}

/********************************************************
*  TOU Calendar - FindNextCalendarEvent
*       return index in the activity calendar table 
*              or gTouCalendar_NoOfInstances_c         */
static uint8_t zclTouCalendar_FindNextCalendarEvent(void)
{
  uint8_t i; 
  ZCLTime_t eventStartTime;
  
  for(i = mServerGetCalendarInf.index.cmdIndex; i < gTouCalendar_NoOfInstances_c; i++)
  {
      if(gServerActivityCalendarInfo[i].entryStatus == gEntryNotUsed_c)
        break;
      /* check calendar type */
      if ((mServerGetCalendarInf.calendarType == gTouCalendar_Type_NotSpecified_c) ||
            (gServerActivityCalendarInfo[i].calendar.type == mServerGetCalendarInf.calendarType))
      {
        uint32_t duration = 0xFFFFFFFF;
        if(i+1 < gTouCalendar_NoOfInstances_c)
          if(gServerActivityCalendarInfo[i+1].entryStatus == gEntryUsed_c)
            duration = gServerActivityCalendarInfo[i+1].calendar.startTime - gServerActivityCalendarInfo[i].calendar.startTime;
        
        /* Get start time */
        eventStartTime = OTA2Native32(gServerActivityCalendarInfo[i].calendar.startTime);
      
        /* Check if entry satisfies condition*/
        if(duration != 0xFFFFFFFF)
        {
          if(mServerGetCalendarInf.startTime < eventStartTime + duration)
            return i;
        }
        else
          return i;
          
      }
  }
  return gTouCalendar_NoOfInstances_c; 
}  


/********************************************************
*  TOU Calendar - zclTouCalendar_FindNextSpecialDayEvent
*       return index in the special day info table 
*              or gTouCalendar_NoOfInstances_c         */
static uint8_t zclTouCalendar_FindNextSpecialDayEvent(void)
{
  uint8_t i; 
  ZCLTime_t eventStartTime;
  
  for(i = mServerGetSpeciaDayInf.index; i < gTouCalendar_NoOfInstances_c; i++)
  {
      if(gServerSpecialDayInfo[i].entryStatus == gEntryNotUsed_c)
        break;
      /* check calendar type */
      if ((mServerGetSpeciaDayInf.calendarType == gTouCalendar_Type_NotSpecified_c) ||
            (gServerSpecialDayInfo[i].specialDay.calendarType == mServerGetSpeciaDayInf.calendarType))
      {        
        uint32_t duration = 0xFFFFFFFF;
        if(i+1 < gTouCalendar_NoOfInstances_c)
          if(gServerSpecialDayInfo[i+1].entryStatus == gEntryUsed_c)
            duration = gServerSpecialDayInfo[i+1].specialDay.startTime - gServerSpecialDayInfo[i].specialDay.startTime;
        
        /* Get start time */
        eventStartTime = OTA2Native32(gServerSpecialDayInfo[i].specialDay.startTime);
      
        /* Check if entry satisfies condition*/
        if(duration != 0xFFFFFFFF)
        {
          if((mServerGetSpeciaDayInf.startTime >= eventStartTime+duration))
            return i;
        }
        else
          return i;
      }
  }
  return gTouCalendar_NoOfInstances_c; 
} 


/************************************************************************
*  TOU Calendar - FindIssuerCalendarId
*       return  - index of instances that match with issuerCalendarId or
*                 gTouCalendar_NoOfInstances_c                          */
static uint8_t zclTouCalendar_FindIssuerCalendarId(uint32_t issuerCalendarId,zclTouCalendar_ActivityCalendarInfo_t *pCalendarInfo)
{
  uint8_t i;
  if (!issuerCalendarId)
    return 0;
  for(i=0; i< gTouCalendar_NoOfInstances_c;i++)
    if(issuerCalendarId == pCalendarInfo[i].calendar.issuerId)
     return i;
  return gTouCalendar_NoOfInstances_c; 
}

/************************************************************************
*  TOU Calendar - FindIssuerEventId
*       return  - index of event that match with issuerEventId or
*                 gTouCalendar_NoOfInstances_c                          */
static uint8_t zclTouCalendar_FindIssuerEventId(uint32_t issuerEventId, zclTouCalendar_SpecialDayInfo_t *pSpecialDayInfo)
{
  uint8_t i;
  for(i=0; i< gTouCalendar_NoOfInstances_c;i++)
    if(issuerEventId == pSpecialDayInfo[i].specialDay.issuerEventId)
     return i;
  return gTouCalendar_NoOfInstances_c; 
}
/******************************************************
* SendTouCalendarClusterEvt
*     - Send the TOU Calendar Cluster Event to the TS */
void SendTouCalendarClusterEvt(uint32_t evtId)
{
  mTouCalendarClusterEvt = evtId;
  ZclSE_SendClusterEvt(gzclEvtHandleTouCalendarClusterEvt_c);
}


/*****************************************
* ZCL_HandleSETouCalendarClusterEvt
*      - Handle the TouCalendar Cluster Event */
void ZCL_HandleSETouCalendarClusterEvt(void)
{
  uint32_t events = mTouCalendarClusterEvt;  
  
  if (events & gzclEvtHandleTouCalendar_GetCalendar_c)
    zclTouCalendar_HandleGetCalendar();
  if (events & gzclEvtHandleTouCalendar_GetDayProfiles_c)
    zclTouCalendar_HandleGetDayProfiles();
  if (events & gzclEvtHandleTouCalendar_GetWeekProfiles_c)
    zclTouCalendar_HandleGetWeekProfiles();
  if (events & gzclEvtHandleTouCalendar_GetSeasons_c)
    zclTouCalendar_HandleGetSeasons();
  if (events & gzclEvtHandleTouCalendar_GetSpecialDays_c)
    zclTouCalendar_HandleGetSpecialDays();
}


/*****************************************
* zclTouCalendar_HandleGetCalendar 
      - Handle the Get Calendar command */
static void zclTouCalendar_HandleGetCalendar(void)
{
  uint8_t *pCmdRsp;
  uint8_t entryIndex, offset = 0;
  ZCLTime_t startTime;
  uint32_t issuerId;
  uint8_t currentIndex = mServerGetCalendarInf.index.cmdIndex;

  pCmdRsp = AF_MsgAlloc();
  if(!pCmdRsp)
    return;
  /* complete the publishCalendar command */

  FLib_MemCpy(&pCmdRsp[offset], &mServerGetCalendarInf.addrInfo, sizeof(afAddrInfo_t));
  offset+= sizeof(afAddrInfo_t);

  pCmdRsp[offset] = mServerGetCalendarInf.zclTransactionId;
  offset+=1;
  
  /*add ProviderId*/
  issuerId = gServerActivityCalendarInfo[currentIndex].providerId;
  issuerId = Native2OTA32(issuerId);
  FLib_MemCpy(&pCmdRsp[offset], &issuerId, sizeof(uint32_t));
  offset+=sizeof(uint32_t);
  
  /*add issuerEventId */
  issuerId = gServerActivityCalendarInfo[currentIndex].calendar.issuerEventId;
  issuerId = Native2OTA32(issuerId);
  FLib_MemCpy(&pCmdRsp[offset], &issuerId, sizeof(uint32_t));
  offset+=sizeof(uint32_t);
  
  /*add issuerCalendarId*/
  issuerId = gServerActivityCalendarInfo[currentIndex].calendar.issuerId;
  issuerId = Native2OTA32(issuerId);
  FLib_MemCpy(&pCmdRsp[offset], &issuerId, sizeof(uint32_t));
  offset+=sizeof(uint32_t);
  
  /*add startTime*/
  startTime = Native2OTA32(gServerActivityCalendarInfo[currentIndex].calendar.startTime);
  FLib_MemCpy(&pCmdRsp[offset], &startTime, sizeof(uint32_t));
  offset+=sizeof(uint32_t);

  /*add calendarType*/
  pCmdRsp[offset] = gServerActivityCalendarInfo[currentIndex].calendar.type;
  offset+=1;

  //pCmdRsp[offset] = gTouCalendar_CalendarTimeRef_StandardTime_c;
  //offset+=1;
  FLib_MemCpy(&pCmdRsp[offset], &gServerActivityCalendarInfo[currentIndex].calendar.name,
            gServerActivityCalendarInfo[currentIndex].calendar.name.length+1); 
  offset+= gServerActivityCalendarInfo[currentIndex].calendar.name.length+1;
  pCmdRsp[offset] = gServerActivityCalendarInfo[currentIndex].calendar.noOfSeasonEntries;
  offset+=1;
  pCmdRsp[offset] = gServerActivityCalendarInfo[currentIndex].calendar.noOfWeekProfileEntries;
  offset+=1;
  pCmdRsp[offset] = gServerActivityCalendarInfo[currentIndex].calendar.noOfDayProfileEntries;;
  offset+=1;

  /* send the command over the air */
  if(zclTouCalendar_PublishCalendar((zclTouCalendar_PublishCalendar_t *)pCmdRsp)!=gZclSuccess_c)
  {
    MSG_Free(pCmdRsp);
    return;
  } 
  /* update get calendar information */
  mServerGetCalendarInf.noOfCalendarsRemaining--;
  mServerGetCalendarInf.index.cmdIndex++;
    
  if (mServerGetCalendarInf.noOfCalendarsRemaining > 0)
  {
    /* Find next valid entry */
    entryIndex = zclTouCalendar_FindNextCalendarEvent();
    if(entryIndex != gTouCalendar_NoOfInstances_c)
    {
      mServerGetCalendarInf.index.cmdIndex = entryIndex;
      /* Send event to TS*/
      SendTouCalendarClusterEvt(gzclEvtHandleTouCalendar_GetCalendar_c);
    }
  }  
  MSG_Free(pCmdRsp);
}


/*****************************************
* zclTouCalendar_HandleGetDayProfiles 
      - Handle the Get DayProfiles command */
static void zclTouCalendar_HandleGetDayProfiles(void)
{
  
  zclTouCalendar_PublishDayProfile_t *pCommandRsp;
  uint8_t maxScheduleEntries = 0, i = 0;
  uint8_t calendarIndex = mServerGetDayProfilesInf.calendarIndex;
  uint8_t dayIndex = mServerGetDayProfilesInf.dayIndex;
  uint8_t cmdIndex = mServerGetDayProfilesInf.commandIndex.cmdIndex;
  
  pCommandRsp = AF_MsgAlloc();
  if(!pCommandRsp)
    return;
  
  /* complete the publishDayProfile command */
  FLib_MemCpy(&pCommandRsp->addrInfo, &mServerGetDayProfilesInf.addrInfo, sizeof(afAddrInfo_t));
  pCommandRsp->zclTransactionId = mServerGetDayProfilesInf.zclTransactionId;
  
  pCommandRsp->cmdFrame.providerId = gServerActivityCalendarInfo[calendarIndex].providerId;
  pCommandRsp->cmdFrame.issuerEventId = gServerActivityCalendarInfo[calendarIndex].calendar.issuerEventId; 
  pCommandRsp->cmdFrame.issuerCalendarId = mServerGetDayProfilesInf.issuerCalendarId;
  
  /* Find how much day Schedule Entries can fit into the packet */
  maxScheduleEntries = (uint8_t)(AF_MaxPayloadLen(&mServerGetDayProfilesInf.addrInfo) - sizeof(zclFrame_t) - 3) / sizeof(zclTouCalendar_DayScheduleEntries_t);
  
  for(i = dayIndex;i<mServerGetDayProfilesInf.NumberOfDays;i++)
  {
    uint8_t availableScheduleEntries = gServerActivityCalendarInfo[calendarIndex].calendar.dayProfileTable[i].noOfDayScheduleEntries;
    uint8_t frameScheduleEntries = 0;
    
    if(availableScheduleEntries == 0x00)
      continue;  

    pCommandRsp->cmdFrame.dayId = gServerActivityCalendarInfo[calendarIndex].calendar.dayProfileTable[i].dayId;
    pCommandRsp->cmdFrame.totalNoOfScheduleEntries = availableScheduleEntries;
    pCommandRsp->cmdFrame.commandIndex = mServerGetDayProfilesInf.commandIndex;
  
    if(availableScheduleEntries >  (cmdIndex+1)*maxScheduleEntries)
    { 
      frameScheduleEntries = maxScheduleEntries;
      mServerGetDayProfilesInf.commandIndex.cmdIndex++; 
      mServerGetDayProfilesInf.dayIndex = i;    
    }
    else
    {
      frameScheduleEntries = availableScheduleEntries - cmdIndex*maxScheduleEntries;
      /* prepare for the next PublishDayProfile Command */
      mServerGetDayProfilesInf.commandIndex.cmdIndex = 0;
      mServerGetDayProfilesInf.dayIndex = i+1;
    }
    mServerGetDayProfilesInf.commandIndex.TotalNumberOfCmds = 0x01;
    FLib_MemCpy(&pCommandRsp->cmdFrame.dayScheduleEntries, &gServerActivityCalendarInfo[calendarIndex].calendar.dayProfileTable[i].dayScheduleEntry[maxScheduleEntries*cmdIndex], 
                        frameScheduleEntries*sizeof(zclTouCalendar_DayScheduleEntries_t));
    
    if(zclTouCalendar_PublishDayProfile(pCommandRsp, sizeof(zclCmdTouCalendar_PublishDayProfile_t)+ (frameScheduleEntries-1)*sizeof(zclTouCalendar_DayScheduleEntries_t))== gZclSuccess_c)
    {
        /* send event to TS */  
      SendTouCalendarClusterEvt(gzclEvtHandleTouCalendar_GetDayProfiles_c);
    }
    MSG_Free(pCommandRsp);
    return;
  }
  MSG_Free(pCommandRsp);
}

/*****************************************
* zclTouCalendar_HandleGetWeekProfiles 
      - Handle the Get WeekProfiles command */
static void zclTouCalendar_HandleGetWeekProfiles(void)
{
  
  zclTouCalendar_PublishWeekProfile_t commandRsp;
  uint8_t calendarIndex = mServerGetWeekProfilesInf.calendarIndex;
  uint8_t weekIndex = mServerGetWeekProfilesInf.weekIndex;
  
  /* complete the publishWeekProfile command */
  FLib_MemCpy(&commandRsp.addrInfo, &mServerGetWeekProfilesInf.addrInfo, sizeof(afAddrInfo_t));
  commandRsp.zclTransactionId = mServerGetWeekProfilesInf.zclTransactionId;
  
  commandRsp.cmdFrame.providerId = gServerActivityCalendarInfo[calendarIndex].providerId;
  commandRsp.cmdFrame.issuerEventId = gServerActivityCalendarInfo[calendarIndex].calendar.issuerEventId;
  commandRsp.cmdFrame.issuerCalendarId = mServerGetWeekProfilesInf.issuerCalendarId;
  FLib_MemCpy(&commandRsp.cmdFrame.weekProfile, &gServerActivityCalendarInfo[calendarIndex].calendar.weekProfileTable[weekIndex],sizeof(zclTouCalendar_WeekProfile_t));
  
  if(zclTouCalendar_PublishWeekProfile(&commandRsp) == gZclSuccess_c)
  {
    mServerGetWeekProfilesInf.weekIndex++;
    if(mServerGetWeekProfilesInf.weekIndex < mServerGetWeekProfilesInf.noOfweeks)
    {
      /* send event to TS */  
      SendTouCalendarClusterEvt(gzclEvtHandleTouCalendar_GetWeekProfiles_c);
    }
  }
}

/*****************************************
* zclTouCalendar_HandleGetSeasons 
      - Handle the Get Seasons command */
static void zclTouCalendar_HandleGetSeasons(void)
{
  
  zclTouCalendar_PublishSeasons_t *pCommandRsp;
  uint8_t calendarIndex = mServerGetSeasonsInf.calendarIndex;
  uint8_t cmdIndex = mServerGetSeasonsInf.commandIndex.cmdIndex;
  uint8_t maxSeasonEntries = 0;
  uint8_t availableSeasonsEntries, frameSeasonEntries; 
  pCommandRsp = AF_MsgAlloc();
  if(!pCommandRsp)
    return;
  
  /* complete the publishSeasons command */
  FLib_MemCpy(&pCommandRsp->addrInfo, &mServerGetSeasonsInf.addrInfo, sizeof(afAddrInfo_t));
  pCommandRsp->zclTransactionId = mServerGetSeasonsInf.zclTransactionId;
  
  pCommandRsp->cmdFrame.providerId = gServerActivityCalendarInfo[calendarIndex].providerId;
  pCommandRsp->cmdFrame.issuerEventId = gServerActivityCalendarInfo[calendarIndex].calendar.issuerEventId;   
  pCommandRsp->cmdFrame.issuerCalendarId = mServerGetSeasonsInf.issuerCalendarId;
  pCommandRsp->cmdFrame.commandIndex = mServerGetSeasonsInf.commandIndex;
  
  
  /* Find how much Season Entries can fit into the packet */
  maxSeasonEntries = (uint8_t)(AF_MaxPayloadLen(&mServerGetSeasonsInf.addrInfo) - sizeof(zclFrame_t) - sizeof(uint32_t) - 1) / sizeof(zclTouCalendar_SeasonEntry_t);
 
  availableSeasonsEntries = gServerActivityCalendarInfo[calendarIndex].calendar.noOfSeasonEntries;
  if(availableSeasonsEntries/maxSeasonEntries > 0)
	  mServerGetSeasonsInf.commandIndex.TotalNumberOfCmds = availableSeasonsEntries/maxSeasonEntries + 1;
  else
	  mServerGetSeasonsInf.commandIndex.TotalNumberOfCmds = availableSeasonsEntries/maxSeasonEntries;
  
  
  if(availableSeasonsEntries >  (cmdIndex+1)*maxSeasonEntries)
  { 
    frameSeasonEntries = maxSeasonEntries;
    mServerGetSeasonsInf.commandIndex.cmdIndex++;      
  }
  else
  {
    frameSeasonEntries = availableSeasonsEntries - cmdIndex*maxSeasonEntries;
    /* prepare for the next PublishSeasonProfile Command */
    mServerGetSeasonsInf.commandIndex.cmdIndex = 0;
  }
  
  FLib_MemCpy(&pCommandRsp->cmdFrame.seasonEntry, &gServerActivityCalendarInfo[calendarIndex].calendar.seasonTable[maxSeasonEntries*cmdIndex], 
                        frameSeasonEntries);
  
  
  if(zclTouCalendar_PublishSeasons(pCommandRsp) == gZclSuccess_c)
  {
	  if(mServerGetSeasonsInf.commandIndex.TotalNumberOfCmds >= cmdIndex+1)
	  {
		  /* send event to TS */  
		  SendTouCalendarClusterEvt(gzclEvtHandleTouCalendar_GetSeasons_c);
	  }
  }
  MSG_Free(pCommandRsp);
}

/*****************************************
* zclTouCalendar_HandleGetSpecialDays
      - Handle the Get SpecialDays command */
static void zclTouCalendar_HandleGetSpecialDays(void)
{
  
  zclTouCalendar_PublishSpecialDays_t *pCommandRsp;
  uint8_t entryIndex, frameSpecialDayEntries = 0;
  uint8_t currentIndex = mServerGetSpeciaDayInf.index;
  uint8_t maxSpecialDayEntries = 0, availableSpecialDayEntries = 0;
  uint8_t cmdIndex = mServerGetSpeciaDayInf.commandIndex;
  uint8_t i;
  pCommandRsp = AF_MsgAlloc();
  if(!pCommandRsp)
    return;
  
  /* complete the publishSpecialDays command */
  FLib_MemCpy(&pCommandRsp->addrInfo, &mServerGetSpeciaDayInf.addrInfo, sizeof(afAddrInfo_t));
  pCommandRsp->zclTransactionId = mServerGetSpeciaDayInf.zclTransactionId;
  
  pCommandRsp->cmdFrame.providerId = gServerSpecialDayInfo[currentIndex].providerId;
  pCommandRsp->cmdFrame.issuerCalendarId = mServerGetSpeciaDayInf.issuerCalendarId;
  pCommandRsp->cmdFrame.startTime = Native2OTA32(gServerSpecialDayInfo[currentIndex].specialDay.startTime);
  pCommandRsp->cmdFrame.issuerEventId = Native2OTA32(gServerSpecialDayInfo[currentIndex].specialDay.issuerEventId);
  pCommandRsp->cmdFrame.calendarType = gServerSpecialDayInfo[currentIndex].specialDay.calendarType;
  
  pCommandRsp->cmdFrame.totalNoSpecialDays = gServerSpecialDayInfo[currentIndex].specialDay.noOfSpecialDayEntries;
  
  /* Find how much Season Entries can fit into the packet */
  maxSpecialDayEntries = (uint8_t)(AF_MaxPayloadLen(&mServerGetSpeciaDayInf.addrInfo) - sizeof(zclFrame_t) - 2*sizeof(uint32_t) - 3) / sizeof(zclTouCalendar_SpecialDayEntry_t);
  
  availableSpecialDayEntries = gServerSpecialDayInfo[currentIndex].specialDay.noOfSpecialDayEntries;
  
  if(availableSpecialDayEntries >  (cmdIndex+1)*maxSpecialDayEntries)
  { 
    frameSpecialDayEntries = maxSpecialDayEntries;
    mServerGetSeasonsInf.commandIndex.cmdIndex++;      
  }
  else
  {
    frameSpecialDayEntries = availableSpecialDayEntries - cmdIndex*maxSpecialDayEntries;
    /* prepare for the next PublishDayProfile Command */
    mServerGetSeasonsInf.commandIndex.cmdIndex = 0;
  }
  
  for(i=0;i<frameSpecialDayEntries;i++)
  {
    pCommandRsp->cmdFrame.specialDayEntry[i].specialDayDate = gServerSpecialDayInfo[currentIndex].specialDay.specialDayEntries[maxSpecialDayEntries*cmdIndex+i].specialDayDate;
    pCommandRsp->cmdFrame.specialDayEntry[i].dayIdRef = gServerSpecialDayInfo[currentIndex].specialDay.specialDayEntries[maxSpecialDayEntries*cmdIndex+i].dayIdRef;
  }
    // FLib_MemCpy(&pCommandRsp->cmdFrame.specialDayEntry, &gServerSpecialDayInfo[currentIndex].specialDay.specialDayEntries[maxSpecialDayEntries*cmdIndex], 
  //                      frameSpecialDayEntries*sizeof(zclTouCalendar_SpecialDayEntry_t));
  
  /* send the command over the air */
  if(zclTouCalendar_PublishSpecialDays(pCommandRsp, sizeof(zclCmdTouCalendar_PublishSpecialDays_t)+ (frameSpecialDayEntries-1)*sizeof(zclTouCalendar_SpecialDayEntry_t))!=gZclSuccess_c)
    return;
  
  /* update get special days information */
  if(mServerGetSeasonsInf.commandIndex.cmdIndex == 0)
  {
    mServerGetSpeciaDayInf.noOfEvents--;
    mServerGetSpeciaDayInf.index++;
    
    if (mServerGetSpeciaDayInf.noOfEvents > 0)
    {
      /* Find next valid entry */
      entryIndex = zclTouCalendar_FindNextSpecialDayEvent();
      if(entryIndex != gTouCalendar_NoOfInstances_c)
      {
        mServerGetSpeciaDayInf.index = entryIndex;
        /* Send event to TS*/
        SendTouCalendarClusterEvt(gzclEvtHandleTouCalendar_GetSpecialDays_c);
      }
    }
  }
  else
    /* Send event to TS*/
    SendTouCalendarClusterEvt(gzclEvtHandleTouCalendar_GetSpecialDays_c);
}

/*!
 * @fn 		zbStatus_t ZtcTouCalendar_StoreServerActivityCalendarInfo(uint8_t *pMsg)
 *
 * @brief	Stores activity calendar information received from the Provider. 
 *
 */
zbStatus_t ZtcTouCalendar_StoreServerActivityCalendarInfo(uint8_t *pMsg)
{
  uint8_t i, entryIdx = 0x00;
  uint32_t issuerId = 0, issuerEventId = 0;
  uint32_t providerId = 0;
  ZCLTime_t duration = 0, startTime = 0;
  uint8_t offset = 0, nameLength = 0;
  bool_t sendPublish;
   
  sendPublish = pMsg[offset];
  offset += 1;  
  
  FLib_MemCpy(&providerId, &pMsg[offset], sizeof(uint32_t));
  offset += sizeof(uint32_t);
  
  FLib_MemCpy(&issuerEventId, &pMsg[offset], sizeof(uint32_t));
  offset += sizeof(uint32_t); 
  
  FLib_MemCpy(&issuerId, &pMsg[offset], sizeof(uint32_t));
  offset += sizeof(uint32_t);
  
  
  /* calendarName */
  nameLength = pMsg[offset];
  offset += 1;
  FLib_MemCpy(&gServerActivityCalendarInfo[entryIdx].calendar.name, &pMsg[offset-1], nameLength+1);
  offset+=nameLength;
  
  FLib_MemCpy(&startTime, &pMsg[offset], sizeof(ZCLTime_t));
  offset+=sizeof(ZCLTime_t);
  
  /* Search for an existing entry*/
  entryIdx = zclTouCalendar_FindIssuerCalendarId(issuerId, gServerActivityCalendarInfo);
  
  /*If it is a new entry find an empty slot for it or check for overlapping calendar */
  if(entryIdx == gTouCalendar_NoOfInstances_c)  
  {
    entryIdx = 0;
    for(i = 0; i < gTouCalendar_NoOfInstances_c-1; i++)
    {
        /* check the entry status */
        if(gServerActivityCalendarInfo[i].entryStatus == gEntryNotUsed_c)
        {
          entryIdx = i;
          break;
        }
        
        /* get calendar duration */
        if(gServerActivityCalendarInfo[i+1].entryStatus == gEntryNotUsed_c)
          duration = 0xFFFFFFFF;
        else
          duration = gServerActivityCalendarInfo[i+1].calendar.startTime - gServerActivityCalendarInfo[i].calendar.startTime;
        
        /* check overlapping callendars */
        if((duration!= 0xFFFFFFFF)&&
           (gServerActivityCalendarInfo[i].calendar.startTime + duration > startTime))
        {
          /* overlaping */
          entryIdx = i;
          break;
        }
        if(i == gTouCalendar_NoOfInstances_c-2)
           entryIdx = gTouCalendar_NoOfInstances_c-1;
    }
  }
  
  /* Save the information in the Activity Calendar Info Table*/
  gServerActivityCalendarInfo[entryIdx].entryStatus = gEntryUsed_c;
  
  gServerActivityCalendarInfo[entryIdx].providerId = providerId;
  gServerActivityCalendarInfo[entryIdx].calendar.issuerEventId = issuerEventId;
  
  /* issuerId*/
  gServerActivityCalendarInfo[entryIdx].calendar.issuerId = issuerId;
  
 
  /* start Time */
  gServerActivityCalendarInfo[entryIdx].calendar.startTime = startTime;
  
  /* calendar type */
  gServerActivityCalendarInfo[entryIdx].calendar.type = pMsg[offset];
  offset+=1;
  
  /* season table */
  gServerActivityCalendarInfo[entryIdx].calendar.noOfSeasonEntries = pMsg[offset];
  offset+=1;
  for(i=0;i< gServerActivityCalendarInfo[entryIdx].calendar.noOfSeasonEntries;i++)
  {
    FLib_MemCpyReverseOrder(&gServerActivityCalendarInfo[entryIdx].calendar.seasonTable[i].seasonStartDate, &pMsg[offset+i*sizeof(zclTouCalendar_SeasonEntry_t)], sizeof(zclDate_t));
    gServerActivityCalendarInfo[entryIdx].calendar.seasonTable[i].weekIdRef = pMsg[offset+i*sizeof(zclTouCalendar_SeasonEntry_t) + sizeof(zclDate_t)];
  }
  offset+=gServerActivityCalendarInfo[entryIdx].calendar.noOfSeasonEntries*sizeof(zclTouCalendar_SeasonEntry_t);
  
  /* noOfWeekProfileEntry + weekTable */
  gServerActivityCalendarInfo[entryIdx].calendar.noOfWeekProfileEntries = pMsg[offset];
  offset+=1;
  for(i=0;i< gServerActivityCalendarInfo[entryIdx].calendar.noOfWeekProfileEntries;i++)
    FLib_MemCpy(&gServerActivityCalendarInfo[entryIdx].calendar.weekProfileTable[i], &pMsg[offset+i*sizeof(zclTouCalendar_WeekProfile_t)], sizeof(zclTouCalendar_WeekProfile_t));
  offset += gServerActivityCalendarInfo[entryIdx].calendar.noOfWeekProfileEntries*sizeof(zclTouCalendar_WeekProfile_t);
  
  /* noOfDayProfileEntry + dayTable */
  gServerActivityCalendarInfo[entryIdx].calendar.noOfDayProfileEntries = pMsg[offset];
  offset+=1;
  for(i = 0; i< gServerActivityCalendarInfo[entryIdx].calendar.noOfDayProfileEntries;i++)
  {
    gServerActivityCalendarInfo[entryIdx].calendar.dayProfileTable[i].dayId = pMsg[offset];
    offset+=1;
    gServerActivityCalendarInfo[entryIdx].calendar.dayProfileTable[i].noOfDayScheduleEntries =  pMsg[offset];
    offset+=1;
    FLib_MemCpy(&gServerActivityCalendarInfo[entryIdx].calendar.dayProfileTable[i].dayScheduleEntry, &pMsg[offset], pMsg[offset-1]*sizeof(zclTouCalendar_DayScheduleEntries_t));
    offset+=pMsg[offset-1]*sizeof(zclTouCalendar_DayScheduleEntries_t);
  }
  
  if(sendPublish == TRUE)
  {
    afAddrInfo_t addrInfo = {gZbAddrModeIndirect_c, {0x00, 0x00}, 0x08, {gaZclClusterTouCalendar_c}, 0x08, gApsTxOptionSecEnabled_c, 1};
    
    addrInfo.radiusCounter = afDefaultRadius_c;
    /* Copy the calendar information*/
    FLib_MemCpy(&mServerGetCalendarInf.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
    mServerGetCalendarInf.zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId);
    mServerGetCalendarInf.noOfCalendarsRemaining = 0x01;
    mServerGetCalendarInf.index.cmdIndex = entryIdx;
    SendTouCalendarClusterEvt(gzclEvtHandleTouCalendar_GetCalendar_c);    
  }
  return gZclSuccess_c;
}

/*!
 * @fn 		zbStatus_t ZtcTouCalendar_StoreServerSpecialDayInfo(zclTouCalendar_SpecialDayInfo_t *pMsg)
 *
 * @brief	Stores special days information received from the Provider. 
 *
 */
zbStatus_t ZtcTouCalendar_StoreServerSpecialDayInfo(zclTouCalendar_SpecialDayInfo_t *pMsg)
{
  uint8_t i, entryIdx = 0x00;
  ZCLTime_t duration = 0;
  bool_t sendPublish = pMsg->entryStatus;
  
  /* Search for an existing entry*/
  entryIdx = zclTouCalendar_FindIssuerEventId(pMsg->specialDay.issuerEventId, gServerSpecialDayInfo);
   
  /*If it is a new entry find an empty slot for it or check for overlapping special days */
  if(entryIdx == gTouCalendar_NoOfInstances_c)  
  {
    entryIdx = 0;
    for(i = 0; i < gTouCalendar_NoOfInstances_c-1; i++)
    {        
        /* check the entry status */
        if(gClientSpecialDayInfo[i].entryStatus == gEntryNotUsed_c)
        {
          entryIdx = i;
          break;
        }
              
        gClientSpecialDayInfo[i].providerId = pMsg->providerId;
        /* get calendar duration */
        if(gClientSpecialDayInfo[i+1].entryStatus == gEntryNotUsed_c)
          duration = 0xFFFFFFFF;
        else
          duration = gClientSpecialDayInfo[i+1].specialDay.startTime - gClientSpecialDayInfo[i].specialDay.startTime;
        
        /* check overlapping events */
        if((duration!= 0xFFFFFFFF)&&
           (gClientSpecialDayInfo[i].specialDay.startTime + duration > pMsg->specialDay.startTime))
        {
          /* overlaping */
          entryIdx = i;
          break;
        }
        if(i == gTouCalendar_NoOfInstances_c-2)
           entryIdx = gTouCalendar_NoOfInstances_c-1;
    }
  }
  
  gServerSpecialDayInfo[entryIdx].entryStatus = gEntryUsed_c;
  gServerSpecialDayInfo[entryIdx].providerId = pMsg->providerId;
  gServerSpecialDayInfo[entryIdx].specialDay.issuerEventId = pMsg->specialDay.issuerEventId;
  gServerSpecialDayInfo[entryIdx].specialDay.calendarType = pMsg->specialDay.calendarType;
  gServerSpecialDayInfo[entryIdx].specialDay.startTime = pMsg->specialDay.startTime;
  gServerSpecialDayInfo[entryIdx].specialDay.noOfSpecialDayEntries = pMsg->specialDay.noOfSpecialDayEntries;
  if(gServerSpecialDayInfo[entryIdx].specialDay.noOfSpecialDayEntries> gTouCalendar_NoOfSpecialDayEntries_c)
    return gZclInsufficientSpace_c;
  for(i=0; i< pMsg->specialDay.noOfSpecialDayEntries;i++)
  {
    gServerSpecialDayInfo[entryIdx].specialDay.specialDayEntries[i].dayIdRef = pMsg->specialDay.specialDayEntries[i].dayIdRef;
    gServerSpecialDayInfo[entryIdx].specialDay.specialDayEntries[i].specialDayDate = pMsg->specialDay.specialDayEntries[i].specialDayDate;
  }

  if(sendPublish == TRUE)
  {
    afAddrInfo_t addrInfo = {gZbAddrModeIndirect_c, {0x00, 0x00}, 0x08, {gaZclClusterTouCalendar_c}, 0x08, gApsTxOptionSecEnabled_c, 1};
    
    addrInfo.radiusCounter = afDefaultRadius_c;
    /* Copy the calendar information*/
    FLib_MemCpy(&mServerGetSpeciaDayInf.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
    mServerGetSpeciaDayInf.zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId);
    mServerGetSpeciaDayInf.index = entryIdx;
    mServerGetSpeciaDayInf.calendarType = pMsg->specialDay.calendarType;
    mServerGetSpeciaDayInf.commandIndex = 0;
    mServerGetSpeciaDayInf.noOfEvents = pMsg->specialDay.noOfSpecialDayEntries;
    mServerGetSpeciaDayInf.startTime = pMsg->specialDay.startTime;
    
    SendTouCalendarClusterEvt(gzclEvtHandleTouCalendar_GetSpecialDays_c);
  }
  return gZclSuccess_c;
}
#endif /* gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d  */ 
