/*! @file 	ZclHVAC.h
 *
 * @brief	Types, definitions and prototypes for the  for the HVAC(Heating/Ventilation/Air-Conditioning) domain.
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
#ifndef _ZCL_HVAC_H
#define _ZCL_HVAC_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "ZCL.h"


/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/

/******************************************
	Thermostat Cluster
*******************************************/
#define gHaThermostatWeeklyScheduleEntries_d 2
#define gHaThermostatMaxLogEntries_d         2

#if (TRUE == gBigEndian_c)
/* 6.3.2.2.1 Thermostat information cluster attributes */
#define gZclAttrThermostat_LocalTemperatureId_c             0x0000 /* M - Local Temperature */
#define gZclAttrThermostat_OutdoorTemperatureId_c           0x0100 /* M - Outdoor Temperature */
#define gZclAttrThermostat_OccupancyId_c                    0x0200 /* O - Occupancy */
#define gZclAttrThermostat_AbsMinHeatSetpointLimitId_c      0x0300 /* O - AbsMinHeatSetpointLimit */
#define gZclAttrThermostat_AbsMaxHeatSetpointLimitId_c      0x0400 /* O - AbsMaxHeatSetpointLimit */
#define gZclAttrThermostat_AbsMinCoolSetpointLimitId_c      0x0500 /* O - AbsMinCoolSetpointLimit */
#define gZclAttrThermostat_AbsMaxCoolSetpointLimitId_c      0x0600 /* O - AbsMaxCoolSetpointLimit */
#define gZclAttrThermostat_PICoolingDemandId_c              0x0700 /* O - PI Cooling Demand */
#define gZclAttrThermostat_PIHeatingDemandId_c              0x0800 /* O - PI Heating Demand */
#define gZclAttrThermostat_SystemTypeConfiguration_c        0x0900 /* O - HVAC System Type Configuration */

/* 6.3.2.2.2 Thermostat settings attributes */
#define gZclAttrThermostat_LocalTemperatureCalibrationId_c  0x1000 /* O - Local Temperature Calibration */
#define gZclAttrThermostat_OccupiedCoolingSetpointId_c      0x1100 /* M - Occupied Cooling Setpoint */
#define gZclAttrThermostat_OccupiedHeatingSetpointId_c      0x1200 /* M - Occupied Heating Setpoint */
#define gZclAttrThermostat_UnoccupiedCoolingSetpointId_c    0x1300 /* O - Unoccupied Cooling Setpoint */
#define gZclAttrThermostat_UnoccupiedHeatingSetpointId_c    0x1400 /* O - Unoccupied Heating Setpoint */
#define gZclAttrThermostat_MinHeatSetpointLimitId_c         0x1500 /* M - MinHeatSetpointLimit */
#define gZclAttrThermostat_MaxHeatSetpointLimitId_c         0x1600 /* M - MaxHeatSetpointLimit */
#define gZclAttrThermostat_MinCoolSetpointLimitId_c         0x1700 /* M - MinCoolSetpointLimit */
#define gZclAttrThermostat_MaxCoolSetpointLimitId_c         0x1800 /* M - MaxCoolSetpointLimit */
#define gZclAttrThermostat_MinSetpointDeadBandId_c          0x1900 /* M - MinSetpointDeadBand */
#define gZclAttrThermostat_RemoteSensingId_c                0x1a00 /* O - Remote Sensing */
#define gZclAttrThermostat_ControlSequenceOfOperationId_c   0x1b00 /* M - Control Sequence of Operation */
#define gZclAttrThermostat_SystemModeId_c                   0x1c00 /* M - System mode */
#define gZclAttrThermostat_AlarmMaskId_c                    0x1d00 /* O - Alarm Mask */
#define gZclAttrThermostat_RunningModeId_c                  0x1e00 /* O - Running Mode */

/* Thermostat Schedule & HVAC Relay Attribute Set */
#define gZclAttrThermostat_StartOfWeek_c                        0x2000 /* O - Start of Week */
#define gZclAttrThermostat_NumberOfWeeklyTransitions_c          0x2100 /* O - Number of Weekly Transitions */
#define gZclAttrThermostat_NumberOfDailyTransitions_c           0x2200 /* O - Number of Day Transitions */
#define gZclAttrThermostat_TemperatureSetpointHold_c            0x2300 /* O - Temperature Setpoint Hold */
#define gZclAttrThermostat_TemperatureSetpointHoldDuration_c    0x2400 /* O - Temperature Setpoint Hold Duration */
#define gZclAttrThermostat_ProgrammingOperationMode_c           0x2500 /* O - Thermostat Programming Operation Mode */
#define gZclAttrThermostat_RunningState_c                       0x2900 /* O - Thermostat Running State */

/* Thermostat Setpoint Change Tracking Attribute Set */
#define gZclAttrThermostat_SetpointChangeSource_c               0x3000 /* O - Setpoint Change Source */
#define gZclAttrThermostat_SetpointChangeAmount_c               0x3100 /* O - Setpoint Change Amount */
#define gZclAttrThermostat_SetpointChangeSourceTimestamp_c      0x3200 /* O - Setpoint Change Source Timestamp */

/* Thermostat AC information Attribute Set */
#define gZclAttrThermostat_AcType_c                      0x4000 /* O - AC type */
#define gZclAttrThermostat_AcCapacity_c                  0x4100 /* O - AC capacity */
#define gZclAttrThermostat_AcRefrigerantType_c           0x4200 /* O - AC Refrigerant Type */
#define gZclAttrThermostat_AcCompresorType_c             0x4300 /* O - AC Compressor Type */
#define gZclAttrThermostat_AcErrorCode_c                 0x4400 /* O - AC error code */
#define gZclAttrThermostat_AcLouverPosition_c            0x4500 /* O - AC louver position */
#define gZclAttrThermostat_AcCoilTemperature_c           0x4600 /* O - AC coil temperature */
#define gZclAttrThermostat_AcCapacityFormat_c            0x4700 /* O - AC Capacity format */

