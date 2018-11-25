/*! @file 	  ZclHVAC.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for the HVAC(Heating/Ventilation/Air-Conditioning) domain
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
#include "ZclHVAC.h"

#include "ZCLHvac.h"
#include "HaProfile.h"

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
  Pump Configuration and Control Cluster Data
  See ZCL Specification Section 6.2
*******************************/

/******************************
  Thermostat Cluster Data
  See ZCL Specification Section 6.3
*******************************/

haThermostatWeeklyScheduleRAM_t gHaThermostatWeeklyScheduleData;
haThermostatLogSystemRAM_t gHAThermostatLog = {
   0x01,        /* unread Entries */   
   {{0x0168,     /* Time of Day = 360 minutes since midnight*/
   0x07,        /* Relay Status*/        
   0x0960,      /* Local Temperature */       
   0x0A,        /* Humidity [percentage] */
   0x0960},     /* Setpoint */
   {0, 0, 0, 0, 0}}
};

const zclAttrDef_t gaZclThermostatClusterAttrDef[] = {
   /*Attributes of the Thermostat Information attribute set */
  { gZclAttrIdThermostat_LocalTemperatureId_c,         gZclDataTypeInt16_c,gZclAttrFlagsRdOnly_c| gZclAttrFlagsInRAM_c|gZclAttrFlagsReportable_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,LocalTemperature)},
#if gZclClusterOptionals_d
  { gZclAttrIdThermostat_OutdoorTemperatureId_c,       gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,OutdoorTemperature)},
  { gZclAttrIdThermostat_OccupancyId_c,                gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,Occupancy)},
  { gZclAttrIdThermostat_AbsMinHeatSetpointLimitId_c,  gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AbsMinHeatSetpointLimit)},
  { gZclAttrIdThermostat_AbsMaxHeatSetpointLimitId_c,  gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AbsMaxHeatSetpointLimit)},
  { gZclAttrIdThermostat_AbsMinCoolSetpointLimitId_c,  gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AbsMinCoolSetpointLimit)},
  { gZclAttrIdThermostat_AbsMaxCoolSetpointLimitId_c,  gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AbsMaxCoolSetpointLimit)},
  { gZclAttrIdThermostat_PICoolingDemandId_c,          gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsReportable_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,PICoolingDemand)},
  { gZclAttrIdThermostat_PIHeatingDemandId_c,          gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsReportable_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,PIHeatingDemand)},
  { gZclAttrIdThermostat_SystemTypeConfiguration_c,    gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,SystemTypeConfig)},
#endif
    /*Attributes of the Thermostat settings attribute set */
  {gZclAttrIdThermostat_OccupiedCoolingSetpointId_c,   gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsReportable_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,OccupiedCoolingSetpoint)},
  {gZclAttrIdThermostat_OccupiedHeatingSetpointId_c,   gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsReportable_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,OccupiedHeatingSetpoint)},
  {gZclAttrIdThermostat_MinHeatSetpointLimitId_c,      gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,MinHeatSetpointLimit)},
  {gZclAttrIdThermostat_MaxHeatSetpointLimitId_c,      gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,MaxHeatSetpointLimit)},
  {gZclAttrIdThermostat_MinCoolSetpointLimitId_c,      gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,MinCoolSetpointLimit)},
  { gZclAttrIdThermostat_MaxCoolSetpointLimitId_c,     gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,MaxCoolSetpointLimit)},
  { gZclAttrIdThermostat_MinSetpointDeadBandId_c,      gZclDataTypeInt8_c,gZclAttrFlagsInRAM_c, sizeof(int8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,MinSetpointDeadBand)},
  { gZclAttrIdThermostat_ControlSequenceOfOperationId_c,     gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,ControlSequenceOfOperation)},
  { gZclAttrIdThermostat_SystemModeId_c,gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsReportable_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,SystemMode)}
#if gZclClusterOptionals_d
  ,{gZclAttrIdThermostat_LocalTemperatureCalibrationId_c,   gZclDataTypeInt8_c,gZclAttrFlagsInRAM_c, sizeof(int8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,LocalTemperatureCalibration)},
  {gZclAttrIdThermostat_UnoccupiedCoolingSetpointId_c,      gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,UnoccupiedCoolingSetpoint)},
  {gZclAttrIdThermostat_UnoccupiedHeatingSetpointId_c,      gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,UnoccupiedHeatingSetpoint)},
  { gZclAttrIdThermostat_RemoteSensingId_c,                 gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,Remotesensing)},
  { gZclAttrIdThermostat_AlarmMaskId_c,                     gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AlarmMask)},
  { gZclAttrIdThermostat_RunningModeId_c,                   gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,RunningMode)},
  /* Thermostat Schedule & HVAC Relay Attribute Set */  
  { gZclAttrIdThermostat_StartOfWeek_c,                    gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,StartOfWeek)},
  { gZclAttrIdThermostat_NumberOfWeeklyTransitions_c,      gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,NumberOfWeeklyTransitions)},
  { gZclAttrIdThermostat_NumberOfDailyTransitions_c,       gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,NumberOfDailyTransitions)},
  { gZclAttrIdThermostat_TemperatureSetpointHold_c,        gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,TempSetpointHold)},
  { gZclAttrIdThermostat_TemperatureSetpointHoldDuration_c,gZclDataTypeUint16_c,gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,TempSetpointHoldDuration)},
  { gZclAttrIdThermostat_ProgrammingOperationMode_c,       gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsReportable_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,ProgOperationMode)},
  { gZclAttrIdThermostat_RunningState_c,                   gZclDataTypeBitmap16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,RunningState)},
  /* Thermostat Setpoint Change Tracking Attribute Set */
  { gZclAttrIdThermostat_SetpointChangeSource_c,       gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,SetpointChangeSource)},
  { gZclAttrIdThermostat_SetpointChangeAmount_c,       gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,SetpointChangeAmount)},
  { gZclAttrIdThermostat_SetpointChangeSourceTimestamp_c, gZclDataTypeUint32_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint32_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,SetpointChangeSourceTimestamp)},
  /* Thermostat AC information Attribute Set */
  { gZclAttrIdThermostat_AcType_c,            gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AcType)},
  { gZclAttrIdThermostat_AcCapacity_c,        gZclDataTypeUint16_c,gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AcCapacity)},
  { gZclAttrIdThermostat_AcRefrigerantType_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AcRefrigerantType)},
  { gZclAttrIdThermostat_AcCompresorType_c,   gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AcCompresorType)},
  { gZclAttrIdThermostat_AcErrorCode_c,       gZclDataTypeUint32_c,gZclAttrFlagsInRAM_c, sizeof(uint32_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AcErrorCode)},
  { gZclAttrIdThermostat_AcLouverPosition_c,  gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AcLouverPosition)},
  { gZclAttrIdThermostat_AcCoilTemperature_c, gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AcCoilTemp)},
  { gZclAttrIdThermostat_AcCapacityFormat_c,  gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AcCapacityFormat)},
