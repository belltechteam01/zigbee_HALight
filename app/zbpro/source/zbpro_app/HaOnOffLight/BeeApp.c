/*****************************************************************************
* BeeApp.c
*
* This application (HaOnOffLight) contains the code necessary for an on/off
* light. It is controled over-the-air by an HaOnOffSwitch, but can also turn
* on it's local light.
*
* Copyright (c) 2013, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* USER INTERFACE
* --------------
*
* Like most BeeStack sample applications, this application uses the the common
* ASL user interface. The ASL interface has two "modes" to allow the keys, LEDs
* and (optional on NCB) the LCD display to be used for multiple purposes.
* "Configuration" mode includes commands such as starting and leaving the network,
* whereas "application" mode includes application specific functions and display.
*
* Each key (switch) can be pressed for a short duration (short press) or
* long duration of about 1+ seconds (long press). Examples include SW1 or LSW3.
*
* Application specific code can be found here. Code common among applications
* can be found in ASL_UserInterface.c
*
* Configure Mode (EZ Mode Commissioning Enabled: gASL_EnableEZCommissioning_d = TRUE):
* SW1  - Start EZ-Mode Commissioning procedure (Network Steering followed by Finding and Binding)
* SW2  - Start EZ-Mode Commissioning procedure (Only Network Steering)
* SW3  - Start EZ-Mode Commissioning procedure (Only Finding and Binding)
* SW4  - Choose channel (Cycle through single start-up channel). Only functional when NOT on network.
* LSW1 - Toggle display/keyboard mode (Configure mode/ Application mode)
* LSW2 - Reset to Factory Defaults(EZ-Mode Commissioning procedure - FactoryFresh)
* LSW3 - Reset to Factory Defaults(EZ-Mode Commissioning procedure - FactoryFresh)
* LSW4 - 
*
* OnOffSwitch Application Mode:
* SW1  - Toggle local light
* SW2  -
* SW3  - (ASL) Toggle Identify mode on/off (will stay on for 20 seconds)
* SW4  - (ASL) Recall scene(must store scene  first with LSW4)
* LSW1 - (ASL) Toggle display/keyboard mode (Config and Application)
* LSW2 -
* LSW3 - Add any OnOffLights in identify mode to a group
* LSW4 - Add any OnOffLights in identify mode to scene
*****************************************************************************/
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "BeeStack_Globals.h"
#include "BeeStackConfiguration.h"
#include "AppZdoInterface.h"
#include "TS_Interface.h"
#include "TMR_Interface.h"
#include "AppAfInterface.h"
#include "FunctionLib.h"
#include "EndPointConfig.h"
#include "BeeApp.h"
#include "ZDOStateMachineHandler.h"
#include "ZdoApsInterface.h"
#include "BeeAppInit.h"
#include "NVM_Interface.h"
#include "ZtcInterface.h"
#include "ASL_ZdpInterface.h"
#include "ASL_UserInterface.h"
#include "HaProfile.h"
#include "ZigbeeTask.h"
#if gASL_EnableEZCommissioning_d
#include "EzCommissioning.h"
#endif


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/
void BeeAppTask(tsEvent_t events);
void BeeAppDataIndication(void);
void BeeAppDataConfirm(void);
#if gInterPanCommunicationEnabled_c
void BeeAppInterPanDataConfirm(void);
void BeeAppInterPanDataIndication(void);
#endif 
zbStatus_t APSME_AddGroupRequest(zbApsmeAddGroupReq_t *pRequest);
void OnOffLight_AppSetLightState(zbEndPoint_t endPoint, zclCmd_t command);
void OnOffLight_AddGroups(void);

void BeeAppGenericTimerCallback(zbTmrTimerID_t timerID);
#if gASL_EnableEZCommissioning_d
void BeeApp_EzCommissioningInit(void);
#endif
/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

