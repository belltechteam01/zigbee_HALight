/*! @file 	ZclGeneral.h
 *
 * @brief	Types, definitions and prototypes for the ZCL General functional domain.
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
 *  [R3] - docs-11-0037-10-0Zll-zigbee-light-link-zll-profile specification.pdf 
 */
#ifndef _ZCL_GENERAL_H
#define _ZCL_GENERAL_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "ZclOptions.h"
#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "ZCL.h"
#include "EndPointConfig.h"


/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/
/* Basic Cluster Attribute Defaults, not for setting by the OEM */
/* See ZclOptions.h for the list of OEM settable attributes */
#define gZclAttrBasic_ZclVersion_c        0x01
#define gZclAttrBasic_StackVersion_c      0x02
#define gZclLevel_OnOffCmd_c              0x01
#define gZclLevel_NotOnOffCmd_c           0x00
#define gZclLevel_UndefinedOnLevel_c      0xFF
#define gZclGroup_NameSupport_c           0x00  /* Group Names not supported */
#if gZclIncludeSceneName_d
#define gZclScene_NameSupport_c           0x01  /* Scene Names supported */
#else
#define gZclScene_NameSupport_c           0x00 /* Scene Names not supported */
#endif
#define DummyEndPoint                     0xFF
#define gpGroupTable ZbBeeStackApsGlobals(gaApsGroupTable)


/*The Level Min and Max values representes the range, in which the Level Control,
  this values are application dependant.
  For the Dimmable Light application:
    When the Level is set to 0, the light shall be turned fully off. When the level is set to 254, the light
  shall be turned on to the maximum level possible for the device.
  */
#define gZclLevelMinValue        0x00  /* 0 */
#define gZclLevelMaxValue        0xFF              
#define gZclLevelNumberOfLevels  0x03
#define gZclLevelDelta  (uint8_t)((gZclLevelMaxValue - gZclLevelMinValue + 1)/gZclLevelNumberOfLevels)

#define gZclCluster_InvalidDataIndex_d      0xFF     
    
/******************************************
	Basic Cluster
*******************************************/

#if ( TRUE == gBigEndian_c )

/* basic cluster attribute IDs (see ZclGeneral.c for values) */
#define gZclAttrBasic_ZCLVersionId_c               0x0000 /* M - ZCLVersion */
#define gZclAttrBasic_ApplicationVersionId_c       0x0100 /* O - ApplicationVersion */
#define gZclAttrBasic_StackVersionId_c             0x0200 /* O - StackVersion */
#define gZclAttrBasic_HWVersionId_c                0x0300 /* O - HWVersion */
#define gZclAttrBasic_ManufacturerNameId_c         0x0400 /* O - ManufacturerName */
#define gZclAttrBasic_ModelIdentifierId_c          0x0500 /* O - ModelIdentifier */
#define gZclAttrBasic_DateCodeId_c                 0x0600 /* O - DateCode */
#define gZclAttrBasic_PowerSourceId_c              0x0700 /* M - PowerSource */
#define gZclAttrBasic_LocationDescriptionId_c      0x1000 /* O - LocationDescription */
#define gZclAttrBasic_PhysicalEnvironmentId_c      0x1100 /* O - PhysicalEnvironment */
#define gZclAttrBasic_DeviceEnabledId_c            0x1200 /* M - DeviceEnabled Boolean */
#define gZclAttrBasic_AlarmMaskId_c                0x1300 /* O - AlarmMask */
#if gASL_EnableZLLClustersData_d
#define gZclAttrBasic_SoftwareBuild_c              0x0040 /* M - Software build */     
#endif
#else
/* basic cluster attribute IDs (see ZclGeneral.c for values) */
#define gZclAttrBasic_ZCLVersionId_c               0x0000 /* M - ZCLVersion */
#define gZclAttrBasic_ApplicationVersionId_c       0x0001 /* O - ApplicationVersion */
#define gZclAttrBasic_StackVersionId_c             0x0002 /* O - StackVersion */
#define gZclAttrBasic_HWVersionId_c                0x0003 /* O - HWVersion */
#define gZclAttrBasic_ManufacturerNameId_c         0x0004 /* O - ManufacturerName */
#define gZclAttrBasic_ModelIdentifierId_c          0x0005 /* O - ModelIdentifier */
#define gZclAttrBasic_DateCodeId_c                 0x0006 /* O - DateCode */
#define gZclAttrBasic_PowerSourceId_c              0x0007 /* M - PowerSource */
#define gZclAttrBasic_LocationDescriptionId_c      0x0010 /* O - LocationDescription */
#define gZclAttrBasic_PhysicalEnvironmentId_c      0x0011 /* O - PhysicalEnvironment */
#define gZclAttrBasic_DeviceEnabledId_c            0x0012 /* M - DeviceEnabled Boolean */
#define gZclAttrBasic_AlarmMaskId_c                0x0013 /* O - AlarmMask */
#if gASL_EnableZLLClustersData_d
#define gZclAttrBasic_SoftwareBuild_c              0x4000 /* M - Software build */     
#endif
#endif /* #if ( TRUE == gBigEndian_c ) */

/* Device Information Set */
#define gZclAttrSetBasicDeviceInformation_c         0x00 

#define gZclAttrIdBasicZCLVersionId_c               0x00 /* M - ZCLVersion */
#define gZclAttrIdBasicApplicationVersionId_c       0x01 /* O - ApplicationVersion */
#define gZclAttrIdBasicStackVersionId_c             0x02 /* O - StackVersion */
#define gZclAttrIdBasicHWVersionId_c                0x03 /* O - HWVersion */
#define gZclAttrIdBasicManufacturerNameId_c         0x04 /* O - ManufacturerName */
#define gZclAttrIdBasicModelIdentifierId_c          0x05 /* O - ModelIdentifier */
#define gZclAttrIdBasicDateCodeId_c                 0x06 /* O - DateCode */
#define gZclAttrIdBasicPowerSourceId_c              0x07 /* M - PowerSource */
#define gZclAttrIdBasicLocationDescriptionId_c      0x10 /* O - LocationDescription */
#define gZclAttrIdBasicPhysicalEnvironmentId_c      0x11 /* O - PhysicalEnvironment */
#define gZclAttrIdBasicDeviceEnabledId_c            0x12 /* M - DeviceEnabled Boolean */
#define gZclAttrIdBasicAlarmMaskId_c                0x13 /* O - AlarmMask */

#if gASL_EnableZLLClustersData_d
#define gZclAttrSetBasicZllData_c                   0x40 
#define gZclAttrIdBasic_SoftwareBuild_c             0x00 /* M - Software build */     
#endif


/* power source (8-bit value) */
#define gZclAttrBasicPowerSource_Unknown_c     0x00 /* Unknown */
#define gZclAttrBasicPowerSource_Mains_c       0x01 /* Mains (single phase) */
#define gZclAttrBasicPowerSource_Mains3p_c     0x02 /* Mains (3 phase) */
#define gZclAttrBasicPowerSource_Battery_c     0x03 /* Battery */
#define gZclAttrBasicPowerSource_DC_c          0x04 /* DC source */
#define gZclAttrBasicPowerSource_Emergency1_c  0x05 /* Emergency mains constantly powered */
#define gZclAttrBasicPowerSource_Emergency2_c  0x06 /* Emergency mains and transfer switch */

/* enabled attribute values */
#define gZclDeviceDisabled_c 0x00
#define gZclDeviceEnabled_c  0x01

/* 16-bytes in RAM for location description */
#define gZclAttrBasic_LocationDescriptionLen_c 16

#define gZclAttrBasicPhyEnvUnspecifiedEnvironment 0x00
#define gZclAttrBasicPhylEnvSpecifiedEnvironment  0x01

/* Physical Environment  */
#define gZclAttrBasic_PhysicalEnvironment  gZclAttrBasicPhylEnvSpecifiedEnvironment

/* basic cluster commands */
#define gZclCmdBasic_Reset_c                   0x00 /* O - Reset to Factory Defaults */

/* reset system to factory defaults */
typedef PACKED_STRUCT zclBasicResetReq_tag
{
  afAddrInfo_t      addrInfo;   /* IN: source/dst address, cluster, etc... */
  /* no payload, just the command */
} zclBasicResetReq_t;


/* list of attributes in RAM for this cluster */
typedef PACKED_STRUCT zclBasicAttrsRAM_tag
{
  uint8_t    zclVersion;                /* zclVersion */ 
  uint8_t    powerSourceId;             /* powerSourceId */
  bool_t     deviceEnabled ;            /* 0x0012 DeviceEnabled Boolean, 0x01, RdWr - M */
#if gZclClusterOptionals_d
  uint8_t    appVersion;                /* appVersion */
  uint8_t    stackVersion;              /* stackVersion */
  uint8_t    hardwareVersion;           /* hardwareVersion */
  zclStr10_t manufacturerName;          /* manufacturerName */
  zclStr05_t modelIdentifier;           /* modelIdentifier */
  zclStr09_t dateCode;                  /* dateCode */
  zclStr16_t locationDescription;       /* 0x0010 LocationDescription, str16, "" RdWr - O */
  uint8_t    physicalEnvironment;       /* 0x0011 PhysicalEnvironment, 0x00, RdWr - O */  
  uint8_t    alarmMask;                 /* 0x0013 AlarmMask, 0x00, RdWr - O */
#endif  
#if gASL_EnableZLLClustersData_d
  zclStr09_t swBuild;                  /* sw build */
#endif  
} zclBasicAttrsRAM_t;


/**********************************************************************
	Power Configuration Cluster (ZigBee Cluster Library Chapter 3.3)
**********************************************************************/

/* Power Configuration Attributes Sets */
#if ( TRUE == gBigEndian_c )
#define gZclAttrPwrConfigMainsInformationSet_c           0x0000    /* Mains Information attributes Set */
#define gZclAttrPwrConfigMainsSettingsSet_c              0x1000    /* Mains Settings attributes Set */
#define gZclAttrPwrConfigBatteryInformationSet_c         0x2000    /* Battery Information attributes Set */
#define gZclAttrPwrConfigBatterySettingsSet_c            0x3000    /* Battery Settings attributes Set */
#define gZclAttrPwrConfigBatterySource2InformationSet_c  0x4000    /* Battery Source 2 Information attributes Set */
#define gZclAttrPwrConfigBatterySource2SettingsSet_c     0x5000    /* Battery Source 2 Settings attributes Set */
#define gZclAttrPwrConfigBatterySource3InformationSet_c  0x6000    /* Battery Source 3 Information attributes Set */
#define gZclAttrPwrConfigBatterySource3SettingsSet_c     0x7000    /* Battery Source 3 Settings attributes Set */
#else
#define gZclAttrPwrConfigMainsInformationSet_c           0x0000    /* Mains Information attributes Set */
#define gZclAttrPwrConfigMainsSettingsSet_c              0x0010    /* Mains Settings attributes Set */
#define gZclAttrPwrConfigBatteryInformationSet_c         0x0020    /* Battery Information attributes Set */
#define gZclAttrPwrConfigBatterySettingsSet_c            0x0030    /* Battery Settings attributes Set */
#define gZclAttrPwrConfigBatterySource2InformationSet_c  0x0040    /* Battery Source 2 Information attributes Set */
#define gZclAttrPwrConfigBatterySource2SettingsSet_c     0x0050    /* Battery Source 2 Settings attributes Set */
#define gZclAttrPwrConfigBatterySource3InformationSet_c  0x0060    /* Battery Source 3 Information attributes Set */
#define gZclAttrPwrConfigBatterySource3SettingsSet_c     0x0070    /* Battery Source 3 Settings attributes Set */

#endif /* #if ( TRUE == gBigEndian_c ) */

/* 3.3.2.2.1 Mains Information Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrPwrConfigMainsInfMainsVoltage_c    0x0000    /* O - Mains Voltage */
#define gZclAttrPwrConfigMainsInfMainsFrequency_c  0x0100    /* O - Mains Frequency */
#else
#define gZclAttrPwrConfigMainsInfMainsVoltage_c    0x0000    /* O - Mains Voltage */
#define gZclAttrPwrConfigMainsInfMainsFrequency_c  0x0001    /* O - Mains Frequency */
#endif /* #if ( TRUE == gBigEndian_c ) */

                                 
/* 3.3.2.2.2 Mains Settings Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrPwrConfigMainsStgMainsAlarmMask_c             0x1000    /* O - Mains Information attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageMinThreshold_c   0x1100    /* O - Mains Settings attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageMaxThreshold_c   0x1200    /* O - Battery Information attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageDwellTripPoint_c 0x1300    /* O - Battery Settings attributes Set */
#else
#define gZclAttrPwrConfigMainsStgMainsAlarmMask_c             0x0010    /* O - Mains Information attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageMinThreshold_c   0x0011    /* O - Mains Settings attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageMaxThreshold_c   0x0012    /* O - Battery Information attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageDwellTripPoint_c 0x0013    /* O - Battery Settings attributes Set */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* 3.3.2.2.3 Battery Information Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrPwrConfigBatteryInfBatteryVoltage_c                     0x2000    /* O - Battery Voltage */
#define gZclAttrPwrConfigBatteryInfBatteryPercentageRemaining_c         0x2100    /* O - Battery Percentage Remaining */
#else
#define gZclAttrPwrConfigBatteryInfBatteryVoltage_c                     0x0020    /* O - Battery Voltage */
#define gZclAttrPwrConfigBatteryInfBatteryPercentageRemaining_c         0x0021    /* O - Battery Percentage Remaining */
#endif /* #if ( TRUE == gBigEndian_c ) */


/* 3.3.2.2.4 Battery Settings Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrPwrConfigBatteryStgBatteryManufacturer_c             0x3000    /* O - Battery Manufacturer */
#define gZclAttrPwrConfigBatteryStgBatterySize_c                     0x3100    /* O - Battery Size */
#define gZclAttrPwrConfigBatteryStgBatteryAHrRating_c                0x3200    /* O - Battery A/Hr Rating */
#define gZclAttrPwrConfigBatteryStgBatteryQuantity_c                 0x3300    /* O - Battery Quantity */
#define gZclAttrPwrConfigBatteryStgBatteryRatedVoltage_c             0x3400    /* O - Battery Rated Voltage */
#define gZclAttrPwrConfigBatteryStgBatteryAlarmMask_c                0x3500    /* O - Battery Alarm Mask */
#define gZclAttrPwrConfigBatteryStgBatteryVoltageMinThreshold_c      0x3600    /* O - Battery Voltage Min Threshold */
#define gZclAttrPwrConfigBatteryStgBatteryVoltageThreshold1_c        0x3700    /* O - Battery Voltage Threshold 1 */
#define gZclAttrPwrConfigBatteryStgBatteryVoltageThreshold2_c        0x3800    /* O - Battery Voltage Threshold 2 */
#define gZclAttrPwrConfigBatteryStgBatteryVoltageThreshold3_c        0x3900    /* O - Battery Voltage Threshold 3 */
#define gZclAttrPwrConfigBatteryStgBatteryPercentageMinThreshold_c   0x3A00    /* O - Battery Percentage Min Threshold */
#define gZclAttrPwrConfigBatteryStgBatteryPercentageThreshold1_c     0x3B00    /* O - Battery Percentage Threshold 1 */
#define gZclAttrPwrConfigBatteryStgBatteryPercentageThreshold2_c     0x3C00    /* O - Battery Percentage Threshold 2 */
#define gZclAttrPwrConfigBatteryStgBatteryPercentagehreshold3_c      0x3D00    /* O - Battery Percentage Threshold 3 */
#define gZclAttrPwrConfigBatteryStgBatteryAlarmState_c               0x3E00    /* O - Battery Alarm State */    
#else
#define gZclAttrPwrConfigBatteryStgBatteryManufacturer_c             0x0030    /* O - Battery Manufacturer */
#define gZclAttrPwrConfigBatteryStgBatterySize_c                     0x0031    /* O - Battery Size */
#define gZclAttrPwrConfigBatteryStgBatteryAHrRating_c                0x0032    /* O - Battery A/Hr Rating */
#define gZclAttrPwrConfigBatteryStgBatteryQuantity_c                 0x0033    /* O - Battery Quantity */
#define gZclAttrPwrConfigBatteryStgBatteryRatedVoltage_c             0x0034    /* O - Battery Rated Voltage */
#define gZclAttrPwrConfigBatteryStgBatteryAlarmMask_c                0x0035    /* O - Battery Alarm Mask */
#define gZclAttrPwrConfigBatteryStgBatteryVoltageMinThreshold_c      0x0036    /* O - Battery Voltage Min Threshold */
#define gZclAttrPwrConfigBatteryStgBatteryVoltageThreshold1_c        0x0037    /* O - Battery Voltage Threshold 1 */
#define gZclAttrPwrConfigBatteryStgBatteryVoltageThreshold2_c        0x0038    /* O - Battery Voltage Threshold 2 */
#define gZclAttrPwrConfigBatteryStgBatteryVoltageThreshold3_c        0x0039    /* O - Battery Voltage Threshold 3 */
#define gZclAttrPwrConfigBatteryStgBatteryPercentageMinThreshold_c   0x003A    /* O - Battery Percentage Min Threshold */
#define gZclAttrPwrConfigBatteryStgBatteryPercentageThreshold1_c     0x003B    /* O - Battery Percentage Threshold 1 */
#define gZclAttrPwrConfigBatteryStgBatteryPercentageThreshold2_c     0x003C    /* O - Battery Percentage Threshold 2 */
#define gZclAttrPwrConfigBatteryStgBatteryPercentagehreshold3_c      0x003D    /* O - Battery Percentage Threshold 3 */
#define gZclAttrPwrConfigBatteryStgBatteryAlarmState_c               0x003E    /* O - Battery Alarm State */ 
#endif /* #if ( TRUE == gBigEndian_c ) */


#define gZclAttrSetPwrConfig_c                                      0x00    
#define gZclAttrPwrConfigIdMainsInfMainsVoltage_c                   0x00    /* O - Mains Voltage */
#define gZclAttrPwrConfigIdMainsInfMainsFrequency_c                 0x01    /* O - Mains Frequency */
#define gZclAttrPwrConfigIdMainsStgMainsAlarmMask_c                 0x10    /* O - Mains Information attributes Set */
#define gZclAttrPwrConfigIdMainsStgMainsVoltageMinThreshold_c       0x11    /* O - Mains Settings attributes Set */
#define gZclAttrPwrConfigIdMainsStgMainsVoltageMaxThreshold_c       0x12    /* O - Battery Information attributes Set */
#define gZclAttrPwrConfigIdMainsStgMainsVoltageDwellTripPoint_c     0x13    /* O - Battery Settings attributes Set */
#define gZclAttrPwrConfigIdBatteryInfBatteryVoltage_c               0x20    /* O - Battery Voltage */
#define gZclAttrPwrConfigIdBatteryInfBatteryPercentageRemaining_c   0x21    /* O - Battery Percentage Remaining */
#define gZclAttrPwrConfigIdBatteryStgBatteryManufacturer_c          0x30    /* O - Battery Manufacturer */
#define gZclAttrPwrConfigIdBatteryStgBatterySize_c                  0x31    /* O - Battery Size */
#define gZclAttrPwrConfigIdBatteryStgBatteryAHrRating_c             0x32    /* O - Battery A/Hr Rating */
#define gZclAttrPwrConfigIdBatteryStgBatteryQuantity_c              0x33    /* O - Battery Quantity */
#define gZclAttrPwrConfigIdBatteryStgBatteryRatedVoltage_c          0x34    /* O - Battery Rated Voltage */
#define gZclAttrPwrConfigIdBatteryStgBatteryAlarmMask_c             0x35    /* O - Battery Alarm Mask */
#define gZclAttrPwrConfigIdBatteryStgBatteryVoltageMinThreshold_c   0x36    /* O - Battery Voltage Min Threshold */
#define gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold1_c     0x37    /* O - Battery Voltage Threshold 1 */
#define gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold2_c     0x38    /* O - Battery Voltage Threshold 2 */
#define gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold3_c     0x39    /* O - Battery Voltage Threshold 3 */
#define gZclAttrPwrConfigIdBatteryStgBatteryPercentageMinThreshold_c 0x3A    /* O - Battery Percentage Min Threshold */
#define gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold1_c   0x3B    /* O - Battery Percentage Threshold 1 */
#define gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold2_c   0x3C    /* O - Battery Percentage Threshold 2 */
#define gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold3_c   0x3D    /* O - Battery Percentage Threshold 3 */
#define gZclAttrPwrConfigIdBatteryStgBatteryAlarmState_c             0x3E    /* O - Battery Alarm State */

/* Mains Alarm Mask */
typedef PACKED_STRUCT gZclAlarmMask_tag
{
    uint8_t mainsVoltage2Low     :1;    /* Mains voltage too Low */                             
    uint8_t mainsVoltage2High    :1;    /* Mains voltage too High */ 
    uint8_t mainsPowerSupplyLost :1;    /* Mains Power Supply lost/ unavailable (e.g device is running on battery) */    
    uint8_t reserved             :5;    /* Reserved */                                    
}gZclAlarmMask_t; 

/* Battery size */
enum
{
   gBatterySize_NoBattery = 0x00,       /* No Battery */
   gBatterySize_BuiltIn,                /* Built In */
   gBatterySize_Other,                  /* Other */
   gBatterySize_AA,                     /* AA */
   gBatterySize_AAA,                    /* AAA */
   gBatterySize_C,                      /* C */
   gBatterySize_D,                      /* D */
                                        /* 0x07-0xFE Reserved */
   gBatterySize_Unknown = 0xFF,         /* Unknown */                                    
};

/* Battery Alarm Mask */
typedef PACKED_STRUCT gZclBatteryAlarmMask_tag
{
    uint8_t gBatteryAlarmMask_BatteryVoltage2Low   :1;      /* Battery voltage too low (i.e.: BatterryVoltageMinThreshold value has been reached) */                             
    uint8_t gBatteryAlarmMask_BatteryAlarm1        :1;      /* Battery Alarm 1(i.e.: Battery Voltage Threshold 1 or Battery Percentage Threshold 1 value has been reached) */                              
    uint8_t gBatteryAlarmMask_BatteryAlarm2        :1;      /* Battery Alarm 2(i.e.: Battery Voltage Threshold 2 or Battery Percentage Threshold 2 value has been reached) */                                 
    uint8_t gBatteryAlarmMask_BatteryAlarm3        :1;      /* Battery Alarm 3(i.e.: Battery Voltage Threshold 3 or Battery Percentage Threshold 3 value has been reached) */                                 
    uint8_t Reserved                               :4;      /* Reserved */
}gZclBatteryAlarmMask_t;

/* Alarm Code Field Enumeration for Battery Alarms */
enum{
    gBatteryAlarms_AlarmCode0_Source1 = 0x10,       /* BatteryVoltageMinThreshold or BatteryPercentageMinThreshold reached for Battery Source 1*/
    gBatteryAlarms_AlarmCode1_Source1 = 0x11,       /* BatteryVoltageThreshold1 or BatteryPercentageThreshold1 reached for Battery Source 1*/
    gBatteryAlarms_AlarmCode2_Source1 = 0x12,       /* BatteryVoltageThreshold2 or BatteryPercentageThreshold2 reached for Battery Source 1*/
    gBatteryAlarms_AlarmCode3_Source1 = 0x13,       /* BatteryVoltageThreshold3 or BatteryPercentageThreshold3 reached for Battery Source 1*/
                                                    /* 0x14-0x19 Reserved */
    gBatteryAlarms_AlarmCode0_Source2 = 0x20,       /* BatteryVoltageMinThreshold or BatteryPercentageMinThreshold reached for Battery Source 2*/
    gBatteryAlarms_AlarmCode1_Source2 = 0x21,       /* BatteryVoltageThreshold1 or BatteryPercentageThreshold1 reached for Battery Source 2*/
    gBatteryAlarms_AlarmCode2_Source2 = 0x22,       /* BatteryVoltageThreshold2 or BatteryPercentageThreshold2 reached for Battery Source 2*/
    gBatteryAlarms_AlarmCode3_Source2 = 0x23,       /* BatteryVoltageThreshold3 or BatteryPercentageThreshold3 reached for Battery Source 2*/
                                                    /* 0x24-0x29 Reserved */
    gBatteryAlarms_AlarmCode0_Source3 = 0x30,       /* BatteryVoltageMinThreshold or BatteryPercentageMinThreshold reached for Battery Source 3*/
    gBatteryAlarms_AlarmCode1_Source3 = 0x31,       /* BatteryVoltageThreshold1 or BatteryPercentageThreshold1 reached for Battery Source 3*/
    gBatteryAlarms_AlarmCode2_Source3 = 0x32,       /* BatteryVoltageThreshold2 or BatteryPercentageThreshold2 reached for Battery Source 3*/
    gBatteryAlarms_AlarmCode3_Source3 = 0x33,       /* BatteryVoltageThreshold3 or BatteryPercentageThreshold3 reached for Battery Source 3*/
                                                    /* 0x34-0x3F Reserved */
    gBatteryAlarms_MainsPowerSuplyLost = 0x3A,      /* Mains Power suply lost/unavailable (i.e. device is running on battery )*/    
                                                    /* 0x3B-0xFE Reserved */
    gBatteryAlarms_AlarmNotBeGenerated = 0xFF,      /* Alarm shall not be generated */    
};


