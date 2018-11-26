/************************************************************************************
* HaOnOffLightEndPoint.c
*
* This module contains the OnOffLight endpoint data, and pointers to the proper
* handling code from ZCL.
*
* Copyright (c) 2013, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"

#include "HaProfile.h"
#include "ZCLGeneral.h"
#if gZclEnableOTAClient_d ||  gZclEnableOTAServer_d 
#include "ZclOta.h"
#endif

#if gAddValidationFuncPtrToClusterDef_c
#define mNoValidationFuncPtr_c ,NULL
#else
#define mNoValidationFuncPtr_c
#endif


#define gHaNumOnOffLights_c              1
#define gHaDevicePan0_MaxAttrReporting_c 1
#define gHaDevicePan1_MaxAttrReporting_c 1

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/
/* make sure to have once copy of this structure per instance of this device */
haOnOffLightAttrRAM_t gHaDevicePan0Data;
haOnOffLightAttrRAM_t gHaDevicePan1Data;

/* Scene data for the OnOffLight device */
zclOnOffLightSceneData_t gHaDevicePan0SceneData;
zclOnOffLightSceneData_t gHaDevicePan1SceneData;

/*HA OnOff Light Application Reporting setup */
zclReportingSetup_t gHaDevicePan0ReportSetup[gHaDevicePan0_MaxAttrReporting_c];
zclReportingSetup_t gHaDevicePan1ReportSetup[gHaDevicePan1_MaxAttrReporting_c];


/* only need 1 copy of this structure for all instances of this device type */
afClusterDef_t const gaHaOnOffLightClusterList[] =
{
  { { gaZclClusterBasic_c    }, (pfnIndication_t)ZCL_BasicClusterServer,        NULL,     (void *)(&gZclBasicClusterAttrSetList),       MbrOfs(haOnOffLightAttrRAM_t,basicAttrs)        mNoValidationFuncPtr_c, NULL},
  { { gaZclClusterIdentify_c }, (pfnIndication_t)ZCL_IdentifyClusterServer,     NULL,     (void *)(&gZclIdentifyClusterAttrSetList) ,   MbrOfs(haOnOffLightAttrRAM_t,identifyAttrs)     mNoValidationFuncPtr_c, (void *)(&gZclIdentifyClusterCommandsDefList)},
  { { gaZclClusterOnOff_c    }, (pfnIndication_t)ZCL_OnOffClusterServer,        NULL,     (void *)(&gZclOnOffClusterAttrSetList),       MbrOfs(haOnOffLightAttrRAM_t,onOffAttrs)        mNoValidationFuncPtr_c ,(void *)(&gZclOnOffClusterCommandsDefList)},
  { { gaZclClusterGroups_c   }, (pfnIndication_t)ZCL_GroupClusterServer,        NULL,     (void *)(&gZclGroupClusterAttrSetList),       MbrOfs(haOnOffLightAttrRAM_t,onOffAttrs)        mNoValidationFuncPtr_c, (void *)(&gZclGroupClusterCommandsDefList)},
  { { gaZclClusterScenes_c   }, (pfnIndication_t)ZCL_SceneClusterServer,        NULL,     (void *)(&gZclSceneClusterAttrSetList),       0 mNoValidationFuncPtr_c, (void *)(&gZclSceneClusterCommandsDefList)}
#if gZclEnablePollControlCluster_d
#if gEndDevCapability_d
  , { { gaZclClusterPollControl_c   },(pfnIndication_t)ZCL_PollControlClusterServer, NULL, (void *)(&gZclPollControlClusterAttrSetList), MbrOfs(haOnOffLightAttrRAM_t,pollControl), ZclPollControlValidateAttributes,(void *)(&gZclPollControlClusterCommandsDefList)}  
#endif 
#if gComboDeviceCapability_d   
    , { { gaZclClusterPollControl_c   },(pfnIndication_t)ZCL_PollControlClusterServer, (pfnIndication_t)ZCL_PollControlClusterClient, (void *)(&gZclPollControlClusterAttrSetList),MbrOfs(haOnOffLightAttrRAM_t,pollControl), ZclPollControlValidateAttributes , (void *)(&gZclPollControlClusterCommandsDefList)}  
#endif
#if gRouterCapability_d || gCoordinatorCapability_d
     , { { gaZclClusterPollControl_c   }, NULL, (pfnIndication_t)ZCL_PollControlClusterClient, (void *)(&gZclPollControlClusterAttrSetList), 0 mNoValidationFuncPtr_c ,(void *)(&gZclPollControlClusterCommandsDefList)}     
#endif
#endif  /* gZclEnablePollControlCluster_d */
#if gZclEnableOTAServer_d && gZclEnableOTAClient_d
  , { { gaZclClusterOTA_c      }, (pfnIndication_t)ZCL_OTAClusterServer, (pfnIndication_t)ZCL_OTAClusterClient, (void *)(&gZclOTAClusterAttrSetList)L,0 mNoValidationFuncPtr_c , (void *)(&gZclOTAClusterCommandsDefList) }
#elif gZclEnableOTAServer_d  
  , { { gaZclClusterOTA_c      }, (pfnIndication_t)ZCL_OTAClusterServer, NULL, (void *)(&gZclOTAClusterAttrSetList), 0 mNoValidationFuncPtr_c, (void *)(&gZclOTAClusterCommandsDefList)}
#elif gZclEnableOTAClient_d  
  , { { gaZclClusterOTA_c      }, NULL, (pfnIndication_t)ZCL_OTAClusterClient, (void *)(&gZclOTAClusterAttrSetList), 0 mNoValidationFuncPtr_c , (void *)(&gZclOTAClusterCommandsDefList) }
#endif   

};

