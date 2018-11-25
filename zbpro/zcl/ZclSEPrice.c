/*! @file	ZclSEPrice.c
 *
 * @brief	This source file describes specific functionality implemented
 *			for the Price cluster.
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
#include "ZdoApsInterface.h"
#include "HaProfile.h"
#include "ZCLGeneral.h"
#include "ZCLFoundation.h"
#include "zcl.h"
#include "SEProfile.h"
#include "zclSEPrice.h"
#include "zclSE.h"
#include "display.h"
#include "eccapi.h"
#include "Led.h"
#include "ZdoApsInterface.h"
#include "ASL_ZdpInterface.h"
#include "ApsMgmtInterface.h"
#include "beeapp.h"

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

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_Optionals_d
static void DeleteClientTariffInformation(ProviderID_t providerID, SEEvtId_t issuerTariffID, uint8_t tariffType);
static void DeleteClientBlockThresholds(ProviderID_t providerID, SEEvtId_t issuerTariffID);
static void DeleteClientPriceMatrix(ProviderID_t providerID, SEEvtId_t issuerTariffID);
static void DeleteClientTierLabels(ProviderID_t providerID, SEEvtId_t issuerTariffID);
#endif
#if gASL_ZclPrice_Optionals_d || gASL_ZclSE_12_Features_d
static ZCLTime_t Zcl_GetDuration(Duration24_t duration);
static void Zcl_SubtractDuration(Duration24_t aDuration, uint32_t timeInMinutes);
#endif

/* Price cluster */
static zbStatus_t ZCL_ProcessClientPublishPrice(zclCmdPrice_PublishPriceRsp_t *pMsg, bool_t IsExtended);
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_ProcessClientPublishBlockPeriod(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg);
#endif 
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_ProcessClientPublishConversionFactor(zclCmdPrice_PublishConversionFactorRsp_t *pMsg);
static zbStatus_t ZCL_ProcessClientPublishCalorificValue(zclCmdPrice_PublishCalorificValueRsp_t *pMsg);
#endif
static zbStatus_t ZCL_ProcessGetCurrPriceReq
(
addrInfoType_t *pAddrInfo,
zclCmdPrice_GetCurrPriceReq_t * pGetCurrPrice,
bool_t IsInterPanFlag
);
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_ProcessGetScheduledPricesReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetScheduledPricesReq_t * pGetScheduledPrice, 
bool_t IsInterPanFlag
);
static zbStatus_t ZCL_ProcessGetBlockPeriodsReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t      tsq,
  zclCmdPrice_GetBlockPeriodsReq_t * pGetBlockPeriods
);
#endif
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_ProcessGetConversionFactorReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_GetConversionFactorReq_t * pGetConversionFactor
);
static zbStatus_t ZCL_ProcessGetCalorificValueReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_GetCalorificValueReq_t * pGetCalorificValue
);
#endif
#if gASL_ZclSE_12_Features_d
static zbStatus_t ZCL_ProcessGetCO2ValueReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t      tsq,
  zclCmdPrice_GetCO2ValueReq_t * pGetCO2Value
);

static zbStatus_t ZCL_ProcessGetTariffInformationReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t         tsq,
  zclCmdPrice_GetTariffInformationReq_t * pGetTariffInformation
);

static zbStatus_t ZCL_ProcessGetPriceMatrixReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t         tsq,
  zclCmdPrice_GetPriceMatrixReq_t * pGetPriceMatrix
);

static zbStatus_t ZCL_ProcessGetBlockThresholdsReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t         tsq,
  zclCmdPrice_GetBlockThresholdsReq_t * pGetBlockThresholds
);

static zbStatus_t ZCL_ProcessGetTierLabelsReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t         tsq,
  zclCmdPrice_GetTierLabelsReq_t * pGetTierLabels
);

static zbStatus_t ZCL_ProcessGetBillingPeriodReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t         tsq,
  zclCmdPrice_GetBillingPeriodReq_t * pGetBillingPeriod
);

static zbStatus_t ZCL_ProcessCPPEventRsp
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t         tsq,  
  zclCmdPrice_CPPEventRsp_t * pCPPEvent
);

static zbStatus_t ZCL_ProcessGetConsolidatedBillReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t         tsq,  
  zclCmdPrice_GetConsolidatedBillReq_t * pGetConsolidatedBill
);

static zbStatus_t ZCL_ProcessGetCurrencyConversionReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t         tsq
);

static uint8_t AddTariffInformationInTable
(
publishTariffInformationEntry_t *pTable,
uint8_t len, 
zclCmdPrice_PublishTariffInformationRsp_t *pMsg
);

static uint8_t AddBillingPeriodsInTable
(
publishBillingPeriodEntry_t *pTable,
uint8_t len,
zclCmdPrice_PublishBillingPeriodRsp_t *pMsg
);

static uint8_t AddCO2ValueInTable
(
publishCO2ValueEntry_t *pTable,
uint8_t len,
zclCmdPrice_PublishCO2ValueRsp_t *pMsg
);

static uint8_t AddConsolidatedBillsInTable
(
publishConsolidatedBillEntry_t *pTable,
uint8_t len,
zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg
);

static uint8_t AddCPPEventsInTable
(
  publishCPPEventEntry_t *pTable,
  uint8_t len,
  zclCmdPrice_PublishCPPEventRsp_t *pMsg
);
#endif

#if gASL_ZclPrice_Optionals_d
static uint8_t AddBlockPeriodsInTable
(
  publishBlockPeriodEntry_t *pTable,
  uint8_t len,
  zclCmdPrice_PublishBlockPeriodRsp_t *pMsg
);
static uint8_t AddCalorificValueInTable
(
  publishCalorificValueEntry_t *pTable,
  uint8_t len,
  zclCmdPrice_PublishCalorificValueRsp_t *pMsg
);
static uint8_t AddConversionFactorInTable
(
  publishConversionFactorEntry_t *pTable,
  uint8_t len,
  zclCmdPrice_PublishConversionFactorRsp_t *pMsg
);
#endif

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_Optionals_d
static uint8_t AddCreditPaymentsInTable(publishCreditPaymentEntry_t *pTable, uint8_t len, zclCmdPrice_PublishCreditPaymentRsp_t *pMsg);
static uint8_t AddCurrencyConversionInTable
(
publishCurrencyConversionEntry_t *pTable,
uint8_t len, 
zclCmdPrice_PublishCurrencyConversionRsp_t *pMsg
);
static zbStatus_t ZCL_ProcessClientPublishCreditPayment(zclCmdPrice_PublishCreditPaymentRsp_t *pMsg);
#endif
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_SendPublishBlockPeriod(addrInfoType_t *pAddrInfo, zclTSQ_t tsq, publishBlockPeriodEntry_t *pMsg);
#endif
static zbStatus_t ZCL_SendPublishPrice(addrInfoType_t *pAddrInfo, publishPriceEntry_t *pMsg, bool_t IsInterPanFlag);
zbStatus_t ZCL_PriceAck(zclPrice_PriceAck_t *pReq);
static zbStatus_t ZCL_SendPriceAck(zbApsdeDataIndication_t *pIndication);
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_SendPublishConversionFactor(addrInfoType_t *pAddrInfo, zclTSQ_t tsq, publishConversionFactorEntry_t * pMsg);
static zbStatus_t ZCL_SendPublishCalorificValue(addrInfoType_t *pAddrInfo, zclTSQ_t tsq, publishCalorificValueEntry_t * pMsg);
#endif
static uint8_t AddPriceInTable(publishPriceEntry_t *pTable, uint8_t len, zclCmdPrice_PublishPriceRsp_t *pMsg, bool_t IsExtended);
static uint8_t CheckForPriceUpdate(zclCmdPrice_t *pMsg, publishPriceEntry_t *pTable, uint8_t len, bool_t IsExtended);
#if gASL_ZclPrice_Optionals_d
static uint8_t CheckForBlockPeriodUpdate(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg, publishBlockPeriodEntry_t *pTable, uint8_t len);
#endif
#if gASL_ZclSE_12_Features_d
static zbStatus_t ZCL_SendPublishTariffInformation(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishTariffInformationEntry_t * pMsg);
#if gASL_ZclPrice_PublishPriceMatrixRsp_d
static zbStatus_t ZCL_SendPublishPriceMatrix(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishPriceMatrixEntry_t * pMsg);
#endif
#if gASL_ZclPrice_PublishBlockThresholdsRsp_d
static zbStatus_t ZCL_SendPublishBlockThresholds(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishBlockThresholdsEntry_t * pMsg);
static void ZCL_HandleGetBlockThresholds(void);
#endif
#if gASL_ZclPrice_PublishTierLabelsRsp_d
static zbStatus_t ZCL_SendPublishTierLabels(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishTierLabelsEntry_t * pMsg);
static void ZCL_HandleGetTierLabels(void);
#endif
static uint8_t CheckForBillingPeriodUpdate(zclCmdPrice_PublishBillingPeriodRsp_t *pMsg, publishBillingPeriodEntry_t *pTable, uint8_t len);
static zbStatus_t ZCL_SendPublishBillingPeriod(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishBillingPeriodEntry_t * pMsg);
static uint8_t CheckForConsolidatedBillUpdate(zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg, publishConsolidatedBillEntry_t *pTable, uint8_t len);
static zbStatus_t ZCL_SendPublishConsolidatedBill(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishConsolidatedBillEntry_t * pMsg);
static zbStatus_t ZCL_SendPublishCPPEvent(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishCPPEventEntry_t * pMsg);
static uint8_t CheckForCreditPaymentUpdate(zclCmdPrice_PublishCreditPaymentRsp_t *pMsg, publishCreditPaymentEntry_t *pTable, uint8_t len);
static zbStatus_t ZCL_SendPublishCurrencyConversion(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishCurrencyConversionEntry_t * pMsg);
#endif
#if gASL_ZclPrice_GetCurrPriceReq_d || gASL_ZclPrice_GetSheduledPricesReq_d 
static void TimerClientPriceCallBack(zbTmrTimerID_t timerID);
#endif

static zbStatus_t ZCL_ProcessGetCurrPriceReq
(
addrInfoType_t *pAddrInfo,
zclCmdPrice_GetCurrPriceReq_t * pGetCurrPrice,
bool_t IsInterPanFlag
);

#if gInterPanCommunicationEnabled_c
static zbStatus_t ZCL_SendInterPriceAck(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZCL_InterPriceAck(zclPrice_InterPriceAck_t *pReq);
static void StoreInterPanAddr(InterPanAddrInfo_t *pAddrInfo);
static void InterPanJitterTimerCallBack(zbTmrTimerID_t timerID);
#endif /* #if gInterPanCommunicationEnabled_c */

#if gASL_ZclPrice_Optionals_d
static uint8_t FindNextCalorificValueEvent(void);
static uint8_t FindNextConversionFactorEvent(void);
static void ZCL_HandleClientBlockPeriod(void);
static void ZCL_HandleConversionFactorUpdate(index_t entryIdx);
static void ZCL_HandleCalorificValueUpdate(index_t entryIdx);
#if gASL_ZclPrice_PublishPriceRsp_d
static void ZCL_HandleGetScheduledPrices(void);
#endif
static void ZCL_HandleBlockPeriodUpdate(index_t entryIdx);
#if gASL_ZclPrice_PublishBlockPeriodsRsp_d
static void ZCL_HandleGetBlockPeriods(void);
#endif
#if gASL_ZclPrice_PublishConversionFactorRsp_d
static void ZCL_HandleGetConversionFactor(void);
#endif
#if gASL_ZclPrice_PublishCalorificValueRsp_d
static void ZCL_HandleGetCalorificValue(void);
#endif
#endif

#if gASL_ZclPrice_GetCurrPriceReq_d || gASL_ZclPrice_GetSheduledPricesReq_d  
static void ZCL_HandleClientPrices(void);
#endif
static void ZCL_HandlePublishPriceUpdate(void);

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_Optionals_d
static uint8_t FindNextBillingPeriodEvent(void);
static zbStatus_t ZCL_ProcessClientPublishCO2Value(zclCmdPrice_PublishCO2ValueRsp_t *pMsg);
static zbStatus_t ZCL_ProcessClientPublishTariffInformation(zclCmdPrice_PublishTariffInformationRsp_t *pMsg);
static zbStatus_t ZCL_ProcessClientPublishCurrencyConversion(zclCmdPrice_PublishCurrencyConversionRsp_t *pMsg);
static zbStatus_t ZCL_ProcessClientPublishPriceMatrix(zclCmdPrice_PublishPriceMatrixRsp_t *pMsg, uint8_t payloadLen);
static zbStatus_t ZCL_ProcessClientPublishBlockThresholds(zclCmdPrice_PublishBlockThresholdsRsp_t *pMsg, uint8_t payloadLen);
static zbStatus_t ZCL_ProcessClientPublishTierLabels(zclCmdPrice_PublishTierLabelsRsp_t *pMsg, uint8_t payloadLen);
static zbStatus_t ZCL_ProcessClientPublishConsolidatedBill(zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg);
static zbStatus_t ZCL_ProcessClientPublishCPPEvent(zclCmdPrice_PublishCPPEventRsp_t *pMsg);
static zbStatus_t ZCL_ProcessClientPublishBillingPeriod(zclCmdPrice_PublishBillingPeriodRsp_t *pMsg);
#if gASL_ZclPrice_PublishPriceMatrixRsp_d
static void ZCL_HandleGetPriceMatrix(void);
#endif
static uint8_t FindNextTariffInformationEvent(void);
#if gASL_ZclPrice_PublishCO2ValueRsp_d
static void ZCL_HandleGetCO2Value(void);
static zbStatus_t ZCL_SendPublishCO2Value(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishCO2ValueEntry_t * pMsg);
#endif
#if gASL_ZclPrice_PublishTariffInformationRsp_d
static void ZCL_HandleGetTariffInformation(void);
#endif
static uint8_t FindServerTariffInformationEntryByTariffId(SEEvtId_t *pIssuerTariffId);
static uint8_t FindClientTariffInformationEntry(SEEvtId_t providerID, SEEvtId_t issuerTariffID);
#if gASL_ZclPrice_PublishBillingPeriodRsp_d
static void ZCL_HandleGetBillingPeriod(void);
#endif
static void ZCL_HandleClientBillingPeriod(void);
static void ZCL_HandleBillingPeriodUpdate(index_t iEntryIdx);
static void ZCL_HandleCPPEventUpdate(index_t iEntryIdx);
static void ZCL_HandleCO2ValueUpdate(index_t iEntryIdx);
static void ZCL_HandleTariffInformationUpdate(index_t iEntryIdx);
static void ZCL_HandleCurrencyConversionUpdate(void);
#if gASL_ZclPrice_PublishConsolidatedBillRsp_d
static void ZCL_HandleGetConsolidatedBill(void);
#endif
static void ZCL_HandleClientConsolidatedBill(void);
static void ZCL_HandleConsolidatedBillUpdate(index_t iEntryIdx);
static void ZCL_HandleCreditPaymentUpdate(index_t iEntryIdx);
#endif
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
/* Save information used to respond to the GetBlockPeriod request*/
typedef PACKED_STRUCT getBlockPeriod_tag
{  
  addrInfoType_t addrInfo;
  zclTSQ_t      tsq;
  ZCLTime_t     startTime;
  uint8_t       numOfEvtsRemaining;
  uint8_t       tariffType;
  uint8_t       index;
} getBlockPeriod_t;

typedef PACKED_STRUCT getConversionFactor_tag
{  
  addrInfoType_t addrInfo;
  zclTSQ_t      tsq;
  ZCLTime_t     startTime;
  uint8_t       numOfEvtsRemaining;
  uint8_t       index;  
} getConversionFactor_t;

typedef PACKED_STRUCT getCalorificValue_tag
{  
  addrInfoType_t addrInfo;
  zclTSQ_t      tsq;
  ZCLTime_t     startTime;
  uint8_t       numOfEvtsRemaining;
  uint8_t       index;  
} getCalorificValue_t;

typedef PACKED_STRUCT getTariffInformation_tag
{  
  afAddrInfo_t  addrInfo;
  zclTSQ_t      tsq;
  ZCLTime_t     earliestStartTime;
  uint32_t      minIssuerEvtId;
  uint8_t       tariffType;
  uint8_t       numOfEvtsRemaining;
  uint8_t       index;
}getTariffInformation_t;

typedef PACKED_STRUCT getCO2Value_tag
{  
  afAddrInfo_t  addrInfo;
  zclTSQ_t      tsq;
  ZCLTime_t     earliestStartTime;
  uint32_t      minIssuerEvtId;
  uint8_t       tariffType;
  uint8_t       numOfEvtsRemaining;
  uint8_t       index;
} getCO2Value_t;


typedef PACKED_STRUCT getPriceMatrix_tag
{  
  afAddrInfo_t    addrInfo;
  zclTSQ_t        tsq;
  uint8_t         Index;
  uint8_t         CmdIndex;
  uint8_t         Offset;
  uint8_t         Length;
} getPriceMatrix_t;

typedef PACKED_STRUCT getBlockThresholds_tag
{  
  afAddrInfo_t  addrInfo;
  zclTSQ_t      tsq;
  uint8_t       Index;
  uint8_t       CmdIndex;
  uint8_t       Offset;
  uint8_t       Length;
} getBlockThresholds_t;

typedef PACKED_STRUCT getTierLabels_tag
{  
  afAddrInfo_t    addrInfo;
  zclTSQ_t        tsq;
  uint8_t         Index;
  uint8_t         CmdIndex;
  uint8_t         Offset;
  uint8_t         Length;
} getTierLabels_t;

typedef PACKED_STRUCT getBillingPeriod_tag
{  
  afAddrInfo_t  addrInfo;
  zclTSQ_t      tsq;
  ZCLTime_t     earliestStartTime;
  uint32_t      minIssuerEvtId;
  uint8_t       tariffType;
  uint8_t       numOfEvtsRemaining;
  uint8_t       index;
} getBillingPeriod_t;

typedef PACKED_STRUCT getConsolidatedBill_tag
{  
  afAddrInfo_t  addrInfo;
  zclTSQ_t      tsq;
  ZCLTime_t     earliestStartTime;
  uint32_t      minIssuerEvtId;
  uint8_t       tariffType;
  uint8_t       numOfEvtsRemaining;
  uint8_t       index;
} getConsolidatedBill_t;
/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
/*Used to keep the new message(event)*/
uint32_t mPriceClusterEvt = 0x00;

/* TimerId for Price Client */
zbTmrTimerID_t gPriceClientTimerID;
/* TimerId for Block Period */
#if gASL_ZclPrice_Optionals_d
zbTmrTimerID_t gBlockPeriodClientTimerID;
#endif
#if gASL_ZclSE_12_Features_d
zbTmrTimerID_t  gBillingPeriodClientTimerID;
zbTmrTimerID_t  gConsolidatedBillClientTimerID;
#endif
publishPriceEntry_t gaClientPriceTable[gNumofClientPrices_c];
publishPriceEntry_t gaServerPriceTable[gNumofServerPrices_c];
#if gASL_ZclPrice_Optionals_d
publishBlockPeriodEntry_t gaClientBlockPeriodsTable[gNumofClientBlockPeriods_c];
publishBlockPeriodEntry_t gaServerBlockPeriodsTable[gNumofServerBlockPeriods_c];
#endif
#if gASL_ZclPrice_Optionals_d
publishConversionFactorEntry_t gaClientConversionFactorTable[gNumofClientConversionFactors_c];
publishConversionFactorEntry_t gaServerConversionFactorTable[gNumofServerConversionFactors_c];
publishCalorificValueEntry_t gaClientCalorificValueTable[gNumofClientCalorificValue_c];
publishCalorificValueEntry_t gaServerCalorificValueTable[gNumofServerCalorificValue_c];
#endif
#if gASL_ZclSE_12_Features_d
publishCO2ValueEntry_t gaClientCO2ValueTable[gNumofClientCO2Value_c];
publishCO2ValueEntry_t gaServerCO2ValueTable[gNumofServerCO2Value_c];
publishPriceMatrixEntry_t gaClientPriceMatrixTable[gNumofClientPriceMatrix_c];
publishPriceMatrixEntry_t gaServerPriceMatrixTable[gNumofServerPriceMatrix_c];
publishBlockThresholdsEntry_t gaClientBlockThresholdsTable[gNumofClientBlockThresholds_c];
publishBlockThresholdsEntry_t gaServerBlockThresholdsTable[gNumofServerBlockThresholds_c];
publishTierLabelsEntry_t gaClientTierLabelsTable[gNumofClientTierLabels_c];
publishTierLabelsEntry_t gaServerTierLabelsTable[gNumofServerTierLabels_c];
publishBillingPeriodEntry_t gaClientBillingPeriodTable[gNumofClientBillingPeriods_c];
publishBillingPeriodEntry_t gaServerBillingPeriodTable[gNumofServerBillingPeriods_c];
publishCPPEventEntry_t gaClientCPPEventTable[gNumofClientCPPEvents_c];
publishCPPEventEntry_t gaServerCPPEventTable[gNumofServerCPPEvents_c];
publishConsolidatedBillEntry_t gaClientConsolidatedBillTable[gNumofClientConsolidatedBill_c];
publishConsolidatedBillEntry_t gaServerConsolidatedBillTable[gNumofServerConsolidatedBill_c];
publishTariffInformationEntry_t gaClientTariffInformationTable[gNumofClientTariffInformation_c];
publishTariffInformationEntry_t gaServerTariffInformationTable[gNumofServerTariffInformation_c];
#endif
publishCreditPaymentEntry_t gaClientCreditPaymentTable[gNumofClientCreditPayment_c];
publishCreditPaymentEntry_t gaServerCreditPaymentTable[gNumofServerCreditPayment_c];

publishCurrencyConversionEntry_t gClientCurrencyConversion;
publishCurrencyConversionEntry_t gServerCurrencyConversion;

index_t mUpdatePriceIndex;
#if gASL_ZclPrice_Optionals_d
index_t mUpdateBlockPeriodIndex;
#endif
index_t mInterPanIndex;

addrInfoType_t mAddrPriceInfo; 

uint32_t mGetPriceStartTime;
uint8_t mGetNumOfPriceEvts;
bool_t mIsInterPanFlag;
uint8_t mIndex;

#if gASL_ZclPrice_Optionals_d 
getBlockPeriod_t mGetBlockPeriod;
#endif
#if gASL_ZclPrice_Optionals_d
getConversionFactor_t mGetConversionFactor;
getCalorificValue_t mGetCalorificValue;
#endif
#if gASL_ZclSE_12_Features_d
getPriceMatrix_t mGetPriceMatrix;
getBlockThresholds_t mGetBlockThresholds;
getCO2Value_t mGetCO2Value;
getTierLabels_t mGetTierLabels;
getBillingPeriod_t mGetBillingPeriod;
getConsolidatedBill_t mGetConsolidatedBill;
getTariffInformation_t mGetTariffInformation;
#endif
storedInterPanAddrInfo_t gaInterPanAddrTable[gNumOfInterPanAddr_c];
storedPriceAddrInfo_t gaPriceAddrTable[gNumOfPriceAddr_c];

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

#if gASL_ZclSE_TiersNumber_d + gASL_ZclSE_ExtendedPriceTiersNumber_d > 0
#if gASL_ZclSE_TiersNumber_d > 0 
const zclAttrDef_t gaZclPriceTierLabelSetAttrDef[] = {
  {gZclAttrIdPrice_Tier1PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[0])},
  {gZclAttrIdPrice_Tier2PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[1])},
  {gZclAttrIdPrice_Tier3PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[2])},
  {gZclAttrIdPrice_Tier4PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[3])},
  {gZclAttrIdPrice_Tier5PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[4])},
  {gZclAttrIdPrice_Tier6PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[5])},
  {gZclAttrIdPrice_Tier7PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[6])},
  {gZclAttrIdPrice_Tier8PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[7])},
  {gZclAttrIdPrice_Tier9PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[8])},
  {gZclAttrIdPrice_Tier10PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[9])},
  {gZclAttrIdPrice_Tier11PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[10])},
  {gZclAttrIdPrice_Tier12PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[11])},
  {gZclAttrIdPrice_Tier13PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[12])},
  {gZclAttrIdPrice_Tier14PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[13])},
  {gZclAttrIdPrice_Tier15PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[14])}
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 0
  ,{gZclAttrIdPrice_Tier16PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[15])}
#endif
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 0
  ,{gZclAttrIdPrice_Tier17PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[16])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 1
  ,{gZclAttrIdPrice_Tier18PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[17])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 2
  ,  {gZclAttrIdPrice_Tier19PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[18])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 3
  ,  {gZclAttrIdPrice_Tier20PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[19])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 4
  ,{gZclAttrIdPrice_Tier21PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[20])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 5
  ,{gZclAttrIdPrice_Tier22PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[21])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 6
  ,{gZclAttrIdPrice_Tier23PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[22])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 7
  ,{gZclAttrIdPrice_Tier24PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[23])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 8
  ,{gZclAttrIdPrice_Tier25PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[24])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 9
  ,{gZclAttrIdPrice_Tier26PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[25])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 10
  ,{gZclAttrIdPrice_Tier27PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[26])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 11
  ,{gZclAttrIdPrice_Tier28PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[27])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 12
  ,{gZclAttrIdPrice_Tier29PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[28])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 13
  ,{gZclAttrIdPrice_Tier30PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[29])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 14
  ,{gZclAttrIdPrice_Tier31PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[30])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 15
  ,{gZclAttrIdPrice_Tier32PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[31])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 16
  ,{gZclAttrIdPrice_Tier33PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[32])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 17
  ,{gZclAttrIdPrice_Tier34PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[33])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 18
  ,{gZclAttrIdPrice_Tier35PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[34])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 19
  ,{gZclAttrIdPrice_Tier36PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[35])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 20
  ,{gZclAttrIdPrice_Tier37PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[36])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 21
  ,{gZclAttrIdPrice_Tier38PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[37])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 22
  ,{gZclAttrIdPrice_Tier39PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[38])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 23
  ,{gZclAttrIdPrice_Tier40PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[39])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 24
  ,{gZclAttrIdPrice_Tier41PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[40])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 25
  ,{gZclAttrIdPrice_Tier42PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[41])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 26
  ,{gZclAttrIdPrice_Tier43PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[42])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 27
  ,{gZclAttrIdPrice_Tier44PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[43])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 28
  ,{gZclAttrIdPrice_Tier45PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[44])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 29
  ,{gZclAttrIdPrice_Tier46PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[45])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 30
  ,{gZclAttrIdPrice_Tier47PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[46])}
#endif  
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 31
  ,{gZclAttrIdPrice_Tier48PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12,(void*) MbrOfs(zclPriceServerAttrRAM_t, TierPriceLabel[47])}
#endif
};
#endif
#endif

#if gASL_ZclPrice_BlockThresholdNumber_d > 0
const zclAttrDef_t gaZclPriceBlockThresholdSetAttrDef[] = {
  {gZclAttrIdPrice_Block1Threshold_c, gZclDataTypeUint48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, BlockThresholdPrice)}
#if gASL_ZclSE_12_Features_d  
  ,{gZclAttrIdPrice_BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, BlockThresholdCount)},
  {gZclAttrIdPrice_Tier1Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier1BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier1BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier1BlockThresholdCount)},
  {gZclAttrIdPrice_Tier2Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier2BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier2BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier2BlockThresholdCount)},
  {gZclAttrIdPrice_Tier3Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier3BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier3BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier3BlockThresholdCount)},
  {gZclAttrIdPrice_Tier4Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier4BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier4BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier4BlockThresholdCount)},
  {gZclAttrIdPrice_Tier5Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier5BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier5BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier5BlockThresholdCount)},
  {gZclAttrIdPrice_Tier6Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier6BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier6BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier6BlockThresholdCount)},
  {gZclAttrIdPrice_Tier7Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier7BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier7BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier7BlockThresholdCount)},
  {gZclAttrIdPrice_Tier8Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier8BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier8BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier8BlockThresholdCount)},
  {gZclAttrIdPrice_Tier9Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier9BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier9BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier9BlockThresholdCount)},
  {gZclAttrIdPrice_Tier10Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier10BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier10BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier10BlockThresholdCount)},
  {gZclAttrIdPrice_Tier11Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier11BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier11BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier11BlockThresholdCount)},
  {gZclAttrIdPrice_Tier12Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier12BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier12BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier12BlockThresholdCount)},
  {gZclAttrIdPrice_Tier13Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier13BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier13BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier13BlockThresholdCount)},
  {gZclAttrIdPrice_Tier14Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier14BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier14BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier14BlockThresholdCount)},
  {gZclAttrIdPrice_Tier15Block1Threshold_c, gZclDataTypeInt48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier15BlockThresholdPrice)},
  {gZclAttrIdPrice_Tier15BlockThresholdCount_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c , 1, (void *) MbrOfs(zclPriceServerAttrRAM_t, Tier15BlockThresholdCount)}
#endif  
};

#endif

const zclAttrDef_t gaZclPriceBlockPeriodSetAttrDef[] = {
  {gZclAttrIdPrice_StartofBlockPeriod_c,  gZclDataTypeUTCTime_c,   gZclAttrFlagsInRAMRdOnly_c, sizeof(ZCLTime_t),      (void*) MbrOfs(zclPriceServerAttrRAM_t, StartofBlockPeriod )},
  {gZclAttrIdPrice_BlockPeriodDuration_c, gZclDataTypeUint24_c,    gZclAttrFlagsInRAMRdOnly_c, sizeof(Duration24_t),   (void*) MbrOfs(zclPriceServerAttrRAM_t, BlockPeriodDuration )},
  {gZclAttrIdPrice_ThresholdMultiplier_c, gZclDataTypeUint24_c,    gZclAttrFlagsInRAMRdOnly_c, sizeof(Multiplier24_t), (void*) MbrOfs(zclPriceServerAttrRAM_t, ThresholdMultiplier )},
  {gZclAttrIdPrice_ThresholdDivisor_c,    gZclDataTypeUint24_c,    gZclAttrFlagsInRAMRdOnly_c, sizeof(Divisor24_t),    (void*) MbrOfs(zclPriceServerAttrRAM_t, ThresholdDivisor)}
};

const zclAttrDef_t gaZclPriceCommodityTypeSetAttrDef[] = {
  {gZclAttrIdPrice_CommodityType_c,     gZclDataTypeEnum8_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(uint8_t), (void*) MbrOfs(zclPriceServerAttrRAM_t, CommodityType )},
  {gZclAttrIdPrice_StandingCharge_c,    gZclDataTypeUint32_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(CommodityCharge_t), (void*) MbrOfs(zclPriceServerAttrRAM_t, StandingCharge )},
  {gZclAttrIdPrice_ConversionFactor_c,  gZclDataTypeUint32_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(ConversionFactor_t), (void*) MbrOfs(zclPriceServerAttrRAM_t, ConversionFactor )},
  {gZclAttrIdPrice_ConversionFactorTrlDigit_c, gZclDataTypeBitmap8_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(uint8_t), (void*) MbrOfs(zclPriceServerAttrRAM_t, ConversionFactorTrlDigit )},
  {gZclAttrIdPrice_CalorificValue_c,     gZclDataTypeUint32_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(CalorificValue_t), (void*) MbrOfs(zclPriceServerAttrRAM_t, CalorificValue )},
  {gZclAttrIdPrice_CalorificValueUnit_c, gZclDataTypeEnum8_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(uint8_t), (void*) MbrOfs(zclPriceServerAttrRAM_t, CalorificValueUnit )},
  {gZclAttrIdPrice_CalorificValueTrlDigit_c, gZclDataTypeBitmap8_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(uint8_t), (void*) MbrOfs(zclPriceServerAttrRAM_t, CalorificValueTrlDigit) }
};

