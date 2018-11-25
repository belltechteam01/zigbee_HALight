/*! @file 	  EzCommissioning.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for the EZ-Mode Commissioning method.
 *
 * @copyright Copyright(c) 2013, Freescale, Inc. All rights reserved.
 *
 * @license	Redistribution and use in source and binary forms, with or without modification,
 *			are permitted provided that the following conditions are met:
 *
 *			o Redistributions of source code must retain the above copyright notice, this list
 *			of conditions and the following disclaimer.
 *
 *			o Redistributions in binary form must reproduce the above copyright notice, this
 *			list of conditions and the following disclaimer in the documentation and/or
 *			other materials provided with the distribution.
 *
 *			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *			contributors may be used to endorse or promote products derived from this
 *			software without specific prior written permission.
 *
 *			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *			ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *			WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 *			ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *			(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *			LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *			ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *			(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *			SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "EmbeddedTypes.h"
#include "NV_Data.h"
#include "NVM_Interface.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "TMR_Interface.h"
#include "ZdoApsInterface.h"
#include "ApsMgmtInterface.h"
#include "BeeAppInit.h"
#include "BeeCommon.h"
#include "HaProfile.h"
#include "ASL_UserInterface.h"
#include "BeeApp.h"
#include "ZCL.h"
#include "ZclFoundation.h"
#include "ZdpManager.h"
#include "EndPointConfig.h"
#include "ASL_ZdpInterface.h"
#include "EzCommissioning.h"
#include "ZigbeeTask.h"

#include "ZDOVariables.h"
#include "ZdoApsInterface.h"
#if gASL_EnableEZCommissioning_d
/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/


#if gInstantiableStackEnabled_d
  #define gmUserInterfaceMode gmUserInterfaceMode[zbProInstance]
#endif
/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void EZCommissioning_StateMachine(tsEvent_t events);
#if gEndDevCapability_d || gComboDeviceCapability_d
static void EZCommissioning_UpdatePollRate(bool_t fastPollRate);
#endif
#if gASL_EnableEZCommissioning_Initiator_d
static void EZComissioning_SendIdentifyQueryReq(uint16_t time, bool_t senderInIdentify);
static void EZCommissioning_SendBind(void);
#if gSimple_Desc_req_d
static void EZComissioning_SendSimpleDescReq(void);
#endif
static bool_t EZComissioning_DevBindMatchClusters(uint8_t indexDevice);
#if gASL_EzCommissioning_EnableGroupBindCapability_d
static void EZCommissioning_PerformGroupBind(uint8_t indexDevice);
#endif
#endif
void EZCommissioning_AutoCloseTimerCallback(zbTmrTimerID_t timerId);
void EZCommissioning_AutoClose(uint32_t timeout);


extern void PWRLib_Reset(void);
/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
tsTaskID_t gEzCmsTaskId = gTsInvalidTaskID_c;

#if !gInstantiableStackEnabled_d

#if gASL_EnableEZCommissioning_Initiator_d  
extern zbClusterId_t gaZdpMatchedOutCluster[];
extern index_t giMatchIndex;

zdoEZComDBTable_t gaEZComDBTable;
#endif
zbTmrTimerID_t gEZCommissioningAutocloseTimerId = gTmrInvalidTimerID_c;
char gGroupName[] = {0x09, 0x54, 0x65, 0x73, 0x74, 0x47, 0x72, 0x6f, 0x75, 0x70};

bool_t gEZCommissioningOneShoot;
bool_t gEZCommissioningUseGroup = FALSE;
bool_t gEZCommissioning_ResetDevice = FALSE;
tsEvent_t gEZCommissioning_LastEvent =  0x00; 

#if gASL_EnableEZCommissioning_Initiator_d
uint8_t *gpEZCommissioningTempData = NULL;	 
#endif

#if gComboDeviceCapability_d
uint8_t gEZCommissioningNetworkDiscoveryAttempts;
#endif
uint8_t gEZCommissioningState = EZCommissioning_IdleState_c;
uint8_t gEZCommissioningPrimaryDeviceType = EZCommissioning_PrimaryDeviceType_d;

#else
  ezCommisioningData_t *pEzCommisioningData;
  ezCommisioningData_t EzCommisioningData[2]; 
#endif

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

void EZCommissioning_Init(void);
void EZComissioning_Start(uint8_t startMode);
void EZCommissioning_DBTimeoutCallback( uint8_t iTimerId );

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private Debug stuff
*************************************************************************************
************************************************************************************/

/*!
 * @fn 		void EZCommissioning_Init(void) 
 *
 * @brief	Init EZ-Mode Commissioning Procedure
 *
 */
void EZCommissioning_Init(void)
{
 #if(gInstantiableStackEnabled_d == 0) 
  if(gEzCmsTaskId == gTsInvalidTaskID_c)
  {
 #endif   
    gEzCmsTaskId = TS_CreateTask(gTsFirstApplicationTaskPriority_c +2, EZCommissioning_StateMachine);
 #if(gInstantiableStackEnabled_d == 0)
  }
 #endif

  EZCommissioning_SetState(EZCommissioning_InitialState_c);
#if gASL_EnableEZCommissioning_Initiator_d  
  EZCommissioningConfigData(gaEZComDBTable.aState) = EZCommissioning_BindTableAvailable_c;  
#endif  
  EZCommissioningConfigData(gEZCommissioningAutocloseTimerId) = ZbTMR_AllocateTimer();
}

/*!
 * @fn 		void EZComissioning_Start(uint8_t startMode)
 *
 * @brief	Start EZ-Mode Commissioning Procedure
 *
 */
