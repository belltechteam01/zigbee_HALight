/*! @file	ZclSEPrepayment.c
 *
 * @brief	This source file describes specific functionality implemented
 *			for the Prepayment cluster.
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
#include "AppAfInterface.h"
#include "TMR_Interface.h"
#include "SEProfile.h"
#include "ZCL.h"
#include "zclSE.h"
#include "ZclSEPrepayment.h"
#include "ZclSECalendar.h"


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
#if gASL_ZclPrepayment_SupplyOptionals_d  
  static zbStatus_t ZCL_SelectAvailEmergCreditReq(zbApsdeDataIndication_t *pIndication);
#endif
#if !gASL_ZclSE_12_Features_d && gASL_ZclPrepayment_SupplyOptionals_d
  static zbStatus_t ZCL_ProcessChangeSupplyReq(zbApsdeDataIndication_t *pIndication);
#endif  

#if gASL_ZclSE_12_Features_d
  static zbStatus_t ZCL_ProcessChangeDebtReq(zbApsdeDataIndication_t *pIndication);
  static zbStatus_t ZCL_ProcessEmergencyCredtitSetupReq(zbApsdeDataIndication_t *pIndication);
  static zbStatus_t ZCL_ProcessConsumerTopUpReq(zbApsdeDataIndication_t *pIndication);
  static zbStatus_t ZCL_ProcessCreditAdjustmentReq(zbApsdeDataIndication_t *pIndication);
  static zbStatus_t ZCL_ProcessChangePaymentModeReq(zbApsdeDataIndication_t *pIndication);
  static zbStatus_t ZCL_ProcessGetPrepaySnapshotReq(zbApsdeDataIndication_t *pIndication);
  static zbStatus_t ZCL_ProcessGetTopUpLogReq(zbApsdeDataIndication_t *pIndication);
  static zbStatus_t ZCL_ProcessSetLowCreditWarningLevelReq(zbApsdeDataIndication_t *pIndication);
  static zbStatus_t ZCL_ProcessGetDebtRepaymentLogReq(zbApsdeDataIndication_t *pIndication);
  static bool_t ZCL_CheckFrendlyCreditCalendar(zclCmdPrepayment_ChangePaymentModeRes_t* pChangePaymentMode);
#else
#if gASL_ZclPrepayment_SupplyOptionals_d   
  static zbStatus_t zclPrepayment_Server_SupplyStatusRsp(zclPrepayment_SupplyStatRsp_t *pReq);
  static zbStatus_t ZCL_SendSupplyStatusRsp(zbApsdeDataIndication_t *pIndication);
#endif  
  void PrepaymentTimerCallBack(zbTmrTimerID_t tmrID);  
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
zbTmrTimerID_t gPrepaymentTimerID;
uint8_t mProposedSupplyStatus;

/************** PREPAYMENT CLUSTER  ******************************************/

const zclAttrDef_t gaZclPrepaymentInfoSetAttrDef[] = {
    {gZclAttrIdPrepaymentPaymentControl_c,             gZclDataTypeBitmap16_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint16_t), (void*) MbrOfs(zclPrepaymentAttrRAM_t, paymentCtrl)}
#if gASL_ZclPrepayment_Optionals_d                            
    ,{gZclAttrIdPrepaymentCreditRemaining_c,          gZclDataTypeInt32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t), (void*) MbrOfs(zclPrepaymentAttrRAM_t, creditRemaining)}
    ,{gZclAttrIdPrepaymentEmergencyCreditRemaining_c, gZclDataTypeInt32_c,      gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t), (void*) MbrOfs(zclPrepaymentAttrRAM_t, emergencyCreditRemaining)}
    ,{gZclAttrIdPrepaymentCreditStatus_c,             gZclDataTypeBitmap8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, creditStatus)}
    ,{gZclAttrIdPrepaymentCreditRemainingTimeStamp_c, gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(ZCLTime_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, creditRemainingTimeStamp)}
    ,{gZclAttrIdPrepaymentAccumulatedDebt_c,          gZclDataTypeInt32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, accumulatedDebt)}
    ,{gZclAttrIdPrepaymentOverallDebtCap_c,           gZclDataTypeInt32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, overallDebtCap)}
    ,{gZclAttrIdPrepaymentEmergencyCreditLimitAllowance_c,  gZclDataTypeUint32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),       (void*) MbrOfs(zclPrepaymentAttrRAM_t, emergencyCreditLimitAllowance)}
    ,{gZclAttrIdPrepaymentEmergencyCreditThreshold_c,       gZclDataTypeInt32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),       (void*) MbrOfs(zclPrepaymentAttrRAM_t, emergencyCreditThreshold)}
    ,{gZclAttrIdPrepaymentTotalCreditAdded_c,               gZclDataTypeUint48_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint48_t),      (void*) MbrOfs(zclPrepaymentAttrRAM_t, totalCreditAdded)}
    ,{gZclAttrIdPrepaymentMaxCreditLimit_c,                 gZclDataTypeUint32_c,	gZclAttrFlagsInRAM_c,	sizeof(uint32_t),      (void*) MbrOfs(zclPrepaymentAttrRAM_t, maxCreditLimit)}
    ,{gZclAttrIdPrepaymentFriendlyCreditWarning_c,          gZclDataTypeUint8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),       (void*) MbrOfs(zclPrepaymentAttrRAM_t, friendlyCreditWarning)}
    ,{gZclAttrIdPrepaymentLowCreditWarning_c,               gZclDataTypeUint32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),       (void*) MbrOfs(zclPrepaymentAttrRAM_t, lowCreditWarning)}
    ,{gZclAttrIdPrepaymentIHDLowCreditWarning_c,            gZclDataTypeUint32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),       (void*) MbrOfs(zclPrepaymentAttrRAM_t, IHDLowCreditWarning)}
    ,{gZclAttrIdPrepaymentInterruptSuspendTime_c,           gZclDataTypeUint8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),       (void*) MbrOfs(zclPrepaymentAttrRAM_t, intSuspendTime)}
    ,{gZclAttrIdPrepaymentCutOffValue_c,                    gZclDataTypeInt32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),      (void*) MbrOfs(zclPrepaymentAttrRAM_t, cutOffValue)}
    ,{gZclAttrIdPrepaymentPaymentCardID_c,                  gZclDataTypeOctetStr_c,	gZclAttrFlagsInRAM_c,	sizeof(zclStr20_t), (void*) MbrOfs(zclPrepaymentAttrRAM_t, paymentCardID)}
#endif
    
};
  
