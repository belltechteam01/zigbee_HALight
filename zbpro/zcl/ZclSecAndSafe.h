/*! @file 	ZclSecAndSafe.h
 *
 * @brief	Types, definitions and prototypes for the ZCL Security and Safety domain.
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
#ifndef _ZCL_SECANDSAFE_H
#define _ZCL_SECANDSAFE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "ZclOptions.h"
#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "EmbeddedTypes.h"
#include "ZCL.h"
#include "ZCLGeneral.h"

/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/

/********************************************************
	IAS Zone Cluster (ZigBee Cluster Library Chapter 8.2)
********************************************************/


#define gZclAttrSetIASZone_c    0x00
#define gZclAttrIdIASZoneZoneInformationSet_c   0x00    /* Zone Information attributes Set */
#define gZclAttrIdIASZoneZoneSettingsSet_c      0x01    /* Zone Settings attributes Set */

#if ( TRUE == gBigEndian_c )
#define gZclAttrIASZoneZoneInformationSet_c   0x0000    /* Zone Information attributes Set */
#define gZclAttrIASZoneZoneSettingsSet_c      0x0100    /* Zone Settings attributes Set */
#else
#define gZclAttrIASZoneZoneInformationSet_c   0x0000    /* Zone Information attributes Set */
#define gZclAttrIASZoneZoneSettingsSet_c      0x0001    /* Zone Settings attributes Set */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* 8.2.2.2.1 Zone Information Attributes Set */

#define gZclAttrIdZoneInformationZoneState_c     0x00    /* M - Zone State attributes */
#define gZclAttrIdZoneInformationZoneType_c      0x01    /* M - Zone Type attributes */
#define gZclAttrIdZoneInformationZoneStatus_c    0x02    /* M - Zone Status attributes */

#if ( TRUE == gBigEndian_c )
#define gZclAttrZoneInformationZoneState_c     0x0000    /* M - Zone State attributes */
#define gZclAttrZoneInformationZoneType_c      0x0100    /* M - Zone Type attributes */
#define gZclAttrZoneInformationZoneStatus_c    0x0200    /* M - Zone Status attributes */
#else
#define gZclAttrZoneInformationZoneState_c     0x0000    /* M - Zone State attributes */
#define gZclAttrZoneInformationZoneType_c      0x0001    /* M - Zone Type attributes */
#define gZclAttrZoneInformationZoneStatus_c    0x0002    /* M - Zone Status attributes */
#endif /* #if ( TRUE == gBigEndian_c ) */


/* 8.2.2.2.1.1 Zone State Attributes */
enum
{
   gZclZoneState_NotEnrolled_c = 0x00,       /* Not Enrolled */
   gZclZoneState_Enrolled_c,                 /* Enrolled */
                                             /* 0x02-0xFF Reserved */
};

/* 8.2.2.2.1.2 Zone Type Attributes */

#define gIASApp_ZoneTypeId_d    14 /* number of valid zone types */

#if ( TRUE == gBigEndian_c )
#define gZclZoneType_StandardCIE_c               0x0000   /* Standard CIE */
#define gZclZoneType_MotionSensor_c              0x0D00   /* Motion Sensor */
#define gZclZoneType_ContactSwitch_c             0x1500   /* Contact Switch */
#define gZclZoneType_FireSensor_c                0x2800   /* Fire Sensor */
#define gZclZoneType_WaterSensor_c               0x2A00   /* Water Sensor */
#define gZclZoneType_GasSensor_c                 0x2B00   /* Gas Sensor */
#define gZclZoneType_PersonalEmergencyDevice_c   0x2C00   /* Personal Emergency Device */
#define gZclZoneType_VibrationMovement_c         0x2D00   /* Vibration/Movement Sensor */
#define gZclZoneType_RemoteControl_c             0x0F01   /* Remote Control */
#define gZclZoneType_KeyFob_c                    0x1501   /* Key fob */
#define gZclZoneType_Keypad_c                    0x1D02   /* Keypad */
#define gZclZoneType_StandardWarning_c           0x2502   /* Standard Warning Device */
#define gZclZoneType_GlassBreakSensor_c          0x5602   /* Glass break Sensor */
#define gZclZoneType_CarbonMonoxideSensor_c      0x5702   /* Carbon Monoxide Sensor */
#define gZclZoneType_InvalidZone_c               0xFFFF   /* Invalid Zone Type */
                                                          /* Other Values < 0x7FFF are Reserved */
                                                          /* 0x8000-0xFFFE Reserved for manufacturer 
                                                              specific types */
#else
#define gZclZoneType_StandardCIE_c               0x0000   /* Standard CIE */
#define gZclZoneType_MotionSensor_c              0x000D   /* Motion Sensor */
#define gZclZoneType_ContactSwitch_c             0x0015   /* Contact Switch */
#define gZclZoneType_FireSensor_c                0x0028   /* Fire Sensor */
#define gZclZoneType_WaterSensor_c               0x002A   /* Water Sensor */
#define gZclZoneType_GasSensor_c                 0x002B   /* Gas Sensor */
#define gZclZoneType_PersonalEmergencyDevice_c   0x002C   /* Personal Emergency Device */
#define gZclZoneType_VibrationMovement_c         0x002D   /* Vibration/Movement Sensor */
#define gZclZoneType_RemoteControl_c             0x010F   /* Remote Control */
#define gZclZoneType_KeyFob_c                    0x0115   /* Key fob */
#define gZclZoneType_Keypad_c                    0x021D   /* Keypad */
#define gZclZoneType_StandardWarning_c           0x0225   /* Standard Warning Device */
#define gZclZoneType_GlassBreakSensor_c          0x0256   /* Glass break Sensor */
#define gZclZoneType_CarbonMonoxideSensor_c      0x0257   /* Carbon Monoxide Sensor */
#define gZclZoneType_InvalidZone_c               0xFFFF   /* Invalid Zone Type */
                                                          /* Other Values < 0x7FFF are Reserved */
                                                          /* 0x8000-0xFFFE Reserved for manufacturer 
                                                              specific types */