/* HomeAutomation 1.2 - Section 5.3.2 - Join Parameter: 
   A device may attempt to rejoin for a period of maximum 15 minutes, and should
   back off for minimum 15 minutes before attempting to rejoin again*/ 
#define gAppMaxZdoNwkDiscTime_d         900     /* 15 min */ 
#define gAppZdoNwkDiscPauseTime_d       900     /* 15 min */

 
/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

#if gEndDevCapability_d || gComboDeviceCapability_d
/* poll rate information: */
#define gAppMediumPollRateTimeout_d     240     /* 4 mim */ 
#define gAppMediumPollRate_d            8000    /* 8 sec */
#endif /* gEndDevCapability_d || gComboDeviceCapability_d */

#if !gInstantiableStackEnabled_d && gASL_EnableEZCommissioning_d
EZ_ModeDeviceInf_t gEZModeDeviceInf[gNum_EndPoints_c];
#endif

#if gInstantiableStackEnabled_d
  #define gmUserInterfaceMode gmUserInterfaceMode[zbProInstance]
#endif
/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/


/*****************************************************************************
* BeeAppInit
*
* Initializes the application
*
* Initialization function for the App Task. This is called during
* initialization and should contain any application specific initialization
* (ie. hardware initialization/setup, table initialization, power up
* notification.
*
*****************************************************************************/
void BeeAppInit
  (
  void
  )
{
  index_t i;
  bool_t updateDeviceData = TRUE;
#if gASL_EnableEZCommissioning_d 
  uint8_t identifyCommissioningState = 0;
  zbClusterId_t identifyClusterId = {gaZclClusterIdentify_c};       
#endif 

#if gASL_EnableEZCommissioning_d  && gInstantiableStackEnabled_d
  bool_t processRestoredData = FALSE;
#endif  
  
  /* register the application endpoint(s), so we receive callbacks */
  #if gInstantiableStackEnabled_d
    for(i=0; i<EndPointConfigData(gNum_EndPoints); ++i){
  #else
    for(i=0; i<gNum_EndPoints_c; ++i){
  #endif  
    (void)AF_RegisterEndPoint(EndPointConfigData(endPointList[i].pEndpointDesc));
  }

  /* where to send switch commands from */
  BeeAppDataInit(appEndPoint) = EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint);

  /* initialize common user interface */
  ASL_InitUserInterface("HaOnOffLight");

  /* control of external light (if available) */
  ExternalDeviceInit();

  /* init application timers */
  BeeAppDataInit(gAppGenericTimerId) = ZbTMR_AllocateTimer();
  
#if(gInstantiableStackEnabled_d == 1)    
  ZbBeeStackGlobalsParams(gaEndPointDesc[ 0 ].pDescription) = pEndPointData->endPoint0Desc;
  ZbBeeStackGlobalsParams(gaEndPointDesc[ 1 ].pDescription) = pEndPointData->broadcastEndPointDesc;  
#endif 
                          
  /* init poll control clusetr */
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)     
      if (!IsLocalDeviceReceiverOnWhenIdle())  
        zclPollControl_ClusterServerInit();
#endif   
      
#if gASL_EnableEZCommissioning_d     
  /* EZ commissioning Init */
  BeeApp_EzCommissioningInit();

#if !gInstantiableStackEnabled_d  
  /* restore application dataSet */  
  if(gNVM_OK_c == NvRestoreDataSet(&gHaDevicePan0Data, TRUE))  
#else
  if(0 == zbProInstance)
  {
    if(gNVM_OK_c == NvRestoreDataSet(&gHaDevicePan0Data, TRUE))  
    {
      processRestoredData = TRUE;
    }
  }
  else
  {
    if(gNVM_OK_c == NvRestoreDataSet(&gHaDevicePan1Data, TRUE))  
    {
      processRestoredData = TRUE;
    }
  }
  if(processRestoredData)
