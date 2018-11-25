/******************************************************************************
* This is the Header file for the  BeeStack Non Volatile Memory Management.
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
#ifndef _BEESTACK_NVMManagement_H_
#define _BEESTACK_NVMManagement_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/


typedef uint8_t zdoNvmObject_t;
// save on interval
#define zdoNvmObject_RoutingTable_c             0x01
#define zdoNvmObject_NeighborTable_c            0x02
#define zdoNvmObject_AddrMap_c                  0x03
#define zdoNvmObject_GroupTable_c               0x04
#define zdoNvmObject_BindingTable_c             0x05
#define zdoNvmObject_ApsLinkKeySet_c            0x06  // APS device key pair set.
#define zdoNvmObject_NwkData_c                  0x07  // PIB, permitjoining, etc..
#define zdoNvmObject_ZclData_c                  0x08
// save on idle
#define zdoNvmObject_All_c                      0x09
// save on count
#define zdoNvmObject_NwkOutgoingSecCounters_c   0x0A
#define zdoNvmObject_NwkIncomingSecCounters_c   0x0B
#define zdoNvmObject_ApsSecurityCounters_c      0x0C
#define zdoNvmObject_SecureSet_c                0x0D
#define zdoNvmObject_AtomicSave_c               0x0E  // All data set, no waiting.
#define zdoNvmObject_AddrMapPermanent_c         0x0F

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

typedef enum{
  gNvSaveOnInterval_c, 
  gNvSaveOnCount_c,   
  gNvSaveOnIdle_c   
}nvSaveType_t;

/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/

extern bool_t ZDO_NvmRestoreAllData(void);

extern void   ZdoNwkMng_SaveToNvm(zdoNvmObject_t iNvmObject, void* pAddress);

extern void   ZdoNwkMng_SaveDataSetPtr(void* pAddress, nvSaveType_t nvSaveType, bool_t saveAll);

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

/* None */
#ifdef __cplusplus
}
#endif

#endif /*_BEESTACK_NVMManagement_H_*/