#else

/* 6.3.2.2.1 Thermostat information cluster attributes */
#define gZclAttrThermostat_LocalTemperatureId_c             0x0000 /* M - Local Temperature */
#define gZclAttrThermostat_OutdoorTemperatureId_c           0x0001 /* M - Outdoor Temperature */
#define gZclAttrThermostat_OccupancyId_c                    0x0002 /* O - Occupancy */
#define gZclAttrThermostat_AbsMinHeatSetpointLimitId_c      0x0003 /* O - AbsMinHeatSetpointLimit */
#define gZclAttrThermostat_AbsMaxHeatSetpointLimitId_c      0x0004 /* O - AbsMaxHeatSetpointLimit */
#define gZclAttrThermostat_AbsMinCoolSetpointLimitId_c      0x0005 /* O - AbsMinCoolSetpointLimit */
#define gZclAttrThermostat_AbsMaxCoolSetpointLimitId_c      0x0006 /* O - AbsMaxCoolSetpointLimit */
#define gZclAttrThermostat_PICoolingDemandId_c              0x0007 /* O - PI Cooling Demand */
#define gZclAttrThermostat_PIHeatingDemandId_c              0x0008 /* O - PI Heating Demand */
#define gZclAttrThermostat_SystemTypeConfiguration_c        0x0009 /* O - HVAC System Type Configuration */

/* 6.3.2.2.2 Thermostat settings attributes */
#define gZclAttrThermostat_LocalTemperatureCalibrationId_c  0x0010 /* O - Local Temperature Calibration */
#define gZclAttrThermostat_OccupiedCoolingSetpointId_c      0x0011  /* M - Occupied Cooling Setpoint */
#define gZclAttrThermostat_OccupiedHeatingSetpointId_c      0x0012 /* M - Occupied Heating Setpoint */
#define gZclAttrThermostat_UnoccupiedCoolingSetpointId_c    0x0013 /* O - Unoccupied Cooling Setpoint */
#define gZclAttrThermostat_UnoccupiedHeatingSetpointId_c    0x0014 /* O - Unoccupied Heating Setpoint */
#define gZclAttrThermostat_MinHeatSetpointLimitId_c         0x0015 /* M - MinHeatSetpointLimit */
#define gZclAttrThermostat_MaxHeatSetpointLimitId_c         0x0016 /* M - MaxHeatSetpointLimit */
#define gZclAttrThermostat_MinCoolSetpointLimitId_c         0x0017 /* M - MinCoolSetpointLimit */
#define gZclAttrThermostat_MaxCoolSetpointLimitId_c         0x0018 /* M - MaxCoolSetpointLimit */
#define gZclAttrThermostat_MinSetpointDeadBandId_c          0x0019 /* M - MinSetpointDeadBand */
#define gZclAttrThermostat_RemoteSensingId_c                0x001a /* O - Remote Sensing */
#define gZclAttrThermostat_ControlSequenceOfOperationId_c   0x001b /* M - Control Sequence of Operation */
#define gZclAttrThermostat_SystemModeId_c                   0x001c /* M - System mode */
#define gZclAttrThermostat_AlarmMaskId_c                    0x001d /* O - Alarm Mask */
#define gZclAttrThermostat_RunningModeId_c                  0x001e /* O - Running Mode */

/* Thermostat Schedule & HVAC Relay Attribute Set */
#define gZclAttrThermostat_StartOfWeek_c                        0x0020 /* O - Start of Week */
#define gZclAttrThermostat_NumberOfWeeklyTransitions_c          0x0021 /* O - Number of Weekly Transitions */
#define gZclAttrThermostat_NumberOfDailyTransitions_c           0x0022 /* O - Number of Day Transitions */
#define gZclAttrThermostat_TemperatureSetpointHold_c            0x0023 /* O - Temperature Setpoint Hold */
#define gZclAttrThermostat_TemperatureSetpointHoldDuration_c    0x0024 /* O - Temperature Setpoint Hold Duration */
#define gZclAttrThermostat_ProgrammingOperationMode_c           0x0025 /* O - Thermostat Programming Operation Mode */
#define gZclAttrThermostat_RunningState_c                       0x0029 /* O - Thermostat Running State */

/* Thermostat Setpoint Change Tracking Attribute Set */
#define gZclAttrThermostat_SetpointChangeSource_c               0x0030 /* O - Setpoint Change Source */
#define gZclAttrThermostat_SetpointChangeAmount_c               0x0031 /* O - Setpoint Change Amount */
#define gZclAttrThermostat_SetpointChangeSourceTimestamp_c      0x0032 /* O - Setpoint Change Source Timestamp */

/* Thermostat AC information Attribute Set */
#define gZclAttrThermostat_AcType_c                      0x0040 /* O - AC type */
#define gZclAttrThermostat_AcCapacity_c                  0x0041 /* O - AC capacity */
#define gZclAttrThermostat_AcRefrigerantType_c           0x0042 /* O - AC Refrigerant Type */
#define gZclAttrThermostat_AcCompresorType_c             0x0043 /* O - AC Compressor Type */
#define gZclAttrThermostat_AcErrorCode_c                 0x0044 /* O - AC error code */
#define gZclAttrThermostat_AcLouverPosition_c            0x0045 /* O - AC louver position */
#define gZclAttrThermostat_AcCoilTemperature_c           0x0046 /* O - AC coil temperature */
#define gZclAttrThermostat_AcCapacityFormat_c            0x0047 /* O - AC Capacity format */

#endif /* #if (TRUE == gBigEndian_c) */


