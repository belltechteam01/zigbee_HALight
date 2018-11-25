/*! @file 	ZclSE.h
 *
 * @brief	Types, definitions and prototypes for the ZigBee SE implementition.
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
 
#ifndef _ZCLSE_H
#define _ZCLSE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"
#include "AfApsInterface.h"
#include "ZCL.h"
#include "SEProfile.h"

/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/
/*Note all these variables should be allocted in msg system*/
/*total size of memory needed : 
42 22 21 22 21 16 16 22 2 = 205 bytes.
*/

/* Macros used for the Smart Energy state machine*/
#define  SE_SetState(state)  (gZclSEState = (state))
#define  SE_GetState()  gZclSEState

/* States for the Smart Energy state machine */
typedef enum 
{
  gZclSE_AutoJoiningState_c,
  gZclSE_InitState_c,
  gZclSE_ServiceDiscoveryForKEState_c,
  gZclSE_KeyEstabState_c,
  gZclSE_ServiceDiscoveryForMDUPairingState_c,
  gZclSE_MDUPairingState_c,
  gZclSE_ServiceDiscoveryState_c,
  gZclSE_BindServiceState_c,
  gZclSE_TimeServerDiscoveryState_c,
  gZclSE_RejoinAndRecoveryState_c,
  gZclSE_InitiateOverTheAirUpdate_c,
  gZclSE_ReadyState_c
} zclSEState_t;

/* ZCL SE main task */
#define gZclEvtKeyEstablishmentSuccess_c    (1 << 0)
#define gZclEvtKeyEstablishmentFailed_c     (1 << 1)
#define gZclEvtTrustCenterRunning_c         (1 << 2)
#define gZclEvtSEDeviceRunning_c            (1 << 3)
#define gZclEvtServiceDiscoveryFinished_c   (1 << 4)
#define gZclEvtServiceBindFinished_c        (1 << 5)
#define gZclEvtTimeServerQueried_c          (1 << 6) 
#define gZclEvtHandleSEClusterEvt_c         (1 << 7)
#define gZclEvtESIDiscoveryTimeOut_c        (1 << 8)
#define gZclEvtKeepAliveFailure_c           (1 << 9)
#define gZclEvtRejoinFailure_c              (1 << 10)
#define gZclEvtSEPeriodicScan_c             (1 << 11)
#define gZclEvtPairingFinished_c            (1 << 12)
#define gZclEvtQueryNextPairing_c           (1 << 13)

/* ZCL SE cluster events */

#define gZclEvtHandleLdCtrl_c                   (1 << 1)
#define gzclEvtHandleReportEventsStatus_c       (1 << 2)
#define gzclEvtHandleGetScheduledLdCtlEvts_c    (1 << 3)
#define gzclEvtHandlePriceClusterEvt_c          (1 << 4)
#define gZclEvtHandleGetSnapshot_c              (1 << 5)
#define gZclEvtHandleMDUPairingRspEvt_c         (1 << 6)  
#define gzclEvtHandleTouCalendarClusterEvt_c    (1 << 7)

/* ZCL SE Price cluster events */
#define gzclEvtHandleGetScheduledPrices_c (1 << 0)
#define gzclEvtHandleClientPrices_c (1 << 1)
#define gzclEvtHandlePublishPriceUpdate_c (1<< 2)
#define gzclEvtHandleGetBlockPeriods_c (1 << 4)
#define gzclEvtHandleGetTierLabels_c  (1 << 5)  
#define gzclEvtHandleGetConversionFactor_c (1 << 6)
#define gzclEvtHandleGetCalorificValue_c (1 << 7)
#define gzclEvtHandleClientBlockPeriod_c (1 << 8)
#define gzclEvtHandleGetBillingPeriod_c   (1 << 11)
#define gzclEvtHandleClientBillingPeriod_c   (1 << 13)
#define gzclEvtHandleGetConsolidatedBill_c   (1 << 14)
#define gzclEvtHandleClientConsolidatedBill_c   ((uint32_t)1 << 16)
#define gzclEvtHandleGetCO2Value_c ((uint32_t)1 << 18)
#define gzclEvtHandleGetTariffInformation_c ((uint32_t)1 << 19)
#define gzclEvtHandleGetPriceMatrix_c ((uint32_t)1 << 21)
#define gzclEvtHandleGetBlockThresholds_c ((uint32_t)1 << 22)
#define gzclEvtHandlePublishCPPEvent_c ((uint32_t)1 << 23)


