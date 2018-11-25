/*! @file 	ZclSEPrepayment.h
 *
 * @brief	Types, definitions and prototypes for the Prepayment cluster implementation.
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
#ifndef _ZCL_PREPAYMENT_H
#define _ZCL_PREPAYMENT_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "AppAfInterface.h"
#include "TMR_Interface.h"
#include "SEProfile.h"
#include "ZCL.h"
#include "zclSE.h"

/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/
/**********************************************
Prepayment cluster definitions
***********************************************/

/* Supply Status Attribute */
#define gSePrepaymentSupplyInterrupt_c   0x00
#define gSePrepaymentSupplyArmed_c       0x01
#define gSePrepaymentSupplyRestore_c     0x02

#define gSePrepaymentCtrlFlags_DisconnectionEnabled_c   0x01
#define gSePrepaymentCtrlFlags_CreditMgmtEnabled_c      0x04
#define gSePrepaymentCtrlFlags_CreditDisplayEnabled_c   0x10
#define gSePrepaymentCtrlFlags_AccountBase_c            0x40
#define gSePrepaymentCtrlFlags_ContractorFitter_c       0x80
#define gSePrepaymentCtrlFlags_Reserved_c               0x2A

#define gSePrepaymentCreditStatusFlags_CreditOK           0x01
#define gSePrepaymentCreditStatusFlags_LowCredit          0x02
#define gSePrepaymentCreditStatusFlags_EmergencyEnabled   0x04
#define gSePrepaymentCreditStatusFlags_EmergencyAvailable 0x08
#define gSePrepaymentCreditStatusFlags_EmergencySelected  0x10
#define gSePrepaymentCreditStatusFlags_EmergencyInUse     0x20
#define gSePrepaymentCreditStatusFlags_EmergencyExhausted 0x40

/* Prepayment Info Set */
#define gZclAttrPrepaymentInfoSet_c 	                      0x00

#define gZclAttrIdPrepaymentPaymentControl_c                  0x00
#define gZclAttrIdPrepaymentCreditRemaining_c                 0x01
#define gZclAttrIdPrepaymentEmergencyCreditRemaining_c        0x02
#define gZclAttrIdPrepaymentCreditStatus_c                    0x03
#define gZclAttrIdPrepaymentCreditRemainingTimeStamp_c        0x04
#define gZclAttrIdPrepaymentAccumulatedDebt_c                 0x05
#define gZclAttrIdPrepaymentOverallDebtCap_c                  0x06
#define gZclAttrIdPrepaymentCreditStatus_c                    0x03
#define gZclAttrIdPrepaymentEmergencyCreditLimitAllowance_c   0x10
#define gZclAttrIdPrepaymentEmergencyCreditThreshold_c        0x11
#define gZclAttrIdPrepaymentTotalCreditAdded_c                0x20
#define gZclAttrIdPrepaymentMaxCreditLimit_c                  0x21
#define gZclAttrIdPrepaymentFriendlyCreditWarning_c           0x30
#define gZclAttrIdPrepaymentLowCreditWarning_c                0x31
#define gZclAttrIdPrepaymentIHDLowCreditWarning_c             0x32
#define gZclAttrIdPrepaymentInterruptSuspendTime_c            0x33
#define gZclAttrIdPrepaymentCutOffValue_c                     0x40
#define gZclAttrIdPrepaymentPaymentCardID_c                   0x80

/* Prepayment Top Up Set */
#define gZclAttrPrepaymentTopUpSet_c              0x01

#define gZclAttrIdPrepaymentTopUpDateTime1_c        0x00
#define gZclAttrIdPrepaymentTopUpAmount1_c          0x01
#define gZclAttrIdPrepaymentOriginatingDevice1_c    0x02
#define gZclAttrIdPrepaymentTopUpCode1_c            0x03
#define gZclAttrIdPrepaymentTopUpDateTime2_c        0x10
#define gZclAttrIdPrepaymentTopUpAmount2_c          0x11
#define gZclAttrIdPrepaymentOriginatingDevice2_c    0x12
#define gZclAttrIdPrepaymentTopUpCode2_c            0x13
#define gZclAttrIdPrepaymentTopUpDateTime3_c        0x20
#define gZclAttrIdPrepaymentTopUpAmount3_c          0x21
#define gZclAttrIdPrepaymentOriginatingDevice3_c    0x22
#define gZclAttrIdPrepaymentTopUpCode3_c            0x23
#define gZclAttrIdPrepaymentTopUpDateTime4_c        0x30
#define gZclAttrIdPrepaymentTopUpAmount4_c          0x31
#define gZclAttrIdPrepaymentOriginatingDevice4_c    0x32
#define gZclAttrIdPrepaymentTopUpCode4_c            0x33
#define gZclAttrIdPrepaymentTopUpDateTime5_c        0x40
#define gZclAttrIdPrepaymentTopUpAmount5_c          0x41
#define gZclAttrIdPrepaymentOriginatingDevice5_c    0x42
#define gZclAttrIdPrepaymentTopUpCode5_c            0x43