/* Battery Alarm State Attribute */
typedef PACKED_STRUCT gZclPwrCfgBatteryAlarmState_tag
{
    uint32_t batteryVoltageMinThresholdSource1     :1;    /* BatteryVoltageMinThreshold or BatteryPercentageMinThreshold reached for Battery Source 1 */                             
    uint32_t batteryVoltageMinThreshold1Source1    :1;    /* BatteryVoltageThreshold1 or BatteryPercentageThreshold1 reached for Battery Source 1 */     
    uint32_t batteryVoltageMinThreshold2Source1    :1;    /* BatteryVoltageThreshold2 or BatteryPercentageThreshold2 reached for Battery Source 1  */     
    uint32_t batteryVoltageMinThreshold3Source1    :1;    /* BatteryVoltageThreshold3 or BatteryPercentageThreshold3 reached for Battery Source 1  */   
    uint32_t reserved1                             :6;    /* Reserved 1*/        
    uint32_t batteryVoltageMinThresholdSource2     :1;    /* BatteryVoltageMinThreshold or BatteryPercentageMinThreshold reached for Battery Source 2 */                             
    uint32_t batteryVoltageMinThreshold1Source2    :1;    /* BatteryVoltageThreshold1 or BatteryPercentageThreshold1 reached for Battery Source 2 */     
    uint32_t batteryVoltageMinThreshold2Source2    :1;    /* BatteryVoltageThreshold2 or BatteryPercentageThreshold2 reached for Battery Source 2  */     
    uint32_t batteryVoltageMinThreshold3Source2    :1;    /* BatteryVoltageThreshold3 or BatteryPercentageThreshold3 reached for Battery Source 2  */   
    uint32_t reserved2                             :6;    /* Reserved 2*/   
    uint32_t mainsPowerSupplyLost                  :1;    /* Mains power supply lost/unavailable (i.e., device is running on battery)*/      
    uint32_t reserved3                             :1;    /* Reserved 3*/     
}gZclPwrCfgBatteryAlarmState_t; 

/* The comand use for this cluster are the read/write attributes */
typedef PACKED_STRUCT zclPowerCfgAttrsRAM_tag
{
  uint16_t    mainsVoltage; 
  uint8_t     mainsFrequency;
  uint8_t     mainsAlarmMask;
  uint16_t    mainsVoltageMinThreshold;
  uint16_t    mainsVoltageMaxThreshold;
  uint16_t    mainsVoltageDwellTripPoint;
  uint8_t     batteryVoltage[zclReportableCopies_c];
  uint8_t     batteryPercentageRemaining;
  zclStr16_t  batteryMfg;
  uint8_t     batterySize;
  uint16_t    batteryAHrRating;
  uint8_t     batteryQuantity;
  uint8_t     batteryRatedVoltage;
  uint8_t     batteryAlarmMask;
  uint8_t     batteryVoltageMinThreshold;
  uint8_t     batteryVoltageThreshold1;
  uint8_t     batteryVoltageThreshold2;
  uint8_t     batteryVoltageThreshold3;
  uint8_t     batteryPercentageMinThreshold;
  uint8_t     batteryPercentageThreshold1;
  uint8_t     batteryPercentageThreshold2;
  uint8_t     batteryPercentageThreshold3;
  uint32_t    batteryAlarmState;  
} zclPowerCfgAttrsRAM_t;


/*******************************************************************************
    Device Temperature Configuration Cluster (ZigBee Cluster Library Chapter 3.4)
*******************************************************************************/

typedef uint8_t  SumElem_t[3];

/* Device Temperature Configuration Attributes Sets */
#if ( TRUE == gBigEndian_c )
#define gZclAttrTempCfgTempInformationSet_c    0x0000    /* Device Temperature Information Set */
#define gZclAttrTempCfgTempSettingsSet_c       0x0100    /* Device Temperature Settings Set */
                                                         /*0x002-0xFFF - reserved*/
#else
#define gZclAttrTempCfgTempInformationSet_c    0x0000    /* Device Temperature Information Set */
#define gZclAttrTempCfgTempSettingsSet_c       0x0001    /* Device Temperature Settings Set */
                                                         /*0x002-0xFFF - reserved*/
#endif /* #if ( TRUE == gBigEndian_c ) */

#if ( TRUE == gBigEndian_c )
/* 3.4.2.2.1 Device temperature information attributes */
#define gZclAttrTempCfgTempInfCurrentTemp_c             0x0000    /* M - Current temperature */
#define gZclAttrTempCfgTempInfMinTempExperienced_c      0x0100    /* O - MinTempExperienced */
#define gZclAttrTempCfgTempInfMaxTempExperienced_c      0x0200    /* O - MaxTempExperienced */
#define gZclAttrTempCfgTempInfOverTempTotalDwell_c      0x0300    /* O - OverTempTotalDwell */
/* 3.4.2.2.2 Device temperature settings attributes */
#define gZclAttrTempCfgTempStgDeviceTempAlarmMask_c     0x1000    /* O - DeviceTempAlarmMask */
#define gZclAttrTempCfgTempStgLowTempThreshold_c        0x1100    /* O - LowTempThreshold */
#define gZclAttrTempCfgTempStgHighTempThreshold_c       0x1200    /* O - HighTempThreshold*/
#define gZclAttrTempCfgTempStgLowTempDwellTripPoint_c   0x1300    /* O - LowTempDwellTripPoint*/
#define gZclAttrTempCfgTempStgHighTempDwellTripPoint_c  0x1400    /* O - HighTempDwellTripPoint*/
#else
/* 3.4.2.2.1 Device temperature information attributes */
#define gZclAttrTempCfgTempInfCurrentTemp_c             0x0000    /* M - Current temperature */
#define gZclAttrTempCfgTempInfMinTempExperienced_c      0x0001    /* O - MinTempExperienced */
#define gZclAttrTempCfgTempInfMaxTempExperienced_c      0x0002    /* O - MaxTempExperienced */
#define gZclAttrTempCfgTempInfOverTempTotalDwell_c      0x0003    /* O - OverTempTotalDwell */
/* 3.4.2.2.2 Device temperature settings attributes */
#define gZclAttrTempCfgTempStgDeviceTempAlarmMask_c     0x0010    /* O - DeviceTempAlarmMask */
#define gZclAttrTempCfgTempStgLowTempThreshold_c        0x0011    /* O - LowTempThreshold */
#define gZclAttrTempCfgTempStgHighTempThreshold_c       0x0012    /* O - HighTempThreshold*/
#define gZclAttrTempCfgTempStgLowTempDwellTripPoint_c   0x0013    /* O - LowTempDwellTripPoint*/
#define gZclAttrTempCfgTempStgHighTempDwellTripPoint_c  0x0014    /* O - HighTempDwellTripPoint*/
#endif 


#define gZclAttrTempCfgSet_c             0x00   
/* 3.4.2.2.1 Device temperature information attributes */
#define gZclAttrIdTempCfgTempInfCurrentTemp_c             0x00    /* M - Current temperature */
#define gZclAttrIdTempCfgTempInfMinTempExperienced_c      0x01    /* O - MinTempExperienced */
#define gZclAttrIdTempCfgTempInfMaxTempExperienced_c      0x02    /* O - MaxTempExperienced */
#define gZclAttrIdTempCfgTempInfOverTempTotalDwell_c      0x03    /* O - OverTempTotalDwell */
/* 3.4.2.2.2 Device temperature settings attributes */
#define gZclAttrIdTempCfgTempStgDeviceTempAlarmMask_c     0x10    /* O - DeviceTempAlarmMask */
#define gZclAttrIdTempCfgTempStgLowTempThreshold_c        0x11    /* O - LowTempThreshold */
#define gZclAttrIdTempCfgTempStgHighTempThreshold_c       0x12    /* O - HighTempThreshold*/
#define gZclAttrIdTempCfgTempStgLowTempDwellTripPoint_c   0x13    /* O - LowTempDwellTripPoint*/
#define gZclAttrIdTempCfgTempStgHighTempDwellTripPoint_c  0x14    /* O - HighTempDwellTripPoint*/



/* DeviceTemp Alarm Mask */

typedef PACKED_STRUCT gZclTempAlarmMask_tag
{
    uint8_t MainsVoltage2Low   :1;    /* Device temp too Low */                             
    uint8_t MainsVoltage2High  :1;    /* Device temp too High */                              
    uint8_t Reserved           :6;    /* Reserved */                                    
}gZclTempAlarmMask_t; 

/* The comand use for this cluster are the read/write attributes */

typedef PACKED_STRUCT zclTempCfgAttrsRAM_tag
{
  int16_t     currentTemp; 
  int16_t     minTempExperienced;
  int16_t     maxTempExperienced;
  uint16_t    overTempTotalDwell;
  uint8_t     deviceTempAlarmMask;
  int16_t     lowTempThreshold;
  int16_t     highTempThreshold;
  SumElem_t   lowTempDwellTripPoint;
  SumElem_t   highTempDwellTripPoint;
} zclTempCfgAttrsRAM_t;


/******************************************
	Identify Cluster
*******************************************/
/* identify attribute IDs (see HaProfile.c for values */
#if ( TRUE == gBigEndian_c )
#define gZclAttrIdentify_Time_c                         0x0000    /* time left (secs) in identify mode */
#define gZclAttrIdentify_CommissioningState_c           0x0100     /* the commissioning state of the device(docs-12-0177-26-00ha-ez-mode) */
#else
#define gZclAttrIdentify_Time_c                         0x0000    /* time left (secs) in identify mode */
#define gZclAttrIdentify_CommissioningState_c           0x0001     /* the commissioning state of the device(docs-12-0177-26-00ha-ez-mode) */
#endif

#define gZclAttrIdentify_TimeId_c               0x00  /* time left (secs) in identify mode */
#define gZclAttrIdentify_CommissioningStateId_c 0x01    /* the commissioning state of the device(docs-12-0177-26-00ha-ez-mode) */



/* identify time attribute default value*/
#define gZclIdentifyAttr_IdentifyOff_d               0x0000    

/* Bit fields of the Identify cluster CommissioningState attribute */
#define gZclCommissioningState_NotInNetwork_d       0x00   
#define gZclCommissioningState_NetworkState_d       (1<<0)
#define gZclCommissioningState_OperationalState_d   (1<<1)

/* identify cluster commands */
#define gZclCmdIdentify_c                   0x00  /* identify command */
#define gZclCmdIdentifyQuery_c              0x01  /* devices will respond if in identify mode */
#define gZclCmdEzModeInvoke_c               0x02  /* this command invokes EZ-Mode over the air */
#define gZclCmdUpdateCommissioningState_c   0x03  /* this command updates the CommissioningState attribute over the air */
#if gASL_EnableZLLClustersData_d
#define gZclCmdIdentifyTriggerEffect_c      0x40   /* trigger effect */               
#endif

#define gZclCmdIdentifyQueryRsp_c  0x00  /* response to query command */

#define gZclCmdUpdateCommissioningState_Set_c	0x01
#define gZclCmdUpdateCommissioningState_Clear_c	0x02

/* list of attributes in RAM for this cluster */
typedef PACKED_STRUCT zclIdentifyAttrsRAM_tag
{
  uint16_t identifyTime;
#if gASL_EnableEZCommissioning_d   
  uint8_t  identifyCommissioningState;    
#endif  
}zclIdentifyAttrsRAM_t;

/* note: identify query command has no payload */
typedef PACKED_STRUCT zclCmdIdentifyCmd_tag
{
  uint16_t  iTimeIdentify;
} zclCmdIdentify_t;

typedef PACKED_STRUCT zclIdentifyReq_tag
{
  afAddrInfo_t         addrInfo;  /* IN: source/dst address, cluster, etc... */
  zclCmdIdentify_t     cmdFrame;  /* IN: command frame */
} zclIdentifyReq_t;


typedef PACKED_STRUCT zclIdentifyQueryReq_tag
{
  afAddrInfo_t         addrInfo;  /* IN: source/dst address, cluster, etc... */
  uint8_t              zclTransactionId;
} zclIdentifyQueryReq_t;

typedef PACKED_STRUCT zclCmdEzModeInvokeReq_tag
{
  uint8_t   action;
}zclCmdEzModeInvokeReq_t;

typedef PACKED_STRUCT zclEzModeInvokeReq_tag
{
  afAddrInfo_t                  addrInfo;  /* IN: source/dst address, cluster, etc... */
  zclCmdEzModeInvokeReq_t       cmdFrame;
}zclEzModeInvokeReq_t;

typedef PACKED_STRUCT zclCmdUpdateCommissioningStateReq_tag
{
  uint8_t   action;
  uint8_t	commissioningStateMask;
}zclCmdUpdateCommissioningStateReq_t;

typedef PACKED_STRUCT zclUpdateCommissioningStateReq_tag
{
  afAddrInfo_t                          addrInfo;  /* IN: source/dst address, cluster, etc... */
  zclCmdUpdateCommissioningStateReq_t   cmdFrame;
}zclUpdateCommissioningStateReq_t;


#if gASL_EnableZLLClustersData_d
enum{
  gIdentifyEffectId_Blink_c = 0x00, /* E.g. Light is turned on/off once */
  gIdentifyEffectId_Breathe_c = 0x01, /* E.g. Light turned on/off over 1 second, and repeat 15 times */
  gIdentifyEffectId_Okay_c = 0x02, /* E.g. Colored light turns green for 1 second; non-colored light flashes twice */
  gIdentifyEffectId_ChannelChange_c = 0x0b, /* E.g., Colored light turns orange for 8 seconds; non-colored light switches 
                                            to maximum brightness for 0.5s and then minimum brightness for 7.5s */
  gIdentifyEffectId_FinishEffect_c = 0xfe, /* Complete the current effect sequence before terminating */
  gIdentifyEffectId_StopEffect_c = 0xff /* Terminate the effect as soon as possible */
};

enum{
  gIdentifyVariantEffect_Default_c = 0x00 /* Default */
  /* 0x01 - 0xff reserved */
};

/* [R3] - payload format for trigger effect command */
typedef PACKED_STRUCT zclCmdIdentifyTriggerEffect_tag
{
  uint8_t effectId;
  uint8_t effectVariant;
}zclCmdIdentifyTriggerEffect_t;

/* [R3] - 6.3.1.2.1 Trigger effect command */
typedef PACKED_STRUCT zclIdentifyTriggerEffect_tag
{
  afAddrInfo_t                          addrInfo;  /* IN: source/dst address, cluster, etc... */
  uint8_t                               zclTransactionId;
  zclCmdIdentifyTriggerEffect_t         cmdFrame;
}zclIdentifyTriggerEffect_t;
#endif


/* payload format for the Identify Query response command - this payload was updated for the EZ Commissioning Procedure (12-0177 document)*/
typedef PACKED_STRUCT zclCmdIdentifyQueryRsp_tag
{
  uint16_t  iTimeLeft;                  /* in seconds */
} zclCmdIdentifyQueryRsp_t;

typedef PACKED_STRUCT zclIdentifyQueryRsp_tag
{
  afAddrInfo_t                  addrInfo;  /* IN: source/dst address, cluster, etc... */
  uint8_t                       zclTransactionId;
  zclCmdIdentifyQueryRsp_t      cmdFrame;  /* IN: command frame */
} zclIdentifyQueryRsp_t;

/******************************************
	Binding Session  Cluster               

- document: docs-12-0192-01-00ha-bindingsession-cluster.pdf - 
- used for EZ Commissioning Procedure
*******************************************/

#define gZclAttrBindingSession_Timeout_c                          0x0000    /* O - Timeout (in seconds) */
#define gZclAttrBindingSession_LeaderAddr_c                       0x0001    /* O - Leader Addr */
#define gZclAttrBindingSession_LeaderEP_c                         0x0003    /* O - Leader EP */ 

/* Binding Session cluster commands received*/
#define gZclBindingSession_OpenBindingSession_c          0x00  /* M - OpenBindingSession */
#define gZclBindingSession_CloseBindingSession_c         0x01  /* M - CloseBindingSession */
#define gZclBindingSession_LeaderNotification_c          0x02  /* M - Leader Notification */
#define gZclBindingSession_CancelBindingSession_c        0x03  /* O - Cancel Binding Session */        

/* payload format for the open binding session command*/
typedef PACKED_STRUCT zclCmdBindingSession_Open_tag
{
  uint16_t  sessionTime;
} zclCmdBindingSession_Open_t;

/* OpenBindingSession command*/
typedef PACKED_STRUCT zclBindingSession_Open_tag
{
  afAddrInfo_t                    addrInfo;  /* IN: source/dst address, cluster, etc... */
  zclCmdBindingSession_Open_t     cmdFrame;  /* IN: command frame */
} zclBindingSession_Open_t;

/* CloseBindingSession command - has no payload*/
typedef PACKED_STRUCT zclBindingSession_Close_tag
{
  afAddrInfo_t                    addrInfo;  /* IN: source/dst address, cluster, etc... */
} zclBindingSession_Close_t;


/* payload format for the leader notification command */
typedef PACKED_STRUCT zclCmdBindingSession_LeaderNotif_tag
{
  uint16_t  sessionTime;                 
} zclCmdBindingSession_LeaderNotif_t;

typedef PACKED_STRUCT zclBindingSession_LeaderNotif_tag
{
  afAddrInfo_t                              addrInfo;  /* IN: source/dst address, cluster, etc... */
  zclCmdBindingSession_LeaderNotif_t      cmdFrame;  /* IN: command frame */
} zclBindingSession_LeaderNotif_t;

/* CancelBindingSession command - has no payload*/
typedef PACKED_STRUCT zclBindingSession_Cancel_tag
{
  afAddrInfo_t                    addrInfo;  /* IN: source/dst address, cluster, etc... */
} zclBindingSession_Cancel_t;

/******************************************
	Group Cluster
*******************************************/

/* Group Attributes */
#define gZclAttrGroup_NameSupport_c             0x0000 /* rdOnly M - Group Name Supported */

/* list of attributes in RAM for this cluster */
typedef PACKED_STRUCT zclGroupAttrsRAM_tag
{
  uint8_t groupNameSupport; 
}zclGroupAttrsRAM_t;


/* 3.6.2 Group Commands */
#define gZclCmdGroup_AddGroup_c                 0x00 /* M - Add group */
#define gZclCmdGroup_ViewGroup_c                0x01 /* M - View group */
#define gZclCmdGroup_GetGroupMembership_c       0x02 /* M - Get group membership */
#define gZclCmdGroup_RemoveGroup_c              0x03 /* M - Remove group */
#define gZclCmdGroup_RemoveAllGroups_c          0x04 /* M - Remove all groups */
#define gZclCmdGroup_AddGroupIfIdentifying_c    0x05 /* M - Add group if identifying */

/* 3.6.2.2.3 Add Group command */
typedef PACKED_STRUCT zclCmdGroup_AddGroup_tag
{
  zbGroupId_t aGroupId;   /* group ID*/
  char szGroupName[1];    /* up to 16-bytes, string with group name */
} zclCmdGroup_AddGroup_t;

/* 3.6.2.2.4 View Group command */
typedef PACKED_STRUCT zclCmdGroup_ViewGroup_tag
{
  zbGroupId_t aGroupId;  /* group ID*/
} zclCmdGroup_ViewGroup_t;

/* 3.6.2.2.5 Get group membership command */
typedef PACKED_STRUCT zclCmdGroup_GetGroupMembership_tag
{
  uint8_t     count;        /* count of following list */
  zbGroupId_t aGroupId[1];  /* group ID*/
} zclCmdGroup_GetGroupMembership_t;

/* 3.6.2.2.6 Remove Group command */
typedef PACKED_STRUCT zclCmdGroup_RemoveGroup_tag
{
  zbGroupId_t aGroupId;  /* group ID*/
} zclCmdGroup_RemoveGroup_t;

/* 3.6.2.2.7 Remove All Groups command */
typedef PACKED_STRUCT zclCmdGroup_RemoveAllGroups_tag
{
  zclCmd_t     command;  /* remove all groups command */
} zclCmdGroup_RemoveAllGroups_t;

/* 3.6.2.2.8 Add Group If Identifying command */
typedef PACKED_STRUCT zclCmdGroup_AddGroupIfIdentifying_tag
{
  zbGroupId_t aGroupId;  /* group ID*/
  char szGroupName[1];   /* string with group name */
} zclCmdGroup_AddGroupIfIdentifying_t;

/* add endpoint to a group */
typedef PACKED_STRUCT zclGroupAddGroupReq_tag
{
  afAddrInfo_t            addrInfo;  /* IN: source/dst address, cluster, etc... */
  zclCmdGroup_AddGroup_t  cmdFrame;  /* IN: command frame */
} zclGroupAddGroupReq_t;

/* View Group Request */
typedef PACKED_STRUCT zclGroupViewGroupReq_tag
{
  afAddrInfo_t             addrInfo;   /* IN: source/dst address, cluster, etc... */
  zclCmdGroup_ViewGroup_t  cmdFrame;   /* IN: command frame */
} zclGroupViewGroupReq_t;

/* Get Group Membership */
typedef PACKED_STRUCT zclGroupGetGroupMembershipReq_tag
{
  afAddrInfo_t                      addrInfo;   /* IN: source/dst address, cluster, etc... */
  zclCmdGroup_GetGroupMembership_t  cmdFrame;   /* IN: command frame */
} zclGroupGetGroupMembershipReq_t;

/* Remove Group Request */
typedef PACKED_STRUCT zclGroupRemoveGroupReq_tag
{
  afAddrInfo_t              addrInfo;   /* IN: source/dst address, cluster, etc... */
  zclCmdGroup_RemoveGroup_t cmdFrame;   /* IN: command frame */
} zclGroupRemoveGroupReq_t;

/* Remove All Groups Request */
typedef PACKED_STRUCT zclGroupRemoveAllGroupsReq_tag
{
  afAddrInfo_t          addrInfo;   /* IN: source/dst address, cluster, etc... */
  /* remove all groups has no payload */
} zclGroupRemoveAllGroupsReq_t;

/* Add Group If Identifying */
typedef PACKED_STRUCT zclGroupAddGroupIfIdentifyingReq_tag
{
  afAddrInfo_t          addrInfo;                 /* IN: */
  zclCmdGroup_AddGroupIfIdentifying_t  cmdFrame;  /* IN: command frame */
} zclGroupAddGroupIfIdentifyingReq_t;

/* responses to above commands */
#define gZclCmdGroup_AddGroupRsp_c                       0x00 /* M - AddGroup */
#define gZclCmdGroup_ViewGroupRsp_c                      0x01 /* M - ViewGroup */
#define gZclCmdGroup_GetGroupMembershipRsp_c             0x02 /* M - GetGroupMembership */
#define gZclCmdGroup_RemoveGroupRsp_c                    0x03 /* M - RemoveGroup */

/* 3.6.2.3.1 Add Group Response command */
typedef PACKED_STRUCT zclCmdGroup_AddGroupRsp_tag
{
  zclStatus_t    status;
  zbGroupId_t   aGroupId;
} zclCmdGroup_AddGroupRsp_t;

/* 3.6.2.3.2 View Group Response command */
typedef PACKED_STRUCT zclCmdGroup_ViewGroupRsp_tag
{
  zclStatus_t    status;
  zbGroupId_t   aGroupId;
  char          szName[1];
} zclCmdGroup_ViewGroupRsp_t; 

/* 0x00 - 0xfd is the # of groups available */
typedef uint8_t haCapacity_t;
#define gZclCapacityNotFull_c   0xfe /* at least 1 entry available */
#define gZclCapacityFull_c         0xff

/* 3.6.2.3.3 Get Group Membership Response command */
typedef PACKED_STRUCT zclCmdGroup_GetGroupMembershipRsp_tag
{
  haCapacity_t  capacity;     /* # of groups total */
  uint8_t       count;        /*  */
  zbGroupId_t   aGroupId[1];
} zclCmdGroup_GetGroupMembershipRsp_t;

/* 3.6.2.3.4 Remove Group Response command */
typedef PACKED_STRUCT zclCmdGroup_RemoveGroupRsp_tag
{
  zclStatus_t    status;
  zbGroupId_t   aGroupId;
} zclCmdGroup_RemoveGroupRsp_t;



/******************************************
	Scenes Cluster
*******************************************/
#if ( TRUE == gBigEndian_c )
/* scene attributes */
#define gZclAttrScene_SceneCount_c         0x0000 /* rdOnly M - SceneCount */
#define gZclAttrScene_CurrentScene_c       0x0100 /* rdOnly M - CurrentScene */
#define gZclAttrScene_CurrentGroup_c       0x0200 /* rdOnly M - CurrentGroup */
#define gZclAttrScene_SceneValid_c         0x0300 /* rdOnly M - SceneValid */
#define gZclAttrScene_NameSupport_c        0x0400 /* rdOnly M - Scene Name Supported */
#define gZclAttrScene_LastConfiguredBy_c   0x0500 /* rdOnly O - LastConfiguredBy IEEE Address */
#else
#define gZclAttrScene_SceneCount_c         0x0000 /* rdOnly M - SceneCount */
#define gZclAttrScene_CurrentScene_c       0x0001 /* rdOnly M - CurrentScene */
#define gZclAttrScene_CurrentGroup_c       0x0002 /* rdOnly M - CurrentGroup */
#define gZclAttrScene_SceneValid_c         0x0003 /* rdOnly M - SceneValid */
#define gZclAttrScene_NameSupport_c        0x0004 /* rdOnly M - Scene Name Supported */
#define gZclAttrScene_LastConfiguredBy_c   0x0005 /* rdOnly O - LastConfiguredBy IEEE Address */
#endif /* #if ( TRUE == gBigEndian_c ) */

