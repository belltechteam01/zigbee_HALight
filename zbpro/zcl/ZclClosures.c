/*! @file 	  ZclClosures.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for ZCL Closures domain.
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
 *			list of conditions and the following disclaimer in the documentation and/or
 *			other materials provided with the distribution.
 *
 *			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *			contributors may be used to endorse or promote products derived from this
 *			software without specific prior written permission.
 *
 *			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *			ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *			WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 *			ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *			(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *			LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *			ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *			(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *			SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
 /* 
 *  [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
 *  [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf 
 */
 
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZclFoundation.h"
#include "HaProfile.h"
#include "ZclClosures.h"

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
zbStatus_t DoorLock_CheckRFIDPINCode(zclCmdDoorLockReq_t *pRFIDCode, bool_t requireScheduledCheck);
static bool_t DoorLockCluster_LockUnlock(uint8_t lockUnlockAttr);
#if gZclClusterOptionals_d 
static bool_t DoorLock_CheckWeekDayScheduled(uint8_t indexInWeekDaySchedule);
static bool_t DoorLock_CheckYearDayScheduled(uint8_t indexInYearDaySchedule);
static void DoorLockTimerCallback(uint8_t tmr);
static zbStatus_t DoorLockCluster_ProcessGetLogRecord(uint8_t logIndex, afAddrInfo_t addrInfo, uint8_t transactionId);
static zbStatus_t DoorLockCluster_ProcessSetPinRfidCode(zclCmdDoorLockSetPinCode_t *pPinCodeInf, bool_t requireScheduledCheck, bool_t isRFIDCode);
static zbStatus_t DoorLockCluster_ProcessGetPinRFIDCode(uint16_t userID, afAddrInfo_t addrInfo, uint8_t transactionId, bool_t isRFIDCode);
static zbStatus_t DoorLockCluster_ProcessClearPinRfidCode(uint16_t userID, bool_t isRFIDCode);
static zbStatus_t DoorLockCluster_ProcessClearAllPinRfidCodes(bool_t isRFIDCode);
static zbStatus_t DoorLockCluster_ProcessSetUserType(zclCmdDoorLockSetUserType_t userData);
static zbStatus_t DoorLockCluster_ProcessGetUserType(uint16_t userID, afAddrInfo_t addrInfo, uint8_t transactionId);
static zbStatus_t DoorLockCluster_ProcessGetUserStatus(uint16_t userID, afAddrInfo_t addrInfo, uint8_t transactionId);
static zbStatus_t DoorLockCluster_ProcessSetUserStatus(zclCmdDoorLockSetUserStatus_t userData);
static zbStatus_t DoorLockCluster_ProcessSetWeekDaySchedule(zclCmdDoorLockSetWeekDaySchedule_t data);
static zbStatus_t DoorLockCluster_ProcessGetWeekDaySchedule(zclCmdDoorLockGetWeekDaySchedule_t scheduleInf, afAddrInfo_t addrInfo, uint8_t transactionId);
static zbStatus_t DoorLockCluster_ProcessClearWeekDaySchedule(zclCmdDoorLockGetWeekDaySchedule_t scheduleInf);
static zbStatus_t DoorLockCluster_ProcessSetYearDaySchedule(zclCmdDoorLockSetYearDaySchedule_t scheduleInf);
static zbStatus_t DoorLockCluster_ProcessGetYearDaySchedule(zclCmdDoorLockGetYearDaySchedule_t scheduleInf, afAddrInfo_t addrInfo, uint8_t transactionId);
static zbStatus_t DoorLockCluster_ProcessClearYearDaySchedule(zclCmdDoorLockGetYearDaySchedule_t scheduleInf);
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

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/******************************
  Shade Configuration Cluster 
  See ZCL Specification Section 7.2
*******************************/
/* Shade Configuration Cluster Attribute Definitions */
const zclAttrDef_t gaZclShadeCfgClusterAttrDef[] = {
   /*Attributes of the Shade Information attribute set */
#if gZclClusterOptionals_d
  { gZclAttrShadeCfgIdInfPhysicalClosedLimit_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclShadeCfgAttrsRAM_t, physicalClosedLimit)},
  { gZclAttrShadeCfgIdInfMotorStepSize_c,       gZclDataTypeUint8_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclShadeCfgAttrsRAM_t, motorStepSize)},
#endif
  { gZclAttrShadeCfgIdInfStatus_c, gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclShadeCfgAttrsRAM_t, status)},
  /*Attributes of the Shade Settings attribute set */
  { gZclAttrShadeCfgIdStgClosedLimit_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclShadeCfgAttrsRAM_t, closedLimit)},
  { gZclAttrShadeCfgIdStgMode_c,        gZclDataTypeEnum8_c,  gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *) MbrOfs(zclShadeCfgAttrsRAM_t, mode)}
};


const zclAttrSet_t gaZclShadeCfgClusterAttrSet[] = {
  {gZclAttrShadeCfgIdSet_c, (void *)&gaZclShadeCfgClusterAttrDef, NumberOfElements(gaZclShadeCfgClusterAttrDef)}
};

const zclAttrSetList_t gZclShadeCfgClusterAttrSetList = {
  NumberOfElements(gaZclShadeCfgClusterAttrSet),
  gaZclShadeCfgClusterAttrSet
};


/******************************
  Door Lock Cluster 
  See ZCL Specification Section 7.3
*******************************/
/* Door Lock Cluster Attribute Definitions */
const zclAttrDef_t gaZclDoorLockClusterAttrDef[] = {
  { gZclAttrIdDoorLockLockState_c,  	gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsReportable_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, lockState)},
  { gZclAttrIdDoorLockLockType_c,   	gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, lockType)},
  { gZclAttrIdDoorLockActuatorEnable_c, gZclDataTypeBool_c,  gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) MbrOfs(zclDoorLockAttrsRAM_t, actuatorEnabled)}
#if gZclClusterOptionals_d
  , { gZclAttrIdDoorLockDoorState_c,   		gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, doorState)},
  { gZclAttrIdDoorLockDoorOpenEvents_c,  	gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c , sizeof( uint32_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, doorOpenEvents)},
  { gZclAttrIdDoorLockDoorClosedEvents_c,   gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c , sizeof(uint32_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, doorClosedEvents)},
  { gZclAttrIdDoorLockDoorOpenPeriod_c,  	gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c , sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, doorOpenPeriod)},
  /* 10.1.2.11 User, PIN, Schedule, Log Information Attribute Set */
  { gZclAttrIdDoorLockNoOfLogReqSupported_c,  		gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, noOfLogRecordSupported)},
  { gZclAttrIdDoorLockNoOfTotalUsersSupported_c,   	gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, noOfTotalUsersSupported)},
  { gZclAttrIdDoorLockNoOfPinUsersSupported_c,  	gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, noOfPinUsersSupported)},
  { gZclAttrIdDoorLockNoOfRFIDUsersSupported_c, 	gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, noOfRFIDUsersSupported)},
  { gZclAttrIdDoorLockNoOfWeekDaySchedulesSupportedPerUser_c,   gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, noOfWeekDayScheduledPerUser)},
  { gZclAttrIdDoorLockNoOfYearDaySchedulesSupportedPerUser_c,   gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, noOfYearDayScheduledPerUser)},
  { gZclAttrIdDoorLockNoOfHolidaySchedulesSupported_c,   		gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, noOfHolidaySchedulesSupported)},
  { gZclAttrIdDoorLockMaxPinCodeLength_c,   gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, maxPinCodeLength)},
  { gZclAttrIdDoorLockMinPinCodeLength_c,  	gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, minPinCodeLength)},
  { gZclAttrIdDoorLockMaxRfidCodeLength_c,  gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, maxRFIDCodeLength)},
  { gZclAttrIdDoorLockMinRfidCodeLength_c,  gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, minRFIDCodeLength)},
  /* 10.1.2.12 Operational Settings Attribute Set */
  { gZclAttrIdDoorLockEnableLogging_c, 			 gZclDataTypeBool_c,     gZclAttrFlagsInRAM_c , sizeof(uint8_t), 	 (void *) MbrOfs(zclDoorLockAttrsRAM_t, enableLogging)},
  { gZclAttrIdDoorLockLanguage_c,  				 gZclDataTypeStr_c,      gZclAttrFlagsInRAM_c , sizeof(zclStr2Oct_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, language)},
  { gZclAttrIdDoorLockLedSettings_c,   			 gZclDataTypeUint8_c,    gZclAttrFlagsInRAM_c , sizeof(uint8_t), (void *) MbrOfs(zclDoorLockAttrsRAM_t, ledSettings)},
  { gZclAttrIdDoorLockAutoRelockTime_c, 		 gZclDataTypeUint32_c,   gZclAttrFlagsInRAM_c , sizeof(uint32_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, autoRelockTime)},
  { gZclAttrIdDoorLockSoundVolume_c,   			 gZclDataTypeUint8_c,    gZclAttrFlagsInRAM_c , sizeof(uint8_t), (void *) MbrOfs(zclDoorLockAttrsRAM_t, soundVolume)},
  { gZclAttrIdDoorLockOperatingMode_c,  		 gZclDataTypeEnum8_c,    gZclAttrFlagsInRAM_c , sizeof(uint8_t), (void *) MbrOfs(zclDoorLockAttrsRAM_t, operatingMode)},
  { gZclAttrIdDoorLockSupportedOperatingModes_c, gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, supportedOperatingMode)},
  { gZclAttrIdDoorLockDefaultConfigRegister_c,   gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, defaultConfigRegister)},
  { gZclAttrIdDoorLockEnableLocalProgramming_c,  gZclDataTypeBool_c,   	 gZclAttrFlagsInRAM_c , sizeof(uint8_t), (void *) MbrOfs(zclDoorLockAttrsRAM_t, enableLocalProgramming)},
  { gZclAttrIdDoorLockEnableOneTouchLocking_c,   gZclDataTypeBool_c,   	 gZclAttrFlagsInRAM_c , sizeof(uint8_t), (void *) MbrOfs(zclDoorLockAttrsRAM_t, enableOneTouchLocking)},
  { gZclAttrIdDoorLockEnableInsideStatusLED_c,   gZclDataTypeBool_c,     gZclAttrFlagsInRAM_c , sizeof(uint8_t), (void *) MbrOfs(zclDoorLockAttrsRAM_t, enableInsideStatusLed)},
  { gZclAttrIdDoorLockEnablePrivacyModeButton_c, gZclDataTypeBool_c,     gZclAttrFlagsInRAM_c , sizeof(uint8_t), (void *) MbrOfs(zclDoorLockAttrsRAM_t, enablePrivacyModeButton)},
  /* 10.1.2.13 Security Settings Attribute Set */
  { gZclAttrIdDoorLockWrongCodeEntryLimit_c,  			gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c , sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, wrongCodeEntryLimit)},
  { gZclAttrIdDoorLockUserCodeTemporaryDisableTime_c,   gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c , sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, userCodeTemporaryDisableTime)},
  { gZclAttrIdDoorLockSendPinOverTheAir_c,   			gZclDataTypeBool_c,  gZclAttrFlagsInRAM_c , sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, sendPinOta)},
  { gZclAttrIdDoorLockRequirePinForRFOperation_c,  		gZclDataTypeBool_c,  gZclAttrFlagsInRAM_c , sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, requirePinRFOperation)},
  { gZclAttrIdDoorLockZigbeeSecurityLevel_c,  			gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c , sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, zigBeeSecurityLevel)},
  /* 10.1.2.14 Alarm and Event Masks Attribute Set */
  { gZclAttrIdDoorLockAlarmMask_c,  					gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c , sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, alarmMask)},
  { gZclAttrIdDoorLockKeypadOperationEventMask_c,   	gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c , sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, keypadOperationEventMask)},
  { gZclAttrIdDoorLockRFOperationEventMask_c,   		gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c , sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, rfOperationEventMask)},
  { gZclAttrIdDoorLockManualOperationEventMask_c,  		gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c , sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, manualOperationEventMask)},
  { gZclAttrIdDoorLockRFIDOperationEventMask_c,   		gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c , sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, rfidOperarionEventMask)},
  { gZclAttrIdDoorLockKeypadProgrammingEventMask_c,   	gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c , sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, keypadProgrammingEventMask)},
  { gZclAttrIdDoorLockRFProgrammingEventMask_c,  		gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c , sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, rfProgrammingEventMask)},
  { gZclAttrIdDoorLockRFIDProgrammingEventMask_c,   	gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c , sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, doorClosedEvents)},
#endif
};

const zclAttrSet_t gaZclDoorLockClusterAttrSet[] = {
  {gZclAttrSetDoorLock_c, (void *)&gaZclDoorLockClusterAttrDef, NumberOfElements(gaZclDoorLockClusterAttrDef)}
};

const zclAttrSetList_t gZclDoorLockClusterAttrSetList = {
  NumberOfElements(gaZclDoorLockClusterAttrSet),
  gaZclDoorLockClusterAttrSet
};

