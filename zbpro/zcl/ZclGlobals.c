/******************************************************************************
* ZclGlobals.c
*
* Copyright (c) 2013, Freescale, Inc. All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*******************************************************************************/
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"

#include "HaProfile.h"
#include "EndpointConfig.h"


/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/
#define gBasicManufacturerName_c        {0x09, "Freescale       "}


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

/* Default ZCL Attribute Values */

/******************************************************************************
* Basic Cluster
******************************************************************************/
const zclBasicAttrsRAM_t gDefaultBasicAttrData = 
{
  gZclAttrBasic_ZclVersion_c,                                           /* zcl version */
  gZclAttrBasic_PowerSource_c,                                          /* powerSource */
  gZclDeviceEnabled_c,                                                  /* device Enabled */
#if gZclClusterOptionals_d  
  gZclAttrBasic_ApplicationVersion_c,                                   /* Application version */
  gZclAttrBasic_StackVersion_c,                                         /* Stack Version */
  gZclAttrBasic_HWVersion_c,                                            /* Hardware Version */
  {sizeof(gszZclAttrBasic_MfgName_c)-1, gszZclAttrBasic_MfgName_c},     /* manufacturer Name */
  {sizeof(gszZclAttrBasic_Model_c)-1, gszZclAttrBasic_Model_c},         /* model */
  {sizeof(gszZclAttrBasic_DateCode_c)-1, gszZclAttrBasic_DateCode_c},   /* dateCode*/ 
  gBasicManufacturerName_c,                                             /* location Description */        
  gZclAttrBasic_PhysicalEnvironment,                                    /* physicalEnvironment */
  0x00                                                                  /* alarmMask*/
#endif
#if gASL_EnableZLLClustersData_d
  ,{sizeof(gszZclAttrBasic_SwBuildId_c)-1, gszZclAttrBasic_SwBuildId_c},   /* dateCode*/  
#endif    
};


/******************************************************************************
* Power Configuration Cluster
******************************************************************************/
const zclPowerCfgAttrsRAM_t gDefaultPowerCfgAttrData =
{
  0x0000,                                                               /*gZclAttrPwrConfigMainsInfMainsVoltage_c*/
  0x00,                                                                 /*gZclAttrPwrConfigMainsInfMainsFrequency_c*/
  0x00,                                                                 /*gZclAttrPwrConfigMainsStgMainsAlarmMask_c*/
  0x0000,                                                               /*gZclAttrPwrConfigMainsStgMainsVoltageMinThreshold_c*/
  0x0000,                                                               /*gZclAttrPwrConfigMainsStgMainsVoltageMaxThreshold_c*/
  0x0000,                                                               /*gZclAttrPwrConfigMainsStgMainsVoltageDwellTripPoint_c*/
  {0x00, 0x00, 0x00},                                                   /*gZclAttrPwrConfigBatteryInfBatteryVoltage_c*/
  0x00,                                                                 /*gZclAttrPwrConfigBatteryInfBatteryPercentageRemaining_c */
  {sizeof(gszZclAttrBasic_MfgName_c)-1, gszZclAttrBasic_MfgName_c},     /*gZclAttrPwrConfigBatteryStgBatteryManufacturer_c*/
  0x00,                                                                 /*gZclAttrPwrConfigBatteryStgBatterySize_c*/
  0x0000,                                                               /*gZclAttrPwrConfigBatteryStgBatteryAHrRating_c*/ 
  0x00,                                                                 /*gZclAttrPwrConfigBatteryStgBatteryQuantity_c*/
  0x00,                                                                 /*gZclAttrPwrConfigBatteryStgBatteryRatedVoltage_c*/
  0x00,                                                                 /*gZclAttrPwrConfigBatteryStgBatteryAlarmMask_c*/
  0x00,                                                                 /*gZclAttrPwrConfigBatteryStgBatteryVoltageMinThreshold_c*/
  0x00,                                                                 /*gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold1_c*/
  0x00,                                                                 /*gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold2_c*/
  0x00,                                                                 /*gZclAttrPwrConfigIdBatteryStgBatteryVoltageThreshold3_c*/
  0x00,                                                                 /*gZclAttrPwrConfigIdBatteryStgBatteryPercentageMinThreshold_c*/
  0x00,                                                                 /*gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold1_c*/
  0x00,                                                                 /*gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold2_c*/
  0x00,                                                                 /*gZclAttrPwrConfigIdBatteryStgBatteryPercentageThreshold3_c*/
  0x00                                                                  /*gZclAttrPwrConfigIdBatteryStgBatteryAlarmState_c*/       
};

