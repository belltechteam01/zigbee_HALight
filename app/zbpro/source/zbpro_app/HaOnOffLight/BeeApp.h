/************************************************************************************
* This header file is for Ha OnOffLight application Interface
*
* (c) Copyright 2005, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#ifndef _BAPP_H_
#define _BAPP_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "BeeStack_Globals.h"
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "ZCL.h"
#include "HaProfile.h"
#include "keyboard.h"
#include "AppToPlatformConfig.h"
#include "ASL_UserInterface.h"
/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/
#if(gInstantiableStackEnabled_d == 0)
  #define BeeAppData(val)          val
#else
  #define BeeAppData(val)          pBeeAppData->val
#endif
#define gHaOnOffLight_d 1

#define gIdentifyTimeSecs_d 10

#define BeeKitGroupDefault_d 0x01,0x00

#define BeeKitSceneDefault_d 1

/* events for the application */

/* The top 7 events are reserved by BeeStack. Apps may use bits 0-8 */
#define gAppEvtDataConfirm_c                    (1 << 15)
#define gAppEvtDataIndication_c                 (1 << 14)
#define gAppEvtStoreScene_c                     (1 << 12) /*Store Scene event */
#define gAppEvtAddGroup_c                       (1 << 13) /*Add group event */
#define gAppEvtSyncReq_c                        (1 << 11)
#define gInterPanAppEvtDataConfirm_c            (1 << 10)
#define gInterPanAppEvtDataIndication_c         (1 << 9) 

#define gAppRadiusCounter_c      gNwkMaximumDepth_c * 2


/* Default Aps security option - Home Automation uses only Nwk security */
#if gApsLinkKeySecurity_d
  #if gZDPNwkSec_d
    #define zdoTxOptionsDefault_c (gApsTxOptionUseNwkKey_c)
  #else
    #define zdoTxOptionsDefault_c (gApsTxOptionSecEnabled_c)
  #endif
  #define afTxOptionsDefault_c (gApsTxOptionSecEnabled_c)
#else
/* default to no APS layer security (both ZigBee and ZigBee Pro) */
  #define afTxOptionsDefault_c gApsTxOptionNone_c
  #define zdoTxOptionsDefault_c gApsTxOptionNone_c
#endif



/************************Control macros for an external light *****************/
/*example on how the macro's can be configured to control an GPIO pin */
#if(gTargetMC13213SRB_d)
#define ExternalDeviceInit()    PTBDD |= 0x40;
#define ExternalDeviceOn()      PTBD|=0x40
#define ExternalDeviceOff()     PTBD&=~0x40
#define ExternalDeviceToggle()  PTBD^=0x40
#else
#define ExternalDeviceInit()    
#define ExternalDeviceOn()
#define ExternalDeviceOff()
#define ExternalDeviceToggle()
#endif



/* This data set contains app layer variables to be preserved across resets */
#if !gInstantiableStackEnabled_d
#define gAPP_DATA_SET_FOR_NVM\
      {&gAslData,                  		1,         sizeof(ASL_Data_t),                  nvmId_AslData_c},           /* state of ASL */\
      {&gHaDevicePan0Data,         		1,         sizeof(haOnOffLightAttrRAM_t),       nvmId_AppData_c},           /* state of light */\
      {&gHaDevicePan0SceneData,    		1,         sizeof(zclOnOffLightSceneData_t),    nvmId_AppSceneData_c}       /* state of scene */ 
      /* insert any user data for NVM here.... */
#else
#define gAPP_DATA_SET_FOR_NVM\
      {&gAslData[0],                  		1,         sizeof(ASL_Data_t),                  nvmId_AslData_c},           /* state of ASL */\
      {&gHaDevicePan0Data,         		1,         sizeof(haOnOffLightAttrRAM_t),       nvmId_AppData_c},           /* state of light */\
      {&gHaDevicePan0SceneData,    		1,         sizeof(zclOnOffLightSceneData_t),    nvmId_AppSceneData_c},      /* state of scene */ \
      {&gAslData[1],                  		1,         sizeof(ASL_Data_t),                  nvmId_AslDataPan1_c},       /* state of ASL */\
      {&gHaDevicePan1Data,         		1,         sizeof(haOnOffLightAttrRAM_t),       nvmId_AppDataPan1_c},       /* state of light */\
      {&gHaDevicePan1SceneData,    		1,         sizeof(zclOnOffLightSceneData_t),    nvmId_AppSceneDataPan1_c}   /* state of scene */ 

      /* insert any user data for NVM here.... */
#endif

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
/* data used for EZ-mode commissioning - NwkSteering and Finding and Binding */
extern haOnOffLightAttrRAM_t    gHaDevicePan0Data;
extern haOnOffLightAttrRAM_t    gHaDevicePan1Data;
extern zclOnOffLightSceneData_t gHaDevicePan0SceneData;
extern zclOnOffLightSceneData_t gHaDevicePan1SceneData;

/* data used for EZ-mode commissioning - Factory Fresh (Reset to factory default) */
extern zclBasicAttrsRAM_t       gDefaultBasicAttrData;
extern zclOnOffAttrsRAM_t       gDefaultOnOffAttrData;
extern zclIdentifyAttrsRAM_t    gDefaultIdentifyAttrData;
extern zclGroupAttrsRAM_t       gDefaultGroupAttrData;
extern zclSceneAttrs_t          gDefaultSceneAttrData;
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)
extern zclPollControlAttrsRAM_t gDefaultPollControlAttrData;
#endif

#if gASL_EnableEZCommissioning_Initiator_d
extern EZ_ModeClusterList_t gEZModeTargetClusterList;
#endif /* gASL_EnableEZCommissioning_Initiator_d */   
void BeeAppTask(tsEvent_t events);
/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/
void InitZtcForZcl(void);

void BeeAppInit(void);
void BeeAppHandleKeys(key_event_t events);

void BeeApp_FactoryFresh(void);

#if gEndDevCapability_d || gComboDeviceCapability_d
void BeeAppSetMediumPollRate(void);
#endif
/* tell applicaton about change in HA status */
void BeeAppUpdateDevice(zbEndPoint_t endPoint, zclUIEvent_t event, zclAttrId_t attrId, zbClusterId_t clusterId, void *pData);
/**********************************************************************************/
#ifdef __cplusplus
}
#endif
#endif /* _BAPP_H_ */