#endif      
  {
     uint8_t endpoint = ZCL_GetEndPointForSpecificCluster(identifyClusterId, TRUE, 0, NULL);
     /* get commissioning State */
     (void)ZCL_GetAttribute(endpoint, identifyClusterId, gZclAttrIdentify_CommissioningState_c, gZclServerAttr_c, &identifyCommissioningState, NULL);
     
     /* check commissioning state*/ 
     if (identifyCommissioningState & gZclCommissioningState_NetworkState_d) 
     {
        ZDO_Start(gStartSilentRejoinWithNvm_c);
        updateDeviceData = FALSE;
     }
  }
#endif  
  
  if(updateDeviceData)
  {
    BeeApp_FactoryFresh();
  } 
  
  NlmeSetRequest(gNwkIsConcentrator_c, FALSE);
}

/*****************************************************************************
* BeeAppTask
*
* ZigBee Application Task event processor.  This function is called to
* process all events for the task. Events include timers, messages and any
* other user defined events
******************************************************************************/
void BeeAppTask
  (
  tsEvent_t events    /*IN: events for the application task */
  )
{
  /* received one or more data confirms */
  if(events & gAppEvtDataConfirm_c)
    BeeAppDataConfirm();

  /* received one or more data indications */
  if(events & gAppEvtDataIndication_c)
    BeeAppDataIndication();
  
#if gInterPanCommunicationEnabled_c
    /* received one or more data confirms */
  if(events & gInterPanAppEvtDataConfirm_c)
    BeeAppInterPanDataConfirm();

  /* received one or more data indications */
  if(events & gInterPanAppEvtDataIndication_c)
    BeeAppInterPanDataIndication();
#endif 

  if(events & gAppEvtAddGroup_c)
    ASL_ZclAddGroupHandler();

  if(events & gAppEvtStoreScene_c)
    ASL_ZclStoreSceneHandler();

  if(events & gAppEvtSyncReq_c)
    ASL_Nlme_Sync_req(FALSE);
}

/*****************************************************************************
* BeeAppHandleKeys
*
* Handles all key events for this node. See also KBD_Init().
*
*
* The default keyboard handling uses a model system: a network configuration-mode
* and an application run-mode. Combined with the concepts of short and
* long-press, this gives the application a total of 16 keys on a 4 button system
* (4 buttons * 2 modes * short and long).
*
* Config-mode covers joining and leaving a network, binding and other
* non-application specific keys, and are common across all Freescale applications.
*
* Run-mode covers application specific keys.
*
*****************************************************************************/
void BeeAppHandleKeys
  (
  key_event_t events  /*IN: Events from keyboard modul */
  )
{
  /* Application-mode keys */
  if( gmUserInterfaceMode == gApplicationMode_c ) 
  {
      switch ( events ) 
      {
      	  /* Toggle local light */
      	  case gKBD_EventSW1_c:
      		  OnOffLight_AppSetLightState(BeeAppDataInit(appEndPoint), gZclCmdOnOff_Toggle_c);
      		  break;

      	  /* not used by application */
      	  case gKBD_EventSW2_c:
      		  break;

      	  /* not used by application */
      	  case gKBD_EventLongSW2_c:
      		  break;

      	  /* all other keys handled by ASL */
      	  default:
      		  ASL_HandleKeys(events);
      		  break;
      }
  }
  else 
  {
	  /* If you are not on Appication Mode, then call ASL_HandleKeys, to be eable to use the Configure Mode */
	  ASL_HandleKeys(events);
  }
}

