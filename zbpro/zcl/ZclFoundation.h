/******************************************************************************
* ZclFoundation.h
*
* This module contains code that handles ZigBee Cluster Library 
*
* Copyright (c) 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
* [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf
*******************************************************************************/
#ifndef _ZCL_FOUNDATION_INTERFACE_H_
#define _ZCL_FOUNDATION_INTERFACE_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"

#include "HaProfile.h"

#ifdef gZclEnablePartition_d    
#if (gZclEnablePartition_d == TRUE)

uint8_t ZCL_BuildReadAttrResponseWithPartition
  (
  afDeviceDef_t  *pDevice,    /* IN: device definition for this endpoint */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint8_t count,              /* IN: # of responses */
  zclFrame_t *pFrame,         /* IN: read attr request */
  uint8_t maxAsduLen,         /* IN: maximum ASDU length */ 
  zclFrame_t *pFrameRsp,       /* IN/OUT: response frame, allocated above, filled in here... */
  bool_t partitionReadAttr
  );
  
uint8_t ZCL_BuildWriteAttrResponseWithPartition
  (
  afDeviceDef_t  *pDevice,    /* IN: data to be acted on by this cluster */
  afClusterDef_t *pCluster,   /* IN: cluster definition to write attrs */
  uint8_t asduLen,            /* IN: length of input frame */
  zclFrame_t *pFrame,         /* IN: input frame */
  zclFrame_t *pFrameRsp,      /* IN/OUT: response to write attribute */
  bool_t partitionedWriteAttr,/* IN: write to a partition table instead of main atttr table */
  bool_t forceWritingROAttr   /* IN: write data even if the attribute is readonly */
  );
#endif  
#endif

#if gFragmentationCapability_d
uint16_t ZCL_BuildReadAttrResponseWithFragmentation
  (
  afDeviceDef_t  *pDevice,    /* IN: device definition for this endpoint */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint16_t count,              /* IN: # of responses */  
  zbApsdeDataIndication_t *pIndication,         /* IN: data indication */
  afToApsdeMessage_t *pMsg       /* IN/OUT: response frame, allocated above, filled in here... */
  );
#endif

/*
  Function to receive the response from a ZCL call. Comes in originally on the 
  APSDE-DATA.indication, into the application's BeeAppDataIndication() function.
*/
typedef void (*fnZclResponseHandler_t)(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);

#if(gInstantiableStackEnabled_d == 0)
  #define ZbZclFoundationGlobals(val)        val
#else
  #define ZbZclFoundationGlobals(val)        pzclFoundationGlobals->val
#endif

#if(gInstantiableStackEnabled_d == 1)
/* Nwk struct of globals */  
typedef struct zclFoundationGlobals_tag
{
#endif

EXTERN uint8_t gZclTransactionId;

EXTERN zclAttrDef_t gCurrentAttr;

/* Used for identify */
EXTERN uint8_t gZclIdentifyTimerID;

#if gZclEnableReporting_c
EXTERN zclReportingClientSetup_t gZclReportingClientSetup[gZClMaxAttrReportingReceivedTable_c];
#endif

/* Used for reporting attributes */
EXTERN uint8_t gZclReportingTimerID;

EXTERN fnZclResponseHandler_t gfnZclResponseHandler;

#if gZclEnableReporting_c
EXTERN index_t gZclReportDeviceIndex;  /* for state machine, sending a report */
EXTERN index_t gZclReportClusterIndex; /*  */
EXTERN index_t gAsynchronousClusterIndex;
EXTERN bool_t  gfZclReportRestart;
#endif

#if(gInstantiableStackEnabled_d == 1)  
}zclFoundationGlobals_t;
#endif

#if(gInstantiableStackEnabled_d == 1)  
extern zclFoundationGlobals_t* pzclFoundationGlobals;
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif /* _ZCL_FOUNDATION_INTERFACE_H_ */