void EZComissioning_Start(uint8_t startMode)
{
  uint8_t identifyCommissioningState = 0;
  zbClusterId_t identifyClusterId = {gaZclClusterIdentify_c};
        
  /* get CommissioningState */
  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), identifyClusterId, gZclAttrIdentify_CommissioningState_c, gZclServerAttr_c, &identifyCommissioningState, NULL);
  
  if(startMode & gEzCommissioning_FactoryFresh_c)
  {
    appState = 0;
    BeeApp_FactoryFresh();
    ZDO_StopEx(gZdoStopMode_ResetTables_c);
    EZCommissioningConfigData(gEZCommissioning_ResetDevice) = TRUE;
    return;
  }
  if(startMode & gEzCommissioning_NetworkSteering_c)
  {
    EZCommissioningConfigData(gEZCommissioningOneShoot) = FALSE;
    TS_SendEvent(gEzCmsTaskId, gStart_c);	
    if(!(identifyCommissioningState & gZclCommissioningState_NetworkState_d))
    	BeeAppUpdateDevice(BeeAppDataInit(appEndPoint), gStartNetwork_c, 0, 0, NULL);
  }
  if(startMode & gEzCommissioning_FindingAndBinding_c)
  {
	if(startMode & gEzCommissioning_NetworkSteering_c)
	{
	  EZCommissioningConfigData(gEZCommissioningOneShoot) = TRUE;
	}
	/* No Network Steering, No Factory Fresh. Check if the device is on the network before moving to Finding and Binding */
	else if(!(startMode & gEzCommissioning_FactoryFresh_c) && (identifyCommissioningState & gZclCommissioningState_NetworkState_d))
	{
	  EZCommissioning_SetState(EZCommissioning_Identify_c);
	  TS_SendEvent(gEzCmsTaskId, gStartIdentify_c);
	}
	/* Finding and Binding with Network Fresh but without Network Steering doesn't do anything */
  }
}

/*!
 * @fn 		void EZCommissioning_StateMachine(tsEvent_t events)
 *
 * @brief	Process EZ-Mode Commissioning States
 *
 */
void EZCommissioning_StateMachine(tsEvent_t events)
{
  uint8_t identifyCommissioningState = 0;
  zbClusterId_t identifyClusterId = {gaZclClusterIdentify_c};
  uint8_t indexDevice = GetIndexFromEZModeDeviceTable(BeeAppDataInit(appEndPoint));      
  
  
  EZCommissioningConfigData(gEZCommissioning_LastEvent) = events;
  
  /* get CommissioningState */
  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), identifyClusterId, gZclAttrIdentify_CommissioningState_c, gZclServerAttr_c, &identifyCommissioningState, NULL);
  
  switch (EZCommissioning_GetState())
  {
  	/* Beginning of the Network Steering procedure as specified in the Home Automation 1.2 specification revision: 053520r29 */
    case EZCommissioning_InitialState_c:
    {
      if(events & gStart_c)
      {
        if(ZDO_GetState() == gZdoInitialState_c)
        {
#if gComboDeviceCapability_d
          EZCommissioningConfigData(gEZCommissioningNetworkDiscoveryAttempts) = EZCommissioning_NetworkDiscoveryAttempts_c;
#endif
          EZCommissioning_SetState(EZCommissioning_NetworkSteering_c);
#if gCoordinatorCapability_d
          /* Device is not in a network. Form a network. */
          TS_SendEvent(gEzCmsTaskId, gFormNetwork_c);
#else
          TS_SendEvent(gEzCmsTaskId, gJoinNetwork_c);
#endif
        }
        else
        {
          /* Device is in a network. Broadcast permit join */
#if gMgmt_Permit_Joining_req_d
#if gComboDeviceCapability_d
    	  if(NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
	  {
    	     zbNwkAddr_t address;
    		
             Copy2Bytes(address, gaBroadcastZCnZR);
    	     /* Send Mgmt_Permit_Joining_Req */
             ASL_Mgmt_Permit_Joining_req(&ZbZclFoundationGlobals(gZclTransactionId), address, EZOpenNetworkTime_c*60, TRUE);
            
             if(EZCommissioningConfigData(gEZCommissioningOneShoot))
             {
                EZCommissioning_SetState(EZCommissioning_Identify_c);
                TS_SendEvent(gEzCmsTaskId, gStartIdentify_c);
                EZCommissioningConfigData(gEZCommissioningOneShoot) = FALSE;
             }
   	  }
#endif
        }
      }
      if(events & gUpdatePollRate_c)
      {
#if gEndDevCapability_d || gComboDeviceCapability_d
    	  EZCommissioning_UpdatePollRate(FALSE);
#endif      	  
      }
      break;
    }
    case EZCommissioning_NetworkSteering_c:
    {
      if(events & gFormNetwork_c)
      {
        ZDO_Start(gZdoStartMode_Zc_c | gZdoStartMode_RamSet_c | gZdoStartMode_Associate_c);
      }
      if(events & gJoinNetwork_c)
      {
        ZDO_Start(EZCommissioningConfigData(gEZCommissioningPrimaryDeviceType) | gZdoStartMode_RamSet_c);
      }
#if gComboDeviceCapability_d
      if(events & gJoiningfailed_c)
      {
        if(NlmeGetRequest(gDevType_c) != gCoordinator_c)
        {
          EZCommissioningConfigData(gEZCommissioningNetworkDiscoveryAttempts)--;
          
          if(!EZCommissioningConfigData(gEZCommissioningNetworkDiscoveryAttempts))
          {
            ZDO_StopEx(gZdoStopMode_Stop_c);
            TS_SendEvent(gEzCmsTaskId, gFormNetwork_c);
          }
        }
      }
#endif
      if(events & gDeviceInNetwork_c)
      {
        if(!(identifyCommissioningState&gZclCommissioningState_NetworkState_d))
        {
          identifyCommissioningState |= gZclCommissioningState_NetworkState_d;
          /* set CommissioningState on both endpoints */
#if gInstantiableStackEnabled_d
          for(uint8_t i=0; i<EndPointConfigData(gNum_EndPoints); i++)
#else
          for(uint8_t i=0; i<gNum_EndPoints_c; i++)
#endif  
          (void)ZCL_SetAttribute(EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint), identifyClusterId, gZclAttrIdentify_CommissioningState_c, gZclServerAttr_c, &identifyCommissioningState);
          ZCL_SaveNvmZclData();
        }
#if (gMgmt_Permit_Joining_req_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
#if gComboDeviceCapability_d
    	if(NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
    	{
      	  /* Send Mgmt_Permit_Joining_Req. Routers send to 0xFFFC, Coordinator sends to itself. */
    	  zbNwkAddr_t aDestAddr = {0x00, 0x00};
#if (gRouterCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
    	  if(NlmeGetRequest(gDevType_c) == gRouter_c)
#endif
    	  {
    		Copy2Bytes(aDestAddr, gaBroadcastZCnZR);
    	  }
#endif
          ASL_Mgmt_Permit_Joining_req(&ZbZclFoundationGlobals(gZclTransactionId), aDestAddr, EZOpenNetworkTime_c*60, TRUE);
    	}
#endif
    	if(EZCommissioningConfigData(gEZCommissioningOneShoot))
    	{
    	  EZCommissioningConfigData(gEZCommissioningOneShoot) = FALSE;
    	  /* Move to Finding and Binding */
    	  EZCommissioning_SetState(EZCommissioning_Identify_c);
    	  TS_SendEvent(gEzCmsTaskId, gStartIdentify_c);
    	}
    	else
    	{
    	  EZCommissioning_SetState(EZCommissioning_InitialState_c);
    	}
      }
      break;
    }
    /* End of the Network Steering procedure as specified in the Home Automation 1.2 specification revision: 053520r29 */
    /* Beginning of the Finding and Binding procedure as specified in the Home Automation 1.2 specification revision: 053520r29 */
    case EZCommissioning_Identify_c:
    {
      if(events & gStartIdentify_c)
      {
        BeeAppUpdateDevice(BeeAppDataInit(appEndPoint), gZclUI_EZCommissioning_FindingAndBinding_c, 0, 0, NULL);	  
        ZCL_SetIdentifyMode(BeeAppDataInit(appEndPoint), Native2OTA16(EZIdWindowTime_c*60)); 
        TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); /*start FastPoll Rate -  EndDevice*/
      }
      if(events & gSendIdentifyReq_c)
      {
#if gASL_EnableEZCommissioning_Initiator_d
        if(indexDevice!=gEZModeInvalidIndex_d)
          if(EZCommissioningConfigData(gEZModeDeviceInf[indexDevice].isInitiator) == TRUE)
          {
              uint16_t identifyTime = 0; 
              (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), identifyClusterId, gZclAttrIdentify_Time_c, gZclServerAttr_c, &identifyTime, NULL);	
              if(!EZCommissioningConfigData(gpEZCommissioningTempData))
                EZCommissioningConfigData(gpEZCommissioningTempData) = MSG_Alloc(gEZCommisioning_TempDataLength_d);
              if(!EZCommissioningConfigData(gpEZCommissioningTempData))
                return;
              EZCommissioningConfigData(gpEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1]) = 0xFF;
              EZComissioning_SendIdentifyQueryReq(identifyTime, TRUE);
          }
#endif
      }
      if(events & gReceivedIdentifyRsp_c)
      {
#if gASL_EzCommissioning_EnableGroupBindCapability_d
        if(EZCommissioningConfigData(gEZCommissioningUseGroup))
        {
          EZCommissioning_SetState(EZCommissioning_GroupBinding_c);
          #if gEndDevCapability_d || gComboDeviceCapability_d
              TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); 
          #else      
              TS_SendEvent(gEzCmsTaskId, gPerformGroupBind_c);
          #endif  
        }
        else