#define gZclAttrSceneIdSet_c         	     0x00 
#define gZclAttrSceneId_SceneCount_c         0x00 /* rdOnly M - SceneCount */
#define gZclAttrSceneId_CurrentScene_c       0x01 /* rdOnly M - CurrentScene */
#define gZclAttrSceneId_CurrentGroup_c       0x02 /* rdOnly M - CurrentGroup */
#define gZclAttrSceneId_SceneValid_c         0x03 /* rdOnly M - SceneValid */
#define gZclAttrSceneId_NameSupport_c        0x04 /* rdOnly M - Scene Name Supported */
#define gZclAttrSceneId_LastConfiguredBy_c   0x05 /* rdOnly O - LastConfiguredBy IEEE Address */


/* scene commands */
#define gZclCmdScene_AddScene_c            0x00 /* M - AddScene */
#define gZclCmdScene_ViewScene_c           0x01 /* M - ViewScene */
#define gZclCmdScene_RemoveScene_c         0x02 /* M - RemoveScene */
#define gZclCmdScene_RemoveAllScenes_c     0x03 /* M - RemoveAllScenes */
#define gZclCmdScene_StoreScene_c          0x04 /* M - StoreScene */
#define gZclCmdScene_RecallScene_c         0x05 /* M - RecallScene */
#define gZclCmdScene_GetSceneMembership_c  0x06 /* M - GetSceneMembership */

#if gASL_EnableZLLClustersData_d  
#define gZclCmdScene_EnhancedAddScene_c    0x40 /* M - EnhancedAddScene */
#define gZclCmdScene_EnhancedViewScene_c   0x41 /* M - EnhancedViewScene */
#define gZclCmdScene_CopyScene_c           0x42 /* M - EnhancedViewScene */
#endif 

/* scene data for each cluster */
typedef PACKED_STRUCT zclSceneOtaData_tag
{
  zclClusterId_t    clusterId;
  uint8_t           length;
  uint8_t           aData[1];
} zclSceneOtaData_t;

/* 3.7.2.4.1 Add Scene command */
typedef PACKED_STRUCT zclCmdScene_AddScene_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
  zclSceneId_t      sceneId;          /* scene ID */
  uint16_t          transitionTime;   /* in seconds */
  char              szSceneName[17];  /* length-encoded string */
  zclSceneOtaData_t aData[1];         /* variable scene length data */
} zclCmdScene_AddScene_t;

/* 3.7.2.4.2 View Scene command */
typedef PACKED_STRUCT zclCmdScene_ViewScene_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
  zclSceneId_t      sceneId;          /* scene ID */
} zclCmdScene_ViewScene_t;


/* 3.7.2.4.3 Remove Scene command */
typedef PACKED_STRUCT zclCmdScene_RemoveScene_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
  zclSceneId_t      sceneId;          /* scene ID */
} zclCmdScene_RemoveScene_t;

/* 3.7.2.4.4 Remove All Scenes command */
/* removes all scenes on this group */
typedef PACKED_STRUCT zclCmdScene_RemoveAllScenes_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
} zclCmdScene_RemoveAllScenes_t;

/* 3.7.2.4.5 Store Scene command */
/* stores current state into a scene variable */
typedef PACKED_STRUCT zclCmdScene_StoreScene_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
  zclSceneId_t      sceneId;          /* scene ID */
} zclCmdScene_StoreScene_t;

/* 3.7.2.4.6 Recall Scene command */
/* recall a stored or added scene */
typedef PACKED_STRUCT zclCmdScene_RecallScene_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
  zclSceneId_t      sceneId;          /* scene ID */
} zclCmdScene_RecallScene_t;


/* 3.7.2.4.7 Get Scene Membership command */
typedef PACKED_STRUCT zclCmdScene_GetSceneMembership_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
} zclCmdScene_GetSceneMembership_t;

/* add scene request */
typedef PACKED_STRUCT zclSceneAddSceneReq_tag
{
  afAddrInfo_t            addrInfo;   /* IN: */
  zclCmdScene_AddScene_t  cmdFrame;   /* IN: command frame */
} zclSceneAddSceneReq_t;

/* view scene request */
typedef PACKED_STRUCT zclSceneViewSceneReq_tag
{
  afAddrInfo_t            addrInfo;   /* IN: */
  zclCmdScene_ViewScene_t  cmdFrame;   /* IN: command frame */
} zclSceneViewSceneReq_t;

/* remove scene request */
typedef PACKED_STRUCT zclSceneRemoveSceneReq_tag
{
  afAddrInfo_t            addrInfo;     /* IN: */
  zclCmdScene_RemoveScene_t  cmdFrame;   /* IN: command frame */
} zclSceneRemoveSceneReq_t;

/* remove all scenes request */
typedef PACKED_STRUCT zclSceneRemoveAllScenesReq_tag
{
  afAddrInfo_t                  addrInfo;   /* IN: */
  zclCmdScene_RemoveAllScenes_t  cmdFrame;   /* IN: command frame */
} zclSceneRemoveAllScenesReq_t;

/* store scene request */
typedef PACKED_STRUCT zclSceneStoreSceneReq_tag
{
  afAddrInfo_t             addrInfo;   /* IN: */
  zclCmdScene_StoreScene_t  cmdFrame;   /* IN: command frame */
} zclSceneStoreSceneReq_t;

/* recall scene request */
typedef PACKED_STRUCT zclSceneRecallSceneReq_tag
{
  afAddrInfo_t              addrInfo;   /* IN: */
  zclCmdScene_RecallScene_t  cmdFrame;   /* IN: command frame */
} zclSceneRecallSceneReq_t;

/* get scene membership request */
typedef PACKED_STRUCT zclSceneGetSceneMembershipReq_tag
{
  afAddrInfo_t                    addrInfo;   /* IN: */
  zclCmdScene_GetSceneMembership_t cmdFrame;   /* IN: command frame */
} zclSceneGetSceneMembershipReq_t;


#if gASL_EnableZLLClustersData_d  
/* [R3] 6.5.1.3.4.1 payload format for copy scene  Command */
typedef PACKED_STRUCT zclCmdScene_CopyScene_tag
{
  uint8_t  mode; /* first byte - Copy all scene, bits 1-7 reserved */
  zbGroupId_t groupIdFrom;
  uint8_t  sceneIdFrom;
  zbGroupId_t groupIdTo;
  uint8_t  sceneIdTo;
} zclCmdScene_CopyScene_t;

/* [R3] 6.5.1.3.4 Copy scene  Command */
typedef PACKED_STRUCT zclSceneCopyScene_t
{
  afAddrInfo_t addrInfo;   /* IN: */
  zclCmdScene_CopyScene_t cmdFrame;   /* IN: command frame */
}zclSceneCopyScene_t;
#endif

/* combination of all scene commands (so a single pointer can be used) */
typedef PACKED_UNION zclSceneReq_tag {
  zclCmdScene_AddScene_t            addScene;
  zclCmdScene_ViewScene_t           viewScene;
  zclCmdScene_RemoveScene_t         removeScene;
  zclCmdScene_RemoveAllScenes_t     removeAllScenes;
  zclCmdScene_StoreScene_t          storeScene;
  zclCmdScene_RecallScene_t         recallScene;
  zclCmdScene_GetSceneMembership_t  getSceneMembership;
#if gASL_EnableZLLClustersData_d 
  zclCmdScene_CopyScene_t           copyScene;    
#endif  
} zclSceneReq_t;

/* responses to above commands */
#define gZclCmdScene_AddSceneRsp_c           0x00 /* M - AddSceneRsp */
#define gZclCmdScene_ViewSceneRsp_c          0x01 /* ViewSceneRsp */
#define gZclCmdScene_RemoveSceneRsp_c        0x02 /* RemoveSceneRsp */
#define gZclCmdScene_RemoveAllScenesRsp_c    0x03 /* RemoveAllScenesRsp */
#define gZclCmdScene_StoreSceneRsp_c         0x04 /* StoreSceneRsp */
/* no response for recall scene */
#define gZclCmdScene_GetSceneMembershipRsp_c 0x06 /* GetSceneMembershipRsp */

#if gASL_EnableZLLClustersData_d  
#define gZclCmdScene_EnhancedAddSceneRsp_c    0x40 /* M - EnhancedAddScene */
#define gZclCmdScene_EnhancedViewSceneRsp_c   0x41 /* M - EnhancedViewScene */
#define gZclCmdScene_CopySceneRsp_c           0x42 /* M - EnhancedViewScene */
#endif 

/* 3.7.2.5.1 Add Scene Response command */
typedef PACKED_STRUCT zclCmdScene_AddSceneRsp_tag
{
  zclStatus_t   status;
  zbGroupId_t   aGroupId;
  zclSceneId_t  sceneId;
} zclCmdScene_AddSceneRsp_t;

/* 3.7.2.5.2 View Scene Response command */
typedef PACKED_STRUCT zclCmdScene_ViewSceneRsp_tag
{
  zbGroupId_t       aGroupId;           /* group ID */
  zclSceneId_t      sceneId;           /* scene ID */
  char              szSceneName[17];    /* scene name */
  uint16_t          transitionTime;     /* in seconds */
  zclSceneOtaData_t aData[1];          /* variable scene length data */
} zclCmdScene_ViewSceneRsp_t;

/* 3.7.2.5.3 Remove Scene Response command */
typedef PACKED_STRUCT zclCmdScene_RemoveSceneRsp_tag
{
  zclStatus_t   status;
  zbGroupId_t   aGroupId;
  zclSceneId_t  sceneId;
} zclCmdScene_RemoveSceneRsp_t;

/* 3.7.2.5.4 Remove All Scenes Response command */
typedef PACKED_STRUCT zclCmdScene_RemoveAllScenesRsp_tag
{
  zclStatus_t   status;
  zbGroupId_t   aGroupId;
} zclCmdScene_RemoveAllScenesRsp_t;

/* 3.7.2.5.5 Store Scene Response command */
typedef PACKED_STRUCT zclCmdScene_StoreSceneRsp_tag
{
  zclStatus_t   status;
  zbGroupId_t   aGroupId;
  zclSceneId_t  sceneId;
} zclCmdScene_StoreSceneRsp_t;

/* 3.7.2.5.6 Get Scene Membership Response command */
typedef PACKED_STRUCT zclCmdScene_GetSceneMembershipRsp_tag
{
  zclStatus_t   status;          /* status */
  haCapacity_t  capacity;         /* capacity for scenes */
  zbGroupId_t   aGroupId;         /* group */
  uint8_t       sceneCount;       /* scenes that belong to the group */
  zclSceneId_t  scenes[1];       /* list of scenes */
} zclCmdScene_GetSceneMembershipRsp_t;

#if gASL_EnableZLLClustersData_d  
/* [R3] 6.5.1.3.4.1 payload format for copy scene  Command */
typedef PACKED_STRUCT zclCmdScene_CopySceneRsp_tag
{
  uint8_t  status;      /* succes, invalid field, insufficient_space */
  zbGroupId_t groupIdFrom;
  uint8_t  sceneIdFrom;
} zclCmdScene_CopySceneRsp_t;

/* [R3] 6.5.1.3.4  copy scene  Command */
typedef PACKED_STRUCT  zclScene_CopySceneRsp_tag
{  
  afAddrInfo_t                   addrInfo;
  uint8_t                        zclTransactionId;
  zclCmdScene_CopySceneRsp_t     cmdFrame;
} zclScene_CopySceneRsp_t;

#endif

typedef PACKED_UNION zclSceneRsp_tag {
  zclCmdScene_AddSceneRsp_t           addSceneRsp;
  zclCmdScene_ViewSceneRsp_t          viewSceneRsp;
  zclCmdScene_RemoveSceneRsp_t        removeSceneRsp;
  zclCmdScene_RemoveAllScenesRsp_t    removeAllScenesRsp;
  zclCmdScene_StoreSceneRsp_t         storeSceneRsp;
  zclCmdScene_GetSceneMembershipRsp_t getSceneMembershipRsp;
#if gASL_EnableZLLClustersData_d  
  zclCmdScene_CopySceneRsp_t          copySceneRsp;  
#endif  
  
} zclSceneRsp_t;



/* Scene data structures */

/* Application specific scene structure always "inherits" the zclSceneTable structure */

/* Thermostat scene data */
typedef PACKED_STRUCT zclThermostatSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* Thermostat specific scene data */
  int16_t occupiedCoolingSetpoint;
  int16_t occupiedHeatingSetpoint;
  uint8_t systemMode;
} zclThermostatSceneTableEntry_t;

typedef PACKED_STRUCT zclThermostatSceneData_tag
{
  zclSceneAttrs_t scene;  
  zclThermostatSceneTableEntry_t thermostatSceneTable[gHaMaxScenes_c];
} zclThermostatSceneData_t;


/* OnOff Light scene data */
typedef PACKED_STRUCT zclOnOffLightSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* On/Off Light specific scene data */
  uint8_t onOff;
} zclOnOffLightSceneTableEntry_t;

typedef PACKED_STRUCT zclOnOffLightSceneData_tag
{
  zclSceneAttrs_t scene;
  zclOnOffLightSceneTableEntry_t onOffLightSceneTable[gHaMaxScenes_c];
} zclOnOffLightSceneData_t;

/* DoorLock scene data */
typedef PACKED_STRUCT zclDoorLockSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /*Door Lock specific scene data */
  uint8_t lockState;
} zclDoorLockSceneTableEntry_t;

typedef PACKED_STRUCT zclDoorLockSceneData_tag
{
  zclSceneAttrs_t scene;
  zclDoorLockSceneTableEntry_t doorLockSceneTable[gHaMaxScenes_c];
} zclDoorLockSceneData_t;

/* Dimmable Light (no OnOff feature) scene data */
typedef PACKED_STRUCT zclDimmableLightSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* Dimmable Light specific scene data */
  uint8_t currentLevel;  
} zclDimmableLightSceneTableEntry_t;

typedef PACKED_STRUCT zclDimmableLightSceneData_tag
{
  zclSceneAttrs_t scene;
  zclDimmableLightSceneTableEntry_t dimmableLightSceneTable[gHaMaxScenes_c];
} zclDimmableLightSceneData_t;


/* Dimmable Light with OnOff scene data */
typedef PACKED_STRUCT zclDimmableLightOnOffSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* Dimmable Light specific scene data */
  uint8_t onOff;
  uint8_t currentLevel;  
} zclDimmableLightOnOffSceneTableEntry_t;

typedef PACKED_STRUCT zclDimmableLightOnOffSceneData_tag
{
  zclSceneAttrs_t scene;
  zclDimmableLightOnOffSceneTableEntry_t onOffSceneTable[gHaMaxScenes_c];
} zclDimmableLightOnOffSceneData_t;

/* Color Light scene data */
typedef PACKED_STRUCT zclColorLightSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* Color Light specific scene data */
  uint8_t onOff;
  uint8_t currentLevel;  
  uint16_t currentX;
  uint16_t currentY;
#if gASL_EnableZLLClustersData_d  
  uint16_t enhancedCurrentHue;
  uint8_t  currentSaturation;
  uint8_t  colorLoopActive;
  uint8_t  colorLoopDirection;
  uint16_t colorLoopTime;
#endif  
} zclColorLightSceneTableEntry_t;

typedef PACKED_STRUCT zclColorLightSceneData_tag
{
  zclSceneAttrs_t scene;
  zclColorLightSceneTableEntry_t onOffSceneTable[gHaMaxScenes_c];
} zclColorLightSceneData_t;

/* Shade with OnOff scene data */
typedef PACKED_STRUCT zclShadeOnOffSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* Shade specific scene data */
  uint8_t onOff;
  uint8_t currentLevel;  
} zclShadeOnOffSceneTableEntry_t;

typedef PACKED_STRUCT zclShadeOnOffSceneData_tag
{
  zclSceneAttrs_t scene;
  zclDimmableLightOnOffSceneTableEntry_t onOffSceneTable[gHaMaxScenes_c];
} zclShadeOnOffSceneData_t;

/******************************************
	On/Off Cluster
*******************************************/
/* on/off cluster attributes */
#define gZclAttrOnOff_OnOffId_c                 0x0000  /* M - onoff status */

#if gASL_EnableZLLClustersData_d
#if (TRUE == gBigEndian_c)
#define gZclAttrOnOff_GlobalSceneCtrl_c         0x0040  /* M - Global scene control */
#define gZclAttrOnOff_OnTime_c                  0x0140  /* M - OnTime */
#define gZclAttrOnOff_OffWaitTime_c             0x0240  /* M - OffWaitTime */
#else
#define gZclAttrOnOff_GlobalSceneCtrl_c         0x4000  /* M - Global scene control */
#define gZclAttrOnOff_OnTime_c                  0x4001  /* M - OnTime */
#define gZclAttrOnOff_OffWaitTime_c             0x4002  /* M - OffWaitTime */
#endif
#endif

#define gZclAttrOnOffSet_c                        0x00  /* OnOff attributes Set */
#define gZclAttrIdOnOff_OnOffId_c                 0x00  /* M - onoff status */

#if gASL_EnableZLLClustersData_d
#define gZclAttrOnOffZllSet_c                     0x40  /* OnOff ZLL attributes Set */
#define gZclAttrIdOnOff_GlobalSceneCtrl_c         0x00  /* M - Global scene control */
#define gZclAttrIdOnOff_OnTime_c                  0x01  /* M - OnTime */
#define gZclAttrIdOnOff_OffWaitTime_c             0x02  /* M - OffWaitTime */
#endif

typedef PACKED_STRUCT zclOnOffAttrsRAM_tag
{
  uint8_t  onOff[zclReportableCopies_c];
#if gASL_EnableZLLClustersData_d
  bool_t globalSceneCtrl;
  uint16_t onTime;
  uint16_t offWaitTime;
#endif  
} zclOnOffAttrsRAM_t;


#define gZclCmdOnOff_Off_c         0x00    /* M - turn device off */
#define gZclCmdOnOff_On_c          0x01    /* M - turn device on */
#define gZclCmdOnOff_Toggle_c      0x02    /* M - toggle device */
#if gASL_EnableZLLClustersData_d
#define gZclCmdOnOff_OffWithEffect_c                    0x40    /* M - Off With Effect Command */
#define gZclCmdOnOff_OnWithRecallGlobalScene_c          0x41    /* M - On With Recall Global Scene Command */
#define gZclCmdOnOff_OnWithTimedOff_c                   0x42    /* M - On With Timed Off Command */
#endif

#define gZclOnffAttr_Off_d      0x00    
#define gZclOnffAttr_On_d       0x01

typedef zclCmd_t zclOnOffCmd_t;

/* note: on/off/toggle commands have no payload */
typedef PACKED_STRUCT zclOnOffReq_tag
{
  afAddrInfo_t    addrInfo;     /* IN: source/dst address, cluster, etc... */
  zclOnOffCmd_t   command;      /* IN: on, off, toggle */
} zclOnOffReq_t;

/* on/off cluster commands */
typedef PACKED_STRUCT zclCmdOnOffSetState_tag
{
  zclCmd_t command;                /* on/off commands */
} zclCmdOnOffSetState_t;

/* sends on/off/toggle command */
typedef PACKED_STRUCT zclOnOffSetState_tag
{
  afAddrInfo_t            addrInfo;   /* IN: */
  zclCmdOnOffSetState_t    cmdFrame;  /* IN: handles all 3 commands, on/off/toggle */
} zclOnOffSetState_t;

#if gASL_EnableZLLClustersData_d
#define gOnOff_DefaultFadeToOff_d       800    /* miliseconds */
#define gOnOff_DimDown50FadeToOff_d     800    /* miliseconds */
#define gOnOff_SecondFadeToOff_d        12000  /* miliseconds */
#define gOnOff_DimUp20FadeToOff_d       500    /* miliseconds */
#define gOnOff_DyingLightFadeToOff_d    1000   /* miliseconds */

/* [R3] 6.6.1.4.4 payload format for Off with effect Command */
typedef PACKED_STRUCT zclCmdOnOff_OffWithEffect_tag
{
  uint8_t effectId;
  uint8_t effectVariant;
}zclCmdOnOff_OffWithEffect_t;

/* values of the effect identifier */
enum{
  gOnOffEffectId_DelayedAllOff_c = 0x00,
  gOnOffEffectId_DyingLight_c
    /* 0x02-0xFF reserved */
};

/* values of the effect variant */
enum{
  gOnOffEffectVariant_Default_c = 0x00, /* fade off in 0.8 seconds if EffectId = 0x00 or  
                                      20% dimUp in 0.5 s then fade to off in 1second if EffectId= 0x01 */
  gOnOffEffectVariant_NoFade_c,
  gOnOffEffectVariant_DimDown_c   /* 50% dim down in 0.8 seconds then fade to off in 12 seconds if EffectId = 0x00*/
    /* 0x03-0xFF reserved */
};

/* [R3] 6.6.1.4.4 Off with effect Command */
typedef PACKED_STRUCT zclOnOff_OffWithEffect_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdOnOff_OffWithEffect_t cmdFrame;
}zclOnOff_OffWithEffect_t;

/* [R3] 6.6.1.4.5 On with recall global scene Command */
typedef PACKED_STRUCT zclOnOff_OnWithRecallGlobalScene_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
}zclOnOff_OnWithRecallGlobalScene_t;

/* [R3] 6.6.1.4.6 payload format for On with timed off Command */
typedef PACKED_STRUCT zclCmdOnOff_OnWithTimedOff_tag
{
  uint8_t onOffControl;
  uint16_t onTime;
  uint16_t offWaitTime;
}zclCmdOnOff_OnWithTimedOff_t;

/* [R3] 6.6.1.4.6 On with timed off Command */
typedef PACKED_STRUCT zclOnOff_OnWithTimedOff_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdOnOff_OnWithTimedOff_t cmdFrame;
}zclOnOff_OnWithTimedOff_t;
#endif
typedef PACKED_STRUCT zclOnOffServerSetup_tag
{
  uint8_t       endpoint;                       /* endpoint */
  uint8_t       onOffTransitionTmrId;           /* transition timer*/
  uint32_t      nextTimePeriod;                 /* next Time period */
  uint8_t       onOffLastCommand;               /* OnOff last Command */
}zclOnOffServerSetup_t;

/******************************************
	On/Off Switch Configuration Cluster
*******************************************/
#if ( TRUE == gBigEndian_c )
/* on/off switch configuration cluster information attributes */
#define gZclAttrOnOffSwitchCfg_SwitchType_c               0x0000  /* M - SwitchType */ 
/* on/off switch configurarion cluster setting attributes */
#define gZclAttrOnOffSwitchCfg_SwitchAction_c             0x1000 /* M - Switch action */
#else
/* on/off switch configuration cluster information attributes */
#define gZclAttrOnOffSwitchCfg_SwitchType_c               0x0000  /* M - SwitchType */ 
/* on/off switch configurarion cluster setting attributes */
#define gZclAttrOnOffSwitchCfg_SwitchAction_c             0x0010 /* M - Switch action */
#endif /* #if ( TRUE == gBigEndian_c ) */



#define gZclAttrOnOffSwitchCfgSet_c                 0x00   /* OnOffSwitchConfig attributes Set */ 
#define gZclAttrIdOnOffSwitchCfg_SwitchType_c       0x00   /* M - SwitchType */ 
#define gZclAttrIdOnOffSwitchCfg_SwitchAction_c     0x10   /* M - Switch action */

typedef uint8_t  geZclSwitchType_t;
/* on/off switch configuration Switch type attr values */
enum {
    gZclSwitchType_ToggleSw_c      = 0x00, /* Toggle Switch Type */
    gZclSwitchType_Momentary_c     = 0x01, /* Momentary Switch type */
    gZclSwitchType_MultiFunction_c = 0x02  /* Multi function Type*/
};

typedef uint8_t  geZclSwitchAction_t;
/* on/off switch configuration Switch Acction Attr values */
enum {
     gZclSwitchAction_Off_c = 0x00,
     gZclSwitchAction_On_c = 0x01,
     gZclSwitchAction_Toggle_c = 0x02
};

typedef PACKED_STRUCT zclOnOffSwitchCfgAttrsRAM_tag{
   geZclSwitchType_t     SwitchType;    /* Switch type is an enumeration of the diferent switch types */
   geZclSwitchAction_t   SwitchAction;  /* Switch Action is an enumeration of the diferent switch action*/
} zclOnOffSwitchCfgAttrsRAM_t;

/* on/off switch configuration cluster commands */

/* No commands are generated by the client, except those 
    to read and write the attributes of the server. */

/******************************************
	Level Control Cluster
*******************************************/
#define gZclAttrSetLevelCtrl_c                         0x00

