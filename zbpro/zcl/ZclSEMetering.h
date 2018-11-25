/*! @file 	ZclSEMetering.h
 *
 * @brief	Types, definitions and prototypes for the Metering cluster implementation.
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
#ifndef _ZCLSEMETERING_H
#define _ZCLSEMETERING_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"
#include "AfApsInterface.h"
#include "ZCL.h"
#include "SEProfile.h"

/*Meter cluster definitions */
#ifndef gMetFormat_c
#define gMetFormat_c  0x28
#endif

#ifndef gProfIntrvPeriod_c
#define gProfIntrvPeriod_c              7  
#endif

#ifndef gMaxNumberOfPeriodsDelivered_c
#define gMaxNumberOfPeriodsDelivered_c  5
#endif

/* Number of snapshots that can be saved */
#ifndef gNum_Snapshots_c
#define gNum_Snapshots_c  2
#endif

/* Maximum number of entries that can fit into the snapshot payload */
#ifndef gNum_MaxSnapshotsPayloadEntries_c
#define gNum_MaxSnapshotsPayloadEntries_c  4
#endif

/* Maximum number of cached commands on the mirror */
#ifndef gNum_CachedCommands_c
#define gNum_CachedCommands_c  2
#endif

/* Maximum payload for cached commands on the mirror */
#ifndef gMaxCachedCommandPayload_c
#define gMaxCachedCommandPayload_c  20
#endif

/* Minimum delay in ms for the Mirror Report Attribute Response */
#ifndef gMirrorReportRsp_Delay_c
#define gMirrorReportRsp_Delay_c  25 
#endif
/* Fast Poll Rate of the BOMD (in ms)*/
#ifndef gBOMDAwakePollRate_c
#define gBOMDAwakePollRate_c  500
#endif

/* Number of polls made by a BOMD when in Awake mode*/
#ifndef gBOMDAwakeNumberOfPolls_c
#define gBOMDAwakeNumberOfPolls_c  3
#endif

#ifndef gASL_ZclSmplMet_MaxFastPollInterval_d
#define gASL_ZclSmplMet_MaxFastPollInterval_d 900//15min
#endif
#define gUpdateConsumption_c  10

#define gLastValue_c 2
#define gCurrentValue_c 0

#if(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_Daily_c)
#define gTimerValue_c 1440
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_60mins_c)
#define gTimerValue_c 60
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_30mins_c)  
#define gTimerValue_c 30
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_15mins_c)    
#define gTimerValue_c 15
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_10mins_c)      
#define gTimerValue_c 10
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_7dot5mins_c)
#define gTimerValue_c 450
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_5mins_c)
#define gTimerValue_c 300
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_2dot5mins_c)  
#define gTimerValue_c 150
#else 
#define gTimerValue_c 0
#endif

#ifndef gMetDefaultUpdatePeriod_c
#define gMetDefaultUpdatePeriod_c  0x1E
#endif

/* Command IDs for the Simple Metering Client*/
#define gZclCmdSmplMet_GetProfReq_c              0x00 /* O */
#define gZclCmdSmplMet_RequestMirrorRsp_c        0x01 /* O */
#define gZclCmdSmplMet_MirrorRemovedRsp_c        0x02 /* O */
#define gZclCmdSmplMet_ReqFastPollModeReq_c      0x03 /* O */
#define gZclCmdSmplMet_GetSnapshot_Req_c         0x04 /* O */
#define gZclCmdSmplMet_TakeSnapshot_Req_c        0x05 /* O */
#define gZclCmdSmplMet_MirrorReportAttr_Rsp_c    0x06 /* O */


/* Command IDs for the Simple Metering Server */
#define gZclCmdSmplMet_GetProfRsp_c              0x00 /* O */
#define gZclCmdSmplMet_RequestMirrorReq_c        0x01 /* O */
#define gZclCmdSmplMet_RemoveMirrorReq_c         0x02 /* O */
#define gZclCmdSmplMet_ReqFastPollModeRsp_c      0x03 /* O */
#define gZclCmdSmplMet_GetSnapshot_Rsp_c         0x04 /* O */

/* Status Field Values for zclCmdSimpleMetering_GetProfileResponse */
#define gSMGetProfRsp_SuccessStatus_c                   0x00 
#define gSMGetProfRsp_UndefIntrvChannelStatus_c         0x01 
#define gSMGetProfRsp_IntrvChannelNotSupportedStatus_c  0x02 
#define gSMGetProfRsp_InvalidEndTimeStatus_c            0x03 
#define gSMGetProfRsp_MorePeriodsRequestedStatus_c      0x04 
#define gSMGetProfRsp_NoIntrvsAvailableStatus_c         0x05

