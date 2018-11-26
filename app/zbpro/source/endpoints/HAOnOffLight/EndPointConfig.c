/******************************************************************************
* This is the Source file for device End point registration
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
#include "AppZdoInterface.h"
#include "AppAfInterface.h"
#include "AppZdoInterface.h"
#include "ZdpManager.h"

#include "ZbAppInterface.h"
#include "EndPointConfig.h"


#include "HaProfile.h"


/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/
/* None */

/******************************************************************************
*******************************************************************************
* Public type definitions
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

/* None */

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
#if gNum_EndPoints_c != 0  

/* BEGIN Endpoint Descriptor Section */

/*-- Endpoint Start --
Application name: HA OnOffLight application
Description: HA OnOffLight endpoint description.
*/

const uint8_t Endpoint8_InputClusterList[10] = {
  0x0, 0x0,  /* Basic */
  0x3, 0x0,  /* Identify */
  0x4, 0x0,  /* Groups */
  0x5, 0x0,  /* Scenes */
  0x6, 0x0  /* OnOff */
};

const uint8_t Endpoint8_OutputClusterList[1];

const zbSimpleDescriptor_t Endpoint8_simpleDescriptor = {
  8, /* Endpoint number */
  0x4, 0x1, /* Application profile ID */
  0x0, 0x1, /* Application device ID */
  0, /* Application version ID */
  5, /* Number of input clusters */
  (uint8_t *) Endpoint8_InputClusterList, /* Input cluster list */
  0, /* Number of output clusters */
  (uint8_t *) Endpoint8_OutputClusterList, /* Output cluster list */
};


const zbSimpleDescriptor_t Endpoint9_simpleDescriptor = {
  9, /* Endpoint number */
  0x4, 0x1, /* Application profile ID */
  0x0, 0x1, /* Application device ID */
  0, /* Application version ID */
  5, /* Number of input clusters */
  (uint8_t *) Endpoint8_InputClusterList, /* Input cluster list */
  0, /* Number of output clusters */
  (uint8_t *) Endpoint8_OutputClusterList, /* Output cluster list */
};


endPointDesc_t Endpoint8_EndPointDesc = {
  (zbSimpleDescriptor_t *)&Endpoint8_simpleDescriptor,  /* pointer to the simple descriptor stored for the endpoint */
  AppMsgCallBack,        /* Callback function for MSG data */
  AppCnfCallBack,        /* Callback function for data confirmation */
gApsWindowSizeDefault_c        /* Values 1 - 8, 0 if fragmentation is not supported. */
};


endPointDesc_t Endpoint9_EndPointDesc = {
  (zbSimpleDescriptor_t *)&Endpoint9_simpleDescriptor,  /* pointer to the simple descriptor stored for the endpoint */
  AppMsgCallBack,        /* Callback function for MSG data */
  AppCnfCallBack,        /* Callback function for data confirmation */
gApsWindowSizeDefault_c        /* Values 1 - 8, 0 if fragmentation is not supported. */
};

/*-- Endpoint End --*/

/*- Endpoint list Start-*/

const endPointList_t endPointList[2] = {


  {&Endpoint8_EndPointDesc, &gHaOnOffLightDeviceDef},
  {&Endpoint9_EndPointDesc, &gHaOnOffLightDevice2Def}
};
#if(gInstantiableStackEnabled_d == 1)
const endPointData_t endPointDataTable[1] = 
{
  {
    &endPoint0Desc,
    &broadcastEndPointDesc,
    endPointList,
    gNum_EndPoints_c
  }
};

const endPointData_t* pEndPointData;
#endif/* gInstantiableStackEnabled_d == 1 */

/*- Endpoint list End-*/
/* END Endpoint Descriptor Section */
#endif

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

/* None */

/*****************************************************************************/

