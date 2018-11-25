/*! @file 	  ZclGeneral.c
 *
 * @brief	  This source file describes specific functionality implemented for ZCL General 
 *			  functional domain
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

#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "EmbeddedTypes.h"
#include "NV_Data.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "TMR_Interface.h"
#include "ZdoApsInterface.h"
#include "ApsMgmtInterface.h"
#include "BeeAppInit.h"
#include "BeeCommon.h"
#include "HaProfile.h"
#include "ASL_UserInterface.h"
#include "BeeApp.h"
#include "ZCL.h"
#include "ZdpManager.h"
#include "ZclClosures.h"
#include "zclSensing.h"
#include "zclOta.h"
#include "EndPointConfig.h"
#include "ASL_ZdpInterface.h"
#include "EzCommissioning.h"
#include "ZllCommissioning.h"
#include "ZclSEMessaging.h"
#include "ZclSEPrice.h"

#include "ZDOVariables.h"
#include "ZdoApsInterface.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#ifndef gHostApp_d 
#define gNvZclDataSet_c gNvAppDataSet_ID_c
#else
#define gNvZclDataSet_c gNvDataSet_App_ID_c
#endif

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

/*****************************
* Identify Cluster prototypes
*****************************/
#if gASL_EnableZLLClustersData_d
zbStatus_t ZCL_IdentifyTriggerEffectHandler(zbApsdeDataIndication_t *pIndication);
#endif
static zbStatus_t ZCL_IdentifyQueryRspHandler(zbApsdeDataIndication_t *pIndication);
/*****************************
* Scene Cluster prototypes
*****************************/
zclSceneTableEntry_t *ZCL_AllocateScene(afDeviceDef_t *pDevice, zclCmdScene_StoreScene_t *pSceneId);
zclSceneTableEntry_t *ZCL_FindScene(afDeviceDef_t *pDevice, zclCmdScene_RecallScene_t *pSceneId);
zbStatus_t ZCL_AddScene(zclSceneTableEntry_t *pScene, zclCmdScene_AddScene_t *pAddSceneReq, uint8_t indicationLen, uint8_t commandId);
zbStatus_t ZCL_ViewScene(zclSceneTableEntry_t *pScene,zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice, uint8_t commandId);
void ZCL_GetSceneMembership(afDeviceDef_t *pDevice, zbGroupId_t aGroupId,zbApsdeDataIndication_t *pIndication);
#if gASL_EnableZLLClustersData_d
zbStatus_t ZCL_SceneCopyScene(afDeviceDef_t *pDevice, zbApsdeDataIndication_t *pIndication);
#endif
/*****************************
* Group Cluster prototypes
*****************************/
zbStatus_t ZCL_GroupAddGroupRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_AddGroup_t*command);
zbStatus_t ZCL_GroupAddGroupIfIdentify(zbApsdeDataIndication_t *pIndication,zclCmdGroup_AddGroupIfIdentifying_t*command);
zbStatus_t ZCL_GroupRemoveGroupRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_RemoveGroup_t*command);
zbStatus_t ZCL_GroupRemoveAllGroups(zbApsdeDataIndication_t *pIndication,zclCmdGroup_RemoveAllGroups_t*command);
zclStatus_t ZCL_GroupViewGroupRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_ViewGroup_t* command);
zclStatus_t ZCL_GroupGetGroupMembershipRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_GetGroupMembership_t* command);
/*****************************
* OnOff Cluster prototypes
*****************************/
#if gASL_EnableZLLClustersData_d
zbStatus_t OnOffServer_SetTimeout(uint8_t indexSetup, uint32_t duration);
zbStatus_t OnOffServer_OffWithEffectHandler(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t OnOffServer_OnWithRecallGlobalSceneHandler(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t OnOffServer_OnWithTimedOffHandler(zbApsdeDataIndication_t *pIndication);
#endif
/*****************************
* Level Control Cluster prototypes
*****************************/
zbStatus_t ZCL_LevelControlOnOffCommandsHandler(uint8_t endpoint, uint8_t onOffStatus, uint16_t transitionTime, uint8_t lastCurrentValue);
zbStatus_t ZCL_LevelControlMoveToLevel(zclCmdLevelControl_MoveToLevel_t * pReq, bool_t withOnOff, uint8_t endpoint);
zbStatus_t ZCL_LevelControlMove(zclCmdLevelControl_Move_t * pReq, bool_t withOnOff, uint8_t endpoint);
zclStatus_t ZCL_LevelControlStep(zclCmdLevelControl_Step_t *pReq, bool_t withOnOff, uint8_t endpoint);
zclStatus_t ZCL_LevelControlStop(uint8_t endpoint);
/*****************************
* Poll Control Cluster prototypes
*****************************/
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)
zbStatus_t zclPollControl_StartFastPollMode(uint8_t endpoint, uint16_t timeout);
void zclPollControl_StopFastPollMode(uint8_t endpoint, bool_t timeoutStop);
void ZclPollControl_CheckInCallback(uint8_t tmrId);
void ZclPollControl_FastPollModeCallback(uint8_t tmrId);
static uint16_t ZclPollControl_SearchMaxFastTimeout(uint8_t setupIndex);
static zbStatus_t zclPollControl_ValidateClient(uint8_t addrMode, zbNwkAddr_t aAddr, zbEndPoint_t endPoint);
static bool_t ZclPollControlValidateFastPollTimeout(uint8_t setupIndex, uint16_t value);
static zbStatus_t zclPollControl_ValidateClient(uint8_t addrMode, zbNwkAddr_t aAddr, zbEndPoint_t endPoint);
#endif


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/*****************************
* Group Cluster private data definitions
*****************************/
/* for handling OTA group commands */
typedef union zclAnyGroupReq_tag {
  zbApsmeAddGroupReq_t          addGroup;
  zbApsmeRemoveGroupReq_t       removeGroup;
  zbApsmeRemoveAllGroupsReq_t   removeAllGroups;
} zclAnyGroupReq_t;


/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

/*****************************
* OnOff Cluster Public memory definitions
*****************************/
#if gASL_EnableZLLClustersData_d
zclOnOffServerSetup_t gOnOffServerSetup[gNoOfOnOffServerInstances_d];
#endif
/*****************************
* Level Control Cluster Public memory definitions
*****************************/
uint8_t  gZclLevel_Step=1;
zclLevelControlSetup_t  gLevelControlServerSetup[gNoOfLevelControlServerInstances_d];
/*****************************
* Scene Cluster Public memory definitions
*****************************/
bool_t gSceneClusterEnhancedCommand = FALSE;
/*****************************
* Occupancy Cluster Public memory definitions
*****************************/
zbTmrTimerID_t occupancyTimer;
bool_t gSetAvailableOccupancy = FALSE;
/*****************************
* Shade configuration Cluster Public memory definitions
*****************************/
#if gZclEnableShadeConfigurationCluster_d  
bool_t gShadeDevice = FALSE;
gZclShadeStatus_t   gStatusShadeCfg = {0,0,0,0,0};
bool_t gShadeDeviceActive = FALSE;
#endif

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/******************************
  Basic Cluster Data
  See ZCL Specification Section 3.2
*******************************/

/* Basic Cluster Attribute Definitions */
const zclAttrDef_t gaZclBasicClusterAttrDef[] = {
  { gZclAttrIdBasicZCLVersionId_c,          gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),        (void *)MbrOfs(zclBasicAttrsRAM_t, zclVersion)},
#if gZclClusterOptionals_d
  { gZclAttrIdBasicApplicationVersionId_c,  gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),        (void *)MbrOfs(zclBasicAttrsRAM_t, appVersion)},
  { gZclAttrIdBasicStackVersionId_c,        gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),        (void *)MbrOfs(zclBasicAttrsRAM_t, stackVersion)},
  { gZclAttrIdBasicHWVersionId_c,           gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),        (void *)MbrOfs(zclBasicAttrsRAM_t, hardwareVersion)},
  { gZclAttrIdBasicManufacturerNameId_c,    gZclDataTypeStr_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(zclStr10_t),     (void *)MbrOfs(zclBasicAttrsRAM_t, manufacturerName)}, 
  { gZclAttrIdBasicModelIdentifierId_c,     gZclDataTypeStr_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(zclStr05_t),     (void *)MbrOfs(zclBasicAttrsRAM_t, modelIdentifier)},
  { gZclAttrIdBasicDateCodeId_c,            gZclDataTypeStr_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(zclStr09_t),     (void *)MbrOfs(zclBasicAttrsRAM_t, dateCode)},
#endif
  { gZclAttrIdBasicPowerSourceId_c,         gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsInLine_c, sizeof(uint8_t),        (void *)MbrOfs(zclBasicAttrsRAM_t, powerSourceId)},
#if gZclClusterOptionals_d
  { gZclAttrIdBasicLocationDescriptionId_c, gZclDataTypeStr_c,   gZclAttrFlagsInRAM_c, sizeof(zclStr16_t),      (void *)MbrOfs(zclBasicAttrsRAM_t,locationDescription)},
  { gZclAttrIdBasicPhysicalEnvironmentId_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),       (void *)MbrOfs(zclBasicAttrsRAM_t, physicalEnvironment)},
#endif
  { gZclAttrIdBasicDeviceEnabledId_c,       gZclDataTypeBool_c,  gZclAttrFlagsInRAM_c, sizeof(uint8_t),        (void *)MbrOfs(zclBasicAttrsRAM_t, deviceEnabled)}
#if gZclClusterOptionals_d
  , { gZclAttrIdBasicAlarmMaskId_c,         gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),     (void *)MbrOfs(zclBasicAttrsRAM_t, alarmMask)}
#endif
};

#if gASL_EnableZLLClustersData_d
/* Basic Cluster ZLL Attribute Definitions */
const zclAttrDef_t gaZclBasicZllClusterAttrDef[] = {
  { gZclAttrIdBasic_SoftwareBuild_c, gZclDataTypeStr_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(zclStr09_t),     (void *)MbrOfs(zclBasicAttrsRAM_t, swBuild)},
};
#endif

const zclAttrSet_t gaZclBasicClusterAttrSet[] = {
  {gZclAttrSetBasicDeviceInformation_c, (void *)&gaZclBasicClusterAttrDef, NumberOfElements(gaZclBasicClusterAttrDef)}
#if gASL_EnableZLLClustersData_d
  ,{gZclAttrSetBasicZllData_c, (void *)&gaZclBasicZllClusterAttrDef, NumberOfElements(gaZclBasicZllClusterAttrDef)}
#endif  
};

const zclAttrSetList_t gZclBasicClusterAttrSetList = {
  NumberOfElements(gaZclBasicClusterAttrSet),
  gaZclBasicClusterAttrSet
};

/* Basic Cluster Attribute Definitions */
const zclAttrDef_t gaZclBasicClusterMirrorAttrDef[] = {
  { gZclAttrIdBasicZCLVersionId_c,          gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclBasicAttrMirrorRAM_t,ZCLVersion) },
  { gZclAttrIdBasicPowerSourceId_c,         gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclBasicAttrMirrorRAM_t,iPowerSource) },
};

const zclAttrSet_t gaZclBasicClusterMirrorAttrSet[] = {
  {gZclAttrSetBasicDeviceInformation_c, (void *)&gaZclBasicClusterMirrorAttrDef, NumberOfElements(gaZclBasicClusterMirrorAttrDef)}
};

const zclAttrSetList_t gZclBasicClusterMirrorAttrSetList = {
  NumberOfElements(gaZclBasicClusterMirrorAttrSet),
  gaZclBasicClusterMirrorAttrSet
};

/******************************
  Power Configuration Cluster Data
  See ZCL Specification Section 3.3
*******************************/

/* Power Configuration Cluster Attribute Definitions */
const zclAttrDef_t gaZclPowerCfgClusterAttrDef[] = {
   //mains information
  { gZclAttrPwrConfigIdMainsInfMainsVoltage_c,    gZclDataTypeUint16_c, gZclAttrFlagsInRAMRdOnly_c ,  sizeof(uint16_t), (void *)MbrOfs(zclPowerCfgAttrsRAM_t, mainsVoltage)},
  { gZclAttrPwrConfigIdMainsInfMainsFrequency_c,  gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c ,    sizeof(uint8_t),(void *)MbrOfs(zclPowerCfgAttrsRAM_t, mainsFrequency)},
  //mains settings
  { gZclAttrPwrConfigIdMainsStgMainsAlarmMask_c,             gZclDataTypeBitmap8_c,  gZclAttrFlagsInRAM_c,    sizeof(uint8_t), (void *)MbrOfs(zclPowerCfgAttrsRAM_t, mainsAlarmMask)},
  { gZclAttrPwrConfigIdMainsStgMainsVoltageMinThreshold_c,   gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c,    sizeof(uint16_t),(void *)MbrOfs(zclPowerCfgAttrsRAM_t, mainsVoltageMinThreshold)},
  { gZclAttrPwrConfigIdMainsStgMainsVoltageMaxThreshold_c,   gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c,    sizeof(uint16_t),(void *)MbrOfs(zclPowerCfgAttrsRAM_t, mainsVoltageMaxThreshold)},
  { gZclAttrPwrConfigIdMainsStgMainsVoltageDwellTripPoint_c, gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c,    sizeof(uint16_t),(void *)MbrOfs(zclPowerCfgAttrsRAM_t, mainsVoltageDwellTripPoint)},
  //battery information
  { gZclAttrPwrConfigIdBatteryInfBatteryVoltage_c,    gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c| gZclAttrFlagsReportable_c ,    sizeof(uint8_t), (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryVoltage)},
  { gZclAttrPwrConfigIdBatteryInfBatteryPercentageRemaining_c,    gZclDataTypeUint8_c,  gZclAttrFlagsInRAMRdOnly_c ,    sizeof(uint8_t), (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryPercentageRemaining)},
  //battery settings
  { gZclAttrPwrConfigIdBatteryStgBatteryManufacturer_c,       gZclDataTypeStr_c,     gZclAttrFlagsInRAM_c,   sizeof(zclStr16_t), (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryMfg)},
  { gZclAttrPwrConfigIdBatteryStgBatterySize_c,               gZclDataTypeEnum8_c,   gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batterySize)},
  { gZclAttrPwrConfigIdBatteryStgBatteryAHrRating_c,          gZclDataTypeUint16_c,  gZclAttrFlagsInRAM_c,   sizeof(uint16_t),   (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryAHrRating)},
  { gZclAttrPwrConfigIdBatteryStgBatteryQuantity_c,           gZclDataTypeUint8_c,   gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryQuantity)},
  { gZclAttrPwrConfigIdBatteryStgBatteryRatedVoltage_c,       gZclDataTypeUint8_c,   gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryRatedVoltage)},
  { gZclAttrPwrConfigIdBatteryStgBatteryAlarmMask_c,          gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryAlarmMask)},
  { gZclAttrPwrConfigIdBatteryStgBatteryVoltageMinThreshold_c,gZclDataTypeUint8_c,   gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryVoltageMinThreshold)}, 
  { gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold1_c,  gZclDataTypeUint8_c,   gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryVoltageThreshold1)},
  { gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold2_c,  gZclDataTypeUint8_c,   gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryVoltageThreshold2)},  
  { gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold3_c,  gZclDataTypeUint8_c,   gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryVoltageThreshold3)}, 
  { gZclAttrPwrConfigIdBatteryStgBatteryPercentageMinThreshold_c,gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryPercentageMinThreshold)}, 
  { gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold1_c,  gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryPercentageThreshold1)},
  { gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold2_c,  gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryPercentageThreshold2)},  
  { gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold3_c,  gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c,   sizeof(uint8_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryPercentageThreshold3)},
  { gZclAttrPwrConfigIdBatteryStgBatteryAlarmState_c,           gZclDataTypeBitmap32_c, gZclAttrFlagsNoFlags_c,   sizeof(uint32_t),    (void *)MbrOfs(zclPowerCfgAttrsRAM_t, batteryAlarmState)}
};

const zclAttrSet_t gaZclPowerCfgClusterAttrSet[] = {
  {gZclAttrSetPwrConfig_c, (void *)&gaZclPowerCfgClusterAttrDef, NumberOfElements(gaZclPowerCfgClusterAttrDef)}
};

const zclAttrSetList_t gZclPowerCfgClusterAttrSetList = {
  NumberOfElements(gaZclPowerCfgClusterAttrSet),
  gaZclPowerCfgClusterAttrSet
};


/******************************
  Device Temperature Configuration Cluster Data
  See ZCL Specification Section 3.4
*******************************/

/* Device Temperature Configuration Cluster Attribute Definitions */
const zclAttrDef_t gaZclTemperatureCfgClusterAttrDef[] = {
  //device temperature information 
  { gZclAttrIdTempCfgTempInfCurrentTemp_c, gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, currentTemp)}
#if gZclClusterOptionals_d
  , { gZclAttrIdTempCfgTempInfMinTempExperienced_c, gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, minTempExperienced)},
  { gZclAttrIdTempCfgTempInfMaxTempExperienced_c,   gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, maxTempExperienced)},
  { gZclAttrIdTempCfgTempInfOverTempTotalDwell_c,   gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, overTempTotalDwell)},
#endif  
  //device temperature settings
#if gZclClusterOptionals_d
  { gZclAttrIdTempCfgTempStgDeviceTempAlarmMask_c,    gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),   (void *)MbrOfs(zclTempCfgAttrsRAM_t, deviceTempAlarmMask)},
  { gZclAttrIdTempCfgTempStgLowTempThreshold_c,       gZclDataTypeInt16_c,   gZclAttrFlagsInRAM_c, sizeof(int16_t),   (void *)MbrOfs(zclTempCfgAttrsRAM_t, lowTempThreshold)},
  { gZclAttrIdTempCfgTempStgHighTempThreshold_c,      gZclDataTypeInt16_c,   gZclAttrFlagsInRAM_c, sizeof(int16_t),   (void *)MbrOfs(zclTempCfgAttrsRAM_t, highTempThreshold)},
  { gZclAttrIdTempCfgTempStgLowTempDwellTripPoint_c,  gZclDataTypeUint24_c,  gZclAttrFlagsInRAM_c, sizeof(SumElem_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, lowTempDwellTripPoint)},
  { gZclAttrIdTempCfgTempStgHighTempDwellTripPoint_c, gZclDataTypeUint24_c,  gZclAttrFlagsInRAM_c, sizeof(SumElem_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, highTempDwellTripPoint)}
#endif 
};

const zclAttrSet_t gaZclTemperatureCfgClusterAttrSet[] = {
  {gZclAttrTempCfgSet_c, (void *)&gaZclTemperatureCfgClusterAttrDef, NumberOfElements(gaZclTemperatureCfgClusterAttrDef)}
};

const zclAttrSetList_t gaZclTemperatureCfgClusterAttrSetList = {
  NumberOfElements(gaZclTemperatureCfgClusterAttrSet),
  gaZclTemperatureCfgClusterAttrSet
};

/******************************
  Identify Cluster Data
  See ZCL Specification Section 3.5
*******************************/
const zclAttrDef_t gaZclIdentifyClusterAttrDef[] = {
  { gZclAttrIdentify_TimeId_c,                  gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c, sizeof(uint16_t), (void *)MbrOfs(zclIdentifyAttrsRAM_t, identifyTime)}//(void *)&gZclIdentifyTime}
#if gASL_EnableEZCommissioning_d
  ,{ gZclAttrIdentify_CommissioningStateId_c,   gZclDataTypeBitmap8_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,  sizeof(uint8_t),  (void *)MbrOfs(zclIdentifyAttrsRAM_t, identifyCommissioningState)}
#endif
};

const zclAttrSet_t gaZclIdentifyClusterAttrSet[] = {
  {gZclAttrIdentify_TimeId_c, (void *)&gaZclIdentifyClusterAttrDef, NumberOfElements(gaZclIdentifyClusterAttrDef)}
};

const zclAttrSetList_t gZclIdentifyClusterAttrSetList = {
  NumberOfElements(gaZclIdentifyClusterAttrSet),
  gaZclIdentifyClusterAttrSet
};

const zclCmd_t gaZclIdentifyClusterCmdReceivedDef[]={
  // commands received 
  gZclCmdIdentify_c,
  gZclCmdIdentifyQuery_c,
  gZclCmdEzModeInvoke_c,
  gZclCmdUpdateCommissioningState_c
#if gASL_EnableZLLClustersData_d    
  ,gZclCmdIdentifyTriggerEffect_c
#endif    
};

const zclCmd_t gaZclIdentifyClusterCmdGeneratedDef[]={
  // commands generated 
  gZclCmdIdentifyQueryRsp_c
};

const zclCommandsDefList_t gZclIdentifyClusterCommandsDefList =
{
   NumberOfElements(gaZclIdentifyClusterCmdReceivedDef), gaZclIdentifyClusterCmdReceivedDef,
   NumberOfElements(gaZclIdentifyClusterCmdGeneratedDef), gaZclIdentifyClusterCmdGeneratedDef
};


/******************************
  Groups Cluster Data
  The groups cluster is concerned with management of the group table on a device.
  See ZCL Specification Section 3.6
*******************************/
const zclAttrDef_t gaZclGroupClusterAttrDef[] = {                                                  
  { gZclAttrGroup_NameSupport_c, gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *)MbrOfs(zclGroupAttrsRAM_t, groupNameSupport)}
};

const zclAttrSet_t gaZclGroupClusterAttrSet[] = {
  {gZclAttrGroup_NameSupport_c, (void *)&gaZclGroupClusterAttrDef, NumberOfElements(gaZclGroupClusterAttrDef)}
};

const zclAttrSetList_t gZclGroupClusterAttrSetList = {
  NumberOfElements(gaZclGroupClusterAttrSet),
  gaZclGroupClusterAttrSet
};

const zclCmd_t gaZclGroupClusterCmdReceivedDef[]={
  // 3.6.2 commands received 
  gZclCmdGroup_AddGroup_c,
  gZclCmdGroup_ViewGroup_c,
  gZclCmdGroup_GetGroupMembership_c,
  gZclCmdGroup_RemoveGroup_c,
  gZclCmdGroup_RemoveAllGroups_c,
  gZclCmdGroup_AddGroupIfIdentifying_c
};

const zclCmd_t gaZclGroupClusterCmdGeneratedDef[]={
  // 3.6.2 commands generated 
  gZclCmdGroup_AddGroupRsp_c,
  gZclCmdGroup_ViewGroupRsp_c,
  gZclCmdGroup_GetGroupMembershipRsp_c,
  gZclCmdGroup_RemoveGroupRsp_c
};

const zclCommandsDefList_t gZclGroupClusterCommandsDefList =
{
   NumberOfElements(gaZclGroupClusterCmdReceivedDef), gaZclGroupClusterCmdReceivedDef,
   NumberOfElements(gaZclGroupClusterCmdGeneratedDef), gaZclGroupClusterCmdGeneratedDef
};


/******************************
  Scenes Cluster Data
  See ZCL Specification Section 3.7
*******************************/
uint8_t gZclScene_NameSupport = gZclScene_NameSupport_c;

const zclAttrDef_t gaZclSceneClusterAttrDef[] = {
  { gZclAttrSceneId_SceneCount_c,       gZclDataTypeUint8_c,   gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclSceneAttrs_t, sceneCount)   },
  { gZclAttrSceneId_CurrentScene_c,     gZclDataTypeUint8_c,   gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclSceneAttrs_t, currentScene) },
  { gZclAttrSceneId_CurrentGroup_c,     gZclDataTypeUint16_c,  gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *)MbrOfs(zclSceneAttrs_t, currentGroup) },
  { gZclAttrSceneId_SceneValid_c,       gZclDataTypeBool_c,    gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclSceneAttrs_t, sceneValid) },
  { gZclAttrSceneId_NameSupport_c,      gZclDataTypeBitmap8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t),  (void *)&gZclScene_NameSupport }
#if gZclClusterOptionals_d
  , { gZclAttrSceneId_LastConfiguredBy_c, gZclDataTypeIeeeAddr_c, gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zbIeeeAddr_t),  (void *)MbrOfs(zclSceneAttrs_t, lastConfiguredBy) }
#endif
};

const zclAttrSet_t gaZclSceneClusterAttrSet[] = {
  {gZclAttrSceneIdSet_c, (void *)&gaZclSceneClusterAttrDef, NumberOfElements(gaZclSceneClusterAttrDef)}
};

const zclAttrSetList_t gZclSceneClusterAttrSetList = {
  NumberOfElements(gaZclSceneClusterAttrSet),
  gaZclSceneClusterAttrSet
};


const zclCmd_t gaZclSceneClusterCmdReceivedDef[]={
  // 3.7.2 commands received 
  gZclCmdScene_AddScene_c,
  gZclCmdScene_ViewScene_c,
  gZclCmdScene_RemoveScene_c,
  gZclCmdScene_RemoveAllScenes_c,
  gZclCmdScene_StoreScene_c,
  gZclCmdScene_RecallScene_c,
  gZclCmdScene_GetSceneMembership_c
};

const zclCmd_t gaZclSceneClusterCmdGeneratedDef[]={
  // 3.7.2 commands generated 
  gZclCmdScene_AddSceneRsp_c,
  gZclCmdScene_ViewSceneRsp_c,
  gZclCmdScene_RemoveSceneRsp_c,
  gZclCmdScene_RemoveAllScenesRsp_c,
  gZclCmdScene_StoreSceneRsp_c,
  gZclCmdScene_GetSceneMembershipRsp_c
};

const zclCommandsDefList_t gZclSceneClusterCommandsDefList =
{
   NumberOfElements(gaZclSceneClusterCmdReceivedDef), gaZclSceneClusterCmdReceivedDef,
   NumberOfElements(gaZclSceneClusterCmdGeneratedDef), gaZclSceneClusterCmdGeneratedDef
};

/******************************
  On/Off Cluster Data
  See ZCL Specification Section 3.8
*******************************/

/* used by OnOffLight, DimmingLight, and any other device that supports the OnOff cluster server */
const zclAttrDef_t gaZclOnOffClusterAttrDef[] = {
  { gZclAttrIdOnOff_OnOffId_c, gZclDataTypeBool_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c| gZclAttrFlagsReportable_c | gZclAttrFlagsInSceneTable_c, sizeof(uint8_t), (void *)MbrOfs(zclOnOffAttrsRAM_t, onOff)}
};

#if gASL_EnableZLLClustersData_d
const zclAttrDef_t gaZclOnOffClusterZllAttrDef[] = {
  { gZclAttrIdOnOff_GlobalSceneCtrl_c, gZclDataTypeBool_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclOnOffAttrsRAM_t, globalSceneCtrl)},
  { gZclAttrIdOnOff_OnTime_c, gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c, sizeof(uint16_t), (void *)MbrOfs(zclOnOffAttrsRAM_t, onTime)},
  { gZclAttrIdOnOff_OffWaitTime_c, gZclDataTypeUint16_c,   gZclAttrFlagsInRAM_c, sizeof(uint16_t), (void *)MbrOfs(zclOnOffAttrsRAM_t, offWaitTime)}
};
#endif

const zclAttrSet_t gaZclOnOffClusterAttrSet[] = {
  {gZclAttrOnOffSet_c, (void *)&gaZclOnOffClusterAttrDef, NumberOfElements(gaZclOnOffClusterAttrDef)}
#if gASL_EnableZLLClustersData_d
  ,{gZclAttrOnOffZllSet_c, (void *)&gaZclOnOffClusterZllAttrDef, NumberOfElements(gaZclOnOffClusterZllAttrDef)}
#endif  
};

const zclAttrSetList_t gZclOnOffClusterAttrSetList = {
  NumberOfElements(gaZclOnOffClusterAttrSet),
  gaZclOnOffClusterAttrSet
};


const zclCmd_t gaZclOnOffClusterCmdReceivedDef[]={
  /* commands received */
  gZclCmdOnOff_Off_c, gZclCmdOnOff_On_c, gZclCmdOnOff_Toggle_c
#if gASL_EnableZLLClustersData_d
  ,gZclCmdOnOff_OffWithEffect_c, gZclCmdOnOff_OnWithRecallGlobalScene_c, 
  gZclCmdOnOff_OnWithTimedOff_c
#endif
};

const zclCommandsDefList_t gZclOnOffClusterCommandsDefList =
{
   NumberOfElements(gaZclOnOffClusterCmdReceivedDef), gaZclOnOffClusterCmdReceivedDef,
   0, NULL
};



/******************************
  On/Off Switch Configure Cluster Data
  See ZCL Specification Section 3.9
*******************************/
/* used by OnOffSwitch,Configuration Tool , and any other device that supports the OnOff Switch Configure cluster server */
const zclAttrDef_t gaZclOnOffSwitchConfigureClusterAttrDef[] = {
  { gZclAttrIdOnOffSwitchCfg_SwitchType_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclOnOffSwitchCfgAttrsRAM_t, SwitchType) },
  { gZclAttrIdOnOffSwitchCfg_SwitchAction_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zclOnOffSwitchCfgAttrsRAM_t, SwitchAction) }
};

const zclAttrSet_t gaZclOnOffSwitchConfigureClusterAttrSet[] = {    
  { gZclAttrOnOffSwitchCfgSet_c, gaZclOnOffSwitchConfigureClusterAttrDef, NumberOfElements(gaZclOnOffSwitchConfigureClusterAttrDef)}
};


const zclAttrSetList_t gZclOnOffSwitchConfigureClusterAttrSetList = {    
  NumberOfElements(gaZclOnOffSwitchConfigureClusterAttrSet),
  gaZclOnOffSwitchConfigureClusterAttrSet
};

/******************************
  Alarms Cluster Data
  See ZCL Specification Section 3.11
*******************************/
zclAlarmsAttrsRAM_t gZclAlarmAttr;

const zclAttrDef_t gaZclAlarmsClusterAttrDef[] = {
  {gZclAttrIdAlarms_AlarmCount_c, gZclDataTypeUint16_c, gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)&gZclAlarmAttr},
};

const zclAttrSet_t gaZclAlarmsClusterAttrSet[] = {
  {gZclAttrSetAlarms_AlarmInformation_c, (void *)&gaZclAlarmsClusterAttrDef, NumberOfElements(gaZclAlarmsClusterAttrDef)}
};

const zclAttrSetList_t gZclAlarmsClusterAttrSetList = {    
  NumberOfElements(gaZclAlarmsClusterAttrSet),
  gaZclAlarmsClusterAttrSet
};

const zclCmd_t gaZclAlarmsClusterCmdReceivedDef[]={
  //3.11.2.4 commands received 
  gAlarmClusterRxCommandID_ResetAlarm_c,
  gAlarmClusterRxCommandID_ResetAllAlarms_c,
  gAlarmClusterRxCommandID_GetAlarm_c,
  gAlarmClusterRxCommandID_ResetAlarmLog_c
};

const zclCmd_t gaZclAlarmsClusterCmdGeneratedDef[]={
  //3.11.2.5 commands generated 
  gAlarmClusterTxCommandID_Alarm_c,
  gAlarmClusterTxCommandID_GetAlarmResponse_c,
  gAlarmClusterTxCommandID_ClearAlarm_c
};

const zclCommandsDefList_t gZclAlarmsClusterCommandsDefList =
{
   NumberOfElements(gaZclAlarmsClusterCmdReceivedDef),  gaZclAlarmsClusterCmdReceivedDef,
   NumberOfElements(gaZclAlarmsClusterCmdGeneratedDef), gaZclAlarmsClusterCmdGeneratedDef
};


/******************************
  Level Control Cluster Data
  See ZCL Specification Section 3.10
*******************************/

/* Level Cluster Attribute Definitions */
const zclAttrDef_t gaZclLevelCtrlClusterAttrDef[] = {
   { gZclAttrIdLevelControl_CurrentLevelId_c,        gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsReportable_c |gZclAttrFlagsInSceneTable_c, sizeof(uint8_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,currentLevel)}
#if gZclClusterOptionals_d
  ,{ gZclAttrIdLevelControl_RemainingTimeId_c,       gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(zbLevelCtrlTime_t),(void*)MbrOfs(zclLevelCtrlAttrsRAM_t,remainingTime)  }
  ,{ gZclAttrIdLevelControl_OnOffTransitionTimeId_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(zbLevelCtrlTime_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,onOffTransitionTime) }
  ,{ gZclAttrIdLevelControl_OnLevelId_c,             gZclDataTypeUint8_c,  gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,onLevel) }
  ,{ gZclAttrIdLevelControl_OnTransitionTimeId_c,    gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(zbLevelCtrlTime_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,onTransitionTime) }
  ,{ gZclAttrIdLevelControl_OffTransitionTimeId_c,   gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(zbLevelCtrlTime_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,offTransitionTime) }
  ,{ gZclAttrIdLevelControl_DefaultMoveRateId_c,     gZclDataTypeUint8_c,  gZclAttrFlagsInRAM_c, sizeof(uint8_t),           (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,defaultMoveRate) }
#endif /* optionals */
};

const zclAttrSet_t gaZclLevelCtrlClusterAttrSet[] = {
  {gZclAttrSetLevelCtrl_c, (void *)&gaZclLevelCtrlClusterAttrDef, NumberOfElements(gaZclLevelCtrlClusterAttrDef)}
};

const zclAttrSetList_t gZclLevelCtrlClusterAttrSetList = {
  NumberOfElements(gaZclLevelCtrlClusterAttrSet),
  gaZclLevelCtrlClusterAttrSet
};

const zclCmd_t gaZclLevelCtrlClusterCmdReceivedDef[]={
  // 3.10.2 commands received 
  gZclCmdLevelControl_MoveToLevel_c,
  gZclCmdLevelControl_Move_c,
  gZclCmdLevelControl_Step_c,
  gZclCmdLevelControl_Stop_c,
  gZclCmdLevelControl_MoveToLevelOnOff_c,
  gZclCmdLevelControl_MoveOnOff_c,
  gZclCmdLevelControl_StepOnOff_c,
  gZclCmdLevelControl_StopOnOff_c
};