/* Snapshot Payload Type */
#define gZclMet_CurrSummDlvrdRcvdSnapshot_c      0x01
#define gZclMet_TOUInfoSetDlvrdSnapshot_c        0x02
#define gZclMet_TOUInfoSetRcvdSnapshot_c         0x03
#define gZclMet_BlockInfoSetDlvrdSnapshot_c      0x04
#define gZclMet_BlockInfoSetRcvdSnapshot_c       0x05

/* Snapshot Cause */
#define gZclMet_GeneralSnapshot_c                 0x0001
#define gZclMet_EndOfBillPeriodSnapshot_c         0x0002
#define gZclMet_EndOfBlockPeriodSnapshot_c        0x0004
#define gZclMet_TariffInfoChangeSnapshot_c        0x0008
#define gZclMet_CVChangeSnapshot_c                0x0040
#define gZclMet_CFChangeSnapshot_c                0x0080
#define gZclMet_TOUCalendarChangeSnapshot_c       0x0100
#define gZclMet_CPPSnapshot_c                     0x0200
#define gZclMet_ManualSnapshot_c                  0x0400

/* Notification Control Flags */
#define gZclMet_MirrorReportAttrRsp_c             0x01
#define gZclMet_ChangeReportingProfile_c          0x02

/* Notification Control Flags */
#define gZclMet_NewOTAFirmwareFlag_c              0x01
#define gZclMet_CBKEUpdateReqFlag_c               0x02
#define gZclMet_TimeSyncFlag_c                    0x04
#define gZclMet_NewPasswordFlag_c                 0x08
#define gZclMet_StayAwakeReqHANFlag_c             0x40
#define gZclMet_StayAwakeReqHESFlag_c             0x80

/* Price Notification Flags */
#define gZclMet_PublishPriceFlag_c                    0x01
#define gZclMet_PublishBlockPeriodFlag_c              0x02
#define gZclMet_PublishTariffInfoFlag_c               0x04
#define gZclMet_PublishCFFlag_c                       0x08
#define gZclMet_PublishCVFlag_c                       0x10
#define gZclMet_PublishCO2Flag_c                      0x20
#define gZclMet_PublishBillPeriodFlag_c               0x40
#define gZclMet_PublishConsolidatedBillFlag_c         0x80

/* Prepayment Notification Flags */
#define gZclMet_SelectAvailableEmergCredit_c          0x01

/* Get Profile Response command payload */
typedef PACKED_STRUCT zclCmdSmplMet_GetProfRsp_tag
{
  ZCLTime_t EndTime;
  uint8_t   Status;
  uint8_t   ProfIntrvPeriod;
  uint8_t   NumOfPeriodsDlvrd;
  IntrvForProfiling_t Intrvs[1];  
} zclCmdSmplMet_GetProfRsp_t;

/* Get Profile Response request */
typedef PACKED_STRUCT zclSmplMet_GetProfRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSmplMet_GetProfRsp_t cmdFrame;
} zclSmplMet_GetProfRsp_t;

typedef PACKED_STRUCT zclGetProfEntry_tag
{
  uint8_t IntrvChannel;
  uint32_t endTime;
  Consmp Intrv;
} zclGetProfEntry_t;

typedef PACKED_STRUCT zclCmdSmplMet_ReqFastPollModeRsp_tag
{
  uint8_t appliedUpdatePeriod;
  ZCLTime_t EndTime;
}zclCmdSmplMet_ReqFastPollModeRsp_t;

typedef PACKED_STRUCT zclSmplMet_ReqFastPollModeRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSmplMet_ReqFastPollModeRsp_t cmdFrame;
}zclSmplMet_ReqFastPollModeRsp_t;

/* Interval Channel Values for zclCmdSimpleMetering_GetProfileReq */
#define gIntrvChannel_ConsmpDlvrd_c 0x00 
#define gIntrvChannel_ConsmpRcvd_c  0x01

typedef PACKED_STRUCT zclCmdSmplMet_GetProfReq_tag
{
  uint8_t   IntrvChannel;
  ZCLTime_t EndTime;  
  uint8_t   NumOfPeriods;
} zclCmdSmplMet_GetProfReq_t;

typedef PACKED_STRUCT zclSmplMet_GetProfReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSmplMet_GetProfReq_t cmdFrame;
} zclSmplMet_GetProfReq_t;

/* Mirroring commands*/

typedef PACKED_STRUCT zclMet_RequestMirrorReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
} zclMet_RequestMirrorReq_t;

