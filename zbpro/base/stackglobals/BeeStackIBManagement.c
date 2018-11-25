/******************************************************************************
*  This is the Source file for the  BeeStack Information Base Management.
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
#include "FunctionLib.h"

#include "zigbee.h"
#include "BeeStack_Globals.h"
#include "BeeStackInterface.h" 
#include "BeeStackIBManagement.h"
#include "BeeStackUtil.h"
#include "NwkVariables.h"
#include "ApsVariables.h"
#include "AfVariables.h"
#include "ZdoVariables.h"
#include "BeeStackRamAlloc.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

static void* BeeStackGetAttAddress(zbIBRequest_t zbIBRequest, zbIBAttributeId_t attId, uint8_t  instID);
static uint8_t BeeStackGetAttSize(zbIBRequest_t zbIBRequest, zbIBAttributeId_t attId, uint8_t  instID);
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* A scalar can be thought of as a table with only one entry. */
/* Used for non-array entries in zbIBData_t.iMaxRecords. */
#define mIBScalar   1

/* Mark entries in the IB table as read/write or read-only. */
/* Used in zbIBData_t.access. */
typedef enum {
  mZbIBRO,
  mZbIBRW,
  mZbIBRWUseFunc
} mZbIBAccess_t;

/* utility messages can access. */
typedef struct zbIBDataInf_tag 
{
  zbIBAttributeId_t attId;
  mZbIBAccess_t     access;
  uint8_t           entrySize;
}zbIBDataInf_t;

typedef struct zbIBData_tag 
{
  zbIBAttributeId_t attId;
  mZbIBAccess_t     access;
  uint8_t           entrySize;
  index_t           maxEntries;
  void              *pTable;
}zbIBData_t;

typedef uint8_t (*pZbIBFunc_t) (uint8_t index, void *pData);

typedef struct zbIBAccessTbl_tag
{
 zbIBAttributeId_t attId;
 pZbIBFunc_t pSetTblEntry;
 pZbIBFunc_t pGetTblEntry; 
}zbIBAccessTbl_t;
/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/* Table of functions to access the IB tables; */
static zbIBAccessTbl_t const maZbIBAccessFuncTbl[] =
{
 { gApsAddressMap_c, AddrMap_SetTableEntry, AddrMap_GetTableEntry }
#if gApsLinkKeySecurity_d 
 ,{ gApsDeviceKeyPairSet_c, DevKeyPairSet_SetTableEntry, DevKeyPairSet_GetTableEntry}
#endif
};

zbIBDataInf_t const maZbIBNwkDataInf[] = {
  {gNwkSequenceNumber_c,                 mZbIBRO,         sizeof(ZbBeeStackNwkGlobals(gNibSequenceNumber))},
  {gNwkPassiveAckTimeout_c,              mZbIBRO,         sizeof(gNibPassiveAckTimeout)},
  {gNwkMaxBroadcastRetries_c,            mZbIBRO,         sizeof(gNibMaxBroadcastRetries)},
  {gNwkMaxChildren_c,                    mZbIBRO,         sizeof(gNibMaxChildren)},
  {gNwkMaxDepth_c,                       mZbIBRO,         sizeof(gNibMaxDepth)},
  {gNwkMaxRouter_c,                      mZbIBRO,         sizeof(gNibMaxRouter)},
  {gNwkNetworkBroadcastDeliveryTime_c,   mZbIBRO,         sizeof(gNibNetworkBroadcastDeliveryTime)},
  {gNwkReportConstantCost_c,             mZbIBRO,         sizeof(gNibReportConstantCost)},
  {gNwkRouteDiscoveryRetriesPermitted_c, mZbIBRO,         sizeof(ZbBeeStackNwkGlobals(gNibRouteDiscoveryRetriesPermitted))},
  {gNwkSymLink_c,                        mZbIBRO,         sizeof(gNibSymLink)},
  {gNwkCapabilityInformation_c,          mZbIBRW,         sizeof(ZbBeeStackNwkGlobals(gNwkData.capabilityInformation))},
  {gNwkAddrAlloc_c,                      mZbIBRO,         sizeof(gNibAddrAlloc)},
  {gNwkUseTreeRouting_c,                 mZbIBRO,         sizeof(gNibUseTreeRouting)},
  {gNwkManagerAddr_c,                    mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aNetworkManagerAddress))},  
  {gNwkTransactionPersistenceTime_c,     mZbIBRO,         sizeof(ZbBeeStackNwkGlobals(gNibTransactionPersistenceTime))},
  {gNwkLeaveRequestAllowed_c,            mZbIBRW,         sizeof(ZbBeeStackNwkGlobals(gNibNwkLeaveRequestAllowed))},
  //in ZIGBEE 2006 - is just RO ????
  {gNwkShortAddress_c,                   mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aShortAddress))},
  {gNwkStackProfile_c,                   mZbIBRO,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.stackProfile))},
  {gNwkProtocolVersion_c,                mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.protocolVersion))},
  {gNwkExtendedPanId_c,                  mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aNwkExtendedPanId))},
