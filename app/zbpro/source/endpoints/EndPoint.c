/******************************************************************************
* This is the Source file for device End point
*
* (c) Copyright 2014, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*
******************************************************************************/
#include "EmbeddedTypes.h"
#include "BeeStack_Globals.h"
#include "BeeStackConfiguration.h"
#include "EndPoint.h"

/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/
/* None */

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/


/* Simple Discriptor for 0xFF(Broadcast)*/
const zbSimpleDescriptor_t BroadcastEp =
{
  /*End Point(1Byte), Device Description(2Bytes), Profile ID (2 Bytes),
  AppDeviceVersionAndFlag(1Byte), NumOfInputClusters(1Byte),
  PointerToInputClusterList(1Byte), NumOfOutputClusters(1Byte),
  PointerToOutputClusterList(1Byte) */
  0xFF,
  0x00,0x00,    /* profile ID */
  0x00,0x00,    /* device ID */
  0x00,         /* flags - no user or complex descriptor */
  0x00,         /* # of input clusters */
  NULL,         /* ptr to input clusters */
  0x00,         /* # of output clusters */
  NULL          /* ptr to output clusters */
};


/* End point 0xFF(Broadcast) description */
endPointDesc_t broadcastEndPointDesc =
{
  (zbSimpleDescriptor_t *)&BroadcastEp,   /* simple descriptor */
  AppBroadcastMsgCallBack,                /* broadcast MSG callback */
  NULL,                                   /* no broadcast confirm callback */
  0x00                                    /* Values 1 - 8, 0 if fragmentation is not supported. */
};


/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/
/* None */

/*****************************************************************************/