#endif /* #if ( TRUE == gBigEndian_c ) */


/* 8.2.2.2.2 Zone Settings Attributes Set */
#define gZclAttrIdZoneSettingsIASCIEAddress_c     0x10    /* M - IAS CIE Address attributes */
#define gZclAttrIdZoneSettingsZoneId_c            0x11    /* M - Zone Id */
#define gZclAttrIdNoOfZoneSensitivityLevels_c     0x12    /* O - Number of Zone Sensitivity Levels */  
#define gZclAttrIdCurrentZoneSensitivityLevel_c   0x13    /* O - Current Zone Sensitivity Level */ 

#if ( TRUE == gBigEndian_c )
#define gZclAttrZoneSettingsIASCIEAddress_c     0x1000    /* M - IAS CIE Address attributes */
#define gZclAttrZoneSettingsZoneId_c            0x1100    /* M - Zone Id */
#define gZclAttrNoOfZoneSensitivityLevels_c     0x1200    /* O - Number of Zone Sensitivity Levels */  
#define gZclAttrCurrentZoneSensitivityLevel_c   0x1300    /* O - Current Zone Sensitivity Level */ 
#else
#define gZclAttrZoneSettingsIASCIEAddress_c     0x0010    /* M - IAS CIE Address attributes */
#define gZclAttrZoneSettingsZoneId_c            0x0011    /* M - Zone Id */
#define gZclAttrNoOfZoneSensitivityLevels_c     0x0012    /* O - Number of Zone Sensitivity Levels */  
#define gZclAttrCurrentZoneSensitivityLevel_c   0x0013    /* O - Current Zone Sensitivity Level */ 
#endif /* #if ( TRUE == gBigEndian_c ) */   

#ifndef gIASZoneEnableSensitivityLevels_d
#define gIASZoneEnableSensitivityLevels_d     TRUE
#endif

/* 8.2.2.2.1.3 Zone Status Attributes */
typedef PACKED_STRUCT gZclZoneStatus_tag
{
    uint16_t Alarm1             :1;                                 
    uint16_t Alarm2             :1;                                 
    uint16_t Tamper             :1;                                 
    uint16_t Battery            :1;                                
    uint16_t SupervisionReports :1;                     
    uint16_t RestoreReports     :1;                         
    uint16_t Trouble            :1;                                
    uint16_t ACmains            :1;
    uint16_t Test               :1;
    uint16_t BatteryDefect      :1;
    uint16_t Reserved           :6;  
}gZclZoneStatus_t;                   
  
#define ZONESTATUS_ALARMED_d                    1
#define ZONESTATUS_NOT_ALARMED_d                0
#define ZONESTATUS_TAMPERED_d                   1
#define ZONESTATUS_NOT_TAMPERED_d               0
#define ZONESTATUS_LOW_BATTERY_d                1
#define ZONESTATUS_BATTERY_OK_d                 0
#define ZONESTATUS_REPORTS_d                    1
#define ZONESTATUS_NOT_REPORT_d                 0
#define ZONESTATUS_FAILURE_d                    1
#define ZONESTATUS_OK_d                         0
#define ZONESTATUS_TEST_MODE_d                  1
#define ZONESTATUS_OPERATION_MODE_d             0
#define ZONESTATUS_DEFECTIVE_BATTERY_d          1
#define ZONESTATUS_FUNCTIONING_NORMALLY_d       0                                        

/* IAS Zone attributes RAM structure */
typedef PACKED_STRUCT zclIASZoneAttrsRAM_tag
{
  uint8_t         zoneState; 
  uint16_t        zoneType;
  uint16_t        zoneStatus;
  IEEEaddress_t   IASCIEAddress;
  uint8_t         zoneId;
#if gIASZoneEnableSensitivityLevels_d  
  uint8_t         noZoneSensLevel;
  uint8_t         curentZoneSensLevel;      
#endif  
} zclIASZoneAttrsRAM_t;

/* 8.2.2.3 IAS Zone server cluster Commands Received */

#define gZclCmdRxIASZone_ZoneEnrollResponse_c                   0x00    /* M - Zone Enroll Response */
#define gZclCmdRxIASZone_InitiateNormalOperationMode_c          0x01    /* O - Initiate normal operation mode */
#define gZclCmdRxIASZone_InitiateTestMode_c                     0x02    /* O - Initiate test mode */

#if (TRUE == gBigEndian_c)
#define gZclIasZoneManufacturerCode_c   0x1002
#else
#define gZclIasZoneManufacturerCode_c   0x0210
#endif

/* payload format for Zone EnrollResponse Command */
typedef PACKED_STRUCT zclCmdIASZone_ZoneEnrollResponse_tag 
{
 uint8_t  EnrollResponseCode;
 uint8_t  ZoneID;
}zclCmdIASZone_ZoneEnrollResponse_t; 

typedef PACKED_STRUCT zclIASZone_ZoneEnrollResponse_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASZone_ZoneEnrollResponse_t cmdFrame;
}zclIASZone_ZoneEnrollResponse_t; 

/* Enroll Response Code Enumeration*/
enum
{
   gEnrollResponseCode_Succes_c = 0x00,   /* Success */
   gEnrollResponseCode_NotSupported_c,    /* Not Supported */
   gEnrollResponseCode_NoEnrollPermit_c,  /* No Enroll Permit */
   gEnrollResponseCode_TooManyZones_c,    /* Too Many Zones */
                                          /* 0x04-0xFE Reserved */
};

