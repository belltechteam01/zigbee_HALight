/************************************************************************************
* This header contains  device End points defaults
*
* (c) Copyright 2014, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#ifndef _ENDPOINT_H_
#define _ENDPOINT_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "BeeStack_Globals.h"
#include "BeeStackConfiguration.h"
#include "AppAfInterface.h"

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/
#define GetEndpoint(Number) (endPointList[Number].pEndpointList->pSimpleDesc->endPoint)

#if(gInstantiableStackEnabled_d == 0)
  #define EndPointConfigData(val)     val
#else
  #define EndPointConfigData(val)      pEndPointData->val
#endif


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

typedef PACKED_STRUCT endPointList_tag {
  endPointDesc_t *pEndpointDesc;  /* pointer to descriptor */
  const afDeviceDef_t *pDevice;  /* pointer to device (or profile specific structure */
} endPointList_t;

#if(gInstantiableStackEnabled_d == 1)
typedef struct endPointData_tag
{

  endPointDesc_t* endPoint0Desc; 
  
  endPointDesc_t* broadcastEndPointDesc;
     
  endPointList_t const* endPointList;
 
  uint8_t gNum_EndPoints;
  
}endPointData_t;
#endif
	
/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/
void AppMsgCallBack( apsdeToAfMessage_t *pMsg);
void AppCnfCallBack( apsdeToAfMessage_t *pMsg );
void AppBroadcastMsgCallBack(apsdeToAfMessage_t *pMsg);

/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/

/* End point 0xFF(Broadcast) description */
extern endPointDesc_t broadcastEndPointDesc;
extern endPointDesc_t endPoint0Desc;

#if(gInstantiableStackEnabled_d == 0)
  extern const endPointList_t endPointList[]; 
#else /*(gInstantiableStackEnabled_d == 0)*/
  extern const  endPointData_t* pEndPointData;
  extern const  endPointData_t  endPointDataTable[];
#endif/*(gInstantiableStackEnabled_d == 0)*/

/*****************************************************************************/
#ifdef __cplusplus
}
#endif
#endif /* _ENDPOINT_H_*/