/* Prepayment Fuel Debt Set*/
#define gZclAttrPrepaymentDebtSet_c                       0x02
#define gZclAttrIdPrepaymentDebtLabel1_c                  0x10
#define gZclAttrIdPrepaymentDebtAmount1_c                 0x11
#define gZclAttrIdPrepaymentDebtRecoveryMethod1_c         0x12
#define gZclAttrIdPrepaymentDebtRecoveryStartTime1_c      0x13
#define gZclAttrIdPrepaymentDebtRecoveryCollectionTime1_c 0x14
#define gZclAttrIdPrepaymentDebtRecoveryFequency1_c       0x16
#define gZclAttrIdPrepaymentDebtRecoveryAmount1_c         0x17
#define gZclAttrIdPrepaymentDebtRecoveryVendPerCent1_c    0x19
#define gZclAttrIdPrepaymentDebtRecoveryRateCap1_c        0x1B
#define gZclAttrIdPrepaymentDebtLabel2_c                  0x20
#define gZclAttrIdPrepaymentDebtAmount2_c                 0x21
#define gZclAttrIdPrepaymentDebtRecoveryMethod2_c         0x22
#define gZclAttrIdPrepaymentDebtRecoveryStartTime2_c      0x23
#define gZclAttrIdPrepaymentDebtRecoveryCollectionTime2_c 0x24
#define gZclAttrIdPrepaymentDebtRecoveryFequency2_c       0x26
#define gZclAttrIdPrepaymentDebtRecoveryAmount2_c         0x27
#define gZclAttrIdPrepaymentDebtRecoveryVendPerCent2_c    0x29
#define gZclAttrIdPrepaymentDebtRecoveryRateCap2_c        0x2B
#define gZclAttrIdPrepaymentDebtLabel3_c                  0x30
#define gZclAttrIdPrepaymentDebtAmount3_c                 0x31
#define gZclAttrIdPrepaymentDebtRecoveryMethod3_c         0x32
#define gZclAttrIdPrepaymentDebtRecoveryStartTime3_c      0x33
#define gZclAttrIdPrepaymentDebtRecoveryCollectionTime3_c 0x34
#define gZclAttrIdPrepaymentDebtRecoveryFequency3_c       0x36
#define gZclAttrIdPrepaymentDebtRecoveryAmount3_c         0x37
#define gZclAttrIdPrepaymentDebtRecoveryVendPerCent3_c    0x39
#define gZclAttrIdPrepaymentDebtRecoveryRateCap3_c        0x3B

#if gASL_ZclSE_12_Features_d
  /* DebtRecoveryMethod Enumerated Attributed */
  typedef enum zclAttrIdPrepaymentDebtRecoveryMethod_tag
  {
    gZclAttrIdPrepaymentDebtRecoveryMethod_TimeBased_c,
    gZclAttrIdPrepaymentDebtRecoveryMethod_BalanceBased_c,
    gZclAttrIdPrepaymentDebtRecoveryMethod_CatchupBased_c
  } zclAttrIdPrepaymentDebtRecoveryMethod_t;

  /* DebtRecoveryFequency Enumerated Attributed */
  typedef enum zclAttrIdPrepaymentDebtRecoveryFequency_tag
  {
    gZclAttrIdPrepaymentDebtRecoveryFequency_PerHour_c,
    gZclAttrIdPrepaymentDebtRecoveryFequency_PerDay_c,
    gZclAttrIdPrepaymentDebtRecoveryFequency_PerWeek_c,
    gZclAttrIdPrepaymentDebtRecoveryFequency_PerMonth_c,
    gZclAttrIdPrepaymentDebtRecoveryFequency_PerQuarter_c
  } zclAttrIdPrepaymentDebtRecoveryFequency_t;
#endif

#if  !gASL_ZclSE_12_Features_d

  /* Prepayment Supply Set*/
  #define gZclAttrPrepaymentSupplySet_c 		        0x03

  #define gZclAttrIdPrepaymentProposedChangeProviderID_c    0x00
  #define gZclAttrIdPrepaymentProposedChangeImplTime_c      0x01
  #define gZclAttrIdPrepaymentProposedChangeSupplyStatus_c  0x02
  #define gZclAttrIdPrepaymentDelayedSuplyIntValueRem_c     0x03
  #define gZclAttrIdPrepaymentDelayedSupplyIntValType_c     0x04