const zclCommandsDefList_t gZclLevelCtrlClusterCommandsDefList =
{
   NumberOfElements(gaZclLevelCtrlClusterCmdReceivedDef), gaZclLevelCtrlClusterCmdReceivedDef,
   0, NULL
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************
  Basic Cluster
  See ZCL Specification Section 3.2
*******************************/

/*!
 * @fn 		void ZCL_ResetDevice(afDeviceDef_t *pDevice) 
 *
 * @brief	ZCL Reset Device
 *
 */
void ZCL_ResetDevice
  (
  afDeviceDef_t *pDevice  /* IN: device definition to reset */
  )
{
  
  if (pDevice->pData != NULL) 
  {   
#if gZclEnableReporting_c    
    /* clear reporting setup */
    ZCL_StopAttrReporting(pDevice);
#endif    
  }
}

/*!
 * @fn 		zbStatus_t ZCL_BasicClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Basic Cluster Server. 
 *
 */
zbStatus_t ZCL_BasicClusterServer
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

/******************************
  Power Configuration Cluster
  See ZCL Specification Section 3.3
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_PowerCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the PowerConfiguration Cluster Server. 
 *
 */
zbStatus_t ZCL_PowerCfgClusterServer
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
  Temperature Configuration Cluster
  See ZCL Specification Section 3.4
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_TempCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Temperature Configuration Cluster Server. 
 *
 */
zbStatus_t ZCL_TempCfgClusterServer
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
  Binary Input(Basic) Cluster
  See ZCL Specification Section 3.4
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_BinaryInputClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the BinaryInput Cluster Server. 
 *
 */
zbStatus_t ZCL_BinaryInputClusterServer
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
  Identify Cluster
  See ZCL Specification Section 3.5
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_IdentifyClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Identify Cluster Client. 
 *
 */
zbStatus_t ZCL_IdentifyClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  
  
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	  status = gZclSuccess_c;
  
  /* handle the command */
  command = pFrame->command;
  
  if (command == gZclCmdIdentifyQueryRsp_c)
  {
    (void)ZCL_IdentifyQueryRspHandler(pIndication);
    return status;
  }
  else
    return gZclUnsupportedClusterCommand_c;
}


/*!
 * @fn 		zbStatus_t ZCL_IdentifyClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Identify Cluster Server. 
 *
 */
zbStatus_t ZCL_IdentifyClusterServer
(
  zbApsdeDataIndication_t *pIndication, /* IN: */
  afDeviceDef_t *pDevice                /* IN: */
)
{
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
    
  /* avoid compiler warnings */
  (void)pDevice;

  /* check command */
  pFrame = (void *)(pIndication->pAsdu);
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
      status = gZclSuccess_c;
	
  switch (pFrame->command) 
  {
      case gZclCmdIdentify_c:
        {
          uint16_t time;
          FLib_MemCpy((void*)&time,(void*)(pFrame + 1),sizeof(time));
          ZCL_SetIdentifyMode(pIndication->dstEndPoint,time);
          return status; 
        }
      case gZclCmdIdentifyQuery_c:
        {
          uint16_t identifyTime;
          (void)ZCL_GetAttribute(pIndication->dstEndPoint,pIndication->aClusterId, gZclAttrIdentify_Time_c , gZclServerAttr_c, &identifyTime, NULL);
          /* respond back to caller if in identify mode */
          if(identifyTime) 
          {
            pFrame->command = gZclCmdIdentifyQueryRsp_c;
            return ZCL_Reply(pIndication, sizeof(uint16_t), &identifyTime);
          }
          return status;
        }    
      case gZclCmdEzModeInvoke_c:
        {
#if gASL_EnableEZCommissioning_d    
          BeeAppDataInit(appEndPoint) = pIndication->dstEndPoint;
          EZComissioning_Start(*((uint8_t*)(pFrame + 1)));
#else
          status = gZbFailed_c;
#endif      
          return status;
        }
      case gZclCmdUpdateCommissioningState_c:
        {
#if gASL_EnableEZCommissioning_d    
          uint8_t commissioningState = 0;  
          switch(*((uint8_t*)(pFrame + 1)))
          {
            case gZclCmdUpdateCommissioningState_Set_c:
            {
              commissioningState |= (*(((uint8_t*)(pFrame + 1)) + 1));
              break;
            }
            case gZclCmdUpdateCommissioningState_Clear_c:
            {
              commissioningState &= ~(*(((uint8_t*)(pFrame + 1)) + 1));
            }
          }
          if(commissioningState&gZclCommissioningState_OperationalState_d)
          {
              BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_EZCommissioning_Succesfull_c, 0, 0, NULL);
      
              /* set up commissioningState */
              (void)ZCL_SetAttribute(pIndication->dstEndPoint,pIndication->aClusterId, gZclAttrIdentify_CommissioningState_c, gZclServerAttr_c, &commissioningState);
              ZCL_SaveNvmZclData();
          }
#else
          status = gZbFailed_c; 
#endif 
          return status;
        }
        
#if gASL_EnableZLLClustersData_d      
    case gZclCmdIdentifyTriggerEffect_c:
      {
        zbStatus_t tempStatus = ZCL_IdentifyTriggerEffectHandler(pIndication);
        if(tempStatus != gZclSuccess_c)
          status = tempStatus;
        return status;  
      }
#endif      
    default:
        return gZclUnsupportedClusterCommand_c;
  }
}

/*!
 * @fn 		void ZCL_IdentifyTimer( uint8_t timerId )
 *
 * @brief	Local command for entering identify mode for a period of time.
 *
 */
void ZCL_IdentifyTimer( uint8_t timerId )
{
    uint16_t identifyTime;
    zbClusterId_t clusterId = {gaZclClusterIdentify_c};
    uint8_t event = gIdentifyToggle_c;   
    /* avoid compiler warning */
    (void)timerId;

#if gInstantiableStackEnabled_d
     for(uint8_t i=0; i<EndPointConfigData(gNum_EndPoints); i++)
#else
     for(uint8_t i=0; i<gNum_EndPoints_c; i++)
#endif  
     {
       uint8_t endpoint = EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint);
       if(ZCL_GetAttribute(endpoint, clusterId, gZclAttrIdentify_Time_c , gZclServerAttr_c, &identifyTime, NULL)== gZbSuccess_c)
       {	
          identifyTime = OTA2Native16(identifyTime);
    
          /* decrement the time */
          if(identifyTime)
          {
            identifyTime--;        
#if gASL_EnableEZCommissioning_d      
            if((EZCommissioningConfigData(gEZCommissioning_LastEvent) == gStartIdentify_c)||
                (EZCommissioningConfigData(gEZCommissioning_LastEvent) == gUpdatePollRate_c) ||
                  (EZCommissioningConfigData(gEZCommissioning_LastEvent) == gSendIdentifyReq_c))
            {
              if(identifyTime%EZIdQueryTime_c == 0)
              {
                TS_SendEvent(gEzCmsTaskId, gSendIdentifyReq_c);
              }
            }
#endif    
            identifyTime = Native2OTA16(identifyTime);
            (void)ZCL_SetAttribute(endpoint, clusterId, gZclAttrIdentify_Time_c, gZclServerAttr_c, &identifyTime);
        
            if(!identifyTime)
            {
                ZbTMR_StopTimer(ZbZclFoundationGlobals(gZclIdentifyTimerID));
#if gASL_EnableEZCommissioning_d  
                TS_SendEvent(gEzCmsTaskId, gIdentifyEnd_c);
#endif
                event = gIdentifyOff_c;
            }
            BeeAppUpdateDevice(endpoint, event, 0, 0, NULL);
          }
       }
     }
}
/*!
 * @fn 		void ZCL_SetIdentifyMode(zbEndPoint_t endPoint,	uint16_t time) 
 *
 * @brief	Local command for entering identify mode for a period of time. Set to 0xffff
 * 			for forever. Set to 0x0000 to turn identify off.
 *
 */
void ZCL_SetIdentifyMode
(
    zbEndPoint_t endPoint,
    uint16_t time /* IN: in seconds */
)
{
    uint8_t event;
    zbClusterId_t clusterId = {gaZclClusterIdentify_c};
        
    /* set up identify mode */
    (void)ZCL_SetAttribute(endPoint, clusterId, gZclAttrIdentify_Time_c, gZclServerAttr_c, &time);

    /* turn identify on */
    if(time)
    {
        event = gIdentifyOn_c;
        ZbTMR_StartTimer(ZbZclFoundationGlobals(gZclIdentifyTimerID), gTmrLowPowerIntervalMillisTimer_c, 1000 , ZCL_IdentifyTimer);
    }
    /* turn identify off */
    else
    {
        event = gIdentifyOff_c;	
        ZbTMR_StopTimer(ZbZclFoundationGlobals(gZclIdentifyTimerID));
#if gASL_EnableEZCommissioning_d  
        TS_SendEvent(gEzCmsTaskId, gIdentifyEnd_c);
#endif
    }

    /* let UI know we're now in/out of identify mode */
    BeeAppUpdateDevice(endPoint, event, 0, 0, NULL);
}


/*!
 * @fn 		void ZCL_IdentifyTimeLeft( zbEndPoint_t endPoint )
 *
 * @brief	Local command for determining time left in identify mode
 *
 */
uint16_t ZCL_IdentifyTimeLeft
(
    zbEndPoint_t endPoint
)
{
    zbClusterId_t clusterId = {gaZclClusterIdentify_c};
    uint16_t identifyTime = 0;

    (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrIdentify_Time_c , gZclServerAttr_c, &identifyTime, NULL);

    return identifyTime;
}

/*!
 * @fn 		zbStatus_t zclIdentify_IdentifyQueryReq(zclIdentifyQueryReq_t *pReq, uint8_t len) 
 *
 * @brief	Sends over-the-air an IdentifyQueryRequest command from the Identify Cluster Client. 
 *
 */
zbStatus_t zclIdentify_IdentifyQueryReq
(
    zclIdentifyQueryReq_t *pReq, uint8_t len
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIdentify_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdIdentifyQuery_c, len,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclIdentify_IdentifyQueryRsp(zclIdentifyQueryRsp_t *pReq, uint8_t len)
 *
 * @brief	Sends over-the-air an IdentifyQueryResponse command from the Identify Cluster Server. 
 *
 */
zbStatus_t zclIdentify_IdentifyQueryRsp
(
    zclIdentifyQueryRsp_t *pReq, uint8_t len
)
{
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIdentify_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdIdentifyQueryRsp_c, len,(zclGenericReq_t *)pReq);
}


/*!
 * @fn 		zbStatus_t zclIdentify_IdentifyQueryReq(zclIdentifyQueryReq_t *pReq, uint8_t len) 
 *
 * @brief	Process the IdentifyQueryResponse command received from the Identify Cluster Server. 
 *
 */
static zbStatus_t ZCL_IdentifyQueryRspHandler(zbApsdeDataIndication_t *pIndication)
{
#if gASL_EnableEZCommissioning_d && gASL_EnableEZCommissioning_Initiator_d
  if((EZCommissioningConfigData(gpEZCommissioningTempData)) && (EZCommissioningConfigData(gEZCommissioning_LastEvent) == gSendIdentifyReq_c))
  {
    FLib_MemCpy(EZCommissioningConfigData(gpEZCommissioningTempData), pIndication, sizeof(zbApsdeDataIndication_t));
    TS_SendEvent(gEzCmsTaskId, gReceivedIdentifyRsp_c);
  }
#endif    
  return gZclSuccess_c;
  
}

#if gASL_EnableZLLClustersData_d
/*!
 * @fn 		zbStatus_t zclIdentify_TriggerEffectReq(zclIdentifyTriggerEffectReq_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to HUE  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclIdentify_TriggerEffectReq
(
  zclIdentifyTriggerEffect_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIdentify_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdIdentifyTriggerEffect_c, sizeof(zclCmdIdentifyTriggerEffect_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZCL_IdentifyTriggerEffectHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle TriggerEffect command received from Identify Client
 *
 */
zbStatus_t ZCL_IdentifyTriggerEffectHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclCmdIdentifyTriggerEffect_t *pTriggerEffect;
  uint8_t event = gIdentifyOff_c;
  uint16_t identifyTime;
  zbClusterId_t clusterId = {gaZclClusterIdentify_c};
  
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pTriggerEffect = (zclCmdIdentifyTriggerEffect_t *)(pFrame+1);  
  
  switch(pTriggerEffect->effectId)
  {
    case gIdentifyEffectId_Blink_c:
      /* set identify time for 1 second */
      identifyTime = 0x01;
      break;
    case gIdentifyEffectId_Breathe_c:
      /* set identify time for 15 second */
      identifyTime = 0x0F;
      break;
    case gIdentifyEffectId_Okay_c:
      /* set identify time for 3 second */
      identifyTime = 0x03;
      break;
    case gIdentifyEffectId_ChannelChange_c:
      /* set identify time for 2 second */
      identifyTime = 0x02;
      break;
    case gIdentifyEffectId_FinishEffect_c:
    case gIdentifyEffectId_StopEffect_c:
      identifyTime = 0x00;
      break;
    default:
      /* set identify time for 1 second */
      identifyTime = 0x01;
      break;      
  }
  
  /* set identify time */
  (void)ZCL_SetAttribute(pIndication->dstEndPoint, clusterId, gZclAttrIdentify_Time_c, gZclServerAttr_c, &identifyTime);
  
  /* turn identify on */
  if(identifyTime)
  {
    event = gIdentifyOn_c;
    ZbTMR_StartTimer(ZbZclFoundationGlobals(gZclIdentifyTimerID), gTmrLowPowerIntervalMillisTimer_c, 1000 , ZCL_IdentifyTimer);
  }
  
  /* let UI know we're now in/out of identify mode */
  BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, pTriggerEffect);
  
  return status;
}
#endif

/******************************
  Groups Cluster
  See ZCL Specification Section 3.6
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_GroupClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Group Cluster Client. 
 *
 */
zbStatus_t ZCL_GroupClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;

  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  /* handle the command */
  command = pFrame->command;
  
  switch(command) {
    case gZclCmdGroup_AddGroupRsp_c:
    case gZclCmdGroup_ViewGroupRsp_c:
    case gZclCmdGroup_GetGroupMembershipRsp_c:
    case gZclCmdGroup_RemoveGroupRsp_c:
      return (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c;
  default:
      return gZclUnsupportedClusterCommand_c;
  }
}

/*!
 * @fn 		zbStatus_t ZCL_GroupClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Group Cluster Server. 
 *
 */
zbStatus_t ZCL_GroupClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  zbStatus_t (*pGroupFunc)(zbApsdeDataIndication_t *pIndication, void *command);  
  pGroupFunc = NULL;
  
  /* prevent compiler warning */
  (void)pDevice;

  pFrame = (void *)pIndication->pAsdu;
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;
	
	
  /* handle the command */
  command = pFrame->command;
  
  switch(command) 
  {
    /* add group if identifying */
    case gZclCmdGroup_AddGroupIfIdentifying_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command))  ZCL_GroupAddGroupIfIdentify;  
      break;
    /* add group to endpoint */
    case gZclCmdGroup_AddGroup_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupAddGroupRsp;
      status = gZclSuccess_c;
      break;
    /* view the group */
    case gZclCmdGroup_ViewGroup_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupViewGroupRsp;
      status = gZclSuccess_c;
      break;
    /* check what groups this endpoint belongs to */
    case gZclCmdGroup_GetGroupMembership_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupGetGroupMembershipRsp;
      status = gZclSuccess_c;
      break;
    /* remove the group */
    case gZclCmdGroup_RemoveGroup_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupRemoveGroupRsp;
      status = gZclSuccess_c;
      break;
    /* remove all groups from the endpoint */
    case gZclCmdGroup_RemoveAllGroups_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupRemoveAllGroups;
      break;
    default:
      return gZclUnsupportedClusterCommand_c;
  }
  if (pGroupFunc)
    (void)pGroupFunc(pIndication, (void *)(pFrame + 1));

  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_GroupAddGroupRsp(zbApsdeDataIndication_t *pIndication, zclCmdGroup_AddGroup_t *command) 
 *
 * @brief	Process AddGroup command and sends over-the-air an AddGroupResponse from the Group Cluster Server. 
 *
 */
zbStatus_t ZCL_GroupAddGroupRsp
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_AddGroup_t*command
)
{
  zclCmdGroup_AddGroupRsp_t Resp;
  uint8_t iPayloadLen;
  zbApsmeAddGroupReq_t  ApsmeAddGroup;
  zbStatus_t status = gZclSuccess_c;

  Copy2Bytes(ApsmeAddGroup.aGroupId,command->aGroupId);
  Copy2Bytes(Resp.aGroupId,command->aGroupId);
  ApsmeAddGroup.endPoint = pIndication->dstEndPoint;

  if(ApsGroupIsMemberOfEndpoint(Resp.aGroupId,ApsmeAddGroup.endPoint))
  {
    Resp.status = gZclDuplicateExists_c;
  } 
  else
  {
    Resp.status = (zclStatus_t)APSME_AddGroupRequest(&ApsmeAddGroup);
    if(gApsTableFull_c == Resp.status)
      Resp.status = gZclInsufficientSpace_c;
  }
  iPayloadLen = sizeof(zclCmdGroup_AddGroupRsp_t);
  
  /* send response */
#if gASL_EnableZLLClustersData_d    
  if(!pIndication->fWasBroadcast && (pIndication->dstAddrMode != gZbAddrModeGroup_c))
#endif  
    status = ZCL_Reply(pIndication, iPayloadLen, &Resp);

  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_GroupAddGroupIfIdentify(zbApsdeDataIndication_t *pIndication, zclCmdGroup_AddGroupIfIdentifying_t *command) 
 *
 * @brief	Process AddGroupIfIdentify command received from the Group Cluster Client. 
 *
 */
zbStatus_t ZCL_GroupAddGroupIfIdentify
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_AddGroupIfIdentifying_t*command
)
{
  zbClusterId_t clusterId = {gaZclClusterIdentify_c};
  uint16_t identifyTime = 0;

  (void)ZCL_GetAttribute(pIndication->dstEndPoint, clusterId, gZclAttrIdentify_Time_c , gZclServerAttr_c, &identifyTime, NULL);  
   
  if(identifyTime) 
  {
    zbApsmeAddGroupReq_t  ApsmeAddGroup;
    Copy2Bytes(ApsmeAddGroup.aGroupId,command->aGroupId);
    ApsmeAddGroup.endPoint = pIndication->dstEndPoint;
    return APSME_AddGroupRequest(&ApsmeAddGroup);
  }
  return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t ZCL_GroupRemoveGroupRsp(zbApsdeDataIndication_t *pIndication, zclCmdGroup_RemoveGroup_t *command) 
 *
 * @brief	Process RemoveGroup command and sends over-the-air an RemoveGroupResponse from the Group Cluster Server. 
 *
 */
zbStatus_t ZCL_GroupRemoveGroupRsp
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_RemoveGroup_t*command
)
{
  zclCmdGroup_RemoveGroupRsp_t Resp;
  zbApsmeRemoveGroupReq_t  ApsmeRemoveGroup;
  uint8_t iPayloadLen;
  zbStatus_t status = gZclSuccess_c;

  Copy2Bytes(ApsmeRemoveGroup.aGroupId,command->aGroupId);
	ApsmeRemoveGroup.endPoint = pIndication->dstEndPoint;
  Resp.status = APSME_RemoveGroupRequest(&ApsmeRemoveGroup);
  if((gApsInvalidGroup_c == Resp.status) || (gApsInvalidParameter_c == Resp.status))
    Resp.status = gZclNotFound_c;  
  Copy2Bytes(Resp.aGroupId,command->aGroupId);

  iPayloadLen = sizeof(Resp);

  /* send response */
#if gASL_EnableZLLClustersData_d    
  if(!pIndication->fWasBroadcast && (pIndication->dstAddrMode != gZbAddrModeGroup_c))
#endif    
    status = ZCL_Reply(pIndication, iPayloadLen, &Resp);

  return status;
  
}

/*!
 * @fn 		zbStatus_t ZCL_GroupRemoveAllGroups(zbApsdeDataIndication_t *pIndication, zclCmdGroup_RemoveAllGroups_t *command) 
 *
 * @brief	Process RemoveAllGroups command received from the Group Cluster Client. 
 *
 */
zbStatus_t ZCL_GroupRemoveAllGroups
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_RemoveAllGroups_t*command
)
{
  zbApsmeRemoveAllGroupsReq_t  ApsmeRemoveAllGroups;

  (void) command; /*To avoid compile errors*/
  ApsmeRemoveAllGroups.endPoint = pIndication->dstEndPoint;

  return APSME_RemoveAllGroupsRequest(&ApsmeRemoveAllGroups);
}

#if(gInstantiableStackEnabled_d == 1)
extern zbStackTablesSizes_t* pZbStackTablesSizes; 
#endif
/*!
 * @fn 		zbStatus_t ZCL_FindGroup(zbGroupId_t aGroupId) 
 *
 * @brief	Return succes if the group is in the group table, otherwise return NotFound
 *
 */
zclStatus_t ZCL_FindGroup
(
  zbGroupId_t  aGroupId
)
{
  zclStatus_t  status;
  zbGroupId_t  InvalidGroup={0xF8,0xFF};
  int i;

  status = gZclInvalidValue_c;  /*In case the group to be search is 0x0000 or 0xFFF7*/  
  if (!IsEqual2Bytes(aGroupId,InvalidGroup) && !Cmp2BytesToZero(aGroupId))
  {
    status = gZclNotFound_c;
    for(i = 0; i < ZbStackTablesSizes(gApsMaxGroups); ++i)
    {
      if (!IsEqual2Bytes(gpGroupTable[i].aGroupId,aGroupId))
        /*If group is no the one we are looking then continue to next one in the list */
        continue;

      /*Only in case that the group is found then stop the loop*/
      status = gZbSuccess_c;
      break;
    }
  }
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_GroupViewGroupRsp(zbApsdeDataIndication_t *pIndication, zclCmdGroup_ViewGroup_t *command) 
 *
 * @brief	Process ViewGroup command and sends over-the-air the ViewGroupResponse command from the Group Cluster Server. 
 *
 */
zclStatus_t ZCL_GroupViewGroupRsp
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_ViewGroup_t*command
)
{
  zclCmdGroup_ViewGroupRsp_t Resp;
  uint8_t iPayloadLen;
  zbStatus_t status = gZclSuccess_c;
  
  Resp.status = ZCL_FindGroup(command->aGroupId);
  /*
  The String Names are optionals in Ha, for size reasons in BeeStack are not supported
  instead a NULL empty string is send out all the time.
  */
  Resp.szName[0] = 0x00;
  iPayloadLen = sizeof(zclCmdGroup_ViewGroupRsp_t);

  Copy2Bytes(Resp.aGroupId, command->aGroupId);

  /* send response */
#if gASL_EnableZLLClustersData_d    
  if(!pIndication->fWasBroadcast && (pIndication->dstAddrMode != gZbAddrModeGroup_c))
#endif  
    status = ZCL_Reply(pIndication, iPayloadLen, &Resp);

  return status;

}

/*!
 * @fn 		zbStatus_t ZCL_GroupGetGroupMembershipRsp(zbApsdeDataIndication_t *pIndication, zclCmdGroup_GetGroupMembership_t *command) 
 *
 * @brief	Process GetGroupMembership command and sends over-the-air the GetGroupMembershipResponse command from the Group Cluster Server. 
 *
 */
zbStatus_t ZCL_GroupGetGroupMembershipRsp
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_GetGroupMembership_t*command
)
{
  zbIndex_t   iGroupEntry;
  zclCmdGroup_GetGroupMembershipRsp_t   *pResp;
  zbGroupTable_t *pPtr;
  afToApsdeMessage_t *pMsg;
  zbStatus_t status = gZclSuccess_c;
  
  zbSize_t iPayloadLen;
  uint8_t limit;
  zbCounter_t  count;
  zbCounter_t  NumOfGroups = 0;


  pMsg = AF_MsgAlloc();
  if (!pMsg)
    return gZbNoMem_c;

  BeeUtilZeroMemory(pMsg, gMaxRxTxDataLength_c);
  pResp = (zclCmdGroup_GetGroupMembershipRsp_t *)(((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t));

  count = 0;
  limit = (!(command->count))? 0:1;
  for (; command->count >= limit; command->count--)
  {
    NumOfGroups = 0;
    for(iGroupEntry=0; iGroupEntry<ZbStackTablesSizes(gApsMaxGroups); ++iGroupEntry)
    {
      pPtr = &gpGroupTable[iGroupEntry];

      if (!Cmp2BytesToZero(pPtr->aGroupId))
      {
        if((!limit) || (IsEqual2Bytes(command->aGroupId[count], pPtr->aGroupId)))
        {
          // if the group matches the group table entry
          Copy2Bytes(pResp->aGroupId[pResp->count], pPtr->aGroupId);
          pResp->count++;
        }
        NumOfGroups++;  
      }
    }

    if (!limit)
    {
      break;
    }
    else
    {
      count++;
    }
  }    

  pResp->capacity = ZbStackTablesSizes(gApsMaxGroups) - NumOfGroups;

  iPayloadLen = MbrOfs(zclCmdGroup_GetGroupMembershipRsp_t,aGroupId[0])+(pResp->count * sizeof(zbClusterId_t));

  if((pResp->count)||(!limit))
  {
#if gASL_EnableZLLClustersData_d    
    if(!pIndication->fWasBroadcast && (pIndication->dstAddrMode != gZbAddrModeGroup_c))
#endif      
      status =  ZCL_ReplyNoCopy(pIndication, iPayloadLen, pMsg);
#if gASL_EnableZLLClustersData_d        
    else
      MSG_Free(pMsg);
#endif    
    
    return status;
  }
  else
  {
      zclFrame_t     *pFrame;
      pFrame = (void *)pIndication->pAsdu;
      MSG_Free(pMsg);
      return (pFrame->frameControl&gZclFrameControl_DisableDefaultRsp)?gZbSuccess_c:gZclSuccessDefaultRsp_c;
  }
}

/******************************
  Scenes Cluster
  See ZCL Specification Section 3.7
*******************************/
/*!
 * @fn   	zbStatus_t ZCL_SceneClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Scene Cluster Client. 
 *
 */
zbStatus_t ZCL_SceneClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;
  
  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  /* handle the command */
  command = pFrame->command;
  
  switch(command) {
/* responses to above commands */
  case gZclCmdScene_AddSceneRsp_c:
  case gZclCmdScene_ViewSceneRsp_c:
  case gZclCmdScene_RemoveSceneRsp_c:
  case gZclCmdScene_RemoveAllScenesRsp_c:
  case gZclCmdScene_StoreSceneRsp_c:
  case gZclCmdScene_GetSceneMembershipRsp_c:
    return (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c;
  default:
    return gZclUnsupportedClusterCommand_c;
  }
}

/*!
 * @fn 		zbStatus_t ZCL_SceneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Scene Cluster Server. 
 *
 */
zbStatus_t ZCL_SceneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
{
  zclFrame_t      *pFrame;
  zclSceneTableEntry_t *pScene;          /* scene table entry */
  zclSceneReq_t   *pSceneReq;
  uint8_t         *pData;
  zclSceneAttrs_t *pSceneData;
  zclCmdScene_AddSceneRsp_t sceneRsp; /* generic short response (status, group, sceneID) */
  uint8_t         payloadLen;
  bool_t          fNoReply = FALSE;
  zbStatus_t      status = gZclSuccessDefaultRsp_c;
  
#if  gZclClusterOptionals_d 
  uint8_t         ieeeAddress[8];
  uint8_t         *pSrcLongAddress =  APS_GetIeeeAddress(pIndication->aSrcAddr, ieeeAddress);
  bool_t          bModifyConfiguratorAddr = FALSE;  
#endif


  /* get ptr to device data for this endpoint */
  pData = pDevice->pData;
  pSceneData = (zclSceneAttrs_t*) pDevice->pSceneData;      

  /* get ptr to ZCL frame */
  pFrame = (void *)pIndication->pAsdu;
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;
  
  
  /* get a pointer to the scene (add, view, restore, etc..) request portion of the zcl frame */
  pSceneReq = (zclSceneReq_t *)(pFrame + 1);

  /* create common reply */
  sceneRsp.status = gZbSuccess_c;
  FLib_MemCpy(&sceneRsp.aGroupId, pSceneReq, sizeof(sceneRsp) - sizeof(sceneRsp.status));
  payloadLen = sizeof(sceneRsp);
  
  
  if (pSceneData)
  {
    /* many commands need to know if the scene exists */
    pScene = ZCL_FindScene(pDevice, (void *)(pSceneReq));  
    if (((ZCL_FindGroup(pSceneReq->addScene.aGroupId)!= gZbSuccess_c) && 
         (!Cmp2BytesToZero(pSceneReq->addScene.aGroupId)))
#if gASL_EnableZLLClustersData_d         
        &&(pFrame->command != gZclCmdScene_CopyScene_c)
#endif      
       )
    {
      sceneRsp.status = gZclInvalidField_c;
      if (gZclCmdScene_RemoveAllScenes_c == pFrame->command)
        payloadLen -= sizeof(zclSceneId_t); /* don't send the sceneId for RemoveAllScenesCmd */  
    }
    else
    {
      /* handle the scene command */
      switch(pFrame->command)
      {
        /* Add a scene */
#if gASL_EnableZLLClustersData_d  
        case gZclCmdScene_EnhancedAddScene_c:
#endif  
        case gZclCmdScene_AddScene_c:
    
          status = gZclSuccess_c;	
          /* add the scene (or get the slot if it already exists) */
          pScene = ZCL_AllocateScene(pDevice, (void *)(pSceneReq));
    
          /* no room to add the scene */
          if(!pScene)
            sceneRsp.status = gZclInsufficientSpace_c;
    
          /* add the scene (similar to store scene) */
          else {          
            sceneRsp.status = ZCL_AddScene(pScene, &pSceneReq->addScene, pIndication->asduLength - sizeof(zclFrame_t), pFrame->command);
            ZCL_SaveNvmZclData();
            #if  gZclClusterOptionals_d   
            if (gZbSuccess_c == status) 
              bModifyConfiguratorAddr = TRUE;  
            #endif
          }
          break;
    
        /* View scene */
#if gASL_EnableZLLClustersData_d  
        case gZclCmdScene_EnhancedViewScene_c:
#endif          
        case gZclCmdScene_ViewScene_c:
          status = gZclSuccess_c;
          if (pScene)
          {
            sceneRsp.status = ZCL_ViewScene(pScene, pIndication, pDevice, pFrame->command);
          } else
          { 
            sceneRsp.status = gZclNotFound_c;
          }

          /* reply already sent if no error */
          if(sceneRsp.status == gZbSuccess_c)
            fNoReply = TRUE;
          break;
    
        /* Remove scene */
        case gZclCmdScene_RemoveScene_c:
          status = gZclSuccess_c;
          /* found, remove it */
          if(pScene) {
            BeeUtilZeroMemory(pScene, sizeof(zclSceneTableEntry_t));
            --(pSceneData->sceneCount);
            ZCL_SaveNvmZclData();
          }
          else
            sceneRsp.status = gZclNotFound_c;
          break;
    
        /* Remove all scenes */
        case gZclCmdScene_RemoveAllScenes_c:    
          status = gZclSuccess_c;
          /* reset all the scenes with the specified group id */
          ZCL_ResetScenes(pDevice, sceneRsp.aGroupId);
          /* send back shortened response */
          payloadLen -= sizeof(zclSceneId_t);
          break;
    
        /* Store scene */
        case gZclCmdScene_StoreScene_c:
          status = gZclSuccess_c;
          /* (re)allocate the scene for storage */
          pScene = ZCL_AllocateScene(pDevice, (void *)pSceneReq);
    
          /* store current data to the scene */
          if(pScene) {
            /* copy the attributes to store */
            HA_StoreScene(pIndication->dstEndPoint, pScene);
            ZCL_SaveNvmZclData();
            #if  gZclClusterOptionals_d   
            bModifyConfiguratorAddr = TRUE;  
            #endif
          }
          else {
            sceneRsp.status = gZclInsufficientSpace_c;
          }
          break;
    
        /* recall scene */
        case gZclCmdScene_RecallScene_c:
    
          /* recall scene data */
          if(pScene) {
            HA_RecallScene(pIndication->dstEndPoint, pData, pScene);
            pSceneData->sceneValid = gZclSceneValid_c;
            pSceneData->currentScene = pScene->sceneId;
            Copy2Bytes(pSceneData->currentGroup, pScene->aGroupId);
          }
  
          fNoReply = TRUE;
          break;
    
        /* get scene membership */
        case gZclCmdScene_GetSceneMembership_c:
          status = gZclSuccess_c;
          ZCL_GetSceneMembership(pDevice, pSceneReq->getSceneMembership.aGroupId, pIndication);
          fNoReply = TRUE;
          break;
#if gASL_EnableZLLClustersData_d     
        case gZclCmdScene_CopyScene_c:  
          return ZCL_SceneCopyScene(pDevice, pIndication);  
#endif         
    
        /* command not supported on this cluster */
        default:
          status = gZclUnsupportedClusterCommand_c;
        }
      }
      
    #if  gZclClusterOptionals_d   
      /* If IEEE address of the source was not found set on 0xFFFFFFFFFFFF */    
      if (bModifyConfiguratorAddr) 
      {      
        if (pSrcLongAddress)
          FLib_MemCpy(&pSceneData->lastConfiguredBy, pSrcLongAddress, sizeof(zbIeeeAddr_t));
        else
          FLib_MemSet(&pSceneData->lastConfiguredBy, 0xFF, sizeof(zbIeeeAddr_t));
      }      
    #endif
  }  
  else
    /* pSceneData not available */
     status = gZclUnsupportedClusterCommand_c;  

  /* send a standard scene response */
  if(!pIndication->fWasBroadcast && (pIndication->dstAddrMode != gZbAddrModeGroup_c) && !fNoReply) {
    return ZCL_Reply(pIndication, payloadLen, &sceneRsp);
  }

  /* worked */
  return status;
}

/*!
 * @fn 		void ZCL_ResetScenes(afDeviceDef_t *pDevice, zbGroupId_t groupId)
 *
 * @brief	Processes the ResetScenes command received on the Scene Cluster Server. 
 *
 */
void ZCL_ResetScenes(afDeviceDef_t *pDevice, zbGroupId_t groupId)
{  
  zclSceneAttrs_t *pSceneData = (zclSceneAttrs_t*) pDevice->pSceneData;
  zclSceneTableEntry_t *pSceneTableEntry;
  index_t i;
   
  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + 
                                                 sizeof(zclSceneAttrs_t) + 
                                                 i*pSceneData->sceneTableEntrySize);
    
    if (IsEqual2Bytes(groupId, pSceneTableEntry->aGroupId))
    {
      BeeUtilZeroMemory(pSceneTableEntry, sizeof(zclSceneTableEntry_t));        
      --(pSceneData->sceneCount);
      pSceneData->currentScene = 0x00;
      pSceneData->sceneValid = gZclSceneInvalid_c;
    }    
  }
      
  
  ZCL_SaveNvmZclData();
}