#define gZclAttrIdLevelControl_CurrentLevelId_c        0x00 /* M - CurrentLevel */
#define gZclAttrIdLevelControl_RemainingTimeId_c       0x01 /* O - RemainingTime */
#define gZclAttrIdLevelControl_OnOffTransitionTimeId_c 0x10 /* O - OnOffTransitionTime */
#define gZclAttrIdLevelControl_OnLevelId_c             0x11 /* O - OnLevel */
#define gZclAttrIdLevelControl_OnTransitionTimeId_c    0x12 /* O - OnTransitionTime */
#define gZclAttrIdLevelControl_OffTransitionTimeId_c   0x13 /* O - OffTransitionTime */
#define gZclAttrIdLevelControl_DefaultMoveRateId_c     0x14 /* O - DefaultMoveRate */

#if ( TRUE == gBigEndian_c )
/* level control cluster attributes */
#define gZclAttrLevelControl_CurrentLevelId_c        0x0000 /* M - CurrentLevel */
#define gZclAttrLevelControl_RemainingTimeId_c       0x0100 /* O - RemainingTime */
#define gZclAttrLevelControl_OnOffTransitionTimeId_c 0x1000 /* O - OnOffTransitionTime */
#define gZclAttrLevelControl_OnLevelId_c             0x1100 /* O - OnLevel */
#define gZclAttrLevelControl_OnTransitionTimeId_c    0x1200 /* O - OnTransitionTime */
#define gZclAttrLevelControl_OffTransitionTimeId_c   0x1300 /* O - OffTransitionTime */
#define gZclAttrLevelControl_DefaultMoveRateId_c     0x1400 /* O - DefaultMoveRate */
#else
/* level control cluster attributes */
#define gZclAttrLevelControl_CurrentLevelId_c        0x0000 /* M - CurrentLevel */
#define gZclAttrLevelControl_RemainingTimeId_c       0x0001 /* O - RemainingTime */
#define gZclAttrLevelControl_OnOffTransitionTimeId_c 0x0010 /* O - OnOffTransitionTime */
#define gZclAttrLevelControl_OnLevelId_c             0x0011 /* O - OnLevel */
#define gZclAttrLevelControl_OnTransitionTimeId_c    0x0012 /* O - OnTransitionTime */
#define gZclAttrLevelControl_OffTransitionTimeId_c   0x0013 /* O - OffTransitionTime */
#define gZclAttrLevelControl_DefaultMoveRateId_c     0x0014 /* O - DefaultMoveRate */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* level control cluster commands */
#define gZclCmdLevelControl_MoveToLevel_c        0x00 /* M - Move to Level */
#define gZclCmdLevelControl_Move_c               0x01 /* M - Move */
#define gZclCmdLevelControl_Step_c               0x02 /* M - Step */
#define gZclCmdLevelControl_Stop_c               0x03 /* M - Stop */
#define gZclCmdLevelControl_MoveToLevelOnOff_c   0x04 /* M - Move to Level (with On/Off) */
#define gZclCmdLevelControl_MoveOnOff_c          0x05 /* M - Move (with On/Off) */
#define gZclCmdLevelControl_StepOnOff_c          0x06 /* M - Step (with On/Off) */
#define gZclCmdLevelControl_StopOnOff_c          0x07 /* M - Stop (same as above, repeated for symmetry) */

typedef uint8_t zclMoveMode_t;
#define zclMoveMode_Up           0x00 /* Up */
#define zclMoveMode_Down         0x01 /* Down */

#define gZclLevelControl_DefaultMoveRate_d 0x64

/* 3.10.2.3.1/3.10.2.3.5 Move to Level Commands */
typedef PACKED_STRUCT zclCmdLevelControl_MoveToLevel_tag
{
  uint8_t  level;
  uint16_t transitionTime; /* in seconds */
} zclCmdLevelControl_MoveToLevel_t;

/* 3.10.2.3.2/3.10.2.3.6 Move Commands */
typedef PACKED_STRUCT zclCmdLevelControl_Move_tag
{
  zclMoveMode_t moveMode;
  uint8_t       rate;   /* in steps per second */
} zclCmdLevelControl_Move_t;

/* 3.10.2.3.3/3.10.2.3.7 Step Commands */
typedef PACKED_STRUCT zclCmdLevelControl_Step_tag
{
  zclMoveMode_t   stepMode;
  uint8_t         stepSize;         /* amount to step */
  uint16_t        transitionTime;   /* time in 1/10th second */
} zclCmdLevelControl_Step_t;

/* 3.10.2.3.4/3.10.2.3.8 Stop Commands */
typedef PACKED_STRUCT zclCmdLevelControl_Stop_tag
{
  uint8_t stopCmd;                  
} zclCmdLevelControl_Stop_t;        /* dummy PACKED_STRUCT, no actual payload */

/* level control move to level requests */
typedef PACKED_STRUCT zclLevelControlMoveToLevel_tag
{
  afAddrInfo_t                      addrInfo;  /* IN: */
  zclCmdLevelControl_MoveToLevel_t   cmdFrame;  /* command frame */
} zclLevelControlMoveToLevel_t;

/* level control move requests */
typedef PACKED_STRUCT zclLevelControlMove_tag
{
  afAddrInfo_t               addrInfo;  /* IN: */
  zclCmdLevelControl_Move_t   cmdFrame;  /* IN: command frame */
} zclLevelControlMove_t;

/* level control move requests */
typedef PACKED_STRUCT zclLevelControlStep_tag
{
  afAddrInfo_t               addrInfo;  /* IN: */
  zclCmdLevelControl_Step_t  cmdFrame;  /* IN: command frame */
} zclLevelControlStep_t;


/* level control stop requests */
typedef PACKED_STRUCT zclLevelControlStop_tag
{
  afAddrInfo_t               addrInfo;  /* IN: */
  zclCmdLevelControl_Stop_t  cmdFrame;
} zclLevelControlStop_t;


typedef PACKED_UNION zclLevelControlReq_tag 
{
  zclLevelControlMoveToLevel_t moveToLevelReq;
  zclLevelControlMove_t moveReq;
  zclLevelControlStep_t stepReq;
  zclLevelControlStop_t stopReq;
  zclLevelControlMoveToLevel_t moveToLevelOnOffReq;
  zclLevelControlMove_t moveOnOffReq;
  zclLevelControlStep_t stepOnOffReq;
  zclLevelControlStop_t stopOnOffReq;
} zclLevelControlReq_t; 

typedef PACKED_UNION zclLevelControlCmd_tag 
{
  zclCmdLevelControl_MoveToLevel_t MoveToLevelCmd;
  zclCmdLevelControl_Move_t MoveCmd;
  zclCmdLevelControl_Step_t StepCmd;
  zclCmdLevelControl_Stop_t StopCmd;
} zclLevelControlCmd_t; 


typedef uint16_t zbLevelCtrlTime_t;

/* Level values */
typedef uint8_t zclLevelValue_t;
#define  gZclLevel_off                  (gZclLevelMinValue)
#define  gZclLevel_LowestPossible       (gZclLevel_off + 1)
#define  gZclLevel_low                  (gZclLevelDelta - 1)
#define  gZclLevel_medium               (gZclLevelDelta*2) - 1
#define  gZclLevel_high                 (gZclLevelMaxValue)
#define  gZclLevel_HighestPossible      (gZclLevelMaxValue - 1)

typedef PACKED_STRUCT zclLevelCtrlAttrsRAM_tag
{
  zclLevelValue_t     currentLevel[zclReportableCopies_c];   /* Current level off the light */
#if gZclClusterOptionals_d
  zbLevelCtrlTime_t   remainingTime;            /* Remaining time to complete the current command 1/10ths of a sec */
  zbLevelCtrlTime_t   onOffTransitionTime;      /* Time taken to move to of from the target level when switched on of off */
  zclLevelValue_t     onLevel;                  /* current level is set to this value when turn on the switch */
  zbLevelCtrlTime_t   onTransitionTime;         /* Time taken to move the current level from 0 to 255 when an ON Command is received */
  zbLevelCtrlTime_t   offTransitionTime;        /* Time taken to move the current level from 255 to 0 when an Off Command is received  */
  uint8_t             defaultMoveRate;          /* Time taken to move the current level from 0 to 255 or 255 to 0 when a MoveCommand(rateField = 0xFF) is received*/
#endif
} zclLevelCtrlAttrsRAM_t;


/* keep a copy for each level control instance */
typedef PACKED_STRUCT zclLevelControlSetup_tag
{
  zbEndPoint_t                  endpoint;                        /* endpoint */
  uint8_t                       zclLevel_TransitionTmrID;        /* used for the Level control cluster commands*/
  bool_t                        levelOnOff;                      /* Level_OnOff */
  uint8_t                       levelCommand;                    /* LevelCmd*/
  uint8_t                       moveMode;                        /* MoveMode */
  uint8_t                       onoffState;                      /* OnOffState */
  uint16_t                      timeBtwChanges;                  /* TimeBetweenChanges*/
  zclLevelValue_t               lastCurrentLevel;                /* LastCurrentLevel*/
  zclLevelValue_t               newCurrentLevel;                 /* NewCurrentLevel*/
  zclLevelValue_t               levelDifference;                 /* Level_diference  */
  zbTmrTimeInMilliseconds_t       remainingTime;                   /* RemainingTimeInMilliseconds*/
}zclLevelControlSetup_t;

/*************************************************************************
*               Commissioning cluster definitions
*              (064699r12  Tables 5, 12, 13 and 14)
**************************************************************************/
#define gZclAttrIdCommissioning_ShortAddressID_c                 0x00
#define gZclAttrIdCommissioning_ExtendedPANIdID_c                0x01
#define gZclAttrIdCommissioning_PanIdID_c                        0x02
#define gZclAttrIdCommissioning_ChannelMaskID_c                  0x03
#define gZclAttrIdCommissioning_ProtocolVersionID_c              0x04
#define gZclAttrIdCommissioning_StackProfileID_c                 0x05
#define gZclAttrIdCommissioning_StartupControlID_c               0x06
#define gZclAttrIdCommissioning_TrustCenterAddressID_c           0x10
#define gZclAttrIdCommissioning_TrustCenterMasterKeyID_c         0x11
#define gZclAttrIdCommissioning_NetworkKeyID_c                   0x12
#define gZclAttrIdCommissioning_UseInsecureJoinID_c              0x13
#define gZclAttrIdCommissioning_PreconfiguredLinkKeyID_c         0x14
#define gZclAttrIdCommissioning_NetworkKeySeqNumID_c             0x15
#define gZclAttrIdCommissioning_NetworkKeyTypeID_c               0x16
#define gZclAttrIdCommissioning_NetworkManagerAddressID_c        0x17

/* Attributes Identifier for Join Parameters attribute set*/
#define gZclAttrIdCommissioning_ScanAttemptsID_c                 0x20
#define gZclAttrIdCommissioning_TimeBetweenScansID_c             0x21
#define gZclAttrIdCommissioning_RejoinIntervalID_c               0x22
#define gZclAttrIdCommissioning_MaxRejoinIntervalID_c            0x23

/* Attributes Identifier for End Device Parameters attribute set*/
#define gZclAttrIdCommissioning_IndirectPollRateID_c             0x30
#define gZclAttrIdCommissioning_ParentRetryThreshoID_c           0x31
                                                            
/* Attributes Identifier for Concentrator Parameters  attribute set*/
#define gZclAttrIdCommissioning_ConcentratorFlagID_c             0x40
#define gZclAttrIdCommissioning_ConcentratorRadiusID_c           0x41
#define gZclAttrIdCommissioning_ConcentratorDiscoveryTimeID_c    0x42

#if (gBigEndian_c)
/* Attributes Identifier for Startup Parameters  attribute set */
#define gZclAttrCommissioning_ShortAddressID_c                 0x0000
#define gZclAttrCommissioning_ExtendedPANIdID_c                0x0100
#define gZclAttrCommissioning_PanIdID_c                        0x0200
#define gZclAttrCommissioning_ChannelMaskID_c                  0x0300
#define gZclAttrCommissioning_ProtocolVersionID_c              0x0400
#define gZclAttrCommissioning_StackProfileID_c                 0x0500
#define gZclAttrCommissioning_StartupControlID_c               0x0600
#define gZclAttrCommissioning_TrustCenterAddressID_c           0x1000
#define gZclAttrCommissioning_TrustCenterMasterKeyID_c         0x1100
#define gZclAttrCommissioning_NetworkKeyID_c                   0x1200
#define gZclAttrCommissioning_UseInsecureJoinID_c              0x1300
#define gZclAttrCommissioning_PreconfiguredLinkKeyID_c         0x1400
#define gZclAttrCommissioning_NetworkKeySeqNumID_c             0x1500
#define gZclAttrCommissioning_NetworkKeyTypeID_c               0x1600
#define gZclAttrCommissioning_NetworkManagerAddressID_c        0x1700

/* Attributes Identifier for Join Parameters attribute set*/
#define gZclAttrCommissioning_ScanAttemptsID_c                 0x2000
#define gZclAttrCommissioning_TimeBetweenScansID_c             0x2100
#define gZclAttrCommissioning_RejoinIntervalID_c               0x2200
#define gZclAttrCommissioning_MaxRejoinIntervalID_c            0x2300

/* Attributes Identifier for End Device Parameters attribute set*/
#define gZclAttrCommissioning_IndirectPollRateID_c             0x3000
#define gZclAttrCommissioning_ParentRetryThreshoID_c           0x3100
                                                            
/* Attributes Identifier for Concentrator Parameters  attribute set*/
#define gZclAttrCommissioning_ConcentratorFlagID_c             0x4000
#define gZclAttrCommissioning_ConcentratorRadiusID_c           0x4100
#define gZclAttrCommissioning_ConcentratorDiscoveryTimeID_c    0x4200

#else /* #if (gBigEndian_c) */

/* Attributes Identifier for Startup Parameters  attribute set */
#define gZclAttrCommissioning_ShortAddressID_c                 0x0000
#define gZclAttrCommissioning_ExtendedPANIdID_c                0x0001
#define gZclAttrCommissioning_PanIdID_c                        0x0002
#define gZclAttrCommissioning_ChannelMaskID_c                  0x0003
#define gZclAttrCommissioning_ProtocolVersionID_c              0x0004
#define gZclAttrCommissioning_StackProfileID_c                 0x0005
#define gZclAttrCommissioning_StartupControlID_c               0x0006
#define gZclAttrCommissioning_TrustCenterAddressID_c           0x0010
#define gZclAttrCommissioning_TrustCenterMasterKeyID_c         0x0011
#define gZclAttrCommissioning_NetworkKeyID_c                   0x0012
#define gZclAttrCommissioning_UseInsecureJoinID_c              0x0013
#define gZclAttrCommissioning_PreconfiguredLinkKeyID_c         0x0014
#define gZclAttrCommissioning_NetworkKeySeqNumID_c             0x0015
#define gZclAttrCommissioning_NetworkKeyTypeID_c               0x0016
#define gZclAttrCommissioning_NetworkManagerAddressID_c        0x0017

/* Attributes Identifier for Join Parameters attribute set*/
#define gZclAttrCommissioning_ScanAttemptsID_c                 0x0020
#define gZclAttrCommissioning_TimeBetweenScansID_c             0x0021
#define gZclAttrCommissioning_RejoinIntervalID_c               0x0022
#define gZclAttrCommissioning_MaxRejoinIntervalID_c            0x0023

/* Attributes Identifier for End Device Parameters attribute set*/
#define gZclAttrCommissioning_IndirectPollRateID_c             0x0030
#define gZclAttrCommissioning_ParentRetryThreshoID_c           0x0031
                                                            
/* Attributes Identifier for Concentrator Parameters  attribute set*/
#define gZclAttrCommissioning_ConcentratorFlagID_c             0x0040
#define gZclAttrCommissioning_ConcentratorRadiusID_c           0x0041
#define gZclAttrCommissioning_ConcentratorDiscoveryTimeID_c    0x0042

#endif /* #if (gBigEndian_c) */


/* StartupControl attribute usage  */ 
enum
{
  gIsPartofNetwork_c     = 0x00, /*the device should consider itself part of the network indicated by the ExtendedPANId */
  gFormWithExtPanId_c    = 0x01, /* form a network with extended PAN ID */
  gRejoinWithExtPanId_c  = 0x02, /* the device should rejoin the network with extended PAN ID given by the ExtendedPANId attribute */
  gStartFromScratch_c    = 0x03  /* start "from scratch" and join the network using (unsecured) MAC association */
};

/*(064699r12  Table 15 - Commands Received by the Commissioning Cluster Server)*/
#define gZclCmdCommissiong_RestartDeviceRequest_c               0x00  /*M*/
#define gZclCmdCommissiong_SaveStartupParametersRequest_c       0x01  /*O*/
#define gZclCmdCommissiong_RestoreStartupParametersRequest_c    0x02  /*O*/
#define gZclCmdCommissiong_ResetStartupParametersRequest_c      0x03  /*M*/

/*(064699r12  Table 17 - Commands Generated by the Commissioning Cluster Server)*/
#define gZclCmdCommissiong_RestartDeviceResponse_c              0x00  /*M*/
#define gZclCmdCommissiong_SaveStartupParametersResponse_c      0x01  /*M*/
#define gZclCmdCommissiong_RestoreStartupParametersResponse_c   0x02  /*M*/
#define gZclCmdCommissiong_ResetStartupParametersResponse_c     0x03  /*M*/


typedef PACKED_STRUCT zclCmdCommissiong_RestartDeviceRequest_tag
{
  uint8_t   Options;       /* Startup mode, immediate subfield*/
  uint8_t   Delay;         /* Startup delay in seconds */
  uint8_t   Jitter;        /* Random jitter range in milliseconds */
} zclCmdCommissiong_RestartDeviceRequest_t;

#define gCommisioningClusterRestartDeviceRequestOptions_StartUpModeSubField_Mask_c      0x03
#define gCommisioningClusterRestartDeviceRequestOptions_ImmediateSubField_Mask_c        0x04


#define gCommisioningClusterRestartDeviceRequestOptions_ImmediateSubField_RestartImmediately_c   0x00
#define gCommisioningClusterRestartDeviceRequestOptions_ImmediateSubField_Restartconveniently_c  0x04

#define gCommisioningClusterRestartDeviceRequestOptions_ModeSubField_RestartUsingCurrentStartupParameters_c  0x00
#define gCommisioningClusterRestartDeviceRequestOptions_ModeSubField_RestartUsingCurrentStackParameter_c     0x01


typedef PACKED_STRUCT zclCmdCommissiong_SaveStartupParametersRequest_tag
{
  uint8_t   Options;       /* Reserved */
  uint8_t   Index;         /* Startup Attribute Set index */
} zclCmdCommissiong_SaveStartupParametersRequest_t;

typedef PACKED_STRUCT zclCmdCommissiong_RestoreStartupParametersRequest_tag
{
  uint8_t   Options;       /* Reserved */
  uint8_t   Index;         /* Startup Attribute Set index */
} zclCmdCommissiong_RestoreStartupParametersRequest_t;


typedef PACKED_STRUCT zclCmdCommissiong_ResetStartupParametersRequest_tag
{
  uint8_t   Options;       /* Reserved */
  uint8_t   Index;         /* Startup Attribute Set index */
} zclCmdCommissiong_ResetStartupParametersRequest_t;

#define gCommisioningClusterResetStartupParametersRequestOptions_ResetCurrentSubField_Mask_c      0x01
#define gCommisioningClusterResetStartupParametersRequestOptions_ResetAllSubField_Mask_c          0x02
#define gCommisioningClusterResetStartupParametersRequestOptions_EraseIndexSubField_Mask_c        0x04

typedef PACKED_UNION zclCommissioningCmd_tag 
{
  zclCmdCommissiong_RestartDeviceRequest_t RestartDeviceCmd;
  zclCmdCommissiong_SaveStartupParametersRequest_t    SaveStartupParameterCmd;
  zclCmdCommissiong_RestoreStartupParametersRequest_t RestoreStartupParameterCmd;
  zclCmdCommissiong_ResetStartupParametersRequest_t   ResetStartupParameterCmd;
} zclCommissioningCmd_t;

/*
All commands have the same type of response so there is only defined one
type def.
*/

typedef PACKED_STRUCT zclCmdCommissiong_response_tag
{
  uint8_t   status;         /* response status */
} zclCmdCommissiong_response_t;


/* Commisioning Response format:
   All responses uses the same format as a Default ZCL response defined by:
   zclCmdDefaultRsp_t
 */

typedef PACKED_STRUCT  zclCommissioningRestartDeviceReq_tag 
{  
  afAddrInfo_t addrInfo; 
  zclCmdCommissiong_RestartDeviceRequest_t cmdFrame;
} zclCommissioningRestartDeviceReq_t;

typedef PACKED_STRUCT  zclCommissioningSaveStartupParametersReq_tag 
{  
  afAddrInfo_t addrInfo; 
  zclCmdCommissiong_SaveStartupParametersRequest_t cmdFrame;
} zclCommissioningSaveStartupParametersReq_t;

typedef PACKED_STRUCT  zclCommissioningRestoreStartupParametersReq_tag 
{  
  afAddrInfo_t addrInfo; 
  zclCmdCommissiong_RestoreStartupParametersRequest_t cmdFrame;
} zclCommissioningRestoreStartupParametersReq_t;

typedef PACKED_STRUCT  zclCommissioningResetStartupParametersReq_tag 
{  
  afAddrInfo_t addrInfo; 
  zclCmdCommissiong_ResetStartupParametersRequest_t cmdFrame;
} zclCommissioningResetStartupParametersReq_t;


extern const zclAttrSetList_t gZclCommissioningServerClusterAttrSetList;
extern zbCommissioningAttributes_t gCommisioningServerAttrsData;

/*******************************************************
	Alarms Cluster (ZigBee Cluster Library Chapter 3.11)
*******************************************************/
/* 3.11.2.2.1 Alarm Information Attributes Set */

/* Alarms cluster attributes set */
#define gZclAttrSetAlarms_AlarmInformation_c    0x00 /* Alarm Information */

#define gZclAttrIdAlarms_AlarmCount_c             0x00  /* 0 - AlarmCount */

#if ( TRUE == gBigEndian_c )
#define  gZclAttrAlarms_AlarmCount_c           0x0000 /* 0 - AlarmCount */
#else
#define  gZclAttrAlarms_AlarmCount_c           0x0000 /* 0 - AlarmCount */
#endif /* #if ( TRUE == gBigEndian_c ) */


/* 3.11.2.3.1 Alarm Table */

typedef PACKED_STRUCT zclAlarmsAttrsRAM_tag
{
  uint16_t count;
}zclAlarmsAttrsRAM_t;

typedef PACKED_STRUCT gZclAlarmTable_tag
{            
    uint8_t     AlarmCode; 
    uint16_t    ClusterID;
    uint32_t    TimeStamp;                                      
}gZclAlarmTable_t; 

#define MaxAlarmsPermitted 0x03 

/* 3.11.2.4 Commands Received */
#define gAlarmClusterRxCommandID_ResetAlarm_c           0x00 /* M - Reset Alarm */
#define gAlarmClusterRxCommandID_ResetAllAlarms_c       0x01 /* M - Reset All alarms */
#define gAlarmClusterRxCommandID_GetAlarm_c             0x02 /* O - Get Alarm */
#define gAlarmClusterRxCommandID_ResetAlarmLog_c        0x03 /* O - Reset Alarm Log */
                                                        /* 0x04-0xFF Reserved */

/* Reset Alarm Payload */
typedef PACKED_STRUCT zclCmdAlarmInformation_ResetAlarm_tag 
{
   uint8_t   AlarmCode;
   uint16_t  ClusterID;
}zclCmdAlarmInformation_ResetAlarm_t; 

typedef PACKED_STRUCT zclAlarmInformation_ResetAlarm_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdAlarmInformation_ResetAlarm_t cmdFrame;
}zclAlarmInformation_ResetAlarm_t; 

/* 3.11.2.5 Commands Generated */
#define gAlarmClusterTxCommandID_Alarm_c                0x00    /* M - Alarm */
#define gAlarmClusterTxCommandID_GetAlarmResponse_c     0x01    /* O - Get Alarm Response */
#define gAlarmClusterTxCommandID_ClearAlarm_c           0x03    /* O - Clear Alarm*/
                                                 /* 0x02, 0x04-0xFF Reserved */


/* for no payload command */
typedef PACKED_STRUCT zclAlarmInformation_NoPayload_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
}zclAlarmInformation_NoPayload_t; 


/* Alarm Payload */
typedef PACKED_STRUCT zclCmdAlarmInformation_Alarm_tag 
{
 uint8_t   AlarmCode;
 uint16_t  ClusterID;
}zclCmdAlarmInformation_Alarm_t; 

typedef PACKED_STRUCT zclAlarmInformation_Alarm_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdAlarmInformation_Alarm_t cmdFrame;
}zclAlarmInformation_Alarm_t; 

/* Get Alarm Response Payload */
typedef PACKED_STRUCT zclCmdAlarmInformation_GetAlarmResponse_tag 
{
 uint8_t   Status;
 uint8_t   AlarmCode;
 uint16_t  ClusterID;
 uint32_t   TimeStamp;
}zclCmdAlarmInformation_GetAlarmResponse_t;