#define gZclAttrSetThermostat_c                               0x00

#define gZclAttrIdThermostat_LocalTemperatureId_c             0x00 /* M - Local Temperature */
#define gZclAttrIdThermostat_OutdoorTemperatureId_c           0x01 /* M - Outdoor Temperature */
#define gZclAttrIdThermostat_OccupancyId_c                    0x02 /* O - Occupancy */
#define gZclAttrIdThermostat_AbsMinHeatSetpointLimitId_c      0x03 /* O - AbsMinHeatSetpointLimit */
#define gZclAttrIdThermostat_AbsMaxHeatSetpointLimitId_c      0x04 /* O - AbsMaxHeatSetpointLimit */
#define gZclAttrIdThermostat_AbsMinCoolSetpointLimitId_c      0x05 /* O - AbsMinCoolSetpointLimit */
#define gZclAttrIdThermostat_AbsMaxCoolSetpointLimitId_c      0x06 /* O - AbsMaxCoolSetpointLimit */
#define gZclAttrIdThermostat_PICoolingDemandId_c              0x07 /* O - PI Cooling Demand */
#define gZclAttrIdThermostat_PIHeatingDemandId_c              0x08 /* O - PI Heating Demand */
#define gZclAttrIdThermostat_SystemTypeConfiguration_c        0x09 /* O - HVAC System Type Configuration */
#define gZclAttrIdThermostat_LocalTemperatureCalibrationId_c  0x10 /* O - Local Temperature Calibration */
#define gZclAttrIdThermostat_OccupiedCoolingSetpointId_c      0x11 /* M - Occupied Cooling Setpoint */
#define gZclAttrIdThermostat_OccupiedHeatingSetpointId_c      0x12 /* M - Occupied Heating Setpoint */
#define gZclAttrIdThermostat_UnoccupiedCoolingSetpointId_c    0x13 /* O - Unoccupied Cooling Setpoint */
#define gZclAttrIdThermostat_UnoccupiedHeatingSetpointId_c    0x14 /* O - Unoccupied Heating Setpoint */
#define gZclAttrIdThermostat_MinHeatSetpointLimitId_c         0x15 /* M - MinHeatSetpointLimit */
#define gZclAttrIdThermostat_MaxHeatSetpointLimitId_c         0x16 /* M - MaxHeatSetpointLimit */
#define gZclAttrIdThermostat_MinCoolSetpointLimitId_c         0x17 /* M - MinCoolSetpointLimit */
#define gZclAttrIdThermostat_MaxCoolSetpointLimitId_c         0x18 /* M - MaxCoolSetpointLimit */
#define gZclAttrIdThermostat_MinSetpointDeadBandId_c          0x19 /* M - MinSetpointDeadBand */
#define gZclAttrIdThermostat_RemoteSensingId_c                0x1a /* O - Remote Sensing */
#define gZclAttrIdThermostat_ControlSequenceOfOperationId_c   0x1b /* M - Control Sequence of Operation */
#define gZclAttrIdThermostat_SystemModeId_c                   0x1c /* M - System mode */
#define gZclAttrIdThermostat_AlarmMaskId_c                    0x1d /* O - Alarm Mask */
#define gZclAttrIdThermostat_RunningModeId_c                  0x1e /* O - Running Mode */
#define gZclAttrIdThermostat_StartOfWeek_c                        0x20 /* O - Start of Week */
#define gZclAttrIdThermostat_NumberOfWeeklyTransitions_c          0x21 /* O - Number of Weekly Transitions */
#define gZclAttrIdThermostat_NumberOfDailyTransitions_c           0x22 /* O - Number of Day Transitions */
#define gZclAttrIdThermostat_TemperatureSetpointHold_c            0x23 /* O - Temperature Setpoint Hold */
#define gZclAttrIdThermostat_TemperatureSetpointHoldDuration_c    0x24 /* O - Temperature Setpoint Hold Duration */
#define gZclAttrIdThermostat_ProgrammingOperationMode_c           0x25 /* O - Thermostat Programming Operation Mode */
#define gZclAttrIdThermostat_RunningState_c                       0x29 /* O - Thermostat Running State */
#define gZclAttrIdThermostat_SetpointChangeSource_c               0x30 /* O - Setpoint Change Source */
#define gZclAttrIdThermostat_SetpointChangeAmount_c               0x31 /* O - Setpoint Change Amount */
#define gZclAttrIdThermostat_SetpointChangeSourceTimestamp_c      0x32 /* O - Setpoint Change Source Timestamp */
#define gZclAttrIdThermostat_AcType_c                             0x40 /* O - AC type */
#define gZclAttrIdThermostat_AcCapacity_c                         0x41 /* O - AC capacity */
#define gZclAttrIdThermostat_AcRefrigerantType_c                  0x42 /* O - AC Refrigerant Type */
#define gZclAttrIdThermostat_AcCompresorType_c                    0x43 /* O - AC Compressor Type */
#define gZclAttrIdThermostat_AcErrorCode_c                        0x44 /* O - AC error code */
#define gZclAttrIdThermostat_AcLouverPosition_c                   0x45 /* O - AC louver position */
#define gZclAttrIdThermostat_AcCoilTemperature_c                  0x46 /* O - AC coil temperature */
#define gZclAttrIdThermostat_AcCapacityFormat_c                   0x47 /* O - AC Capacity format */


enum{
/* 6.3.2.2.2.12 Thermostat Control Sequence of Operation Attribute Values */
   gZclControlSecuence_CoolingOnly_c = 0x00,                        /* Cooling Only */
   gZclControlSecuenceCoolingWithReheat_c = 0x01,                   /* Cooling with reheat */
   gZclControlSecuence_HeatingOnly_c = 0x02,                        /* Heating Only */
   gZclControlSecuence_HeatingWithReheat_c = 0x03,                  /* Heating with reheat */
   gZclControlSecuence_CoolingAndHeating4Pipes_c = 0x04,            /* Cooling and Heating */
   gZclControlSecuence_CoolingAndHeating4PipesWithReheat_c = 0x05   /* Cooling and Heating 4Pipes With Reheat */
};

