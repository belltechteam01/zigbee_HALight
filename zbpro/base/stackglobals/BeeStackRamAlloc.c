/******************************************************************************
*  This is the Source file for the  BeeStack Ram Space Allocation.
*
* (c) Copyright 2013, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "BeeStack_Globals.h"
#include "nwkcommon.h"
#include "ZdoNwkInterface.h"
#include "NwkVariables.h"
#include "ApsVariables.h"
#include "AfVariables.h"
#include "ZdoVariables.h"
#include "BeeStack_Globals.h"
#include "BeeStackRamAlloc.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define gUniqueElement_d 0xFFFF

/* Defines for offsets inside structures */ 
#define TSizeOfs(value)     MbrOfs(zbStackTablesSizes_t, value)
#define GPSOfs(value)       MbrOfs(zbStackStuctPointers_t, value)
#define NwkGOfs(value)      MbrOfs(zbBeeStackNwkGlobals_t, value)
#define ApsGOfs(value)      MbrOfs(zbBeeStackApsGlobals_t, value)
#define ZdoPrOfs(value)     MbrOfs(zbZdoPrivateData_t, value)
#define BeeStackGOfs(value) MbrOfs(beeStackGlobalsParams_t, value) 


/******************************************************************************
*******************************************************************************
* Private prototypes
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
* Private memory declarations
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public Memory Declarations 
*******************************************************************************
*******************************************************************************/

/* pointers to main beestack structures */ 
#if(gInstantiableStackEnabled_d == 1)  
zbStackStuctPointers_t* pZbStackStuctPointersTable[gZbMaxInstanceCount_c] = {NULL};
zbStackTablesSizes_t* pZbStackTablesSizes = NULL;
#endif

#if(gInstantiableStackEnabled_d == 1)  
const zbStackTablesSizes_t zbStackDefaultInitTablesSizes =
{
    /* nwk */
  gNwkInfobaseMaxNeighborTableEntry_c,           /* gNwkInfobaseMaxNeighborTableEntry;*/
#if gNwkSmartSiblingReplacement_c  
   gNwkInfobaseMaxNeighborTableEntry_c * (gNwkInfobaseMaxNeighborTableEntry_c/8 + 1),  /* gMaxNeighborRelationsTableEntries */
#else
   1,                                                                                  /* gMaxNeighborRelationsTableEntries */
#endif   
  gNwkInfobaseMaxNeighborTableEntry_c,           /* gIncomingFrameCounterSetLimit; */
  gPacketsOnHoldTableSize_c,                     /* gPacketsOnHoldTableSize; */
  gHandleTrackingTableSize_c,                    /* gHandleTrackingTableSize, */
#if ( !gNwkBroadcastPassiveAckRetryCapability_d ) || (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)   
  gMaxBroadcastTransactionTableEntries_c,        /* gMaxBroadcastTransactionTableEntries */
#endif  
  gNwkRoutingMaxRouteDiscoveyTableEntry_c,       /* gMaxRouteDiscoveryTableEntries */
#if gConcentratorFlag_d
  gNwkInfobaseMaxSourceRouteTableEntry_c,        /* gMaxSourceRouteEntries */
#else
  1,
#endif

  /* aps */
  gApsMaxSecureMaterialEntries_c,                /* gApsMaxSecureMaterialEntries; */
  gApsMaxLinkKeys_c,                             /* gApsKeySetCount; */
  gApsAddressMapBitMaskInBits_c,                 /* gApsAddressMapBitMaskInBits; */
  gApsMaxAddrMapEntries_c,                       /* gApsMaxAddrMapEntries; */
  gMaximumApsBindingTableEntries_c,              /* gApsMaxBindingEntries; */
  gApsMaxGroups_c,                               /* gApsMaxGroups; */
  gApsMaxDataHandlingCapacity_c,                 /* gMaxApsTxTableEntries; */
  gApsRxFragmentationCapacity_c,                 /* gMaxApsRxFragmentationTableEntries; */
  gApscMinDuplicationRejectionTableSize_c,       /* giApsDuplicationRejectionTableSize; */
  gDefaulEntriesInSKKEStateMachine_c,            /* gSKKEStateMachineSize;*/
#if gHighSecurity_d 
  gDefaultEntriesInEAStateMachine_c,             /* gEAMaterialSize; */
#endif
#if gApsMaxEntriesForPermissionsTable_c
  gApsMaxEntriesForPermissionsTable_c,           /* gApsMaxEntriesForPermissionsTable; */
#endif                                           /* gApsMaxEntriesForPermissionsTable_c */
#if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) && gDefaultValueOfMaxEntriesForExclusionTable_c
  gDefaultValueOfMaxEntriesForExclusionTable_c,   /* gExclusionMax; */
#endif
#if gICanHearYouTableCapability_d
  gDefaultValueOfMaxEntriesForICanHearYouTable_c, /* gICanHearYouMax; */
#endif
  (gNumberOfEndPoints_c + 2),                      /* gNoOfEndPoints; */
#if gBkup_Discovery_cache_d || gDiscovery_store_rsp_d || gMgmt_Cache_rsp_d || gRemove_node_cache_rsp_d || gFind_node_cache_rsp_d|| gActive_EP_store_rsp_d || gPower_Desc_store_rsp_d || gNode_Desc_store_rsp_d || gSimple_Desc_store_rsp_d    
  gMaximumDiscoveryStoreTableSize_c,                /* gMaximumDiscoveryStoreTableSize */
#endif    
#if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d  
  mEdbMaxEntries_c, /* mEdbMaxEntries */
#endif
#if gMgmt_NWK_Disc_rsp_d
  gMaxArray_d,  /* gMaxArray */
#endif
#if gBind_Register_rsp_d || gRecover_Source_Bind_rsp_d || gBackup_Source_Bind_rsp_d || (gBindCapability_d && gBind_rsp_d)
  gMaximumDevicesHoldingBindingInfo_c, /* gMaximumDevicesHoldingBindingInfo */
#endif
#if gSystem_Server_Discovery_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gRecover_Bind_Table_rsp_d || gBackup_Source_Bind_rsp_d || gRecover_Source_Bind_rsp_d || gReplace_Device_rsp_d
  gMaximumSystemServerciesResponses_c /* gMaximumSystemServerciesResponses_c */
#endif  
};
#endif

