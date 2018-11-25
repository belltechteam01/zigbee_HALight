/*! @file 	ZclSEPrice.h
 *
 * @brief	Types, definitions and prototypes for the Price cluster implementation.
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
#ifndef _ZCLSEPRICE_H
#define _ZCLSEPRICE_H

#ifdef __cplusplus
    extern "C" {
#endif

/* header files needed by home automation */
#include "EmbeddedTypes.h"
#include "AfApsInterface.h"
#include "ZCL.h"
#include "SEProfile.h"
/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		zbStatus_t ZCL_PriceClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Price Cluster server. 
 *
 */
zbStatus_t ZCL_PriceClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZCL_PriceClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Price Cluster client. 
 *
 */
zbStatus_t ZCL_PriceClusterClient(zbApsdeDataIndication_t *pIndication,	afDeviceDef_t *pDev );

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
); 

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
);

/*!
 * @fn 		zbStatus_t ZCL_PriceAck(zclPrice_PriceAck_t *pReq)
 *
 * @brief	Sends over-the-air a Price Acknowledgement frame from the client side. 
 *
 */
zbStatus_t ZCL_PriceAck(zclPrice_PriceAck_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_InterPriceAck(zclPrice_InterPriceAck_t *pReq)
 *
 * @brief	Sends over-the-air a Price Acknowledgement frame from the InterPAN client side. 
 *
 */
zbStatus_t ZCL_InterPriceAck(zclPrice_InterPriceAck_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetCurrPriceReq(zclPrice_GetCurrPriceReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Current Price frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetCurrPriceReq(zclPrice_GetCurrPriceReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetScheduledPricesReq(zclPrice_GetScheduledPricesReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Scheduled Prices frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetScheduledPricesReq(zclPrice_GetScheduledPricesReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetBlockPeriodsReq(zclPrice_GetBlockPeriodsReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Block Periods frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetBlockPeriodsReq(zclPrice_GetBlockPeriodsReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetCalorificValueReq(zclPrice_GetCalorificValueReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Calorific Value frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetCalorificValueReq(zclPrice_GetCalorificValueReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetConversionFactorReq(zclPrice_GetConversionFactorReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Conversion Factor frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetConversionFactorReq(zclPrice_GetConversionFactorReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetBillingPeriodReq(zclPrice_GetBillingPeriodReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Billing Period frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetBillingPeriodReq(zclPrice_GetBillingPeriodReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetPriceMatrixReq(zclPrice_GetPriceMatrixReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Price Matrix frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetPriceMatrixReq(zclPrice_GetPriceMatrixReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetBlockThresholdsReq(zclPrice_GetBlockThresholdsReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Block Threshold frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetBlockThresholdsReq(zclPrice_GetBlockThresholdsReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetTierLabelsReq(zclPrice_GetTierLabelsReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Tier Labels frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetTierLabelsReq(zclPrice_GetTierLabelsReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetCO2ValueReq(zclPrice_GetCO2ValueReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get CO2 Value frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetCO2ValueReq(zclPrice_GetCO2ValueReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetTariffInformationReq(zclPrice_GetTariffInformationReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Tariff Information frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetTariffInformationReq(zclPrice_GetTariffInformationReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetConsolidatedBillReq(zclPrice_GetConsolidatedBillReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Consolidated Bill frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetConsolidatedBillReq(zclPrice_GetConsolidatedBillReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_CPPEventRsp(zclPrice_CPPEventRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Get CPP Event Response frame from the client side. 
 *
 */
zbStatus_t zclPrice_CPPEventRsp(zclPrice_CPPEventRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_GetCurrencyConversionReq(zclPrice_GetCurrencyConversionReq *pReq)
 *
 * @brief	Sends over-the-air a Get Currency Conversion frame from the client side. 
 *
 */
zbStatus_t zclPrice_GetCurrencyConversionReq(zclPrice_GetCurrencyConversionReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishPriceRsp(zclPrice_PublishPriceRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Price frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishPriceRsp(zclPrice_PublishPriceRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishExtendedPriceRsp(zclPrice_PublishPriceRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Price frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishExtendedPriceRsp(zclPrice_PublishPriceRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishBlockPeriodRsp(zclPrice_PublishBlockPeriodRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Block Period frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishBlockPeriodRsp(zclPrice_PublishBlockPeriodRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishConversionFactorRsp(zclPrice_PublishConversionFactorRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Conversion Factor frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishConversionFactorRsp(zclPrice_PublishConversionFactorRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishCalorificValueRsp(zclPrice_PublishCalorificValueRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Calorific Value frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishCalorificValueRsp(zclPrice_PublishCalorificValueRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_CancelTariffReq(zclPrice_CancelTariffReq_t *pReq)
 *
 * @brief	Sends over-the-air a Cancel Tariff frame from the server side. 
 *
 */
zbStatus_t zclPrice_CancelTariffReq(zclPrice_CancelTariffReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishBillingPeriodRsp(zclPrice_PublishBillingPeriodRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Billing Period frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishBillingPeriodRsp(zclPrice_PublishBillingPeriodRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishPriceMatrixRsp(zclPrice_PublishPriceMatrixRsp *pReq, uint8_t length)
 *
 * @brief	Sends over-the-air a Publish Price Matrix frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishPriceMatrixRsp(zclPrice_PublishPriceMatrixRsp_t *pReq, uint8_t length);

/*!
 * @fn 		zbStatus_t zclPrice_PublishBlockThresholdsRsp(zclPrice_PublishBlockThresholdsRsp_t *pReq, 
 *											uint8_t numOfTiers, uint8_t numOfBlockThresholdsPerTier)
 *
 * @brief	Sends over-the-air a Publish Block Thresholds frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishBlockThresholdsRsp(zclPrice_PublishBlockThresholdsRsp_t *pReq, uint8_t numOfTiers, uint8_t numOfBlockThresholdsPerTier);

/*!
 * @fn 		zbStatus_t zclPrice_PublishCO2ValueRsp(zclPrice_PublishCO2ValueRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish CO2 Value frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishCO2ValueRsp(zclPrice_PublishCO2ValueRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishTariffInformationRsp(zclPrice_PublishTariffInformationRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Tariff Information frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishTariffInformationRsp(zclPrice_PublishTariffInformationRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishConsolidatedBillRsp(zclPrice_PublishConsolidatedBillRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Consolidated Bill frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishConsolidatedBillRsp(zclPrice_PublishConsolidatedBillRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishCPPEventRsp(zclPrice_PublishCPPEventRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish CPP Event frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishCPPEventRsp(zclPrice_PublishCPPEventRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishTierLabelsRsp(zclPrice_PublishTierLabelsRsp_t *pReq, uint8_t length)
 *
 * @brief	Sends over-the-air a Publish Tier Labels frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishTierLabelsRsp(zclPrice_PublishTierLabelsRsp_t *pReq, uint8_t length);

/*!
 * @fn 		zbStatus_t zclPrice_PublishCurrencyConversionRsp(zclPrice_PublishCurrencyConversionRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Currency Conversion frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishCurrencyConversionRsp(zclPrice_PublishCurrencyConversionRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_PublishCreditPaymentRsp(zclPrice_PublishCreditPaymentRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Publish Credit Prepayment frame from the server side. 
 *
 */
zbStatus_t zclPrice_PublishCreditPaymentRsp(zclPrice_PublishCreditPaymentRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_InterPanGetCurrPriceReq(zclPrice_InterPanGetCurrPriceReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Current Price frame from the InterPAN client side. 
 *
 */
zbStatus_t zclPrice_InterPanGetCurrPriceReq(zclPrice_InterPanGetCurrPriceReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_InterPanGetScheduledPricesReq(zclPrice_InterPanGetScheduledPricesReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Scheduled Prices frame from the InterPAN client side. 
 *
 */
zbStatus_t zclPrice_InterPanGetScheduledPricesReq(zclPrice_InterPanGetScheduledPricesReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrice_InterPanPublishPriceRsp(zclPrice_InterPanPublishPriceRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Get Scheduled Prices frame from the InterPAN server side. 
 *
 */
zbStatus_t zclPrice_InterPanPublishPriceRsp(zclPrice_InterPanPublishPriceRsp_t *pReq);



#if gASL_ZclPrice_Optionals_d
/*!
 * @fn 		void ZCL_DeleteScheduleServerBlockPeriods(void)
 *
 * @brief	Deletes all Block Periods stored on the server.
 *
 *
 */
void ZCL_DeleteScheduleServerBlockPeriods(void);

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerBlockPeriodsEvents ( ztcCmdPrice_PublishBlockPeriodRsp_t *pMsg)
 *
 * @brief	Store Block Periods information received from the Provider 
 * 			The server side doesn't keep track of the Block Periods status, only stores the 
 * 			received Block Periods and take care that Nested and overlapping Block Periods commands not to occur
 *
 */
zbStatus_t ZCL_ScheduleServerBlockPeriodsEvents(ztcCmdPrice_PublishBlockPeriodRsp_t *pMsg);

/*!
 * @fn 		zbStatus_t ZCL_UpdateServerBlockPeriodEvents(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg)
 *
 * @brief	Updates on server Block Period information from the Provider.
 *
 *
 */
zbStatus_t ZCL_UpdateServerBlockPeriodEvents(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg);
#endif

#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_12_Features_d

/*!
 * @fn 		void ZCL_DeleteServerCalorificValueStore(void)
 *
 * @brief	Deletes all Calorific Values on server.
 *
 *
 */
void ZCL_DeleteServerCalorificValueStore(void);

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerCalorificValueStore ( ztcCmdPrice_PublishCalorificValueRsp_t *pMsg)
 *
 * @brief	Stores on server Calorific Values from the Provider.
 *
 *
 */
zbStatus_t ZCL_ScheduleServerCalorificValueStore ( ztcCmdPrice_PublishCalorificValueRsp_t *pMsg);

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerConversionFactorStore ( ztcCmdPrice_PublishConversionFactorRsp_t *pMsg)
 *
 * @brief	Stores on server Conversion Factor information from the Provider.
 *
 *
 */
zbStatus_t ZCL_ScheduleServerConversionFactorStore ( ztcCmdPrice_PublishConversionFactorRsp_t *pMsg);
#endif
#if gASL_ZclSE_12_Features_d 

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerTariffInformationEvents ( ztcCmdPrice_PublishTariffInformationRsp_t *pMsg)
 *
 * @brief	Stores on server the  Tariff Information received from the Provider.
 *
 *
 */
zbStatus_t ZCL_ScheduleServerTariffInformationEvents ( ztcCmdPrice_PublishTariffInformationRsp_t *pMsg);

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerBillingPeriodEvents ( ztcCmdPrice_PublishBillingPeriodRsp_t *pMsg)
 *
 * @brief	Store Billing Periods information received from the Provider 
 * 			The server side doesn't keep track of the Billing Periods status, only stores the 
 * 			received Billing Periods and take care that Nested and overlapping Billing Periods commands not to occur
 *
 */	 
zbStatus_t ZCL_ScheduleServerBillingPeriodEvents(ztcCmdPrice_PublishBillingPeriodRsp_t *pMsg);

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerCO2ValueEvents ( ztcCmdPrice_PublishCO2ValueRsp_t *pMsg)
 *
 * @brief	Stores on server the CO2 Value Event received from the Provider.
 *
 *
 */
zbStatus_t ZCL_ScheduleServerCO2ValueEvents(ztcCmdPrice_PublishCO2ValueRsp_t *pMsg);

/*!
 * @fn 		zbStatus_t ZCL_StoreServerPriceMatrix( ztcCmdPrice_PublishPriceMatrixRsp_t *pMsg)
 *
 * @brief	Store on server the Price Matrix received from the provider
 *
 */	 
zbStatus_t ZCL_StoreServerPriceMatrix(ztcCmdPrice_PublishPriceMatrixRsp_t *pMsg);

/*!
 * @fn 		zbStatus_t ZCL_StoreServerTierLabels( ztcCmdPrice_PublishTierLabelsRsp_t *pMsg)
 *
 * @brief	Store on server the Tier Labels received from the provider
 *
 */
zbStatus_t ZCL_StoreServerTierLabels(ztcCmdPrice_PublishTierLabelsRsp_t *pMsg);
/*!
 * @fn 		zbStatus_t ZCL_StoreServerBlockThresholds( ztcCmdPrice_PublishBlockThresholdsRsp_t *pMsg)
 *
 * @brief	Store on server the block thresholds received from the Provider
 *
 */
zbStatus_t ZCL_StoreServerBlockThresholds(ztcCmdPrice_PublishBlockThresholdsRsp_t *pMsg);

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerConsolidatedBillEvents ( ztcCmdPrice_PublishConsolidatedBillRsp_t *pMsg)
 *
 * @brief	Stores on server Consolidated Bill information received from the Provider .
 *          The server side doesn't keep track of the Consolidated Bill status, only stores the 
 *          received Consolidated Bill and take care that Nested and overlapping Consolidated Bill commands not to occur.
 */
zbStatus_t ZCL_ScheduleServerConsolidatedBillEvents(ztcCmdPrice_PublishConsolidatedBillRsp_t *pMsg);

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerCPPEvents(zclCmdPrice_PublishCPPEventRsp_t *pMsg)
 *
 * @brief	Stores on server the CPP Event received from the Provider.
 *
 *
 */
zbStatus_t ZCL_ScheduleServerCPPEvents(zclCmdPrice_PublishCPPEventRsp_t *pMsg);

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerCurrencyConversion (ztcCmdPrice_PublishCurrencyConversionRsp_t *pMsg)
 *
 * @brief	Stores on server Currency Conversion information from the Provider.
 *
 */
zbStatus_t ZCL_ScheduleServerCurrencyConversion(ztcCmdPrice_PublishCurrencyConversionRsp_t *pMsg);

/*!
 * @fn 		zbStatus_t ZCL_StoreServerCreditPayment (ztcCmdPrice_PublishCreditPaymentRsp_t *pMsg)
 *
 * @brief	Stores on server Credit Payment information from the Provider.
 *
 */
zbStatus_t ZCL_StoreServerCreditPayment(ztcCmdPrice_PublishCreditPaymentRsp_t *pMsg);
#endif

/*!
 * @fn 		void ZCL_HandleSEPriceClusterEvt(void)
 *
 * @brief	Handle the Price Cluster Event.
 *
 */
void ZCL_HandleSEPriceClusterEvt(void);

/*!
 * @fn 		void SendPriceClusterEvt(uint32_t evtId)
 *
 * @brief	Send the Price Cluster Event to the TS.
 *
 */
void SendPriceClusterEvt(uint32_t evtId);

/*!
 * @fn 		void ZCL_PriceClientInit(void)
 *
 * @brief	Initializes the Price Client functionality
 *
 */
void ZCL_PriceClientInit(void);

/*!
 * @fn 		void ZCL_PriceServerInit(void)
 *
 * @brief	Initializes the Price Server functionality
 *
 */
void ZCL_PriceServerInit(void);

/*!
 * @fn 		void ZCL_DeleteClientPrice(uint8_t *pEvtId)
 *
 * @brief	Deletes a price on the client, given an event id.
 *
 */
void ZCL_DeleteClientPrice(uint8_t *pEvtId);

/*!
 * @fn 		zbStatus_t ZCL_ScheduleServerPriceEvents ( ztcCmdPrice_PublishPriceRsp_t *pMsg, bool_t IsExtended)
 *
 * @brief	Store Price information received from the Provider 
 * 			The server side doesn't keep track of the price status, only stores the
 * 			received prices and take care that Nested and overlapping Publish Price commands not to occur.
 *
 */	
zbStatus_t ZCL_ScheduleServerPriceEvents ( ztcCmdPrice_PublishPriceRsp_t *pMsg, bool_t IsExtended);

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
zbStatus_t ZCL_UpdateServerPriceEvents(zclCmdPrice_PublishPriceRsp_t *pMsg);

/*!
 * @fn 		void ZCL_DeleteServerScheduledPrices(void)
 *
 * @brief	Deletes all prices on the server.
 *
 */
void ZCL_DeleteServerScheduledPrices(void);

/*!
 * @fn 		void ZCL_DeleteServerConversionFactor(void)
 *
 * @brief	Deletes all Conversion Factor information on server.
 *
 */
void ZCL_DeleteServerConversionFactor(void);

extern publishPriceEntry_t gaClientPriceTable[gNumofClientPrices_c];
extern publishPriceEntry_t gaServerPriceTable[gNumofServerPrices_c];
extern publishCPPEventEntry_t gaClientCPPEventTable[gNumofClientCPPEvents_c];
#ifdef __cplusplus
}
#endif
#endif /* _ZCLSEPRICE_H */
