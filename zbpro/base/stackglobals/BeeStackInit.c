/******************************************************************************
*  This file is to Initialize the entire BeeStack.
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#include "EmbeddedTypes.h"
#include "NVM_Interface.h"
#include "TMR_Interface.h"
#include "TS_Interface.h"
  
#if gLpmIncluded_d 
  #include "pwr_interface.h"
  #include "pwr_configuration.h"
#endif
#ifndef __IAR_SYSTEMS_ICC__
#if !gNvStorageIncluded_d  && gNvHalIncluded_d
  #include "NV_FlashHal.h"
#endif
#endif
#include "ZtcInterface.h"

#include "BeeStack_Globals.h"
#include "BeeStackConfiguration.h"
#include "ZdoApsInterface.h"
#ifndef gHostApp_d
  //#include "AppAspInterface.h"
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
void ResetSecurityMaterials(void);
void BeeStackTaskInit(void);
void TS_PlaceHolderTaskInit(void);
void BeeStackLpmInit(void);
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

typedef struct beeTaskEntry_tag {
  tsTaskID_t *pTaskID;
  void (*pInitFunction)(void);
  void (*pEventHandler)(tsEvent_t);
  tsTaskPriority_t priority;
} beeTaskEntry_t;

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

#define Task(taskIdGlobal, taskInitFunc, taskMainFunc, priority) \
  { &taskIdGlobal, taskInitFunc, taskMainFunc, priority },
  
  beeTaskEntry_t const beeTaskTable[] = {
  #include "BeeStackTasksTbl.h"
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

void BeeStackInit(void) 
{

  BeeStackTaskInit();
  ResetSecurityMaterials();
  #if(gInstantiableStackEnabled_d == 0)
  /* Init low power module for zigbee task */  
  BeeStackLpmInit();
  #endif
}

void BeeStackLpmInit(void) 
{

  #if (gLpmIncluded_d || gComboDeviceCapability_d)
  #if gComboDeviceCapability_d
    if(ZbBeeStackNwkGlobals(gLpmIncluded))
  #endif
    {
    #ifndef gHostApp_d
    #if cPWR_UsePowerDownMode    
      PWR_CheckForAndEnterNewPowerState_Init();
      PWRLib_SetCurrentZigbeeStackPowerState(StackPS_DeepSleep);
    #endif    
  #endif
    }
  #endif
}


void ResetSecurityMaterials(void)
{
  /* R20 */
  /* Reset the security material with the proper set.! */
#if gStandardSecurity_d || gHighSecurity_d
   SSP_NwkResetSecurityMaterials();
#if gApsLinkKeySecurity_d
   SSP_ApsResetSecurityMaterial();
#endif
#endif
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/* This function can be used in the table in BeeStackTasksTbl.h for any task */
/* that does not have it's own init function. */
void TS_PlaceHolderTaskInit(void) {
}

/*****************************************************************************/

/* Init the BeeStack tasks. Add all of them to the kernel's task table, and */
/* call of the init functions. */
void BeeStackTaskInit(void) 
{
  
  index_t i;
  
  /* Add the tasks to the kernel's task table first. That way, if any of the */
  /* init functions want to set events, all of the task ids will be defined. */
  for (i = 0; i < 5; ++i) 
  {

    *beeTaskTable[i].pTaskID = TS_CreateTask(beeTaskTable[i].priority,
                                               beeTaskTable[i].pEventHandler);
    
  } 

  for (i = 0; i < 5; ++i) 
  {
    (*beeTaskTable[i].pInitFunction)();
  }
}/* BeeStackTaskInit() */