const zclAttrDef_t gaZclPrepaymentTopUpSetAttrDef[] = {
    {gZclAttrIdPrepaymentTopUpDateTime1_c,	gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(ZCLTime_t),	      (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpTime1)},
    {gZclAttrIdPrepaymentTopUpAmount1_c,	gZclDataTypeUint48_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(sePrepaymentTopUp_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpAmount1)},
    {gZclAttrIdPrepaymentOriginatingDevice1_c,	gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),	      (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpOriginatingDevice1)},
    {gZclAttrIdPrepaymentTopUpCode1_c,	        gZclDataTypeOctetStr_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(zclStr25_t),           (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpCode1)},
    {gZclAttrIdPrepaymentTopUpDateTime2_c,	gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(ZCLTime_t),	      (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpTime2)},
    {gZclAttrIdPrepaymentTopUpAmount2_c,	gZclDataTypeUint48_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(sePrepaymentTopUp_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpAmount2)},
    {gZclAttrIdPrepaymentOriginatingDevice2_c,	gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),	      (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpOriginatingDevice2)},
    {gZclAttrIdPrepaymentTopUpCode2_c,	        gZclDataTypeOctetStr_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(zclStr25_t),            (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpCode2)},
    {gZclAttrIdPrepaymentTopUpDateTime3_c,	gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(ZCLTime_t),	      (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpTime3)},
    {gZclAttrIdPrepaymentTopUpAmount3_c,	gZclDataTypeUint48_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(sePrepaymentTopUp_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpAmount3)},
    {gZclAttrIdPrepaymentOriginatingDevice3_c,	gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),	      (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpOriginatingDevice3)},
    {gZclAttrIdPrepaymentTopUpCode3_c,	        gZclDataTypeOctetStr_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(zclStr25_t),            (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpCode3)},
    {gZclAttrIdPrepaymentTopUpDateTime4_c,	gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(ZCLTime_t),	      (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpTime4)},
    {gZclAttrIdPrepaymentTopUpAmount4_c,	gZclDataTypeUint48_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(sePrepaymentTopUp_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpAmount4)},
    {gZclAttrIdPrepaymentOriginatingDevice4_c,	gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),	      (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpOriginatingDevice4)},
    {gZclAttrIdPrepaymentTopUpCode4_c,	        gZclDataTypeOctetStr_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(zclStr25_t),            (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpCode4)},
    {gZclAttrIdPrepaymentTopUpDateTime5_c,	gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(ZCLTime_t),	      (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpTime5)},
    {gZclAttrIdPrepaymentTopUpAmount5_c,	gZclDataTypeUint48_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(sePrepaymentTopUp_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpAmount5)},
    {gZclAttrIdPrepaymentOriginatingDevice5_c,	gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),	      (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpOriginatingDevice5)},
    {gZclAttrIdPrepaymentTopUpCode5_c,	        gZclDataTypeOctetStr_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(zclStr25_t),            (void*) MbrOfs(zclPrepaymentAttrRAM_t, topUpCode5)},
};

const zclAttrDef_t gaZclPrepaymentDebtSetAttrDef[] = {
  {gZclAttrIdPrepaymentDebtLabel1_c,     	      gZclDataTypeOctetStr_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(zclStr12_t), (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtLabel1)},
  {gZclAttrIdPrepaymentDebtAmount1_c,                 gZclDataTypeUint32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),   (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtAmount1)},
  {gZclAttrIdPrepaymentDebtRecoveryMethod1_c,         gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryMethod1)},
  {gZclAttrIdPrepaymentDebtRecoveryStartTime1_c,      gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(ZCLTime_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryStartTime1)},
  {gZclAttrIdPrepaymentDebtRecoveryCollectionTime1_c, gZclDataTypeUint16_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),   (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryCollectionTime1)},
  {gZclAttrIdPrepaymentDebtRecoveryFequency1_c,       gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryFequency1)},
  {gZclAttrIdPrepaymentDebtRecoveryAmount1_c,         gZclDataTypeUint32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryAmount1)},
  {gZclAttrIdPrepaymentDebtRecoveryVendPerCent1_c,    gZclDataTypeUint16_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint16_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryVendPerCent1)},
  {gZclAttrIdPrepaymentDebtRecoveryRateCap1_c,        gZclDataTypeUint8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryRateCap1)},
  {gZclAttrIdPrepaymentDebtLabel2_c,     	      gZclDataTypeOctetStr_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(zclStr12_t), (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtLabel2)},
  {gZclAttrIdPrepaymentDebtAmount2_c,                 gZclDataTypeUint32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),   (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtAmount2)},
  {gZclAttrIdPrepaymentDebtRecoveryMethod2_c,         gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryMethod2)},
  {gZclAttrIdPrepaymentDebtRecoveryStartTime2_c,      gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(ZCLTime_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryStartTime2)},
  {gZclAttrIdPrepaymentDebtRecoveryCollectionTime2_c, gZclDataTypeUint16_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),   (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryCollectionTime2)},
  {gZclAttrIdPrepaymentDebtRecoveryFequency2_c,       gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryFequency2)},
  {gZclAttrIdPrepaymentDebtRecoveryAmount2_c,         gZclDataTypeUint32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryAmount2)},
  {gZclAttrIdPrepaymentDebtRecoveryVendPerCent2_c,    gZclDataTypeUint16_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint16_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryVendPerCent2)},
  {gZclAttrIdPrepaymentDebtRecoveryRateCap2_c,        gZclDataTypeUint8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryRateCap2)},
  {gZclAttrIdPrepaymentDebtLabel3_c,     	      gZclDataTypeOctetStr_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(zclStr12_t), (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtLabel3)},
  {gZclAttrIdPrepaymentDebtAmount3_c,                 gZclDataTypeUint32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),   (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtAmount3)},
  {gZclAttrIdPrepaymentDebtRecoveryMethod3_c,         gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryMethod3)},
  {gZclAttrIdPrepaymentDebtRecoveryStartTime3_c,      gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(ZCLTime_t),  (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryStartTime3)},
  {gZclAttrIdPrepaymentDebtRecoveryCollectionTime3_c, gZclDataTypeUint16_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),   (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryCollectionTime3)},
  {gZclAttrIdPrepaymentDebtRecoveryFequency3_c,       gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryFequency3)},
  {gZclAttrIdPrepaymentDebtRecoveryAmount3_c,         gZclDataTypeUint32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryAmount3)},
  {gZclAttrIdPrepaymentDebtRecoveryVendPerCent3_c,    gZclDataTypeUint16_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint16_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryVendPerCent3)},
  {gZclAttrIdPrepaymentDebtRecoveryRateCap3_c,        gZclDataTypeUint8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),    (void*) MbrOfs(zclPrepaymentAttrRAM_t, debtRecoveryRateCap3)},
};