/* 6.3.2.2.4 Thermostat Alarm Mask attribute values */
#define gZclThermostat_InitializationFailure_c    0 /* Initialization Failure */
#define gZclThermostat_HardwareFailure_c          1 /* Hardware Failure */
#define gZclThermostat_SelfCalibrationFailure_c   2 /* Self Calibration Failure */

/* Thermostat Mode Values*/
enum{
/* 6.3.2.3.1.2 Thermostat  Mode Values */
   gThermostatModeHeat_c = 0x00,
   gThermostatModeCool_c = 0x01,
   gThermostatModeBoth_c = 0x02
};

/* 6.3.2.2.3 Thermostat System Mode Attribute Values */
enum{
   gThermostat_SystemMode_Off_c = 0x00,
   gThermostat_SystemMode_Auto_c,
   gThermostat_SystemMode_Cool_c,
   gThermostat_SystemMode_Heat_c,
   gThermostat_SystemMode_EmergencyHeating_c,
   gThermostat_SystemMode_PreCooling_c,
   gThermostat_SystemMode_FanOnly_c,
   gThermostat_SystemMode_Dry_c,
   gThermostat_SystemMode_Sleep_c
     /* reserverd 0x0A - 0xFF */
};

/* HVAC System Type Configuration struct */
typedef PACKED_STRUCT zclThermostatSystemTypeConfig_tag
{
  unsigned coolingSystemStage :2;
  unsigned heatingSystemStage :2;
  unsigned heatingSystemType  :1;
  unsigned heatingFuelSource  :1;
  unsigned reserved           :2;  
}zclThermostatSystemTypeConfig_t;

/* cooling system stage enumeration */
enum{
  gThermostat_CoolingSystemStage_coolStage1_c = 0x00,
  gThermostat_CoolingSystemStage_coolStage2_c = 0x01,
  gThermostat_CoolingSystemStage_coolStage3_c = 0x10
  /* Reserved   = 0x11 */
};

/* heating system stage enumeration */
enum{
  gThermostat_HeatingSystemStage_heatStage1_c = 0x00,
  gThermostat_HeatingSystemStage_heatStage2_c = 0x01,
  gThermostat_HeatingSystemStage_heatStage3_c = 0x10
  /* Reserved   = 0x11 */
};

/* heating system type enumeration */
enum{
  gThermostat_HeatingSystemType_Conventional_c = 0x00,
  gThermostat_HeatingSystemType_HeatPump_c     = 0x01
};

/* heating fuel source enumeration */
enum{
  gThermostat_HeatingFuelSource_Electric_c = 0x00,
  gThermostat_HeatingFuelSource_Gas_c      = 0x01
};

/* thermostat running mode atribute values */
enum{
  gThermostat_RunningMode_Off_c = 0x00,
  /* Reserved 0x01-0x02*/
  gThermostat_RunningMode_Cool_c = 0x03,
  gThermostat_RunningMode_Heat_c = 0x04
  /* Reserved 0x05-0x0FF*/
};

/* HVAC Remote Sensing  struct */
typedef PACKED_STRUCT zclThermostatRemoteSensing_tag
{
  unsigned localTempSendedMode   :1;
  unsigned outdoorTempSendedMode :1;
  unsigned occupancySendedMode   :1;
  unsigned reserved              :5;   
}zclThermostatRemoteSensing_t;

/* remote sending attribute bit values */
#define gThermostatRemoteSensing_LocalTempSendedInternally_d            0x00  /* bit 0 */
#define gThermostatRemoteSensing_LocalTempSendedRemotely_d              0x01  /* bit 0 */
#define gThermostatRemoteSensing_OutdoorTempSendedInternally_d          0x00  /* bit 1 */ 
#define gThermostatRemoteSensing_OutdoorTempSendedRemotely_d            0x01  /* bit 1 */
#define gThermostatRemoteSensing_OccupancyTempSendedInternally_d        0x00  /* bit 2 */
#define gThermostatRemoteSensing_OccupancyTempSendedRemotely_d          0x01  /* bit 2 */

/* start of week attribute values */
enum{
  gThermostat_StartOfWeek_Sunday_c = 0x00,
  gThermostat_StartOfWeek_Monday_c,
  gThermostat_StartOfWeek_Tuesday_c,
  gThermostat_StartOfWeek_Wednesday_c,
  gThermostat_StartOfWeek_Thursday_c,
  gThermostat_StartOfWeek_Friday_c,
  gThermostat_StartOfWeek_Saturday_c
  /* reseved : 0x07-0xFF */
};

/* temperature setpoint hold attribute values */
enum{
  gThermostat_TempSetpoint_HoldOff_c = 0x00,
  gThermostat_TempSetpoint_HoldOn_c
  /* reseved : 0x02-0xFF */
};

/* thermostat programming operation mode attribute struct */
typedef PACKED_STRUCT zclThermostatProgOperationMode_tag
{
  unsigned simpleOrScheduledMode   :1;
  unsigned autoRecoveryMode        :1;
  unsigned economyEnergyStarMode   :1;
  unsigned reserved                :5;     
}zclThermostatProgOperationMode_t;