/* HA1.2 Errata CCB1861 */
typedef PACKED_STRUCT zclIASZone_InitNormalOpMode_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
}zclIASZone_InitNormalOpMode_t; 

typedef PACKED_STRUCT zclCmdIASZone_InitTestMode_tag
{
  uint8_t testModeDuration;     /* in seconds */
  uint8_t currentZoneSensLevel;
}zclCmdIASZone_InitTestMode_t;

/* HA1.2 Errata CCB1861 */
typedef PACKED_STRUCT zclIASZone_InitTestMode_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASZone_InitTestMode_t cmdFrame;
}zclIASZone_InitTestMode_t; 

/* 8.2.2.4 IAS Zone server cluster Commands Generated */

#define gZclCmdTxIASZone_ZoneStatusChange_c             0x00    /* M - Zone Status Change Notification */
#define gZclCmdTxIASZone_ZoneEnrollRequest_c            0x01    /* M - Zone Enroll Request */

/* payload format for Zone Status Change Notification Command */
typedef PACKED_STRUCT zclCmdIASZone_ZoneStatusChange_tag 
{
 uint16_t   ZoneStatus;
 uint8_t    ExtendedStatus;
 uint8_t    ZoneId;
 uint16_t   Delay;
}zclCmdIASZone_ZoneStatusChange_t; 

typedef PACKED_STRUCT zclIASZone_ZoneStatusChange_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASZone_ZoneStatusChange_t cmdFrame;
}zclIASZone_ZoneStatusChange_t;

/* payload format for Zone Enroll Request Command */
typedef PACKED_STRUCT zclCmdIASZone_ZoneEnrollRequest_tag
{
 uint16_t   ZoneType;
 uint16_t   ManufacturerCode;
}zclCmdIASZone_ZoneEnrollRequest_t;

typedef PACKED_STRUCT zclIASZone_ZoneEnrollRequest_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASZone_ZoneEnrollRequest_t cmdFrame;
}zclIASZone_ZoneEnrollRequest_t;

/* HA1.2 Errata CCB1861 */
typedef PACKED_STRUCT zclIASZone_InitNormalOpModeRsp_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
}zclIASZone_InitNormalOpModeRsp_t; 

/* HA1.2 Errata CCB1861 */
typedef PACKED_STRUCT zclIASZone_InitTestModeRsp_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
}zclIASZone_InitTestModeRsp_t; 

typedef PACKED_STRUCT zclIASSetAttrValue_tag
{
  uint8_t      ep;     /* endpoint*/
  uint16_t     valueAttr;
  uint16_t     attrID;
} zclIASSetAttrValue_t;

#if ( TRUE == gBigEndian_c )
#define Mask_IASZone_Alarm1               0x0100
#define Mask_IASZone_Alarm2               0x0200
#define Mask_IASZone_SupervisionReports   0x1000
#define MASK_IASZone_Test                 0x0001      
#else
#define Mask_IASZone_Alarm1               0x0001
#define Mask_IASZone_Alarm2               0x0002
#define Mask_IASZone_SupervisionReports   0x0010
#define MASK_IASZone_Test                 0x0100  
#endif

extern uint16_t gIasZoneTypeTable[gIASApp_ZoneTypeId_d];


/******************************************************
	IAS ACE Cluster (ZigBee Cluster Library Chapter 8.3)
******************************************************/
enum
{
   gZclPanelStatus_Disarmed_c = 0x00,        /* Disarmed */
   gZclPanelStatus_ArmedStay_c,              /* Armed stay */
   gZclPanelStatus_ArmedNight_c,             /* Armed night */
   gZclPanelStatus_ArmedAway_c,              /* Armed away */   
   gZclPanelStatus_ExitDelay_c,              /* Exit Delay */     
   gZclPanelStatus_EntryDelay_c,             /* Entry Delay */  
   gZclPanelStatus_NotReadyToArm_c,          /* Not ready to Arm */     
   gZclPanelStatus_InAlarm_c,                /* In Alarm */  
   gZclPanelStatus_ArmingStay_c,             /* Arming Stay */     
   gZclPanelStatus_ArmingNight_c,            /* Arming Night */   
   gZclPanelStatus_ArmingAway_c,             /* Arming Away */     
                                             /* 0x0b-0xFF Reserved */
};

/* 8.3.2.3 Zone Table */
typedef PACKED_STRUCT gZclZoneTable_tag
{            
    uint8_t          ZoneID; 
    uint16_t         ZoneType;
    IEEEaddress_t    ZoneAddress;   
    uint16_t         ZoneStatus;
    uint8_t          Endpoint;   
    uint8_t          BypassStatus;   
}gZclZoneTable_t;

#define gIASApp_MaxSupportedZones_d 0x05
extern uint8_t gIndexInIASZoneTable;/* store the number of devices that are in the gtableZone */
extern gZclZoneTable_t gIASZoneTable[gIASApp_MaxSupportedZones_d];

/* 8.3.2.4 IAS ACE server cluster Commands Received */
#define gZclCmdRxIASACE_Arm_c                 0x00      /* M - Arm */
#define gZclCmdRxIASACE_Bypass_c              0x01      /* M - Bypass */
#define gZclCmdRxIASACE_Emergency_c           0x02      /* M - Emergency */
#define gZclCmdRxIASACE_Fire_c                0x03      /* M - Fire */
#define gZclCmdRxIASACE_Panic_c               0x04      /* M - Panic */
#define gZclCmdRxIASACE_GetZoneIDMap_c        0x05      /* M - Get Zone ID Map */
#define gZclCmdRxIASACE_GetZoneInformation_c  0x06      /* M - Get Zone Information */
#define gZclCmdRxIASACE_GetPanelStatus_c      0x07      /* M - Get Panel Status*/                                                     
#define gZclCmdRxIASACE_GetBypassedZoneList_c 0x08      /* M - Get Bypassed Zone List */
#define gZclCmdRxIASACE_GetZoneStatus_c       0x09      /* M - Get Zone STatus */
                                                        /*0x0A -0xFF -reserved */