/******************************************************************************
* Identify Cluster
******************************************************************************/
const zclIdentifyAttrsRAM_t gDefaultIdentifyAttrData = 
{
  gZclIdentifyAttr_IdentifyOff_d                        /* Identify Time */
#if gASL_EnableEZCommissioning_d    
  ,gZclCommissioningState_NotInNetwork_d                /* IdentifyCommissioningState */
#endif    
};


/******************************************************************************
* Group Cluster
******************************************************************************/
const zclGroupAttrsRAM_t gDefaultGroupAttrData =
{
   gZclGroup_NameSupport_c                              /* Group Name Support */
};

/******************************************************************************
* Scene Cluster
******************************************************************************/
const zclSceneAttrs_t gDefaultSceneAttrData =
{
    0x00,                                               /* scene Count*/
    0x00, 0x00,                                         /* current group */
    0x00,                                               /* current scene */
    0x00,                                               /* is this currently a valid scene? */
#if gZclClusterOptionals_d
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,     /* IEEE address of the device that last configured the scenes */
#endif
    0x00                                                /* size of an entry in the scenes table */
};

/******************************************************************************
* OnOff Cluster
******************************************************************************/
const zclOnOffAttrsRAM_t gDefaultOnOffAttrData = 
{
  {gZclOnffAttr_Off_d, gZclOnffAttr_Off_d, 0x00}        /* OnOff State */
};

/******************************************************************************
* OnOff Switch Configuration Cluster
******************************************************************************/
const zclOnOffSwitchCfgAttrsRAM_t gDefaultOnOffSwitchCfgAttrData = 
{
  gZclSwitchType_ToggleSw_c,                            /* Switch Type */
  gZclSwitchAction_Off_c                                /* Switch Action */      
};

/******************************************************************************
*  Level Control Cluster
******************************************************************************/
const zclLevelCtrlAttrsRAM_t gDefaultLevelCtrlAttrData =
{
 {gZclLevel_LowestPossible,gZclLevel_LowestPossible,0x00}                                       /* Current Level */
#if gZclClusterOptionals_d
  ,0x0000                                               /* RemainingTime */
  ,0x0000                                               /* onOffTransitionTime */
  ,0xFF                                                 /* OnLevel  */
  ,0x0000                                               /* onTransitionTime */
  ,0x0000                                               /* offTransitionTime */
  ,gZclLevelControl_DefaultMoveRate_d	                /* defaultMoveRate */
#endif
};

