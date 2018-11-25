/*! @file 	ZclClosures.h
 *
 * @brief	Types, definitions and prototypes for the ZCL Closures Domain implementition.
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
#ifndef _ZCL_CLOSURES_H
#define _ZCL_CLOSURES_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"



/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************
	Shade Configuration Cluster
*******************************************/

/* Shade Configuration Configuration Attributes Sets */
#if ( TRUE == gBigEndian_c )
#define gZclAttrShadeCfgShadeInformationSet_c       0x0000    /* Shade Information Set */
#define gZclAttrShadeCfgShadeSettingsSet_c          0x0100    /* Shade Settings Set */
                                                              /*0x002-0xFFF - reserved*/
#else
#define gZclAttrShadeCfgShadeInformationSet_c       0x0000    /* Shade Information Set */
#define gZclAttrShadeCfgShadeSettingsSet_c          0x0001    /* Shade Settings Set */
                                                              /*0x002-0xFFF - reserved*/
#endif /* #if ( TRUE == gBigEndian_c ) */

#if ( TRUE == gBigEndian_c )
/* 7.2.2.2.1 Shade information attributes */
#define gZclAttrShadeCfgInfPhysicalClosedLimit_c    0x0000    /* O - PhysicalClosedLimit */
#define gZclAttrShadeCfgInfMotorStepSize_c          0x0100    /* O - MotorStepSize*/
#define gZclAttrShadeCfgInfStatus_c                 0x0200    /* M - Status*/
/* 7.2.2.2.2 Shade settings attributes */
#define gZclAttrShadeCfgStgClosedLimit_c            0x1000    /* M - ClosedLimit */
#define gZclAttrShadeCfgStgMode_c                   0x1100    /* M - Mode */
#else
/* 7.2.2.2.1 Shade information attributes */
#define gZclAttrShadeCfgInfPhysicalClosedLimit_c    0x0000    /* O - PhysicalClosedLimit */
#define gZclAttrShadeCfgInfMotorStepSize_c          0x0001    /* O - MotorStepSize*/
#define gZclAttrShadeCfgInfStatus_c                 0x0002    /* M - Status*/
/* 7.2.2.2.2 Shade settings attributes */
#define gZclAttrShadeCfgStgClosedLimit_c            0x0010    /* M - ClosedLimit */
#define  gZclAttrShadeCfgStgMode_c                  0x0011    /* M - Mode */
#endif 

#define gZclAttrShadeCfgIdSet_c						  0x00
#define gZclAttrShadeCfgIdInfPhysicalClosedLimit_c    0x00    /* O - PhysicalClosedLimit */
#define gZclAttrShadeCfgIdInfMotorStepSize_c          0x01    /* O - MotorStepSize*/
#define gZclAttrShadeCfgIdInfStatus_c                 0x02    /* M - Status*/
/* 7.2.2.2.2 Shade settings attributes */
#define gZclAttrShadeCfgIdStgClosedLimit_c            0x10    /* M - ClosedLimit */
#define  gZclAttrShadeCfgIdStgMode_c                  0x11    /* M - Mode */

/* Shade Configuration Status */
typedef struct gZclShadeStatus_tag
{
    unsigned ShadeOperational   :1;    /* Shade Operational (0- No, 1-Yes) - Read only */                             
    unsigned ShadeAdjusting     :1;    /* Shade Adjusting (0- No, 1-Yes) - Read only */    
    unsigned ShadeDirection     :1;    /* Shade Direction (0- closing, 1-opening) - Read only */ 
    unsigned ShadeForwardDir    :1;    /* Shade Forward Direction of motor (0- closing, 1-opening) - Read/Write */ 
    unsigned Reserved           :4;    /* Reserved */                                    
}gZclShadeStatus_t; 

/* Shade configuration Mode */
enum
{
   gModeNormal = 0x00,       /* Shade - Normal Mode */
   gModeConfigure            /* Shade - Configure Mode */  
                             /* 0x02 - 0xfe - reserved*/ 
};

/* The comand use for this cluster are the read/write attributes */

typedef PACKED_STRUCT zclShadeCfgAttrsRAM_tag
{
#if gZclClusterOptionals_d  
  uint16_t    physicalClosedLimit; 
  uint8_t     motorStepSize;
#endif
  uint8_t     status;
  uint16_t    closedLimit;
  uint8_t     mode;
} zclShadeCfgAttrsRAM_t;


/******************************************
	Door Lock Cluster
*******************************************/
/* Door Lock Attributes Sets */

#define gZclAttrSetDoorLock_c                   					0x00
/* 10.1.2.10 Basic Information Attribute Set */
#define gZclAttrIdDoorLockLockState_c           					0x00    /* M - LockState */
#define gZclAttrIdDoorLockLockType_c            					0x01    /* M - LockType */
#define gZclAttrIdDoorLockActuatorEnable_c      					0x02    /* M - ActuatorEnable */
#define gZclAttrIdDoorLockDoorState_c           					0x03    /* O - DoorState */
#define gZclAttrIdDoorLockDoorOpenEvents_c      					0x04    /* O - DoorOpenEvents */
#define gZclAttrIdDoorLockDoorClosedEvents_c    					0x05    /* O - DoorClosedEvents */
#define gZclAttrIdDoorLockDoorOpenPeriod_c      					0x06    /* O - OpenPeriod */

/* 10.1.2.11 User, PIN, Schedule, Log Information Attribute Set */
#define gZclAttrIdDoorLockNoOfLogReqSupported_c     				0x10    /* O - Number of log request supported */
#define gZclAttrIdDoorLockNoOfTotalUsersSupported_c 				0x11    /* O - Number of total users supported */
#define gZclAttrIdDoorLockNoOfPinUsersSupported_c					0x12    /* O - Number of pin users supported */
#define gZclAttrIdDoorLockNoOfRFIDUsersSupported_c					0x13    /* O - Number of RFID users supported */
#define gZclAttrIdDoorLockNoOfWeekDaySchedulesSupportedPerUser_c	0x14    /* O - Number of week day schedules supported per user */
#define gZclAttrIdDoorLockNoOfYearDaySchedulesSupportedPerUser_c	0x15    /* O - Number of year day schedules supported per user */
#define gZclAttrIdDoorLockNoOfHolidaySchedulesSupported_c			0x16    /* O - Number of holiday schedules supported  */
#define gZclAttrIdDoorLockMaxPinCodeLength_c						0x17    /* O - max pin code length  */
#define gZclAttrIdDoorLockMinPinCodeLength_c						0x18    /* O - min pin code length  */
#define gZclAttrIdDoorLockMaxRfidCodeLength_c						0x19    /* O - max RFID code length  */
#define gZclAttrIdDoorLockMinRfidCodeLength_c						0x1A    /* O - min RFID code length  */

/* 10.1.2.12 Operational Settings Attribute Set */
#define gZclAttrIdDoorLockEnableLogging_c     				0x20    /* O -enable logging */
#define gZclAttrIdDoorLockLanguage_c     					0x21    /* O -language */
#define gZclAttrIdDoorLockLedSettings_c     				0x22    /* O -led settings */
#define gZclAttrIdDoorLockAutoRelockTime_c     				0x23    /* O -autoRelockTime */
#define gZclAttrIdDoorLockSoundVolume_c     				0x24    /* O -sound volume */
#define gZclAttrIdDoorLockOperatingMode_c     				0x25    /* O -operatingMode */
#define gZclAttrIdDoorLockSupportedOperatingModes_c     	0x26    /* O -supported operating Modes */
#define gZclAttrIdDoorLockDefaultConfigRegister_c     		0x27    /* O -default configuration register */
#define gZclAttrIdDoorLockEnableLocalProgramming_c     		0x28    /* O -enable local programming */
#define gZclAttrIdDoorLockEnableOneTouchLocking_c     		0x29    /* O -enable one touch locking */
#define gZclAttrIdDoorLockEnableInsideStatusLED_c     		0x2A    /* O -enable one touch locking */
#define gZclAttrIdDoorLockEnablePrivacyModeButton_c     	0x2B    /* O -enable privacy mode button */

/* 10.1.2.13 Security Settings Attribute Set */
#define gZclAttrIdDoorLockWrongCodeEntryLimit_c     		0x30    /* O -wrong Code entry limit */
#define gZclAttrIdDoorLockUserCodeTemporaryDisableTime_c   	0x31    /* O -user code temporary disable time */
#define gZclAttrIdDoorLockSendPinOverTheAir_c   			0x32    /* O -send pin over the air */
#define gZclAttrIdDoorLockRequirePinForRFOperation_c   		0x33    /* O -requirePin for RF operation */
#define gZclAttrIdDoorLockZigbeeSecurityLevel_c   			0x34    /* O -ZigBee security level */

/* 10.1.2.14 Alarm and Event Masks Attribute Set */
#define gZclAttrIdDoorLockAlarmMask_c     					0x40    /* O -alarmMask */
#define gZclAttrIdDoorLockKeypadOperationEventMask_c   		0x41    /* O -keypad operation event mask */
#define gZclAttrIdDoorLockRFOperationEventMask_c   			0x42    /* O -RF operation event mask */
#define gZclAttrIdDoorLockManualOperationEventMask_c   		0x43    /* O -manual operation event mask */
#define gZclAttrIdDoorLockRFIDOperationEventMask_c   		0x44    /* O -RFID operation event mask */
#define gZclAttrIdDoorLockKeypadProgrammingEventMask_c   	0x45    /* O -keypad programming event mask */
#define gZclAttrIdDoorLockRFProgrammingEventMask_c   		0x46    /* O -RF programming event mask */
#define gZclAttrIdDoorLockRFIDProgrammingEventMask_c   		0x47    /* O -RFID programming event mask */

#if ( TRUE == gBigEndian_c )
/* 10.1.2.10 Basic Information Attribute Set */
#define gZclAttrDoorLockLockState_c           0x0000    /* M - LockState */
#define gZclAttrDoorLockLockType_c            0x0100    /* M - LockType */
#define gZclAttrDoorLockActuatorEnable_c      0x0200    /* M - ActuatorEnable */
#define gZclAttrDoorLockDoorState_c           0x0300    /* O - DoorState */
#define gZclAttrDoorLockDoorOpenEvents_c      0x0400    /* O - DoorOpenEvents */
#define gZclAttrDoorLockDoorClosedEvents_c    0x0500    /* O - DoorClosedEvents */
#define gZclAttrDoorLockDoorOpenPeriod_c      0x0600    /* O - OpenPeriod */

