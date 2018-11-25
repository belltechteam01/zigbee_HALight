/******************************************************************************
*  This is the Source file for the  BeeStack Non Volatile Memory Management.
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

#include "NVM_interface.h"
#include "NV_Data.h"
#include "BeeStackNVMManagement.h"
#include "BeeStackInterface.h"
#include "BeeStack_Globals.h"
#include "ZigbeeTask.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/*
 * Name: gNvIncrementAfterRestore_c
 * Description: see gNvCountsBetweenSaves_c
 */
#ifndef gNvIncrementAfterRestore_c
#define gNvIncrementAfterRestore_c      gNvCountsBetweenSaves_c
#endif

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
* Public functions
*******************************************************************************
******************************************************************************/
/*
  Restore all the data

  Important: gNvNwkAndApsSecDataSet_ID_c must be restored after the gNvNwkDataSet_ID_c
*/
bool_t ZDO_NvmRestoreAllData(void)
{
#if gNvStorageIncluded_d    
  uint8_t i=0;

  while(gNvEndOfTableId_c != NVM_DataTable[i].DataEntryID)
  { 
    if(((zbProInstance) != (NVM_DataTable[i].DataEntryID>>12))||
       (NULL == NVM_DataTable[i].pData))
    {
      i++;
      continue;
    }
    
    if(NvRestoreDataSet(NVM_DataTable[i].pData, TRUE))
    {
       return FALSE;
    }
    i++;
  }  
  return TRUE;
#else
  return FALSE;
#endif  
}

/*
  This function shearches the data set ID that corresponds to a particular data, 
  (specified by the pAddress pointer) and save the data.
*/
void ZdoNwkMng_SaveDataSetPtr(void* pAddress, nvSaveType_t nvSaveType, bool_t saveAll)
{
#if gNvStorageIncluded_d
    if (nvSaveType == gNvSaveOnInterval_c)
    {
      #if gEndDevCapability_d || gComboDeviceCapability_d
        #if gComboDeviceCapability_d
        if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
        #endif
        if (IsLocalDeviceReceiverOnWhenIdle())
        {
          (void)NvSaveOnInterval(pAddress);
        }
        else
        {
          (void)NvSaveOnIdle(pAddress,TRUE);
        }
        #if gComboDeviceCapability_d
        else
         (void)NvSaveOnInterval(pAddress);
        #endif
      #else  
      (void)NvSaveOnInterval(pAddress);
      #endif
    }
    else if (nvSaveType == gNvSaveOnCount_c)
      NvSaveOnCount(pAddress);
    else
      NvSaveOnIdle(pAddress,saveAll);
#else
    (void)pAddress;
    (void)nvSaveType;
    (void)saveAll;
#endif    
}

/*
  Save all NVM Data
*/
void ZdoNwkMng_SaveAllData(void)
{
#if gNvStorageIncluded_d  
  uint8_t i = 0;

  while(gNvEndOfTableId_c != NVM_DataTable[i].DataEntryID)
  {
    if(((zbProInstance) != (NVM_DataTable[i].DataEntryID>>12))||
       (NULL == NVM_DataTable[i].pData))
    {
      i++;
      continue;
    }
    NvSaveOnIdle(NVM_DataTable[i].pData, TRUE);
    i++;
  }
#endif  
}

/* This function is used to save data appropriately (usually interval) based on object type */
void ZdoNwkMng_SaveToNvm(zdoNvmObject_t iNvmObject, void* pAddress)
{
  switch(iNvmObject)
  {
    /* Save network data and/or neighbor table. */
    case zdoNvmObject_NwkData_c:
      ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackNwkGlobals(gNwkData), gNvSaveOnInterval_c, TRUE);
      break;
    case zdoNvmObject_NeighborTable_c:
      if(pAddress)
      {
        ZdoNwkMng_SaveDataSetPtr(pAddress, gNvSaveOnIdle_c, FALSE);
      }
      else
      {
      ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackNwkGlobals(gaNeighborTable), gNvSaveOnInterval_c, TRUE);
      }
      break;
      
    /* Saves address map */  
    case zdoNvmObject_AddrMapPermanent_c:
      if(pAddress)
      {
        ZdoNwkMng_SaveDataSetPtr(pAddress, gNvSaveOnIdle_c, FALSE);
        ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackApsGlobals(gaApsAddressMapBitMask), gNvSaveOnIdle_c, TRUE);
      }
      break;

    /* All the data related to communication is saved here. */
    case zdoNvmObject_BindingTable_c:
      if(pAddress)
      {
        if((pAddress >= (void*)ZbBeeStackApsGlobals(gaApsBindingTable))
           && (pAddress <= (void*)&ZbBeeStackApsGlobals(gaApsBindingTable[gMaximumApsBindingTableEntries_c])))
        {
          ZdoNwkMng_SaveDataSetPtr(pAddress, gNvSaveOnIdle_c, FALSE);
        }
      }
      else
      {
        ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackApsGlobals(gaApsBindingTable), gNvSaveOnInterval_c, TRUE);
      }
      break;
    
    case zdoNvmObject_GroupTable_c:
      if(pAddress)
      {
        ZdoNwkMng_SaveDataSetPtr(pAddress, gNvSaveOnIdle_c, FALSE);
      }
      else
      {
      ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackApsGlobals(gaApsGroupTable), gNvSaveOnInterval_c, TRUE);
      }
      break;  
    