#else
  
  /* Prepayment Alarm Set*/
  #define gZclAttrPrepaymentAlarmSet_c                      0x03

  #define gZclAttrIdPrepaymentAlarmStatus_c                 0x00
  #define gZclAttrIdPrepaymentGenericAlarmMask_c            0x01
  #define gZclAttrIdPrepaymentPrepaySwitchAlarmMask_c       0x02
  #define gZclAttrIdPrepaymentPrepayEventAlarmMask_c        0x03

  /* GenericAlarmGroup */
  #define gSePrepaymentAlarmStatusFlags_LowCreditWarning                0x0001
  #define gSePrepaymentAlarmStatusFlags_TopUpCodeError                  0x0002
  #define gSePrepaymentAlarmStatusFlags_TopUpCodeAlreadyUsed            0x0004
  #define gSePrepaymentAlarmStatusFlags_TopUpCodeInvalid                0x0008
  #define gSePrepaymentAlarmStatusFlags_FriendlyCreditInUse             0x0010
  #define gSePrepaymentAlarmStatusFlags_FriendlyCreditPeriodEndWarning  0x0020
  #define gSePrepaymentAlarmStatusFlags_ECAvailable                     0x0040
  #define gSePrepaymentAlarmStatusFlags_UnauthorisedEnergyUse           0x0080
  #define gSePrepaymentAlarmStatusFlags_DisconnectedSupplyDueToCredit   0x0100
  #define gSePrepaymentAlarmStatusFlags_DisconnectedSupplyDueToTamper   0x0200
  #define gSePrepaymentAlarmStatusFlags_DisconnectedSupplyDueToHES      0x0400
  #define gSePrepaymentAlarmStatusFlags_PhysicalAttack                  0x0800
  #define gSePrepaymentAlarmStatusFlags_ElectronicAttack                0x1000
  #define gSePrepaymentAlarmStatusFlags_ManufactureAlarmCodeA           0x2000
  #define gSePrepaymentAlarmStatusFlags_ManufactureAlarmCodeB           0x4000
  #define gSePrepaymentAlarmStatusFlags_Reserved                        0x8000

  /* GenericAlarmGroup */
  #define gSePrepaymentGenericAlarmGroup_LowCredit_c                    0x00
  #define gSePrepaymentGenericAlarmGroup_NoCredit_c                     0x01
  #define gSePrepaymentGenericAlarmGroup_CreditExhausted_c              0x02
  #define gSePrepaymentGenericAlarmGroup_EmergencyCreditEnabled_c       0x03
  #define gSePrepaymentGenericAlarmGroup_EmergencyCreditExhausted_c     0x04

  /* PrepaySwitchAlarmGroup */
  #define gSePrepaymentPrepaySwitchAlarmGroup_SupplyON_c                         0x10
  #define gSePrepaymentPrepaySwitchAlarmGroup_SupplyARM_c                        0x11
  #define gSePrepaymentPrepaySwitchAlarmGroup_SupplyOFF_c                        0x12
  #define gSePrepaymentPrepaySwitchAlarmGroup_DisconnectionFailure_c             0x13
  #define gSePrepaymentPrepaySwitchAlarmGroup_DisconnectedDueToTamperDetected_c  0x14
  #define gSePrepaymentPrepaySwitchAlarmGroup_RemoteDisconnected_c               0x15

  /* PrepayEventAlarmGroup */
  #define gSePrepaymentPrepayEventAlarmGroup_PhysicalAttackOnThePrepayMeter_c    0x20
  #define gSePrepaymentPrepayEventAlarmGroup_ElectronicAttackOnThePrepayMeter_c  0x21
  #define gSePrepaymentPrepayEventAlarmGroup_DiscountApplied_c                   0x22
#endif

#if  !gASL_ZclSE_12_Features_d  
  /* Prepayment full attribute ids */
  #if ( TRUE == gBigEndian_c )
    #define gZclAttrPrepaymentCreditStatus_c                0x0300 
    #define gZclAttrPrepaymentProposedChangeImplTime_c      0x0103
    #define gZclAttrPrepaymentProposedChangeSupplyStatus_c  0x0203
    #define gZclAttrPrepaymentProposedChangeProviderID_c    0x0003
    #define gZclAttrPrepaymentDelayedSuplyIntValueRem_c     0x0303
  #else
    #define gZclAttrPrepaymentCreditStatus_c                0x0003    
    #define gZclAttrPrepaymentProposedChangeImplTime_c      0x0301
    #define gZclAttrPrepaymentProposedChangeSupplyStatus_c  0x0302
    #define gZclAttrPrepaymentProposedChangeProviderID_c    0x0300
    #define gZclAttrPrepaymentDelayedSuplyIntValueRem_c     0x0303
  #endif /* #if ( TRUE == gBigEndian_c ) */
#else
  #if ( TRUE == gBigEndian_c )
/*  Payment Information  Attribute Set */
    #define gZclAttrPrepaymentCreditRemaining_c                 0x0100  
    #define gZclAttrPrepaymentEmergencyCreditRemaining_c        0x0200
    #define gZclAttrPrepaymentCreditStatus_c                    0x0300
    #define gZclAttrPrepaymentEmergencyCreditThreshold_c        0x1100
    #define gZclAttrPrepaymentTotalCreditAdded_c                0x2000