/* 10.1.2.11 User, PIN, Schedule, Log Information Attribute Set */
#define gZclAttrDoorLockNoOfLogReqSupported_c     				0x1000    /* O - Number of log request supported */
#define gZclAttrDoorLockNoOfTotalUsersSupported_c 				0x1100    /* O - Number of total users supported */
#define gZclAttrDoorLockNoOfPinUsersSupported_c					0x1200    /* O - Number of pin users supported */
#define gZclAttrDoorLockNoOfRFIDUsersSupported_c				0x1300    /* O - Number of RFID users supported */
#define gZclAttrDoorLockNoOfWeekDaySchedulesSupportedPerUser_c	0x1400    /* O - Number of week day schedules supported per user */
#define gZclAttrDoorLockNoOfYearDaySchedulesSupportedPerUser_c	0x1500    /* O - Number of year day schedules supported per user */
#define gZclAttrDoorLockNoOfHolidaySchedulesSupported_c			0x1600    /* O - Number of holiday schedules supported  */
#define gZclAttrDoorLockMaxPinCodeLength_c						0x1700    /* O - max pin code length  */
#define gZclAttrDoorLockMinPinCodeLength_c						0x1800    /* O - min pin code length  */
#define gZclAttrDoorLockMaxRfidCodeLength_c						0x1900    /* O - max RFID code length  */
#define gZclAttrDoorLockMinRfidCodeLength_c						0x1A00    /* O - min RFID code length  */

/* 10.1.2.12 Operational Settings Attribute Set */
#define gZclAttrDoorLockEnableLogging_c     			0x2000    /* O -enable logging */
#define gZclAttrDoorLockLanguage_c     					0x2100    /* O -language */
#define gZclAttrDoorLockLedSettings_c     				0x2200    /* O -led settings */
#define gZclAttrDoorLockAutoRelockTime_c     			0x2300    /* O -autoRelockTime */
#define gZclAttrDoorLockSoundVolume_c     				0x2400    /* O -sound volume */
#define gZclAttrDoorLockOperatingMode_c     			0x2500    /* O -operatingMode */
#define gZclAttrDoorLockSupportedOperatingModes_c     	0x2600    /* O -supported operating Modes */
#define gZclAttrDoorLockDefaultConfigRegister_c     	0x2700    /* O -default configuration register */
#define gZclAttrDoorLockEnableLocalProgramming_c     	0x2800    /* O -enable local programming */
#define gZclAttrDoorLockEnableOneTouchLocking_c     	0x2900    /* O -enable one touch locking */
#define gZclAttrDoorLockEnableInsideStatusLED_c     	0x2A00    /* O -enable one touch locking */
#define gZclAttrDoorLockEnablePrivacyModeButton_c     	0x2B00    /* O -enable privacy mode button */

/* 10.1.2.13 Security Settings Attribute Set */
#define gZclAttrDoorLockWrongCodeEntryLimit_c     		0x3000    /* O -wrong Code entry limit */
#define gZclAttrDoorLockUserCodeTemporaryDisableTime_c  0x3100    /* O -user code temporary disable time */
#define gZclAttrDoorLockSendPinOverTheAir_c   			0x3200    /* O -send pin over the air */
#define gZclAttrDoorLockRequirePinForRFOperation_c   	0x3300    /* O -requirePin for RF operation */
#define gZclAttrDoorLockZigbeeSecurityLevel_c   		0x3400    /* O -ZigBee security level */

/* 10.1.2.14 Alarm and Event Masks Attribute Set */
#define gZclAttrDoorLockAlarmMask_c     				0x4000    /* O -alarmMask */
#define gZclAttrDoorLockKeypadOperationEventMask_c   	0x4100    /* O -keypad operation event mask */
#define gZclAttrDoorLockRFOperationEventMask_c   		0x4200    /* O -RF operation event mask */
#define gZclAttrDoorLockManualOperationEventMask_c   	0x4300    /* O -manual operation event mask */
#define gZclAttrDoorLockRFIDOperationEventMask_c   		0x4400    /* O -RFID operation event mask */
#define gZclAttrDoorLockKeypadProgrammingEventMask_c   	0x4500    /* O -keypad programming event mask */
#define gZclAttrDoorLockRFProgrammingEventMask_c   		0x4600    /* O -RF programming event mask */
#define gZclAttrDoorLockRFIDProgrammingEventMask_c   	0x4700    /* O -RFID programming event mask */
#else
/* 10.1.2.10 Basic Information Attribute Set */
#define gZclAttrDoorLockLockState_c           0x0000    /* M - LockState */
#define gZclAttrDoorLockLockType_c            0x0001    /* M - LockType */
#define gZclAttrDoorLockActuatorEnable_c      0x0002    /* M - ActuatorEnable */
#define gZclAttrDoorLockDoorState_c           0x0003    /* O - DoorState */
#define gZclAttrDoorLockDoorOpenEvents_c      0x0004    /* O - DoorOpenEvents */
#define gZclAttrDoorLockDoorClosedEvents_c    0x0005    /* O - DoorClosedEvents */
#define gZclAttrDoorLockDoorOpenPeriod_c      0x0006    /* O - OpenPeriod */

/* 10.1.2.11 User, PIN, Schedule, Log Information Attribute Set */
#define gZclAttrDoorLockNoOfLogReqSupported_c     				0x0010    /* O - Number of log request supported */
#define gZclAttrDoorLockNoOfTotalUsersSupported_c 				0x0011    /* O - Number of total users supported */
#define gZclAttrDoorLockNoOfPinUsersSupported_c					0x0012    /* O - Number of pin users supported */
#define gZclAttrDoorLockNoOfRFIDUsersSupported_c				0x0013    /* O - Number of RFID users supported */
#define gZclAttrDoorLockNoOfWeekDaySchedulesSupportedPerUser_c	0x0014    /* O - Number of week day schedules supported per user */
#define gZclAttrDoorLockNoOfYearDaySchedulesSupportedPerUser_c	0x0015    /* O - Number of year day schedules supported per user */
#define gZclAttrDoorLockNoOfHolidaySchedulesSupported_c			0x0016    /* O - Number of holiday schedules supported  */
#define gZclAttrDoorLockMaxPinCodeLength_c						0x0017    /* O - max pin code length  */
#define gZclAttrDoorLockMinPinCodeLength_c						0x0018    /* O - min pin code length  */
#define gZclAttrDoorLockMaxRfidCodeLength_c						0x0019    /* O - max RFID code length  */
#define gZclAttrDoorLockMinRfidCodeLength_c						0x001A    /* O - min RFID code length  */

/* 10.1.2.12 Operational Settings Attribute Set */
#define gZclAttrDoorLockEnableLogging_c     			0x0020    /* O -enable logging */
#define gZclAttrDoorLockLanguage_c     					0x0021    /* O -language */
#define gZclAttrDoorLockLedSettings_c     				0x0022    /* O -led settings */
#define gZclAttrDoorLockAutoRelockTime_c     			0x0023    /* O -autoRelockTime */
#define gZclAttrDoorLockSoundVolume_c     				0x0024    /* O -sound volume */
#define gZclAttrDoorLockOperatingMode_c     			0x0025    /* O -operatingMode */
#define gZclAttrDoorLockSupportedOperatingModes_c     	0x0026    /* O -supported operating Modes */
#define gZclAttrDoorLockDefaultConfigRegister_c     	0x0027    /* O -default configuration register */
#define gZclAttrDoorLockEnableLocalProgramming_c     	0x0028    /* O -enable local programming */
#define gZclAttrDoorLockEnableOneTouchLocking_c     	0x0029    /* O -enable one touch locking */
#define gZclAttrDoorLockEnableInsideStatusLED_c     	0x002A    /* O -enable one touch locking */
#define gZclAttrDoorLockEnablePrivacyModeButton_c     	0x002B    /* O -enable privacy mode button */

/* 10.1.2.13 Security Settings Attribute Set */
#define gZclAttrDoorLockWrongCodeEntryLimit_c     		0x0030    /* O -wrong Code entry limit */
#define gZclAttrDoorLockUserCodeTemporaryDisableTime_c  0x0031    /* O -user code temporary disable time */
#define gZclAttrDoorLockSendPinOverTheAir_c   			0x0032    /* O -send pin over the air */
#define gZclAttrDoorLockRequirePinForRFOperation_c   	0x0033    /* O -requirePin for RF operation */
#define gZclAttrDoorLockZigbeeSecurityLevel_c   		0x0034    /* O -ZigBee security level */

/* 10.1.2.14 Alarm and Event Masks Attribute Set */
#define gZclAttrDoorLockAlarmMask_c     				0x0040    /* O -alarmMask */
#define gZclAttrDoorLockKeypadOperationEventMask_c   	0x0041    /* O -keypad operation event mask */
#define gZclAttrDoorLockRFOperationEventMask_c   		0x0042    /* O -RF operation event mask */
#define gZclAttrDoorLockManualOperationEventMask_c   	0x0043    /* O -manual operation event mask */
#define gZclAttrDoorLockRFIDOperationEventMask_c   		0x0044    /* O -RFID operation event mask */
#define gZclAttrDoorLockKeypadProgrammingEventMask_c   	0x0045    /* O -keypad programming event mask */
#define gZclAttrDoorLockRFProgrammingEventMask_c   		0x0046    /* O -RF programming event mask */
#define gZclAttrDoorLockRFIDProgrammingEventMask_c   	0x0047    /* O -RFID programming event mask */
#endif 

/* Door LockState */
enum
{
   gDoorLockStateNotFullyLocked = 0x00,    /* LockState - Not Fully Locked */
   gDoorLockStateLocked,                   /* LockState - Locked */  
   gDoorLockStateUnlocked,                 /* LockState - Unlocked */ 
                                           /* 0x03 - 0xfe - reserved, 0xff - not defined */ 
};

