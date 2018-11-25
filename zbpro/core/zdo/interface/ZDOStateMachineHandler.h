/*****************************************************************************
* This is a main header file  for ZDO State Machine handler code file
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#ifndef _ZDOSTATEMACHINEHANDLER_H_
#define _ZDOSTATEMACHINEHANDLER_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "TMR_Interface.h"
#include "ZdoApsInterface.h"

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/*
  The ZDO state machine	public macros were moved to ZdoApsInterface.h, to be
  able to access them from the Lib.
*/

/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* This function handles the evets sent for State Machine of EndDevice.
*
*****************************************************************************/
void TS_ZdoStateMachineTask
(
  tsEvent_t events /*IN: Diffrent events Sent to ZDO State Machine either from
                       state machine or from ZDONwkManager or ZDPManager*/
);

/* Initialize the ZDO state machine task. */
void TS_ZdoStateMachineTaskInit
(
  void
);

#if gEndDevCapability_d || gComboDeviceCapability_d
  void ZDO_StopPolling(void);
#endif


/******************************************************************************
* Force the Nwk Rejoin if the limit of the Tx Failures or poll rewquest has
* been reached.
*
* Interface assumption:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
#if gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d
  void ZDO_ProcessDeviceNwkRejoin(void);
#endif

/******************************************************************************
*******************************************************************************
* Public Type Definations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public Memory Declerations
*******************************************************************************
******************************************************************************/
extern void AppResetApplicationQueues(void);

extern void AF_Reset(void);
#if gConcentratorFlag_d
  void ZDO_SendRouteDiscoveryManyToOne(uint8_t iRadius, bool_t noRouteCacheFlag);
#else
  #define ZDO_SendRouteDiscoveryManyToOne(iRadius, noRouteCacheFlag);
#endif

#if gStandardSecurity_d || gHighSecurity_d
  void RestoreIncomingFrameCounter(void);
#endif


/******************************************************************************
*******************************************************************************
* Public Function
*******************************************************************************
******************************************************************************/

/*None*/
#ifdef __cplusplus
}
#endif
#endif /*_ZDOSTATEMACHINEHANDLER_H_*/
