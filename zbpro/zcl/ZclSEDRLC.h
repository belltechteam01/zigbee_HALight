/*! @file 	ZclSEDRLC.h
 *
 * @brief	Types, definitions and prototypes for the DRLC cluster implementation.
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

#ifndef _ZCLSEDRLC_H
#define _ZCLSEDRLC_H

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

#define gZclCmdDmndRspLdCtrl_LdCtrlEvtReq_c                 0x00 /* M */
#define gZclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_c           0x01 /* M */
#define gZclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_c       0x02 /* M */

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/*!
 * @fn 		void ZCL_LdCtrlClientInit(void)
 *
 * @brief	Initalize the DRLC Client functionality
 *
 */
void ZCL_LdCtrlClientInit(void);

/*!
 * @fn 		zbStatus_t ZCL_DmndRspLdCtrlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev)
 *
 * @brief	Processes the requests received on the DRLC server.
 *
 */
zbStatus_t ZCL_DmndRspLdCtrlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev); 

/*!
 * @fn 		zbStatus_t ZCL_DmndRspLdCtrlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev)
 *
 * @brief	Processes the requests received on the DRLC client.
 *
 */
zbStatus_t ZCL_DmndRspLdCtrlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev); 

/*!
 * @fn 		zbStatus_t ZclDmndRspLdCtrl_ReportEvtStatus(zclDmndRspLdCtrl_ReportEvtStatus_t *pReq)
 *
 * @brief	Sends the ReportEventStatus command over-the-air from the DRLC client.
 *
 */
zbStatus_t ZclDmndRspLdCtrl_ReportEvtStatus(zclDmndRspLdCtrl_ReportEvtStatus_t *pReq);

/*!
 * @fn 		zbStatus_t ZclDmndRspLdCtrl_LdCtrlEvtReq(zclDmndRspLdCtrl_LdCtrlEvtReq_t *pReq)
 *
 * @brief	Sends the LoadControlEvent command over-the-air from the DRLC server.
 *
 */
zbStatus_t ZclDmndRspLdCtrl_LdCtrlEvtReq(zclDmndRspLdCtrl_LdCtrlEvtReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclDmndRspLdCtrl_CancelLdCtrlEvtReq(zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t *pReq)
 *
 * @brief	Sends the CancelLoadControlEvent command over-the-air from the DRLC server.
 *
 */
zbStatus_t ZclDmndRspLdCtrl_CancelLdCtrlEvtReq(zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclDmndRspLdCtrl_CancelAllLdCtrlEvtReq(zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *pReq)
 *
 * @brief	Sends the CancelAllLoadControlEvents command over-the-air from the DRLC server.
 *
 */
zbStatus_t ZclDmndRspLdCtrl_CancelAllLdCtrlEvtReq(zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclDmndRspLdCtrl_GetScheduledEvtsReq(zclDmndRspLdCtrl_GetScheduledEvts_t *pReq)
 *
 * @brief	Sends the GetScheduledEvents command over-the-air from the DRLC client.
 *
 */
zbStatus_t ZclDmndRspLdCtrl_GetScheduledEvtsReq(zclDmndRspLdCtrl_GetScheduledEvts_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerLdCtrlEvents (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg)
 *
 * @brief	Store Load Control information received from the Provider.
 *    		The server side doesn't keep track of the  status, only stores the 
 *    		received events and take care that Nested and overlapping  commands not to occur.
 */	
zbStatus_t ZCL_ScheduleServerLdCtrlEvents(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg);

/*!
 * @fn 		void ZCL_DeleteServerScheduledEvents(void)
 *
 * @brief	Deletes all scheduled events on the DRLC server.
 *
 */
void ZCL_DeleteServerScheduledEvents(void);

/*!
 * @fn 		void ZCL_HandleScheduledEventNow(void)
 *
 * @brief	Handles a current scheduled DRLC event
 *    		
 */	
void ZCL_HandleScheduledEventNow(void);

/*!
 * @fn 		void ZCL_HandleReportEventsStatus(void)
 *
 * @brief	Handles the ReportEventsStatus command
 *    		
 */	
void ZCL_HandleReportEventsStatus(void);

/*!
 * @fn 		void ZCL_HandleGetScheduledLdCtlEvts(void)
 *
 * @brief	Handles the processing of the GetScheduledLoadControlEvent command.
 *
 */
void ZCL_HandleGetScheduledLdCtlEvts(void);

/*!
 * @fn 		uint8_t ZCL_SetOptStatusOnEvent(uint8_t *pEvtId, uint8_t optStatus) 
 *
 * @brief	Set "Opt In" or "Opt Out" Status for an event Id.
 *
 */
uint8_t ZCL_SetOptStatusOnEvent(uint8_t *pEvtId, uint8_t optStatus); 

/*!
 * @fn 		uint8_t ZCL_DeleteLdCtrlEvent(uint8_t *pEvtId) 
 *
 * @brief	Deletes a scheduled DRLC server event for an event Id.
 *
 */
uint8_t ZCL_DeleteLdCtrlEvent(uint8_t *pEvtId); 

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
);

/*!
 * @fn 		void ZCL_AcceptVoluntaryLdCrtlEvt(bool_t flag)
 *
 * @brief	Accepting voluntary load control events.
 *
 */
void ZCL_AcceptVoluntaryLdCrtlEvt(bool_t flag);

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*******************************************************************************
********************************************************************************
* Public memory declarations
********************************************************************************
*******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif /* ZCLSEDRLC_H */