/* programming operation mode attribute bit values */
#define gThermostatProgOperationMode_SimpleSetpoint_d            0x00  /* bit 0 */
#define gThermostatProgOperationMode_ScheduleProgramming_d       0x01  /* bit 0 */
#define gThermostatProgOperationMode_AutoRecoveryOff_d           0x00  /* bit 1 */ 
#define gThermostatProgOperationMode_AutoRecoveryOn_d            0x01  /* bit 1 */
#define gThermostatProgOperationMode_EconomyEnergyStarOff_d      0x00  /* bit 2 */
#define gThermostatProgOperationMode_EconomyEnergyStarOn_d       0x01  /* bit 2 */

/* thermostat running state attribute struct */
typedef PACKED_STRUCT zclThermostatRunningState_tag
{
  unsigned heatState             :1; /* 0x01- On, 0x00 -OFF */
  unsigned coolState             :1; /* 0x01- On, 0x00 -OFF */
  unsigned fanState              :1; /* 0x01- On, 0x00 -OFF */
  unsigned heat2StageState       :1; /* 0x01- On, 0x00 -OFF */
  unsigned cool2StageState       :1; /* 0x01- On, 0x00 -OFF */
  unsigned fan2StageState        :1; /* 0x01- On, 0x00 -OFF */
  unsigned fan3StageState        :1; /* 0x01- On, 0x00 -OFF */
  unsigned reserved1             :1; /* 0x01- On, 0x00 -OFF */    
  uint8_t reserved; 
}zclThermostatRunningState_t;

/* setpoint change source attribute values */
enum{
  gThermostat_SetpointChangeSource_Manual_c = 0x00,     /* manual, user-initiated setpoint change via the thermostat */
  gThermostat_SetpointChangeSource_Schedule_c,          /* schedule/internal programming initiated setpoint change */
  gThermostat_SetpointChangeSource_Externally_c,        /* externally initiated setpoint change */
  /* reseved : 0x03-0xFF */
};

/* thermostat AC type attribute values */
enum{
  gThermostat_AcType_CoolingAndFixedSpeed_c  = 0x01,     
  gThermostat_AcType_HeatPumpAndFixedSpeed_c, 
  gThermostat_AcType_CoolingAndInverter_c, 
  gThermostat_AcType_HeatPumpAndInverter_c
};

/* thermostat AC refrigerant Type attribute values */
enum{
  gThermostat_AcRefrigerantType_R22_c  = 0x01,     
  gThermostat_AcRefrigerantType_R410a_c,   
  gThermostat_AcRefrigerantType_R407c_c  
};

/* thermostat AC Compressor Type attribute values */
enum{
  gThermostat_AcCompressorTypeT1_c  = 0x01,   /* T1, max working ambient 43 grade C*/  
  gThermostat_AcCompressorTypeT2_c,           /* T2, max working ambient 35 grade C*/    
  gThermostat_AcCompressorTypeT3_c            /* T3, max working ambient 52 grade C*/    
};


/* thermostat AC Louver Position attribute values */
enum{
  gThermostat_AcLouverPosition_FullyClosed_c  = 0x01,   
  gThermostat_AcLouverPosition_FullyOpen_c,   
  gThermostat_AcLouverPosition_QuarterOpen_c,   
  gThermostat_AcLouverPosition_HalfOpen_c, 
  gThermostat_AcLouverPosition_ThreeQuarterOpen_c
  /* reserved 0x06 - 0xFF */
};


/* 6.3.2.3 Thermostat Server Commands Received */
#define gZclCmdThermostat_SetpointRaiseLower_c  0x00    /* Setpoint Raise / Lower */
#define gZclCmdThermostat_SetWeeklySchedule_c   0x01    /* Set Weekly Schedule */
#define gZclCmdThermostat_GetWeeklySchedule_c   0x02    /* Get Weekly Schedule */
#define gZclCmdThermostat_ClearWeeklySchedule_c 0x03    /* Clear Weekly Schedule */
#define gZclCmdThermostat_GetRelayStatusLog_c   0x04    /* Get Relay Status Log */

/* Thermostat Server Commands Sent */
#define gZclCmdThermostat_GetWeeklyScheduleRsp_c 0x00  /* Get Weekly Schedule Rsp */
#define gZclCmdThermostat_GetRelayStatusLogRsp_c 0x01  /* Get Relay Status Log Rsp*/

#define gThermostat_HeatSetpointFieldPresent_d 1<<0
#define gThermostat_CoolSetpointFieldPresent_d 1<<1
typedef PACKED_STRUCT zclThermostatAttrsRAM_tag
{
   /*Attributes of the Thermostat Information attribute set */
  int16_t   LocalTemperature[zclReportableCopies_c];        /* Local or Outdoor temperature */
#if gZclClusterOptionals_d
  uint16_t  OutdoorTemperature;
  uint8_t   Occupancy; 
  int16_t   AbsMinHeatSetpointLimit;
  int16_t   AbsMaxHeatSetpointLimit;
  int16_t   AbsMinCoolSetpointLimit;
  int16_t   AbsMaxCoolSetpointLimit;
  uint8_t   PICoolingDemand[zclReportableCopies_c];
  uint8_t   PIHeatingDemand[zclReportableCopies_c];
  zclThermostatSystemTypeConfig_t   SystemTypeConfig;
#endif

  /*Attributes of the Thermostat settings attribute set */
  int16_t   OccupiedCoolingSetpoint[zclReportableCopies_c];
  int16_t   OccupiedHeatingSetpoint[zclReportableCopies_c];
  int16_t   MinHeatSetpointLimit;
  int16_t   MaxHeatSetpointLimit;
  int16_t   MinCoolSetpointLimit;
  int16_t   MaxCoolSetpointLimit;
  int8_t    MinSetpointDeadBand;	
  uint8_t   ControlSequenceOfOperation;
  uint8_t   SystemMode[zclReportableCopies_c];
	
#if gZclClusterOptionals_d
  int8_t    LocalTemperatureCalibration;
  int16_t   UnoccupiedCoolingSetpoint;
  int16_t   UnoccupiedHeatingSetpoint;
  zclThermostatRemoteSensing_t   Remotesensing;
  uint8_t   AlarmMask;
  uint8_t   RunningMode;
  /* Thermostat Schedule & HVAC Relay Attribute Set */
  uint8_t   StartOfWeek;
  uint8_t   NumberOfWeeklyTransitions;
  uint8_t   NumberOfDailyTransitions;
  uint8_t   TempSetpointHold;
  uint16_t  TempSetpointHoldDuration;
  zclThermostatProgOperationMode_t  ProgOperationMode;
  zclThermostatRunningState_t       RunningState; 
  /* Thermostat Setpoint Change Tracking Attribute Set */
  uint8_t    SetpointChangeSource;
  int16_t    SetpointChangeAmount;
  uint32_t   SetpointChangeSourceTimestamp;
  /* Thermostat AC information Attribute Set */
  uint8_t    AcType;
  uint16_t   AcCapacity;
  uint8_t    AcRefrigerantType;
  uint8_t    AcCompresorType;
  uint32_t   AcErrorCode;
  uint8_t    AcLouverPosition;
  int16_t    AcCoilTemp;
  uint8_t    AcCapacityFormat;   
#endif
} zclThermostatAttrsRAM_t;