/* ZCL SE Tou Calendar cluster events */
#define gzclEvtHandleTouCalendar_GetCalendar_c      (1 << 0)
#define gzclEvtHandleTouCalendar_GetDayProfiles_c   (1 << 1)
#define gzclEvtHandleTouCalendar_GetWeekProfiles_c  (1 << 2)
#define gzclEvtHandleTouCalendar_GetSeasons_c       (1 << 3)
#define gzclEvtHandleTouCalendar_GetSpecialDays_c   (1 << 4)


#define gZclSEMaxNoFailedKeepAlives_c  3

typedef struct ESPRegTable_tag{
EspRegisterDevice_t DevInfo;
zbEndPoint_t        KeyEstabEndPoint;
ZCLTime_t           DateTimeStamp;
uint8_t             DevStatus;
}ESPRegTable_t;

#define RegTable_DevStatusIntialState_c 0x01
#define RegTable_DevStatusKeyEstablishedState_c 0x02
#define RegTable_DevStatusStaleKeyState_c 0x03
#define RegTable_InvalidIndex_c 0xff

#define gInvalidTableIndex_c 0xFF

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/*!
 * @fn 		void SE_Init(void)
 *
 * @brief	Initialize the SE task and functionality
 *
 */
void SE_Init(void);

/*!
 * @fn 		void TS_SETask(uint16_t events)
 *
 * @brief	Task for the Smart Energy Profile. Used to signal ourselves of events.
 *
 */
void TS_SETask(uint16_t events);

/*!
 * @fn 		void ZCL_HandleSEClusterEvt(void)
 *
 * @brief	Handles any of the SE Clusters Events
 *
 */
void ZCL_HandleSEClusterEvt(void);

/*!
 * @fn 		void ZCL_ApplyECDSASign(uint8_t *pBufIn, uint8_t lenIn, uint8_t *pBufOut)
 *
 * @brief	Applies ECDSA Signing on the supplied input buffer.
 *
 */
void ZCL_ApplyECDSASign(uint8_t *pBufIn, uint8_t lenIn, uint8_t *pBufOut);

/*!
 * @fn 		int ECC_GetRandomDataFunc(unsigned char *buffer, unsigned long sz)
 *
 * @brief	Assigns random values to a supplied buffer
 *
 */
int ECC_GetRandomDataFunc(unsigned char *buffer, unsigned long sz);

/*!
 * @fn 		int ECC_HashFunc(unsigned char *digest, unsigned long sz, unsigned char *data)
 *
 * @brief	Applies MatyasMeyerOseasHash on the supplied data.
 *
 */
int ECC_HashFunc(unsigned char *digest, unsigned long sz, unsigned char *data);


/*!
 * @fn 		void AddUpdateDeviceDescriptor(zbNodeDescriptorResponse_t *pNodeDesc)
 *
 * @brief	Adds or updates the remote device descriptor information
 *
 */
void AddUpdateDeviceDescriptor(zbNodeDescriptorResponse_t *pNodeDesc);

/*!
 * @fn 		uint16_t GetMaxOutTransferSize(zbNwkAddr_t  aNwkAddr)
 *
 * @brief	Gets the Maximum Outgoing Transfer Size to a remote node
 *
 */
uint16_t GetMaxOutTransferSize(zbNwkAddr_t  aNwkAddr);

/*!
 * @fn 		bool_t GenerateKeyFromInstallCode(uint8_t length,uint8_t* pInstallationCode, uint8_t *pKey)
 *
 * @brief	This function computes an initial key based on an installation code.
 *
 * @param   [in]  		pInstallationCode	pointer to an installation code (max 144bit incl crc16)
 * @param   [in]  		length				length of installation key (in bytes incl CRC)
 * @param   [in,out]  	pKey				pointer to key (or hash)
 */
bool_t GenerateKeyFromInstallCode(uint8_t length,uint8_t* pInstallationCode, uint8_t *pKey);

/*!
 * @fn 		zbStatus_t ZCL_ESIReRegisterDevice(EspReRegisterDevice_t *Devinfo)
 *
 * @brief	Re-Registers an SE device locally on the Trust Center.
 *
 */
zbStatus_t ZCL_ESIReRegisterDevice(EspReRegisterDevice_t *DevInfo);

/*!
 * @fn 		zbStatus_t ZCL_ESPRegisterDevice(EspRegisterDevice_t *Devinfo)
 *
 * @brief	Registers an SE device locally on the Trust Center.
 *
 */
zbStatus_t ZCL_ESPRegisterDevice(EspRegisterDevice_t *Devinfo);

/*!
 * @fn 		zbStatus_t ZCL_ESPDeRegisterDevice(EspDeRegisterDevice_t *Devinfo)
 *
 * @brief	Deregisters an SE device locally on the Trust Center.
 *
 */