#endif
};

const zclAttrSet_t gaZclThermostatClusterAttrSet[] = {
   {gZclAttrSetThermostat_c, (void *)&gaZclThermostatClusterAttrDef, NumberOfElements(gaZclThermostatClusterAttrDef)}
};

const zclAttrSetList_t gZclThermostatClusterAttrSetList = {
  NumberOfElements(gaZclThermostatClusterAttrSet),
  gaZclThermostatClusterAttrSet
};

const zclCmd_t gaZclThermostatClusterCmdReceivedDef[]={
  //6.3.2.3 commands received 
  gZclCmdThermostat_SetpointRaiseLower_c,
  gZclCmdThermostat_SetWeeklySchedule_c,
  gZclCmdThermostat_GetWeeklySchedule_c,
  gZclCmdThermostat_ClearWeeklySchedule_c,
  gZclCmdThermostat_GetRelayStatusLog_c
};

const zclCmd_t gaZclThermostatClusterCmdGeneratedDef[]={
  //6.3.2.4 commands generated 
  gZclCmdThermostat_GetWeeklyScheduleRsp_c,
  gZclCmdThermostat_GetRelayStatusLogRsp_c
};

const zclCommandsDefList_t gZclThermostaClusterCommandsDefList =
{
   NumberOfElements(gaZclThermostatClusterCmdReceivedDef),  gaZclThermostatClusterCmdReceivedDef,
   NumberOfElements(gaZclThermostatClusterCmdGeneratedDef), gaZclThermostatClusterCmdGeneratedDef
};


/******************************
  Fan Control Cluster Data
  See ZCL Specification Section 6.4
*******************************/
zclFanControlAttrs_t gZclFanControlAttrs;

const zclAttrDef_t gaZclFanControlClusterAttrDef[] = {
  { gZclAttrIdFanControl_FanModeId_c,gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t), (void *) MbrOfs(zclFanControlAttrs_t, FanMode)},
  { gZclAttrIdFanControl_FanModeSequence_c,gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t), (void *) MbrOfs(zclFanControlAttrs_t, FanModeSequence)}
  
};

const zclAttrSet_t gaZclFanControlClusterAttrSet[] = {
  {gZclAttrSetFanControl_c, (void *)&gaZclFanControlClusterAttrDef, NumberOfElements(gaZclFanControlClusterAttrDef)}
};


const zclAttrSetList_t gZclFanControlClusterAttrSetList = {
  NumberOfElements(gaZclFanControlClusterAttrSet),
  gaZclFanControlClusterAttrSet
};

/******************************
  Dehumidification Cluster Data
  See ZCL Specification Section 6.5
*******************************/

/******************************
  Thermostat User Interface Configuration Cluster Data
  See ZCL Specification Section 6.6
*******************************/
const zclAttrDef_t gaZclThermostatUICfgClusterAttrDef[] = {
  {gZclAttrIdThermostatUserInterface_TempDisplayModeId_c,gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t),(void*) MbrOfs(zclThermostatUICfgAttrsRAM_t,DisplayMode)},
  {gZclAttrIdThermostatUserInterface_KeypadLockoutId_c,gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t),(void*) MbrOfs(zclThermostatUICfgAttrsRAM_t,KeyPadLockout)}
#if gZclClusterOptionals_d
  ,{gZclAttrIdThermostatUserInterface_ScheduleProgrammingVisibility_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void*)MbrOfs(zclThermostatUICfgAttrsRAM_t,ScheduleProgrammingVisibility)}
#endif    
};

const zclAttrSet_t gaZclThermostatUICfgClusterAttrSet[] = {
  {gZclAttrSetThermostatUICfg_c, (void *)&gaZclThermostatUICfgClusterAttrDef, NumberOfElements(gaZclThermostatUICfgClusterAttrDef)}
};