const zclAttrDef_t gaZclPriceBlockPriceInfoSetAttrDef[] = {
  {gZclAttrIdPrice_NoTierBlock1Price_c, gZclDataTypeUint32_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockPriceInfoNumber_d - 1,  (void *) MbrOfs(zclPriceServerAttrRAM_t,BlockPriceInfoPrice)}
};

#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 0
const zclAttrDef_t gaZclPriceExtendedPriceInfoSetAttrDef[] = {
  {gZclAttrIdPrice_PriceTier16_c, gZclDataTypeUint32_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclSE_ExtendedPriceTiersNumber_d - 1,  (void *) MbrOfs(zclPriceServerAttrRAM_t,ExtendedPriceInfo)}
};
#endif

const zclAttrDef_t gaZclPriceTariffInfoSetAttrDef[] = {
  {gZclAttrIdPrice_TariffLabel_c,               gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(zclStr12_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,TariffLabel)},
  {gZclAttrIdPrice_NoOfPriceTiersInUse_c,       gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c,    sizeof(uint8_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,NoOfPriceTiersInUse)},
  {gZclAttrIdPrice_NoOfBlockTholdInUse_c,       gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c,    sizeof(uint8_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,NoOfBlockThresholdsInUse)},
  {gZclAttrIdPrice_TierBlockMode_c,             gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c,    sizeof(uint8_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,TierBlockMode)},
  {gZclAttrIdPrice_BlockTholdMask_c,            gZclDataTypeBitmap16_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,BlockThresholdMask)},
  {gZclAttrIdPrice_UnitOfMeasure_c,             gZclDataTypeEnum8_c,  gZclAttrFlagsInRAMRdOnly_c,    sizeof(uint8_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,UnitOfMeasure)},
  {gZclAttrIdPrice_Currency_c,                  gZclDataTypeUint16_c,  gZclAttrFlagsInRAMRdOnly_c,   sizeof(uint16_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,Currency)},
  {gZclAttrIdPrice_PriceTrlDigit_c,             gZclDataTypeUint32_c,  gZclAttrFlagsInRAMRdOnly_c,   sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,PriceTrailingDigit)}
};

const zclAttrDef_t gaZclPriceBillingInfoSetAttrDef[] = {
  {gZclAttrIdPrice_CurrBillingPeriodStartUTCTime_c,     gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(ZCLTime_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CurrBillingPeriodStartUTCTime)},
  {gZclAttrIdPrice_CurrBillPeriodDuration_c,            gZclDataTypeUint24_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(Duration24_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CurrBillPeriodDuration)},
  {gZclAttrIdPrice_LastBillPeriodStart_c,               gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(ZCLTime_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,LastBillPeriodStart)},
  {gZclAttrIdPrice_LastBillPeriodDuration_c,            gZclDataTypeUint24_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(Duration24_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,LastBillPeriodDuration)},
  {gZclAttrIdPrice_LastBillPeriodConsolidatedBill_c,    gZclDataTypeUint32_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,LastBillPeriodConsolidatedBill)}
};

const zclAttrDef_t gaZclPriceCreditPaymentSetAttrDef[] = {
  {gZclAttrIdPrice_CreditPaymentDueDate_c,              gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(ZCLTime_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentDueDate)},
  {gZclAttrIdPrice_CreditPaymentStatus_c,               gZclDataTypeBitmap8_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint8_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentStatus)},
  {gZclAttrIdPrice_CreditPaymentOverDueAmount_c,        gZclDataTypeInt32_c,    gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentOverDueAmount)},
  {gZclAttrIdPrice_PaymentDiscount_c,                   gZclDataTypeInt32_c,    gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,PaymentDiscount)},
  {gZclAttrIdPrice_PaymentDiscountPeriod_c,             gZclDataTypeEnum8_c,    gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,PaymentDiscountPeriod)},
  {gZclAttrIdPrice_CreditPayment1_c,                    gZclDataTypeUint32_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPayment1)},
  {gZclAttrIdPrice_CreditPaymentDate1_c,                gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(ZCLTime_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentDate1)},
  {gZclAttrIdPrice_CreditPaymentRef1_c,                 gZclDataTypeOctetStr_c, gZclAttrFlagsInRAMRdOnly_c,  sizeof(zclStr20_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentRef1)},
  {gZclAttrIdPrice_CreditPayment2_c,                    gZclDataTypeUint32_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPayment2)},
  {gZclAttrIdPrice_CreditPaymentDate2_c,                gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(ZCLTime_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentDate2)},
  {gZclAttrIdPrice_CreditPaymentRef2_c,                 gZclDataTypeOctetStr_c, gZclAttrFlagsInRAMRdOnly_c,  sizeof(zclStr20_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentRef2)},
  {gZclAttrIdPrice_CreditPayment3_c,                    gZclDataTypeUint32_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPayment3)},
  {gZclAttrIdPrice_CreditPaymentDate3_c,                gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(ZCLTime_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentDate3)},
  {gZclAttrIdPrice_CreditPaymentRef3_c,                 gZclDataTypeOctetStr_c, gZclAttrFlagsInRAMRdOnly_c,  sizeof(zclStr20_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentRef3)},
  {gZclAttrIdPrice_CreditPayment4_c,                    gZclDataTypeUint32_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPayment4)},
  {gZclAttrIdPrice_CreditPaymentDate4_c,                gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(ZCLTime_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentDate4)},
  {gZclAttrIdPrice_CreditPaymentRef4_c,                 gZclDataTypeOctetStr_c, gZclAttrFlagsInRAMRdOnly_c,  sizeof(zclStr20_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentRef4)},
  {gZclAttrIdPrice_CreditPayment5_c,                    gZclDataTypeUint32_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPayment5)},
  {gZclAttrIdPrice_CreditPaymentDate5_c,                gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(ZCLTime_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentDate5)},
  {gZclAttrIdPrice_CreditPaymentRef5_c,                 gZclDataTypeOctetStr_c, gZclAttrFlagsInRAMRdOnly_c,  sizeof(zclStr20_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,CreditPaymentRef5)}
};

const zclAttrDef_t gaZclPriceTaxCtlSetAttrDef[] = {
  {gZclAttrIdPrice_DomesticPercentage_c,        gZclDataTypeUint16_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint16_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,DomesticPercentage)},
  {gZclAttrIdPrice_DomesticFuelTaxRate_c,       gZclDataTypeUint32_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,DomesticFuelTaxRate)},
  {gZclAttrIdPrice_DomesticFuelTaxRateTrlDgt,   gZclDataTypeBitmap8_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint8_t), (void *) MbrOfs(zclPriceServerAttrRAM_t,DomesticFuelTaxRateTrlDgt)}
};

#if gASL_ZclSE_12_Features_d
#if gASL_ZclSE_TiersNumber_d > 0
const zclAttrDef_t gaZclPriceExportTierLabelSetAttrDef[] = {
  {gZclAttrIdPrice_Tier1PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[0])},
  {gZclAttrIdPrice_Tier2PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[1])},
  {gZclAttrIdPrice_Tier3PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[2])},
  {gZclAttrIdPrice_Tier4PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[3])},
  {gZclAttrIdPrice_Tier5PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[4])},
  {gZclAttrIdPrice_Tier6PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[5])},
  {gZclAttrIdPrice_Tier7PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[6])},
  {gZclAttrIdPrice_Tier8PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[7])},
  {gZclAttrIdPrice_Tier9PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[8])},
  {gZclAttrIdPrice_Tier10PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[9])},
  {gZclAttrIdPrice_Tier11PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[10])},
  {gZclAttrIdPrice_Tier12PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[11])},
  {gZclAttrIdPrice_Tier13PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[12])},
  {gZclAttrIdPrice_Tier14PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[13])},
  {gZclAttrIdPrice_Tier15PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[14])},
  {gZclAttrIdPrice_Tier16PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[15])},
  {gZclAttrIdPrice_Tier17PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[16])},
  {gZclAttrIdPrice_Tier18PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[17])},
  {gZclAttrIdPrice_Tier19PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[18])},
  {gZclAttrIdPrice_Tier20PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[19])},
  {gZclAttrIdPrice_Tier21PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[20])},
  {gZclAttrIdPrice_Tier22PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[21])},
  {gZclAttrIdPrice_Tier23PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[22])},
  {gZclAttrIdPrice_Tier24PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[23])},
  {gZclAttrIdPrice_Tier25PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[24])},
  {gZclAttrIdPrice_Tier26PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[25])},
  {gZclAttrIdPrice_Tier27PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[26])},
  {gZclAttrIdPrice_Tier28PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[27])},
  {gZclAttrIdPrice_Tier29PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[28])},
  {gZclAttrIdPrice_Tier30PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[29])},
  {gZclAttrIdPrice_Tier31PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[30])},
  {gZclAttrIdPrice_Tier32PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[31])},
  {gZclAttrIdPrice_Tier33PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[32])},
  {gZclAttrIdPrice_Tier34PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[33])},
  {gZclAttrIdPrice_Tier35PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[34])},
  {gZclAttrIdPrice_Tier36PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[35])},
  {gZclAttrIdPrice_Tier37PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[36])},
  {gZclAttrIdPrice_Tier38PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[37])},
  {gZclAttrIdPrice_Tier39PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[38])},
  {gZclAttrIdPrice_Tier40PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[39])},
  {gZclAttrIdPrice_Tier41PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[40])},
  {gZclAttrIdPrice_Tier42PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[41])},
  {gZclAttrIdPrice_Tier43PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[42])},
  {gZclAttrIdPrice_Tier44PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[43])},
  {gZclAttrIdPrice_Tier45PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[44])},
  {gZclAttrIdPrice_Tier46PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[45])},
  {gZclAttrIdPrice_Tier47PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[46])},
  {gZclAttrIdPrice_Tier48PriceLabel_c, gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c , 12, (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierPriceLabel[47])}
};
#endif

#if gASL_ZclPrice_BlockThresholdNumber_d > 0 &&  gASL_ZclSE_12_Features_d
const zclAttrDef_t gaZclPriceExportBlockThresholdSetAttrDef[] = {
  {gZclAttrIdPrice_Block1Threshold_c, gZclDataTypeUint48_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockThresholdNumber_d - 1,(void *)  MbrOfs(zclPriceServerAttrRAM_t,RcvdBlockThresholds[0])}
};
#endif

const zclAttrDef_t gaZclPriceExportBlockPeriodSetAttrDef[] = {
  {gZclAttrIdPrice_StartofBlockPeriod_c,  gZclDataTypeUTCTime_c,   gZclAttrFlagsInRAMRdOnly_c, sizeof(ZCLTime_t),      (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdStartofBlockPeriod )},
  {gZclAttrIdPrice_BlockPeriodDuration_c, gZclDataTypeUint24_c,    gZclAttrFlagsInRAMRdOnly_c, sizeof(Duration24_t),   (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdBlockPeriodDuration )},
  {gZclAttrIdPrice_ThresholdMultiplier_c, gZclDataTypeUint24_c,    gZclAttrFlagsInRAMRdOnly_c, sizeof(Multiplier24_t), (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdThresholdMultiplier )},
  {gZclAttrIdPrice_ThresholdDivisor_c,    gZclDataTypeUint24_c,    gZclAttrFlagsInRAMRdOnly_c, sizeof(Divisor24_t),    (void*) MbrOfs(zclPriceServerAttrRAM_t, RcvdThresholdDivisor) }
};

const zclAttrDef_t gaZclPriceExportBlockPriceInfoSetAttrDef[] = {
  {gZclAttrIdPrice_NoTierBlock1Price_c, gZclDataTypeUint32_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclPrice_BlockPriceInfoNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t,RcvdBlockPriceInfo[0])}
};

#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 0
const zclAttrDef_t gaZclPriceExportExtendedPriceInfoSetAttrDef[] = {
  {gZclAttrIdPrice_PriceTier16_c, gZclDataTypeUint32_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsVector_c, gASL_ZclSE_ExtendedPriceTiersNumber_d - 1, (void *) MbrOfs(zclPriceServerAttrRAM_t,RcvdBlockPriceInfo[0])}
};
#endif

const zclAttrDef_t gaZclPriceExportTariffInfoSetAttrDef[] = {
  {gZclAttrIdPrice_TariffLabel_c,               gZclDataTypeOctetStr_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(zclStr12_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdTariffLabel)},
  {gZclAttrIdPrice_NoOfPriceTiersInUse_c,       gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c,    sizeof(uint8_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdNoOfPriceTiersInUse)},
  {gZclAttrIdPrice_NoOfBlockTholdInUse_c,       gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c,    sizeof(uint8_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdNoOfBlockThresholdsInUse)},
  {gZclAttrIdPrice_TierBlockMode_c,             gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c,    sizeof(uint8_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdTierBlockMode)},
  {gZclAttrIdPrice_BlockTholdMask_c,            gZclDataTypeBitmap16_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdBlockThresholdMask)},
  {gZclAttrIdPrice_RcvdTariffResPeriod_d,       gZclDataTypeEnum8_c,  gZclAttrFlagsInRAMRdOnly_c,    sizeof(uint8_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdUnitOfMeasure)},
  {gZclAttrIdPrice_Currency_c,                  gZclDataTypeUint16_c,  gZclAttrFlagsInRAMRdOnly_c,   sizeof(uint16_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdCurrency)},
  {gZclAttrIdPrice_PriceTrlDigit_c,             gZclDataTypeUint32_c,  gZclAttrFlagsInRAMRdOnly_c,   sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdPriceTrailingDigit)}
};

const zclAttrDef_t gaZclPriceExportBillingInfoSetAttrDef[] = {
  {gZclAttrIdPrice_CurrBillingPeriodStartUTCTime_c,     gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(ZCLTime_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdCurrBillingPeriodStartUTCTime)},
  {gZclAttrIdPrice_CurrBillPeriodDuration_c,            gZclDataTypeUint24_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(Duration24_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdCurrBillPeriodDuration)},
  {gZclAttrIdPrice_LastBillPeriodStart_c,               gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAMRdOnly_c,  sizeof(ZCLTime_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdLastBillPeriodStart)},
  {gZclAttrIdPrice_LastBillPeriodDuration_c,            gZclDataTypeUint24_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(Duration24_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdLastBillPeriodDuration)},
  {gZclAttrIdPrice_LastBillPeriodConsolidatedBill_c,    gZclDataTypeUint32_c,   gZclAttrFlagsInRAMRdOnly_c,  sizeof(uint32_t), (void *) MbrOfs(zclPriceServerAttrRAM_t, RcvdLastBillPeriodConsolidatedBill)}
};
#endif

#if gZcl_EnablePriceClient_d 
const zclAttrDef_t gaZclPriceClientAttrDef[] = {
  {gZclAttrIdClientPrice_PriceIncreaseRandomizeMinutes_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsClientAttribute_c, sizeof(uint8_t), (void *)(MbrOfs(zclPriceClientAttrRAM_t,PriceIncreaseRandomizeMinutes) + MbrOfs(zclPriceAttrRAM_t, PriceClientAttrs))},
  {gZclAttrIdClientPrice_PriceDecreaseRandomizeMinutes_c, gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsClientAttribute_c, sizeof(uint8_t), (void *)(MbrOfs(zclPriceClientAttrRAM_t,PriceDecreaseRandomizeMinutes)+ MbrOfs(zclPriceAttrRAM_t, PriceClientAttrs))},
  {gZclAttrIdClientPrice_CommodityType_c, gZclDataTypeEnum8_c,  gZclAttrFlagsInRAMRdOnly_c | gZclAttrFlagsIsClientAttribute_c, sizeof(uint8_t), (void *)(MbrOfs(zclPriceClientAttrRAM_t, ClientCommodityType)+ MbrOfs(zclPriceAttrRAM_t, PriceClientAttrs))}
};
#endif

const zclAttrSet_t gaZclPriceAttrSet[] = {
#if gZcl_EnablePriceClient_d 
  {gZclAttrClientPriceSet_c, (void *)&gaZclPriceClientAttrDef, NumberOfElements(gaZclPriceClientAttrDef)},
#endif  
#if gASL_ZclSE_TiersNumber_d  
  {gZclAttrPrice_TierLabelSet_c,      (void *)&gaZclPriceTierLabelSetAttrDef,       NumberOfElements(gaZclPriceTierLabelSetAttrDef)},
#endif
#if gASL_ZclPrice_BlockThresholdNumber_d  
  {gZclAttrPrice_BlockThresholdSet_c, (void *)&gaZclPriceBlockThresholdSetAttrDef,  NumberOfElements(gaZclPriceBlockThresholdSetAttrDef)},
#endif
  {gZclAttrPrice_BlockPeriodSet_c,    (void *)&gaZclPriceBlockPeriodSetAttrDef,     NumberOfElements(gaZclPriceBlockPeriodSetAttrDef)},
  {gZclAttrPrice_CommodityTypeSet_c,  (void *)&gaZclPriceCommodityTypeSetAttrDef,   NumberOfElements(gaZclPriceCommodityTypeSetAttrDef)},
  {gZclAttrPrice_BlockPriceInfoSet_c, (void *)&gaZclPriceBlockPriceInfoSetAttrDef,  NumberOfElements(gaZclPriceBlockPriceInfoSetAttrDef)}
#if gASL_ZclSE_12_Features_d
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 0  
  ,{gZclAttrPrice_ExtendedPriceInfoSet_c,       (void *)&gaZclPriceExtendedPriceInfoSetAttrDef,         NumberOfElements(gaZclPriceExtendedPriceInfoSetAttrDef)}
#endif  
  ,{gZclAttrPrice_TariffInfoSet_c,              (void *)&gaZclPriceTariffInfoSetAttrDef,                NumberOfElements(gaZclPriceTariffInfoSetAttrDef)}
  ,{gZclAttrPrice_BillingInfoSet_c,             (void *)&gaZclPriceBillingInfoSetAttrDef,               NumberOfElements(gaZclPriceBillingInfoSetAttrDef)}
  ,{gZclAttrPrice_CreditPaymentSet_c,           (void *)&gaZclPriceCreditPaymentSetAttrDef,             NumberOfElements(gaZclPriceCreditPaymentSetAttrDef)}
  ,{gZclAttrPrice_TaxControlSet_c,              (void *)&gaZclPriceTaxCtlSetAttrDef,                    NumberOfElements(gaZclPriceTaxCtlSetAttrDef)}
  ,{gZclAttrPrice_ExportTierLabelSet_c,         (void *)&gaZclPriceExportTierLabelSetAttrDef,           NumberOfElements(gaZclPriceExportTierLabelSetAttrDef)}
  ,{gZclAttrPrice_ExportBlockThresholdSet_c,    (void *)&gaZclPriceExportBlockThresholdSetAttrDef,      NumberOfElements(gaZclPriceExportBlockThresholdSetAttrDef)}
  ,{gZclAttrPrice_ExportBlockPeriodSet_c,       (void *)&gaZclPriceExportBlockPeriodSetAttrDef,         NumberOfElements(gaZclPriceExportBlockPeriodSetAttrDef)}
  ,{gZclAttrPrice_ExportBlockPriceInfoSet_c,    (void *)&gaZclPriceExportBlockPriceInfoSetAttrDef,      NumberOfElements(gaZclPriceExportBlockPriceInfoSetAttrDef)}
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 0   
  ,{gZclAttrPrice_ExportExtendedPriceInfoSet_c, (void *)&gaZclPriceExportExtendedPriceInfoSetAttrDef,   NumberOfElements(gaZclPriceExportExtendedPriceInfoSetAttrDef)}
#endif  
  ,{gZclAttrPrice_ExportTariffInfoSet_c,        (void *)&gaZclPriceExportTariffInfoSetAttrDef,          NumberOfElements(gaZclPriceExportTariffInfoSetAttrDef)}
  ,{gZclAttrPrice_ExportBillingInfoSet_c,       (void *)&gaZclPriceExportBillingInfoSetAttrDef,         NumberOfElements(gaZclPriceExportBillingInfoSetAttrDef)}
#endif  
};