#endif
        {
          EZCommissioning_SetState(EZCommissioning_Binding_c);
          TS_SendEvent(gEzCmsTaskId, gSendSimpleDescReq_c);
        }
      }
      if(events & gIdentifyEnd_c)
      {
    	EZCommissioning_SetState(EZCommissioning_InitialState_c);		
    	TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); /*stop FastPoll Rate -  EndDevice*/		
      }
      if(events & gUpdatePollRate_c)
      {      
#if gEndDevCapability_d || gComboDeviceCapability_d
    	  EZCommissioning_UpdatePollRate(TRUE);
#endif
      }
      break;
    }
#if gASL_EzCommissioning_EnableGroupBindCapability_d
    case EZCommissioning_GroupBinding_c:
    {
      if(events & gPerformGroupBind_c)
      {
        EZCommissioning_PerformGroupBind(indexDevice);
      }
#if gEndDevCapability_d || gComboDeviceCapability_d      
      if(events & gUpdatePollRate_c)
      {
    	EZCommissioning_UpdatePollRate(FALSE);  
    	TS_SendEvent(gEzCmsTaskId, gPerformGroupBind_c);
      }
#endif       
      if(events & gUpdateDevice_c)
      {   	
        BeeAppUpdateDevice(BeeAppDataInit(appEndPoint), gZclUI_EZCommissioning_Succesfull_c, 0, 0, NULL);
        
        EZCommissioningConfigData(gEZCommissioning_LastEvent) = 0x00;
      }      
      break;
    }