typedef PACKED_STRUCT zclAlarms_GetAlarmResponse_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdAlarmInformation_GetAlarmResponse_t cmdFrame;
}zclAlarms_GetAlarmResponse_t;

/* Get Alarm Response Payload (HA1.2 errata)*/
typedef PACKED_STRUCT zclCmdAlarms_ClearAlarm_tag 
{
  uint8_t       alarmCode;
  uint16_t      clusterID;
}zclCmdAlarms_ClearAlarm_t;

/* (HA1.2 errata) */
typedef PACKED_STRUCT zclAlarms_ClearAlarm_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdAlarms_ClearAlarm_t cmdFrame;
}zclAlarms_ClearAlarm_t;

/******************************
  Time Cluster
  See ZCL Specification (075123r02) Section 3.12
*******************************/

#define gZclAttrSetTime_c         0x00

#define gZclAttrIdTime_c            0x00 /* M - Time */
#define gZclAttrIdTimeStatus_c      0x01 /* M - Time Status */
#define gZclAttrIdTimeZone_c        0x02 /* O - Time Zone */
#define gZclAttrIdDstStart_c        0x03 /* O - DST Start */
#define gZclAttrIdDstEnd_c          0x04 /* O - DST End */
#define gZclAttrIdDstShift_c        0x05 /* O - DST Shift */
#define gZclAttrIdStandardTime_c    0x06 /* O - Standard Time */
#define gZclAttrIdLocalTime_c       0x07 /* O - Local Time */
#define gZclAttrIdLastSetTime_c     0x08 /* O - Last Set Time */
#define gZclAttrIdValidUntilTime_c  0x09 /* O - Valid Until Time */


/* Full attribute ids*/
#if ( TRUE == gBigEndian_c )
#define gZclAttrTime_c                  0x0000 
#define gZclAttrTimeStatus_c            0x0100
#define gZclAttrTimeZone_c              0x0200
#define gZclAttrDstStart_c              0x0300
#define gZclAttrDstEnd_c                0x0400
#define gZclAttrDstShift_c              0x0500
#define gZclAttrStandardTime_c          0x0600
#define gZclAttrLocalTime_c	        0x0700
#define gZclAttrLastSetTime_c           0x0800
#define gZclAttrValidUntilTime_c        0x0900
#else
#define gZclAttrTime_c                  0x0000 
#define gZclAttrTimeStatus_c            0x0001
#define gZclAttrTimeZone_c              0x0002
#define gZclAttrDstStart_c              0x0003
#define gZclAttrDstEnd_c                0x0004
#define gZclAttrDstShift_c              0x0005
#define gZclAttrStandardTime_c		0x0006
#define gZclAttrLocalTime_c		0x0007
#define gZclAttrLastSetTime_c   	0x0008
#define gZclAttrValidUntilTime_c	0x0009
#endif /* #if ( TRUE == gBigEndian_c ) */


#define zclTimeStatusMaster         0x01
#define zclTimeStatusSynchronized   0x02
#define zclTimeStatusMasterZoneDst  0x04
#define zclTimeStatusSuperseeded    0x08


/* time cluster default information: */
/* ZCL Time Cluster starts at 01 Jan 2000 00:00:00 GMT, hence -946684800 offset to unix Epox 01 Jan 1970 00:00:00 GMT */
#define mTimeClusterAppTimeEpoxOffset_c 946684800
/* 14 may 2013 12:00:00 GMT */
#define mDefaultValueOfTimeClusterAppTime_c 1368532800 - mTimeClusterAppTimeEpoxOffset_c  
/* Time Zone GMT+2 - Bucharest, Romania */
#define mDefaultValueOfTimeClusterAppTimeZone_c 7200
/* Start daylight savings 2013 for Bucharest, Romania 30 March 2013*/
#define mDefaultValueOfTimeClusterAppDstStart_c 1364616000 - mTimeClusterAppTimeEpoxOffset_c  
/* End daylight savings 2013 for Bucharest, Romania 30 september 2013 */
#define mDefaultValueOfTimeClusterAppDstEnd_c 1380513600 - mTimeClusterAppTimeEpoxOffset_c   
/* Daylight Saving + 1h */
#define mDefaultValueOfClusterAppDstShift_c 3600
/* Valid Until Time - mDefaultValueOfTimeClusterTime_c + aprox 5 years */
#define mDefaultValueOfTimeClusterAppValidUntilTime_c mDefaultValueOfTimeClusterAppTime_c + 31536000


#define gTime_StartZigbeeYearTime_c		2000	/* Zigbee Start Year Time =  2000*/
#define gTime_StartZigbeeWeekDay_c		6		/* Zigbee Start time = 1 January  2000 (Saturday = 5) */
#define gTime_SecondsDay_c				86400	/* 60*60*24 */
#define gTime_SecondsHour_c				3600
#define gTime_SecondsMinute_c			60	
#define gTime_NoOfDays_c				7		
#define gTime_NoOfHours_c				24
#define Time_LeapYear(year)	(!((year) % 4) && (((year) % 100) || !((year) % 400)))	
#define Time_YearSize(year)	(Time_LeapYear(year)?366:365)


/* gZclDataTypeUTCTime_c - note stored as little endian.*/
typedef uint32_t ZCLTime_t;

typedef PACKED_STRUCT ZCLTimeServerAttrsRAM_tag
{
   /*Attributes of the Time attribute set */
  ZCLTime_t   Time;
  uint8_t     TimeStatus;
  int32_t     TimeZone;
  uint32_t    DstStart;
  uint32_t    DstEnd;
  int32_t     DstShift;
  uint32_t    StandardTime;
  uint32_t    LocalTime;
  ZCLTime_t   LastSetTime;
  ZCLTime_t   ValidUntilTime;
} ZCLTimeServerAttrsRAM_t;

/* structure used to set the time in the Time cluster - note all data is little endian*/
typedef PACKED_STRUCT ZCLTimeConf_tag
{
  ZCLTime_t   Time;
  uint8_t     TimeStatus;
  int32_t     TimeZone;
  uint32_t    DstStart;
  uint32_t    DstEnd;
  int32_t     DstShift;
  ZCLTime_t   ValidUntilTime;
} ZCLTimeConf_t;


typedef PACKED_STRUCT zclTime_GetTimeInf_tag
{
  uint8_t   timeWeekDay;	/* Sunday = 0x00, Monday=0x01...Saturday=0x06 */
  uint8_t 	timeHour;		/* 1.. 24 */	
  uint8_t	timeMinute; 	/* 1.. 60 */  
  uint8_t	timeday;		/* 1.. 31 */
  uint8_t	timeMonth;		/* 1.. 12 */
  uint16_t 	timeYear;   	/* > 2000 */	
}zclTime_GetTimeInf_t;

extern const zclAttrSetList_t gZclTimeServerClusterAttrSetList;

/*Initialize the Time Cluster  - note input must be little endian*/
void ZCL_TimeInit(uint8_t endpoint, ZCLTimeConf_t *pTimeConf);

/*Set UTC Time from the backend - native endianess */
void ZCL_SetUTCTime(uint8_t endpoint, uint32_t time);

/*Get UTC Time from the backend - native endianess */
uint32_t ZCL_GetUTCTime(uint8_t endpoint);

/*Set TimeZone from the backend - native endianess */
void ZCL_SetTimeZone(uint8_t endpoint, int32_t timeZone);
/*Set Daylight saving interval from the backend - native endianess */
void ZCL_SetTimeDst(uint8_t endpoint, uint32_t DstStart, uint32_t DstEnd, int32_t DstShift);

/*************************************************************
	RSSI Location Cluster (ZigBee Cluster Library Chapter 3.13)
*************************************************************/

#if gBigEndian_c
#define gZclAttrRSSILocationInformationSet_c            0x0000
#define gZclAttrRSSILocationSettingsSet_c               0x0100
#else /* #if gBigEndian_c */
#define gZclAttrRSSILocationInformationSet_c            0x0000
#define gZclAttrRSSILocationSettingsSet_c               0x0001
#endif /* #if gBigEndian_c */


/* Location Information Attribute Set */
#if (TRUE == gBigEndian_c)
#define gZclAttrLocationInformation_LocationType_c         0x0000 /*M*/
#define gZclAttrLocationInformation_LocationMethod_c       0x0100 /*M*/
#define gZclAttrLocationInformation_LocationAge_c          0x0200 /*O*/
#define gZclAttrLocationInformation_QualityMeasure_c       0x0300 /*O*/
#define gZclAttrLocationInformation_NumberOfDevices_c      0x0400 /*O*/
#else
#define gZclAttrLocationInformation_LocationType_c         0x0000 /*M*/
#define gZclAttrLocationInformation_LocationMethod_c       0x0001 /*M*/
#define gZclAttrLocationInformation_LocationAge_c          0x0002 /*O*/
#define gZclAttrLocationInformation_QualityMeasure_c       0x0003 /*O*/
#define gZclAttrLocationInformation_NumberOfDevices_c      0x0004 /*O*/
#endif /* #if (TRUE == gBigEndian_c) */

/* Location Type Attribute */

typedef PACKED_STRUCT LocationType_tag
{
 unsigned   Absolute         : 1;
 unsigned   Dimension        : 1;
 unsigned   CoordinateSystem : 2;
 unsigned   Reserved         : 4;
}LocationType_t;

#define ABSOLUTE_LOCATION   1
#define MEASURED_LOCATION   0
#define TWO_DIMENSIONAL     1
#define THREE_DIMENSIONAL   0
#define RECTANGULAR         0

/* Location Method Attribute */

enum
{
  gLocationMethod_Lateration_c = 0,     /* Lateration */
  gLocationMethod_Signposting_c,        /* Signposting */
  gLocationMethod_RFfingerprinting_c,   /* RF Fingerprinting */
  gLocationMethod_OutOfBand_c,          /* Out of Band */
                                        /* 0x04-0x3F Reserved */
                                        /* 0x40-0xFF Reserved for manufacturer */
}; 

/* Location Settings Attribute Set */
#if (TRUE == gBigEndian_c)
#define gZclAttrLocationSettings_Coordinate1_c              0x1000 /*M*/
#define gZclAttrLocationSettings_Coordinate2_c              0x1100 /*M*/
#define gZclAttrLocationSettings_Coordinate3_c              0x1200 /*O*/
#define gZclAttrLocationSettings_Power_c                    0x1300 /*M*/
#define gZclAttrLocationSettings_PathLossExponent_c         0x1400 /*M*/
#define gZclAttrLocationSettings_ReportingPeriod_c          0x1500 /*O*/
#define gZclAttrLocationSettings_CalculationPeriodt_c       0x1600 /*O*/
#define gZclAttrLocationSettings_NumberRSSImeasurements_c   0x1700 /*M*/
#else
#define gZclAttrLocationSettings_Coordinate1_c              0x0010 /*M*/
#define gZclAttrLocationSettings_Coordinate2_c              0x0011 /*M*/
#define gZclAttrLocationSettings_Coordinate3_c              0x0012 /*O*/
#define gZclAttrLocationSettings_Power_c                    0x0013 /*M*/
#define gZclAttrLocationSettings_PathLossExponent_c         0x0014 /*M*/
#define gZclAttrLocationSettings_ReportingPeriod_c          0x0015 /*O*/
#define gZclAttrLocationSettings_CalculationPeriodt_c       0x0016 /*O*/
#define gZclAttrLocationSettings_NumberRSSImeasurements_c   0x0017 /*M*/
#endif /* #if (TRUE == gBigEndian_c) */

/* 3.13.2.3 RSSI Location server cluster Commands Received */
enum
{
  gZclCmdRxRSSI_SetAbsoluteLocation_c = 0,     /* M - Set Absolute Location */
  gZclCmdRxRSSI_SetDeviceConfiguration_c,      /* M - Set Device Configuration */
  gZclCmdRxRSSI_GetDeviceConfiguration_c,      /* M - Get Device Configuration */
  gZclCmdRxRSSI_GetLocationData_c,             /* M - Get Location Data */
                                               /* 0x04-0xFF Reserved */
}; 


typedef PACKED_STRUCT zclCmdRSSI_AbsoluteLocation_tag 
{
 int16_t  Coordinate1;
 int16_t  Coordinate2;
 int16_t  Coordinate3;
 int16_t  Power;
 uint16_t PathLossExponent;
}zclCmdRSSI_AbsoluteLocation_t; 

typedef PACKED_STRUCT zclCmdRSSI_SetDeviceConfiguration_tag 
{
 int16_t  Power;
 uint16_t PathLossExponent;
 uint16_t CalculationPeriod;
 uint16_t NumberRSSImeasurements;
 uint16_t ReportingPeriod;
}zclCmdRSSI_SetDeviceConfiguration_t;

typedef PACKED_STRUCT 
{
  uint32_t  AddressHigh;
  uint32_t  AddressLow;
}IEEEaddress_t;

typedef PACKED_STRUCT zclCmdRSSI_GetDeviceConfiguration_tag 
{
  IEEEaddress_t TargetAddress;  
}zclCmdRSSI_GetDeviceConfiguration_t;

typedef PACKED_STRUCT GetLocationData_tag
{
 unsigned   Reserved           : 3;
 unsigned   CompactResponse    : 1;
 unsigned   BroadcastResponse  : 1;
 unsigned   BroadcastIndicator : 1;
 unsigned   ReCalculate        : 1;
 unsigned   AbsoluteOnly       : 1;
}GetLocationData_t;

typedef PACKED_STRUCT zclCmdRSSI_GetLocationData_tag 
{
  GetLocationData_t   GetLocationData;
  uint8_t             NumberResponses;   
  IEEEaddress_t       TargetAddress;  
}zclCmdRSSI_GetLocationData_t;


/* 3.13.2.4 RSSI Location server cluster Commands Generated */
enum
{
  gZclCmdTxRSSI_DeviceConfigurationResponse_c = 0,     /* M - Set Absolute Location */
  gZclCmdTxRSSI_LocationDataResponse_c,                /* M - Set Device Configuration */
  gZclCmdTxRSSI_LocationDataNotification_c,            /* M - Get Device Configuration */
  gZclCmdTxRSSI_CompactLocationDataNotification_c,     /* M - Get Location Data */
  gZclCmdTxRSSI_RSSIping_c,                            /* M - Get Location Data */
                                                       /* 0x05-0xFF Reserved */
}; 
 

typedef PACKED_STRUCT zclCmdRSSI_DeviceConfigurationResponse_tag 
{
 uint8_t   Status;
 int16_t   Power;
 uint16_t  PathLossExponent;
 uint16_t  CalculationPeriod;
 uint8_t   NumberRSSImeasurements;
 uint16_t  ReportingPeriod;
}zclCmdRSSI_DeviceConfigurationResponse_t; 

typedef PACKED_STRUCT zclCmdRSSI_LocationDataResponse_tag 
{
 uint8_t          Status;
 LocationType_t   LocationType;
 int16_t          Coordinate1;
 int16_t          Coordinate2;
 int16_t          Coordinate3;
 int16_t          Power;
 uint16_t         PathLossExponent;
 uint8_t          LocationMethod;
 uint8_t          QualityMeasure;
 uint16_t         LocationAge;
}zclCmdRSSI_LocationDataResponse_t; 

typedef PACKED_STRUCT zclCmdRSSI_LocationDataNotification_tag 
{
 LocationType_t   LocationType;
 int16_t          Coordinate1;
 int16_t          Coordinate2;
 int16_t          Coordinate3;
 int16_t          Power;
 uint16_t         PathLossExponent;
 uint8_t          LocationMethod;
 uint8_t          QualityMeasure;
 uint16_t         LocationAge;
}zclCmdRSSI_LocationDataNotification_t; 

typedef PACKED_STRUCT zclCmdRSSI_CompactLocationDataNotification_tag 
{
 LocationType_t   LocationType;
 int16_t          Coordinate1;
 int16_t          Coordinate2;
 int16_t          Coordinate3;
 uint8_t          QualityMeasure;
 uint16_t         LocationAge;
}zclCmdRSSI_CompactLocationDataNotification_t; 

typedef PACKED_STRUCT zclCmdRSSI_RSSIping_tag 
{
 LocationType_t   LocationType;
}zclCmdRSSI_RSSIping_t; 

/**********************************************************************
	Binary Input (Basic) Cluster (ZigBee Cluster Library Chapter 3.14.4)
**********************************************************************/

/*Binary Input(Basic) Attributes Sets */
#define gZclAttrBinaryInputFirstSet_c 	            0x00

#define gZclAttrIdBinaryInputActiveText_c           0x04    /* O - Active Text */
#define gZclAttrIdBinaryInputDescription_c          0x1C    /* O - Description */
#define gZclAttrIdBinaryInputInactiveText_c         0x2E    /* O - Inactive Text */
#define gZclAttrIdBinaryInputOutOfService_c         0x51    /* M - OutOfService */
#define gZclAttrIdBinaryInputPolarity_c             0x54    /* O - Polarity */
#define gZclAttrIdBinaryInputPresentValue_c         0x55    /* M - PresentValue */
#define gZclAttrIdBinaryInputReliability_c          0x67    /* O - Reliability */
#define gZclAttrIdBinaryInputStatusFlags_c          0x6F    /* M - StatusFlags  */

#define gZclAttrBinaryInputSecondSet_c              0x01

#define gZclAttrIdBinaryInputApplicationType_c      0x00    /* O - ApplicationType */


/* The comand use for this cluster are the read/write attributes */

typedef PACKED_STRUCT zclBinaryInputAttrsRAM_tag
{
#if gZclClusterOptionals_d
  zclStr16_t  activeText; 
  zclStr16_t  description; 
  zclStr16_t  inactiveText; 
  uint8_t     polarity;
  uint8_t     reliability;
  uint32_t    applicationType;
#endif
  bool_t      outOfService;
  bool_t      presentValue[zclReportableCopies_c];
  uint8_t     statusFlags[zclReportableCopies_c];
} zclBinaryInputAttrsRAM_t;

typedef PACKED_STRUCT zclBinaryInput_StatusFlags_tag
{
 unsigned   InAlarm      :1;
 unsigned   Fault        :1;
 unsigned   Overridden   :1;
 unsigned   OutOfService :1;
 unsigned   reserved     :4; 
}zclBinaryInput_StatusFlags_t;


#if gZclClusterOptionals_d
/* Reliability Attribute values */
enum{
  noFaultDetected = 0x00,
  noSensor, /*only for input cluster*/
  overRange,
  underRange,
  openLoop,
  shortedLoop,
  noOutput, /*only for input Sensor*/
  unreliableOther,
  processError,
  multiStateFault,
  configurationError 
};
#endif

/**************************************************************************
	Alpha-Secure Key Establishment Cluster (Health Care Profile Annex A.2)
***************************************************************************/
#if ( TRUE == gBigEndian_c )
#define gZclAttrASKEInformationSet_c   0x0000    /* ASKE Information attributes Set */
#else
#define gZclAttrASKEInformationSet_c   0x0000    /* ASKE Information attributes Set */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* ASKE Information Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrASKEInformationASKEversion_c              0x0000    /* M - ASKE version */
#define gZclAttrASKEInformationMaxSDnumber_c              0x0100    /* M - Max Security Domain number*/
#define gZclAttrASKEInformationSDlist_c                   0x0200    /* M - Security Domain List */
#define gZclAttrASKEInformationMaxKeyingMaterialSize_c    0x0300    /* M - Max size of a single KM instance*/
#define gZclAttrASKEInformationMaxRevocationListLenght_c  0x0400    /* O - Max number of devices in a single instance of a revocation list */
#define gZclAttrASKEInformationASKEtimeout_c              0x0500    /* M - ASKE timeout */
#define gZclAttrASKEInformationKeySegmentSizesupport_c    0x0600    /* M - List of Key segment sizes */
#else
#define gZclAttrASKEInformationASKEversion_c              0x0000    /* M - ASKE version */
#define gZclAttrASKEInformationMaxSDnumber_c              0x0001    /* M - Max Security Domain number*/
#define gZclAttrASKEInformationSDlist_c                   0x0002    /* M - Security Domain List */
#define gZclAttrASKEInformationMaxKeyingMaterialSize_c    0x0003    /* M - Max size of a single KM instance*/
#define gZclAttrASKEInformationMaxRevocationListLenght_c  0x0004    /* O - Max number of devices in a single instance of a revocation list */
#define gZclAttrASKEInformationASKEtimeout_c              0x0005    /* M - ASKE timeout */
#define gZclAttrASKEInformationKeySegmentSizesupport_c    0x0006    /* M - List of Key segment sizes */
#endif /* #if ( TRUE == gBigEndian_c ) */

#define gCurrentASKEversion 0x00


/*
typedef PACKED_STRUCT 
{
  uint32_t  AddressHigh;
  uint32_t  AddressLow;
}IEEEaddress_t;
*/

typedef PACKED_STRUCT 
{
  uint32_t  TSLKsecurityKey1;
  uint32_t  TSLKsecurityKey2;
  uint32_t  TSLKsecurityKey3;
  uint32_t  TSLKsecurityKey4;
}TSLKsecurityKey_t;

typedef PACKED_STRUCT 
{
  IEEEaddress_t      IEEEAddress;
  TSLKsecurityKey_t  TSLKsecurityKey;
}TrustServer_t;


typedef PACKED_STRUCT ASKESecurityDomainTable_tag
{ 
  uint8_t       Index;                     /*  */
  uint16_t      SDidentifier;              /*  */
  IEEEaddress_t EPID;                      /*  */
  uint8_t       FPPorder;                  /*  */
  uint8_t       Lambda;                    /*  */
  uint16_t      KeySegmentSize;            /*  */
  //¿? Variable ¿?  Irreducible;           /*  */
  uint8_t       Entropy;                   /*  */
  uint8_t       CryptoIdentityType;        /*  */
  uint16_t      CryptoIdentity16;          /*  */
  uint16_t      KeyingMaterial;            /*  */
  uint8_t       KeyingMaterialVersion;     /*  */
  uint8_t       RevocationList;            /*  */
  TrustServer_t TrustServer;               /*  */
} ASKESecurityDomainTable_tag;             
                                           
enum                                       
{
   gCryptoIndentityType_16bitAddress_c = 0x00,       /* 16-bit Address */
   gCryptoIndentityType_64bitAddress_c,              /* 64-bit Address */
   gCryptoIndentityType_16bitIdentifier_c            /* 16-bit Identifier */
                                                     /* 0x03-0xFF Reserved */
};

/* A.2.8.5 ASKE cluster Commands Received */

enum
{
   gZclCmdRxASKE_ConfigureSD_c = 0x00,    /* O - Configure SD */
   gZclCmdRxASKE_UpdateRevocationList_c,  /* O - Update Revocation List */
   gZclCmdRxASKE_RemoveSD_c,              /* O - Remove SD */
   gZclCmdRxASKE_ReadSD_c,                /* O - Read SD */
   gZclCmdRxASKE_InitiateASKE_c,          /* O - Initiate ASKE */
   gZclCmdRxASKE_ConfirmASKEkey_c,        /* O - Confirm ASKE key */
   gZclCmdRxASKE_TerminateASKE_c,         /* O - Terminate ASKE */
   gZclCmdRxASKE_GenerateAMK_c,           /* O - Generate AMK */
                                          /* 0x08-0xFF Reserved */
};


typedef PACKED_STRUCT zclCmdASKE_ConfigureSD_tag
{
 uint16_t       Options;
 uint16_t       ALKupdateTime;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 TrustServer_t  TrustServer;
 uint16_t       KeyingMaterialSize;
 uint8_t        FPPorder;
 uint8_t        Lambda;
 uint16_t       KeySegmentSize;
 uint8_t        Entropy; 
 //¿? octet string ¿?  Irreducible;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
 uint16_t       KeyingMaterialOffset;
 uint8_t        KeyingMaterialVersion;
 //¿? variable ¿?  KeyingMaterial;
}zclCmdASKE_ConfigureSD_t; 

typedef PACKED_STRUCT zclCmdASKE_UpdateRevocationList_tag
{
 uint16_t       Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 uint8_t        CryptoIdentityType;
 uint8_t        NumberOfDevices;
 //¿? variable ¿?  CryptoIndentityList;
}zclCmdASKE_UpdateRevocationList_t;

enum
{
   gOptions_ClearRevocationList_c = 0,      /* Clear Revocation List */
   gOptions_AddToRevocationList_c,          /* Clear Revocation List */
   gOptions_RemoveFromRevocationList_c,     /* Clear Revocation List */
   gOptions_SendRevocationListLength_c,     /* Send Revocation List Length */
   gOptions_SendRevocationList_c,           /* Send Revocation List */
                                            /* 5-7 Reserved */
};


typedef PACKED_STRUCT zclCmdASKE_RemoveSD_tag
{
 uint16_t       Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
}zclCmdASKE_RemoveSD_t;

typedef PACKED_STRUCT zclCmdASKE_ReadSD_tag
{
 uint16_t       Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 uint16_t       Offset;
}zclCmdASKE_ReadSD_t;