const zclCmd_t gaZclDoorLockClusterCmdReceivedDef[]={
  // commands received 
  gZclCmdDoorLock_Lock_c, gZclCmdDoorLock_Unlock_c, gZclCmdDoorLock_Toggle_c, gZclCmdDoorLock_UnlockWithTimeout_c,
  gZclCmdDoorLock_GetLogRecord_c, gZclCmdDoorLock_SetPinCode_c, gZclCmdDoorLock_GetPinCode_c, gZclCmdDoorLock_ClearPinCode_c,
  gZclCmdDoorLock_ClearAllPinCodes_c, gZclCmdDoorLock_SetUserStatus_c, gZclCmdDoorLock_GetUserStatus_c, gZclCmdDoorLock_SetWeekdaySchedule_c,
  gZclCmdDoorLock_GetWeekdaySchedule_c, gZclCmdDoorLock_ClearWeekdaySchedule_c, gZclCmdDoorLock_SetYeardaySchedule_c, gZclCmdDoorLock_GetYeardaySchedule_c,
  gZclCmdDoorLock_ClearYeardaySchedule_c, gZclCmdDoorLock_SetHolidaySchedule_c, gZclCmdDoorLock_GetHolidaySchedule_c, gZclCmdDoorLock_ClearHolidaySchedule_c,
  gZclCmdDoorLock_SetUserType_c, gZclCmdDoorLock_GetUserType_c, gZclCmdDoorLock_SetRFIDCode_c, gZclCmdDoorLock_GetRFIDCode_c,
  gZclCmdDoorLock_ClearRFIDCode_c, gZclCmdDoorLock_ClearAllRFIDCodes_c
};
const zclCmd_t gaZclDoorLockClusterCmdGeneratedDef[]={
  // commands generated 
  gZclCmdDoorLock_LockRsp_c, gZclCmdDoorLock_UnlockRsp_c, gZclCmdDoorLock_ToggleRsp_c,  gZclCmdDoorLock_UnlockWithTimeoutRsp_c,
  gZclCmdDoorLock_GetLogRecordRsp_c,  gZclCmdDoorLock_SetPinCodeRsp_c, gZclCmdDoorLock_GetPinCodeRsp_c,  gZclCmdDoorLock_ClearPinCodeRsp_c,
  gZclCmdDoorLock_ClearAllPinCodesRsp_c,  gZclCmdDoorLock_SetUserStatusRsp_c, gZclCmdDoorLock_GetUserStatusRsp_c, gZclCmdDoorLock_SetWeekdayScheduleRsp_c ,
  gZclCmdDoorLock_GetWeekdayScheduleRsp_c, gZclCmdDoorLock_ClearWeekdayScheduleRsp_c, gZclCmdDoorLock_SetYeardayScheduleRsp_c, gZclCmdDoorLock_GetYeardayScheduleRsp_c,
  gZclCmdDoorLock_ClearYeardayScheduleRsp_c, gZclCmdDoorLock_SetUserTypeRsp_c, gZclCmdDoorLock_GetUserTypeRsp_c, gZclCmdDoorLock_SetRFIDCodeRsp_c, gZclCmdDoorLock_GetRFIDCodeRsp_c,
  gZclCmdDoorLock_ClearRFIDCodeRsp_c, gZclCmdDoorLock_ClearAllRFIDCodesRsp_c, gZclCmdDoorLock_OperationEventNotification_c, gZclCmdDoorLock_ProgrammingEventNotification_c
};

const zclCommandsDefList_t gZclDoorLockClusterCommandsDefList =
{
   NumberOfElements(gaZclDoorLockClusterCmdReceivedDef),  gaZclDoorLockClusterCmdReceivedDef,
   NumberOfElements(gaZclDoorLockClusterCmdGeneratedDef), gaZclDoorLockClusterCmdGeneratedDef
};

/* no of users available on Server */
uint8_t gDoorLock_CurrentNoOfUsers;	
/* server door lock users table */
zclDoorLock_usersTable_t gDoorLockUsersInformation[gZclDoorLock_NoOfTotalUsersSupported_c];
/* server door lock week day and year day schedule tables :*/
zclDoorLock_WeekDayScheduleTable_t gDoorLock_WeekDayScheduledInf[gZclDoorLock_NoOfTotalUsersSupported_c];
zclDoorLock_YearDayScheduleTable_t gDoorLock_YearDayScheduledInf[gZclDoorLock_NoOfTotalUsersSupported_c];
/* timer used to lock automatically after unlocking the door */
zbTmrTimerID_t gDoorLockAppTimer = gTmrInvalidTimerID_c;	
/* time attributes */
extern ZCLTimeServerAttrsRAM_t gZclTimeServerAttrs;

/******************************
  Window Covering Cluster 
  See Zigbee Home Automation Section 9.3
*******************************/
/* Window Covering Cluster Attribute Definitions */
const zclAttrDef_t gaZclWindowCoveringClusterAttrDef[] = {
  { gZclAttrIdWindowCoveringType_c,  					gZclDataTypeEnum8_c,   gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *) MbrOfs(zclWindowCoveringAttrsRAM_t, type)},
  { gZclAttrIdWindowCoveringConfigStatus_c,   			gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *) MbrOfs(zclWindowCoveringAttrsRAM_t, configStatus)},
  { gZclAttrIdWindowCoveringMode_c, 					gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *) MbrOfs(zclWindowCoveringAttrsRAM_t, mode)}
#if gZclClusterOptionals_d
  ,{ gZclAttrIdWindowCoveringInstalledOpenLimitLift_c,	gZclDataTypeUint16_c,  gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclWindowCoveringAttrsRAM_t, installedOpenLimitLift)},
  { gZclAttrIdWindowCoveringInstalledClosedLimitLift_c, gZclDataTypeUint16_c,  gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclWindowCoveringAttrsRAM_t, installedClosedLimitLift)},
  { gZclAttrIdWindowCoveringInstalledOpenLimitTilt_c, 	gZclDataTypeUint16_c,  gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclWindowCoveringAttrsRAM_t, installedOpenLimitTilt)},
  { gZclAttrIdWindowCoveringInstalledClosedLimitTilt_c, gZclDataTypeUint16_c,  gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclWindowCoveringAttrsRAM_t, installedClosedLimitTilt)},
  { gZclAttrIdWindowCoveringPhysicalClosedLimitLift_c,gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c,  sizeof(uint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, physicalClosedLimitLift)},
  { gZclAttrIdWindowCoveringPhysicalClosedLimitTilt_c,  gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c,  sizeof(uint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, physicalClosedLimitTilt)},
  { gZclAttrIdWindowCoveringCurrentPositionLift_c,   	gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c | gZclAttrFlagsInSceneTable_c,  sizeof(uint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, currentPositionLift)},
  { gZclAttrIdWindowCoveringCurrentPositionTilt_c,  	gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c | gZclAttrFlagsInSceneTable_c,  sizeof(uint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, currentPositionTilt)},
  { gZclAttrIdWindowCoveringNoOfActuationsLift_c,   	gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c,  sizeof(uint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, noOfActuationsLift)},
  { gZclAttrIdWindowCoveringNoOfActuationsTilt_c,  		gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c,  sizeof(uint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, noOfActuationsTilt)},
  { gZclAttrIdWindowCoveringCurrentPositionLiftPercentage_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsInSceneTable_c, sizeof(uint8_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, currentPositionLiftPercentage)},
  { gZclAttrIdWindowCoveringCurrentPositionTiltPercentage_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsInSceneTable_c, sizeof(uint8_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, currentPositionTiltPercentage)},
  { gZclAttrIdWindowCoveringVelocityLift_c,  			gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, velocity)},
  { gZclAttrIdWindowCoveringAccelerationTimeLift_c,   	gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, accelerationTimeLift)},
  { gZclAttrIdWindowCoveringDecelerationTimeLift_c,  	gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, decelerationTimeLift)},
  { gZclAttrIdWindowCoveringIntermediateSetpointsLift_c,gZclDataTypeArray_c, gZclAttrFlagsInRAM_c, sizeof(zclArrayUint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, intermediateSetpointsLift)},
  { gZclAttrIdWindowCoveringIntermediateSetpointsTilt_c,gZclDataTypeArray_c, gZclAttrFlagsInRAM_c, sizeof(zclArrayUint16_t),(void *) MbrOfs(zclWindowCoveringAttrsRAM_t, intermediateSetpointsTilt)}

#endif
};

const zclAttrSet_t gaZclWindowCoveringClusterAttrSet[] = {
  {gZclAttrSetWindowCovering_c, (void *)&gaZclWindowCoveringClusterAttrDef, NumberOfElements(gaZclWindowCoveringClusterAttrDef)}
};

const zclAttrSetList_t gZclWindowCoveringClusterAttrSetList = {
  NumberOfElements(gaZclWindowCoveringClusterAttrSet),
  gaZclWindowCoveringClusterAttrSet
};


const zclCmd_t gaZclWindowCoveringClusterCmdReceivedDef[]={
  // commands received 
  gZclCmdWindowCovering_UpOpen_c,  				/* M - Up/Open */
  gZclCmdWindowCovering_DownClose_c, 			/* M - Down/Close */
  gZclCmdWindowCovering_Stop_c, 				/* M - Stop */
  gZclCmdWindowCovering_GoToLiftValue_c, 		/* O - Go To Lift Value */
  gZclCmdWindowCovering_GoToLiftPercentage_c,  	/* O - Go To Lift Percentage */
  gZclCmdWindowCovering_GoToTiltValue_c,  		/* O - Go To Tilt Value */
  gZclCmdWindowCovering_GoToTiltPercentage_c    /* O - Go To Tilt Percentage */
};

const zclCommandsDefList_t gZclWindowCoveringClusterCommandsDefList =
{
   NumberOfElements(gaZclWindowCoveringClusterCmdReceivedDef), gaZclWindowCoveringClusterCmdReceivedDef,
   0, NULL
};


/******************************
  Shade Configuration Cluster 
  See ZCL Specification Section 7.2
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_ShadeCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Shade Configuration Cluster Server. 
 *
 */
zbStatus_t ZCL_ShadeCfgClusterServer
(
	zbApsdeDataIndication_t *pIndication,
	afDeviceDef_t *pDevice
)
{
	/* avoid compiler warnings */
	(void)pIndication;
	(void)pDevice;
	/* The comand use for this cluster are the read/write attributes */
	
	return gZclUnsupportedClusterCommand_c;
}

/*!
 * @fn 		zbStatus_t ZCL_ShadeCfgClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Shade Configuration Cluster Client. 
 *
 */
zbStatus_t ZCL_ShadeCfgClusterClient
(
        zbApsdeDataIndication_t *pIndication, 
        afDeviceDef_t *pDevice
)
{
        /* avoid compiler warnings */
	(void)pIndication;
	(void)pDevice;
	
	/* The comand use for this cluster are the read/write attributes */
	return gZclUnsupportedClusterCommand_c;  
}

/******************************
  Door Lock Cluster
  See ZCL Specification Section 3.8
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_DoorLockClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the DoorLock Cluster Server. 
 *
 */
zbStatus_t ZCL_DoorLockClusterServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
	
  afAddrInfo_t addrInfo;	
  uint8_t status = gZclSuccess_c;
  zclDoorLockStatusRsp_t  cmdRsp;
  uint8_t sendStatusRsp = FALSE;
  uint8_t lockUnlockAttr;						/* current value for lock/unlock attribute */
  uint8_t requirePinForRFOperation = FALSE;		/* require or not the Pin for RF notification attribute*/
  zclFrame_t *pFrame;
  uint8_t commandRspId;
  
  /* not used in this function */
  (void)pDevice;
  
  /* get the attribute */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrDoorLockLockState_c, gZclServerAttr_c, &lockUnlockAttr, NULL);
#if gZclClusterOptionals_d          
  (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrDoorLockRequirePinForRFOperation_c , gZclServerAttr_c, &requirePinForRFOperation,NULL);        