#endif
    case EZCommissioning_Binding_c:
    {
#if gASL_EnableEZCommissioning_Initiator_d   	
      if(events & gSendSimpleDescReq_c)
      {
#if gSimple_Desc_req_d
        EZComissioning_SendSimpleDescReq();
#endif
      }
      if(events & gReceivedSimpleDescRsp_c)
      {
        if(EZComissioning_DevBindMatchClusters(indexDevice))
        {

          EZCommissioningConfigData(gaEZComDBTable.waitForIeeeAddrRsp) = FALSE;
          if(APS_GetIeeeAddress(EZCommissioningConfigData(gaEZComDBTable.aBindingTarget), EZCommissioningConfigData(gaEZComDBTable.aSrcIeeeAddress)))
            TS_SendEvent(gEzCmsTaskId, gPerformBind_c);
          else
          {
            EZCommissioningConfigData(gaEZComDBTable.waitForIeeeAddrRsp) = TRUE;
            (void)ASL_IEEE_addr_req(NULL, EZCommissioningConfigData(gaEZComDBTable.aBindingTarget), EZCommissioningConfigData(gaEZComDBTable.aBindingTarget), 0x00, 0x00);
          }
        }
        else
        {
          /* Ez Commissioning failed */
          EZCommissioning_SetState(EZCommissioning_InitialState_c);
          TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); /*stop FastPoll Rate -  EndDevice*/
        }
      }

      if(events & gReceivedIEEEAddress_c)
      {
        TS_SendEvent(gEzCmsTaskId, gPerformBind_c);
      }
      
      if(events & gPerformBind_c)
      {
        EZCommissioning_SendBind();
      }
#if gEndDevCapability_d || gComboDeviceCapability_d      
      if(events & gUpdatePollRate_c)
      {
    	EZCommissioning_UpdatePollRate(FALSE);  
    	TS_SendEvent(gEzCmsTaskId, gUpdateDevice_c);
      }
#endif      
      if(events & gUpdateDevice_c)
      {     	
        BeeAppUpdateDevice(BeeAppDataInit(appEndPoint), gZclUI_EZCommissioning_Succesfull_c, 0, 0, NULL); 
        EZCommissioningConfigData(gEZCommissioning_LastEvent) = 0x00;
      }      
#endif      
      break;
    }
    case EZCommissioning_IdleState_c:
    default:    	
      break;
  }
}
#if gASL_EnableEZCommissioning_Initiator_d
/*!
 * @fn 		static void EZComissioning_SendIdentifyQueryReq(uint16_t time, bool_t senderInIdentify)
 *
 * @brief	Send over the air an IdentifyQueryReq (Broadcast). 
 *
 */
static void EZComissioning_SendIdentifyQueryReq(uint16_t time, bool_t senderInIdentify)
{
   zclIdentifyQueryReq_t *pReq;
   afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0xFF, 0xFF}, 0x08, {gaZclClusterIdentify_c}, 0x00, gApsTxOptionNone_c, 1};
   uint8_t len = 0;
   
   pReq = AF_MsgAlloc();
           
   if(!pReq)
      return;
           
   /* get address information */
   addrInfo.radiusCounter = afDefaultRadius_c;
#if gNum_EndPoints_c != 0    
   addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
   addrInfo.dstEndPoint = 0xff;
#endif  
   FLib_MemCpy(&pReq->addrInfo, &addrInfo, sizeof(addrInfo));
   /*send Identify Query Request - Broadcast */
   (void)zclIdentify_IdentifyQueryReq(pReq, len);
          
   MSG_Free(pReq);
}

/*!
 * @fn 		static void EZComissioning_SendIdentifyQueryReq(uint16_t time, bool_t senderInIdentify)
 *
 * @brief	Send over the air an Simple Descriptor Request to a specific EZ-mode Target device. 
 *
 */
#if gSimple_Desc_req_d
static void EZComissioning_SendSimpleDescReq(void)
{
  if(EZCommissioningConfigData(gpEZCommissioningTempData))
    ASL_Simple_Desc_req(NULL, ((zbApsdeDataIndication_t *)EZCommissioningConfigData(gpEZCommissioningTempData))->aSrcAddr, ((zbApsdeDataIndication_t *)EZCommissioningConfigData(gpEZCommissioningTempData))->srcEndPoint);
}
#endif

#endif
/*!
 * @fn 		static bool_t EZComissioning_DevBindMatchClusters(void)
 *
 * @brief	Checks if the Identify Query Rsp entry matches clusters with the original entry. Return TRUE if is does, FALSE if not
 *
 */