/*****************************************************************************
* BeeAppUpdateDevice
*
* Contains application specific
*
*
* The default keyboard handling uses a model system: a network configuration-mode
* and an application run-mode. Combined with the concepts of short and
* long-press, this gives the application a total of 16 keys on a 4 button system
* (4 buttons * 2 modes * short and long).
*
* Config-mode covers joining and leaving a network, binding and other
* non-application specific keys, and are common across all Freescale applications.
*
* Run-mode covers application specific keys.
*
*****************************************************************************/
void BeeAppUpdateDevice
  (
  zbEndPoint_t endPoint,    /* IN: endpoint update happend on */
  zclUIEvent_t event,        /* IN: state to update */
  zclAttrId_t attrId, 
  zbClusterId_t clusterId, 
  void *pData
  )
{
 (void) attrId;
 (void) clusterId;
 (void) pData;
 
  switch (event)
  {
    	case gZclUI_Off_c:
          {
               
               #if !gInstantiableStackEnabled_d  
               if(EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint) == endPoint)
                  ASL_SetLed(LED2,gLedOff_c);
                else
                  ASL_SetLed(LED4,gLedOff_c);
                #else
                if(0 == zbProInstance)
                {
                  ASL_SetLed(LED2,gLedOff_c);
                }
                else
                {
                  ASL_SetLed(LED4,gLedOff_c);
                }
                #endif
                ASL_LCDWriteString("Light Off" );
    		ExternalDeviceOff();
    		break;
          }

    	case gZclUI_On_c:
          {
                #if !gInstantiableStackEnabled_d 
                if(EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint) == endPoint)
                  ASL_SetLed(LED2,gLedOn_c);
                else
                  ASL_SetLed(LED4,gLedOn_c);
                #else
                if(0 == zbProInstance)
                {
                  ASL_SetLed(LED2,gLedOn_c);
                }
                else
                {
                  ASL_SetLed(LED4,gLedOn_c);
                }
                #endif
    		ASL_LCDWriteString("Light On" );
    		ExternalDeviceOn();
    		break;
          }
    		
    	case gMatchDescriptorSuccess_c: 
    	{
        	if(!InterpretMatchDescriptor(gSendingNwkData.NwkAddrOfIntrest, gSendingNwkData.endPoint))
    		{
    			ASL_UpdateDevice(endPoint,event);
    		}
    		break; 
    	}
	
        case gZDOToAppMgmtNwkDiscovery_c:
        	/* start rejoing procedure: attempt to rejoin for a period of maximum 15 minutes, and should
			back off for minimum 15 minutes before attempting to rejoin again */	
        	ZbTMR_StartSecondTimer(BeeAppDataInit(gAppGenericTimerId), gAppMaxZdoNwkDiscTime_d, BeeAppGenericTimerCallback);
        	break;    
      
        /* Formed/joined network */
        case gZDOToAppMgmtZCRunning_c:
        case gZDOToAppMgmtZRRunning_c:
        case gZDOToAppMgmtZEDRunning_c:    
        {
        	/* Set the full channel mask in case it was changed by using switch 4 */
        	zbChannels_t aFullChannelMask = {0x00, 0xf8, 0xff, 0x07};    	
        	ApsmeSetRequest(gApsChannelMask_c, aFullChannelMask);
        	ZbTMR_StopTimer(BeeAppDataInit(gAppGenericTimerId));
#if gEndDevCapability_d || gComboDeviceCapability_d
        	if (!IsLocalDeviceReceiverOnWhenIdle())
        	{
        		ZbTMR_StartSecondTimer(BeeAppDataInit(gAppGenericTimerId), gAppMediumPollRateTimeout_d, BeeAppGenericTimerCallback);
        	}
#endif

#if gZclEnableReporting_c
                ZCL_CheckAndStartAttrReporting(); 
#endif
        	ASL_UpdateDevice(endPoint,event);
        	break;
        }
    
        case gZDOToAppMgmtStopped_c:
        {
        	ZbTMR_StopTimer(BeeAppDataInit(gAppGenericTimerId)); 
        	ASL_UpdateDevice(endPoint,event);   
        	break;
        }
	  
        default:
        {
        	/* If the recived event is not any of the others then is a Configure mode event, so it is treated on the
      	  	ASL_UpdateDevice, like: StartNetwork, JoinNetwork, Identify, PermitJoin, etc */
        	ASL_UpdateDevice(endPoint,event);
        	break;
        }
    }
}