#if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
  #if gRnplusCapability_d  
    {gNwkRouteTable_c,                     mZbIBRW,         sizeof(routeTable_t)},
  #endif  
  {gNwkNeighborTable_c,                  mZbIBRW,         sizeof(neighborTable_t)},
  #if gNwkSymLinkCapability_d  
    {gNwkLinkStatusPeriod_c,              mZbIBRW,         sizeof(ZbBeeStackNwkGlobals(gNibNwkLinkStatusPeriod))},
  #endif
#endif  
  {gNwkPanId_c,                          mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aPanId))},
  {gDevType_c,                           mZbIBRO,         sizeof(ZbBeeStackNwkGlobals(gNwkData.deviceType))},
  {gSoftwareVersion_c,                   mZbIBRO,         sizeof(gaSoftwareVersion)},
  {gNwkLinkRetryThreshold_c,             mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.parentLinkRetryThreshold))},
#if (gEndDevCapability_d || gComboDeviceCapability_d )  
  {gNwkIndirectPollRate_c,               mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aIndirectPollRate))},
  {gConfirmationPollTimeOut_c,           mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gBeeStackConfig.gConfirmationPollTimeOut))},
#endif  
#if gStandardSecurity_d || gHighSecurity_d
  {gNwkSecurityLevel_c,                  mZbIBRW,         sizeof(ZbBeeStackNwkGlobals(gNwkData.nwkSecurityLevel))},
  {gNwkSecurityMaterialSet_c,            mZbIBRW,         sizeof(ZbBeeStackNwkGlobals(gNwkData.aNwkSecurityMaterialSet))},
  {gSASNwkKey_c,                         mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aNetworkKey))},
  {gNwkActiveKeySeqNumber_c,             mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.activeNetworkKeySeqNum))},
  {gNwkAllFresh_c,                       mZbIBRO,         sizeof(ZbBeeStackNwkGlobals(gNwkData.nwkAllFresh))},
  {gNwkSecureAllFrames_c,                mZbIBRO,         sizeof(ZbBeeStackNwkGlobals(gNwkData.nwkSecureAllFrames))},
  {gNwkPreconfiguredKey_c,               mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gBeeStackConfig.gNwkKeyPreconfigured))},
#endif
  {gNwkState_c,                          mZbIBRO,         sizeof(ZbBeeStackNwkGlobals(gNwkState))}
  #if gNwkMulticastCapability_d
  ,{gNwkUseMulticast_c,                   mZbIBRW,         sizeof(ZbBeeStackNwkGlobals(gNibNwkUseMulticast))}
#endif
#if gNwkManyToOneCapability_d
  ,{gNwkIsConcentrator_c,                 mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.fConcentratorFlag))}
  ,{gNwkConcentratorRadius_c,             mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.concentratorRadius))}
  ,{gNwkConcentratorDiscoveryTime_c,      mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.concentratorDiscoveryTime))}
#endif
  ,{gNwkMaxSourceRoute_c,                 mZbIBRO,         sizeof(gNibNwkMaxSourceRoute)},
  {gNwkLogicalChannel_c,                 mZbIBRW,         sizeof(ZbBeeStackNwkGlobals(gNwkData.logicalChannel))}
};
  