/* Door LockType */
enum
{
   gDoorLockTypeDeadbolt = 0x00,          /* LockType - Dead bolt */
   gDoorLockTypeMagnetic,                 /* LockType - Magnetic */  
   gDoorLockTypeOther,                    /* LockType - Other */ 
   gDoorLockTypeMortise,                  /* LockType - Mortise */ 
   gDoorLockTypeRIM,                 	  /* LockType - RIM */ 
   gDoorLockTypeLatchBold,                /* LockType - LatchBold */   
   gDoorLockTypeCylindricalLock,          /* LockType - Cylindrical Lock */ 
   gDoorLockTypeTubularLock,         	  /* LockType - Tubular Lock */ 
   gDoorLockTypeInterconnectedLock,       /* LockType - Interconnected Lock */ 
   gDoorLockTypeDeadLatch,         		  /* LockType - Dead latch */   
   gDoorLockTypeDoorFurniture             /* LockType - Door Furniture */  
                                          /* 0x0B - 0xfF - reserved */ 
};

/* Door  State */
enum
{
   gDoorStateOpen = 0x00,                 /* DoorState - Open */
   gDoorStateClosed,                      /* DoorState - Closed */
   gDoorStateErrorJammed,                 /* DoorState - Error : jammed */
   gDoorStateErrorForcedOpen,             /* DoorState - Error : forced open */
   gDoorStateErrorUnspecified,            /* DoorState - Error : unspecified */
                                          /* 0x05 - 0xfe - reserved, 0xff - not defined */
};

/* Operating mode */
enum
{
   gDoorOperatingMode_Normal = 0x00,                /* Normal */
   gDoorOperatingMode_Vacation,                     /* Vacation */
   gDoorOperatingMode_Privacy,                 		/* Privacy */
   gDoorOperatingMode_NoRFLockUnlock,             	/* No RF lock/Unlock */
   gDoorOperatingMode_Passage           			/* Passage */
                                          /* 0x05 - 0xfe - reserved, 0xff - not defined */
};

#define gDoorSupportedOperatingMode_Normal_c 			(0x01<<0)
#define gDoorSupportedOperatingMode_Vacation_c 			(0x01<<1)
#define gDoorSupportedOperatingMode_Privacy_c 			(0x01<<2)
#define gDoorSupportedOperatingMode_NoRFLockUnlock_c	(0x01<<3)
#define gDoorSupportedOperatingMode_Passage_c			(0x01<<4)

/* LED settings */
enum
{
	gDoorLockLEDSettings_NeverUseLed = 0x00,                /* Never use led for signalization */
	gDoorLockLEDSettings_UseLedExceptAccesAllowedEvents,    /* use led for signalization except for access allowed events */
	gDoorLockLEDSettings_UseLedForALLEvents,    			/* use led for signalization for all  events */
};


/* sound Volume */
enum
{
	gDoorLockSoundVolume_SilentMode = 0x00,               	 /* silentMode */
	gDoorLockSoundVolume_LowVolume,   						 /* low volume */
	gDoorLockSoundVolume_HighVolume,    					 /* high volume */
};



typedef PACKED_STRUCT zclDoorLockAttrsRAM_tag
{
  uint8_t    lockState[zclReportableCopies_c]; 
  uint8_t    lockType;
  uint8_t    actuatorEnabled;
#if gZclClusterOptionals_d  
  uint8_t    doorState[zclReportableCopies_c];
  uint32_t   doorOpenEvents;
  uint32_t   doorClosedEvents;
  uint32_t   doorOpenPeriod;  
  /* 10.1.2.11 User, PIN, Schedule, Log Information Attribute Set */
  uint16_t	noOfLogRecordSupported;
  uint16_t	noOfTotalUsersSupported;
  uint16_t	noOfPinUsersSupported;
  uint16_t	noOfRFIDUsersSupported;
  uint8_t	noOfWeekDayScheduledPerUser;
  uint8_t	noOfYearDayScheduledPerUser;
  uint8_t	noOfHolidaySchedulesSupported;
  uint8_t	maxPinCodeLength;
  uint8_t	minPinCodeLength;
  uint8_t	maxRFIDCodeLength;
  uint8_t 	minRFIDCodeLength;
  /* 10.1.2.12 Operational Settings Attribute Set */
  bool_t	   enableLogging;
  zclStr2Oct_t language;
  uint8_t	   ledSettings;
  uint32_t	   autoRelockTime;
  uint8_t	   soundVolume;
  uint8_t	   operatingMode;
  uint16_t	   supportedOperatingMode;
  uint16_t	   defaultConfigRegister;
  bool_t	   enableLocalProgramming;
  bool_t	   enableOneTouchLocking;
  bool_t	   enableInsideStatusLed;
  bool_t	   enablePrivacyModeButton;
  /* 10.1.2.13 Security Settings Attribute Set */
  uint8_t	wrongCodeEntryLimit;
  uint8_t   userCodeTemporaryDisableTime;
  bool_t	sendPinOta;
  bool_t	requirePinRFOperation;
  uint8_t	zigBeeSecurityLevel;
  /* 10.1.2.14 Alarm and Event Masks Attribute Set */
  uint16_t	alarmMask;
  uint16_t	keypadOperationEventMask;
  uint16_t	rfOperationEventMask;
  uint16_t	manualOperationEventMask;
  uint16_t	rfidOperarionEventMask;
  uint16_t	keypadProgrammingEventMask;
  uint16_t	rfProgrammingEventMask;
  uint16_t	rfidProgrammingEventMask;
#endif  
} zclDoorLockAttrsRAM_t;

/* Door Lock cluster commands */
typedef PACKED_STRUCT zclCmdDoorLockSetState_tag
{
  zclCmd_t command;                /* door lock commands */
} zclCmdDoorLockSetState_t;

/* 10.1.2.15 Commands Received  */
#define gZclCmdDoorLock_Lock_c         				0x00    /* M - lock door */
#define gZclCmdDoorLock_Unlock_c       				0x01    /* M - unlock door */
#define gZclCmdDoorLock_Toggle_c       				0x02    /* O - toggle door */
#define gZclCmdDoorLock_UnlockWithTimeout_c       	0x03    /* O - unlock door with timeout */
#define gZclCmdDoorLock_GetLogRecord_c       		0x04    /* O - get log record */
#define gZclCmdDoorLock_SetPinCode_c       			0x05    /* O - Set pin code */
#define gZclCmdDoorLock_GetPinCode_c       			0x06    /* O - Get pin code */
#define gZclCmdDoorLock_ClearPinCode_c       		0x07    /* O - Clear pin code */
#define gZclCmdDoorLock_ClearAllPinCodes_c       	0x08    /* O - Clear all pin codes */
#define gZclCmdDoorLock_SetUserStatus_c       		0x09    /* O - set user status */
#define gZclCmdDoorLock_GetUserStatus_c       		0x0A    /* O - get user status */
#define gZclCmdDoorLock_SetWeekdaySchedule_c       	0x0B    /* O - Set Weekday Schedule */
#define gZclCmdDoorLock_GetWeekdaySchedule_c       	0x0C    /* O - Get Weekday Schedule */
#define gZclCmdDoorLock_ClearWeekdaySchedule_c      0x0D    /* O - Clear Weekday Schedule */
#define gZclCmdDoorLock_SetYeardaySchedule_c       	0x0E    /* O - Set YearDay Schedule */
#define gZclCmdDoorLock_GetYeardaySchedule_c       	0x0F    /* O - Get YearDay Schedule */
#define gZclCmdDoorLock_ClearYeardaySchedule_c      0x10    /* O - Clear YearDay Schedule */
#define gZclCmdDoorLock_SetHolidaySchedule_c       	0x11    /* O - Set Holiday Schedule */
#define gZclCmdDoorLock_GetHolidaySchedule_c       	0x12    /* O - Get Holiday Schedule */
#define gZclCmdDoorLock_ClearHolidaySchedule_c      0x13    /* O - Clear Holiday Schedule */
#define gZclCmdDoorLock_SetUserType_c     			0x14    /* O - Set User Type */
#define gZclCmdDoorLock_GetUserType_c     			0x15    /* O - Get User Type */
#define gZclCmdDoorLock_SetRFIDCode_c     			0x16    /* O - Set RFID Code */
#define gZclCmdDoorLock_GetRFIDCode_c     			0x17    /* O - Get RFID Code */
#define gZclCmdDoorLock_ClearRFIDCode_c     		0x18    /* O - Clear RFID Code */
#define gZclCmdDoorLock_ClearAllRFIDCodes_c     	0x19    /* O - Clear all RFID Codes */

/* 10.1.2.16 Commands Generated  */
#define gZclCmdDoorLock_LockRsp_c      				0x00    /* M - lock door response */
#define gZclCmdDoorLock_UnlockRsp_c   				0x01    /* M - unlock door response */
#define gZclCmdDoorLock_ToggleRsp_c       			0x02    /* O - toggle door response */
#define gZclCmdDoorLock_UnlockWithTimeoutRsp_c      0x03    /* O - unlock door with timeout response*/
#define gZclCmdDoorLock_GetLogRecordRsp_c       	0x04    /* O - get log record response*/
#define gZclCmdDoorLock_SetPinCodeRsp_c       		0x05    /* O - Set pin code response*/
#define gZclCmdDoorLock_GetPinCodeRsp_c       		0x06    /* O - Get pin code response*/
#define gZclCmdDoorLock_ClearPinCodeRsp_c       	0x07    /* O - Clear pin code response*/
#define gZclCmdDoorLock_ClearAllPinCodesRsp_c       0x08    /* O - Clear all pin codes response*/
#define gZclCmdDoorLock_SetUserStatusRsp_c       	0x09    /* O - set user status response */
#define gZclCmdDoorLock_GetUserStatusRsp_c       	0x0A    /* O - get user status response*/
#define gZclCmdDoorLock_SetWeekdayScheduleRsp_c     0x0B    /* O - Set Weekday Schedule response*/
#define gZclCmdDoorLock_GetWeekdayScheduleRsp_c     0x0C    /* O - Get Weekday Schedule response*/
#define gZclCmdDoorLock_ClearWeekdayScheduleRsp_c   0x0D    /* O - Clear Weekday Schedule response*/
#define gZclCmdDoorLock_SetYeardayScheduleRsp_c     0x0E    /* O - Set YearDay Schedule response*/
#define gZclCmdDoorLock_GetYeardayScheduleRsp_c     0x0F    /* O - Get YearDay Schedule response*/
#define gZclCmdDoorLock_ClearYeardayScheduleRsp_c   0x10    /* O - Clear YearDay Schedule response*/
#define gZclCmdDoorLock_SetHolidayScheduleRsp_c     0x11    /* O - Set Holiday Schedule response */
#define gZclCmdDoorLock_GetHolidayScheduleRsp_c     0x12    /* O - Get Holiday Schedule response*/
#define gZclCmdDoorLock_ClearHolidayScheduleRsp_c   0x13    /* O - Clear Holiday Schedule response*/
#define gZclCmdDoorLock_SetUserTypeRsp_c     		0x14    /* O - Set User Type response*/
#define gZclCmdDoorLock_GetUserTypeRsp_c     		0x15    /* O - Get User Type response*/
#define gZclCmdDoorLock_SetRFIDCodeRsp_c     		0x16    /* O - Set RFID Code response*/
#define gZclCmdDoorLock_GetRFIDCodeRsp_c     		0x17    /* O - Get RFID Code response*/
#define gZclCmdDoorLock_ClearRFIDCodeRsp_c     		0x18    /* O - Clear RFID Code response*/
#define gZclCmdDoorLock_ClearAllRFIDCodesRsp_c     	0x19    /* O - Clear all RFID Codes response*/
#define gZclCmdDoorLock_OperationEventNotification_c     	0x20    /* O - operation event notification*/
#define gZclCmdDoorLock_ProgrammingEventNotification_c     	0x21    /* O - programming event notification*/