#endif 
  pFrame = (void *)(pIndication->pAsdu);
  /*Create the destination address*/
  AF_PrepareForReply(&addrInfo, pIndication);  
  
  switch (pFrame->command) {
    case gZclCmdDoorLock_Lock_c:
    {
        zclCmdDoorLockReq_t *pCmdPayload = (zclCmdDoorLockReq_t *)(pFrame+1);	    
    	cmdRsp.cmdFrame.statusCmd = gDoorLockStatus_Succes_c;	
        if(requirePinForRFOperation)
        { 
        	cmdRsp.cmdFrame.statusCmd = DoorLock_CheckRFIDPINCode(pCmdPayload, TRUE);  
        }
        if(cmdRsp.cmdFrame.statusCmd == gDoorLockStatus_Succes_c)
        {
        	lockUnlockAttr = gDoorLockStateLocked;  /* lock */
        	(void)DoorLockCluster_LockUnlock(lockUnlockAttr);
        }
        commandRspId = gZclCmdDoorLock_LockRsp_c;
        sendStatusRsp = TRUE;
    	break;
    }
    case gZclCmdDoorLock_Unlock_c:
    {

        zclCmdDoorLockReq_t *pCmdPayload = (zclCmdDoorLockReq_t *)(pFrame+1);	
    	cmdRsp.cmdFrame.statusCmd = gDoorLockStatus_Succes_c;	        
        if(requirePinForRFOperation)
        { 
        	cmdRsp.cmdFrame.statusCmd = DoorLock_CheckRFIDPINCode(pCmdPayload, TRUE); 
        } 
            
        if(cmdRsp.cmdFrame.statusCmd == gDoorLockStatus_Succes_c)
        {
			#if gZclClusterOptionals_d 
        	{
            	uint32_t autoRelockTime = 0;
            	(void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrDoorLockAutoRelockTime_c, gZclServerAttr_c, &autoRelockTime, NULL);
            	autoRelockTime = OTA2Native32(autoRelockTime);
            	if(autoRelockTime)
            	{
            		if(gDoorLockAppTimer == gTmrInvalidTimerID_c)
            			gDoorLockAppTimer = ZbTMR_AllocateTimer(); 
              		ZbTMR_StartSecondTimer(gDoorLockAppTimer, (uint16_t)autoRelockTime, DoorLockTimerCallback);
            	}
            }
			#endif	
               	
            lockUnlockAttr = gDoorLockStateUnlocked;   /* unlock */
            (void)DoorLockCluster_LockUnlock(lockUnlockAttr);
        }  
    	commandRspId = gZclCmdDoorLock_UnlockRsp_c;
        sendStatusRsp = TRUE;
    	break;
    }
#if gZclClusterOptionals_d       
    case gZclCmdDoorLock_Toggle_c:
    {
        zclCmdDoorLockReq_t *pCmdPayload = (zclCmdDoorLockReq_t *)(pFrame+1);	    
    	cmdRsp.cmdFrame.statusCmd = gDoorLockStatus_Succes_c;	
        if(requirePinForRFOperation)
        { 
        	cmdRsp.cmdFrame.statusCmd = DoorLock_CheckRFIDPINCode(pCmdPayload, TRUE); 
        }   
        if(cmdRsp.cmdFrame.statusCmd == gDoorLockStatus_Succes_c)
        {
        	lockUnlockAttr =  (lockUnlockAttr== gDoorLockStateLocked)?gDoorLockStateUnlocked:gDoorLockStateLocked;
        	(void)DoorLockCluster_LockUnlock(lockUnlockAttr);
        	if(lockUnlockAttr == gDoorLockStateUnlocked)
        	{
				#if gZclClusterOptionals_d 
        		{
        			uint32_t autoRelockTime = 0;
        			(void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrDoorLockAutoRelockTime_c, gZclServerAttr_c, &autoRelockTime, NULL);
        			autoRelockTime = OTA2Native32(autoRelockTime);
        			if(autoRelockTime)
        			{
        				if(gDoorLockAppTimer == gTmrInvalidTimerID_c)
        					gDoorLockAppTimer = ZbTMR_AllocateTimer(); 
        				ZbTMR_StartSecondTimer(gDoorLockAppTimer, (uint16_t)autoRelockTime, DoorLockTimerCallback);
        			}
        		}
				#endif	
        	}
        	
        }
    	commandRspId = gZclCmdDoorLock_ToggleRsp_c;
        sendStatusRsp = TRUE;
    	break;    
    }
    case gZclCmdDoorLock_UnlockWithTimeout_c:
    {
    	zclCmdDoorUnlockWithTimeout_t *pCmdPayload = (zclCmdDoorUnlockWithTimeout_t *)(pFrame+1);	
    	cmdRsp.cmdFrame.statusCmd = gDoorLockStatus_Succes_c;	 
        if(requirePinForRFOperation)
        { 
        	cmdRsp.cmdFrame.statusCmd = DoorLock_CheckRFIDPINCode(&pCmdPayload->pinInformation, TRUE); 
        } 
        if(cmdRsp.cmdFrame.statusCmd == gDoorLockStatus_Succes_c)
        {            
              lockUnlockAttr = gDoorLockStateUnlocked;  /* unlock */
              (void)DoorLockCluster_LockUnlock(lockUnlockAttr);
        }    	
		if(gDoorLockAppTimer == gTmrInvalidTimerID_c)
		{
			gDoorLockAppTimer = ZbTMR_AllocateTimer(); 
		    if(gDoorLockAppTimer == gTmrInvalidTimerID_c)
		    	cmdRsp.cmdFrame.statusCmd = gDoorLockStatus_Failed_c;
		}
		if(cmdRsp.cmdFrame.statusCmd == gZclSuccess_c)
		{
			pCmdPayload->timeout = OTA2Native16(pCmdPayload->timeout);
			ZbTMR_StartSecondTimer(gDoorLockAppTimer, pCmdPayload->timeout, DoorLockTimerCallback);
		}
		commandRspId = gZclCmdDoorLock_UnlockWithTimeoutRsp_c;
        sendStatusRsp = TRUE;
    	break;
    }
    case gZclCmdDoorLock_GetLogRecord_c:
    {
    	zclCmdDoorLockGetLogRecord_t cmdPayload = *(zclCmdDoorLockGetLogRecord_t *)(pFrame+1);
    	status = DoorLockCluster_ProcessGetLogRecord(cmdPayload.logIndex, addrInfo, pFrame->transactionId);
    	break;
    }
    case gZclCmdDoorLock_SetPinCode_c:
    {
    	zclCmdDoorLockSetPinCode_t	*pCmdPayload = (zclCmdDoorLockSetPinCode_t *)(pFrame+1);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessSetPinRfidCode(pCmdPayload, FALSE, FALSE);
    	commandRspId = gZclCmdDoorLock_SetPinCodeRsp_c;
    	sendStatusRsp = TRUE;
    	break;
    }
    case gZclCmdDoorLock_GetPinCode_c:
    {
    	zclCmdDoorLockGetPinCode_t cmdPayload = *(zclCmdDoorLockGetPinCode_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	status = DoorLockCluster_ProcessGetPinRFIDCode(cmdPayload.userID, addrInfo, pFrame->transactionId, FALSE);
    	break;
    }
    case gZclCmdDoorLock_ClearPinCode_c:
    {
    	zclCmdDoorLockGetPinCode_t cmdPayload = *(zclCmdDoorLockGetPinCode_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessClearPinRfidCode(cmdPayload.userID, FALSE);
    	commandRspId = gZclCmdDoorLock_ClearPinCodeRsp_c;
    	sendStatusRsp = TRUE;
    	break;    	
    }
    case gZclCmdDoorLock_ClearAllPinCodes_c:
    {
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessClearAllPinRfidCodes(FALSE);
    	commandRspId = gZclCmdDoorLock_ClearAllPinCodesRsp_c;
    	sendStatusRsp = TRUE;
    	break;    	
    }
    case gZclCmdDoorLock_SetUserStatus_c:
    {
    	zclCmdDoorLockSetUserStatus_t cmdPayload = *(zclCmdDoorLockSetUserStatus_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessSetUserStatus(cmdPayload);
    	commandRspId = gZclCmdDoorLock_SetUserTypeRsp_c;
    	sendStatusRsp = TRUE;
    	break;  
    }
    case gZclCmdDoorLock_GetUserStatus_c:
    {
    	zclCmdDoorLockGetUserStatus_t cmdPayload = *(zclCmdDoorLockGetUserStatus_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessGetUserStatus(cmdPayload.userID, addrInfo, pFrame->transactionId);
    	break;
    }
    case gZclCmdDoorLock_SetWeekdaySchedule_c:
    {
    	zclCmdDoorLockSetWeekDaySchedule_t cmdPayload = *(zclCmdDoorLockSetWeekDaySchedule_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessSetWeekDaySchedule(cmdPayload);
    	commandRspId = gZclCmdDoorLock_SetWeekdayScheduleRsp_c;
    	sendStatusRsp = TRUE;
    	break;  
    }  	
    case gZclCmdDoorLock_GetWeekdaySchedule_c:
    {
    	zclCmdDoorLockGetWeekDaySchedule_t cmdPayload = *(zclCmdDoorLockGetWeekDaySchedule_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessGetWeekDaySchedule(cmdPayload, addrInfo, pFrame->transactionId);
    	break;
    }
    case gZclCmdDoorLock_ClearWeekdaySchedule_c:
    {
    	zclCmdDoorLockGetWeekDaySchedule_t cmdPayload = *(zclCmdDoorLockGetWeekDaySchedule_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessClearWeekDaySchedule(cmdPayload);
    	commandRspId = gZclCmdDoorLock_ClearWeekdayScheduleRsp_c;
    	sendStatusRsp = TRUE;
    	break;
    }
    case gZclCmdDoorLock_SetYeardaySchedule_c:
    {
    	zclCmdDoorLockSetYearDaySchedule_t cmdPayload = *(zclCmdDoorLockSetYearDaySchedule_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessSetYearDaySchedule(cmdPayload);
    	commandRspId = gZclCmdDoorLock_SetYeardayScheduleRsp_c;
    	sendStatusRsp = TRUE;
    	break;    
    }
    case gZclCmdDoorLock_GetYeardaySchedule_c:
    {
    	zclCmdDoorLockGetYearDaySchedule_t cmdPayload = *(zclCmdDoorLockGetYearDaySchedule_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessGetYearDaySchedule(cmdPayload, addrInfo, pFrame->transactionId);
    	break;
    }
    case gZclCmdDoorLock_ClearYeardaySchedule_c:
    {
    	zclCmdDoorLockGetYearDaySchedule_t cmdPayload = *(zclCmdDoorLockGetYearDaySchedule_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessClearYearDaySchedule(cmdPayload);
    	commandRspId = gZclCmdDoorLock_ClearYeardayScheduleRsp_c;
    	sendStatusRsp = TRUE;
    	break;
    }    	
    case gZclCmdDoorLock_SetUserType_c:
    {
    	zclCmdDoorLockSetUserType_t cmdPayload = *(zclCmdDoorLockSetUserType_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessSetUserType(cmdPayload);
    	commandRspId = gZclCmdDoorLock_SetUserTypeRsp_c;
    	sendStatusRsp = TRUE;
    	break;
    }
    case gZclCmdDoorLock_GetUserType_c:
    {
    	zclCmdDoorLockGetUserType_t cmdPayload = *(zclCmdDoorLockGetUserType_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessGetUserType(cmdPayload.userID, addrInfo, pFrame->transactionId);
    	break;
    }
    case gZclCmdDoorLock_SetRFIDCode_c:
    {
    	zclCmdDoorLockSetPinCode_t	*pCmdPayload = (zclCmdDoorLockSetPinCode_t *)(pFrame+1);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessSetPinRfidCode(pCmdPayload, FALSE, TRUE);
    	commandRspId = gZclCmdDoorLock_SetRFIDCodeRsp_c;
    	sendStatusRsp = TRUE;
    	break;   
    }
    case gZclCmdDoorLock_GetRFIDCode_c:
    {
    	zclCmdDoorLockGetPinCode_t cmdPayload = *(zclCmdDoorLockGetPinCode_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	status = DoorLockCluster_ProcessGetPinRFIDCode(cmdPayload.userID, addrInfo, pFrame->transactionId, TRUE);
    	break;
    }
    case gZclCmdDoorLock_ClearRFIDCode_c:
    {
    	zclCmdDoorLockGetPinCode_t cmdPayload = *(zclCmdDoorLockGetPinCode_t *)(pFrame+1);
    	cmdPayload.userID = OTA2Native16(cmdPayload.userID);
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessClearPinRfidCode(cmdPayload.userID, TRUE);
    	commandRspId = gZclCmdDoorLock_ClearRFIDCodeRsp_c;
    	sendStatusRsp = TRUE;
    	break;  
    }
    case gZclCmdDoorLock_ClearAllRFIDCodes_c:
    {
    	cmdRsp.cmdFrame.statusCmd = DoorLockCluster_ProcessClearAllPinRfidCodes(TRUE);
    	commandRspId = gZclCmdDoorLock_ClearAllRFIDCodesRsp_c;
    	sendStatusRsp = TRUE;
    	break;  
    }
    case gZclCmdDoorLock_SetHolidaySchedule_c:
    case gZclCmdDoorLock_GetHolidaySchedule_c:
    case gZclCmdDoorLock_ClearHolidaySchedule_c:
#endif    	
    default:
    	status = gZclUnsupportedClusterCommand_c;   
    	break;
  }
  if(sendStatusRsp)
  {
	  FLib_MemCpy(&cmdRsp.addrInfo, &addrInfo, sizeof(addrInfo));
	  cmdRsp.zclTransactionId = pFrame->transactionId;
	  /* respond back to caller */
	  status = zclDoorLock_StatusRsp(&cmdRsp, commandRspId);
  }
	  
  
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_DoorLockClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the DoorLock Cluster Client. 
 *
 */
zbStatus_t ZCL_DoorLockClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmdDoorLockStatusRsp_t command;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  uint8_t event = gZclUI_NoEvent_c;
 
  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	  status = gZclSuccess_c;
  
  FLib_MemCpy(&command ,(pFrame + 1), sizeof(zclCmdDoorLockStatusRsp_t));
  /* handle the command */
  switch(pFrame->command){
     case gZclCmdDoorLock_LockRsp_c:
     {
          if(command.statusCmd == gZclSuccess_c)
            event = gZclUI_Lock_c;
          break;
     }
     case gZclCmdDoorLock_UnlockRsp_c:
     {
          if(command.statusCmd == gZclSuccess_c)
             event = gZclUI_Unlock_c;
          break;
     }
#if gZclClusterOptionals_d    
     case gZclCmdDoorLock_ToggleRsp_c:
     case gZclCmdDoorLock_UnlockWithTimeoutRsp_c:	 
     case gZclCmdDoorLock_SetPinCodeRsp_c:
     case gZclCmdDoorLock_SetRFIDCodeRsp_c:
     {
    	 zclCmdDoorLockStatusRsp_t cmdPayload = *(zclCmdDoorLockStatusRsp_t *)(pFrame+1);
    	 if(cmdPayload.statusCmd == gDoorLockStatus_Succes_c)
    		 event = gZclUI_Enable_c;
    	 break;
     }     
     case gZclCmdDoorLock_GetLogRecordRsp_c:
     case gZclCmdDoorLock_GetPinCodeRsp_c:
     case gZclCmdDoorLock_ClearPinCodeRsp_c:
     case gZclCmdDoorLock_ClearAllPinCodesRsp_c:
     case gZclCmdDoorLock_SetUserStatusRsp_c:
     case gZclCmdDoorLock_GetUserStatusRsp_c:
     case gZclCmdDoorLock_SetWeekdayScheduleRsp_c:
     case gZclCmdDoorLock_GetWeekdayScheduleRsp_c:
     case gZclCmdDoorLock_ClearWeekdayScheduleRsp_c:
     case gZclCmdDoorLock_SetYeardayScheduleRsp_c:
     case gZclCmdDoorLock_GetYeardayScheduleRsp_c:
     case gZclCmdDoorLock_ClearYeardayScheduleRsp_c:
     case gZclCmdDoorLock_SetHolidayScheduleRsp_c:
     case gZclCmdDoorLock_GetHolidayScheduleRsp_c:
     case gZclCmdDoorLock_ClearHolidayScheduleRsp_c:
     case gZclCmdDoorLock_SetUserTypeRsp_c:
     case gZclCmdDoorLock_GetUserTypeRsp_c:
     case gZclCmdDoorLock_GetRFIDCodeRsp_c:    	 
     case gZclCmdDoorLock_ClearRFIDCodeRsp_c:
     case gZclCmdDoorLock_ClearAllRFIDCodesRsp_c:
     case gZclCmdDoorLock_OperationEventNotification_c:
     case gZclCmdDoorLock_ProgrammingEventNotification_c:
     	return status;
#endif     	
     default:
         return  gZclUnsupportedClusterCommand_c;    
  }
  BeeAppUpdateDevice(0, event, 0, 0, NULL);
  return status;
}

/*!
 * @fn 		zbStatus_t zclDoorLock_LockUnlockToogleReq(zclDoorLockReq_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a DoorLock(0x00)/Unlock(0x01)/Toogle(0x02) request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_LockUnlockToogleReq
(
  zclDoorLockReq_t *pReq,
  uint8_t commandId
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockReq_t)-1+pReq->cmdFrame.length;	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(commandId, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_UnlockWithTimeoutReq(zclDoorUnlockWithTimeout_t *pReq) 
 *
 * @brief	Sends over-the-air a UnlockWithTimeout request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_UnlockWithTimeoutReq
(
	zclDoorUnlockWithTimeout_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorUnlockWithTimeout_t)-1+pReq->cmdFrame.pinInformation.length;	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_UnlockWithTimeout_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetLogRecord(zclDoorLockGetLogRecord_t *pReq) 
 *
 * @brief	Sends over-the-air a GetLogRecord  from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetLogRecord
(
	zclDoorLockGetLogRecord_t *pReq
)  
{
	uint8_t payloadLen  = sizeof(zclCmdDoorLockGetLogRecord_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_GetLogRecord_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_SetPinRFIDCode(zclDoorLockSetPinRFIDCode_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a SetPinCode/SetRfidCode request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetPinRFIDCode
(
	zclDoorLockSetPinRFIDCode_t *pReq,
	uint8_t commandId
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockSetPinCode_t)-1+pReq->cmdFrame.pinInformation.length;	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(commandId, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetPinRFIDCode(zclDoorLockGetPinRFIDCode_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a GetPinCode/GetRfidCode request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetPinRFIDCode
(
	zclDoorLockGetPinRFIDCode_t *pReq,
	uint8_t commandID
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockGetPinCode_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(commandID, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_ClearPinRFIDCode(zclDoorLockClearPinRFIDCode_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a ClearPinCode/ClearRfidCode request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_ClearPinRFIDCode
(
	zclDoorLockClearPinRFIDCode_t *pReq,
	uint8_t commandID
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockGetPinCode_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_ClearPinCode_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_ClearAllPinRFIDCodes(zclDoorLockClearAllPinRFIDCodes_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a ClearAllPinCodes/ClearAllRfidCodes request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_ClearAllPinRFIDCodes
(
	zclDoorLockClearAllPinRFIDCodes_t *pReq,
	uint8_t	commandID
)  
{
	uint8_t payloadLen = 0;	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(commandID, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_SetUserType(zclDoorLockSetUserType_t *pReq)
 *
 * @brief	Sends over-the-air a SetUserType command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetUserType
(
	zclDoorLockSetUserType_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockSetUserType_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_SetUserType_c, payloadLen, (zclGenericReq_t *)pReq);
}


/*!
 * @fn 		zbStatus_t zclDoorLock_GetUserType(zclDoorLockGetUserType_t *pReq)
 *
 * @brief	Sends over-the-air a GetUserType command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetUserType
(
	zclDoorLockGetUserType_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockGetUserType_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_GetUserType_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_SetUserStatus(zclDoorLockSetUserStatus_t *pReq)
 *
 * @brief	Sends over-the-air a SetUserStatus command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetUserStatus
(
	zclDoorLockSetUserStatus_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockSetUserStatus_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_SetUserStatus_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetUserStatus(zclDoorLockGetUserStatus_t *pReq)
 *
 * @brief	Sends over-the-air a GetUserStatus command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetUserStatus
(
	zclDoorLockGetUserStatus_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockGetUserStatus_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_GetUserStatus_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_SetWeekDaySchedule(zclDoorLockSetWeekDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a SetWeekDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetWeekDaySchedule
(
	zclDoorLockSetWeekDaySchedule_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockSetWeekDaySchedule_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_SetWeekdaySchedule_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetWeekDaySchedule(zclDoorLockGetWeekDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a GetWeekDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetWeekDaySchedule
(
	zclDoorLockGetWeekDaySchedule_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockGetWeekDaySchedule_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_GetWeekdaySchedule_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_ClearWeekDaySchedule(zclDoorLockClearWeekDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a ClearWeekDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_ClearWeekDaySchedule
(
	zclDoorLockClearWeekDaySchedule_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockGetWeekDaySchedule_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_ClearWeekdaySchedule_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_SetYearDaySchedule(zclDoorLockSetYearDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a SetYearDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetYearDaySchedule
(
	zclDoorLockSetYearDaySchedule_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockSetYearDaySchedule_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_SetYeardaySchedule_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetYearDaySchedule(zclDoorLockGetYearDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a GetYearDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetYearDaySchedule
(
	zclDoorLockGetYearDaySchedule_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockGetYearDaySchedule_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_GetYeardaySchedule_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_ClearYearDaySchedule(zclDoorLockClearYearDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a ClearYearDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_ClearYearDaySchedule
(
	zclDoorLockClearYearDaySchedule_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockGetYearDaySchedule_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_ClearYeardaySchedule_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_SetHolidaySchedule(zclDoorLockSetHolidaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a SetHolidaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetHolidaySchedule
(
	zclDoorLockSetHolidaySchedule_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockSetHolidaySchedule_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_SetHolidaySchedule_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetHolidaySchedule(zclDoorLockGetHolidaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a GetHolidaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetHolidaySchedule
(
	zclDoorLockGetHolidaySchedule_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockGetHolidaySchedule_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_GetHolidaySchedule_c, payloadLen, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDoorLock_ClearHolidaySchedule(zclDoorLockClearHolidaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a ClearHolidaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_ClearHolidaySchedule
(
	zclDoorLockClearHolidaySchedule_t *pReq
)  
{
	uint8_t payloadLen = sizeof(zclCmdDoorLockGetHolidaySchedule_t);	
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdDoorLock_ClearHolidaySchedule_c, payloadLen, (zclGenericReq_t *)pReq);
}

 /*!
 * @fn 		zbStatus_t zclDoorLock_StatusRsp(zclDoorLockStatusRsp_t *pCommandRsp, uint8_t commandId) 
 *
 * @brief	Generic function used to send a LockRsp/UnlockRsp/ToogleRsp/UnlockWithTimeoutRsp/SetPinCodeRsp/ClearPinCodeRsp/
 *			ClearAllPinCodesRsp/SetUserStatusRsp/SetWeekDayScheduleRsp/ClearWeekDayScheduleRsp/SetYearDayScheduleRsp/ClearYearDayScheduleRsp/
 *			SetHolidayScheduleRsp/SetUserTypeRsp/ClearRFIDCodeRsp/ClearAllRfidCodesRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_StatusRsp
( 
  zclDoorLockStatusRsp_t *pCommandRsp, 
  uint8_t commandId
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = sizeof(zclCmdDoorLockStatusRsp_t);
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          commandId,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &pCommandRsp->zclTransactionId, 
                          &iPayloadLen,
                          (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
 
 return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetLogRecordRsp(zclDoorLockgetLogRecordRsp_t *pCommandRsp)
 *
 * @brief	Sends over-the-air a GetLogRecordRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetLogRecordRsp
( 
  zclDoorLockgetLogRecordRsp_t *pCommandRsp
)
{
   afToApsdeMessage_t *pMsg;
   uint8_t iPayloadLen = sizeof(zclCmdDoorLockGetLogRecordRsp_t) - 1 + pCommandRsp->cmdFrame.pinLength;
   pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
		   	   	   	   	   	 gZclCmdDoorLock_GetLogRecordRsp_c,
	                         gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
	                         &pCommandRsp->zclTransactionId, 
	                         &iPayloadLen,
	                         (uint8_t*)&(pCommandRsp->cmdFrame));
	if(!pMsg)
	  return gZclNoMem_c;
	 
   return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetPinRFIDCodeRsp(zclDoorLockGetPinRFIDCodeRsp_t *pCommandRsp, uint8_t commandID)
 *
 * @brief	Sends over-the-air a GetPinCodeRsp/GetRfidCodeRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetPinRFIDCodeRsp
( 
	zclDoorLockGetPinRFIDCodeRsp_t *pCommandRsp,
	uint8_t commandID
)
{
   afToApsdeMessage_t *pMsg;
   uint8_t iPayloadLen = sizeof(zclCmdDoorLockGetPinCodeRsp_t) - 1 + pCommandRsp->cmdFrame.pinLength;
   pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
		   	   	   	   	   	 commandID,
	                         gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
	                         &pCommandRsp->zclTransactionId, 
	                         &iPayloadLen,
	                         (uint8_t*)&(pCommandRsp->cmdFrame));
	if(!pMsg)
	  return gZclNoMem_c;
	 
   return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}


/*!
 * @fn 		zbStatus_t zclDoorLock_GetUserTypeRsp(zclDoorLockGetUserTypeRsp_t *pCommandRsp)
 *
 * @brief	Sends over-the-air a GetUserTypeRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetUserTypeRsp
( 
   zclDoorLockGetUserTypeRsp_t *pCommandRsp
)
{
   afToApsdeMessage_t *pMsg;
   uint8_t iPayloadLen = sizeof(zclCmdDoorLockGetUserTypeRsp_t);
   pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
		   	   	   	   	   	 gZclCmdDoorLock_GetUserTypeRsp_c,
	                         gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
	                         &pCommandRsp->zclTransactionId, 
	                         &iPayloadLen,
	                         (uint8_t*)&(pCommandRsp->cmdFrame));
	if(!pMsg)
	  return gZclNoMem_c;
	 
   return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetUserStatusRsp(zclDoorLockGetUserStatusRsp_t *pCommandRsp)
 *
 * @brief	Sends over-the-air a GetUserStatusRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetUserStatusRsp
( 
   zclDoorLockGetUserStatusRsp_t *pCommandRsp
)
{
   afToApsdeMessage_t *pMsg;
   uint8_t iPayloadLen = sizeof(zclCmdDoorLockGetUserStatusRsp_t);
   pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
		   	   	   	   	   	 gZclCmdDoorLock_GetUserStatusRsp_c,
	                         gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
	                         &pCommandRsp->zclTransactionId, 
	                         &iPayloadLen,
	                         (uint8_t*)&(pCommandRsp->cmdFrame));
	if(!pMsg)
	  return gZclNoMem_c;
	 
   return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetWeekDayScheduleRsp(zclDoorLockGetWeekDayScheduleRsp_t *pCommandRsp)
 *
 * @brief	Sends over-the-air a GetWeekDayScheduleRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetWeekDayScheduleRsp
( 
	zclDoorLockGetWeekDayScheduleRsp_t *pCommandRsp
)
{
   afToApsdeMessage_t *pMsg;
   uint8_t iPayloadLen;
   if(pCommandRsp->cmdFrame.status == gZclSuccess_c)
	   iPayloadLen = sizeof(zclCmdDoorLockGetWeekDayScheduleRsp_t);
   else
	   iPayloadLen = sizeof(uint16_t)+ 2; /*schedule id, userId, status*/
   
   pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
		   	   	   	   	     gZclCmdDoorLock_GetWeekdayScheduleRsp_c,
	                         gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
	                         &pCommandRsp->zclTransactionId, 
	                         &iPayloadLen,
	                         (uint8_t*)&(pCommandRsp->cmdFrame));
	if(!pMsg)
	  return gZclNoMem_c;
	 
   return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t zclDoorLock_GetYearDayScheduleRsp(zclDoorLockGetYearDayScheduleRsp_t *pCommandRsp)
 *
 * @brief	Sends over-the-air a GetYearDayScheduleRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetYearDayScheduleRsp
( 
	zclDoorLockGetYearDayScheduleRsp_t *pCommandRsp
)
{
   afToApsdeMessage_t *pMsg;
   uint8_t iPayloadLen;
   if(pCommandRsp->cmdFrame.status == gZclSuccess_c)
	   iPayloadLen = sizeof(zclCmdDoorLockGetYearDayScheduleRsp_t);
   else
	   iPayloadLen = sizeof(uint16_t)+ 2; /*schedule id, userId, status*/
   
   pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
		   	   	   	   	   	 gZclCmdDoorLock_GetYeardayScheduleRsp_c,
	                         gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
	                         &pCommandRsp->zclTransactionId, 
	                         &iPayloadLen,
	                         (uint8_t*)&(pCommandRsp->cmdFrame));
	if(!pMsg)
	  return gZclNoMem_c;
	 
   return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t DoorLock_CheckRFIDPINCode(zclCmdDoorLockReq_t *pCode, bool_t requireScheduledCheck)
 *
 * @brief	Check RFID/Pin code information
 *
 */
zbStatus_t DoorLock_CheckRFIDPINCode(zclCmdDoorLockReq_t *pCode, bool_t requireScheduledCheck)
{
	
  zbStatus_t status = gDoorLockStatus_Succes_c;
#if gZclClusterOptionals_d
  uint8_t i=0;	
  uint8_t minPinCodeLength = 0;
  uint8_t maxPinCodeLength = 0;
  uint8_t minRFIDCodeLength = 0;
  uint8_t maxRFIDCodeLength = 0;
  zbClusterId_t clusterId = {gaZclClusterDoorLock_c};
  uint8_t endPoint = 0xFF;
  endPoint = ZCL_GetEndPointForSpecificCluster(clusterId, TRUE, 0, NULL);
 
  status = gDoorLockStatus_Failed_c;
  if(endPoint == 0xFF)
	  return status;
  (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrDoorLockMaxRfidCodeLength_c , gZclServerAttr_c, &maxRFIDCodeLength,NULL);   
  (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrDoorLockMinRfidCodeLength_c , gZclServerAttr_c, &minRFIDCodeLength,NULL);   
  (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrDoorLockMaxPinCodeLength_c  , gZclServerAttr_c, &maxPinCodeLength,NULL);   
  (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrDoorLockMinPinCodeLength_c  , gZclServerAttr_c, &minPinCodeLength,NULL);  

  if((pCode->length < minPinCodeLength) || (pCode->length > maxPinCodeLength) ||(pCode->length < minRFIDCodeLength) || (pCode->length > maxRFIDCodeLength) ) 
	  return status;
  for(i=0; i< gZclDoorLock_NoOfTotalUsersSupported_c; i++)
  {
	  if((gDoorLockUsersInformation[i].userID != gZclDoorLock_InvalidUserId_c) && (gDoorLockUsersInformation[i].userType != gDoorLockUserType_NonAccessUser_c))
	  {
		  if((gDoorLockUsersInformation[i].rfidInf.length == pCode->length)&&
				  (FLib_MemCmp(pCode->pinCode, gDoorLockUsersInformation[i].rfidInf.pinCode, pCode->length)))
			  status =  gDoorLockStatus_Succes_c;
		  else
			  if((gDoorLockUsersInformation[i].pinInf.length == pCode->length)&&
				  (FLib_MemCmp(pCode->pinCode, gDoorLockUsersInformation[i].pinInf.pinCode, pCode->length)))
				  status = gDoorLockStatus_Succes_c;
	  }

	  if(status == gDoorLockStatus_Succes_c)
	  {
		  if(requireScheduledCheck)
		  {
			  if(gDoorLockUsersInformation[i].userType == gDoorLockUserType_WeekDayScheduleUser_c)
			  {
				  if(!DoorLock_CheckWeekDayScheduled(gDoorLockUsersInformation[i].indexInWeekDayScheduledTable))
					  status = gDoorLockStatus_Failed_c;
			  }
			  if(gDoorLockUsersInformation[i].userType == gDoorLockUserType_YearDayScheduleUser_c)
			  {
				  if(!DoorLock_CheckYearDayScheduled(gDoorLockUsersInformation[i].indexInYearDayScheduledTable))
					  status = gDoorLockStatus_Failed_c;
			  }			  

		  }
		  return status;
	  }
  }
#endif
  return status; 	        	  
}


/*!
 * @fn 		static bool_t DoorLock_CheckWeekDayScheduled(uint8_t indexInWeekDaySchedule)
 *
 * @brief	Check week day schedule informations. Return TRUE for valid weekDaySchedule, otherwise return FALSE.
 *
 */
#if gZclClusterOptionals_d 
static bool_t DoorLock_CheckWeekDayScheduled(uint8_t indexInWeekDaySchedule)
{
	uint8_t i=0;
	zclTime_GetTimeInf_t currentTimeInf = App_GetTimeInf(gZclTimeServerAttrs.LocalTime);
	bool_t status = FALSE;
	uint32_t minuteCurrentTime = currentTimeInf.timeHour*60 + currentTimeInf.timeMinute;
	uint32_t minuteStartTime = 0;
	uint32_t minuteEndTime = 0;
		
	if(gDoorLock_WeekDayScheduledInf[indexInWeekDaySchedule].slotStatus == FALSE || indexInWeekDaySchedule == gZclDoorLock_InvalidIndex_c)
		return status;
	for(i=0;i<gZclDoorLock_NoOfWeekDayScheduledPerUser_c; i++)
	{					
		/* check DayMask */
		if(gDoorLock_WeekDayScheduledInf[indexInWeekDaySchedule].weekDayScheduledInf[i].dayMask & (0x01<<currentTimeInf.timeWeekDay))
		{
			minuteStartTime = gDoorLock_WeekDayScheduledInf[indexInWeekDaySchedule].weekDayScheduledInf[i].startHour*60 + \
					gDoorLock_WeekDayScheduledInf[indexInWeekDaySchedule].weekDayScheduledInf[i].startMinute;
			minuteEndTime = gDoorLock_WeekDayScheduledInf[indexInWeekDaySchedule].weekDayScheduledInf[i].endHour*60 + \
					gDoorLock_WeekDayScheduledInf[indexInWeekDaySchedule].weekDayScheduledInf[i].endMinute;
			/* check Hours  and Minute */
			if(minuteCurrentTime >= minuteStartTime && minuteCurrentTime<=minuteEndTime)
			{
				status = TRUE;
				break;
			}
		}
			
	}
	return status;
}
#endif

/*!
 * @fn 		static bool_t DoorLock_CheckYearDayScheduled(uint8_t indexInYearDaySchedule)
 *
 * @brief	Check year day schedule informations. Return TRUE for valid yearDaySchedule, otherwise return FALSE.
 *
 */
#if gZclClusterOptionals_d
static bool_t DoorLock_CheckYearDayScheduled(uint8_t indexInYearDaySchedule)
{
	uint8_t i=0;
	bool_t status = FALSE;
	uint32_t localTime = OTA2Native32(gZclTimeServerAttrs.LocalTime);
				
	if(gDoorLock_YearDayScheduledInf[indexInYearDaySchedule].slotStatus == FALSE || indexInYearDaySchedule==gZclDoorLock_InvalidIndex_c)
		return status;
	for(i=0;i<gZclDoorLock_NoOfYearDayScheduledPerUser_c; i++)
	{					
		/* check DayMask */
		if(localTime >= gDoorLock_YearDayScheduledInf[indexInYearDaySchedule].yearDayScheduledInf[i].zigBeeLocalStartTime &&
				localTime <= gDoorLock_YearDayScheduledInf[indexInYearDaySchedule].yearDayScheduledInf[i].zigBeeLocalEndTime)
		{
			status = TRUE;
			break;
		}
	}
	return status;
}
#endif

/*!
 * @fn 		static void DoorLockTimerCallback(uint8_t tmr)
 *
 * @brief	Callback used to unlock the door
 *
 */
#if gZclClusterOptionals_d 
static void DoorLockTimerCallback(uint8_t tmr) 
{
	(void)tmr;
	(void)DoorLockCluster_LockUnlock(gDoorLockStateLocked);
	ZbTMR_FreeTimer(gDoorLockAppTimer);
	gDoorLockAppTimer = gTmrInvalidTimerID_c;
}
#endif

/*!
 * @fn 		static bool_t DoorLockCluster_LockUnlock(uint8_t lockUnlockAttr)
 *
 * @brief	Update the doorLock attributes and send current doorlock event state to the application
 *
 */
static bool_t DoorLockCluster_LockUnlock(uint8_t lockUnlockAttr)
{
#if gZclClusterOptionals_d 
  uint8_t doorStateAttr;
  uint32_t doorEvents = 0;
#endif  	
  uint8_t event;
  zbClusterId_t clusterId = {gaZclClusterDoorLock_c};
  uint8_t endPoint = 0xFF;
  endPoint = ZCL_GetEndPointForSpecificCluster(clusterId, TRUE, 0, NULL);
  
  if(lockUnlockAttr == gDoorLockStateLocked)  /* lock */
  {
	#if gZclClusterOptionals_d
	  doorStateAttr = gDoorStateClosed; /* door closed */  
	  (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrDoorLockDoorClosedEvents_c, gZclServerAttr_c, &doorEvents, NULL);
	  doorEvents = OTA2Native32(doorEvents);
	  doorEvents++;
	  doorEvents = Native2OTA32(doorEvents);
	  (void)ZCL_SetAttribute(endPoint, clusterId, gZclAttrDoorLockDoorState_c, gZclServerAttr_c, &doorStateAttr);
	  (void)ZCL_SetAttribute(endPoint, clusterId, gZclAttrDoorLockDoorClosedEvents_c , gZclServerAttr_c, &doorEvents);
	#endif     
	event = gZclUI_Lock_c;
  }
  else	/* unlock */
  {
    #if gZclClusterOptionals_d
      doorStateAttr = gDoorStateOpen; /* door open */
      (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrDoorLockDoorOpenEvents_c , gZclServerAttr_c, &doorEvents, NULL);
      doorEvents = OTA2Native32(doorEvents);
      doorEvents++;
      doorEvents = Native2OTA32(doorEvents);
      (void)ZCL_SetAttribute(endPoint, clusterId, gZclAttrDoorLockDoorState_c, gZclServerAttr_c, &doorStateAttr);
      (void)ZCL_SetAttribute(endPoint, clusterId, gZclAttrDoorLockDoorOpenEvents_c , gZclServerAttr_c, &doorEvents);
    #endif
      event = gZclUI_Unlock_c;
  }
  
  /* sets the attribute and will report if needed */
  (void)ZCL_SetAttribute(endPoint,clusterId, gZclAttrDoorLockLockState_c, gZclServerAttr_c, &lockUnlockAttr);
  /* send the event to the app */
  BeeAppUpdateDevice(endPoint, event, 0, 0, NULL);
 return TRUE; 	
}

/*!
 * @fn 		zbStatus_t DoorLockCluster_ProcessGetLogRecord(uint8_t logIndex, afAddrInfo_t addrInfo, uint8_t transactionId)
 *
 * @brief	Process the GetLogRecord command received from a DoorLock Client
 *
 */
#if gZclClusterOptionals_d
static zbStatus_t DoorLockCluster_ProcessGetLogRecord(uint8_t logIndex, afAddrInfo_t addrInfo, uint8_t transactionId)
{
  zbStatus_t status = gZclFailure_c;
  zclDoorLockgetLogRecordRsp_t *pCommandRsp;	

  pCommandRsp = AF_MsgAlloc(); 
    if(!pCommandRsp)
      return gZclNoMem_c;
  
  FLib_MemCpy(&pCommandRsp->addrInfo, &addrInfo, sizeof(addrInfo));
  pCommandRsp->zclTransactionId = transactionId;
 
 /*To Do: process log Index */
 /*                         */
 /* for test: 			    */
 FLib_MemSet(&pCommandRsp->cmdFrame, 0x00, sizeof(zclCmdDoorLockGetLogRecordRsp_t));
    
 /* respond back to caller  */	
 status =  zclDoorLock_GetLogRecordRsp(pCommandRsp);
 MSG_Free(pCommandRsp);
 
 return status;
}

/*!
 * @fn 		static zbStatus_t DoorLockCluster_ProcessSetPinRfidCode(zclCmdDoorLockSetPinCode_t *pPinCodeInf, bool_t requireScheduledCheck, bool_t isRFIDCode)
 *
 * @brief	Process the SetPinCode/SetRfidCode command received from a DoorLock Client
 *
 */
static zbStatus_t DoorLockCluster_ProcessSetPinRfidCode(zclCmdDoorLockSetPinCode_t *pPinCodeInf, bool_t requireScheduledCheck, bool_t isRFIDCode)
{
  uint8_t i = 0;
  zbStatus_t status = gDoorLockPinCodeStatus_MemoryFull_c;
  uint8_t currentPosition = gZclDoorLock_NoOfTotalUsersSupported_c;
  
  /* check the status */
  if(((pPinCodeInf->userStatus != gDoorLockUserStatus_Enabled_c) && (pPinCodeInf->userStatus != gDoorLockUserStatus_Disabled_c))
		  || (OTA2Native16(pPinCodeInf->userID) > gZclDoorLock_NoOfTotalUsersSupported_c))
  {
    return gDoorLockPinCodeStatus_GeneralFailure_c;
  }
  
  /* check the Rfid Pin Code */
  if(DoorLock_CheckRFIDPINCode(&pPinCodeInf->pinInformation, requireScheduledCheck) == gDoorLockStatus_Succes_c)
  {
 	return gDoorLockPinCodeStatus_DuplicateCodeError_c;
  }
  
  /* check if userId is or not in the door lock users table */
  for(i=0; i< gZclDoorLock_NoOfTotalUsersSupported_c; i++)
  {
	  if(gDoorLockUsersInformation[i].userID == OTA2Native16(pPinCodeInf->userID))
 		currentPosition = i;
  }
  
  if(currentPosition == gZclDoorLock_NoOfTotalUsersSupported_c)
  {
	  if(gDoorLock_CurrentNoOfUsers == gZclDoorLock_NoOfTotalUsersSupported_c)
		  return gDoorLockPinCodeStatus_MemoryFull_c;
	  else
	  {
		  currentPosition = gDoorLock_CurrentNoOfUsers;
		  gDoorLock_CurrentNoOfUsers++;
	  }
  }

 	
  gDoorLockUsersInformation[currentPosition].userID = OTA2Native16(pPinCodeInf->userID);
  gDoorLockUsersInformation[currentPosition].userStatus = pPinCodeInf->userStatus;
  gDoorLockUsersInformation[currentPosition].userType = pPinCodeInf->userType;
  if(isRFIDCode)
  {
 	gDoorLockUsersInformation[currentPosition].rfidInf.length = pPinCodeInf->pinInformation.length;
 	FLib_MemCpy(&gDoorLockUsersInformation[currentPosition].rfidInf.pinCode, &pPinCodeInf->pinInformation.pinCode, pPinCodeInf->pinInformation.length);
  }
  else
  {
 	gDoorLockUsersInformation[currentPosition].pinInf.length = pPinCodeInf->pinInformation.length;
 	FLib_MemCpy(&gDoorLockUsersInformation[currentPosition].pinInf.pinCode, &pPinCodeInf->pinInformation.pinCode, pPinCodeInf->pinInformation.length);
  }
  status = gDoorLockStatus_Succes_c;
  return status;
}

/*!
 * @fn 		static zbStatus_t DoorLockCluster_ProcessGetPinRFIDCode(uint16_t userID, afAddrInfo_t addrInfo, uint8_t transactionId, bool_t isRFIDCode)
 *
 * @brief	Process the GetPinCode/GetRfidCode command received from a DoorLock Client
 *
 */
static zbStatus_t DoorLockCluster_ProcessGetPinRFIDCode(uint16_t userID, afAddrInfo_t addrInfo, uint8_t transactionId, bool_t isRFIDCode)
{ 
  zbStatus_t status = gZclFailure_c;
  zclDoorLockGetPinRFIDCodeRsp_t *pCommandRsp;	
  uint8_t i =0;
  uint8_t length = 0x00;
  uint8_t commandID = 0x00;
  
  length = (isRFIDCode)?gDoorLockUsersInformation[i].rfidInf.length:gDoorLockUsersInformation[i].pinInf.length;
   
  pCommandRsp = AF_MsgAlloc(); 
    if(!pCommandRsp)
      return gZclNoMem_c;
    
  FLib_MemCpy(&pCommandRsp->addrInfo, &addrInfo, sizeof(addrInfo));
  pCommandRsp->zclTransactionId = transactionId;

  for(i=0; i< gDoorLock_CurrentNoOfUsers; i++)
  {
	  if(gDoorLockUsersInformation[i].userID == userID)
	  {
		  if(length == 0x00)
		  {
			  pCommandRsp->cmdFrame.userStatus = gDoorLockUserStatus_Available_c;
			  pCommandRsp->cmdFrame.userType = gDoorLockUserType_NotSupported_c;
			  pCommandRsp->cmdFrame.pinLength = 0x00;
		  }
		  else
		  {
			  pCommandRsp->cmdFrame.userStatus = gDoorLockUsersInformation[i].userStatus;
			  pCommandRsp->cmdFrame.userType = gDoorLockUsersInformation[i].userType;
			  pCommandRsp->cmdFrame.pinLength = length;
			  if(isRFIDCode)
			  {
				  commandID = gZclCmdDoorLock_GetRFIDCodeRsp_c;
				  FLib_MemCpy(&pCommandRsp->cmdFrame.pin, &gDoorLockUsersInformation[i].rfidInf.pinCode, length);
			  }
			  else
			  {
				  commandID = gZclCmdDoorLock_GetPinCodeRsp_c;
				  FLib_MemCpy(&pCommandRsp->cmdFrame.pin, &gDoorLockUsersInformation[i].rfidInf.pinCode, length);
			  }
		  }
		  pCommandRsp->cmdFrame.userID = Native2OTA16(userID);
		  /* respond back to caller  */	
		  status =  zclDoorLock_GetPinRFIDCodeRsp(pCommandRsp, commandID);
		  MSG_Free(pCommandRsp);
		  break;
	  }
  }
 
 return status;
}

/*!
 * @fn 		static zbStatus_t DoorLockCluster_ProcessClearPinRfidCode(uint16_t userID, bool_t isRFIDCode)
 *
 * @brief	Process the ClearPinCode/ClearRfidCode command received from a DoorLock Client
 *
 */
static zbStatus_t DoorLockCluster_ProcessClearPinRfidCode(uint16_t userID, bool_t isRFIDCode)
{
  uint8_t i = 0;
  zbStatus_t status = gDoorLockStatus_Failed_c;
  
  uint8_t length = (isRFIDCode)?gDoorLockUsersInformation[i].pinInf.length:gDoorLockUsersInformation[i].rfidInf.length;
 
  for(i=0; i< gDoorLock_CurrentNoOfUsers; i++)
  {
	  if(gDoorLockUsersInformation[i].userID == userID)	
	  {
		  if(isRFIDCode)
		  {
			  gDoorLockUsersInformation[i].rfidInf.length = 0x00;
		  }
		  else
		  {
			  gDoorLockUsersInformation[i].pinInf.length = 0x00;
		  }
		  if(length == 0x00)
		  {
		    gDoorLockUsersInformation[i].userStatus = gDoorLockUserStatus_Available_c;
		    gDoorLockUsersInformation[i].userType = gDoorLockUserType_NonAccessUser_c;
		    /* set schedules to default values*/
		    gDoorLockUsersInformation[i].indexInWeekDayScheduledTable = gZclDoorLock_InvalidIndex_c; 
		   	gDoorLockUsersInformation[i].indexInYearDayScheduledTable = gZclDoorLock_InvalidIndex_c;
		   	gDoorLock_WeekDayScheduledInf[i].slotStatus = FALSE;
		   	gDoorLock_YearDayScheduledInf[i].slotStatus = FALSE;
		   	FLib_MemSet(&gDoorLock_WeekDayScheduledInf[i].weekDayScheduledInf, 0x00, sizeof(zclDoorLock_WeekDayScheduledInf_t)*gZclDoorLock_NoOfWeekDayScheduledPerUser_c);
		   	FLib_MemSet(&gDoorLock_YearDayScheduledInf[i].yearDayScheduledInf, 0x00, sizeof(zclDoorLock_YearDayScheduledInf_t)*gZclDoorLock_NoOfYearDayScheduledPerUser_c);
		  }
		  status = gDoorLockStatus_Succes_c;
		  break;
	  }	
  }
  return status;
}

/*!
 * @fn 		static zbStatus_t DoorLockCluster_ProcessClearAllPinRfidCodes(bool_t isRFIDCode)
 *
 * @brief	Process the ClearAllPinCodes/ClearAllRfidCodes command received from a DoorLock Client
 *
 */
static zbStatus_t DoorLockCluster_ProcessClearAllPinRfidCodes(bool_t isRFIDCode)
{
  uint8_t i = 0;
  zbStatus_t status = gDoorLockStatus_Failed_c;
  uint8_t length = (isRFIDCode)?gDoorLockUsersInformation[i].pinInf.length:gDoorLockUsersInformation[i].rfidInf.length;
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	  if(isRFIDCode)
	  {
	    gDoorLockUsersInformation[i].rfidInf.length = 0x00;
	  }
	  else
	  {
	    gDoorLockUsersInformation[i].pinInf.length = 0x00;
	  }
	  if(length == 0x00)
	  {
	    gDoorLockUsersInformation[i].userStatus = gDoorLockUserStatus_Available_c;
	    gDoorLockUsersInformation[i].userType = gDoorLockUserType_NonAccessUser_c;
	    /* set schedules to default values */
	    gDoorLockUsersInformation[i].indexInWeekDayScheduledTable = gZclDoorLock_InvalidIndex_c; 
	    gDoorLockUsersInformation[i].indexInYearDayScheduledTable = gZclDoorLock_InvalidIndex_c;
	    gDoorLock_WeekDayScheduledInf[i].slotStatus = FALSE;
	    gDoorLock_YearDayScheduledInf[i].slotStatus = FALSE;
	    FLib_MemSet(&gDoorLock_WeekDayScheduledInf[i].weekDayScheduledInf, 0x00, sizeof(zclDoorLock_WeekDayScheduledInf_t)*gZclDoorLock_NoOfWeekDayScheduledPerUser_c);
	    FLib_MemSet(&gDoorLock_YearDayScheduledInf[i].yearDayScheduledInf, 0x00, sizeof(zclDoorLock_YearDayScheduledInf_t)*gZclDoorLock_NoOfYearDayScheduledPerUser_c);
	  }
	  status = gDoorLockStatus_Succes_c;
  }
  return status;
}

/*!
 * @fn 		static zbStatus_t  DoorLockCluster_ProcessSetUserType(zclCmdDoorLockSetUserType_t userData)
 *
 * @brief	Process the SetUserType command received from a DoorLock Client
 *
 */
static zbStatus_t  DoorLockCluster_ProcessSetUserType(zclCmdDoorLockSetUserType_t userData)
{
  zbStatus_t status = gDoorLockStatus_Failed_c;
  uint8_t i= 0;
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	 if(userData.userID == gDoorLockUsersInformation[i].userID) 
	 {
		 gDoorLockUsersInformation[i].userType = userData.userType;
		 status = gDoorLockStatus_Succes_c;
		 if(!(userData.userType == gDoorLockUserType_WeekDayScheduleUser_c || userData.userType == gDoorLockUserType_YearDayScheduleUser_c))
		 {		 
			/* remove the user's schedule */
			if(gDoorLockUsersInformation[i].indexInWeekDayScheduledTable != gZclDoorLock_InvalidIndex_c)
			{
				gDoorLock_WeekDayScheduledInf[gDoorLockUsersInformation[i].indexInWeekDayScheduledTable].slotStatus = FALSE;
				gDoorLockUsersInformation[i].indexInWeekDayScheduledTable = gZclDoorLock_InvalidIndex_c; 
				FLib_MemSet(&gDoorLock_WeekDayScheduledInf[i].weekDayScheduledInf, 0x00, sizeof(zclDoorLock_WeekDayScheduledInf_t)*gZclDoorLock_NoOfWeekDayScheduledPerUser_c);
			}
			if(gDoorLockUsersInformation[i].indexInYearDayScheduledTable != gZclDoorLock_InvalidIndex_c)
			{
				gDoorLock_YearDayScheduledInf[gDoorLockUsersInformation[i].indexInYearDayScheduledTable].slotStatus = FALSE;
				gDoorLockUsersInformation[i].indexInYearDayScheduledTable = gZclDoorLock_InvalidIndex_c; 
				FLib_MemSet(&gDoorLock_YearDayScheduledInf[i].yearDayScheduledInf, 0x00, sizeof(zclDoorLock_YearDayScheduledInf_t)*gZclDoorLock_NoOfYearDayScheduledPerUser_c);
			}
		 }
		 break;
	 }	
  }
  return status;
}

/*!
 * @fn 		static zbStatus_t  DoorLockCluster_ProcessGetUserType(uint16_t userID, afAddrInfo_t addrInfo, uint8_t transactionId)
 *
 * @brief	Process the GetUserType command received from a DoorLock Client
 *
 */
static zbStatus_t  DoorLockCluster_ProcessGetUserType(uint16_t userID, afAddrInfo_t addrInfo, uint8_t transactionId)
{
  zbStatus_t status = gZclInvalidField_c;
  zclDoorLockGetUserTypeRsp_t commandRsp;	
  uint8_t i=0;

  
  FLib_MemCpy(&commandRsp.addrInfo, &addrInfo, sizeof(addrInfo));
  commandRsp.zclTransactionId = transactionId; 
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	 if(userID == gDoorLockUsersInformation[i].userID) 
	 {
		 commandRsp.cmdFrame.userID = Native2OTA16(userID);
		 commandRsp.cmdFrame.userType = gDoorLockUsersInformation[i].userType;
		 
		 status = zclDoorLock_GetUserTypeRsp(&commandRsp);
		 break;
	 }	
  }
  return status;
}

/*!
 * @fn 		static zbStatus_t  DoorLockCluster_ProcessSetUserStatus(zclCmdDoorLockSetUserStatus_t userData)
 *
 * @brief	Process the SetUserStatus command received from a DoorLock Client
 *
 */
static zbStatus_t  DoorLockCluster_ProcessSetUserStatus(zclCmdDoorLockSetUserStatus_t userData)
{
  zbStatus_t status = gDoorLockStatus_Failed_c;
  uint8_t  i= 0;
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	 if(userData.userID == gDoorLockUsersInformation[i].userID) 
	 {
		 gDoorLockUsersInformation[i].userType = userData.userStatus;
		 status = gDoorLockStatus_Succes_c;
		 break;
	 }	
  }
  return status;
}

/*!
 * @fn 		static zbStatus_t  DoorLockCluster_ProcessGetUserStatus(uint16_t userID, afAddrInfo_t addrInfo, uint8_t transactionId)
 *
 * @brief	Process the GetUserStatus command received from a DoorLock Client
 *
 */
static zbStatus_t  DoorLockCluster_ProcessGetUserStatus(uint16_t userID, afAddrInfo_t addrInfo, uint8_t transactionId)
{
  zbStatus_t status = gZclInvalidField_c;
  zclDoorLockGetUserStatusRsp_t commandRsp;	
  uint8_t i= 0; 
  
  FLib_MemCpy(&commandRsp.addrInfo, &addrInfo, sizeof(addrInfo));
  commandRsp.zclTransactionId = transactionId; 
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	 if(userID == gDoorLockUsersInformation[i].userID) 
	 {
		 commandRsp.cmdFrame.userID = Native2OTA16(userID);
		 commandRsp.cmdFrame.userStatus = gDoorLockUsersInformation[i].userStatus;
		 
		 status = zclDoorLock_GetUserStatusRsp(&commandRsp);
		 break;
	 }	
  }
  return status;
}

/*!
 * @fn 		static zbStatus_t  DoorLockCluster_ProcessSetWeekDaySchedule(zclCmdDoorLockSetWeekDaySchedule_t scheduleInf)
 *
 * @brief	Process the SetWeekDaySchedule command received from a DoorLock Client
 *
 */
static zbStatus_t  DoorLockCluster_ProcessSetWeekDaySchedule(zclCmdDoorLockSetWeekDaySchedule_t scheduleInf)
{
  zbStatus_t status = gDoorLockStatus_Failed_c;
  uint8_t i= 0, j= 0;
  uint8_t weekDayIndex = 0;
  uint8_t indexScheduledId = gZclDoorLock_InvalidIndex_c;
  uint8_t indexDayMask = gZclDoorLock_InvalidIndex_c;
  uint8_t currentScheduledIndex = gZclDoorLock_InvalidIndex_c;
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	 if(scheduleInf.userID == gDoorLockUsersInformation[i].userID) 
	 {
		 weekDayIndex = gDoorLockUsersInformation[i].indexInWeekDayScheduledTable;
		 if(weekDayIndex == gZclDoorLock_InvalidIndex_c)
		 {
			 for(j=0; j<gDoorLock_CurrentNoOfUsers; j++)
			 {
				 if(gDoorLock_WeekDayScheduledInf[j].slotStatus == FALSE)
				 {
					 gDoorLock_WeekDayScheduledInf[j].slotStatus = TRUE;
					 gDoorLockUsersInformation[i].indexInWeekDayScheduledTable = j;
					 weekDayIndex = j;
					 break;
				 }
			 }
		 }
		 
		 if(weekDayIndex == gZclDoorLock_InvalidIndex_c)
			 return status;
		 
		 for(j=0; j<gZclDoorLock_NoOfWeekDayScheduledPerUser_c; j++)
		 {
			 
			 if(gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].scheduleId == scheduleInf.scheduleID)
			 {
				 /* to avoid multiple scheduled with the same Scheduled Id */
				 indexScheduledId = j;
				 break;
			 }
			 if(gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].dayMask == 0x00)
			 {
				 /* keep the first table index with no scheduled information */
				 if(indexDayMask == gZclDoorLock_InvalidIndex_c)
					 indexDayMask = j;
			 }
		 }
		 
		 currentScheduledIndex = (indexScheduledId != gZclDoorLock_InvalidIndex_c)?indexScheduledId:indexDayMask;
		 /* update scheduled information */
		 if(currentScheduledIndex!=gZclDoorLock_InvalidIndex_c)	 
		 {
			 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[currentScheduledIndex].scheduleId = scheduleInf.scheduleID;
			 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[currentScheduledIndex].dayMask = scheduleInf.daysMask;
			 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[currentScheduledIndex].endHour = scheduleInf.endHour;
			 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[currentScheduledIndex].endMinute = scheduleInf.endMinute;
			 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[currentScheduledIndex].startHour = scheduleInf.startHour;
			 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[currentScheduledIndex].startMinute = scheduleInf.startMinute;
		
			 status = gDoorLockStatus_Succes_c;
			 return status;
		 }
	 }	
  }
  return status;
}

/*!
 * @fn 		static zbStatus_t  DoorLockCluster_ProcessGetWeekDaySchedule(zclCmdDoorLockGetWeekDaySchedule_t scheduleInf, afAddrInfo_t addrInfo, uint8_t transactionId)
 *
 * @brief	Process the GetWeekDaySchedule command received from a DoorLock Client
 *
 */
static zbStatus_t  DoorLockCluster_ProcessGetWeekDaySchedule(zclCmdDoorLockGetWeekDaySchedule_t scheduleInf, afAddrInfo_t addrInfo, uint8_t transactionId)
{
  zclDoorLockGetWeekDayScheduleRsp_t commandRsp;
  uint8_t weekDayIndex = 0; 
  uint8_t i= 0, j= 0; 
  
  FLib_MemCpy(&commandRsp.addrInfo, &addrInfo, sizeof(addrInfo));
  commandRsp.zclTransactionId = transactionId; 
  commandRsp.cmdFrame.status = gZclInvalidField_c;
  commandRsp.cmdFrame.userID = Native2OTA16(scheduleInf.userID);
  commandRsp.cmdFrame.scheduleID = scheduleInf.scheduleID;
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	 if(scheduleInf.userID == gDoorLockUsersInformation[i].userID) 
	 {
		 weekDayIndex = gDoorLockUsersInformation[i].indexInWeekDayScheduledTable;
		 commandRsp.cmdFrame.status = gZclNotFound_c;
		 if((weekDayIndex != gZclDoorLock_InvalidIndex_c) && (gDoorLock_WeekDayScheduledInf[weekDayIndex].slotStatus == TRUE)) 
		 {
			 for(j=0; j<gZclDoorLock_NoOfWeekDayScheduledPerUser_c; j++)
			 {
				 if(gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].scheduleId == scheduleInf.scheduleID) 
				 {
					 commandRsp.cmdFrame.status = gZclSuccess_c;
					 commandRsp.cmdFrame.daysMask = gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].dayMask;
					 commandRsp.cmdFrame.endHour = gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].endHour;
					 commandRsp.cmdFrame.endMinute = gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].endMinute;
					 commandRsp.cmdFrame.startHour = gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].startHour;
					 commandRsp.cmdFrame.startMinute = gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].startMinute;
					 break;
				 }
			 }
		 }
		 break;
	 }	
  }

  return zclDoorLock_GetWeekDayScheduleRsp(&commandRsp);
}