/*!
 * @fn 		zbStatus_t ZCL_AddScene(zclSceneTableEntry_t *pScene, zclCmdScene_AddScene_t *pAddSceneReq, uint8_t indicationLen)
 *
 * @brief	Processes the AddScene command received on the Scene Cluster Server. 
 *
 */
zbStatus_t ZCL_AddScene
  (
    zclSceneTableEntry_t *pScene, 
    zclCmdScene_AddScene_t *pAddSceneReq, 
    uint8_t indicationLen, 
    uint8_t commandId)
{
  uint8_t NameLen;
  uint8_t *pAddSceneData;
  zclSceneOtaData_t *pClusterData;
  uint8_t len;
  uint8_t *pData;

  len = MbrOfs(zclCmdScene_AddScene_t, szSceneName[0]);

  /* copy up through the name */
  FLib_MemCpy(pScene, pAddSceneReq, len);
#if gASL_EnableZLLClustersData_d   
  if(commandId == gZclCmdScene_AddScene_c)
  {
    pScene->transitionTime100ms = 0x00;
  }
  else /* gZclCmdScene_EnhancedAddScene_c*/
  {
    pScene->transitionTime100ms = pScene->transitionTime%10;
    pScene->transitionTime = pScene->transitionTime/10;
    gSceneClusterEnhancedCommand = TRUE;
  }
#endif 
  
  /* find the start of the cluster data (after the name) */
  pAddSceneData = (uint8_t *)&pAddSceneReq->szSceneName;
  NameLen = (*pAddSceneData);
#if gZclIncludeSceneName_d
  FLib_MemCpy(pScene->szSceneName, pAddSceneReq->szSceneName, NameLen+1);
#endif

  pData = (uint8_t*)(pAddSceneData + (NameLen+1));

  len +=(NameLen+1);
  
  /* no data to copy */
  if(indicationLen < len)
    return gZclMalformedCommand_c;
  
  
  NameLen = indicationLen - len;

/* Here starts the copying process */
  while(NameLen) 
  {
    pClusterData = (zclSceneOtaData_t*)pData;
    len = 3;   
    if (pClusterData->length > 0)
     {       
       if (HA_AddScene(pClusterData,pScene) != gZbSuccess_c)
         return gZclMalformedCommand_c;
       else
         len += pClusterData->length;
     }     
     /* get to the next extended field */ 
     pData += len;
     NameLen -= len;      
  }
  ZCL_SaveNvmZclData();
  return gZbSuccess_c;
  
}

/*!
 * @fn 		zbStatus_t ZCL_ViewScene(zclSceneTableEntry_t *pScene, zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the AddScene command and sends over-the-air an AddSceneResponse command from the Scene Cluster Server. 
 *
 */
zbStatus_t ZCL_ViewScene
  (
  zclSceneTableEntry_t *pScene,        /* IN */
  zbApsdeDataIndication_t *pIndication, /* IN */
  afDeviceDef_t *pDevice,  /* IN */
  uint8_t commandId
  )
{
  afToApsdeMessage_t *pMsg;
  uint8_t *pPayload;
  uint8_t payloadLen;

  /*To avoid Compiler errors*/
 (void)pDevice;
  /* allocate space for the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return gZclNoMem_c;

  /* build the reply */
  pPayload = ((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t);

  /* copy in fixed portion of scene */
  payloadLen = MbrOfs(zclCmdScene_AddScene_t, szSceneName);
  FLib_MemCpy(pPayload + 1, pScene, payloadLen);

 #if gASL_EnableZLLClustersData_d   
  if(commandId == gZclCmdScene_EnhancedViewScene_c)
  {
    uint16_t transitionTime = pScene->transitionTime *10 + pScene->transitionTime100ms;
    /* Update transition time field*/
    FLib_MemCpy(pPayload + 1 + payloadLen - 2, &transitionTime, sizeof(uint16_t));
    gSceneClusterEnhancedCommand = TRUE;
  }
#endif  
  
  /* put in status at beginning */
  pPayload[0] = gZbSuccess_c;
  ++payloadLen;

#if gZclIncludeSceneName_d
  /* copy in string */
  FLib_MemCpy(pPayload + payloadLen, pScene->szSceneName, 1 + *pScene->szSceneName);
  payloadLen += 1 + *pScene->szSceneName;
#else
  /* empty string */
  pPayload[payloadLen++] = 0;
#endif

  /* copy in cluster data */
  payloadLen += HA_ViewSceneData((void *)(pPayload + payloadLen), pScene);

  /* send payload over the air */
#if gASL_EnableZLLClustersData_d    
  if(!pIndication->fWasBroadcast && (pIndication->dstAddrMode != gZbAddrModeGroup_c))
#endif  
    return ZCL_ReplyNoCopy(pIndication, payloadLen, pMsg);
#if gASL_EnableZLLClustersData_d   
  else
    MSG_Free(pMsg);
  return gZbSuccess_c;
#endif  
}

/*!
 * @fn 		zbStatus_t ZCL_GetSceneMembership(afDeviceDef_t *pDevice, zbGroupId_t aGroupId, zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Processes the GetSceneMembership command and sends over-the-air an GetSceneMembershipResponse command from the Scene Cluster Server. 
 *
 */
void ZCL_GetSceneMembership
  (
  afDeviceDef_t *pDevice,
  zbGroupId_t aGroupId,                 /* IN */
  zbApsdeDataIndication_t *pIndication  /* IN */
  )
{
  afToApsdeMessage_t *pMsg;
  uint8_t payloadLen;
  uint8_t i;
  uint8_t count;    /* # of scenes found that match this group */
  zclCmdScene_GetSceneMembershipRsp_t *pRsp;
  
  zclSceneAttrs_t *pSceneAttrs;
  zclSceneTableEntry_t *pSceneTableEntry;

  /* allocate space for the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return;   /* can't send response, no buffers */
  
 
  pSceneAttrs = (zclSceneAttrs_t*) pDevice->pSceneData;  

  /* fill in the response */
  pRsp = (void *)(((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t));

  /* remember how many scenes are available */
  pRsp->status =gZbSuccess_c;
  Copy2Bytes(pRsp->aGroupId, aGroupId);
  pRsp->capacity = gHaMaxScenes_c - pSceneAttrs->sceneCount;

  /* show any that match this group */
  count = 0;
  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t) + i*pSceneAttrs->sceneTableEntrySize);
    if(IsEqual2Bytes(pSceneTableEntry->aGroupId, aGroupId)) {
      pRsp->scenes[count] = pSceneTableEntry->sceneId;
      ++count;
    }
  }

  payloadLen = MbrOfs(zclCmdScene_GetSceneMembershipRsp_t, scenes) + count;
  pRsp->sceneCount = count;

  /* send payload over the air */
  if(!pIndication->fWasBroadcast && (pIndication->dstAddrMode != gZbAddrModeGroup_c))
    (void)ZCL_ReplyNoCopy(pIndication, payloadLen, pMsg);
  else
    MSG_Free(pMsg);
}

#if gASL_EnableZLLClustersData_d
/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_GetEndpointListRsp(zllCommissioning_GetEndpointListRsp_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air a GetEndpointList response from the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t ZCL_SceneCopySceneRsp
( 
  zclScene_CopySceneRsp_t *pCommandRsp
) 
{
  afToApsdeMessage_t *pMsg;
  uint8_t iPayloadLen = sizeof(zclCmdScene_CopySceneRsp_t);
  pMsg = ZCL_CreateFrame( &(pCommandRsp->addrInfo), 
                          gZclCmdScene_CopySceneRsp_c,
	                  gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
	                  &pCommandRsp->zclTransactionId, 
	                  &iPayloadLen,
	                  (uint8_t*)&(pCommandRsp->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
	 
  return ZCL_DataRequestNoCopy(&(pCommandRsp->addrInfo),  iPayloadLen, pMsg);  
}

/*!
 * @fn 		zbStatus_t ZCL_SceneCopyScene(afDeviceDef_t *pDevice, zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Processes the CopyScene command and sends over-the-air a CopyScene Response command from the Scene Cluster Server. 
 *
 */
zbStatus_t ZCL_SceneCopyScene
  (
  afDeviceDef_t *pDevice,
  zbApsdeDataIndication_t *pIndication  /* IN */
  )
{
  zclFrame_t *pFrame;
  zclScene_CopySceneRsp_t copySceneRsp;
  zclCmdScene_CopyScene_t *pReqPayload;
  zclSceneAttrs_t *pSceneAttrs;
  zclCmdScene_StoreScene_t sceneDataFrom, sceneDataTo;
  zclSceneTableEntry_t *pSceneFrom, *pSceneTo; 
  bool_t copyAll = TRUE;
   
  /* Create the destination address */
  AF_PrepareForReply(&copySceneRsp.addrInfo, pIndication); 
  
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pReqPayload = (zclCmdScene_CopyScene_t *)(pFrame+1);  
 
  pSceneAttrs = (zclSceneAttrs_t*) pDevice->pSceneData;  
  
  /* validate payload */
  Copy2Bytes(&sceneDataFrom.aGroupId, pReqPayload->groupIdFrom);
  sceneDataFrom.sceneId =  pReqPayload->sceneIdFrom;
  Copy2Bytes(&sceneDataTo.aGroupId, pReqPayload->groupIdTo);
  sceneDataTo.sceneId =  pReqPayload->sceneIdTo; 
  
  copySceneRsp.cmdFrame.status = gZbSuccess_c;
  Copy2Bytes(&copySceneRsp.cmdFrame.groupIdFrom , pReqPayload->groupIdFrom);
  copySceneRsp.cmdFrame.sceneIdFrom = pReqPayload->sceneIdFrom;
  
  /* validate groupFrom field*/
  if(ZCL_FindGroup(pReqPayload->groupIdFrom)!= gZbSuccess_c ||
     ZCL_FindGroup(pReqPayload->groupIdTo)!= gZbSuccess_c)
  {
    copySceneRsp.cmdFrame.status = gZclInvalidField_c;
  }

  if(!(pReqPayload->mode & 0x01))
  {
    /* validate sceneFrom field*/
    pSceneFrom = ZCL_FindScene(pDevice, (zclCmdScene_RecallScene_t *)&sceneDataFrom);
    if(!pSceneFrom)
    {
      copySceneRsp.cmdFrame.status = gZclInvalidField_c;
    }
    copyAll = FALSE;
  }
  
  if(copySceneRsp.cmdFrame.status == gZbSuccess_c)
  {
     if(copyAll)
     {
      uint8_t i;
      /* get the first scene to be copied */
      for(i=0; i<gHaMaxScenes_c; ++i)
      {          
          pSceneFrom = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t) + i*pSceneAttrs->sceneTableEntrySize);
          if(IsEqual2Bytes(pSceneFrom->aGroupId, sceneDataFrom.aGroupId)) 
          {
            /* set scene destination */
            sceneDataTo.sceneId = pSceneFrom->sceneId;
            /* allocate the scene */
            pSceneTo = ZCL_AllocateScene(pDevice, &sceneDataTo);
            if(!pSceneTo)
            {
                copySceneRsp.cmdFrame.status = gZclInsufficientSpace_c;
                copyAll = FALSE;
                break;
            }
            else
            {
                /* copy the scene */
                HA_CopyScene(pSceneFrom, pSceneTo);
            } 
          }
      }
     }
    else
    {
        /* alocate the pScene destination */
        pSceneTo = ZCL_AllocateScene(pDevice, &sceneDataTo);
        if(!pSceneTo)
        {
          copySceneRsp.cmdFrame.status = gZclInsufficientSpace_c;
        }
        else
        {
          /* copy the scene */
          HA_CopyScene(pSceneFrom, pSceneTo);
        }
    }
  }
 
  if(!pIndication->fWasBroadcast && (pIndication->dstAddrMode != gZbAddrModeGroup_c))
    return ZCL_SceneCopySceneRsp(&copySceneRsp);
  else
    return gZclSuccess_c;
}
#endif


/*!
 * @fn 		zclSceneTableEntry_t *ZCL_AllocateScene(afDeviceDef_t *pDevice, zclCmdScene_StoreScene_t *pSceneId)
 *
 * @brief	Allocate a scene entry. Used for storing and recalling a scene.
 *			Returns NULL if not found, or pointer to entry in scene table if found.
 *
 */
zclSceneTableEntry_t *ZCL_AllocateScene(afDeviceDef_t *pDevice, zclCmdScene_StoreScene_t *pSceneId)
{
  index_t i;
  zclSceneTableEntry_t *pSceneTableEntry;
  zclSceneAttrs_t* pSceneAttrs = (zclSceneAttrs_t*) pDevice->pSceneData;

  /* invalid scene, not found */
  pSceneTableEntry = ZCL_FindScene(pDevice, (void *)pSceneId);
  if(pSceneTableEntry)
  { 
	pSceneAttrs->currentScene = pSceneId->sceneId;
    Copy2Bytes(pSceneAttrs->currentGroup, pSceneId->aGroupId);
    return pSceneTableEntry;
  }
#if gASL_EnableZLLClustersData_d   
  /* first position is reserved for the OnOff global scene */
  if(pSceneId->sceneId == 0x00 && pSceneId->aGroupId[0] == 0x00 && 
     pSceneId->aGroupId[1] == 0x00)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t));
    pSceneAttrs->currentScene = pSceneId->sceneId;
    Copy2Bytes(pSceneAttrs->currentGroup, pSceneId->aGroupId);
    FLib_MemCpy(pSceneTableEntry, pSceneId, sizeof(*pSceneId));
    ++(pSceneAttrs->sceneCount);
    return pSceneTableEntry;   
  }
  else
    for(i=1; i<gHaMaxScenes_c; ++i) 
#else
  for(i=0; i<gHaMaxScenes_c; ++i)
#endif    
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t) + i*pSceneAttrs->sceneTableEntrySize);
    if(!pSceneTableEntry->sceneId)
    {
      pSceneAttrs->currentScene = pSceneId->sceneId;
      Copy2Bytes(pSceneAttrs->currentGroup, pSceneId->aGroupId);
      FLib_MemCpy(pSceneTableEntry, pSceneId, sizeof(*pSceneId));
      ++(pSceneAttrs->sceneCount);
      return pSceneTableEntry;
    }
  }

  /* not found */
  return NULL;
}

/*!
 * @fn 		zclSceneTableEntry_t *ZCL_FindScene(afDeviceDef_t *pDevice, zclCmdScene_RecallScene_t *pSceneId)
 *
 * @brief	Find a scene based on group id and scene id
 *			Returns NULL if not found, or pointer to entry in scene table if found.
 *
 */
zclSceneTableEntry_t *ZCL_FindScene(afDeviceDef_t *pDevice, zclCmdScene_RecallScene_t *pSceneId)
{
  index_t i;
  zclSceneTableEntry_t *pSceneTableEntry;
  zclSceneAttrs_t* pSceneAttrs = (zclSceneAttrs_t*) pDevice->pSceneData;

  /* verify no. of scenes currently in 
     the device scene table */
  if(!pSceneAttrs->sceneCount)
    return NULL;

  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t) + i*pSceneAttrs->sceneTableEntrySize);

    if ( FLib_MemCmp((uint8_t*) pSceneId, (uint8_t*) pSceneTableEntry, sizeof(zclCmdScene_RecallScene_t)))          
      return pSceneTableEntry;
  }

  /* not found */
  return NULL;
}


/******************************
  On/Off Cluster
  See ZCL Specification Section 3.8
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_OnOffClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the OnOff Cluster Server. 
 *
 */
zbStatus_t ZCL_OnOffClusterServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  zclCmd_t command;
  uint8_t onOffAttr;
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  uint8_t event = gZclUI_NoEvent_c;
#if gASL_EnableZLLClustersData_d
  uint16_t tempData;
#endif  
  
  /* not used in this function */
  (void)pDevice;
  
  /* get onOff status*/
  (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffAttr, NULL);
  
  /* get ptr to ZCL frame */
  pFrame = (void *)pIndication->pAsdu;
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    status = gZclSuccess_c;
  
  /* determine what to do based on the Toogle event */
  command = pFrame->command;
  if(command == gZclCmdOnOff_Toggle_c) 
  {
    command = onOffAttr ? gZclCmdOnOff_Off_c : gZclCmdOnOff_On_c;
  }
  
 
  switch (command) 
  {
    case gZclCmdOnOff_Off_c:
    {
      event = gZclUI_Off_c;  
      onOffAttr = 0;
      
#if gASL_EnableZLLClustersData_d 
      /* [R3]6.6.1.4.1 Off command extensions: 
         set OnTime attribute to 0x0000 */
      tempData = 0x0000;
      (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &tempData);
#endif   
      
      break;
    }
    case gZclCmdOnOff_On_c:
    {
      event = gZclUI_On_c;
      onOffAttr = 1;   
      
#if gASL_EnableZLLClustersData_d 
      /* [R3] 6.6.1.4.2 On command extensions */
      
      /* get OnTime attribute */
      (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &tempData, NULL);
      if(tempData == 0x0000)
      {
        /* set OffWaitTime to 0x0000 */
        (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempData);
      }
      /* set global scene to TRUE */
      (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &onOffAttr);

#endif       
      break;
    }
#if gASL_EnableZLLClustersData_d
    case gZclCmdOnOff_OffWithEffect_c:
    {
      zbStatus_t tempStatus = OnOffServer_OffWithEffectHandler(pIndication, pDevice);
      if(tempStatus != gZclSuccess_c)
        status = tempStatus;
      return status;
    }
    case gZclCmdOnOff_OnWithRecallGlobalScene_c:
    {
      zbStatus_t tempStatus = OnOffServer_OnWithRecallGlobalSceneHandler(pIndication, pDevice);
      if(tempStatus != gZclSuccess_c)
        status = tempStatus;
      return status;
    }
    case gZclCmdOnOff_OnWithTimedOff_c:
    {
      zbStatus_t tempStatus = OnOffServer_OnWithTimedOffHandler(pIndication);
      if(tempStatus != gZclSuccess_c)
        status = tempStatus;
      return status;
    }    
#endif    
    default:
      return gZclUnsupportedClusterCommand_c;      
  }

#if gZclEnableLevelControlServer_d 
  {
    /* [R2]3.10.2.1. Create dependencies btw OnOff and Level control cluster*/
    zbClusterId_t aLevelClusterId = {gaZclClusterLevelControl_c};
    uint8_t indexSetup = ZCL_LevelControl_GetIndexFromLevelSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
#if gZclClusterOptionals_d
    uint8_t onLevel = gZclLevel_UndefinedOnLevel_c;
#endif
    
    if(indexSetup == gZclCluster_InvalidDataIndex_d)
      return status;
    
#if gZclClusterOptionals_d
    (void)ZCL_GetAttribute(pIndication->dstEndPoint, aLevelClusterId, gZclAttrLevelControl_OnLevelId_c, gZclServerAttr_c, &onLevel, NULL);
    if (onLevel != gZclLevel_UndefinedOnLevel_c)
      gLevelControlServerSetup[indexSetup].lastCurrentLevel = onLevel;    /* Set to the OnLevel value */              
#endif
    /* process onOff Commands */
    (void)ZCL_LevelControlOnOffCommandsHandler(pIndication->dstEndPoint, onOffAttr, 0xFFFF, gLevelControlServerSetup[indexSetup].lastCurrentLevel);
  }
  (void)event;
  
#else   /*gZclEnableLevelControlServer_d */ 
  
  /* sets the attribute and will report if needed */
  (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffAttr);
  /* send the event to the app */
  BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);  
  
#endif
  
  /* worked */
  return status;
}

#if gASL_EnableZLLClustersData_d
/*!
 * @fn 		zbStatus_t OnOff_OffWithEffectReq(zclOnOff_OffWithEffect_t *pReq) 
 *
 * @brief	Sends over-the-air an Off with Effect request from OnOff Cluster Client. 
 *
 */
zbStatus_t OnOff_OffWithEffectReq
(
  zclOnOff_OffWithEffect_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterOnOff_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdOnOff_OffWithEffect_c, sizeof(zclCmdOnOff_OffWithEffect_t), (zclGenericReq_t *)pReq);
}


/*!
 * @fn 		zbStatus_t OnOff_OnWithRecallGlobalSceneReq(zclOnOff_OnWithRecallGlobalScene_t *pReq) 
 *
 * @brief	Sends over-the-air an On with Recall Global Scene request from OnOff Cluster Client. 
 *
 */
zbStatus_t OnOff_OnWithRecallGlobalSceneReq
(
  zclOnOff_OnWithRecallGlobalScene_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterOnOff_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdOnOff_OnWithRecallGlobalScene_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t OnOff_OnWithTimedOffReq(zclOnOff_OnWithTimedOff_t *pReq) 
 *
 * @brief	Sends over-the-air an On with Timed Off request from OnOff Cluster Client. 
 *
 */
zbStatus_t OnOff_OnWithTimedOffReq
(
  zclOnOff_OnWithTimedOff_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterOnOff_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdOnOff_OnWithTimedOff_c, sizeof(zclCmdOnOff_OnWithTimedOff_t), (zclGenericReq_t *)pReq);
}
#endif

#if gASL_EnableZLLClustersData_d
/*!
 * @fn 		zbStatus_t ZCL_OnOffServerInit(void)
 *
 * @brief	Init COnOff Cluster Server
 *
 */
zbStatus_t ZCL_OnOffServerInit(void)
{ 
  zbStatus_t status = gZclFailure_c;
  uint8_t nextStartIndex = 0, i = 0;
  zbClusterId_t onOffClusterId = {gaZclClusterOnOff_c};
    
  for(i = 0; i< gNoOfOnOffServerInstances_d; i++)
  {
      uint8_t endpoint;
      uint8_t startIndex = nextStartIndex;
     
      gOnOffServerSetup[i].endpoint = gZclCluster_InvalidDataIndex_d;
      gOnOffServerSetup[i].onOffTransitionTmrId = gTmrInvalidTimerID_c;
      
      endpoint = ZCL_GetEndPointForSpecificCluster(onOffClusterId, TRUE, startIndex, &nextStartIndex);
      if(endpoint != gZclCluster_InvalidDataIndex_d)  
      {       
        gOnOffServerSetup[i].endpoint = endpoint;
        status = gZclSuccess_c;
      }
  }

  return status;
}

/*!
 * @fn 		uint8_t OnOffServer_GetIndexFromOnOffSetupTable(uint8_t endpoint, uint8_t tmrId)
 *
 * @brief	return - index in the OnOff setup table if succes
 *                     - invalid data - otherwise
 */
static uint8_t OnOffServer_GetIndexFromOnOffSetupTable(uint8_t endpoint, uint8_t tmrId)
{
  uint8_t i;
  if(endpoint != gZclCluster_InvalidDataIndex_d) 
  { 
    for(i=0;i<gNoOfOnOffServerInstances_d; i++)
      if(endpoint == gOnOffServerSetup[i].endpoint)
        return i;
  }
  if(tmrId != gTmrInvalidTimerID_c)
  {
     for(i=0;i<gNoOfOnOffServerInstances_d; i++)
      if(tmrId == gOnOffServerSetup[i].onOffTransitionTmrId)
        return i;
  }
  return gZclCluster_InvalidDataIndex_d;
}
/*!
 * @fn 		static void OnOffServer_TimerCallBack(uint8_t tmrId)
 *
 * @brief	OnOff Server timer Callback 
 *
 */
static void OnOffServer_TimerCallBack(uint8_t tmrId)
{
  zbClusterId_t aOnOffClusterId = {gaZclClusterOnOff_c};
  uint8_t onOffState = 0x00;
  uint16_t tempTime = 0x0000;
  
  /* verify onoff server table setup */
  uint8_t indexSetup = OnOffServer_GetIndexFromOnOffSetupTable(gZclCluster_InvalidDataIndex_d, tmrId);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return;
  
  /* get current OnOff State */
  (void)ZCL_GetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffState, NULL);

  
  if(gOnOffServerSetup[indexSetup].onOffLastCommand == gZclCmdOnOff_OffWithEffect_c)
  {
    /* verify next Time Period */
    if(gOnOffServerSetup[indexSetup].nextTimePeriod != 0x00)
    {
      (void)OnOffServer_SetTimeout(indexSetup, gOnOffServerSetup[indexSetup].nextTimePeriod);
      gOnOffServerSetup[indexSetup].nextTimePeriod = 0x00;
      return;
    }
    else
    {
      if(onOffState)
      {
        /* set onOff and OnTime Attributes */
        onOffState = 0x00;
        (void)ZCL_SetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffState); 
        /* set global scene control to FALSE */
        (void)ZCL_SetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &onOffState);        
      }
      /* get OffWaitTime */
      (void)ZCL_GetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime, NULL);
      /* start decreasing the OffWaitTime */
      (void)ZCL_GetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime, NULL);
      if(tempTime > 0x00)
      {
         tempTime--;
         (void)ZCL_SetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime);
      }
    }
  }
  if(gOnOffServerSetup[indexSetup].onOffLastCommand == gZclCmdOnOff_OnWithTimedOff_c)
  {
    if(onOffState == 0x00)
    {
       /* get OffWaitTime */
       (void)ZCL_GetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime, NULL);
       if(tempTime > 0x00)
       {
         tempTime--;
         (void)ZCL_SetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime);
       }
    }
    else
    {
       /* get OnWaitTime */
       (void)ZCL_GetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &tempTime, NULL);
       if(tempTime > 0x00)
       {
         tempTime--;
         (void)ZCL_SetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &tempTime);
         if(tempTime == 0x00)
         {
           /* set onOff attribute */
           onOffState = 0x00;
          (void)ZCL_SetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffState); 
          /* set offWaitTime to 0x0000 */
          (void)ZCL_SetAttribute(gOnOffServerSetup[indexSetup].endpoint, aOnOffClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime); 
 
         }
       }
    } 
  }
  
  if(tempTime)
  {
    (void)OnOffServer_SetTimeout(indexSetup, 100);
    return;
  }
  
  /* free the timer */
  ZbTMR_FreeTimer(gOnOffServerSetup[indexSetup].onOffTransitionTmrId);
  gOnOffServerSetup[indexSetup].onOffTransitionTmrId = gTmrInvalidTimerID_c; 
}

/*!
 * @fn 		zbStatus_t OnOffServer_SetTimeout(uint8_t indexSetup, uint32_t duration)
 *
 * @brief	Set OnOff timeout. Duration is in miliseconds
 *
 */
zbStatus_t OnOffServer_SetTimeout(uint8_t indexSetup, uint32_t duration)
{
  if(gOnOffServerSetup[indexSetup].onOffTransitionTmrId == gTmrInvalidTimerID_c)
  {
    gOnOffServerSetup[indexSetup].onOffTransitionTmrId = ZbTMR_AllocateTimer(); 
    if(gOnOffServerSetup[indexSetup].onOffTransitionTmrId == gTmrInvalidTimerID_c)
      return gZclFailure_c;
  }
  
  ZbTMR_StartTimer(gOnOffServerSetup[indexSetup].onOffTransitionTmrId, gTmrSingleShotTimer_c, duration, OnOffServer_TimerCallBack);
  return gZclSuccess_c;
}
#endif
/*!
 * @fn 		zbStatus_t ZCL_LevelControlOnOffCommandsHandler(uint8_t endpoint, uint8_t onOffStatus, uint16_t transitionTime, uint8_t lastCurrentValue)

 *
 * @brief	Process OnOff Command, according with Level control cluster.
 *
 */
zbStatus_t ZCL_LevelControlOnOffCommandsHandler(uint8_t endpoint, uint8_t onOffStatus, uint16_t transitionTime, uint8_t lastCurrentValue)
{
  zbClusterId_t aLevelClusterId = {gaZclClusterLevelControl_c};
  zbClusterId_t aOnOffClusterId = {gaZclClusterOnOff_c};
  zclCmdLevelControl_MoveToLevel_t Req;
  uint8_t minLevel = gZclLevel_LowestPossible;
  uint8_t indexSetup = ZCL_LevelControl_GetIndexFromLevelSetupTable(endpoint, gTmrInvalidTimerID_c);
   
#if gZclEnableShadeConfigurationCluster_d  
  zbClusterId_t aClusterShadeId = {gaZclClusterShadeCfg_c};  
#endif

  if(onOffStatus > 0x01)
    return gZclFailure_c;
  
#if gZclEnableShadeConfigurationCluster_d   
  if(gShadeDevice == TRUE)
  {
     gShadeDeviceActive = TRUE; 
    (void)ZCL_GetAttribute(endpoint, aClusterShadeId, gZclAttrShadeCfgInfStatus_c, gZclServerAttr_c, &gStatusShadeCfg, NULL);
  }
#endif  

  if(indexSetup == gZclCluster_InvalidDataIndex_d)
      return gZclFailure_c;
  gLevelControlServerSetup[indexSetup].levelCommand = gZclLevel_OnOffCmd_c;
  
  /* Keep a copy of current level*/
  (void)ZCL_GetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &gLevelControlServerSetup[indexSetup].lastCurrentLevel, NULL);      

  if(onOffStatus == 0x00)
  {    
      /* move current level to the minimum level allowed, over a time period*/
      Req.level = minLevel;     
  }
  else
  {    
      /*set onOff attribute - used to display level during the transition time */
      (void)ZCL_SetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffStatus);   
      /* set the current level to the minimum Level allowed  for the device */
      (void)ZCL_SetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &minLevel);
      /* move current level to OnLevel or to stored Level, over a time period */
      Req.level = lastCurrentValue;       
  }
  
  /* Set transition time to 0xFFFF to use the OnOffTransition time, otherwise use a valid value*/
  Req.transitionTime = transitionTime; 
  /* sets the attribute and will report if needed */
  gLevelControlServerSetup[indexSetup].onoffState = onOffStatus;
  return ZCL_LevelControlMoveToLevel(&Req, TRUE, gLevelControlServerSetup[indexSetup].endpoint);
}

#if gASL_EnableZLLClustersData_d
/*!
 * @fn 		static zbStatus_t OnOffServer_OffWithEffectHandler(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Handle Off With Effect command received from OnOff Client
 *
 */
zbStatus_t OnOffServer_OffWithEffectHandler(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclCmdOnOff_OffWithEffect_t *pOffWithEffect;
  bool_t globalSceneCtrl;
  uint32_t transitionTime;
  uint16_t tempTime;
  uint8_t indexSetup, onOffState;
#if gZclEnableLevelControlServer_d
  uint8_t level;
  zbClusterId_t aLevelClusterId = {gaZclClusterLevelControl_c};
#endif  
  
  /* verify onoff server table setup */
  indexSetup = OnOffServer_GetIndexFromOnOffSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
  
  /* verify OnOff status -  if is Off, discard the command */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffState, NULL);
  if(onOffState == 0x00)
    return gZclFailure_c;
  
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pOffWithEffect = (zclCmdOnOff_OffWithEffect_t *)(pFrame+1); 
  
  /* validate payload: if not supported, use default */
  if(pOffWithEffect->effectId > gOnOffEffectId_DyingLight_c)
    pOffWithEffect->effectId = gOnOffEffectId_DyingLight_c;
  
  /* verify Global Scene Control Attribute */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &globalSceneCtrl, NULL);
  if(globalSceneCtrl == TRUE)
  {
    /* store settings in global scene */
    zclSceneTableEntry_t *pScene; /* scene table entry */
    zclCmdScene_StoreScene_t globalScene = {{0x00, 0x00}, 0x00};

    /* (re)allocate the scene for storage */
    pScene = ZCL_AllocateScene(pDevice, &globalScene);

    /* store current data to the scene */
    if(pScene) 
    {
      HA_StoreScene(pIndication->dstEndPoint, pScene);
      ZCL_SaveNvmZclData();
    }
    
    /* set global scene control to False */
    globalSceneCtrl = FALSE;
    (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &globalSceneCtrl);
  }
  
  /* set onTime to 0x0000 */
  tempTime = 0x0000;
  (void)ZCL_SetAttribute(gOnOffServerSetup[indexSetup].endpoint, pIndication->aClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &tempTime); 

  gOnOffServerSetup[indexSetup].onOffLastCommand = gZclCmdOnOff_OffWithEffect_c;
  onOffState = 0x00; /* move down */
  
  /* verify effect Id and Effect Variant */
  if(pOffWithEffect->effectId == gOnOffEffectId_DelayedAllOff_c)
  {
    /* set transition time */
    switch(pOffWithEffect->effectVariant)
    {
      case gOnOffEffectVariant_Default_c:
      {
        transitionTime = gOnOff_DefaultFadeToOff_d;
        gOnOffServerSetup[indexSetup].nextTimePeriod = 0x00;       
        break;
      }
      case gOnOffEffectVariant_NoFade_c:
      {
        transitionTime = 0x00;
        gOnOffServerSetup[indexSetup].nextTimePeriod = 0x00;     
        break;
      }
      case gOnOffEffectVariant_DimDown_c:
      {
        transitionTime = gOnOff_DimDown50FadeToOff_d;
        
        /* keep onOff State up, until next time period */
        onOffState = 0x01; 
        gOnOffServerSetup[indexSetup].nextTimePeriod = gOnOff_SecondFadeToOff_d;
        
#if gZclEnableLevelControlServer_d
        /* get current Level */
       (void)ZCL_GetAttribute(pIndication->dstEndPoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &level, NULL);
       /* dim down 50% */
       level = (level/2 > gZclLevel_LowestPossible)?level/2:gZclLevel_LowestPossible;
#endif        
        break;
      }     
      default:
      {
        transitionTime = 0x00;
        gOnOffServerSetup[indexSetup].nextTimePeriod = 0x00;
        break;
      }
    }
  }
  else
  {
    if(pOffWithEffect->effectVariant == gOnOffEffectVariant_Default_c)
    {
      transitionTime = gOnOff_DimUp20FadeToOff_d;
      
      /*  keep onOff State up, until next time period */;
      onOffState = 0x01; 
      gOnOffServerSetup[indexSetup].nextTimePeriod = gOnOff_DyingLightFadeToOff_d;
      
#if gZclEnableLevelControlServer_d
      /* get current Level */
      (void)ZCL_GetAttribute(pIndication->dstEndPoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &level, NULL);
      /* dim up 20% */
      level = ((level + (level*20)/100) < gZclLevel_HighestPossible)?level + (level*20)/100:gZclLevel_HighestPossible;
#endif      
    }
    else
    {
      transitionTime = 0x00;
      gOnOffServerSetup[indexSetup].nextTimePeriod = 0x00;
    }
  }
  
 