/* app information: */

#define gZclDoorLock_NoOfTotalUsersSupported_c		0x02	/* no of total users supported */
#define gZclDoorLock_NoOfWeekDayScheduledPerUser_c	0x02	/* no of Week Day Scheduled Per User*/
#define gZclDoorLock_NoOfYearDayScheduledPerUser_c	0x02	/* no of Year Day Scheduled Per User*/
#define gZclDoorLock_MaxPinRfidCodeLength_c		    0x04	/* max number of Pin/Rfid code length*/

#define gZclDoorLock_InvalidIndex_c					0xFF

#define gZclDoorLock_InvalidUserId_c				0xFFFF		
#define gZclDoorLock_DefaultUserId_c				0x0000

#define gZclDoorLock_DefaultMasterKeyLength_c	0x04
#define gZclDoorLock_DefaultMasterKey_c			0x31, 0x32, 0x33, 0x34
#define gZclDoorLock_DefaultNonAccessUserKey_c	0x00, 0x00, 0x00, 0x00
/* user status value */
enum{
 gDoorLockUserStatus_Available_c = 0x00,
 gDoorLockUserStatus_Enabled_c,
 gDoorLockUserStatus_Reserved,
 gDoorLockUserStatus_Disabled_c,
 gDoorLockUserStatus_NotSupported_c = 0xFF
};

/* user type value */
enum{
 gDoorLockUserType_UnrestrictedUser_c = 0x00,
 gDoorLockUserType_YearDayScheduleUser_c,
 gDoorLockUserType_WeekDayScheduleUser_c,
 gDoorLockUserType_MasterUser_c,
 gDoorLockUserType_NonAccessUser_c,
 gDoorLockUserType_NotSupported_c = 0xFF
};


typedef PACKED_STRUCT zclDoorLock_PinCodeInf_tag
{
  uint8_t length;
  uint8_t pinCode[gZclDoorLock_MaxPinRfidCodeLength_c];
}zclDoorLock_PinCodeInf_t;

typedef PACKED_STRUCT zclDoorLock_usersTable_tag
{
  uint16_t userID;
  uint8_t  userStatus;
  uint8_t  userType;
  zclDoorLock_PinCodeInf_t 	pinInf;
  zclDoorLock_PinCodeInf_t	rfidInf;	
  uint8_t	indexInWeekDayScheduledTable;
  uint8_t	indexInYearDayScheduledTable;
}zclDoorLock_usersTable_t;


typedef PACKED_STRUCT zclDoorLock_WeekDayScheduledInf_tag
{
  uint8_t scheduleId;
  uint8_t dayMask;
  uint8_t startHour;
  uint8_t startMinute;
  uint8_t endHour;
  uint8_t endMinute;
}zclDoorLock_WeekDayScheduledInf_t;

typedef PACKED_STRUCT zclDoorLock_WeekDayScheduleTable_tag
{
 bool_t slotStatus;										/* False = FREE; TRUE = Occupied */		
 zclDoorLock_WeekDayScheduledInf_t weekDayScheduledInf[gZclDoorLock_NoOfWeekDayScheduledPerUser_c];
}zclDoorLock_WeekDayScheduleTable_t;

typedef PACKED_STRUCT zclDoorLock_YearDayScheduledInf_tag
{
 uint8_t 	scheduleId;
 uint32_t	zigBeeLocalStartTime;
 uint32_t	zigBeeLocalEndTime;
}zclDoorLock_YearDayScheduledInf_t;

typedef PACKED_STRUCT zclDoorLock_YearDayScheduleTable_tag
{
 bool_t slotStatus;										/* False = FREE; TRUE = Occupied */		
 zclDoorLock_YearDayScheduledInf_t yearDayScheduledInf[gZclDoorLock_NoOfYearDayScheduledPerUser_c];
}zclDoorLock_YearDayScheduleTable_t;


/*payload format for the lock/unlock/toggle command*/
typedef PACKED_STRUCT zclCmdDoorLockReq_tag
{
  uint8_t       length;
  uint8_t 		pinCode[1];		/* pin/RFID code */
}zclCmdDoorLockReq_t;


/* [R1] - 10.1.2.15.1 LockDoor, 10.1.2.15.2 UnlockDoor, 10.1.2.15.3 Toggle */
typedef PACKED_STRUCT zclDoorLockReq_tag
{
  afAddrInfo_t                   addrInfo;
  uint8_t                        zclTransactionId;
  zclCmdDoorLockReq_t			 cmdFrame;
}zclDoorLockReq_t;


/*payload format for the unlock with timeout command*/
typedef PACKED_STRUCT zclCmdDoorUnlockWithTimeout_tag
{
  uint16_t				timeout;		/* timeout in seconds*/	
  zclCmdDoorLockReq_t	pinInformation;
}zclCmdDoorUnlockWithTimeout_t;


/* [R1] - 10.1.2.15.4  UnlockWithTimeout command */
typedef PACKED_STRUCT zclDoorUnlockWithTimeout_tag
{
  afAddrInfo_t                  	addrInfo;
  uint8_t                        	zclTransactionId;
  zclCmdDoorUnlockWithTimeout_t		cmdFrame;
}zclDoorUnlockWithTimeout_t;

/*payload format for the get log record command*/
typedef PACKED_STRUCT zclCmdDoorLockGetLogRecord_tag
{
  uint8_t		logIndex;			
}zclCmdDoorLockGetLogRecord_t;


/* [R1] - 10.1.2.15.5  Get log record command */
typedef PACKED_STRUCT zclDoorLockGetLogRecord_tag
{
  afAddrInfo_t                  	addrInfo;
  uint8_t                        	zclTransactionId;
  zclCmdDoorLockGetLogRecord_t		cmdFrame;
}zclDoorLockGetLogRecord_t;

/*payload format for the set pin code command and Set RFID code command*/
typedef PACKED_STRUCT zclCmdDoorLockSetPinCode_tag
{
  uint16_t				userID;
  uint8_t 				userStatus;
  uint8_t				userType;
  zclCmdDoorLockReq_t	pinInformation;
}zclCmdDoorLockSetPinCode_t;


/* [R1] - 10.1.2.15.7  Set pin code command, 10.1.2.15.24 Set RFID code command */
typedef PACKED_STRUCT zclDoorLockSetPinRFIDCode_tag
{
  afAddrInfo_t                  	addrInfo;
  uint8_t                        	zclTransactionId;
  zclCmdDoorLockSetPinCode_t		cmdFrame;
}zclDoorLockSetPinRFIDCode_t;

/*payload format for the Get pin code command*/
typedef PACKED_STRUCT zclCmdDoorLockGetPinCode_tag
{
  uint16_t		userID;
}zclCmdDoorLockGetPinCode_t;


/* [R1] - 10.1.2.15.8  Get pin code command, 10.1.2.15.25 Get RFID code command*/
typedef PACKED_STRUCT zclDoorLockGetPinRFIDCode_tag
{
  afAddrInfo_t                  	addrInfo;
  uint8_t                        	zclTransactionId;
  zclCmdDoorLockGetPinCode_t		cmdFrame;
}zclDoorLockGetPinRFIDCode_t;

/* [R1] - 10.1.2.15.9  Clear pin code command, 10.1.2.15.26 Clear RFID code command*/
typedef PACKED_STRUCT zclDoorLockClearPinRFIDCode_tag
{
  afAddrInfo_t                  	addrInfo;
  uint8_t                        	zclTransactionId;
  zclCmdDoorLockGetPinCode_t		cmdFrame;
}zclDoorLockClearPinRFIDCode_t;

/* [R1] - 10.1.2.15.10  Clear All pin codes command,  10.1.2.15.27 Clear All RFID codes command  */
typedef PACKED_STRUCT zclDoorLockClearAllPinRFIDCodes_tag
{
  afAddrInfo_t                  	addrInfo;
  uint8_t                        	zclTransactionId;
}zclDoorLockClearAllPinRFIDCodes_t;

/*payload format for the Set user status command*/
typedef PACKED_STRUCT zclCmdDoorLockSetUserStatus_tag
{
  uint16_t		userID;
  uint8_t 		userStatus;
}zclCmdDoorLockSetUserStatus_t;


/* [R1] - 10.1.2.15.11 Set user status command */
typedef PACKED_STRUCT zclDoorLockSetUserStatus_tag
{
  afAddrInfo_t                  	addrInfo;
  uint8_t                        	zclTransactionId;
  zclCmdDoorLockSetUserStatus_t		cmdFrame;
}zclDoorLockSetUserStatus_t;