zbIBDataInf_t const maZbIBApsmeDataInf[] = {
#if gStandardSecurity_d || gHighSecurity_d
  {gApsTrustCenterAddress_c,             mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aTrustCenterAddress))},
  {gApsSecurityTimeOutPeriod_c,          mZbIBRW,         sizeof(ZbBeeStackApsGlobals(gBeeStackParameters.apsSecurityTimeOutPeriod))},
  #if gApsLinkKeySecurity_d  
    {gApsDeviceKeyPairSet_c,               mZbIBRWUseFunc,  sizeof(zbApsDeviceKeyPairSet_t)},
  #endif
#endif
  {gApsAddressMap_c,                     mZbIBRWUseFunc,  sizeof(zbAddressMap_t)},
#if gBindCapability_d  
  {gApsBindingTable_c,                   mZbIBRW,         sizeof(apsBindingTable_t)},
#endif  
  {gApsDesignatedCoordinator_c,          mZbIBRW,         sizeof(ZbBeeStackApsGlobals(gBeeStackParameters.gfApsDesignatedCoordinator))},
  {gApsChannelMask_c,                    mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask))},
  {gApsUseExtendedPANID_c,               mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aApsUseExtendedPanId))},
  {gApsGroupTable_c,                     mZbIBRW,         (sizeof(zbGroupId_t) + gEndPointsMaskSizeInBytes_c)},
  {gApsNonmemberRadius_c,                mZbIBRW,         sizeof(ZbBeeStackApsGlobals(gBeeStackParameters.gApsNonmemberRadius))},
  {gApsUseInsecureJoin_c,                mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.fUseInsecureJoin))},
#if (gStandardSecurity_d || gHighSecurity_d) && gApsLinkKeySecurity_d  
  {gSASTrustCenterMasterKey_c,           mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aTrustCenterMasterKey))},
  {gSASTrustCenterLinkKey_c,             mZbIBRW,         sizeof(ZbBeeStackGlobalsParams(gSAS_Ram.aPreconfiguredTrustCenterLinkKey))},
#endif  
#if gFragmentationCapability_d
  {gApsInterframeDelay_c,                mZbIBRW,         sizeof(ZbBeeStackApsGlobals(gBeeStackParameters.gApsInterframeDelay))},
  {gApsMaxWindowSize_c,                  mZbIBRW,         sizeof(ZbBeeStackApsGlobals(gBeeStackParameters.gApsMaxWindowSize))},
  {gApsMaxFragmentLength_c,              mZbIBRW,         sizeof(ZbBeeStackApsGlobals(gBeeStackParameters.gApsMaxFragmentLength))},