/*  Top-up Attribute Set */  
    #define gZclAttrPrepaymentTopUpDateTime1_c                  0x0001
    #define gZclAttrPrepaymentTopUpDateAmount1_c                0x0101
    #define gZclAttrPrepaymentTopUpOriginatingDevice1_c         0x0201
    #define gZclAttrPrepaymentTopUpCode1_c                      0x0301
    #define gZclAttrPrepaymentTopUpDateTime2_c                  0x1001
    #define gZclAttrPrepaymentTopUpDateAmount2_c                0x1101
    #define gZclAttrPrepaymentTopUpOriginatingDevice2_c         0x1201
    #define gZclAttrPrepaymentTopUpCode2_c                      0x1301
    #define gZclAttrPrepaymentTopUpDateTime3_c                  0x2001
    #define gZclAttrPrepaymentTopUpDateAmount3_c                0x2101
    #define gZclAttrPrepaymentTopUpOriginatingDevice3_c         0x2201
    #define gZclAttrPrepaymentTopUpCode3_c                      0x2301
    #define gZclAttrPrepaymentTopUpDateTime4_c                  0x3001
    #define gZclAttrPrepaymentTopUpDateAmount4_c                0x3101
    #define gZclAttrPrepaymentTopUpOriginatingDevic4_c          0x3201
    #define gZclAttrPrepaymentTopUpCode4_c                      0x3301
    #define gZclAttrPrepaymentTopUpDateTime5_c                  0x4001
    #define gZclAttrPrepaymentTopUpDateAmount5_c                0x4101
    #define gZclAttrPrepaymentTopUpOriginatingDevice5_c         0x4201
    #define gZclAttrPrepaymentTopUpCode5_c                      0x4301
/*  Debt Attribute Set */ 
    #define gZclAttrPrepaymentDebtLabel1_c                      0x0002
    #define gZclAttrPrepaymentDebtAmount1_c                     0x0102
    #define gZclAttrPrepaymentDebtRecoveryMethod1_c             0x0202
    #define gZclAttrPrepaymentDebtRecoveryStartTime1_c          0x0302
    #define gZclAttrPrepaymentDebtRecoveryCollectionTime1_c     0x0402
    #define gZclAttrPrepaymentDebtRecoveryFequency1_c           0x0602
    #define gZclAttrPrepaymentDebtRecoveryAmount1_c             0x0702
    #define gZclAttrPrepaymentDebtRecoveryBalancePercentage1_c  0x0902
    #define gZclAttrPrepaymentDebtRecoveryMaxMissed1            0x0B02
  #else
/*  Payment Information  Attribute Set */
    #define gZclAttrPrepaymentCreditRemaining_c                 0x0001
    #define gZclAttrPrepaymentEmergencyCreditRemaining_c        0x0002
    #define gZclAttrPrepaymentCreditStatus_c                    0x0003
    #define gZclAttrPrepaymentEmergencyCreditThreshold_c        0x0011
    #define gZclAttrPrepaymentTotalCreditAdded_c                0x0020
/*  Top-up Attribute Set */
    #define gZclAttrPrepaymentTopUpDateTime1_c                  0x0100
    #define gZclAttrPrepaymentTopUpDateAmount1_c                0x0101
    #define gZclAttrPrepaymentTopUpOriginatingDevice1_c         0x0102
    #define gZclAttrPrepaymentTopUpCode1_c                      0x0103
    #define gZclAttrPrepaymentTopUpDateTime2_c                  0x0110
    #define gZclAttrPrepaymentTopUpDateAmount2_c                0x0111
    #define gZclAttrPrepaymentTopUpOriginatingDevice2_c         0x0112
    #define gZclAttrPrepaymentTopUpCode2_c                      0x0112
    #define gZclAttrPrepaymentTopUpDateTime3_c                  0x0120
    #define gZclAttrPrepaymentTopUpDateAmount3_c                0x0121
    #define gZclAttrPrepaymentTopUpOriginatingDevice3_c         0x0122
    #define gZclAttrPrepaymentTopUpCode3_c                      0x0123
    #define gZclAttrPrepaymentTopUpDateTime4_c                  0x0130
    #define gZclAttrPrepaymentTopUpDateAmount4_c                0x0131
    #define gZclAttrPrepaymentTopUpOriginatingDevic4_c          0x0132
    #define gZclAttrPrepaymentTopUpCode4_c                      0x0133
    #define gZclAttrPrepaymentTopUpDateTime5_c                  0x0140
    #define gZclAttrPrepaymentTopUpDateAmount5_c                0x0141
    #define gZclAttrPrepaymentTopUpOriginatingDevice5_c         0x0142
    #define gZclAttrPrepaymentTopUpCode5_c                      0x0143
/*  Debt Attribute Set */  
    #define gZclAttrPrepaymentDebtLabel1_c                      0x0200
    #define gZclAttrPrepaymentDebtAmount1_c                     0x0201
    #define gZclAttrPrepaymentDebtRecoveryMethod1_c             0x0202
    #define gZclAttrPrepaymentDebtRecoveryStartTime1_c          0x0203
    #define gZclAttrPrepaymentDebtRecoveryCollectionTime1_c     0x0204
    #define gZclAttrPrepaymentDebtRecoveryFequency1_c           0x0206
    #define gZclAttrPrepaymentDebtRecoveryAmount1_c             0x0207
    #define gZclAttrPrepaymentDebtRecoveryBalancePercentage1_c  0x0209
    #define gZclAttrPrepaymentDebtRecoveryMaxMissed1            0x020B
  #endif /* #if ( TRUE == gBigEndian_c ) */
#endif /* !gASL_ZclSE_12_Features_d */  
  
/*----------Server commands---------*/
#if  !gASL_ZclSE_12_Features_d  
  #define gZclCmdPrepayment_Server_SupplyStatRsp_c              0x00 /* O */