const zclAttrSetList_t gZclThermostatUICfgClusterAttrSetList = {
  NumberOfElements(gaZclThermostatUICfgClusterAttrSet),
  gaZclThermostatUICfgClusterAttrSet
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************
  Pump Configuration and Control Cluster
  See ZCL Specification Section 6.2
*******************************/

/******************************
  Thermostat Cluster
  See ZCL Specification Section 6.3
*******************************/


/*!
 * @fn 		zbStatus_t ZCL_ThermostatClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Thermostat Cluster Server. 
 *
 */
zbStatus_t ZCL_ThermostatClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    zclFrame_t *pFrame;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    zclUIEvent_t event;
    /*
      BUGBUG: Parameter not used.
    */
    (void)pDevice;
    /* ZCL frame */
    pFrame = (zclFrame_t*)pIndication->pAsdu;
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;
    
    /* handle the LevelControl commands */
    switch(pFrame->command)
    {
      /* Thermostat Setpoint Raise Lower */
    case gZclCmdThermostat_SetpointRaiseLower_c:
      {
        zclCmdThermostat_SetpointRaiseLower_t cmdPayload;
        FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdThermostat_SetpointRaiseLower_t));
        (void)ZCL_ThermostatSetpointRaiseLower(pIndication->dstEndPoint,&cmdPayload);
        if (cmdPayload.Amount > 0)
          event = gZclUI_ThermostatRaiseSetpoint_c;
        else
          event = gZclUI_ThermostatLowerSetpoint_c;

        BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
        break;
      }
    case gZclCmdThermostat_SetWeeklySchedule_c:
    {
      zbStatus_t statusCmd = ZCL_ProcessSetWeeklySchedule(pIndication);
      if(statusCmd != gZclSuccess_c)
    	  status = statusCmd;
      break;
    }
    case gZclCmdThermostat_GetWeeklySchedule_c:
      status = ZCL_ProcessGetWeeklySchedule(pIndication);
      break;
    case gZclCmdThermostat_ClearWeeklySchedule_c:
      (void)ZCL_ThermostatClearWeeklySchedule(&gHaThermostatWeeklyScheduleData);
      break;
    case gZclCmdThermostat_GetRelayStatusLog_c:
      status = ZCL_ThermostatGetRelayStatusLog(pIndication);
      break;  
      /* command not supported on this cluster */
    default:
      return gZclUnsupportedClusterCommand_c;
    }
    return status;
}

/*!
 * @fn 		zbStatus_t ZCL_ThermostatClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    zclFrame_t *pFrame;
    zbStatus_t status = gZclSuccessDefaultRsp_c;

    /*
      BUGBUG: Parameter not used.
    */
    (void)pDevice;
    /* ZCL frame */
    pFrame = (zclFrame_t*)pIndication->pAsdu;

    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
       status = gZclSuccess_c;  
    
    
    /* handle the LevelControl commands */
    switch(pFrame->command)
    {
    case gZclCmdThermostat_GetWeeklyScheduleRsp_c:
    case gZclCmdThermostat_GetRelayStatusLogRsp_c:
      break;
      /* command not supported on this cluster */
    default:
      return gZclUnsupportedClusterCommand_c;
    }
    return status;
}

/* Server Sent Commands */

/*!
 * @fn 		zbStatus_t ZCL_GetWeeklyScheduleRsp(zclThermostat_GetWeeklyScheduleRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a GetWeekDayScheduleResponse command from the Thermostat Cluster Server. 
 *
 */