/*****************************************************************************
* OnOffLight_AppSetLightState
*
* Turn on/off the local light. Note: this will end up ultimately calling
* BeeAppUpdateDevice(), but goes through the ZCL engine.
*****************************************************************************/

void OnOffLight_AppSetLightState
  (
  zbEndPoint_t endPoint,    /* IN: APP endpoint */
  zclCmd_t command          /* IN: */
  )
{
  zclOnOffReq_t req;

  /* set up address info for a unicast to ourselves */
  req.addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
  Copy2Bytes(req.addrInfo.dstAddr.aNwkAddr, NlmeGetRequest(gNwkShortAddress_c));
  req.addrInfo.dstEndPoint = endPoint;
  Set2Bytes(req.addrInfo.aClusterId, gZclClusterOnOff_c);
  req.addrInfo.srcEndPoint = endPoint;
  req.addrInfo.txOptions = gZclTxOptions;
  req.addrInfo.radiusCounter = afDefaultRadius_c;

  /* send the command */
  req.command = command;

  (void)ASL_ZclOnOffReq(&req);
}

/*****************************************************************************
  BeeAppDataIndication

  Process incoming ZigBee over-the-air messages.
*****************************************************************************/
void BeeAppDataIndication(void)
{
  apsdeToAfMessage_t *pMsg;
  zbApsdeDataIndication_t *pIndication;
  zbStatus_t status = gZclMfgSpecific_c;

  while(MSG_Pending(&BeeAppDataInit(gAppDataIndicationQueue)))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &BeeAppDataInit(gAppDataIndicationQueue) );

    /* ask ZCL to handle the frame */
    pIndication = &(pMsg->msgData.dataIndication);
    status = ZCL_InterpretFrame(pIndication);

    /* not handled by ZCL interface, handle cluster here... */
    if(status == gZclMfgSpecific_c)
    {
      /* insert manufacturer specific code here... */	  
	  ZCL_SendDefaultMfgResponse(pIndication);
    }

    /* Free memory allocated by data indication */
    AF_FreeDataIndicationMsg(pMsg);
  }
  
#if gEndDevCapability_d || gComboDeviceCapability_d
  /* change poll rate when received data - used for demo application */
  if (!IsLocalDeviceReceiverOnWhenIdle())
  {
    uint32_t longPollInterval = mDefaultValueOfIndirectPollRate_c;
    
#if gZclEnablePollControlCluster_d
    longPollInterval = (ZclPollControl_GetMinLongPollIntervalValue())*1000/4;
#endif
    
    if(NlmeGetRequest(gNwkIndirectPollRate_c) > longPollInterval)
    {
      (void)ZDO_NLME_ChangePollRate(longPollInterval);
    }
    BeeAppSetMediumPollRate();

  }
#endif
}

/*****************************************************************************
  BeeAppDataConfirm

  Process incoming ZigBee over-the-air data confirms.
*****************************************************************************/
void BeeAppDataConfirm
  (
  void
  )
{
  apsdeToAfMessage_t *pMsg;
  zbApsdeDataConfirm_t *pConfirm;

  while(MSG_Pending(&BeeAppDataInit(gAppDataConfirmQueue)))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &BeeAppDataInit(gAppDataConfirmQueue) );
    pConfirm = &(pMsg->msgData.dataConfirm);

    /* Action taken when confirmation is received. */
    if( pConfirm->status != gZbSuccess_c )
    {
      /* The data wasn't delivered -- Handle error code here */
    }

    /* Free memory allocated in Call Back function */
    MSG_Free(pMsg);
  }
}

#if gInterPanCommunicationEnabled_c

