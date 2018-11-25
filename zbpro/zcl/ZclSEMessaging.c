/*! @file	ZclSEMessaging.c
 *
 * @brief	This source file describes specific functionality implemented
 *			for the Messaging cluster.
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

#include "ZclFoundation.h"
#include "SEProfile.h"
#include "ZclSE.h"
#include "ZclSEMessaging.h"

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
static void MsgDisplayTimerCallBack(zbTmrTimerID_t tmrID);
static void ZclMsg_StoreMessage(zclDisplayMsgReq_t *pReq);
static zbStatus_t ZCL_ProcessDisplayMsg(addrInfoType_t* pAddrInfo, zclSEGenericRxCmd_t *pCmd);
static void ZCL_StoreMessage(uint8_t *pMsgLocation, zclSEGenericRxCmd_t *pCmd);
static zbStatus_t ZclMsg_SendLastMessage(afAddrInfo_t* pAddrInfo);
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

static zbTmrTimerID_t mMsgDisplayTimerID;
static zclMsg_MsgTableEntry_t mMsgLastMessage;
static zclMsg_MsgTableEntry_t maMsgClientTable[gNumOfMsgTableEntry_c];

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		void ZCL_MsgInit(void)
 *
 * @brief	Initializes the Messaging client functionality.
 *
 */
void ZCL_MsgInit(void)
{
  /*Set default msg cluster response addr.*/
  mMsgDisplayTimerID = ZbTMR_AllocateTimer();
  ZbTMR_StartSecondTimer(mMsgDisplayTimerID, 1, MsgDisplayTimerCallBack);
}

/*!
 * @fn 		zbStatus_t ZCL_MsgClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Messaging Cluster client. 
 *
 */
zbStatus_t ZCL_MsgClusterClient
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zclFrame_t *pFrame; 
  afAddrInfo_t addrInfo;    
  zbStatus_t status = gZclSuccessDefaultRsp_c;

  (void) pDev;
  pFrame = (void *)pIndication->pAsdu;
  
   /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    status = gZclSuccess_c;
  
  /* prepare for response in the address info (back to sender) */
  AF_PrepareForReply(&addrInfo, pIndication);
  
  Cmd = pFrame->command;
      
  switch(Cmd) {
  case gZclCmdMsg_DisplayMsgReq_c: 
    {
      zclCmdMsg_DisplayMsgReq_t *pReq;
	  zclSEGenericRxCmd_t rxCmd;
      pReq = (zclCmdMsg_DisplayMsgReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));
	  rxCmd.pSECmd = (void*)pReq;
      if (pIndication->fragmentHdr.pData)
        rxCmd.pRxFrag = &pIndication->fragmentHdr;
      else
        rxCmd.pRxFrag = NULL;
      (void)ZCL_ProcessDisplayMsg((addrInfoType_t*)&addrInfo, &rxCmd);
    }  
  break;  
 case gZclCmdMsg_CancelMsgReq_c:
  {
    zclCmdMsg_CancelMsgReq_t *pReq = (zclCmdMsg_CancelMsgReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_MsgCancelMessageReceived_c, 0, 0, pReq);
  }  
  break;   
  default:
    status = gZclUnsupportedClusterCommand_c;
  break;  
 }
 return status;
}

/*!
 * @fn 		zbStatus_t ZCL_MsgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Messaging Cluster server. 
 *
 */
zbStatus_t ZCL_MsgClusterServer
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
  
   /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    status = gZclSuccess_c;
  
  Cmd = pFrame->command;

 switch(Cmd) {
  case gZclCmdMsg_GetLastMsgReq_c: 
  {
    zclGetLastMsgReq_t *pReq;
    pReq = (zclGetLastMsgReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_MsgGetLastMessageReceived_c, 0, 0, pReq);
    
    if (mMsgLastMessage.EntryStatus == gEntryNotUsed_c)
      return gZclNotFound_c;
    else
    {
      afAddrInfo_t replyAddrInfo;
      AF_PrepareForReply((afAddrInfo_t*)&replyAddrInfo, pIndication);
      status = ZclMsg_SendLastMessage(&replyAddrInfo);
    }
  }  
  break;  
 case gZclCmdMsg_MsgConfReq_c:
  {
    zclCmdMsg_MsgConfReq_t *pReq = (zclCmdMsg_MsgConfReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_MsgMessageConfirmReceived_c, 0, 0, pReq);
  }  
  break;   
  default:
    status = gZclUnsupportedClusterCommand_c;
  break;  
 }
 return status;
}