/* a list of attributes (with their cluster) that can be reported */
/* only need 1 copy of this structure for all instances of this device type */
zclReportAttr_t const gaHaOnOffLightReportList[gHaDevicePan0_MaxAttrReporting_c] = 
{
  { { gaZclClusterOnOff_c    }, gZclAttrOnOff_OnOffId_c }
};


/* 
  have one copy of this structure per instance of this device. For example, if 
  there are 3 OnOffLights, make sure this array is set to 3 and initialized to 
  3 devices
*/
afDeviceDef_t const gHaOnOffLightDeviceDef =
{
 (pfnIndication_t)ZCL_InterpretFoundationFrame,
  NumberOfElements(gaHaOnOffLightClusterList),
  (afClusterDef_t *)gaHaOnOffLightClusterList,  /* pointer to Cluster List */
  NumberOfElements(gaHaOnOffLightReportList),
  (zclReportAttr_t *)gaHaOnOffLightReportList,  /* pointer to report attribute list */
  &gHaDevicePan0ReportSetup[0],                 /* pointer to report setup */
  &gHaDevicePan0Data,                           /* pointer to endpoint data */
  &gHaDevicePan0SceneData,                      /* pointer to scene data */
};


afDeviceDef_t const gHaOnOffLightDevice2Def =
{
 (pfnIndication_t)ZCL_InterpretFoundationFrame,
  NumberOfElements(gaHaOnOffLightClusterList),
  (afClusterDef_t *)gaHaOnOffLightClusterList,  /* pointer to Cluster List */
  NumberOfElements(gaHaOnOffLightReportList),
  (zclReportAttr_t *)gaHaOnOffLightReportList,  /* pointer to report attribute list */
  &gHaDevicePan1ReportSetup[0],                 /* pointer to report setup */
  &gHaDevicePan1Data,                          /* pointer to endpoint data */
  &gHaDevicePan1SceneData                      /* pointer to scene data */
};

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* HA_StoreScene
*
* Stores the attributes in the scene
*
*****************************************************************************/
void HA_StoreScene(zbEndPoint_t endPoint, zclSceneTableEntry_t *pScene)
{
  zclOnOffLightSceneTableEntry_t *data;
  zbClusterId_t aClusterId;
  uint8_t attrLen;
  
  data = (zclOnOffLightSceneTableEntry_t*) pScene;
  
  Set2Bytes(aClusterId, gZclClusterOnOff_c);  
  (void) ZCL_GetAttribute(endPoint, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &(data->onOff), &attrLen);  
}