const zclAttrSetList_t gZclPriceAttrSetList = {
  NumberOfElements(gaZclPriceAttrSet),
  gaZclPriceAttrSet
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		void ZCL_PriceClientInit(void)
 *
 * @brief	Initializes the Price Client functionality
 *
 */
void ZCL_PriceClientInit(void)
{
  gPriceClientTimerID =  ZbTMR_AllocateTimer();
#if gASL_ZclPrice_Optionals_d
  gBlockPeriodClientTimerID = ZbTMR_AllocateTimer();
#endif
#if gASL_ZclSE_12_Features_d
  gBillingPeriodClientTimerID = ZbTMR_AllocateTimer();
  gConsolidatedBillClientTimerID = ZbTMR_AllocateTimer();
#endif
} 	

/*!
 * @fn 		void ZCL_PriceServerInit(void)
 *
 * @brief	Initializes the Price Server functionality
 *
 */
void ZCL_PriceServerInit(void)
{
#if gASL_ZclPrice_Optionals_d 
  mGetBlockPeriod.index = gInvalidTableIndex_c;
#endif
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_12_Features_d
  mGetConversionFactor.index = gInvalidTableIndex_c;
  mGetCalorificValue.index = gInvalidTableIndex_c;
#endif
#if gASL_ZclSE_12_Features_d
  mGetPriceMatrix.Index = gInvalidTableIndex_c;
  mGetBlockThresholds.Index = gInvalidTableIndex_c;
  mGetCO2Value.index = gInvalidTableIndex_c;
  mGetTierLabels.Index = gInvalidTableIndex_c;
  mGetBillingPeriod.index = gInvalidTableIndex_c;
  mGetConsolidatedBill.index = gInvalidTableIndex_c;
  mGetTariffInformation.index = gInvalidTableIndex_c;
#endif
} 	

/*!
 * @fn 		zbStatus_t ZCL_PriceClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Price Cluster server. 
 *
 */
zbStatus_t ZCL_PriceClusterServer
(
     zbApsdeDataIndication_t *pIndication, /* IN: */
     afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
#if  gASL_ZclPrice_Optionals_d || gASL_ZclSE_12_Features_d
  zclTSQ_t tsq;
#endif  
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  afAddrInfo_t replyAddrInfo;
  zclCmdPrice_GetCurrPriceReq_t *pGetCurrPrice;
#if gASL_ZclPrice_Optionals_d
  zclCmdPrice_GetScheduledPricesReq_t *pGetScheduledPrice;
  zclCmdPrice_GetBlockPeriodsReq_t *pGetBlockPeriods;
#endif
#if gASL_ZclPrice_Optionals_d
  zclCmdPrice_GetConversionFactorReq_t *pGetConversionFactor;
  zclCmdPrice_GetCalorificValueReq_t *pGetCalorificValue;
#endif
#if gASL_ZclSE_12_Features_d
  zclCmdPrice_GetPriceMatrixReq_t *pGetPriceMatrix;
  zclCmdPrice_GetBlockThresholdsReq_t *pGetBlockThresholds;
  zclCmdPrice_GetCO2ValueReq_t *pGetCO2Value;
  zclCmdPrice_GetTierLabelsReq_t *pGetTierLabels;
  zclCmdPrice_GetBillingPeriodReq_t *pGetBillingPeriod;    
  zclCmdPrice_CPPEventRsp_t *pCPPEventRsp;  
  zclCmdPrice_GetConsolidatedBillReq_t *pGetConsolidatedBill;  
  zclCmdPrice_GetTariffInformationReq_t *pGetTariffInformation;
#endif
  
  (void) pDev;
  pFrame = (void *)pIndication->pAsdu;
  
  /* Get the cmd and the SE message */
  Cmd = pFrame->command;
#if  gASL_ZclPrice_Optionals_d || gASL_ZclSE_12_Features_d  
  tsq = pFrame->transactionId;
#endif  
  /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    status = gZclSuccess_c;
  
  /* get address ready for reply */
  AF_PrepareForReply(&replyAddrInfo, pIndication); 
  switch(Cmd) {
  case gZclCmdPrice_GetCurrPriceReq_c:
    pGetCurrPrice = (zclCmdPrice_GetCurrPriceReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetCurrPriceReq((addrInfoType_t*)&replyAddrInfo, pGetCurrPrice, FALSE);
    break;
    
#if gASL_ZclPrice_Optionals_d   
  case gZclCmdPrice_GetScheduledPricesReq_c:
    pGetScheduledPrice = (zclCmdPrice_GetScheduledPricesReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetScheduledPricesReq((addrInfoType_t*)&replyAddrInfo, pGetScheduledPrice, FALSE);
    break;
#endif    
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_GetBillingPeriod_c:
    pGetBillingPeriod = (zclCmdPrice_GetBillingPeriodReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetBillingPeriodReq(&replyAddrInfo, tsq, pGetBillingPeriod);
    break;
#endif

#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_GetTariffInformation_c:
    pGetTariffInformation = (zclCmdPrice_GetTariffInformationReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetTariffInformationReq(&replyAddrInfo, tsq, pGetTariffInformation);
    break;
#endif
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_GetConsolidatedBill_c:
    pGetConsolidatedBill = (zclCmdPrice_GetConsolidatedBillReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetConsolidatedBillReq(&replyAddrInfo, tsq, pGetConsolidatedBill);
    break;
#endif
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_GetCPPEventResponse_c:
    pCPPEventRsp = (zclCmdPrice_CPPEventRsp_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessCPPEventRsp(&replyAddrInfo, tsq, pCPPEventRsp);
    break;
#endif
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_GetPriceMatrix_c:
    pGetPriceMatrix = (zclCmdPrice_GetPriceMatrixReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetPriceMatrixReq(&replyAddrInfo, tsq, pGetPriceMatrix);
    break;
#endif
    
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_GetCO2Value_c:
    pGetCO2Value = (zclCmdPrice_GetCO2ValueReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetCO2ValueReq(&replyAddrInfo, tsq, pGetCO2Value);
    break;
#endif

#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_GetBlockThresholds_c:
    pGetBlockThresholds = (zclCmdPrice_GetBlockThresholdsReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetBlockThresholdsReq(&replyAddrInfo, tsq, pGetBlockThresholds);
    break;
#endif
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_GetTierLabels_c:
    pGetTierLabels = (zclCmdPrice_GetTierLabelsReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetTierLabelsReq(&replyAddrInfo, tsq, pGetTierLabels);
    break;
#endif
    
  case gZclCmdPrice_PriceAck_c:
    status = gZclSuccess_c;
    break;
    
#if gASL_ZclPrice_Optionals_d     
  case gZclCmdPrice_GetBlockPeriods_c:
    pGetBlockPeriods = (zclCmdPrice_GetBlockPeriodsReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetBlockPeriodsReq(&replyAddrInfo, tsq, pGetBlockPeriods);
    break;
#endif

#if gASL_ZclPrice_Optionals_d    
  case gZclCmdPrice_GetConversionFactor_c:
    pGetConversionFactor = (zclCmdPrice_GetConversionFactorReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetConversionFactorReq(&replyAddrInfo, tsq, pGetConversionFactor);
    break;
  
  case gZclCmdPrice_GetCalorificValue_c:
    pGetCalorificValue = (zclCmdPrice_GetCalorificValueReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetCalorificValueReq(&replyAddrInfo, tsq, pGetCalorificValue);
    break;
#endif
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_GetCurrencyConversion_c:
    status = ZCL_ProcessGetCurrencyConversionReq(&replyAddrInfo, tsq);
    break;
    
#endif    
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
    
  }
  
  return status;
}
  
/*!
 * @fn 		zbStatus_t ZCL_PriceClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Price Cluster client. 
 *
 */
zbStatus_t ZCL_PriceClusterClient
(
     zbApsdeDataIndication_t *pIndication, /* IN: */
     afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  (void)pDev;
  pFrame = (void *)pIndication->pAsdu;
  Cmd = pFrame->command;
  
  /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;
  
  switch(Cmd) {
    case gZclCmdPrice_PublishPriceRsp_c:
      
      status =  ZCL_ProcessClientPublishPrice((zclCmdPrice_PublishPriceRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))), FALSE);
      if(((((zclCmdPrice_PublishPriceRsp_t *)(((uint8_t *)pIndication->pAsdu + sizeof(zclFrame_t))))->PriceControl)
          & 0x01)==0x01)
      {
        (void)ZCL_SendPriceAck(pIndication);
      }
      break;
#if gASL_ZclPrice_Optionals_d
    case gZclCmdPrice_PublishBlockPeriodRsp_c:
      
      status =  ZCL_ProcessClientPublishBlockPeriod((zclCmdPrice_PublishBlockPeriodRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
      if(((((zclCmdPrice_PublishBlockPeriodRsp_t *)(((uint8_t *)pIndication->pAsdu + sizeof(zclFrame_t))))->BlockPeriodControl)
          & 0x01)==0x01)
      {
        (void)ZCL_SendPriceAck(pIndication);
      }
      break;
#endif
#if gASL_ZclPrice_Optionals_d      
    case gZclCmdPrice_PublishConversionFactorRsp_c:
      status = ZCL_ProcessClientPublishConversionFactor((zclCmdPrice_PublishConversionFactorRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
      break;
    
    case gZclCmdPrice_PublishCalorificValueRsp_c:
      status = ZCL_ProcessClientPublishCalorificValue((zclCmdPrice_PublishCalorificValueRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
      break;
#endif
      
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_PublishCO2ValueRsp_c:
    status = ZCL_ProcessClientPublishCO2Value((zclCmdPrice_PublishCO2ValueRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_PublishTariffInformationRsp_c:
    status = ZCL_ProcessClientPublishTariffInformation((zclCmdPrice_PublishTariffInformationRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_PublishCurrencyConversionRsp_c:
    status = ZCL_ProcessClientPublishCurrencyConversion((zclCmdPrice_PublishCurrencyConversionRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_PublishPriceMatrixRsp_c:
    status = ZCL_ProcessClientPublishPriceMatrix((zclCmdPrice_PublishPriceMatrixRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))), pIndication->asduLength - sizeof(zclFrame_t));
    break;
#endif
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_PublishBlockThresholdsRsp_c:
    status = ZCL_ProcessClientPublishBlockThresholds((zclCmdPrice_PublishBlockThresholdsRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))), pIndication->asduLength - sizeof(zclFrame_t));
    break;
#endif
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_PublishTierLabelsRsp_c:
    status = ZCL_ProcessClientPublishTierLabels((zclCmdPrice_PublishTierLabelsRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))), pIndication->asduLength - sizeof(zclFrame_t));
    break;
#endif
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_PublishBillingPeriodRsp_c:
    status = ZCL_ProcessClientPublishBillingPeriod((zclCmdPrice_PublishBillingPeriodRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
    
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_PublishConsolidatedBillRsp_c:
    status = ZCL_ProcessClientPublishConsolidatedBill((zclCmdPrice_PublishConsolidatedBillRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
#if gASL_ZclSE_12_Features_d
  case gZclCmdPrice_PublishCPPEventRsp_c:
    {
      status = ZCL_ProcessClientPublishCPPEvent((zclCmdPrice_PublishCPPEventRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    }
    break;
#endif
#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_Optionals_d
  case gZclCmdPrice_PublishCreditPaymentRsp_c:
    {
      status = ZCL_ProcessClientPublishCreditPayment((zclCmdPrice_PublishCreditPaymentRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    }
    break;

  case gZclCmdPrice_PublishExtendedPriceRsp_c:
    {
      zclCmdPrice_PublishExtendedPriceRsp_t *pCmd = (zclCmdPrice_PublishExtendedPriceRsp_t *)((uint8_t *)pIndication->pAsdu + sizeof(zclFrame_t));
      
      /* Check if Price ACK needs to be sent */
      if (pCmd->PriceTrailingDigitAndPriceCtl & 0x01)
      {
        (void)ZCL_SendPriceAck(pIndication);
      }      
      
      status =  ZCL_ProcessClientPublishPrice((void*)pCmd, TRUE);
    }
    break;
    
  case gZclCmdPrice_CancelTariffReq_c:
    {
      zclCmdPrice_CancelTariffReq_t *pReq = (zclCmdPrice_CancelTariffReq_t *)((uint8_t *)pIndication->pAsdu + sizeof(zclFrame_t));
      
      DeleteClientTariffInformation(pReq->ProviderID, pReq->IssuerTariffID, pReq->TariffType);
      status = gZclSuccess_c;
    }
    break;

#endif
    default: 
      status = gZclUnsupportedClusterCommand_c;
      break;
  }
  
  return status;
}

#if gInterPanCommunicationEnabled_c
/*!
 * @fn 		zbStatus_t ZCL_InterPanPriceClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the InterPAN Price Cluster client. 
 *
 */
zbStatus_t ZCL_InterPanPriceClusterClient
(
	zbInterPanDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
 zclCmd_t *pCmd;
zbStatus_t status = gZclSuccess_c;

(void)pDev;
pCmd = (zclCmd_t *)&(((zclFrame_t *)pIndication->pAsdu)->command);
if(*pCmd == gZclCmdPrice_PublishPriceRsp_c)
{
    status = ZCL_ProcessClientPublishPrice((zclCmdPrice_PublishPriceRsp_t *)(pCmd+sizeof(zclCmd_t)), FALSE);
    if(status != gZclSuccess_c)
    {
      return status;
    }
    else
    {
      return ZCL_SendInterPriceAck(pIndication);
    }
}

 return gZclUnsupportedClusterCommand_c;
}

/*!
 * @fn 		zbStatus_t ZCL_InterPanPriceClusterServer(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the InterPAN Price Cluster server. 
 *
 */
zbStatus_t ZCL_InterPanPriceClusterServer
(
zbInterPanDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
) 
{
  
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  addrInfoType_t replyAddrInfo;
  zclCmdPrice_GetCurrPriceReq_t *pGetCurrPrice;
#if gASL_ZclPrice_Optionals_d
  zclCmdPrice_GetScheduledPricesReq_t *pGetScheduledPrice;
#endif
  (void) pDev;
  /* Get the cmd and the SE message */
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  /* get address ready for reply */
  PrepareInterPanForReply((InterPanAddrInfo_t *)&replyAddrInfo, pIndication); 
  switch(Cmd) {
  case gZclCmdPrice_GetCurrPriceReq_c:
    pGetCurrPrice = (zclCmdPrice_GetCurrPriceReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetCurrPriceReq(&replyAddrInfo, pGetCurrPrice, TRUE);
    
    break;
#if gASL_ZclPrice_Optionals_d     
  case gZclCmdPrice_GetScheduledPricesReq_c:
    pGetScheduledPrice = (zclCmdPrice_GetScheduledPricesReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetScheduledPricesReq(&replyAddrInfo, pGetScheduledPrice, TRUE);
    break;  
#endif
  case gZclCmdPrice_PriceAck_c:
    status = gZclSuccess_c;
    break;
    
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
    
  }
  
  return status;
  
}
#endif
/*!
 * @fn 		zbStatus_t ZCL_PriceAck(zclPrice_PriceAck_t *pReq)
 *
 * @brief	Sends over-the-air a Price Acknowledgement frame from the client side. 
 *
 */
zbStatus_t ZCL_PriceAck(zclPrice_PriceAck_t *pReq)
{
  return ZCL_SendClientRspSeqPassed(gZclCmdPrice_PriceAck_c, sizeof(zclCmdPrice_PriceAck_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_InterPriceAck(zclPrice_InterPriceAck_t *pReq)
 *
 * @brief	Sends over-the-air a Price Acknowledgement frame from the InterPAN client side. 
 *
 */
zbStatus_t ZCL_InterPriceAck(zclPrice_InterPriceAck_t *pReq)
{
  return ZCL_SendInterPanClientRspSeqPassed(gZclCmdPrice_PriceAck_c, sizeof(zclCmdPrice_PriceAck_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPrice_GetCurrPriceReq(zclPrice_GetCurrPriceReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Current Price frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetCurrPriceReq
(
zclPrice_GetCurrPriceReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetCurrPriceReq_c, sizeof(zclCmdPrice_GetCurrPriceReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetScheduledPricesReq(zclPrice_GetScheduledPricesReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Scheduled Prices frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetScheduledPricesReq
(
zclPrice_GetScheduledPricesReq_t *pReq
)
{ 
  
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetScheduledPricesReq_c, sizeof(zclCmdPrice_GetScheduledPricesReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetBlockPeriodsReq(zclPrice_GetBlockPeriodsReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Block Periods frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetBlockPeriodsReq
(
zclPrice_GetBlockPeriodsReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetBlockPeriods_c, sizeof(zclCmdPrice_GetBlockPeriodsReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishPriceRsp(zclPrice_PublishPriceRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Price frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishPriceRsp
(
zclPrice_PublishPriceRsp_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  
  /*NOTE - depending on whether the Rate Label has a fixed length this function needs to package
  the telegram.
  The Stucture allows for 13 bytes of text, which is a size of 12 including the length.(12 byte of data)     
  aka
  copy from currentTime to rest of telegram to the location of Ratelabel+sizeof(length)+length.
  */
  
  pSrc  = (uint8_t *) &pReq->cmdFrame.Price.IssuerEvt[0];
  pDst  = &pReq->cmdFrame.Price.RateLabel.aStr[pReq->cmdFrame.Price.RateLabel.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrice_PublishPriceRsp_t) - (sizeof(ProviderID_t) + sizeof(uint8_t) + pReq->cmdFrame.Price.RateLabel.length);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
  length = sizeof(zclCmdPrice_PublishPriceRsp_t) - (sizeof(zclStr12_t) - sizeof(uint8_t) - pReq->cmdFrame.Price.RateLabel.length);
  
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishPriceRsp_c, length,(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishExtendedPriceRsp(zclPrice_PublishPriceRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Price frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishExtendedPriceRsp
(
zclPrice_PublishPriceRsp_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  
  /*NOTE - depending on whether the Rate Label has a fixed length this function needs to package
  the telegram.
  The Stucture allows for 13 bytes of text, which is a size of 12 including the length.(12 byte of data)     
  aka
  copy from currentTime to rest of telegram to the location of Ratelabel+sizeof(length)+length.
  */
  
  pSrc  = (uint8_t *) &pReq->cmdFrame.ExtendedPrice.IssuerEvt[0];
  pDst  = &pReq->cmdFrame.ExtendedPrice.RateLabel.aStr[pReq->cmdFrame.ExtendedPrice.RateLabel.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrice_PublishPriceRsp_t) - (sizeof(ProviderID_t) + sizeof(uint8_t) + pReq->cmdFrame.ExtendedPrice.RateLabel.length);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
  length = sizeof(zclCmdPrice_PublishPriceRsp_t) - (sizeof(zclStr12_t) - sizeof(uint8_t) - pReq->cmdFrame.ExtendedPrice.RateLabel.length);
  
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishExtendedPriceRsp_c, length,(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishBlockPeriodRsp(zclPrice_PublishBlockPeriodRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Block Period frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishBlockPeriodRsp
(
zclPrice_PublishBlockPeriodRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishBlockPeriodRsp_c,  sizeof(zclCmdPrice_PublishBlockPeriodRsp_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishConversionFactorRsp(zclPrice_PublishConversionFactorRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Conversion Factor frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishConversionFactorRsp
(
zclPrice_PublishConversionFactorRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishConversionFactorRsp_c,  sizeof(zclCmdPrice_PublishConversionFactorRsp_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishCalorificValueRsp(zclPrice_PublishCalorificValueRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Calorific Value frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishCalorificValueRsp
(
zclPrice_PublishCalorificValueRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishCalorificValueRsp_c,  sizeof(zclCmdPrice_PublishCalorificValueRsp_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetCalorificValueReq(zclPrice_GetCalorificValueReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Calorific Value frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetCalorificValueReq
(
zclPrice_GetCalorificValueReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetCalorificValue_c,  sizeof(zclCmdPrice_GetCalorificValueReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_CancelTariffReq(zclPrice_CancelTariffReq_t *pReq)
 *
 * @brief	Sends over-the-air a Cancel Tariff frame from the server side. 
 *
 */
zbStatus_t zclPrice_CancelTariffReq
(
zclPrice_CancelTariffReq_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdPrice_CancelTariffReq_c,  sizeof(zclCmdPrice_CancelTariffReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetConversionFactorReq(zclPrice_GetConversionFactorReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Conversion Factor frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetConversionFactorReq
(
zclPrice_GetConversionFactorReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetConversionFactor_c,  sizeof(zclCmdPrice_GetConversionFactorReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishTariffInformationRsp(zclPrice_PublishTariffInformationRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Tariff Information frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishTariffInformationRsp
(
zclPrice_PublishTariffInformationRsp_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  /*NOTE - depending on whether the Rate Label has a fixed length this function needs to package
  the telegram.
  The Stucture allows for 12 bytes of text, which is a size of 12 including the length.(11 byte of data)     
  aka
  copy from currentTime to rest of telegram to the location of Ratelabel+sizeof(length)+length.
  
  */
  
  pSrc  = (uint8_t *) &pReq->cmdFrame.NumberPriceTiersUse;
  pDst  = &pReq->cmdFrame.TariffLabel.aStr[pReq->cmdFrame.TariffLabel.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrice_PublishTariffInformationRsp_t) - MbrOfs(zclCmdPrice_PublishTariffInformationRsp_t, TariffLabel)
          - sizeof(uint8_t) - pReq->cmdFrame.TariffLabel.length;  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
  length = sizeof(zclCmdPrice_PublishTariffInformationRsp_t) - (sizeof(zclStr25_t) - sizeof(uint8_t) - (pReq->cmdFrame.TariffLabel.length + 1));
  
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishTariffInformationRsp_c, length,(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishCurrencyConversionRsp(zclPrice_PublishCurrencyConversionRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Currency Conversion frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishCurrencyConversionRsp
(
zclPrice_PublishCurrencyConversionRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishCurrencyConversionRsp_c, sizeof(zclCmdPrice_PublishCurrencyConversionRsp_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishBillingPeriodRsp(zclPrice_PublishBillingPeriodRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Billing Period frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishBillingPeriodRsp
(
zclPrice_PublishBillingPeriodRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishBillingPeriodRsp_c,  sizeof(zclCmdPrice_PublishBillingPeriodRsp_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetBillingPeriodReq(zclPrice_GetBillingPeriodReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Billing Period frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetBillingPeriodReq
(
zclPrice_GetBillingPeriodReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetBillingPeriod_c, sizeof(zclCmdPrice_GetBillingPeriodReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetPriceMatrixReq(zclPrice_GetPriceMatrixReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Price Matrix frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetPriceMatrixReq
(
zclPrice_GetPriceMatrixReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetPriceMatrix_c, sizeof(zclCmdPrice_GetPriceMatrixReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetBlockThresholdsReq(zclPrice_GetBlockThresholdsReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Block Threshold frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetBlockThresholdsReq
(
zclPrice_GetBlockThresholdsReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetBlockThresholds_c, sizeof(zclCmdPrice_GetBlockThresholdsReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishPriceMatrixRsp(zclPrice_PublishPriceMatrixRsp *pReq, uint8_t length)
 *
 * @brief	Sends over-the-air a Publish Price Matrix frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishPriceMatrixRsp
(
zclPrice_PublishPriceMatrixRsp_t *pReq,
uint8_t length
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishPriceMatrixRsp_c,  sizeof(zclCmdPrice_PublishPriceMatrixRsp_t) + 
                                    (length - 1) * sizeof(TierBlockPrice_t) ,(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishBlockThresholdsRsp(zclPrice_PublishBlockThresholdsRsp_t *pReq, 
 *											uint8_t numOfTiers, uint8_t numOfBlockThresholdsPerTier)
 *
 * @brief	Sends over-the-air a Publish Block Thresholds frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishBlockThresholdsRsp
(
zclPrice_PublishBlockThresholdsRsp_t *pReq,
uint8_t numOfTiers,
uint8_t numOfBlockThresholdsPerTier
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishBlockThresholdsRsp_c,  sizeof(zclCmdPrice_PublishBlockThresholdsRsp_t) + 
                                    (numOfBlockThresholdsPerTier -1) * sizeof(BlockThreshold_t) + /* Block thresholds for first tier */
                                     (numOfTiers - 1) * (sizeof(TierBlockThreshold_t) + (numOfBlockThresholdsPerTier - 1) * sizeof(BlockThreshold_t)) /* Block thresholds for the rest of the tier */
                                    ,(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetCO2ValueReq(zclPrice_GetCO2ValueReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get CO2 Value frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetCO2ValueReq
(
zclPrice_GetCO2ValueReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetCO2Value_c, sizeof(zclCmdPrice_GetCO2ValueReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishCO2ValueRsp(zclPrice_PublishCO2ValueRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish CO2 Value frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishCO2ValueRsp
(
zclPrice_PublishCO2ValueRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishCO2ValueRsp_c,  sizeof(zclCmdPrice_PublishCO2ValueRsp_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetTierLabelsReq(zclPrice_GetTierLabelsReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Tier Labels frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetTierLabelsReq
(
zclPrice_GetTierLabelsReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetTierLabels_c, sizeof(zclCmdPrice_GetTierLabelsReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishTierLabelsRsp(zclPrice_PublishTierLabelsRsp_t *pReq,uint8_t length)
 *
 * @brief	Sends over-the-air a Publish Tier Labels frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishTierLabelsRsp
(
zclPrice_PublishTierLabelsRsp_t *pReq,
uint8_t length
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishTierLabelsRsp_c,  sizeof(zclCmdPrice_PublishTierLabelsRsp_t) + 
                                    length - sizeof(zclTierEntry_t) ,(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetTariffInformationReq(zclPrice_GetTariffInformationReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Tariff Information frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetTariffInformationReq
(
zclPrice_GetTariffInformationReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetTariffInformation_c, sizeof(zclCmdPrice_GetTariffInformationReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetCurrencyConversionReq(zclPrice_GetCurrencyConversionReq *pReq)
 *
 * @brief	Sends over-the-air a Get Currency Conversion frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetCurrencyConversionReq
(
zclPrice_GetCurrencyConversionReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetCurrencyConversion_c, 0,(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_GetConsolidatedBillReq(zclPrice_GetConsolidatedBillReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Consolidated Bill frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetConsolidatedBillReq
(
zclPrice_GetConsolidatedBillReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetConsolidatedBill_c, sizeof(zclCmdPrice_GetConsolidatedBillReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishConsolidatedBillRsp(zclPrice_PublishConsolidatedBillRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Consolidated Bill frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishConsolidatedBillRsp
(
zclPrice_PublishConsolidatedBillRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishConsolidatedBillRsp_c,  sizeof(zclCmdPrice_PublishConsolidatedBillRsp_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_CPPEventRsp(zclPrice_CPPEventRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Get CPP Event Response frame from the client side. 
 *
 */
zbStatus_t zclPrice_CPPEventRsp
(
zclPrice_CPPEventRsp_t *pRsp
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetCPPEventResponse_c, sizeof(zclCmdPrice_CPPEventRsp_t),(zclGenericReq_t *)pRsp);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishCPPEventRsp(zclPrice_PublishCPPEventRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish CPP Event frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishCPPEventRsp
(
zclPrice_PublishCPPEventRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishCPPEventRsp_c,  sizeof(zclCmdPrice_PublishCPPEventRsp_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_PublishCreditPaymentRsp(zclPrice_PublishCreditPaymentRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Credit Prepayment frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishCreditPaymentRsp
(
zclPrice_PublishCreditPaymentRsp_t *pReq
)
{
  uint8_t length;
  
  /* Strip the unused string location */
  length = sizeof(zclCmdPrice_PublishCreditPaymentRsp_t) - (sizeof(zclStr20_t) - sizeof(uint8_t) - pReq->cmdFrame.CreditPaymentRef.length);
  
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishCreditPaymentRsp_c, length,(zclGenericReq_t *)pReq);	
}


#if gInterPanCommunicationEnabled_c

/*!
 * @fn 		zbStatus_t zclPrice_InterPanGetCurrPriceReq(zclPrice_InterPanGetCurrPriceReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Current Price frame from the InterPAN client side. 
 *
 */
zbStatus_t zclPrice_InterPanGetCurrPriceReq
(
zclPrice_InterPanGetCurrPriceReq_t *pReq
)
{
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdPrice_GetCurrPriceReq_c, sizeof(zclCmdPrice_GetCurrPriceReq_t), pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_InterPanGetScheduledPricesReq(zclPrice_InterPanGetScheduledPricesReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Scheduled Prices frame from the InterPAN client side. 
 *
 */
zbStatus_t zclPrice_InterPanGetScheduledPricesReq
(
zclPrice_InterPanGetScheduledPricesReq_t *pReq
)
{ 
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdPrice_GetScheduledPricesReq_c, sizeof(zclCmdPrice_GetScheduledPricesReq_t), pReq);	
}

/*!
 * @fn 		zbStatus_t zclPrice_InterPanPublishPriceRsp(zclPrice_InterPanPublishPriceRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Get Scheduled Prices frame from the InterPAN server side. 
 *
 */
zbStatus_t zclPrice_InterPanPublishPriceRsp
(
zclPrice_InterPanPublishPriceRsp_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  /*NOTE - depending on whether the Rate Label has a fixed length this function needs to package
  the telegram.
  The Stucture allows for 12 bytes of text, which is a size of 12 including the length.(11 byte of data)     
  aka
  copy from currentTime to rest of telegram to the location of Ratelabel+sizeof(length)+length.
  
  */
  pSrc  = (uint8_t *) &pReq->cmdFrame.IssuerEvt[0];
  pDst  = &pReq->cmdFrame.RateLabel.aStr[pReq->cmdFrame.RateLabel.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrice_PublishPriceRsp_t) - (sizeof(ProviderID_t) + sizeof(uint8_t) + pReq->cmdFrame.RateLabel.length);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
  length = sizeof(zclCmdPrice_PublishPriceRsp_t) - (11-pReq->cmdFrame.RateLabel.length);
  return ZCL_SendInterPanServerRspSeqPassed(gZclCmdPrice_PublishPriceRsp_c, length, pReq);
}

#endif /* #if gInterPanCommunicationEnabled_c */

/*!
 * @fn 		void ZCL_DeleteClientPrice(uint8_t *pEvtId)
 *
 * @brief	Deletes a price on the client, given an event id.
 *
 */
void ZCL_DeleteClientPrice(uint8_t *pEvtId)
{
  uint8_t i;
  for(i = 0; i < gNumofClientPrices_c; i++)
  {
    publishPriceEntry_t *pEntry = &gaClientPriceTable[i];
    
    if (pEntry->EntryStatus != 0x00)
    {
      SEEvtId_t evtId;
      
      if (pEntry->IsExtended)
        FLib_MemCpy(&evtId, &pEntry->Price.ExtendedPrice.IssuerEvt, sizeof(SEEvtId_t));
      else
        FLib_MemCpy(&evtId, &pEntry->Price.Price.IssuerEvt, sizeof(SEEvtId_t));
        
      if(FLib_MemCmp(pEvtId, evtId, 4))
      {
        gaClientPriceTable[i].EntryStatus = 0x00;
        return;
      }
      
    }  
  }
}

/*!
 * @fn 		void ZCL_DeleteServerScheduledPrices(void)
 *
 * @brief	Deletes all prices on the server.
 *
 */
void ZCL_DeleteServerScheduledPrices(void)
{
  uint8_t i;
  for(i = 0; i < gNumofServerPrices_c; i++)
    gaServerPriceTable[i].EntryStatus = 0x00;
}

/*!
 * @fn 		zbStatus_t ZCL_UpdateServerPriceEvents( zclCmdPrice_PublishPriceRsp_t *pMsg)
 *
 * @brief	Update a price from the Provider.In the case of an update to the
 * 			pricing information from the commodity provider, the Publish Price command
 * 			should be unicast to all individually registered devices implementing the Price
 * 			Cluster on the ZigBee Smart Energy network. When responding to a request via 
 * 			the Inter-PAN SAP, the Publish Price command should be broadcast to the PAN of
 * 			the requester after a random delay between 0 and 0.5 seconds, to avoid a potential
 * 			broadcast storm of packets.
 *
 */
zbStatus_t ZCL_UpdateServerPriceEvents( zclCmdPrice_PublishPriceRsp_t *pMsg)
{
  uint8_t updateIndex;
  
  /* Shift information to compensate the label length*/
  FLib_MemInPlaceCpy(&pMsg->IssuerEvt, 
                     &pMsg->RateLabel.aStr[pMsg->RateLabel.length],
                     sizeof(zclCmdPrice_PublishPriceRsp_t) - sizeof(uint32_t) - sizeof(zclStr12Oct_t));
   
  updateIndex = CheckForPriceUpdate((zclCmdPrice_t *)pMsg, (publishPriceEntry_t *)&gaServerPriceTable[0], gNumofServerPrices_c, FALSE);
  /* the Price is updated??? */
  if (updateIndex < 0xFE)
  {
    /* Send the Publish Prices to all SE or Non-SE registered devices */
    mInterPanIndex = 0;
    mUpdatePriceIndex = updateIndex;
    SendPriceClusterEvt(gzclEvtHandlePublishPriceUpdate_c);
    return gZclSuccess_c;
  }
  else
    return gZclFailure_c;
}

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerPriceEvents ( ztcCmdPrice_PublishPriceRsp_t *pMsg, bool_t IsExtended)
 *
 * @brief	Store Price information received from the Provider 
 * 			The server side doesn't keep track of the price status, only stores the
 * 			received prices and take care that Nested and overlapping Publish Price commands not to occur.
 *
 */	 
zbStatus_t ZCL_ScheduleServerPriceEvents ( ztcCmdPrice_PublishPriceRsp_t *pMsg, bool_t IsExtended)
{
  uint8_t i, newEntry;
  uint32_t currentTime, startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime; 
  uint16_t duration;
  //0 if no overlapp exception, 1- gNumofServerPrices_c if a overlapp exception occurs
  bool_t overlapException = 0;
  
  /* Shift information to compensate the label length*/
  FLib_MemInPlaceCpy(&pMsg->publishPriceRsp.IssuerEvt, 
                     &pMsg->publishPriceRsp.RateLabel.aStr[pMsg->publishPriceRsp.RateLabel.length],
                     sizeof(ztcCmdPrice_PublishPriceRsp_t) - sizeof(uint32_t) - sizeof(zclStr12Oct_t));
    
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  msgStartTime = OTA2Native32(pMsg->publishPriceRsp.StartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  duration = OTA2Native16(pMsg->publishPriceRsp.DurationInMinutes);
  msgStopTime = msgStartTime + (60 * (uint32_t)duration);
  
  /* Nested and overlapping Publish Price commands are not allowed */
  for(i = 0; i < gNumofServerPrices_c; i++)
  {
    if(gaServerPriceTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    if (gaServerPriceTable[i].IsExtended)
    {
      startTime = OTA2Native32(gaServerPriceTable[i].Price.ExtendedPrice.StartTime);
      duration = OTA2Native16(gaServerPriceTable[i].Price.ExtendedPrice.DurationInMinutes);
    }
    else
    {
      startTime = OTA2Native32(gaServerPriceTable[i].Price.Price.StartTime);
      duration = OTA2Native16(gaServerPriceTable[i].Price.Price.DurationInMinutes);
    }
    
    stopTime = startTime + (60 * (uint32_t)duration);
    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
    {
      continue;
    }else
    {
      /*
      The only exception to this is that if an event with a newer Issuer Event ID
      overlaps with the end of the current active price but is not yet active,
      the active price is not deleted but its duration is modified to 0xFFFF 
      (until changed) so that the active price ends when the new event begins
      */
      if((startTime <= currentTime)&&(stopTime>currentTime)&&(currentTime <= msgStartTime))
      {
        overlapException = i+1;
        break;
      }
      /*the price overlapp... take no action*/
      return gZclFailure_c;
    }
  }
  
  if((overlapException != 0) && (currentTime == msgStartTime))
    return ZCL_UpdateServerPriceEvents(&pMsg->publishPriceRsp);
  
  newEntry = AddPriceInTable((publishPriceEntry_t *)&gaServerPriceTable[0], gNumofServerPrices_c, &pMsg->publishPriceRsp, IsExtended);
  if(newEntry == 0xff)
    return gZclFailure_c;

  if(overlapException != 0)
  {
    if (gaServerPriceTable[i].IsExtended)
      gaServerPriceTable[overlapException-1].Price.ExtendedPrice.DurationInMinutes = 0xFFFF;  
    else
      gaServerPriceTable[overlapException-1].Price.Price.DurationInMinutes = 0xFFFF;  
  }
   
  if(pMsg->SendUnsolicited)
  {
     mInterPanIndex = 0;
     mUpdatePriceIndex = newEntry;
     SendPriceClusterEvt(gzclEvtHandlePublishPriceUpdate_c);
  }
  return gZclSuccess_c;
} 
	 

#if gASL_ZclPrice_Optionals_d
/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerBlockPeriodsEvents ( ztcCmdPrice_PublishBlockPeriodRsp_t *pMsg)
 *
 * @brief	Store Block Periods information received from the Provider 
 * 			The server side doesn't keep track of the Block Periods status, only stores the 
 * 			received Block Periods and take care that Nested and overlapping Block Periods commands not to occur
 *
 */
zbStatus_t ZCL_ScheduleServerBlockPeriodsEvents ( ztcCmdPrice_PublishBlockPeriodRsp_t *pMsg)
{
  uint8_t i, newEntry;
  uint32_t currentTime, startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime; 
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  msgStartTime = OTA2Native32(pMsg->publishBlockPeriodRsp.BlockPeriodStartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  
  msgStopTime = msgStartTime + (60 * Zcl_GetDuration(pMsg->publishBlockPeriodRsp.BlockPeriodDuration));
  
  //if(msgStopTime <= currentTime)
  //	return status;
  
  /* Nested and overlapping Block Periods commands are not allowed */
  for(i = 0; i < gNumofServerBlockPeriods_c; i++)
  {
    
    if(gaServerBlockPeriodsTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerBlockPeriodsTable[i].EffectiveStartTime);
    stopTime = startTime + (60 * Zcl_GetDuration(pMsg->publishBlockPeriodRsp.BlockPeriodDuration));
    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
      continue;
    /*the price overlapp... take no action*/
    return gZclFailure_c;
  }
    
  newEntry = AddBlockPeriodsInTable((publishBlockPeriodEntry_t *)&gaServerBlockPeriodsTable[0], gNumofServerBlockPeriods_c, &pMsg->publishBlockPeriodRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  
  /* Send unsolicited update */
  if(pMsg->SendUnsolicited)
    ZCL_HandleBlockPeriodUpdate(newEntry);
  
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerBillingPeriodEvents ( ztcCmdPrice_PublishBillingPeriodRsp_t *pMsg)
 *
 * @brief	Store Billing Periods information received from the Provider 
 * 			The server side doesn't keep track of the Billing Periods status, only stores the 
 * 			received Billing Periods and take care that Nested and overlapping Billing Periods commands not to occur
 *
 */	 
zbStatus_t ZCL_ScheduleServerBillingPeriodEvents ( ztcCmdPrice_PublishBillingPeriodRsp_t *pMsg)
{
  uint8_t i, newEntry;
  uint32_t startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime; 
  
  msgStartTime = OTA2Native32(pMsg->publishBillingPeriodRsp.BillingPeriodStartTime);
  if (msgStartTime == 0x00000000)
    msgStartTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  
  msgStopTime = msgStartTime + (60 * Zcl_GetDuration(pMsg->publishBillingPeriodRsp.BillingPeriodDuration));
  
  /* Nested and overlapping Block Periods commands are not allowed */
  for(i = 0; i < gNumofServerBillingPeriods_c; i++)
  {
    
    if(gaServerBillingPeriodTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerBillingPeriodTable[i].EffectiveStartTime);
    stopTime = startTime + (60 * Zcl_GetDuration(pMsg->publishBillingPeriodRsp.BillingPeriodDuration));
    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
      continue;
    /*the price overlapp... take no action*/
    return gZclFailure_c;
  }
    
  newEntry = AddBillingPeriodsInTable((publishBillingPeriodEntry_t *)&gaServerBillingPeriodTable[0], gNumofServerBillingPeriods_c, &pMsg->publishBillingPeriodRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  
  /* Send unsolicited update */
  if(pMsg->SendUnsolicited)
    ZCL_HandleBillingPeriodUpdate(newEntry);
  
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t ZCL_StoreServerBlockThresholds( ztcCmdPrice_PublishBlockThresholdsRsp_t *pMsg)
 *
 * @brief	Store on server the block thresholds received from the Provider
 *
 */	 
zbStatus_t ZCL_StoreServerBlockThresholds( ztcCmdPrice_PublishBlockThresholdsRsp_t *pMsg)
{
  uint8_t i, entryIdx = 0xFF, tariffEntryIdx, noBlockThresholdsInUse;
  publishBlockThresholdsEntry_t *pEntry;
  TierBlockThreshold_t *pDst;
  
  tariffEntryIdx = FindServerTariffInformationEntryByTariffId(&pMsg->IssuerTariffID);
  
  if (tariffEntryIdx == gInvalidTableIndex_c)
    return gZclNotFound_c;
  
  noBlockThresholdsInUse = gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.NumberBlockThreshholdsUse;

  /* Search for an existing entry*/
  for(i = 0; i < gNumofServerBlockThresholds_c; i++)
  {    
    if(FLib_MemCmp(gaServerBlockThresholdsTable[i].IssuerTariffID, pMsg->IssuerTariffID, sizeof(SEEvtId_t)))
    {
      entryIdx = i;
      break;
    }
  }
  
  /*If it is a new entry find an empty slot for it*/
  if (entryIdx == 0xFF)
    for(i = 0; i < gNumofServerBlockThresholds_c; i++)
    {    
      if(gaServerBlockThresholdsTable[i].EntryStatus == gEntryNotUsed_c)
      {
        entryIdx = i;
        break;
      }
    }
  
  /* Cannot store infromation*/  
  if(entryIdx == 0xff)
    return gZclFailure_c;
  
  pEntry = &gaServerBlockThresholdsTable[entryIdx]; 
  
  /* Save the information in the Price Matrix Table*/
  FLib_MemCpy(pEntry->IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  FLib_MemCpy(&pEntry->IssuerTariffID, pMsg->IssuerTariffID, sizeof(SEEvtId_t));
  FLib_MemCpy(&pEntry->ProviderID, pMsg->ProviderID, sizeof(SEEvtId_t));
  pEntry->StartTime = pMsg->StartTime;
  pEntry->SubPayloadCtl = pMsg->SubPayloadCtl;
  pEntry->EntryStatus = gEntryUsed_c;
  
  if (pMsg->TierNumber > 0)
    pDst = (TierBlockThreshold_t *)((uint8_t*)&pEntry->BlockThresholds[0] + (pMsg->TierNumber - 1) * (noBlockThresholdsInUse * sizeof(BlockThreshold_t) + sizeof(uint8_t)));
  else
    pDst = &pEntry->BlockThresholds[0];
  pDst->id.tierBlockID.blockNum = pMsg->Length;
  pDst->id.tierBlockID.tierNum = pMsg->TierNumber;
  FLib_MemCpy(&pDst->aBlockThresholds[0], &pMsg->aBlockThresholds[0], noBlockThresholdsInUse * sizeof(BlockThreshold_t));
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t ZCL_StoreServerPriceMatrix( ztcCmdPrice_PublishPriceMatrixRsp_t *pMsg)
 *
 * @brief	Store on server the Price Matrix received from the provider
 *
 */	 
zbStatus_t ZCL_StoreServerPriceMatrix( ztcCmdPrice_PublishPriceMatrixRsp_t *pMsg)
{
  uint8_t i, entryIdx = 0xFF;
  publishPriceMatrixEntry_t *pEntry;
  
  /* Search for an existing entry*/
  for(i = 0; i < gNumofServerPriceMatrix_c; i++)
  {    
    if(FLib_MemCmp(gaServerPriceMatrixTable[i].IssuerTariffID, pMsg->IssuerTariffID, sizeof(SEEvtId_t)))
    {
      entryIdx = i;
      break;
    }
  }
  
  /*If it is a new entry fin an empty slot for it*/
  if (entryIdx == 0xFF)
    for(i = 0; i < gNumofServerPriceMatrix_c; i++)
    {    
      if(gaServerPriceMatrixTable[i].EntryStatus == gEntryNotUsed_c)
      {
        entryIdx = i;
        break;
      }
    }
  
  /* Cannot store infromation*/  
  if(entryIdx == 0xff)
    return gZclFailure_c;
  
  /* Save the information in the Price Matrix Table*/
  pEntry = &gaServerPriceMatrixTable[entryIdx];
  FLib_MemCpy(pEntry->IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  FLib_MemCpy(pEntry->IssuerTariffID, pMsg->IssuerTariffID, sizeof(SEEvtId_t));
  FLib_MemCpy(pEntry->ProviderID, pMsg->ProviderID, sizeof(SEEvtId_t));
  pEntry->StartTime = pMsg->StartTime;
  pEntry->Length = pMsg->Offset + pMsg->Length;
  pEntry->EntryStatus = gEntryUsed_c;
  pEntry->SubPayloadCtl = pMsg->SubPayloadCtl;
  FLib_MemCpy(&pEntry->TierBlockPrice[pMsg->Offset], &pMsg->TierBlockPrice[0], pMsg->Length * sizeof(TierBlockPrice_t));
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t ZCL_StoreServerTierLabels( ztcCmdPrice_PublishTierLabelsRsp_t *pMsg)
 *
 * @brief	Store on server the Tier Labels received from the provider
 *
 */	 
zbStatus_t ZCL_StoreServerTierLabels( ztcCmdPrice_PublishTierLabelsRsp_t *pMsg)
{
  uint8_t i, entryIdx = 0xFF;
  publishTierLabelsEntry_t *pEntry;
  
  /* Search for an existing entry*/
  for(i = 0; i < gNumofServerTierLabels_c; i++)
  {    
    if(FLib_MemCmp(gaServerTierLabelsTable[i].IssuerTariffID, pMsg->IssuerTariffID, sizeof(SEEvtId_t)))
    {
      entryIdx = i;
      break;
    }
  }
  
  /*If it is a new entry fin an empty slot for it*/
  if (entryIdx == 0xFF)
    for(i = 0; i < gNumofServerTierLabels_c; i++)
    {    
      if(gaServerTierLabelsTable[i].EntryStatus == gEntryNotUsed_c)
      {
        entryIdx = i;
        break;
      }
    }
  
  /* Cannot store infromation*/  
  if(entryIdx == 0xff)
    return gZclFailure_c;
  
  /* Save the information in the Tier Labels Table*/
  pEntry = &gaServerTierLabelsTable[entryIdx];
  FLib_MemCpy(pEntry->IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  FLib_MemCpy(pEntry->IssuerTariffID, pMsg->IssuerTariffID, sizeof(SEEvtId_t));
  FLib_MemCpy(pEntry->ProviderID, pMsg->ProviderID, sizeof(SEEvtId_t));
  pEntry->Length = pMsg->Offset + pMsg->Length;
  pEntry->EntryStatus = gEntryUsed_c;
  FLib_MemCpy((uint8_t*)pEntry->TierLabel + pMsg->Offset, &pMsg->TierLabel[0], pMsg->Length);
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerTariffInformationEvents ( ztcCmdPrice_PublishTariffInformationRsp_t *pMsg)
 *
 * @brief	Stores on server the  Tariff Information received from the Provider.
 *
 *
 */
zbStatus_t ZCL_ScheduleServerTariffInformationEvents ( ztcCmdPrice_PublishTariffInformationRsp_t *pMsg)
{
  uint8_t newEntry;
  
  /* Shift information to compensate the label length*/
  FLib_MemInPlaceCpy(&pMsg->publishTariffInformationRsp.NumberPriceTiersUse, 
                     &pMsg->publishTariffInformationRsp.TariffLabel.aStr[pMsg->publishTariffInformationRsp.TariffLabel.length],
                     sizeof(ztcCmdPrice_PublishTariffInformationRsp_t) - 4*sizeof(uint32_t) - 1 - sizeof(zclStr24Oct_t));

  newEntry = AddTariffInformationInTable((publishTariffInformationEntry_t *)&gaServerTariffInformationTable[0], gNumofServerTariffInformation_c, &pMsg->publishTariffInformationRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  if(pMsg->SendUnsolicited)
  {
    ZCL_HandleTariffInformationUpdate(newEntry);
  }
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerCO2ValueEvents ( ztcCmdPrice_PublishCO2ValueRsp_t *pMsg)
 *
 * @brief	Stores on server the CO2 Value Event received from the Provider.
 *
 *
 */
zbStatus_t ZCL_ScheduleServerCO2ValueEvents ( ztcCmdPrice_PublishCO2ValueRsp_t *pMsg)
{
  uint8_t iEntryIdx;
    
  iEntryIdx = AddCO2ValueInTable(&gaServerCO2ValueTable[0], gNumofServerCO2Value_c, &pMsg->publishCO2ValueRsp);
 
  if(iEntryIdx == gInvalidTableIndex_c)
    return gZclFailure_c;
  
  if(pMsg->SendUnsolicited)
  {
    ZCL_HandleCO2ValueUpdate(iEntryIdx);
  }
  
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerConsolidatedBillEvents ( ztcCmdPrice_PublishConsolidatedBillRsp_t *pMsg)
 *
 * @brief	Stores on server Consolidated Bill information received from the Provider .
 *          The server side doesn't keep track of the Consolidated Bill status, only stores the 
 *          received Consolidated Bill and take care that Nested and overlapping Consolidated Bill commands not to occur.
 */
zbStatus_t ZCL_ScheduleServerConsolidatedBillEvents ( ztcCmdPrice_PublishConsolidatedBillRsp_t *pMsg)
{
  uint8_t i, newEntry;
  uint32_t startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime; 
  
  msgStartTime = OTA2Native32(pMsg->publishConsolidatedBillRsp.BillingPeriodStartTime);
  if (msgStartTime == 0x00000000)
    msgStartTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  
  msgStopTime = msgStartTime + (60 * Zcl_GetDuration(pMsg->publishConsolidatedBillRsp.BillingPeriodDuration));
  
  /* Nested and overlapping Block Periods commands are not allowed */
  for(i = 0; i < gNumofServerConsolidatedBill_c; i++)
  {
    
    if(gaServerConsolidatedBillTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerConsolidatedBillTable[i].EffectiveStartTime);
    stopTime = startTime + (60 * Zcl_GetDuration(pMsg->publishConsolidatedBillRsp.BillingPeriodDuration));
    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
      continue;
    /*the event overlaps... take no action*/
    return gZclFailure_c;
  }
    
  newEntry = AddConsolidatedBillsInTable((publishConsolidatedBillEntry_t *)&gaServerConsolidatedBillTable[0], gNumofServerConsolidatedBill_c, &pMsg->publishConsolidatedBillRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  
  /* Send unsolicited update */
  if(pMsg->SendUnsolicited)
    ZCL_HandleConsolidatedBillUpdate(newEntry);
  
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerCPPEvents(zclCmdPrice_PublishCPPEventRsp_t *pMsg)
 *
 * @brief	Stores on server the CPP Event received from the Provider.
 *
 *
 */
zbStatus_t ZCL_ScheduleServerCPPEvents(zclCmdPrice_PublishCPPEventRsp_t *pMsg)
{
  uint8_t i, newEntry;
  ZCLTime_t startTime, stopTime, msgStartTime, msgStopTime; 
  uint16_t duration;
  
  /* here get the currentTime */
  msgStartTime = OTA2Native32(pMsg->CPPEventStartTime);
  if(msgStartTime == 0x00000000)
    msgStartTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
    
  duration = GetNative16BitInt(pMsg->CPPEventDurationInMinutes);
  msgStopTime = msgStartTime + (uint32_t)duration * 60;
  
  /* Nested and overlapping CPP Events commands are not allowed */
  for(i = 0; i < gNumofServerCPPEvents_c; i++)
  {    
    if(gaServerCPPEventTable[i].EntryStatus == 0x00)
      continue;
    
    /* Get the timing */
    startTime = OTA2Native32(gaServerCPPEventTable[i].EffectiveStartTime);
    stopTime = startTime + + (uint32_t)duration * 60;
    
    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
      continue;
    /*the period overlaps... take no action*/
    return gZclFailure_c;
  }
  newEntry = AddCPPEventsInTable((publishCPPEventEntry_t *)&gaServerCPPEventTable[0], gNumofServerCPPEvents_c, pMsg);
  if(newEntry == 0xff)
    return gZclFailure_c;
  
  ZCL_HandleCPPEventUpdate(newEntry);
  
  return gZclSuccess_c;
} 
#endif


#if gASL_ZclPrice_Optionals_d
/*!
 * @fn 		void ZCL_DeleteScheduleServerBlockPeriods(void)
 *
 * @brief	Deletes all Block Periods stored on the server.
 *
 *
 */
void ZCL_DeleteScheduleServerBlockPeriods(void)
{
  uint8_t i;
  for(i = 0; i < gNumofServerBlockPeriods_c; i++)
    gaServerBlockPeriodsTable[i].EntryStatus = 0x00;
}
#endif

/*!
 * @fn 		zbStatus_t ZCL_UpdateServerBlockPeriodEvents(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg)
 *
 * @brief	Updates on server Block Period information from the Provider.
 *
 *
 */
#if gASL_ZclPrice_Optionals_d
zbStatus_t ZCL_UpdateServerBlockPeriodEvents(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg)
{
  uint8_t updateIndex;
  
  updateIndex = CheckForBlockPeriodUpdate(pMsg, (publishBlockPeriodEntry_t *)&gaServerBlockPeriodsTable[0], gNumofServerBlockPeriods_c);
  /* the Block Period is updated??? */
  if (updateIndex < 0xFE)
  {
    ZCL_HandleBlockPeriodUpdate(updateIndex);
    return gZclSuccess_c;
  }
  else
    return gZclFailure_c;
}


/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerCalorificValueStore ( ztcCmdPrice_PublishCalorificValueRsp_t *pMsg)
 *
 * @brief	Stores on server Calorific Values from the Provider.
 *
 *
 */
zbStatus_t ZCL_ScheduleServerCalorificValueStore ( ztcCmdPrice_PublishCalorificValueRsp_t *pMsg)
{
  uint8_t newEntry;
    
  newEntry = AddCalorificValueInTable(&gaServerCalorificValueTable[0], gNumofServerCalorificValue_c, &pMsg->publishCalorificValueRsp);
 
  if(newEntry == 0xff)
    return gZclFailure_c;
  
  if(pMsg->SendUnsolicited)
  {
    ZCL_HandleCalorificValueUpdate(newEntry);
  }
  return gZclSuccess_c;
} 

/*!
 * @fn 		void ZCL_DeleteServerCalorificValueStore(void)
 *
 * @brief	Deletes all Calorific Values on server.
 *
 */
void ZCL_DeleteServerCalorificValueStore(void)
{
  uint8_t i;
  for(i = 0; i < gNumofServerCalorificValue_c; i++)
    gaServerCalorificValueTable[i].EntryStatus = 0x00;
}

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerConversionFactorStore ( ztcCmdPrice_PublishConversionFactorRsp_t *pMsg)
 *
 * @brief	Stores on server Conversion Factor information from the Provider.
 *
 */
zbStatus_t ZCL_ScheduleServerConversionFactorStore ( ztcCmdPrice_PublishConversionFactorRsp_t *pMsg)
{
  uint8_t newEntry;
    
  newEntry = AddConversionFactorInTable(&gaServerConversionFactorTable[0], gNumofServerConversionFactors_c, &pMsg->publishConversionFactorRsp);
 
  if(newEntry == 0xff)
    return gZclFailure_c;
  
  if(pMsg->SendUnsolicited)
  {
    ZCL_HandleConversionFactorUpdate(newEntry);
  }
  return gZclSuccess_c;
} 

/*!
 * @fn 		void ZCL_DeleteServerConversionFactor(void)
 *
 * @brief	Deletes all Conversion Factor information on server.
 *
 */
void ZCL_DeleteServerConversionFactor(void)
{
  uint8_t i;
  for(i = 0; i < gNumofServerConversionFactors_c; i++)
    gaServerConversionFactorTable[i].EntryStatus = 0x00;
}
#endif

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_Optionals_d
/*!
 * @fn 		zbStatus_t ZCL_StoreServerCreditPayment (ztcCmdPrice_PublishCreditPaymentRsp_t *pMsg)
 *
 * @brief	Stores on server Credit Payment information from the Provider.
 *
 */
zbStatus_t ZCL_StoreServerCreditPayment (ztcCmdPrice_PublishCreditPaymentRsp_t *pMsg)
{
  uint8_t newEntry;
    
  newEntry = AddCreditPaymentsInTable((publishCreditPaymentEntry_t *)&gaServerCreditPaymentTable[0], gNumofServerCreditPayment_c, &pMsg->publishCreditPaymentRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  
  /* Compensate for the variable length of the CreditPaymentRef parameter */
  pMsg->SendUnsolicited = pMsg->publishCreditPaymentRsp.CreditPaymentRef.aStr[pMsg->publishCreditPaymentRsp.CreditPaymentRef.length];
  
  /* Send unsolicited update */
  if(pMsg->SendUnsolicited)
    ZCL_HandleCreditPaymentUpdate(newEntry);
  
  return gZclSuccess_c;
} 

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerCurrencyConversion (ztcCmdPrice_PublishCurrencyConversionRsp_t *pMsg)
 *
 * @brief	Stores on server Currency Conversion information from the Provider.
 *
 */
zbStatus_t ZCL_ScheduleServerCurrencyConversion (ztcCmdPrice_PublishCurrencyConversionRsp_t *pMsg)
{
  uint8_t status;
    
  status = AddCurrencyConversionInTable(&gServerCurrencyConversion, 1, &pMsg->publishCurrencyConversionRsp);
  if(status != gZclSuccess_c)
    return gZclFailure_c;
  
  /* Send unsolicited update */
  if(pMsg->SendUnsolicited)
    ZCL_HandleCurrencyConversionUpdate();
  
  return gZclSuccess_c;
} 
#endif

/*!
 * @fn 		void ZCL_HandleSEPriceClusterEvt(void)
 *
 * @brief	Handle the Price Cluster Event.
 *
 */
void ZCL_HandleSEPriceClusterEvt(void)
{
  uint32_t events = mPriceClusterEvt;  

#if gASL_ZclPrice_PublishPriceRsp_d
  
#if gASL_ZclPrice_Optionals_d 
  if (events & gzclEvtHandleGetScheduledPrices_c)
    ZCL_HandleGetScheduledPrices();
#endif
  
#else
  (void)events;
#endif
  
  if(events & gzclEvtHandlePublishPriceUpdate_c)
  	ZCL_HandlePublishPriceUpdate();

#if gASL_ZclPrice_PublishConversionFactorRsp_d
#if gASL_ZclPrice_Optionals_d 
  if(events & gzclEvtHandleGetConversionFactor_c)
        ZCL_HandleGetConversionFactor();
#endif
#endif

#if gASL_ZclPrice_PublishCalorificValueRsp_d
#if gASL_ZclPrice_Optionals_d 
  if(events & gzclEvtHandleGetCalorificValue_c)
        ZCL_HandleGetCalorificValue();
#endif
#endif
  
#if gASL_ZclPrice_PublishBlockPeriodsRsp_d
#if gASL_ZclPrice_Optionals_d 
  if(events & gzclEvtHandleGetBlockPeriods_c)
        ZCL_HandleGetBlockPeriods();
#endif
#endif
  
#if gASL_ZclPrice_GetCurrPriceReq_d || gASL_ZclPrice_GetSheduledPricesReq_d   
  if (events & gzclEvtHandleClientPrices_c)
    ZCL_HandleClientPrices();
#endif
  
#if gASL_ZclPrice_Optionals_d  
#if gASL_ZclPrice_GetBlockPeriodsReq_d
   if (events & gzclEvtHandleClientBlockPeriod_c)
    ZCL_HandleClientBlockPeriod();
#endif
#endif

#if gASL_ZclSE_12_Features_d
#if gASL_ZclPrice_PublishBillingPeriodRsp_d
  if(events & gzclEvtHandleGetBillingPeriod_c)
    ZCL_HandleGetBillingPeriod();
#endif  
#endif   
   
#if gASL_ZclSE_12_Features_d  
#if gASL_ZclPrice_GetBillingPeriodReq_d
  if (events & gzclEvtHandleClientBillingPeriod_c)
    ZCL_HandleClientBillingPeriod();
#endif
#endif   

#if gASL_ZclSE_12_Features_d
#if gASL_ZclPrice_PublishPriceMatrixRsp_d  
  if(events & gzclEvtHandleGetPriceMatrix_c)
    ZCL_HandleGetPriceMatrix();
#endif  
#endif  

#if gASL_ZclSE_12_Features_d
#if gASL_ZclPrice_PublishTierLabelsRsp_d  
  if(events & gzclEvtHandleGetTierLabels_c)
    ZCL_HandleGetTierLabels();
#endif  
#endif  

#if gASL_ZclSE_12_Features_d
#if gASL_ZclPrice_PublishBlockThresholdsRsp_d  
  if(events & gzclEvtHandleGetBlockThresholds_c)
    ZCL_HandleGetBlockThresholds();
#endif  
#endif  

#if gASL_ZclSE_12_Features_d
#if gASL_ZclPrice_PublishCO2ValueRsp_d  
  if(events & gzclEvtHandleGetCO2Value_c)
    ZCL_HandleGetCO2Value();
#endif  
#endif  

#if gASL_ZclSE_12_Features_d
#if gASL_ZclPrice_PublishTariffInformationRsp_d  
  if(events & gzclEvtHandleGetTariffInformation_c)
    ZCL_HandleGetTariffInformation();
#endif  
#endif  

#if gASL_ZclSE_12_Features_d 
#if gASL_ZclPrice_PublishConsolidatedBillRsp_d  
  if(events & gzclEvtHandleGetConsolidatedBill_c)
    ZCL_HandleGetConsolidatedBill();
#endif 
#endif  
   
#if gASL_ZclSE_12_Features_d  
#if gASL_ZclPrice_GetConsolidatedBillReq_d
  if (events & gzclEvtHandleClientConsolidatedBill_c)
    ZCL_HandleClientConsolidatedBill();
#endif
#endif   
    
}

/*!
 * @fn 		void SendPriceClusterEvt(uint32_t evtId)
 *
 * @brief	Send the Price Cluster Event to the TS.
 *
 */
/*  */
void SendPriceClusterEvt(uint32_t evtId)
{
  mPriceClusterEvt = evtId;
  ZclSE_SendClusterEvt(gzclEvtHandlePriceClusterEvt_c);
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
#if gASL_ZclPrice_Optionals_d || gASL_ZclSE_12_Features_d
static ZCLTime_t Zcl_GetDuration(Duration24_t aDuration)
{
  uint8_t unit;
  ZCLTime_t retDuration;
  Duration24_t aRetDuration;
  
  /* Use an auxiliary variable */
  FLib_MemCpy(aRetDuration, aDuration, sizeof(Duration24_t));
  /* Get the unit of measure*/
  unit = (aDuration[2] & 0xC0) >> 6;
  /* Clear the unit of measure */  
  aRetDuration[2] = aRetDuration[2] & 0x3F;
  /* Get integer value */
  retDuration = GetNative32BitIntFrom3ByteArray(aRetDuration);
    
  switch(unit)
  {
    case 0:
      /* Duration is in minutes */
      retDuration = retDuration; 
      break;
    case 1:
      /* Duration is in days */
      retDuration = retDuration * 60 * 24;
      break;
    case 2:
      /* Duration is in weeks */
      retDuration = retDuration * 60 * 24 * 7;
      break;
    case 3:
      /* Duration is in months */
      retDuration = retDuration * 60 * 24 * 7 * 4;
      break;
  }
  return retDuration;
}
#endif
#if gASL_ZclPrice_Optionals_d || gASL_ZclSE_12_Features_d
static void Zcl_SubtractDuration(Duration24_t aDuration, uint32_t timeInMinutes)
{
  uint8_t aRemDuration[4];
  ZCLTime_t remDuration = Zcl_GetDuration(aDuration) - timeInMinutes;
  
  SetNative32BitInt(aRemDuration, remDuration);
  FLib_MemCpy(aDuration, aRemDuration, sizeof(Duration24_t));
}
#endif
/******************************************************************************
* Get / Publish Price
******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/* The Publish Price command is generated in response to receiving a Get Current
Price command , a Get Scheduled Prices command , or when an update to the pricing information is available
from the commodity provider. */
static zbStatus_t ZCL_SendPublishPrice(addrInfoType_t *pAddrInfo, publishPriceEntry_t * pMsg, bool_t IsInterPanFlag) 
{
  zclPrice_PublishPriceRsp_t req;
  #if gInterPanCommunicationEnabled_c
  zclPrice_InterPanPublishPriceRsp_t interPanReq;
  #endif   
  ZCLTime_t currentTime, stopTime, newDuration = 0, startTime;
  Duration_t duration;
    
  if (pMsg->IsExtended)
  {
    startTime = OTA2Native32(pMsg->Price.ExtendedPrice.StartTime);
    duration = OTA2Native16(pMsg->Price.ExtendedPrice.DurationInMinutes);
  }
  else
  {
    startTime = OTA2Native32(pMsg->Price.Price.StartTime);
    duration = OTA2Native16(pMsg->Price.Price.DurationInMinutes);
  }

  /* Get the current time */
  currentTime = ZCL_GetUTCTime(pAddrInfo->addrInfo.srcEndPoint);
  if((startTime == 0x00000000) && (duration != 0xFFFF))
  {
    stopTime = OTA2Native32(pMsg->EffectiveStartTime) + 60 * (uint32_t)duration;
    newDuration = stopTime - currentTime;
  }
  
  if(!IsInterPanFlag)
  {
    FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
    req.zclTransactionId =  ZbZclFoundationGlobals(gZclTransactionId)++;
    FLib_MemCpy(&req.cmdFrame, pMsg, sizeof(zclCmdPrice_PublishPriceRsp_t));
    
    if (pMsg->IsExtended)
    {
      req.cmdFrame.ExtendedPrice.CurrTime = currentTime;
      if((startTime == 0x00000000) &&  (duration != 0xFFFF))
      {
        uint32_t rem = newDuration % 60;
        req.cmdFrame.ExtendedPrice.DurationInMinutes = Native2OTA16((uint16_t)(newDuration / 60 + ((rem > 30)?1:0)));
      }
      return zclPrice_PublishExtendedPriceRsp(&req);      
    }
    else
    {
      req.cmdFrame.Price.CurrTime = currentTime;
      if((startTime == 0x00000000) &&  (duration != 0xFFFF))
      {
        uint32_t rem = newDuration % 60;
        req.cmdFrame.Price.DurationInMinutes = Native2OTA16((uint16_t)(newDuration / 60 + ((rem > 30)?1:0)));
      }
      return zclPrice_PublishPriceRsp(&req);
    }
  }  
#if gInterPanCommunicationEnabled_c
  else	
  {
    FLib_MemCpy(&interPanReq.addrInfo, pAddrInfo, sizeof(InterPanAddrInfo_t));	
    interPanReq.zclTransactionId =  ZbZclFoundationGlobals(gZclTransactionId)++;
    FLib_MemCpy(&interPanReq.cmdFrame, pMsg, sizeof(zclCmdPrice_PublishPriceRsp_t));
    interPanReq.cmdFrame.CurrTime = currentTime;
    if(((pMsg->Price.Price.StartTime) == 0x00000000) &&  (pMsg->Price.Price.DurationInMinutes != 0xFFFF))
    {
      uint32_t rem = newDuration % 30;
      req.cmdFrame.Price.DurationInMinutes = Native2OTA16((uint16_t)(newDuration / 60 + (rem > 30)?1:0));
    }
    return zclPrice_InterPanPublishPriceRsp(&interPanReq);		
  } 
#else
return FALSE;
#endif     
}

static zbStatus_t ZCL_SendPriceAck(zbApsdeDataIndication_t *pIndication)
{
  uint32_t currentTime;
  zclPrice_PriceAck_t priceAck;
  zclCmdPrice_PublishPriceRsp_t * pMsg;
  
  pMsg = (zclCmdPrice_PublishPriceRsp_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  AF_PrepareForReply(&priceAck.addrInfo, pIndication);
  priceAck.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  FLib_MemCpy(&priceAck.cmdFrame.ProviderID, pMsg->ProviderID, sizeof(ProviderID_t));
  FLib_MemCpy(&priceAck.cmdFrame.IssuerEvt, pMsg->IssuerEvt, sizeof(SEEvtId_t));
  currentTime = ZCL_GetUTCTime(priceAck.addrInfo.srcEndPoint);
  currentTime = Native2OTA32(currentTime);
  priceAck.cmdFrame.PriceAckTime = currentTime;
  priceAck.cmdFrame.PriceControl = pMsg->PriceControl;
  
  return ZCL_PriceAck(&priceAck);
}

/******************************************************************************/
#if gInterPanCommunicationEnabled_c
static zbStatus_t ZCL_SendInterPriceAck(zbInterPanDataIndication_t *pIndication)
{
  uint32_t currentTime;
  zclPrice_InterPriceAck_t priceAck;
  zclCmdPrice_PublishPriceRsp_t * pMsg;
  
  pMsg = (zclCmdPrice_PublishPriceRsp_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  PrepareInterPanForReply((InterPanAddrInfo_t *)&priceAck.addrInfo, pIndication);
  priceAck.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  FLib_MemCpy(&priceAck.cmdFrame.ProviderID, pMsg->ProviderID, sizeof(ProviderID_t));
  FLib_MemCpy(&priceAck.cmdFrame.IssuerEvt, pMsg->IssuerEvt, sizeof(SEEvtId_t));
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  currentTime = Native2OTA32(currentTime);
  priceAck.cmdFrame.PriceAckTime = currentTime;
  priceAck.cmdFrame.PriceControl = pMsg->PriceControl;
  
  return ZCL_InterPriceAck(&priceAck);
}
#endif
/******************************************************************************/
/* Add Price in Table... so that to have the scheduled price in asccendent order */
static uint8_t AddPriceInTable(publishPriceEntry_t *pTable, uint8_t len, zclCmdPrice_PublishPriceRsp_t *pMsg, bool_t IsExtended)
{
  uint32_t startTime, msgStartTime, currentTime,stopTime;
  Duration_t duration;
  uint8_t i, poz = 0xff;
   /*
      The only exception to this is that if an event with a newer Issuer Event ID
      overlaps with the end of the current active price but is not yet active,
      the active price is not deleted but its duration is modified to 0xFFFF 
      (until changed) so that the active price ends when the new event begins
  */
  bool_t exceptOverlap = FALSE;
  bool_t verifyExceptOverlap = FALSE;
  
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->StartTime);
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the price in asccendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        if(poz == 1)
        {
          verifyExceptOverlap = TRUE;
        }
        break;
      }
    }
    else
    {
      poz = i;
       if(poz == 1)
       {
          verifyExceptOverlap = TRUE;
       }
      break;
    }
    
  }
  
  if (pTable->IsExtended)
    duration = OTA2Native16((pTable)->Price.ExtendedPrice.DurationInMinutes);
  else
    duration = OTA2Native16((pTable)->Price.Price.DurationInMinutes);
  
  if(verifyExceptOverlap)
  {
    startTime = OTA2Native32((pTable)->EffectiveStartTime);
    stopTime = startTime + (uint32_t)duration * 60;
    if((stopTime > msgStartTime) && (msgStartTime > currentTime))
    {
      exceptOverlap = TRUE;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus != gEntryNotUsed_c)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishPriceEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishPriceRsp_t));
  
  if((pMsg->StartTime == 0x00000000)||
     (pMsg->StartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->StartTime ;
  if(exceptOverlap)
  {
    if (pTable->IsExtended)
      (pTable)->Price.ExtendedPrice.DurationInMinutes = 0xFFFF;
    else
      (pTable)->Price.Price.DurationInMinutes = 0xFFFF;
  }
  
  (pTable+poz)->IsExtended = IsExtended;
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Price was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, (pTable+poz));
  return poz;
}

/******************************************************************************/
/*Check and Updated a Price.
When new pricing information is provided that replaces older pricing
information for the same time period, IssuerEvt field allows devices to determine which
information is newer. It is expected that the value contained in this field is a
unique number managed by upstream servers.
Thus, newer pricing information will have a value in the Issuer Event ID field that
is larger than older pricing information.
*/
static uint8_t CheckForPriceUpdate(zclCmdPrice_t *pMsg, publishPriceEntry_t *pTable, uint8_t len, bool_t IsExtended)
{
  uint8_t i;
  uint32_t msgIssuerEvt, entryIssuerEvt;
  ZCLTime_t startTime, entryStartTime;
  Duration_t duration, entryDuration;
  
  if (IsExtended)
  {
    msgIssuerEvt = FourBytesToUint32(OTA2Native32(pMsg->ExtendedPrice.IssuerEvt));
    startTime = OTA2Native32(pMsg->ExtendedPrice.StartTime);
    duration = OTA2Native16(pMsg->ExtendedPrice.DurationInMinutes);
  }
  else
  {
    msgIssuerEvt = FourBytesToUint32(OTA2Native32(pMsg->Price.IssuerEvt));
    startTime = OTA2Native32(pMsg->Price.StartTime);
    duration = OTA2Native16(pMsg->Price.DurationInMinutes);
  }
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00 )
    {
      if (IsExtended)
      {
        entryIssuerEvt = FourBytesToUint32(OTA2Native32((pTable+i)->Price.ExtendedPrice.IssuerEvt));
        entryStartTime = OTA2Native32((pTable+i)->Price.ExtendedPrice.StartTime);
        entryDuration = OTA2Native16((pTable+i)->Price.ExtendedPrice.DurationInMinutes);
      }
      else
      {
        entryIssuerEvt = FourBytesToUint32(OTA2Native32((pTable+i)->Price.Price.IssuerEvt));
        entryStartTime = OTA2Native32((pTable+i)->Price.Price.StartTime);
        entryDuration = OTA2Native16((pTable+i)->Price.Price.DurationInMinutes);
        
      }
      if ((entryStartTime == startTime && entryDuration == duration) ||
         ((startTime == 0x00000000) &&  entryDuration < duration))
      {
        if(entryIssuerEvt < msgIssuerEvt)
        {
          FLib_MemCpy((pTable+i), pMsg, sizeof(zclCmdPrice_PublishPriceRsp_t));
          
          if((startTime == 0x00000000)||
             (startTime == 0xffffffff))
          
          {
            /*  here get the currentTime  */
            (pTable+i)->EffectiveStartTime = Native2OTA32(ZCL_GetUTCTime(BeeAppDataInit(appEndPoint)));
          }
          else
            (pTable+i)->EffectiveStartTime = startTime;
          
          return i;
        }
        else 
          return 0xFE; /* reject it */
      }
    }
  }
  return 0xff;
  
}

/******************************************************************************/
/* The Price timer callback that keep track of current active price */
#if gASL_ZclPrice_GetCurrPriceReq_d || gASL_ZclPrice_GetSheduledPricesReq_d 
static void TimerClientPriceCallBack(zbTmrTimerID_t timerID)
{
  (void) timerID;
  SendPriceClusterEvt(gzclEvtHandleClientPrices_c);
}
#endif
/******************************************************************************/
#if gASL_ZclPrice_GetCurrPriceReq_d || gASL_ZclPrice_GetSheduledPricesReq_d  
/* Handle the Client Prices signalling when the current price starts, was updated or is completed */
static void ZCL_HandleClientPrices(void)
{
  uint32_t currentTime, startTime, nextTime=0x00000000, stopTime;
  uint16_t duration;
  publishPriceEntry_t *pEntry = &gaClientPriceTable[0];
  publishPriceEntry_t *pEntryNext = &gaClientPriceTable[1];
  
  /* the Price table is kept in ascendent order; check if any price is scheduled*/
  if(pEntry->EntryStatus == 0x00)
  {
    ZbTMR_StopTimer(gPriceClientTimerID);
    return;
  }
  
  /* Get the timing */
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  startTime = OTA2Native32(pEntry->EffectiveStartTime);

  if (pEntry->IsExtended)
    duration = OTA2Native16(pEntry->Price.ExtendedPrice.DurationInMinutes);
  else
    duration = OTA2Native16(pEntry->Price.Price.DurationInMinutes);
  
  stopTime = startTime + (60*(uint32_t)duration);
  
  /* Check if the duration id 0xFFFF*/
  if(duration == 0xFFFF)
  {// verify the next gaClientPriceTable entry
     if(pEntryNext->EntryStatus != 0x00)
     {
       startTime = OTA2Native32(pEntryNext->EffectiveStartTime);
       if(startTime <= currentTime)
       {
         pEntry->EntryStatus = gPriceCompletedStatus_c; /* entry is not used anymore */
         /* Call the App to signal that a Price was completed; User should check EntryStatus */
         BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, pEntry);
         
         /* Activate the new event*/
         FLib_MemInPlaceCpy(pEntry, (pEntry+1), (gNumofClientPrices_c-1) * sizeof(publishPriceEntry_t));
         gaClientPriceTable[1].EntryStatus = 0x00;
         //duration = OTA2Native16(pEntry->Price.DurationInMinutes);
         stopTime = startTime + (60*(uint32_t)duration);
         
         pEntry->EntryStatus = gPriceStartedStatus_c;
         /* Call the App to signal that a Price was started; User should check EntryStatus */
         BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, pEntry);
       }
     }
  }
  else
  {
  
    /* Check if the Price Event is completed */
    if(stopTime <= currentTime)
    {
      pEntry->EntryStatus = gPriceCompletedStatus_c; /* entry is not used anymore */
      /* Call the App to signal that a Price was completed; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, pEntry);
      pEntry->EntryStatus = 0x00;
      FLib_MemInPlaceCpy(pEntry, (pEntry+1), (gNumofClientPrices_c-1) * sizeof(publishPriceEntry_t));
      gaClientPriceTable[gNumofClientPrices_c-1].EntryStatus = 0x00;
      startTime = OTA2Native32(pEntry->EffectiveStartTime);
     // duration = OTA2Native16(pEntry->Price.DurationInMinutes);
    }
    else	
      if(startTime <= currentTime) /* check if the Price event have to be started or updated */
      {
        if((pEntry->EntryStatus == gPriceReceivedStatus_c)||
           (pEntry->EntryStatus == gPriceUpdateStatus_c))
        {
          pEntry->EntryStatus = gPriceStartedStatus_c;
          /* Call the App to signal that a Price was started; User should check EntryStatus */
          BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, pEntry);
        }
      }
  }
    
  if(currentTime < startTime)
    nextTime = startTime - currentTime;
  else
    if(currentTime < stopTime)
    {
      if(duration == 0xFFFF)
      {
         if(pEntryNext->EntryStatus != 0x00)
         {
           nextTime = pEntryNext->EffectiveStartTime;
         }
      }
      else
      nextTime = stopTime - currentTime;
    }
  if (nextTime)
    ZbTMR_StartSecondTimer(gPriceClientTimerID,(uint16_t)nextTime, TimerClientPriceCallBack);
  
}
#endif
/******************************************************************************/
/* Process the received Publish Price */
static zbStatus_t ZCL_ProcessClientPublishPrice(zclCmdPrice_PublishPriceRsp_t *pMsg, bool_t IsExtended)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  FLib_MemInPlaceCpy(&pMsg->IssuerEvt, &(pMsg->RateLabel.aStr[pMsg->RateLabel.length]), 
                     (sizeof(zclCmdPrice_PublishPriceRsp_t) - sizeof(ProviderID_t)- sizeof(zclStr12_t)));

  /* Check if it is a PriceUpdate */
  updateIndex = CheckForPriceUpdate((zclCmdPrice_t *)pMsg, (publishPriceEntry_t *)&gaClientPriceTable[0], gNumofClientPrices_c, IsExtended);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new Price information in the table */
    newEntry = AddPriceInTable((publishPriceEntry_t *)&gaClientPriceTable[0], gNumofClientPrices_c, pMsg, IsExtended);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      /* the price was updated */
      gaClientPriceTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      /* Call the App to signal that a Price was updated; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, &gaClientPriceTable[updateIndex]);
      status = gZclSuccess_c;
    }
  }
  
  if(status == gZclSuccess_c)
    SendPriceClusterEvt(gzclEvtHandleClientPrices_c);
  
  return status;
}

#if gInterPanCommunicationEnabled_c
/******************************************************************************/
static void StoreInterPanAddr(InterPanAddrInfo_t *pAddrInfo)
{
  uint8_t i, newEntry = 0xff;
  
  for(i = 0; i < gNumOfInterPanAddr_c; i++)
  {
    /* If PanID already exist in the table, overwrite the existing entry*/
    if(gaInterPanAddrTable[i].entryStatus != 0x00 &&
       FLib_MemCmp(&(pAddrInfo->dstPanId), &(gaInterPanAddrTable[i].addrInfo.dstPanId), sizeof(zbPanId_t)) )
    {
      FLib_MemCpy(&gaInterPanAddrTable[i], pAddrInfo, sizeof(InterPanAddrInfo_t));
      return;
    }
    if(newEntry == 0xff && 
       gaInterPanAddrTable[i].entryStatus ==0x00)
      newEntry = i;
  }
  
  if(newEntry != 0xff)
  {
    FLib_MemCpy(&gaInterPanAddrTable[newEntry], pAddrInfo, sizeof(InterPanAddrInfo_t));
    gaInterPanAddrTable[newEntry].entryStatus = 0xff;
  } 
}
#endif

/******************************************************************************/
static void RegisterDevForPrices(afAddrInfo_t *pAddrInfo)
{
  zbBindUnbindRequest_t  bindRequest;
  zbAddressMap_t addrMap;
   
  /* Bind our endpoint to the price client endpoint*/
  if (pAddrInfo->dstAddrMode == 0x02)
  {
    (void)AddrMap_SearchTableEntry(NULL, (zbNwkAddr_t*)&pAddrInfo->dstAddr, &addrMap);
    Copy8Bytes(bindRequest.destData.extendedMode.aDstAddress, addrMap.aIeeeAddr);
  }
  else if (pAddrInfo->dstAddrMode == 0x03)
  {
    Copy8Bytes(bindRequest.destData.extendedMode.aDstAddress, (uint8_t *)&pAddrInfo->dstAddr);
  }
  Set2Bytes(bindRequest.aClusterId, gZclClusterPrice_c);
  Copy8Bytes(bindRequest.aSrcAddress, NlmeGetRequest(gNwkIeeeAddress_c));
  bindRequest.srcEndPoint = pAddrInfo->dstEndPoint;
  bindRequest.addressMode = gZbAddrMode64Bit_c;
  bindRequest.destData.extendedMode.dstEndPoint = pAddrInfo->srcEndPoint;     
  
  APP_ZDP_BindUnbindRequest(NULL, NlmeGetRequest(gNwkShortAddress_c), gBind_req_c, &bindRequest);
}

#if gInterPanCommunicationEnabled_c
/******************************************************************************/
static void InterPanJitterTimerCallBack(zbTmrTimerID_t timerID)
{
  uint8_t status;

  (void)timerID;
  gaInterPanAddrTable[mInterPanIndex].addrInfo.dstAddrMode = gAddrModeShort_c; 
  BeeUtilSetToF(&gaInterPanAddrTable[mInterPanIndex].addrInfo.dstAddr, sizeof(zbNwkAddr_t)); ///broadcast address 
  status = ZCL_SendPublishPrice((addrInfoType_t *)&gaInterPanAddrTable[mInterPanIndex],
                                &gaServerPriceTable[mUpdatePriceIndex], TRUE);
  if(status == gZclSuccess_c)
    ++mInterPanIndex;
  SendPriceClusterEvt(gzclEvtHandlePublishPriceUpdate_c);
}
#endif /* #if gInterPanCommunicationEnabled_c */

/******************************************************************************/
/* Handle the Price Update */
static void ZCL_HandlePublishPriceUpdate(void)
{
  addrInfoType_t addrInfo;
  
  addrInfo.addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishPrice(&addrInfo, &gaServerPriceTable[mUpdatePriceIndex], FALSE);

#if gInterPanCommunicationEnabled_c  
{ 
  static zbTmrTimerID_t interPanTimerID = 0x00;
  uint16_t randomTime;
    
  /* Check the InterPan Price addr table for non-Se device and send the publish price */  
  if(mInterPanIndex < gNumOfInterPanAddr_c)
  {
    if(gaInterPanAddrTable[mInterPanIndex].entryStatus != 0x00)
    { 
      if(!interPanTimerID)
        interPanTimerID = ZbTMR_AllocateTimer();
      if(interPanTimerID)
      {  
        randomTime = (uint16_t)gInterPanMaxJitterTime_c/10;
        randomTime = (uint16_t)GetRandomRange(0, (uint8_t)randomTime) * 10;
        ZbTMR_StartSingleShotTimer (interPanTimerID, randomTime, InterPanJitterTimerCallBack);
      } 
    }
    
  }
}  
#endif /* #if gInterPanCommunicationEnabled_c */
}

/******************************************************************************/
/* Process the Get Current Price Req; reply with the active scheduled price */
static zbStatus_t ZCL_ProcessGetCurrPriceReq
(
addrInfoType_t *pAddrInfo,
zclCmdPrice_GetCurrPriceReq_t * pGetCurrPrice,
bool_t IsInterPanFlag)
{
  uint8_t i;
  uint32_t currentTime, startTime, stopTime;
  uint16_t duration;
  
  pGetCurrPrice->CmdOptions = pGetCurrPrice->CmdOptions & gGetCurrPrice_RequestorRxOnWhenIdle_c;
  
  if(pGetCurrPrice->CmdOptions)
  {
    if(IsInterPanFlag)
    {
#if gInterPanCommunicationEnabled_c       
      //Store InterPanAddr 
      StoreInterPanAddr((InterPanAddrInfo_t *)pAddrInfo);
#else
      return gZclFailure_c;
#endif /* #if gInterPanCommunicationEnabled_c  */
      
    }
    else
    {
      //set the device as being registered having the Rx on Idle(to send Price update) 
      RegisterDevForPrices((afAddrInfo_t *)pAddrInfo);
    }
  }
   /* here get the currentTime */
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  /* reply with the active scheduled price */
  for(i = 0; i < gNumofServerPrices_c; i++)
  {
    if(gaServerPriceTable[i].EntryStatus != gEntryNotUsed_c)
    {
      if (gaServerPriceTable[i].IsExtended)
      {
        startTime = OTA2Native32(gaServerPriceTable[i].Price.ExtendedPrice.StartTime);
        duration = OTA2Native16(gaServerPriceTable[i].Price.ExtendedPrice.DurationInMinutes);
      }
      else
      {
        startTime = OTA2Native32(gaServerPriceTable[i].Price.Price.StartTime);
        duration = OTA2Native16(gaServerPriceTable[i].Price.Price.DurationInMinutes);
      }
        
      stopTime = startTime + ((uint32_t)duration*60);
      if((startTime == 0x00000000) || (startTime == 0xFFFFFFFF) ||
         ((startTime <= currentTime) && (currentTime <= stopTime)))
	        return ZCL_SendPublishPrice(pAddrInfo, &gaServerPriceTable[i], IsInterPanFlag);	
    }
  }
  
  return gZclNotFound_c;  
}


/******************************************************************************/
#if gASL_ZclPrice_Optionals_d && gASL_ZclPrice_PublishPriceRsp_d
static void ZCL_HandleGetScheduledPrices(void)
{
  uint32_t  startTime;
  uint8_t status;
  
  if(mIndex < gNumofServerPrices_c)
  {
    /* Check if the entry is used and if there are more scheduled price to be send*/
    if((gaServerPriceTable[mIndex].EntryStatus != 0x00) && mGetNumOfPriceEvts)
    {
      startTime = OTA2Native32(gaServerPriceTable[mIndex].EffectiveStartTime);
      if(mGetPriceStartTime <= startTime)
      {
        /* Send This Pubish Price */
       	status = ZCL_SendPublishPrice(&mAddrPriceInfo, &gaServerPriceTable[mIndex], mIsInterPanFlag);
      	if(status == gZclSuccess_c)
      	{
          --mGetNumOfPriceEvts;
      	}
        
      }
      /* GO and send the next price */
      mIndex++;
      SendPriceClusterEvt(gzclEvtHandleGetScheduledPrices_c);
    }
    
  }
}
#endif
/******************************************************************************/
#if gASL_ZclPrice_Optionals_d 
static zbStatus_t ZCL_ProcessGetScheduledPricesReq
(
  addrInfoType_t *pAddrInfo, 
  zclCmdPrice_GetScheduledPricesReq_t * pGetScheduledPrice, 
  bool_t IsInterPanFlag
)
{
  FLib_MemCpy(&mAddrPriceInfo, pAddrInfo, sizeof(addrInfoType_t));

  mGetPriceStartTime = OTA2Native32(pGetScheduledPrice->StartTime);
  mGetNumOfPriceEvts = pGetScheduledPrice->NumOfEvts;
  if (!mGetNumOfPriceEvts)
  {
    mGetNumOfPriceEvts = gNumofServerPrices_c;
    mGetPriceStartTime = 0x00000000; /*all price information*/
  }
  mIsInterPanFlag = IsInterPanFlag;
  mIndex = 0;
  SendPriceClusterEvt(gzclEvtHandleGetScheduledPrices_c);
  return gZclSuccess_c;
} 
#endif

/******************************************************************************
* Get / Publish Block Period
******************************************************************************/

/******************************************************************************/
/* The Publish Block Period command is generated in response to receiving a Get Block Periods
command or when an update to the block period information is available
from the commodity provider. */
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_SendPublishBlockPeriod(addrInfoType_t *pAddrInfo, zclTSQ_t tsq, publishBlockPeriodEntry_t * pMsg) 
{
  zclPrice_PublishBlockPeriodRsp_t *pReq;    
  ZCLTime_t currentTime, elapsedTime;
  zbStatus_t status;
  
  pReq = AF_MsgAlloc();
  
  if (!pReq)
    return gZclNoMem_c;
  
  FLib_MemCpy(&pReq->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  pReq->zclTransactionId = tsq;
  
  FLib_MemCpy(&pReq->cmdFrame, &pMsg->blockPeriod, sizeof(zclCmdPrice_PublishBlockPeriodRsp_t));
  
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  
  if((pReq->cmdFrame.BlockPeriodStartTime == 0x00000000) && 
     (GetNative32BitIntFrom3ByteArray(pReq->cmdFrame.BlockPeriodDuration) != 0xFFFFFF))
  {
    // get the duration value
    elapsedTime = (currentTime - OTA2Native32(pMsg->EffectiveStartTime)) / 60;
    Zcl_SubtractDuration(pReq->cmdFrame.BlockPeriodDuration, elapsedTime);
  }
  
  status = zclPrice_PublishBlockPeriodRsp(pReq);
  
  MSG_Free(pReq);
  
  return status;
}
#endif

#if gASL_ZclPrice_Optionals_d
/******************************************************************************/
/* The Block Period timer callback that keep track of current active price */
static void TimerClientBlockPeriodCallBack(zbTmrTimerID_t timerID)
{
  (void) timerID;
  SendPriceClusterEvt(gzclEvtHandleClientBlockPeriod_c);
}

/******************************************************************************/
/* Process the received Publish Block Period */
static zbStatus_t ZCL_ProcessClientPublishBlockPeriod(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a BlockPeriod Update */
  updateIndex = CheckForBlockPeriodUpdate(pMsg, (publishBlockPeriodEntry_t *)&gaClientBlockPeriodsTable[0], gNumofClientBlockPeriods_c);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new Block Period information in the table */
    newEntry = AddBlockPeriodsInTable((publishBlockPeriodEntry_t *)&gaClientBlockPeriodsTable[0], gNumofClientBlockPeriods_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      /* the price was updated */
      gaClientBlockPeriodsTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      /* Call the App to signal that a Price was updated; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_BlockPeriodEvt_c, 0, 0, &gaClientBlockPeriodsTable[updateIndex]);
      status = gZclSuccess_c;
    }
  }
  
  if(status == gZclSuccess_c)
    SendPriceClusterEvt(gzclEvtHandleClientBlockPeriod_c);
  
  return status;
}
#endif
/******************************************************************************/
/* Handle the Client Block Period signalling when the current price starts, was updated or is completed */
#if gASL_ZclPrice_Optionals_d
static void ZCL_HandleClientBlockPeriod(void)
{
  uint32_t currentTime, startTime, nextTime=0x00000000, stopTime;
  publishBlockPeriodEntry_t *pEntry = &gaClientBlockPeriodsTable[0];
  
  
  /* the Block Period table is kept in ascendent order; check if any Block Period is scheduled*/
  if(pEntry->EntryStatus == 0x00)
  {
    ZbTMR_StopTimer(gBlockPeriodClientTimerID);
    return;
  }
  
  /* Get the timing */
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  startTime = OTA2Native32(pEntry->EffectiveStartTime);
  stopTime = startTime + (60 * Zcl_GetDuration(pEntry->blockPeriod.BlockPeriodDuration));
  /* Check if the Price Event is completed */
  if(stopTime <= currentTime)
  {
    pEntry->EntryStatus = gBlockPeriodCompletedStatus_c; /* entry is not used anymore */
    /* Call the App to signal that a Price was completed; User should check EntryStatus */
    BeeAppUpdateDevice (0, gZclUI_BlockPeriodEvt_c, 0, 0, pEntry);
    pEntry->EntryStatus = 0x00;
    FLib_MemInPlaceCpy(pEntry, (pEntry+1), (gNumofClientBlockPeriods_c-1) * sizeof(publishBlockPeriodEntry_t));
    gaClientBlockPeriodsTable[gNumofClientBlockPeriods_c-1].EntryStatus = 0x00;
  }
  else
  {
    if(startTime <= currentTime) /* check if the Price event have to be started or updated */
    {
      if((pEntry->EntryStatus == gBlockPeriodReceivedStatus_c)||
         (pEntry->EntryStatus == gBlockPeriodUpdateStatus_c))
      {
        pEntry->EntryStatus = gBlockPeriodStartedStatus_c;
        /* Call the App to signal that a BlockPeriod was started; User should check EntryStatus */
        BeeAppUpdateDevice (0, gZclUI_BlockPeriodEvt_c, 0, 0, pEntry);
      }
    }
  }
  if(currentTime < startTime)
    nextTime = startTime - currentTime;
  else
    if(currentTime < stopTime)
      nextTime = stopTime - currentTime;
  if (nextTime)
    ZbTMR_StartSecondTimer(gBlockPeriodClientTimerID,(uint16_t)nextTime, TimerClientBlockPeriodCallBack);
  
}
#endif

#if gASL_ZclPrice_Optionals_d 
/* Find an Event in the Events Table...  */
static uint8_t FindNextBlockPeriodEvent(void)
{
  uint8_t i; 
  ZCLTime_t eventStopTime;
  
  for(i = mGetBlockPeriod.index; i < gNumofServerBlockPeriods_c; i++)
  {
    publishBlockPeriodEntry_t *pEntry = &gaServerBlockPeriodsTable[i];
    
    // if exits this entry
    if(pEntry->EntryStatus != 0x00)  
    {      
      //get the eventStopTime
      eventStopTime = OTA2Native32(pEntry->EffectiveStartTime) + 
        60 * Zcl_GetDuration(pEntry->blockPeriod.BlockPeriodDuration);
      
      if((mGetBlockPeriod.startTime <= eventStopTime) && (mGetBlockPeriod.numOfEvtsRemaining > 0) 
#if gASL_ZclSE_12_Features_d         
         && ((mGetBlockPeriod.tariffType == pEntry->blockPeriod.TariffType))
#endif        
        )
      {
        return i;
      }
    }
  }
  return 0xFF; 
}   
#endif

/* Process the Get Block Periods Req; reply with the Publish Block Period for all
currently scheduled periods*/
#if gASL_ZclPrice_Optionals_d 
static zbStatus_t ZCL_ProcessGetBlockPeriodsReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t      tsq,
  zclCmdPrice_GetBlockPeriodsReq_t * pGetBlockPeriods
)
{
  uint8_t entryIndex;
    
  /* Copy address information and TSQ*/
  FLib_MemCpy(&mGetBlockPeriod.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
  mGetBlockPeriod.tsq = tsq;
  
  mGetBlockPeriod.index = 0;
  
  /* Get the start time */ 
  if(pGetBlockPeriods->StartTime == 0x00000000)
    mGetBlockPeriod.startTime = ZCL_GetUTCTime(pAddrInfo->srcEndPoint);
  else
    mGetBlockPeriod.startTime = OTA2Native32(pGetBlockPeriods->StartTime);

  /* Get number of events*/  
  if(pGetBlockPeriods->NumOfEvts == 0)
    mGetBlockPeriod.numOfEvtsRemaining = gNumofServerBlockPeriods_c;
  else
    mGetBlockPeriod.numOfEvtsRemaining = pGetBlockPeriods->NumOfEvts;
  
  /* Default is a delivered tariff*/
  if (pGetBlockPeriods->TariffType > 2)
    mGetBlockPeriod.tariffType = 0;
  else
    mGetBlockPeriod.tariffType = pGetBlockPeriods->TariffType;
  
  /* Find next valid entry */
  entryIndex = FindNextBlockPeriodEvent();
  if(entryIndex != 0xFF)
  {
    mGetBlockPeriod.index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetBlockPeriods_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif
/******************************************************************************/
/* Handle the GetBlockPeriods command*/
#if gASL_ZclPrice_Optionals_d && gASL_ZclPrice_PublishBlockPeriodsRsp_d
static void ZCL_HandleGetBlockPeriods(void)
{
  uint8_t status;
  uint8_t entryIndex;
     
  status= ZCL_SendPublishBlockPeriod(&mGetBlockPeriod.addrInfo, mGetBlockPeriod.tsq
       ,&gaServerBlockPeriodsTable[mGetBlockPeriod.index]);
  
  if(status == gZclSuccess_c)
  {
    mGetBlockPeriod.numOfEvtsRemaining = mGetBlockPeriod.numOfEvtsRemaining - 1;
    mGetBlockPeriod.index = mGetBlockPeriod.index + 1;
    if (mGetBlockPeriod.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      entryIndex = FindNextBlockPeriodEvent();
      if(entryIndex != 0xFF)
      {
        mGetBlockPeriod.index = entryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetBlockPeriods_c);
      }
    }
  }
}
#endif

/******************************************************************************/
#if gASL_ZclPrice_Optionals_d
/* Add Block Periods in Table... so that to have the scheduled price in asccendent order */
static uint8_t AddBlockPeriodsInTable(publishBlockPeriodEntry_t *pTable, uint8_t len, zclCmdPrice_PublishBlockPeriodRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime;
  uint8_t i, poz = 0xff;
  
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->BlockPeriodStartTime);
  if(msgStartTime == 0x000000)
  {
    msgStartTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  }
  
  /* keep the Block Periods in asccendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->blockPeriod.BlockPeriodStartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishBlockPeriodEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishBlockPeriodRsp_t));
  
  if(pMsg->BlockPeriodStartTime == 0x00000000)
  {
    /*  here get the currentTime  */
    (pTable+poz)->EffectiveStartTime = Native2OTA32(ZCL_GetUTCTime(BeeAppDataInit(appEndPoint)));
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->BlockPeriodStartTime;
  /* new price was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Price was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_BlockPeriodEvt_c, 0, 0, (pTable+poz));
  return poz;
}
#endif
/******************************************************************************/
/*Check and Updated a Block Period.
When new block period information is provided that replaces older block period
information for the same time period, IssuerEvt field allows devices to determine which
information is newer. It is expected that the value contained in this field is a
unique number managed by upstream servers.
Thus, newer pricing information will have a value in the Issuer Event ID field that
is larger than older pricing information.
*/
#if gASL_ZclPrice_Optionals_d
static uint8_t CheckForBlockPeriodUpdate(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg, publishBlockPeriodEntry_t *pTable, uint8_t len)
{
  uint8_t i;
  uint32_t msgIssuerEvt, entryIssuerEvt;
  
  msgIssuerEvt = FourBytesToUint32(pMsg->IssuerEvtID);
  msgIssuerEvt = OTA2Native32(msgIssuerEvt);
  
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      entryIssuerEvt = FourBytesToUint32(&(pTable+i)->blockPeriod.IssuerEvtID);
      entryIssuerEvt = OTA2Native32(entryIssuerEvt);
      if ((pTable+i)->blockPeriod.BlockPeriodStartTime == pMsg->BlockPeriodStartTime &&
          FLib_MemCmp(&(pTable+i)->blockPeriod.BlockPeriodDuration, &pMsg->BlockPeriodDuration, sizeof(Duration24_t)))
      {
        if(entryIssuerEvt < msgIssuerEvt)
        {
          FLib_MemCpy((pTable+i), pMsg, sizeof(zclCmdPrice_PublishBlockPeriodRsp_t));
          return i;
        }
        else 
          return 0xFE; /* reject it */
      }
    }
  }
  return 0xff;
}
#endif

#if gASL_ZclPrice_Optionals_d
static void ZCL_HandleBlockPeriodUpdate(index_t entryIdx)
{
  addrInfoType_t addrInfo;
  
  addrInfo.addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishBlockPeriod(&addrInfo, ZbZclFoundationGlobals(gZclTransactionId), &gaServerBlockPeriodsTable[entryIdx]);
  return;
}
#endif

/******************************************************************************
* Get / Publish Conversion Factor
******************************************************************************/

#if gASL_ZclPrice_Optionals_d
/******************************************************************************/
/* Process the received Publish Conversion Factor */
static zbStatus_t ZCL_ProcessClientPublishConversionFactor(zclCmdPrice_PublishConversionFactorRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a Conversion Factor Update */
  for(updateIndex=0;updateIndex<gNumofClientConversionFactors_c;updateIndex++)
  {
    if(gaClientConversionFactorTable[updateIndex].ConversionFactor.StartTime == 
       pMsg->StartTime)
    {
      if(gaClientConversionFactorTable[updateIndex].ConversionFactor.IssuerEvt >= 
         pMsg->IssuerEvt)
      {
        updateIndex = 0xFE;// reject it;
      }
      else
      {
        FLib_MemCpy(&gaClientConversionFactorTable[updateIndex], pMsg, sizeof(zclCmdPrice_PublishConversionFactorRsp_t));
      }
      break;
    }
  }
  /*if the Publish Conversion Factor is not rejected, add it in the table */
  if(updateIndex == (gNumofClientConversionFactors_c-1))
  {
    if(updateIndex != 0xfe)
    {
      /* the Conversion Factor was updated */
      gaClientConversionFactorTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      status = gZclSuccess_c;
    }
    else
    {
    /* Add the new Conversion Factor information in the table */
    newEntry = AddConversionFactorInTable((publishConversionFactorEntry_t *)&gaClientConversionFactorTable[0], gNumofClientConversionFactors_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
    }
  }
  
  //if(status == gZclSuccess_c)
    //TS_SendEvent(gZclTaskId, gzclEvtHandleClientPrices_c);
  
  return status;
}
#endif


/******************************************************************************/
/* Process the Get Conversion Factor Req; reply with the Publish Convertor
Factor for all currently scheduled periods*/
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_ProcessGetConversionFactorReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_GetConversionFactorReq_t * pGetConversionFactor
)
{
  uint8_t entryIndex;
    
  /* Copy address information and TSQ*/
  FLib_MemCpy(&mGetConversionFactor.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
  mGetConversionFactor.tsq = tsq;
  
  mGetConversionFactor.index = 0;
  
  /* Get the start time */ 
  if(pGetConversionFactor->StartTime == 0x00000000)
    mGetConversionFactor.startTime = ZCL_GetUTCTime(pAddrInfo->srcEndPoint);
  else
    mGetConversionFactor.startTime = OTA2Native32(pGetConversionFactor->StartTime);

  /* Get number of events*/  
  if(pGetConversionFactor->NumOfEvts == 0)
    mGetConversionFactor.numOfEvtsRemaining = gNumofServerConversionFactors_c;
  else
    mGetConversionFactor.numOfEvtsRemaining = pGetConversionFactor->NumOfEvts;
  
  /* Find next valid entry */
  entryIndex = FindNextConversionFactorEvent();
  if(entryIndex != 0xFF)
  {
    mGetConversionFactor.index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetConversionFactor_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

#if gASL_ZclPrice_Optionals_d 
/* Find an Event in the Events Table...  */
static uint8_t FindNextConversionFactorEvent(void)
{
  uint8_t i; 
  
  for(i = mGetConversionFactor.index; i < gNumofServerConversionFactors_c; i++)
  {
    publishConversionFactorEntry_t *pEntry = &gaServerConversionFactorTable[i];
    
    /* Check if the entry exists */
    if(pEntry->EntryStatus != 0x00)  
    {      
      if(mGetConversionFactor.numOfEvtsRemaining > 0)
      {
       /* Check for a future event */
       if (mGetConversionFactor.startTime <= pEntry->ConversionFactor.StartTime)
       {
        return i;
       }
       else
       {
         if (i < gNumofServerConversionFactors_c - 1) 
         {
           /* Check for the current event in that timeframe */
           publishConversionFactorEntry_t *pNextEntry = &gaServerConversionFactorTable[i+1];
           if ((pNextEntry->EntryStatus != 0x00)  && (mGetConversionFactor.startTime < pNextEntry->ConversionFactor.StartTime))
             return i;
         }
         else
           return i;
       }
      }
    }
  }
  return 0xFF; 
}   
#endif

/******************************************************************************/
/* The Publish Conversion Factor command is generated in response to receiving a Get Conversion Factor
command or if a new conversion factor is available. */
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_SendPublishConversionFactor(addrInfoType_t *pAddrInfo, zclTSQ_t tsq, publishConversionFactorEntry_t * pMsg) 
{
  zclPrice_PublishConversionFactorRsp_t* pRsp;  
  zbStatus_t status;
  
  pRsp = AF_MsgAlloc();
  
  if (!pRsp)
    return gZclNoMem_c;
  
  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  pRsp->zclTransactionId = tsq;
  FLib_MemCpy(&pRsp->cmdFrame, pMsg, sizeof(zclCmdPrice_PublishConversionFactorRsp_t));

  status = zclPrice_PublishConversionFactorRsp(pRsp);
  
  MSG_Free(pRsp);
  
  return status;
}
#endif
/******************************************************************************/
/* Handle the GetConversionFactor command*/
#if gASL_ZclPrice_Optionals_d && gASL_ZclPrice_PublishConversionFactorRsp_d
static void ZCL_HandleGetConversionFactor(void)
{
  uint8_t status;
  uint8_t iEntryIndex;
     
  status= ZCL_SendPublishConversionFactor(&mGetConversionFactor.addrInfo, mGetConversionFactor.tsq
       ,&gaServerConversionFactorTable[mGetConversionFactor.index]);
  
  if(status == gZclSuccess_c)
  {
    mGetConversionFactor.numOfEvtsRemaining = mGetConversionFactor.numOfEvtsRemaining - 1;
    mGetConversionFactor.index = mGetConversionFactor.index + 1;
    if (mGetConversionFactor.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      iEntryIndex = FindNextConversionFactorEvent();
      if(iEntryIndex != 0xFF)
      {
        mGetConversionFactor.index = iEntryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetConversionFactor_c);
      }
    }
  }
}
#endif

#if gASL_ZclPrice_Optionals_d  
static void ZCL_HandleConversionFactorUpdate(index_t iEntryIdx)
{
  addrInfoType_t addrInfo;
  
  addrInfo.addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishConversionFactor(&addrInfo, ZbZclFoundationGlobals(gZclTransactionId)++, &gaServerConversionFactorTable[iEntryIdx]);
  return;
}
#endif

#if gASL_ZclPrice_Optionals_d
/* Add Conversion Factor in Table... so that to have the Conversion Factor in asccendent order */
static uint8_t AddConversionFactorInTable(publishConversionFactorEntry_t *pTable, uint8_t len, zclCmdPrice_PublishConversionFactorRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->StartTime);
  
  /* keep the Conversion Factor in asccendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->ConversionFactor.StartTime);
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishConversionFactorEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishConversionFactorRsp_t));
  
  if((pMsg->StartTime == 0x00000000)||
     (pMsg->StartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->StartTime ;
  /* new price was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Price was received; User should check EntryStatus */
  // BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, (pTable+poz));
  return poz;
}
#endif

/******************************************************************************
* Get / Publish Calorific Value
******************************************************************************/
#if gASL_ZclPrice_Optionals_d
/******************************************************************************/
/* Process the received Publish Calorific Value */
static zbStatus_t ZCL_ProcessClientPublishCalorificValue(zclCmdPrice_PublishCalorificValueRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a Calorific Value Update */
  for(updateIndex=0; updateIndex<gNumofClientCalorificValue_c; updateIndex++)
  {
    if(gaClientCalorificValueTable[updateIndex].CalorificValue.StartTime == 
       pMsg->StartTime)
    {
       if(gaClientCalorificValueTable[updateIndex].CalorificValue.IssuerEvtID >= 
         pMsg->IssuerEvtID)
      {
        updateIndex = 0xFE;// reject it;
      }
      else
      {
        FLib_MemCpy(&gaClientCalorificValueTable[updateIndex], pMsg, sizeof(zclCmdPrice_PublishCalorificValueRsp_t));
      }
      break;
    }
  }
  /*if the Publish Calorific Value is not rejected, add it in the table */
  if(updateIndex == (gNumofClientCalorificValue_c-1))
  {
    if(updateIndex != 0xfe)
    {
      /* the Calorific Value was updated */
      gaClientCalorificValueTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      status = gZclSuccess_c;
    }
    else
    {
    /* Add the new Calorific Value information in the table */
    newEntry = AddCalorificValueInTable((publishCalorificValueEntry_t *)&gaClientCalorificValueTable[0], gNumofClientCalorificValue_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
    }
  }
  
  //if(status == gZclSuccess_c)
    //TS_SendEvent(gSETaskID, gzclEvtHandleClientPrices_c);
  
  return status;
}
#endif
/******************************************************************************/
/* Process the Get Calorific Value Req; reply with the Publish Calorific
value for all currently scheduled periods*/
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_ProcessGetCalorificValueReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_GetCalorificValueReq_t * pGetCalorificValue
)
{
  uint8_t entryIndex;
    
  /* Copy address information and TSQ*/
  FLib_MemCpy(&mGetCalorificValue.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
  mGetCalorificValue.tsq = tsq;
  
  mGetCalorificValue.index = 0;
  
  /* Get the start time */ 
  if(pGetCalorificValue->StartTime == 0x00000000)
    mGetCalorificValue.startTime = ZCL_GetUTCTime(pAddrInfo->srcEndPoint);
  else
    mGetCalorificValue.startTime = OTA2Native32(pGetCalorificValue->StartTime);

  /* Get number of events*/  
  if(pGetCalorificValue->NumOfEvts == 0)
    mGetCalorificValue.numOfEvtsRemaining = gNumofServerCalorificValue_c;
  else
    mGetCalorificValue.numOfEvtsRemaining = pGetCalorificValue->NumOfEvts;
  
  /* Find next valid entry */
  entryIndex = FindNextCalorificValueEvent();
  if(entryIndex != 0xFF)
  {
    mGetCalorificValue.index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetCalorificValue_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}

/******************************************************************************/
/* The Publish Calorific Value command is generated in response to receiving a Get 
Calorific Value of if a new Calorific value is available.*/
static zbStatus_t ZCL_SendPublishCalorificValue(addrInfoType_t *pAddrInfo, zclTSQ_t tsq, publishCalorificValueEntry_t * pMsg) 
{
  zclPrice_PublishCalorificValueRsp_t* pRsp;
  zbStatus_t status;
  
  pRsp = AF_MsgAlloc();
  
  if (!pRsp)
    return gZclNoMem_c;
  
  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  pRsp->zclTransactionId = tsq;
  FLib_MemCpy(&pRsp->cmdFrame, pMsg, sizeof(zclCmdPrice_PublishCalorificValueRsp_t));
  
  status = zclPrice_PublishCalorificValueRsp(pRsp);
  
  MSG_Free(pRsp);
  
  return status;
}
#endif

#if gASL_ZclPrice_Optionals_d 
/* Find an Event in the Events Table...  */
static uint8_t FindNextCalorificValueEvent(void)
{
  uint8_t i; 
  
  for(i = mGetCalorificValue.index; i < gNumofServerCalorificValue_c; i++)
  {
    publishCalorificValueEntry_t *pEntry = &gaServerCalorificValueTable[i];
    
    /* Check if the entry exists */
    if(pEntry->EntryStatus != 0x00)  
    {      
      if(mGetCalorificValue.numOfEvtsRemaining > 0)
      {
       /* Check for a future event */
       if (mGetCalorificValue.startTime <= pEntry->CalorificValue.StartTime)
       {
        return i;
       }
       else
       {
         if (i < gNumofServerCalorificValue_c - 1) 
         {
           /* Check for the current event in that timeframe */
           publishCalorificValueEntry_t *pNextEntry = &gaServerCalorificValueTable[i+1];
           if ((pNextEntry->EntryStatus != 0x00)  && (mGetCalorificValue.startTime < pNextEntry->CalorificValue.StartTime))
             return i;
         }
         else
           return i;
       }
      }
    }
  }
  return 0xFF; 
}   
#endif

/******************************************************************************/
/* Handle the GetCalorificValue command*/
#if gASL_ZclPrice_Optionals_d && gASL_ZclPrice_PublishCalorificValueRsp_d
static void ZCL_HandleGetCalorificValue(void)
{
  uint8_t status;
  uint8_t iEntryIndex;
     
  status= ZCL_SendPublishCalorificValue(&mGetCalorificValue.addrInfo, mGetCalorificValue.tsq
       ,&gaServerCalorificValueTable[mGetCalorificValue.index]);
  
  if(status == gZclSuccess_c)
  {
    mGetCalorificValue.numOfEvtsRemaining = mGetCalorificValue.numOfEvtsRemaining - 1;
    mGetCalorificValue.index = mGetCalorificValue.index + 1;
    if (mGetCalorificValue.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      iEntryIndex = FindNextCalorificValueEvent();
      if(iEntryIndex != 0xFF)
      {
        mGetCalorificValue.index = iEntryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetCalorificValue_c);
      }
    }
  }
}

#endif

#if gASL_ZclPrice_Optionals_d 
static void ZCL_HandleCalorificValueUpdate(index_t iEntryIdx)
{
  addrInfoType_t addrInfo;
  
  addrInfo.addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishCalorificValue(&addrInfo, ZbZclFoundationGlobals(gZclTransactionId)++, &gaServerCalorificValueTable[iEntryIdx]);
  return;
}
#endif

#if gASL_ZclPrice_Optionals_d
/* Add Clorific Value in Table... so that to have the calorific values are in asccendent order */
static uint8_t AddCalorificValueInTable(publishCalorificValueEntry_t *pTable, uint8_t len, zclCmdPrice_PublishCalorificValueRsp_t *pMsg)
{
  ZCLTime_t startTime, msgStartTime;
  uint8_t i, poz = 0xff;
  
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->StartTime);
  if((msgStartTime == 0x00000000))
    msgStartTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  
  /* keep the calorific values in asccendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->CalorificValue.StartTime);
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishCalorificValueEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishCalorificValueRsp_t));
  
  if((pMsg->StartTime == 0x00000000)||
     (pMsg->StartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    (pTable+poz)->EffectiveStartTime = Native2OTA32(ZCL_GetUTCTime(BeeAppDataInit(appEndPoint)));
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->StartTime ;
  /* new price was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Price was received; User should check EntryStatus */
  //BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, (pTable+poz));
  return poz;
}
#endif

/******************************************************************************
* Get / Publish Block Thresholds
******************************************************************************/

/******************************************************************************/
/* The Publish Block Threshold command is generated in response to receiving a Get Block Threshold
command.*/
#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_PublishBlockThresholdsRsp_d
static zbStatus_t ZCL_SendPublishBlockThresholds(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishBlockThresholdsEntry_t * pMsg) 
{
  
  zclPrice_PublishBlockThresholdsRsp_t *pReq;   
  uint8_t noBlockThresholdsInUse, noPriceTiersInUse, noSubPayloadsPerFrame, tariffEntryIdx, asduLen;
  TierBlockThreshold_t *pSrc;
  zbStatus_t status;
  
  tariffEntryIdx = FindServerTariffInformationEntryByTariffId(&pMsg->IssuerTariffID);
  
  if (tariffEntryIdx == gInvalidTableIndex_c)
    return gZclNotFound_c;
  
  noBlockThresholdsInUse = gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.NumberBlockThreshholdsUse;
  if (noBlockThresholdsInUse == 0)
    return gZclNotFound_c;
  
  noPriceTiersInUse = gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.NumberPriceTiersUse;
  if ((pMsg->SubPayloadCtl == 1) || (noPriceTiersInUse == 0))
    /* Block Thresholds supplied apply to all TOU tiers || Apply for no tier*/
    noPriceTiersInUse = 1;
  
  /* Find how much threshold subpayloads can fit into the packet */
  noSubPayloadsPerFrame = (uint8_t)(GetMaxOutTransferSize(pAddrInfo->dstAddr.aNwkAddr) - sizeof(zclFrame_t) - 4 * sizeof(SEEvtId_t) - 3*sizeof(uint8_t)) / (sizeof(uint8_t) + noBlockThresholdsInUse);
  
  if (((mGetBlockThresholds.CmdIndex + 1) * noSubPayloadsPerFrame) < noPriceTiersInUse)
    mGetBlockThresholds.Length = noSubPayloadsPerFrame;
  else  
    mGetBlockThresholds.Length = noPriceTiersInUse - (mGetBlockThresholds.CmdIndex * noSubPayloadsPerFrame);
  
  /* Allocate buffer for OTA frame*/
  pReq = AF_MsgAlloc();
  if(!pReq)
    return gZbNoMem_c;
  
  FLib_MemCpy(&pReq->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  /* Check if fragmentation is required */
  asduLen = sizeof(zclFrame_t) + sizeof(zclCmdPrice_PublishBlockThresholdsRsp_t) - sizeof(TierBlockThreshold_t) 
    + mGetBlockThresholds.Length * (sizeof(TierBlockThreshold_t) + (noBlockThresholdsInUse - 1) * sizeof(BlockThreshold_t));
  if(asduLen > AF_MaxPayloadLen(pAddrInfo))
    pReq->addrInfo.txOptions |= gApsTxOptionFragmentationRequested_c;
  
  pReq->zclTransactionId = tsq;
  
  /* Add application payload*/
  FLib_MemCpy(pReq->cmdFrame.IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  FLib_MemCpy(pReq->cmdFrame.IssuerTariffID, pMsg->IssuerTariffID, sizeof(SEEvtId_t));
  FLib_MemCpy(pReq->cmdFrame.ProviderID, pMsg->ProviderID, sizeof(SEEvtId_t));
  pReq->cmdFrame.StartTime = pMsg->StartTime;
  pReq->cmdFrame.SubPayloadCtl = pMsg->SubPayloadCtl;
  pReq->cmdFrame.CmdIdx = mGetBlockThresholds.CmdIndex;
  pReq->cmdFrame.NumCmds = noPriceTiersInUse / noSubPayloadsPerFrame + 1 ;
  
  pSrc = (TierBlockThreshold_t *)((uint8_t*)&pMsg->BlockThresholds[0] + mGetBlockThresholds.CmdIndex * (noBlockThresholdsInUse * sizeof(BlockThreshold_t) + sizeof(uint8_t)));
  FLib_MemCpy(pReq->cmdFrame.BlockThresholds, pSrc, mGetBlockThresholds.Length * (noBlockThresholdsInUse * sizeof(BlockThreshold_t) + sizeof(uint8_t)));
 
  /* Send request OTA*/
  status = zclPrice_PublishBlockThresholdsRsp(pReq, mGetBlockThresholds.Length, noBlockThresholdsInUse);
  MSG_Free(pReq);
  return status;
 
}
#endif


#if gASL_ZclSE_12_Features_d 
static zbStatus_t ZCL_ProcessGetBlockThresholdsReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_GetBlockThresholdsReq_t * pGetBlockThresholds
)
{
  uint8_t iEntryIdx = gInvalidTableIndex_c;
  
  
  /* Check if there is another publish event in progress */
  if(mGetBlockThresholds.Index != gInvalidTableIndex_c)
  {
    return gZclNoMem_c;
  }
  
  /* Find entry in BlockThreshold table*/  
  for (iEntryIdx = 0; iEntryIdx < gNumofServerBlockThresholds_c; iEntryIdx++)
  {
    if (FLib_MemCmp(gaServerBlockThresholdsTable[iEntryIdx].IssuerTariffID, pGetBlockThresholds->IssuerTariffID, sizeof(SEEvtId_t)))
      break;
  }
  
  if(iEntryIdx != gInvalidTableIndex_c)
  {
    mGetBlockThresholds.Index = iEntryIdx;
    /* This is the first command*/
    mGetBlockThresholds.CmdIndex = 0x00;
    /* Copy the address and event id*/
    FLib_MemCpy(&mGetBlockThresholds.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
    mGetBlockThresholds.tsq = tsq;
    /* Start at index 0 */
    mGetBlockThresholds.Offset = 0;
      
    SendPriceClusterEvt(gzclEvtHandleGetBlockThresholds_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_PublishBlockThresholdsRsp_d
static void ZCL_HandleGetBlockThresholds(void)
{
  zbStatus_t status;
     
  status = ZCL_SendPublishBlockThresholds(&mGetBlockThresholds.addrInfo, mGetBlockThresholds.tsq
       ,&gaServerBlockThresholdsTable[mGetBlockThresholds.Index]);
  
  if(status == gZclSuccess_c)
  {
    mGetBlockThresholds.CmdIndex = mGetBlockThresholds.CmdIndex + 1;
    mGetBlockThresholds.Offset = mGetBlockThresholds.Offset + mGetBlockThresholds.Length;
    if (mGetBlockThresholds.Offset < gaServerBlockThresholdsTable[mGetBlockThresholds.Index].Length)
      SendPriceClusterEvt(gzclEvtHandleGetBlockThresholds_c);
    else
      mGetBlockThresholds.Index = gInvalidTableIndex_c;
  }
  else
  {
    mGetBlockThresholds.Index = gInvalidTableIndex_c;
  }
}
#endif

#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Process the received Publish Block Thresholds */
static zbStatus_t ZCL_ProcessClientPublishBlockThresholds(zclCmdPrice_PublishBlockThresholdsRsp_t *pMsg, uint8_t payloadLen)
{
  uint8_t entryIdx = 0xFF;
  uint8_t i, expectedNoBlockThresholds, noBlockThresholdsInUse, noBlockThresholdsPerFrame, tariffEntryIdx; 
  publishBlockThresholdsEntry_t *pEntry;
  
   /* Search for an existing entry*/
  for(i = 0; i < gNumofClientBlockThresholds_c; i++)
  {    
    if(FLib_MemCmp(gaClientBlockThresholdsTable[i].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t)))
    {
      entryIdx = i;
      break;
    }
  }
  
  /*If it is a new entry find an empty slot for it*/
  if (entryIdx == 0xFF)
    for(i = 0; i < gNumofClientBlockThresholds_c; i++)
    {    
      if(gaClientBlockThresholdsTable[i].EntryStatus == gEntryNotUsed_c)
      {
        entryIdx = i;
        break;
      }
    }
  
  /* Cannot store infromation*/  
  if(entryIdx == 0xff)
    return gZclFailure_c;
  
  pEntry = &gaClientBlockThresholdsTable[entryIdx];
  
  tariffEntryIdx = FindClientTariffInformationEntry(pMsg->ProviderID, pMsg->IssuerEvtID);
  
  if (tariffEntryIdx == gInvalidTableIndex_c)
    return gZclFailure_c;
  
  noBlockThresholdsInUse = gaClientTariffInformationTable[tariffEntryIdx].TariffInformation.NumberBlockThreshholdsUse;
  noBlockThresholdsPerFrame = (uint8_t)((payloadLen - sizeof(SEEvtId_t) - 1) / sizeof(BlockThreshold_t));
  
  if (((pMsg->CmdIdx + 1) * noBlockThresholdsPerFrame) < noBlockThresholdsInUse)
    expectedNoBlockThresholds = noBlockThresholdsPerFrame;
  else  
    expectedNoBlockThresholds = noBlockThresholdsInUse - (pMsg->CmdIdx * noBlockThresholdsPerFrame);  
  
  /* Save the information in the client Block Threshold Table*/
  pEntry->EntryStatus = gEntryUsed_c;
  
  FLib_MemCpy(pEntry->IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  FLib_MemCpy(pEntry->ProviderID, pMsg->ProviderID, sizeof(SEEvtId_t));
  FLib_MemCpy(pEntry->IssuerTariffID, pMsg->IssuerTariffID, sizeof(SEEvtId_t));
  pEntry->StartTime = pMsg->StartTime;
  pEntry->SubPayloadCtl = pMsg->SubPayloadCtl;
  pEntry->Length = (pMsg->CmdIdx * noBlockThresholdsPerFrame) + noBlockThresholdsPerFrame;
  pEntry->EntryStatus = gEntryUsed_c;
  FLib_MemCpy(&pEntry->BlockThresholds[pMsg->CmdIdx * noBlockThresholdsPerFrame], &pMsg->BlockThresholds[0], expectedNoBlockThresholds);
  
  return gZclSuccess_c;
}
#endif

/******************************************************************************
* Get / Publish Price Matrix
******************************************************************************/

/******************************************************************************/
/* The Publish Price Matrix command is generated in response to receiving a Get Price matrix
command.*/
#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_PublishPriceMatrixRsp_d
static zbStatus_t ZCL_SendPublishPriceMatrix(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishPriceMatrixEntry_t * pMsg) 
{
  
  zclPrice_PublishPriceMatrixRsp_t *pReq;   
  uint8_t noPriceTiersInUse, noPriceBlockTholdsInUse, noPricesPerFrame, tariffEntryIdx, noPrices;
  zbStatus_t status;
  
   /* Find how much prices can fit into the packet */
  tariffEntryIdx = FindServerTariffInformationEntryByTariffId(&pMsg->IssuerTariffID);
  
  if (tariffEntryIdx == gInvalidTableIndex_c)
    return gZclNotFound_c;
    
  noPriceTiersInUse = gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.NumberPriceTiersUse;
  noPriceBlockTholdsInUse = gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.NumberBlockThreshholdsUse;
  noPricesPerFrame = (uint8_t)(GetMaxOutTransferSize(pAddrInfo->dstAddr.aNwkAddr) - sizeof(zclFrame_t) - 4*sizeof(SEEvtId_t) - 3*sizeof(uint8_t)) / sizeof(TierBlockPrice_t);
  
  /* TOU Tariff */
  if (gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.TariffType.scheme == 0x00)      
    noPrices = noPriceTiersInUse;
  /* Block Tariff */
  if (gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.TariffType.scheme == 0x01)      
    noPrices = noPriceBlockTholdsInUse + 1;
  /* Block/TOU Tariff */
  if (gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.TariffType.scheme > 0x01)     
    noPrices = (noPriceBlockTholdsInUse + 1) * noPriceTiersInUse;
  
  if (((mGetPriceMatrix.CmdIndex + 1) * noPricesPerFrame) < noPrices)
    mGetPriceMatrix.Length = noPricesPerFrame;
  else  
    mGetPriceMatrix.Length = noPrices - (mGetPriceMatrix.CmdIndex * noPricesPerFrame);
  
  /* Allocate buffer for OTA frame*/
  pReq = AF_MsgAlloc();
  if(!pReq)
    return gZbNoMem_c;
  
  FLib_MemCpy(&pReq->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  
  if(sizeof(zclCmdPrice_PublishPriceMatrixRsp_t) +  (mGetPriceMatrix.Length - 1) * sizeof(TierBlockPrice_t) > AF_MaxPayloadLen(pAddrInfo) - sizeof(zclFrame_t))
        pReq->addrInfo.txOptions |= gApsTxOptionFragmentationRequested_c;
  
  pReq->zclTransactionId = tsq;
  
  /* Add application payload*/
  FLib_MemCpy(pReq->cmdFrame.IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  FLib_MemCpy(pReq->cmdFrame.IssuerTariffID, pMsg->IssuerTariffID, sizeof(SEEvtId_t));
  FLib_MemCpy(pReq->cmdFrame.ProviderID, pMsg->ProviderID, sizeof(SEEvtId_t));
  pReq->cmdFrame.StartTime = pMsg->StartTime;
  pReq->cmdFrame.SubPayloadCtl = pMsg->SubPayloadCtl;
  pReq->cmdFrame.CmdIdx = mGetPriceMatrix.CmdIndex;
  pReq->cmdFrame.NoCmds = noPrices/noPricesPerFrame + 1;
  FLib_MemCpy(pReq->cmdFrame.TierBlockPrice, &pMsg->TierBlockPrice[mGetPriceMatrix.Offset], mGetPriceMatrix.Length * sizeof(TierBlockPrice_t));
 
  /* Send request OTA*/
  status = zclPrice_PublishPriceMatrixRsp(pReq, mGetPriceMatrix.Length);
  MSG_Free(pReq);
  return status;
 
}
#endif

#if gASL_ZclSE_12_Features_d 
static zbStatus_t ZCL_ProcessGetPriceMatrixReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_GetPriceMatrixReq_t * pGetPriceMatrix
)
{
  uint8_t entryIdx;
  
  mGetPriceMatrix.Index = 0xFF;
  
  /* Find entry in Price Matrix table*/  
  for (entryIdx = 0; entryIdx < gNumofServerPriceMatrix_c; entryIdx++)
  {
    if (FLib_MemCmp(gaServerPriceMatrixTable[entryIdx].IssuerTariffID, pGetPriceMatrix->IssuerTariffID, sizeof(SEEvtId_t)))
    {
      mGetPriceMatrix.Index = entryIdx;
      break;
    }
  }
  if(mGetPriceMatrix.Index != 0xFF)
  {
    /* This is the first command*/
    mGetPriceMatrix.CmdIndex = 0x00;
    /* Copy the address and event id*/
    FLib_MemCpy(&mGetPriceMatrix.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
    /* Start at index 0 */
    mGetPriceMatrix.Offset = 0;
      
    SendPriceClusterEvt(gzclEvtHandleGetPriceMatrix_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_PublishPriceMatrixRsp_d
static void ZCL_HandleGetPriceMatrix(void)
{
  zbStatus_t status;
     
  status = ZCL_SendPublishPriceMatrix(&mGetPriceMatrix.addrInfo, mGetPriceMatrix.tsq
       ,&gaServerPriceMatrixTable[mGetPriceMatrix.Index]);
  
  if(status == gZclSuccess_c)
  {
    mGetPriceMatrix.CmdIndex = mGetPriceMatrix.CmdIndex + 1;
    mGetPriceMatrix.Offset = mGetPriceMatrix.Offset + mGetPriceMatrix.Length;
    if (mGetPriceMatrix.Offset < gaServerPriceMatrixTable[mGetPriceMatrix.Index].Length)
      SendPriceClusterEvt(gzclEvtHandleGetPriceMatrix_c);
  }
}
#endif

#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Process the received Publish Price Matrix */
static zbStatus_t ZCL_ProcessClientPublishPriceMatrix(zclCmdPrice_PublishPriceMatrixRsp_t *pMsg, uint8_t payloadLen)
{
  uint8_t entryIdx = 0xFF;
  uint8_t i, expectedNoPrices, noPriceTiersInUse, noPriceBlocksInUse, noPricesPerFrame, tariffEntryIdx; 
  publishPriceMatrixEntry_t *pEntry;
    
   /* Search for an existing entry*/
  for(i = 0; i < gNumofClientPriceMatrix_c; i++)
  {    
    if(FLib_MemCmp(gaClientPriceMatrixTable[i].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t)))
    {
      entryIdx = i;
      break;
    }
  }
  
  /*If it is a new entry find an empty slot for it*/
  if (entryIdx == 0xFF)
    for(i = 0; i < gNumofClientPriceMatrix_c; i++)
    {    
      if(gaClientPriceMatrixTable[i].EntryStatus == gEntryNotUsed_c)
      {
        entryIdx = i;
        break;
      }
    }
  
  /* Cannot store infromation*/  
  if(entryIdx == 0xff)
    return gZclFailure_c;
  
  pEntry = &gaClientPriceMatrixTable[entryIdx];
  
  tariffEntryIdx = FindClientTariffInformationEntry(pMsg->ProviderID, pMsg->IssuerEvtID);
  noPriceTiersInUse = gaClientTariffInformationTable[tariffEntryIdx].TariffInformation.NumberPriceTiersUse;
  noPriceBlocksInUse = gaClientTariffInformationTable[tariffEntryIdx].TariffInformation.NumberBlockThreshholdsUse + 1;
  noPricesPerFrame = (uint8_t)(payloadLen - sizeof(SEEvtId_t) - 1) / sizeof(Price_t);
  
  if (((pMsg->CmdIdx + 1) * noPricesPerFrame) < (noPriceTiersInUse * noPriceBlocksInUse))
    expectedNoPrices = noPricesPerFrame;
  else  
    expectedNoPrices = (noPriceTiersInUse * noPriceBlocksInUse) - (pMsg->CmdIdx * noPricesPerFrame);  
  
  /* Save the information in the Price Matrix Table*/
  FLib_MemCpy(pEntry->IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  pEntry->Length = (pMsg->CmdIdx * noPricesPerFrame) + expectedNoPrices;
  pEntry->EntryStatus = gEntryUsed_c;
  FLib_MemCpy(&pEntry->TierBlockPrice[pMsg->CmdIdx * noPricesPerFrame], &pMsg->TierBlockPrice[0], expectedNoPrices);
  
  return gZclSuccess_c;
}
#endif

/******************************************************************************
* Get / Publish Tier Labels
******************************************************************************/

/******************************************************************************/
/* The Publish Tier Labels command is generated in response to receiving a Get Tier Labels
command.*/
#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_PublishTierLabelsRsp_d
static zbStatus_t ZCL_SendPublishTierLabels(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishTierLabelsEntry_t * pMsg) 
{
  
  zclPrice_PublishTierLabelsRsp_t *pReq;   
  uint8_t iTariffIndex, numLabels = 0, payloadCursor = mGetTierLabels.Offset;
  uint16_t maxSubPayloadLen;
  zbStatus_t status;
    
  /* Find how much prices can fit into the packet */
  iTariffIndex = FindServerTariffInformationEntryByTariffId(&pMsg->IssuerTariffID);
  
  if (iTariffIndex == gInvalidTableIndex_c)
    return gZclNotFound_c;
  
  /* Allocate buffer for OTA frame*/
  pReq = AF_MsgAlloc();
  if(!pReq)
    return gZbNoMem_c;
  
  /* Get the number of Price Tiers that the tariff uses */
//  noPriceTiersInUse = gaServerTariffInformationTable[iTariffIndex].TariffInformation.NumberPriceTiersUse;
    
  /* Masimum ASDU left for the tier labels */
  maxSubPayloadLen = GetMaxOutTransferSize(pAddrInfo->dstAddr.aNwkAddr) - sizeof(zclFrame_t) - 3*sizeof(SEEvtId_t) - 3*sizeof(uint8_t);
  
  mGetTierLabels.Length = (maxSubPayloadLen > pMsg->Length) ? pMsg->Length : maxSubPayloadLen;

  /* Count the number of labels we can fit */
  while (payloadCursor < pMsg->Length)
  {
    zclTierEntry_t *pEntry = (zclTierEntry_t *)((uint8_t *)&pMsg->TierLabel[0] + payloadCursor);

    payloadCursor += pEntry->tierLabel.length + 2 * sizeof(uint8_t);
    
    numLabels++;
    
 }
  
 /* Add address information */
  FLib_MemCpy(&pReq->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  
  /* Check if fragmentation is required */
  if(sizeof(zclCmdPrice_PublishTierLabelsRsp_t) - sizeof(zclTierEntry_t) + mGetTierLabels.Length > AF_MaxPayloadLen(pAddrInfo) - sizeof(zclFrame_t))
        pReq->addrInfo.txOptions |= gApsTxOptionFragmentationRequested_c;
  
  pReq->zclTransactionId = tsq;
  
  /* Add application payload*/
  FLib_MemCpy(pReq->cmdFrame.IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  FLib_MemCpy(pReq->cmdFrame.IssuerTariffID, pMsg->IssuerTariffID, sizeof(SEEvtId_t));
  FLib_MemCpy(pReq->cmdFrame.ProviderID, pMsg->ProviderID, sizeof(SEEvtId_t));
  pReq->cmdFrame.NumOfLabels = numLabels;
  pReq->cmdFrame.CmdIdx = mGetTierLabels.CmdIndex;
  pReq->cmdFrame.TotalNumOfCmds = (uint8_t)(pMsg->Length / maxSubPayloadLen) + 1;
  FLib_MemCpy(&pReq->cmdFrame.TierLabel[0], (uint8_t *)&pMsg->TierLabel[0] +  mGetTierLabels.Offset, mGetTierLabels.Length);
 
  /* Send request OTA*/
  status = zclPrice_PublishTierLabelsRsp(pReq, mGetTierLabels.Length);
  MSG_Free(pReq);
  return status;
 
}
#endif

#if gASL_ZclSE_12_Features_d 
static zbStatus_t ZCL_ProcessGetTierLabelsReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_GetTierLabelsReq_t * pGetTierLabels
)
{
  uint8_t entryIdx;
  
  mGetTierLabels.Index = 0xFF;
  
  /* Find entry in Price Matrix table*/  
  for (entryIdx = 0; entryIdx < gNumofServerTierLabels_c; entryIdx++)
  {
    if (FLib_MemCmp(gaServerTierLabelsTable[entryIdx].IssuerTariffID, pGetTierLabels->IssuerTariffID, sizeof(SEEvtId_t)))
    {
      mGetTierLabels.Index = entryIdx;
      break;
    }
  }
  if(mGetTierLabels.Index != 0xFF)
  {
    /* This is the first command*/
    mGetTierLabels.CmdIndex = 0x00;
    /* Copy the address and event id*/
    FLib_MemCpy(&mGetTierLabels.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
    mGetTierLabels.tsq = tsq;
    
    /* Start at index 0 */
    mGetTierLabels.Offset = 0;
      
    SendPriceClusterEvt(gzclEvtHandleGetTierLabels_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_PublishTierLabelsRsp_d
static void ZCL_HandleGetTierLabels(void)
{
  zbStatus_t status;
     
  status = ZCL_SendPublishTierLabels(&mGetTierLabels.addrInfo, mGetTierLabels.tsq
       ,&gaServerTierLabelsTable[mGetTierLabels.Index]);
  
  if(status == gZclSuccess_c)
  {
    mGetTierLabels.CmdIndex = mGetTierLabels.CmdIndex + 1;
    mGetTierLabels.Offset = mGetTierLabels.Offset + mGetTierLabels.Length;
    if (mGetTierLabels.Offset < gaServerTierLabelsTable[mGetTierLabels.Index].Length)
      SendPriceClusterEvt(gzclEvtHandleGetTierLabels_c);
  }
}
#endif

#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Process the received Publish Price Matrix */
static zbStatus_t ZCL_ProcessClientPublishTierLabels(zclCmdPrice_PublishTierLabelsRsp_t *pMsg, uint8_t payloadLen)
{
  uint8_t entryIdx = 0xFF;
  uint8_t i; 
  
   /* Search for an existing entry*/
  for(i = 0; i < gNumofClientTierLabels_c; i++)
  {    
    if(FLib_MemCmp(gaClientTierLabelsTable[i].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t)))
    {
      entryIdx = i;
      break;
    }
  }
  
  /*If it is a new entry find an empty slot for it*/
  if (entryIdx == 0xFF)
    for(i = 0; i < gNumofClientTierLabels_c; i++)
    {    
      if(gaClientTierLabelsTable[i].EntryStatus == gEntryNotUsed_c)
      {
        entryIdx = i;
        break;
      }
    }
  
  /* Cannot store infromation*/  
  if(entryIdx == 0xff)
    return gZclFailure_c;

  return gZclSuccess_c;
}
#endif

/******************************************************************************
* Get / Publish Tariff Information
******************************************************************************/

/******************************************************************************/
/* The Publish Tariff Information command is generated in response to receiving a Get Tariff Information
command or when an update to the Tariff Information is available
from the commodity provider. */
#if gASL_ZclSE_12_Features_d
static zbStatus_t ZCL_SendPublishTariffInformation(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishTariffInformationEntry_t * pMsg) 
{
  zclPrice_PublishTariffInformationRsp_t *pRsp;
  zbStatus_t status;

  pRsp = AF_MsgAlloc();
  
  if (!pRsp)
    return gZclNoMem_c;  
    
  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  pRsp->zclTransactionId = tsq;
  FLib_MemCpy(&pRsp->cmdFrame, pMsg, sizeof(zclCmdPrice_PublishTariffInformationRsp_t));
  
  status = zclPrice_PublishTariffInformationRsp(pRsp);
  
  MSG_Free(pRsp);
  
  return status;
}
#endif

#if gASL_ZclSE_12_Features_d 
static zbStatus_t ZCL_ProcessGetTariffInformationReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t         tsq,
  zclCmdPrice_GetTariffInformationReq_t * pGetTariffInformation
)
{
  uint8_t entryIndex;
  
  /* Copy the address*/
  FLib_MemCpy(&mGetTariffInformation.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  mGetTariffInformation.tsq = tsq;
  mGetTariffInformation.index = 0x00;
  
  if (pGetTariffInformation->NumOfCmds > 0)
    mGetTariffInformation.numOfEvtsRemaining = pGetTariffInformation->NumOfCmds;
  else
    mGetTariffInformation.numOfEvtsRemaining = gNumofServerTariffInformation_c;
  
  if (pGetTariffInformation->MinIssuerEvtId == 0xFFFFFFFF)
    mGetTariffInformation.minIssuerEvtId = 0;
  else
    mGetTariffInformation.minIssuerEvtId = pGetTariffInformation->MinIssuerEvtId;
  
  mGetTariffInformation.tariffType = pGetTariffInformation->TariffType;
  
  /* Check valid value for tarif type*/
  if ((mGetTariffInformation.tariffType > 0x02) &&
      (mGetTariffInformation.tariffType != 0xFF))
  {
    return gZclFailure_c;
  }
  /* Get the start time */
  mGetTariffInformation.earliestStartTime = OTA2Native32(pGetTariffInformation->EarliestStartTime);
      
  /* Find next valid entry */
  entryIndex = FindNextTariffInformationEvent();
  if(entryIndex != 0xFF)
  {
    mGetTariffInformation.index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetTariffInformation_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Process the received Publish Tariff Information */
static zbStatus_t ZCL_ProcessClientPublishTariffInformation(zclCmdPrice_PublishTariffInformationRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;
  uint32_t msgIssuerId, entryIssuerId;
  
  /* Shift information to compensate the label length*/
  FLib_MemInPlaceCpy(&pMsg->NumberPriceTiersUse, &pMsg->TariffLabel.aStr[pMsg->TariffLabel.length],
                     sizeof(zclCmdPrice_PublishTariffInformationRsp_t) - 4*sizeof(uint32_t) - 1 - sizeof(zclStr24Oct_t));
  
  msgIssuerId = GetNative32BitInt(pMsg->IssuerTariffID);
    /* Check if it is a Tariff Information Update */
  for(updateIndex=0; updateIndex < gNumofClientTariffInformation_c; updateIndex++)
  {
    if(FLib_MemCmp(&gaClientTariffInformationTable[updateIndex].TariffInformation.StartTime, &pMsg->StartTime, sizeof(ZCLTime_t))
       && (gaClientTariffInformationTable[updateIndex].EntryStatus != gEntryNotUsed_c))
    {
      entryIssuerId = GetNative32BitInt(gaClientTariffInformationTable[updateIndex].TariffInformation.IssuerTariffID);
      if(entryIssuerId > msgIssuerId)
      {
        updateIndex = 0xFE;// reject it;
      }
      else
      {
        FLib_MemCpy(&gaClientTariffInformationTable[updateIndex], pMsg, sizeof(zclCmdPrice_PublishTariffInformationRsp_t));
      }
      break;
    }
  }
  /*if the Publish Tariff Information is not rejected, add it in the table */
  
  if(updateIndex != 0xfe)
  {
    if(updateIndex == gNumofClientTariffInformation_c)
    {
      /* Add the new Tariff information in the table */
      newEntry = AddTariffInformationInTable((publishTariffInformationEntry_t *)&gaClientTariffInformationTable[0], gNumofClientTariffInformation_c, pMsg);
      if (newEntry != 0xff)
      status = gZclSuccess_c; 
    }
    else
    {
      /* the Tariff Information was updated */
      gaClientTariffInformationTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      status = gZclSuccess_c;
    }
  }
  
  //if(status == gZclSuccess_c)
    //TS_SendEvent(gSETaskID, gzclEvtHandleTariffInformation_c);
  
  return status;
}
#endif

#if gASL_ZclSE_12_Features_d 
/* Find an Event in the Events Table...  */
static uint8_t FindNextTariffInformationEvent(void)
{
  uint8_t i; 
  
  for(i = mGetTariffInformation.index; i < gNumofServerTariffInformation_c; i++)
  {
    publishTariffInformationEntry_t *pEntry = &gaServerTariffInformationTable[i];
    /* If this entry exists */
    if(pEntry->EntryStatus != 0x00)  
    {
      /* Check the tariff type */
      if ((mGetTariffInformation.tariffType != 0xFF) &&
          (pEntry->TariffInformation.TariffType.tariffType != mGetTariffInformation.tariffType))
      {
        continue;
      }
      
      /* Check the event id */
      if (pEntry->TariffInformation.IssuerEvtID < mGetTariffInformation.minIssuerEvtId)
      {
        continue;
      }
      
      /* Check the start time */
      if((mGetTariffInformation.earliestStartTime <= pEntry->EffectiveStartTime))
      {
        /* It is a scheduled tariff */
        return i;
      }
    }
  }
  return 0xFF; 
}   
#endif

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_PublishTariffInformationRsp_d
static void ZCL_HandleGetTariffInformation(void)
{
  uint8_t status;
  uint8_t entryIndex;
     
  status= ZCL_SendPublishTariffInformation(&mGetTariffInformation.addrInfo,
       mGetTariffInformation.tsq ,&gaServerTariffInformationTable[mGetTariffInformation.index]);
  
  if(status == gZclSuccess_c)
  {
    mGetTariffInformation.numOfEvtsRemaining = mGetTariffInformation.numOfEvtsRemaining - 1;
    mGetTariffInformation.index = mGetTariffInformation.index + 1;
    if (mGetTariffInformation.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      entryIndex = FindNextTariffInformationEvent();
      if(entryIndex != 0xFF)
      {
        mGetTariffInformation.index = entryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetTariffInformation_c);
      }
    }
  }
}
#endif

#if gASL_ZclSE_12_Features_d
static void ZCL_HandleTariffInformationUpdate(index_t iEntryIdx)
{
  afAddrInfo_t addrInfo;
  
  addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishTariffInformation(&addrInfo, ZbZclFoundationGlobals(gZclTransactionId)++, &gaServerTariffInformationTable[iEntryIdx]);
  return;
}
#endif 

#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Add Tariff Information in Table... so that to have the scheduled price in ascendent order */
static uint8_t AddTariffInformationInTable(publishTariffInformationEntry_t *pTable, uint8_t len, zclCmdPrice_PublishTariffInformationRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->StartTime);
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the Tariff Information in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->TariffInformation.StartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishTariffInformationEntry_t));
  
  /* Copy tariff information*/
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishTariffInformationRsp_t));
  
  if(pMsg->StartTime == 0x00000000) 
  {
    (pTable+poz)->EffectiveStartTime = Native2OTA32(ZCL_GetUTCTime(BeeAppDataInit(appEndPoint)));
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->StartTime ;
  
  /* New tariff information was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  
  /* Call the App to signal that a Tariff Information was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_TariffInformationEvt_c, 0, 0, (pTable+poz));
  return poz;
}
#endif

#if gASL_ZclSE_12_Features_d
static uint8_t FindServerTariffInformationEntryByTariffId(SEEvtId_t *pIssuerTariffID)
{
  uint8_t i;
  
   /* Search for an existing entry*/
  for(i = 0; i < gNumofServerTariffInformation_c; i++)
  {    
    if(FLib_MemCmp(gaServerTariffInformationTable[i].TariffInformation.IssuerTariffID, pIssuerTariffID, sizeof(SEEvtId_t)))
    {
      return i;
    }
  }
  return 0xFF;
}
#endif

#if gASL_ZclSE_12_Features_d
static uint8_t FindClientTariffInformationEntry(SEEvtId_t providerID, SEEvtId_t issuerTariffID)
{
  uint8_t i;
  
   /* Search for an existing entry*/
  for(i = 0; i < gNumofClientTariffInformation_c; i++)
  {    
    if(FLib_MemCmp(gaClientTariffInformationTable[i].TariffInformation.ProviderID, providerID, sizeof(SEEvtId_t)) &&
       FLib_MemCmp(gaClientTariffInformationTable[i].TariffInformation.IssuerTariffID, issuerTariffID, sizeof(SEEvtId_t)))
    {
      return i;
    }
  }
  return 0xFF;
}
#endif

/******************************************************************************
* Get / Publish CO2 Value
******************************************************************************/
/* The Publish CO2 Value command is generated in response to receiving a Get CO2 Value
command or when an update to the CO2 Value information is available
from the commodity provider. */
#if gASL_ZclSE_12_Features_d
static zbStatus_t ZCL_SendPublishCO2Value(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishCO2ValueEntry_t * pMsg) 
{
  zclPrice_PublishCO2ValueRsp_t* pRsp;  
  zbStatus_t status;
  
  pRsp = AF_MsgAlloc();
  
  if (!pRsp)
    return gZclNoMem_c;
  
  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  pRsp->zclTransactionId = tsq;
  FLib_MemCpy(&pRsp->cmdFrame, pMsg, sizeof(zclCmdPrice_PublishCO2ValueRsp_t));

  status = zclPrice_PublishCO2ValueRsp(pRsp);
  
  MSG_Free(pRsp);
  
  return status;
}
#endif

#if gASL_ZclSE_12_Features_d 
/* Find an Event in the Events Table...  */
static uint8_t FindNextCO2ValueEvent(void)
{
  uint8_t i; 
  
  for(i = mGetCO2Value.index; i < gNumofServerCO2Value_c; i++)
  {
    publishCO2ValueEntry_t *pEntry = &gaServerCO2ValueTable[i];
    /* If this entry exists */
    if(pEntry->EntryStatus != 0x00)  
    {
      /* Check the tariff type */
      if ((mGetCO2Value.tariffType != 0xFF) &&
          (pEntry->CO2Value.TariffType != mGetCO2Value.tariffType))
      {
        continue;
      }
      
      /* Check the event id */
      if (pEntry->CO2Value.IssuerEvtID < mGetCO2Value.minIssuerEvtId)
      {
        continue;
      }
      
      /* Check the start time */
      if((mGetCO2Value.earliestStartTime <= pEntry->EffectiveStartTime))
      {
        /* It is a scheduled tariff */
        return i;
      }
    }
  }
  return 0xFF; 
}   
#endif

#if gASL_ZclSE_12_Features_d
static void ZCL_HandleCO2ValueUpdate(index_t iEntryIdx)
{
  afAddrInfo_t addrInfo;
  
  addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishCO2Value(&addrInfo, ZbZclFoundationGlobals(gZclTransactionId)++, &gaServerCO2ValueTable[iEntryIdx]);
  return;
}
#endif 

#if gASL_ZclSE_12_Features_d 
static zbStatus_t ZCL_ProcessGetCO2ValueReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_GetCO2ValueReq_t * pGetCO2Value
)
{
  uint8_t entryIndex;
  
  /* Copy the address*/
  FLib_MemCpy(&mGetCO2Value.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  mGetCO2Value.tsq = tsq;
  mGetCO2Value.index = 0x00;
  
  if (pGetCO2Value->NumOfCmds > 0)
    mGetCO2Value.numOfEvtsRemaining = pGetCO2Value->NumOfCmds;
  else
    mGetCO2Value.numOfEvtsRemaining = gNumofServerCO2Value_c;
  
  if (pGetCO2Value->MinIssuerEvtId == 0xFFFFFFFF)
    mGetCO2Value.minIssuerEvtId = 0;
  else
    mGetCO2Value.minIssuerEvtId = pGetCO2Value->MinIssuerEvtId;
  
  mGetCO2Value.tariffType = pGetCO2Value->TariffType;
  
  /* Check valid value for tarif type*/
  if ((mGetCO2Value.tariffType > 0x02) &&
      (mGetCO2Value.tariffType != 0xFF))
  {
    return gZclFailure_c;
  }
  /* Get the start time */
  mGetCO2Value.earliestStartTime = OTA2Native32(pGetCO2Value->EarliestStartTime);
      
  /* Find next valid entry */
  entryIndex = FindNextCO2ValueEvent();
  if(entryIndex != 0xFF)
  {
    mGetCO2Value.index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetCO2Value_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_PublishCO2ValueRsp_d
static void ZCL_HandleGetCO2Value(void)
{
  uint8_t status;
  uint8_t entryIndex;
     
  status= ZCL_SendPublishCO2Value(&mGetCO2Value.addrInfo, mGetCO2Value.tsq
       ,&gaServerCO2ValueTable[mGetCO2Value.index]);
  
  if(status == gZclSuccess_c)
  {
    mGetCO2Value.numOfEvtsRemaining = mGetCO2Value.numOfEvtsRemaining - 1;
    mGetCO2Value.index = mGetCO2Value.index + 1;
    if (mGetCO2Value.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      entryIndex = FindNextCO2ValueEvent();
      if(entryIndex != 0xFF)
      {
        mGetCO2Value.index = entryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetCO2Value_c);
      }
    }
  }
}
#endif

#if gASL_ZclSE_12_Features_d
/* Add CO2 Value in Table... so that to have the scheduled price in ascendent order */
static uint8_t AddCO2ValueInTable(publishCO2ValueEntry_t *pTable, uint8_t len, zclCmdPrice_PublishCO2ValueRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->StartTime);
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the CO2 Value in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->CO2Value.StartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishCO2ValueEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishCO2ValueRsp_t));
  
  if((pMsg->StartTime == 0x00000000)||
     (pMsg->StartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->StartTime ;
  /* new billing period was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Block period was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_CO2ValueEvt_c, 0, 0, (pTable+poz));
  return poz;
}
#endif

#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Process the received Publish CO2 Value */
static zbStatus_t ZCL_ProcessClientPublishCO2Value(zclCmdPrice_PublishCO2ValueRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

   uint32_t msgIssuerId, entryIssuerId;
  
  msgIssuerId = pMsg->IssuerEvtID;
    /* Check if it is a CO2 Value Update */
  for(updateIndex=0; updateIndex < gNumofClientCO2Value_c; updateIndex++)
  {
    if(FLib_MemCmp(&gaClientCO2ValueTable[updateIndex].CO2Value.StartTime, &pMsg->StartTime, sizeof(ZCLTime_t))
       && (gaClientCO2ValueTable[updateIndex].EntryStatus != gEntryNotUsed_c))
    {
      entryIssuerId = gaClientCO2ValueTable[updateIndex].CO2Value.IssuerEvtID;
      if(entryIssuerId > msgIssuerId)
      {
        updateIndex = 0xFE;// reject it;
      }
      else
      {
        FLib_MemCpy(&gaClientCO2ValueTable[updateIndex], pMsg, sizeof(zclCmdPrice_PublishCO2ValueRsp_t));
      }
      break;
    }
  }
  /*if the Publish CO2 value is not rejected, add it in the table */
  
  if(updateIndex != 0xfe)
  {
    if(updateIndex == gNumofClientCO2Value_c)
    {
      /* Add the new CO2 value in the table */
      newEntry = AddCO2ValueInTable((publishCO2ValueEntry_t *)&gaClientCO2ValueTable[0], gNumofClientCO2Value_c, pMsg);
      if (newEntry != 0xff)
      status = gZclSuccess_c; 
    }
    else
    {
      /* the CO2 value was updated */
      gaClientCO2ValueTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      status = gZclSuccess_c;
    }
  }
  
  //if(status == gZclSuccess_c)
    //TS_SendEvent(gSETaskID, gzclEvtHandleClientPrices_c);
  
  return status;
}
#endif


/******************************************************************************
* Get / Publish CPP Event
******************************************************************************/
#if gASL_ZclSE_12_Features_d
static zbStatus_t ZCL_SendPublishCPPEvent(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishCPPEventEntry_t * pMsg) 
{
  zclPrice_PublishCPPEventRsp_t *pReq;    
  uint32_t currentTime, stopTime, newDuration;
  uint16_t duration;
  zbStatus_t status;
  
  pReq = AF_MsgAlloc();
  
  if (!pReq)
    return gZclNoMem_c;
 
  currentTime = ZCL_GetUTCTime(pAddrInfo->srcEndPoint);
  
  FLib_MemCpy(&pReq->cmdFrame.IssuerEvtID, &pMsg->CPPEvent.IssuerEvtID, sizeof(SEEvtId_t));
  pReq->cmdFrame.CPPEventStartTime = pMsg->CPPEvent.CPPEventStartTime;
  if((pMsg->CPPEvent.CPPEventStartTime) == 0x00000000)
  {
    /* Get the duration value */
    duration = GetNative16BitInt(pMsg->CPPEvent.CPPEventDurationInMinutes);
    stopTime = Native2OTA32(pMsg->EffectiveStartTime) + ( 60 *  duration);
    newDuration = (stopTime - currentTime) / 60;
    FLib_MemCpy(&pReq->cmdFrame.CPPEventDurationInMinutes, &newDuration, sizeof(Duration16_t));
  }
  else
  {
    FLib_MemCpy(&pReq->cmdFrame.CPPEventDurationInMinutes, &pMsg->CPPEvent.CPPEventDurationInMinutes, sizeof(Duration16_t));
  }

  pReq->cmdFrame.TariffType = pMsg->CPPEvent.TariffType;
  pReq->cmdFrame.CPPPriceTier = pMsg->CPPEvent.CPPPriceTier;
  pReq->cmdFrame.CPPAuth = pMsg->CPPEvent.CPPAuth;
 
  FLib_MemCpy(&pReq->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  pReq->zclTransactionId = tsq;
  
  status = zclPrice_PublishCPPEventRsp(pReq);
      
  MSG_Free(pReq);
  
  return status;
}
#endif

#if gASL_ZclSE_12_Features_d 
static zbStatus_t ZCL_ProcessCPPEventRsp
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_CPPEventRsp_t * pCPPEventResponse
)
{
  zbStatus_t status = gZclFailure_c;
  uint8_t i;
  uint8_t index = 0xFF;
  
  for(i = 0; i < gNumofServerCPPEvents_c; i++)
  {    
    if(FLib_MemCmp(&gaServerCPPEventTable[i].CPPEvent.IssuerEvtID, &pCPPEventResponse->IssuerEvtID, sizeof(SEEvtId_t)))
      index = i;
  }
  
  if (index == 0xFF)
    return gZclFailure_c;
       
  /* Check the status of the Authorization*/
  if ((pCPPEventResponse->CPPAuth == gPriceCPPEventAccepted) ||
      (pCPPEventResponse->CPPAuth == gPriceCPPEventRejected))
  {
    gaServerCPPEventTable[index].CPPEvent.CPPAuth = pCPPEventResponse->CPPAuth;
    status = ZCL_SendPublishCPPEvent(pAddrInfo, tsq, &gaServerCPPEventTable[index]);
  }
  return status;
}
#endif

#if gASL_ZclSE_12_Features_d
static void ZCL_HandleCPPEventUpdate(index_t iEntryIdx)
{
  afAddrInfo_t addrInfo;
  
  addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishCPPEvent(&addrInfo, ZbZclFoundationGlobals(gZclTransactionId)++, &gaServerCPPEventTable[iEntryIdx]);
}
#endif 

#if gASL_ZclSE_12_Features_d
static uint8_t CheckForCPPEventUpdate(zclCmdPrice_PublishCPPEventRsp_t *pMsg, publishCPPEventEntry_t *pTable, uint8_t len)
{
  uint8_t i;
  uint32_t entryIssuerEvt;
 
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      entryIssuerEvt = FourBytesToUint32(&(pTable+i)->CPPEvent.IssuerEvtID);
      entryIssuerEvt = OTA2Native32(entryIssuerEvt);
      if (FLib_MemCmp(&(pTable+i)->CPPEvent.IssuerEvtID, &pMsg->IssuerEvtID, sizeof(SEEvtId_t)))
        FLib_MemCpy((pTable+i), pMsg, sizeof(zclCmdPrice_PublishCPPEventRsp_t));
        return i;
    }
  }
  return 0xff;
}
#endif

#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Add CPP Event in Table... so that to have the scheduled events in asccendent order */
static uint8_t AddCPPEventsInTable(publishCPPEventEntry_t *pTable, uint8_t len, zclCmdPrice_PublishCPPEventRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->CPPEventStartTime);
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the events in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->CPPEvent.CPPEventStartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishCPPEventEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishCPPEventRsp_t));
  
  if(pMsg->CPPEventStartTime == 0x00000000)
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->CPPEventStartTime ;
  /* new event was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  return poz;
}
#endif

#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Process the received Publish CPP Event */
static zbStatus_t ZCL_ProcessClientPublishCPPEvent(zclCmdPrice_PublishCPPEventRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 

  /* Check if it is a CPP Event Update */
  updateIndex = CheckForCPPEventUpdate(pMsg, (publishCPPEventEntry_t *)&gaClientCPPEventTable[0], gNumofClientCPPEvents_c);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new CPP Event in the table */
    updateIndex = AddCPPEventsInTable((publishCPPEventEntry_t *)&gaClientCPPEventTable[0], gNumofClientCPPEvents_c, pMsg);
    if (updateIndex != 0xff)
      status = gZclSuccess_c;
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      FLib_MemCpy(&gaClientCPPEventTable[updateIndex], pMsg, sizeof(zclCmdPrice_PublishCPPEventRsp_t));
      status = gZclSuccess_c;
    }
}
  
  if ((status == gZclSuccess_c) && gaClientCPPEventTable[updateIndex].CPPEvent.CPPAuth != gPriceCPPEventForced)
    BeeAppUpdateDevice (0, gZclUI_CPPEvt_c, 0, 0, &updateIndex);
  
  return gZclSuccess_c;
}
#endif

/******************************************************************************
* Get / Publish Billing Period
******************************************************************************/

/******************************************************************************/
/* The Publish Billing Period command is generated in response to receiving a Get Billing Period
command or when an update to the billing period information is available
from the commodity provider. */
#if gASL_ZclSE_12_Features_d
static zbStatus_t ZCL_SendPublishBillingPeriod(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishBillingPeriodEntry_t * pMsg) 
{
  zclPrice_PublishBillingPeriodRsp_t *pRsp;    
  ZCLTime_t currentTime, elapsedTime;
  zbStatus_t status;
  
  pRsp = AF_MsgAlloc();
  
  if (!pRsp)
    return gZclNoMem_c;
  
  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  pRsp->zclTransactionId = tsq;
  
  FLib_MemCpy(&pRsp->cmdFrame, &pMsg->BillingPeriod, sizeof(zclCmdPrice_PublishBillingPeriodRsp_t));
  
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  
  if((pRsp->cmdFrame.BillingPeriodStartTime == 0x00000000) && 
     (!CmpToFs(pRsp->cmdFrame.BillingPeriodDuration, sizeof(Duration24_t))))
  {
    // get the duration value
    elapsedTime = (currentTime - OTA2Native32(pMsg->EffectiveStartTime)) / 60;
    Zcl_SubtractDuration(pRsp->cmdFrame.BillingPeriodDuration, elapsedTime);
  }
  
  status = zclPrice_PublishBillingPeriodRsp(pRsp);
  
  MSG_Free(pRsp);
  
  return status;

}
#endif

#if gASL_ZclSE_12_Features_d 
static zbStatus_t ZCL_ProcessGetBillingPeriodReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_GetBillingPeriodReq_t * pGetBillingPeriod
)
{
  uint8_t entryIndex;
    
  /* Copy address information and TSQ*/
  FLib_MemCpy(&mGetBillingPeriod.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  mGetBillingPeriod.tsq = tsq;
  
  mGetBillingPeriod.index = 0;
  
  /* Get the start time */ 
  mGetBillingPeriod.earliestStartTime = OTA2Native32(pGetBillingPeriod->EarliestStartTime);

  /* Get the minimum issuer id */ 
 if (pGetBillingPeriod->MinIssuerEvtId == 0xFFFFFFFF)
    mGetBillingPeriod.minIssuerEvtId = 0;
  else
    mGetBillingPeriod.minIssuerEvtId = pGetBillingPeriod->MinIssuerEvtId;

  /* Get number of events*/  
  if(pGetBillingPeriod->NumOfCmds == 0)
    mGetBillingPeriod.numOfEvtsRemaining = gNumofServerBillingPeriods_c;
  else
    mGetBillingPeriod.numOfEvtsRemaining = pGetBillingPeriod->NumOfCmds;

  mGetBillingPeriod.tariffType = pGetBillingPeriod->TariffType;
  
  /* Find next valid entry */
  entryIndex = FindNextBillingPeriodEvent();
  if(entryIndex != 0xFF)
  {
    mGetBillingPeriod.index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetBillingPeriod_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

#if gASL_ZclSE_12_Features_d 
/* Find an Event in the Events Table...  */
static uint8_t FindNextBillingPeriodEvent(void)
{
  uint8_t i; 
  
  for(i = mGetBillingPeriod.index; i < gNumofServerBillingPeriods_c; i++)
  {
    publishBillingPeriodEntry_t *pEntry = &gaServerBillingPeriodTable[i];
    
    // if exits this entry
    if(pEntry->EntryStatus != 0x00)  
    {      
      if((mGetBillingPeriod.earliestStartTime <= pEntry->EffectiveStartTime) && 
         (mGetBillingPeriod.numOfEvtsRemaining > 0) &&
         ((mGetBillingPeriod.tariffType == pEntry->BillingPeriod.TariffType)) &&
         (mGetBillingPeriod.minIssuerEvtId <= pEntry->BillingPeriod.IssuerEvtID))
      {
        return i;
      }
    }
  }
  return 0xFF; 
}   
#endif

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_PublishBillingPeriodRsp_d
static void ZCL_HandleGetBillingPeriod(void)
{
  uint8_t status;
  uint8_t entryIndex;
     
  status= ZCL_SendPublishBillingPeriod(&mGetBillingPeriod.addrInfo, mGetBillingPeriod.tsq
       ,&gaServerBillingPeriodTable[mGetBillingPeriod.index]);
  
  if(status == gZclSuccess_c)
  {
    mGetBillingPeriod.numOfEvtsRemaining = mGetBillingPeriod.numOfEvtsRemaining - 1;
    mGetBillingPeriod.index = mGetBillingPeriod.index + 1;
    if (mGetBillingPeriod.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      entryIndex = FindNextBillingPeriodEvent();
      if(entryIndex != 0xFF)
      {
        mGetBillingPeriod.index = entryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetBillingPeriod_c);
      }
    }
  }
}
#endif

#if gASL_ZclSE_12_Features_d
static void ZCL_HandleBillingPeriodUpdate(index_t iEntryIdx)
{
  afAddrInfo_t addrInfo;
  
  addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishBillingPeriod(&addrInfo, ZbZclFoundationGlobals(gZclTransactionId)++, &gaServerBillingPeriodTable[iEntryIdx]);
}
#endif 

#if gASL_ZclSE_12_Features_d
static uint8_t AddBillingPeriodsInTable(publishBillingPeriodEntry_t *pTable, uint8_t len, zclCmdPrice_PublishBillingPeriodRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->BillingPeriodStartTime);
  
  if(msgStartTime == 0x000000)
  {
    msgStartTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  }
  
  /* keep the Billing Periods in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->BillingPeriod.BillingPeriodStartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishBillingPeriodEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishBillingPeriodRsp_t));
  
  if (pMsg->BillingPeriodStartTime == 0x00000000)
  {
    /*  here get the currentTime  */
    (pTable+poz)->EffectiveStartTime = Native2OTA32(ZCL_GetUTCTime(BeeAppDataInit(appEndPoint)));
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->BillingPeriodStartTime ;
  /* new billing period was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Billing Period was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_BillingPeriodEvt_c, 0, 0, (pTable+poz));
  return poz;
}

/******************************************************************************/
/* The Billing Period timer callback that keep track of current active billing period */
static void TimerClientBillingPeriodCallBack(zbTmrTimerID_t timerID)
{
  (void) timerID;
   SendPriceClusterEvt(gzclEvtHandleClientBillingPeriod_c);
}

/******************************************************************************/
/* Process the received Publish Billing Period */
static zbStatus_t ZCL_ProcessClientPublishBillingPeriod(zclCmdPrice_PublishBillingPeriodRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a BillingPeriod Update */
  updateIndex = CheckForBillingPeriodUpdate(pMsg, (publishBillingPeriodEntry_t *)&gaClientBillingPeriodTable[0], gNumofClientBillingPeriods_c);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new Block Period information in the table */
    newEntry = AddBillingPeriodsInTable((publishBillingPeriodEntry_t *)&gaClientBillingPeriodTable[0], gNumofClientBillingPeriods_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      /* the price was updated */
      gaClientBillingPeriodTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      /* Call the App to signal that a Price was updated; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_BillingPeriodEvt_c, 0, 0, &gaClientBillingPeriodTable[updateIndex]);
      status = gZclSuccess_c;
    }
  }
  
  if(status == gZclSuccess_c)
    SendPriceClusterEvt(gzclEvtHandleClientBillingPeriod_c);
  
  return gZclSuccess_c;
}
#endif

/******************************************************************************/
/*Check and Updated a BillingPeriod.
When new BillingPeriod information is provided that replaces older block period
information for the same time period, IssuerEvt field allows devices to determine which
information is newer. It is expected that the value contained in this field is a
unique number managed by upstream servers.
Thus, newer BillingPeriod information will have a value in the Issuer Event ID field that
is larger than older pricing information.
*/
#if gASL_ZclSE_12_Features_d
static uint8_t CheckForBillingPeriodUpdate(zclCmdPrice_PublishBillingPeriodRsp_t *pMsg, publishBillingPeriodEntry_t *pTable, uint8_t len)
{
  uint8_t i;
  uint32_t msgIssuerEvt, entryIssuerEvt;
  
  msgIssuerEvt = FourBytesToUint32(pMsg->IssuerEvtID);
  msgIssuerEvt = OTA2Native32(msgIssuerEvt);
  
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      entryIssuerEvt = FourBytesToUint32(&(pTable+i)->BillingPeriod.IssuerEvtID);
      entryIssuerEvt = OTA2Native32(entryIssuerEvt);
      if ((pTable+i)->BillingPeriod.BillingPeriodStartTime == pMsg->BillingPeriodStartTime &&
          FLib_MemCmp(&(pTable+i)->BillingPeriod.BillingPeriodDuration, &pMsg->BillingPeriodDuration, sizeof(Duration24_t)))
      {
        if(entryIssuerEvt < msgIssuerEvt)
        {
          FLib_MemCpy((pTable+i), pMsg, sizeof(zclCmdPrice_PublishBillingPeriodRsp_t));
          return i;
        }
        else 
          return 0xFE; /* reject it */
      }
    }
  }
  return 0xff;
}
#endif

/******************************************************************************/
/* Handle the Client Billing Period signalling when the current Billing starts, was updated or is completed */
#if gASL_ZclSE_12_Features_d
static void ZCL_HandleClientBillingPeriod(void)
{
  uint32_t currentTime, startTime, nextTime = 0x00000000, stopTime;
  uint32_t duration;
  publishBillingPeriodEntry_t *pEntry = &gaClientBillingPeriodTable[0];
  
  /* the Billing Period table is kept in ascendent order; check if any Billing Period is scheduled*/
  if(pEntry->EntryStatus == 0x00)
  {
    ZbTMR_StopTimer(gBillingPeriodClientTimerID);
    return;
  }
  
  /* Get the timing */
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  startTime = OTA2Native32(pEntry->EffectiveStartTime);
  duration = GetNative32BitIntFrom3ByteArray(pEntry->BillingPeriod.BillingPeriodDuration);
  stopTime = startTime + (60 * duration);
  /* Check if the Price Event is completed */
  if(stopTime <= currentTime)
  {
    pEntry->EntryStatus = gPriceCompletedStatus_c; /* entry is not used anymore */
    /* Call the App to signal that a Price was completed; User should check EntryStatus */
    BeeAppUpdateDevice (0, gZclUI_BillingPeriodEvt_c, 0, 0, pEntry);
    pEntry->EntryStatus = 0x00;
    FLib_MemInPlaceCpy(pEntry, (pEntry+1), (gNumofClientBillingPeriods_c-1) * sizeof(publishBillingPeriodEntry_t));
    gaClientBillingPeriodTable[gNumofClientBillingPeriods_c-1].EntryStatus = 0x00;
  }
  else
  {
    if(startTime <= currentTime) /* check if the Price event have to be started or updated */
    {
      if((pEntry->EntryStatus == gPriceReceivedStatus_c)||
         (pEntry->EntryStatus == gPriceUpdateStatus_c))
      {
        pEntry->EntryStatus = gPriceStartedStatus_c;
        /* Call the App to signal that a BillingPeriod was started; User should check EntryStatus */
        BeeAppUpdateDevice (0, gZclUI_BillingPeriodEvt_c, 0, 0, pEntry);
      }
    }
  }

  if(currentTime < startTime)
    nextTime = startTime - currentTime;
  else
    if(currentTime < stopTime)
      nextTime = stopTime - currentTime;
  if (nextTime)
    ZbTMR_StartSecondTimer(gBillingPeriodClientTimerID,(uint16_t)nextTime, TimerClientBillingPeriodCallBack);
  
}
#endif

/******************************************************************************
* Get / Publish Consolodated Bill
******************************************************************************/

/******************************************************************************/
/* The Publish Consolidated Bill command is generated in response to receiving a Get COnsolidated Bill
command or when an update to the billing period information is available
from the commodity provider. */
#if gASL_ZclSE_12_Features_d
static zbStatus_t ZCL_SendPublishConsolidatedBill(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishConsolidatedBillEntry_t * pMsg) 
{
  zclPrice_PublishConsolidatedBillRsp_t *pRsp;    
  ZCLTime_t currentTime, elapsedTime;
  zbStatus_t status;
  
  pRsp = AF_MsgAlloc();
  
  if (!pRsp)
    return gZclNoMem_c;
  
  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  pRsp->zclTransactionId = tsq;
  
  FLib_MemCpy(&pRsp->cmdFrame, &pMsg->ConsolidatedBill, sizeof(zclCmdPrice_PublishConsolidatedBillRsp_t));
  
  currentTime = ZCL_GetUTCTime(pAddrInfo->srcEndPoint);
  
  if((pRsp->cmdFrame.BillingPeriodStartTime == 0x00000000) && 
     (!CmpToFs(pRsp->cmdFrame.BillingPeriodDuration, sizeof(Duration24_t))))
  {
    // get the duration value
    elapsedTime = (currentTime - OTA2Native32(pMsg->EffectiveStartTime)) / 60;
    Zcl_SubtractDuration(pRsp->cmdFrame.BillingPeriodDuration, elapsedTime);
  }
  
  status = zclPrice_PublishConsolidatedBillRsp(pRsp);
  
  MSG_Free(pRsp);
  
  return status;
}
#endif

#if gASL_ZclSE_12_Features_d 
/* Find an Event in the Events Table...  */
uint8_t FindNextConsolidatedBillEvent(void)
{
  uint8_t i; 
  
  for(i = mGetConsolidatedBill.index; i < gNumofServerConsolidatedBill_c; i++)
  {
    publishConsolidatedBillEntry_t *pEntry = &gaServerConsolidatedBillTable[i];
    
    // if exits this entry
    if(pEntry->EntryStatus != 0x00)  
    {      
      if((mGetConsolidatedBill.earliestStartTime <= pEntry->EffectiveStartTime) && 
         (mGetConsolidatedBill.numOfEvtsRemaining > 0) &&
         ((mGetConsolidatedBill.tariffType == pEntry->ConsolidatedBill.TariffType)) &&
         (mGetConsolidatedBill.minIssuerEvtId <= pEntry->ConsolidatedBill.IssuerEvtID))
      {
        return i;
      }
    }
  }
  return 0xFF; 
}   
#endif

#if gASL_ZclSE_12_Features_d 
static zbStatus_t ZCL_ProcessGetConsolidatedBillReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t tsq,
  zclCmdPrice_GetConsolidatedBillReq_t * pGetConsolidatedBill
)
{
  uint8_t entryIndex;
    
  /* Copy address information and TSQ*/
  FLib_MemCpy(&mGetConsolidatedBill.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  mGetConsolidatedBill.tsq = tsq;
  
  mGetConsolidatedBill.index = 0;
  
  /* Get the start time */ 
  mGetConsolidatedBill.earliestStartTime = OTA2Native32(pGetConsolidatedBill->EarliestStartTime);

  /* Get the minimum issuer id */ 
 if (pGetConsolidatedBill->MinIssuerEvtId == 0xFFFFFFFF)
    mGetConsolidatedBill.minIssuerEvtId = 0;
  else
    mGetConsolidatedBill.minIssuerEvtId = pGetConsolidatedBill->MinIssuerEvtId;

  /* Get number of events*/  
  if(pGetConsolidatedBill->NumOfCmds == 0)
    mGetConsolidatedBill.numOfEvtsRemaining = gNumofServerConsolidatedBill_c;
  else
    mGetConsolidatedBill.numOfEvtsRemaining = pGetConsolidatedBill->NumOfCmds;

  mGetConsolidatedBill.tariffType = pGetConsolidatedBill->TariffType;
  
  /* Find next valid entry */
  entryIndex = FindNextConsolidatedBillEvent();
  if(entryIndex != 0xFF)
  {
    mGetConsolidatedBill.index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetConsolidatedBill_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_PublishConsolidatedBillRsp_d
static void ZCL_HandleGetConsolidatedBill(void)
{
  uint8_t status;
  uint8_t entryIndex;
     
  status= ZCL_SendPublishConsolidatedBill(&mGetConsolidatedBill.addrInfo, mGetConsolidatedBill.tsq
       ,&gaServerConsolidatedBillTable[mGetConsolidatedBill.index]);
  
  if(status == gZclSuccess_c)
  {
    mGetConsolidatedBill.numOfEvtsRemaining = mGetConsolidatedBill.numOfEvtsRemaining - 1;
    mGetConsolidatedBill.index = mGetConsolidatedBill.index + 1;
    if (mGetConsolidatedBill.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      entryIndex = FindNextConsolidatedBillEvent();
      if(entryIndex != 0xFF)
      {
        mGetConsolidatedBill.index = entryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetConsolidatedBill_c);
      }
    }
  }
}
#endif

#if gASL_ZclSE_12_Features_d
static void ZCL_HandleConsolidatedBillUpdate(index_t iEntryIdx)
{
  afAddrInfo_t addrInfo;
  
  addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishConsolidatedBill(&addrInfo, ZbZclFoundationGlobals(gZclTransactionId)++, &gaServerConsolidatedBillTable[iEntryIdx]);
  return;
}
#endif 


#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Add Consolidated Bill in Table... so that to have the scheduled consolidated bill in asccendent order */
static uint8_t AddConsolidatedBillsInTable(publishConsolidatedBillEntry_t *pTable, uint8_t len, zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->BillingPeriodStartTime);
  
  if(msgStartTime == 0x000000)
  {
    msgStartTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  }
  
  /* keep the Consolidated Bill in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->ConsolidatedBill.BillingPeriodStartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishConsolidatedBillEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishConsolidatedBillRsp_t));
  
  if (pMsg->BillingPeriodStartTime == 0x00000000)
  {
    /*  here get the currentTime  */
    (pTable+poz)->EffectiveStartTime = Native2OTA32(ZCL_GetUTCTime(BeeAppDataInit(appEndPoint)));
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->BillingPeriodStartTime ;
  /* new billing period was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Consolidated Bill was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_ConsolidatedBillEvt_c, 0, 0, (pTable+poz));
  return poz;
}

/******************************************************************************/
/* The Consolidated Bill timer callback that keep track of current active consolidated bill */
static void TimerClientConsolidatedBillCallBack(zbTmrTimerID_t timerID)
{
  (void) timerID;
   SendPriceClusterEvt(gzclEvtHandleClientConsolidatedBill_c);
}

/******************************************************************************/
/* Process the received Publish Consolidated Bill */
static zbStatus_t ZCL_ProcessClientPublishConsolidatedBill(zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a Consolidated Bill Update */
  updateIndex = CheckForConsolidatedBillUpdate(pMsg, (publishConsolidatedBillEntry_t *)&gaClientConsolidatedBillTable[0], gNumofClientConsolidatedBill_c);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new Consolidated Bill information in the table */
    newEntry = AddConsolidatedBillsInTable((publishConsolidatedBillEntry_t *)&gaClientConsolidatedBillTable[0], gNumofClientConsolidatedBill_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      /* the price was updated */
      gaClientConsolidatedBillTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      /* Call the App to signal that a Consolidated Bill was updated; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_ConsolidatedBillEvt_c, 0, 0, &gaClientConsolidatedBillTable[updateIndex]);
      status = gZclSuccess_c;
    }
  }
  
  if(status == gZclSuccess_c)
    SendPriceClusterEvt(gzclEvtHandleClientConsolidatedBill_c);
  
  return gZclSuccess_c;
}
#endif

/******************************************************************************/
/*Check and Updated a Consolidated Bill.
When new Consolidated Bill information is provided that replaces older block period
information for the same time period, IssuerEvt field allows devices to determine which
information is newer. It is expected that the value contained in this field is a
unique number managed by upstream servers.
Thus, newer Consolidated Bill information will have a value in the Issuer Event ID field that
is larger than older pricing information.
*/
#if gASL_ZclSE_12_Features_d
static uint8_t CheckForConsolidatedBillUpdate(zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg, publishConsolidatedBillEntry_t *pTable, uint8_t len)
{
  uint8_t i;
  uint32_t msgIssuerEvt, entryIssuerEvt;
  
  msgIssuerEvt = OTA2Native32(pMsg->IssuerEvtID);

  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      entryIssuerEvt = FourBytesToUint32(&(pTable+i)->ConsolidatedBill.IssuerEvtID);
      entryIssuerEvt = OTA2Native32(entryIssuerEvt);
      if ((pTable+i)->ConsolidatedBill.BillingPeriodStartTime == pMsg->BillingPeriodStartTime &&
          FLib_MemCmp(&(pTable+i)->ConsolidatedBill.BillingPeriodDuration, &pMsg->BillingPeriodDuration, sizeof(Duration24_t)))
      {
        if(entryIssuerEvt < msgIssuerEvt)
        {
          FLib_MemCpy((pTable+i), pMsg, sizeof(zclCmdPrice_PublishConsolidatedBillRsp_t));
          return i;
        }
        else 
          return 0xFE; /* reject it */
      }
    }
  }
  return 0xff;
}
#endif

/******************************************************************************/
/* Handle the Client Consolidated Bill signalling when the current Billing starts, was updated or is completed */
#if gASL_ZclSE_12_Features_d
static void ZCL_HandleClientConsolidatedBill(void)
{
  uint32_t currentTime, startTime, nextTime = 0x00000000, stopTime;
  uint32_t duration;
  publishConsolidatedBillEntry_t *pEntry = &gaClientConsolidatedBillTable[0];
  
  /* the Billing Period table is kept in ascendent order; check if any Billing Period is scheduled*/
  if(pEntry->EntryStatus == 0x00)
  {
    ZbTMR_StopTimer(gConsolidatedBillClientTimerID);
    return;
  }
  
  /* Get the timing */
  currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  startTime = OTA2Native32(pEntry->EffectiveStartTime);
  duration = GetNative32BitIntFrom3ByteArray(pEntry->ConsolidatedBill.BillingPeriodDuration);
  stopTime = startTime + (60 * duration);
  /* Check if the Price Event is completed */
  if(stopTime <= currentTime)
  {
    pEntry->EntryStatus = gPriceCompletedStatus_c; /* entry is not used anymore */
    /* Call the App to signal that a Price was completed; User should check EntryStatus */
    BeeAppUpdateDevice (0, gZclUI_ConsolidatedBillEvt_c, 0, 0, pEntry);
    pEntry->EntryStatus = 0x00;
    FLib_MemInPlaceCpy(pEntry, (pEntry+1), (gNumofClientConsolidatedBill_c-1) * sizeof(publishConsolidatedBillEntry_t));
    gaClientConsolidatedBillTable[gNumofClientConsolidatedBill_c-1].EntryStatus = 0x00;
  }
  else
  {
    if(startTime <= currentTime) /* check if the Price event have to be started or updated */
    {
      if((pEntry->EntryStatus == gPriceReceivedStatus_c)||
         (pEntry->EntryStatus == gPriceUpdateStatus_c))
      {
        pEntry->EntryStatus = gPriceStartedStatus_c;
        /* Call the App to signal that a ConsolidatedBill was started; User should check EntryStatus */
        BeeAppUpdateDevice (0, gZclUI_ConsolidatedBillEvt_c, 0, 0, pEntry);
      }
    }
  }

  if(currentTime < startTime)
    nextTime = startTime - currentTime;
  else
    if(currentTime < stopTime)
      nextTime = stopTime - currentTime;
  if (nextTime)
    ZbTMR_StartSecondTimer(gConsolidatedBillClientTimerID,(uint16_t)nextTime, TimerClientConsolidatedBillCallBack);
  
}
#endif

/******************************************************************************
* Get / Publish Credit Payment
******************************************************************************/

/******************************************************************************/
/* The Publish Credit Payment command is generated in response to receiving a Get Credit Payment
command or when an update to the Credit Payment information is available
from the commodity provider. */
#if gASL_ZclSE_12_Features_d
static zbStatus_t ZCL_SendPublishCreditPayment(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishCreditPaymentEntry_t * pMsg) 
{
  zclPrice_PublishCreditPaymentRsp_t *pRsp;    
  zbStatus_t status;
  
  pRsp = AF_MsgAlloc();
  
  if (!pRsp)
    return gZclNoMem_c;
  
  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  pRsp->zclTransactionId = tsq;
  
  FLib_MemCpy(&pRsp->cmdFrame, &pMsg->CreditPayment, sizeof(zclCmdPrice_PublishCreditPaymentRsp_t));
  
  status = zclPrice_PublishCreditPaymentRsp(pRsp);
  
  MSG_Free(pRsp);
  
  return status;
}
#endif

#if gASL_ZclSE_12_Features_d
static void ZCL_HandleCreditPaymentUpdate(index_t iEntryIdx)
{
  afAddrInfo_t addrInfo;
  
  addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishCreditPayment(&addrInfo, ZbZclFoundationGlobals(gZclTransactionId)++, &gaServerCreditPaymentTable[iEntryIdx]);
  return;
}
#endif 


#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_Optionals_d
/******************************************************************************/
/* Add Credit Payment in Table... so that to have the scheduled Credit Payment in asccendent order */
static uint8_t AddCreditPaymentsInTable(publishCreditPaymentEntry_t *pTable, uint8_t len, zclCmdPrice_PublishCreditPaymentRsp_t *pMsg)
{
  ZCLTime_t dueTime, msgDueTime;
  uint8_t i, poz = 0xff;
  
  msgDueTime = OTA2Native32(pMsg->CreditPaymentDueDate);
    
  /* keep the Credit Payment in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != gEntryNotUsed_c)
    {
      dueTime = OTA2Native32((pTable+i)->CreditPayment.CreditPaymentDueDate);
     
      if(dueTime > msgDueTime) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishCreditPaymentEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishCreditPaymentRsp_t));

  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;

  /* Call the App to signal that a Credit Payment was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_CreditPaymentEvt_c, 0, 0, (pTable+poz));
  return poz;
}

/******************************************************************************/
/* Process the received Publish Credit Payment */
static zbStatus_t ZCL_ProcessClientPublishCreditPayment(zclCmdPrice_PublishCreditPaymentRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a Credit Payment Update */
  updateIndex = CheckForCreditPaymentUpdate(pMsg, (publishCreditPaymentEntry_t *)&gaClientCreditPaymentTable[0], gNumofClientCreditPayment_c);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new Credit Payment information in the table */
    newEntry = AddCreditPaymentsInTable((publishCreditPaymentEntry_t *)&gaClientCreditPaymentTable[0], gNumofClientCreditPayment_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      /* the price was updated */
      gaClientCreditPaymentTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      /* Call the App to signal that a Credit Payment was updated; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_CreditPaymentEvt_c, 0, 0, &gaClientCreditPaymentTable[updateIndex]);
      status = gZclSuccess_c;
    }
  }
  
//  if(status == gZclSuccess_c)
//    SendPriceClusterEvt(gzclEvtHandleClientCreditPayment_c);
//  
  return status;
}
#endif

/******************************************************************************/
/*Check and Updated a Credit Payment.
When new Credit Payment information is provided that replaces older credit payment
information for the same time period, IssuerEvt field allows devices to determine which
information is newer. It is expected that the value contained in this field is a
unique number managed by upstream servers.
Thus, newer Credit Payment information will have a value in the Issuer Event ID field that
is larger than older pricing information.
*/
#if gASL_ZclSE_12_Features_d
static uint8_t CheckForCreditPaymentUpdate(zclCmdPrice_PublishCreditPaymentRsp_t *pMsg, publishCreditPaymentEntry_t *pTable, uint8_t len)
{
  return 0xff;
}
#endif

/******************************************************************************
* Get / Publish Currency Conversion
******************************************************************************/

/******************************************************************************/
/* The Publish Currency Conversion command is generated in response to receiving a Get Currency Conversion
command or when an update to the Currency Conversion is available
from the commodity provider. */
#if gASL_ZclSE_12_Features_d
static zbStatus_t ZCL_SendPublishCurrencyConversion(afAddrInfo_t *pAddrInfo, zclTSQ_t tsq, publishCurrencyConversionEntry_t * pMsg) 
{
  zclPrice_PublishCurrencyConversionRsp_t *pRsp;
  zbStatus_t status;

  pRsp = AF_MsgAlloc();
  
  if (!pRsp)
    return gZclNoMem_c;  
    
  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  pRsp->zclTransactionId = tsq;
  FLib_MemCpy(&pRsp->cmdFrame, pMsg, sizeof(zclCmdPrice_PublishCurrencyConversionRsp_t));
  
  status = zclPrice_PublishCurrencyConversionRsp(pRsp);
  
  MSG_Free(pRsp);
  
  return status;
}
#endif

#if gASL_ZclSE_12_Features_d 
static zbStatus_t ZCL_ProcessGetCurrencyConversionReq
(
  afAddrInfo_t *pAddrInfo, 
  zclTSQ_t         tsq
)
{
  if(gServerCurrencyConversion.EntryStatus != gEntryNotUsed_c)
  {
    return ZCL_SendPublishCurrencyConversion(pAddrInfo, tsq, &gServerCurrencyConversion);
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Process the received Publish Currency Conversion */
static zbStatus_t ZCL_ProcessClientPublishCurrencyConversion(zclCmdPrice_PublishCurrencyConversionRsp_t *pMsg)
{
  return AddCurrencyConversionInTable(&gClientCurrencyConversion, 1, pMsg);
}
#endif

#if gASL_ZclSE_12_Features_d
static void ZCL_HandleCurrencyConversionUpdate(void)
{
  afAddrInfo_t addrInfo;
  
  addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
  Set2Bytes(&addrInfo.aClusterId, gZclClusterPrice_c);
  addrInfo.txOptions = afTxOptionsDefault_c;
  addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  (void)ZCL_SendPublishCurrencyConversion(&addrInfo, ZbZclFoundationGlobals(gZclTransactionId)++, &gServerCurrencyConversion);
  return;
}
#endif 

#if gASL_ZclSE_12_Features_d
/******************************************************************************/
/* Add Currency Conversion in Table... so that to have the scheduled price in ascendent order */
static uint8_t AddCurrencyConversionInTable(publishCurrencyConversionEntry_t *pTable, uint8_t len, zclCmdPrice_PublishCurrencyConversionRsp_t *pMsg)
{  
  /* Copy Currency Conversion*/
  FLib_MemCpy(pTable, pMsg, sizeof(zclCmdPrice_PublishCurrencyConversionRsp_t));

  if(pMsg->StartTime == 0x00000000)
  {
    (pTable)->EffectiveStartTime = Native2OTA32(ZCL_GetUTCTime(BeeAppDataInit(appEndPoint)));
  }
  else
    (pTable)->EffectiveStartTime = pMsg->StartTime;
  
  /* New Currency Conversion was received */	
  (pTable)->EntryStatus = gPriceReceivedStatus_c;
  
  /* Call the App to signal that a Currency Conversion was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_CurrencyConversionEvt_c, 0, 0, (pTable));
  return gZclSuccess_c;
}
#endif


/******************************************************************************
* Cancel Tariff
******************************************************************************/
#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_Optionals_d
static void DeleteClientTariffInformation(ProviderID_t providerID, SEEvtId_t issuerTariffID, uint8_t tariffType)
{
  publishTariffInformationEntry_t *pEntry;
  index_t i;
  
  for (i = 0; i< gNumofClientTariffInformation_c; i++)
  {
    pEntry = &gaClientTariffInformationTable[i];
    
    if (FLib_MemCmp(pEntry->TariffInformation.ProviderID, providerID, sizeof(ProviderID_t)) &&
        FLib_MemCmp(pEntry->TariffInformation.IssuerTariffID, issuerTariffID, sizeof(SEEvtId_t)) &&
        pEntry->TariffInformation.TariffType.tariffType == tariffType)
      break;
  }
  
  /* Found matching tariff. Delete linking information */
  DeleteClientBlockThresholds(providerID, issuerTariffID);
  DeleteClientPriceMatrix(providerID, issuerTariffID);
  DeleteClientTierLabels(providerID, issuerTariffID);
  
  /* Clear Information. If it is not the last entry, shift other events */
  if (i == gNumofClientTariffInformation_c)
    pEntry->EntryStatus = gEntryNotUsed_c;
  
  while(i < gNumofClientTariffInformation_c - 1)
  {    
    FLib_MemInPlaceCpy(pEntry, pEntry + 1,  sizeof(publishTariffInformationEntry_t));
    pEntry++;
    i++;
  }
}
                           
static void DeleteClientBlockThresholds(ProviderID_t providerID, SEEvtId_t issuerTariffID)
{
  publishBlockThresholdsEntry_t *pEntry;
  index_t i;
  
  for (i = 0; i< gNumofClientBlockThresholds_c; i++)
  {
    pEntry = &gaClientBlockThresholdsTable[i];
    
    if (FLib_MemCmp(pEntry->ProviderID, providerID, sizeof(ProviderID_t)) &&
        FLib_MemCmp(pEntry->IssuerTariffID, issuerTariffID, sizeof(SEEvtId_t)))
      break;
  }  
  
  /* Clear Information. If it is not the last entry, shift other events */
  if (i == gNumofClientBlockThresholds_c)
    pEntry->EntryStatus = gEntryNotUsed_c;
  
  while(i < gNumofClientBlockThresholds_c - 1)
  {    
    FLib_MemInPlaceCpy(pEntry, pEntry + 1,  sizeof(publishBlockThresholdsEntry_t));
    pEntry++;
    i++;
  }
} 

static void DeleteClientPriceMatrix(ProviderID_t providerID, SEEvtId_t issuerTariffID)
{
  publishPriceMatrixEntry_t *pEntry;
  index_t i;
  
  for (i = 0; i< gNumofClientPriceMatrix_c; i++)
  {
    pEntry = &gaClientPriceMatrixTable[i];
    
    if (FLib_MemCmp(pEntry->ProviderID, providerID, sizeof(ProviderID_t)) &&
        FLib_MemCmp(pEntry->IssuerTariffID, issuerTariffID, sizeof(SEEvtId_t)))
      break;
  }  

  /* Clear Information. If it is not the last entry, shift other events */
  if (i == gNumofClientPriceMatrix_c)
    pEntry->EntryStatus = gEntryNotUsed_c;

#if (gNumofClientPriceMatrix_c > 1) 
  while(i < gNumofClientPriceMatrix_c - 1)
  {    
    FLib_MemInPlaceCpy(pEntry, pEntry + 1,  sizeof(publishPriceMatrixEntry_t) - sizeof(TierBlockPrice_t)*gASL_ZclSE_TiersNumber_d*gASL_ZclSE_BlocksNumber_d + (pEntry+1)->Length);
    pEntry++;
    i++;
  }
#endif  
} 

static void DeleteClientTierLabels(ProviderID_t providerID, SEEvtId_t issuerTariffID)
{
  publishTierLabelsEntry_t *pEntry;
  index_t i;
  
  for (i = 0; i< gNumofClientTierLabels_c; i++)
  {
    pEntry = &gaClientTierLabelsTable[i];
    
    if (FLib_MemCmp(pEntry->ProviderID, providerID, sizeof(ProviderID_t)) &&
        FLib_MemCmp(pEntry->IssuerTariffID, issuerTariffID, sizeof(SEEvtId_t)))
      break;
  }  

  /* Clear Information. If it is not the last entry, shift other events */
  if (i == gNumofClientTierLabels_c)
    pEntry->EntryStatus = gEntryNotUsed_c;
  
  while(i < gNumofClientTierLabels_c - 1)
  {    
    FLib_MemInPlaceCpy(pEntry, pEntry + 1,  sizeof(publishTierLabelsEntry_t) - gASL_ZclSE_TiersNumber_d* sizeof(zclTierEntry_t)+ (pEntry+1)->Length);
    pEntry++;
    i++;
  }
} 
#endif