#if !gASL_ZclSE_12_Features_d
const zclAttrDef_t gaZclPrepaymentSupplySetAttrDef[] = {
  {gZclAttrIdPrepaymentProposedChangeProviderID_c,	gZclDataTypeUint32_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint32_t),	(void*) MbrOfs(zclPrepaymentAttrRAM_t, proposedChangeProviderId)},
  {gZclAttrIdPrepaymentProposedChangeImplTime_c,	gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(ZCLTime_t),	(void*) MbrOfs(zclPrepaymentAttrRAM_t, proposedChangeImplementationTime)},
  {gZclAttrIdPrepaymentProposedChangeSupplyStatus_c,	gZclDataTypeEnum8_c,	gZclAttrFlagsInRAMRdOnly_c,	sizeof(uint8_t),	(void*) MbrOfs(zclPrepaymentAttrRAM_t, proposedChangeSupplyStatus)},
  {gZclAttrIdPrepaymentDelayedSuplyIntValueRem_c,	gZclDataTypeUint16_c,	gZclAttrFlagsInRAM_c,	sizeof(uint16_t),	(void*) MbrOfs(zclPrepaymentAttrRAM_t, delayedSupplyIntValueRemaining)},
  {gZclAttrIdPrepaymentDelayedSupplyIntValType_c,	gZclDataTypeEnum8_c,	gZclAttrFlagsInRAM_c,	sizeof(uint8_t),	(void*) MbrOfs(zclPrepaymentAttrRAM_t, delayedSupplyIntValueType)}
};
#else
const zclAttrDef_t gaZclPrepaymentAlarmSetAttrDef[] = {
  {gZclAttrIdPrepaymentAlarmStatus_c,	        gZclDataTypeBitmap16_c,	gZclAttrFlagsInRAM_c,	sizeof(uint16_t),	(void*) MbrOfs(zclPrepaymentAttrRAM_t, prepaymentAlarmStatus)},
  {gZclAttrIdPrepaymentGenericAlarmMask_c,	gZclDataTypeBitmap16_c,	gZclAttrFlagsInRAM_c,	sizeof(uint16_t),	(void*) MbrOfs(zclPrepaymentAttrRAM_t, genericAlarmMask)},
  {gZclAttrIdPrepaymentPrepaySwitchAlarmMask_c,	gZclDataTypeBitmap16_c,	gZclAttrFlagsInRAM_c,	sizeof(uint16_t),	(void*) MbrOfs(zclPrepaymentAttrRAM_t, prepaySwitchAlarmMask)},
  {gZclAttrIdPrepaymentPrepayEventAlarmMask_c,	gZclDataTypeBitmap16_c,	gZclAttrFlagsInRAM_c,	sizeof(uint16_t),	(void*) MbrOfs(zclPrepaymentAttrRAM_t, prepayEventAlarmMask)}
};
#endif

const zclAttrSet_t gaZclPrepaymentAttrSet[] = {
  {gZclAttrPrepaymentInfoSet_c, (void *)&gaZclPrepaymentInfoSetAttrDef, NumberOfElements(gaZclPrepaymentInfoSetAttrDef)}
#if gASL_ZclPrepayment_TopUpOptionals_d || gASL_ZclSE_12_Features_d  
  ,{gZclAttrPrepaymentTopUpSet_c, (void *)&gaZclPrepaymentTopUpSetAttrDef, NumberOfElements(gaZclPrepaymentTopUpSetAttrDef)}
#endif  
#if gASL_ZclPrepayment_DebtOptionals_d || gASL_ZclSE_12_Features_d	
  ,{gZclAttrPrepaymentDebtSet_c, (void *)&gaZclPrepaymentDebtSetAttrDef, NumberOfElements(gaZclPrepaymentDebtSetAttrDef)}
#endif  
#if gASL_ZclPrepayment_SupplyOptionals_d && !gASL_ZclSE_12_Features_d 
  ,{gZclAttrPrepaymentSupplySet_c, (void *)&gaZclPrepaymentSupplySetAttrDef, NumberOfElements(gaZclPrepaymentSupplySetAttrDef)}
#elif gASL_ZclPrepayment_AlarmOptionals_d || gASL_ZclSE_12_Features_d
  ,{gZclAttrPrepaymentAlarmSet_c, (void *)&gaZclPrepaymentAlarmSetAttrDef, NumberOfElements(gaZclPrepaymentAlarmSetAttrDef)}
#endif
};

const zclAttrSetList_t gZclPrepaymentAttrSetList = {
  NumberOfElements(gaZclPrepaymentAttrSet),
  gaZclPrepaymentAttrSet
};

#if gASL_ZclPrepayment_SupplyOptionals_d
/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
/******************************************************************************
 * Name: ZCL_SelectAvailEmergCreditReq
 * Description: [R2]5.2.3.1
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_SelectAvailEmergCreditReq
(
zbApsdeDataIndication_t *pIndication
)
{
  zbStatus_t status;
  uint8_t creditStatus;
  afAddrInfo_t addrInfo;
  AF_PrepareForReply(&addrInfo, pIndication);
         
  /* Update the Credit Status Attribute*/
  status = ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrIdPrepaymentCreditStatus_c , gZclServerAttr_c, &creditStatus, NULL);
  if(gZclSuccess_c == status)
  {
    if (creditStatus & gSePrepaymentCreditStatusFlags_EmergencyAvailable)
    {
      creditStatus |= (gSePrepaymentCreditStatusFlags_EmergencyEnabled | gSePrepaymentCreditStatusFlags_EmergencySelected);
      return ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrIdPrepaymentCreditStatus_c , gZclServerAttr_c, &creditStatus);        
    }
  }
  return gZclSuccessDefaultRsp_c;
}
#endif

#if gASL_ZclSE_12_Features_d 
/******************************************************************************
 * Name: ZCL_ProcessChangeDebtReq
 * Description: [R2]5.2.3.2
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_ProcessChangeDebtReq
(
 zbApsdeDataIndication_t *pIndication
 )
{
  zbStatus_t status;
  afAddrInfo_t addrInfo;
  zclCmdPrepayment_ChangeDebtReq_t* pChangeDebtReq = (zclCmdPrepayment_ChangeDebtReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  AF_PrepareForReply(&addrInfo, pIndication);

  /* Check if the debt label must be same as cmd debt label or just replaced debt label with the new one */
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentDebtLabel1_c , gZclServerAttr_c, &pChangeDebtReq->debtLabel);
  if(gZclSuccess_c != status) return status; 
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentDebtAmount1_c , gZclServerAttr_c, &pChangeDebtReq->debtAmount); 
  if(gZclSuccess_c != status) return status;
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentDebtRecoveryMethod1_c , gZclServerAttr_c, &pChangeDebtReq->debtRecoveryMethod); 
  if(gZclSuccess_c != status) return status;
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentDebtRecoveryStartTime1_c , gZclServerAttr_c, &pChangeDebtReq->debtRecoveryStartTime); 
  if(gZclSuccess_c != status) return status;
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentDebtRecoveryCollectionTime1_c , gZclServerAttr_c, &pChangeDebtReq->debtRecoveryCollectionTime); 
  if(gZclSuccess_c != status) return status;
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentDebtRecoveryFequency1_c , gZclServerAttr_c, &pChangeDebtReq->debtRecoveryFrequency); 
  if(gZclSuccess_c != status) return status;
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentDebtRecoveryAmount1_c , gZclServerAttr_c, &pChangeDebtReq->debtRecoveryAmount); 
  if(gZclSuccess_c != status) return status;
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentDebtRecoveryBalancePercentage1_c , gZclServerAttr_c, &pChangeDebtReq->debtRecoveryBalancePercentage); 
  if(gZclSuccess_c != status) return status;
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentDebtRecoveryMaxMissed1 , gZclServerAttr_c, &pChangeDebtReq->debtRecoveryMaxMissed); 
  return status;
  
}
#endif