#if(gInstantiableStackEnabled_d == 1)  
const beeStackRamSpaceAlloc_t beeStackRamSpaceTable[]=
{
  {GPSOfs(pZbStackTablesSizes)   , sizeof(zbStackTablesSizes_t)           , gUniqueElement_d                                                   , 0},
  /* Nwk */
  {GPSOfs(pZbNwkPrivateData)     , sizeof(zbNwkPrivateData_t)             , gUniqueElement_d                                                   , 0},
  {GPSOfs(pZbNwkPublicData)      , sizeof(zbNwkPublicData_t)              , gUniqueElement_d                                                   , 0},
  {GPSOfs(pZbBeeStackNwkGlobals) , sizeof(zbBeeStackNwkGlobals_t)         , gUniqueElement_d                                                   , 0},
#if gNwkSymLinkCapability_d && (( gCoordinatorCapability_d || gRouterCapability_d ) || gComboDeviceCapability_d)    
  /* gaNeighborTableAscendantSorted */
  {GPSOfs(pZbBeeStackNwkGlobals) , sizeof(uint8_t)                        , TSizeOfs(gMaxNeighborTableEntries)             ,  NwkGOfs(gaNeighborTableAscendantSorted)},
#endif  
  /* gaPacketsOnHoldTable */
  {GPSOfs(pZbBeeStackNwkGlobals) , sizeof(packetsOnHoldTable_t)           , TSizeOfs(gPacketsOnHoldTableSize)              , NwkGOfs(gaPacketsOnHoldTable)},
  /* gaHandleTrackingTable */
  {GPSOfs(pZbBeeStackNwkGlobals) , sizeof(handleTrackingTable_t)          , TSizeOfs(gHandleTrackingTableSize)             , NwkGOfs(gaHandleTrackingTable)},
#if gStandardSecurity_d || gHighSecurity_d  
  /* gaIncomingFrameCounterSet1 */
  {GPSOfs(pZbBeeStackNwkGlobals)  , sizeof(zbIncomingFrameCounterSet_t)    , TSizeOfs(gIncomingFrameCounterSetLimit)        , NwkGOfs(gaIncomingFrameCounterSet1)},
  /* gaIncomingFrameCounterSet2 */
  {GPSOfs(pZbBeeStackNwkGlobals)  , sizeof(zbIncomingFrameCounterSet_t)    , TSizeOfs(gIncomingFrameCounterSetLimit)        , NwkGOfs(gaIncomingFrameCounterSet2)},
#endif /*gStandardSecurity_d || gHighSecurity_d*/  
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
  #if gRnplusCapability_d  
  /* gaRouteTable */
  {GPSOfs(pZbBeeStackNwkGlobals) , sizeof(routeTable_t)                   , TSizeOfs(gMaxNeighborTableEntries)             , NwkGOfs(gaRouteTable)},
  #endif
#endif  
  /* gaNeighborTable */
  {GPSOfs(pZbBeeStackNwkGlobals) , sizeof(neighborTable_t)                , TSizeOfs(gMaxNeighborTableEntries)              , NwkGOfs(gaNeighborTable)},
  {GPSOfs(pZbBeeStackNwkGlobals) , sizeof(uint16_t)                        , TSizeOfs(gMaxNeighborRelationsTableEntries)     , NwkGOfs(gNeighborRelationsTable)},
#if ( !gNwkBroadcastPassiveAckRetryCapability_d ) || (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)  
  /* gaBroadcastTransactionTable */
  {GPSOfs(pZbBeeStackNwkGlobals) , sizeof(broadcastTransactionTable_t)    , TSizeOfs(gMaxBroadcastTransactionTableEntries)  , NwkGOfs(gaBroadcastTransactionTable)},
#endif  
  /* gaRouteDiscoveryTable */
  {GPSOfs(pZbBeeStackNwkGlobals) , sizeof(routeDiscoveryTable_t)          , TSizeOfs(gMaxRouteDiscoveryTableEntries)        , NwkGOfs(gaRouteDiscoveryTable)},
  /* gaSourceRouteTable */
  {GPSOfs(pZbBeeStackNwkGlobals) , sizeof(sourceRouteTable_t)             , TSizeOfs(gMaxSourceRouteEntries)                , NwkGOfs(gaSourceRouteTable)},
    
  /* APS */
  {GPSOfs(pZbApsPrivateData)     , sizeof(zbApsPrivateData_t)             , gUniqueElement_d                                                   , 0},
  {GPSOfs(pZbApsPublicData)      , sizeof(zbApsPublicData_t)              , gUniqueElement_d                                                   , 0},
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(zbBeeStackApsGlobals_t)         , gUniqueElement_d                                                   , 0},
  /* gaApsDeviceKeyPairSet */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(zbApsDeviceKeyPairSet_t)        , TSizeOfs(giApsDeviceKeyPairCount)              , ApsGOfs(gaApsDeviceKeyPairSet)},
  /* gaApsKeySet */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(zbAESKey_t)                     , TSizeOfs(gApsKeySetCount)                      , ApsGOfs(gaApsKeySet)},