/*!
 * @fn 		static zbStatus_t  DoorLockCluster_ProcessClearWeekDaySchedule(zclCmdDoorLockGetWeekDaySchedule_t scheduleInf)
 *
 * @brief	Process the ClearWeekDaySchedule command received from a DoorLock Client
 *
 */
static zbStatus_t  DoorLockCluster_ProcessClearWeekDaySchedule(zclCmdDoorLockGetWeekDaySchedule_t scheduleInf)
{
  zbStatus_t status = gDoorLockStatus_Failed_c;
  uint8_t i=0, j=0;
  uint8_t counter=0;
  uint8_t weekDayIndex =0;
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	 if(scheduleInf.userID == gDoorLockUsersInformation[i].userID) 
	 {
		 weekDayIndex = gDoorLockUsersInformation[i].indexInWeekDayScheduledTable;
		 if((weekDayIndex != gZclDoorLock_InvalidIndex_c) && (gDoorLock_WeekDayScheduledInf[weekDayIndex].slotStatus == TRUE)) 
		 {
			 for(j=0; j<gZclDoorLock_NoOfWeekDayScheduledPerUser_c; j++)
			 {
				 if(gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].scheduleId == scheduleInf.scheduleID) 
				 {
					 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].dayMask = 0x00;
					 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].endHour = 0x00;
					 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].endMinute = 0x00;
					 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].startHour = 0x00;
					 gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].startMinute = 0x00;
					 
					 status = gDoorLockStatus_Succes_c;
				 }
				 if(gDoorLock_WeekDayScheduledInf[weekDayIndex].weekDayScheduledInf[j].dayMask == 0x00)
				 {
					 counter++;
				 }
			 }  
			 if(counter == gZclDoorLock_NoOfWeekDayScheduledPerUser_c)
			 {
				 gDoorLock_WeekDayScheduledInf[weekDayIndex].slotStatus = FALSE;
				 gDoorLockUsersInformation[i].indexInWeekDayScheduledTable = gZclDoorLock_InvalidIndex_c;
			 }
				 
		 }
		 break;
	 }
  }
  return status; 
}