#else
  #define gZclCmdPrepayment_Server_GetPrepaySnapshotRsp_c       0x00 /* O / M UK */ 
  #define gZclCmdPrepayment_Server_ChangePaymentModeRsp_c       0x01 /* O / M UK */ 
  #define gZclCmdPrepayment_Server_ConsumerTopUpRsp_c           0x02 /* O / M UK */ 
  #define gZclCmdPrepayment_Server_GetCommandsRsp_c             0x03 /* O / M UK */ 
  #define gZclCmdPrepayment_Server_PublishTopUpLogRsp_c         0x04 /* O / M UK */ 
#endif
  
/*----------Client commands---------*/
#define gZclCmdPrepayment_Client_SelAvailEmergCreditReq_c       0x00 /* O / M UK */

#if  !gASL_ZclSE_12_Features_d  
  #define gZclCmdPrepayment_Client_ChangeSupplyReq_c              0x01 /* O */
#else  
  #define gZclCmdPrepayment_Client_ChangeDebtReq_c                0x01 /* O / M UK */  
  #define gZclCmdPrepayment_Client_EmergencyCreditSetupReq_c      0x02 /* O / M UK */
  #define gZclCmdPrepayment_Client_ConsumerTopUpReq_c             0x03 /* O / M UK */
  #define gZclCmdPrepayment_Client_CreditAdjustmentReq_c          0x04 /* O / M UK */ 
  #define gZclCmdPrepayment_Client_ChangePaymentModeReq_c         0x05 /* O / M UK */
  #define gZclCmdPrepayment_Client_GetPrepaySnapshotReq_c         0x06 /* O / M UK */
  #define gZclCmdPrepayment_Client_GetTopUpLogReq_c               0x07 /* O / M UK */
  #define gZclCmdPrepayment_Client_SetLowCreditWarningLevelReq_c  0x08 /* O / M UK */
  #define gZclCmdPrepayment_Client_GetDebtRepaymentLogReq_c       0x09 /* O / M UK */
#endif  
  
typedef PACKED_STRUCT zclCmdPrepayment_SupplyStatRsp_tag
{
  ProviderID_t    providerId;
  ZCLTime_t       implementationDateTime;
  uint8_t         supplyStatus;
}zclCmdPrepayment_SupplyStatRsp_t;

typedef PACKED_STRUCT zclPrepayment_SupplyStatRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrepayment_SupplyStatRsp_t cmdFrame;
}zclPrepayment_SupplyStatRsp_t;

typedef PACKED_STRUCT zclCmdPrepayment_SelAvailEmergCreditReq_tag
{
  ZCLTime_t       cmdDateTime;
  uint8_t         originatingDevice;
  zclStr32Oct_t   siteId;
  zclStr16Oct_t   meterSerialNumber;
}zclCmdPrepayment_SelAvailEmergCreditReq_t;

typedef PACKED_STRUCT zclPrepayment_SelAvailEmergCreditReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrepayment_SelAvailEmergCreditReq_t cmdFrame;
}zclPrepayment_SelAvailEmergCreditReq_t;

typedef PACKED_STRUCT zclCmdPrepayment_ChangeSupplyReq_tag
{
  ProviderID_t    providerId;
  ZCLTime_t       reqDateTime;
  zclStr32Oct_t   siteId;
  zclStr16Oct_t   meterSerialNumber;
  ZCLTime_t       implementationDateTime;
  uint8_t         proposedSupplyStatus;
  uint8_t         originatorId;
}zclCmdPrepayment_ChangeSupplyReq_t;

typedef PACKED_STRUCT zclPrepayment_ChangeSupplyReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrepayment_ChangeSupplyReq_t cmdFrame;
}zclPrepayment_ChangeSupplyReq_t;


typedef enum zclPrepaymentDebtTypeField_tag
{
  gZclPrepaymentDebtType1_Absolute_c,
  gZclPrepaymentDebtType1_Incremental_c,
  gZclPrepaymentDebtType2_Absolute_c,
  gZclPrepaymentDebtType2_Incremental_c,
  gZclPrepaymentDebtType3_Absolute_c,
  gZclPrepaymentDebtType4_Incremental_c
}zclPrepaymentDebtTypeField_t;

typedef PACKED_STRUCT zclCmdPrepayment_ChangeDebtReq_tag
{
  ZCLTime_t                     cmdIssueDateTime;
  zclStr12_t                    debtLabel;
  uint32_t                      debtAmount;
  uint8_t                       debtRecoveryMethod;
  zclPrepaymentDebtTypeField_t  debtType;
  ZCLTime_t                     debtRecoveryStartTime;
  uint16_t                      debtRecoveryCollectionTime;
  ZCLTime_t                     debtRecoveryFrequency;
  uint32_t                      debtRecoveryAmount;
  uint16_t                      debtRecoveryBalancePercentage;
  uint8_t                       debtRecoveryMaxMissed;
  zclStr16Oct_t                 signature;
}zclCmdPrepayment_ChangeDebtReq_t;