#if gASL_ZclSE_12_Features_d 
/******************************************************************************
 * Name: ZCL_ProcessEmergencyCredtitSetupReq
 * Description: [R2]5.2.3.3
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_ProcessEmergencyCredtitSetupReq
(
 zbApsdeDataIndication_t *pIndication
 )
{
  zbStatus_t status;
  afAddrInfo_t addrInfo;
  zclCmdPrepayment_EmergencyCreditSetupReq_t* pEmergencyCreditSetupReq 
    = (zclCmdPrepayment_EmergencyCreditSetupReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  AF_PrepareForReply(&addrInfo, pIndication);
  
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrIdPrepaymentEmergencyCreditRemaining_c , gZclServerAttr_c,
                            &pEmergencyCreditSetupReq->emergencyCreditLimit); 
  if(gZclSuccess_c != status) return status;
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrIdPrepaymentEmergencyCreditThreshold_c , gZclServerAttr_c,
                            &pEmergencyCreditSetupReq->emergencyCreditThreshold); 
  return status;

}
#endif

#if gASL_ZclSE_12_Features_d 
/******************************************************************************
 * Name: ZCL_ProcessConsumerTopUpReq
 * Description: [R2]5.2.3.4
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_ProcessConsumerTopUpReq
(
 zbApsdeDataIndication_t *pIndication
)
{
  
  zbStatus_t status;
  uint8_t IndexI,IndexY;
  uint8_t buffAttr[25];
  uint32_t currentTime;
  uint16_t baseAttributeID = gZclAttrPrepaymentTopUpDateTime4_c;
  zclFrame_t *pFrame=(zclFrame_t *)pIndication->pAsdu;
  afAddrInfo_t addrInfo;
  zclCmdPrepayment_ConsumerTopUpReq_t* pConsumerTopUpReq = 
            (zclCmdPrepayment_ConsumerTopUpReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  zclPrepayment_ConsumerTopUpRes_t* pConsumerTopUpRes;
  
  AF_PrepareForReply(&addrInfo, pIndication);
  /* Move Top-up attributes one Id position  - to make space for the new Id */
  for(IndexY = 0; IndexY < 4 ; IndexY++)
  { 
    for(IndexI = 0; IndexI < 4 ; IndexI++)
    {
      status = ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, baseAttributeID+IndexI, gZclServerAttr_c, buffAttr, NULL);
      if(gZclSuccess_c != status) return status;
      status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, baseAttributeID+0x10+IndexI, gZclServerAttr_c, buffAttr);
      if(gZclSuccess_c != status) return status;  
    }
    baseAttributeID -= 0x10;
  }
  currentTime = ZCL_GetUTCTime(addrInfo.srcEndPoint);
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId,gZclAttrPrepaymentTopUpDateTime1_c, gZclServerAttr_c, &currentTime);
  if(gZclSuccess_c != status) return status;
  /* Set TopUp Date amounnt */
  status = ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId,gZclAttrPrepaymentTotalCreditAdded_c, gZclServerAttr_c, buffAttr,NULL);
  if(gZclSuccess_c != status) return status;
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId,gZclAttrPrepaymentTopUpDateAmount1_c, gZclServerAttr_c, buffAttr);
  if(gZclSuccess_c != status) return status;
  
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId,gZclAttrPrepaymentTopUpOriginatingDevice1_c, gZclServerAttr_c,&pConsumerTopUpReq->originatingDevice);
  if(gZclSuccess_c != status) return status;
  
  status = ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId,gZclAttrPrepaymentTopUpCode1_c, gZclServerAttr_c,(void*)&pConsumerTopUpReq->topUpCode);
  if(gZclSuccess_c != status) return status;

  pConsumerTopUpRes = AF_MsgAlloc();
  if(!pConsumerTopUpRes)
    return gZclNoMem_c;
  
  FLib_MemCpy((void*)&pConsumerTopUpRes->addrInfo,(void*)&addrInfo,sizeof(afAddrInfo_t));
  pConsumerTopUpRes->zclTransactionId = pFrame->transactionId;  
  /* Prepare and send ConsumerTopUpResponse command */
  
  pConsumerTopUpRes->cmdFrame.resultType = gZclAccepted_c;
  
  switch (pConsumerTopUpReq->originatingDevice)
  {
    case gZclOrigESI_c:
      pConsumerTopUpRes->cmdFrame.sourceOfTopUp = gZclSourceESI_c;
      break;
    case gZclOrigMeter_c:
      pConsumerTopUpRes->cmdFrame.sourceOfTopUp = gZclSourceMeter_c;
      break;
    case gZclOrigIPD_c:
      pConsumerTopUpRes->cmdFrame.sourceOfTopUp = gZclSourceIPD_c;
      break;
    default:
      MSG_Free(pConsumerTopUpRes);
      return gZclInvalidField_c;   
  } 
  status = ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId,gZclAttrIdPrepaymentCreditRemaining_c,
                             gZclServerAttr_c, &pConsumerTopUpRes->cmdFrame.creditRemaining,NULL);
  if(gZclSuccess_c != status) return status;
  
  status = ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId,gZclAttrIdPrepaymentTotalCreditAdded_c, gZclServerAttr_c,
                            &pConsumerTopUpRes->cmdFrame.topUpValue,NULL);
  if(gZclSuccess_c != status) return status;  
  
  status = ZCL_SendServerRspSeqPassed(gZclCmdPrepayment_Server_ConsumerTopUpRsp_c,sizeof(zclCmdPrepayment_ConsumerTopUpRes_t),(zclGenericReq_t*) pConsumerTopUpRes);;  
  
  MSG_Free(pConsumerTopUpRes);
  
  return status;
}
#endif

#if gASL_ZclSE_12_Features_d 
/******************************************************************************
 * Name: ZCL_ProcessCreditAdjustmentReq
 * Description: [R2]5.2.3.5
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_ProcessCreditAdjustmentReq(
 zbApsdeDataIndication_t *pIndication
)
{
  zbStatus_t status=gZclSuccess_c;
  afAddrInfo_t addrInfo;
  zclCmdPrepayment_CreditAdjustmentReq_t* pCreditAdjReq = 
            (zclCmdPrepayment_CreditAdjustmentReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  
  AF_PrepareForReply(&addrInfo, pIndication);
  
  /* check the relation with the emergency credit !!!*/
  /* also credit remaining is uint32_and pCreditAdjReq->zclCreditAdjValue is an int48_t value */
  switch(pCreditAdjReq->zclCreditAdjType)
  {
    case gZclCreditIncremental_c:
      {
        uint32_t creditRemaining;
        uint64_t newCredit;
        status = ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId,gZclAttrIdPrepaymentCreditRemaining_c, gZclServerAttr_c,
                                  &creditRemaining,NULL);
        if(gZclSuccess_c != status) return status;
        FLib_MemCpy(&newCredit,pCreditAdjReq->zclCreditAdjValue,sizeof(pCreditAdjReq->zclCreditAdjValue));
        creditRemaining = creditRemaining+newCredit;
        return ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrIdPrepaymentCreditRemaining_c, gZclServerAttr_c,
                                  &creditRemaining);
      }
    case gZclCreditAbsolute_c:
        return ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrIdPrepaymentCreditRemaining_c, gZclServerAttr_c,
                                  pCreditAdjReq->zclCreditAdjValue);
     break;
    default: 
      return gZclInvalidField_c;
  }
}
#endif