typedef PACKED_STRUCT zclMet_RemoveMirrorReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
} zclMet_RemoveMirrorReq_t;

typedef PACKED_STRUCT zclCmdMet_RequestMirrorRsp_tag
{
  uint16_t   EndPointID;
} zclCmdMet_RequestMirrorRsp_t;

typedef PACKED_STRUCT zclMet_RequestMirrorRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdMet_RequestMirrorRsp_t cmdFrame;
} zclMet_RequestMirrorRsp_t;

typedef PACKED_STRUCT zclCmdMet_MirrorRemovedRsp_tag
{
  uint16_t   EndPointID;
} zclCmdMet_MirrorRemovedRsp_t;

typedef PACKED_STRUCT zclMet_MirrorRemovedRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdMet_MirrorRemovedRsp_t cmdFrame;
} zclMet_MirrorRemovedRsp_t;

/* Reqest Fast Poll Mode command payload */
typedef PACKED_STRUCT zclSmplMet_FastPollModeReq_tag
{
  uint8_t FastPollUpdatePeriod; /* In seconds */
  uint8_t Duration; /* In minutes */
} zclSmplMet_FastPollModeReq_t;

typedef PACKED_STRUCT zclSmplMet_RequestMirrorRsp_tag
{
  zbEndPoint_t EndPointID;
} zclSmplMet_RequestMirrorRsp_t;

typedef PACKED_STRUCT zclSmplMet_MirrorRemovedRsp_tag
{
  zbEndPoint_t EndPointID;
} zclSmplMet_MirrorRemovedRsp_t;

typedef PACKED_STRUCT zclCmdSmplMet_ReqFastPollModeReq_tag
{
  uint8_t updatePeriod;
  uint8_t duration;
}zclCmdSmplMet_ReqFastPollModeReq_t;

typedef PACKED_STRUCT zclSmplMet_ReqFastPollModeReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSmplMet_ReqFastPollModeReq_t cmdFrame;
}zclSmplMet_ReqFastPollModeReq_t;

typedef PACKED_STRUCT zclCmdSmplMet_GetSnapshotReq_tag
{
  ZCLTime_t   startTime;
  uint8_t     numberOfSnapshots;
  uint16_t    snapshotCause;
}zclCmdSmplMet_GetSnapshotReq_t;

typedef PACKED_STRUCT zclSmplMet_GetSnapshotReq_tag
{
  afAddrInfo_t                    addrInfo;
  uint8_t                         zclTransactionId;
  zclCmdSmplMet_GetSnapshotReq_t  cmdFrame;
}zclSmplMet_GetSnapshotReq_t;

typedef PACKED_STRUCT zclMet_SendSnapshot_tag
{
  zclSmplMet_GetSnapshotReq_t   req;
  index_t                       currentIndex;
  index_t                       nextFrag;
}zclMet_SendSnapshot_t;

typedef PACKED_STRUCT zclCmdSmplMet_GetSnapshotRsp_tag
{
  SEEvtId_t           issuerEventId;
  ZCLTime_t           snapshotTime;
  uint8_t             commandIndex;
  uint16_t            snapshotCause;
  uint8_t             snapshotPayloadType;
  uint8_t             snapshotPayload[1];
}zclCmdSmplMet_GetSnapshotRsp_t;

typedef PACKED_STRUCT zclSmplMet_GetSnapshotRsp_tag
{
  afAddrInfo_t                      addrInfo;
  uint8_t                           zclTransactionId;
  zclCmdSmplMet_GetSnapshotRsp_t    cmdFrame;
}zclSmplMet_GetSnapshotRsp_t;

typedef PACKED_STRUCT zclSmplMet_CurrSummDlvrdRcvd_tag
{
  Summ_t    currentSummationDelivered;
  Summ_t    currentSummationReceived;
}zclSmplMet_CurrSummDlvrdRcvd_t;

typedef PACKED_STRUCT zclSmplMet_TouInfSet_tag
{
  uint8_t   nrOfTiersInUse;
  Summ_t    tierSummation[1];
}zclSmplMet_TouInfSet_t;

typedef PACKED_STRUCT zclSmplMet_BlockInfSet_tag
{
  uint8_t       nrOfTiersAndBlockThresholdsInUse;
  BlockInf_t    tierBlock[1];
}zclSmplMet_BlockInfSet_t;

typedef PACKED_STRUCT zclMet_TakeSnapshotReq_tag
{
  afAddrInfo_t                      addrInfo;
  uint8_t                           zclTransactionId;
}zclMet_TakeSnapshotReq_t;