typedef PACKED_STRUCT zclPrepayment_ChangeDebtReq_tag
{
  afAddrInfo_t                    addrInfo;
  uint8_t                          zclTransactionId;
  zclCmdPrepayment_ChangeDebtReq_t cmdFrame;
}zclPrepayment_ChangeDebtReq_t;

typedef PACKED_STRUCT zclCmdPrepayment_EmergencyCreditSetupReq_tag
{
  ZCLTime_t  cmdIssueDateTime;
  uint32_t   emergencyCreditLimit;
  uint32_t   emergencyCreditThreshold;
}zclCmdPrepayment_EmergencyCreditSetupReq_t;

typedef PACKED_STRUCT zclPrepayment_EmergencyCreditSetupReq_tag
{
  afAddrInfo_t                     addrInfo;
  uint8_t                          zclTransactionId;
  zclCmdPrepayment_ChangeDebtReq_t cmdFrame;
}zclPrepayment_EmergencyCreditSetupReq_t;

typedef enum zclOriginatingTopUpDeviceField_tag
{
  gZclOrigESI_c,
  gZclOrigMeter_c,
  gZclOrigIPD_c 
}zclOriginatingTopUpDeviceField_t;

typedef PACKED_STRUCT zclCmdPrepayment_ConsumerTopUpReq_tag
{
  zclOriginatingTopUpDeviceField_t originatingDevice;
  zclStr25_t                       topUpCode;
}zclCmdPrepayment_ConsumerTopUpReq_t;

typedef PACKED_STRUCT zclPrepayment_ConsumerTopUpReq_tag
{
  afAddrInfo_t                        addrInfo;
  uint8_t                             zclTransactionId;
  zclCmdPrepayment_ConsumerTopUpReq_t cmdFrame;
}zclPrepayment_ConsumerTopUpReq_t;


typedef enum zclCreditAdjustmentType_tag
{
  gZclCreditIncremental_c,
  gZclCreditAbsolute_c
}zclCreditAdjustmentType_t;

typedef PACKED_STRUCT zclCmdPrepayment_CreditAdjustmentReq_tag
{
  ZCLTime_t                 cmdIssueDateTime;
  zclCreditAdjustmentType_t zclCreditAdjType;
  int48_t                   zclCreditAdjValue;
  zclStr16Oct_t             signature;
}zclCmdPrepayment_CreditAdjustmentReq_t;

typedef PACKED_STRUCT zclPrepayment_CreditAdjustmentReq_tag
{
  afAddrInfo_t                           addrInfo;
  uint8_t                                zclTransactionId;
  zclCmdPrepayment_CreditAdjustmentReq_t cmdFrame;
}zclPrepayment_CreditAdjustmentReq_t;

/* Proposed Payment Control Flags */
#define gDisconnectionEnabled_c     1<<0
#define gCreditManagementEnabled_c  1<<2
#define gCreditDisplayEnabled_c     1<<4

typedef PACKED_STRUCT zclCmdPrepayment_ChangePaymentModeReq_tag
{
  uint32_t       supplierID;
  uint32_t       modeEventID;
  ZCLTime_t      implementationDateTime;
  uint8_t        proposedPaymentControl;
  int32_t        cutOffValue; 
  zclStr16Oct_t  signature;
}zclCmdPrepayment_ChangePaymentModeReq_t;

typedef PACKED_STRUCT zclPrepayment_ChangePaymentModeReq_tag
{
  afAddrInfo_t                            addrInfo;
  uint8_t                                 zclTransactionId;
  zclCmdPrepayment_ChangePaymentModeReq_t cmdFrame;
}zclPrepayment_ChangePaymentModeReq_t;


typedef PACKED_STRUCT zclCmdPrepayment_GetPrepaySnapshotReq_tag
{
  ZCLTime_t startTime;
  int8_t    noOfSnaphots;
  uint16_t  snapshotCause;
}zclCmdPrepayment_GetPrepaySnapshotReq_t;

typedef PACKED_STRUCT zclPrepayment_GetPrepaySnapshotReq_tag
{
  afAddrInfo_t                            addrInfo;
  uint8_t                                 zclTransactionId;
  zclCmdPrepayment_ChangePaymentModeReq_t cmdFrame;
}zclPrepayment_GetPrepaySnapshotReq_t;

typedef PACKED_STRUCT zclCmdPrepayment_GetTopUpLogReq_tag
{
  uint8_t noOfevents;
}zclCmdPrepayment_GetTopUpLogReq_t;

typedef PACKED_STRUCT zclPrepayment_GetTopUpLogReq_tag
{
  afAddrInfo_t                            addrInfo;
  uint8_t                                 zclTransactionId;
  zclCmdPrepayment_GetTopUpLogReq_t       cmdFrame;
}zclPrepayment_GetTopUpLogReq_t;

typedef PACKED_STRUCT zclCmdPrepayment_SetLowCreditWarningLevelReq_tag
{
  uint32_t lowCreditWarningLevel;
}zclCmdPrepayment_SetLowCreditWarningLevelReq_t;

typedef PACKED_STRUCT zclPrepayment_SetLowCreditWarningLevelReq_tag
{
  afAddrInfo_t                                    addrInfo;
  uint8_t                                         zclTransactionId;
  zclCmdPrepayment_SetLowCreditWarningLevelReq_t  cmdFrame;
}zclPrepayment_SetLowCreditWarningLevelReq_t;