/*****************************************************************************
  BeeAppInterPanDataIndication

  Process InterPan incoming ZigBee over-the-air messages.
*****************************************************************************/
void BeeAppInterPanDataIndication(void)
{
  InterPanMessage_t *pMsg;
  zbInterPanDataIndication_t *pIndication;
  zbStatus_t status = gZclMfgSpecific_c;

  while(MSG_Pending(&gInterPanAppDataIndicationQueue))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &gInterPanAppDataIndicationQueue );

    /* ask ZCL to handle the frame */
    pIndication = &(pMsg->msgData.InterPandataIndication );
    status = ZCL_InterpretInterPanFrame(pIndication);

    /* not handled by ZCL interface, handle cluster here... */
    if(status == gZclMfgSpecific_c)
    {
      /* insert manufacturer specific code here... */
    }

    /* Free memory allocated by data indication */
    MSG_Free(pMsg);
  }
}


/*****************************************************************************
  BeeAppDataConfirm

  Process InterPan incoming ZigBee over-the-air data confirms.
*****************************************************************************/
void BeeAppInterPanDataConfirm
(
void
)
{
  InterPanMessage_t *pMsg;
  zbInterPanDataConfirm_t *pConfirm;
  
  while(MSG_Pending(&gInterPanAppDataConfirmQueue))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &gInterPanAppDataConfirmQueue );
    pConfirm = &(pMsg->msgData.InterPandataConf);
    
    /* Action taken when confirmation is received. */
    if( pConfirm->status != gZbSuccess_c )
    {
      /* The data wasn't delivered -- Handle error code here */
    }
    
    /* Free memory allocated in Call Back function */
    MSG_Free(pMsg);
  }
}
#endif 




/*****************************************************************************
  BeeAppGenericTimerCallback

  Update poll rate interval and rejoin interval(HA1.2)
*****************************************************************************/
void BeeAppGenericTimerCallback(zbTmrTimerID_t timerId)
{
  if(ZDO_IsRunningState())
  {
#if gEndDevCapability_d || gComboDeviceCapability_d	  
    if (!IsLocalDeviceReceiverOnWhenIdle())
    {
      uint32_t longPollInterval = gAppMediumPollRate_d;
#if gZclEnablePollControlCluster_d
      longPollInterval = (ZclPollControl_GetMinLongPollIntervalValue())*1000/4;
#endif  /* gZclEnablePollControlCluster_d */
        
      (void)ZDO_NLME_ChangePollRate(longPollInterval);
    }
#endif  /* gEndDevCapability_d || gComboDeviceCapability_d	*/   
  }
  else
  {
    if(NlmeGetRequest(gRejoinInterval_c) != gAppZdoNwkDiscPauseTime_d)
    {
      ZbTMR_StartSecondTimer(BeeAppDataInit(gAppGenericTimerId), 2 * NlmeGetRequest(gRejoinInterval_c), BeeAppGenericTimerCallback);
      NlmeSetRequest(gRejoinInterval_c, gAppZdoNwkDiscPauseTime_d);
    }
    else
    {
      NlmeSetRequest(gRejoinInterval_c, mDefaultValueOfRejoinInterval_c);
      ZbTMR_StartSecondTimer(BeeAppDataInit(gAppGenericTimerId), gAppZdoNwkDiscPauseTime_d + gAppMaxZdoNwkDiscTime_d - 2 * NlmeGetRequest(gRejoinInterval_c), BeeAppGenericTimerCallback);
    }
  }
}

#if gEndDevCapability_d || gComboDeviceCapability_d
/*****************************************************************************
  BeeAppSetMediumPollRate

*****************************************************************************/
void BeeAppSetMediumPollRate(void)
{
#if gComboDeviceCapability_d
  if(NlmeGetRequest(gDevType_c) != gEndDevice_c)
  {
    return;
  }
#endif
  
  if(!ZDO_IsRunningState())
  {
    return;
  }
  
  ZbTMR_StopTimer(BeeAppDataInit(gAppGenericTimerId));
  ZbTMR_StartSecondTimer(BeeAppDataInit(gAppGenericTimerId), gAppMediumPollRateTimeout_d, BeeAppGenericTimerCallback);
}
#endif