#endif
  {gApsLastChannelEnergy_c,              mZbIBRO,         sizeof(ZbBeeStackApsGlobals(gApsLastChannelEnergy))},
  {gApsLastChannelFailureRate_c,         mZbIBRO,         sizeof(ZbBeeStackApsGlobals(gApsChannelFailureRate))},
  {gApsChannelTimer_c,                   mZbIBRO,         sizeof(ZbBeeStackApsGlobals(gApsChannelTimer))}
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
/******************************************************************************
* Retrieve or store information from the NWK or APS Information Base.
******************************************************************************/
zbStatus_t BeeStack_GetSetAttrValue(zbIBRequest_t zbIBRequest, uint8_t instId,  zbIBReqParams_t *pZbIBRequest)
{
  zbIBData_t iBData;
  index_t i;
  index_t totalDataSize;
  zbIBDataInf_t const *pIbDataInf = maZbIBApsmeDataInf;
  uint8_t ibDataInfEntries = NumberOfElements(maZbIBApsmeDataInf);
  
  /* Check attr struct pointer */
  if(NULL == pZbIBRequest)
  {
    return gZbInvalidParameter_c;
  }
  
  if((zbIBNlmeSetReq_c == zbIBRequest) || (zbIBNlmeGetReq_c == zbIBRequest))
  {
    pIbDataInf = maZbIBNwkDataInf;
    ibDataInfEntries = NumberOfElements(maZbIBNwkDataInf);
  }
  
  /* Look for attribute in the att table */
  for (i = 0; i < ibDataInfEntries; ++i)
  {
    if (pZbIBRequest->attId == pIbDataInf->attId) 
    {
      FLib_MemCpy((void*)&iBData,(void*)pIbDataInf,sizeof(zbIBDataInf_t));
      break;
    }
    pIbDataInf++;
  }

  /* Attr not found in table */
  if (ibDataInfEntries == i) 
  {
    return gZbUnsupportedAttribute_c;
  }
  
  /* In case of set - check if attr is could be written */
  if(((zbIBApsmeSetReq_c == zbIBRequest) || (zbIBNlmeSetReq_c == zbIBRequest))&&((iBData.access != mZbIBRW) && (iBData.access != mZbIBRWUseFunc)))
  {
    return gZbNotPermitted_c;
  }
  
  /* get max number of entries (tables) */
  iBData.maxEntries = BeeStackGetAttSize(zbIBRequest,pZbIBRequest->attId,instId);
  /* get attribute address (tables) */
  iBData.pTable     = BeeStackGetAttAddress(zbIBRequest,pZbIBRequest->attId,instId);
   
  /* Check for request errors */
  if ((pZbIBRequest->iIndex >= iBData.maxEntries)||
     (pZbIBRequest->iIndex + pZbIBRequest->iEntries > iBData.maxEntries)) 
  {
    return gZbInvalidParameter_c;
  }

  if (!pZbIBRequest->iEntries) 
  {
    pZbIBRequest->iEntries = 1;
  }
  
  pZbIBRequest->iEntrySize = iBData.entrySize;
  
  /* Check if attr is access trough a function */
  if (iBData.access == mZbIBRWUseFunc)
  {
    zbIBAccessTbl_t  const *pZbIBAccessFuncTbl = NULL;
    
    /* Find the entry in maZbIBAccessFuncTbl */
    for (i = 0; i < NumberOfElements(maZbIBAccessFuncTbl); ++i) 
    {
      if (iBData.attId == maZbIBAccessFuncTbl[i].attId) 
      {
        pZbIBAccessFuncTbl = &maZbIBAccessFuncTbl[i];
        break;
      }
    }
    
    if (NULL == pZbIBAccessFuncTbl)
    {
      return gZbInvalidParameter_c;
    }
    else
    { 
      uint8_t *pEntries = ((uint8_t *)pZbIBRequest->pData);  
      for(i = pZbIBRequest->iIndex; i < ( pZbIBRequest->iIndex + pZbIBRequest->iEntries ); i++)
      {
        if((zbIBApsmeGetReq_c == zbIBRequest) || (zbIBNlmeGetReq_c == zbIBRequest))
        {
          (void)(pZbIBAccessFuncTbl)->pGetTblEntry(i, pEntries);
        }
        else
        {
          (void)(pZbIBAccessFuncTbl)->pSetTblEntry(i, pEntries);
        }
        pEntries = pEntries + iBData.entrySize;
      }
      return gZbSuccess_c;
    }
  }
  
  totalDataSize = (index_t) (pZbIBRequest->iEntrySize * pZbIBRequest->iEntries);
  
  if((zbIBApsmeGetReq_c == zbIBRequest) || (zbIBNlmeGetReq_c == zbIBRequest))
  {
    FLib_MemCpy(((uint8_t *)pZbIBRequest->pData),
                 (void *)(((uint8_t *)iBData.pTable) + ((uint16_t)pZbIBRequest->iEntrySize * pZbIBRequest->iIndex)),
                 totalDataSize);
  }
  else
  {
     FLib_MemCpy(((uint8_t *) iBData.pTable + ((uint16_t)pZbIBRequest->iEntrySize * pZbIBRequest->iIndex)),
                 (void *)pZbIBRequest->pData,
                 totalDataSize);
     
     //check for multiple instances
     if(zbIBNlmeGetReq_c == zbIBNlmeSetReq_c)
     {
       if (pZbIBRequest->attId == gNwkPanId_c)
         (void)SetPibAttributeValue(gMPibPanId_c, (void *)pZbIBRequest->pData);
       else if (pZbIBRequest->attId == gNwkShortAddress_c)
         (void)SetPibAttributeValue(gMPibShortAddress_c, (void *)pZbIBRequest->pData);
     }
  }

  return gZbSuccess_c;
}