typedef PACKED_STRUCT zclCmdASKE_InitiateASKE_tag
{
 uint16_t       SDindetifier;
 uint8_t        ASKEversion;
 uint8_t        KeyingMaterialVersion;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
 uint8_t        EphemeralData[16];
}zclCmdASKE_InitiateASKE_t;

typedef PACKED_STRUCT zclCmdASKE_ConfirmASKEkey_tag
{
 uint8_t        HMAC[16];
}zclCmdASKE_ConfirmASKEkey_t;

typedef PACKED_STRUCT zclCmdASKE_TerminateASKE_tag
{
 uint8_t        Status;
}zclCmdASKE_TerminateASKE_t;

typedef PACKED_STRUCT zclCmdASKE_GenerateAMK_tag
{
 uint16_t       SDindetifier;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
}zclCmdASKE_GenerateAMK_t;

/* A.2.8.6 ASKE cluster Commands Generated */

enum
{
   gZclCmdTxASKE_ConfigureSDResponse_c = 0x00,    /* O - Configure SD Resposne */
   gZclCmdTxASKE_UpdateRevocationListResponse_c,  /* O - Update Revocation List Resposne */
   gZclCmdTxASKE_RemoveSDResponse_c,              /* O - Remove SD Resposne */
   gZclCmdTxASKE_ReadSDResponse_c,                /* O - Read SD Resposne */
   gZclCmdTxASKE_InitiateASKEResponse_c,          /* O - Initiate ASKE Resposne */
   gZclCmdTxASKE_ConfirmASKEkeyResponse_c,        /* O - Confirm ASKE key Resposne */
   gZclCmdTxASKE_TerminateASKE_c,                 /* O - Terminate ASKE */
   gZclCmdTxASKE_GenerateAMKResponse_c,           /* O - Generate AMK Resposne */
   gZclCmdTxASKE_ReportRevokedNode_c,             /* O - Report Revoked Node */
   gZclCmdTxASKE_RequestConfiguration_c,          /* M - Request Configuration */
                                                  /* 0x0A-0xFF Reserved */
};


typedef PACKED_STRUCT zclCmdASKE_ConfigureSDresponse_tag
{
 uint8_t    Status;
 uint8_t    Index;
}zclCmdASKE_ConfigureSDresponse_t;

typedef PACKED_STRUCT zclCmdASKE_UpdateRevocationListResponse_tag
{
 uint8_t        Status;
 uint8_t        Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        TotalListLength;
 uint8_t        StartIndex;
 uint8_t        CryptoIdentityType;
 uint8_t        ListCount;
 //¿? variable ¿?  CryptoIDList;
}zclCmdASKE_UpdateRevocationListResponse_t;

typedef PACKED_STRUCT zclCmdASKE_RemoveSDresponse_tag
{
 uint8_t        Status;
}zclCmdASKE_RemoveSDresponse_t;

typedef PACKED_STRUCT zclCmdASKE_ReadSDresponse_tag
{
 uint8_t        Status;
 uint8_t        Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 TrustServer_t  TrustServer;
 uint16_t       KeyingMaterialSize;
 uint8_t        FPPorder;
 uint8_t        Lambda;
 uint16_t       KeySegmentSize;
 uint8_t        Entropy;
 //¿? octet string ¿?  Irreducible;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
 uint16_t       KeyingMaterialOffset;
 uint8_t        KeyingMaterialVersion;
 //¿? variable ¿?  KeyingMaterial;
}zclCmdASKE_ReadSDresponse_t;

typedef PACKED_STRUCT zclCmdASKE_InitiateASKEresponse_tag
{
 uint8_t        ASKEversion;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
 uint8_t        EphemeralData[16];
}zclCmdASKE_InitiateASKEresponse_t;

typedef PACKED_STRUCT zclCmdASKE_ConfirmASKEkeyResponse_tag
{
 uint8_t        HMAC[16];
}zclCmdASKE_ConfirmASKEkeyResponse_t;

typedef PACKED_STRUCT zclCmdASKE_GenerateAMKresponse_tag
{
 uint8_t        Status;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
}zclCmdASKE_GenerateAMKresponse_t;

typedef PACKED_STRUCT zclCmdASKE_ReportRevokedNode_tag
{
 uint16_t        SDidentifier;
 uint8_t        CryptoIdentityType;
 uint16_t       RevokedCryptoID16;
 IEEEaddress_t  RevokedCryptoID;
}zclCmdASKE_ReportRevokedNode_t;

typedef PACKED_STRUCT zclCmdASKE_RequestConfiguration_tag
{
 IEEEaddress_t  IEEEaddress;
 uint16_t       ShortAddress;
}zclCmdASKE_RequestConfiguration_t;

enum
{
   STATUS_SUCCESS_c = 0x00,         
   STATUS_FAILURE_c,                
   STATUS_UNSUP_CLUSTER_COMMAND_c = 0x81,  
   STATUS_INVALID_FIELD_c = 0x85,          
   STATUS_UNSUPPORTED_ATTRIBUTE_c,  
   STATUS_READ_ONLY_c = 0x88,              
   STATUS_INSUFFICIENT_SPACE_c,     
   STATUS_NOT_FOUND_c = 0x8B,              
   STATUS_DEFINED_OUT_OF_BAND_c = 0x91,    
   STATUS_INCONSISTENT_c,           
   STATUS_ACTION_DENIED_c,          
   STATUS_TIMEOUT_c,                                              
};


/**************************************************************************
	Alpha-Secure Access Control Cluster (Health Care Profile Annex A.3)
***************************************************************************/
#if ( TRUE == gBigEndian_c )
#define gZclAttrASACInformationSet_c   0x0000    /* ASAC Information attributes Set */
#else
#define gZclAttrASACInformationSet_c   0x0000    /* ASAC Information attributes Set */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* ASKE Information Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrASACInformationASACversion_c                   0x0000    /* M - ASAC version */
#define gZclAttrASACInformationMaxSDnumber_c                   0x0100    /* M - Max Security Domain number*/
#define gZclAttrASACInformationMaxKeyingMaterialSize_c         0x0200    /* M - Max Keying Material Size */
#define gZclAttrASACInformationMaxPermissionsTableEntries_c    0x0300    /* M - Max Permission Table Entries */
#define gZclAttrASACInformationMaxCertificateTableEntries_c    0x0400    /* M - Max Certificate Table Entries */
#define gZclAttrASACInformationMaxRevocationListLenght_c       0x0500    /* O - Max number of devices in a single instance of a revocation list */
#define gZclAttrASACInformationASACtimeout_c                   0x0600    /* M - ASAC timeout */
#define gZclAttrASACInformationMainSDlist_c                    0x0700    /* M - Main Security Domain List */
#define gZclAttrASACInformationKeySegmentSizeSupport_c         0x0800    /* M - Key Segment Size Support*/

#else
#define gZclAttrASACInformationASACversion_c                   0x0000    /* M - ASAC version */
#define gZclAttrASACInformationMaxSDnumber_c                   0x0001    /* M - Max Security Domain number*/
#define gZclAttrASACInformationMaxKeyingMaterialSize_c         0x0002    /* M - Max Keying Material Size */
#define gZclAttrASACInformationMaxPermissionsTableEntries_c    0x0003    /* M - Max Permission Table Entries */
#define gZclAttrASACInformationMaxCertificateTableEntries_c    0x0004    /* M - Max Certificate Table Entries */
#define gZclAttrASACInformationMaxRevocationListLenght_c       0x0005    /* O - Max number of devices in a single instance of a revocation list */
#define gZclAttrASACInformationASACtimeout_c                   0x0006    /* M - ASAC timeout */
#define gZclAttrASACInformationMainSDlist_c                    0x0007    /* M - Main Security Domain List */
#define gZclAttrASACInformationKeySegmentSizeSupport_c         0x0008    /* M - Key Segment Size Support*/
#endif /* #if ( TRUE == gBigEndian_c ) */

#define gCurrentASACversion 0x00

typedef PACKED_STRUCT 
{
  uint8_t  PropertyID;
  uint8_t  PropertyType;
  //-----  PropertyValue;   // this value depends on the property type (see following enumeration)
}LightWeightDigCert_t;

enum                                       
{
   gPropertyType_IEEEaddress_c = 0x00, /* IEEE address */
   gPropertyType_ProfileID_c,          /* Profile ID = 16-bit */
   gPropertyType_DeviceID_c,           /* Device ID = 16-bit */
   gPropertyType_ServerClusters_c,     /* Server Clusters = Set of cluster ID */
   gPropertyType_ClientClusters_c,     /* Client Clusters = Set of cluster ID */
   gPropertyType_UniqueNumber_c,       /* Unique Number = 128-bit */
                                       /* 0x06-0x40 Reserved Generic */
                                       /* 0x40-0x80 Reserved Application Profile */
                                       /* 0x80-0xFF Reserved Manufacturer */
};


typedef PACKED_STRUCT 
{
  uint32_t  LDCfingerprint1;
  uint32_t  LDCfingerprint2;
  uint32_t  LDCfingerprint3;
  uint32_t  LDCfingerprint4;
}LCDfingerprint_t;

typedef PACKED_STRUCT 
{
  unsigned  DefaultPermission : 1;
  unsigned  Reserved          : 7;
}DefaultPermission_t;

typedef PACKED_STRUCT ASACSecurityDomainTable_tag
{ 
  uint8_t                Index;                      /* M - Number of the ASAC SD table entry */
  uint16_t               SDidentifier;               /* M - Unique identifier of the current SD */
  uint8_t                SDpointer;                  /* M - Pointer to the position of the SD in MASKE */
  uint8_t                SDdescriptor;               /* O - Descriptor of the features associated with teh current SD */
  uint16_t               MainSD;                     /* O - Main SD */
  IEEEaddress_t          EPID;                       /* O - EPID */
  LightWeightDigCert_t   LightWeightDigCertificate;  /* O - Light Weight Digital Certificate  */
  LCDfingerprint_t       LDCfingerprint;             /* O - LDC fingerprint */
  uint8_t                ACcryptoIdentitySize;       /* M - AC Crypto Identity Size */
  uint8_t                ACcryptoIdentity;           /* O - AC crypto Identity */ 
  uint8_t                SegmentNumber;              /* M - Segment Number */
  uint8_t                FPPorder;                   /* M - FPP order */
  uint8_t                Lambda;                     /* M - Lambda */
  uint16_t               KeySegmentSize;             /* M - Key segment size */
  uint8_t                Irreducible;                /* O - Irreducible */
  uint16_t               KeyingMaterial;             /* M - Keying Material */
  uint8_t                KeyingMaterialVersion;      /* M - Keying Material version */
  uint8_t                RevocationListSize;         /* O - Revocation List Size */
  uint8_t                RevocationList;             /* O - Revocation List */
  DefaultPermission_t    DefaultPermission;           /* O - Default Permission*/
  TrustServer_t          TrustServer;                /* O - Trust Server */
} ASACSecurityDomainTable_tag;

typedef PACKED_STRUCT 
{
  unsigned  ServerSideCommand           : 1;
  unsigned  ClientSideCommand           : 1;
  unsigned  ZCLCommand                  : 1;
  unsigned  ManufacturerSpecificCommand : 1;
  unsigned  GenericCommand              : 1;
  unsigned  ClusterSpecificCommand      : 1;
  unsigned  Reserved                    : 2;
}SpecificSel_t;

typedef PACKED_STRUCT 
{
  uint8_t        Eightbitmap;
  SpecificSel_t  SpecificSelector;
}Command_t;

typedef PACKED_STRUCT 
{
  unsigned  Reserved              : 4;
  unsigned  NumberOfLDCproperties : 4;
  unsigned  NumberOfSDs           : 4;
  unsigned  NumberOfIEEE          : 4;
}Options_t;

typedef PACKED_STRUCT ASACPermissionsTable_tag
{ 
  uint16_t               Profile;                /* M - Profile */
  uint8_t                EndPoint;               /* M - EndPoint */
  uint16_t               Cluster;                /* M - Cluster */
  uint16_t               Attribute;              /* M - Attribute */
  Command_t              Command;                /* M - Command */
  uint16_t               AssociatedMainSD;       /* M - Associated Main SD */
  Options_t              Options;                /* M - Options */
   //-----               RequiredLDCproperties;  /* O - Required LDC properties */
   //-----                RequiredSubSDs;        /* O - Required Sub SDs */
   //-----                RequiredIEEE;          /* O - Required IEEE */

} ASACPermissionsTable_t;


/* A.3.7.6 ASAC cluster Commands Received */

enum
{
   gZclCmdRxASAC_ConfigureSD_c = 0x00,    /* O - Configure SD */
   gZclCmdRxASAC_UpdateRevocationList_c,  /* O - Update Revocation List */
   gZclCmdRxASAC_ConfigureACpolicies_c,   /* O - Configure AC Policies */
   gZclCmdRxASAC_ReadSD_c,                /* O - Read SD */
   gZclCmdRxASAC_ReadACpolicies_c,        /* O - Read AC policies */
   gZclCmdRxASAC_RemoveAC_c,              /* O - Remove AC */
   gZclCmdRxASAC_ACpropertiesReq_c,       /* O - AC properties request */
   gZclCmdRxASAC_TSreportResponse_c,      /* O - TS report response */
   gZclCmdRxASAC_InitiateASAC_c,          /* O - Initiate ASAC */
   gZclCmdRxASAC_ConfirmASACkey_c,        /* O - Confirm ASAC key */
   gZclCmdRxASAC_TerminateASAC_c,         /* O - Terminate ASAC */
   gZclCmdRxASAC_GenerateAMK_c,           /* O - Generate AMK */
   gZclCmdRxASAC_LDCtransport_c,          /* O - LDC transport */
                                          /*     0x0D-0xFF Reserved */
};

/* Configure SD command Payload */


typedef PACKED_STRUCT 
{
 unsigned ALKUpdateSchedule    : 3;
 unsigned SDparametersPresent  : 1;
 unsigned EPIDpresent          : 1;
 unsigned IndexPresent         : 1;
 unsigned TrustServerPresent   : 1;
 unsigned NewKMstructure       : 1;
 unsigned ACcryptoIDpresent    : 1;
 unsigned PartialKM            : 1;
 unsigned CompleteKM           : 1;
 unsigned LDCpresent           : 1;
 unsigned Reserved             : 4;
}ConfigSD_Options_t;

typedef PACKED_STRUCT zclCmdASAC_ConfigureSD_tag
{
 ConfigSD_Options_t       Options;
 uint16_t       ALKupdateTime;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 uint8_t        SDpointer;
 uint8_t        SDdescriptor;
 uint16_t       MainSDID;
 TrustServer_t  TrustServer;
 DefaultPermission_t DefaultPermission;
 uint16_t       KeyingMaterialSize;
 uint8_t        FPPorder;
 uint8_t        Lambda;
 uint8_t        SegmentNumber; 
 uint16_t       KeySegmentSize; 
 //¿? octet string ¿?  Irreducible;
 uint8_t        ACIdentitySize;
 uint16_t       ACcryptoID;
 uint16_t       KeyingMaterialOffset;
 uint8_t        KeyingMaterialVersion;
 //¿? variable ¿?  KeyingMaterial;
 //------       LDC;
}zclCmdASAC_ConfigureSD_t;

enum                                       
{
   gALKupdate_InmmediateUpdate_c = 0,    /* Inmmediate Update all ALK from previous KM */
   gALKupdate_UpdateNextComm_c,          /* Update all ALK from previous KM on next communication */
   gALKupdate_UpdateAfterTimeElapse_c,   /* Update all ALK from previous KM after a Time elapsed*/
   gALKupdate_NoUpdate_c,                /* Do not Update all ALK until explicity instructed */
                                         /* 4-7 Reserved Manufacturer */
};

/* Update Revocation List command Payload */


typedef PACKED_STRUCT 
{
 unsigned Action        : 3;
 unsigned EPIDpresent   : 1;
 unsigned Reserved      : 4;
        
}RevListCommand_Options_t;

typedef PACKED_STRUCT zclCmdASAC_UpdateRevocationList_tag
{
 RevListCommand_Options_t    Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        ListIndex;
 uint8_t        NumberOfDevices;
 //---          ACcryptoIDList;
}zclCmdASAC_UpdateRevocationList_t;

/* Configure AC Policies command Payload */

typedef PACKED_STRUCT 
{
 unsigned ProfileIDpresent   : 1;
 unsigned Endpointpresent    : 1;
 unsigned CommandPresent     : 1;
 unsigned AttributePresent   : 1;
 unsigned Reserved           : 4;
        
}ACpolicies_Options_t;

typedef PACKED_STRUCT 
{
 unsigned Reserved              : 4;
 unsigned NumberOfLDCproperties : 4;
 unsigned NumberOfSubSDs        : 4;
 unsigned NumberOfIEEE          : 4;
        
}ACpolicies_ACoptions_t;


typedef PACKED_STRUCT zclCmdASAC_ConfigureACpolicies_tag
{
 ACpolicies_Options_t    Options;
 uint16_t       Profile;
 uint8_t        Endpoint;
 uint16_t       Cluster;
 ACpolicies_ACoptions_t  ACoptions;
 uint16_t       Command;
 uint16_t       Attribute;
 uint16_t       MainSDID;
 //-----        RequiredLDCproperties;  /* O - Required LDC properties */
 //-----        RequiredSubSDs;        /* O - Required Sub SDs */
 //-----        RequiredIEEE;          /* O - Required IEEE */
}zclCmdASAC_ConfigureACpolicies_t;

/* Read SD command Payload */

typedef PACKED_STRUCT 
{

 unsigned SDidentifierPresent : 1;
 unsigned EPIDpresent         : 1;
 unsigned IndexPresent        : 1;
 unsigned ReadTS              : 1;
 unsigned ReadMainSDKMstruct  : 1;
 unsigned ReadMainSDACcryptoID: 1;
 unsigned ReadMainSDKM        : 1;
 unsigned bit7                : 1;   //missing description in HC profile for this bit (figure 57, A.3.7.6.1.4.1)
 unsigned ReadSubSDKMstruct   : 1;
 unsigned ReadSubSDACcryptoID : 1;
 unsigned ReadSubSDKM         : 1;
 unsigned Reserved            : 5;        
}ReadSD_Options_t;


typedef PACKED_STRUCT zclCmdASAC_ReadSD_tag
{
 ReadSD_Options_t  Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 uint16_t       MainSDoffset;
 uint16_t       SubSDoffset;
}zclCmdASAC_ReadSD_t;

/* Read AC Policies command Payload */

typedef PACKED_STRUCT 
{

 unsigned MainSDID          : 1;
 unsigned ProfilePresent    : 1;
 unsigned ClusterPresent    : 1;
 unsigned CommandPresent    : 1;
 unsigned AttributePresent  : 1;
 unsigned Reserved          : 3;        
}ReadACpolicies_Options_t;


typedef PACKED_STRUCT zclCmdASAC_ReadACpolicies_tag
{
 ReadACpolicies_Options_t  Options;
 uint16_t       MainSDID;
 uint16_t       Profile;
 uint16_t       Cluster;
 uint16_t       Command;
 uint16_t       Attribute;
}zclCmdASAC_ReadACpolicies_t;

/* Remove AC command Payload */

typedef PACKED_STRUCT 
{

 unsigned RemoveSD      : 1;
 unsigned RemoveSubSDs  : 1;
 unsigned RemovePolicy  : 1;
 unsigned IndexPresent  : 1;
 unsigned SDidentifier  : 1;
 unsigned EPIDpresent   : 1; 
 unsigned Reserved      : 2; 
}RemoveAC_Options_t;

typedef PACKED_STRUCT zclCmdASAC_RemoveAC_tag
{
 RemoveAC_Options_t  Options;
 uint8_t       Index;
 uint16_t       SDidentifier;
 IEEEaddress_t  EPID;
}zclCmdASAC_RemoveAC_t;

/* AC properties command Payload */

// This command has no payload


/* TS Report Response command Payload */

// This command has no payload

/* Initiate ASAC command Payload */

typedef PACKED_STRUCT 
{

 unsigned LDCfingerprintPresent      : 1; 
 unsigned Reserved                   : 7; 
}InitiateASAC_Options_t;

typedef PACKED_STRUCT zclCmdASAC_InitiateASAC_tag
{
 InitiateASAC_Options_t  Options;
 uint16_t       MainSDidentifier;
 uint8_t       ASACversion;
 //-----  InitiatorACcryptoID;
 uint8_t  InitiatorLDCfingerprint[16];
 uint8_t  InitiatorEphemeralData[16];
 uint8_t  NumberOfSubSDpointers;
 //-----  SubSDpointers;
}zclCmdASAC_InitiateASAC_t;

/* Confirm ASAC command Payload */
typedef PACKED_STRUCT zclCmdASAC_ConfirmASAC_tag
{
 uint8_t HMAC[16];
}zclCmdASAC_ConfirmASAC_t;

/* Terminate ASAC command Payload */
typedef PACKED_STRUCT zclCmdASAC_TerminateASAC_tag
{
 uint8_t Status;
}zclCmdASAC_TerminateASAC_t;


enum                                       
{
   gConfirmASACstatus_Success_c = 0x00,          /* Success */
   gConfirmASACstatus_Failure_c,                 /* Failure */
   gConfirmASACstatus_UnsupClusterCommand_c,     /* Unsup Cluster Command */
   gConfirmASACstatus_InvalidField_c,            /* Invalid Field */
   gConfirmASACstatus_UnsupportedAttribute_c,    /* Unsupported Attribute */
   gConfirmASACstatus_ReadOnly_c,                /* Read Only */ 
   gConfirmASACstatus_InsufficientSpace_c,       /* Insufficient Space */
   gConfirmASACstatus_NotFound_c,                /* Not Found */
   gConfirmASACstatus_DefinedOutOfBand_c,        /* Defined out of Band */
   gConfirmASACstatus_Inconsistent_c,            /* Inconsistent */
   gConfirmASACstatus_ActionDenied_c,            /* Action Denied */
   gConfirmASACstatus_Timeout_c,                 /* Timeout */
};

/* Generate AMK command Payload */


typedef PACKED_STRUCT 
{

 unsigned LDCfingerprintPresent      : 1; 
 unsigned Reserved                   : 7; 
}GenerateAMK_Options_t;

typedef PACKED_STRUCT zclCmdASAC_GenerateAMK_tag
{
 GenerateAMK_Options_t        Options;
 uint16_t       MainSDindetifier;
 uint8_t        ASACversion;
 //------       InitiatorACcryptoID;
 uint8_t        LDCfingerprint[16];
 uint8_t        NumberOfSubSDsPointers;
 //------       SubSDpointers;
}zclCmdASAC_GenerateAMK_t;

/* LDC Transport command Payload */

typedef PACKED_STRUCT 
{

 unsigned OwnACcryptoPresent : 1;
 unsigned OwnLDCpresent      : 1;
 unsigned RequestACcryptoID  : 1;
 unsigned RequestLDC         : 1; 
 unsigned Reserved           : 4; 
}LDCtransport_Options_t;

typedef PACKED_STRUCT zclCmdASAC_LDCtransport_tag
{
 LDCtransport_Options_t  Options;
 uint16_t       MainSD;
 //------        ACcryptoID;
 //------       LDC;
}zclCmdASAC_LDCtransport_t;


/* A.3.7.7 ASAC cluster Commands Generated */

enum
{
   gZclCmdTxASAC_ConfigureSDresponse_c = 0x00,    /* O - Configure SD response */
   gZclCmdTxASAC_UpdateRevocationListResponse_c,  /* O - Update Revocation List Response */
   gZclCmdTxASAC_ConfigureACpoliciesResponse_c,   /* O - Configure AC Policies Response */
   gZclCmdTxASAC_ReadSDresponse_c,                /* O - Read SD Response */
   gZclCmdTxASAC_ReadACpoliciesResponse_c,        /* O - Read AC policies Response */
   gZclCmdTxASAC_RemoveACresponse_c,              /* O - Remove AC Response */
   gZclCmdTxASAC_ACpropertiesResponse_c,          /* O - AC properties Response */
   gZclCmdTxASAC_TSreport_c,                      /* O - TS report */
   gZclCmdTxASAC_InitiateASACresponse_c,          /* O - Initiate ASAC Response */
   gZclCmdTxASAC_ConfirmASACkeyResponse_c,        /* O - Confirm ASAC key Response */
   gZclCmdTxASAC_TerminateASAC_c,                 /* O - Terminate ASAC */
   gZclCmdTxASAC_GenerateAMKresponse_c,           /* O - Generate AMK Response */
   gZclCmdTxASAC_LDCtransportResponse_c,          /* O - LDC transport Response */
   gZclCmdTxASAC_RequestConfiguration_c,          /* O - Request Configuration */
                                                  /*     0x0E-0xFF Reserved */
};

/* Configure SD response command Payload */


typedef PACKED_STRUCT zclCmdASAC_ConfigureSDresponse_tag
{
 uint8_t       Status;
 uint8_t       Index;
}zclCmdASAC_ConfigureSDresponse_t;

/* Update Revocation List response command Payload */

typedef PACKED_STRUCT zclCmdASAC_UpdateRevocationListResponse_tag
{
 uint8_t        Status;
 uint8_t        Options;
 uint16_t       MainSDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        TotalListLenght;
 uint8_t        CryptoIdentityType;
 uint8_t        ListCount;
 //---          ACcryptoIDList;
}zclCmdASAC_UpdateRevocationListResponse_t;