#if gZclEnableLevelControlServer_d  
  /* stop any transition for OnOff cluster */
  if(gOnOffServerSetup[indexSetup].onOffTransitionTmrId != gTmrInvalidTimerID_c)
    ZbTMR_StopTimer(gOnOffServerSetup[indexSetup].onOffTransitionTmrId);
  /* process onOff Command using Level Control timer */
  transitionTime = transitionTime/100;
  gOnOffServerSetup[indexSetup].nextTimePeriod /=100;
  (void)ZCL_LevelControlOnOffCommandsHandler(pIndication->dstEndPoint, onOffState, transitionTime, level);
#else
  /* process onOff command using OnOff timer */
  (void)OnOffServer_SetTimeout(indexSetup, transitionTime);
#endif
  
  return status;
}

/*!
 * @fn 		static zbStatus_t OnOffServer_OnWithRecallGlobalSceneHandler(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Handle On With Recall Global Scene command received from OnOff Client
 *
 */
zbStatus_t OnOffServer_OnWithRecallGlobalSceneHandler(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
{
  zbStatus_t status = gZclSuccess_c;
  uint8_t *pData;
  zclSceneAttrs_t *pSceneData;
  bool_t globalSceneCtrl;
  uint8_t onOffState, indexSetup;
  uint16_t onTime;
  zclSceneTableEntry_t *pScene; /* scene table entry */
  zclCmdScene_RecallScene_t globalScene = {{0x00, 0x00}, 0x00};
  
  /* verify onoff server table setup */
  indexSetup = OnOffServer_GetIndexFromOnOffSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
  
  /* verify GlobalScene Control attribute*/
  (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &globalSceneCtrl, NULL);
  if(globalSceneCtrl == TRUE)
    return gZclFailure_c;
  
  /* verify OnOff status -  if is On, discard the command */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffState, NULL);
  if(onOffState == 0x01)
    return gZclFailure_c;
  
  /* get ptr to device data for this endpoint */
  pData = pDevice->pData;
  pSceneData = (zclSceneAttrs_t*) pDevice->pSceneData;    
  
  pScene = ZCL_FindScene(pDevice, &globalScene); 
  /* recall the scene */
  if(pScene) 
  {
    HA_RecallScene(pIndication->dstEndPoint, pData, pScene);
    pSceneData->sceneValid = gZclSceneValid_c;
    pSceneData->currentScene = pScene->sceneId;
    Copy2Bytes(pSceneData->currentGroup, pScene->aGroupId);
    
    /* set global scene control attribute to TRUE */
    globalSceneCtrl = TRUE;
    (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &globalSceneCtrl);
    
    /* get onTime Attribute */
    (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &onTime, NULL);
    if(onTime == 0x0000)
    {
      /* set off wait time to 0x0000*/
      (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &onTime);
    }
 
  }
  else
  {
    status = gZclFailure_c;
  }
 
  return status;
}

/*!
 * @fn 		static zbStatus_t OnOffServer_OnWithTimedOffHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle On With Timed Off command received from OnOff Client
 *
 */
zbStatus_t OnOffServer_OnWithTimedOffHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclCmdOnOff_OnWithTimedOff_t *pOnWithTimedOff;
  uint8_t onOffState, indexSetup;
  uint16_t offWaitTime, onTime;
  
  /* verify onoff server table setup */
  indexSetup = OnOffServer_GetIndexFromOnOffSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
  
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pOnWithTimedOff = (zclCmdOnOff_OnWithTimedOff_t *)(pFrame+1); 
  pOnWithTimedOff->offWaitTime = OTA2Native16(pOnWithTimedOff->offWaitTime);
  pOnWithTimedOff->onTime = OTA2Native16(pOnWithTimedOff->onTime);
  
  gOnOffServerSetup[indexSetup].onOffLastCommand = gZclCmdOnOff_OnWithTimedOff_c;
  
  /* verify OnOff status: discard the command if OnOff = 0x00 and onoffControl = 0x01 */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffState, NULL);
  if((onOffState == 0x00) && (pOnWithTimedOff->onOffControl & 0x01))
    return gZclFailure_c;
  
  /* get off WaitTime, onTime */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &offWaitTime, NULL);
  (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &onTime, NULL);

  if((offWaitTime > 0x00) && (onOffState == 0x00))
  {
    /* set offWait time attribute to the min(currentValue, commandFieldValue) */
    offWaitTime = (offWaitTime > pOnWithTimedOff->offWaitTime)?pOnWithTimedOff->offWaitTime:offWaitTime;
  }
  else
  {
    /* set on time atribute to the max(currentValue, commandFieldValue)*/
     onTime = (onTime > pOnWithTimedOff->onTime)?onTime:pOnWithTimedOff->onTime;
    (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &onTime);
    /* set offWait Time atribute to commandFieldValue */
    offWaitTime = pOnWithTimedOff->offWaitTime;

    /*set OnOff atribute to 0x01*/
    onOffState = 0x01;
    (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffState);
    
#if gASL_EnableZLLClustersData_d 
    /* set global scene control to TRUE */
    (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &onOffState);
#endif
  }
  
  /* set offWait Time atribute */
  (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &offWaitTime);

  
  /* remain in the current state if onTime and offWaitTime are both 0xFFFF or 0x0000 */
  if(onTime == 0xFFFF && offWaitTime == 0xFFFF)
    return status;
  
  if((onOffState == 0x01 && onTime > 0x00)||
     (onOffState == 0x00 && offWaitTime > 0x00))
  {
      /* update the device every 1/10th second until both the OnTime 
         and OffWaitTime attributes are equal to 0x0000 */
     (void)OnOffServer_SetTimeout(indexSetup, 100);
  }
  
  
  return status;
}
#endif


/*!
 * @fn 		zbStatus_t ZCL_GenericReqNoData(afAddrInfo_t *pAddrInfo, zclCmd_t command)
 *
 * @brief	Standard request when there is no payload.Frame control disables default response from receiver.
 *
 */
zbStatus_t ZCL_GenericReqNoData(afAddrInfo_t *pAddrInfo, zclCmd_t command)
{
  uint8_t payloadLen;
  afToApsdeMessage_t *pMsg;

  /* create the frame and copy in payload */
  payloadLen = 0;
  pMsg = ZCL_CreateFrame(pAddrInfo,command,gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp,
    NULL, &payloadLen,NULL);
  if(!pMsg)
    return gZclNoMem_c;

  /* send the frame to the destination */
  return ZCL_DataRequestNoCopy(pAddrInfo, payloadLen, pMsg);
}

/*!
 * @fn 		zbStatus_t ZCL_GenericReqNoDataServer(afAddrInfo_t *pAddrInfo, zclCmd_t command)
 *
 * @brief	Standard request from Server when there is no payload.Frame control disables default response from receiver.
 *
 */
zbStatus_t ZCL_GenericReqNoDataServer(afAddrInfo_t *pAddrInfo, zclCmd_t command)
{
  uint8_t payloadLen;
  afToApsdeMessage_t *pMsg;

  /* create the frame and copy in payload */
  payloadLen = 0;
  pMsg = ZCL_CreateFrame(pAddrInfo, command,gZclFrameControl_FrameTypeSpecific |gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp,
    NULL, &payloadLen,NULL);
  if(!pMsg)
    return gZclNoMem_c;

  /* send the frame to the destination */
  return ZCL_DataRequestNoCopy(pAddrInfo, payloadLen, pMsg);
}

/******************************
  On/Off Switch Configuration Cluster
  See ZCL Specification Section 3.9
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_OnOffSwitchClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the OnOffSwitch Cluster Server. 
 *
 */
zbStatus_t ZCL_OnOffSwitchClusterServer 
(
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
)
{
  (void) pIndication;
  (void) pDevice;
   return gZbSuccess_c;
}

/******************************
  Level Control Cluster
  See ZCL Specification Section 3.10
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_LevelControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Level Control Cluster Server. 
 *
 */
zbStatus_t ZCL_LevelControlClusterServer
  (
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
  ) 
{ 
  zclFrame_t *pFrame;
  zclLevelControlCmd_t Command; 
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  bool_t withOnOff = FALSE;
  uint8_t indexSetup; 
#if gZclEnableShadeConfigurationCluster_d  
  zbClusterId_t aClusterShadeId = {gaZclClusterShadeCfg_c};
  if(gShadeDevice == TRUE)
  {
     gShadeDeviceActive = TRUE; 
    (void)ZCL_GetAttribute(gZcl_ep, aClusterShadeId, gZclAttrShadeCfgInfStatus_c, gZclServerAttr_c, &gStatusShadeCfg, NULL);
  }
#endif 
  
  (void)pDevice;

  indexSetup = ZCL_LevelControl_GetIndexFromLevelSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZbFailed_c;
  
  gLevelControlServerSetup[indexSetup].levelCommand = gZclLevel_NotOnOffCmd_c;

  /* ZCL frame */
  pFrame = (zclFrame_t*)pIndication->pAsdu;

  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;
    
  
  /* make local copy of command (might be move to level, step, etc...) */
  FLib_MemCpy(&Command,(pFrame + 1), sizeof(Command));
  
  
  /* handle the LevelControl commands */
  switch(pFrame->command)
  {
    /* Move to Level Commands */
    case gZclCmdLevelControl_MoveToLevelOnOff_c:
      withOnOff = TRUE;
    case gZclCmdLevelControl_MoveToLevel_c:        
      /* set on level and current level */
      (void)ZCL_LevelControlMoveToLevel(&Command.MoveToLevelCmd, withOnOff, pIndication->dstEndPoint);
      break;

    /* Move commands */
    case gZclCmdLevelControl_MoveOnOff_c:    
      withOnOff = TRUE;    
    case gZclCmdLevelControl_Move_c:
      (void)ZCL_LevelControlMove(&Command.MoveCmd, withOnOff, pIndication->dstEndPoint);
      break;

    /* Step Commands */
    case gZclCmdLevelControl_StepOnOff_c:
      withOnOff = TRUE;    
    case gZclCmdLevelControl_Step_c:
      (void)ZCL_LevelControlStep(&Command.StepCmd, withOnOff, pIndication->dstEndPoint);
      break;

    /* Stop Commands */
    case gZclCmdLevelControl_StopOnOff_c:
    case gZclCmdLevelControl_Stop_c:
      (void)ZCL_LevelControlStop(pIndication->dstEndPoint);
      break;      

    /* command not supported on this cluster */
    default:
      return gZclUnsupportedClusterCommand_c;
  }
  return status;
}


/*****************************************************************************
* ZCL_LevelControlServerInit Fuction
* -return TRUE - InitSucces
          FALSE - otherwise
*****************************************************************************/
zbStatus_t ZCL_LevelControlServerInit(void)
{ 
  zbStatus_t status = gZclFailure_c;
  uint8_t nextStartIndex = 0, i = 0;
  zbClusterId_t levelControlClusterId = {gaZclClusterLevelControl_c};
    
  for(i = 0; i< gNoOfLevelControlServerInstances_d; i++)
  {
      uint8_t endpoint;
      uint8_t startIndex = nextStartIndex;
     
      gLevelControlServerSetup[i].endpoint = gZclCluster_InvalidDataIndex_d;
      gLevelControlServerSetup[i].zclLevel_TransitionTmrID = gTmrInvalidTimerID_c;
      
      endpoint = ZCL_GetEndPointForSpecificCluster(levelControlClusterId, TRUE, startIndex, &nextStartIndex);
      if(endpoint != gZclCluster_InvalidDataIndex_d)  
      {
  
        gLevelControlServerSetup[i].endpoint = endpoint;
        gLevelControlServerSetup[i].levelOnOff = FALSE;
        gLevelControlServerSetup[i].levelCommand = gZclLevel_NotOnOffCmd_c;
        gLevelControlServerSetup[i].onoffState = gZclCmdOnOff_Off_c;
        gLevelControlServerSetup[i].timeBtwChanges = 0x00;
        gLevelControlServerSetup[i].levelDifference = 0x00;
        gLevelControlServerSetup[i].newCurrentLevel = gZclLevel_off;
        /* Initializa the timer for the Level Control Cluster */
        gLevelControlServerSetup[i].zclLevel_TransitionTmrID = ZbTMR_AllocateTimer(); 
        if(gLevelControlServerSetup[i].zclLevel_TransitionTmrID == gTmrInvalidTimerID_c)
          return gZclFailure_c;
        status = gZclSuccess_c;
      }
  }

  return status;
}
/*****************************************************************************
* ZCL_LevelControl_GetIndexFromLevelSetupTable Fuction
* -return - index in the level setup table if succes
          - invalid data - otherwise
*****************************************************************************/
uint8_t ZCL_LevelControl_GetIndexFromLevelSetupTable(uint8_t endpoint, uint8_t tmrId)
{
  if(endpoint != gZclCluster_InvalidDataIndex_d) 
  {  
    for(uint8_t i=0;i<gNoOfLevelControlServerInstances_d; i++)
      if(endpoint == gLevelControlServerSetup[i].endpoint)
        return i;
  }
  if(tmrId != gTmrInvalidTimerID_c)
  {
     for(uint8_t i=0;i<gNoOfLevelControlServerInstances_d; i++)
      if(tmrId == gLevelControlServerSetup[i].zclLevel_TransitionTmrID)
        return i;
  }
  return gZclCluster_InvalidDataIndex_d;
}


/*!
 * @fn 		void ZCL_LevelControlTimer( uint8_t timerId )
 *
 * @brief	Callback used to update the current level. Sends events to application.
 *
 */
void ZCL_LevelControlTimer( uint8_t timerId )
{
  zbClusterId_t aLevelClusterId = {gaZclClusterLevelControl_c};
  zbClusterId_t aClusterId = {gaZclClusterOnOff_c};
#if gZclEnableShadeConfigurationCluster_d  
  zbClusterId_t aClusterShadeId = {gaZclClusterShadeCfg_c};
#endif
  uint8_t zclCurrentLevelTmp;
  uint8_t OnOffStatus; 
  bool_t  updateOnOffStatus = FALSE;
  uint8_t indexSetup;
  
  /* verify level control table */
  indexSetup = ZCL_LevelControl_GetIndexFromLevelSetupTable(gZclCluster_InvalidDataIndex_d, timerId);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return;
  
  /* get data of interest: current level, onoffStatus */
  (void)ZCL_GetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &OnOffStatus,NULL);
  (void)ZCL_GetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &zclCurrentLevelTmp, NULL);
 
  /* process level control command */
  if (gLevelControlServerSetup[indexSetup].levelDifference >= gZclLevel_Step)      
  {
    gLevelControlServerSetup[indexSetup].levelDifference -= gZclLevel_Step;
    if (zclMoveMode_Down == gLevelControlServerSetup[indexSetup].moveMode)
    {
#if gZclEnableShadeConfigurationCluster_d   
      /* update shade configuration data if required*/
      if(gShadeDevice == TRUE)
      {
          statusShadeCfg.ShadeDirection = 0;
          statusShadeCfg.ShadeForwardDir = 0;
      }
#endif      
      zclCurrentLevelTmp -= gZclLevel_Step;
    }
    else
    {
#if gZclEnableShadeConfigurationCluster_d
      /* update shade configuration data if required*/      
      if(gShadeDevice == TRUE)
      {
          statusShadeCfg.ShadeDirection = 1;
          statusShadeCfg.ShadeForwardDir = 1;
      }
#endif      
      zclCurrentLevelTmp +=gZclLevel_Step;
      
      /* level is increased -> should update the OnOff Status if required */
      if (gLevelControlServerSetup[indexSetup].levelCommand == gZclLevel_OnOffCmd_c) 
      {
        uint8_t onoffStatus = 0x01; 
        /* There was an OnOff command, update the OnOff attribute */
        (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onoffStatus);     
#if gASL_EnableZLLClustersData_d 
        /* set global scene control to TRUE */
        (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &onoffStatus);
#endif
      }
    }
  }
  else
  {
      gLevelControlServerSetup[indexSetup].levelDifference  = 0;
      zclCurrentLevelTmp = gLevelControlServerSetup[indexSetup].newCurrentLevel;
  }
  
  /* update current Level */
  (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c,&zclCurrentLevelTmp);

  /* Recalculate the remaining time, substracting the time Between Changes */
  if (gLevelControlServerSetup[indexSetup].remainingTime)
  {
    if ((gLevelControlServerSetup[indexSetup].levelDifference  == 0) &&(gLevelControlServerSetup[indexSetup].remainingTime > gLevelControlServerSetup[indexSetup].timeBtwChanges))
      gLevelControlServerSetup[indexSetup].timeBtwChanges = (uint16_t)gLevelControlServerSetup[indexSetup].remainingTime;
    else
      gLevelControlServerSetup[indexSetup].remainingTime -= gLevelControlServerSetup[indexSetup].timeBtwChanges;               
  }
   
#if gZclClusterOptionals_d 
  {
    uint16_t remainingTime =  Native2OTA16((uint16_t) (gLevelControlServerSetup[indexSetup].remainingTime / 100));    
   (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c, gZclServerAttr_c,&remainingTime);
  }
#endif

  /* verify level difference */
  if(gLevelControlServerSetup[indexSetup].levelDifference) 
  {
    /* We didn't get to the specified level, so restart the timer */
    ZbTMR_StartSingleShotTimer(gLevelControlServerSetup[indexSetup].zclLevel_TransitionTmrID,(zbTmrTimeInMilliseconds_t)gLevelControlServerSetup[indexSetup].timeBtwChanges, ZCL_LevelControlTimer);
  }
  else 
  {
    /* get the specified level */
    updateOnOffStatus = TRUE;
    
#if gZclClusterOptionals_d 
    {
      /* set remaining time to 0x00 */
      uint16_t remainingTime = 0;
      (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c, gZclServerAttr_c,&remainingTime);
    }
#endif
    
    /* verify level command: OnOff cluster command or Level Control Cluster command */
    if (gLevelControlServerSetup[indexSetup].levelCommand == gZclLevel_OnOffCmd_c) 
    {
      uint8_t currentLevelValue = 0; 	
     
      /* There was an OnOff command, update the OnOff attribute */
      (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &gLevelControlServerSetup[indexSetup].onoffState);

#if gASL_EnableZLLClustersData_d 
      /* update global scene control*/
      (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &gLevelControlServerSetup[indexSetup].onoffState);
#endif
        
      /* get OnLevel - store temporarily the value in the OnOffStatus */
      OnOffStatus = gZclLevel_UndefinedOnLevel_c; 
      
#if gZclClusterOptionals_d				        
      (void)ZCL_GetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aLevelClusterId, gZclAttrLevelControl_OnLevelId_c, gZclServerAttr_c, &OnOffStatus, NULL);        
#endif

      /*  update level control attribute */
      currentLevelValue = gLevelControlServerSetup[indexSetup].lastCurrentLevel;
      if (OnOffStatus == gZclLevel_UndefinedOnLevel_c)
      {
        /* If OnLevel is not defined, set the CurrentLevel to the last stored level */
        (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &currentLevelValue);
      }
      else
      {
         if(gLevelControlServerSetup[indexSetup].onoffState == gZclCmdOnOff_On_c)
         {
           /* for On command if OnLevel is defined, set the Current Level to the last stored Level */
            currentLevelValue = OnOffStatus; 
            (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &currentLevelValue);
         }
      }
#if gASL_EnableZLLClustersData_d    
      /* verify onoff server table setup, 
          in order to update OnOffState, OnTime and OffWait Time*/
      if(gOnOffServerSetup[indexSetup].onOffLastCommand == gZclCmdOnOff_OffWithEffect_c &&
              gOnOffServerSetup[indexSetup].nextTimePeriod != 0x00)
      {
          uint32_t nextTimePeriod = gOnOffServerSetup[indexSetup].nextTimePeriod;
        
          gOnOffServerSetup[indexSetup].nextTimePeriod = 0x00;
          (void)ZCL_LevelControlOnOffCommandsHandler(gLevelControlServerSetup[indexSetup].endpoint, 0x00, nextTimePeriod, gZclLevel_LowestPossible);    
      }
      else
      {
          uint16_t tempTime = 0x0000;
        
          /* start decreasing the OffWaitTime */
          (void)ZCL_GetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime, NULL);
          if(tempTime > 0x00)
          {
            tempTime--;
            (void)ZCL_SetAttribute(gOnOffServerSetup[indexSetup].endpoint, aClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime);
            (void)OnOffServer_SetTimeout(indexSetup, 100);
          }
       }
#endif      
    }
    else if (gLevelControlServerSetup[indexSetup].levelOnOff)
    {         
        uint8_t minLevel = gZclLevel_LowestPossible;
        
        /* There was an Move or MoveToLevel with OnOff option, so update the OnOff attribute, based on the current level */   
        if (zclCurrentLevelTmp > minLevel)
        {
          OnOffStatus = gZclCmdOnOff_On_c;
        }
        else
        {
#if gASL_EnableZLLClustersData_d 
          /* [R3]: 6.6.6.1. On receipt of a level control cluster command that causes the OnOff 
            attribute to be set to 0x00, the OnTime attribute shall be set to 0x0000 */
          uint16_t tempTime = 0x00; 
          (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &tempTime);
#endif          
          OnOffStatus = gZclCmdOnOff_Off_c;     
        }
        
        (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &OnOffStatus);
#if gASL_EnableZLLClustersData_d 
        /* update global scene control*/
        (void)ZCL_SetAttribute(gLevelControlServerSetup[indexSetup].endpoint, aClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &OnOffStatus);
#endif
#if gZclEnableShadeConfigurationCluster_d      
        if(gShadeDevice == TRUE)
          gShadeDeviceActive = FALSE;     
#endif      
    }
  }
  BeeAppUpdateDevice(gLevelControlServerSetup[indexSetup].endpoint, gZclUI_GoToLevel_c, 0, 0, (void*) &updateOnOffStatus);
}

/*!
 * @fn 		zbStatus_t ZCL_LevelControlMoveToLevel(zclCmdLevelControl_MoveToLevel_t *pReq, bool_t withOnOff) 
 *
 * @brief	On receipt of this command, a device shall move from its current level to the value given in the Level
 * 			field. The meaning of ‘level’ is device dependent – e.g. for a light it may mean brightness level.
 * 			The movement shall be continuous, i.e. not a step function, and the time taken to move to the new level
 * 			shall be equal to the Transition time field, in seconds.
 *
 */
zbStatus_t ZCL_LevelControlMoveToLevel
(
  zclCmdLevelControl_MoveToLevel_t * pReq,
  bool_t withOnOff,
  uint8_t endpoint
)
{
  zbClusterId_t aLevelClusterId = {gaZclClusterLevelControl_c};
  zbClusterId_t aOnOffClusterId = {gaZclClusterOnOff_c};
  zclLevelValue_t zclCurrentLevelTmp;
  uint16_t TransitionTime = 0;
  uint16_t OnOffTransitionTime = 0;
  uint16_t OnTransitionTime = 0;
  uint16_t OffTransitionTime = 0;
  uint8_t minLevel = gZclLevel_LowestPossible;
  uint8_t maxLevel = gZclLevel_HighestPossible;
  uint8_t indexSetup = ZCL_LevelControl_GetIndexFromLevelSetupTable(endpoint, gTmrInvalidTimerID_c);
        
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZbFailed_c;
        
  (void)ZCL_GetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &zclCurrentLevelTmp, NULL);
  TransitionTime = OTA2Native16(pReq->transitionTime);
  
  if (TransitionTime == 0xFFFF)
  {
#if gZclClusterOptionals_d
    (void) ZCL_GetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_OnOffTransitionTimeId_c, gZclServerAttr_c, &OnOffTransitionTime, NULL);
    (void) ZCL_GetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_OnTransitionTimeId_c, gZclServerAttr_c, &OnTransitionTime, NULL);
    (void) ZCL_GetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_OffTransitionTimeId_c, gZclServerAttr_c, &OffTransitionTime, NULL);
    OnOffTransitionTime = OTA2Native16(OnOffTransitionTime);
    OnTransitionTime = OTA2Native16(OnTransitionTime);
    OffTransitionTime = OTA2Native16(OffTransitionTime);
#endif     
    if (zclCurrentLevelTmp <= pReq->level)
        TransitionTime = (OnTransitionTime==0xFFFF)?OnOffTransitionTime:OnTransitionTime; 
    else	  
        TransitionTime = (OffTransitionTime==0xFFFF)?OnOffTransitionTime:OffTransitionTime; 
  }
  
  gLevelControlServerSetup[indexSetup].moveMode = zclMoveMode_Down;
  gLevelControlServerSetup[indexSetup].levelOnOff = withOnOff;
  
  /* check max and min limits */
  if(pReq->level > maxLevel)
    pReq->level = maxLevel;
  if(pReq->level < minLevel)
    pReq->level = minLevel;

  gLevelControlServerSetup[indexSetup].levelDifference  = zclCurrentLevelTmp - pReq->level;	
  if (zclCurrentLevelTmp <= pReq->level) 
  {
    uint8_t OnOffStatus = 0x01;
 #if gASL_EnableZLLClustersData_d   
    uint16_t tempTime = 0x00;
#endif    
    gLevelControlServerSetup[indexSetup].moveMode = zclMoveMode_Up;
    gLevelControlServerSetup[indexSetup].levelDifference  = pReq->level - zclCurrentLevelTmp;
    
    (void)ZCL_SetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c,&OnOffStatus);
 #if gASL_EnableZLLClustersData_d   
    /* set global scene control to TRUE */
    (void)ZCL_SetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &OnOffStatus);
    /* get OnTime */
    (void)ZCL_GetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &tempTime, NULL);
    /* set OffWaitTime to 0x00 if required */
    if(tempTime == 0x0000)
    {
      (void)ZCL_SetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime);
    }
#endif    
  }
  gZclLevel_Step = 1;
  /* set next level */
  gLevelControlServerSetup[indexSetup].newCurrentLevel = pReq->level;
	
  gLevelControlServerSetup[indexSetup].remainingTime = (zbTmrTimeInMilliseconds_t)(TransitionTime *100);	
  #if gZclClusterOptionals_d 
  {
    uint16_t remainingTime =  Native2OTA16((uint16_t) (gLevelControlServerSetup[indexSetup].remainingTime / 100));
    (void)ZCL_SetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c, gZclServerAttr_c,&remainingTime);
  }
  #endif		
	
  gLevelControlServerSetup[indexSetup].timeBtwChanges = (uint16_t)(gLevelControlServerSetup[indexSetup].remainingTime/gLevelControlServerSetup[indexSetup].levelDifference );
  if(gLevelControlServerSetup[indexSetup].timeBtwChanges != 0x00)
    ZbTMR_StartSingleShotTimer(gLevelControlServerSetup[indexSetup].zclLevel_TransitionTmrID, gLevelControlServerSetup[indexSetup].timeBtwChanges, ZCL_LevelControlTimer);
  else
    ZCL_LevelControlTimer(gLevelControlServerSetup[indexSetup].zclLevel_TransitionTmrID);
  return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t ZCL_LevelControlMove(zclCmdLevelControl_Move_t *pReq, bool_t withOnOff) 
 *
 * @brief	On receipt of this command, a device shall move from its current level in an up 
 * 			or down direction in a continuous fashion, as detailed by the mode:
 *
 * 			Up:      Increase the device’s level at the rate given in the Rate field. If the level
 *					 reaches the maximum allowed for the device, stop. If the device is currently
 *					 powered off, do not power it on.
 *			Up with OnOff: If the device requires powering on, do so, then proceed as for the Up mode.
 *			Down:    Decrease the device’s level at the rate given in the Rate field. If the level
 *					 reaches the minimum allowed for the device, stop. If the device is currently
 *					 powered off, do not power it on.
 *			Down with OnOff: Decrease the device’s level at the rate given in the Rate field. If the level
 *					 reaches the minimum allowed for the device, stop, then, if the device can be
 *					 powered off, do so.
 *
 * 			The Rate field specifies the rate of movement in steps per second. A step is a change 
 * 			in the device’s level of one unit.
 *
 */
zbStatus_t ZCL_LevelControlMove 
(
  zclCmdLevelControl_Move_t * pReq,
  bool_t withOnOff,
  uint8_t endpoint
)
{
  zbClusterId_t    aLevelClusterId = {gaZclClusterLevelControl_c};
  zbClusterId_t    aOnOffClusterId = {gaZclClusterOnOff_c};
  zclLevelValue_t  CurrentLevel;
  uint8_t          OnOffStatus;  
  uint8_t          minLevel = gZclLevel_LowestPossible;
  uint8_t          maxLevel = gZclLevel_HighestPossible;     
  uint8_t          defaultRate = gZclLevelControl_DefaultMoveRate_d;
  uint8_t          indexSetup = ZCL_LevelControl_GetIndexFromLevelSetupTable(endpoint, gTmrInvalidTimerID_c);
        
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZbFailed_c;
  
  
  (void)ZCL_GetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &CurrentLevel, NULL);
  (void)ZCL_GetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c,&OnOffStatus, NULL);
  
  gLevelControlServerSetup[indexSetup].moveMode =pReq->moveMode;
  gLevelControlServerSetup[indexSetup].levelOnOff = withOnOff;
  gZclLevel_Step = 1;
  
  if ((gLevelControlServerSetup[indexSetup].moveMode == zclMoveMode_Up) && withOnOff && (!OnOffStatus))
  {
#if gASL_EnableZLLClustersData_d   
    uint16_t tempTime = 0x00;
#endif    
    OnOffStatus = 1;
    (void)ZCL_SetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c,&OnOffStatus);
#if gASL_EnableZLLClustersData_d   
    /* set global scene control to TRUE */
    (void)ZCL_SetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &OnOffStatus);
    /* get OnTime */
    (void)ZCL_GetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &tempTime, NULL);
    /* set OffWaitTime to 0x00 if required */
    if(tempTime == 0x0000)
    {
      (void)ZCL_SetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime);
    }
#endif   
  }
  
  switch ( gLevelControlServerSetup[indexSetup].moveMode)
  {
    case zclMoveMode_Up:
          gLevelControlServerSetup[indexSetup].levelDifference  = maxLevel - CurrentLevel;
          gLevelControlServerSetup[indexSetup].newCurrentLevel = maxLevel;
          break;
    case zclMoveMode_Down:
          gLevelControlServerSetup[indexSetup].levelDifference  = CurrentLevel - minLevel;
          gLevelControlServerSetup[indexSetup].newCurrentLevel = minLevel;
          break;
  }
  

  if(pReq->rate == 0xFF)
  {
    /* use default rate */
 #if gZclClusterOptionals_d     
    (void)ZCL_GetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_DefaultMoveRateId_c, gZclServerAttr_c,&defaultRate, NULL);
#endif
    pReq->rate = (defaultRate == 0xFF)?250:defaultRate; 
  }
  
  gLevelControlServerSetup[indexSetup].timeBtwChanges = (uint16_t)(1000/pReq->rate);
  gLevelControlServerSetup[indexSetup].remainingTime = (zbTmrTimeInMilliseconds_t)(gLevelControlServerSetup[indexSetup].timeBtwChanges*gLevelControlServerSetup[indexSetup].levelDifference );
  
  #if gZclClusterOptionals_d 
  {
    uint16_t remainingTime =  Native2OTA16((uint16_t) (gLevelControlServerSetup[indexSetup].remainingTime / 100));
    (void)ZCL_SetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c, gZclServerAttr_c,&remainingTime);
  }
  #endif	
  
  ZbTMR_StartSingleShotTimer(gLevelControlServerSetup[indexSetup].zclLevel_TransitionTmrID, gLevelControlServerSetup[indexSetup].timeBtwChanges, ZCL_LevelControlTimer);
  
  return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t ZCL_LevelControlStep(zclCmdLevelControl_Step_t *pReq, bool_t withOnOff) 
 *
 * @brief	On receipt of this command, a device shall move from its current level in an up 
 * 			or down direction in a continuous fashion, as detailed by the mode:
 *
 * 			Up:      Increase the device’s level by the number of units indicated in the Amount
 *    				 field. If the level is already at the maximum allowed for the device, then do
 *    				 nothing. If the value specified in the Amount field would cause the
 *    				 maximum value to be exceeded, then move to the maximum value using the
 *    				 full transition time. If the device is currently powered off, do not power it on.
 *			Up with OnOff: If the device requires powering on, do so, then proceed as for the Up mode.
 *			Down:    Decrease the device’s level by the number of units indicated in the Amount
 *     				 field. If the level is already at the minimum allowed for the device, then do
 *     				 nothing. If the value specified in the Amount field would cause the
 *     				 minimum value to be exceeded, then move to the minimum value using the
 *     				 full transition time. If the device is currently powered off, do not power it on.
 *			Down with OnOff: Carry out the Down action. If the new level is at or below the minimum
 *                 	 allowed for the device, and the device can be powered off, then do so.
 *
 * 			The Transition time field specifies the time, in 1/10ths of a second, the time that shall be taken to
 * 			perform the step. A step is a change in the device’s level by the number of units specified in the
 * 			Amount field.
 *
 */
