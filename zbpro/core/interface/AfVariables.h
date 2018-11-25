/***************************************************************************
* This is the header file for the AfVariables
*
* (c) Copyright 2013, Freescale, Inc. All rights reserved.
*
* Freescale Semiconductor Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/
#ifndef _AF_INTERNAL_VARIABLES_H_
#define _AF_INTERNAL_VARIABLES_H_

#ifdef __cplusplus
    extern "C" {
#endif
/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

#if(gInstantiableStackEnabled_d == 0)
  #define ZbAfPrivateData(val)           val 
#else
   #define ZbAfPrivateData(val)             pZbAfPrivateData->val
#endif
/*****************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
/* Af module internal used ram global structure */
#if(gInstantiableStackEnabled_d == 1)
typedef struct zbAfPrivateData_tag
{
#endif  

EXTERN  anchor_t gApsToAfdeQ;

#if(gInstantiableStackEnabled_d == 1)  
}zbAfPrivateData_t;
#endif
/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
#if(gInstantiableStackEnabled_d == 1)
/* pointer used to access instance id variables */
extern zbAfPrivateData_t* pZbAfPrivateData;
#endif
/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif /* _Af_INTERNAL_VARIABLES_H_ */