#define gIasDefaultLengthArmDisarmCode_d  0x04   
#define gIasDefaultArmDisarmCode_d  0x31, 0x32, 0x33, 0x34

/* arm command payload */
typedef PACKED_STRUCT zclCmdIASACE_Arm_tag 
{
 uint8_t  ArmMode;
 uint8_t  ArmDisarmCodeLength;
 uint8_t  ArmDisarmCodeZoneId[1]; //first bytes are ArmDisarmCode, the last byte is the ZoneId
}zclCmdIASACE_Arm_t;

typedef PACKED_STRUCT zclIASACE_Arm_tag 
{
   afAddrInfo_t        addrInfo;
   uint8_t             zclTransactionId;
   zclCmdIASACE_Arm_t  cmdFrame;
}zclIASACE_Arm_t;

/* Arm Mode Field Enumeration*/
enum
{
   gArmMode_Disarm_c = 0x00,      /* Disarm */
   gArmMode_ArmDayHomeZone_c,     /* Arm Day/Home Zones Only */
   gArmMode_ArmNightSleepZone_c,  /* Arm Night/Sleep Zones Only */
   gArmMode_ArmAllZones_c,        /* Arm All Zones */
                                  /* 0x08-0xFF Reserved */
};

/* bypass command payload */
typedef PACKED_STRUCT zclCmdIASACE_Bypass_tag 
{
 uint8_t  bypassData[1]; //NumberOfZones +  zoneId's + codeLength + armDisarmCode
}zclCmdIASACE_Bypass_t;

typedef PACKED_STRUCT zclIASACE_Bypass_tag 
{
   afAddrInfo_t           addrInfo;
   uint8_t                zclTransactionId;
   zclCmdIASACE_Bypass_t  cmdFrame;
}zclIASACE_Bypass_t;

/* Emergency, Fire, Panic, Get Zone ID MAp Command */
typedef PACKED_STRUCT zclIASACE_EFP_tag 
{
   afAddrInfo_t           addrInfo;
   uint8_t                zclTransactionId;
}zclIASACE_EFP_t;

/* payload for Get Zone Information Command*/
typedef PACKED_STRUCT zclCmdIASACE_GetZoneInformation_tag 
{
 uint8_t  ZoneID; 
}zclCmdIASACE_GetZoneInformation_t;

typedef PACKED_STRUCT zclIASACE_GetZoneInformation_tag 
{
   afAddrInfo_t                       addrInfo;
   uint8_t                            zclTransactionId;
   zclCmdIASACE_GetZoneInformation_t  cmdFrame;
}zclIASACE_GetZoneInformation_t;


/* Get panel Status Command */
typedef PACKED_STRUCT zclIASACE_GetPanelStatus_tag 
{
   afAddrInfo_t           addrInfo;
   uint8_t                zclTransactionId;
}zclIASACE_GetPanelStatus_t;

/* Get Bypassed zone list Command */
typedef PACKED_STRUCT zclIASACE_GetBypassedZoneList_tag 
{
   afAddrInfo_t                         addrInfo;
   uint8_t                              zclTransactionId;
}zclIASACE_GetBypassedZoneList_t;

/* Payload format for Get Zone Status Command */
typedef PACKED_STRUCT zclCmdIASACE_GetZoneStatus_tag 
{
   uint8_t      startingZoneId;
   uint8_t      maxNoOfZoneIds;
   bool_t       zoneStatusMaskFlag;
   uint16_t     zoneStatusMask; 
}zclCmdIASACE_GetZoneStatus_t;

/* Get Zone Status Command */
typedef PACKED_STRUCT zclIASACE_GetZoneStatus_tag 
{
   afAddrInfo_t                         addrInfo;
   uint8_t                              zclTransactionId;
   zclCmdIASACE_GetZoneStatus_t         cmdFrame;        
}zclIASACE_GetZoneStatus_t;

/* 8.3.2.5 IAS ACE server cluster Commands Generated */
#define gZclCmdTxIASACE_ArmRsp_c              0x00    /* M - Arm Response */
#define gZclCmdTxIASACE_GetZoneIDMApRsp_c     0x01    /* M - Get Zone ID Map Response */
#define gZclCmdTxIASACE_GetZoneInfRsp_c       0x02    /* M - Get Zone Information Response */
#define gZclCmdTxIASACE_ZoneStatusChanged_c   0x03    /* M - Zone Status Changed */
#define gZclCmdTxIASACE_PanelStatusChanged_c  0x04    /* M - Panel Status Changed  */
#define gZclCmdTxIASACE_GetPanelStatusRsp_c   0x05    /* M - Get Panel Status Response */
#define gZclCmdTxIASACE_SetBypassedZoneList_c 0x06    /* M - Set Bypassed Zone List */
#define gZclCmdTxIASACE_BypassResponse_c      0x07    /* M - Bypass Response */
#define gZclCmdTxIASACE_GetZoneStatusRsp_c    0x08    /* M - Get Zone Status Response */   
                                                      /* 0x09 - 0xFF - reserved */

/* payload for Arm Response command */
typedef PACKED_STRUCT zclCmdIASACE_ArmRsp_tag 
{
 uint8_t    ArmNotification;
}zclCmdIASACE_ArmRsp_t; 

typedef PACKED_STRUCT zclIASACE_ArmRsp_tag 
{
   afAddrInfo_t           addrInfo;
   uint8_t                zclTransactionId;
   zclCmdIASACE_ArmRsp_t  cmdFrame;
}zclIASACE_ArmRsp_t; 