#if gASL_ZclSE_12_Features_d 
/******************************************************************************
 * Name: ZCL_ProcessChangePaymentModeReq
 * Description: [R2]5.2.3.6
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_ProcessChangePaymentModeReq(
 zbApsdeDataIndication_t *pIndication
)
{
  zbStatus_t status=gZclSuccess_c;
  afAddrInfo_t addrInfo;
  zclFrame_t *pFrame=(zclFrame_t *)pIndication->pAsdu;
  zclCmdPrepayment_ChangePaymentModeReq_t* pChangePaymentModeReq = 
            (zclCmdPrepayment_ChangePaymentModeReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));

  zclPrepayment_ChangePaymentModeRes_t* pChangePayModeRes;  
  
   /* Call the App to signal that a Change Payment mode was received; User should check EntryStatus */
  BeeAppUpdateDevice(0, gZclUI_SEPrepayment_ChangePaymentModeReq_c, 0, 0, pChangePaymentModeReq);
  
  /* Prepare and Send ChangePaymentModeResponse */  
  pChangePayModeRes = AF_MsgAlloc();
  if(!pChangePayModeRes)
    return gZclNoMem_c;
  
  AF_PrepareForReply(&addrInfo, pIndication);
  
  FLib_MemCpy((void*)&pChangePayModeRes->addrInfo,(void*)&addrInfo,sizeof(afAddrInfo_t));
  pChangePayModeRes->zclTransactionId = pFrame->transactionId;  
 
  pChangePayModeRes->cmdFrame.frendlyCredit = 
               ZCL_CheckFrendlyCreditCalendar(&pChangePayModeRes->cmdFrame);

  status = ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId,gZclAttrPrepaymentEmergencyCreditRemaining_c, gZclServerAttr_c,
                            &pChangePayModeRes->cmdFrame.emergencyCreditLimit,NULL);
  if(gZclSuccess_c != status) return status;
  
  status = ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId,gZclAttrPrepaymentEmergencyCreditThreshold_c, gZclServerAttr_c,
                            &pChangePayModeRes->cmdFrame.emergencyCreditThreshold,NULL);
  if(gZclSuccess_c != status) return status;
 
  status = ZCL_SendServerRspSeqPassed(gZclCmdPrepayment_Server_ChangePaymentModeRsp_c,sizeof(zclCmdPrepayment_ChangePaymentModeRes_t),(zclGenericReq_t*)pChangePayModeRes);  
  
  MSG_Free(pChangePayModeRes);
  
  return status;
}
#endif

#if gASL_ZclSE_12_Features_d
/******************************************************************************
 * Name: ZCL_ProcessGetPrepaySnapshotReq
 * Description: [R2]5.2.3.7
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_ProcessGetPrepaySnapshotReq(
 zbApsdeDataIndication_t *pIndication
)
{
   zclCmdPrepayment_GetPrepaySnapshotReq_t* pReq = 
       (zclCmdPrepayment_GetPrepaySnapshotReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  
   /* Call the App to signal that a Get Prepay 1 Snapshot request was received*/
  BeeAppUpdateDevice(0, gZclUI_SEPrepayment_GetPrepaySnapshotReq_c, 0, 0, pReq);
  
  return gZclSuccess_c;
}
#endif

#if gASL_ZclSE_12_Features_d 
/******************************************************************************
 * Name: ZCL_ProcessGetTopUpLogReq
 * Description: [R2]5.2.3.8
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_ProcessGetTopUpLogReq(
 zbApsdeDataIndication_t *pIndication
)
{
  zclCmdPrepayment_GetTopUpLogReq_t* pReq = 
       (zclCmdPrepayment_GetTopUpLogReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  
   /* Call the App to signal that a Get Prepay 1 Snapshot request was received*/
  BeeAppUpdateDevice(0, gZclUI_SEPrepayment_GetTopUpLogReq_c, 0, 0, pReq);
  
  return gZclSuccess_c;
}
#endif

#if gASL_ZclSE_12_Features_d 
/******************************************************************************
 * Name: ZCL_ProcessSetLowCreditWarningLevelReq
 * Description: Process Client Set Low Credit Warning Level request
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_ProcessSetLowCreditWarningLevelReq(
 zbApsdeDataIndication_t *pIndication
)
{
  zclCmdPrepayment_SetLowCreditWarningLevelReq_t* pReq = 
       (zclCmdPrepayment_SetLowCreditWarningLevelReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  
   /* Call the App to signal that a Set Low Credit Warning Level request was received*/
  BeeAppUpdateDevice(0, gZclUI_SEPrepayment_SetLowCreditWarningLevelReq_c, 0, 0, pReq);
  
  return gZclSuccess_c;
}
#endif

#if gASL_ZclSE_12_Features_d 
/******************************************************************************
 * Name: ZCL_ProcessGetDebtRepaymentLogReq
 * Description: Process Client Get Debt Repaymenty Log request
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_ProcessGetDebtRepaymentLogReq(
 zbApsdeDataIndication_t *pIndication
)
{
  zclCmdPrepayment_GetDebtRepaymentLogReq_t* pReq = 
              (zclCmdPrepayment_GetDebtRepaymentLogReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  
   /* Call the App to signal that a Get Debt Repaymenty Log request was received*/
  BeeAppUpdateDevice(0, gZclUI_SEPrepayment_GetDebtRepaymentLog, 0, 0, pReq);
  return gZclSuccess_c;
}
#endif