/******************************************************************************
*  Color Control Cluster
******************************************************************************/
const zclColorCtrlAttrsRAM_t gDefaultColorCtrlAttrData =
{
  {0x00, 0x00, 0x00}, /* current Hue*/
  {0xFE, 0xFE, 0x00}, /* current saturation */ 
   0x0000, /* remaining time */
   {gColorCtrlDefaultRedX_c, gColorCtrlDefaultRedX_c, 0}, /* current X - for Hue  = 0x00, Saturation = 0xFE, Level = 0xFE */
   {gColorCtrlDefaultRedY_c, gColorCtrlDefaultRedY_c, 0}, /* current Y - for Hue = 0x00, Saturation = 0xFE, Level = 0xFE  */
#if gColorCtrlEnableDriftCompensation_c     
   0x00, /* drift compesation */
   {0x04, "None           "}, /* compensation text */
#endif   
#if gColorCtrlEnableColorTemperature_c
   {0x00FA, 0x00FA, 0x00},/* color temperature */
#endif   
   0x01, /* color Mode */
#if gColorCtrlEnablePrimariesInformation_c   
   0x03, /* noOfPrimaries */
   gColorCtrlDefaultRedX_c, /* primary1X */
   gColorCtrlDefaultRedY_c, /* primary1Y */
   0xFE, /* primary1Intensity */
   gColorCtrlDefaultGreenX_c, /* primary2X */
   gColorCtrlDefaultGreenY_c, /* primary2Y */
   0xFE, /* primary2Intensity */
   gColorCtrlDefaultBlueX_c, /* primary3X */
   gColorCtrlDefaultBlueY_c, /* primary3Y */
   0xFE, /* primary3Intensity */
#if gColorCtrlEnablePrimariesAdditionalInf_c   
   0x0000, /* primary4X */
   0x0000, /* primary4Y */
   0x00, /* primary4Intensity */   
   0x0000, /* primary5X */
   0x0000, /* primary5Y */
   0x00, /* primary5Intensity */      
   0x0000, /* primary6X */
   0x0000, /* primary6Y */
   0x00, /* primary6Intensity */   
#endif /* gColorCtrlEnablePrimariesAdditionalInf_c */   
#endif /* gColorCtrlEnablePrimariesInformation_c */   
#if gColorCtrlEnableColorPointSettings_c 
   gColorCtrlDefaultWhiteX_c, /* white point X */
   gColorCtrlDefaultWhiteY_c, /* white point Y */
   gColorCtrlDefaultRedX_c, /* color point Red  X */
   gColorCtrlDefaultRedY_c, /* color point Red  Y */
   0xFE,   /* color point Red Intensity */     
   gColorCtrlDefaultGreenX_c, /* color point Green  X */
   gColorCtrlDefaultGreenY_c, /* color point Green  Y */
   0xFE,   /* color point Green Intensity */     
   gColorCtrlDefaultBlueX_c, /* color point Blue  X */
   gColorCtrlDefaultBlueY_c, /* color point Blue  Y */
   0xFE,   /* color point Blue Intensity */        
#endif /* gColorCtrlEnableColorPointSettings_c */  
#if gColorCtrlEnableZllFunctionality_c    
   0x0000, /* enhanced current hue */
   0x01, /* enhanced color mode */
   0x00, /* color loop active */   
   0x00, /* color loop direction */  
   0x0019, /* color loop time */  
   0x2300, /* color loop start enhanced hue */ 
   0x0000, /* color loop stored enhanced hue */ 
   {0x01, 0x01, 0x01, 0x01, 0x00, 0x00}, /* color loop capabilities */
#if gColorCtrlEnableColorTemperature_c
   0x0000, /* color temp physical min */
   0xFFFF, /* color temp physical max */
#endif /* gColorCtrlEnableColorTemperature_c */   
#endif   
};


/******************************************************************************
* Time Cluster
******************************************************************************/
ZCLTimeServerAttrsRAM_t gDefaultTimeAttrData = 
{
  0x00000000,                                           /* gZclAttrTime_c */
  (zclTimeStatusMaster | zclTimeStatusMasterZoneDst),   /* gZclAttrTimeStatus_c */
  0x00000000,                                           /* gZclAttrTimeZone_c */
  0x00000000,                                           /* gZclAttrDstStart_c */
  0x00000000,                                           /* gZclAttrDstEnd_c */
  0x00000000,                                           /* gZclAttrDstShift_c */
  0x00000000,                                           /* gZclAttrStandardTime_c */
  0x00000000,                                           /* gZclAttrLocalTime_c */
  0xFFFFFFFF,                                           /* gZclAttrLastSetTime_c */
  0xFFFFFFFF,                                           /* gZclAttrValidUntilTime_c */
};

/******************************************************************************
* Poll Control Cluster
******************************************************************************/
#if gZclEnablePollControlCluster_d 
const zclPollControlAttrsRAM_t gDefaultPollControlAttrData = 
{
    gZclPollControl_CheckInInterval_Test_c,       /* checkInInterval = 360 quarterseconds */
    gZclPollControl_LongPollInterval_Test_c,      /* longPollInterval = 5 seconds */
    gZclPollControl_ShortPollInterval_Test_c,     /* shortPollInterval = (2*1/4) seconds */
    gZclPollControl_FastPollTimeout_Test_c        /* fastPollTimeout = 10 seconds */
#if gZclClusterOptionals_d 
    ,gZclPollControl_CheckInIntervalMin_Test_c,   /* checkInIntervalMin: 180 quarterseconds */
    gZclPollControl_LongPollIntervalMin_Test_c,   /* longPollIntervalMin: 12 quarterSeconds */
    gZclPollControl_FastPollTimeoutMax_Test_c,    /* fastPollTimeoutMax: 240 quarterSeconds */
#endif
};
#endif

/******************************************************************************
* Appliance Statistics Cluster
******************************************************************************/
#if gZclEnableApplianceStatistics_d
const zclApplianceStatisticsAttrsRAM_t gDefaultApplianceStatisticsAttrData = 
{
  gApplianceStatistics_LogMaxSize_c,                    /* Log Max size*/
  gApplianceStatistics_LogQueueMaxSize_c                /* Log Queue Max Size*/
};
#endif