#if (gApsAckCapability_d)
  /* gaApsTxTimerIDTable */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(zbTmrTimerID_t)                   , TSizeOfs(gMaxApsTxTableEntries)                , ApsGOfs(gaApsTxTimerIDTable)},
#endif /*gApsAckCapability_d*/
  /* gaApsAddressMapBitMask */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(uint8_t)                        , TSizeOfs(gApsAddressMapBitMaskInBits)          , ApsGOfs(gaApsAddressMapBitMask)},
  /* gaApsAddressMap */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(zbAddressMap_t)                 , TSizeOfs(gApsMaxAddressMapEntries)             , ApsGOfs(gaApsAddressMap)},
  /* gaApsBindingTable */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(apsBindingTable_t)              , TSizeOfs(gApsMaxBindingEntries)                , ApsGOfs(gaApsBindingTable)},
  /* gaApsGroupTable */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(zbGroupTable_t)                 , TSizeOfs(gApsMaxGroups)                        , ApsGOfs(gaApsGroupTable)},
   /* gaApsTxTable */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(apsTxTable_t)                   , TSizeOfs(gMaxApsTxTableEntries)                , ApsGOfs(gaApsTxTable)},
  /* apsTxFragCtrlSettings */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(apsTxFragCtrlSettings_t)        , TSizeOfs(gMaxApsTxTableEntries)                , ApsGOfs(apsTxFragCtrlSettings)},
  /* gApsRxFragmentationTable */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(apsRxFragmentationTable_t)      , TSizeOfs(gMaxApsRxFragmentationTableEntries)   , ApsGOfs(gApsRxFragmentationTable)},
  /* gaApsDuplicateRejectionTable */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(zbApsDuplicateRejectionTable_t) , TSizeOfs(giApsDuplicationRejectionTableSize)   , ApsGOfs(gaApsDuplicateRejectionTable)},
  
  /* apSKKEMaterial */
  {GPSOfs(pZbBeeStackApsGlobals), sizeof(zbKeyEstablish_t*)              , TSizeOfs(gSKKEStateMachineSize)     , ApsGOfs(apSKKEMaterial)},