zbStatus_t ZCL_ESPDeRegisterDevice(EspDeRegisterDevice_t *Devinfo);

/*!
 * @fn 		index_t ZCL_AddToRegTable(EspRegisterDevice_t *Devinfo)
 *
 * @brief	Adds an SE device in the Registration Table on the 
 *			Trust Center.
 *
 */
index_t ZCL_AddToRegTable(EspRegisterDevice_t *Devinfo);

/*!
 * @fn 		index_t ZCL_FindIeeeInRegTable(zbIeeeAddr_t aExtAddr)
 *
 * @brief	Finds the index of an SE device in the Registration Table on the 
 *			Trust Center.
 *
 */
index_t ZCL_FindIeeeInRegTable(zbIeeeAddr_t aExtAddr);

/*!
 * @fn 		bool_t ZCL_CreateRegDevBackupEntry(uint8_t index, seRegDev_t* pEntry)
 *
 * @brief	Creates a backup of the registration table entry.
 *
 */
bool_t ZCL_CreateRegDevBackupEntry(uint8_t index, seRegDev_t* pEntry);

/*!
 * @fn 		uint8_t ZclSE_GetNumOfRegisteredDevices(void)
 *
 * @brief	Returns the number of SE registered devices on the Trust Center.
 *
 */ 
uint8_t ZclSE_GetNumOfRegisteredDevices(void);

/*!
 * @fn 		bool_t ZclSE_ValidateKey(zbIeeeAddr_t aExtAddr)
 *
 * @brief	Checks if the application key used to encrypt frames from the 
 *			input address is stale or preconfigured. 
 *
 */
bool_t ZclSE_ValidateKey(zbIeeeAddr_t aExtAddr);

/*!
 * @fn 		zbStatus_t ZCL_SE_TCSwapOut_SetDeviceData(seRegDev_t *pReq)
 *
 * @brief	Restores Trust Center data after Swap-Out
 *
 */
zbStatus_t ZCL_SE_TCSwapOut_SetDeviceData(seRegDev_t *pReq);

/*!
 * @fn 		void ZCL_SetKeyEstabComplete(zbEndPoint_t remoteEndPoint, zbIeeeAddr_t aExtAddr)
 *
 * @brief	Change status of new registered device to KeyEstablished.
 *
 */
void ZCL_SetKeyEstabComplete(zbEndPoint_t remoteEndPoint, zbIeeeAddr_t aExtAddr);

/*!
 * @fn 		void ZclSE_SendClusterEvt(uint32_t evtId)
 *
 * @brief	Sends the Price Cluster Event to the Task Scheduler 
 *
 */
void ZclSE_SendClusterEvt(uint32_t evtId);

/*!
 * @fn 		bool_t ZCL_CheckTimeServer(zbApsdeDataIndication_t *pIndication, uint8_t timeStatus)
 *
 * @brief	Checks whether the remote node is the authorative Time Server.
 *
 */
bool_t ZCL_CheckTimeServer(zbApsdeDataIndication_t *pIndication, uint8_t timeStatus);


#if gInterPanCommunicationEnabled_c
extern pfnInterPanIndication_t pfnInterPanServerInd;
extern pfnInterPanIndication_t pfnInterPanClientInd;
#endif
extern ESPRegisterDevFunc *pSE_ESPRegFunc;
extern ESPDeRegisterDevFunc *pSE_ESPDeRegFunc;
extern tsTaskID_t gSETaskID;
extern zclSEState_t gZclSEState;
extern const zbNwkAddr_t gTrustCenterAddr;
extern zbEndPoint_t gTCEndPoint;
//extern uint16_t        gZdoOrgPollRate; 
extern zbEndPoint_t gFTCEndPoint;
extern ZCLTimeServerAttrsRAM_t gZclTimeServerAttrs;
extern seESIDescriptor_t gZclSeESIList[gMaxNoOfESISupported_c];
extern seDeviceDescriptor_t gZclSeDeviceList[];
extern bool_t gZclSE_KeyEstablished;
extern ESPRegTable_t RegistrationTable[];
extern bool_t ZCl_SEClusterSecuritycheck(zbApsdeDataIndication_t *pIndication);
extern void ZCL_ESPInit(void);
extern void PrepareInterPanForReply(InterPanAddrInfo_t *pAdrrDest, zbInterPanDataIndication_t *pIndication);
extern void ZCL_Register_EspRegisterDeviceFunctions(ESPRegisterDevFunc *RegFunc,ESPDeRegisterDevFunc *DeRegFunc);
#ifdef __cplusplus
}
#endif
#endif /* _ZCLSE_H */