typedef PACKED_STRUCT zclCmdSmplMet_MirrorReportAttrRsp_tag
{
  uint8_t       notifFlags;
  uint16_t      priceNotifFlags;
  uint8_t       calNotifFlags;
  uint16_t      prePayNotifFlags;
  uint8_t       devMgmtNotifFlags;
}zclCmdSmplMet_MirrorReportAttrRsp_t;

typedef PACKED_STRUCT zclSmplMet_MirrorReportAttrRsp_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdSmplMet_MirrorReportAttrRsp_t cmdFrame;
}zclSmplMet_MirrorReportAttrRsp_t;

typedef PACKED_STRUCT zclMet_MirroringTable_tag
{
  index_t       endPointListIdx;
  zbNwkAddr_t   aNwkAddress;
  bool_t        isActive;
}zclMet_MirroringTable_t;

typedef PACKED_STRUCT zclMet_CachedCmdsTable_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       payloadLen;
  uint8_t       aPayload[gMaxCachedCommandPayload_c];
  uint8_t       entryStatus;
}zclMet_CachedCmdsTable_t;

typedef PACKED_STRUCT zclMet_SnapshotsTable_tag
{
  SEEvtId_t     IssuerEvtID;
  ZCLTime_t     SnapshotTime;
  uint8_t       CommandIdx;
  uint16_t      SnapshotCause;
  uint8_t       SnapshotPayloadType;
  uint8_t       SnapshotPayload[gNum_MaxSnapshotsPayloadEntries_c * sizeof(Summ_t)];
  uint8_t       EntryStatus;
}zclMet_SnapshotsTable_t;

/*!
 * @fn 		void ZCL_SmplMet_MeterInit(void)
 *
 * @brief	Initializes the Metering Server functionality.
 *
 */
void ZCL_SmplMet_MeterInit(void);

/*!
 * @fn 		zbStatus_t ZclSmplMet_ClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Metering Cluster server. 
 *
 */
zbStatus_t ZclSmplMet_ClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZclSmplMet_ClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Metering Cluster client. 
 *
 */
zbStatus_t ZclSmplMet_ClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);

/*!
 * @fn 		zbStatus_t ZclMet_MirrorClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Metering Cluster client on the mirror endpoint. 
 *
 */
zbStatus_t ZclMet_MirrorClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);

/*!
 * @fn 		zbStatus_t ZclSmplMet_GetProfReq(zclSmplMet_GetProfReq_t *pReq)
 *
 * @brief	Sends a Get Profile Request from the Metering client. 
 *
 */