#if gASL_EnableEZCommissioning_Initiator_d
static bool_t EZComissioning_DevBindMatchClusters(uint8_t indexDevice)
{
  bool_t  fFoundMatch = FALSE;
  uint8_t matchIndex = 0;
  zbClusterId_t *tempApp, *tempRcv;
  uint8_t noInClusters = 0, noOutClusters = 0, offset = 0, currentIndex = 0;
  zbSimpleDescriptorResponse_t* pSimpleDescRsp = (zbSimpleDescriptorResponse_t*)EZCommissioningConfigData(gpEZCommissioningTempData);

  if(EZCommissioningConfigData(gaEZComDBTable.aState) == EZCommissioning_BindTableNotAvailable_c)
    return fFoundMatch;
  
#if gNum_EndPoints_c != 0   
  /*
    check if the output device clusters  match to the input clusters of the second
    device. 
  */
  tempApp = (void *)EZCommissioningConfigData(gEZModeDeviceInf[indexDevice].pClusterList->pAppOutClusterList); 
  noInClusters = pSimpleDescRsp->simpleDescriptor.inClusterList.cNumClusters;
  tempRcv = (void *)((uint8_t*)(&pSimpleDescRsp->simpleDescriptor.inClusterList) + 1);
  offset = noInClusters;
  ZbZdoPrivateData(giMatchIndex) = 0;
  if (Zdp_MatchClusters(EZCommissioningConfigData(gEZModeDeviceInf[indexDevice].pClusterList->appNumOutClusters), 
                        noInClusters,
                        tempApp,
                        tempRcv,
                        TRUE))
  {
    matchIndex = (ZbZdoPrivateData(giMatchIndex) > EZCommissioning_MaxNoOfBoundClusterPerDevice_d)? EZCommissioning_MaxNoOfBoundClusterPerDevice_d: ZbZdoPrivateData(giMatchIndex);
    FLib_MemCpy(&EZCommissioningConfigData(gaEZComDBTable.aMatchCluster), &ZbZdoPrivateData(gaZdpMatchedOutCluster),(sizeof(zbClusterId_t) * matchIndex));
    EZCommissioningConfigData(gaEZComDBTable.iMatchIndex) = matchIndex;
    fFoundMatch = TRUE;
  }  
  /*
    check if the input device clusters  match to the output clusters of the second
    device. 
  */
  tempApp = (void *)EZCommissioningConfigData(gEZModeDeviceInf[indexDevice].pClusterList->pAppInClusterList);
  noOutClusters = pSimpleDescRsp->simpleDescriptor.inClusterList.aClusterList[offset][0];
  tempRcv = (void *)((uint8_t*)(&pSimpleDescRsp->simpleDescriptor.inClusterList.aClusterList[offset][1]));
  ZbZdoPrivateData(giMatchIndex) = 0;
  if (Zdp_MatchClusters(EZCommissioningConfigData(gEZModeDeviceInf[indexDevice].pClusterList->appNumInClusters), 
                        noOutClusters,
                        tempApp,
                        tempRcv,
                        TRUE))
  {
	if(ZbZdoPrivateData(giMatchIndex) + EZCommissioningConfigData(gaEZComDBTable.iMatchIndex) > EZCommissioning_MaxNoOfBoundClusterPerDevice_d)  
		matchIndex = EZCommissioning_MaxNoOfBoundClusterPerDevice_d - EZCommissioningConfigData(gaEZComDBTable.iMatchIndex);
	else
		matchIndex = ZbZdoPrivateData(giMatchIndex);
    currentIndex = EZCommissioningConfigData(gaEZComDBTable.iMatchIndex);
    FLib_MemCpy(&EZCommissioningConfigData(gaEZComDBTable.aMatchCluster[currentIndex]), &ZbZdoPrivateData(gaZdpMatchedOutCluster),(sizeof(zbClusterId_t) * matchIndex));
    EZCommissioningConfigData(gaEZComDBTable.iMatchIndex) += matchIndex;
    fFoundMatch = TRUE;
  } 
  
#endif /* gNum_EndPoints_c != 0  */
  
  if(fFoundMatch == TRUE)
  {
    Copy2Bytes(EZCommissioningConfigData(gaEZComDBTable.aBindingTarget), pSimpleDescRsp->aNwkAddrOfInterest);
    //APS_GetIeeeAddress(pSimpleDescRsp->aNwkAddrOfInterest, gaEZComDBTable.aSrcIeeeAddress);
    EZCommissioningConfigData(gaEZComDBTable.srcEndPoint) = pSimpleDescRsp->simpleDescriptor.endPoint;
    EZCommissioningConfigData(gaEZComDBTable.aState) = EZCommissioning_BindTableNotAvailable_c;
  }
  return fFoundMatch;  
}

/*!
 * @fn 		static void EZCommissioning_SendBind(void)
 *
 * @brief	Create internal one-to-one bindings for matched clusters (only on the Initiator device)
 *
 */
static void EZCommissioning_SendBind(void)
{
  appToZdpMessage_t *pMsg;
  zbBindUnbindRequest_t bindRequest;
  zbClusterId_t clusterId;
  uint8_t currentIndex = 0;

  /* allocate the message */
  pMsg = MSG_AllocType(appToZdpMessage_t);
  if(!pMsg)
  {
    /* Try again later */
    TS_SendEvent(gEzCmsTaskId, gPerformBind_c);
    return;
  }
  
  if(EZCommissioningConfigData(gaEZComDBTable.iMatchIndex) > 0)
  {
    EZCommissioningConfigData(gaEZComDBTable.iMatchIndex)--;
    if(EZCommissioningConfigData(gaEZComDBTable.iMatchIndex))
    {
      TS_SendEvent(gEzCmsTaskId, gPerformBind_c);
    }
    else
    {
      /* Done */
      EZCommissioningConfigData(gaEZComDBTable.aState) = EZCommissioning_BindTableAvailable_c;
      EZCommissioning_AutoClose(EZCommissioning_AutoCloseTimeout_c);
#if gEndDevCapability_d || gComboDeviceCapability_d
      TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); 
#else      
      TS_SendEvent(gEzCmsTaskId, gUpdateDevice_c); 
#endif   
      if(EZCommissioningConfigData(gpEZCommissioningTempData))
        MSG_Free(EZCommissioningConfigData(gpEZCommissioningTempData));
      EZCommissioningConfigData(gpEZCommissioningTempData) = NULL;
    }
  }
  else
  {
    /* Done */
    EZCommissioning_AutoClose(EZCommissioning_AutoCloseTimeout_c);
#if gEndDevCapability_d || gComboDeviceCapability_d
    TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); 
#else      
    TS_SendEvent(gEzCmsTaskId, gUpdateDevice_c); 
#endif 
    if(EZCommissioningConfigData(gpEZCommissioningTempData))
      MSG_Free(EZCommissioningConfigData(gpEZCommissioningTempData));
    EZCommissioningConfigData(gpEZCommissioningTempData) = NULL;
  }
  currentIndex = EZCommissioningConfigData(gaEZComDBTable.iMatchIndex);
  Copy2Bytes(clusterId, EZCommissioningConfigData(gaEZComDBTable.aMatchCluster[currentIndex]));

  /* fill in the message */
  pMsg->msgType = gBind_req_c;
  Copy2Bytes(pMsg->aDestAddr, NlmeGetRequest(gNwkShortAddress_c));
  /* fill in the request */
  Copy8Bytes(bindRequest.aSrcAddress, NlmeGetRequest(gNwkIeeeAddress_c));
#if gNum_EndPoints_c != 0    
  bindRequest.srcEndPoint = BeeAppDataInit(appEndPoint);
#else
  bindRequest.srcEndPoint = 0x08;