/* Arm Notification Enumeration*/
enum
{
   gArmNotif_AllZoneDisarm_c = 0x00,      /* All Zones Disarmed */
   gArmNotif_DayHomeZoneArmed_c,          /* Only Day/Home Zones Armed */
   gArmNotif_NightSleepZoneArmed_c,       /* Only Night/Sleep Zones Armed */
   gArmNotif_AllZonesArmed_c,             /* Arm All Zones */
                                          /* 0x04-0xFE Reserved */
};

/*payload for Get Zone ID Map Response Command*/
typedef PACKED_STRUCT zclCmdIASACE_GetZoneIDMApRsp_tag
{
 uint16_t   ZoneIDMapSection[16];
}zclCmdIASACE_GetZoneIDMApRsp_t;

typedef PACKED_STRUCT zclIASACE_GetZoneIDMApRsp_tag 
{
   afAddrInfo_t                    addrInfo;
   uint8_t                         zclTransactionId;
   zclCmdIASACE_GetZoneIDMApRsp_t  cmdFrame;
}zclIASACE_GetZoneIDMApRsp_t;

/* payload for Get Zone Information Response Command */
typedef PACKED_STRUCT zclCmdIASACE_GetZoneInfRsp_tag
{
 uint8_t          ZoneID;
 uint16_t         ZoneType; 
 IEEEaddress_t    IEEEAddress;
 uint8_t          zoneLabelLength;
 uint8_t          zoneLabel[1];
}zclCmdIASZone_GetZoneInfRsp_t;

typedef PACKED_STRUCT zclIASACE_GetZoneInfRsp_tag 
{
   afAddrInfo_t                    addrInfo;
   uint8_t                         zclTransactionId;
   zclCmdIASZone_GetZoneInfRsp_t   cmdFrame;
}zclIASACE_GetZoneInfRsp_t;


/* payload format for the zone status changed command*/
typedef PACKED_STRUCT zclCmdIASACE_ZoneStatusChanged_tag
{
  uint8_t       zoneId;
  uint16_t      zoneStatus;  
  uint8_t       audibleNotification;
  uint8_t       zoneLabelLength;
  uint8_t       zoneLabel[1];
}zclCmdIASACE_ZoneStatusChanged_t;

/* IAS ACE cluster extension: 10.8.4.4 */
typedef PACKED_STRUCT zclIASACE_ZoneStatusChanged_tag 
{
   afAddrInfo_t                         addrInfo;
   uint8_t                              zclTransactionId;
   zclCmdIASACE_ZoneStatusChanged_t     cmdFrame;
}zclIASACE_ZoneStatusChanged_t;

/* payload format for the panel status changed command*/
typedef PACKED_STRUCT zclCmdIASACE_PanelStatusChanged_tag
{
  uint8_t       panelStatus;
  uint8_t       secondsRemaining; /* shall be provided if the Panel Status = gPanelStatus_ExitDelay_c or gPanelStatus_EntryDelay_c*/
  uint8_t       audibleNotification;
  uint8_t       alarmStatus;
}zclCmdIASACE_PanelStatusChanged_t;

/* Panel Status Enumeration*/
enum
{
  gPanelStatus_Disarmed_c = 0x00, /* panel disarmed(all zone disarmed) and ready to arm */
  gPanelStatus_ArmedStay_c,       /* armed stay */
  gPanelStatus_ArmedBNight_c,     /* armed night */
  gPanelStatus_ArmedAway_c,       /* armed away */
  gPanelStatus_ExitDelay_c,       /* exit delay */
  gPanelStatus_EntryDelay_c,      /* entry delay */
  gPanelStatus_NotReadyToArm_c,   /* not ready to arm */      
                                  /* 0x07 - 0xFF reserved */
};

/* IAS ACE cluster extension: 10.8.4.5 */
typedef PACKED_STRUCT zclIASACE_PanelStatusChanged_tag
{
   afAddrInfo_t                            addrInfo;
   uint8_t                                 zclTransactionId;
   zclCmdIASACE_PanelStatusChanged_t       cmdFrame;
}zclIASACE_PanelStatusChanged_t;


/* payload format for the get panel status rsp command*/
typedef PACKED_STRUCT zclCmdIASACE_GetPanelStatusRsp_tag
{
  uint8_t       panelStatus;
  uint8_t       secondsRemaining; 
  uint8_t       audibleNotification;
  uint8_t       alarmStatus;
}zclCmdIASACE_GetPanelStatusRsp_t;

/* IAS ACE cluster extension: HA1.2 Errata */
typedef PACKED_STRUCT zclIASACE_GetPanelStatusRsp_tag
{
   afAddrInfo_t                            addrInfo;
   uint8_t                                 zclTransactionId;
   zclCmdIASACE_GetPanelStatusRsp_t       cmdFrame;
}zclIASACE_GetPanelStatusRsp_t;


/* values of bypass result: HA1.2 Errata */
enum
{
  gBypass_ZoneBypassed = 0x00,
  gBypass_ZoneNotBypassed,
  gBypass_NotAllowed,
  gBypass_InvalidZoneID,
  gBypass_UnknownZoneID,
  gBypass_InvalidArmDisarmCode
                    /* 0x06 - 0xFF reserved */
};
  
 
/* payload format for the get panel status rsp command*/
typedef PACKED_STRUCT zclCmdIASACE_BypassRsp_tag
{
  uint8_t       noOfZones;
  uint8_t       bypassResult[1]; 
}zclCmdIASACE_BypassRsp_t;