/*!
 * @fn 		zbStatus_t ZCL_InterPanMsgClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the InterPAN Messaging Cluster client. 
 *
 */
zbStatus_t ZCL_InterPanMsgClusterClient
(
	zbInterPanDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
 (void) pDev;

 Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
 /*Both Display message and Cancel message commands are passed up to the application
   so it can take appropiate action on the display.
 */
 switch(Cmd) {
  case gZclCmdMsg_DisplayMsgReq_c: 
  {
    InterPanAddrInfo_t addrInfo;    
    zclCmdMsg_DisplayMsgReq_t *pReq;
    zclSEGenericRxCmd_t rxCmd;
    /* prepare for response in the address info (back to sender) */
    PrepareInterPanForReply(&addrInfo, pIndication);
    pReq = (zclCmdMsg_DisplayMsgReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));  
    rxCmd.pSECmd = (void*)pReq;
    rxCmd.pRxFrag = NULL;
    pReq->MsgCtrl = pReq->MsgCtrl | 0x02; // set msg conf as beeing InterPan
    status = ZCL_ProcessDisplayMsg((addrInfoType_t*)&addrInfo, &rxCmd);
  }  
  break;  
 case gZclCmdMsg_CancelMsgReq_c:
  {
    zclCmdMsg_CancelMsgReq_t *pReq = (zclCmdMsg_CancelMsgReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(0, gZclUI_MsgCancelMessageReceived_c, 0, 0, pReq);
  }  
  break;   
  default:
    status = gZclUnsupportedClusterCommand_c;
  break;  
 }
 return status;
}

/*!
 * @fn 		zbStatus_t ZCL_InterPanMsgClusterServer(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the InterPAN Messaging Cluster server. 
 *
 */