/*!
 * @fn 		static zbStatus_t  DoorLockCluster_ProcessSetYearDaySchedule(zclCmdDoorLockSetYearDaySchedule_t scheduleInf)
 *
 * @brief	Process the SetYearDaySchedule command received from a DoorLock Client
 *
 */
static zbStatus_t  DoorLockCluster_ProcessSetYearDaySchedule(zclCmdDoorLockSetYearDaySchedule_t scheduleInf)
{
  zbStatus_t status = gDoorLockStatus_Failed_c;
  uint8_t i= 0, j= 0;
  uint8_t yearDayIndex = 0;
  uint8_t indexScheduledId = gZclDoorLock_InvalidIndex_c;
  uint8_t indexYearTime = gZclDoorLock_InvalidIndex_c;
  uint8_t currentScheduledIndex = gZclDoorLock_InvalidIndex_c;
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	 if(scheduleInf.userID == gDoorLockUsersInformation[i].userID) 
	 {
		 yearDayIndex = gDoorLockUsersInformation[i].indexInYearDayScheduledTable;
		 if(yearDayIndex == gZclDoorLock_InvalidIndex_c)
		 {
			 for(j=0; j<gDoorLock_CurrentNoOfUsers; j++)
			 {
				 if(gDoorLock_YearDayScheduledInf[j].slotStatus == FALSE)
				 {
					 gDoorLock_YearDayScheduledInf[j].slotStatus = TRUE;
					 gDoorLockUsersInformation[i].indexInYearDayScheduledTable = j;
					 yearDayIndex = j;
					 break;
				 }
			 }
		 }
		 
		 if(yearDayIndex == gZclDoorLock_InvalidIndex_c)
			 return status;
		 
		 for(j=0; j<gZclDoorLock_NoOfYearDayScheduledPerUser_c; j++)
		 {
			 
			 if(gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].scheduleId == scheduleInf.scheduleID)
			 {
				 /* to avoid multiple scheduled with the same Scheduled Id */
				 indexScheduledId = j;
				 break;
			 }
			 if((gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].zigBeeLocalEndTime == 0x00) &&
			 					 (gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].zigBeeLocalStartTime == 0x00))
			 {
				 /* keep the first table index with no scheduled information */
				 if(indexYearTime == gZclDoorLock_InvalidIndex_c)
					 indexYearTime = j;
			 }
		 }
		 
		 currentScheduledIndex = (indexScheduledId != gZclDoorLock_InvalidIndex_c)?indexScheduledId:indexYearTime;
		 /* update scheduled information */
		 if(currentScheduledIndex!=gZclDoorLock_InvalidIndex_c)	 
		 {
			gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].zigBeeLocalEndTime = OTA2Native32(scheduleInf.zigBeeLocalEndTime);
			gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].zigBeeLocalStartTime = OTA2Native32(scheduleInf.zigBeeLocalStartTime);
			gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].scheduleId = scheduleInf.scheduleID;
			status = gDoorLockStatus_Succes_c;
			return status;
		 }
	 }	
  }
  return status;
}