/* IAS ACE cluster extension: HA1.2 Errata */
typedef PACKED_STRUCT zclIASACE_BypassRsp_tag
{
   afAddrInfo_t                   addrInfo;
   uint8_t                        zclTransactionId;
   zclCmdIASACE_BypassRsp_t       cmdFrame;
}zclIASACE_BypassRsp_t;

/* payload format for the get zone status response*/
typedef PACKED_STRUCT zclGetZoneStatusRspRecord_tag
{
   uint8_t    zoneID;
   uint16_t   status;
}zclGetZoneStatusRspRecord_t;

/* payload format for the get zone status response*/
typedef PACKED_STRUCT zclCmdIASACE_GetZoneStatusRsp_tag
{
   bool_t   zoneStatusComplete;
   uint8_t  noOfZones;
  zclGetZoneStatusRspRecord_t zoneRecord[1];
}zclCmdIASACE_GetZoneStatusRsp_t;

/* IAS ACE cluster extension: HA1.2 Errata */
typedef PACKED_STRUCT zclIASACE_GetZoneStatusRsp_tag
{
   afAddrInfo_t                         addrInfo;
   uint8_t                              zclTransactionId;
   zclCmdIASACE_GetZoneStatusRsp_t      cmdFrame;
}zclIASACE_GetZoneStatusRsp_t;

/* payload format for the get panel status rsp command*/
typedef PACKED_STRUCT zclCmdIASACE_SetBypassedZoneList_tag
{
  uint8_t       noOfZones;
  uint8_t       zoneId[1]; 
}zclCmdIASACE_SetBypassedZoneList_t;

/* IAS ACE cluster extension: HA1.2 Errata */
typedef PACKED_STRUCT zclIASACE_SetBypassedZoneList_tag
{
   afAddrInfo_t                         addrInfo;
   uint8_t                              zclTransactionId;
   zclCmdIASACE_SetBypassedZoneList_t   cmdFrame;
}zclIASACE_SetBypassedZoneList_t;

/******************************************************
	IAS WD Cluster (ZigBee Cluster Library Chapter 8.4)
******************************************************/

/* 8.4.2.2 IAS WD Attributes */
#if ( TRUE == gBigEndian_c )
#define gZclAttrIASWDMaxDuration_c     0x0000    /* M - Max Duration */
#else
#define gZclAttrIASWDMaxDuration_c     0x0000    /* M - Max Duration */
#endif /* #if ( TRUE == gBigEndian_c ) */

#define gZclAttrIASWDSet_c			0x00
#define gZclAttrIASWDIdMaxDuration_c		0x00

typedef PACKED_STRUCT zclIASWDAttrsRAM_tag
{
  uint16_t    maxDuration; 
} zclIASWDAttrsRAM_t;

/* 8.4.2.3 IAS WD server cluster Commands Received */
enum
{
   gZclCmdRxIASWD_StartWarning_c = 0x00,    /* M - Start Warning */
   gZclCmdRxIASWD_Squawk_c,                 /* M - Squawk */
                                            /* 0x02-0xFF Reserved */
};

typedef PACKED_STRUCT zclIASWarningModeStrobeSirenLevel_tag
{
 uint8_t WarningMode  : 4;
 uint8_t Strobe       : 2;
 uint8_t SirenLevel   : 2;
}zclIASWarningModeStrobeSirenLevel_t;

/* Start Warning Payload */
typedef PACKED_STRUCT zclCmdIASWD_StartWarning_tag
{
 zclIASWarningModeStrobeSirenLevel_t   WarningModeStrobeSirenLevel;
 uint16_t                              WarningDuration;
 uint8_t                               StrobeDutyCycle;
 uint8_t                               StrobeLevel;
}zclCmdIASWD_StartWarning_t; 

typedef PACKED_STRUCT zclCmdIASWD_WarningInf_tag
{
  zclIASWarningModeStrobeSirenLevel_t   WarningModeStrobeSirenLevel;
  uint16_t                              WarningDuration;
  uint8_t                               StrobeDutyCycle;
}zclCmdIASWD_WarningInf_t;

typedef PACKED_STRUCT zclIASWD_StartWarning_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASWD_StartWarning_t cmdFrame;
}zclIASWD_StartWarning_t;

enum
{
  WarningMode_Stop = 0,         /* Stop (no warning) */
  WarningMode_Burglar,          /* Burglar */
  WarningMode_Fire,             /* Fire */
  WarningMode_Emergency,        /* Emergency */ 
  WarningMode_PolicePanic,      /* Police Panic */ 
  WarningMode_FirePanic,        /* Fire Panic */ 
  WarningMode_EmergencyPanic    /* Emergency Panic*/   
                                /* 0x07-0xFF Reserved */  
};

enum
{
  Strobe_NoStrobe = 0,      /* No Strobe */
  Strobe_StrobeParallel,    /* Use Strobe in Parallel to warning */
                            /* 2-3 Reserved */
};   

enum
{
  SirenLevel_Low = 0,       /* Low level sound */
  SirenLevel_Medium ,       /* Medium level sound */
  SirenLevel_High,          /* High level sound */
  SirenLevel_VeryHigh,      /* VeryHigh level sound */
};

enum
{
  StrobeLevel_Low = 0,       /* Low level strobe */
  StrobeLevel_Medium ,       /* Medium level strobe */
  StrobeLevel_High,          /* High level strobe */
  StrobeLevel_VeryHigh,      /* VeryHigh level strobe */
                             /* 0x04-0xFF Reserved */ 
};

/* Squawk Payload */
typedef PACKED_STRUCT zclCmdIASWD_Squawk_tag
{
 uint8_t SquawkMode : 4;
 uint8_t Strobe : 1;
 uint8_t Reserved : 1;
 uint8_t SquawkLevel : 2;
}zclCmdIASWD_Squawk_t;  