#if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#if gRnplusCapability_d
    case zdoNvmObject_RoutingTable_c:
      if(pAddress)
      {
        ZdoNwkMng_SaveDataSetPtr(pAddress, gNvSaveOnIdle_c, FALSE);
      }
      break;
#endif
#endif
      
#if gStandardSecurity_d || gHighSecurity_d
#if gApsLinkKeySecurity_d
    /* Saves the Aps security material set plus the address map. */
    case zdoNvmObject_ApsLinkKeySet_c:
      if(pAddress)
      {
        /*
          The Address Map entry will be saved by the Address Map management functions
        */
        /* Save the APS Device Key Pair Set entry */
        ZdoNwkMng_SaveDataSetPtr(pAddress, gNvSaveOnIdle_c, FALSE);
        
        /* Save the APS Key table entry */
        if(((zbApsDeviceKeyPairSet_t*)pAddress)->iKey < gApsMaxLinkKeys_c)
        {
          ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackApsGlobals(gaApsKeySet[((zbApsDeviceKeyPairSet_t*)pAddress)->iKey]), gNvSaveOnIdle_c, FALSE);
        }
      }
      else
      {
        ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet), gNvSaveOnInterval_c, TRUE);
        ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackApsGlobals(gaApsKeySet), gNvSaveOnInterval_c, TRUE);
        ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackApsGlobals(gaApsAddressMap), gNvSaveOnInterval_c, TRUE);
        ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackApsGlobals(gaApsAddressMapBitMask), gNvSaveOnInterval_c, TRUE);
      }
      break;
#endif
    /* Saves NWK security material. No point in saving the incoming counters. */
    case zdoNvmObject_SecureSet_c:
      ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackNwkGlobals(gNwkData), gNvSaveOnInterval_c, TRUE);
      break;
#endif

    /* Save all data. */
    case zdoNvmObject_All_c:
      ZdoNwkMng_SaveAllData();
      break;

#if gStandardSecurity_d || gHighSecurity_d      
    case zdoNvmObject_NwkOutgoingSecCounters_c:
    {
      uint8_t i;
      
      for(i=0; i< NumberOfElements(NlmeGetRequest(gNwkSecurityMaterialSet_c)); i++)
      {
        if(ZbBeeStackNwkGlobals(gNwkData.aNwkSecurityMaterialSet[i].keySeqNumber) == NlmeGetRequest(gNwkActiveKeySeqNumber_c))
        {
          ZdoNwkMng_SaveDataSetPtr(&(ZbBeeStackNwkGlobals(gNwkData.aNwkSecurityMaterialSet[i].outgoingFrameCounter)), gNvSaveOnCount_c, TRUE);
        }
      }
      break;
    }
#if gNwkSecSaveIncomingCounters_c
    case zdoNvmObject_NwkIncomingSecCounters_c:
      if(pAddress)
      {
        if(((zbIncomingFrameCounterSet_t*)pAddress)->IncomingFrameCounter % gNvIncrementAfterRestore_c == 0)
        {
          ZdoNwkMng_SaveDataSetPtr(pAddress, gNvSaveOnIdle_c, FALSE);
        }
      }
      else
      {
        uint8_t i;
        
        for(i=0; i< NumberOfElements(NlmeGetRequest(gNwkSecurityMaterialSet_c)); i++)
        {
          if(ZbBeeStackNwkGlobals(gNwkData.aNwkSecurityMaterialSet[i].keySeqNumber) == NlmeGetRequest(gNwkActiveKeySeqNumber_c))
          {
            if(i == 0)
            {
              ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackNwkGlobals(gaIncomingFrameCounterSet1), gNvSaveOnInterval_c, TRUE);
            }
            else if(i == 1)
            {
              ZdoNwkMng_SaveDataSetPtr(&ZbBeeStackNwkGlobals(gaIncomingFrameCounterSet2), gNvSaveOnInterval_c, TRUE);
            }
          }
        }
      }
      break;
#endif

#if gApsLinkKeySecurity_d
    case zdoNvmObject_ApsSecurityCounters_c:
      if(pAddress)
      {
        if(((zbApsDeviceKeyPairSet_t*)pAddress)->OutgoingFrameCounter % gNvIncrementAfterRestore_c == 0)
        {
          ZdoNwkMng_SaveDataSetPtr(pAddress, gNvSaveOnIdle_c, FALSE);
        }
      }
      break;
#endif
#endif

    /* Make sure all data sets are saved at once. */
    case zdoNvmObject_AtomicSave_c:
      NvAtomicSave(FALSE);
      NvOperationEnd();
      break;
     default:
      break; 
  }
}


/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/* None */
 