#endif
  bindRequest.addressMode = gZbAddrMode64Bit_c;
  Copy8Bytes(bindRequest.destData.extendedMode.aDstAddress, EZCommissioningConfigData(gaEZComDBTable.aSrcIeeeAddress));
  bindRequest.destData.extendedMode.dstEndPoint = EZCommissioningConfigData(gaEZComDBTable.srcEndPoint); 
  Copy2Bytes(bindRequest.aClusterId, clusterId);

  /* copy the request */
  FLib_MemCpy(&pMsg->msgData.bindReq, &bindRequest, sizeof(bindRequest));

  /* call ZDP to send the message out */
  (void)APP_ZDP_SapHandler( pMsg );
}


#if gASL_EzCommissioning_EnableGroupBindCapability_d
/*!
 * @fn 		static void EZCommissioning_PerformGroupBind(void)
 *
 * @brief	Perform internal group binding(only on the Initiator device)
 *
 */
static void EZCommissioning_PerformGroupBind(uint8_t indexDevice)
{
  appToZdpMessage_t *pMsg;
  zbBindUnbindRequest_t bindRequest;
  zbGroupId_t aDestGroupAddr = {EZCommissioning_GroupAddr_c};
  
  if(EZCommissioningConfigData(gpEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1]) == 0xFF)
  {
#if gNum_EndPoints_c != 0
    EZCommissioningConfigData(gpEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1]) = EZCommissioningConfigData(gEZModeDeviceInf[indexDevice].pClusterList->appNumOutClusters);
#else
    EZCommissioningConfigData(gpEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1]) = 0;
#endif
  }
  if(EZCommissioningConfigData(gpEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1]) == 0)
  {
    /* Done */
    if(EZCommissioningConfigData(gpEZCommissioningTempData))
      MSG_Free(EZCommissioningConfigData(gpEZCommissioningTempData));
    EZCommissioningConfigData(gpEZCommissioningTempData) = NULL;
    EZCommissioning_SetState(EZCommissioning_InitialState_c);
    TS_SendEvent(gEzCmsTaskId, gUpdatePollRate_c); /*stop FastPoll Rate -  EndDevice*/
    return;
  }
  else
  {
    EZCommissioningConfigData(gpEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1])--;
  }

  /* allocate the message */
  pMsg = AF_MsgAlloc(); //MSG_AllocType(appToZdpMessage_t);
  if(!pMsg)
  {
    /* Try again later */
    EZCommissioningConfigData(gpEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1])++;
    TS_SendEvent(gEzCmsTaskId, gPerformGroupBind_c);
    return;
  }

  /* fill in the message */
  pMsg->msgType = gBind_req_c;
  Copy2Bytes(pMsg->aDestAddr, NlmeGetRequest(gNwkShortAddress_c));
  /* fill in the request */
  Copy8Bytes(bindRequest.aSrcAddress, NlmeGetRequest(gNwkIeeeAddress_c));
#if gNum_EndPoints_c != 0    
  bindRequest.srcEndPoint = BeeAppDataInit(appEndPoint);
#else
  bindRequest.srcEndPoint = 0x08;
#endif   
  bindRequest.addressMode = gZbAddrModeGroup_c;
  Copy2Bytes(bindRequest.destData.groupMode.aDstaddress, aDestGroupAddr); 
  Copy2Bytes(bindRequest.aClusterId, &EZCommissioningConfigData(gEZModeDeviceInf[indexDevice].pClusterList->pAppOutClusterList[2*EZCommissioningConfigData(gpEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1])]));

  /* copy the request */
  FLib_MemCpy(&pMsg->msgData.bindReq, &bindRequest, sizeof(bindRequest));

  /* call ZDP to send the message out */
  (void)APP_ZDP_SapHandler( pMsg );
  
  if(EZCommissioningConfigData(gpEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1]) == 0)
  {
    /* Send AddGrioupIfIdentify request */
    zclGroupAddGroupIfIdentifyingReq_t* pAddGroupIfIdentifyReq;
    zbApsdeDataIndication_t* pSourceInfo;
    pSourceInfo = (zbApsdeDataIndication_t*)EZCommissioningConfigData(gpEZCommissioningTempData);
    
    pAddGroupIfIdentifyReq = MSG_Alloc(sizeof(zclGroupAddGroupIfIdentifyingReq_t) + 9);//9 represents the TestGroup string length
    
    if(!pAddGroupIfIdentifyReq)
    {
      /* Try again later */
      EZCommissioningConfigData(gpEZCommissioningTempData[gEZCommisioning_TempDataLength_d-1])++;
      TS_SendEvent(gEzCmsTaskId, gPerformGroupBind_c);
      return;
    }
    /* Fill destination information */
    pAddGroupIfIdentifyReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
    Copy2Bytes(pAddGroupIfIdentifyReq->addrInfo.dstAddr.aNwkAddr, pSourceInfo->aSrcAddr);
    pAddGroupIfIdentifyReq->addrInfo.dstEndPoint = pSourceInfo->srcEndPoint;
    Set2Bytes(pAddGroupIfIdentifyReq->addrInfo.aClusterId, gZclClusterGroups_c);
    pAddGroupIfIdentifyReq->addrInfo.srcEndPoint = BeeAppDataInit(appEndPoint);
    pAddGroupIfIdentifyReq->addrInfo.txOptions = 0;
    pAddGroupIfIdentifyReq->addrInfo.radiusCounter = afDefaultRadius_c;
    
    Copy2Bytes(pAddGroupIfIdentifyReq->cmdFrame.aGroupId, aDestGroupAddr); 
    FLib_MemCpy(pAddGroupIfIdentifyReq->cmdFrame.szGroupName, gGroupName, 10);
    
    (void)ASL_ZclGroupAddGroupIfIdentifyReq(pAddGroupIfIdentifyReq);
    /* Done */
    if(EZCommissioningConfigData(gpEZCommissioningTempData))
      MSG_Free(EZCommissioningConfigData(gpEZCommissioningTempData));
    EZCommissioningConfigData(gpEZCommissioningTempData) = NULL;
    EZCommissioning_AutoClose(EZCommissioning_AutoCloseTimeout_c);    
    TS_SendEvent(gEzCmsTaskId, gUpdateDevice_c); 
  }
  else
  {
    TS_SendEvent(gEzCmsTaskId, gPerformGroupBind_c);
  }
}
#endif /* gASL_EzCommissioning_EnableGroupBindCapability_d */
#endif /* gASL_EnableEZCommissioning_Initiator_d */