/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
/***************************************************************************
*  Get attribute address from ram or flash for a beestackinstanceID
*  
*  Return value: attribute address
*   
*   
****************************************************************************/
static void* BeeStackGetAttAddress(zbIBRequest_t zbIBRequest, zbIBAttributeId_t attId, uint8_t  instID)
{
  if((zbIBNlmeGetReq_c == zbIBRequest) || (zbIBNlmeSetReq_c == zbIBRequest))
  {
    switch(attId)
    {
      case gNwkSequenceNumber_c:     
#if(gInstantiableStackEnabled_d == 1)  
      return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNibSequenceNumber);
#else
      return (void *)&ZbBeeStackNwkGlobals(gNibSequenceNumber);
#endif  
      case gNwkPassiveAckTimeout_c:
        return (void*)&gNibPassiveAckTimeout;
      case gNwkMaxBroadcastRetries_c:
        return (void*)&gNibMaxBroadcastRetries; 
      case gNwkMaxChildren_c:
        return (void *)&gNibMaxChildren;
      case gNwkMaxDepth_c:
        return (void*)&gNibMaxDepth;
      case gNwkMaxRouter_c:
        return (void*)&gNibMaxRouter;
      case gNwkNetworkBroadcastDeliveryTime_c:
        return (void*)&gNibMaxRouter;
      case gNwkReportConstantCost_c:
        return (void*)&gNibMaxRouter;
      case gNwkRouteDiscoveryRetriesPermitted_c:
#if(gInstantiableStackEnabled_d == 1)       
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNibRouteDiscoveryRetriesPermitted);
#else  
        return (void *)&ZbBeeStackNwkGlobals(gNibRouteDiscoveryRetriesPermitted);
#endif  
      case gNwkSymLink_c:
        return (void*)&gNibMaxRouter;
      case gNwkCapabilityInformation_c:
#if(gInstantiableStackEnabled_d == 1)         
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNwkData.capabilityInformation);
#else
        return (void *)&ZbBeeStackNwkGlobals(gNwkData.capabilityInformation);
#endif
      case gNwkAddrAlloc_c:
        return (void*)&gNibMaxRouter;
      case gNwkUseTreeRouting_c:
        return (void*)&gNibMaxRouter;
      case gNwkManagerAddr_c:
#if(gInstantiableStackEnabled_d == 1)     
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aNetworkManagerAddress);
#else
        return (void *)&ZbBeeStackGlobalsParams(gSAS_Ram.aNetworkManagerAddress);      
#endif
      case gNwkTransactionPersistenceTime_c:          
#if(gInstantiableStackEnabled_d == 1)
        return(void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNibTransactionPersistenceTime);
#else
        return(void *)&ZbBeeStackNwkGlobals(gNibTransactionPersistenceTime);
#endif        
      case gNwkShortAddress_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aShortAddress);
#else
        return (void *)ZbBeeStackGlobalsParams(gSAS_Ram.aShortAddress);
#endif
      case gNwkStackProfile_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.stackProfile); 
#else
        return (void *)&ZbBeeStackGlobalsParams(gSAS_Ram.stackProfile); 
#endif 
      case gNwkProtocolVersion_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.protocolVersion);
#else
        return (void *)&ZbBeeStackGlobalsParams(gSAS_Ram.protocolVersion);
#endif  
      case gNwkExtendedPanId_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aNwkExtendedPanId);
#else
        return (void *)ZbBeeStackGlobalsParams(gSAS_Ram.aNwkExtendedPanId);
#endif  
#if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#if gRnplusCapability_d      
      case gNwkRouteTable_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gaRouteTable);