zbStatus_t ZCL_InterPanMsgClusterServer
(
zbInterPanDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;

 (void) pDev;

 Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;

 switch(Cmd) {
  case gZclCmdMsg_GetLastMsgReq_c: 
  {
    zclInterPanGetLastMsgReq_t *pReq;
    pReq = (zclInterPanGetLastMsgReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(0, gZclUI_MsgGetLastMessageReceived_c, 0, 0, pReq);
  }  
  break;  
 case gZclCmdMsg_MsgConfReq_c:
  {
    zclCmdMsg_MsgConfReq_t *pReq = (zclCmdMsg_MsgConfReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(0, gZclUI_MsgMessageConfirmReceived_c, 0, 0, pReq);
  }  
  break;   
  default:
    status = gZclUnsupportedClusterCommand_c;
  break;  
 }
 return status;
}

/*!
 * @fn 		zbStatus_t ZclMsg_DisplayMsgReq(zclDisplayMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air a Display Message frame from the Messaging server. 
 *
 */
zbStatus_t ZclMsg_DisplayMsgReq
(
zclDisplayMsgReq_t *pReq
)
{
  uint8_t Length;
  /*1 is the length byte*/
  Length = (pReq->cmdFrame.length) + (sizeof(zclCmdMsg_DisplayMsgReq_t) - sizeof(uint8_t));	
  (void)ZclMsg_StoreMessage(pReq);
  return ZCL_SendServerReqSeqPassed(gZclCmdMsg_DisplayMsgReq_c, Length, (zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t ZclMsg_CancelMsgReq(zclCancelMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air a Cancel Message frame from the Messaging server. 
 *
 */
zbStatus_t ZclMsg_CancelMsgReq
(
zclCancelMsgReq_t *pReq
)
{
  mMsgLastMessage.EntryStatus = gEntryNotUsed_c;  
  return ZCL_SendServerReqSeqPassed(gZclCmdMsg_CancelMsgReq_c, sizeof(zclCmdMsg_CancelMsgReq_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZclMsg_GetLastMsgReq(zclGetLastMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Last Message frame from the Messaging client. 
 *
 */
zbStatus_t ZclMsg_GetLastMsgReq
(
zclGetLastMsgReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdMsg_GetLastMsgReq_c, 0 , (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZclMsg_MsgConf(zclMsgConfReq_t *pReq)
 *
 * @brief	Sends over-the-air a Message Confirmation frame from the Messaging client. 
 *
 */
zbStatus_t ZclMsg_MsgConf
(
zclMsgConfReq_t *pReq
)
{
  return ZCL_SendClientRspSeqPassed(gZclCmdMsg_CancelMsgReq_c, sizeof(zclCmdMsg_MsgConfReq_t),(zclGenericReq_t *)pReq);
}



#if gInterPanCommunicationEnabled_c 
/*!
 * @fn 		zbStatus_t ZclMsg_InterPanDisplayMsgReq(zclInterPanDisplayMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air an InterPAN Display Message frame from the Messaging server. 
 *
 */
zbStatus_t ZclMsg_InterPanDisplayMsgReq
(
zclInterPanDisplayMsgReq_t *pReq
)
{
  uint8_t Length;
  /*1 is the length byte*/
  Length = (pReq->cmdFrame.length) + (sizeof(zclCmdMsg_DisplayMsgReq_t) - sizeof(uint8_t));	
  
  return ZCL_SendInterPanServerReqSeqPassed(gZclCmdMsg_DisplayMsgReq_c, Length,pReq);	
}

/*!
 * @fn 		zbStatus_t ZclMsg_InterPanCancelMsgReq(zclInterPanCancelMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air an InterPAN Cancel Message frame from the Messaging server. 
 *
 */
zbStatus_t ZclMsg_InterPanCancelMsgReq
(
zclInterPanCancelMsgReq_t *pReq
)
{ 
  return ZCL_SendInterPanServerReqSeqPassed(gZclCmdMsg_CancelMsgReq_c, sizeof(zclCmdMsg_CancelMsgReq_t),pReq);
}

/*!
 * @fn 		zbStatus_t ZclMsg_InterPanGetLastMsgReq(zclInterPanGetLastMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air an InterPAN Get Last Message frame from the Messaging client. 
 *
 */
zbStatus_t ZclMsg_InterPanGetLastMsgReq
(
zclInterPanGetLastMsgReq_t *pReq
)
{
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdMsg_GetLastMsgReq_c, 0 , pReq);
}

/*!
 * @fn 		zbStatus_t ZclMsg_InterPanMsgConf(zclInterPanMsgConfReq_t *pReq)
 *
 * @brief	Sends over-the-air an InterPAN Message Confirmation frame from the Messaging client. 
 *
 */
zbStatus_t ZclMsg_InterPanMsgConf
(
zclInterPanMsgConfReq_t *pReq
)
{
  return ZCL_SendInterPanClientRspSeqPassed(gZclCmdMsg_CancelMsgReq_c, sizeof(zclCmdMsg_MsgConfReq_t),pReq);
}
#endif /*#if gInterPanCommunicationEnabled_c */


/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
static zbStatus_t ZclMsg_SendLastMessage(afAddrInfo_t* pAddrInfo)
{
  zclDisplayMsgReq_t *pReq;
  zbStatus_t status;
  ZCLTime_t startTime, endTime, currentTime = ZCL_GetUTCTime(pAddrInfo->srcEndPoint);
  Duration_t duration;
  
  startTime = OTA2Native32(mMsgLastMessage.EffectiveStartTime);
  duration = OTA2Native16(mMsgLastMessage.DurationInMinutes);
  endTime = startTime + (uint32_t)(duration * 60);
  if ((duration != 0xFFFF) && (currentTime > endTime))
  {
    mMsgLastMessage.EntryStatus = gEntryNotUsed_c;
    return gZclNotFound_c;
  } 
  
  pReq = MSG_Alloc(sizeof(zclDisplayMsgReq_t) + mMsgLastMessage.length);
  if(!pReq)
    return gZbNoMem_c;
  
  /* Add application payload*/
  FLib_MemCpy(&pReq->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  pReq->zclTransactionId =  ZbZclFoundationGlobals(gZclTransactionId)++;
  
  FLib_MemCpy(&pReq->cmdFrame.MsgID, &mMsgLastMessage.MsgID, sizeof(MsgId_t));
  pReq->cmdFrame.MsgCtrl = mMsgLastMessage.MsgCtrl;
  FLib_MemCpy(&pReq->cmdFrame.StartTime, &mMsgLastMessage.StartTime, sizeof(ZCLTime_t));
  
  if ((mMsgLastMessage.StartTime == 0) && (duration != 0xFFFF))
  {
    duration -= (Duration_t)((currentTime - startTime) / 60 ); 
    duration = OTA2Native16(duration);
  }
  FLib_MemCpy(&pReq->cmdFrame.DurationInMinutes, &duration, sizeof(Duration_t));
  
  /* Functionality is limited to a message of maximum 32 characters*/
  pReq->cmdFrame.length = mMsgLastMessage.length;
  FLib_MemCpy(&pReq->cmdFrame.msgData, &mMsgLastMessage.msgData, mMsgLastMessage.length);  
  
  /* Send request OTA*/
  status = ZclMsg_DisplayMsgReq(pReq);
  MSG_Free(pReq);
  return status;
}

/******************************************************************************/
/* Stores a limited received message in the Client Message Table              */
/******************************************************************************/
static void ZCL_StoreMessage(uint8_t *pMsgLocation, zclSEGenericRxCmd_t *pCmd)
{
  uint8_t tmpLength = 0;
  uint8_t len;
  uint8_t *pMsgData;
  zbRxFragmentedHdr_t *pRxFrag = NULL;
  
  zclSEGenericRxCmd_t * pRxCmd = (zclSEGenericRxCmd_t *)pCmd; 
  /* Point to Se cmd */
  zclCmdMsg_DisplayMsgReq_t *pReq = (zclCmdMsg_DisplayMsgReq_t *)pRxCmd->pSECmd;  
        
  /* Get the length and point to Msg data*/
  len = pReq->length;
  pMsgData = &pReq->msgData[0];
  
  /* If the received cmd is fragmented, point to first fragment */
  if (pRxCmd->pRxFrag)
  { 
    pRxFrag = pRxCmd->pRxFrag;
    /* Get the msg length from first fragment */
    len = pRxFrag->iDataSize-(sizeof(zclFrame_t)+(sizeof(zclCmdMsg_DisplayMsgReq_t)-sizeof(uint8_t)));
  }
  /* Get the received message */ 
  while(tmpLength < gZclMaxRcvdMsgLength)
  {
    if(len > gZclMaxRcvdMsgLength - tmpLength)
    {
      FLib_MemCpy(pMsgLocation + tmpLength, pMsgData, (gZclMaxRcvdMsgLength-tmpLength));
      tmpLength += (gZclMaxRcvdMsgLength-tmpLength);
    }            
    else
    {
      if (len > 0)
      {
        FLib_MemCpy(pMsgLocation + tmpLength, pMsgData, len );
        tmpLength += len;
      }
      /* Go to next fragment if a fragmented cmd was received */
      if (pRxFrag)
        pRxFrag = pRxFrag->pNextDataBlock;
      
      if (pRxFrag)
      {
        len = pRxFrag->iDataSize;
        pMsgData = pRxFrag->pData;
      }
      else
      {
        /* Fill rest of the array with " " */
        for(; tmpLength<gZclMaxRcvdMsgLength; tmpLength++)
        {
          *(pMsgLocation + tmpLength) =' ';
        }
      } /* if (pRxFrag) */
    }
  } /* while */
}

static zbStatus_t ZCL_ProcessDisplayMsg(addrInfoType_t* pAddrInfo, zclSEGenericRxCmd_t *pCmd)
{
  uint8_t i;
  ZCLTime_t   msgStartTime, currentTime, msgStopTime;
  Duration_t  durationInMinutes;
  zclMsg_MsgTableEntry_t *pEntry;
  zclCmdMsg_DisplayMsgReq_t *pReq = pCmd->pSECmd;
  
  msgStartTime = OTA2Native32(pReq->StartTime);
  durationInMinutes = OTA2Native16(pReq->DurationInMinutes);  
  
  /* Get the currentTime  */ 
  currentTime = ZCL_GetUTCTime(pAddrInfo->addrInfo.srcEndPoint);
  /* Event is started now???*/
  if(msgStartTime == 0x00000000)
  {
    /*Set the start time for the new event(keep the current time) */
    msgStartTime = currentTime;
    /*set it OTA value */
    pReq->StartTime = Native2OTA32(currentTime);
  }
  msgStopTime = msgStartTime + ((uint32_t)durationInMinutes * 60);
  
  /* Event had expired?? */
  if (msgStopTime <= currentTime)
  {
    return gZclSuccess_c; 
  }
  
  for (i = 0; i < gNumOfMsgTableEntry_c; i++)
  {
    pEntry = &maMsgClientTable[i];
    
     if(pEntry->EntryStatus == gNoMessage_c)
      break;
    
    if (FLib_Cmp2Bytes(&pEntry->addrInfo.addrInfo.dstAddr, &pAddrInfo->addrInfo.dstAddr))
    {
      if (FLib_MemCmp(&pEntry->MsgID, &pReq->MsgID, sizeof(MsgId_t)))
      {
        return gZclSuccess_c;
      }
      else
      {
        break;
      }
    }
  }
  
  if (i < gNumOfMsgTableEntry_c)
  {
    /*move the content to the left*/
    FLib_MemInPlaceCpy(pEntry + 1, pEntry, (gNumOfMsgTableEntry_c-i-1)* sizeof(zclMsg_MsgTableEntry_t));
    
    FLib_MemCpy(&pEntry->addrInfo, pAddrInfo, sizeof(addrInfoType_t));
    FLib_MemCpy(pEntry->MsgID, pReq, sizeof(zclCmdMsg_DisplayMsgReq_t));
    ZCL_StoreMessage(pEntry->msgData, pCmd);
    
    pEntry->EntryStatus = gMessagePending_c;
    BeeAppUpdateDevice(0, gZclUI_MsgDisplayMessageReceived_c, 0, 0, pEntry);
  } 
  
  return gZclSuccess_c; 
}

/*****************************************************************************/
static void MsgDisplayTimerCallBack(zbTmrTimerID_t tmrID)
{
   BeeAppUpdateDevice(0, gZclUI_MsgUpdateTimeInDisplay_c, 0, 0, NULL); 
   ZbTMR_StartSecondTimer(mMsgDisplayTimerID, 1, MsgDisplayTimerCallBack);
   (void)tmrID; /* Unused parameter. */
}

static void ZclMsg_StoreMessage(zclDisplayMsgReq_t *pReq)
{
  /* Functionality is limited to a message of maximum 32 characters*/
  mMsgLastMessage.EntryStatus = gEntryUsed_c;
  FLib_MemCpy(&mMsgLastMessage.MsgID, &pReq->cmdFrame.MsgID, sizeof(MsgId_t));
  mMsgLastMessage.MsgCtrl = pReq->cmdFrame.MsgCtrl;
  FLib_MemCpy(&mMsgLastMessage.StartTime, &pReq->cmdFrame.StartTime, sizeof(ZCLTime_t));
  FLib_MemCpy(&mMsgLastMessage.DurationInMinutes, &pReq->cmdFrame.DurationInMinutes, sizeof(Duration_t));
  if (pReq->cmdFrame.StartTime == 0)
  {
    uint32_t currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
    currentTime = Native2OTA32(currentTime);
    FLib_MemCpy(&mMsgLastMessage.EffectiveStartTime, &currentTime, sizeof(ZCLTime_t));    
  }
  else
    FLib_MemCpy(&mMsgLastMessage.EffectiveStartTime, &pReq->cmdFrame.StartTime, sizeof(ZCLTime_t));    
  mMsgLastMessage.length = FLib_GetMin(pReq->cmdFrame.length, gZclMaxRcvdMsgLength);
  FLib_MemCpy(&mMsgLastMessage.msgData, &pReq->cmdFrame.msgData, mMsgLastMessage.length);  
}