/*!
 * @fn 		void EZCommissioning_AutoClose(uint32_t timeout)
 *
 * @brief	Called in case of succes to close an EZ-Mode Commissioning session
 *
 */
void EZCommissioning_AutoClose(uint32_t timeout)
{
  
  uint8_t identifyCommissioningState = 0;
  zbClusterId_t identifyClusterId = {gaZclClusterIdentify_c};
        
  /* get CommissioningState */
  (void)ZCL_GetAttribute(BeeAppDataInit(appEndPoint), identifyClusterId, gZclAttrIdentify_CommissioningState_c, gZclServerAttr_c, &identifyCommissioningState, NULL);
  
  
  if(!timeout)
  {
    if(!(identifyCommissioningState&gZclCommissioningState_OperationalState_d))
    {
    	identifyCommissioningState |= gZclCommissioningState_OperationalState_d;
        (void)ZCL_SetAttribute(BeeAppDataInit(appEndPoint), identifyClusterId, gZclAttrIdentify_CommissioningState_c, gZclServerAttr_c, &identifyCommissioningState);
    	ZCL_SaveNvmZclData();
    }	  
	
#if gASL_EnableEZCommissioning_Initiator_d
    {
#if gASL_ZclIdentifyReq_d
      zclIdentifyReq_t identifyRequest;
#endif
#if gASL_ZclCmdUpdateCommissioningStateReq_d
      zclUpdateCommissioningStateReq_t updateCommissioningStateRequest;
#endif
      afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0x08, {gaZclClusterIdentify_c}, 0x00, gApsTxOptionNone_c, 1};
      if(EZCommissioningConfigData(gEZCommissioningUseGroup))
      {
        zbApsdeDataIndication_t* pSourceInfo;
        pSourceInfo = (zbApsdeDataIndication_t*)EZCommissioningConfigData(gpEZCommissioningTempData);        
        Copy2Bytes(addrInfo.dstAddr.aNwkAddr, pSourceInfo->aSrcAddr);
        addrInfo.dstEndPoint = pSourceInfo->srcEndPoint;
      }
      else
      {
        Copy2Bytes(addrInfo.dstAddr.aNwkAddr, EZCommissioningConfigData(gaEZComDBTable.aBindingTarget));
        addrInfo.dstEndPoint = EZCommissioningConfigData(gaEZComDBTable.srcEndPoint);
      }
      addrInfo.radiusCounter = afDefaultRadius_c;
      addrInfo.srcEndPoint = ZCL_GetEndPointForSpecificCluster(addrInfo.aClusterId, TRUE, 0, NULL);
#if gASL_ZclIdentifyReq_d
	  /* Set the Identify Time on the Target to 0 */
      FLib_MemCpy(&identifyRequest.addrInfo, &addrInfo, sizeof(addrInfo));
      identifyRequest.cmdFrame.iTimeIdentify = 0;
      (void)ASL_ZclIdentifyReq(&identifyRequest);
#endif

#if gASL_ZclCmdUpdateCommissioningStateReq_d
      /* Notify the Target that it has been commissioned */
      FLib_MemCpy(&updateCommissioningStateRequest.addrInfo, &addrInfo, sizeof(addrInfo));
      updateCommissioningStateRequest.cmdFrame.action = 0x01;
      updateCommissioningStateRequest.cmdFrame.commissioningStateMask = (gZclCommissioningState_NetworkState_d | gZclCommissioningState_OperationalState_d);
      (void)ASL_ZclUpdateCommissioningStateReq(&updateCommissioningStateRequest);
#endif
    }
#endif    
    EZCommissioning_SetState(EZCommissioning_InitialState_c);
    ZCL_SetIdentifyMode(BeeAppDataInit(appEndPoint), 0);   
  }
  else
  {
    ZbTMR_StartSingleShotTimer(EZCommissioningConfigData(gEZCommissioningAutocloseTimerId), timeout, EZCommissioning_AutoCloseTimerCallback);
  }
}

/*!
 * @fn 		void EZCommissioning_AutoCloseTimerCallback(zbTmrTimerID_t timerId)
 *
 * @brief	EZ-Mode Commissioning Auto Close Timer Callback
 *
 */
void EZCommissioning_AutoCloseTimerCallback(zbTmrTimerID_t timerId)
{
   (void)timerId;
   EZCommissioning_AutoClose(0);
}

/*!
 * @fn 		void EZCommissioning_VerifyIEEEaddrRsp(zbIeeeAddr_t  aIeeeAddr)
 *
 * @brief	Check and temporarily store the target IEEEaddress(used into a multi-hop solution)
 *
 */
#if gASL_EnableEZCommissioning_Initiator_d 
void EZCommissioning_VerifyIEEEaddrRsp(zbIeeeAddr_t  aIeeeAddr)
{
  if(EZCommissioningConfigData(gaEZComDBTable.waitForIeeeAddrRsp))
  {
   FLib_MemCpy(EZCommissioningConfigData(gaEZComDBTable.aSrcIeeeAddress), aIeeeAddr, 8);
   TS_SendEvent(gEzCmsTaskId, gReceivedIEEEAddress_c);
  }
}
#endif

/*!
 * @fn 		void EZCommissioning_Reset(void)
 *
 * @brief	Perform EZ-Mode Commissioning Reset
 *
 */