/* payload format for the get user status command*/
typedef PACKED_STRUCT zclCmdDoorLockGetUserStatus_tag
{
  uint16_t		userID;
}zclCmdDoorLockGetUserStatus_t;

/* [R1] - 10.1.2.15.12 Get user status command */
typedef PACKED_STRUCT zclDoorLockGetUserStatus_tag
{
  afAddrInfo_t                  	addrInfo;
  uint8_t                        	zclTransactionId;
  zclCmdDoorLockGetPinCode_t		cmdFrame;
}zclDoorLockGetUserStatus_t;


/*payload format for the Set week day schedule command*/
typedef PACKED_STRUCT zclCmdDoorLockSetWeekDaySchedule_tag
{
  uint8_t		scheduleID;
  uint16_t 		userID;
  uint8_t 		daysMask;
  uint8_t		startHour;
  uint8_t 		startMinute;
  uint8_t		endHour;
  uint8_t		endMinute;
}zclCmdDoorLockSetWeekDaySchedule_t;


/* [R1] - 10.1.2.15.13 set week day schedule command */
typedef PACKED_STRUCT zclDoorLockSetWeekDaySchedule_tag
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockSetWeekDaySchedule_t	cmdFrame;
}zclDoorLockSetWeekDaySchedule_t;

/*payload format for the Get/clear week day schedule command*/
typedef PACKED_STRUCT zclCmdDoorLockGetWeekDaySchedule_tag
{
  uint8_t		scheduleID;
  uint16_t 		userID;
}zclCmdDoorLockGetWeekDaySchedule_t;


/* [R1] - 10.1.2.15.14 Get week day schedule command */
typedef PACKED_STRUCT zclDoorLockGetWeekDaySchedule_tag
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockGetWeekDaySchedule_t	cmdFrame;
}zclDoorLockGetWeekDaySchedule_t;


/* [R1] - 10.1.2.15.15 Clear week day schedule command */
typedef PACKED_STRUCT zclDoorLockClearWeekDaySchedule_tag
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockGetWeekDaySchedule_t	cmdFrame;
}zclDoorLockClearWeekDaySchedule_t;

/*payload format for the Set year day schedule command*/
typedef PACKED_STRUCT zclCmdDoorLockSetYearDaySchedule_tag
{
  uint8_t		scheduleID;
  uint16_t 		userID;
  uint32_t		zigBeeLocalStartTime;
  uint32_t		zigBeeLocalEndTime;
}zclCmdDoorLockSetYearDaySchedule_t;


/* [R1] - 10.1.2.15.16 Set year day schedule  command */
typedef PACKED_STRUCT zclDoorLockSetYearDaySchedule_tag
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockSetYearDaySchedule_t	cmdFrame;
}zclDoorLockSetYearDaySchedule_t;

/*payload format for the Get/clear year day schedule command*/
typedef PACKED_STRUCT zclCmdDoorLockGetYearDaySchedule_tag
{
  uint8_t		scheduleID;
  uint16_t 		userID;
} zclCmdDoorLockGetYearDaySchedule_t;


/* [R1] - 10.1.2.15.17 Get year day schedule  command */
typedef PACKED_STRUCT zclDoorLockGetYearDaySchedule_tag
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockGetYearDaySchedule_t	cmdFrame;
}zclDoorLockGetYearDaySchedule_t;


/* [R1] - 10.1.2.15.18 Clear year day schedule  command */
typedef PACKED_STRUCT zclDoorLockClearYearDaySchedule_tag
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockGetYearDaySchedule_t	cmdFrame;
}zclDoorLockClearYearDaySchedule_t;

/*payload format for the Set holiday schedule command*/
typedef PACKED_STRUCT zclCmdDoorLockSetHolidaySchedule_tag
{
  uint8_t		scheduleID;
  uint32_t		zigBeeLocalStartTime;
  uint32_t		zigBeeLocalEndTime;
  uint8_t 		operatingModeDuringHoliday;
}zclCmdDoorLockSetHolidaySchedule_t;


/* [R1] - 10.1.2.15.19 Set holiday schedule  command */
typedef PACKED_STRUCT zclDoorLockSetHolidaySchedule_t
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockSetHolidaySchedule_t	cmdFrame;
}zclDoorLockSetHolidaySchedule_t;

/*payload format for the Get/clear holiday schedule command*/
typedef PACKED_STRUCT zclCmdDoorLockGetHolidaySchedule_tag
{
  uint8_t		scheduleID;
}zclCmdDoorLockGetHolidaySchedule_t;


/* [R1] - 10.1.2.15.20 Get holiday schedule  command */
typedef PACKED_STRUCT zclDoorLockGetHolidaySchedule_t
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockGetHolidaySchedule_t	cmdFrame;
}zclDoorLockGetHolidaySchedule_t;

/* [R1] - 10.1.2.15.21 Clear holiday schedule  command */
typedef PACKED_STRUCT zclDoorLockClearHolidaySchedule_t
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockGetHolidaySchedule_t	cmdFrame;
}zclDoorLockClearHolidaySchedule_t;


/*payload format for the set user type command*/
typedef PACKED_STRUCT zclCmdDoorLockSetUserType_tag
{
  uint16_t		userID;
  uint8_t 		userType;
}zclCmdDoorLockSetUserType_t;


/* [R1] - 10.1.2.15.22 set user type  command */
typedef PACKED_STRUCT zclDoorLockSetUserType_tag
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockSetUserType_t			cmdFrame;
}zclDoorLockSetUserType_t;

/*payload format for the get user type command*/
typedef PACKED_STRUCT zclCmdDoorLockGetUserType_tag
{
  uint16_t		userID;
}zclCmdDoorLockGetUserType_t;


/* [R1] - 10.1.2.15.23 Get user type  command */
typedef PACKED_STRUCT zclDoorLockGetUserType_tag
{
  afAddrInfo_t                  		addrInfo;
  uint8_t                        		zclTransactionId;
  zclCmdDoorLockGetUserType_t			cmdFrame;
}zclDoorLockGetUserType_t;



typedef PACKED_UNION zclZtcDoorLockReq_tag 
{
  zclDoorLockReq_t 				doorLockReq;
  zclDoorUnlockWithTimeout_t	doorLockUnlockWithTimeout;
  zclDoorLockSetPinRFIDCode_t	doorLockSetPinReq;
} zclZtcDoorLockReq_t; 


/*used to send a doorlock request command */
typedef PACKED_STRUCT zclDoorLockCmdReq_tag
{ 
  uint8_t 				cmdId;	
  zclZtcDoorLockReq_t	zclZtcDoorLockReq;
}zclDoorLockCmdReq_t;


typedef PACKED_UNION zclZtcDoorLockClearReq_tag 
{
  zclDoorLockClearPinRFIDCode_t 	doorLockClearPinRfidCode;
  zclDoorLockClearAllPinRFIDCodes_t	doorLockClearAllPinRfidCodes;
  zclDoorLockClearWeekDaySchedule_t	doorLockClearWeekDaySchedule;
  zclDoorLockClearYearDaySchedule_t doorLockClearYearDaySchedule;
  zclDoorLockClearHolidaySchedule_t doorLockClearHolidaySchedule;
} zclZtcDoorLockClearReq_t; 


/*used to send a doorlockClear commands request */
typedef PACKED_STRUCT zclDoorLockClearCmdReq_tag
{ 
  uint8_t 					cmdId;	
  zclZtcDoorLockClearReq_t	zclZtcDoorLockClearReq;
}zclDoorLockClearCmdReq_t;


/* payload format for the 10.1.2.16.1 LockResponse, 10.1.2.16.2 UnlockResponse, 10.1.2.16.3 ToggleResponse, 10.1.2.16.4 Unlock with timeout response, 
 * 10.1.2.16.6 SetPincode Response, 10.1.2.16.8 Clear Pin Code Response, 10.1.2.16.9 Clear All pin codes response,
 * 10.1.2.16.10 Set User Status response, 10.1.2.16.12 Set weekDay Schedule response, 10.1.2.16.14 Clear week day Schedule Rsp,
 * 10.1.2.16.15 Set year day schedule Rsp, 10.1.2.16.17 clear year day schedule Rsp, 10.1.2.16.18 Set holiday schedule Rsp,  
 * 10.1.2.16.21 Set user type response, 10.1.2.16.25 Clear RFID code response, 10.1.2.16.26 Clear all RFID codes response*/
typedef PACKED_STRUCT zclCmdDoorLockStatusRsp_tag
{
  uint8_t                 statusCmd;
} zclCmdDoorLockStatusRsp_t;


#define gDoorLockStatus_Succes_c 						0x00
#define gDoorLockStatus_Failed_c						0x01
#define gDoorLockPinCodeStatus_GeneralFailure_c	 		0x01
#define gDoorLockPinCodeStatus_MemoryFull_c 			0x02
#define gDoorLockPinCodeStatus_DuplicateCodeError_c 	0x03

/* 10.1.2.16.1 LockResponse, 10.1.2.16.2 UnlockResponse, 10.1.2.16.3 ToggleResponse, 10.1.2.16.4 Unlock with timeout response, 
 * 10.1.2.16.6 SetPincode Response, 10.1.2.16.8 Clear Pin Code Response, 10.1.2.16.9 Clear All pin codes response,
 * 10.1.2.16.10 Set User Status response, 10.1.2.16.12 Set weekDay Schedule response, 10.1.2.16.14 Clear week day Schedule Rsp,
 * 10.1.2.16.15 Set year day schedule Rsp, 10.1.2.16.17 clear year day schedule Rsp, 10.1.2.16.18 Set holiday schedule Rsp,  
 * 10.1.2.16.21 Set user type response, 10.1.2.16.25 Clear RFID code response, 10.1.2.16.26 Clear all RFID codes response*/
typedef PACKED_STRUCT zclDoorLockStatusRsp_tag
{
  afAddrInfo_t           		addrInfo;
  uint8_t                 		zclTransactionId;
  zclCmdDoorLockStatusRsp_t	  	cmdFrame;
}zclDoorLockStatusRsp_t;