typedef PACKED_STRUCT zclIASWD_Squawk_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASWD_Squawk_t cmdFrame;
}zclIASWD_Squawk_t;

enum
{
  SquawkMode_SystemArmed = 0,     /* Notification sound for "System is Armed" */
  SquawkMode_SystemDisarmed,      /* Notification sound for "System is DisArmed" */
                                  /* 0x02-0xFF Reserved */  
};   

#define NO_STROBE           0
#define STROBE_PARALLEL     1    

enum
{
  SquawkLevel_LowLevel = 0,     /* Low Level Sound */
  SquawkLevel_MediumLevel,      /* Medium Level Sound */
  SquawkLevel_HighLevel,        /* High Level Sound */
  SquawkLevel_VeryHighLevel,    /* Very High Level Sound */
};              


/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/
/*!
 * @fn 		zbStatus_t ZCL_IASZoneClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS Zone Cluster Client. 
 *
 */
zbStatus_t ZCL_IASZoneClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ZCL_IASZoneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS Zone Cluster Server. 
 *
 */
zbStatus_t ZCL_IASZoneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ZCL_IASACEClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS ACE Cluster Client. 
 *
 */
zbStatus_t ZCL_IASACEClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ZCL_IASACEClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS ACE Cluster Server. 
 *
 */
zbStatus_t ZCL_IASACEClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ZCL_IASWDClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS WarningDevice Cluster Client. 
 *
 */
zbStatus_t ZCL_IASWDClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ZCL_IASWDClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS WarningDevice Cluster Server. 
 *
 */
zbStatus_t ZCL_IASWDClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t IASZone_ZoneStatusChange(zclIASZone_ZoneStatusChange_t *pReq) 
 *
 * @brief	Sends over-the-air a ZoneStatusChange command from the IAS Zone Cluster Server. 
 *
 */
zbStatus_t IASZone_ZoneStatusChange(zclIASZone_ZoneStatusChange_t *pReq);
/*!
 * @fn 		void ZCL_SendStatusChangeNotification(zbEndPoint_t SrcEndpoint, uint16_t zoneStatusChangeNotif) 
 *
 * @brief	Helper function to create and send ZoneStatusChange frame over the air
 *
 */
void ZCL_SendStatusChangeNotification(zbEndPoint_t SrcEndpoint, uint16_t zoneStatusChangeNotif);
/*!
 * @fn 		zbStatus_t IASZone_ZoneEnrollRequest(zclIASZone_ZoneEnrollRequest_t *pReq) 
 *
 * @brief	Sends over-the-air a ZoneEnrollRequest command from the IAS Zone Cluster Server. 
 *
 */
zbStatus_t IASZone_ZoneEnrollRequest(zclIASZone_ZoneEnrollRequest_t *pReq);
/*!
 * @fn 		void ZCL_SendZoneEnrollRequest(uint8_t endpoint) 
 *
 * @brief	Helper function to create and send enrollRequest frame over the air
 *
 */
void ZCL_SendZoneEnrollRequest(uint8_t endpoint);
/*!
 * @fn 		zbStatus_t IASZone_ZoneEnrollResponse(zclIASZone_ZoneEnrollResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ZoneEnrollResponse command from the IAS Zone Cluster Client. 
 *
 */
zbStatus_t IASZone_ZoneEnrollResponse(zclIASZone_ZoneEnrollResponse_t *pReq);
/*!
 * @fn 		zbStatus_t IASZone_InitNormalOpModeRequest(zclIASZone_InitNormalOpMode_t *pReq) 
 *
 * @brief	Sends over-the-air a InitNormalOperationMode command from the IAS Zone Cluster Client. 
 *
 */
zbStatus_t IASZone_InitNormalOpModeRequest(zclIASZone_InitNormalOpMode_t *pReq);
/*!
 * @fn 		zbStatus_t IASZone_InitNormalOpModeRequest(zclIASZone_InitTestMode_t *pReq) 
 *
 * @brief	Sends over-the-air a InitTestMode command from the IAS Zone Cluster Client. 
 *
 */
zbStatus_t IASZone_InitTestModeRequest(zclIASZone_InitTestMode_t *pReq); 
zbStatus_t IASZone_AddEntryInDeviceClientTable(gZclZoneTable_t *pIasZoneEntry);

#if gAddValidationFuncPtrToClusterDef_c
/*!
 * @fn 		bool_t  ZCL_LevelControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Validation function for Level control attributes
 *
 */
bool_t  ZCL_IASZoneValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData);
#endif
/*!
 * @fn 		zbStatus_t IASACE_Arm(zclIASACE_Arm_t *pReq) 
 *
 * @brief	Sends over-the-air an Arm command  from the IAS ACE Cluster Client. 
 *
 */  
zbStatus_t IASACE_Arm(zclIASACE_Arm_t *pReq);
/*!
 * @fn 		void ZCL_SendArmCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t armMode) 
 *
 * @brief	Helper function to create and send an Arm command over the air
 *
 */
void ZCL_SendArmCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t armMode);
/*!
 * @fn 		zbStatus_t IASACE_ArmRsp(zclIASACE_ArmRsp_t *pReq)
 *
 * @brief	Sends over-the-air an ArmResponse command  from the IAS ACE Cluster Server. 
 *
 */ 
zbStatus_t IASACE_ArmRsp(zclIASACE_ArmRsp_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_Bypass(zclIASACE_Bypass_t *pReq) 
 *
 * @brief	Sends over-the-air an Bypass command  from the IAS ACE Cluster Client. 
 *
 */
zbStatus_t IASACE_Bypass(zclIASACE_Bypass_t *pReq);
/*!
 * @fn 		void ZCL_SendBypassCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t NrOfZones, uint8_t *pZoneId) 
 *
 * @brief	Helper function to create and send an Bypass command over the air
 *
 */
void ZCL_SendBypassCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t NrOfZones, uint8_t *zoneID); 
/*!
 * @fn 		void ZCL_SendEFPCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t CommandEFP)
 *
 * @brief	Helper function to create and send an Emergency/Fire/Panic command over the air. 
 *
 */  
