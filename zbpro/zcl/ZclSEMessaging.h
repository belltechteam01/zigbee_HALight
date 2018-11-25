/*! @file 	ZclSEMessaging.h
 *
 * @brief	Types, definitions and prototypes for the Messaging cluster implementation.
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
 *			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   		contributors may be used to endorse or promote products derived from this
 *   		software without specific prior written permission.
 *
 *			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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

#ifndef _ZCLSEMESSAGING_H
#define _ZCLSEMESSAGING_H

#ifdef __cplusplus
    extern "C" {
#endif
/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

#define gZclCmdMsg_DisplayMsgReq_c              0x00 /* M */
#define gZclCmdMsg_CancelMsgReq_c               0x01 /* M */


#define gZclSEMsgCtrl_SendMsgThroughNormalCmdFunctionToClient_c   0x00 
#define gZclSEMsgCtrl_AdditionallyPassMsgOntoTheAnonDataCluster_c 0x01
#define gZclSEMsgCtrl_OnlySendMsgThroughTheAnonDataCluster_c      0x02

#define gZclSEMsgCtrl_MsgUrgency_Low_c       0x00 
#define gZclSEMsgCtrl_MsgUrgency_Medium_c    0x04 
#define gZclSEMsgCtrl_MsgUrgency_High_c      0x08 
#define gZclSEMsgCtrl_MsgUrgency_Crit_c      0x0C 

#define gZclSEMsgCtrl_MsgConfNotRequired_c      0x00 
#define gZclSEMsgCtrl_MsgConfRequired_c         0x80 

#define gNoMessage_c 0
#define gMessagePending_c 1
#define gMessageDisplayed_c 2
#define gMessageCanceled_c 3

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef PACKED_STRUCT zclCmdMsg_DisplayMsgReq_tag
{
  MsgId_t       MsgID;
  uint8_t       MsgCtrl;
  ZCLTime_t     StartTime;
  Duration_t    DurationInMinutes;
  //zclStr32_t    Msg;
  uint8_t     length;
  uint8_t     msgData[1];
} zclCmdMsg_DisplayMsgReq_t;

typedef PACKED_STRUCT  zclDisplayMsgReq_tag 
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdMsg_DisplayMsgReq_t cmdFrame;
} zclDisplayMsgReq_t;

typedef PACKED_STRUCT  zclInterPanDisplayMsgReq_tag 
{  
  InterPanAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdMsg_DisplayMsgReq_t cmdFrame;
} zclInterPanDisplayMsgReq_t;

typedef PACKED_STRUCT zclCmdMsg_CancelMsgReq_tag
{
  MsgId_t       MsgID;
  uint8_t       MsgCtrl;
} zclCmdMsg_CancelMsgReq_t;

typedef PACKED_STRUCT  zclCancelMsgReq_tag 
{  
  afAddrInfo_t addrInfo; 
  uint8_t zclTransactionId;
  zclCmdMsg_CancelMsgReq_t cmdFrame;
} zclCancelMsgReq_t;

typedef PACKED_STRUCT  zclInterPanCancelMsgReq_tag 
{  
  InterPanAddrInfo_t addrInfo; 
  uint8_t zclTransactionId;
  zclCmdMsg_CancelMsgReq_t cmdFrame;
} zclInterPanCancelMsgReq_t;

#define gZclCmdMsg_GetLastMsgReq_c              0x00 /* M */
#define gZclCmdMsg_MsgConfReq_c         0x01 /* M */

typedef PACKED_STRUCT zclCmdMsg_MsgConfReq_tag
{
  MsgId_t   MsgID;
  ZCLTime_t ConfTime;  
} zclCmdMsg_MsgConfReq_t;

typedef PACKED_STRUCT zclGetLastMsgReq_tag
{
  afAddrInfo_t         addrInfo;  /* IN: source/dst address, cluster, etc... */
  uint8_t zclTransactionId;
} zclGetLastMsgReq_t;

typedef PACKED_STRUCT zclInterPanGetLastMsgReq_tag
{
  InterPanAddrInfo_t         addrInfo;  /* IN: source/dst address, cluster, etc... */
  uint8_t zclTransactionId;
} zclInterPanGetLastMsgReq_t;