typedef enum zclDebtType_tag
{
  gZclDebt1_c,
  gZclDebt2_c,
  gZclDebt3_c 
}zclDebtType_t;

typedef PACKED_STRUCT zclCmdPrepayment_GetDebtRepaymentLogReq_tag
{
  uint8_t       noOfDebt;
  zclDebtType_t debtType;
}zclCmdPrepayment_GetDebtRepaymentLogReq_t;

typedef PACKED_STRUCT zclPrepayment_GetDebtRepaymentLogReq_tag
{
  afAddrInfo_t                               addrInfo;
  uint8_t                                    zclTransactionId;
  zclCmdPrepayment_GetDebtRepaymentLogReq_t  cmdFrame;
}zclPrepayment_GetDebtRepaymentLogReq_t;

typedef PACKED_STRUCT zclCmdPrepayment_GetTopUpCodeLogReq_tag
{
  uint8_t noOfEvents;
}zclCmdPrepayment_GetTopUpCodeLogReq_t;

/* Server respones commands */
#define gGeneralCause_c          1<<0
#define gTopUpAddition_c         1<<1
#define gDebtCreditAddition_c    1<<2
#define gChangeSupplier_c        1<<3
#define gChangeTenancy_c         1<<4

typedef enum zclSnapshotPayloadType_tag
{
  gZclDebtCreditAddition_c
}zclSnapshotPayloadType_t;

typedef PACKED_STRUCT zclSnapshotPayloadType_DebtCreditStatus_tag
{
  uint32_t    type1DebtRemaining;
  uint32_t    type2DebtRemaining;
  uint32_t    type3DebtRemaining;
  int32_t     emergencyCreditRemaining;
  int32_t     creditRemaining;
}zclSnapshotPayloadType_DebtCreditStatus_t;

typedef PACKED_STRUCT zclCmdPrepayment_GetPrepaySnapshotRes_tag
{
  uint32_t                  issuerEventID;
  ZCLTime_t                 snapshotTime;
  uint8_t                   cmdIndex;
  uint16_t                  snapshotCause;
  zclSnapshotPayloadType_t  snapshotPayloadType;
  uint8_t                   snapshotPayload[1];
}zclCmdPrepayment_GetPrepaySnapshotRes_t;

#define gFrendlycreditenabled_c     1<<0

typedef PACKED_STRUCT zclCmdPrepayment_ChangePaymentModeRes_tag
{
  uint8_t  frendlyCredit;
  uint32_t frendlyCreditCalendarID;
  uint32_t emergencyCreditLimit;
  uint32_t emergencyCreditThreshold;
}zclCmdPrepayment_ChangePaymentModeRes_t;

/* [R1] 5.2.4.2 Consumer Top Up Response */
typedef PACKED_STRUCT zclPrepayment_ChangePaymentModeRes_tag 
{
   afAddrInfo_t                                 addrInfo;
   uint8_t                                      zclTransactionId;
   zclCmdPrepayment_ChangePaymentModeRes_t      cmdFrame;
}zclPrepayment_ChangePaymentModeRes_t; 

typedef enum zclResultOfTopUpField_tag
{
  gZclAccepted_c,
  gZclRejectedInvalidTopUp_c,
  gZclRejectedDuplicateTopUp_c,
  gZclRejectedError_c,
  gZclRejectedMaxCreditReached_c,
  gZclRejectedKeypadLock_c 
}zclResultOfTopUpField_t;

typedef enum zclSourceOfTopUpDeviceField_tag
{
  gZclSourceIPD_c,
  gZclSourceESI_c,
  gZclSourceMeter_c 
}zclSourceOfTopUpDeviceField_t;

typedef PACKED_STRUCT zclCmdPrepayment_ConsumerTopUpRes_tag
{
  zclResultOfTopUpField_t resultType;
  int48_t topUpValue;
  zclSourceOfTopUpDeviceField_t sourceOfTopUp;
  int32_t creditRemaining;
}zclCmdPrepayment_ConsumerTopUpRes_t;

/* [R1] 5.2.4.3 Consumer Top Up Response */
typedef PACKED_STRUCT zclPrepayment_ConsumerTopUpRes_tag 
{
   afAddrInfo_t                                 addrInfo;
   uint8_t                                      zclTransactionId;
   zclCmdPrepayment_ConsumerTopUpRes_t          cmdFrame;
}zclPrepayment_ConsumerTopUpRes_t; 


typedef PACKED_STRUCT zclCmdPrepayment_GetCommand_tag
{
  uint8_t prePayNotificationFlags;
} zclCmdPrepayment_GetCommand_t;

typedef PACKED_STRUCT zclPrepayment_GetCommandRsp_tag
{
   afAddrInfo_t                           addrInfo;
   uint8_t                                zclTransactionId;
   zclCmdPrepayment_GetCommand_t          cmdFrame;
}zclPrepayment_GetCommandRsp_t; 