/*****************************************************************************
  BeeApp_FactoryFresh

  Add device to factory fresh state.
*****************************************************************************/
void BeeApp_FactoryFresh(void)
{
#if gInstantiableStackEnabled_d    
  if(0 == zbProInstance) 
  {
#endif    
    /* endpoint1 information */
    gHaDevicePan0Data.reportMask[0] = 0x00;
    gHaDevicePan0Data.basicAttrs = gDefaultBasicAttrData;
    gHaDevicePan0Data.identifyAttrs = gDefaultIdentifyAttrData;
    gHaDevicePan0Data.onOffAttrs = gDefaultOnOffAttrData;
    gHaDevicePan0Data.groupAttrs = gDefaultGroupAttrData;
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)
    gHaDevicePan0Data.pollControl = gDefaultPollControlAttrData;
#endif    
    gHaDevicePan0SceneData.scene = gDefaultSceneAttrData;
    gHaDevicePan0SceneData.scene.sceneTableEntrySize = sizeof(zclOnOffLightSceneTableEntry_t);
#if gInstantiableStackEnabled_d      
  }
  else
  {
#endif    
    /* endpoint2 information */
    gHaDevicePan1Data.reportMask[0] = 0x00;
    gHaDevicePan1Data.basicAttrs = gDefaultBasicAttrData;
    gHaDevicePan1Data.identifyAttrs = gDefaultIdentifyAttrData;
    gHaDevicePan1Data.onOffAttrs = gDefaultOnOffAttrData;
    gHaDevicePan1Data.groupAttrs = gDefaultGroupAttrData;
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)
    gHaDevicePan1Data.pollControl = gDefaultPollControlAttrData;
#endif  
    gHaDevicePan1SceneData.scene = gDefaultSceneAttrData;
    gHaDevicePan1SceneData.scene.sceneTableEntrySize = sizeof(zclOnOffLightSceneTableEntry_t);
#if gInstantiableStackEnabled_d       
  }
#endif  
}


#if gASL_EnableEZCommissioning_d
/*****************************************************************************
  BeeApp_EzCommissioningInit

  Init EZ mode commissioning procedure
*****************************************************************************/
void BeeApp_EzCommissioningInit(void)
{
  for(uint8_t i=0;i<gNum_EndPoints_c; i++)
  {
    EZCommissioningConfigData(gEZModeDeviceInf[i].endpoint) = EndPointConfigData(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint);  
#if !gASL_EnableEZCommissioning_Initiator_d      
    EZCommissioningConfigData(gEZModeDeviceInf[i].isInitiator) = FALSE;  
#endif    
  }
 
#if gASL_EnableEZCommissioning_Initiator_d   
#if gInstantiableStackEnabled_d    
  if(0 == zbProInstance) 
  {   
    EZCommissioningConfigData(gEZModeDeviceInf[0].isInitiator) = FALSE;                             /* set the device role: Target or Initiator */ 
    EZCommissioningConfigData(gEZModeDeviceInf[0].pClusterList) = &gEZModeTargetClusterList;        /* Clusters of interest */        
  }
  else
  {
    EZCommissioningConfigData(gEZModeDeviceInf[0].isInitiator) = FALSE;                              /* set the device role: Target or Initiator */ 
    EZCommissioningConfigData(gEZModeDeviceInf[0].pClusterList) = &gEZModeTargetClusterList;         /* Clusters of interest */        
  } 
#else
  {
    for(uint8_t i=0;i<gNum_EndPoints_c; i++)
    {    
      EZCommissioningConfigData(gEZModeDeviceInf[i].isInitiator) = FALSE;                           /* set the device role: Target or Initiator */
      EZCommissioningConfigData(gEZModeDeviceInf[i].pClusterList) = &gEZModeTargetClusterList;       /* Clusters of interest */  
    } 
  }  
#endif /* gInstantiableStackEnabled_d */  
#endif /* gASL_EnableEZCommissioning_Initiator_d */ 
  
  EZCommissioning_Init();
}
#endif