zclStatus_t ZCL_LevelControlStep
(
  zclCmdLevelControl_Step_t *pReq,
  bool_t withOnOff,
  uint8_t endpoint
    
)
{
  zbClusterId_t aLevelClusterId = {gaZclClusterLevelControl_c};
  zbClusterId_t aOnOffClusterId = {gaZclClusterOnOff_c};
  uint16_t TransitionTime = 0;
  zclLevelValue_t CurrentLevel;
  uint8_t OnOffStatus;
  uint8_t minLevel = gZclLevel_LowestPossible;
  uint8_t maxLevel = gZclLevel_HighestPossible;  
  uint8_t indexSetup = ZCL_LevelControl_GetIndexFromLevelSetupTable(endpoint, gTmrInvalidTimerID_c);
        
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZbFailed_c;
        
  (void)ZCL_GetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, gZclServerAttr_c, &CurrentLevel, NULL);
  (void)ZCL_GetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &OnOffStatus, NULL);

  TransitionTime = OTA2Native16(pReq->transitionTime);
  gLevelControlServerSetup[indexSetup].levelDifference  = pReq->stepSize;
  gZclLevel_Step = 1;
  gLevelControlServerSetup[indexSetup].moveMode = pReq->stepMode;
  gLevelControlServerSetup[indexSetup].levelOnOff = withOnOff;	
  
  if (( gLevelControlServerSetup[indexSetup].moveMode == zclMoveMode_Up) && withOnOff && (!OnOffStatus))
  {
#if gASL_EnableZLLClustersData_d   
    uint16_t tempTime = 0x00;
#endif  
    
    OnOffStatus = 1;
    (void)ZCL_SetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c,&OnOffStatus);
    
#if gASL_EnableZLLClustersData_d   
    /* set global scene control to TRUE */
    (void)ZCL_SetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_GlobalSceneCtrl_c, gZclServerAttr_c, &OnOffStatus);
    /* get OnTime */
    (void)ZCL_GetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OnTime_c, gZclServerAttr_c, &tempTime, NULL);
    /* set OffWaitTime to 0x00 if required */
    if(tempTime == 0x0000)
    {
      (void)ZCL_SetAttribute(endpoint, aOnOffClusterId, gZclAttrOnOff_OffWaitTime_c, gZclServerAttr_c, &tempTime);
    }
#endif                 
  }
  if (OnOffStatus)
  {
    switch (gLevelControlServerSetup[indexSetup].moveMode)
    {
        case zclMoveMode_Up:
          gLevelControlServerSetup[indexSetup].newCurrentLevel = CurrentLevel + pReq->stepSize;
          if ((maxLevel - CurrentLevel) < pReq->stepSize)
          {
              gLevelControlServerSetup[indexSetup].newCurrentLevel = maxLevel;
              gLevelControlServerSetup[indexSetup].levelDifference  = maxLevel - CurrentLevel;
          }
          break;
        case zclMoveMode_Down:
	  gLevelControlServerSetup[indexSetup].newCurrentLevel = CurrentLevel - pReq->stepSize;
          if (CurrentLevel < pReq->stepSize + minLevel)
          {
              gLevelControlServerSetup[indexSetup].newCurrentLevel = minLevel;
              gLevelControlServerSetup[indexSetup].levelDifference  = CurrentLevel - minLevel;
          }
          else
          {
              gLevelControlServerSetup[indexSetup].newCurrentLevel = CurrentLevel - pReq->stepSize;
          }
          break;
     }
    
    gZclLevel_Step = 1;
    if(TransitionTime == 0xFFFF)
    {
      /* If the Transition time field is 0xffff the device should move as fast as it is able */
      TransitionTime = 0x01; /* 100 ms */
    }
    gLevelControlServerSetup[indexSetup].remainingTime = (zbTmrTimeInMilliseconds_t)(TransitionTime*100);
#if gZclClusterOptionals_d 
    {
      uint16_t remainingTime =  Native2OTA16((uint16_t) (gLevelControlServerSetup[indexSetup].remainingTime / 100));          
      (void)ZCL_SetAttribute(endpoint, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c, gZclServerAttr_c,&remainingTime);
    }
#endif		
    gLevelControlServerSetup[indexSetup].timeBtwChanges = (uint16_t)(gLevelControlServerSetup[indexSetup].remainingTime/gLevelControlServerSetup[indexSetup].levelDifference );
    ZbTMR_StartSingleShotTimer(gLevelControlServerSetup[indexSetup].zclLevel_TransitionTmrID, gLevelControlServerSetup[indexSetup].timeBtwChanges, ZCL_LevelControlTimer);
  }
  return gZbSuccess_c;
}

/*!
 * @fn 		zclStatus_t ZCL_LevelControlStop(void) 
 *
 * @brief	On receipt of this command, a device shall stop any level control actions
 * 		    (move, move to level, step) that are in progress 
 */
zclStatus_t ZCL_LevelControlStop(uint8_t endpoint)
{
   uint8_t  indexSetup = ZCL_LevelControl_GetIndexFromLevelSetupTable(endpoint, gTmrInvalidTimerID_c);
        
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZbFailed_c;
    
  ZbTMR_StopTimer(gLevelControlServerSetup[indexSetup].zclLevel_TransitionTmrID);
  return gZbSuccess_c;  
}

#if gAddValidationFuncPtrToClusterDef_c
/*!
 * @fn 		bool_t  ZCL_LevelControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Validation function for Level control attributes
 *
 */
bool_t  ZCL_LevelControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
{  
  zclCmdWriteAttrRecord_t *pRecord = (zclCmdWriteAttrRecord_t*) pData;  
  bool_t status = TRUE;
#if gZclClusterOptionals_d     
  zclAttrData_t *pAttrData = (zclAttrData_t *) pRecord->aData;
  uint8_t minLevel = gZclLevel_LowestPossible;
  uint8_t maxLevel = gZclLevel_HighestPossible;  
  uint8_t onLevel;
  (void) ZCL_GetAttribute(endPoint, clusterId, gZclAttrLevelControl_OnLevelId_c,  gZclServerAttr_c, &onLevel,  NULL);
#endif  
  
  switch (pRecord->attrId)
  {        
#if gZclClusterOptionals_d    
    case gZclAttrLevelControl_OnLevelId_c:
      {
        if(pAttrData->data8 == gZclLevel_UndefinedOnLevel_c)   
          status = TRUE;
        else
        {
          if((pAttrData->data8 < minLevel) || (pAttrData->data8 > maxLevel))
             status = FALSE;
        }
        break;
      }
#endif      
    default:
      {
        status = TRUE;
        break;
      }
  }
  
 /* to avoid ompiler warnings */
 (void) endPoint;
 (void) clusterId;

  return status;
} 
#endif /* gAddValidationFuncPtrToClusterDef_c */




/******************************
  Alarms Cluster
  See ZCL Specification Section 3.11
*******************************/
extern gZclAlarmTable_t gAlarmsTable[MaxAlarmsPermitted];

/*!
 * @fn 		zbStatus_t ZCL_AlarmsClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alarm Cluster Client. 
 *
 */
zbStatus_t ZCL_AlarmsClusterClient
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
  
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;   
    
    /* handle the command */
    command = pFrame->command;  
    
    
    switch(command)
    {
        case gAlarmClusterTxCommandID_Alarm_c:
        case gAlarmClusterTxCommandID_GetAlarmResponse_c:
        case gAlarmClusterTxCommandID_ClearAlarm_c:
          /* These should be passed up to a host*/
          return status;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
} 

/*!
 * @fn 		zbStatus_t ZCL_AlarmsClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alarm Cluster Server. 
 *
 */
zbStatus_t ZCL_AlarmsClusterServer
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command = 0;
    zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm;
    zclAlarms_GetAlarmResponse_t *pGetAlarmResponse;
    zbClusterId_t aClusterId;
    uint8_t  i=0, min=0;
    uint16_t count = 0;
    uint32_t minTimeStamp = 0;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;
    
    Copy2Bytes(aClusterId, pIndication->aClusterId);
    
    /* handle the command */
    (void)ZCL_GetAttribute( pIndication->dstEndPoint, aClusterId, gZclAttrAlarms_AlarmCount_c, gZclServerAttr_c, &count, NULL);
    count = OTA2Native16(count);
    command = pFrame->command;  
    switch(command)
    {
        case gAlarmClusterRxCommandID_ResetAlarm_c:
          FLib_MemCpy(&CmdResetAlarm ,(pFrame + 1), sizeof(zclCmdAlarmInformation_ResetAlarm_t));  
          ResetAlarm(CmdResetAlarm, pIndication->dstEndPoint);
          return status;
        case gAlarmClusterRxCommandID_ResetAllAlarms_c:
          ResetAllAlarm(pIndication->dstEndPoint);
          return status;
        case gAlarmClusterRxCommandID_GetAlarm_c:
          pGetAlarmResponse = MSG_Alloc(sizeof(zclCmdAlarmInformation_ResetAlarm_t));
          
          if(!pGetAlarmResponse)
          {
            return gZclNoMem_c;
          }
          
          pGetAlarmResponse->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
          Copy2Bytes(pGetAlarmResponse->addrInfo.dstAddr.aNwkAddr, pIndication->aSrcAddr); 
          pGetAlarmResponse->addrInfo.dstEndPoint = pIndication->dstEndPoint;
          pGetAlarmResponse->addrInfo.srcEndPoint = pIndication->srcEndPoint;
          pGetAlarmResponse->addrInfo.txOptions = 0;
          pGetAlarmResponse->addrInfo.radiusCounter = afDefaultRadius_c;
          if(count > 1)
          {
            //minTimeStamp = gAlarmsTable[0].TimeStamp;
            FLib_MemCpy(&minTimeStamp, &gAlarmsTable[0].TimeStamp, 4);  
            for(i=0; i<(count-1); i++)
              if(minTimeStamp > gAlarmsTable[i+1].TimeStamp)
              {
            	min = i+1;  
                //minTimeStamp = gAlarmsTable[min].TimeStamp;
            	FLib_MemCpy(&minTimeStamp, &gAlarmsTable[i+1].TimeStamp, 4);       
              }
          }
          else
            min = 0;        
          pGetAlarmResponse->cmdFrame.AlarmCode = gAlarmsTable[min].AlarmCode;
          Copy2Bytes(&pGetAlarmResponse->cmdFrame.ClusterID, &gAlarmsTable[min].ClusterID);
          FLib_MemCpy(&pGetAlarmResponse->cmdFrame.TimeStamp, &gAlarmsTable[min].TimeStamp, 4);   
          pGetAlarmResponse->cmdFrame.Status = (count == 0x00)?STATUS_NOT_FOUND_c:STATUS_SUCCESS_c;
		  /* update alarm table */
          if(count > 0)
            for(i=min;i<count-1;i++)
        	//gAlarmsTable[i] = gAlarmsTable[i+1];
        	FLib_MemCpy(&gAlarmsTable[i], &gAlarmsTable[i+1], sizeof(gZclAlarmTable_t)); 
          /*update alarm count attribut*/
          count--;
          count = Native2OTA16(count);
          (void)ZCL_SetAttribute( pIndication->dstEndPoint, aClusterId, gZclAttrAlarms_AlarmCount_c, gZclServerAttr_c, &count);			
          return Alarms_GetAlarmResponse(pGetAlarmResponse);   
        case gAlarmClusterRxCommandID_ResetAlarmLog_c:
          count = 0;
          (void)ZCL_SetAttribute( pIndication->dstEndPoint, aClusterId, gZclAttrAlarms_AlarmCount_c, gZclServerAttr_c, &count);
          return status;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t Alarms_ResetAlarm(zclAlarmInformation_ResetAlarm_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetAlarm command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_ResetAlarm
(
zclAlarmInformation_ResetAlarm_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_ResetAlarm_c, sizeof(zclCmdAlarmInformation_ResetAlarm_t), (zclGenericReq_t *)pReq);

}

/*!
 * @fn 		void ResetAlarm(zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm, uint8_t endpoint)
 *
 * @brief	Process reset alarm command received on the Alarm Cluster Server. 
 *
 */
void ResetAlarm(zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm, uint8_t endpoint)
{
#if (gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d)  ||  gZclEnableThermostat_c 
#if gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d
   zbClusterId_t aClusterIdZone = {gaZclClusterSmplMet_c};
   uint16_t attrId = {gZclAttrMetASGenericAlarmMask_c};
   uint16_t alarmMask;
#else   
  zbClusterId_t aClusterIdZone = {gaZclClusterThermostat_c};
  uint16_t attrId = {gZclAttrThermostat_AlarmMaskId_c};
  uint8_t alarmMask;
#endif  
  if(FLib_Cmp2Bytes(&CmdResetAlarm.ClusterID, &aClusterIdZone[0]) == TRUE){
    
    (void)ZCL_GetAttribute(endpoint, aClusterIdZone, attrId, gZclServerAttr_c, &alarmMask, NULL);
     alarmMask = (~(1<<CmdResetAlarm.AlarmCode)) & alarmMask;
    (void)ZCL_SetAttribute(endpoint, aClusterIdZone, attrId, gZclServerAttr_c, &alarmMask);      
  }
#endif  
}

/*!
 * @fn 		zbStatus_t Alarms_ResetAllAlarms(zclAlarmInformation_NoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetAllAlarm command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_ResetAllAlarms
(
zclAlarmInformation_NoPayload_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_ResetAllAlarms_c, 0, (zclGenericReq_t *)pReq);

}

/*!
 * @fn 		zbStatus_t Alarms_ResetAlarmLog(zclAlarmInformation_NoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetAlarmLog command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_ResetAlarmLog
(
zclAlarmInformation_NoPayload_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_ResetAlarmLog_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t Alarms_GetAlarm(zclAlarmInformation_NoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a GetAlarm command from the Alarm Cluster Client. 
 *
 */
zbStatus_t Alarms_GetAlarm
(
zclAlarmInformation_NoPayload_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_GetAlarm_c, 0, (zclGenericReq_t *)pReq);

}
/*!
 * @fn 		void ResetAllAlarm(uint8_t endpoint)
 *
 * @brief	Process reset all alarm command received on the Alarm Cluster Server. 
 *
 */
void ResetAllAlarm(uint8_t endpoint)
{
#if (gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d)  ||  gZclEnableThermostat_c 
#if gASL_ZclMet_Optionals_d  && gZclEnableMeteringServer_d
   zbClusterId_t aClusterIdZone[2] = {gaZclClusterALarms_c, gaZclClusterSmplMet_c};
   uint16_t attrId = {gZclAttrMetASGenericAlarmMask_c};
   uint16_t valueAttr = 0;
#else     
   zbClusterId_t aClusterIdZone[2] = {gaZclClusterALarms_c, gaZclClusterThermostat_c};
   uint16_t attrId = {gZclAttrThermostat_AlarmMaskId_c};
   uint8_t  valueAttr = 0;
#endif    
   uint16_t count = 0, i=0;
   bool_t testedCluster = FALSE;

   (void)ZCL_GetAttribute(endpoint, aClusterIdZone[0], gZclAttrAlarms_AlarmCount_c, gZclServerAttr_c, &count, NULL);
   count = Native2OTA16(count);
   for(i=0; i< count; i++)
   {
    if(FLib_Cmp2Bytes(&gAlarmsTable[i].ClusterID,&aClusterIdZone[1]) == TRUE)
      if(testedCluster == FALSE)
      {
         valueAttr = 0;
        (void)ZCL_SetAttribute(endpoint, aClusterIdZone[1], attrId, gZclServerAttr_c, &valueAttr);  
         testedCluster = TRUE;
      }
   }  
#endif  
}

/*!
 * @fn 		zbStatus_t Alarms_Alarm(zclAlarmInformation_Alarm_t *pReq) 
 *
 * @brief	Sends over-the-air a Alarm command from the Alarm Cluster Server. 
 *
 */
zbStatus_t Alarms_Alarm
(
zclAlarmInformation_Alarm_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendServerReqSeqPassed(gAlarmClusterTxCommandID_Alarm_c, sizeof(zclCmdAlarmInformation_Alarm_t), (zclGenericReq_t *)pReq);

}

/*!
 * @fn 		zbStatus_t Alarms_GetAlarmResponse(zclAlarms_GetAlarmResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GetAlarmResponse command from the Alarm Cluster Server. 
 *
 */
zbStatus_t Alarms_GetAlarmResponse
(
  zclAlarms_GetAlarmResponse_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendServerRspSeqPassed(gAlarmClusterTxCommandID_GetAlarmResponse_c,sizeof(zclCmdAlarmInformation_GetAlarmResponse_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t Alarms_ClearAlarm(zclAlarms_GetAlarmResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GetAlarmResponse command from the Alarm Cluster Server. 
 *
 */
zbStatus_t Alarms_ClearAlarm
(
  zclAlarms_ClearAlarm_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendServerRspSeqPassed(gAlarmClusterTxCommandID_GetAlarmResponse_c,sizeof(zclCmdAlarms_ClearAlarm_t),(zclGenericReq_t *)pReq);
}
/*!
 * @fn 		uint8_t TestAlarmStatus(uint8_t index, uint8_t endPoint)
 *
 * @brief	Check the Alarm Mask bit.
 *
 */
uint8_t TestAlarmStatus(uint8_t index, uint8_t endPoint)
{
#if gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d 
   zbClusterId_t aClusterIdZone = {gaZclClusterSmplMet_c};
   uint16_t alarmMask = 0x00;
   uint16_t attrId = {gZclAttrMetASGenericAlarmMask_c};
#else   
   zbClusterId_t aClusterIdZone = {gaZclClusterThermostat_c};
   uint8_t alarmMask = 0x00;
   uint16_t attrId = {gZclAttrThermostat_AlarmMaskId_c};
#endif
   if(IsEqual2BytesInt(aClusterIdZone, gAlarmsTable[index].ClusterID) == TRUE)
   {    
      (void)ZCL_GetAttribute(endPoint, aClusterIdZone, attrId, gZclServerAttr_c, &alarmMask, NULL);        
      if((alarmMask & (1<<gAlarmsTable[index].AlarmCode)) >> gAlarmsTable[index].AlarmCode == 1)
       return STATUS_SUCCESS_c;
   }
   return STATUS_NOT_FOUND_c;
}

/******************************
  Time Cluster
  See ZCL Specification (075123r02) Section 3.12
*******************************/


/* note 
- by setting zclTimeStatusMaster and zclTimeStatusMasterZoneDst all Time attributes becomes read only
ajustmets to the time and time zones must be done from the backend.
*/

const zclAttrDef_t gaZclTimeServerAttrDef[] = {
  { gZclAttrIdTime_c,           gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAM_c, sizeof(uint32_t),  (void *)MbrOfs(ZCLTimeServerAttrsRAM_t, Time) },
  { gZclAttrIdTimeStatus_c,     gZclDataTypeBitmap8_c,  gZclAttrFlagsInRAM_c, sizeof(uint8_t),   (void *)MbrOfs(ZCLTimeServerAttrsRAM_t, TimeStatus)},
  { gZclAttrIdTimeZone_c,       gZclDataTypeInt32_c,    gZclAttrFlagsInRAM_c, sizeof(int32_t),   (void *)MbrOfs(ZCLTimeServerAttrsRAM_t, TimeZone) },
  { gZclAttrIdDstStart_c,       gZclDataTypeUint32_c,   gZclAttrFlagsInRAM_c, sizeof(uint32_t),  (void *)MbrOfs(ZCLTimeServerAttrsRAM_t, DstStart) },
  { gZclAttrIdDstEnd_c,         gZclDataTypeUint32_c,   gZclAttrFlagsInRAM_c, sizeof(uint32_t),  (void *)MbrOfs(ZCLTimeServerAttrsRAM_t, DstEnd) },
  { gZclAttrIdDstShift_c,       gZclDataTypeInt32_c,    gZclAttrFlagsInRAM_c, sizeof(int32_t),   (void *)MbrOfs(ZCLTimeServerAttrsRAM_t, DstShift) },
  { gZclAttrIdStandardTime_c,   gZclDataTypeUint32_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint32_t),  (void *)MbrOfs(ZCLTimeServerAttrsRAM_t, StandardTime) },
  { gZclAttrIdLocalTime_c,      gZclDataTypeUint32_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint32_t),  (void *)MbrOfs(ZCLTimeServerAttrsRAM_t, LocalTime) },
  { gZclAttrIdLastSetTime_c,    gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint32_t),  (void *)MbrOfs(ZCLTimeServerAttrsRAM_t, LastSetTime)},
  { gZclAttrIdValidUntilTime_c, gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAM_c, sizeof(uint32_t),  (void *)MbrOfs(ZCLTimeServerAttrsRAM_t, ValidUntilTime)}
};

const zclAttrSet_t gaZclTimeServerClusterAttrSet[] = {
  {gZclAttrSetTime_c, (void *)&gaZclTimeServerAttrDef, NumberOfElements(gaZclTimeServerAttrDef)}
};

const zclAttrSetList_t gZclTimeServerClusterAttrSetList = {
  NumberOfElements(gaZclTimeServerClusterAttrSet),
  gaZclTimeServerClusterAttrSet
};

const uint8_t mYearTable[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

zbTmrTimerID_t zclTimeSecondTimer = gTmrInvalidTimerID_c;

/*!
 * @fn 		void App_InitTimeCluster(void)
 *
 * @brief	Init Time Cluster using default application values.
 *
 */
void App_InitTimeCluster(uint8_t endpoint)
{
  ZCLTimeConf_t defaultTimeConf;

  defaultTimeConf.Time = mDefaultValueOfTimeClusterAppTime_c;
  defaultTimeConf.Time = Native2OTA32( defaultTimeConf.Time);
  defaultTimeConf.TimeStatus = zclTimeStatusMaster;
  defaultTimeConf.TimeZone = mDefaultValueOfTimeClusterAppTimeZone_c;
  defaultTimeConf.TimeZone = Native2OTA32(defaultTimeConf.TimeZone);
  defaultTimeConf.DstStart = mDefaultValueOfTimeClusterAppDstStart_c;
  defaultTimeConf.DstStart = Native2OTA32(defaultTimeConf.DstStart);
  defaultTimeConf.DstEnd = mDefaultValueOfTimeClusterAppDstEnd_c;
  defaultTimeConf.DstEnd = Native2OTA32(defaultTimeConf.DstEnd);
  defaultTimeConf.DstShift = mDefaultValueOfClusterAppDstShift_c;
  defaultTimeConf.DstShift = Native2OTA32(defaultTimeConf.DstShift);
  defaultTimeConf.ValidUntilTime = mDefaultValueOfTimeClusterAppValidUntilTime_c;
  defaultTimeConf.ValidUntilTime = Native2OTA32(defaultTimeConf.ValidUntilTime);
  ZCL_TimeInit(endpoint, &defaultTimeConf);
}

/*!
 * @fn 		zclTime_GetTimeInf_t App_GetTimeInf(ZCLTime_t timestamp)
 *
 * @brief	Get time information(day, month, hour, minute, year) based on timestamp value
 *
 */
zclTime_GetTimeInf_t App_GetTimeInf(ZCLTime_t timestamp)
{
  zclTime_GetTimeInf_t timeInf;
  uint32_t dayclock, dayno;
  uint16_t year = gTime_StartZigbeeYearTime_c;
 
  
  timestamp = OTA2Native32(timestamp);
 
  dayclock =  timestamp % gTime_SecondsDay_c;
  dayno =  timestamp / gTime_SecondsDay_c;
  
  timeInf.timeMinute  = (uint8_t)((dayclock % gTime_SecondsHour_c) / gTime_SecondsMinute_c);
  timeInf.timeHour	= (uint8_t)(dayclock / gTime_SecondsHour_c);
  timeInf.timeWeekDay = (dayno + gTime_StartZigbeeWeekDay_c) % gTime_NoOfDays_c; 
  
  while (dayno >= Time_YearSize(year)) 
  {
	dayno -= Time_YearSize(year);
	year++;
  }
	  
  timeInf.timeYear = year;  

  timeInf.timeMonth = 1;	/* start from January */
  while (dayno >= mYearTable[Time_LeapYear(year)][timeInf.timeMonth]) 
  {
	dayno -= mYearTable[Time_LeapYear(year)][timeInf.timeMonth];
	timeInf.timeMonth++;
  }
  
  timeInf.timeday = (uint8_t)(dayno); 
  
  return timeInf;	
}


/*!
 * @fn 		void ZCL_TimeUpdate(void)
 *
 * @brief	Update the zcl Time attributes. Shall be called every second
 *
 */
void ZCL_TimeUpdate(void)
{

  uint32_t nativeTime;
  uint8_t i=0;
  zbClusterId_t clusterTimeId = {gaZclClusterTime_c};
  
  #if gInstantiableStackEnabled_d 
  for(i=0;i<EndPointConfigData(gNum_EndPoints); i++)
  #else
  for(i=0;i<gNum_EndPoints_c; i++)
  #endif  
  {
    uint8_t currentEndpoint = EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint);
    if(ZCL_GetAttribute(currentEndpoint, clusterTimeId, gZclAttrTime_c, gZclServerAttr_c, &nativeTime,NULL) == gZbSuccess_c)
    {
       uint32_t nativeDstStart, nativeDstEnd, nativeDstShift; 
       uint32_t nativeLocalTime, nativeStandardTime; 
       int32_t nativeTimeZone; 
       
       /* Get current values*/
       (void)ZCL_GetAttribute(currentEndpoint, clusterTimeId, gZclAttrDstStart_c,       gZclServerAttr_c, &nativeDstStart,      NULL);
       (void)ZCL_GetAttribute(currentEndpoint, clusterTimeId, gZclAttrDstEnd_c,         gZclServerAttr_c, &nativeDstEnd,        NULL);
       (void)ZCL_GetAttribute(currentEndpoint, clusterTimeId, gZclAttrDstShift_c,       gZclServerAttr_c, &nativeDstShift,      NULL);
       (void)ZCL_GetAttribute(currentEndpoint, clusterTimeId, gZclAttrLocalTime_c,      gZclServerAttr_c, &nativeLocalTime,     NULL);
       (void)ZCL_GetAttribute(currentEndpoint, clusterTimeId, gZclAttrStandardTime_c,   gZclServerAttr_c, &nativeStandardTime,  NULL);
       (void)ZCL_GetAttribute(currentEndpoint, clusterTimeId, gZclAttrTimeZone_c,       gZclServerAttr_c, &nativeTimeZone,      NULL);
       
         /* swap endianess OTA 2 native*/
        nativeTime = OTA2Native32(nativeTime); 
        nativeTimeZone = OTA2Native32(nativeTimeZone); 
        nativeDstStart = OTA2Native32(nativeDstStart); 
        nativeDstEnd = OTA2Native32(nativeDstEnd); 
        nativeDstShift = OTA2Native32(nativeDstShift); 
        nativeLocalTime = OTA2Native32(nativeLocalTime); 
        nativeStandardTime = OTA2Native32(nativeStandardTime);
        
        /* do calculations*/
        nativeTime++;
  
        /*Standard Time = Time + TimeZone*/
        nativeStandardTime = nativeTime + nativeTimeZone; 
  
        /*
          Local Time = Standard Time + DstShift (if DstStart <= Time <= DstEnd)
          Local Time = Standard Time (if Time < DstStart or Time > DstEnd)
        */
        if ((nativeDstStart <= nativeTime) && (nativeTime <= nativeDstEnd))
          nativeLocalTime = nativeStandardTime + nativeDstShift; 
        else
          nativeLocalTime = nativeStandardTime; 

  
        /*save and swap back to OTA endianness */
        nativeTime = Native2OTA32(nativeTime);
        nativeLocalTime = Native2OTA32(nativeLocalTime); 
        nativeStandardTime = Native2OTA32(nativeStandardTime); 
        
        (void)ZCL_SetAttribute(currentEndpoint, clusterTimeId, gZclAttrTime_c,        gZclServerAttr_c, &nativeTime);
        (void)ZCL_SetAttribute(currentEndpoint, clusterTimeId, gZclAttrLocalTime_c,   gZclServerAttr_c, &nativeLocalTime);
        (void)ZCL_SetAttribute(currentEndpoint, clusterTimeId, gZclAttrStandardTime_c,gZclServerAttr_c, &nativeStandardTime); 
    }
  }
}
/*!
 * @fn 		void ZCL_TimeSecondTimerCallback(zbTmrTimerID_t timerid)
 *
 * @brief	Callback for the time cluster.
 *
 */
void ZCL_TimeSecondTimerCallback(zbTmrTimerID_t timerid)
{
(void) timerid;
  /* restart timer */
  ZbTMR_StartSecondTimer (zclTimeSecondTimer, 1, ZCL_TimeSecondTimerCallback);

  /* update the Zcl Time cluster*/ 
  ZCL_TimeUpdate();
}

/*!
 * @fn 		void ZCL_TimeInit(ZCLTimeConf_t *pTimeConf)
 *
 * @brief	Init Time Cluster using pTimeConf parameter. Note input must be little endian.
 *
 */
void ZCL_TimeInit(uint8_t endpoint, ZCLTimeConf_t *pTimeConf)
{
  zbClusterId_t clusterTimeId = {gaZclClusterTime_c};
  
  /*Initialize the Time cluster atributes */
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrTime_c,        gZclServerAttr_c, &pTimeConf->Time);
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrTimeStatus_c,  gZclServerAttr_c, &pTimeConf->TimeStatus);
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrTimeZone_c,    gZclServerAttr_c, &pTimeConf->TimeZone); 
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrDstStart_c,    gZclServerAttr_c, &pTimeConf->DstStart);
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrDstEnd_c,      gZclServerAttr_c, &pTimeConf->DstEnd); 
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrDstShift_c,    gZclServerAttr_c, &pTimeConf->DstShift); 
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrLastSetTime_c, gZclServerAttr_c, &pTimeConf->Time);
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrValidUntilTime_c,gZclServerAttr_c, &pTimeConf->ValidUntilTime); 

  /* start the timer */
  if(zclTimeSecondTimer == gTmrInvalidTimerID_c)
	  zclTimeSecondTimer =  ZbTMR_AllocateTimer();
  if(zclTimeSecondTimer != gTmrInvalidTimerID_c)
	  ZbTMR_StartSecondTimer (zclTimeSecondTimer, 1, ZCL_TimeSecondTimerCallback);
}

/*!
 * @fn 		void ZCL_SetUTCTime(uint32_t time)
 *
 * @brief	Set UTC time
 *
 */
void ZCL_SetUTCTime(uint8_t endpoint, uint32_t time)
{
   zbClusterId_t clusterTimeId = {gaZclClusterTime_c};
   
   time = Native2OTA32(time);
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrTime_c,        gZclServerAttr_c, &time);
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrLastSetTime_c, gZclServerAttr_c, &time);
  
}

/*!
 * @fn 		uint32_t ZCL_GetUTCTime(void)
 *
 * @brief	Get current UTC time
 *
 */
uint32_t ZCL_GetUTCTime(uint8_t endpoint)
{
  uint32_t time;
  zbClusterId_t clusterTimeId = {gaZclClusterTime_c};
  
  (void)ZCL_GetAttribute(endpoint, clusterTimeId, gZclAttrTime_c, gZclServerAttr_c, &time,NULL);
 
  return OTA2Native32(time);
}

/*!
 * @fn 		void ZCL_SetTimeZone(int32_t timeZone)
 *
 * @brief	Set time zone
 *
 */
void ZCL_SetTimeZone(uint8_t endpoint, int32_t timeZone)
{
  zbClusterId_t clusterTimeId = {gaZclClusterTime_c};
  timeZone = Native2OTA32(timeZone);
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrTimeZone_c,    gZclServerAttr_c, &timeZone); 
}

/*!
 * @fn 		void ZCL_SetTimeDst(uint32_t DstStart, uint32_t DstEnd, int32_t DstShift)
 *
 * @brief	Set DST Start, DST End and DST Shift time
 *
 */
void ZCL_SetTimeDst(uint8_t endpoint, uint32_t DstStart, uint32_t DstEnd, int32_t DstShift)
{
  zbClusterId_t clusterTimeId = {gaZclClusterTime_c};
  DstStart = Native2OTA32(DstStart);
  DstEnd = Native2OTA32(DstEnd);
  DstShift = Native2OTA32(DstShift);
  
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrDstStart_c,    gZclServerAttr_c, &DstStart);
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrDstEnd_c,      gZclServerAttr_c, &DstEnd); 
  (void)ZCL_SetAttribute(endpoint, clusterTimeId, gZclAttrDstShift_c,    gZclServerAttr_c, &DstShift); 
}

/******************************
  RSSI Cluster
  See ZCL Specification Section 3.13
*******************************/
/*!
 * @fn 		zbStatus_t ZCL_RSSILocationClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the RSSI Location Cluster Client. 
 *
 */
zbStatus_t ZCL_RSSILocationClusterClient
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdTxRSSI_DeviceConfigurationResponse_c:
        case gZclCmdTxRSSI_LocationDataResponse_c:
        case gZclCmdTxRSSI_LocationDataNotification_c:
        case gZclCmdTxRSSI_CompactLocationDataNotification_c:
        case gZclCmdTxRSSI_RSSIping_c:
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ZCL_RSSILocationClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the RSSI Location Cluster Server. 
 *
 */
zbStatus_t ZCL_RSSILocationClusterServer
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
  
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;      

    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdRxRSSI_SetAbsoluteLocation_c:
        case gZclCmdRxRSSI_SetDeviceConfiguration_c:
        case gZclCmdRxRSSI_GetDeviceConfiguration_c:
        case gZclCmdRxRSSI_GetLocationData_c:

          /* These should be passed up to a host*/
          return status;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t RSSILocation_SetAbsoluteLocation(zclCmdRSSI_AbsoluteLocation_t *pReq) 
 *
 * @brief	Sends over-the-air an SetAbsoluteLocation command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_SetAbsoluteLocation