typedef PACKED_STRUCT thermostatSetPoint_tag
{
  uint16_t  TransitionTime;
  int16_t   HeatSetPoint;
  int16_t   CoolSetPoint;
} thermostatSetPoint_t;

typedef PACKED_STRUCT thermostatSetPointHeat_tag
{
  uint16_t  TransitionTime;
  int16_t   HeatSetPoint;
} thermostatSetPointHeat_t;

typedef PACKED_STRUCT thermostatSetPointCool_tag
{
  uint16_t  TransitionTime;
  int16_t   CoolSetPoint;
} thermostatSetPointCool_t;

typedef PACKED_STRUCT transitionSetPoint_tag
{
  uint8_t               TransitionDayOfWeek;
  thermostatSetPoint_t  SetPoint;
} transitionSetPoint_t;


/* Data structure for the Thermostat weekly schedule, 7 days plus vacation */
typedef PACKED_STRUCT haThermostatWeeklyScheduleRAM_tag
{
  uint8_t              ModeForSequency[8]; 
  thermostatSetPoint_t WeeklySetPoints[8][gHaThermostatWeeklyScheduleEntries_d];
} haThermostatWeeklyScheduleRAM_t;

/* 6.3.2.3.1  Thermostat Setpoint Raise/Lower Command */
typedef PACKED_STRUCT zclCmdThermostat_SetpointRaiseLower_tag
{
  uint8_t                   Mode;
  int8_t                    Amount;   /* in steps per second */
} zclCmdThermostat_SetpointRaiseLower_t;

/* Setpoint Raise/Lower Request frame */
typedef PACKED_STRUCT zclThermostat_SetpointRaiseLower_tag
{
  afAddrInfo_t  addrInfo;  /* IN: */
  zclCmdThermostat_SetpointRaiseLower_t cmdFrame;  /* command frame */
} zclThermostat_SetpointRaiseLower_t;

/* Set Weekly Schedule command payload*/
typedef PACKED_STRUCT zclCmdThermostat_SetWeeklySchedule_tag
{
  uint8_t               NumberOfTransactionsForSequence;
  uint8_t               DayOfWeekForSequence;
  uint8_t               ModeForSequence;
  PACKED_UNION{
    thermostatSetPoint_t     ThermostatSetPoint[1];     /* Valid when the modeForSequence have heat and cool setpoint field set to 1 */
    thermostatSetPointCool_t ThermostatSetPointCool[1]; /* Valid when the modeForSequence have cool setpoint field set to 1  and heatField set to 0*/
    thermostatSetPointHeat_t ThermostatSetPointHeat[1]; /* Valid when the modeForSequence have heat setpoint field set to 1  and coolField set to 0*/
  }SetpointInf;
} zclCmdThermostat_SetWeeklySchedule_t;


/* Set Weekly Schedule frame */
typedef PACKED_STRUCT zclThermostat_SetWeeklySchedule_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t zclTransactionId;
  zclCmdThermostat_SetWeeklySchedule_t cmdFrame;
} zclThermostat_SetWeeklySchedule_t;

/* Get Weekly Schedule command payload*/
typedef PACKED_STRUCT zclCmdThermostat_GetWeeklySchedule_tag
{
  uint8_t DaysToReturn;
  uint8_t ModeToReturn;
} zclCmdThermostat_GetWeeklySchedule_t;

/* Get Weekly Schedule frame */
typedef PACKED_STRUCT zclThermostat_GetWeeklySchedule_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t zclTransactionId;
  zclCmdThermostat_GetWeeklySchedule_t cmdFrame;
} zclThermostat_GetWeeklySchedule_t;

/* Clear Weekly Schedule frame */
typedef PACKED_STRUCT zclThermostat_ClearWeeklySchedule_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t zclTransactionId;
} zclThermostat_ClearWeeklySchedule_t;

/* Get Relay Status Log frame */
typedef PACKED_STRUCT zclThermostat_GetRelayStatusLog_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t zclTransactionId;
} zclThermostat_GetRelayStatusLog_t;

/* get weekly schedule rsp frame*/
typedef PACKED_STRUCT zclThermostat_GetWeeklyScheduleRsp_tag
{
  afAddrInfo_t                         addrInfo;
  uint8_t                              zclTransactionId;
  zclCmdThermostat_SetWeeklySchedule_t cmdFrame;
} zclThermostat_GetWeeklyScheduleRsp_t;