/*****************************************************************************
* HA_RecallScene
*
* Restores OnOff light to this state.
*
*****************************************************************************/
void HA_RecallScene(zbEndPoint_t endPoint, void *pData, zclSceneTableEntry_t *pScene)
{
  zclUIEvent_t event;
  zclOnOffLightSceneTableEntry_t *data;
  zbClusterId_t aClusterId;
  
  (void)pData;
  
  /* set the current scene */
  data = (zclOnOffLightSceneTableEntry_t*) pScene;

  Set2Bytes(aClusterId, gZclClusterOnOff_c);
  (void)ZCL_SetAttribute(endPoint, aClusterId, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &data->onOff);    

  /* update the device */
  event = data->onOff ? gZclUI_On_c : gZclUI_Off_c;
  BeeAppUpdateDevice(endPoint, event,0,0,NULL);
}

/*****************************************************************************
* HA_ViewSceneData
*
* Creates a payload of scene data
*
* Returns the length of the scene data payload
*****************************************************************************/
uint8_t HA_ViewSceneData(zclSceneOtaData_t *pClusterData, zclSceneTableEntry_t *pScene)
{
  zclOnOffLightSceneTableEntry_t *levelData;
  
  levelData = (zclOnOffLightSceneTableEntry_t*) pScene;
  
  /* build data for the OnOff Light */
  pClusterData->clusterId = gZclClusterOnOff_c;
  pClusterData->length = sizeof(levelData->onOff);
  pClusterData->aData[0] = levelData->onOff;
  
  return sizeof(zclSceneOtaData_t);
}

/*****************************************************************************
* HA_AddScene
*
* Internal add scene, when this device recieves and add-scene over the air.
*
* Returns status (worked or failed).
*****************************************************************************/
zbStatus_t HA_AddScene(zclSceneOtaData_t *pClusterData, zclSceneTableEntry_t *pScene)
{
  zclOnOffLightSceneTableEntry_t *levelData;    
  zbStatus_t ret = gZbSuccess_c;
  
  levelData = (zclOnOffLightSceneTableEntry_t*) pScene;  
  
  /* 
    NOTE: If device is extended with scenes across multiple clusters, 
    there must be an If(pClusterData->clusterId == xx) check for each cluster
  */
  
  if (pClusterData->clusterId == gZclClusterOnOff_c)
  {
    levelData->onOff = pClusterData->aData[0];    
  }
  else
    ret = gZclMalformedCommand_c;
        
  return ret;
}

#if gASL_EnableZLLClustersData_d
/*****************************************************************************
* HA_CopyScene
*
* copy the attributes in the scene
*
*****************************************************************************/
void HA_CopyScene(zclSceneTableEntry_t *pSceneFrom, zclSceneTableEntry_t *pSceneTo)
{
  zclOnOffLightSceneTableEntry_t *onOffDataFrom;
  zclOnOffLightSceneTableEntry_t *onOffDataTo;

  onOffDataFrom = (zclOnOffLightSceneTableEntry_t*) pSceneFrom;
  onOffDataTo = (zclOnOffLightSceneTableEntry_t*) pSceneTo;
  
  onOffDataTo->onOff = onOffDataFrom->onOff;
  onOffDataTo->sceneTableEntry.transitionTime = onOffDataFrom->sceneTableEntry.transitionTime;
  onOffDataTo->sceneTableEntry.transitionTime100ms = onOffDataFrom->sceneTableEntry.transitionTime100ms;  
#if gZclIncludeSceneName_d  
  FLib_MemCpy(&onOffDataTo->sceneTableEntry.szSceneName, onOffDataFrom->sceneTableEntry.szSceneName, sizeof(onOffDataFrom->sceneTableEntry.szSceneName));
#endif  
}
#endif