zbStatus_t ZCL_GetWeeklyScheduleRsp
(
  zclThermostat_GetWeeklyScheduleRsp_t *pReq
)
{
  uint8_t cmdSize, setpointSize;
  if(pReq->cmdFrame.ModeForSequence&gThermostat_CoolSetpointFieldPresent_d)
  {
    if(pReq->cmdFrame.ModeForSequence&gThermostat_HeatSetpointFieldPresent_d)
      setpointSize =  sizeof(thermostatSetPoint_t);
    else
       setpointSize =  sizeof(thermostatSetPointHeat_t);     
  }
  else
    setpointSize =  sizeof(thermostatSetPointCool_t);  
  
  cmdSize = sizeof(zclCmdThermostat_SetWeeklySchedule_t) + (((pReq->cmdFrame.NumberOfTransactionsForSequence) - 1) * setpointSize);

  return ZCL_SendServerRspSeqPassed(gZclCmdThermostat_GetWeeklyScheduleRsp_c, cmdSize, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_GetRelayStatusLogRsp(zclThermostat_GetRelayStatusLogRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a GetRelayStatusLogResponse command from the Thermostat Cluster Server. 
 *
 */
zbStatus_t ZCL_GetRelayStatusLogRsp
(
  zclThermostat_GetRelayStatusLogRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdThermostat_GetRelayStatusLogRsp_c, sizeof(zclCmdThermostat_GetRelayStatusLogRsp_t), (zclGenericReq_t *)pReq);
}


/* Client Sent Commands */
/*!
 * @fn 		zbStatus_t ZCL_ThermostatSetpointRaiseLowerReq(zclThermostat_SetpointRaiseLower_t *pReq) 
 *
 * @brief	Sends over-the-air a SetpointRaiseLower Request from the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatSetpointRaiseLowerReq
(
  zclThermostat_SetpointRaiseLower_t *pReq
)
{
  uint8_t iPayloadLen;
  afToApsdeMessage_t *pMsg;
  iPayloadLen = sizeof(zclCmdThermostat_SetpointRaiseLower_t);
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), gZclCmdThermostat_SetpointRaiseLower_c,gZclFrameControl_FrameTypeSpecific,
                NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
  
 return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen,pMsg);
}

/*!
 * @fn 		zbStatus_t ZCL_ThermostatSetWeeklyScheduleReq(zclThermostat_SetWeeklySchedule_t *pReq) 
 *
 * @brief	Sends over-the-air a SetWeeklySchedule Request from the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatSetWeeklyScheduleReq
(
  zclThermostat_SetWeeklySchedule_t *pReq
)
{
  uint8_t cmdSize = sizeof(zclCmdThermostat_SetWeeklySchedule_t) + (((pReq->cmdFrame.NumberOfTransactionsForSequence) - 1) * sizeof(transitionSetPoint_t));
  return ZCL_SendClientReqSeqPassed(gZclCmdThermostat_SetWeeklySchedule_c, cmdSize, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_ThermostatGetWeeklyScheduleReq(zclThermostat_GetWeeklySchedule_t *pReq) 
 *
 * @brief	Sends over-the-air a GetWeeklySchedule Request from the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatGetWeeklyScheduleReq
(
  zclThermostat_GetWeeklySchedule_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdThermostat_GetWeeklySchedule_c, sizeof(zclCmdThermostat_GetWeeklySchedule_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_ThermostatClearWeeklyScheduleReq(zclThermostat_ClearWeeklySchedule_t *pReq) 
 *
 * @brief	Sends over-the-air a ClearWeeklySchedule Request from the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatClearWeeklyScheduleReq
(
  zclThermostat_ClearWeeklySchedule_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdThermostat_ClearWeeklySchedule_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_ThermostatGetRelayStatusLogReq(zclThermostat_GetRelayStatusLog_t *pReq) 
 *
 * @brief	Sends over-the-air a GetRelayStatusLog Request from the Thermostat Cluster Client. 
 *
 */
zbStatus_t ZCL_ThermostatGetRelayStatusLogReq
(
  zclThermostat_GetRelayStatusLog_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdThermostat_GetRelayStatusLog_c, 0, (zclGenericReq_t *)pReq);
}


/*!
 * @fn 		void ZCL_SetThermostatAttribute(zbEndPoint_t endPoint, zbClusterId_t aClusterId,zclAttrId_t attrId, int16_t attrValue) 
 *
 * @brief	Set Thermostat Attribute
 *
 */
void ZCL_SetThermostatAttribute(zbEndPoint_t endPoint, zbClusterId_t aClusterId,zclAttrId_t attrId, int16_t attrValue) 
{
  int16_t Temp16;
  Temp16 = Native2OTA16(attrValue);
  (void) ZCL_SetAttribute(endPoint,aClusterId,attrId, gZclServerAttr_c,&Temp16);
}

/*!
 * @fn 		zclStatus_t ZCL_ThermostatSetpointRaiseLower(zbEndPoint_t endPoint,  zclCmdThermostat_SetpointRaiseLower_t* pReq)
 *
 * @brief	Process SetpointRaiseLower Command received from the Thermostat Cluster Client.
 *
 */
zclStatus_t ZCL_ThermostatSetpointRaiseLower
(
  zbEndPoint_t endPoint,
  zclCmdThermostat_SetpointRaiseLower_t* pReq
)
{
  int16_t OccupiedCoolingSetpoint;
  int16_t OccupiedHeatingSetpoint;
  int16_t TemperatureCoolSetpoint=0;
  int16_t TemperatureHeatSetpoint =0;
  int16_t  SetpointDiference;
  uint8_t Attrlen;
  int16_t MinHeatSetpointLimit;
  int16_t MaxHeatSetpointLimit;
  int16_t MinCoolSetpointLimit;
  int16_t MaxCoolSetpointLimit;
  int16_t Temp16;
  zbClusterId_t aClusterId;
  Set2Bytes(aClusterId, gZclClusterThermostat_c);

/* Get the data from the attributes to do the operations */
/*   ZCL_GetAttribute(zbEndPoint_t ep, zbClusterId_t aClusterId, zclAttrId_t attrId, uint8_t direction, void * pAttrData, uint8_t * pAttrLen) */
(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_OccupiedCoolingSetpointId_c, gZclServerAttr_c,&Temp16, &Attrlen);
 OccupiedCoolingSetpoint = OTA2Native16(Temp16);

(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_OccupiedHeatingSetpointId_c, gZclServerAttr_c,&Temp16, &Attrlen);
 OccupiedHeatingSetpoint = OTA2Native16(Temp16);
 
(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_MinCoolSetpointLimitId_c, gZclServerAttr_c,&Temp16, &Attrlen);
 MinCoolSetpointLimit = OTA2Native16(Temp16);
 
(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_MinHeatSetpointLimitId_c, gZclServerAttr_c,&Temp16, &Attrlen);
 MinHeatSetpointLimit = OTA2Native16(Temp16);
 
(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_MaxCoolSetpointLimitId_c, gZclServerAttr_c,&Temp16, &Attrlen);
 MaxCoolSetpointLimit = OTA2Native16(Temp16);
 
(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_MaxHeatSetpointLimitId_c, gZclServerAttr_c,&Temp16, &Attrlen);
 MaxHeatSetpointLimit = OTA2Native16(Temp16);

  TemperatureCoolSetpoint = (OccupiedCoolingSetpoint);
  TemperatureHeatSetpoint = (OccupiedHeatingSetpoint);
  SetpointDiference =TemperatureCoolSetpoint - TemperatureHeatSetpoint;
  switch(pReq->Mode){
    case (gThermostatModeHeat_c):
         TemperatureHeatSetpoint += (int16_t)(pReq->Amount*10);
      break;
    case (gThermostatModeCool_c):
         TemperatureCoolSetpoint += (int16_t)(pReq->Amount*10);
     break;
    case (gThermostatModeBoth_c):
         TemperatureHeatSetpoint += (int16_t)(pReq->Amount*10);
         TemperatureCoolSetpoint += (int16_t)(pReq->Amount*10);
      break;
  }/*Switch (pReq-> Mode)*/

         if (pReq->Amount < 0){
             if (TemperatureHeatSetpoint < MinHeatSetpointLimit){
                 TemperatureHeatSetpoint = MinHeatSetpointLimit;
                 TemperatureCoolSetpoint = TemperatureHeatSetpoint + SetpointDiference;
                 }
             if (TemperatureCoolSetpoint < MinCoolSetpointLimit){
                 TemperatureCoolSetpoint = MinCoolSetpointLimit;
                 TemperatureHeatSetpoint = TemperatureCoolSetpoint - SetpointDiference;
                 }
             }
         else{
             if (TemperatureHeatSetpoint > MaxHeatSetpointLimit){
                 TemperatureHeatSetpoint = MaxHeatSetpointLimit;
                 TemperatureCoolSetpoint = TemperatureHeatSetpoint + SetpointDiference;
                 }
             if (TemperatureCoolSetpoint > MaxCoolSetpointLimit){
                 TemperatureCoolSetpoint = MaxCoolSetpointLimit;
                 TemperatureHeatSetpoint = TemperatureCoolSetpoint - SetpointDiference;
                 }
              }

  OccupiedCoolingSetpoint = TemperatureCoolSetpoint;
  OccupiedHeatingSetpoint = TemperatureHeatSetpoint;

/* write the new recalculated values on the cluster attributes */
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_OccupiedCoolingSetpointId_c, OccupiedCoolingSetpoint);
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_OccupiedHeatingSetpointId_c, OccupiedHeatingSetpoint);
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_MinCoolSetpointLimitId_c, MinCoolSetpointLimit);
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_MinHeatSetpointLimitId_c, MinHeatSetpointLimit);
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_MaxCoolSetpointLimitId_c, MaxCoolSetpointLimit);
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_MaxHeatSetpointLimitId_c, MaxHeatSetpointLimit);

  return gZbSuccess_c;
}