#else
        return (void *)ZbBeeStackNwkGlobals(gaRouteTable);
#endif 
#endif    
      case gNwkNeighborTable_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gaNeighborTable); 
#else
        return (void *)ZbBeeStackNwkGlobals(gaNeighborTable); 
#endif
#if gNwkSymLinkCapability_d
      case gNwkLinkStatusPeriod_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNibNwkLinkStatusPeriod);
#else
        return (void *)&ZbBeeStackNwkGlobals(gNibNwkLinkStatusPeriod);
#endif
#endif
#endif
      case gNwkPanId_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aPanId;) 
#else
        return (void *)ZbBeeStackGlobalsParams(gSAS_Ram.aPanId;) 
#endif     
      case gDevType_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNwkData.deviceType); 
#else
        return (void *)&ZbBeeStackNwkGlobals(gNwkData.deviceType); 
#endif
      case gSoftwareVersion_c:
        return (void *)&gaSoftwareVersion; 
      case gNwkLinkRetryThreshold_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.parentLinkRetryThreshold);
#else
        return (void *)&ZbBeeStackGlobalsParams(gSAS_Ram.parentLinkRetryThreshold);
#endif
#if (gEndDevCapability_d || gComboDeviceCapability_d )     
      case gNwkIndirectPollRate_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aIndirectPollRate);
#else
        return (void *)ZbBeeStackGlobalsParams(gSAS_Ram.aIndirectPollRate);
#endif
      case gConfirmationPollTimeOut_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gBeeStackConfig.gConfirmationPollTimeOut);
#else
        return (void *)&ZbBeeStackGlobalsParams(gBeeStackConfig.gConfirmationPollTimeOut);
#endif
#endif     
#if gStandardSecurity_d || gHighSecurity_d
      case gNwkSecurityLevel_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNwkData.nwkSecurityLevel); 
#else
        return (void *)&ZbBeeStackNwkGlobals(gNwkData.nwkSecurityLevel); 
#endif
      case gNwkSecurityMaterialSet_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNwkData.aNwkSecurityMaterialSet); 
#else
        return (void *)ZbBeeStackNwkGlobals(gNwkData.aNwkSecurityMaterialSet); 
#endif 
      case gSASNwkKey_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aNetworkKey);
#else
        return (void *)ZbBeeStackGlobalsParams(gSAS_Ram.aNetworkKey);
#endif  
      case gNwkActiveKeySeqNumber_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.activeNetworkKeySeqNum);
#else
        return (void *)&ZbBeeStackGlobalsParams(gSAS_Ram.activeNetworkKeySeqNum);
#endif  
      case gNwkAllFresh_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNwkData.nwkAllFresh);
#else
        return (void *)&ZbBeeStackNwkGlobals(gNwkData.nwkAllFresh);
#endif  
      case gNwkSecureAllFrames_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNwkData.nwkSecureAllFrames);
#else
        return (void *)&ZbBeeStackNwkGlobals(gNwkData.nwkSecureAllFrames);
#endif
      case gNwkPreconfiguredKey_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gBeeStackConfig.gNwkKeyPreconfigured);
#else
        return (void *)&ZbBeeStackGlobalsParams(gBeeStackConfig.gNwkKeyPreconfigured);
#endif
#endif //gStandardSecurity_d || gHighSecurity_d
      case gNwkState_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNwkState);
#else
        return (void *)&ZbBeeStackNwkGlobals(gNwkState);
#endif
      case gNwkLeaveRequestAllowed_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNibNwkLeaveRequestAllowed);
#else
        return (void *)&ZbBeeStackNwkGlobals(gNibNwkLeaveRequestAllowed);
#endif
#if gNwkMulticastCapability_d
      case gNwkUseMulticast_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNibNwkUseMulticast);
#else
        return (void *)&ZbBeeStackNwkGlobals(gNibNwkUseMulticast);
#endif  
#endif      
#if gNwkManyToOneCapability_d
      case gNwkIsConcentrator_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.fConcentratorFlag);