void ZCL_SendEFPCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t CommandEFP); 
/*!
 * @fn 		zbStatus_t IASACE_Emergency(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an Emergency command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_Emergency(zclIASACE_EFP_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_Fire(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an Fire command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_Fire(zclIASACE_EFP_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_Panic(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an Panic command  from the IAS ACE Cluster Client. 
 *
 */
zbStatus_t IASACE_Panic(zclIASACE_EFP_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_GetPanelStatus(zclIASACE_GetPanelStatus_t *pReq) 
 *
* @brief	Sends over-the-air an GetPanelStatus command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_GetPanelStatus( zclIASACE_GetPanelStatus_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_GetZoneIDMap(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneIdMap command  from the IAS ACE Cluster Client. 
 *
 */
   
zbStatus_t IASACE_GetZoneIDMap(zclIASACE_EFP_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_GetZoneIDMapRsp(zclIASACE_GetZoneIDMApRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneIdMapResponse command  from the IAS ACE Cluster Server. 
 *
 */

zbStatus_t IASACE_GetZoneIDMapRsp(zclIASACE_GetZoneIDMApRsp_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_GetZoneInformation(zclIASACE_GetZoneInformation_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneInformation command  from the IAS ACE Cluster Client. 
 *
 */
zbStatus_t IASACE_GetZoneInformation(zclIASACE_GetZoneInformation_t *pReq);
/*!
 * @fn 		void ZCL_SendGetZoneInformationCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t zoneId) 
 *
 * @brief	Helper function to create and send an GetZoneInformation command over the air. 
 *
 */ 
void ZCL_SendGetZoneInformationCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t zoneId); 
/*!
 * @fn 		zbStatus_t IASACE_BypassRsp(zclIASACE_BypassRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an BypassResponse command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_BypassRsp(zclIASACE_BypassRsp_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_GetPanelStatusRsp(zclIASACE_GetPanelStatusRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an GetPanelStatusResponse command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_GetPanelStatusRsp(zclIASACE_GetPanelStatusRsp_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_GetZoneInformationRsp(zclIASACE_GetZoneInfRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneInformationResponse command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_GetZoneInformationRsp(zclIASACE_GetZoneInfRsp_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_ZoneStatusChanged(zclIASACE_ZoneStatusChanged_t *pReq) 
 *
 * @brief	Sends over-the-air an ZoneStatusChanged command  from the IAS ACE Cluster Server. 
 *
 */ 
zbStatus_t IASACE_ZoneStatusChanged(zclIASACE_ZoneStatusChanged_t *pReq);
/*!
 * @fn 		zbStatus_t IASACE_PanelStatusChanged(zclIASACE_PanelStatusChanged_t *pReq) 
 *
 * @brief	Sends over-the-air an PanelStatusChanged command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_PanelStatusChanged(zclIASACE_PanelStatusChanged_t *pReq);
/*!
 * @fn 		void ZCL_SendPanelStatusChangedCommand(uint8_t endpoint, uint8_t panelStatus)
 *
 * @brief	sends a get panse status rsp over the air. 
 *
 */ 
void ZCL_SendPanelStatusChangedCommand(uint8_t endpoint, uint8_t panelStatus, uint8_t secondsRemaining, uint8_t audibleNotification, uint8_t alarmStatus);
/*!
 * @fn 		zbStatus_t IASACE_GetBypassedZoneList(zclIASACE_GetBypassedZoneList_t *pReq) 
 *
* @brief	Sends over-the-air an GetBypassedZoneList command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_GetBypassedZoneList(zclIASACE_GetBypassedZoneList_t *pReq);
/*!
 * @fn 		zbStatus_t IASWD_StartWarning(zclIASWD_StartWarning_t *pReq) 
 *
 * @brief	Sends over-the-air a StartWarning command from the IAS WD Cluster Client. 
 *
 */
zbStatus_t IASWD_StartWarning(zclIASWD_StartWarning_t *pReq);
/*!
 * @fn 		void ZCL_SendStartWarning(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_StartWarning_t startWarningPayload) 
 *
 * @brief	Helper function to create and send StartWarning command over the air
 *
 */
void ZCL_SendStartWarning(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_StartWarning_t startWarningPayload);
/*!
 * @fn 		zbStatus_t IASWD_Squawk(zclIASWD_Squawk_t *pReq) 
 *
 * @brief	Sends over-the-air a Squawk command from the IAS WD Cluster Client. 
 *
 */
zbStatus_t IASWD_Squawk(zclIASWD_Squawk_t *pReq);
/*!
 * @fn 		void ZCL_SendSquawk(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_Squawk_t squawk)  
 *
 * @brief	Helper function to create and send Squawk command over the air
 *
 */
void ZCL_SendSquawk(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_Squawk_t squawk); 
/*!
 * @fn 		void ZCL_SendSquawkCmdCallback(zbTmrTimerID_t tmrId) 
 *
 * @brief	Callback used to send the Squawk command to all Warning devices, according with the zoneTable
 *
 */
void ZCL_SendSquawkCmdCallback(zbTmrTimerID_t tmrId);
/*!
 * @fn 		void ZCL_SendWarningCmdCallback(zbTmrTimerID_t tmrId) 
 *
 * @brief	Callback used to send the StartWarning command to all Warning devices, according with the zoneTable
 *
 */
void ZCL_SendWarningCmdCallback(zbTmrTimerID_t tmrId);
#ifdef __cplusplus
}
#endif
#endif