/* Configure AC Policies Response command Payload */
typedef PACKED_STRUCT zclCmdASAC_ConfigureACpoliciesResponse_tag
{
 uint8_t        Status;
}zclCmdASAC_ConfigureACpoliciesResponse_t;

/* Read SD Response command Payload */

typedef PACKED_STRUCT 
{
  unsigned  SDIDpresent              : 1;
  unsigned  EPIDpresent              : 1;
  unsigned  IndexPresent             : 1;
  unsigned  SDdescriptorPresent      : 1;
  unsigned  MainSDidentifierPresent  : 1;
  unsigned  TSpresent                : 1;
  unsigned  KMstructPresent          : 1;
  unsigned  ACcryptoIDpresent        : 1;
  unsigned  KMpresent                : 1;
  unsigned  Continuation             : 1;
  unsigned  Reserved                 : 6;

}ReadSDresponse_Options_t;

typedef PACKED_STRUCT zclCmdASAC_ReadSDresponse_tag
{
 uint8_t    Status;
 ReadSDresponse_Options_t  Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 //------   SDdescriptor;  
 uint16_t       MainSDidentifier;
 TrustServer_t  TrustServer;
 DefaultPermission_t DefaultPermission;
 uint16_t       KeyingMaterialSize;
 uint8_t        FPPorder;
 uint8_t        Lambda;
 uint8_t        SegmentNumber; 
 uint16_t       KeySegmentSize; 
 //¿? octet string ¿?  Irreducible;
 uint16_t       ACcryptoID;
 uint16_t       KeyingMaterialOffset;
 uint8_t        KeyingMaterialVersion;
 //¿? variable ¿?  KeyingMaterial;
}zclCmdASAC_ReadSDresponse_t;

/* Read AC policies Response command Payload */
typedef PACKED_STRUCT 
{
  unsigned  ProfileIDpresent    : 1;
  unsigned  EndpointPresent     : 1;
  unsigned  CommandPresent      : 1;
  unsigned  AttributeIDPresent  : 1;
  unsigned  MultipleResponses   : 1;
  unsigned  Reserved            : 3;
}ReadACpoliciesResponse_Options_t;

typedef PACKED_STRUCT zclCmdASAC_ReadACpoliciesResponse_tag
{
 uint8_t    Status;
 ReadACpoliciesResponse_Options_t  Options;
 uint16_t   Profile;
 uint8_t    Endpoint;
 uint16_t   Cluster;
 uint16_t   ACoptions;
 uint16_t   Command;
 uint16_t   Attribute;
 uint16_t       MainSDID;
 //-----    RequiredLDCproperties;
 //-----    Required SubSDs;
 //-----    Required IEEE;
}zclCmdASAC_ReadACpoliciesResponse_t;

/* Remove AC Response command Payload */

typedef PACKED_STRUCT 
{
  unsigned  ProfileIDpresent    : 1;
  unsigned  EndpointPresent     : 1;
  unsigned  CommandPresent      : 1;
  unsigned  AttributeIDPresent  : 1;
  unsigned  MultipleResponses   : 1;
  unsigned  Reserved            : 3;
}RemoveACresponse_Options_t;

typedef PACKED_STRUCT zclCmdASAC_RemoveACresponse_tag
{
 uint8_t    Status;
 ReadACpoliciesResponse_Options_t  Options;
 uint16_t   Profile;
 uint8_t    Endpoint;
 uint16_t   Cluster;
 uint16_t   ACoptions;
 uint16_t   Command;
 uint16_t   Attribute;
 uint16_t   MainSDID;
 //-----    RequiredLDCproperties;
 //-----    Required SubSDs;
 //-----    Required IEEE;
}zclCmdASAC_RemoveACresponse_t;

/* AC properties Response command Payload */

typedef PACKED_STRUCT 
{
  unsigned  ListComplete    : 1;
  unsigned  Reserved        : 7;
}ACPropertiesResponse_Options_t;

typedef PACKED_STRUCT zclCmdASAC_ACpropertiesResponse_tag
{
 uint8_t    Status;
 ACPropertiesResponse_Options_t  Options;
 //-----    PropertiesList;
}zclCmdASAC_ACpropertiesResponse_t;

/* TS report command Payload */

typedef PACKED_STRUCT 
{
  unsigned  EPIDpresent    : 1;
  unsigned  LDC1present    : 1;
  unsigned  IEEE1present    : 1;
  unsigned  LDC2present    : 1;
  unsigned  IEEE2present    : 1;
  unsigned  Reserved       : 3;
}TSreport_Options_t;

typedef PACKED_STRUCT zclCmdASAC_TSreport_tag
{
 uint8_t    Status;
 uint8_t    Options;
 uint16_t   MainSDidentifier;
 IEEEaddress_t  EPID;
 //-----    ACcryptoID1;
 //---   LDC1; 
 IEEEaddress_t  IEEEaddress1;
 //-----    ACcryptoID2;
 //---   LDC2; 
 IEEEaddress_t  IEEEaddress2;
 //-----  MissingSubSDs;
}zclCmdASAC_TSreport_t;

enum                                       
{
   gTSreportStatus_RevokedNode_c = 0x00,    /* Revoked Node */
   gTSreportStatus_MisConfiguration_c,      /* MisConfiguration */
   gTSreportStatus_BogusTerminate_c,        /* Bogus Terminate */
   gTSreportStatus_IDconflict_c,            /* ID conflict */
   gTSreportStatus_FailedExchange_c,        /* Failed Exchange */  
};


/* Initiate ASAC Response command Payload */

typedef PACKED_STRUCT 
{

 unsigned LDCfingerprintPresent      : 1; 
 unsigned SubSDpointersPresent       : 1;
 unsigned Reserved                   : 6; 
}InitASACresponse_Options_t;

typedef PACKED_STRUCT zclCmdASAC_InitiateASACresponse_tag
{
 InitASACresponse_Options_t  Options;
 //-----  ResponderACcryptoID;
 uint8_t  ResponderLDCfingerprint[16];
 uint8_t  ResponderEphemeralData[16];
 uint8_t  NumberOfSubSDpointers;
 //-----  SubSDpointers;
}zclCmdASAC_InitiateASACresponse_t;

/* Confirm ASAC key Response command Payload */

typedef PACKED_STRUCT zclCmdASAC_ConfirmASACkeyResponse_tag
{
 uint8_t HMAC[16];
}zclCmdASAC_ConfirmASACkeyResponse_t;

/* Terminate ASAC command Payload */




/* Generate AMK Response command Payload */

typedef PACKED_STRUCT 
{

 unsigned LDCfingerprintPresent      : 1; 
 unsigned Reserved                   : 7; 
}GenAMKresponse_Options_t;

typedef PACKED_STRUCT zclCmdASAC_GenerateAMKresponse_tag
{
 uint8_t Status;
 GenAMKresponse_Options_t Options;
 //----  ResponderADcryptoID;
 uint8_t ResponderLDCfingerprint[16];
 uint8_t NumberOfSubSDpointers;
 //----  SubSDpointers;
}zclCmdASAC_GenerateAMKresponse_t;

/* LDC transport Response command Payload */

typedef PACKED_STRUCT 
{

 unsigned OwnACcryptoIDpresent   : 1; 
 unsigned OwnLDCpresent          : 1; 
 unsigned Reserved               : 6; 
}LDCtransportResponse_Options_t;

typedef PACKED_STRUCT zclCmdASAC_LDCtransportResponse_tag
{
 LDCtransportResponse_Options_t  Options;
 //----  ADcryptoID;
 //----  LDC;
}zclCmdASAC_LDCtransportResponse_t;

/* Request Configuration command Payload */

typedef PACKED_STRUCT zclCmdASAC_RequestConfiguration_tag
{
 IEEEaddress_t  IEEEaddress;
 uint16_t       ShortAddress;
}zclCmdASAC_RequestConfiguration_t;


typedef PACKED_STRUCT zclSetAttrValue_tag
{
  uint8_t         ep;             /* endpoint*/
  zbClusterId_t   clusterId;      /* cluster ID */
  uint16_t        attrID;         /* attribut ID */
  uint8_t         attrSize;       /* attribut Size */
  uint8_t         direction;        /* client or server attribute*/     
  uint8_t         valueAttr[1];      /* value */
} zclSetAttrValue_t;

typedef PACKED_STRUCT zclProcessUnsolicitedCommand_tag
{
  uint8_t         endpoint;       /* endpoint */
  zbClusterId_t   clusterId;      /* cluster ID */
  uint8_t         commandId;      /* command ID */
  bool_t          state;           /* enable or disable unsolicited command */
  uint32_t        data;           /* some data - e.g. ApplianceStatistics cluster => data = logId */
}zclProcessUnsolicitedCommand_t;

#if gZclEnablePollControlCluster_d
/******************************************
	Poll Control Cluster
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.5 [R1]
*******************************************/

/* Poll Control Server Attributes ([R1] - Table 9.9) */

#if ( TRUE == gBigEndian_c )
#define gZclAttrPollControl_CheckInInterval_c               0x0000   /* M - CheckInInterval */
#define gZclAttrPollControl_LongPollInterval_c              0x0100   /* M - LongPollInterval */
#define gZclAttrPollControl_ShortPollInterval_c             0x0200   /* M - ShortPollInterval */
#define gZclAttrPollControl_FastPollTimeout_c               0x0300   /* M - FastPollTimeout */
#if gZclClusterOptionals_d
#define gZclAttrPollControl_CheckInIntervalMin_c            0x0400   /* O - CheckInInterval Min */
#define gZclAttrPollControl_LongPollIntervalMin_c           0x0500   /* O - LongPollInterval Min*/
#define gZclAttrPollControl_FastPollTimeoutMax_c            0x0600   /* O - FastPollTimeout Max*/
#endif //gZclClusterOptionals_d
#else
#define gZclAttrPollControl_CheckInInterval_c               0x0000   /* M - CheckInInterval */
#define gZclAttrPollControl_LongPollInterval_c              0x0001   /* M - LongPollInterval */
#define gZclAttrPollControl_ShortPollInterval_c             0x0002   /* M - ShortPollInterval */
#define gZclAttrPollControl_FastPollTimeout_c               0x0003   /* M - FastPollTimeout */
#if gZclClusterOptionals_d
#define gZclAttrPollControl_CheckInIntervalMin_c            0x0004   /* O - CheckInInterval Min */
#define gZclAttrPollControl_LongPollIntervalMin_c           0x0005   /* O - LongPollInterval Min*/
#define gZclAttrPollControl_FastPollTimeoutMax_c            0x0006   /* O - FastPollTimeout Max*/
#endif //gZclClusterOptionals_d
#endif 

#define gZclAttrSetPollControl_c                        0x00
#define gZclAttrIdPollControl_CheckInInterval_c         0x00   /* M - CheckInInterval */
#define gZclAttrIdPollControl_LongPollInterval_c        0x01   /* M - LongPollInterval */
#define gZclAttrIdPollControl_ShortPollInterval_c       0x02   /* M - ShortPollInterval */
#define gZclAttrIdPollControl_FastPollTimeout_c         0x03   /* M - FastPollTimeout */
#if gZclClusterOptionals_d
#define gZclAttrIdPollControl_CheckInIntervalMin_c      0x04   /* O - CheckInInterval Min */
#define gZclAttrIdPollControl_LongPollIntervalMin_c     0x05   /* O - LongPollInterval Min*/
#define gZclAttrIdPollControl_FastPollTimeoutMax_c      0x06   /* O - FastPollTimeout Max*/
#endif //gZclClusterOptionals_d



/* [R1] 9.5.4.3 Commands Generated by the Poll Control Server */
#define gZclCmdPollControl_CheckIn_c                      0x00    /* M - Check In */    
/* [R1] 9.11.2.4 Commands Generated by the Poll Control Client */
#define gZclCmdPollControl_CheckInRsp_c                   0x00    /* M - Check In Response */    
#define gZclCmdPollControl_FastPollStop_c                 0x01    /* M - Fast Poll Stop */    
#define gZclCmdPollControl_SetLongPollInterval_c          0x02    /* O - Set Long Poll Interval */    
#define gZclCmdPollControl_SetShortPollInterval_c         0x03    /* O - Set Short Poll Interval */  

#if ( TRUE == gBigEndian_c )
#define gZclPollControl_FastPollTimeoutClient_Test_c	    0x7800    /* 120 quartesSeconds*/

#define gZclPollControl_CheckInInterval_Default_c           0x40380000    /* 1h */
#define gZclPollControl_CheckInInterval_Test_c              0x68010000    /* 360 quarterSeconds */
#define gZclPollControl_LongPollInterval_Test_c             0x0C000000    /* 12 quarterSeconds*/
#define gZclPollControl_CheckInIntervalMin_Test_c           0xB4000000    /* 180 quarterSeconds */
#define gZclPollControl_LongPollIntervalMin_Test_c	    0x0C000000
#define gZclPollControl_FastPollTimeoutMax_Test_c           0xF000    /* 240 quarterSeconds */
#define gZclPollControl_FastPollTimeout_Test_c              0x2800    /* 40 quarterSeconds */
#define gZclPollControl_ShortPollInterval_Test_c            0x0200    /* 2 quarterSeconds */
#else
#define gZclPollControl_FastPollTimeoutClient_Test_c	    0x0078	  /* 120 quarterSeconds */

#define gZclPollControl_CheckInInterval_Default_c           0x00003840    /* 1h */
#define gZclPollControl_CheckInInterval_Test_c              0x00000168    /* 360 quarterSeconds */
#define gZclPollControl_LongPollInterval_Test_c 	    0x0000000C    /* 12 quarterSeconds*/
#define gZclPollControl_CheckInIntervalMin_Test_c           0x000000B4    /* 180 quarterSeconds */
#define gZclPollControl_LongPollIntervalMin_Test_c	    0x0000000C
#define gZclPollControl_FastPollTimeoutMax_Test_c           0x00F0    /* 240 quarterSeconds */
#define gZclPollControl_FastPollTimeout_Test_c              0x0028    /* 40 quarterSeconds */
#define gZclPollControl_ShortPollInterval_Test_c            0x0002    /* 2 quarterSeconds */
#endif

	
#define gZclPollControl_MaxCheckInInterval_c                0x006E0000                   
#define gZclPollControl_MaxLongPollInterval_c               0x006E0000
#define gZclPollControl_MinLongPollInterval_c               0x04
#define gZclPollControl_MinShortPollInterval_c              0x01
#define gZclPollControl_CheckInRspMaxWaitDuraration_c       0x07

#define gNoOfPollControlServerInstances_d                   0x01

typedef PACKED_STRUCT zclPollControlAttrsRAM_tag
{
  uint32_t    checkInInterval;
  uint32_t    longPollInterval;
  uint16_t    shortPollInterval;
  uint16_t    fastPollTimeout;
#if gZclClusterOptionals_d  
  uint32_t    checkInIntervalMin;
  uint32_t    longPollIntervalMin;
  uint16_t    fastPollTimeoutMax;
#endif  
}zclPollControlAttrsRAM_t;

enum
{
  gPollControl_CheckInRspIdle_c,
  gPollControl_CheckInRspStart_c,
  gPollControl_CheckInRspTimeout_c
};

typedef PACKED_STRUCT zclPollControlClientInf_tag
{
  bool_t      enableFastPooling;
  uint16_t    fastPollTimeout;
}zclPollControlClientSetup_t;

typedef PACKED_STRUCT zclPollControlServerInf_tag
{
  bool_t        enableFastPool;
  uint16_t      fastPollTimeout;
  zbNwkAddr_t   aNwkAddr;
}zclPollControlServerInf_t;

#if gEndDevCapability_d || gComboDeviceCapability_d 
typedef PACKED_STRUCT zclPollControlServerSetup_tag
{ 
  uint8_t                       endpoint; 
  zbTmrTimerID_t                  checkInIntervalTmr; 
  zbTmrTimerID_t                  fastPollTimeoutTmr; 
  uint8_t                       bindingTableEntries;
  uint8_t                       curentIndex;
  uint8_t                       indexCheckInRsp;
  uint8_t                       checkInRspState;
  zclPollControlServerInf_t     currentPollCtrList[gMaximumApsBindingTableEntries_c];
}zclPollControlServerSetup_t;
#endif  

/* [R1] 9.5.4.4 Check - In:  this command don't have payload */
typedef PACKED_STRUCT zclPollControl_CheckIn_tag 
{
   afAddrInfo_t                               addrInfo;
   uint8_t                                    zclTransactionId;
}zclPollControl_CheckIn_t; 

/* payload format for the checkInResponse command */
typedef PACKED_STRUCT zclCmdPollControl_CheckInRsp_tag
{
  bool_t    startFastPolling;
  uint16_t  fastPollTimeout;
}zclCmdPollControl_CheckInRsp_t;

/* [R1] 9.5.5.3 Check - In Response */
typedef PACKED_STRUCT zclPollControl_CheckInRsp_tag 
{
   afAddrInfo_t                               addrInfo;
   uint8_t                                    zclTransactionId;
   zclCmdPollControl_CheckInRsp_t             cmdFrame;
}zclPollControl_CheckInRsp_t; 

/* [R1] 9.5.5.4 Fast Poll Stop:  this command don't have payload */
typedef PACKED_STRUCT zclPollControl_FastPollStop_tag 
{
   afAddrInfo_t                               addrInfo;
   uint8_t                                    zclTransactionId;
}zclPollControl_FastPollStop_t; 

/* payload format for the Set Long Poll Interval command */
typedef PACKED_STRUCT zclCmdPollControl_SetLongPollInterval_tag
{
  uint32_t newLongPollInterval;
}zclCmdPollControl_SetLongPollInterval_t;

/* [R1] 9.5.5.5 Set Long Poll Interval */
typedef PACKED_STRUCT zclPollControl_SetLongPollInterval_tag 
{
   afAddrInfo_t                               addrInfo;
   uint8_t                                    zclTransactionId;
   zclCmdPollControl_SetLongPollInterval_t    cmdFrame;
}zclPollControl_SetLongPollInterval_t; 


/* payload format for the Set Short Poll Interval command */
typedef PACKED_STRUCT zclCmdPollControl_SetShortPollInterval_tag
{
  uint16_t newShortPollInterval;
}zclCmdPollControl_SetShortPollInterval_t;

/* [R1] 9.5.5.5 Set Short Poll Interval */
typedef PACKED_STRUCT zclPollControl_SetShortPollInterval_tag 
{
   afAddrInfo_t                               addrInfo;
   uint8_t                                    zclTransactionId;
   zclCmdPollControl_SetShortPollInterval_t   cmdFrame;
}zclPollControl_SetShortPollInterval_t; 



#endif /* gZclEnablePollControlCluster_d */


#if gInterPanCommunicationEnabled_c
extern pfnInterPanIndication_t pfnInterPanServerInd;
extern pfnInterPanIndication_t pfnInterPanClientInd;
#endif

/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/

/* ask the other node to reset the endpoint */
/*zclStatus_t ZCL_BasicResetReq(zclBasicResetReq_t *pReq);*/
#define ZCL_BasicResetReq(pReq)

/* recall scene data for light/switch, etc... */
/*!
 * @fn 		void HA_RecallScene(zbEndPoint_t endPoint, void *pData, zclSceneTableEntry_t *pScene);
 *
 * @brief	Internal restore scene, when this device recieves and recallScene over the air 
 *
 */
void HA_RecallScene(zbEndPoint_t endPoint, void *pData, zclSceneTableEntry_t *pScene);
/*!
 * @fn 		uint8_t HA_ViewSceneData(zclSceneOtaData_t *pClusterData, zclSceneTableEntry_t *pScene);
 *
 * @brief	Creates a payload of scene data. Returns the length of the scene data payload
 *
 */
uint8_t HA_ViewSceneData(zclSceneOtaData_t *pClusterData, zclSceneTableEntry_t *pScene);
/*!
 * @fn 		zbStatus_t HA_AddScene(zclSceneOtaData_t *pClusterData,zclSceneTableEntry_t *pScene);
 *
 * @brief	Internal add scene, when this device recieves and add-scene over the air 
 *
 */
zbStatus_t HA_AddScene(zclSceneOtaData_t *pClusterData,zclSceneTableEntry_t *pScene);
/*!
 * @fn 		void HA_StoreScene(zbEndPoint_t endPoint, zclSceneTableEntry_t *pScene);
 *
 * @brief	Stores the attributes in the scene
 *
 */
void HA_StoreScene(zbEndPoint_t endPoint, zclSceneTableEntry_t *pScene);
/*!
 * @fn 		HA_CopyScene(zclSceneTableEntry_t *pSceneFrom, zclSceneTableEntry_t *pSceneTo);
 *
 * @brief	Copy Scene parameters
 *
 */
void HA_CopyScene(zclSceneTableEntry_t *pSceneFrom, zclSceneTableEntry_t *pSceneTo);
/*!
 * @fn 		void ZCL_ResetScenes(afDeviceDef_t *pDevice, zbGroupId_t groupId)
 *
 * @brief	Processes the ResetScenes command received on the Scene Cluster Server. 
 *
 */
void ZCL_ResetScenes(afDeviceDef_t *pDevice, zbGroupId_t groupId);

/* identify commands */
/*!
 * @fn 		void ZCL_SetIdentifyMode(zbEndPoint_t endPoint,	uint16_t time) 
 *
 * @brief	Local command for entering identify mode for a period of time. Set to 0xffff
 * 			for forever. Set to 0x0000 to turn identify off.
 *
 */

void ZCL_SetIdentifyMode(zbEndPoint_t endPoint, uint16_t time);
/*!
 * @fn 		void ZCL_IdentifyTimeLeft( zbEndPoint_t endPoint )
 *
 * @brief	Local command for determining time left in identify mode
 *
 */
uint16_t ZCL_IdentifyTimeLeft(zbEndPoint_t endPoint);

/* Commissioning cluster commands */
#if gASL_ZclCommissioningRestartDeviceRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_RestartDeviceReq(zclCommissioningRestartDeviceReq_t *pReq) 
 *
 * @brief	Sends over-the-air a RestartDeviceRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_RestartDeviceReq(zclCommissioningRestartDeviceReq_t *pReq);
#else
#define ZCL_Commisioning_RestartDeviceReq(pReq) FALSE
#endif


#if gASL_ZclCommissioningSaveStartupParametersRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_SaveStartupParametersReq(zclCommissioningSaveStartupParametersReq_t *pReq) 
 *
 * @brief	Sends over-the-air a SaveStartupParametersRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_SaveStartupParametersReq(zclCommissioningSaveStartupParametersReq_t *pReq);
#else
#define ZCL_Commisioning_SaveStartupParametersReq(pReq) FALSE
#endif


#if gASL_ZclCommissioningRestoreStartupParametersRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_RestoreStartupParametersReq(zclCommissioningRestoreStartupParametersReq_t *pReq) 
 *
 * @brief	Sends over-the-air a RestoreStartupParametersRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_RestoreStartupParametersReq(zclCommissioningRestoreStartupParametersReq_t *pReq);
#else
#define ZCL_Commisioning_RestoreStartupParametersReq(pReq) FALSE
#endif

#if gASL_ZclCommissioningResetStartupParametersRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_ResetStartupParametersReq(zclCommissioningResetStartupParametersReq_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetStartupParametersRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_ResetStartupParametersReq(zclCommissioningResetStartupParametersReq_t *pReq);
#else
#define ZCL_Commisioning_ResetStartupParametersReq(pReq) FALSE
#endif


/* cluster servers */
/*!
 * @fn 		zbStatus_t ZCL_BasicClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Basic Cluster Server. 
 *
 */
zbStatus_t ZCL_BasicClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_PowerCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the PowerConfiguration Cluster Server. 
 *
 */
zbStatus_t ZCL_PowerCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_TempCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Temperature Configuration Cluster Server. 
 *
 */
zbStatus_t ZCL_TempCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_BinaryInputClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the BinaryInput Cluster Server. 
 *
 */
zbStatus_t ZCL_BinaryInputClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_GroupClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Group Cluster Client. 
 *
 */
zbStatus_t ZCL_GroupClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_GroupClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Group Cluster Server. 
 *
 */
zbStatus_t ZCL_GroupClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_IdentifyClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Identify Cluster Server. 
 *
 */
zbStatus_t ZCL_IdentifyClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_IdentifyClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Identify Cluster Client. 
 *
 */
zbStatus_t ZCL_IdentifyClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t zclIdentify_IdentifyQueryReq(zclIdentifyQueryReq_t *pReq, uint8_t len) 
 *
 * @brief	Sends over-the-air an IdentifyQueryRequest command from the Identify Cluster Client. 
 *
 */
zbStatus_t zclIdentify_IdentifyQueryReq(zclIdentifyQueryReq_t *pReq, uint8_t len);
/*!
 * @fn 		zbStatus_t zclIdentify_IdentifyQueryRsp(zclIdentifyQueryRsp_t *pReq, uint8_t len
 *
 * @brief	Sends over-the-air an IdentifyQueryResponse command from the Identify Cluster Server. 
 *
 */
