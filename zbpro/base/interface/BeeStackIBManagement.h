/******************************************************************************
* This is the Header file for the  BeeStack Information Base Management.
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
#ifndef _BEESTACK_IBManagement_H_
#define _BEESTACK_IBManagement_H_

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

/* Used for both scalar and vector IB entries. */
typedef struct zbIBReqParams_tag 
{           
  zbIBAttributeId_t attId;             /* Which IB entry is being requested. */
  index_t           iIndex;            /* Index of first entry to get. */
  index_t           iEntries;          /* # of entries to get or # returned. */
  uint8_t           iEntrySize;        /* Size of each entry returned. */
  void              *pData;            /* Ignored in req; # bytes returned. */
} zbIBReqParams_t;

typedef enum
{
  zbIBApsmeGetReq_c,
  zbIBApsmeSetReq_c,
  zbIBNlmeGetReq_c,
  zbIBNlmeSetReq_c
}zbIBRequest_t;

/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
/* None */
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
/******************************************************************************
* Retrieve or Store information from the NWK or APS Information Base.
******************************************************************************/

zbStatus_t BeeStack_GetSetAttrValue(zbIBRequest_t zbIBRequest, uint8_t instId,  zbIBReqParams_t *pZbIBRequest);
#ifdef __cplusplus
}
#endif
#endif /*_BEESTACK_IBManagement_H_*/