/******************************************************************************
* Appliance Identification Cluster
******************************************************************************/
#if gZclEnableApplianceIdentification_d 
const zclApplIdentificationAttrsRAM_t gDefaultApplianceIdentifAttrData = 
{
  {gApplIdentifAttr_CompanyId_c,               
   gApplIdentifAttr_CompanyId_c,                 
   gApplIdentif_ProducTypeId_WhiteGoods_c, 
   gApplIdentif_CecedSpecVersion_CompliantV1_0_Cert_c} /* basicIdentification */
#if gZclClusterOptionals_d
  ,gBasicManufacturerName_c,                            /* companyName = "Freescale" */
  gApplIdentifAttr_CompanyId_c,                         /* companyId = 'F' + 'R' */  
  gBasicManufacturerName_c,                             /* brandName = "Freescale" */
  gApplIdentifAttr_CompanyId_c,                         /* BrandId = 'F' + 'R' */  
  {0x0D, "Freescale0001   "},                           /* model = "Freescale0001" */
  {0x0D, "Freescale0001   "},                           /* partNumber = "Freescale0001" */
  {3, "000   "},                                        /* productRevision */
  {3, "000   "},                                        /* softwareRevision */                             
  {0x02, 0x57, 0x47},                                   /* productTypeName */
  gApplIdentif_ProducTypeId_WhiteGoods_c,               /* productTypeId */
  gApplIdentif_CecedSpecVersion_CompliantV1_0_Cert_c    /* Ceced Specification Version */  
#endif /* gZclClusterOptionals_d  */
};
#endif

/******************************************************************************
* Meter Identification Cluster
******************************************************************************/
#if gZclEnableMeterIdentification_d
const zclMeterIdentificationAttrsRAM_t gDefaultMeterIdentifAttrData = 
{ 
  gBasicManufacturerName_c,                             /* companyName */
  gMeterIdentif_MeterTypeId_UtilitySecondaryMeter_c,    /* meterTypeId */   
  gMeterIdentif_DataQualityId_AllDataCertified_c,       /* dataQualityId */
#if gZclClusterOptionals_d  
  gBasicManufacturerName_c,                             /* customerName */
  {0x0D, "Freescale0001   "},                           /* model = "Freescale0001" */
  {0x0D, "Freescale0001   "},                           /* partNumber = "Freescale0001" */
  {3, "000   "},                                        /* productRevision */
  {3, "000   "},                                        /* softwareRevision */     
  gBasicManufacturerName_c,                             /* utilityName = "Freescale" */
#endif  
  gBasicManufacturerName_c,                             /* pointOfDelivery */
  0x00,                                                 /* availablePower */  
  0x00                                                  /* powerThreshold */
};
#endif


#if gZclEnableOTAClient_d
const zclOTAAttrsRAM_t gDefaultOtaAttrData = 
{
  {0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF},   /* server IEEE address */
  gOTAUpgradeStatusNormal_c,                          /* status  */
   0,                                                /* file offset */
  gOtaCurrentFileVersion_c,                          /* current version */
  gOtaCurrentFileVersion_c                           /* downloaded version */
#if gZigbeeProIncluded_d
  ,gZclStackVer_ZigBeePro_c,                         /* stack version */ 
  gZclStackVer_ZigBeePro_c,                          /* downloaded stack version */
#else
  ,gZclStackVer_ZigBee2007_c,                        /* stack version */
  gZclStackVer_ZigBee2007_c,                         /* downloaded stack version */
#endif /* gZigbeeProIncluded_d */
  gOtaManufacturerCodeFsl_c,                         /* manufacturer Code  */
  gOtaCurrentImageType_c,                            /* image type */   
  0x0000                                             /* minimum activation delay = 0 = no delay */
};
#endif

/******************************************************************************
* EZ mode commissioning data
******************************************************************************/
#if gASL_EnableEZCommissioning_d && gASL_EnableEZCommissioning_Initiator_d

const EZ_ModeClusterList_t gEZModeTargetClusterList = 
{
  0, 	/* Number of input clusters */
  NULL, /* EZMode Input cluster list */
  0,	/* Number of output clusters */
  NULL,	/* EZMode Output cluster list */
};