/*!
 * @fn 		static zbStatus_t  DoorLockCluster_ProcessGetYearDaySchedule(zclCmdDoorLockGetYearDaySchedule_t scheduleInf, afAddrInfo_t addrInfo, uint8_t transactionId)
 *
 * @brief	Process the GetYearDaySchedule command received from a DoorLock Client
 *
 */
static zbStatus_t  DoorLockCluster_ProcessGetYearDaySchedule(zclCmdDoorLockGetYearDaySchedule_t scheduleInf, afAddrInfo_t addrInfo, uint8_t transactionId)
{
  zclDoorLockGetYearDayScheduleRsp_t commandRsp;
  uint8_t yearDayIndex = 0; 
  uint8_t i= 0, j= 0; 
  
  FLib_MemCpy(&commandRsp.addrInfo, &addrInfo, sizeof(addrInfo));
  commandRsp.zclTransactionId = transactionId; 
  commandRsp.cmdFrame.status = gZclInvalidField_c;
  commandRsp.cmdFrame.userID = Native2OTA16(scheduleInf.userID);
  commandRsp.cmdFrame.scheduleID = scheduleInf.scheduleID;
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	 if(scheduleInf.userID == gDoorLockUsersInformation[i].userID) 
	 {
		 yearDayIndex = gDoorLockUsersInformation[i].indexInYearDayScheduledTable;
		 commandRsp.cmdFrame.status = gZclNotFound_c;
		 if((yearDayIndex != gZclDoorLock_InvalidIndex_c) && (gDoorLock_YearDayScheduledInf[yearDayIndex].slotStatus == TRUE)) 
		 {
			 for(j=0; j<gZclDoorLock_NoOfYearDayScheduledPerUser_c; j++)
			 {
				 if(gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].scheduleId == scheduleInf.scheduleID) 
				 {
					 commandRsp.cmdFrame.status = gZclSuccess_c;
					 commandRsp.cmdFrame.zigBeeLocalEndTime = Native2OTA32(gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].zigBeeLocalEndTime);
					 commandRsp.cmdFrame.zigBeeLocalStartTime = Native2OTA32(gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].zigBeeLocalStartTime);
					 break;
				 }
			 }
		 }
		 break;
	 }	
  }

  return zclDoorLock_GetYearDayScheduleRsp(&commandRsp);
}

