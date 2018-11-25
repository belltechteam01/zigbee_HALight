/******************************************************************************
* This file contains the declarations for different BeeStack data stored in nvm
*
* Copyright (c) 2013, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/

/* Used for storing the NWK and APS data set in NVM */

NVM_TblEntry((&ZbBeeStackNwkGlobals(gNwkData)),(1),(sizeof(nvmNwkData_t)),(nvmId_NwkData_c))
NVM_TblEntry((&ZbBeeStackGlobalsParams(gBeeStackConfig)), (1),(sizeof(beeStackConfigParams_t)),(nvmId_BeeStackConfig_c))  
/* gaNvNTDataSet */
#if !gInstantiableStackEnabled_d  
  NVM_TblEntry((ZbBeeStackNwkGlobals(gaNeighborTable)), gNwkInfobaseMaxNeighborTableEntry_c, sizeof(neighborTable_t),nvmId_NeighborTable_c)
#else
  NVM_TblEntry((ZbBeeStackNwkGlobals(gaNeighborTable)), ZbStackTablesSizes(gMaxNeighborTableEntries), sizeof(neighborTable_t),nvmId_NeighborTable_c)    
#endif
  /* gaNvAddrMapDataSet */
#if !gInstantiableStackEnabled_d    
  NVM_TblEntry(ZbBeeStackApsGlobals(gaApsAddressMapBitMask), gApsAddressMapBitMaskInBits_c,                   sizeof(uint8_t), nvmId_ApsAddressMapBitMask_c)
#else
  NVM_TblEntry(ZbBeeStackApsGlobals(gaApsAddressMapBitMask), ZbStackTablesSizes(gApsAddressMapBitMaskInBits), sizeof(uint8_t), nvmId_ApsAddressMapBitMask_c)  
#endif    
#if !gInstantiableStackEnabled_d  
  NVM_TblEntry((ZbBeeStackApsGlobals(gaApsAddressMap)), gApsMaxAddrMapEntries_c, sizeof(zbAddressMap_t), nvmId_ApsAddressMap_c)
#else    
  NVM_TblEntry((ZbBeeStackApsGlobals(gaApsAddressMap)), ZbStackTablesSizes(gApsMaxAddressMapEntries), sizeof(zbAddressMap_t), nvmId_ApsAddressMap_c)
#endif
    
#if gRnplusCapability_d   
  NVM_TblEntry((ZbBeeStackNwkGlobals(gaRouteTable)), gNwkInfobaseMaxRouteTableEntry_c,    sizeof(routeTable_t), nvmId_RouteTable_c)        
#endif
     
#if gApsLinkKeySecurity_d
   NVM_TblEntry(ZbBeeStackApsGlobals(gaApsKeySet),               gApsMaxLinkKeys_c,                   sizeof(zbAESKey_t),                   nvmId_ApsKeySet_c)
   NVM_TblEntry(ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet),     gApsMaxSecureMaterialEntries_c,      sizeof(zbApsDeviceKeyPairSet_t),      nvmId_ApsDeviceKeyPairSet_c)
#endif
     
#if gNwkSecSaveIncomingCounters_c && gStandardSecurity_d
  #if !gInstantiableStackEnabled_d      
    NVM_TblEntry(ZbBeeStackNwkGlobals(gaIncomingFrameCounterSet1), gNwkInfobaseMaxNeighborTableEntry_c, sizeof(zbIncomingFrameCounterSet_t), nvmId_IncomingFrameCounterSet1_c)
  #else
    NVM_TblEntry(ZbBeeStackNwkGlobals(gaIncomingFrameCounterSet1), ZbStackTablesSizes(gMaxNeighborTableEntries), sizeof(zbIncomingFrameCounterSet_t), nvmId_IncomingFrameCounterSet1_c)    
  #endif
  #if !gInstantiableStackEnabled_d     
    NVM_TblEntry(ZbBeeStackNwkGlobals(gaIncomingFrameCounterSet2),  gNwkInfobaseMaxNeighborTableEntry_c, sizeof(zbIncomingFrameCounterSet_t), nvmId_IncomingFrameCounterSet2_c)
  #else
    NVM_TblEntry(ZbBeeStackNwkGlobals(gaIncomingFrameCounterSet2),  ZbStackTablesSizes(gMaxNeighborTableEntries), sizeof(zbIncomingFrameCounterSet_t), nvmId_IncomingFrameCounterSet2_c)  
  #endif    
#endif

NVM_TblEntry((&ZbBeeStackGlobalsParams(gSAS_Ram)),  1,  sizeof(zbCommissioningAttributes_t), nvmId_SAS_Ram_c)
#if !gInstantiableStackEnabled_d
  NVM_TblEntry(ZbBeeStackApsGlobals(gaApsGroupTable),       gApsMaxGroups_c, sizeof(zbGroupTable_t), nvmId_ApsGroupTable_c)
#else
  NVM_TblEntry(ZbBeeStackApsGlobals(gaApsGroupTable),  ZbStackTablesSizes(gApsMaxGroups), sizeof(zbGroupTable_t), nvmId_ApsGroupTable_c)    
#endif    
      
#ifndef gHostApp_d
  NVM_TblEntry((&ZbBeeStackApsGlobals(gBeeStackParameters)),  1,  sizeof(beeStackParameters_t), nvmId_BeeStackParameters_c)
  #if !gInstantiableStackEnabled_d
    NVM_TblEntry(ZbBeeStackApsGlobals(gaApsBindingTable),  gMaximumApsBindingTableEntries_c,  sizeof(apsBindingTable_t), nvmId_ApsBindingTable_c) 
  #else
    NVM_TblEntry(ZbBeeStackApsGlobals(gaApsBindingTable),  ZbStackTablesSizes(gApsMaxBindingEntries),  sizeof(apsBindingTable_t), nvmId_ApsBindingTable_c)  
  #endif    
#endif  
      