#else
        return (void *)&ZbBeeStackGlobalsParams(gSAS_Ram.fConcentratorFlag);
#endif
      case gNwkConcentratorRadius_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.concentratorRadius);
#else
        return (void *)&ZbBeeStackGlobalsParams(gSAS_Ram.concentratorRadius);
#endif
      case gNwkConcentratorDiscoveryTime_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.concentratorDiscoveryTime);
#else
        return (void *)&ZbBeeStackGlobalsParams(gSAS_Ram.concentratorDiscoveryTime);
#endif
      case gNwkMaxSourceRoute_c:
        return (void *)&gNibNwkMaxSourceRoute;
#endif
      case gNwkLogicalChannel_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackNwkGlobals(gNwkData.logicalChannel);
#else
        return (void *)&ZbBeeStackNwkGlobals(gNwkData.logicalChannel);
#endif      
      default:
        return NULL;
    }
  }
  else if((zbIBApsmeGetReq_c == zbIBRequest) || (zbIBApsmeSetReq_c == zbIBRequest))
  {
    switch(attId)
    {
#if gStandardSecurity_d || gHighSecurity_d
      case gApsTrustCenterAddress_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aTrustCenterAddress); 
#else
        return (void *)&ZbBeeStackGlobalsParams(gSAS_Ram.aTrustCenterAddress); 
#endif 
      case gApsSecurityTimeOutPeriod_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gBeeStackParameters.apsSecurityTimeOutPeriod);
#else
        return (void *)&ZbBeeStackApsGlobals(gBeeStackParameters.apsSecurityTimeOutPeriod);
#endif
#if gApsLinkKeySecurity_d  
      case gApsDeviceKeyPairSet_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet);
#else
        return (void *)ZbBeeStackApsGlobals(gaApsDeviceKeyPairSet);
#endif
#endif
#endif //gStandardSecurity_d || gHighSecurity_d
      case gApsAddressMap_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gaApsAddressMap);
#else
        return (void *)ZbBeeStackApsGlobals(gaApsAddressMap);
#endif
#if gBindCapability_d    
      case gApsBindingTable_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gaApsBindingTable);
#else
        return (void *)ZbBeeStackApsGlobals(gaApsBindingTable);
#endif
#endif
      case gApsDesignatedCoordinator_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gBeeStackParameters.gfApsDesignatedCoordinator);  
#else
        return (void *)&ZbBeeStackApsGlobals(gBeeStackParameters.gfApsDesignatedCoordinator);  
#endif
      case gApsChannelMask_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask);
#else
        return (void *)ZbBeeStackGlobalsParams(gSAS_Ram.aChannelMask);
#endif
      case gApsUseExtendedPANID_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aApsUseExtendedPanId);
#else
        return (void *)ZbBeeStackGlobalsParams(gSAS_Ram.aApsUseExtendedPanId);
#endif
      case gApsGroupTable_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gaApsGroupTable); 
#else
        return (void *)ZbBeeStackApsGlobals(gaApsGroupTable); 
#endif 
      case gApsNonmemberRadius_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gBeeStackParameters.gApsNonmemberRadius); 
#else
        return (void *)&ZbBeeStackApsGlobals(gBeeStackParameters.gApsNonmemberRadius); 
#endif 
      case gApsUseInsecureJoin_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.fUseInsecureJoin); 
#else
        return (void *)&ZbBeeStackGlobalsParams(gSAS_Ram.fUseInsecureJoin); 
#endif 
#if (gStandardSecurity_d || gHighSecurity_d) && gApsLinkKeySecurity_d
      case gSASTrustCenterMasterKey_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aTrustCenterMasterKey);
#else
        return (void *)ZbBeeStackGlobalsParams(gSAS_Ram.aTrustCenterMasterKey);
#endif
      case gSASTrustCenterLinkKey_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)pZbStackStuctPointersTable[instID]->ZbBeeStackGlobalsParams(gSAS_Ram.aPreconfiguredTrustCenterLinkKey);
#else
        return (void *)ZbBeeStackGlobalsParams(gSAS_Ram.aPreconfiguredTrustCenterLinkKey);