/* payload format for the GetRelayStatusLogRsp command*/
typedef PACKED_STRUCT zclCmdThermostat_GetRelayStatusLogRsp_tag
{
  uint16_t      TimeOfDay;
  uint16_t      RelayStatus;
  int16_t       LocalTemperature;
  uint8_t       HumidityInPercentage;
  int16_t       SetPoint;
  uint16_t      UnreadEntries;
} zclCmdThermostat_GetRelayStatusLogRsp_t;

/* get relay status log rsp frame*/
typedef PACKED_STRUCT zclThermostat_GetRelayStatusLogRsp_tag
{
  afAddrInfo_t                            addrInfo;
  uint8_t                                 zclTransactionId;
  zclCmdThermostat_GetRelayStatusLogRsp_t cmdFrame;
} zclThermostat_GetRelayStatusLogRsp_t;

typedef PACKED_STRUCT zlcThermostat_logInf_tag
{
  uint16_t      TimeOfDay;
  uint16_t      RelayStatus;
  int16_t       LocalTemperature;
  uint8_t       HumidityInPercentage;
  int16_t       SetPoint;
}zlcThermostat_logInf_t;

typedef PACKED_STRUCT haThermostatLogSystemRAM_tag
{
  uint16_t               UnreadEntries;
  zlcThermostat_logInf_t logInf[gHaThermostatMaxLogEntries_d];
}haThermostatLogSystemRAM_t;

/******************************************
	Fan Control Cluster
*******************************************/
#define gZclAttrSetFanControl_c                    0x00
#define gZclAttrIdFanControl_FanModeId_c           0x00 /*Fan Mode*/
#define gZclAttrIdFanControl_FanModeSequence_c     0x01 /*Fan Mode Sequence*/

#if (TRUE == gBigEndian_c)
/* 6.4.2.2 Attributes of the Fan Control cluster */
#define gZclAttrFanControl_FanModeId_c           0x0000 /*Fan Mode*/
#define gZclAttrFanControl_FanModeSequence_c     0x0100 /*Fan Mode Sequence*/
#else
#define gZclAttrFanControl_FanModeId_c           0x0000 /*Fan Mode*/
#define gZclAttrFanControl_FanModeSequence_c     0x0001 /*Fan Mode Sequence*/
#endif

enum{
/* 6.4.2.2.1 FanMode attribute values */
    gZcl_FanMode_Off_c    = 0x00, /* Off */
    gZcl_FanMode_Low_c    = 0x01, /* On */
    gZcl_FanMode_Medium_c = 0x02, /* Medium */
    gZcl_FanMode_High_c   = 0x03, /* High */
    gZcl_FanMode_On_c     = 0x04, /* On */
    gZcl_FanMode_Auto_c   = 0x05, /*Auto (the fan speed is self-regulated)*/
    gZcl_FanMode_Smart_c  = 0x06 /*Smart (when the heated/cooled space is occupied, the fan is always on)*/
};

enum {
/* 6.4.2.2.2  FanSequenceOperation attribute values*/
    gZclFanModeSequence_LowMedHigh_c = 0x00, /*  Low/Med/High  */
    gZclFanModeSequence_LowHigh_c = 0x01, /*  Low/High  */
    gZclFanModeSequence_LowMedHighAuto_c = 0x02, /*  Low/Med/High/Auto  */
    gZclFanModeSequence_LowHighAuto_c = 0x03, /*  Low/High/Auto  */
    gZclFanModeSequence_OnAuto_c = 0x04, /*  On/Auto  */
};


typedef PACKED_STRUCT zclFanControlAttrs_tag
{
  uint8_t FanMode;   /* Fan Mode is an enumeration */
  uint8_t FanModeSequence; /* Fan Mode Sequence of operation */
} zclFanControlAttrs_t;


/******************************************
	Thermostat User Interface Configuration
*******************************************/
#define gZclAttrSetThermostatUICfg_c 0x00

#define gZclAttrIdThermostatUserInterface_TempDisplayModeId_c                   0x00 /* Temperature display mode */
#define gZclAttrIdThermostatUserInterface_KeypadLockoutId_c                     0x01 /* Keypad Lockout */
#define gZclAttrIdThermostatUserInterface_ScheduleProgrammingVisibility_c       0x02 /* ScheduleProgrammingVisibility */

#if (TRUE == gBigEndian_c)
/* 6.6.2.2 Attributes of the Thermostat User Interface Configuration cluster */
#define gZclAttrThermostatUserInterface_TempDisplayModeId_c    0x0000 /* Temperature display mode */
#define gZclAttrThermostatUserInterface_KeypadLockoutId_c      0x0100 /* Keypad Lockout */
#define gZclAttrThermostatUserInterface_ScheduleProgrammingVisibility_c       0x0200 /* ScheduleProgrammingVisibility */

#else
#define gZclAttrThermostatUserInterface_TempDisplayModeId_c    0x0000 /* Temperature display mode */
#define gZclAttrThermostatUserInterface_KeypadLockoutId_c      0x0001 /* Keypad Lockout */
#define gZclAttrThermostatUserInterface_ScheduleProgrammingVisibility_c       0x0002 /* ScheduleProgrammingVisibility */

#endif

enum {
/* KeypadLockout Attribute values */
    gZclKeypad_NoLockout_c = 0x00,
    gZclKeypad_LockoutLevel1_c = 0x01,
    gZclKeypad_LockoutLevel2_c = 0x02,
    gZclKeypad_LockoutLevel3_c = 0x03,
    gZclKeypad_LockoutLevel4_c = 0x04,
    gZclKeypad_LockoutLevel5_c = 0x05
};

enum {
/* Schedule Programming Visibility attribute values */  
    gZclScheduleProgrammingVisibility_Enabled_c  = 0x00, /* Local schedule programming functionality is enabled at the thermostat */
    gZclScheduleProgrammingVisibility_Disabled_c = 0x01  /* Local Schedule programming functionality is disabled at the thermostat */
 /* reserved: 0x02 -0xFF */    
};