/* payload format for the get log record response */
typedef PACKED_STRUCT zclCmdDoorLockGetLogRecordRsp_tag
{
  uint16_t      logEntryID;
  uint32_t		timestamp;
  uint8_t		eventType;
  uint8_t 		source;
  uint8_t		eventId;
  uint16_t		userID;
  uint8_t		pinLength;
  uint8_t		pin[1];
} zclCmdDoorLockGetLogRecordRsp_t;

/* 10.1.2.16.5 get log record response response*/
typedef PACKED_STRUCT zclDoorLockgetLogRecordRsp_tag
{
  afAddrInfo_t            			addrInfo;
  uint8_t                 			zclTransactionId;
  zclCmdDoorLockGetLogRecordRsp_t	cmdFrame;
}zclDoorLockgetLogRecordRsp_t;

/* payload format for the get pin code response and get RFID code response */
typedef PACKED_STRUCT zclCmdDoorLockGetPinCodeRsp_tag
{
  uint16_t		userID;
  uint8_t		userStatus;
  uint8_t		userType;
  uint8_t		pinLength;
  uint8_t		pin[1];
} zclCmdDoorLockGetPinCodeRsp_t;

/* 10.1.2.16.7 get pin code response, 10.1.2.16.24 get user Type response*/
typedef PACKED_STRUCT zclDoorLockGetPinRFIDCodeRsp_tag
{
  afAddrInfo_t            			addrInfo;
  uint8_t                 			zclTransactionId;
  zclCmdDoorLockGetPinCodeRsp_t		cmdFrame;
}zclDoorLockGetPinRFIDCodeRsp_t;

/* payload format for the get user status response */
typedef PACKED_STRUCT zclCmdDoorLockGetUserStatusRsp_tag
{
  uint16_t		userID;
  uint8_t		userStatus;
} zclCmdDoorLockGetUserStatusRsp_t;

/* 10.1.2.16.11 get user status response */
typedef PACKED_STRUCT zclDoorLockGetUserStatusRsp_tag
{
  afAddrInfo_t            				addrInfo;
  uint8_t                 				zclTransactionId;
  zclCmdDoorLockGetUserStatusRsp_t		cmdFrame;
}zclDoorLockGetUserStatusRsp_t;


/* payload format for the get week day schedule response */
typedef PACKED_STRUCT zclCmdDoorLockGetWeekDayScheduleRsp_tag
{
  uint8_t		scheduleID;
  uint16_t		userID;
  uint8_t 		status;
  uint8_t 		daysMask;
  uint8_t		startHour;
  uint8_t		startMinute;
  uint8_t		endHour;
  uint8_t     	endMinute;
} zclCmdDoorLockGetWeekDayScheduleRsp_t;

/* 10.1.2.16.13 get week day schedule response */
typedef PACKED_STRUCT zclDoorLockGetWeekDayScheduleRsp_tag
{
  afAddrInfo_t            					addrInfo;
  uint8_t                 					zclTransactionId;
  zclCmdDoorLockGetWeekDayScheduleRsp_t		cmdFrame;
}zclDoorLockGetWeekDayScheduleRsp_t;

/* payload format for the get year day schedule response */
typedef PACKED_STRUCT zclCmdDoorLockGetYearDayScheduleRsp_tag
{
  uint8_t		scheduleID;
  uint16_t		userID;
  uint8_t 		status;
  uint32_t		zigBeeLocalStartTime;
  uint32_t		zigBeeLocalEndTime;
} zclCmdDoorLockGetYearDayScheduleRsp_t;

/* 10.1.2.16.16 get year day schedule response */
typedef PACKED_STRUCT zclDoorLockGetYearDayScheduleRsp_tag
{
  afAddrInfo_t            					addrInfo;
  uint8_t                 					zclTransactionId;
  zclCmdDoorLockGetYearDayScheduleRsp_t		cmdFrame;
}zclDoorLockGetYearDayScheduleRsp_t;

/* payload format for the get holiday schedule response */
typedef PACKED_STRUCT zclCmdDoorLockGetHolidayScheduleRsp_tag
{
  uint8_t		scheduleID;
  uint8_t 		status;
  uint32_t		zigBeeLocalStartTime;
  uint32_t		zigBeeLocalEndTime;
  uint8_t		operatingModeDuringHoliday;
}zclCmdDoorLockGetHolidayScheduleRsp_t;

/* 10.1.2.16.19 get holiday schedule response */
typedef PACKED_STRUCT zclDoorLockGetHolidayScheduleRsp_tag
{
  afAddrInfo_t            					addrInfo;
  uint8_t                 					zclTransactionId;
  zclCmdDoorLockGetHolidayScheduleRsp_t		cmdFrame;
}zclDoorLockGetHolidayScheduleRsp_t;

/* payload format for the get user type response */
typedef PACKED_STRUCT zclCmdDoorLockGetUserTypeRsp_tag
{
  uint16_t 		userID;
  uint8_t		userType;
}zclCmdDoorLockGetUserTypeRsp_t;

/* 10.1.2.16.22 get user Type response */
typedef PACKED_STRUCT zclDoorLockGetUserTypeRsp_tag
{
  afAddrInfo_t            					addrInfo;
  uint8_t                 					zclTransactionId;
  zclCmdDoorLockGetUserTypeRsp_t			cmdFrame;
}zclDoorLockGetUserTypeRsp_t;



/* operation event source value */
enum
{
	gDoorLockOperationEventSource_Keypad = 0x00,
	gDoorLockOperationEventSource_RF,
	gDoorLockOperationEventSource_Manual,
	gDoorLockOperationEventSource_RFID,
	gDoorLockOperationEventSource_Indeterminate = 0xFF
};

/* payload format for the operation event notification  */
typedef PACKED_STRUCT zclCmdDoorLockOperationEventNotif_tag
{
  uint8_t 		operationEventSource;
  uint8_t		operationEventCode;
  uint16_t 		userID;
  uint8_t		pin;
  uint32_t		zigbeeLocalTime;
  uint8_t		data[1];
}zclCmdDoorLockOperationEventNotif_t;

/* 10.1.2.16.27 operation event notification */
typedef PACKED_STRUCT zclDoorLockOperationEventNotif_tag
{
  afAddrInfo_t            					addrInfo;
  uint8_t                 					zclTransactionId;
  zclCmdDoorLockOperationEventNotif_t		cmdFrame;
}zclDoorLockOperationEventNotif_t;


/* payload format for the Programming event notification  */
typedef PACKED_STRUCT zclCmdDoorLockProgrammingEventNotif_tag
{
  uint8_t 		programEventSource;
  uint8_t		programEventCode;
  uint16_t 		userID;
  uint8_t		pin;
  uint8_t		userType;
  uint8_t		userStatus;
  uint32_t		zigbeeLocalTime;
  uint8_t		data[1];
}zclCmdDoorLockProgrammingEventNotif_t;

/* 10.1.2.16.28 Programming event notification */
typedef PACKED_STRUCT zclDoorLockProgrammingEventNotif_tag
{
  afAddrInfo_t            					addrInfo;
  uint8_t                 					zclTransactionId;
  zclCmdDoorLockProgrammingEventNotif_t		cmdFrame;
}zclDoorLockProgrammingEventNotif_t;

/******************************************
	Window Covering Cluster
*******************************************/
/* Window Covering Attributes Sets */

#define gZclAttrSetWindowCovering_c   							0x00
#define gZclAttrIdWindowCoveringType_c           				0x00    /* M - Window Covering Type */
#define gZclAttrIdWindowCoveringPhysicalClosedLimitLift_c      	0x01    /* O - PhysicalClosedLimitLift */
#define gZclAttrIdWindowCoveringPhysicalClosedLimitTilt_c      	0x02    /* O - PhysicalClosedLimitTilt */
#define gZclAttrIdWindowCoveringCurrentPositionLift_c      		0x03    /* O - CurrentPositionLift */
#define gZclAttrIdWindowCoveringCurrentPositionTilt_c      		0x04    /* O - CurrentPositionTilt */
#define gZclAttrIdWindowCoveringNoOfActuationsLift_c      		0x05    /* O - NoOfActuationsLift */
#define gZclAttrIdWindowCoveringNoOfActuationsTilt_c      		0x06    /* O - NoOfActuationsTilt */
#define gZclAttrIdWindowCoveringConfigStatus_c      			0x07    /* M - ConfigStatus */
#define gZclAttrIdWindowCoveringCurrentPositionLiftPercentage_c 0x08    /* O - CurrentPositionLiftPercentage */
#define gZclAttrIdWindowCoveringCurrentPositionTiltPercentage_c	0x09    /* O - CurrentPositionTiltPercentage */
#define gZclAttrIdWindowCoveringInstalledOpenLimitLift_c        0x10    /* M - InstalledOpenLimitLift */
#define gZclAttrIdWindowCoveringInstalledClosedLimitLift_c      0x11    /* M - InstalledClosedLimitLift */
#define gZclAttrIdWindowCoveringInstalledOpenLimitTilt_c      	0x12    /* M - InstalledOpenLimitTilt */
#define gZclAttrIdWindowCoveringInstalledClosedLimitTilt_c      0x13    /* M - InstalledClosedLimitTilt */
#define gZclAttrIdWindowCoveringVelocityLift_c      			0x14    /* O - VelocityLift */
#define gZclAttrIdWindowCoveringAccelerationTimeLift_c      	0x15    /* O - AccelerationTimeLift */
#define gZclAttrIdWindowCoveringDecelerationTimeLift_c      	0x16    /* O - DecelerationTimeLift */
#define gZclAttrIdWindowCoveringMode_c      					0x17    /* M - Mode */
#define gZclAttrIdWindowCoveringIntermediateSetpointsLift_c 	0x18    /* O - IntermediateSetpointsLift */
#define gZclAttrIdWindowCoveringIntermediateSetpointsTilt_c		0x19    /* O - IntermediateSetpointsTilt */