zbStatus_t ZclSmplMet_GetProfReq(zclSmplMet_GetProfReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMet_RemoveMirrorReq(zclMet_RemoveMirrorReq_t *pReq)
 *
 * @brief	Sends a Remove Mirror from the Metering server. 
 *
 */
zbStatus_t ZclMet_RemoveMirrorReq(zclMet_RemoveMirrorReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMet_RequestMirrorReq(zclMet_RequestMirrorReq_t *pReq)
 *
 * @brief	Sends a Request Mirror from the Metering server. 
 *
 */
zbStatus_t ZclMet_RequestMirrorReq(zclMet_RequestMirrorReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMet_RequestMirrorRsp(zclMet_RequestMirrorRsp_t *pReq)
 *
 * @brief	Sends a Request Mirror from the Metering client. 
 *
 */
zbStatus_t ZclMet_RequestMirrorRsp(zclMet_RequestMirrorRsp_t *pRsp);

/*!
 * @fn 		zbStatus_t ZclMet_MirrorRemovedRsp(zclMet_MirrorRemovedRsp_t *pReq)
 *
 * @brief	Sends a Mirror Removed from the Metering client. 
 *
 */
zbStatus_t ZclMet_MirrorRemovedRsp(zclMet_MirrorRemovedRsp_t *pRsp);

/*!
 * @fn 		zbStatus_t ZclSmplMet_ReqFastPollModeReq(zclSmplMet_ReqFastPollModeReq_t *pReq)
 *
 * @brief	Sends a Request Fast Poll Mode from the Metering client. 
 *
 */
zbStatus_t ZclSmplMet_ReqFastPollModeReq(zclSmplMet_ReqFastPollModeReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclSmplMet_ReqFastPollModeRsp(zclSmplMet_ReqFastPollModeRsp_t *pReq)
 *
 * @brief	Sends a Request Fast Poll Mode Response from the Metering server. 
 *
 */
zbStatus_t ZclSmplMet_ReqFastPollModeRsp(zclSmplMet_ReqFastPollModeRsp_t *pReq);

/*!
 * @fn 		zbStatus_t ZclSmplMet_AcceptFastPollModeReq(bool_t acceptFastPollReq)
 *
 * @brief	Sets the current device's accept fast poll mode. 
 *
 */
zbStatus_t ZclSmplMet_AcceptFastPollModeReq(bool_t acceptFastPollReq);

/*!
 * @fn 		uint16_t ZclSmplMet_GetFastPollRemainingTime()
 *
 * @brief	Gets the remaining time in seconds that the device must maintain
 *                fast poll mode. 
 *
 */
uint16_t ZclSmplMet_GetFastPollRemainingTime();

/*!
 * @fn 		zbStatus_t ZclSmplMet_GetProfRsp(zclSmplMet_GetProfRsp_t *pReq)
 *
 * @brief	Sends a Get Profile Response from the Metering client. 
 *
 */
zbStatus_t ZclSmplMet_GetProfRsp(zclSmplMet_GetProfRsp_t *pReq);

/*!
 * @fn 		zbStatus_t ZclSmplMet_GetSnapshotReq(zclSmplMet_GetSnapshotReq_t *pReq)
 *
 * @brief	Sends a Get Snapshot Request from the Metering client. 
 *
 */
zbStatus_t ZclSmplMet_GetSnapshotReq(zclSmplMet_GetSnapshotReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclSmplMet_GetSnapshotRsp(zclSmplMet_GetSnapshotRsp_t *pReq)
 *
 * @brief	Sends a Get Snapshot Response from the Metering server. 
 *
 */
zbStatus_t ZclSmplMet_GetSnapshotRsp(zclSmplMet_GetSnapshotRsp_t *pRsp);

/*!
 * @fn 		zbStatus_t ZclMet_TakeSnapshotRsp(zclMet_TakeSnapshotReq_t *pReq)
 *
 * @brief	Sends a Take Snapshot Response from the Metering client. 
 *
 */
zbStatus_t ZclMet_TakeSnapshotRsp(zclMet_TakeSnapshotReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZclMet_MirrorReportAttrRsp(zclSmplMet_MirrorReportAttrRsp_t *pReq)
 *
 * @brief	Sends a Mirror Report Attribute Response from the Metering client. 
 *
 */
zbStatus_t ZclMet_MirrorReportAttrRsp(zclSmplMet_MirrorReportAttrRsp_t *pRsp);

/*!
 * @fn 		zbStatus_t ZclMet_TakeSnapshot(void)
 *
 * @brief	Takes a manual snapshot on the Metering server. 
 *
 */
zbStatus_t ZclMet_TakeSnapshot(void);

/*!
 * @fn 		void ZCL_HandleSEMetGetSnapshot(void)
 *
 * @brief	Handles a Get Snapshot Request transaction on the Metering server. 
 *
 */
void ZCL_HandleSEMetGetSnapshot(void);

/*!
 * @fn 		zbEndPoint_t ZclMet_CreateAndRegisterMirrorEndpoint(zbNwkAddr_t aNwkAddress)
 *
 * @brief	Registers the mirror endpoint and initializes the mirror, given the meter's
 *			short address. 
 *
 */
zbEndPoint_t ZclMet_CreateAndRegisterMirrorEndpoint(zbNwkAddr_t aNwkAddress);

/*!
 * @fn 		void ZCL_InitMirroring(void)
 *
 * @brief	Initializes the mirroring information structure. 
 *
 */
void ZCL_InitMirroring(void);

/*!
 * @fn 		zclMet_MirroringTable_t* getMirroringTableEntry(zbNwkAddr_t aNwkAddress, zbEndPoint_t endPoint)
 *
 * @brief	Gets the mirroring information for a given Meter short address or mirror endpoint. 
 *
 */
zclMet_MirroringTable_t* getMirroringTableEntry(zbNwkAddr_t aNwkAddress, zbEndPoint_t endPoint);

/*!
 * @fn 		zclMet_MirroringTable_t* getFreeMirroringTableEntry(void)
 *
 * @brief	Gets a free entry in the Mirroring information table.
 *
 */
zclMet_MirroringTable_t* getFreeMirroringTableEntry(void);

/* Timers ID for Simple Metering */
extern zbTmrTimerID_t gGetProfileTestTimerID;
extern zbTmrTimerID_t gUpdatePowerConsumptionTimerID;
extern void GetProfileTestTimerCallBack(zbTmrTimerID_t tmrID);
void UpdatePowerConsumption(void);
#ifdef __cplusplus
}
#endif
#endif /* _ZCLSEMETERING_H */