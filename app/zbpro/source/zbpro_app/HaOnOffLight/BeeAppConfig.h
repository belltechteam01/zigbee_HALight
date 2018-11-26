/*! *********************************************************************************
* \file BeeAppConfig.h
* This  file is used as a prefix file and contain settings specific to  the Application.
*
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************** */

#ifndef _BEEAPP_CONFIG_H_
#define _BEEAPP_CONFIG_H_

#ifdef __cplusplus
    extern "C" {
#endif
        
/************************************************************************************
*
*       ZTC
*
************************************************************************************/
#ifndef gZtcNonBlockingFunctionality_c
    #define gZtcNonBlockingFunctionality_c      1
#endif
#ifndef gZtcIncluded_d
    #define gZtcIncluded_d      1
#endif
#ifndef gFSCI_IncludeMacCommands_c
    #define gFSCI_IncludeMacCommands_c      1
#endif
#ifndef gUseHwValidation_c
    #define gUseHwValidation_c      1
#endif
#ifndef gInstantiableStackEnabled_d
    #define gInstantiableStackEnabled_d      0
#endif
/************************************************************************************
*
*       CONFIG FRAMEWORK
*
************************************************************************************/

#include "AppToPlatformConfig.h"

/************************************************************************************
*
*       CONFIG Mac/Phy
*
************************************************************************************/
        
#include "AppToMacPhyConfig.h"

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/* Enable optionals to enable all optional clusters/attributes */
#ifndef gZclClusterOptionals_d
#define gZclClusterOptionals_d                          TRUE
#endif

/***************************************
  Foundation commands
****************************************/

/* enable reporting of attributes (saves about 1.4K ROM) */
#ifndef gZclEnableReporting_c
#define gZclEnableReporting_c                           TRUE
#endif

/***************************************
 EZ mode Commissioning Defines: 
****************************************/  

/* enable Ez mode commissioning */
#ifndef gASL_EnableEZCommissioning_d
#define gASL_EnableEZCommissioning_d                    TRUE
#endif

/* invoke Ez mkode: */
#ifndef gASL_ZclCmdEzModeInvokeReq_d
#define gASL_ZclCmdEzModeInvokeReq_d                    FALSE
#endif

/* update Commissioning state */
#ifndef gASL_ZclCmdUpdateCommissioningStateReq_d
#define gASL_ZclCmdUpdateCommissioningStateReq_d        FALSE
#endif

/* select the EZCommissioning/Touchlink Role: Initiator or Target */
#ifndef gASL_EnableEZCommissioning_Initiator_d
#define gASL_EnableEZCommissioning_Initiator_d          FALSE
#endif

/* Add Group support for EZ commissioning procedure */
#ifndef gASL_EzCommissioning_EnableGroupBindCapability_d
#define gASL_EzCommissioning_EnableGroupBindCapability_d    FALSE
#endif    


/***************************************
  Identify Cluster Defines:
****************************************/

/* Enable Identify  Req ZTC/App support */
#ifndef gASL_ZclIdentifyReq_d
#define gASL_ZclIdentifyReq_d                           FALSE 
#endif

/* Enable Identify Query Req ZTC/App support */
#ifndef gASL_ZclIdentifyQueryReq_d
#define gASL_ZclIdentifyQueryReq_d                      FALSE
#endif


/***************************************
  Group Cluster Defines:
****************************************/

/* Enable Add group ZTC/App support */
#ifndef gASL_ZclGroupAddGroupReq_d
#define gASL_ZclGroupAddGroupReq_d                      FALSE
#endif

/* Enable View group ZTC/App support */
#ifndef gASL_ZclGroupViewGroupReq_d
#define gASL_ZclGroupViewGroupReq_d                     FALSE
#endif

/* Enable Get Group Membership ZTC/App support */
#ifndef gASL_ZclGetGroupMembershipReq_d
#define gASL_ZclGetGroupMembershipReq_d                 FALSE
#endif

/* Enable Remove Group ZTC/App support */
#ifndef gASL_ZclRemoveGroupReq_d
#define gASL_ZclRemoveGroupReq_d                        FALSE
#endif

/* Enable Remove All Groups ZTC/App support */
#ifndef gASL_ZclRemoveAllGroupsReq_d
#define gASL_ZclRemoveAllGroupsReq_d                    FALSE
#endif

/* Enable Add Group Handler functionality  */
#ifndef gASL_ZclAddGroupHandler_d
#define gASL_ZclAddGroupHandler_d                       TRUE
#endif


/***************************************
  Scene Cluster Defines:
****************************************/

/* default 16 scenes per device */
#ifndef gHaMaxScenes_c
#define gHaMaxScenes_c                                  2
#endif

/* The maximum size in bytes for the storable scene */
/* The Scene data array:
              -> For the OnOff scenes it only needs 1 byte (0x01).
              -> For the Dimmer light it only needs 11 bytes (0x0B).
              -> For the Thermostat it needs the gHaMaxSceneSize_c
                 which is 45 bytes (0x2D).
*/
#ifndef gHaMaxSceneSize_c
#define gHaMaxSceneSize_c                               45
#endif

/* Enable Store Scene ZTC/App support */
#ifndef gASL_ZclStoreSceneReq_d
#define gASL_ZclStoreSceneReq_d                         FALSE
#endif

/* Enable Recall Scene ZTC/App support */
#ifndef gASL_ZclRecallSceneReq_d
#define gASL_ZclRecallSceneReq_d                        FALSE
#endif

/* Enable Get Scene Membership ZTC/App support */
#ifndef gASL_ZclGetSceneMembershipReq_d
#define gASL_ZclGetSceneMembershipReq_d                 FALSE
#endif

/* Enable Add Scene ZTC/App support */
#ifndef gASL_ZclSceneAddSceneReq_d
#define gASL_ZclSceneAddSceneReq_d                      FALSE
#endif

/* Enable View Scene ZTC/App support */
#ifndef gASL_ZclViewSceneReq_d
#define gASL_ZclViewSceneReq_d                          FALSE
#endif

/* Enable Remove Scene ZTC/App support */
#ifndef gASL_ZclRemoveSceneReq_d
#define gASL_ZclRemoveSceneReq_d                        FALSE
#endif

/* Enable Remove All Scene ZTC/App support */
#ifndef gASL_ZclRemoveAllScenesReq_d
#define gASL_ZclRemoveAllScenesReq_d                    FALSE
#endif

/* Enable Store Scene Handler functionality  */
#ifndef gASL_ZclStoreSceneHandler_d
#define gASL_ZclStoreSceneHandler_d                     TRUE
#endif

/* Enable Copy Scene ZTC/App support */
#ifndef gASL_ZclCopySceneReq_d
#define gASL_ZclCopySceneReq_d                          FALSE
#endif

/* Enable to include scene name in scene table */
#ifndef gZclIncludeSceneName_d
#define gZclIncludeSceneName_d                          FALSE
#endif

/***************************************
  OnOff Cluster Defines:
****************************************/

/* Set number of OnOff Server instances -  multi Endpoint solution - */
#ifndef gNoOfOnOffServerInstances_d
#define gNoOfOnOffServerInstances_d                     0x01
#endif

/* Enable On/Off/Toggle ZTC/App support */
#ifndef gASL_ZclOnOffReq_d
#define gASL_ZclOnOffReq_d                              FALSE
#endif

/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/

/*None*/

/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

/*None*/
#ifdef __cplusplus
}
#endif
#endif /*_BEEAPP_CONFIG_H_*/