#if gNum_HaOnOffSwitchEndpoints_c
/* OnOff Switch Device */
const uint8_t gEZModeOnOffSwitchInputClusterList[] = 	/* input cluster list for EZ commissioning Bind procedure */
{
   0x03, 0x00,  /* Identify */  
};
const uint8_t gEZModeOnOffSwitchOutputClusterList[] = 	/* output cluster list for EZ commissioning Bind procedure */
{
   0x03, 0x00,  /* Identify */		
   0x06, 0x00   /* OnOff */
};

const EZ_ModeClusterList_t gEZModeOnOffSwitchClusterList = 
{
  NumberOfElements(gEZModeOnOffSwitchInputClusterList)/2, 	/* Number of input clusters */
  (uint8_t *) gEZModeOnOffSwitchInputClusterList, 		/* EZMode Input cluster list */
  NumberOfElements(gEZModeOnOffSwitchOutputClusterList)/2,	/* Number of output clusters */
  (uint8_t *) gEZModeOnOffSwitchOutputClusterList,		/* EZMode Output cluster list */
};
#endif /* gNum_HaOnOffSwitchEndpoints_c */

#if gNum_HaDimmerSwitchEndpoints_c
/* Dimmer Switch Device */
uint8_t gEZModeDimmerSwitchInputClusterList[] = 	/* input cluster list for EZ commissioning Bind procedure */
{
   0x03, 0x00,  /* Identify */  
};
uint8_t gEZModeDimmerSwitchOutputClusterList[] = 	/* output cluster list for EZ commissioning Bind procedure */
{
   0x03, 0x00,  /* Identify */		
   0x06, 0x00,  /* OnOff */
   0x08, 0x00   /* LevelControl */
};

EZ_ModeClusterList_t gEZModeDimmerSwitchClusterList = {
  NumberOfElements(gEZModeDimmerSwitchInputClusterList)/2, 	/* Number of input clusters */
  (uint8_t *) gEZModeDimmerSwitchInputClusterList, 			/* EZMode Input cluster list */
  NumberOfElements(gEZModeDimmerSwitchOutputClusterList)/2,	/* Number of output clusters */
  (uint8_t *) gEZModeDimmerSwitchOutputClusterList,		/* EZMode Output cluster list */
};
#endif

#if gNum_HaCombinedInterfaceEndpoints_c
/* Combined Interface Device */
const uint8_t gEZModeCombinedInputClusterList[] = 	/* input cluster list for EZ commissioning Bind procedure */
{
   0x03, 0x00,  /* Identify */  
};
const uint8_t gEZModeCombinedOutputClusterList[] = 	/* output cluster list for EZ commissioning Bind procedure */
{
   0x03, 0x00,  /* Identify */		
   0x06, 0x00,  /* OnOff */
   0x08, 0x00   /* LevelControl */
};

const EZ_ModeClusterList_t gEZModeCombinedInterfaceClusterList = 
{
  NumberOfElements(gEZModeCombinedInputClusterList)/2, 	/* Number of input clusters */
  (uint8_t *) gEZModeCombinedInputClusterList, 		/* EZMode Input cluster list */
  NumberOfElements(gEZModeCombinedOutputClusterList)/2,	/* Number of output clusters */
  (uint8_t *) gEZModeCombinedOutputClusterList,		/* EZMode Output cluster list */
};
#endif /* gNum_HaCombinedInterfaceEndpoints_c */

#if gNum_HaTempSensorEndpoints_c
/* Temperature Sensor Device */
const uint8_t gEZModeTempSensorInputClusterList[] = 	/* input cluster list for EZ commissioning Bind procedure */
{
   0x03, 0x00,  /* Identify */  
};
const uint8_t gEZModeTempSensorOutputClusterList[] = 	/* output cluster list for EZ commissioning Bind procedure */
{
   0x03, 0x00,  /* Identify */		
   0x02, 0x04,  /* Temperature sensor */
   0x06, 0x04   /* Occupancy Sensor */
};

const EZ_ModeClusterList_t gEZModeTempSensorClusterList = 
{
  NumberOfElements(gEZModeTempSensorInputClusterList)/2, 	/* Number of input clusters */
  (uint8_t *) gEZModeTempSensorInputClusterList, 		/* EZMode Input cluster list */
  NumberOfElements(gEZModeTempSensorOutputClusterList)/2,	/* Number of output clusters */
  (uint8_t *) gEZModeTempSensorOutputClusterList,		/* EZMode Output cluster list */
};
#endif

#endif /* gASL_EnableEZCommissioning_d && gASL_EnableEZCommissioning_Initiator_d */
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