#endif
#endif      
#if gFragmentationCapability_d
      case gApsInterframeDelay_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gBeeStackParameters.gApsInterframeDelay);
#else
        return (void *)&ZbBeeStackApsGlobals(gBeeStackParameters.gApsInterframeDelay);
#endif 
      case gApsMaxWindowSize_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gBeeStackParameters.gApsMaxWindowSize);
#else
        return (void *)&ZbBeeStackApsGlobals(gBeeStackParameters.gApsMaxWindowSize);
#endif
      case gApsMaxFragmentLength_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gBeeStackParameters.gApsMaxFragmentLength);
#else
        return (void *)&ZbBeeStackApsGlobals(gBeeStackParameters.gApsMaxFragmentLength);
#endif
#endif      
      case gApsLastChannelEnergy_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gApsLastChannelEnergy);
#else
        return (void *)&ZbBeeStackApsGlobals(gApsLastChannelEnergy);
#endif
      case gApsLastChannelFailureRate_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gApsChannelFailureRate);
#else
        return (void *)&ZbBeeStackApsGlobals(gApsChannelFailureRate);
#endif
      case gApsChannelTimer_c:
#if(gInstantiableStackEnabled_d == 1)
        return (void *)&pZbStackStuctPointersTable[instID]->ZbBeeStackApsGlobals(gApsChannelTimer);
#else
        return (void *)&ZbBeeStackApsGlobals(gApsChannelTimer);
#endif
      default:
        return NULL;
    }
  }
  return NULL;
}

static uint8_t BeeStackGetAttSize(zbIBRequest_t zbIBRequest, zbIBAttributeId_t attId, uint8_t  instID)
{
  if((zbIBNlmeGetReq_c == zbIBRequest) || (zbIBNlmeSetReq_c == zbIBRequest))
  {
    switch(attId)
    {
      
#if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)     
#if gRnplusCapability_d
      case gNwkRouteTable_c: 
#if(gInstantiableStackEnabled_d == 1)      
        return pZbStackStuctPointersTable[instID]->pZbStackTablesSizes->gMaxSourceRouteEntries;
#else
        return gMaxSourceRouteEntries;
#endif  
#endif      
      case gNwkNeighborTable_c:
#if(gInstantiableStackEnabled_d == 1)  
        return pZbStackStuctPointersTable[instID]->pZbStackTablesSizes->gMaxNeighborTableEntries;
#else
        return gMaxNeighborTableEntries;      
#endif
#endif
      default:
        return mIBScalar;
    }
  }
  else if((zbIBApsmeGetReq_c == zbIBRequest) || (zbIBApsmeSetReq_c == zbIBRequest))
  {
    switch(attId)
    {
#if gBindCapability_d      
      case gApsBindingTable_c:
#if(gInstantiableStackEnabled_d == 1)  
        return pZbStackStuctPointersTable[instID]->pZbStackTablesSizes->gApsMaxBindingEntries;
#else
        return gApsMaxBindingEntries;
#endif
#endif      
      case gApsGroupTable_c:
#if(gInstantiableStackEnabled_d == 1)       
        return pZbStackStuctPointersTable[instID]->pZbStackTablesSizes->gApsMaxGroups;
#else  
        return gApsMaxGroups;
#endif
#if gStandardSecurity_d || gHighSecurity_d
#if gApsLinkKeySecurity_d     
      case gApsDeviceKeyPairSet_c:
#if(gInstantiableStackEnabled_d == 1)  
        return pZbStackStuctPointersTable[instID]->pZbStackTablesSizes->giApsDeviceKeyPairCount;
#else
        return giApsDeviceKeyPairCount;
#endif
#endif
#endif
      case gApsAddressMap_c:
#if(gInstantiableStackEnabled_d == 1)  
        return pZbStackStuctPointersTable[instID]->pZbStackTablesSizes->gApsMaxAddressMapEntries;
#else
        return gApsMaxAddressMapEntries;
#endif      
      default:
        return mIBScalar;
    }
  }
  
  return mIBScalar;
}
/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/* None */ 
