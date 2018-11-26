/************************************************************************************
*
* Copyright (c) 2012, Freescale, Inc.  All rights reserved.
*
* No part of this document may be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#include "EmbeddedTypes.h"
//#include "AppToPlatformConfig.h"

#if gMAC_PHY_INCLUDED_c
  #include "NwkMacInterface.h"
#endif
/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

#if gMAC_PHY_INCLUDED_c
#ifndef gMAC2006_d
  /* Application allocated space for MAC PIB ACL Entry descriptors. */
#if gNumAclEntryDescriptors_c > 0
  aclEntryDescriptor_t gPIBaclEntryDescriptorSet[gNumAclEntryDescriptors_c];

    /* Set number of ACL entries. Used by the MAC. */
  const uint8_t gNumAclEntryDescriptors = gNumAclEntryDescriptors_c;
  #endif /* gNumAclEntryDescriptors_c */
#else
  
  #if gNumKeyTableEntries_c > 0
    KeyDescriptor_t gPIBKeyTable[gNumKeyTableEntries_c];
    const uint8_t gNumKeyTableEntries = gNumKeyTableEntries_c;
    
    /* Allocate KeyIdLookupDescriptor_t, KeyDeviceDescriptor_t, KeyUsageDescriptor_t */ 
    /* These arrays are part of KeyDescriptor_t structure */
    /* Allocate a continuous space for each array based on the gNumKeyTableEntries_c */
    /* The MAC PIB will initialize the pointers accordingly */
    
    #if gNumKeyIdLookupEntries_c > 0
       KeyIdLookupDescriptor_t gPIBKeyIdLookupDescriptorTable[gNumKeyIdLookupEntries_c * gNumKeyTableEntries_c];
       const uint8_t gNumKeyIdLookupEntries = gNumKeyIdLookupEntries_c; /* The number of elements in each virtual array inside the gPIBKeyIdLookupDescriptorTable */
    #endif //gNumKeyIdLookupEntries_c
    
    #if gNumKeyDeviceListEntries_c > 0
       KeyDeviceDescriptor_t  gPIBKeyDeviceDescriptorTable[gNumKeyDeviceListEntries_c * gNumKeyTableEntries_c];
       const uint8_t gNumKeyDeviceListEntries = gNumKeyDeviceListEntries_c; /* The number of elements for each virtual array inside the gPIBKeyDeviceDescriptorTable */
    #endif //gNumKeyDeviceListEntries_c
    
    #if gNumKeyUsageListEntries_c > 0
       KeyUsageDescriptor_t  gPIBKeyUsageDescriptorTable[gNumKeyUsageListEntries_c * gNumKeyTableEntries_c];
       const uint8_t gNumKeyUsageListEntries = gNumKeyUsageListEntries_c; /* The number of elements for each virtual array inside the gPIBKeyDeviceDescriptorTable */
    #endif //gNumKeyUsageListEntries_c
  #endif //gNumKeyTableEntries_c
  
  #if gNumDeviceTableEntries_c > 0
    DeviceDescriptor_t gPIBDeviceTable[gNumDeviceTableEntries_c];
    const uint8_t gNumDeviceTableEntries = gNumDeviceTableEntries_c;    
  #endif //gNumDeviceTableEntries_c

  #if gNumSecurityLevelTableEntries_c > 0
    SecurityLevelDescriptor_t gPIBSecurityLevelTable[gNumSecurityLevelTableEntries_c];
    const uint8_t gNumSecurityLevelTableEntries = gNumSecurityLevelTableEntries_c;    
  #endif //gNumKeyTableEntries_d


#endif  /*gMAC2006_d*/
#endif /*gMAC_PHY_INCLUDED_c*/
/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

