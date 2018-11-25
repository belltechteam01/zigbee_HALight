/******************************************************************************
* This is the Header file for the  BeeStack RAM Alloc.
*
*
* ( c ) Copyright 2013, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#ifndef _BEESTACK_RAMAlloc_H_
#define _BEESTACK_RAMAlloc_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"


/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
typedef struct beeStackRamSpaceAlloc_tag
{
  uint8_t    mainOffset;
  uint32_t   elementSize;
  uint16_t   structSizeOffset;
  uint8_t    secondOffset;
}beeStackRamSpaceAlloc_t;

#if(gInstantiableStackEnabled_d == 1)  
typedef struct zbStackStuctPointers_tag
{
  /* Nwk data structures*/
  zbNwkPrivateData_t*      pZbNwkPrivateData;
  zbNwkPublicData_t*       pZbNwkPublicData;
  zbBeeStackNwkGlobals_t*  pZbBeeStackNwkGlobals;
  
  /* APS data structures*/
  zbApsPrivateData_t*      pZbApsPrivateData;
  zbApsPublicData_t*       pZbApsPublicData;
  zbBeeStackApsGlobals_t*  pZbBeeStackApsGlobals;
  
  /* AF data structures*/
  zbAfPrivateData_t*       pZbAfPrivateData;
  
  /* ZDO data structures*/
  zbZdoPrivateData_t*      pZbZdoPrivateData;
  zbZdoPublicData_t*       pZbZdoPublicData;
  
  /* General data structures*/
  beeStackGlobalsParams_t* pBeeStackGlobalsParams;
  
  /* Instance table sizes */
  zbStackTablesSizes_t*    pZbStackTablesSizes;
}zbStackStuctPointers_t;
#endif
/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
#if(gInstantiableStackEnabled_d == 1)
extern zbStackTablesSizes_t* pZbStackTablesSizes;  
extern zbStackStuctPointers_t* pZbStackStuctPointersTable[gZbMaxInstanceCount_c];
extern const beeStackRamSpaceAlloc_t beeStackRamSpaceTable[];
extern const zbStackTablesSizes_t zbStackDefaultInitTablesSizes;
#endif
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
#if(gInstantiableStackEnabled_d == 1)  
zbStatus_t BeeStackGetRamSpace
(
  const zbStackTablesSizes_t* pZbStackTablesSizes, 
  uint16_t*             ramSize
);

zbStatus_t BeeStackAllocRamSpace
(
  const zbStackTablesSizes_t* pZbStackTablesSizes, 
  void*                 pRamStart,
  uint16_t              ramSize,
  uint8_t               beeStackInstanceId  
);
#endif

#ifdef __cplusplus
}
#endif
#endif /*_BEESTACK_RAMAlloc_H_*/