/* No commands are generated by the server or recived by the client except responses to commands 
    to read and write the attributes of the server */

typedef PACKED_STRUCT zclThermostatUICfgAttrsRAM_tag
{
  uint8_t   DisplayMode;   /* Display mode is an enumeration */
  uint8_t   KeyPadLockout;
  uint8_t   ScheduleProgrammingVisibility;
} zclThermostatUICfgAttrsRAM_t;

/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/
/*!
 * @fn 		zbStatus_t ZCL_ThermostatSetpointRaiseLowerReq(zclThermostat_SetpointRaiseLower_t *pReq) 
 *
 * @brief	Sends over-the-air a SetpointRaiseLower Request from the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatSetpointRaiseLowerReq(zclThermostat_SetpointRaiseLower_t *pReq);
/*!
 * @fn 		zclStatus_t ZCL_ThermostatSetpointRaiseLower(zbEndPoint_t endPoint,  zclCmdThermostat_SetpointRaiseLower_t* pReq)
 *
 * @brief	Process SetpointRaiseLower Command received from the Thermostat Cluster Client.
 *
 */
zclStatus_t ZCL_ThermostatSetpointRaiseLower(zbEndPoint_t endPoint, zclCmdThermostat_SetpointRaiseLower_t* pReq);
/*!
 * @fn 		zbStatus_t ZCL_ThermostatSetWeeklyScheduleReq(zclThermostat_SetWeeklySchedule_t *pReq) 
 *
 * @brief	Sends over-the-air a SetWeeklySchedule Request from the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatSetWeeklyScheduleReq(zclThermostat_SetWeeklySchedule_t *pReq);
/*!
 * @fn 		zbStatus_t ZCL_ProcessSetWeeklySchedule(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Process SetWeeklySchedule Command received from the Thermostat Cluster Client.
 *
 */
zbStatus_t ZCL_ProcessSetWeeklySchedule(zbApsdeDataIndication_t *pIndication);
/*!
 * @fn 		zbStatus_t ZCL_ThermostatGetWeeklyScheduleReq(zclThermostat_GetWeeklySchedule_t *pReq) 
 *
 * @brief	Sends over-the-air a GetWeeklySchedule Request from the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatGetWeeklyScheduleReq(zclThermostat_GetWeeklySchedule_t *pReq);
/*!
 * @fn 		zbStatus_t ZCL_ProcessGetWeeklySchedule(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Process GetWeeklySchedule Command received from the Thermostat Cluster Client.
 *
 */
zbStatus_t ZCL_ProcessGetWeeklySchedule(zbApsdeDataIndication_t *pIndication);

/*!
 * @fn 		zbStatus_t ZCL_ThermostatClearWeeklyScheduleReq(zclThermostat_ClearWeeklySchedule_t *pReq) 
 *
 * @brief	Sends over-the-air a ClearWeeklySchedule Request from the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatClearWeeklyScheduleReq(zclThermostat_ClearWeeklySchedule_t *pReq);
/*!
 * @fn 		zbStatus_t ZCL_ThermostatClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Thermostat Cluster Server. 
 *
 */
zbStatus_t ZCL_ThermostatClusterServer(zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_ThermostatClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatClusterClient(zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
/*!
 * @fn 		zclStatus_t ZCL_ThermostatClearWeeklySchedule(haThermostatWeeklyScheduleRAM_t *pData)
 *
 * @brief	Interface assumptions: pData is not null.
 *
 */
zbStatus_t ZCL_ThermostatClearWeeklySchedule(haThermostatWeeklyScheduleRAM_t *pData);
/*!
 * @fn 		zbStatus_t ZCL_GetRelayStatusLogRsp(zclThermostat_GetRelayStatusLogRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a GetRelayStatusLogResponse command from the Thermostat Cluster Server. 
 *
 */
zbStatus_t ZCL_GetRelayStatusLogRsp( zclThermostat_GetRelayStatusLogRsp_t *pReq);
/*!
 * @fn 		zbStatus_t ZCL_ThermostatGetRelayStatusLogReq(zclThermostat_GetRelayStatusLog_t *pReq) 
 *
 * @brief	Sends over-the-air a GetRelayStatusLog Request from the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatGetRelayStatusLogReq(zclThermostat_GetRelayStatusLog_t *pReq);
/*!
 * @fn 		zbStatus_t ZCL_ThermostatGetRelayStatusLog(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Process GetRelayStatusLog Command received from the Thermostat Cluster Client.
 *
 */
zbStatus_t ZCL_ThermostatGetRelayStatusLog(zbApsdeDataIndication_t *pIndication);
/*!
 * @fn 		zbStatus_t ZCL_FanControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the FanControl Cluster Server. 
 *
 */
zbStatus_t ZCL_FanControlClusterServer(zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_ThermostatUserInterfaceCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ThermostatUserInterfaceConfiguration Cluster Server. 
 *
 */
zbStatus_t ZCL_ThermostatUserInterfaceCfgClusterServer(zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
#if gAddValidationFuncPtrToClusterDef_c
/*!
 * @fn 		bool_t  ZCL_ThermostatValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Validation function for thermostat attributes
 *
 */
bool_t  ZCL_ThermostatValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pAttrDef);
/*!
 * @fn 		bool_t  ZCL_ThermostatUserCfgValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Validation function for thermostat User Interface Configuration attributes
 *
 */
bool_t  ZCL_ThermostatUserCfgValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pAttrDef);
#else
#define ZCL_ThermostatValidateAttributes
#define ZCL_ThermostatUserCfgValidateAttributes
#endif

#ifdef __cplusplus
}
#endif
#endif