#if gHighSecurity_d
  /* gapEntityAuthenticationMaterial */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(zbEntityAuthentication_t*)      , TSizeOfs(gEAMaterialSize)      , ApsGOfs(gapEntityAuthenticationMaterial)},
#endif
#if gApsMaxEntriesForPermissionsTable_c
  /* gaPermissionsTable */
  {GPSOfs(pZbBeeStackApsGlobals) , sizeof(permissionsTable_t)             , TSizeOfs(gApsMaxEntriesForPermissionsTable)    , ApsGOfs(gaPermissionsTable)},
#endif /* gApsMaxEntriesForPermissionsTable_c */
  
  /* AF */
  {GPSOfs(pZbAfPrivateData)      , sizeof(zbAfPrivateData_t)              , gUniqueElement_d                                                   , 0},
  
  /* ZDO */
  {GPSOfs(pZbZdoPrivateData)       , sizeof(zbZdoPrivateData_t)           , gUniqueElement_d                                                   , 0},
#if gBkup_Discovery_cache_d || gDiscovery_store_rsp_d || gMgmt_Cache_rsp_d || gRemove_node_cache_rsp_d || gFind_node_cache_rsp_d|| gActive_EP_store_rsp_d || gPower_Desc_store_rsp_d || gNode_Desc_store_rsp_d || gSimple_Desc_store_rsp_d
  /* gaDiscoveryStoreTable */
  {GPSOfs(pZbZdoPrivateData)       , sizeof(discoveryStoreTable_t)        , TSizeOfs(gMaximumDiscoveryStoreTableSize)      , ZdoPrOfs(gaDiscoveryStoreTable)},
#endif
#if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d
  /* maEdbTable */
  {GPSOfs(pZbZdoPrivateData)       , sizeof(zdoEdbTable_t)                , TSizeOfs(mEdbMaxEntries)                       , ZdoPrOfs(maEdbTable)},
#endif
#if gMgmt_NWK_Disc_rsp_d
  /* strAddrClusterId */ 
  {GPSOfs(pZbZdoPrivateData)       , sizeof(strAddrClusterId_t)           , TSizeOfs(gMaxArray)                            , ZdoPrOfs(strAddrClusterId)},
#endif
#if gBind_Register_rsp_d || gRecover_Source_Bind_rsp_d || gBackup_Source_Bind_rsp_d || (gBindCapability_d && gBind_rsp_d)
  /* aDevicesHoldingItsOwnBindingInfo */
  {GPSOfs(pZbZdoPrivateData)       , sizeof(zbIeeeAddr_t)                 , TSizeOfs(gMaximumDevicesHoldingBindingInfo)    , ZdoPrOfs(aDevicesHoldingItsOwnBindingInfo)},
#endif
#if gSystem_Server_Discovery_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gRecover_Bind_Table_rsp_d || gBackup_Source_Bind_rsp_d || gRecover_Source_Bind_rsp_d || gReplace_Device_rsp_d
  /* mSystemServerDiscResponse */
  {GPSOfs(pZbZdoPrivateData)      , sizeof(zbSystemServerDiscoveryStore_t), TSizeOfs(gMaximumSystemServerciesResponses)    , ZdoPrOfs(mSystemServerDiscResponse)},
#endif
#if (gReplace_Device_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRecover_Bind_Table_rsp_d || gBind_Register_rsp_d || gMgmt_Bind_rsp_d)
  /* gaBindingCacheTable */
  {GPSOfs(pZbZdoPrivateData)      , sizeof(zbApsmeBindEntry_t), TSizeOfs(gMaximumBindingCacheTableList)    , ZdoPrOfs(gaBindingCacheTable)},
#endif
  {GPSOfs(pZbZdoPublicData)       , sizeof(zbZdoPublicData_t)            , gUniqueElement_d                                                   , 0},
  
  
  /* BeeStack */
  {GPSOfs(pBeeStackGlobalsParams), sizeof(beeStackGlobalsParams_t)        , gUniqueElement_d                                                   , 0},
#if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) && gDefaultValueOfMaxEntriesForExclusionTable_c
  /* gaExclusionTable */
  {GPSOfs(pBeeStackGlobalsParams), sizeof(zbIeeeAddr_t)                   , TSizeOfs(gExclusionMax), BeeStackGOfs(gaExclusionTable)},