#if !gASL_ZclSE_12_Features_d && gASL_ZclPrepayment_SupplyOptionals_d
/******************************************************************************
 * Name: zclPrepayment_Server_SupplyStatusRsp
 * Description: Server generated commands
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t zclPrepayment_Server_SupplyStatusRsp
(
zclPrepayment_SupplyStatRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrepayment_Server_SupplyStatRsp_c, sizeof(zclCmdPrepayment_SupplyStatRsp_t),(zclGenericReq_t *)pReq);
}
#endif

#if !gASL_ZclSE_12_Features_d && gASL_ZclPrepayment_SupplyOptionals_d
/******************************************************************************
 * Name: ZCL_ProcessChangeSupplyReq
 * Description: 
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_ProcessChangeSupplyReq
(
zbApsdeDataIndication_t *pIndication
)
{
  zclCmdPrepayment_ChangeSupplyReq_t  *pMsg;
  afAddrInfo_t addrInfo;
  uint8_t supplyStatus, siteIdLength, meterSerialNoLength, proposedSupplyStatus, originatorId;
  ZCLTime_t currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
  ZCLTime_t implementationDateTime, startTime = 0x00000000;
  uint16_t delayedInterruptValue;
  zbClusterId_t clusterIdMet = {gaZclClusterSmplMet_c};
  
  pMsg = (zclCmdPrepayment_ChangeSupplyReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  siteIdLength = pMsg->siteId.length;
  meterSerialNoLength = *(&pMsg->siteId.length + siteIdLength + sizeof(uint8_t)); 
  proposedSupplyStatus = *(&pMsg->siteId.length + siteIdLength + meterSerialNoLength + 2 * sizeof(uint8_t) + sizeof(ZCLTime_t));
  FLib_MemCpy(&implementationDateTime, &pMsg->siteId.length + siteIdLength + meterSerialNoLength + 2 * sizeof(uint8_t), sizeof(ZCLTime_t));
  implementationDateTime = OTA2Native32(implementationDateTime);
  originatorId = *(&pMsg->siteId.length + siteIdLength + meterSerialNoLength + 3 * sizeof(uint8_t) + sizeof(ZCLTime_t));

  AF_PrepareForReply(&addrInfo, pIndication);
  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), clusterIdMet, gZclAttrMetRISSupplyStatus_c , gZclServerAttr_c, &supplyStatus, NULL);
  
  if (proposedSupplyStatus == supplyStatus) 
    return gZclSuccessDefaultRsp_c;
  
  startTime = implementationDateTime;
   
  if (implementationDateTime == 0x00000000)
  {
    if (proposedSupplyStatus == gSePrepaymentSupplyInterrupt_c)
    {
      (void)ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentDelayedSuplyIntValueRem_c , gZclServerAttr_c, &delayedInterruptValue, NULL);
      if (delayedInterruptValue == 0)
       (void)ZCL_SetAttribute(BeeAppDataInit(appEndPoint), clusterIdMet, gZclAttrMetRISSupplyStatus_c , gZclServerAttr_c, &proposedSupplyStatus);
    }
    else
      (void)ZCL_SetAttribute(BeeAppDataInit(appEndPoint), clusterIdMet, gZclAttrMetRISSupplyStatus_c , gZclServerAttr_c, &proposedSupplyStatus);
      
  } 
  else if (implementationDateTime == 0xFFFFFFFF)
  {
    ZbTMR_StopTimer(gPrepaymentTimerID);
    /* Reset Proposed Implementation Time*/
    (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentProposedChangeImplTime_c , gZclServerAttr_c, &startTime);
  }  
   
  else if (startTime < currentTime)
  {
    return gZclInvalidValue_c;
  }
  else
  {
    (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentProposedChangeSupplyStatus_c , gZclServerAttr_c, &proposedSupplyStatus);
    /* Set the proposed implementation time */
    (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentProposedChangeImplTime_c , gZclServerAttr_c, &implementationDateTime);
    /* Set the proposed provider id */
    (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPrepaymentProposedChangeProviderID_c , gZclServerAttr_c, &pMsg->providerId);
    /* Store the proposed status*/
    mProposedSupplyStatus = proposedSupplyStatus;
    /* Set the timer for the supply status change */
    ZbTMR_StartSecondTimer(gPrepaymentTimerID, (uint16_t)(startTime - currentTime), PrepaymentTimerCallBack);
  }
 
  /* Check if we have to send a response */
  if(originatorId & 0x08)
    return ZCL_SendSupplyStatusRsp(pIndication);
  else
    return gZclSuccess_c;
}
#endif

#if !gASL_ZclSE_12_Features_d && gASL_ZclPrepayment_SupplyOptionals_d
/******************************************************************************
 * Name: ZCL_SendSupplyStatusRsp
 * Description: 
 * Param(s): -
 * Return: -
 ******************************************************************************/
static zbStatus_t ZCL_SendSupplyStatusRsp
(
zbApsdeDataIndication_t *pIndication
)
{
  zclPrepayment_SupplyStatRsp_t       response;
  zclCmdPrepayment_ChangeSupplyReq_t  *pMsg;
  /* Get the request to extract the required info */
  pMsg = (zclCmdPrepayment_ChangeSupplyReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  /* Prepare the response */
  AF_PrepareForReply(&response.addrInfo, pIndication);
  response.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  FLib_MemCpy(&response.cmdFrame.providerId, &pMsg->providerId, sizeof(ProviderID_t));
  response.cmdFrame.implementationDateTime = pMsg->implementationDateTime;
  response.cmdFrame.supplyStatus = pMsg->proposedSupplyStatus;
  
  return zclPrepayment_Server_SupplyStatusRsp(&response);
}
#endif



/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		void ZCL_PrepaymentServerInit(void)
 *
 * @brief	Initializes functionality on the Prepayment Server.
 *
 */
void ZCL_PrepaymentServerInit(void)
{
  #if !gASL_ZclSE_12_Features_d
  gPrepaymentTimerID = ZbTMR_AllocateTimer();
  #endif
}

/*!
 * @fn 		zbStatus_t ZCL_PrepaymentClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Prepayment client. 
 *
 */
zbStatus_t ZCL_PrepaymentClient
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
  Cmd = pFrame->command;
  
  switch(Cmd) 
  {
  #if !gASL_ZclSE_12_Features_d  
    case gZclCmdPrepayment_Server_SupplyStatRsp_c: 
      break;
  #else
    case gZclCmdPrepayment_Server_GetPrepaySnapshotRsp_c: 
    case gZclCmdPrepayment_Server_ChangePaymentModeRsp_c: 
    case gZclCmdPrepayment_Server_ConsumerTopUpRsp_c:
    case gZclCmdPrepayment_Server_GetCommandsRsp_c: 
    case gZclCmdPrepayment_Server_PublishTopUpLogRsp_c:
      break;
  #endif      
    default:
      status = gZclUnsupportedClusterCommand_c;
      break;  
 }
   /* Check if we need to send Default Response */
  if((pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) &&
     (status == gZclSuccessDefaultRsp_c))
    status = gZclSuccess_c;
  
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_PrepaymentClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Prepayment server. 
 *
 */
zbStatus_t ZCL_PrepaymentServer
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
  Cmd = pFrame->command;
 
  switch(Cmd) 
  {
#if gASL_ZclPrepayment_SupplyOptionals_d
  case gZclCmdPrepayment_Client_SelAvailEmergCreditReq_c: 
      status = ZCL_SelectAvailEmergCreditReq(pIndication);
      break;
#endif      
#if !gASL_ZclSE_12_Features_d && gASL_ZclPrepayment_SupplyOptionals_d
    case gZclCmdPrepayment_Client_ChangeSupplyReq_c: 
      status = ZCL_ProcessChangeSupplyReq(pIndication);  
      break; 
#endif      
#if gASL_ZclSE_12_Features_d 
    case gZclCmdPrepayment_Client_ChangeDebtReq_c:
      status = ZCL_ProcessChangeDebtReq(pIndication); 
      break;
    case gZclCmdPrepayment_Client_EmergencyCreditSetupReq_c:
      status = ZCL_ProcessEmergencyCredtitSetupReq(pIndication);
      break;
    case gZclCmdPrepayment_Client_ConsumerTopUpReq_c:
      status = ZCL_ProcessConsumerTopUpReq(pIndication);
      break;
    case gZclCmdPrepayment_Client_CreditAdjustmentReq_c:
      status = ZCL_ProcessCreditAdjustmentReq(pIndication);
      break;
    case gZclCmdPrepayment_Client_ChangePaymentModeReq_c:
      status = ZCL_ProcessChangePaymentModeReq(pIndication);
      break;
    case gZclCmdPrepayment_Client_GetPrepaySnapshotReq_c:
      status = ZCL_ProcessGetPrepaySnapshotReq(pIndication);
      break;
    case gZclCmdPrepayment_Client_GetTopUpLogReq_c:
      status = ZCL_ProcessGetTopUpLogReq(pIndication);
      break;
    case gZclCmdPrepayment_Client_SetLowCreditWarningLevelReq_c:
      status = ZCL_ProcessSetLowCreditWarningLevelReq(pIndication);
      break;
    case gZclCmdPrepayment_Client_GetDebtRepaymentLogReq_c:
      status = ZCL_ProcessGetDebtRepaymentLogReq(pIndication);
      break;
#endif  
    default:
      status = gZclUnsupportedClusterCommand_c;
      break;  
  }
  
   /* Check if we need to send Default Response */
  if((pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) &&
     (status == gZclSuccessDefaultRsp_c))
    status = gZclSuccess_c;
  
  return status;
}