(
zclCmdRSSI_AbsoluteLocation_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_SetAbsoluteLocation_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_SetDeviceConfiguration(zclCmdRSSI_SetDeviceConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air an SetDeviceConfiguration command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_SetDeviceConfiguration
(
zclCmdRSSI_SetDeviceConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_SetDeviceConfiguration_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_GetDeviceConfiguration(zclCmdRSSI_GetDeviceConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air an GetDeviceConfiguration command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_GetDeviceConfiguration
(
zclCmdRSSI_GetDeviceConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_GetDeviceConfiguration_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_GetLocationData(zclCmdRSSI_GetLocationData_t *pReq) 
 *
 * @brief	Sends over-the-air an GetLocationData command from the RSSI Location Cluster client. 
 *
 */
zbStatus_t RSSILocation_GetLocationData
(
zclCmdRSSI_GetLocationData_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_GetLocationData_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_DeviceConfigurationResponse(zclCmdRSSI_DeviceConfigurationResponse_t *pReq) 
 *
 * @brief	Sends over-the-air an DeviceConfigurationResponse command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_DeviceConfigurationResponse
(
zclCmdRSSI_DeviceConfigurationResponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_DeviceConfigurationResponse_c,0,(zclGenericReq_t *)pReq);
}


/*!
 * @fn 		zbStatus_t RSSILocation_LocationDataResponse(zclCmdRSSI_LocationDataResponse_t *pReq) 
 *
 * @brief	Sends over-the-air an LocationDataResponse command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_LocationDataResponse
(
zclCmdRSSI_LocationDataResponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_LocationDataResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_LocationDataNotification(zclCmdRSSI_LocationDataNotification_t *pReq) 
 *
 * @brief	Sends over-the-air an LocationDataNotification command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_LocationDataNotification
(
zclCmdRSSI_LocationDataNotification_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_LocationDataNotification_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_CompactLocationDataNotification(zclCmdRSSI_CompactLocationDataNotification_t *pReq) 
 *
 * @brief	Sends over-the-air an CompactLocationDataNotification command from the RSSI Location Cluster Server. 
 *
 */
zbStatus_t RSSILocation_CompactLocationDataNotification
(
zclCmdRSSI_CompactLocationDataNotification_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_CompactLocationDataNotification_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t RSSILocation_RSSIping(zclCmdRSSI_RSSIping_t *pReq) 
 *
 * @brief	Sends over-the-air an RSSIping command from the RSSI Location Cluster Client. 
 *
 */
zbStatus_t RSSILocation_RSSIping
(
zclCmdRSSI_RSSIping_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_RSSIping_c,0,(zclGenericReq_t *)pReq);
}


/******************************
  Binary Input(Basic)Cluster
  See ZCL Specification  Section 3.14.4
*******************************/

/* Binary Input(basic) Cluster Attribute Definitions */
const zclAttrDef_t gaZclBinaryInputClusterFirstSetAttrDef[] = {
#if gZclClusterOptionals_d
  { gZclAttrIdBinaryInputActiveText_c,   gZclDataTypeStr_c, gZclAttrFlagsInRAM_c, sizeof(zclStr16_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, activeText)},
  { gZclAttrIdBinaryInputDescription_c,  gZclDataTypeStr_c, gZclAttrFlagsInRAM_c, sizeof(zclStr16_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, description)},
  { gZclAttrIdBinaryInputInactiveText_c, gZclDataTypeStr_c, gZclAttrFlagsInRAM_c, sizeof(zclStr16_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, inactiveText)},
#endif 
  { gZclAttrIdBinaryInputOutOfService_c, gZclDataTypeBool_c , gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, outOfService)},
#if gZclClusterOptionals_d
  { gZclAttrIdBinaryInputPolarity_c, gZclDataTypeEnum8_c , gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, polarity)},
#endif 
  { gZclAttrIdBinaryInputPresentValue_c, gZclDataTypeBool_c , gZclAttrFlagsInRAM_c| gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t,presentValue)},
#if gZclClusterOptionals_d
  { gZclAttrIdBinaryInputReliability_c, gZclDataTypeEnum8_c , gZclAttrFlagsInRAM_c , sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t,reliability)},
#endif 
  { gZclAttrIdBinaryInputStatusFlags_c, gZclDataTypeBitmap8_c , gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, statusFlags)}
};

#if gZclClusterOptionals_d
const zclAttrDef_t gaZclBinaryInputClusterSecondSetAttrDef[] = {
  { gZclAttrIdBinaryInputApplicationType_c,gZclDataTypeUint32_c  , gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c ,sizeof(uint8_t),(void *)MbrOfs(zclBinaryInputAttrsRAM_t,applicationType)}
};
#endif 

const zclAttrSet_t gaZclBinaryInputClusterAttrSet[] = {
  {gZclAttrBinaryInputFirstSet_c, (void *)&gaZclBinaryInputClusterFirstSetAttrDef, NumberOfElements(gaZclBinaryInputClusterFirstSetAttrDef)}
#if gZclClusterOptionals_d  
  ,{gZclAttrBinaryInputSecondSet_c, (void *)&gaZclBinaryInputClusterSecondSetAttrDef, NumberOfElements(gaZclBinaryInputClusterSecondSetAttrDef)}
#endif 
};

const zclAttrSetList_t gZclBinaryInputClusterAttrSetList = {
  NumberOfElements(gaZclBinaryInputClusterAttrSet),
  gaZclBinaryInputClusterAttrSet
};

/******************************
  Commissioning Cluster
  See Commisioning cluser Specification 064699r12
*******************************/
zbTmrTimerID_t CommisioningTimer;
void LeaveTimerCallback(zbTmrTimerID_t timerid);
void StartTimerCallback(zbTmrTimerID_t timerid);
static uint8_t RestartDelay;
static uint16_t RestartJitter;
static bool_t RestartFromAttributeSet;
#include "TMR_Interface.h"
#include "ZdoApsInterface.h"

zbCommissioningAttributes_t gCommisioningServerAttrsData = {
/* initialize like ROM set..*/
  /*** Startup Attribute Set (064699r12, section 6.2.2.1) ***/
  {mDefaultNwkShortAddress_c},        /* x shortAddress (default 0xff,0xff) */
  {mDefaultNwkExtendedPANID_c},       /* x nwkExtendedPANId */
  {mDefultApsUseExtendedPANID_c},     /* x apsUSeExtendedPANId */
  {mDefaultValueOfPanId_c},           /* x panId */
  { (uint8_t)(mDefaultValueOfChannel_c & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>8) & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>16) & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>24) &0xff)
  },                                  /* x channelMask */
   mNwkProtocolVersion_c,            /* x protocolVersion, always 0x02=ZigBee 2006, 2007 */
   gDefaultValueOfNwkStackProfile_c, /* x stackProfile 0x01 or 0x02 */
   gStartupControl_Associate_c,      /* startupControl */
  {mDefaultValueOfTrustCenterLongAddress_c},  /* x trustCenterAddress */
  {mDefaultValueOfTrustCenterMasterKey_c},  /* trustCenterMasterKey */
  {mDefaultValueOfNetworkKey_c},      /* x networkKey */
   gApsUseInsecureJoinDefault_c,     /* x useInsecureJoin */ 
  {mDefaultValueOfTrustCenterLinkKey_c},      /* preconfiguredLinkKey (w/ trust center) */
   mDefaultValueOfNwkActiveKeySeqNumber_c, /* x networkKeySeqNum */
   mDefaultValueOfNwkKeyType_c,      /* x networkKeyType */
  {gNwkManagerShortAddr_c},           /* x networkManagerAddress, little endian */

  /*** Join Parameters Attribute Set (064699r12, section 6.2.2.2) ***/
   mDefaultValueOfNwkScanAttempts_c, /* x # of scan attempts */
  { (mDefaultValueOfNwkTimeBwnScans_c & 0xff),
    (mDefaultValueOfNwkTimeBwnScans_c >> 8)
  },                                  /* x time between scans(ms) */
  {(mDefaultValueOfRejoinInterval_c&0xff),
   (mDefaultValueOfRejoinInterval_c>>8)
  },                                  /* x rejoin interval (sec) */
  {(mDefaultValueOfMaxRejoinInterval_c & 0xff),
   (mDefaultValueOfMaxRejoinInterval_c >> 8)
  },                                  /* x maxRejoinInterval (sec) */

  /*** End-Device Parameters Attribute Set (064699r12, section 6.2.2.3) ***/
  {(mDefaultValueOfIndirectPollRate_c & 0xff),
   (mDefaultValueOfIndirectPollRate_c >> 8)
  },                                  /* x indirectPollRate(ms) */
  gMaxNwkLinkRetryThreshold_c,      /* x parentRetryThreshold */

  /*** Concentrator Parameters Attribute Set (064699r12, section 6.2.2.4) ***/
  gConcentratorFlag_d,              /* x concentratorFlag */
  gConcentratorRadius_c,            /* x concentratorRadius */
  gConcentratorDiscoveryTime_c,     /* x concentratorDiscoveryTime */
};

const zclAttrDef_t gaZclCommisioningServerAttrDef[] = {
/* Server attributes */
  { gZclAttrIdCommissioning_ShortAddressID_c,              gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aShortAddress) },
  { gZclAttrIdCommissioning_ExtendedPANIdID_c,             gZclDataTypeIeeeAddr_c,      gZclAttrFlagsInRAM_c,                           sizeof(zbIeeeAddr_t),   (void *)MbrOfs(zbCommissioningAttributes_t, aNwkExtendedPanId) },
  { gZclAttrIdCommissioning_PanIdID_c,                     gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aPanId) },
  { gZclAttrIdCommissioning_ChannelMaskID_c,               gZclDataTypeBitmap32_c,      gZclAttrFlagsInRAM_c,                           sizeof(zbChannels_t),   (void *)MbrOfs(zbCommissioningAttributes_t, aChannelMask) },
  { gZclAttrIdCommissioning_ProtocolVersionID_c,           gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, protocolVersion) },
  { gZclAttrIdCommissioning_StackProfileID_c,              gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, stackProfile) },
  { gZclAttrIdCommissioning_StartupControlID_c,            gZclDataTypeEnum8_c,         gZclAttrFlagsInRAM_c,                           sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, startupControl) },
  { gZclAttrIdCommissioning_TrustCenterAddressID_c,        gZclDataTypeIeeeAddr_c,      gZclAttrFlagsInRAM_c,                           sizeof(zbIeeeAddr_t),   (void *)MbrOfs(zbCommissioningAttributes_t, aTrustCenterAddress) },
  { gZclAttrIdCommissioning_TrustCenterMasterKeyID_c,      gZclDataTypeSecurityKey_c,   gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(zbAESKey_t),     (void *)MbrOfs(zbCommissioningAttributes_t, aTrustCenterMasterKey) },
  { gZclAttrIdCommissioning_NetworkKeyID_c,                gZclDataTypeSecurityKey_c,   gZclAttrFlagsInRAM_c,                           sizeof(zbAESKey_t),     (void *)MbrOfs(zbCommissioningAttributes_t, aNetworkKey) },
  { gZclAttrIdCommissioning_UseInsecureJoinID_c,           gZclDataTypeUint8_c,         gZclAttrFlagsInRAM_c,                           sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, fUseInsecureJoin) },
  { gZclAttrIdCommissioning_PreconfiguredLinkKeyID_c,      gZclDataTypeSecurityKey_c,   gZclAttrFlagsInRAM_c,                           sizeof(zbAESKey_t),     (void *)MbrOfs(zbCommissioningAttributes_t, aPreconfiguredTrustCenterLinkKey) },
  { gZclAttrIdCommissioning_NetworkKeySeqNumID_c,          gZclDataTypeUint8_c,         gZclAttrFlagsInRAM_c,                           sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, activeNetworkKeySeqNum) },
  { gZclAttrIdCommissioning_NetworkKeyTypeID_c,            gZclDataTypeUint8_c,         gZclAttrFlagsInRAM_c,                           sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, networkKeyType) },
  { gZclAttrIdCommissioning_NetworkManagerAddressID_c,     gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aNetworkManagerAddress) },
  { gZclAttrIdCommissioning_ScanAttemptsID_c,              gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, scanAttempts) },
  { gZclAttrIdCommissioning_TimeBetweenScansID_c,          gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aTimeBetweenScans) },
  { gZclAttrIdCommissioning_RejoinIntervalID_c,            gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aRejoinInterval) },
  { gZclAttrIdCommissioning_MaxRejoinIntervalID_c,         gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aMaxRejoinInterval) },
  { gZclAttrIdCommissioning_IndirectPollRateID_c,          gZclDataTypeUint16_c,        gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint16_t),       (void *)MbrOfs(zbCommissioningAttributes_t, aIndirectPollRate) },
  { gZclAttrIdCommissioning_ParentRetryThreshoID_c,        gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, parentLinkRetryThreshold) },
  { gZclAttrIdCommissioning_ConcentratorFlagID_c,          gZclDataTypeBool_c,          gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(bool_t),         (void *)MbrOfs(zbCommissioningAttributes_t, fConcentratorFlag) },
  { gZclAttrIdCommissioning_ConcentratorRadiusID_c,        gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, concentratorRadius) },
  { gZclAttrIdCommissioning_ConcentratorDiscoveryTimeID_c, gZclDataTypeUint8_c,         gZclAttrFlagsRdOnly_c | gZclAttrFlagsInRAM_c,   sizeof(uint8_t),        (void *)MbrOfs(zbCommissioningAttributes_t, concentratorDiscoveryTime) }
};

const zclAttrSet_t gZclCommissioningServerClusterAttrSet[] = {
  {gZclAttrSetBasicDeviceInformation_c, (void *)&gaZclCommisioningServerAttrDef, NumberOfElements(gaZclCommisioningServerAttrDef)}
};

const zclAttrSetList_t gZclCommissioningServerClusterAttrSetList = {
  NumberOfElements(gZclCommissioningServerClusterAttrSet),
  gZclCommissioningServerClusterAttrSet
};

ZdoStartMode_t   gStartUpMode_DeviceType = 0;
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
zbStatus_t ZCL_CommisioningClusterServer
(
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
)
{
  zclFrame_t *pFrame;
  zclCommissioningCmd_t *Command; 
  zbStatus_t status = gZclSuccess_c; 
  (void)pDevice;

  /* ZCL frame */
  pFrame = (zclFrame_t*)pIndication->pAsdu;
  Command = (zclCommissioningCmd_t *) (&(pFrame->command) + sizeof(uint8_t));

  switch (pFrame->command)
  {
    case gZclCmdCommissiong_RestartDeviceRequest_c:
      if((Command->RestartDeviceCmd.Options & gCommisioningClusterRestartDeviceRequestOptions_StartUpModeSubField_Mask_c) ==
          gCommisioningClusterRestartDeviceRequestOptions_ModeSubField_RestartUsingCurrentStartupParameters_c) 
      {
       /*
         Consistency check is done on:
         Start up control (form network) if device is not capable of being a coordinator
         Start up control (silent start) if stack profile is 1.
         Pan ID is checked whether it is within range
       */

#if (!(gComboDeviceCapability_d || gCoordinatorCapability_d))
        /* Form network startup only allowed for coordinator or combo device */
        if(gCommisioningServerAttrsData.startupControl == gStartupControl_Form_c) {
          status = gZclInconsistentStatupState_c;
        }
#endif
        // Check that PAN ID is set to 0xFFFF or below 0xFFFE
        if ((gCommisioningServerAttrsData.aPanId[0] != 0xFF) && (gCommisioningServerAttrsData.aPanId[1] != 0xFF))
        {
          if(gCommisioningServerAttrsData.aPanId[0] > gaPIDUpperLimit[0]) 
          {
            status = gZclInconsistentStatupState_c;
          }
        }

        if (gCommisioningServerAttrsData.startupControl == gStartupControl_SilentStart_c)
        {
#if (gDefaultValueOfNwkStackProfile_c == 0x01)
          /* consistency check for stack profile 1 */
          status = gZclInconsistentStatupState_c;
#else
          status = gZclSuccess_c;
#endif
        }

        RestartFromAttributeSet = TRUE;
      } else

        if((Command->RestartDeviceCmd.Options & gCommisioningClusterRestartDeviceRequestOptions_StartUpModeSubField_Mask_c) ==
            gCommisioningClusterRestartDeviceRequestOptions_ModeSubField_RestartUsingCurrentStackParameter_c) 
        {
          RestartFromAttributeSet = FALSE;
        }
        else
        {
          // invalid command, return error code.
          status = gZclUnsupportedClusterCommand_c;
        }

        /*
          if the device is a combo then determine the Type of Devices for the
          start mode.
        */
        gStartUpMode_DeviceType = gCommisioningServerAttrsData.startupControl == gStartupControl_Form_c ? gZdoStartMode_Zc_c:gZdoStartMode_Zr_c;

        /* Set up start delay and just and initiate leave */
        RestartDelay = Command->RestartDeviceCmd.Delay;
        RestartJitter = GetRandomRange(0,Command->RestartDeviceCmd.Jitter) * 80;
        CommisioningTimer = ZbTMR_AllocateTimer();
        status = gZclInconsistentStatupState_c;
        if (CommisioningTimer != gTmrInvalidTimerID_c)
        {
          /*Start timer.with +200 milliseconds so the ZCL response is sent before leave is initiated.*/
          ZbTMR_StartSingleShotTimer(CommisioningTimer,200,LeaveTimerCallback);
          status = gZclSuccess_c;
        }

        if (status != gZclSuccess_c) 
        {
          (void) ZCL_Reply(pIndication, sizeof(status), &status);
          return gZclSuccess_c;
        }
    break;

    case gZclCmdCommissiong_SaveStartupParametersRequest_c:
      // Optional - not supported.
      status = gZclUnsupportedClusterCommand_c;
    break;

    case gZclCmdCommissiong_RestoreStartupParametersRequest_c:
      // Optional - not supported.
      status = gZclUnsupportedClusterCommand_c;
    break;

    case gZclCmdCommissiong_ResetStartupParametersRequest_c:
      if ((Command->ResetStartupParameterCmd.Options & gCommisioningClusterResetStartupParametersRequestOptions_ResetCurrentSubField_Mask_c) ||
          (Command->ResetStartupParameterCmd.Options & gCommisioningClusterResetStartupParametersRequestOptions_ResetAllSubField_Mask_c)) 
      {
        /*Copy set from ROM to ram */ 
        FLib_MemCpy(&gCommisioningServerAttrsData, (void *) &gSAS_Rom, sizeof(gSAS_Rom));
      }
      else
        if (Command->ResetStartupParameterCmd.Options & gCommisioningClusterResetStartupParametersRequestOptions_EraseIndexSubField_Mask_c) 
        {
          /* any other combination return error */  
          status = gZclUnsupportedClusterCommand_c; 
        }
    break;
    
  default:
    return gZclUnsupportedClusterCommand_c;
  }

  (void)ZCL_Reply(pIndication, sizeof(status), &status);
  return gZclSuccess_c;
}

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
zbStatus_t ZCL_CommisioningClusterClient
(
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
)
{
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c; 
  /*  zclCmdCommissiong_response_t *Command; */
 
  (void)pDevice;

  /* ZCL frame */
  pFrame = (zclFrame_t*)pIndication->pAsdu;
/*  Command = (zclCmdCommissiong_response_t *) (&(pFrame->command) + sizeof(uint8_t));*/
  
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	   status = gZclSuccess_c;  

  switch (pFrame->command)
  {
    case gZclCmdCommissiong_RestartDeviceResponse_c:
    case gZclCmdCommissiong_SaveStartupParametersResponse_c:
    case gZclCmdCommissiong_RestoreStartupParametersResponse_c:
    case gZclCmdCommissiong_ResetStartupParametersResponse_c:
      return status;   
    default:
      return gZclUnsupportedClusterCommand_c;      
  }
  }


/* Commissioning cluster Client commands */
#if gASL_ZclCommissioningRestartDeviceRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_RestartDeviceReq(zclCommissioningRestartDeviceReq_t *pReq) 
 *
 * @brief	Sends over-the-air a RestartDeviceRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_RestartDeviceReq
(
	zclCommissioningRestartDeviceReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_RestartDeviceRequest_c, sizeof(zclCmdCommissiong_RestartDeviceRequest_t),(zclGenericReq_t *)pReq);	
}
#endif


#if gASL_ZclCommissioningSaveStartupParametersRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_SaveStartupParametersReq(zclCommissioningSaveStartupParametersReq_t *pReq) 
 *
 * @brief	Sends over-the-air a SaveStartupParametersRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_SaveStartupParametersReq
(
	zclCommissioningSaveStartupParametersReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_SaveStartupParametersRequest_c, sizeof(zclCmdCommissiong_SaveStartupParametersRequest_t),(zclGenericReq_t *)pReq);	
}
#endif


#if gASL_ZclCommissioningRestoreStartupParametersRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_RestoreStartupParametersReq(zclCommissioningRestoreStartupParametersReq_t *pReq) 
 *
 * @brief	Sends over-the-air a RestoreStartupParametersRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_RestoreStartupParametersReq
(
	zclCommissioningRestoreStartupParametersReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_RestoreStartupParametersRequest_c, sizeof(zclCmdCommissiong_RestoreStartupParametersRequest_t),(zclGenericReq_t *)pReq);	
}
#endif