/*!
 * @fn 		zclStatus_t ZCL_ThermostatClearWeeklySchedule(haThermostatWeeklyScheduleRAM_t *pData)
 *
 * @brief	Interface assumptions: pData is not null.
 *
 */
zclStatus_t ZCL_ThermostatClearWeeklySchedule(haThermostatWeeklyScheduleRAM_t *pData)
{
  index_t i, j;
  /* clear gHaThermostatWeeklyScheduleData */
  for(i=0; i<8; i++)
  {
    pData->ModeForSequency [i] = 0xff;
    for(j=0; j<gHaThermostatWeeklyScheduleEntries_d; j++)
    {
      /* an entry is not in use if transitionTime is ffff */
      pData->WeeklySetPoints[i][j].CoolSetPoint = 0xff;
      pData->WeeklySetPoints[i][j].HeatSetPoint = 0xff;
      pData->WeeklySetPoints[i][j].TransitionTime = 0xffff;
    }
  }
  return gZclSuccess_c;
}

/*!
 * @fn 		zbStatus_t ZCL_ProcessSetWeeklySchedule(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Process SetWeeklySchedule Command received from the Thermostat Cluster Client.
 *
 */
zbStatus_t ZCL_ProcessSetWeeklySchedule
(
  zbApsdeDataIndication_t *pIndication
)
{
  uint8_t  j, k;
  zbStatus_t status = gZclSuccess_c;
  zclCmdThermostat_SetWeeklySchedule_t *pMsg;
  int16_t absMinHeatSetpoint, absMaxHeatSetpoint;
  int16_t absMinCoolSetpoint, absMaxCoolSetpoint;
  
  (void) ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrThermostat_AbsMinHeatSetpointLimitId_c, gZclServerAttr_c,&absMinHeatSetpoint, NULL);
  (void) ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrThermostat_AbsMaxHeatSetpointLimitId_c, gZclServerAttr_c,&absMaxHeatSetpoint, NULL);
  (void) ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrThermostat_AbsMinCoolSetpointLimitId_c, gZclServerAttr_c,&absMinCoolSetpoint, NULL);
  (void) ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrThermostat_AbsMaxCoolSetpointLimitId_c, gZclServerAttr_c,&absMaxCoolSetpoint, NULL);

  
  /* get the set Weekly Schedule command */
  pMsg = (zclCmdThermostat_SetWeeklySchedule_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  
  if(pMsg->NumberOfTransactionsForSequence > gHaThermostatWeeklyScheduleEntries_d)
  {
      /* insuffiecient space */
      return gZclInsufficientSpace_c; 
  }
  
  /* go through all the days for the current entry */
  for(j=0; j<8; j++)
  {
    /* check if the current entry applies to the current day */
    if(pMsg->DayOfWeekForSequence & (1 << j))
    {
      /* check if an entry has allready been added for the current day,
      if not erase all entries for the current day */
      /* go through all the entries for this day */
      gHaThermostatWeeklyScheduleData.ModeForSequency[j] = 0xFF;
      for(k=0; k<gHaThermostatWeeklyScheduleEntries_d; k++)
      {
        /* mark all entries as invalid */
        gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k].TransitionTime = 0xffff;
        gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k].CoolSetPoint = 0xFF;
        gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k].HeatSetPoint = 0xFF;
      }
      /* go through all transactions for this day */
      for(k=0; k<pMsg->NumberOfTransactionsForSequence; k++)
      {
        /* look for a free entry */
        if(gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k].TransitionTime == 0xffff)
        {
          /* add the info to the current entry */
          gHaThermostatWeeklyScheduleData.ModeForSequency[j] = pMsg->ModeForSequence;
          
          if(pMsg->ModeForSequence&gThermostat_CoolSetpointFieldPresent_d)
          {
            if(pMsg->ModeForSequence&gThermostat_HeatSetpointFieldPresent_d)
            {
               if((pMsg->SetpointInf.ThermostatSetPoint[k].CoolSetPoint < absMinCoolSetpoint)||
                 (pMsg->SetpointInf.ThermostatSetPoint[k].CoolSetPoint > absMaxCoolSetpoint)||
                 (pMsg->SetpointInf.ThermostatSetPoint[k].HeatSetPoint < absMinHeatSetpoint)||
                 (pMsg->SetpointInf.ThermostatSetPoint[k].HeatSetPoint > absMaxHeatSetpoint))
                 return gZclInvalidField_c;
              
              FLib_MemCpy(&gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k],
                        &pMsg->SetpointInf.ThermostatSetPoint, sizeof(thermostatSetPoint_t));
            }
            else
            {
              if((pMsg->SetpointInf.ThermostatSetPointCool[k].CoolSetPoint < absMinCoolSetpoint)||
                 (pMsg->SetpointInf.ThermostatSetPointCool[k].CoolSetPoint > absMaxCoolSetpoint))
                 return gZclInvalidField_c;
              gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k].TransitionTime = pMsg->SetpointInf.ThermostatSetPointCool[k].TransitionTime;
              gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k].CoolSetPoint = pMsg->SetpointInf.ThermostatSetPointCool[k].CoolSetPoint;
            }
          }
          else
          {
            if(pMsg->ModeForSequence&gThermostat_HeatSetpointFieldPresent_d)
            {
              if((pMsg->SetpointInf.ThermostatSetPointHeat[k].HeatSetPoint < absMinHeatSetpoint)||
                 (pMsg->SetpointInf.ThermostatSetPointHeat[k].HeatSetPoint > absMaxHeatSetpoint))
                 return gZclInvalidField_c;
              gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k].TransitionTime = pMsg->SetpointInf.ThermostatSetPointHeat[k].TransitionTime;
              gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k].HeatSetPoint = pMsg->SetpointInf.ThermostatSetPointHeat[k].HeatSetPoint;
    
            }
          }
       }
     }
    }
  }
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_ProcessGetWeeklySchedule(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Process GetWeeklySchedule Command received from the Thermostat Cluster Client.
 *
 */
zbStatus_t ZCL_ProcessGetWeeklySchedule
(
  zbApsdeDataIndication_t *pIndication
)
{
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  uint8_t day, entryNumber;
  uint8_t numberOfTransitions = 0;
  zclThermostat_GetWeeklyScheduleRsp_t *pCurrentSchedule;
  zclCmdThermostat_GetWeeklySchedule_t *pMsg;
  
  pCurrentSchedule = AF_MsgAlloc();
  if(!pCurrentSchedule)
  {
    return gZclNoMem_c; 
  }
  /* get the get Weekly Schedule command */
  pMsg = (zclCmdThermostat_GetWeeklySchedule_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  /* build the response */
  AF_PrepareForReply(&pCurrentSchedule->addrInfo, pIndication);
  pCurrentSchedule->zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  pCurrentSchedule->cmdFrame.DayOfWeekForSequence = pMsg->DaysToReturn;
  
  /* go through all the days of the week */
  for(day=0; day < 8; day++)
  {
    /* Check what days must be returned in the Current Weekly Schedule command */
    if(pMsg->DaysToReturn & (1 << day))
    {
      /* go through all the entries for the current day in the gHaThermostatWeeklyScheduleData */
      for(entryNumber=0; entryNumber<gHaThermostatWeeklyScheduleEntries_d; entryNumber++)
      {
        /* if the current entry is valid add it to the response */
        if(gHaThermostatWeeklyScheduleData.WeeklySetPoints[day][entryNumber].TransitionTime != 0xffff)
        {
          if(pMsg->ModeToReturn&gThermostat_CoolSetpointFieldPresent_d)
          {
            if(pMsg->ModeToReturn&gThermostat_HeatSetpointFieldPresent_d)
            {
              FLib_MemCpy(&pCurrentSchedule->cmdFrame.SetpointInf.ThermostatSetPoint[numberOfTransitions],
                      &gHaThermostatWeeklyScheduleData.WeeklySetPoints[day][entryNumber],
                      sizeof(thermostatSetPoint_t));            
            }
            else
            {
              pCurrentSchedule->cmdFrame.SetpointInf.ThermostatSetPointCool[numberOfTransitions].CoolSetPoint =
                gHaThermostatWeeklyScheduleData.WeeklySetPoints[day][entryNumber].CoolSetPoint;
              pCurrentSchedule->cmdFrame.SetpointInf.ThermostatSetPointCool[numberOfTransitions].TransitionTime =
                gHaThermostatWeeklyScheduleData.WeeklySetPoints[day][entryNumber].TransitionTime;
            }
          }
          else
          {
            pCurrentSchedule->cmdFrame.SetpointInf.ThermostatSetPointHeat[numberOfTransitions].HeatSetPoint =
               gHaThermostatWeeklyScheduleData.WeeklySetPoints[day][entryNumber].HeatSetPoint;
            pCurrentSchedule->cmdFrame.SetpointInf.ThermostatSetPointHeat[numberOfTransitions].TransitionTime =
                gHaThermostatWeeklyScheduleData.WeeklySetPoints[day][entryNumber].TransitionTime;
    
          }
          numberOfTransitions++;
        }
        /* Maximum 10 entries in one response */
        if(numberOfTransitions == 10)
        {
          /* We send a response with 10 entries and build another response if more entries are requested */
          pCurrentSchedule->cmdFrame.NumberOfTransactionsForSequence = numberOfTransitions;
          status = ZCL_GetWeeklyScheduleRsp(pCurrentSchedule);
          if(status != gZbSuccess_c)
          {
            return status;
          }
          /* Reset numberOfTransitions for the next response */
          numberOfTransitions = 0;
        }
      }
    }
  }
  if(numberOfTransitions)
  {
    pCurrentSchedule->cmdFrame.NumberOfTransactionsForSequence = numberOfTransitions;
    status = ZCL_GetWeeklyScheduleRsp(pCurrentSchedule);
  }
  MSG_Free(pCurrentSchedule);
  return status;
}


/*!
 * @fn 		zbStatus_t ZCL_ThermostatGetRelayStatusLog(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Process GetRelayStatusLog Command received from the Thermostat Cluster Client.
 *
 */
zbStatus_t ZCL_ThermostatGetRelayStatusLog
(
  zbApsdeDataIndication_t *pIndication
)
{
  zclThermostat_GetRelayStatusLogRsp_t cmdRsp;
  uint8_t i;
  
  AF_PrepareForReply(&cmdRsp.addrInfo, pIndication);
  cmdRsp.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;

  if(gHAThermostatLog.UnreadEntries != 0x00)
  {
    cmdRsp.cmdFrame.TimeOfDay = Native2OTA16(gHAThermostatLog.logInf[0].TimeOfDay);
    cmdRsp.cmdFrame.RelayStatus = Native2OTA16(gHAThermostatLog.logInf[0].RelayStatus);
    cmdRsp.cmdFrame.LocalTemperature = Native2OTA16(gHAThermostatLog.logInf[0].LocalTemperature);
    cmdRsp.cmdFrame.HumidityInPercentage = gHAThermostatLog.logInf[0].HumidityInPercentage;
    cmdRsp.cmdFrame.SetPoint = Native2OTA16(gHAThermostatLog.logInf[0].SetPoint);
    gHAThermostatLog.UnreadEntries--;
    /* update the entries */
    for(i=0;i<gHaThermostatMaxLogEntries_d-1; i++)
      gHAThermostatLog.logInf[i] = gHAThermostatLog.logInf[i+1];
    /* complete the command*/
    cmdRsp.cmdFrame.UnreadEntries = Native2OTA16(gHAThermostatLog.UnreadEntries);
  }
  else
  {
    FLib_MemSet(&cmdRsp.cmdFrame, 0x00, sizeof(zclCmdThermostat_GetRelayStatusLogRsp_t));
  }
  return ZCL_GetRelayStatusLogRsp(&cmdRsp);
}

/*!
 * @fn 		bool_t  ZCL_ThermostatValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Validation function for thermostat attributes
 *
 */
#if gAddValidationFuncPtrToClusterDef_c
bool_t  ZCL_ThermostatValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
{ 
  zclCmdWriteAttrRecord_t *pRecord = (zclCmdWriteAttrRecord_t*) pData;  
  zclAttrData_t *pAttrData = (zclAttrData_t*) pRecord->aData;
  uint16_t min, max;
  uint8_t len;
  
  
  switch (pRecord->attrId)
  {    
  case gZclAttrThermostat_LocalTemperatureCalibrationId_c:
    {
      return ((pAttrData->data8 <= 0x19) || (pAttrData->data8 >= 0xE7));     
    }
  case gZclAttrThermostat_OccupiedCoolingSetpointId_c:
  case gZclAttrThermostat_UnoccupiedCoolingSetpointId_c:
    {
      (void) ZCL_GetAttribute(endPoint, clusterId, gZclAttrThermostat_MinCoolSetpointLimitId_c, gZclServerAttr_c,&min, &len);
      (void) ZCL_GetAttribute(endPoint, clusterId, gZclAttrThermostat_MaxCoolSetpointLimitId_c, gZclServerAttr_c,&max, &len);
      min = OTA2Native16(min);
      max = OTA2Native16(max);    
      return ((OTA2Native16(pAttrData->data16) >= min) && (OTA2Native16(pAttrData->data16) <= max));
    }  
  case gZclAttrThermostat_OccupiedHeatingSetpointId_c:  
  case gZclAttrThermostat_UnoccupiedHeatingSetpointId_c:
    {  
      (void) ZCL_GetAttribute(endPoint, clusterId, gZclAttrThermostat_MinHeatSetpointLimitId_c, gZclServerAttr_c,&min, &len);
      (void) ZCL_GetAttribute(endPoint, clusterId, gZclAttrThermostat_MaxHeatSetpointLimitId_c, gZclServerAttr_c,&max, &len);
      min = OTA2Native16(min);
      max = OTA2Native16(max);   		 
      return ((OTA2Native16(pAttrData->data16) >= min) && (OTA2Native16(pAttrData->data16) <= max));
    }    
  case gZclAttrThermostat_MinHeatSetpointLimitId_c:
  case gZclAttrThermostat_MaxHeatSetpointLimitId_c:    
  case gZclAttrThermostat_MinCoolSetpointLimitId_c:
  case gZclAttrThermostat_MaxCoolSetpointLimitId_c:
  case gZclAttrIdThermostat_AbsMinHeatSetpointLimitId_c:
  case gZclAttrIdThermostat_AbsMaxHeatSetpointLimitId_c:
  case gZclAttrIdThermostat_AbsMinCoolSetpointLimitId_c:
  case gZclAttrIdThermostat_AbsMaxCoolSetpointLimitId_c:  
    {    
      return ((OTA2Native16(pAttrData->data16) <= 0x7FFF) || (OTA2Native16(pAttrData->data16) >= 0x954D)); 
    }    
  case gZclAttrThermostat_MinSetpointDeadBandId_c:
    {
      return ((pAttrData->data8 >= 0x0A) && (pAttrData->data8 <= 0x19));
    }  
  case gZclAttrThermostat_ControlSequenceOfOperationId_c:
    {
      return (pAttrData->data8 <= 0x05);
    }  
  case gZclAttrThermostat_SystemModeId_c:
    {
      if((pAttrData->data8 == 0x02)||(pAttrData->data8 > 0x09))
    	  return FALSE;
      return TRUE;
    }
  case gZclAttrThermostat_RemoteSensingId_c:
  case gZclAttrThermostat_AlarmMaskId_c:
    {
      return (pAttrData->data8 <= 0x07);
    }
  case gZclAttrThermostat_StartOfWeek_c:
    {
      return (pAttrData->data8 <= 0x06);
    }
  case gZclAttrThermostat_ProgrammingOperationMode_c:
  case gZclAttrThermostat_SystemTypeConfiguration_c:
    {
      return (pAttrData->data8 <= 0x3F); /*Range : 0x00xxxxxx*/
    }  
  case gZclAttrThermostat_RunningModeId_c:
    {
      return (pAttrData->data8 <= 0x04);
    } 
  case gZclAttrThermostat_TemperatureSetpointHold_c:
    {
      return (pAttrData->data8 <= 0x01);
    }
  case gZclAttrThermostat_AcType_c:
    {
      return (pAttrData->data8 <= 0x04);
    }
  case gZclAttrThermostat_AcCompresorType_c:  
  case gZclAttrThermostat_AcRefrigerantType_c:
    {
      return (pAttrData->data8 <= 0x03);
    } 
  case gZclAttrThermostat_AcLouverPosition_c:
    {
      return (pAttrData->data8 <= 0x05);
    } 
  case gZclAttrThermostat_AcCoilTemperature_c:
    {    
    	return ((OTA2Native16(pAttrData->data16) <= 0x7FFF)||(OTA2Native16(pAttrData->data16) >= 0x954D));
    }   
  case gZclAttrThermostat_TemperatureSetpointHoldDuration_c:
    {
      return ((OTA2Native16(pAttrData->data16) <= 0x05A0) || (OTA2Native16(pAttrData->data16) == 0xFFFF));
    }  
  case gZclAttrThermostat_NumberOfWeeklyTransitions_c:
  default:
    return TRUE;
  }
}

/*!
 * @fn 		bool_t  ZCL_ThermostatUserCfgValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Validation function for thermostat User Interface  Configuration attributes
 *
 */
bool_t  ZCL_ThermostatUserCfgValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
{  
  zclCmdWriteAttrRecord_t *pRecord = (zclCmdWriteAttrRecord_t*) pData;  
  zclAttrData_t *pAttrData = (zclAttrData_t*) pRecord->aData;
 (void) endPoint;
 (void) clusterId;
    
  switch (pRecord->attrId)
  {        
    case gZclAttrThermostatUserInterface_TempDisplayModeId_c:
      {
    return (pAttrData->data8 <= 0x01);        
      }
    case gZclAttrThermostatUserInterface_KeypadLockoutId_c:  
      {
    return (pAttrData->data8 <= 0x05);        
      }
  default:
    return TRUE;
  }
} 
#endif /* gZclEnableAttributeValidation_c */



/******************************
  Fan Control Cluster
  See ZCL Specification Section 6.4
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_FanControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the FanControl Cluster Server. 
 *
 */
zbStatus_t ZCL_FanControlClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    (void) pIndication;
    (void) pDevice;
    return gZclUnsupportedClusterCommand_c;
}

/******************************
  Dehumidification Cluster
  See ZCL Specification Section 6.5
*******************************/

/******************************
  Thermostat User Interface  Configuration Cluster
  See ZCL Specification Section 6.6
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_ThermostatUserInterfaceCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ThermostatUserInterfaceConfiguration Cluster Server. 
 *
 */
zbStatus_t ZCL_ThermostatUserInterfaceCfgClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    (void) pIndication;
    (void) pDevice;
    return gZclUnsupportedClusterCommand_c;
}