#if gASL_ZclPrepayment_SupplyOptionals_d
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_SelAvailEmergCreditReq(zclPrepayment_SelAvailEmergCreditReq_t *pReq)
 *
 * @brief	Sends over-the-air a Select Available Emergency Credit frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_SelAvailEmergCreditReq
(
zclPrepayment_SelAvailEmergCreditReq_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  uint8_t meterSerialNumberLength = pReq->cmdFrame.meterSerialNumber.length;
  uint8_t siteIdLength = pReq->cmdFrame.siteId.length;
  
  pSrc  = (uint8_t *) &pReq->cmdFrame.meterSerialNumber;
  pDst  = &pReq->cmdFrame.siteId.aStr[pReq->cmdFrame.siteId.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrepayment_SelAvailEmergCreditReq_t) - (sizeof(ZCLTime_t) + 2 * sizeof(uint8_t) + pReq->cmdFrame.siteId.length);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
     
  length = sizeof(zclCmdPrepayment_SelAvailEmergCreditReq_t) - (31 - siteIdLength)- (15 - meterSerialNumberLength);
  
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_SelAvailEmergCreditReq_c, length,(zclGenericReq_t *)pReq);
}
#endif

#if !gASL_ZclSE_12_Features_d && gASL_ZclPrepayment_SupplyOptionals_d
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_ChangeSupplyReq(zclPrepayment_ChangeSupplyReq_t *pReq)
 *
 * @brief	Sends over-the-air a Change Supply frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_ChangeSupplyReq