/*!
 * @fn 		static zbStatus_t  DoorLockCluster_ProcessClearYearDaySchedule(zclCmdDoorLockGetYearDaySchedule_t scheduleInf)
 *
 * @brief	Process the ClearYearDaySchedule command received from a DoorLock Client
 *
 */
static zbStatus_t  DoorLockCluster_ProcessClearYearDaySchedule(zclCmdDoorLockGetYearDaySchedule_t scheduleInf)
{
  zbStatus_t status = gDoorLockStatus_Failed_c;
  uint8_t i=0, j=0;
  uint8_t yearDayIndex = 0;
  uint8_t counter=0;
  
  for(i=0; i<gDoorLock_CurrentNoOfUsers; i++)
  {
	 if(scheduleInf.userID == gDoorLockUsersInformation[i].userID) 
	 {
		 yearDayIndex = gDoorLockUsersInformation[i].indexInYearDayScheduledTable;
		 if((yearDayIndex != gZclDoorLock_InvalidIndex_c) && (gDoorLock_YearDayScheduledInf[yearDayIndex].slotStatus == TRUE)) 
		 {
			 for(j=0; j<gZclDoorLock_NoOfYearDayScheduledPerUser_c; j++)
			 {
				 if(gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].scheduleId == scheduleInf.scheduleID) 
				 {
					 gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].scheduleId = 0x00;
					 gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].zigBeeLocalEndTime = 0x00;
					 gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].zigBeeLocalStartTime = 0x00;
					 status = gDoorLockStatus_Succes_c;
				 }
				 if((gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].zigBeeLocalEndTime == 0x00) &&
						 (gDoorLock_YearDayScheduledInf[yearDayIndex].yearDayScheduledInf[j].zigBeeLocalStartTime == 0x00))
				 {
					 counter++;
				 }
			 }  
			 if(counter == gZclDoorLock_NoOfYearDayScheduledPerUser_c)
			 {
				 gDoorLock_YearDayScheduledInf[yearDayIndex].slotStatus = FALSE;
				 gDoorLockUsersInformation[i].indexInYearDayScheduledTable = gZclDoorLock_InvalidIndex_c;
			 }
				 
		 }
		 break;
	 }
  }
  return status; 
}
#endif
/*!
 * @fn 		void DoorLockCluster_InitClusterServer(void)
 *
 * @brief	Init DoorLock Cluster Server functionality
 *
 */