#endif  
#if gICanHearYouTableCapability_d
  /* gaICanHearYouTable */
  {GPSOfs(pBeeStackGlobalsParams), sizeof(zbNwkAddr_t)                    , TSizeOfs(gICanHearYouMax), BeeStackGOfs(gaICanHearYouTable)},
  /* gaICanHearYouLqi */ 
  {GPSOfs(pBeeStackGlobalsParams), sizeof(uint8_t)                        , TSizeOfs(gICanHearYouMax), BeeStackGOfs(gaICanHearYouLqi)},
#endif
  /* gaEndPointDesc */
  {GPSOfs(pBeeStackGlobalsParams), sizeof(endPointPtrArray_t)             , TSizeOfs(gNoOfEndPoints), BeeStackGOfs(gaEndPointDesc)},
  /* gaActiveEndPointList */
  {GPSOfs(pBeeStackGlobalsParams), sizeof(zbEndPoint_t)                   , TSizeOfs(gNoOfEndPoints), BeeStackGOfs(gaActiveEndPointList)}
};
#endif
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
/***************************************************************************
*  Allocates the ram space for a beestack instance 
*  Also verify that providded space is enough
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   gZbInvalidParameter_c - arguments are not valid
*   gZbNoMem_c            - allocated space is not enough
****************************************************************************/
#if(gInstantiableStackEnabled_d == 1)  
zbStatus_t BeeStackAllocRamSpace
(
  const zbStackTablesSizes_t* pZbStackTablesSizes, 
  void*                 pRamStart,
  uint16_t              ramSize,
  uint8_t               beeStackInstanceId  
)
{
  
  uint8_t index;
  uint32_t ramOffset = (uint32_t)pRamStart + sizeof(zbStackStuctPointers_t);
  zbStackStuctPointers_t* pZbStackStuctPointers = (zbStackStuctPointers_t*)pRamStart;
   
  if((NULL == pZbStackTablesSizes) ||
     (NULL == pRamStart) ||
     (ramSize == 0))
  {
    return gZbInvalidParameter_c;
  }
  
  for(index = 0; index < sizeof(beeStackRamSpaceTable)/sizeof(beeStackRamSpaceAlloc_t); index++)
  {
    uint32_t* pStruct = (uint32_t*)((uint8_t*)pZbStackStuctPointers + beeStackRamSpaceTable[index].mainOffset);
    
    if(beeStackRamSpaceTable[index].structSizeOffset == gUniqueElement_d)
    {
      *pStruct = ramOffset;
      ramOffset +=  beeStackRamSpaceTable[index].elementSize;
    }
    else
    {
      uint32_t* pAddOfset = (uint32_t*)((uint32_t)*pStruct + beeStackRamSpaceTable[index].secondOffset);
      *pAddOfset = ramOffset;
      ramOffset +=  beeStackRamSpaceTable[index].elementSize * (*((uint8_t*)pZbStackTablesSizes+beeStackRamSpaceTable[index].structSizeOffset));
    }
    /* check if needed ram space is enough */
    if(ramOffset > ramSize + (uint32_t)pRamStart)
    { 
      return  gZbNoMem_c;
    }
  }
  
  /* Init beestack - sizes structure */
  FLib_MemCpy((void*)pZbStackStuctPointers->pZbStackTablesSizes,
               (void*)pZbStackTablesSizes,
               sizeof(zbStackTablesSizes_t));
  
  pZbStackStuctPointersTable[beeStackInstanceId] = pZbStackStuctPointers;
  
  return gZbSuccess_c;
}
#endif
/***************************************************************************
*  Caluculate RAM space needded for an instance ID
*  
*
* Interface assumptions:
*   NONE.
*
* Return value:
*    
****************************************************************************/
#if(gInstantiableStackEnabled_d == 1)
zbStatus_t BeeStackGetRamSpace
(
  const zbStackTablesSizes_t* pZbStackTablesSizes, 
  uint16_t*             ramSize
)
{
    
  uint8_t index;
  if((NULL == pZbStackTablesSizes) ||
     (NULL == ramSize))
  {
    return gZbInvalidParameter_c;
  }
  *ramSize = sizeof(zbStackStuctPointers_t);
  
  for(index = 0; index < sizeof(beeStackRamSpaceTable)/sizeof(beeStackRamSpaceAlloc_t); index++)
  {
    *ramSize +=  beeStackRamSpaceTable[index].elementSize * 
                  ((beeStackRamSpaceTable[index].structSizeOffset == gUniqueElement_d)? 
                   1 : *((uint8_t*)pZbStackTablesSizes+beeStackRamSpaceTable[index].structSizeOffset));
  }
  
  return gZbSuccess_c;
}
#endif
/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/* None */ 