#if ( TRUE == gBigEndian_c )
#define gZclAttrWindowCoveringType_c           					0x0000    /* M - Window Covering Type */
#define gZclAttrWindowCoveringPhysicalClosedLimitLift_c      	0x0100    /* O - PhysicalClosedLimitLift */
#define gZclAttrWindowCoveringPhysicalClosedLimitTilt_c      	0x0200    /* O - PhysicalClosedLimitTilt */
#define gZclAttrWindowCoveringCurrentPositionLift_c      		0x0300    /* O - CurrentPositionLift */
#define gZclAttrWindowCoveringCurrentPositionTilt_c      		0x0400    /* O - CurrentPositionTilt */
#define gZclAttrWindowCoveringNoOfActuationsLift_c      		0x0500    /* O - NoOfActuationsLift */
#define gZclAttrWindowCoveringNoOfActuationsTilt_c      		0x0600    /* O - NoOfActuationsTilt */
#define gZclAttrWindowCoveringConfigStatus_c      				0x0700    /* M - ConfigStatus */
#define gZclAttrWindowCoveringCurrentPositionLiftPercentage_c 	0x0800    /* O - CurrentPositionLiftPercentage */
#define gZclAttrWindowCoveringCurrentPositionTiltPercentage_c	0x0900    /* O - CurrentPositionTiltPercentage */
#define gZclAttrWindowCoveringInstalledOpenLimitLift_c        	0x1000    /* O - InstalledOpenLimitLift */
#define gZclAttrWindowCoveringInstalledClosedLimitLift_c     	0x1100    /* O - InstalledClosedLimitLift */
#define gZclAttrWindowCoveringInstalledOpenLimitTilt_c      	0x1200    /* O - InstalledOpenLimitTilt */
#define gZclAttrWindowCoveringInstalledClosedLimitTilt_c      	0x1300    /* O - InstalledClosedLimitTilt */
#define gZclAttrWindowCoveringVelocityLift_c      				0x1400    /* O - VelocityLift */
#define gZclAttrWindowCoveringAccelerationTimeLift_c      		0x1500    /* O - AccelerationTimeLift */
#define gZclAttrWindowCoveringDecelerationTimeLift_c      		0x1600    /* O - DecelerationTimeLift */
#define gZclAttrWindowCoveringMode_c      						0x1700    /* M - Mode */
#define gZclAttrWindowCoveringIntermediateSetpointsLift_c 		0x1800    /* O - IntermediateSetpointsLift */
#define gZclAttrWindowCoveringIntermediateSetpointsTilt_c		0x1900    /* O - IntermediateSetpointsTilt */
#else
#define gZclAttrWindowCoveringType_c           					0x0000    /* M - Window Covering Type */
#define gZclAttrWindowCoveringPhysicalClosedLimitLift_c      	0x0001    /* O - PhysicalClosedLimitLift */
#define gZclAttrWindowCoveringPhysicalClosedLimitTilt_c      	0x0002    /* O - PhysicalClosedLimitTilt */
#define gZclAttrWindowCoveringCurrentPositionLift_c      		0x0003    /* O - CurrentPositionLift */
#define gZclAttrWindowCoveringCurrentPositionTilt_c      		0x0004    /* O - CurrentPositionTilt */
#define gZclAttrWindowCoveringNoOfActuationsLift_c      		0x0005    /* O - NoOfActuationsLift */
#define gZclAttrWindowCoveringNoOfActuationsTilt_c      		0x0006    /* O - NoOfActuationsTilt */
#define gZclAttrWindowCoveringConfigStatus_c      				0x0007    /* M - ConfigStatus */
#define gZclAttrWindowCoveringCurrentPositionLiftPercentage_c 	0x0008    /* O - CurrentPositionLiftPercentage */
#define gZclAttrWindowCoveringCurrentPositionTiltPercentage_c	0x0009    /* O - CurrentPositionTiltPercentage */
#define gZclAttrWindowCoveringInstalledOpenLimitLift_c        	0x0010    /* O - InstalledOpenLimitLift */
#define gZclAttrWindowCoveringInstalledClosedLimitLift_c     	0x0011    /* O - InstalledClosedLimitLift */
#define gZclAttrWindowCoveringInstalledOpenLimitTilt_c      	0x0012    /* O - InstalledOpenLimitTilt */
#define gZclAttrWindowCoveringInstalledClosedLimitTilt_c      	0x0013    /* O - InstalledClosedLimitTilt */
#define gZclAttrWindowCoveringVelocityLift_c      				0x0014    /* O - VelocityLift */
#define gZclAttrWindowCoveringAccelerationTimeLift_c      		0x0015    /* O - AccelerationTimeLift */
#define gZclAttrWindowCoveringDecelerationTimeLift_c      		0x0016    /* O - DecelerationTimeLift */
#define gZclAttrWindowCoveringMode_c      						0x0017    /* M - Mode */
#define gZclAttrWindowCoveringIntermediateSetpointsLift_c 		0x0018    /* O - IntermediateSetpointsLift */
#define gZclAttrWindowCoveringIntermediateSetpointsTilt_c		0x0019    /* O - IntermediateSetpointsTilt */
#endif 

/* window covering type: [R1] table 9.22 */
#define gZclAttrWindowCoveringType_Rollershade_c  		 		0x00
#define gZclAttrWindowCoveringType_Rollershade2Motor_c   		0x01
#define gZclAttrWindowCoveringType_RollershadeExterior_c  		0x02
#define gZclAttrWindowCoveringType_RollershadeExterior2Motor_c	0x03
#define gZclAttrWindowCoveringType_Drapery_c					0x04
#define gZclAttrWindowCoveringType_Awning_c						0x05
#define gZclAttrWindowCoveringType_Shutter_c					0x06
#define gZclAttrWindowCoveringType_TiltBlindTiltOnly_c			0x07
#define gZclAttrWindowCoveringType_TiltBlindLiftAndTilt_c		0x08
#define gZclAttrWindowCoveringType_ProjectorScreen_c			0x09

#define gZclWindowCovering_MaxPercentageValue_c	100

typedef PACKED_STRUCT zclWindowCoveringAttrsRAM_tag
{
	uint8_t 	type;
	uint8_t 	configStatus;
	uint16_t 	installedOpenLimitLift;
	uint16_t 	installedClosedLimitLift;
	uint16_t 	installedOpenLimitTilt;
	uint16_t 	installedClosedLimitTilt;	
	uint8_t 	mode;
#if gZclClusterOptionals_d	
	uint16_t 	physicalClosedLimitLift;
	uint16_t 	physicalClosedLimitTilt;
	uint16_t 	currentPositionLift[zclReportableCopies_c];
	uint16_t 	currentPositionTilt[zclReportableCopies_c];
	uint16_t 	noOfActuationsLift;
	uint16_t 	noOfActuationsTilt;
	uint8_t 	currentPositionLiftPercentage;
	uint8_t 	currentPositionTiltPercentage;
	uint16_t 	velocity;
	uint16_t 	accelerationTimeLift;
	uint16_t 	decelerationTimeLift;
	zclArrayUint16_t intermediateSetpointsLift;
	zclArrayUint16_t intermediateSetpointsTilt;	
#endif	
}zclWindowCoveringAttrsRAM_t;


/* 9.3.2.2 Commands Received  */
#define gZclCmdWindowCovering_UpOpen_c         			0x00    /* M - Up/Open */
#define gZclCmdWindowCovering_DownClose_c     			0x01    /* M - Down/Close */
#define gZclCmdWindowCovering_Stop_c     				0x02    /* M - Stop */
#define gZclCmdWindowCovering_GoToLiftValue_c   		0x04    /* O - Go To Lift Value */
#define gZclCmdWindowCovering_GoToLiftPercentage_c  	0x05    /* O - Go To Lift Percentage */
#define gZclCmdWindowCovering_GoToTiltValue_c   		0x07    /* O - Go To Tilt Value */
#define gZclCmdWindowCovering_GoToTiltPercentage_c  	0x08    /* O - Go To Tilt Percentage */



/* [R1] 9.3.2.2.1  Up/Open Command, 9.3.2.2.2  Down/Close Command,  9.3.2.2.3 Stop Command */
/*	no payload data for these commands */
typedef PACKED_STRUCT zclWindowCovering_NoPayload_tag 
{
   afAddrInfo_t                             addrInfo;
   uint8_t                                  zclTransactionId;
}zclWindowCovering_NoPayload_t;


/* payload format for Go To lift/tilt Value Command */
typedef PACKED_STRUCT zclCmdWindowCovering_GoToValue_tag 
{
  uint16_t value;
}zclCmdWindowCovering_GoToValue_t; 

/* [R1] 9.3.2.2.4 Go To Lift Value Command, 9.3.2.2.4.3  Go To Tilt Value Command */
typedef PACKED_STRUCT zclWindowCovering_GoToValue_tag 
{
   afAddrInfo_t                             addrInfo;
   uint8_t                                  zclTransactionId;
   zclCmdWindowCovering_GoToValue_t			cmdFrame;
}zclWindowCovering_GoToValue_t;

/* payload format for Go To lift/tilt Percentage Command */
typedef PACKED_STRUCT zclCmdWindowCovering_GoToPercentage_tag 
{
  uint8_t PercentageValue;
}zclCmdWindowCovering_GoToPercentage_t; 

/* [R1] 9.3.2.2.4.2 Go To Lift Percentage Command, 9.3.2.2.4.4  Go To Tilt Percentage Command */
typedef PACKED_STRUCT zclWindowCovering_GoToPercentage_tag 
{
   afAddrInfo_t                             addrInfo;
   uint8_t                                  zclTransactionId;
   zclCmdWindowCovering_GoToPercentage_t	cmdFrame;
}zclWindowCovering_GoToPercentage_t;



/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/

/* cluster servers */
/*!
 * @fn 		zbStatus_t ZCL_ShadeCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Shade Configuration Cluster Server. 
 *
 */
zbStatus_t ZCL_ShadeCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_DoorLockClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the DoorLock Cluster Server. 
 *
 */
zbStatus_t ZCL_DoorLockClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_WindowCoveringClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the WindowCovering Cluster Server. 
 *
 */
zbStatus_t ZCL_WindowCoveringClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);

/* cluster client */
/*!
 * @fn 		zbStatus_t ZCL_ShadeCfgClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Shade Configuration Cluster Client. 
 *
 */
zbStatus_t ZCL_ShadeCfgClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_DoorLockClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the DoorLock Cluster Client. 
 *
 */
zbStatus_t ZCL_DoorLockClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_WindowCoveringClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Window Covering Cluster Client. 
 *
 */
zbStatus_t ZCL_WindowCoveringClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);