typedef PACKED_STRUCT zclCmdPrepayment_PublishTopUpLogCommand_tag
{
  uint8_t commandIndex;
  uint8_t totalNoOfCommands;
  uint8_t topUpCodePayload[1];
} zclCmdPrepayment_PublishTopUpLogCommand_t;

typedef PACKED_STRUCT debtPayload_tag
{
  ZCLTime_t collectionTime;
  uint32_t  amountCollected;
  zclPrepaymentDebtTypeField_t debtType;
  uint8_t  outstandingDebt;
  
}debtPayload_t;

typedef PACKED_STRUCT zclCmdPrepayment_PublishDebtLogCommand_tag
{
  uint8_t commandIndex;
  uint8_t totalNoOfCommands;
  uint8_t debtPayload[1];
} zclCmdPrepayment_PublishDebtLogCommand_t;



/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
extern zbTmrTimerID_t gPrepaymentTimerID;
extern uint8_t mProposedSupplyStatus;
/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		void ZCL_PrepaymentServerInit(void)
 *
 * @brief	Initializes functionality on the Prepayment Server.
 *
 */
extern void       ZCL_PrepaymentServerInit(void);

/*!
 * @fn 		zbStatus_t ZCL_PrepaymentClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Prepayment client. 
 *
 */
extern zbStatus_t ZCL_PrepaymentClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZCL_PrepaymentClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Prepayment server. 
 *
 */
extern zbStatus_t ZCL_PrepaymentServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

#if gASL_ZclPrepayment_SupplyOptionals_d
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_SelAvailEmergCreditReq(zclPrepayment_SelAvailEmergCreditReq_t *pReq)
 *
 * @brief	Sends over-the-air a Select Available Emergency Credit frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_SelAvailEmergCreditReq(zclPrepayment_SelAvailEmergCreditReq_t *pReq);
#endif

#if !gASL_ZclSE_12_Features_d && gASL_ZclPrepayment_SupplyOptionals_d
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_ChangeSupplyReq(zclPrepayment_ChangeSupplyReq_t *pReq)
 *
 * @brief	Sends over-the-air a Change Supply frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_ChangeSupplyReq(zclPrepayment_ChangeSupplyReq_t *pReq);
#endif

#if gASL_ZclSE_12_Features_d || gASL_ZclPrepayment_SupplyOptionals_d
/*!
 * @fn 		zbStatus_t zclPrepayment_Client_ChangeSupplyReq(zclPrepayment_ChangeSupplyReq_t *pReq)
 *
 * @brief	Sends over-the-air a Change Debt frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_ChangeDebtReq(zclPrepayment_ChangeDebtReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrepayment_Client_ChangeSupplyReq(zclPrepayment_ChangeSupplyReq_t *pReq)
 *
 * @brief	Sends over-the-air an Emergency Credit Setup frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_EmergencyCreditSetupReq(zclPrepayment_EmergencyCreditSetupReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrepayment_Client_ConsumerTopUpReq(zclPrepayment_ConsumerTopUpReq_t *pReq)
 *
 * @brief	Sends over-the-air a Consumer Top Up frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_ConsumerTopUpReq(zclPrepayment_ConsumerTopUpReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrepayment_Client_CreditAdjustmentReq(zclPrepayment_CreditAdjustmentReq_t *pReq)
 *
 * @brief	Sends over-the-air a Credit Adjustment frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_CreditAdjustmentReq(zclPrepayment_CreditAdjustmentReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrepayment_Client_ChangePaymentModeReq(zclPrepayment_ChangePaymentModeReq_t *pReq)
 *
 * @brief	Sends over-the-air a Change Payment Mode frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_ChangePaymentModeReq(zclPrepayment_ChangePaymentModeReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrepayment_Client_GetPrepaySnapshotReq(zclPrepayment_GetPrepaySnapshotReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Prepay Snapshot frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_GetPrepaySnapshotReq(zclPrepayment_GetPrepaySnapshotReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrepayment_Client_GetTopUpLogReq(zclPrepayment_GetTopUpLogReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Top Up Log frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_GetTopUpLogReq(zclPrepayment_GetTopUpLogReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrepayment_Client_SetLowCreditWarningLevelReq(zclPrepayment_SetLowCreditWarningLevelReq_t *pReq)
 *
 * @brief	Sends over-the-air a Set Low Credit Warning Level frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_SetLowCreditWarningLevelReq(zclPrepayment_SetLowCreditWarningLevelReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrepayment_Client_GetDebtRepaymentLogReq(zclPrepayment_GetDebtRepaymentLogReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Debt Repayment Log frame from the Prepayment client. 
 *
 */
extern zbStatus_t zclPrepayment_Client_GetDebtRepaymentLogReq(zclPrepayment_GetDebtRepaymentLogReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclPrepayment_Server_GetCommandRsp(zclPrepayment_GetCommandRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Get Command frame from the Prepayment server. 
 *
 */
extern zbStatus_t zclPrepayment_Server_GetCommandRsp(zclPrepayment_GetCommandRsp_t *pReq);
#endif
#ifdef __cplusplus
}
#endif
#endif /* _ZCL_PREPAYMENT_H */