void DoorLockCluster_InitClusterServer(void)
{
  uint8_t i = 0, j = 0;
  uint8_t defaultPinRFidCode[gZclDoorLock_DefaultMasterKeyLength_c] = {gZclDoorLock_DefaultMasterKey_c};
  
  for(i=0;i<gZclDoorLock_NoOfTotalUsersSupported_c; i++)
  {
	  /* init week day schedule table */
	  gDoorLock_WeekDayScheduledInf[i].slotStatus = FALSE;
	  FLib_MemSet(&gDoorLock_WeekDayScheduledInf[i].weekDayScheduledInf, 0x00, gZclDoorLock_NoOfWeekDayScheduledPerUser_c*sizeof(zclDoorLock_WeekDayScheduledInf_t));		  
	  for(j=0;j<gZclDoorLock_NoOfWeekDayScheduledPerUser_c; j++)
		  gDoorLock_WeekDayScheduledInf[i].weekDayScheduledInf[j].dayMask = 0x00;
	  /* init year day schedule table */
	  gDoorLock_YearDayScheduledInf[i].slotStatus = FALSE;
	  FLib_MemSet(&gDoorLock_YearDayScheduledInf[i].yearDayScheduledInf, 0x00, gZclDoorLock_NoOfYearDayScheduledPerUser_c*sizeof(zclDoorLock_YearDayScheduledInf_t));	  
	  /* init door lock users table*/
	  gDoorLockUsersInformation[i].userID = gZclDoorLock_InvalidUserId_c;
	  gDoorLockUsersInformation[i].userStatus = gDoorLockUserStatus_Disabled_c;
	  gDoorLockUsersInformation[i].userType = gDoorLockUserType_NonAccessUser_c;
	  FLib_MemSet(&gDoorLockUsersInformation[i].pinInf,  0x00, sizeof(zclDoorLock_PinCodeInf_t));
	  FLib_MemSet(&gDoorLockUsersInformation[i].rfidInf, 0x00, sizeof(zclDoorLock_PinCodeInf_t));
	  gDoorLockUsersInformation[i].indexInWeekDayScheduledTable = gZclDoorLock_InvalidIndex_c;
	  gDoorLockUsersInformation[i].indexInYearDayScheduledTable = gZclDoorLock_InvalidIndex_c;
  }
  
  /* init the master user */
  gDoorLockUsersInformation[0].userID = gZclDoorLock_DefaultUserId_c;
  gDoorLockUsersInformation[0].userStatus = gDoorLockUserStatus_Enabled_c;
  gDoorLockUsersInformation[0].userType = gDoorLockUserType_MasterUser_c;
  gDoorLockUsersInformation[0].pinInf.length =  gZclDoorLock_DefaultMasterKeyLength_c;
  FLib_MemCpy(&gDoorLockUsersInformation[0].pinInf.pinCode, &defaultPinRFidCode, gZclDoorLock_DefaultMasterKeyLength_c);
  gDoorLockUsersInformation[0].rfidInf.length =  gZclDoorLock_DefaultMasterKeyLength_c;
  FLib_MemCpy(&gDoorLockUsersInformation[0].rfidInf.pinCode, &defaultPinRFidCode, gZclDoorLock_DefaultMasterKeyLength_c);
  /*update current no of users */
  gDoorLock_CurrentNoOfUsers = 0x01;
}



/******************************
 Window Covering Cluster 
  See HA1.2 Specification Section 9.3
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_WindowCoveringClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the WindowCovering Cluster Server. 
 *
 */
zbStatus_t ZCL_WindowCoveringClusterServer
(
	zbApsdeDataIndication_t *pIndication,
	afDeviceDef_t *pDevice
)
{
	  zclFrame_t *pFrame;
	  zbStatus_t status = gZclUnsupportedClusterCommand_c;  
	 
	  /* prevent compiler warning */
	  (void)pDevice;
	  pFrame = (void *)pIndication->pAsdu;

	  
	  /* handle the command */
	  switch(pFrame->command)
	  {
	     case gZclCmdWindowCovering_UpOpen_c:
	     {
	    	 uint16_t installedOpenLimitLift, installedOpenLimitTilt;
	    	 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledOpenLimitLift_c ,   gZclServerAttr_c, &installedOpenLimitLift,NULL);	    	 
	    	 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledOpenLimitTilt_c ,   gZclServerAttr_c, &installedOpenLimitTilt,NULL);	    	  
        	 /****************************
        	 *    TO DO
        	 *****************************/   	    	 
	    	 status = gZclSuccess_c;
	    	 break;	 	    	 
	     }
	     case gZclCmdWindowCovering_DownClose_c:
	     {
	    	 uint16_t installedClosedLimitLift, installedClosedLimitTilt;
	    	 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledClosedLimitLift_c ,   gZclServerAttr_c, &installedClosedLimitLift,NULL);	    	 
	    	 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledClosedLimitTilt_c ,   gZclServerAttr_c, &installedClosedLimitTilt,NULL);	    	 
	    	 
        	 /****************************
        	 *    TO DO
        	 *****************************/   	    	 
	    	 status = gZclSuccess_c;
	    	 break;	 	    	 
	     }
	     case gZclCmdWindowCovering_Stop_c:
	     {
        	 /****************************
        	 *    TO DO
        	 *****************************/   	    	 
	    	 status = gZclSuccess_c;
	    	 break;	    
	     }
	     case gZclCmdWindowCovering_GoToLiftValue_c:
	     case gZclCmdWindowCovering_GoToTiltValue_c:	    	 
	     {
	    	 zclCmdWindowCovering_GoToValue_t *pCmdPayload;
	    	 uint16_t installedOpenLimit, installedClosedLimit;
	    	 pCmdPayload = (void *)(pFrame+1);
	    	 
	    	 if(pFrame->command == gZclCmdWindowCovering_GoToLiftValue_c)
	    	 {
	    		 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledOpenLimitLift_c ,   gZclServerAttr_c, &installedOpenLimit,NULL);	    	 
	    	 	 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledClosedLimitLift_c , gZclServerAttr_c, &installedClosedLimit,NULL);	
	    	 }
	    	 else
	    	 {
	    		 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledOpenLimitTilt_c ,   gZclServerAttr_c, &installedOpenLimit,NULL);	    	 
	    	 	 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledClosedLimitTilt_c , gZclServerAttr_c, &installedClosedLimit,NULL);	 
	    	 }
             pCmdPayload->value = OTA2Native16(pCmdPayload->value);
             if((pCmdPayload->value > installedOpenLimit)||(pCmdPayload->value <installedClosedLimit))
              	 status = gZclInvalidValue_c;
             else
             {
            	 
#if gZclClusterOptionals_d     
            	 if(pFrame->command == gZclCmdWindowCovering_GoToLiftValue_c)
            		 (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringCurrentPositionLift_c, gZclServerAttr_c, &pCmdPayload->value); 
            	 else
            		 (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringCurrentPositionTilt_c, gZclServerAttr_c, &pCmdPayload->value); 
#endif                       
                 
            	 /****************************
            	 *    TO DO
            	 *****************************/    	 
            	 status = gZclSuccess_c;
             }
	    	 break;
	     }
	     case gZclCmdWindowCovering_GoToLiftPercentage_c:
	     case gZclCmdWindowCovering_GoToTiltPercentage_c:	
	     {
	    	 zclCmdWindowCovering_GoToPercentage_t *pCmdPayload;
	    	 uint8_t installedOpenLimit, installedClosedLimit;
	    	 pCmdPayload = (void *)(pFrame+1);
	    	 if(pFrame->command == gZclCmdWindowCovering_GoToLiftPercentage_c)
	    	 {
	    		 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledOpenLimitLift_c ,   gZclServerAttr_c, &installedOpenLimit,NULL);	    	 
	    	 	 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledClosedLimitLift_c , gZclServerAttr_c, &installedClosedLimit,NULL);	
	    	 }
	    	 else
	    	 {
	    		 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledOpenLimitTilt_c ,   gZclServerAttr_c, &installedOpenLimit,NULL);	    	 
	    	 	 (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringInstalledClosedLimitTilt_c , gZclServerAttr_c, &installedClosedLimit,NULL);	 
	    	 }
	    	 if((pCmdPayload->PercentageValue > installedOpenLimit)||(pCmdPayload->PercentageValue < installedClosedLimit)||(pCmdPayload->PercentageValue > gZclWindowCovering_MaxPercentageValue_c))
              	 status = gZclInvalidValue_c;
             else
             {
#if gZclClusterOptionals_d            	 
            	 if(pFrame->command == gZclCmdWindowCovering_GoToLiftPercentage_c)
            		 (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringCurrentPositionLiftPercentage_c, gZclServerAttr_c, &pCmdPayload->PercentageValue); 
            	 else
            		 (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrIdWindowCoveringCurrentPositionTiltPercentage_c, gZclServerAttr_c, &pCmdPayload->PercentageValue); 
#endif             	 
                 
            	 /****************************
            	 *    TO DO
            	 *****************************/ 
            	 status = gZclSuccess_c;
             }
	    	 break;
	     }	    	 
	     default:
	         return  gZclUnsupportedClusterCommand_c;    
	  }
	  if (status == gZbSuccess_c)
	     status = (pFrame->frameControl&gZclFrameControl_DisableDefaultRsp)?gZbSuccess_c:gZclSuccessDefaultRsp_c;
	   return status;
}

/*!
 * @fn 		zbStatus_t ZCL_WindowCoveringClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Window Covering Cluster Client. 
 *
 */
zbStatus_t ZCL_WindowCoveringClusterClient
(
        zbApsdeDataIndication_t *pIndication, 
        afDeviceDef_t *pDevice
)
{
    /* avoid compiler warnings */
	(void)pIndication;
	(void)pDevice;

	return gZclUnsupportedClusterCommand_c;  
}

/*!
 * @fn 		zbStatus_t zclWindowCovering_UpDownStopReq(zclWindowCovering_NoPayload_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a UpOpen/DownClose/Stop request from the WindowCovering Cluster Client. 
 *
 */
zbStatus_t zclWindowCovering_UpDownStopReq
(
  zclWindowCovering_NoPayload_t *pReq,
  uint8_t commandId
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterWindowCovering_c);	
    return ZCL_SendClientReqSeqPassed(commandId, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclWindowCovering_GoToValueReq(zclWindowCovering_GoToValue_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a GoToLiftValue/GoToTiltValue request from the WindowCovering Cluster Client. 
 *
 */
zbStatus_t zclWindowCovering_GoToValueReq
(
  zclWindowCovering_GoToValue_t *pReq,
  uint8_t commandId
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterWindowCovering_c);	
    return ZCL_SendClientReqSeqPassed(commandId, sizeof(zclCmdWindowCovering_GoToValue_t), (zclGenericReq_t *)pReq);
}


/*!
 * @fn 		zbStatus_t zclWindowCovering_GoToPercentageReq(zclWindowCovering_GoToPercentage_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a GoToLiftPercentage/GoToTiltPercentage request from the WindowCovering Cluster Client. 
 *
 */
zbStatus_t zclWindowCovering_GoToPercentageReq
(
  zclWindowCovering_GoToPercentage_t *pReq,
  uint8_t commandId
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterWindowCovering_c);	
    return ZCL_SendClientReqSeqPassed(commandId, sizeof(zclCmdWindowCovering_GoToPercentage_t), (zclGenericReq_t *)pReq);
}