#if gASL_ZclCommissioningResetStartupParametersRequest_d
/*!
 * @fn 		zbStatus_t ZCL_Commisioning_ResetStartupParametersReq(zclCommissioningResetStartupParametersReq_t *pReq) 
 *
 * @brief	Sends over-the-air a ResetStartupParametersRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t  ZCL_Commisioning_ResetStartupParametersReq
(
	zclCommissioningResetStartupParametersReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_ResetStartupParametersRequest_c, sizeof(zclCmdCommissiong_ResetStartupParametersRequest_t),(zclGenericReq_t *)pReq);	
} 
#endif


/**************************************************************************
	Alpha-Secure Key Establishment Cluster (Health Care Profile Annex A.2)
***************************************************************************/
/*!
 * @fn 		zbStatus_t ZCL_ASKEClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ZCL_ASKEClusterClient
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;
    zbStatus_t status = gZclSuccessDefaultRsp_c;     
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;  
       
    
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdTxASKE_ConfigureSDResponse_c:
        case gZclCmdTxASKE_UpdateRevocationListResponse_c:
        case gZclCmdTxASKE_RemoveSDResponse_c:
        case gZclCmdTxASKE_ReadSDResponse_c:
        case gZclCmdTxASKE_InitiateASKEResponse_c:
        case gZclCmdTxASKE_ConfirmASKEkeyResponse_c:
        case gZclCmdRxASKE_TerminateASKE_c:
        case gZclCmdTxASKE_GenerateAMKResponse_c:
        case gZclCmdTxASKE_ReportRevokedNode_c:
        case gZclCmdTxASKE_RequestConfiguration_c:

          /* These should be passed up to a host*/
          return status;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ZCL_ASKEClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ZCL_ASKEClusterServer
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdRxASKE_ConfigureSD_c:
        case gZclCmdRxASKE_UpdateRevocationList_c:
        case gZclCmdRxASKE_RemoveSD_c:
        case gZclCmdRxASKE_ReadSD_c:
        case gZclCmdRxASKE_InitiateASKE_c:
        case gZclCmdRxASKE_ConfirmASKEkey_c:
        case gZclCmdRxASKE_TerminateASKE_c:
        case gZclCmdRxASKE_GenerateAMK_c:
          /* These should be passed up to a host*/
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ASKE_ConfigureSD(zclCmdASKE_ConfigureSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSD command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_ConfigureSD
(
zclCmdASKE_ConfigureSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_ConfigureSD_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_UpdateRevocationList(zclCmdASKE_UpdateRevocationList_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationList command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_UpdateRevocationList
(
zclCmdASKE_UpdateRevocationList_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_UpdateRevocationList_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_RemoveSD(zclCmdASKE_RemoveSD_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveSD command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_RemoveSD
(
zclCmdASKE_RemoveSD_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_RemoveSD_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ReadSD(zclCmdASKE_ReadSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSD command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_ReadSD
(
zclCmdASKE_ReadSD_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_ReadSD_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_InitiateASKE(zclCmdASKE_InitiateASKE_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASKE command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_InitiateASKE
(
zclCmdASKE_InitiateASKE_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_InitiateASKE_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ConfirmASKEkey(zclCmdASKE_ConfirmASKEkey_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfirmASKEkey command from the Alpha-Secure Key Establishment Cluster Client.  
 *
 */
zbStatus_t ASKE_ConfirmASKEkey
(
zclCmdASKE_ConfirmASKEkey_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_ConfirmASKEkey_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_TerminateASKE(zclCmdASKE_TerminateASKE_t *pReq) 
 *
 * @brief	Sends over-the-air a TerminateASKE command from the Alpha-Secure Key Establishment Cluster Client.  
 *
 */
zbStatus_t ASKE_TerminateASKE
(
zclCmdASKE_TerminateASKE_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_TerminateASKE_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_GenerateAMK(zclCmdASKE_GenerateAMK_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMK command from the Alpha-Secure Key Establishment Cluster Client.  
 *
 */
zbStatus_t ASKE_GenerateAMK
(
zclCmdASKE_GenerateAMK_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_GenerateAMK_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ConfigureSDresponse(zclCmdASKE_ConfigureSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSDresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ConfigureSDresponse
(
zclCmdASKE_ConfigureSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ConfigureSDResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_UpdateRevocationListResponse(zclCmdASKE_UpdateRevocationListResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationListResponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_UpdateRevocationListResponse
(
zclCmdASKE_UpdateRevocationListResponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_UpdateRevocationListResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_RemoveSDresponse(zclCmdASKE_RemoveSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveSDresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_RemoveSDresponse
(
zclCmdASKE_RemoveSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_RemoveSDResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ReadSDresponse(zclCmdASKE_ReadSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSDresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ReadSDresponse
(
zclCmdASKE_ReadSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ReadSDResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_InitiateASKEresponse(zclCmdASKE_InitiateASKEresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASKEresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_InitiateASKEresponse
(
zclCmdASKE_InitiateASKEresponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_InitiateASKEResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ConfirmASKEkeyResponse(zclCmdASKE_ConfirmASKEkeyResponse_t *pReq);
 *
 * @brief	Sends over-the-air a ConfirmASKEkeyResponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ConfirmASKEkeyResponse
(
zclCmdASKE_ConfirmASKEkeyResponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ConfirmASKEkeyResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_GenerateAMKresponse(zclCmdASKE_GenerateAMKresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMKresponse command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_GenerateAMKresponse
(
zclCmdASKE_GenerateAMKresponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_GenerateAMKResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_ReportRevokedNode(zclCmdASKE_ReportRevokedNode_t *pReq) 
 *
 * @brief	Sends over-the-air a ReportRevokedNode command from the Alpha-Secure Key Establishment Cluster Server. 
 *
 */
zbStatus_t ASKE_ReportRevokedNode
(
zclCmdASKE_ReportRevokedNode_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ReportRevokedNode_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASKE_RequestConfiguration(zclCmdASKE_RequestConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air a RequestConfiguration command from the Alpha-Secure Key Establishment Cluster Client. 
 *
 */
zbStatus_t ASKE_RequestConfiguration
(
zclCmdASKE_RequestConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_RequestConfiguration_c,0,(zclGenericReq_t *)pReq);
}


/**************************************************************************
	Alpha-Secure Access Control Cluster (Health Care Profile Annex A.3)
***************************************************************************/
/*!
 * @fn 		zbStatus_t ZCL_ASACClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Access Control Cluster Client. 
 *
 */
zbStatus_t ZCL_ASACClusterClient
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
	zclFrame_t *pFrame;
    zclCmd_t command;
    zbStatus_t status = gZclSuccessDefaultRsp_c;     
    
   
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;  
    
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdTxASAC_ConfigureSDresponse_c:
        case gZclCmdTxASAC_UpdateRevocationListResponse_c:
        case gZclCmdTxASAC_ConfigureACpoliciesResponse_c:
        case gZclCmdTxASAC_ReadSDresponse_c:
        case gZclCmdTxASAC_ReadACpoliciesResponse_c:
        case gZclCmdTxASAC_RemoveACresponse_c:
        case gZclCmdTxASAC_ACpropertiesResponse_c:
        case gZclCmdTxASAC_TSreport_c:
        case gZclCmdTxASAC_InitiateASACresponse_c:
        case gZclCmdTxASAC_ConfirmASACkeyResponse_c:
        case gZclCmdTxASAC_TerminateASAC_c:
        case gZclCmdTxASAC_GenerateAMKresponse_c:
        case gZclCmdTxASAC_LDCtransportResponse_c:
        case gZclCmdTxASAC_RequestConfiguration_c:

          /* These should be passed up to a host*/
          return status;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ZCL_ASACClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Alpha-Secure Access Control Cluster Server. 
 *
 */
zbStatus_t ZCL_ASACClusterServer
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdRxASAC_ConfigureSD_c:
        case gZclCmdRxASAC_UpdateRevocationList_c:
        case gZclCmdRxASAC_ConfigureACpolicies_c:
        case gZclCmdRxASAC_ReadSD_c:
        case gZclCmdRxASAC_ReadACpolicies_c:
        case gZclCmdRxASAC_RemoveAC_c:
        case gZclCmdRxASAC_ACpropertiesReq_c:
        case gZclCmdRxASAC_TSreportResponse_c:
        case gZclCmdRxASAC_InitiateASAC_c:
        case gZclCmdRxASAC_ConfirmASACkey_c:
        case gZclCmdRxASAC_TerminateASAC_c:
        case gZclCmdRxASAC_GenerateAMK_c:
        case gZclCmdRxASAC_LDCtransport_c:
        
          /* These should be passed up to a host*/
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ASAC_ConfigureSD(zclCmdASAC_ConfigureSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSD command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ConfigureSD
(
  zclCmdASAC_ConfigureSD_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ConfigureSD_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_UpdateRevocationList(zclCmdASAC_UpdateRevocationList_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationList command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_UpdateRevocationList
(
  zclCmdASAC_UpdateRevocationList_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_UpdateRevocationList_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ConfigureACpolicies(zclCmdASAC_ConfigureACpolicies_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureACpolicies command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ConfigureACpolicies
(
  zclCmdASAC_ConfigureACpolicies_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ConfigureACpolicies_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ReadSD(zclCmdASAC_ReadSD_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSD command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ReadSD
(
  zclCmdASAC_ReadSD_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ReadSD_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ReadACpolicies(zclCmdASAC_ReadACpolicies_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadACpolicies command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ReadACpolicies
(
  zclCmdASAC_ReadACpolicies_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ReadACpolicies_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_RemoveAC(zclCmdASAC_RemoveAC_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveAC command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_RemoveAC
(
  zclCmdASAC_RemoveAC_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_RemoveAC_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ACpropertiesRequest(void) 
 *
 * @brief	Sends over-the-air a ACpropertiesRequest command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ACpropertiesRequest(void)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
//    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ACpropertiesReq_c,0,(zclGenericReq_t *)pReq);
  return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t ASAC_TSReportResponse(void) 
 *
 * @brief	Sends over-the-air a TSReportResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_TSReportResponse(void)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
//    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_TSreportResponse_c,0,(zclGenericReq_t *)pReq);
return gZbSuccess_c;
}

/*!
 * @fn 		zbStatus_t ASAC_InitiateASAC(zclCmdASAC_InitiateASAC_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASAC command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_InitiateASAC
(
  zclCmdASAC_InitiateASAC_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_InitiateASAC_c,0,(zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t ASAC_ConfirmASACkey(zclCmdASAC_ConfirmASAC_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfirmASACkey command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_ConfirmASACkey
(
  zclCmdASAC_ConfirmASAC_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ConfirmASACkey_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_TerminateASAC(zclCmdASAC_TerminateASAC_t *pReq) 
 *
 * @brief	Sends over-the-air a TerminateASAC command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_TerminateASAC
(
  zclCmdASAC_TerminateASAC_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_TerminateASAC_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_GenerateAMK(zclCmdASAC_GenerateAMK_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMK command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_GenerateAMK
(
  zclCmdASAC_GenerateAMK_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_GenerateAMK_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_LDCtransport(zclCmdASAC_LDCtransport_t *pReq) 
 *
 * @brief	Sends over-the-air a LDCtransport command from the Alpha-Secure Access Control Cluster Client.  
 *
 */
zbStatus_t ASAC_LDCtransport
(
  zclCmdASAC_LDCtransport_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_LDCtransport_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ConfigureSDresponse(zclCmdASAC_ConfigureSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureSDresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ConfigureSDresponse
(
  zclCmdASAC_ConfigureSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ConfigureSDresponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_UpdateRevocationListResponse(zclCmdASAC_UpdateRevocationListResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a UpdateRevocationListResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_UpdateRevocationListResponse
(
  zclCmdASAC_UpdateRevocationListResponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_UpdateRevocationListResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ConfigureACpoliciesResponse(zclCmdASAC_ConfigureACpoliciesResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfigureACpoliciesResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ConfigureACpoliciesResponse
(
  zclCmdASAC_ConfigureACpoliciesResponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ConfigureACpoliciesResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ReadSDresponse(zclCmdASAC_ReadSDresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadSDresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ReadSDresponse
(
  zclCmdASAC_ReadSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ReadSDresponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ReadACpoliciesResponse(zclCmdASAC_ReadACpoliciesResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ReadACpoliciesResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ReadACpoliciesResponse
(
  zclCmdASAC_ReadACpoliciesResponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ReadACpoliciesResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_RemoveACresponse(zclCmdASAC_RemoveACresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a RemoveACresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_RemoveACresponse
(
  zclCmdASAC_RemoveACresponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_RemoveACresponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ACpropertiesResponse(zclCmdASAC_ACpropertiesResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ACpropertiesResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ACpropertiesResponse
(
  zclCmdASAC_ACpropertiesResponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ACpropertiesResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_TSReport(zclCmdASAC_TSreport_t *pReq) 
 *
 * @brief	Sends over-the-air a TSReport command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_TSReport
(
  zclCmdASAC_TSreport_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_TSreport_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_InitiateASACresponse(zclCmdASAC_InitiateASACresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a InitiateASACresponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_InitiateASACresponse
(
  zclCmdASAC_InitiateASACresponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_InitiateASACresponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_ConfirmASACkeyResponse(zclCmdASAC_ConfirmASACkeyResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ConfirmASACkeyResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_ConfirmASACkeyResponse
(
  zclCmdASAC_ConfirmASACkeyResponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ConfirmASACkeyResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_GenerateAMKresponse(zclCmdASAC_GenerateAMKresponse_t *pReq) 
 *
 * @brief	Sends over-the-air a GenerateAMKrespons command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_GenerateAMKresponse
(
  zclCmdASAC_GenerateAMKresponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_GenerateAMKresponse_c,0,(zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t ASAC_LDCtransportResponse(zclCmdASAC_LDCtransportResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a LDCtransportResponse command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_LDCtransportResponse
(
  zclCmdASAC_LDCtransportResponse_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_LDCtransportResponse_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ASAC_RequestConfiguration(zclCmdASAC_RequestConfiguration_t *pReq) 
 *
 * @brief	Sends over-the-air a RequestConfiguration command from the Alpha-Secure Access Control Cluster Server.  
 *
 */
zbStatus_t ASAC_RequestConfiguration
(
  zclCmdASAC_RequestConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_RequestConfiguration_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void LeaveTimerCallback(zbTmrTimerID_t timerid) 
 *
 * @brief	Leave timer callback, executes the leave commands, and set up a timer to restart ZDO.
 *
 */
void LeaveTimerCallback(zbTmrTimerID_t timerid) 
{

  /* Leave network, but keep the current in RAM defaults (for example, node type in a Zx Combo device) */
  ZDO_StopEx(gZdoStopMode_Announce_c | gZdoStopMode_ResetNvm_c);

  /*setup timer for rejoining, a 200 msec is added to insure that the leave command has happend*/  
  ZbTMR_StartSingleShotTimer(timerid,((zbTmrTimeInMilliseconds_t)(RestartDelay) * 1000) + RestartJitter, StartTimerCallback);
}

/*!
 * @fn 		void StartTimerCallback(zbTmrTimerID_t timerid) 
 *
 * @brief	Start timer callback, frees timer used, and restart ZDO
 *
 */
void StartTimerCallback(zbTmrTimerID_t timerid) 
{
 
 
  if (ZDO_GetState() != gZdoInitialState_c)
  {
    /*
      if ZDO state machine has not yet left the network and moved to initial state then,
      check status again after 50 milliseconds
    */
    ZbTMR_StartSingleShotTimer(timerid,(zbTmrTimeInMilliseconds_t)50,StartTimerCallback);
    return;
  }

  /*Free timer, as we do not need it anymore*/
  ZbTMR_FreeTimer(timerid);

  /* Start restart the ZDO with the commisioning cluster settings */ 
  if (RestartFromAttributeSet)
  {
    ASL_SetCommissioningSAS(&gCommisioningServerAttrsData);
    /* Note whether rejoin or associate should be done is determined by the ZDO when a SAS set is used
    */
    ZDO_Start(gStartUpMode_DeviceType | gZdoStartMode_SasSet_c | gZdoStartMode_Associate_c);
  }
  else
  {
    ASL_SetCommissioningSAS(NULL);
    ZDO_Start(gStartUpMode_DeviceType | gZdoStartMode_RamSet_c | gZdoStartMode_Associate_c);
  }
}



/******************************
  Poll Control Cluster 
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.5 [R1]
*******************************/
#if gZclEnablePollControlCluster_d
#if gEndDevCapability_d || gComboDeviceCapability_d 
zclPollControlServerSetup_t gPollControlServerSetup[gNoOfPollControlServerInstances_d];
#endif

static zclPollControlClientSetup_t  gPollControlClientSetup = {
    FALSE,
    gZclPollControl_FastPollTimeoutClient_Test_c  //30 seconds
};

/* Poll Control Cluster Attribute Definitions */
const zclAttrDef_t gaZclPollControlClusterAttrDef[] = {
  { gZclAttrIdPollControl_CheckInInterval_c,       gZclDataTypeUint32_c,      gZclAttrFlagsInRAM_c,                         sizeof(uint32_t),  (void *)MbrOfs(zclPollControlAttrsRAM_t, checkInInterval)},
  { gZclAttrIdPollControl_LongPollInterval_c,      gZclDataTypeUint32_c,      gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t),  (void *)MbrOfs(zclPollControlAttrsRAM_t, longPollInterval)},
  { gZclAttrIdPollControl_ShortPollInterval_c,     gZclDataTypeUint16_c,      gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),  (void *)MbrOfs(zclPollControlAttrsRAM_t, shortPollInterval)},
  { gZclAttrIdPollControl_FastPollTimeout_c,       gZclDataTypeUint16_c,      gZclAttrFlagsInRAM_c,                         sizeof(uint16_t),  (void *)MbrOfs(zclPollControlAttrsRAM_t, fastPollTimeout)}
#if gZclClusterOptionals_d
  ,{ gZclAttrIdPollControl_CheckInIntervalMin_c,   gZclDataTypeUint32_c,      gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t),  (void *)MbrOfs(zclPollControlAttrsRAM_t, checkInIntervalMin)},
  { gZclAttrIdPollControl_LongPollIntervalMin_c,   gZclDataTypeUint32_c,      gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t),  (void *)MbrOfs(zclPollControlAttrsRAM_t, longPollIntervalMin)},
  { gZclAttrIdPollControl_FastPollTimeoutMax_c,    gZclDataTypeUint16_c,      gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),  (void *)MbrOfs(zclPollControlAttrsRAM_t, fastPollTimeoutMax)}  
#endif
 };

const zclAttrDefList_t gZclPollControlClusterAttrDefList = {
  NumberOfElements(gaZclPollControlClusterAttrDef),
  gaZclPollControlClusterAttrDef
};

const zclAttrSet_t gaZclPollControlClusterAttrSet[] = {
  {gZclAttrSetPollControl_c, (void *)&gaZclPollControlClusterAttrDef, NumberOfElements(gaZclPollControlClusterAttrDef)}
};

const zclAttrSetList_t gZclPollControlClusterAttrSetList = {
  NumberOfElements(gaZclPollControlClusterAttrSet),
  gaZclPollControlClusterAttrSet
};

const zclCmd_t gaZclPollControlClusterCmdReceivedDef[]={
  // commands received 
  gZclCmdPollControl_CheckInRsp_c,
  gZclCmdPollControl_FastPollStop_c,
  gZclCmdPollControl_SetLongPollInterval_c,
  gZclCmdPollControl_SetShortPollInterval_c
};
const zclCmd_t gaZclPollControlClusterCmdGeneratedDef[]={
  // commands generated 
  gZclCmdPollControl_CheckIn_c
};

const zclCommandsDefList_t gZclPollControlClusterCommandsDefList =
{
   NumberOfElements(gaZclPollControlClusterCmdReceivedDef), gaZclPollControlClusterCmdReceivedDef,
   NumberOfElements(gaZclPollControlClusterCmdGeneratedDef), gaZclPollControlClusterCmdGeneratedDef
};

/*!
 * @fn 		zbStatus_t ZCL_PollControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Poll Control Cluster Server. 
 *
 */
zbStatus_t ZCL_PollControlClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zbStatus_t status = gZclUnsupportedClusterCommand_c;
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
  if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
#endif
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo;
  uint32_t longPollIntervalAttr, checkInIntervalAttr;
  uint16_t shortPollIntervalAttr;
  uint8_t setupIndex;

  /* to prevent compiler warning */
  (void)pDevice;
  
   status = gZclSuccess_c;  
   pFrame = (void *)pIndication->pAsdu;

  /*Create the destination address*/
   AF_PrepareForReply(&addrInfo, pIndication); 

   /* validate client: */
   status = zclPollControl_ValidateClient(pIndication->srcAddrMode, pIndication->aSrcAddr, pIndication->srcEndPoint);
   if(status != gZclSuccess_c)
      return gZclActionDenied_c;
   
   setupIndex = ZCL_PollControl_GetIndexFromPollControlSetupTable(addrInfo.srcEndPoint, gTmrInvalidTimerID_c);
    
   if(setupIndex == gZclCluster_InvalidDataIndex_d)
      return gZclFailure_c;
   
   /* get values for  short, LongPoll and checkIn interval attributes */
   (void)ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPollControl_CheckInInterval_c,   gZclServerAttr_c, &checkInIntervalAttr,   NULL);
   (void)ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPollControl_LongPollInterval_c,  gZclServerAttr_c, &longPollIntervalAttr,  NULL);
   (void)ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPollControl_ShortPollInterval_c, gZclServerAttr_c, &shortPollIntervalAttr, NULL);
   longPollIntervalAttr  = OTA2Native32(longPollIntervalAttr);
   checkInIntervalAttr   = OTA2Native32(checkInIntervalAttr);
   shortPollIntervalAttr = OTA2Native16(shortPollIntervalAttr);
   
   switch (pFrame->command) 
   {
      case gZclCmdPollControl_CheckInRsp_c:
        { 
          uint8_t i;
          uint16_t fastPollTimeout, fastPollTimeoutAttr;
          zclCmdPollControl_CheckInRsp_t cmdPayload = *(zclCmdPollControl_CheckInRsp_t *)(pFrame+1);
                    
          /* check timeout interval */        
          if(gPollControlServerSetup[setupIndex].checkInRspState == gPollControl_CheckInRspTimeout_c)
            return gZclTimeout_c;
          
          /* validate payload and update poll control table*/
          (void)ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPollControl_FastPollTimeout_c, gZclServerAttr_c, &fastPollTimeoutAttr, NULL);
          fastPollTimeoutAttr = OTA2Native16(fastPollTimeoutAttr);
          fastPollTimeout = OTA2Native16(cmdPayload.fastPollTimeout);
          fastPollTimeout = (fastPollTimeout!=0)?fastPollTimeout:fastPollTimeoutAttr;
          
          i = gPollControlServerSetup[setupIndex].indexCheckInRsp;
          gPollControlServerSetup[setupIndex].currentPollCtrList[i].enableFastPool = cmdPayload.startFastPolling;
          gPollControlServerSetup[setupIndex].currentPollCtrList[i].fastPollTimeout = fastPollTimeout;
          Copy2Bytes(gPollControlServerSetup[setupIndex].currentPollCtrList[i].aNwkAddr, pIndication->aSrcAddr);
          if(!ZclPollControlValidateFastPollTimeout(setupIndex, fastPollTimeout))
          {
            gPollControlServerSetup[setupIndex].currentPollCtrList[i].enableFastPool = FALSE;
            status = gZclInvalidValue_c;
          }
          gPollControlServerSetup[setupIndex].indexCheckInRsp++;
      
          if(gPollControlServerSetup[setupIndex].indexCheckInRsp == gPollControlServerSetup[setupIndex].bindingTableEntries)
          {
            gPollControlServerSetup[setupIndex].indexCheckInRsp = 0;
            gPollControlServerSetup[setupIndex].checkInRspState = gPollControl_CheckInRspIdle_c;
            zclPollControl_StopFastPollMode(addrInfo.srcEndPoint, TRUE);
            for(i=0;i<gPollControlServerSetup[setupIndex].bindingTableEntries;i++)
            {
              if(gPollControlServerSetup[setupIndex].currentPollCtrList[i].enableFastPool== TRUE)
              {
                /* start fast polling process  */
                status = zclPollControl_StartFastPollMode(addrInfo.srcEndPoint, ZclPollControl_SearchMaxFastTimeout(setupIndex));
                break;
              }
            }
          }
          break;
        }
      case gZclCmdPollControl_FastPollStop_c:
        {          
          /* verify that the device is in fast poll mode */
          uint32_t pollRate = shortPollIntervalAttr*250; // seconds 
          if(NlmeGetRequest(gNwkIndirectPollRate_c) > pollRate)
            status = gZclActionDenied_c;
          else
          {
              /* stop fast poll rate if the user match */
              if(IsEqual2Bytes(pIndication->aSrcAddr,  gPollControlServerSetup[setupIndex].currentPollCtrList[gPollControlServerSetup[setupIndex].curentIndex].aNwkAddr) == TRUE)
                zclPollControl_StopFastPollMode(addrInfo.srcEndPoint, FALSE);
          }
          break;
          
        }
#if gZclClusterOptionals_d        
      case gZclCmdPollControl_SetLongPollInterval_c:  
        {
          /* get long poll interval from command received*/
          uint32_t longPollIntervalMinAttr = gZclPollControl_MinLongPollInterval_c;
          uint32_t longPollInterval = *(uint32_t *)(pFrame+1);
          
          longPollInterval = OTA2Native32(longPollInterval);
          
          (void)ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPollControl_LongPollIntervalMin_c,  gZclServerAttr_c, &longPollIntervalMinAttr,  NULL);

          
          /* set status as invalid */  
          status = gZclInvalidValue_c;
                            
          if((longPollInterval <= gZclPollControl_MaxLongPollInterval_c)
             &&(longPollInterval >= gZclPollControl_MinLongPollInterval_c))
          {
            if((longPollInterval >= longPollIntervalMinAttr)&&(longPollInterval >= shortPollIntervalAttr)&& 
                 (longPollInterval <= checkInIntervalAttr))
            {
              uint32_t currentPollRate = NlmeGetRequest(gNwkIndirectPollRate_c);
              
              if(currentPollRate == (longPollIntervalAttr*1000/4))
              {
                /* device is not in the fast Poll Rate => change Poll Rate*/
                (void)ZDO_NLME_ChangePollRate((uint16_t)(longPollInterval*1000/4));
              }
              
              /*store the new value for poll control attribute*/
              longPollInterval = Native2OTA32(longPollInterval);
              (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPollControl_LongPollInterval_c,  gZclServerAttr_c, &longPollInterval);
              
              /* update status */
              status = gZclSuccess_c;
            }
          }
          break;
        }
      case gZclCmdPollControl_SetShortPollInterval_c:  
        {
          /* get short poll interval from command received*/
          uint16_t shortPollInterval = *(uint16_t *)(pFrame+1);
          uint32_t currentPollRate = NlmeGetRequest(gNwkIndirectPollRate_c);
          
          shortPollInterval = OTA2Native16(shortPollInterval);

          if((shortPollInterval >= gZclPollControl_MinShortPollInterval_c)&&
             (shortPollInterval <= longPollIntervalAttr)&&(shortPollInterval <= checkInIntervalAttr))
            
          {
            if(currentPollRate == shortPollIntervalAttr*1000/4)
            {
              ZDO_NLME_ChangePollRate((uint16_t)(shortPollInterval*1000/4));
            }
           
            /*store the new value for poll control attribute*/
            shortPollInterval = Native2OTA16(shortPollInterval);
            (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrPollControl_ShortPollInterval_c,  gZclServerAttr_c, &shortPollInterval);

          }
          else
            status = gZclInvalidValue_c;
          break;
        }
#endif        
      default:
        return  gZclUnsupportedClusterCommand_c;                     
  }
  
  if(status == gZclSuccess_c)
  {
    status = (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)? gZclSuccess_c:gZclSuccessDefaultRsp_c;
  }
  
 #if gComboDeviceCapability_d
  }
 #endif /* gComboDeviceCapability_d */
#endif
   return status;
}

/*!
 * @fn 		zbStatus_t ZCL_PollControlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Poll Control Cluster Client. 
 *
 */
zbStatus_t ZCL_PollControlClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  /* to prevent compiler warning */
  (void)pDevice;
  
   pFrame = (void *)pIndication->pAsdu;
  
   /*Create the destination address*/
   AF_PrepareForReply(&addrInfo, pIndication); 
   if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
         status = gZclSuccess_c;
   switch (pFrame->command) 
   {
      case gZclCmdPollControl_CheckIn_c:  
        {
          /* to include the case when the client is not able to send a CheckInRsp Cmd*/
          if(gPollControlClientSetup.enableFastPooling != 0xFF)
          {
            zclPollControl_CheckInRsp_t commandRsp;
            FLib_MemCpy(&commandRsp.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
            commandRsp.zclTransactionId = pFrame->transactionId;
            commandRsp.cmdFrame.startFastPolling = gPollControlClientSetup.enableFastPooling;
            commandRsp.cmdFrame.fastPollTimeout = gPollControlClientSetup.fastPollTimeout;
            commandRsp.addrInfo.radiusCounter = afDefaultRadius_c;
            return zclPollControl_CheckInRsp(&commandRsp);
          }
          break;
        }
      default:
        return  gZclUnsupportedClusterCommand_c;                     
  }
  return status;
}


/*!
 * @fn 		zbStatus_t zclPollControl_CheckIn(zclPollControl_CheckIn_t *pReq) 
 *
 * @brief	Sends over-the-air a CheckIn command from the PollControl Cluster Server. 
 *
 */
zbStatus_t zclPollControl_CheckIn
( 
    zclPollControl_CheckIn_t *pReq
) 
{
  
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPollControl_c);   
    return ZCL_SendServerReqSeqPassed(gZclCmdPollControl_CheckIn_c, 0, (zclGenericReq_t *)pReq);
  
}

/*!
 * @fn 		zbStatus_t zclPollControl_CheckInRsp(zclPollControl_CheckInRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a CheckInRsp command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_CheckInRsp
( 
    zclPollControl_CheckInRsp_t *pReq
) 
{
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPollControl_c); 
    return ZCL_SendClientReqSeqPassed(gZclCmdPollControl_CheckInRsp_c, sizeof(zclCmdPollControl_CheckInRsp_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPollControl_FastPollStop(zclPollControl_FastPollStop_t *pReq) 
 *
 * @brief	Sends over-the-air a FastPollStop command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_FastPollStop
( 
    zclPollControl_FastPollStop_t *pReq
) 
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPollControl_c);  
    return ZCL_SendClientReqSeqPassed(gZclCmdPollControl_FastPollStop_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPollControl_SetLongPollInterval(zclPollControl_SetLongPollInterval_t *pReq) 
 *
 * @brief	Sends over-the-air a SetLongPollInterval command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_SetLongPollInterval
( 
    zclPollControl_SetLongPollInterval_t *pReq
) 
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPollControl_c);  
    return ZCL_SendClientReqSeqPassed(gZclCmdPollControl_SetLongPollInterval_c, sizeof(zclCmdPollControl_SetLongPollInterval_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclPollControl_SetShortPollInterval(zclPollControl_SetShortPollInterval_t *pReq) 
 *
 * @brief	Sends over-the-air a SetShortPollInterval command from the PollControl Cluster Client. 
 *
 */
zbStatus_t zclPollControl_SetShortPollInterval
( 
    zclPollControl_SetShortPollInterval_t *pReq
) 
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterPollControl_c);  
    return ZCL_SendClientReqSeqPassed(gZclCmdPollControl_SetShortPollInterval_c, sizeof(zclCmdPollControl_SetShortPollInterval_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclPollControl_ClusterInit(void)
 *
 * @brief	Init Poll Control cluster functionality 
 *
 */
#if gEndDevCapability_d || gComboDeviceCapability_d  
zbStatus_t zclPollControl_ClusterServerInit(void)
{
    zbStatus_t status = gZclFailure_c;
    uint8_t nextStartIndex = 0, i = 0;
    zbClusterId_t pollControlClusterId = {gaZclClusterPollControl_c};
    uint32_t minLongPollInterval = 0xFFFFFFFF;
    
    for(i = 0; i< gNoOfPollControlServerInstances_d; i++)
    {
      uint8_t endpoint;
      uint8_t startIndex = nextStartIndex;
      
      gPollControlServerSetup[i].endpoint = gZclCluster_InvalidDataIndex_d;
      gPollControlServerSetup[i].checkInIntervalTmr = gTmrInvalidTimerID_c;
      gPollControlServerSetup[i].fastPollTimeoutTmr = gTmrInvalidTimerID_c;
      gPollControlServerSetup[i].indexCheckInRsp = 0;
      gPollControlServerSetup[i].curentIndex = 0;
      gPollControlServerSetup[i].checkInRspState = gPollControl_CheckInRspIdle_c;
      
      endpoint = ZCL_GetEndPointForSpecificCluster(pollControlClusterId, TRUE, startIndex, &nextStartIndex);
      if(endpoint != gZclCluster_InvalidDataIndex_d)  
      {
        uint32_t checkInIntervalAttr, longPollIntervalAttr;    
        zbClusterId_t pollControlClusterId = {gaZclClusterPollControl_c};
        
        (void)ZCL_GetAttribute(endpoint, pollControlClusterId, gZclAttrPollControl_CheckInInterval_c,   gZclServerAttr_c, &checkInIntervalAttr,   NULL);
        (void)ZCL_GetAttribute(endpoint, pollControlClusterId, gZclAttrPollControl_LongPollInterval_c,  gZclServerAttr_c, &longPollIntervalAttr,  NULL);
        checkInIntervalAttr = OTA2Native32(checkInIntervalAttr);
        longPollIntervalAttr = OTA2Native32(longPollIntervalAttr);
        
        if(longPollIntervalAttr <= minLongPollInterval)
          minLongPollInterval = longPollIntervalAttr;
        
        gPollControlServerSetup[i].checkInIntervalTmr = ZbTMR_AllocateTimer();
        if(gPollControlServerSetup[i].checkInIntervalTmr == gTmrInvalidTimerID_c)
          return gZclNoMem_c;	
    
        if(checkInIntervalAttr != 0x00)
        {       
          /* checkInInterval - measured in quarter seconds */
          ZbTMR_StartTimer(gPollControlServerSetup[i].checkInIntervalTmr, gTmrSingleShotTimer_c,checkInIntervalAttr, ZclPollControl_CheckInCallback);
        }   
         gPollControlServerSetup[i].endpoint = endpoint;
         status = gZclSuccess_c;
      }
    }
    if(minLongPollInterval != 0xFFFFFFFF)
      (void)ZDO_NLME_ChangePollRate((uint16_t)(minLongPollInterval*1000/4));  
    return status;
}

/*****************************************************************************
* ZCL_PollControl_GetIndexFromPollControlSetupTable Fuction
* -return - index in the poll control setup table if succes
          - invalid data - otherwise
*****************************************************************************/
uint8_t ZCL_PollControl_GetIndexFromPollControlSetupTable(uint8_t endpoint, uint8_t tmrId)
{
  if(endpoint != gZclCluster_InvalidDataIndex_d) 
  {  
    for(uint8_t i=0;i<gNoOfPollControlServerInstances_d; i++)
      if(endpoint == gPollControlServerSetup[i].endpoint)
        return i;
  }
  if(tmrId != gTmrInvalidTimerID_c)
  {
     for(uint8_t i=0;i<gNoOfPollControlServerInstances_d; i++)
      if((tmrId == gPollControlServerSetup[i].checkInIntervalTmr) || (tmrId == gPollControlServerSetup[i].fastPollTimeoutTmr))
        return i;
  }
  return gZclCluster_InvalidDataIndex_d;
}

/*!
 * @fn 		void ZclPollControl_CheckInCallback(uint8_t tmrId)
 *
 * @brief	Callback used to send checkIn command, based on CheckInInterval Attribute.
 *
 */
void ZclPollControl_CheckInCallback(uint8_t tmrId)
{
    zclPollControl_CheckIn_t cmd;
    afAddrInfo_t addrInfo = {gZbAddrModeIndirect_c, {0x00, 0x00}, 0xFF, {gaZclClusterPollControl_c}, 0xFF, gApsTxOptionNone_c, 1};  
    bool_t sendCheckInCmd = FALSE;
    uint8_t i = 0, j = 0;
    uint32_t checkInIntervalAttr;
    uint8_t setupIndex = gZclCluster_InvalidDataIndex_d;
 #if gEndDevCapability_d || gComboDeviceCapability_d     
    setupIndex = ZCL_PollControl_GetIndexFromPollControlSetupTable(gZclCluster_InvalidDataIndex_d, tmrId);
#endif
    
    if(setupIndex == gZclCluster_InvalidDataIndex_d)
      return;
    
    addrInfo.radiusCounter = afDefaultRadius_c;
    gPollControlServerSetup[setupIndex].bindingTableEntries = 0;
    
    FLib_MemCpy(&cmd.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
    
    /* update Poll control setup information */
    for(i=0; i< gMaximumApsBindingTableEntries_c; i++)
    {
      for(j= 0; j< ZbBeeStackApsGlobals(gaApsBindingTable[i].iClusterCount); j++)
      {
    	if(IsEqual2BytesInt(ZbBeeStackApsGlobals(gaApsBindingTable[i].aClusterList[j]), gZclClusterPollControl_c)== TRUE)
    	{	
            sendCheckInCmd = TRUE;
            cmd.addrInfo.dstEndPoint = ZbBeeStackApsGlobals(gaApsBindingTable[i].dstEndPoint);
            cmd.addrInfo.srcEndPoint = ZbBeeStackApsGlobals(gaApsBindingTable[i].srcEndPoint);
            /* update binding table entries */
            gPollControlServerSetup[setupIndex].bindingTableEntries++;
            break;
        }
      }
      gPollControlServerSetup[setupIndex].currentPollCtrList[i].enableFastPool = FALSE;  
      gPollControlServerSetup[setupIndex].currentPollCtrList[i].fastPollTimeout = 0x00;
    }
    if(sendCheckInCmd)   
    {
       /* send CheckIn command */
       /* start fast poll mode until the device receive all CheckInRsp */
       gPollControlServerSetup[setupIndex].checkInRspState = gPollControl_CheckInRspStart_c;
       (void)zclPollControl_StartFastPollMode(gPollControlServerSetup[setupIndex].endpoint, (uint16_t)(gZclPollControl_CheckInRspMaxWaitDuraration_c*4));
       (void)zclPollControl_CheckIn(&cmd);
       
    }
    /* */
   (void)ZCL_GetAttribute(gPollControlServerSetup[setupIndex].endpoint, addrInfo.aClusterId, gZclAttrPollControl_CheckInInterval_c,   gZclServerAttr_c, &checkInIntervalAttr,   NULL);
    checkInIntervalAttr = OTA2Native32(checkInIntervalAttr);
    ZbTMR_StartTimer(gPollControlServerSetup[setupIndex].checkInIntervalTmr, gTmrSingleShotTimer_c, checkInIntervalAttr*1000/4, ZclPollControl_CheckInCallback);  
}

/*!
 * @fn 		zbStatus_t zclPollControl_ValidateClient(uint8_t addrMode, zbNwkAddr_t aDstAddr, zbEndPoint_t endPoint)
 *
 * @brief	Test client information. Return succes for matching parameters (binding table), otherwise return gZclNotAuthorized_c;
 *
 */

static zbStatus_t zclPollControl_ValidateClient(uint8_t addrMode, zbNwkAddr_t aAddr, zbEndPoint_t endPoint)
{
  uint8_t i, j;
  zbClusterId_t aClusterId = {gaZclClusterPollControl_c};
  
  if(addrMode == gZbAddrMode16Bit_c)
  {
    zbIeeeAddr_t addressOfInterest;
    if(APS_GetIeeeAddress(aAddr, addressOfInterest))   
    {
      for(i=0; i< gMaximumApsBindingTableEntries_c; i++)
      {
        for(j= 0; j<ZbBeeStackApsGlobals(gaApsBindingTable[i].iClusterCount); j++)
        {
          /* validate the binding is done for the poll control cluster */
          if(FLib_Cmp2Bytes(ZbBeeStackApsGlobals(gaApsBindingTable[i].aClusterList[j]), aClusterId))
          {
            uint8_t indexInAddressMap = ZbBeeStackApsGlobals(gaApsBindingTable[i].dstAddr.index);
            if(Cmp8Bytes(ZbBeeStackApsGlobals(gaApsAddressMap[indexInAddressMap].aIeeeAddr), addressOfInterest) && 
              (ZbBeeStackApsGlobals(gaApsBindingTable[i].dstEndPoint) == endPoint))
            {	
              return gZclSuccess_c;
            }
          }
        }
      }
    }
    return gZclActionDenied_c;
  }
  return gZclFailure_c;
}


/*!
 * @fn 		zbStatus_t zclPollControl_StartFastPollMode(uint16_t timeout)
 *
 * @brief	Start fast poll mode
 *
 */
zbStatus_t zclPollControl_StartFastPollMode(uint8_t endpoint, uint16_t timeout)
{
  zbClusterId_t pollControlClusterId = {gaZclClusterPollControl_c};
  uint16_t bindingPollTimeout, fastPollTimeoutAttr;
  uint32_t currentTimeOut = 0;  
#if gZclClusterOptionals_d 
  uint16_t fastPollTimeoutMaxAttr;
#endif  
    uint8_t setupIndex = gZclCluster_InvalidDataIndex_d;
    
 #if gEndDevCapability_d || gComboDeviceCapability_d     
    setupIndex = ZCL_PollControl_GetIndexFromPollControlSetupTable(endpoint, gTmrInvalidTimerID_c);
#endif
  
  if(setupIndex == gZclCluster_InvalidDataIndex_d)
    return gZclInvalidValue_c;
    
  /* get attribute values: */
  (void)ZCL_GetAttribute(endpoint, pollControlClusterId, gZclAttrPollControl_FastPollTimeout_c,   gZclServerAttr_c, &fastPollTimeoutAttr,   NULL);
  fastPollTimeoutAttr = OTA2Native16(fastPollTimeoutAttr);
  
  /* updated current values */
  currentTimeOut = (uint32_t)((timeout == 0)? fastPollTimeoutAttr:timeout);
  bindingPollTimeout = (ZclPollControl_GetMaxShortPollIntervalValue())*1000/4; /* seconds  */
  
#if gZclClusterOptionals_d  
  /* get fast poll timeout max attr*/
  (void)ZCL_GetAttribute(endpoint, pollControlClusterId, gZclAttrPollControl_FastPollTimeout_c,   gZclServerAttr_c, &fastPollTimeoutMaxAttr,   NULL);
  fastPollTimeoutMaxAttr = OTA2Native16(fastPollTimeoutMaxAttr);
  /* currentTimeOut should be <= fast poll timeout max */
  if(currentTimeOut >  fastPollTimeoutMaxAttr)
      currentTimeOut = fastPollTimeoutMaxAttr;
#endif
  
  /*check fast poll timeout timer */
  if(gPollControlServerSetup[setupIndex].fastPollTimeoutTmr == gTmrInvalidTimerID_c)
  {
    gPollControlServerSetup[setupIndex].fastPollTimeoutTmr = ZbTMR_AllocateTimer();
    if(gPollControlServerSetup[setupIndex].fastPollTimeoutTmr == gTmrInvalidTimerID_c)
      return gZclNoMem_c;	
  }
  /* start timer - measured in quarter seconds */
  ZbTMR_StartTimer(gPollControlServerSetup[setupIndex].fastPollTimeoutTmr, gTmrSingleShotTimer_c, (currentTimeOut*1000)/4, ZclPollControl_FastPollModeCallback);
  
  /*change poll rate */
  if(bindingPollTimeout)
    (void)ZDO_NLME_ChangePollRate((uint16_t)bindingPollTimeout);
  
  return gZclSuccess_c;
}

/*********************************************************
*    ZclPollControl_GetMaxShortPollIntervalValue        */
uint16_t ZclPollControl_GetMaxShortPollIntervalValue(void)
{
  uint16_t maxShortPollInterval = 0x00;
  for(uint8_t i=0; i<gNoOfPollControlServerInstances_d; i++)
  {
    if(gPollControlServerSetup[i].endpoint != gZclCluster_InvalidDataIndex_d)
    {
      uint16_t shortPollIntervalAttr;  
      zbClusterId_t pollControlClusterId = {gaZclClusterPollControl_c};
      
      (void)ZCL_GetAttribute(gPollControlServerSetup[i].endpoint, pollControlClusterId, gZclAttrPollControl_ShortPollInterval_c,   gZclServerAttr_c, &shortPollIntervalAttr,   NULL);
      shortPollIntervalAttr = OTA2Native16(shortPollIntervalAttr);
      if(maxShortPollInterval < shortPollIntervalAttr)
        maxShortPollInterval = shortPollIntervalAttr;
    }
  }
  return maxShortPollInterval;
}


/*********************************************************
*    ZclPollControl_GetMinLongPollIntervalValue        */
uint32_t ZclPollControl_GetMinLongPollIntervalValue(void)
{
  uint32_t minLongPollInterval = 0xFFFFFFFF;
  for(uint8_t i=0; i<gNoOfPollControlServerInstances_d; i++)
  {
    if(gPollControlServerSetup[i].endpoint != gZclCluster_InvalidDataIndex_d)
    {
      uint32_t longPollIntervalAttr;
      zbClusterId_t pollControlClusterId = {gaZclClusterPollControl_c};
      
      (void)ZCL_GetAttribute(gPollControlServerSetup[i].endpoint, pollControlClusterId, gZclAttrPollControl_LongPollInterval_c,   gZclServerAttr_c, &longPollIntervalAttr,   NULL);
      longPollIntervalAttr = OTA2Native32(longPollIntervalAttr);
      if(longPollIntervalAttr < minLongPollInterval)
        minLongPollInterval = longPollIntervalAttr;
    }
  }
  return minLongPollInterval;
}

/*!
 * @fn 		void ZclPollControl_FastPollModeCallback(uint8_t tmrId)
 *
 * @brief	Callback used to stop fast poll mode
 *
 */
void ZclPollControl_FastPollModeCallback(uint8_t tmrId)
{
  uint8_t setupIndex = gZclCluster_InvalidDataIndex_d;
  
 #if gEndDevCapability_d || gComboDeviceCapability_d     
  setupIndex = ZCL_PollControl_GetIndexFromPollControlSetupTable(gZclCluster_InvalidDataIndex_d, tmrId);
#endif
  if(setupIndex == gZclCluster_InvalidDataIndex_d)
    return;
  
  
  if(gPollControlServerSetup[setupIndex].checkInRspState == gPollControl_CheckInRspStart_c)
  {
    uint8_t i;
    gPollControlServerSetup[setupIndex].checkInRspState = gPollControl_CheckInRspTimeout_c;
    for(i=0; i<gPollControlServerSetup[setupIndex].indexCheckInRsp; i++)
    {
      if(gPollControlServerSetup[setupIndex].currentPollCtrList[i].enableFastPool == TRUE)
      {
        /* start fast polling process- if received an valide response */
        (void)zclPollControl_StartFastPollMode(gPollControlServerSetup[setupIndex].endpoint, ZclPollControl_SearchMaxFastTimeout(setupIndex));
         break;
      }
    }
    gPollControlServerSetup[setupIndex].indexCheckInRsp = 0;

  }
  zclPollControl_StopFastPollMode(gPollControlServerSetup[setupIndex].endpoint, TRUE);
}

/*!
 * @fn 		static uint16_t ZclPollControl_SearchMaxFastTimeout(void)
 *
 * @brief	Get maximum fast timeout interval
 *
 */
static uint16_t ZclPollControl_SearchMaxFastTimeout(uint8_t setupIndex)
{
  uint16_t maxFastPollTimeout = gPollControlServerSetup[setupIndex].currentPollCtrList[0].fastPollTimeout;
  uint8_t i;
  gPollControlServerSetup[setupIndex].curentIndex = 0;

  for(i = 1; i< gPollControlServerSetup[setupIndex].bindingTableEntries;i++)
  {
    if(gPollControlServerSetup[setupIndex].currentPollCtrList[i].enableFastPool == FALSE)
      continue;
    if(maxFastPollTimeout < gPollControlServerSetup[setupIndex].currentPollCtrList[i].fastPollTimeout)
    {
      maxFastPollTimeout = gPollControlServerSetup[setupIndex].currentPollCtrList[i].fastPollTimeout;
      gPollControlServerSetup[setupIndex].curentIndex = i;
    }
  }
  return maxFastPollTimeout;
}

/*!
 * @fn 		void zclPollControl_StopFastPollMode(bool_t timeoutStop)
 *
 * @brief	Stop Fast Poll mode function
 *
 */
void zclPollControl_StopFastPollMode(uint8_t endpoint, bool_t timeoutStop)
{
  uint8_t  i = 0, index = 0;
  uint32_t remainingTime = 0, maxRemainingTimeout = 0, remainingTimeout = 0;
  uint32_t longPollInterval = ZclPollControl_GetMinLongPollIntervalValue();
  uint8_t  setupIndex = gZclCluster_InvalidDataIndex_d;
  
 #if gEndDevCapability_d || gComboDeviceCapability_d     
  setupIndex = ZCL_PollControl_GetIndexFromPollControlSetupTable(endpoint, gTmrInvalidTimerID_c);
#endif
  if(setupIndex == gZclCluster_InvalidDataIndex_d)
    return;
  
  
  ZbTMR_StopTimer(gPollControlServerSetup[setupIndex].fastPollTimeoutTmr);

  if(!timeoutStop)
  {
    /* check that every start request from paired device has been stopped */
    for(i=0; i< gMaximumApsBindingTableEntries_c; i++)
    {
      if((gPollControlServerSetup[setupIndex].currentPollCtrList[i].enableFastPool == FALSE)||(i == gPollControlServerSetup[setupIndex].curentIndex))
        continue;
      /* get remaining Time - miliseconds */
      remainingTime = TMR_GetRemainingTime(gPollControlServerSetup[setupIndex].fastPollTimeoutTmr); 
      if(gPollControlServerSetup[setupIndex].currentPollCtrList[i].fastPollTimeout*1000/4 > 
         gPollControlServerSetup[setupIndex].currentPollCtrList[gPollControlServerSetup[setupIndex].curentIndex].fastPollTimeout*1000/4 - remainingTime)
      {
          remainingTimeout = (gPollControlServerSetup[setupIndex].currentPollCtrList[i].fastPollTimeout*1000/4 - \
            (gPollControlServerSetup[setupIndex].currentPollCtrList[gPollControlServerSetup[setupIndex].curentIndex].fastPollTimeout*1000/4 - remainingTime));
          if(remainingTimeout >= maxRemainingTimeout)
          {
            maxRemainingTimeout = remainingTimeout;
            index = i;
          }
      }
    }    
  }
  
  if(maxRemainingTimeout)
  {
    gPollControlServerSetup[setupIndex].curentIndex = index;
    ZbTMR_StartTimer(gPollControlServerSetup[setupIndex].fastPollTimeoutTmr, gTmrSingleShotTimer_c, maxRemainingTimeout, ZclPollControl_FastPollModeCallback);
    return;
  }
  /* free fast poll timeout tmr */
  ZbTMR_FreeTimer(gPollControlServerSetup[setupIndex].fastPollTimeoutTmr);
  gPollControlServerSetup[setupIndex].fastPollTimeoutTmr = gTmrInvalidTimerID_c;
  
  /* restore to the last poll interval -- LongPollInterval */
  (void)ZDO_NLME_ChangePollRate((uint16_t)(longPollInterval*(1000/4)));
}
/*!
 * @fn 		void ZclPollControl_WriteAttribute(void *pData)
 *
 * @brief	Poll control write attributes function
 *
 */
void ZclPollControl_WriteAttribute(uint8_t endpoint, void *pData)
{  
  zclCmdWriteAttrRecord_t *pRecord = (zclCmdWriteAttrRecord_t*) pData;  
  zbClusterId_t pollControlClusterId = {gaZclClusterPollControl_c};
  uint8_t setupIndex = gZclCluster_InvalidDataIndex_d;
  
  setupIndex = ZCL_PollControl_GetIndexFromPollControlSetupTable(endpoint, gTmrInvalidTimerID_c);
  if(setupIndex == gZclCluster_InvalidDataIndex_d)
    return;
  
  switch (pRecord->attrId)
  {        
    case gZclAttrPollControl_CheckInInterval_c:
      {
        uint32_t attrData;
         FLib_MemCpy(&attrData, pRecord->aData, sizeof(attrData));    
         if(attrData > 0x00) 
            ZbTMR_StartTimer(gPollControlServerSetup[setupIndex].checkInIntervalTmr, gTmrSingleShotTimer_c, (attrData*1000)/4, ZclPollControl_CheckInCallback);      
         else
            ZbTMR_StopTimer(gPollControlServerSetup[setupIndex].checkInIntervalTmr);    
         
         (void)ZCL_SetAttribute(gPollControlServerSetup[setupIndex].endpoint, pollControlClusterId, gZclAttrPollControl_CheckInInterval_c, gZclServerAttr_c, &attrData);      
         break;
      }
    case gZclAttrPollControl_FastPollTimeout_c:
      {
         uint16_t attrData;         
         FLib_MemCpy(&attrData, &pRecord->aData, sizeof(uint16_t));
        (void)ZCL_SetAttribute(gPollControlServerSetup[setupIndex].endpoint, pollControlClusterId, gZclAttrPollControl_FastPollTimeout_c, gZclServerAttr_c, &attrData);      

         break;
      }
    default:
       break;
  }
} 

/*!
 * @fn 		bool_t  ZclPollControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Poll control validate attributes function
 *
 */
bool_t  ZclPollControlValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
{  
  zclCmdWriteAttrRecord_t *pRecord = (zclCmdWriteAttrRecord_t*) pData;  
 
  switch (pRecord->attrId)
  {        
      case gZclAttrPollControl_CheckInInterval_c:
      {
        uint32_t attrData;
        uint32_t longPollIntervalAttr;
#if gZclClusterOptionals_d   
        uint32_t checkInIntervalMinAttr;
#endif       
          /* get current attribute values */  
        (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrPollControl_LongPollInterval_c, gZclServerAttr_c, &longPollIntervalAttr,   NULL);
        longPollIntervalAttr = OTA2Native32(longPollIntervalAttr);
#if gZclClusterOptionals_d        
        (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrPollControl_CheckInIntervalMin_c,  gZclServerAttr_c, &checkInIntervalMinAttr,   NULL);
         checkInIntervalMinAttr =  OTA2Native32(checkInIntervalMinAttr);
#endif      
         
        FLib_MemCpy(&attrData, pRecord->aData, sizeof(attrData));    
        attrData = OTA2Native32(attrData);
        if((attrData < longPollIntervalAttr) && (attrData!=0x00)) 	
           return FALSE; 
#if gZclClusterOptionals_d  
        if((attrData < checkInIntervalMinAttr) && (attrData!=0x00)) 
           return FALSE; 
#endif   
         			
      }
  default:
    return TRUE;
  }
} 

/*!
 * @fn 		static bool_t ZclPollControlValidateFastPollTimeout(uint8_t setupIndex, uint16_t value)
 *
 * @brief	Validate fast poll timeout
 *
 */
static bool_t ZclPollControlValidateFastPollTimeout(uint8_t setupIndex, uint16_t value)
{     
  uint16_t fastPollTimeoutMax = gZclPollControl_FastPollTimeoutMax_Test_c;
#if gZclClusterOptionals_d 
  zbClusterId_t pollControlClusterId = {gaZclClusterPollControl_c};
  (void)ZCL_GetAttribute(gPollControlServerSetup[setupIndex].endpoint, pollControlClusterId, gZclAttrPollControl_FastPollTimeoutMax_c,   gZclServerAttr_c, &fastPollTimeoutMax,   NULL);
  fastPollTimeoutMax = OTA2Native16(fastPollTimeoutMax);  
#endif  
  
  if((value < gZclPollControl_MinShortPollInterval_c) ||
     (value > fastPollTimeoutMax)&&(fastPollTimeoutMax > 0))
  {
    return FALSE;
  }
  return TRUE;                 
}
#endif

/*!
 * @fn 		zbStatus_t  ztcPollControl_SetClientInf(zclPollControlClientSetup_t * setClientInf)
 *
 * @brief	Set client poll control information
 *
 */
zbStatus_t ztcPollControl_SetClientInf 
(
    zclPollControlClientSetup_t * setClientInf
)
{
  gPollControlClientSetup.enableFastPooling = setClientInf->enableFastPooling;
  gPollControlClientSetup.fastPollTimeout  = setClientInf->fastPollTimeout;
  return gZclSuccess_c;
}



#endif /* gZclEnablePollControlCluster_d */
/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		zbStatus_t ZCL_ReplyNoCopy(zbApsdeDataIndication_t *pIndication, uint8_t payloadLen, afToApsdeMessage_t *pMsg )
 *
 * @brief	Standard request used for longer replies that must be built in-place.
 *
 */
zbStatus_t ZCL_ReplyNoCopy
  (
  zbApsdeDataIndication_t *pIndication, /* IN: indication that came from other node */
  uint8_t payloadLen,       /* IN: payload length, not counting ZCL frame (3-byte hdr)  */
  afToApsdeMessage_t *pMsg  /* IN: message, with payload at gAsduOffset_c + sizeof(zclFrame_t) */
  )
{
  afAddrInfo_t addrInfo;
  uint8_t frameControl;

  /* get address ready for reply */
  AF_PrepareForReply(&addrInfo, pIndication);
  addrInfo.radiusCounter = gDefaultRadiusCounter;
  
  frameControl = ((zclFrame_t *)pIndication->pAsdu)->frameControl;
  
  /* setup the reply frame */
  ZCL_SetupFrame((void *)(((uint8_t *)pMsg) + gAsduOffset_c), (void *)pIndication->pAsdu);
  
  if (frameControl & gZclFrameControl_MfgSpecific)
    payloadLen += sizeof(zclMfgFrame_t);
  else
    payloadLen += sizeof(zclFrame_t);

  /* send it over the air */
  return ZCL_DataRequestNoCopy(&addrInfo, payloadLen, pMsg);
}

/*!
 * @fn 		zbStatus_t ZCL_Reply(zbApsdeDataIndication_t *pIndication, uint8_t payloadLen,   void *pPayload  )
 *
 * @brief	Standard request used for short replies (that do not need to be built in-place)
 *
 */
zbStatus_t ZCL_Reply
  (
  zbApsdeDataIndication_t *pIndication,   /* IN: */
  uint8_t payloadLen,                     /* IN: */
  void *pPayload                          /* IN: */
  )
{
  afToApsdeMessage_t *pMsg;

  /* allocate space for the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return gZclNoMem_c;

  /* copy in payload */
  FLib_MemCpy(((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t),
    pPayload, payloadLen);

  /* send it over the air */
  return ZCL_ReplyNoCopy(pIndication, payloadLen, pMsg);
}

/*!
 * @fn 		uint8_t *ZCL_InterPanCreatePayload(zclCmd_t command,zclFrameControl_t frameControl, uint8_t *pPayloadLen,  uint8_t *pPayload)
 *
 * @brief	Inter Pan Create Payload function
 *
 */
uint8_t *ZCL_InterPanCreatePayload
  (
  zclCmd_t command,               /* IN: command */
  zclFrameControl_t frameControl, /* IN: frame control field */
  uint8_t *pPayloadLen,           /* IN/OUT: length of payload (then adjusted to length of asdu) */
  uint8_t *pPayload                  /* IN: payload after frame (first byte from pPayload is transactionID)*/
  )
{
  uint8_t *pMsg;
  zclFrame_t *pFrame;
  *pPayloadLen += sizeof(zclFrame_t);
  /* allocate space for the message */
  pMsg = MSG_Alloc(*pPayloadLen);
  if(!pMsg)
    return NULL;
  BeeUtilZeroMemory(pMsg, *pPayloadLen);
  /* set up the frame */
  pFrame = (zclFrame_t *)pMsg;
  pFrame->frameControl = frameControl;
  pFrame->transactionId = pPayload[0];
  pFrame->command = command;
  if(pPayload && (*pPayloadLen))
    /* copy the payload, skip over first byte which is the zcl transaction ID*/
    FLib_MemCpy((pFrame + 1), (pPayload+1), (*pPayloadLen - sizeof(zclFrame_t)));
  /* return ptr to msg buffer */
  return pMsg;
}

/*!
 * @fn 		zclStatus_t ZCL_GenericReq(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
 *
 * @brief	This request works for many of the request types that don't need to parse the data before sending.
 *			Returns the gHaNoMem_c if not enough memory to send.
 *
 */
zclStatus_t ZCL_GenericReq(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),command,gZclFrameControl_FrameTypeSpecific,
    NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/*!
 * @fn 		zclStatus_t ZCL_SendClientReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
 *
 * @brief	Send Req from Client to Server with Zcl Transaction Sequence Id passed as parameter
 *
 */
zclStatus_t ZCL_SendClientReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), command, gZclFrameControl_FrameTypeSpecific,
         &(pReq->cmdFrame),&iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/*!
 * @fn 		zclStatus_t ZCL_SendInterPanClientReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void *pReq)
 *
 * @brief	Send InterPan Req from Client to Server with Zcl Transaction Sequence Id passed as parameter 
 *
 */
zclStatus_t ZCL_SendInterPanClientReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void *pReq)
{
 
  uint8_t *pPayload;
  zclStatus_t status;
  
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, gZclFrameControl_FrameTypeSpecific ,
                                       &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  
  /* send packet over the air */
  status = AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
  
  /* Free the buffer allocated for the payload*/
  if (pPayload)
    MSG_Free(pPayload);
    
  return status;
}
/*!
 * @fn 		zclStatus_t ZCL_SendServerReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
 *
 * @brief	Send Req From Server to Client with Zcl Transaction Sequence Id passed as parameter
 *
 */
zclStatus_t ZCL_SendServerReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp),
          &(pReq->cmdFrame), &iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;
  
  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}
/*!
 * @fn 		zclStatus_t ZCL_SendInterPanServerReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void *pReq)
 *
 * @brief	Send InterPan Req from Server to Client with Zcl Transaction Sequence Id passed as parameter
 *
 */
zclStatus_t ZCL_SendInterPanServerReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void *pReq)
{
 
  uint8_t *pPayload;
  zclStatus_t status;
  
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp) ,
                                       &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  /* send packet over the air */
  status = AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
  
  /* Free the buffer allocated for the payload*/
  if (pPayload)
    MSG_Free(pPayload);
    
  return status;
    
}
/*!
 * @fn 		zclStatus_t ZCL_SendServerRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
 *
 * @brief	Send Rsp From Server To Client with Zcl Transaction Sequence Id passed as parameter
 *
 */
zclStatus_t ZCL_SendServerRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp |gZclFrameControl_DisableDefaultRsp) ,
    &(pReq->cmdFrame), &iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/*!
 * @fn 		zclStatus_t ZCL_SendInterPanServerRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void*pReq)
 *
 * @brief	Send InterPan Rsp From Server To Client with Zcl Transaction Sequence Id passed as parameter 
 *
 */
zclStatus_t ZCL_SendInterPanServerRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void*pReq)
{
  uint8_t *pPayload;
  zclStatus_t status;
  
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp |gZclFrameControl_DisableDefaultRsp) ,
        &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  /* send packet over the air */
  status = AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
  
  /* Free the buffer allocated for the payload*/
  if (pPayload)
    MSG_Free(pPayload);
    
  return status;
}

/*!
 * @fn 		zclStatus_t ZCL_SendClientRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
 *
 * @brief	Send Rsp From Client to Server with Zcl Transaction Sequence Id passed as parameter 
 *
 */
zclStatus_t ZCL_SendClientRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;
  
  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), command,(gZclFrameControl_FrameTypeSpecific |gZclFrameControl_DisableDefaultRsp),
          &(pReq->cmdFrame), &iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/*!
 * @fn 		zclStatus_t ZCL_SendInterPanClientRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void*pReq)
 *
 * @brief	Send InterPan Rsp From Client To Server with Zcl Transaction Sequence Id passed as paramete
 *
 */
zclStatus_t ZCL_SendInterPanClientRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void*pReq)
{
  uint8_t *pPayload;
  zclStatus_t status;
  
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, (gZclFrameControl_FrameTypeSpecific |gZclFrameControl_DisableDefaultRsp) ,
        &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  /* send packet over the air */
  status = AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
  
  /* Free the buffer allocated for the payload*/
  if (pPayload)
    MSG_Free(pPayload);
    
  return status;
}


/*****************************************************************************/
#if gZclClusterOptionals_d
/*!
 * @fn 		zbStatus_t ZCL_SetValueAttr( zclSetAttrValue_t *pSetAttrValueReq)
 *
 * @brief	Local ZTC function used to set the value for some attributes.
 *
 */
zbStatus_t ZCL_SetValueAttr 
  (
  zclSetAttrValue_t *pSetAttrValueReq
  )
{
  afDeviceDef_t  *pDeviceDef;
  afClusterDef_t *pClusterDef;
  zclAttrDef_t   *pAttrDef;
  zbClusterId_t  aClusterId;
  
#if gZclEnableThermostat_c || (gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d)
  uint8_t alarmCode = 0;
#endif

#if gZclEnableThermostat_c
  uint8_t alarmMask = 0;
  uint8_t valueAttr = 0;
#endif  
  
#if (gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d)
  uint16_t alarmMask = 0;
  uint16_t valueAttr = 0;
#endif  
  
  
#if gZclEnableOccupancySensor_c    
  uint8_t sensorType, thresholdOccupancy, occupancyState;
  uint16_t delayOccupancy;
#endif  
  
  /* does the endpoint exist? (and have a device definition?) */
  pDeviceDef = AF_GetEndPointDevice(pSetAttrValueReq->ep);
  if(!pDeviceDef)
    return gZclUnsupportedAttribute_c;

  /* does the cluster exist on this endpoint? */
  pClusterDef = ZCL_FindCluster(pDeviceDef, pSetAttrValueReq->clusterId);
  if(!pClusterDef)
    return gZclUnsupportedAttribute_c;

  /* does the attribute exist? */
  pAttrDef = ZCL_FindAttr(pClusterDef, pSetAttrValueReq->attrID, pSetAttrValueReq->direction);
  if(!pAttrDef)
   return gZclUnsupportedAttribute_c;
  
#if gZclEnableBinaryInput_c
 /*used only for Binary input Cluster*/ 
   Set2Bytes(aClusterId, gZclClusterBinaryInput_c);
   if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
      if((pSetAttrValueReq->attrID == gZclAttrBinaryInputStatusFlags_c)||(pSetAttrValueReq->attrID == gZclAttrBinaryInputApplicationType_c))
        (void)ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, gZclServerAttr_c, &(pSetAttrValueReq->valueAttr)); 
      BeeAppUpdateDevice(pSetAttrValueReq->ep, gZclUI_ChangeNotification, 0, aClusterId, NULL);
      return gZbSuccess_c;
  } 
#endif
   
#if gASL_ZclIASZoneReq_d
  Set2Bytes(aClusterId, gZclClusterIASZone_c);
  if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
      if(pSetAttrValueReq->attrID == gZclAttrZoneInformationZoneStatus_c)
      {
        (void)ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrZoneInformationZoneStatus_c, gZclServerAttr_c, &(pSetAttrValueReq->valueAttr));
        BeeAppUpdateDevice(pSetAttrValueReq->ep, gZclUI_ChangeNotification, 0, 0, NULL);
      }
       return gZbSuccess_c;
  }
#endif

#if gZclEnableOccupancySensor_c  
  Set2Bytes(aClusterId, gZclClusterOccupancy_c);
  if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
    if(pSetAttrValueReq->attrID == gZclAttrOccupancySensing_OccupancyId_c){  /* attribute = occupancy */
      (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_OccupancySensorTypeId_c, gZclServerAttr_c, &sensorType, NULL);
      (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_OccupancyId_c, gZclServerAttr_c, &occupancyState, NULL);
      if(pSetAttrValueReq->valueAttr[0] != occupancyState) //(size for occupancyState attribute = 1 byte)
      {
        gSetAvailableOccupancy = FALSE;
        occupancyTimer = ZbTMR_AllocateTimer();
        if(sensorType == gZclTypeofSensor_PIR_c)
        {        
          (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedThresholdId_c, gZclServerAttr_c, &thresholdOccupancy, NULL);
          if(occupancyState == 1) /*occupied*/
            (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_PIROccupiedToUnoccupiedDelayId_c, gZclServerAttr_c, &delayOccupancy, NULL);
          else /*unoccupied*/
            (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedDelayId_c, gZclServerAttr_c, &delayOccupancy, NULL);      
        }
        else
          if(sensorType == gZclTypeofSensor_Ultrasonic_c)
          {
            (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedThresholdId_c, gZclServerAttr_c, &thresholdOccupancy, NULL);
             if(occupancyState == 1) /*occupied*/
              (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_UltrasonicOccupiedToUnoccupiedDelayId_c, gZclServerAttr_c, &delayOccupancy, NULL);
            else /*unoccupied*/
              (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedDelayId_c, gZclServerAttr_c, &delayOccupancy, NULL);      
        }
	delayOccupancy = OTA2Native16(delayOccupancy);
        ZbTMR_StartSecondTimer( pSetAttrValueReq->ep, delayOccupancy, TmrOccupancyCallBack);       
      }
       return gZbSuccess_c;
    }
    else
      if(pSetAttrValueReq->attrID == gZclAttrOccupancySensing_OccupancySensorTypeId_c) 
      (void)ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, gZclServerAttr_c, &(pSetAttrValueReq->valueAttr));
     return gZbSuccess_c;
  }
#endif
  
#if gZclEnableThermostat_c || (gASL_ZclMet_Optionals_d && gZclEnableMeteringServer_d)  
#if gZclEnableThermostat_c  
  Set2Bytes(aClusterId, gZclClusterThermostat_c);
#else
  Set2Bytes(aClusterId, gZclClusterSmplMet_c); 
#endif
  if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
#if gZclEnableThermostat_c      
    if(pSetAttrValueReq->attrID == gZclAttrThermostat_AlarmMaskId_c){
      valueAttr = pSetAttrValueReq->valueAttr[0];
#else
    if(pSetAttrValueReq->attrID == gZclAttrMetASGenericAlarmMask_c){  
      FLib_MemCpy(&valueAttr, &pSetAttrValueReq->valueAttr, pSetAttrValueReq->attrSize);
#endif          
      (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, gZclServerAttr_c, &alarmMask, NULL);   
      //if it is the same value - can't detect the correct code alarm 
      if((valueAttr != 0)&&(valueAttr != alarmMask))
      {
          alarmCode = (uint8_t)(valueAttr & (~alarmMask)); //sizeof(alarm code) = 1Byte
          if(alarmCode != 0x00)
          {
            alarmCode--;
            BeeAppUpdateDevice(pSetAttrValueReq->ep, gZclUI_AlarmGenerate_c, 0, aClusterId, &alarmCode);
          }
      }
      return ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, gZclServerAttr_c, &valueAttr);      
     }
  } 
#endif
#if gZclEnablePollControlCluster_d
    Set2Bytes(aClusterId, gZclClusterPollControl_c);
    if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE)
    {
        if(pSetAttrValueReq->attrID == gZclAttrPollControl_LongPollInterval_c)
        {
          uint32_t longPollInterval = *((uint32_t *)pSetAttrValueReq->valueAttr);
          ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, gZclServerAttr_c, &longPollInterval);  
          (void)ZDO_NLME_ChangePollRate((uint16_t)(Native2OTA32(longPollInterval)*1000/4));
          return gZbSuccess_c;
         
        }
    }   
    
#endif    
    
   Copy2Bytes(aClusterId, pSetAttrValueReq->clusterId);
   return ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, pSetAttrValueReq->direction, &(pSetAttrValueReq->valueAttr));    
}

/*!
 * @fn 		zbStatus_t ZCL_ProcessUnsolicitedCommand(zclProcessUnsolicitedCommand_t *pProcessUnsolicitedCommand)  
 *
 * @brief	Local ZTC function that manage some unsolicited commands.
 *
 */
zbStatus_t ZCL_ProcessUnsolicitedCommand(zclProcessUnsolicitedCommand_t *pProcessUnsolicitedCommand)  
{
  afDeviceDef_t  *pDeviceDef;
  afClusterDef_t *pClusterDef;
  zbClusterId_t  aClusterId;
  
  /* does the endpoint exist? (and have a device definition?) */
  pDeviceDef = AF_GetEndPointDevice(pProcessUnsolicitedCommand->endpoint);
  if(!pDeviceDef)
    return gZclNotFound_c;

  /* does the cluster exist on this endpoint? */
  pClusterDef = ZCL_FindCluster(pDeviceDef, pProcessUnsolicitedCommand->clusterId);
  if(!pClusterDef)
    return gZclNotFound_c;
  
#if gZclEnableApplianceStatistics_d  && gZclEnableApplianceStatisticsServerOptionals_d
  Set2Bytes(aClusterId, gZclClusterApplianceStatistics_c);
  if(FLib_Cmp2Bytes(pProcessUnsolicitedCommand->clusterId, aClusterId) == TRUE)
  {
    return Zcl_ApplianceStatistics_UnsolicitedCommandHandler(pProcessUnsolicitedCommand->commandId, pProcessUnsolicitedCommand->data);
  }
#endif  
  
#if gZclEnableApplianceEventsAlerts_d &&  gZclEnableApplianceEventsAlertsUnsolicitedCmd_d
  Set2Bytes(aClusterId, gZclClusterApplianceEventsAlerts_c);
  if(FLib_Cmp2Bytes(pProcessUnsolicitedCommand->clusterId, aClusterId) == TRUE)
  {
    return Zcl_ApplianceEventsAlerts_UnsolicitedCommandHandler( pProcessUnsolicitedCommand->commandId, 
                 pProcessUnsolicitedCommand->state,  pProcessUnsolicitedCommand->data);
  }
#endif  
  
#if gZclEnablePwrProfileClusterServer_d  
  Set2Bytes(aClusterId, gZclClusterPowerProfile_c);
  if(FLib_Cmp2Bytes(pProcessUnsolicitedCommand->clusterId, aClusterId) == TRUE)
  {
    return Zcl_PowerProfile_UnsolicitedCommandHandler(pProcessUnsolicitedCommand->commandId, pProcessUnsolicitedCommand->data);   
  }
#endif  
  
  (void)aClusterId;
  return gZbFailed_c;
}

#if gZclEnableOccupancySensor_c  
/*!
 * @fn 		void TmrOccupancyCallBack(zbTmrTimerID_t tmrid)
 *
 * @brief	Occupancy Cluster Callback
 *
 */
void TmrOccupancyCallBack(zbTmrTimerID_t tmrid)
{
  zbClusterId_t   aClusterId = {gaZclClusterOccupancySensor_c};
  uint8_t  occupancyState = 0;
  (void) tmrid;
  gSetAvailableOccupancy = TRUE;
  (void)ZCL_GetAttribute(0x08, aClusterId, gZclAttrOccupancySensing_OccupancyId_c, gZclServerAttr_c, &occupancyState, NULL);
  occupancyState = (occupancyState == 0x00)?0x01:0x00; 
  (void)ZCL_SetAttribute(0x08, aClusterId, gZclAttrOccupancySensing_OccupancyId_c, gZclServerAttr_c, &occupancyState);
  ZbTMR_FreeTimer(occupancyTimer);
}
#endif //gZclEnableOccupancySensor_c
#endif //gZclClusterOptionals_d

/*!
 * @fn 		bool_t InterpretMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint)
 *
 * @brief	This fucntion return TRUE only if the MatchDescriptor Req was generated by a OTA Cluster command
 *
 */
bool_t InterpretMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint)
{
  bool_t status = FALSE;
#if (gZclEnableOTAClient_d)  
  status = OtaClient_ServerDiscoveryProcessMatchDesc(aDestAddress, endPoint);
#endif
  return status;  
}

/*!
 * @fn 		void ZCL_SaveNvmZclData(void)
 *
 * @brief	General function used to save NVM ZCL data
 *
 */
void ZCL_SaveNvmZclData(void)
{

  uint8_t index = 0;
  NVM_DataEntry_t  zcl_DataTable[] =
  {
    gAPP_DATA_SET_FOR_NVM,
    {NULL,0,0}  /* Required end-of-table marker. */
  };
  
  while(zcl_DataTable[index].pData)
  {
    if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      (void)NvSaveOnIdle(zcl_DataTable[index].pData, TRUE); 
    else
      NvSaveOnInterval(zcl_DataTable[index].pData);
    index++;
  }
}

/*!
 * @fn 		zbEndPoint_t ZCL_GetEndPointForSpecificCluster(zbClusterId_t clusterId, bool_t isClusterServer)
 *
 * @brief	 GetEndPoint for a specific cluster based on Endpoint List informations
 *
 */
zbEndPoint_t ZCL_GetEndPointForSpecificCluster(zbClusterId_t clusterId, bool_t isClusterServer, uint8_t startIndex, uint8_t *pNextStartIndex)
{
#if gNum_EndPoints_c != 0     

  uint8_t i, j;
  #if gInstantiableStackEnabled_d 
  for(i=startIndex;i<EndPointConfigData(gNum_EndPoints);i++)
  #else  
  for(i=startIndex;i<gNum_EndPoints_c;i++)
  #endif
  {
   if(pNextStartIndex)
      *pNextStartIndex = i+1; 
   
   if(isClusterServer) 
   {
    for(j=0;j<EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->appNumInClusters);j++)
      if(IsEqual2Bytes(clusterId, &EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->pAppInClusterList[j*2])))
      {
        return EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint);
      }
   }
   else
   {
     for(j=0;j<EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->appNumOutClusters);j++)
      if(IsEqual2Bytes(clusterId, &EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->pAppOutClusterList[j*2])))
      {
        return EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint);
      }
   }
     
  }
#endif  
  if(pNextStartIndex)
    *pNextStartIndex = gZclCluster_InvalidDataIndex_d;
  return gZclCluster_InvalidDataIndex_d;
}

#if gInterPanCommunicationEnabled_c
/*!
 * @fn 		void ZCL_RegisterInterPanClient(pfnInterPanIndication_t pFunc)
 *
 * @brief	Register the InterPAN Client function.
 *
 */
void ZCL_RegisterInterPanClient(pfnInterPanIndication_t pFunc)
{
  pfnInterPanClientInd = pFunc;
}

/*!
 * @fn 		void ZCL_RegisterInterPanServer(pfnInterPanIndication_t pFunc)
 *
 * @brief	Register the InterPAN Server function.
 *
 */
void ZCL_RegisterInterPanServer(pfnInterPanIndication_t pFunc)
{
  pfnInterPanServerInd = pFunc;
}

/*!
 * @fn 		zbStatus_t ZCL_InterPanClusterServer(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Handle ALL Inter Pan Server Indications (filter after cluster ID).
 *
 */
zbStatus_t ZCL_InterPanClusterServer
(
  zbInterPanDataIndication_t *pIndication, /* IN: */
  afDeviceDef_t *pDev                /* IN: */
) 
{
  (void) pIndication;
  (void) pDev;
  
#if(gASL_ZclPrice_InterPanPublishPriceRsp_d )    
  if(IsEqual2BytesInt(pIndication->aClusterId, gZclClusterPrice_c))
    return ZCL_InterPanPriceClusterServer(pIndication, pDev); 
#endif

  if(IsEqual2BytesInt(pIndication->aClusterId, gZclClusterMsg_c))
    return ZCL_InterPanMsgClusterServer(pIndication, pDev); 
  
#if gASL_EnableZllTouchlinkCommissioning_d 
  if(IsEqual2BytesInt(pIndication->aClusterId, gZclClusterZllCommissioning_c))
    return ZCL_ZllTouchlinkClusterServer(pIndication, pDev); 
#endif
  
  /* if user uses Inter Pan with other cluster, add the Cluster ID filter here*/ 
 return gZclUnsupportedClusterCommand_c;
}

/*!
 * @fn 		zbStatus_t ZCL_InterPanClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Handle ALL Inter Pan Client Indications (filter after cluster ID).
 *
 */
zbStatus_t ZCL_InterPanClusterClient
(
  zbInterPanDataIndication_t *pIndication, /* IN: */
  afDeviceDef_t *pDev                /* IN: */
) 
{
  (void) pIndication;
  (void) pDev;

#if(gASL_ZclPrice_InterPanGetCurrPriceReq_d )  
  if(IsEqual2BytesInt(pIndication->aClusterId, gZclClusterPrice_c))
    return ZCL_InterPanPriceClusterClient(pIndication, pDev); 
#endif
  
  if(IsEqual2BytesInt(pIndication->aClusterId, gZclClusterMsg_c))
    return ZCL_InterPanMsgClusterClient(pIndication, pDev); 

#if gASL_EnableZllTouchlinkCommissioning_d  
  if(IsEqual2BytesInt(pIndication->aClusterId, gZclClusterZllCommissioning_c))
    return ZCL_ZllTouchlinkClusterClient(pIndication, pDev); 
#endif
  
  /* if user uses Inter Pan with other cluster, add the Cluster ID filter here*/
  
  return gZclUnsupportedClusterCommand_c;
}
/*!
 * @fn 		void PrepareInterPanForReply(InterPanAddrInfo_t *pAdrrDest, zbInterPanDataIndication_t *pIndication)
 *
 * @brief	Gets address ready for InterPan reply.
 *
 */
void PrepareInterPanForReply(InterPanAddrInfo_t *pAdrrDest, zbInterPanDataIndication_t *pIndication)
{
  
  pAdrrDest->srcAddrMode = pIndication->destAddrMode;
  pAdrrDest->dstAddrMode = pIndication->srcAddrMode;
  FLib_MemCpy(pAdrrDest->dstPanId, pIndication->srcPanId, sizeof(zbPanId_t));
  FLib_MemCpy(pAdrrDest->dstAddr.aIeeeAddr, pIndication->aSrcAddr.aIeeeAddr, sizeof(zbIeeeAddr_t));
  FLib_MemCpy(pAdrrDest->aProfileId, pIndication->aProfileId, sizeof(zbProfileId_t));
  FLib_MemCpy(pAdrrDest->aClusterId, pIndication->aClusterId, sizeof(zbClusterId_t));
}
#endif /* #if gInterPanCommunicationEnabled_c */