(
  zclPrepayment_ChangeSupplyReq_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  uint8_t meterSerialNumberLength = pReq->cmdFrame.meterSerialNumber.length;
  uint8_t siteIdLength = pReq->cmdFrame.siteId.length;
  
  pSrc  = (uint8_t *) &pReq->cmdFrame.meterSerialNumber;
  pDst  = &pReq->cmdFrame.siteId.aStr[pReq->cmdFrame.siteId.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrepayment_ChangeSupplyReq_t) - (sizeof(ZCLTime_t) + sizeof(ProviderID_t)+ sizeof(uint8_t) + pReq->cmdFrame.siteId.length);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
     
  pSrc  = (uint8_t *) &pReq->cmdFrame.implementationDateTime - (31 - siteIdLength);
  pDst  = &pReq->cmdFrame.meterSerialNumber.aStr[meterSerialNumberLength]- (31 - siteIdLength);  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrepayment_ChangeSupplyReq_t) - (sizeof(ZCLTime_t) + sizeof(ProviderID_t)+ sizeof(uint8_t) + siteIdLength
                                                         + sizeof(uint8_t) + meterSerialNumberLength);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
  
  length = sizeof(zclCmdPrepayment_ChangeSupplyReq_t) - (31 - siteIdLength)- (15 - meterSerialNumberLength);
  
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_ChangeSupplyReq_c, length,(zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_ChangeSupplyReq(zclPrepayment_ChangeSupplyReq_t *pReq)
 *
 * @brief	Sends over-the-air a Change Debt frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_ChangeDebtReq
(
  zclPrepayment_ChangeDebtReq_t *pReq
)
{
  /* Remove length bytes and extra bytes from debtLabel and signature - obtain ZCL frame */  
  uint8_t *pSrc,*pDst;
  uint8_t length;
  uint8_t debtLabelLength = pReq->cmdFrame.debtLabel.length;
  uint8_t signatureLength = pReq->cmdFrame.signature.length;
  
  pSrc  = (uint8_t *)&pReq->cmdFrame.signature.aStr[0];
  pDst  = &pReq->cmdFrame.signature.length;  
  
  FLib_MemInPlaceCpy(pDst, pSrc, signatureLength);
  
  pSrc  = (uint8_t *)&pReq->cmdFrame.debtAmount;
  pDst  = &pReq->cmdFrame.debtLabel.aStr[debtLabelLength];

  FLib_MemInPlaceCpy(pDst, pSrc, sizeof(zclCmdPrepayment_ChangeDebtReq_t) - sizeof(ZCLTime_t) - sizeof(zclStr12_t));
  
  pSrc  = (uint8_t *)&pReq->cmdFrame.debtLabel.aStr[0];
  pDst  = &pReq->cmdFrame.debtLabel.length;
    
  FLib_MemInPlaceCpy(pDst, pSrc, sizeof(zclCmdPrepayment_ChangeDebtReq_t) - sizeof(ZCLTime_t));
  
  length = sizeof(zclCmdPrepayment_ChangeDebtReq_t) - 
           (sizeof(zclStr12_t) - sizeof(uint8_t) - debtLabelLength) - 
           (sizeof(zclStr16Oct_t) - sizeof(uint8_t) - signatureLength);
  
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_ChangeDebtReq_c, length,(zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_ChangeSupplyReq(zclPrepayment_ChangeSupplyReq_t *pReq)
 *
 * @brief	Sends over-the-air an Emergency Credit Setup frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_EmergencyCreditSetupReq
(
  zclPrepayment_EmergencyCreditSetupReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_ChangeDebtReq_c ,
                                    sizeof(zclCmdPrepayment_EmergencyCreditSetupReq_t),(zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_ConsumerTopUpReq(zclPrepayment_ConsumerTopUpReq_t *pReq)
 *
 * @brief	Sends over-the-air a Consumer Top Up frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_ConsumerTopUpReq
(
  zclPrepayment_ConsumerTopUpReq_t *pReq
)
{
  /* Remove length bytes and extra bytes from topUpCode - obtain ZCL frame */  
  uint8_t *pSrc,*pDst;
  uint8_t length;
  uint8_t topUpCodeLength = pReq->cmdFrame.topUpCode.length;
  
  pSrc  = (uint8_t *)&pReq->cmdFrame.topUpCode.aStr[0];
  pDst  = &pReq->cmdFrame.topUpCode.length;  
  
  FLib_MemInPlaceCpy(pDst, pSrc, topUpCodeLength);
  
  length = sizeof(zclCmdPrepayment_ConsumerTopUpReq_t) -  
           (sizeof(zclStr25_t) - sizeof(uint8_t) - topUpCodeLength);
  
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_ConsumerTopUpReq_c,
                                    length,(zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_CreditAdjustmentReq(zclPrepayment_CreditAdjustmentReq_t *pReq)
 *
 * @brief	Sends over-the-air a Credit Adjustment frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_CreditAdjustmentReq
(
  zclPrepayment_CreditAdjustmentReq_t *pReq
)
{
  /* Remove length bytes and extra bytes from signature - obtain ZCL frame */  
  uint8_t *pSrc,*pDst;
  uint8_t length;
  uint8_t signatureLength = pReq->cmdFrame.signature.length;
  
  pSrc  = (uint8_t *)&pReq->cmdFrame.signature.aStr[0];
  pDst  = &pReq->cmdFrame.signature.length;  
  
  FLib_MemInPlaceCpy(pDst, pSrc,signatureLength);
  
  length = sizeof(zclCmdPrepayment_ConsumerTopUpReq_t) -  
           (sizeof(zclStr16Oct_t) - sizeof(uint8_t) - signatureLength);
  
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_CreditAdjustmentReq_c,
                                    length,(zclGenericReq_t *)pReq);
}
#endif
#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_ChangePaymentModeReq(zclPrepayment_ChangePaymentModeReq_t *pReq)
 *
 * @brief	Sends over-the-air a Change Payment Mode frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_ChangePaymentModeReq
(
  zclPrepayment_ChangePaymentModeReq_t *pReq
)
{
  /* Remove length bytes and extra bytes from signature - obtain ZCL frame */  
  uint8_t *pSrc,*pDst;
  uint8_t length;
  uint8_t signatureLength = pReq->cmdFrame.signature.length;
  
  pSrc  = (uint8_t *)&pReq->cmdFrame.signature.aStr[0];
  pDst  = &pReq->cmdFrame.signature.length;  
  
  FLib_MemInPlaceCpy(pDst, pSrc,signatureLength);
  
  length = sizeof(zclCmdPrepayment_ChangePaymentModeReq_t) -  
           (sizeof(zclStr16Oct_t) - sizeof(uint8_t) - signatureLength);
  
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_ChangePaymentModeReq_c,
                                    length,(zclGenericReq_t *)pReq);
}
#endif
#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_GetPrepaySnapshotReq(zclPrepayment_GetPrepaySnapshotReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Prepay Snapshot frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_GetPrepaySnapshotReq
(
  zclPrepayment_GetPrepaySnapshotReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_GetPrepaySnapshotReq_c ,
                                    sizeof(zclCmdPrepayment_GetPrepaySnapshotReq_t),(zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_GetTopUpLogReq(zclPrepayment_GetTopUpLogReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Top Up Log frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_GetTopUpLogReq
(
  zclPrepayment_GetTopUpLogReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_GetTopUpLogReq_c ,
                                    sizeof(zclCmdPrepayment_GetTopUpLogReq_t),(zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_SetLowCreditWarningLevelReq(zclPrepayment_SetLowCreditWarningLevelReq_t *pReq)
 *
 * @brief	Sends over-the-air a Set Low Credit Warning Level frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_SetLowCreditWarningLevelReq
(
  zclPrepayment_SetLowCreditWarningLevelReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_SetLowCreditWarningLevelReq_c ,
                                    sizeof(zclCmdPrepayment_SetLowCreditWarningLevelReq_t),(zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_GetDebtRepaymentLogReq(zclPrepayment_GetDebtRepaymentLogReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Debt Repayment Log frame from the Prepayment client. 
 *
 */
zbStatus_t zclPrepayment_Client_GetDebtRepaymentLogReq
(
  zclPrepayment_GetDebtRepaymentLogReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_GetDebtRepaymentLogReq_c ,
                                    sizeof(zclCmdPrepayment_GetDebtRepaymentLogReq_t),(zclGenericReq_t *)pReq);
}
#endif

#if !gASL_ZclSE_12_Features_d
/******************************************************************************
 * Name: PrepaymentTimerCallBack
 * Description: 
 * Param(s): -
 * Return: -
 ******************************************************************************/
void PrepaymentTimerCallBack(zbTmrTimerID_t timerID)
{
  ZCLTime_t time = 0x00000000; 
  zbClusterId_t clusterIdPrepay = {gaZclClusterPrepayment_c};  
  zbClusterId_t clusterIdMet = {gaZclClusterSmplMet_c};  
  (void)timerID;
  
  /* Change Proposed Supply Status*/
  (void)ZCL_SetAttribute(BeeAppDataInit(appEndPoint), clusterIdMet, gZclAttrMetRISSupplyStatus_c , gZclServerAttr_c, &mProposedSupplyStatus);
  /* Reset Proposed Implementation Time*/
  (void)ZCL_SetAttribute(BeeAppDataInit(appEndPoint), clusterIdPrepay, gZclAttrPrepaymentProposedChangeImplTime_c , gZclServerAttr_c, &time);
}
#endif

#if gASL_ZclSE_12_Features_d 
/*!
 * @fn 		zbStatus_t zclPrepayment_Server_GetCommandRsp(zclPrepayment_GetCommandRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Get Command frame from the Prepayment server. 
 *
 */
zbStatus_t zclPrepayment_Server_GetCommandRsp
(
  zclPrepayment_GetCommandRsp_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdPrepayment_Server_GetCommandsRsp_c ,
                                    sizeof(zclCmdPrepayment_GetCommand_t),(zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclSE_12_Features_d 
/******************************************************************************
 * Name: CheckFrendlyCreditCalendar
 * Description: 
 * Param(s): -
 * Return: -
 ******************************************************************************/
static bool_t ZCL_CheckFrendlyCreditCalendar(zclCmdPrepayment_ChangePaymentModeRes_t* pChangePaymentMode)
{
  #if gASL_ZclSE_Use_NextGenFeatures_d || gASL_ZclTouCalendar_Optionals_d
  uint8_t indCalendar;
  ZCLTime_t lastCalendarTime = 0;
  ZCLTime_t currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));
    
  pChangePaymentMode->frendlyCreditCalendarID = 0;
  for(indCalendar = 0; indCalendar < gTouCalendar_NoOfInstances_c; indCalendar++)
  {  
    if(gServerActivityCalendarInfo[indCalendar].entryStatus == gEntryUsed_c)
    {
      if(gServerActivityCalendarInfo[indCalendar].calendar.type == gTouCalendar_CalendarType_FriendlyCreditCalendar_c)
      {
        if((gServerActivityCalendarInfo[indCalendar].calendar.startTime <= currentTime)&&
          (lastCalendarTime <= gServerActivityCalendarInfo[indCalendar].calendar.startTime))
        {
          lastCalendarTime = gServerActivityCalendarInfo[indCalendar].calendar.startTime;
          pChangePaymentMode->frendlyCreditCalendarID = gServerActivityCalendarInfo[indCalendar].calendar.issuerId; 
        }
      }
    }
  }
  if(pChangePaymentMode->frendlyCreditCalendarID != 0)
  {
    return TRUE;
  } 
  #else
  pChangePaymentMode->frendlyCreditCalendarID = 0;
  #endif
  return FALSE; 
}
#endif