typedef PACKED_STRUCT  zclMsgConfReq_tag 
{  
  afAddrInfo_t addrInfo; 
  uint8_t zclTransactionId;
  zclCmdMsg_MsgConfReq_t cmdFrame;
} zclMsgConfReq_t;

typedef PACKED_STRUCT  zclInterPanMsgConfReq_tag 
{  
  InterPanAddrInfo_t addrInfo; 
  uint8_t zclTransactionId;
  zclCmdMsg_MsgConfReq_t cmdFrame;
} zclInterPanMsgConfReq_t;

typedef PACKED_STRUCT zclMsg_MsgTableEntry_tag
{
  addrInfoType_t        addrInfo;    
  uint8_t               zclTransactionId;
  MsgId_t               MsgID;
  uint8_t               MsgCtrl;
  ZCLTime_t             StartTime;
  Duration_t            DurationInMinutes;
  uint8_t               length;
  uint8_t               msgData[gZclMaxRcvdMsgLength];
  ZCLTime_t     		EffectiveStartTime;
  uint8_t               EntryStatus; 
}zclMsg_MsgTableEntry_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/*!
 * @fn 		void ZCL_MsgInit(void)
 *
 * @brief	Initializes the Messaging client functionality.
 *
 */
void ZCL_MsgInit(void);

/*!
 * @fn 		zbStatus_t ZCL_MsgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Messaging Cluster server. 
 *
 */
zbStatus_t ZCL_MsgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZCL_MsgClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Messaging Cluster client. 
 *
 */
zbStatus_t ZCL_MsgClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZCL_InterPanMsgClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the InterPAN Messaging Cluster client. 
 *
 */
zbStatus_t ZCL_InterPanMsgClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZCL_InterPanMsgClusterServer(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the InterPAN Messaging Cluster server. 
 *
 */
zbStatus_t ZCL_InterPanMsgClusterServer(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZclMsg_DisplayMsgReq(zclDisplayMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air a Display Message frame from the Messaging server. 
 *
 */
zbStatus_t ZclMsg_DisplayMsgReq(zclDisplayMsgReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMsg_CancelMsgReq(zclCancelMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air a Cancel Message frame from the Messaging server. 
 *
 */
zbStatus_t ZclMsg_CancelMsgReq(zclCancelMsgReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMsg_GetLastMsgReq(zclGetLastMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Last Message frame from the Messaging client. 
 *
 */
zbStatus_t ZclMsg_GetLastMsgReq(zclGetLastMsgReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMsg_MsgConf(zclMsgConfReq_t *pReq)
 *
 * @brief	Sends over-the-air a Message Confirmation frame from the Messaging client. 
 *
 */
zbStatus_t ZclMsg_MsgConf(zclMsgConfReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMsg_InterPanDisplayMsgReq(zclInterPanDisplayMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air an InterPAN Display Message frame from the Messaging server. 
 *
 */
zbStatus_t ZclMsg_InterPanDisplayMsgReq(zclInterPanDisplayMsgReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMsg_InterPanCancelMsgReq(zclInterPanCancelMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air an InterPAN Cancel Message frame from the Messaging server. 
 *
 */
zbStatus_t ZclMsg_InterPanCancelMsgReq(zclInterPanCancelMsgReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMsg_InterPanGetLastMsgReq(zclInterPanGetLastMsgReq_t *pReq)
 *
 * @brief	Sends over-the-air an InterPAN Get Last Message frame from the Messaging client. 
 *
 */
zbStatus_t ZclMsg_InterPanGetLastMsgReq(zclInterPanGetLastMsgReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMsg_InterPanMsgConf(zclInterPanMsgConfReq_t *pReq)
 *
 * @brief	Sends over-the-air an InterPAN Message Confirmation frame from the Messaging client. 
 *
 */
zbStatus_t ZclMsg_InterPanMsgConf(zclInterPanMsgConfReq_t *pReq);

/*******************************************************************************
********************************************************************************
* Public memory declarations
********************************************************************************
*******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif /* _ZCLSEMESSAGING_H */