/* door lock cluster commands: */
/*client cmds generated: */
/*!
 * @fn 		zbStatus_t zclDoorLock_LockUnlockToogleReq(zclDoorLockReq_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a DoorLock(0x00)/Unlock(0x01)/Toogle(0x02) request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_LockUnlockToogleReq(zclDoorLockReq_t *pReq, uint8_t commandId);
/*!
 * @fn 		zbStatus_t zclDoorLock_UnlockWithTimeoutReq(zclDoorUnlockWithTimeout_t *pReq) 
 *
 * @brief	Sends over-the-air a UnlockWithTimeout request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_UnlockWithTimeoutReq(zclDoorUnlockWithTimeout_t *pReq);
/*!
 * @fn 		zbStatus_t zclDoorLock_GetLogRecord(zclDoorLockGetLogRecord_t *pReq) 
 *
 * @brief	Sends over-the-air a GetLogRecord  from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetLogRecord(zclDoorLockGetLogRecord_t *pReq); 
/*!
 * @fn 		zbStatus_t zclDoorLock_SetPinRFIDCode(zclDoorLockSetPinRFIDCode_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a SetPinCode/SetRfidCode request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetPinRFIDCode(zclDoorLockSetPinRFIDCode_t *pReq, uint8_t commandId); 
/*!
 * @fn 		zbStatus_t zclDoorLock_GetPinRFIDCode(zclDoorLockGetPinRFIDCode_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a GetPinCode/GetRfidCode request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetPinRFIDCode(zclDoorLockGetPinRFIDCode_t *pReq, uint8_t commandID);
/*!
 * @fn 		zbStatus_t zclDoorLock_ClearPinRFIDCode(zclDoorLockClearPinRFIDCode_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a ClearPinCode/ClearRfidCode request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_ClearPinRFIDCode(zclDoorLockClearPinRFIDCode_t *pReq, uint8_t commandID); 
/*!
 * @fn 		zbStatus_t zclDoorLock_ClearAllPinRFIDCodes(zclDoorLockClearAllPinRFIDCodes_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a ClearAllPinCodes/ClearAllRfidCodes request from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_ClearAllPinRFIDCodes(zclDoorLockClearAllPinRFIDCodes_t *pReq, uint8_t commandID); 
/*!
 * @fn 		zbStatus_t zclDoorLock_SetUserType(zclDoorLockSetUserType_t *pReq)
 *
 * @brief	Sends over-the-air a SetUserType command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetUserType(zclDoorLockSetUserType_t *pReq); 
/*!
 * @fn 		zbStatus_t zclDoorLock_GetUserType(zclDoorLockGetUserType_t *pReq)
 *
 * @brief	Sends over-the-air a GetUserType command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetUserType(zclDoorLockGetUserType_t *pReq);
/*!
 * @fn 		zbStatus_t zclDoorLock_SetUserStatus(zclDoorLockSetUserStatus_t *pReq)
 *
 * @brief	Sends over-the-air a SetUserStatus command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetUserStatus(zclDoorLockSetUserStatus_t *pReq);  
/*!
 * @fn 		zbStatus_t zclDoorLock_GetUserStatus(zclDoorLockGetUserStatus_t *pReq)
 *
 * @brief	Sends over-the-air a GetUserStatus command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetUserStatus(zclDoorLockGetUserStatus_t *pReq);
/*!
 * @fn 		zbStatus_t zclDoorLock_SetWeekDaySchedule(zclDoorLockSetWeekDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a SetWeekDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetWeekDaySchedule(zclDoorLockSetWeekDaySchedule_t *pReq);
/*!
 * @fn 		zbStatus_t zclDoorLock_GetWeekDaySchedule(zclDoorLockGetWeekDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a GetWeekDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetWeekDaySchedule(zclDoorLockGetWeekDaySchedule_t *pReq);
/*!
 * @fn 		zbStatus_t zclDoorLock_ClearWeekDaySchedule(zclDoorLockClearWeekDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a ClearWeekDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_ClearWeekDaySchedule(zclDoorLockClearWeekDaySchedule_t *pReq);
/*!
 * @fn 		zbStatus_t zclDoorLock_SetYearDaySchedule(zclDoorLockSetYearDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a SetYearDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetYearDaySchedule(zclDoorLockSetYearDaySchedule_t *pReq);
/*!
 * @fn 		zbStatus_t zclDoorLock_GetYearDaySchedule(zclDoorLockGetYearDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a GetYearDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetYearDaySchedule(zclDoorLockGetYearDaySchedule_t *pReq);
/*!
 * @fn 		zbStatus_t zclDoorLock_ClearYearDaySchedule(zclDoorLockClearYearDaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a ClearYearDaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_ClearYearDaySchedule(zclDoorLockClearYearDaySchedule_t *pReq);
/*!
 * @fn 		zbStatus_t zclDoorLock_SetHolidaySchedule(zclDoorLockSetHolidaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a SetHolidaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_SetHolidaySchedule(zclDoorLockSetHolidaySchedule_t *pReq);
/*!
 * @fn 		zbStatus_t zclDoorLock_GetHolidaySchedule(zclDoorLockGetHolidaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a GetHolidaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_GetHolidaySchedule(zclDoorLockGetHolidaySchedule_t *pReq); 
/*!
 * @fn 		zbStatus_t zclDoorLock_ClearHolidaySchedule(zclDoorLockClearHolidaySchedule_t *pReq)
 *
 * @brief	Sends over-the-air a ClearHolidaySchedule command from the DoorLock Cluster Client. 
 *
 */
zbStatus_t zclDoorLock_ClearHolidaySchedule(zclDoorLockClearHolidaySchedule_t *pReq); 

/*server cmds generated: */
 /*!
 * @fn 		zbStatus_t zclDoorLock_StatusRsp(zclDoorLockStatusRsp_t *pCommandRsp, uint8_t commandId) 
 *
 * @brief	Generic function used to send a LockRsp/UnlockRsp/ToogleRsp/UnlockWithTimeoutRsp/SetPinCodeRsp/ClearPinCodeRsp/
 *			ClearAllPinCodesRsp/SetUserStatusRsp/SetWeekDayScheduleRsp/ClearWeekDayScheduleRsp/SetYearDayScheduleRsp/ClearYearDayScheduleRsp/
 *			SetHolidayScheduleRsp/SetUserTypeRsp/ClearRFIDCodeRsp/ClearAllRfidCodesRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_StatusRsp(zclDoorLockStatusRsp_t *pCommandRsp, uint8_t commandId);
/*!
 * @fn 		zbStatus_t zclDoorLock_GetLogRecordRsp(zclDoorLockgetLogRecordRsp_t *pCommandRsp)
 *
 * @brief	Sends over-the-air a GetLogRecordRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetLogRecordRsp(zclDoorLockgetLogRecordRsp_t *pCommandRsp);
/*!
 * @fn 		zbStatus_t zclDoorLock_GetPinRFIDCodeRsp(zclDoorLockGetPinRFIDCodeRsp_t *pCommandRsp, uint8_t commandID)
 *
 * @brief	Sends over-the-air a GetPinCodeRsp/GetRfidCodeRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetPinRFIDCodeRsp(zclDoorLockGetPinRFIDCodeRsp_t *pCommandRsp, uint8_t commandId);
/*!
 * @fn 		zbStatus_t zclDoorLock_GetUserTypeRsp(zclDoorLockGetUserTypeRsp_t *pCommandRsp)
 *
 * @brief	Sends over-the-air a GetUserTypeRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetUserTypeRsp(zclDoorLockGetUserTypeRsp_t *pCommandRsp);
/*!
 * @fn 		zbStatus_t zclDoorLock_GetUserStatusRsp(zclDoorLockGetUserStatusRsp_t *pCommandRsp)
 *
 * @brief	Sends over-the-air a GetUserStatusRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetUserStatusRsp(zclDoorLockGetUserStatusRsp_t *pCommandRsp);
/*!
 * @fn 		zbStatus_t zclDoorLock_GetWeekDayScheduleRsp(zclDoorLockGetWeekDayScheduleRsp_t *pCommandRsp)
 *
 * @brief	Sends over-the-air a GetWeekDayScheduleRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetWeekDayScheduleRsp(zclDoorLockGetWeekDayScheduleRsp_t *pCommandRsp);
/*!
 * @fn 		zbStatus_t zclDoorLock_GetYearDayScheduleRsp(zclDoorLockGetYearDayScheduleRsp_t *pCommandRsp)
 *
 * @brief	Sends over-the-air a GetYearDayScheduleRsp command from the DoorLock Cluster Server. 
 *
 */
zbStatus_t zclDoorLock_GetYearDayScheduleRsp(zclDoorLockGetYearDayScheduleRsp_t *pCommandRsp);

/*!
 * @fn 		void DoorLockCluster_InitClusterServer(void)
 *
 * @brief	Init DoorLock Cluster Server functionality
 *
 */
void DoorLockCluster_InitClusterServer(void);


/* window covering cluster commands: */
/*!
 * @fn 		zbStatus_t zclWindowCovering_UpDownStopReq(zclWindowCovering_NoPayload_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a UpOpen/DownClose/Stop request from the WindowCovering Cluster Client. 
 *
 */
zbStatus_t zclWindowCovering_UpDownStopReq( zclWindowCovering_NoPayload_t *pReq,  uint8_t commandId);
/*!
 * @fn 		zbStatus_t zclWindowCovering_GoToValueReq(zclWindowCovering_GoToValue_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a GoToLiftValue/GoToTiltValue request from the WindowCovering Cluster Client. 
 *
 */
zbStatus_t zclWindowCovering_GoToValueReq(zclWindowCovering_GoToValue_t *pReq,  uint8_t commandId);
/*!
 * @fn 		zbStatus_t zclWindowCovering_GoToPercentageReq(zclWindowCovering_GoToPercentage_t *pReq, uint8_t commandId) 
 *
 * @brief	Sends over-the-air a GoToLiftPercentage/GoToTiltPercentage request from the WindowCovering Cluster Client. 
 *
 */
zbStatus_t zclWindowCovering_GoToPercentageReq(zclWindowCovering_GoToPercentage_t *pReq,  uint8_t commandId);
#ifdef __cplusplus
}
#endif
#endif