zbStatus_t zclIdentify_IdentifyQueryRsp(zclIdentifyQueryRsp_t *pReq, uint8_t len);
#if gASL_EnableZLLClustersData_d
/*!
 * @fn 		zbStatus_t zclIdentify_TriggerEffectReq(zclIdentifyTriggerEffectReq_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to HUE  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclIdentify_TriggerEffectReq(zclIdentifyTriggerEffect_t *pReq); 
#endif

/*!
 * @fn 		zbStatus_t ZCL_SceneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Scene Cluster Server. 
 *
 */
zbStatus_t ZCL_SceneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_SceneClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Scene Cluster Client. 
 *
 */
zbStatus_t ZCL_SceneClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_OnOffClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the OnOff Cluster Server. 
 *
 */
zbStatus_t ZCL_OnOffClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
#if gASL_EnableZLLClustersData_d
/*!
 * @fn 		zbStatus_t ZCL_OnOffServerInit(void)
 *
 * @brief	Init COnOff Cluster Server
 *
 */
zbStatus_t ZCL_OnOffServerInit(void);
/*!
 * @fn 		zbStatus_t OnOff_OffWithEffectReq(zclOnOff_OffWithEffect_t *pReq) 
 *
 * @brief	Sends over-the-air an Off with Effect request from OnOff Cluster Client. 
 *
 */
zbStatus_t OnOff_OffWithEffectReq(zclOnOff_OffWithEffect_t *pReq);
/*!
 * @fn 		zbStatus_t OnOff_OnWithRecallGlobalSceneReq(zclOnOff_OnWithRecallGlobalScene_t *pReq) 
 *
 * @brief	Sends over-the-air an On with Recall Global Scene request from OnOff Cluster Client. 
 *
 */
zbStatus_t OnOff_OnWithRecallGlobalSceneReq(zclOnOff_OnWithRecallGlobalScene_t *pReq);
/*!
 * @fn 		zbStatus_t OnOff_OnWithTimedOffReq(zclOnOff_OnWithTimedOff_t *pReq) 
 *
 * @brief	Sends over-the-air an On with Timed Off request from OnOff Cluster Client. 
 *
 */
zbStatus_t OnOff_OnWithTimedOffReq(zclOnOff_OnWithTimedOff_t *pReq); 
#endif
/*!
 * @fn 		zbStatus_t ZCL_LevelControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Level Control Cluster Server. 
 *
 */
zbStatus_t ZCL_LevelControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
uint8_t    ZCL_LevelControl_GetIndexFromLevelSetupTable(uint8_t endpoint, uint8_t tmrId);
zbStatus_t ZCL_LevelControlServerInit(void);

/*!
 * @fn 		zbStatus_t ZCL_OnOffSwitchClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the OnOffSwitch Cluster Server. 
 *
 */
#if gAddValidationFuncPtrToClusterDef_c
/*!
 * @fn 		bool_t  ZCL_LevelControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Validation function for Level control attributes
 *
 */
bool_t  ZCL_LevelControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData);
#endif

zbStatus_t ZCL_OnOffSwitchClusterServer( zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_CommisioningClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Commissioning Cluster Server. 
 *			This cluster allows over-the-air updates of key commissioning values such as PAN ID, extended PAN 
 * 			ID and preconfigured security keys. It can also restart the remote node. This allows a 
 * 			commissioning tool to set up a ZigBee node on a "commissioning" network, then tell the node to go
 * 			join the "final" network.
 *
 */
zbStatus_t ZCL_CommisioningClusterServer( zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_CommisioningClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Commissioning Cluster Client. 
 *			This cluster allows over-the-air updates of key commissioning values such as PAN ID, extended PAN 
 * 			ID and preconfigured security keys. It can also restart the remote node. This allows a 
 * 			commissioning tool to set up a ZigBee node on a "commissioning" network, then tell the node to go
 * 			join the "final" network.
 *
 */
zbStatus_t ZCL_CommisioningClusterClient( zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_GenericReqNoData(afAddrInfo_t *pAddrInfo, zclCmd_t command)
 *
 * @brief	Standard request when there is no payload.Frame control disables default response from receiver.
 *
 */
zbStatus_t ZCL_GenericReqNoData(afAddrInfo_t *pAddrInfo, zclCmd_t command);
/*!
 * @fn 		zbStatus_t ZCL_GenericReqNoDataServer(afAddrInfo_t *pAddrInfo, zclCmd_t command)
 *
 * @brief	Standard request from Server when there is no payload.Frame control disables default response from receiver.
 *
 */
zbStatus_t ZCL_GenericReqNoDataServer(afAddrInfo_t *pAddrInfo, zclCmd_t command);

/***** Alarm cluster servers and commands functions ******/
/*!
 * @fn 		zbStatus_t ZCL_AlarmsClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alarm Cluster Client. 
 *
 */
zbStatus_t ZCL_AlarmsClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ZCL_AlarmsClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alarm Cluster Server. 
 *
 */
zbStatus_t ZCL_AlarmsClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t Alarms_ResetAlarm(zclAlarmInformation_ResetAlarm_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetAlarm command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_ResetAlarm(zclAlarmInformation_ResetAlarm_t *pReq);
/*!
 * @fn 		void ResetAlarm(zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm, uint8_t endpoint)
 *
 * @brief	Process reset all alarm command received on the Alarm Cluster Server. 
 *
 */
void ResetAlarm(zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm, uint8_t endpoint);
/*!
 * @fn 		zbStatus_t Alarms_ResetAllAlarms(zclAlarmInformation_NoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetAllAlarm command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_ResetAllAlarms(zclAlarmInformation_NoPayload_t *pReq);
/*!
 * @fn 		void ResetAllAlarm(uint8_t endpoint)
 *
 * @brief	Process reset all alarm command received on the Alarm Cluster Server. 
 *
 */
void ResetAllAlarm(uint8_t endpoint);
/*!
 * @fn 		zbStatus_t Alarms_GetAlarm(zclAlarmInformation_NoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a GetAlarm command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_GetAlarm(zclAlarmInformation_NoPayload_t *pReq);
/*!
 * @fn 		zbStatus_t Alarms_ResetAlarmLog(zclAlarmInformation_NoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetAlarmLog command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_ResetAlarmLog(zclAlarmInformation_NoPayload_t *pReq);
/*!
 * @fn 		zbStatus_t Alarms_Alarm(zclAlarmInformation_Alarm_t *pReq) 
 *
 * @brief	Sends over-the-air a Alarm command from the Alarm Cluster Server. 
 *
 */
zbStatus_t Alarms_Alarm(zclAlarmInformation_Alarm_t *pReq);
/*!
 * @fn 		zbStatus_t Alarms_GetAlarmResponse(zclAlarms_GetAlarmResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GetAlarmResponse command from the Alarm Cluster Server. 
 *
 */
zbStatus_t Alarms_GetAlarmResponse(zclAlarms_GetAlarmResponse_t *pReq);
/*!
 * @fn 		zbStatus_t Alarms_ClearAlarm(zclAlarms_GetAlarmResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GetAlarmResponse command from the Alarm Cluster Server. 
 *
 */
zbStatus_t Alarms_ClearAlarm(zclAlarms_ClearAlarm_t *pReq);
/**********************************************************/
/*!
 * @fn 		void App_InitTimeCluster(void)
 *
 * @brief	Init Time Cluster using default application values.
 *
 */
void App_InitTimeCluster(uint8_t endpoint);
/*!
 * @fn 		zclTime_GetTimeInf_t App_GetTimeInf(ZCLTime_t timestamp)
 *
 * @brief	Get time information(day, month, hour, minute, year) based on timestamp value
 *
 */
zclTime_GetTimeInf_t App_GetTimeInf(ZCLTime_t LocalTimestamp);

/***** RSSI Location cluster servers and commands functions ******/
/*!
 * @fn 		zbStatus_t ZCL_RSSILocationClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the RSSI Location Cluster Client. 
 *
 */
zbStatus_t ZCL_RSSILocationClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ZCL_RSSILocationClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the RSSI Location Cluster Server. 
 *
 */
zbStatus_t ZCL_RSSILocationClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t RSSILocation_SetAbsoluteLocation(zclCmdRSSI_AbsoluteLocation_t *pReq) 
 *
 * @brief	Sends over-the-air an SetAbsoluteLocation command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_SetAbsoluteLocation(zclCmdRSSI_AbsoluteLocation_t *pReq);
/*!
 * @fn 		zbStatus_t RSSILocation_SetDeviceConfiguration(zclCmdRSSI_SetDeviceConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air an SetDeviceConfiguration command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_SetDeviceConfiguration(zclCmdRSSI_SetDeviceConfiguration_t *pReq);
/*!
 * @fn 		zbStatus_t RSSILocation_GetDeviceConfiguration(zclCmdRSSI_GetDeviceConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air an GetDeviceConfiguration command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_GetDeviceConfiguration(zclCmdRSSI_GetDeviceConfiguration_t *pReq);
/*!
 * @fn 		zbStatus_t RSSILocation_GetLocationData(zclCmdRSSI_GetLocationData_t *pReq) 
 *
 * @brief	Sends over-the-air an GetLocationData command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_GetLocationData(zclCmdRSSI_GetLocationData_t *pReq);
/*!
 * @fn 		zbStatus_t RSSILocation_DeviceConfigurationResponse(zclCmdRSSI_DeviceConfigurationResponse_t *pReq) 
 *
 * @brief	Sends over-the-air an DeviceConfigurationResponse command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_DeviceConfigurationResponse(zclCmdRSSI_DeviceConfigurationResponse_t *pReq);
/*!
 * @fn 		zbStatus_t RSSILocation_LocationDataResponse(zclCmdRSSI_LocationDataResponse_t *pReq) 
 *
 * @brief	Sends over-the-air an LocationDataResponse command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_LocationDataResponse(zclCmdRSSI_LocationDataResponse_t *pReq);
/*!
 * @fn 		zbStatus_t RSSILocation_LocationDataNotification(zclCmdRSSI_LocationDataNotification_t *pReq) 
 *
 * @brief	Sends over-the-air an LocationDataNotification command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_LocationDataNotification(zclCmdRSSI_LocationDataNotification_t *pReq);
/*!
 * @fn 		zbStatus_t RSSILocation_CompactLocationDataNotification(zclCmdRSSI_CompactLocationDataNotification_t *pReq) 
 *
 * @brief	Sends over-the-air an CompactLocationDataNotification command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_CompactLocationDataNotification(zclCmdRSSI_CompactLocationDataNotification_t *pReq);
/*!
 * @fn 		zbStatus_t RSSILocation_RSSIping(zclCmdRSSI_RSSIping_t *pReq) 
 *
 * @brief	Sends over-the-air an RSSIping command from the RSSI Location Cluster Client. 
 *
 */
zbStatus_t RSSILocation_RSSIping(zclCmdRSSI_RSSIping_t *pReq);
/**********************************************************/

/***** ASKE cluster servers and commands functions ******/
/*!
 * @fn 		zbStatus_t ZCL_ASKEClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ZCL_ASKEClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ZCL_ASKEClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ZCL_ASKEClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ASKE_ConfigureSD(zclCmdASKE_ConfigureSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSD command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_ConfigureSD(zclCmdASKE_ConfigureSD_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_UpdateRevocationList(zclCmdASKE_UpdateRevocationList_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationList command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_UpdateRevocationList(zclCmdASKE_UpdateRevocationList_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_RemoveSD(zclCmdASKE_RemoveSD_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveSD command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_RemoveSD(zclCmdASKE_RemoveSD_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_ReadSD(zclCmdASKE_ReadSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSD command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_ReadSD(zclCmdASKE_ReadSD_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_InitiateASKE(zclCmdASKE_InitiateASKE_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASKE command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_InitiateASKE(zclCmdASKE_InitiateASKE_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_ConfirmASKEkey(zclCmdASKE_ConfirmASKEkey_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfirmASKEkey command from the Alpha-Secure Key Establishment Cluster Client.  
 *
 */
zbStatus_t ASKE_ConfirmASKEkey(zclCmdASKE_ConfirmASKEkey_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_TerminateASKE(zclCmdASKE_TerminateASKE_t *pReq) 
 *
 * @brief	Sends over-the-air a TerminateASKE command from the Alpha-Secure Key Establishment Cluster Client.  
 *
 */
zbStatus_t ASKE_TerminateASKE(zclCmdASKE_TerminateASKE_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_GenerateAMK(zclCmdASKE_GenerateAMK_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMK command from the Alpha-Secure Key Establishment Cluster Client.  
 *
 */
zbStatus_t ASKE_GenerateAMK(zclCmdASKE_GenerateAMK_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_ConfigureSDresponse(zclCmdASKE_ConfigureSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSDresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ConfigureSDresponse(zclCmdASKE_ConfigureSDresponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_UpdateRevocationListResponse(zclCmdASKE_UpdateRevocationListResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationListResponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_UpdateRevocationListResponse(zclCmdASKE_UpdateRevocationListResponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_RemoveSDresponse(zclCmdASKE_RemoveSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveSDresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_RemoveSDresponse(zclCmdASKE_RemoveSDresponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_ReadSDresponse(zclCmdASKE_ReadSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSDresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ReadSDresponse(zclCmdASKE_ReadSDresponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_InitiateASKEresponse(zclCmdASKE_InitiateASKEresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASKEresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_InitiateASKEresponse(zclCmdASKE_InitiateASKEresponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_ConfirmASKEkeyResponse(zclCmdASKE_ConfirmASKEkeyResponse_t *pReq);
 *
 * @brief	Sends over-the-air a ConfirmASKEkeyResponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ConfirmASKEkeyResponse(zclCmdASKE_ConfirmASKEkeyResponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_GenerateAMKresponse(zclCmdASKE_GenerateAMKresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMKresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_GenerateAMKresponse(zclCmdASKE_GenerateAMKresponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_ReportRevokedNode(zclCmdASKE_ReportRevokedNode_t *pReq) 
 *
 * @brief	Sends over-the-air a ReportRevokedNode command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ReportRevokedNode(zclCmdASKE_ReportRevokedNode_t *pReq);
/*!
 * @fn 		zbStatus_t ASKE_RequestConfiguration(zclCmdASKE_RequestConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air a RequestConfiguration command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_RequestConfiguration(zclCmdASKE_RequestConfiguration_t *pReq);
/**********************************************************/

/***** ASAC cluster servers and commands functions ******/
/*!
 * @fn 		zbStatus_t ZCL_ASACClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Access Control Cluster Client. 
 *
 */
zbStatus_t ZCL_ASACClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ZCL_ASACClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Access Control Cluster Server. 
 *
 */
zbStatus_t ZCL_ASACClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ASAC_ConfigureSD(zclCmdASAC_ConfigureSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSD command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ConfigureSD(zclCmdASAC_ConfigureSD_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_UpdateRevocationList(zclCmdASAC_UpdateRevocationList_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationList command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_UpdateRevocationList(zclCmdASAC_UpdateRevocationList_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ConfigureACpolicies(zclCmdASAC_ConfigureACpolicies_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureACpolicies command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ConfigureACpolicies(zclCmdASAC_ConfigureACpolicies_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ReadSD(zclCmdASAC_ReadSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSD command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ReadSD(zclCmdASAC_ReadSD_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ReadACpolicies(zclCmdASAC_ReadACpolicies_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadACpolicies command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ReadACpolicies(zclCmdASAC_ReadACpolicies_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_RemoveAC(zclCmdASAC_RemoveAC_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveAC command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_RemoveAC(zclCmdASAC_RemoveAC_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ACpropertiesRequest(void) 
 *
 * @brief	Sends over-the-air a ACpropertiesRequest command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ACpropertiesRequest(void);
/*!
 * @fn 		zbStatus_t ASAC_TSReportResponse(void) 
 *
 * @brief	Sends over-the-air a TSReportResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_TSReportResponse(void);
/*!
 * @fn 		zbStatus_t ASAC_InitiateASAC(zclCmdASAC_InitiateASAC_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASAC command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_InitiateASAC(zclCmdASAC_InitiateASAC_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ConfirmASACkey(zclCmdASAC_ConfirmASAC_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfirmASACkey command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ConfirmASACkey(zclCmdASAC_ConfirmASAC_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_TerminateASAC(zclCmdASAC_TerminateASAC_t *pReq) 
 *
 * @brief	Sends over-the-air a TerminateASAC command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_TerminateASAC(zclCmdASAC_TerminateASAC_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_GenerateAMK(zclCmdASAC_GenerateAMK_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMK command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_GenerateAMK(zclCmdASAC_GenerateAMK_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_LDCtransport(zclCmdASAC_LDCtransport_t *pReq) 
 *
 * @brief	Sends over-the-air a LDCtransport command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_LDCtransport(zclCmdASAC_LDCtransport_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ConfigureSDresponse(zclCmdASAC_ConfigureSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSDresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ConfigureSDresponse(zclCmdASAC_ConfigureSDresponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_UpdateRevocationListResponse(zclCmdASAC_UpdateRevocationListResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationListResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_UpdateRevocationListResponse(zclCmdASAC_UpdateRevocationListResponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ConfigureACpoliciesResponse(zclCmdASAC_ConfigureACpoliciesResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureACpoliciesResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ConfigureACpoliciesResponse(zclCmdASAC_ConfigureACpoliciesResponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ReadSDresponse(zclCmdASAC_ReadSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSDresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ReadSDresponse(zclCmdASAC_ReadSDresponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ReadACpoliciesResponse(zclCmdASAC_ReadACpoliciesResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadACpoliciesResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ReadACpoliciesResponse(zclCmdASAC_ReadACpoliciesResponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_RemoveACresponse(zclCmdASAC_RemoveACresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveACresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_RemoveACresponse(zclCmdASAC_RemoveACresponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ACpropertiesResponse(zclCmdASAC_ACpropertiesResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ACpropertiesResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ACpropertiesResponse(zclCmdASAC_ACpropertiesResponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_TSReport(zclCmdASAC_TSreport_t *pReq) 
 *
 * @brief	Sends over-the-air a TSReport command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_TSReport(zclCmdASAC_TSreport_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_InitiateASACresponse(zclCmdASAC_InitiateASACresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASACresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_InitiateASACresponse(zclCmdASAC_InitiateASACresponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_ConfirmASACkeyResponse(zclCmdASAC_ConfirmASACkeyResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfirmASACkeyResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ConfirmASACkeyResponse(zclCmdASAC_ConfirmASACkeyResponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_GenerateAMKresponse(zclCmdASAC_GenerateAMKresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMKrespons command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_GenerateAMKresponse(zclCmdASAC_GenerateAMKresponse_t *pReq);
/*!
 * @fn 		zbStatus_t ASAC_LDCtransportResponse(zclCmdASAC_LDCtransportResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a LDCtransportResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_LDCtransportResponse(zclCmdASAC_LDCtransportResponse_t *pReq);

/*!
 * @fn 		zbStatus_t ASAC_RequestConfiguration(zclCmdASAC_RequestConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air a RequestConfiguration command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_RequestConfiguration(zclCmdASAC_RequestConfiguration_t *pReq);


#if gZclEnablePollControlCluster_d
/*!
 * @fn 		zbStatus_t ZCL_PollControlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Poll Control Cluster Client. 
 *
 */
zbStatus_t ZCL_PollControlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_PollControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Poll Control Cluster Server. 
 *
 */
zbStatus_t ZCL_PollControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t zclPollControl_CheckIn(zclPollControl_CheckIn_t *pReq) 
 *
 * @brief	Sends over-the-air a CheckIn command from the PollControl Cluster Server. 
 *
 */
zbStatus_t zclPollControl_CheckIn(zclPollControl_CheckIn_t *pReq); 
/*!
 * @fn 		zbStatus_t zclPollControl_CheckInRsp(zclPollControl_CheckInRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a CheckInRsp command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_CheckInRsp(zclPollControl_CheckInRsp_t *pReq);
/*!
 * @fn 		zbStatus_t zclPollControl_FastPollStop(zclPollControl_FastPollStop_t *pReq) 
 *
 * @brief	Sends over-the-air a FastPollStop command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_FastPollStop(zclPollControl_FastPollStop_t *pReq); 
/*!
 * @fn 		zbStatus_t zclPollControl_SetLongPollInterval(zclPollControl_SetLongPollInterval_t *pReq) 
 *
 * @brief	Sends over-the-air a SetLongPollInterval command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_SetLongPollInterval(zclPollControl_SetLongPollInterval_t *pReq);
/*!
 * @fn 		zbStatus_t zclPollControl_SetShortPollInterval(zclPollControl_SetShortPollInterval_t *pReq) 
 *
 * @brief	Sends over-the-air a SetShortPollInterval command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_SetShortPollInterval(zclPollControl_SetShortPollInterval_t *pReq); 
/*!
 * @fn 		void ZclPollControl_WriteAttribute(void *pData)
 *
 * @brief	Poll control write attributes function
 *
 */
void ZclPollControl_WriteAttribute(uint8_t endpoint, void *pData);

#if gEndDevCapability_d || gComboDeviceCapability_d  
zbStatus_t zclPollControl_ClusterServerInit(void);
uint16_t ZclPollControl_GetMaxShortPollIntervalValue(void);
uint32_t ZclPollControl_GetMinLongPollIntervalValue(void);
uint8_t ZCL_PollControl_GetIndexFromPollControlSetupTable(uint8_t endpoint, uint8_t tmrId);
#endif
/*!
 * @fn 		bool_t  ZclPollControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Poll control validate attributes function
 *
 */

bool_t  ZclPollControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData);
/*!
 * @fn 		zbStatus_t  ztcPollControl_SetClientInf(zclPollControlClientSetup_t * setClientInf)
 *
 * @brief	Set client poll control information
 *
 */
zbStatus_t ztcPollControl_SetClientInf(zclPollControlClientSetup_t * setClientInf);
#endif //gZclEnablePollControlCluster_d

#if gZclClusterOptionals_d
/*!
 * @fn 		zbStatus_t ZCL_SetValueAttr( zclSetAttrValue_t *pSetAttrValueReq)
 *
 * @brief	Local ZTC function used to set the value for some attributes.
 *
 */
zbStatus_t ZCL_SetValueAttr(zclSetAttrValue_t *pSetAttrValueReq);
/*!
 * @fn 		zbStatus_t ZCL_ProcessUnsolicitedCommand(zclProcessUnsolicitedCommand_t *pProcessUnsolicitedCommand)  
 *
 * @brief	Local ZTC function that manage some unsolicited commands.
 *
 */
zbStatus_t ZCL_ProcessUnsolicitedCommand(zclProcessUnsolicitedCommand_t *pProcessUnsolicitedCommand);
/*!
 * @fn 		void TmrOccupancyCallBack(zbTmrTimerID_t tmrid)
 *
 * @brief	Occupancy Cluster Callback
 *
 */
void TmrOccupancyCallBack(zbTmrTimerID_t tmrid);
#endif
/*!
 * @fn 		bool_t InterpretMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint)
 *
 * @brief	This fucntion return TRUE only if the MatchDescriptor Req was generated by a OTA Cluster command
 *
 */
bool_t InterpretMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint);
/*!
 * @fn 		zbEndPoint_t ZCL_GetEndPointForSpecificCluster(zbClusterId_t clusterId, bool_t isClusterServer)
 *
 * @brief	 GetEndPoint for a specific cluster based on Endpoint List informations
 *
 */
zbEndPoint_t ZCL_GetEndPointForSpecificCluster(zbClusterId_t clusterId, bool_t isClusterServer, uint8_t startIndex, uint8_t *pNextStartIndex);
#if gInterPanCommunicationEnabled_c
/*!
 * @fn 		void ZCL_RegisterInterPanClient(pfnInterPanIndication_t pFunc)
 *
 * @brief	Register the InterPAN Client function.
 *
 */
void ZCL_RegisterInterPanClient(pfnInterPanIndication_t pFunc);
/*!
 * @fn 		void ZCL_RegisterInterPanServer(pfnInterPanIndication_t pFunc)
 *
 * @brief	Register the InterPAN Server function.
 *
 */
void ZCL_RegisterInterPanServer(pfnInterPanIndication_t pFunc);
/*!
 * @fn 		zbStatus_t ZCL_InterPanClusterServer(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Handle ALL Inter Pan Server Indications (filter after cluster ID).
 *
 */
zbStatus_t ZCL_InterPanClusterServer(zbInterPanDataIndication_t *pIndication,afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZCL_InterPanClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Handle ALL Inter Pan Client Indications (filter after cluster ID).
 *
 */
zbStatus_t ZCL_InterPanClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev); 
/*!
 * @fn 		void PrepareInterPanForReply(InterPanAddrInfo_t *pAdrrDest, zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Gets address ready for InterPan reply.
 *
 */
void PrepareInterPanForReply(InterPanAddrInfo_t *pAdrrDest, zbInterPanDataIndication_t *pIndication);
#endif
/**********************************************************/
#ifdef __cplusplus
}
#endif

#endif /*ZCL_GENERAL_H*/