void EZCommissioning_Reset(void)
{
  uint8_t identifyCommissioningState = 0;
  zbClusterId_t identifyClusterId = {gaZclClusterIdentify_c};
        
  
#if gComboDeviceCapability_d
  if(!EZCommissioningConfigData(gEZCommissioningNetworkDiscoveryAttempts) && EZCommissioning_GetState() == EZCommissioning_NetworkSteering_c)
  {
    /* The device failed to join a network. Do not reset the device, it will start as a coordinator */
    return;
  }
#endif
  
  identifyCommissioningState = 0;
  
  /* Set CommissioningState */
  #if gInstantiableStackEnabled_d
  for(uint8_t i=0; i<EndPointConfigData(gNum_EndPoints); i++)
  #else
  for(uint8_t i=0; i<gNum_EndPoints_c; i++)
  #endif  
     (void)ZCL_SetAttribute(EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint), identifyClusterId, gZclAttrIdentify_CommissioningState_c, gZclServerAttr_c, &identifyCommissioningState);
   
  /* Save the new commissioning state in NVM */
  ZdoNwkMng_SaveToNvm(zdoNvmObject_AtomicSave_c, NULL);
  
  if(EZCommissioningConfigData(gEZCommissioning_ResetDevice))
  {    
     /* Reset the device */
     PWRLib_Reset(); 	
  }
  EZCommissioning_SetState(EZCommissioning_InitialState_c);
  ZbTMR_StopTimer(EZCommissioningConfigData(gEZCommissioningAutocloseTimerId));

#if gASL_EnableEZCommissioning_Initiator_d
  if(EZCommissioningConfigData(gpEZCommissioningTempData))
    MSG_Free(EZCommissioningConfigData(gpEZCommissioningTempData));
  EZCommissioningConfigData(gpEZCommissioningTempData) = NULL;
#endif

  ASL_ChangeUserInterfaceModeTo(gConfigureMode_c);
  ASL_DisplayChangeToCurrentMode(gmUserInterfaceMode);
}

/*!
 * @fn 		static void EZCommissioning_UpdatePollRate(bool_t fastPollRate)
 *
 * @brief	Update poll rate for an EndDevice.
 *
 */
 #if gEndDevCapability_d || gComboDeviceCapability_d
static void EZCommissioning_UpdatePollRate(bool_t fastPollRate)
{
  static uint16_t orgPollRate = mDefaultValueOfIndirectPollRate_c-1;
  uint16_t pollRate = 0x00;
	
  if (!IsLocalDeviceReceiverOnWhenIdle())
  {
      /* update poll rate during the Finding And Binding process: go to FastPollMode */ 	
      if((fastPollRate)&&(NlmeGetRequest(gNwkIndirectPollRate_c)!=orgPollRate))
      {
          /* ZED: remember original polling rate */
          orgPollRate = NlmeGetRequest(gNwkIndirectPollRate_c);
          /* set the new poll Rate */
          pollRate = 250;
          #if gZclEnablePollControlCluster_d	
          pollRate = (ZclPollControl_GetMaxShortPollIntervalValue())*1000/4;                     
          #endif
      }
      else
      {
          if(NlmeGetRequest(gNwkIndirectPollRate_c) < orgPollRate)
          {
              pollRate = orgPollRate;		
              orgPollRate--;
          }
      }
      if(pollRate)
      {
	(void)ZDO_NLME_ChangePollRate(pollRate);
        /* Save the new pollRate in NVM */
        #ifdef PROCESSOR_KINETIS
        NvSaveOnIdle(&ZbBeeStackGlobalsParams(gSAS_Ram), TRUE);
        #else		    
          ZCL_SaveNvmZclData();
        #endif 
        }
  }   
}
#endif  /* gEndDevCapability_d || gComboDeviceCapability_d */


/*****************************************************************************
* GetIndexFromEZModeDeviceTable
*
* return the index from EZModeDeviceTable if endpoint exist, otherwise return 
*       gEZModeInvalidIndex_d
*****************************************************************************/
uint8_t GetIndexFromEZModeDeviceTable(uint8_t endpoint)
{
#if !gInstantiableStackEnabled_d  
  for(uint8_t i=0; i<gNum_EndPoints_c; i++)
    if(EZCommissioningConfigData(gEZModeDeviceInf[i].endpoint) == endpoint)
      return i;
  return gEZModeInvalidIndex_d;
#else
  return 0;  
#endif 
}

#if(gInstantiableStackEnabled_d == 1)
void EZModeDataInitVariables(uint8_t instId)
{
  uint8_t i;
  char groupName[10] = {0x09, 0x54, 0x65, 0x73, 0x74, 0x47, 0x72, 0x6f, 0x75, 0x70};
  
  EZModeContextSwitch(instId);
  
  for(i=0;i<10;i++)
    pEzCommisioningData->gGroupName[i] = groupName[i];
  
  pEzCommisioningData->gEZCommissioningAutocloseTimerId = gTmrInvalidTimerID_c;
  pEzCommisioningData->gEZCommissioningOneShoot = FALSE;
  pEzCommisioningData->gEZCommissioningUseGroup = FALSE;
  pEzCommisioningData->gEZCommissioning_ResetDevice = FALSE;
  pEzCommisioningData->gEZCommissioning_LastEvent = 0x00;
#if gASL_EnableEZCommissioning_Initiator_d
  pEzCommisioningData->gpEZCommissioningTempData = NULL;
#endif
#if gComboDeviceCapability_d
  pEzCommisioningData->gEZCommissioningNetworkDiscoveryAttempts = 0x00;
#endif
  pEzCommisioningData->gEZCommissioningState = EZCommissioning_IdleState_c;
  if(zbProInstance == 1)
  {
    pEzCommisioningData->gEZCommissioningPrimaryDeviceType = EZCommissioning_PrimaryDeviceType_dPanId2_d;
  }
  else
  {
    pEzCommisioningData->gEZCommissioningPrimaryDeviceType = EZCommissioning_PrimaryDeviceType_d;
  }
}

void EZModeContextSwitch(uint8_t instId)
{
  pEzCommisioningData = &EzCommisioningData[instId];
}     
#endif     
     
#endif /* gASL_EnableEZCommissioning_d */
