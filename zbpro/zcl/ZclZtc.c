/******************************************************************************
* ZclZtc.c
*
* This module contains BeeStack specific code for interacting with ZCL/HA
* applications through a serial interface, called ZigBee Test Client (ZTC).
* The PC-side application is called Test Tool.
*
* Copyright (c) 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
* [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf
*******************************************************************************/
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZtcInterface.h"

#include "ASL_ZclInterface.h"
#include "HaProfile.h"
#include "SEProfile.h"
#include "zclSE.h"
#include "zclSEDRLC.h"
#include "zclSEKeyEstab.h"
#include "zclSEMDUPairing.h"
#include "zclSEDevMgmt.h"
#include "zclSEPrice.h"
#include "ZclProtocInterf.h"
#include "ZclSEPrepayment.h"
#include "ZclSEMessaging.h"
#include "ZclSEMetering.h"
#include "ZclSECalendar.h"
#include "ZclSETunneling.h"
#include "ZclOTA.h"
#include "ZclZtc.h"

/******************************************************************************
*******************************************************************************
* Private macros & prototypes
*******************************************************************************
******************************************************************************/
zbStatus_t ZCL_SE_TCSwapOut_GetDeviceData(uint8_t startIdx);

/******************************************************************************
*******************************************************************************
* Public Funcitons
*******************************************************************************
******************************************************************************/

void InitZtcForZcl(void)
{
  /* register with ZTC */
  ZTC_RegisterAppInterfaceToTestClient(ZclReceiveZtcMessage);
}

/*
	ZclReceiveZtcMessage

	Receives a message from ZTC (Test Tool). Used to control the ZCL and HA
	application through the serial port.
*/
void ZclHAReceiveZtcMessage
(
	ZTCMessage_t* pMsg    /* IN: message from ZTC/UART (Test Tool) */
)
{
	uint8_t opCode;
	zbStatus_t status;
	zclAnyReq_t *pAnyReq;

	/* ignore invalid opcodes */
	if(pMsg->opCode != gHaZtcOpCodeGroup_c)
		return;

	/* opcodes */
	opCode = pMsg->opCodeId;
	status = gZclUnsupportedGeneralCommand_c;
	pAnyReq = (void *)pMsg->data;
	switch(opCode)
	{
		/* read one or more attributes */
		case gHaZtcOpCode_ReadAttr_c:
			status = ZCL_ReadAttrReq(&(pAnyReq->readAttrReq));
		  break;

		/* write one or more attributes */
		case gHaZtcOpCode_WriteAttr_c:
			status = ZCL_WriteAttrReq(&(pAnyReq->writeAttrReq));
		  break;
#if gASL_ZclZtcConfigureReporting_d
		/* Configure reporting */
		case gHaZtcOpCode_CfgReporting_c:
			status = ZCL_ConfigureReportingReq(&(pAnyReq->cfgReporting));
		  break;
                case gHaZtcOpCode_ReadReportingCfg_c:
			status = ZCL_ReadReportingCfgReq(&(pAnyReq->readReportingCfg));
		  break;
#endif
                  
#if gZclEnableDirection_d
		/* read one or more attributes */
		case gHaZtcOpCode_ReadAttrDirected_c:
			status = ZCL_ReadAttrDirectedReq(&(pAnyReq->readAttrDirectedReq));
		  break;

		/* write one or more attributes */
		case gHaZtcOpCode_WriteAttrDirected_c:
			status = ZCL_WriteAttrDirectedReq(&(pAnyReq->writeAttrDirectedReq));
		  break;
#if gZclDiscoverAttrReq_d            
                /* Send the discover attributes command */
                case gHaZtcOpCode_DiscoverAttrDirected_c:
                  status = ZCL_DiscoverAttrDirectedReq(&(pAnyReq->discoverAttrDirectedReq));
                  break;		  	    
#endif                       
#endif                    
                  
#ifdef gHcGenericApp_d          
#if gHcGenericApp_d
    case gGt_AdvertiseProtocolAddr_c:
      status = GenericTunnel_AdvertiseProtocolAddress(&(pAnyReq->Gt_AdvertiseProtoAddr));
      break;
    case gGt_MatchProtocolAddr_c:
      status = GenericTunnel_MatchProtocolAddress(&(pAnyReq->Gt_MatchProtoAddr));
      break;
    case gGt_MatchProtocolAddrRes_c:
      status = GenericTunnel_MatchProtocolAddressResponse(&(pAnyReq->Gt_MatchProtoAddrRes));
      break;
    case g11073_TransferApdu_c:
      status = IEEE11073ProtocolTunnel_TransferApdu(&(pAnyReq->Ieee11073_TrsApdu));
      break;      
    case g11073_ConnectRequest_c:
      status = IEEE11073ProtocolTunnel_ConnectRequest(&(pAnyReq->Ieee11073_ConnectRequest));
      break;      
    case g11073_DisconnectRequest_c:
      status = IEEE11073ProtocolTunnel_DisconnectRequest(&(pAnyReq->Ieee11073_DisconnectRequest));
      break;
    case g11073_ConnectStatusNotif_c: 
      {
        zclCmd11073_ConnectStatusNotif_t* statusNotif = &(pAnyReq->Ieee11073_ConnectStatusNotif);
        if (statusNotif->connectStatus == zcl11073Status_Disconnected)
          status = IEEE11073ProtocolTunnel_InternalDisconnectReq(&(pAnyReq->Ieee11073_ConnectStatusNotif));
        else
          status = IEEE11073ProtocolTunnel_ConnectStatusNotif(&(pAnyReq->Ieee11073_ConnectStatusNotif));
      }
      break;    
    case g11073_GetIEEE11073MessageProcStatus_c:
      status = IEEE11073ProtocolTunnel_GetIEEE11073MessageProcStatus();
      break;        
#if gZclEnablePartition_d        
	  case g11073_SetPartitionThreshold_c:
	    status =  IEEE11073ProtocolTunnel_SetPartitionThreshold(pAnyReq->Ieee11073_SetPartitionThreshold);
	    break;
#endif	    
#endif
#endif

#ifdef gZclEnablePartition_d    
#if (gZclEnablePartition_d == TRUE)
		case gPartitionZtcOpCode_ReadHandshakeParamReq_c:
			status = ZCL_PartitionReadHandshakeParamReq(&(pAnyReq->partitionReadHandshakeParamReq));
		  break;    
		case gPartitionZtcOpCode_WriteHandshakeParamReq_c:
			status = ZCL_PartitionWriteHandshakeParamReq(&(pAnyReq->partitionWriteHandshakeParamReq));
		  break;    		  
        case gPartitionZtcOpCode_RegisterOutgoingFrame_c:
			status = ZCL_PartitionRegisterTxFrame(&(pAnyReq->partitionClusterFrameInfo));
		  break;   
		case gPartitionZtcOpCode_RegisterIncomingFrame_c:
			status = ZCL_PartitionRegisterRxFrame(&(pAnyReq->partitionClusterFrameInfo));
		  break;   		  
	  case gPartitionZtcOpCode_SetDefaultAttrs_c:
	    status =  ZCL_PartitionSetDefaultAttrs(&(pAnyReq->partitionSetDefaultAttrsReq));
	    break;		  
#endif
#endif

#if gASL_ZclGroupAddGroupReq_d                  
    /* add group request (same for add group and add group if identifying) */
    /* use command 0 in request for add, 5 for add if identifying */
    case gHaZtcOpCode_GroupCmd_AddGroup_c:
      status = ASL_ZclGroupAddGroupReq(&(pAnyReq->addGroupReq));
      break;

    /* Sends a AddGroupIfIdentifying command*/
    case gHaZtcOpCode_GroupCmd_AddGroupIfIdentifying_c:
			status = ASL_ZclGroupAddGroupIfIdentifyReq(&(pAnyReq->addGroupIfIdentifyReq));
      break;
#endif       

#if gASL_ZclGroupViewGroupReq_d      
    /* Sends a ViewGroup command*/
    case gHaZtcOpCode_GroupCmd_ViewGroup_c:
      status = ASL_ZclGroupViewGroupReq(&(pAnyReq->viewGroupReq));
      break;
#endif      


#if gASL_ZclGetGroupMembershipReq_d      
    /* Sends a GetGroupMembership command*/
    case gHaZtcOpCode_GroupCmd_GetGroupMembership_c:
      status = ASL_ZclGetGroupMembershipReq(&(pAnyReq->getGroupMembershipReq));
      break;
#endif      

#if gASL_ZclRemoveGroupReq_d      
    /* Sends a RemoveGroup command*/
    case gHaZtcOpCode_GroupCmd_RemoveGroup_c:
      status = ASL_ZclRemoveGroupReq(&(pAnyReq->removeGroupReq));
      break;
#endif      

#if gASL_ZclRemoveAllGroupsReq_d      
    /* remove all groups */
    case gHaZtcOpCode_GroupCmd_RemoveAllGroups_c:
      status = ASL_ZclRemoveAllGroupsReq(&(pAnyReq->removeAllGroupsReq));
      break;
#endif      

#if gASL_ZclSceneAddSceneReq_d      
    /* Sends a AddScene command */
    case gHaZtcOpCode_SceneCmd_AddScene_c:
      status = ASL_ZclSceneAddSceneReq(&(pAnyReq->addSceneReq), pMsg->length - sizeof(pAnyReq->addSceneReq.addrInfo));
      break;
#if gASL_EnableZLLClustersData_d       
    case gHaZtcOpCode_SceneCmd_EnhancedAddScene_c:    
      status = ASL_ZclSceneEnhancedAddSceneReq(&(pAnyReq->addSceneReq), pMsg->length - sizeof(pAnyReq->addSceneReq.addrInfo));
      break;
#endif      
#endif                        

#if gASL_ZclViewSceneReq_d                        
    /* Sends a ViewScene command */
    case gHaZtcOpCode_SceneCmd_ViewScene_c:
      status = ASL_ZclViewSceneReq(&(pAnyReq->viewSceneReq));
      break;
#if gASL_EnableZLLClustersData_d       
    case gHaZtcOpCode_SceneCmd_EnhancedViewScene_c:    
      status = ASL_ZclEnhancedViewSceneReq(&(pAnyReq->viewSceneReq));
      break;
#endif                         
#endif
                       

#if gASL_ZclRemoveSceneReq_d                        
    /* Sends a RemoveScene command */
    case gHaZtcOpCode_SceneCmd_RemoveScene_c:
			status = ASL_ZclRemoveSceneReq(&(pAnyReq->removeSceneReq));
			break;
#endif                        

                        
#if gASL_ZclRemoveAllScenesReq_d                        
    /* Sends a AddScene command */
    case gHaZtcOpCode_SceneCmd_RemoveAllScenes_c:
			status = ASL_ZclRemoveAllScenesReq(&(pAnyReq->removeAllScenesReq));
			break;
#endif
                       
#if gASL_ZclStoreSceneReq_d                        
    /* Sends a StoreScene command */
    case gHaZtcOpCode_SceneCmd_StoreScene_c:
			status = ASL_ZclStoreSceneReq(&(pAnyReq->storeSceneReq));
			break;
#endif                        

#if gASL_ZclRecallSceneReq_d                        
    /* Sends a RecallScene command */
		case gHaZtcOpCode_SceneCmd_RecallScene_c:
			status = ASL_ZclRecallSceneReq(&(pAnyReq->recallSceneReq));
			break;
#endif
                        
#if gASL_ZclGetSceneMembershipReq_d                        
    /* Sends a GetScene Membership command */
    case gHaZtcOpCode_SceneCmd_GetSceneMembership_c:
			status = ASL_ZclGetSceneMembershipReq(&(pAnyReq->getSceneMembershipreq));
			break;
#endif                        

#if gASL_EnableZLLClustersData_d && gASL_ZclCopySceneReq_d  
    /* Sends a Copy Scene command */
    case gHaZtcOpCode_SceneCmd_CopyScene_c:
			status = ASL_ZclSceneCopySceneReq(&(pAnyReq->copyScene));
			break;                        
                        
#endif                        
                        
#if gASL_ZclOnOffReq_d                        
    /* Send the on/off command */
    case gHaZtcOpCode_OnOffCmd_Off_c:
      status = ASL_ZclOnOffReq(&(pAnyReq->onOffReq));
      break;
#if gASL_EnableZLLClustersData_d      
    case gHaZtcOpCode_OnOffCmd_OffWithEffect_c:
      status = OnOff_OffWithEffectReq(&(pAnyReq->offWithEffect));
      break;
    case gHaZtcOpCode_OnOffCmd_OnWithRecallGlobalScene_c:
      status = OnOff_OnWithRecallGlobalSceneReq(&(pAnyReq->onWithRecallGlobalScene));
      break;
    case gHaZtcOpCode_OnOffCmd_OnWithTimedOff_c:
      status = OnOff_OnWithTimedOffReq(&(pAnyReq->onWithTimedOff));
      break;
#endif      
#endif
#if gASL_ZclColorCtrlReq_d
    /* color control cluster commands */  
    case gZclZtcOpCode_ColorControl_MoveToHue_c:
      status = zclColorCtrl_MoveToHueReq(&(pAnyReq->moveToHue));
      break;
    case gZclZtcOpCode_ColorControl_MoveHue_c:
      status = zclColorCtrl_MoveHueReq(&(pAnyReq->moveHue));
      break;      
    case gZclZtcOpCode_ColorControl_StepHue_c:
      status = zclColorCtrl_StepHueReq(&(pAnyReq->stepHue));
      break;   
    case gZclZtcOpCode_ColorControl_MoveToSaturation_c:
      status = zclColorCtrl_MoveToSaturationReq(&(pAnyReq->moveToSaturation));
      break;         
    case gZclZtcOpCode_ColorControl_MoveSaturation_c:
      status = zclColorCtrl_MoveSaturationReq(&(pAnyReq->moveSaturation));
      break;  
    case gZclZtcOpCode_ColorControl_StepSaturation_c:
      status = zclColorCtrl_StepSaturationReq(&(pAnyReq->stepSaturation));
      break;  
    case gZclZtcOpCode_ColorControl_MoveToHueSaturation_c:
      status = zclColorCtrl_MoveToHueSaturationReq(&(pAnyReq->moveToHueSaturation));
      break;    
    case gZclZtcOpCode_ColorControl_MoveToColor_c:
      status = zclColorCtrl_MoveToColorReq(&(pAnyReq->moveToColor));
      break;    
    case gZclZtcOpCode_ColorControl_MoveColor_c:
      status = zclColorCtrl_MoveColorReq(&(pAnyReq->moveColor));
      break;      
    case gZclZtcOpCode_ColorControl_StepColor_c:
      status = zclColorCtrl_StepColorReq(&(pAnyReq->stepColor));
      break;    
    case gZclZtcOpCode_ColorControl_MoveToColorTemperature_c:
      status = zclColorCtrl_MoveToColorTemperatureReq(&(pAnyReq->moveToColorTemperature));
      break;       
#if gColorCtrlEnableZllFunctionality_c    
    case gZclZtcOpCode_ColorControl_EnhancedMoveToHue_c:
      status = zclColorCtrl_EnhancedMoveToHueReq(&(pAnyReq->enhancedMoveToHue));
      break;     
    case gZclZtcOpCode_ColorControl_EnhancedMoveHue_c:
      status = zclColorCtrl_EnhancedMoveHueReq(&(pAnyReq->enhancedMoveHue));
      break;   
    case gZclZtcOpCode_ColorControl_EnhancedStepHue_c:
      status = zclColorCtrl_EnhancedStepHueReq(&(pAnyReq->enhancedStepHue));
      break;     
    case gZclZtcOpCode_ColorControl_EnhancedMoveToHueSaturation_c:
      status = zclColorCtrl_EnhancedMoveToHueSaturationReq(&(pAnyReq->enhancedMoveToHueSaturation));
      break;    
    case gZclZtcOpCode_ColorControl_ColorLoopSet_c:
      status = zclColorCtrl_ColorLoopSetReq(&(pAnyReq->colorLoopSet));
      break;    
    case gZclZtcOpCode_ColorControl_StopMoveStep_c:
      status = zclColorCtrl_StopMoveStepReq(&(pAnyReq->stopMoveStep));
      break;           
    case gZclZtcOpCode_ColorControl_MoveColorTemperature_c:
      status = zclColorCtrl_MoveColorTemperatureReq(&(pAnyReq->moveColorTemperature));
      break;  
    case gZclZtcOpCode_ColorControl_StepColorTemperature_c:
      status = zclColorCtrl_StepColorTemperatureReq(&(pAnyReq->stepColorTemperature));
      break;        
#endif /* gColorCtrlEnableZllFunctionality_c */     
#endif /* gASL_ZclColorCtrlReq_d */
      
#if gASL_ZclSetWeeklyScheduleReq                        
    /* Send the SetWeeklySchedule command */
    case gHaZtcOpCode_Thermostat_SetWeeklyScheduleReq:
      status = ZCL_ThermostatSetWeeklyScheduleReq(&(pAnyReq->SetWeeklyScheduleReq));
      break;
#endif
      
#if gASL_ZclGetWeeklyScheduleReq                        
    /* Send the GetWeeklySchedule command */
    case gHaZtcOpCode_Thermostat_GetWeeklyScheduleReq:
      status = ZCL_ThermostatGetWeeklyScheduleReq(&(pAnyReq->GetWeeklyScheduleReq));
      break;
#endif
      
#if gASL_ZclClearWeeklyScheduleReq                        
    /* Send the ClearWeeklySchedule command */
    case gHaZtcOpCode_Thermostat_ClearWeeklyScheduleReq:
      status = ZCL_ThermostatClearWeeklyScheduleReq(&(pAnyReq->ClearWeeklyScheduleReq));
      break;
#endif
    
#if gASL_ZclSetpointRaiseLowerReq                        
    /* Send the ClearWeeklySchedule command */
    case gHaZtcOpCode_Thermostat_SetpointRaiseLowerReq:
      status = ZCL_ThermostatSetpointRaiseLowerReq(&(pAnyReq->SetpointRaiseLowerReq));
      break;
#endif
  
#if gASL_ZclDoorLockReq_d                        
    /* Send the lock/unlock command */
    case gHaZtcOpCode_DoorLockCmds_c:
      status = ASL_ZclDoorLockReq(&(pAnyReq->doorLockReq));
      break;
    case gHaZtcOpCode_DoorLockClearCmds_c:
      status = ASL_ZclDoorLock_ClearCmdsReq(&(pAnyReq->doorLockClearReq));
      break;
#endif      
      
#if gASL_ZclIdentifyReq_d      
    /* Send the identify command */
	  case gHaZtcOpCode_IdentifyCmd_Identify_c: 
	    status = ASL_ZclIdentifyReq(&(pAnyReq->identifyReq));
	    break;	    
#endif
	    
#if gASL_ZclCmdEzModeInvokeReq_d      
	/* Send the EzModeInvoke command */
	  case gHaZtcOpCode_IdentifyCmd_EzModeInvoke_c: 
	    status = ASL_ZclEzModeInvokeReq(&(pAnyReq->ezModeInvokeReq));
	    break;	  
#endif
	    
#if gASL_ZclCmdUpdateCommissioningStateReq_d      
	/* Send the UpdateCommissioningState command */
	  case gHaZtcOpCode_IdentifyCmd_UpdateCommissioningState_c: 
	    status = ASL_ZclUpdateCommissioningStateReq(&(pAnyReq->updateCommissioningStateReq));
	    break;	  
#endif
#if gASL_EnableZLLClustersData_d         
        case gHaZtcOpCode_IdentifyCmd_TriggerEffect_c:
          status = zclIdentify_TriggerEffectReq(&(pAnyReq->identifyTriggerEffect));
          break;
#endif            
#if gASL_ZclAlarms_ResetAlarm_d
    case gZclZtcOpCode_Alarms_ResetAlarm_c:
      status = Alarms_ResetAlarm(&(pAnyReq->resetAlarmReq));
      break;        
#endif
	    
#if gASL_ZclAlarms_ResetAllAlarms_d      
    case gZclZtcOpCode_Alarms_ResetAllAlarms_c:
      status = Alarms_ResetAllAlarms(&(pAnyReq->resetAllAlarmsReq));
      break;        
#endif
	    
#if gASL_ZclAlarms_Alarm_d      
    case gZclZtcOpCode_Alarms_Alarm_c:
      status = Alarms_Alarm(&(pAnyReq->alarmReq));
      break;        
#endif
      
#if gASL_ZclAlarms_GetAlarm_d      
    case gZclZtcOpCode_Alarms_GetAlarm_c:
      status = Alarms_GetAlarm(&(pAnyReq->getAlarmReq));
      break;    
    case gZclZtcOpCode_Alarms_ResetAlarmLog_c:
      status = Alarms_ResetAlarmLog(&(pAnyReq->resetAlarmLogReq));   
      break;
#endif      
	    
#if gASL_ZclIdentifyQueryReq_d            
	  /* Send the identify query command */
	  case gHaZtcOpCode_IdentifyCmd_IdentifyQuery_c:
	    status = ASL_ZclIdentifyQueryReq(&(pAnyReq->identifyQueryReq));
	    break;		  
#endif            

#if gZclDiscoverAttrReq_d            
	  /* Send the discover attributes command */
	  case gHaZtcOpCode_DiscoverAttr_c:
	    status = ZCL_DiscoverAttrReq(&(pAnyReq->discoverAttrReq));
	    break;		  	    
#endif            
     
 #if gZclDiscoverAttrExtendedReq_d            
	  /* Send the discover attributes extended command */
	  case gHaZtcOpCode_DiscoverExtendedAttr_c:
	    status = ZCL_DiscoverAttrExtendedReq(&(pAnyReq->discoverAttrExtendedReq));
	    break;		  	    
#endif    
   
 #if gZclDiscoverCommandsReg_d            
	  /* Send the discover attributes extended command */
	  case gHaZtcOpCode_DiscoverCmds_c:
	    status = ZCL_DiscoverCommandsReq(&(pAnyReq->discoverCommandsReq));
	    break;		  	    
#endif             
/*******************************************************************/	  
#if gASL_ZclCommissioningRestartDeviceRequest_d
	  case gHaZtcOpCode_CommissioningRestartDeviceRequest_c:
	    status = ZCL_Commisioning_RestartDeviceReq( (zclCommissioningRestartDeviceReq_t*) &(pAnyReq->RestartDeviceReq));
	    break;		  	    
#endif	  
#if gASL_ZclCommissioningSaveStartupParametersRequest_d            
	  case gHaZtcOpCode_CommissioningSaveStartupParametersRequest_c:
	    status = ZCL_Commisioning_SaveStartupParametersReq((zclCommissioningSaveStartupParametersReq_t *) &(pAnyReq->SaveStartupParameterReq));
	    break;		  	    
#endif
#if gASL_ZclCommissioningRestoreStartupParametersRequest_d
	  case gHaZtcOpCode_CommissioningRestoreStartupParametersRequest_c:
	    status = ZCL_Commisioning_RestoreStartupParametersReq((zclCommissioningRestoreStartupParametersReq_t*) &(pAnyReq->RestoreStartupParameterReq));
	    break;		  	    
#endif
#if gASL_ZclCommissioningResetStartupParametersRequest_d            
	  case gHaZtcOpCode_CommissioningResetStartupParametersRequest_c:
	    status = ZCL_Commisioning_ResetStartupParametersReq((zclCommissioningResetStartupParametersReq_t *) &(pAnyReq->ResetStartupParameterReq));
	    break;		 
#endif	    
/*******************************************************************/	

#if gASL_ZclIASZoneReq_d
    /*send IAS Zone Commands */
         case gHaZtcOpCode_IASZoneCmd_EnrollReq_c:             
            status = IASZone_ZoneEnrollRequest((zclIASZone_ZoneEnrollRequest_t  *) &(pAnyReq->zoneEnrollReq));
	    break;
         case gHaZtcOpCode_IASZoneCmd_ChangeNotif_c:             
            status = IASZone_ZoneStatusChange((zclIASZone_ZoneStatusChange_t *) &(pAnyReq->statusChangeNotifReq));
	    break;  
         case gHaZtcOpCode_IASZoneCmd_InitNormalOpMode_c:             
            status = IASZone_InitNormalOpModeRequest((zclIASZone_InitNormalOpMode_t *) &(pAnyReq->initNormalOpMode));
	    break;  
         case gHaZtcOpCode_IASZoneCmd_InitTestMode_c:             
            status = IASZone_InitTestModeRequest((zclIASZone_InitTestMode_t *) &(pAnyReq->initTestMode));
	    break;   
#if gASL_ZclIASZoneClientReq_d            
        case gHaZtcOpCode_IASZone_AddEntryInClientTable_c:
            status = IASZone_AddEntryInDeviceClientTable((gZclZoneTable_t *)&(pAnyReq->iasZoneEntry));
#endif            
#endif

#if gZclClusterOptionals_d         
         case gHaZtcOpCode_SetValueAttr_c:    
            status = ZCL_SetValueAttr((zclSetAttrValue_t *)pAnyReq);
          break;
         case gHaZtcOpCode_ProcessUnsolicitedCommand_c:
            status = ZCL_ProcessUnsolicitedCommand((zclProcessUnsolicitedCommand_t *)pAnyReq);
          break;
#endif
      
#if gASL_ZclIASACEReq_d
    /*send IAS ACE Commands */
         case gHaZtcOpCode_IASACECmd_Arm_c:             
            status = IASACE_Arm((zclIASACE_Arm_t  *) &(pAnyReq->arm));
	    break;
         case gHaZtcOpCode_IASACECmd_Bypass_c:             
            status = IASACE_Bypass((zclIASACE_Bypass_t *) &(pAnyReq->bypass));
	    break;  
         case gHaZtcOpCode_IASACECmd_Emergency_c:             
            status = IASACE_Emergency((zclIASACE_EFP_t  *) &(pAnyReq->emergency));
	    break;
         case gHaZtcOpCode_IASACECmd_Fire_c:             
            status = IASACE_Fire((zclIASACE_EFP_t *) &(pAnyReq->fire));
	    break; 
         case gHaZtcOpCode_IASACECmd_Panic_c:             
            status = IASACE_Panic((zclIASACE_EFP_t  *) &(pAnyReq->panic));
	    break;
         case gHaZtcOpCode_IASACECmd_GetZoneIDMap_c:             
        	status = IASACE_GetZoneIDMap((zclIASACE_EFP_t  *) &(pAnyReq->zoneIdMap));
	    break; 
         case gHaZtcOpCode_IASACECmd_GetZoneInf_c:             
            status = IASACE_GetZoneInformation((zclIASACE_GetZoneInformation_t  *) &(pAnyReq->zoneInf));
	    break; 
         case gHaZtcOpCode_IASACECmd_GetBypassedZoneList_c:   
            status = IASACE_GetBypassedZoneList((zclIASACE_GetBypassedZoneList_t  *) &(pAnyReq->getBypassedZoneList));
	    break;
         case gHaZtcOpCode_IASACECmd_GetPanelStatus_c:   
            status = IASACE_GetPanelStatus((zclIASACE_GetPanelStatus_t  *) &(pAnyReq->getPanelStatus));
	    break;       
         case gHaZtcOpCode_IASACECmd_PanelStatusChanged_c:   
            status = IASACE_PanelStatusChanged((zclIASACE_PanelStatusChanged_t  *) &(pAnyReq->panelStatusChanged));
	    break;              
            
            
#endif            
   
#if gASL_ZclIASWDReq_d
    /*send IAS WD Commands */
         case gHaZtcOpCode_IASWDCmd_StartWarning_c:             
            status = IASWD_StartWarning((zclIASWD_StartWarning_t  *) &(pAnyReq->startWarning));
	    break;
         case gHaZtcOpCode_IASWDCmd_Squawk_c:             
            status = IASWD_Squawk((zclIASWD_Squawk_t *) &(pAnyReq->squawk));
	    break;  
#endif            
            
#if gZclEnablePwrProfileClusterClient_d
        case gHaZtcOpCode_PwrProfile_PwrProfileReq_c:             
            status = zclPwrProfile_PwrProfileReq((zclPwrProfile_PwrProfileReq_t  *) &(pAnyReq->pwrProfileReq));
	    break;
        case gHaZtcOpCode_PwrProfile_PwrProfileStateReq_c:             
            status = zclPwrProfile_PwrProfileStateReq((zclPwrProfile_PwrProfileStateReq_t  *) &(pAnyReq->pwrProfileStateReq));
	    break;
        case gHaZtcOpCode_PwrProfile_PwrProfileScheduleConstraintsReq_c:             
            status = zclPwrProfile_PwrProfileScheduleConstraintsReq((zclPwrProfile_PwrProfileScheduleConstraintsReq_t  *) &(pAnyReq->pwrProfileScheduleConstraintsReq));
	    break;	 
        case gHaZtcOpCode_PwrProfile_EnergyPhsScheduleStateReq_c:             
            status = zclPwrProfile_EnergyPhsScheduleStateReq((zclPwrProfile_EnergyPhsScheduleStateReq_t  *) &(pAnyReq->energyPhsScheduleStateReq));
	    break;		    
	    
#endif
	    
#if (gZclClusterOptionals_d && gZclEnablePwrProfileClusterServer_d)
        case gHaZtcOpCode_PwrProfile_GetPwrProfilePrice_c:             
            status = zclPwrProfile_GetPwrProfilePrice((zclPwrProfile_GetPwrProfilePrice_t  *) &(pAnyReq->getPwrProfilePrice));
	    break;    
        case gHaZtcOpCode_PwrProfile_GetOverallSchedulePrice_c:             
            status = zclPwrProfile_GetOverallSchedulePrice((zclPwrProfile_GetOverallSchedulePrice_t  *) &(pAnyReq->getOverallSchedulePrice));
	    break; 
        case gHaZtcOpCode_PwrProfile_SetCurrentPwrProfileInf_c:             
            status = ZtcPwrProfileServer_SetCurrentPwrProfileInformation((zclPwrProfileInf_t  *) &(pAnyReq->pwrProfileInf));
	    break;
#endif   
#if gZclEnablePwrProfileClusterServer_d	    
#if (gZclClusterOptionals_d)	    
        case gHaZtcOpCode_PwrProfile_GetPwrProfilePriceExtended_c:             
            status = zclPwrProfile_GetPwrProfilePriceExtended((zclPwrProfile_GetPwrProfilePriceExtended_t  *) &(pAnyReq->getPwrProfilePriceExtended));
	    break; 
#endif	    
        case gHaZtcOpCode_PwrProfile_EnergyPhsScheduleReq_c:             
            status = zclPwrProfile_EnergyPhsScheduleReq((zclPwrProfile_EnergyPhsScheduleReq_t  *) &(pAnyReq->energyPhsScheduleReq));
	    break; 	    
	    
#endif	    

#if gZclZtcEnableApplianceCtrlClientSupport_d	    
        case gHaZtcOpCode_ApplianceCtrl_ExecutionCommand_c:             
            status = zclApplianceCtrl_ExecutionCommand((zclApplCtrl_ExecutionCommand_t  *) &(pAnyReq->executionCmd));
	    break; 
        case gHaZtcOpCode_ApplianceCtrl_SignalState_c:             
            status = zclApplianceCtrl_SignalState((zclApplCtrl_CommandWithNoPayload_t  *) &(pAnyReq->signalState));
	    break; 	
        case gHaZtcOpCode_ApplianceCtrl_WriteFunctions_c:             
            status = zclApplianceCtrl_WriteFunctions((zclApplCtrl_WriteFunction_t  *) &(pAnyReq->writeFunction));
	    break; 
        case gHaZtcOpCode_ApplianceCtrl_OverloadCommand_c:             
            status = zclApplianceCtrl_OverloadCommand((zclApplCtrl_OverloadCommand_t  *) &(pAnyReq->overloadCmd));
	    break; 		    
#endif

#if gZclEnableApplianceEventsAlerts_d
        case gHaZtcOpCode_ApplianceEventsAndAlerts_GetAlerts:             
            status = zclApplianceEventsAlerts_GetAlerts((zclApplianceEventsAlerts_GetAlerts_t  *) &(pAnyReq->getAlerts));
	    break; 	    
#endif	    

#if gZclEnableApplianceStatistics_d
        case gHaZtcOpCode_ApplianceStatistics_LogReq_c:             
            status = zclApplianceStatistics_LogRequest((zclApplianceStatistics_LogReq_t  *) &(pAnyReq->loqRequest));
	    break; 	
        case gHaZtcOpCode_ApplianceStatistics_LogQueueReq_c:             
            status = zclApplianceStatistics_LogQueueReq((zclApplianceStatistics_LogQueueReq_t  *) &(pAnyReq->logQueueReq));
	    break; 	 
#endif		    
	    
	    
#if gASL_ZclDisplayMsgReq_d
        case gAmiZtcOpCode_Msg_DisplayMsgReq_c:
	    status = ZclMsg_DisplayMsgReq((zclDisplayMsgReq_t *) &(pAnyReq->DisplayMsgReq));
	    break;	
#endif
#if gASL_ZclCancelMsgReq_d            
	  case gAmiZtcOpCode_Msg_CancelMsgReq_c:
	    status = ZclMsg_CancelMsgReq((zclCancelMsgReq_t *) &(pAnyReq->CancelMsgReq));
	    break;
#endif 
#if gASL_ZclGetLastMsgReq_d            
	  case gAmiZtcOpCode_Msg_GetLastMsgRequest_c:
	    status = ZclMsg_GetLastMsgReq((zclGetLastMsgReq_t *) &(pAnyReq->GetLastMsgReq));
	    break;
#endif
#if gASL_ZclMsgConfReq_d            
	  case gAmiZtcOpCode_Msg_MsgConfReq:
	    status = ZclMsg_MsgConf((zclMsgConfReq_t *) &(pAnyReq->MsgConfReq));
	    break;
#endif
            
#if gInterPanCommunicationEnabled_c    
#if gASL_ZclInterPanDisplayMsgReq_d
        case gAmiZtcOpCode_Msg_InterPanDisplayMsgReq_c:
	    status = ZclMsg_InterPanDisplayMsgReq((zclInterPanDisplayMsgReq_t *) &(pAnyReq->InterPanDisplayMsgReq));
	    break;	
#endif
#if gASL_ZclInterPanCancelMsgReq_d            
	  case gAmiZtcOpCode_Msg_InterPanCancelMsgReq_c:
	    status = ZclMsg_InterPanCancelMsgReq((zclInterPanCancelMsgReq_t *) &(pAnyReq->InterPanCancelMsgReq));
	    break;
#endif 
#if gASL_ZclInterPanGetLastMsgReq_d            
	  case gAmiZtcOpCode_Msg_InterPanGetLastMsgRequest_c:
	    status = ZclMsg_InterPanGetLastMsgReq((zclInterPanGetLastMsgReq_t *) &(pAnyReq->InterPanGetLastMsgReq));
	    break;
#endif
#if gASL_ZclMsgConfReq_d            
	  case gAmiZtcOpCode_Msg_InterPanMsgConfReq:
	    status = ZclMsg_InterPanMsgConf((zclInterPanMsgConfReq_t *) &(pAnyReq->InterPanMsgConfReq));
	    break;
#endif
#endif /*#if gInterPanCommunicationEnabled_c    */            
            
#if gASL_ZclSmplMet_GetProfReq_d            
	  case gAmiZtcOpCode_SmplMet_GetProfReq_c:
	    status = ZclSmplMet_GetProfReq((zclSmplMet_GetProfReq_t *) &(pAnyReq->SmplMet_GetProfReq));
	    break;
#endif            
#if gASL_ZclSmplMet_GetProfRsp_d
	  case gAmiZtcOpCode_SmplMet_GetProfRsp_c:
	    status = ZclSmplMet_GetProfRsp((zclSmplMet_GetProfRsp_t *) &(pAnyReq->SmplMet_GetProfRsp));
	    break;	              
#endif
#if gZclFastPollMode_d
#if gASL_ZclSmplMet_FastPollModeReq_d
	  case gAmiZtcOpCode_SmplMet_FastPollModeReq_c:
	    status = ZclSmplMet_ReqFastPollModeReq((zclSmplMet_ReqFastPollModeReq_t *) &(pAnyReq->SmplMet_GetProfRsp));
	    break;	              
#endif
#if gASL_ZclSmplMet_FastPollModeRsp_d
	  case gAmiZtcOpCode_SmplMet_FastPollModeRsp_c:
	    status = ZclSmplMet_ReqFastPollModeRsp((zclSmplMet_ReqFastPollModeRsp_t *) &(pAnyReq->SmplMet_GetProfRsp));
	    break;	              
#endif
#if gASL_ZclSmplMet_AcceptFastPollModeReq_d
          case gAmiZtcOpCode_SmplMet_AcceptFastPollModeReq_c:
            status = ZclSmplMet_AcceptFastPollModeReq(pAnyReq->SmplMet_AcceptFastPollModeReq);
          break;
#endif
#endif          
#if gASL_ZclMet_RequestMirrorReq_d
          case gAmiZtcOpCode_Met_RequestMirrorReq_c:
            status = ZclMet_RequestMirrorReq((zclMet_RequestMirrorReq_t *) &(pAnyReq->Met_RequestMirrorReq));
          break;
#endif
#if gASL_ZclMet_RemoveMirrorReq_d
          case gAmiZtcOpCode_Met_RemoveMirrorReq_c:
            status = ZclMet_RemoveMirrorReq((zclMet_RemoveMirrorReq_t *) &(pAnyReq->Met_RemoveMirrorReq));
          break;
#endif
#if gASL_ZclSmplMet_GetSnapshotReq_d
          case gAmiZtcOpCode_SmplMet_GetSnapshotReq_c:
            status = ZclSmplMet_GetSnapshotReq(&(pAnyReq->SmplMet_GetSnapShotReq));
#endif
#if gASL_ZclDmndRspLdCtrl_ReportEvtStatus_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_ReportEvtStatusReq_c:
	    status = ZclDmndRspLdCtrl_ReportEvtStatus((zclDmndRspLdCtrl_ReportEvtStatus_t *) &(pAnyReq->DmndRspLdCtrl_ReportEvtStatus));
	    break;
#endif
#if gASL_ZclDmndRspLdCtrl_GetScheduledEvtsReq_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_GetScheduledEvtsReq_c:
	    status = ZclDmndRspLdCtrl_GetScheduledEvtsReq((zclDmndRspLdCtrl_GetScheduledEvts_t *) &(pAnyReq->DmndRspLdCtrl_GetScheduledEvts));
	    break;
#endif 
#if gASL_ZclDmndRspLdCtrl_LdCtrlEvtReq_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_LdCtrlEvtReq_c:
	    status = ZclDmndRspLdCtrl_LdCtrlEvtReq((zclDmndRspLdCtrl_LdCtrlEvtReq_t *) &(pAnyReq->DmndRspLdCtrl_LdCtrlEvtReq));
	    break;
#endif
#if gASL_ZclDmndRspLdCtrl_CancelLdCtrlEvtReq_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_CancelLdCtrlEvtReq_c:
	    status = ZclDmndRspLdCtrl_CancelLdCtrlEvtReq((zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t *) &(pAnyReq->DmndRspLdCtrl_CancelLdCtrlEvtReq));
	    break;
#endif
#if gASL_ZclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_CancelAllLdCtrlEvtReq_c:
	    status = ZclDmndRspLdCtrl_CancelAllLdCtrlEvtReq((zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *) &(pAnyReq->DmndRspLdCtrl_CancelAllLdCtrlEvtsReq));
	    break;
#endif
#if gASL_ZclDmndRspLdCtrl_ScheduledServerEvts_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_ScheduleServerEvt_c:
	    status = ZCL_ScheduleServerLdCtrlEvents((zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)pAnyReq);
	    break;
#endif              
#if gASL_ZclPrice_GetCurrPriceReq_d            
	  case gAmiZtcOpCode_Price_GetCurrPriceReq_c:
	    status = zclPrice_GetCurrPriceReq((zclPrice_GetCurrPriceReq_t *) &(pAnyReq->Price_GetCurrPriceReq));
	    break;
#endif
#if gASL_ZclPrice_GetSheduledPricesReq_d            
	  case gAmiZtcOpCode_Price_GetSheduledPricesReq_c:
	    status = zclPrice_GetScheduledPricesReq((zclPrice_GetScheduledPricesReq_t *) &(pAnyReq->Price_GetSheduledPricesReq));
	    break;
#endif

#if gASL_ZclPrice_PublishPriceRsp_d            
	  case gAmiZtcOpCode_Price_PublishPriceRsp_c:
	    status = zclPrice_PublishPriceRsp((zclPrice_PublishPriceRsp_t *) &(pAnyReq->Price_PublishPriceRsp));
	    break;
#endif     

#if gASL_ZclPrice_PublishBlockPeriodsRsp_d            
	  case gAmiZtcOpCode_Price_PublishBlockPeriodsRsp_c:
	    status = zclPrice_PublishBlockPeriodRsp((zclPrice_PublishBlockPeriodRsp_t *) &(pAnyReq->Price_BlockPeriodsRsp));
	    break;
#endif 
            
#if gASL_ZclPrice_GetBlockPeriodsReq_d            
	  case gAmiZtcOpCode_Price_GetBlockPeriodsReq_c:
	    status = zclPrice_GetBlockPeriodsReq((zclPrice_GetBlockPeriodsReq_t *) &(pAnyReq->Price_GetBlockPeriodsReq));
	    break;
#endif 

#if gASL_ZclPrice_GetCalorificValueReq_d            
	  case gAmiZtcOpCode_Price_GetCalorificValueReq_c:
	    status = zclPrice_GetCalorificValueReq((zclPrice_GetCalorificValueReq_t *) &(pAnyReq->Price_GetCalorificValueReq));
	    break;
#endif 
            
#if gASL_ZclPrice_GetConversionFactorReq_d
        case gAmiZtcOpCode_Price_GetConversionFactorReq_c:
          status = zclPrice_GetConversionFactorReq((zclPrice_GetConversionFactorReq_t *) &(pAnyReq->Price_GetConversionFactor));
          break;
#endif

#if gASL_ZclSE_12_Features_d           
#if gASL_ZclPrice_GetBillingPeriodReq_d            
	  case gAmiZtcOpCode_Price_GetBillingPeriodReq_c:
	    status = zclPrice_GetBillingPeriodReq((zclPrice_GetBillingPeriodReq_t *) &(pAnyReq->Price_GetBillingPeriodReq));
	    break;
#endif 
#endif 
            
#if gASL_ZclSE_12_Features_d           
#if gASL_ZclPrice_GetPriceMatrixReq_d            
	  case gAmiZtcOpCode_Price_GetPriceMatrixReq_c:
	    status = zclPrice_GetPriceMatrixReq((zclPrice_GetPriceMatrixReq_t *) &(pAnyReq->Price_GetPriceMatrixReq));
	    break;
#endif 
#endif 
            
#if gASL_ZclSE_12_Features_d           
#if gASL_ZclPrice_GetBlockThresholdsReq_d            
	  case gAmiZtcOpCode_Price_GetBlockThresholdsReq_c:
	    status = zclPrice_GetBlockThresholdsReq((zclPrice_GetBlockThresholdsReq_t *) &(pAnyReq->Price_GetBlockThresholdsReq));
	    break;
#endif 
#endif 
            
#if gASL_ZclSE_12_Features_d           
#if gASL_ZclPrice_GetTariffInformationReq_d            
	  case gAmiZtcOpCode_Price_GetTariffInformationReq_c:
	    status = zclPrice_GetTariffInformationReq((zclPrice_GetTariffInformationReq_t *) &(pAnyReq->Price_GetTariffInformationReq));
	    break;
#endif 
#endif 
            
            
#if gASL_ZclSE_12_Features_d           
#if gASL_ZclPrice_GetCO2ValueReq_d            
	  case gAmiZtcOpCode_Price_GetCO2ValueReq_c:
	    status = zclPrice_GetCO2ValueReq((zclPrice_GetCO2ValueReq_t *) &(pAnyReq->Price_GetCO2ValueReq));
	    break;
#endif 
#endif 
            
#if gASL_ZclSE_12_Features_d           
#if gASL_ZclPrice_GetConsolidatedBillReq_d            
	  case gAmiZtcOpCode_Price_GetConsolidatedBillReq_c:
	    status = zclPrice_GetConsolidatedBillReq((zclPrice_GetConsolidatedBillReq_t *) &(pAnyReq->Price_GetConsolidatedBillReq));
	    break;
#endif 
#endif 
            
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_12_Features_d            
#if gASL_ZclPrice_PublishConversionFactorRsp_d            
	  case gAmiZtcOpCode_Price_ScheduelServeConversionFactorStore_c:
	    status = ZCL_ScheduleServerConversionFactorStore((ztcCmdPrice_PublishConversionFactorRsp_t *) &(pAnyReq->ztcPublishConversionFactorRsp));
	    break;
#endif
#endif
          
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_12_Features_d 
#if gASL_ZclPrice_PublishCalorificValueRsp_d            
	  case gAmiZtcOpCode_Price_ScheduelServeCalorificValueStore_c:
	    status = ZCL_ScheduleServerCalorificValueStore((ztcCmdPrice_PublishCalorificValueRsp_t *) &(pAnyReq->ztcPublishCalorificValueRsp));
	    break;
#endif  
#endif
       
#if gASL_ZclPrice_Optionals_d            
#if gASL_ZclPrice_PublishBlockPeriodsRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerBlockPeriods_c:
	    status = ZCL_ScheduleServerBlockPeriodsEvents(( ztcCmdPrice_PublishBlockPeriodRsp_t *)pAnyReq);
	    break;
#endif
#endif

#if gASL_ZclSE_12_Features_d            
#if gASL_ZclPrice_PublishTariffInformationRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerTariffInformation_c:
	    status = ZCL_ScheduleServerTariffInformationEvents(( ztcCmdPrice_PublishTariffInformationRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_12_Features_d            
#if gASL_ZclPrice_PublishBillingPeriodRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerBillingPeriod_c:
	    status = ZCL_ScheduleServerBillingPeriodEvents(( ztcCmdPrice_PublishBillingPeriodRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_12_Features_d            
#if gASL_ZclPrice_PublishCO2ValueRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerCO2Value_c:
	    status = ZCL_ScheduleServerCO2ValueEvents(( ztcCmdPrice_PublishCO2ValueRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_12_Features_d            
#if gASL_ZclPrice_PublishPriceMatrixRsp_d            
	  case gAmiZtcOpCode_Price_StoreServerPriceMatrix_c:
	    status = ZCL_StoreServerPriceMatrix(( ztcCmdPrice_PublishPriceMatrixRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_12_Features_d            
#if gASL_ZclPrice_PublishBlockThresholdsRsp_d            
	  case gAmiZtcOpCode_Price_StoreServerBlockThresholds_c:
	    status = ZCL_StoreServerBlockThresholds(( ztcCmdPrice_PublishBlockThresholdsRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_12_Features_d            
#if gASL_ZclPrice_PublishConsolidatedBillRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerConsolidatedBill_c:
	    status = ZCL_ScheduleServerConsolidatedBillEvents(( ztcCmdPrice_PublishConsolidatedBillRsp_t *)pAnyReq);
	    break;
#endif
#endif

#if gASL_ZclSE_12_Features_d            
#if gASL_ZclPrice_PublishCPPEventRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerCPPEvent_c:
	    status = ZCL_ScheduleServerCPPEvents((zclCmdPrice_PublishCPPEventRsp_t *)pAnyReq);
	    break;
#endif
#endif

#if gASL_ZclPrice_Optionals_d
#if gASL_ZclPrice_PublishBlockPeriodsRsp_d            
	  case gAmiZtcOpCode_Price_UpdateServerBlockPeriods_c:
	    status = ZCL_UpdateServerBlockPeriodEvents((zclCmdPrice_PublishBlockPeriodRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclPrice_Optionals_d           
#if gASL_ZclPrice_PublishBlockPeriodsRsp_d            
	  case gAmiZtcOpCode_Price_DeleteServerScheduledBlockPeriods_c:
	      ZCL_DeleteScheduleServerBlockPeriods();
              status = gZclSuccess_c;
	    break;
#endif
#endif
            
#if gASL_ZclPrice_PublishPriceRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerPrice_c:
	    status = ZCL_ScheduleServerPriceEvents((ztcCmdPrice_PublishPriceRsp_t *)pAnyReq, FALSE);
	    break;
#endif   

#if gASL_ZclPrice_PublishPriceRsp_d            
	  case gAmiZtcOpCode_Price_UpdateServerPrice_c:
	    status = ZCL_UpdateServerPriceEvents((zclCmdPrice_PublishPriceRsp_t *)pAnyReq);
	    break;
#endif   

#if gASL_ZclPrice_PublishPriceRsp_d            
	  case gAmiZtcOpCode_Price_DeleteServerScheduledPrices_c:
	       ZCL_DeleteServerScheduledPrices();
               status = gZclSuccess_c;
	    break;
#endif               
#if gInterPanCommunicationEnabled_c    
#if gASL_ZclPrice_InterPanGetCurrPriceReq_d            
          case gAmiZtcOpCode_Price_InterPanGetCurrPriceReq_c:
                status = zclPrice_InterPanGetCurrPriceReq((zclPrice_InterPanGetCurrPriceReq_t *) &(pAnyReq->InterPanGetCurrPriceReq));
            break;
#endif
#if gASL_ZclPrice_InterPanGetSheduledPricesReq_d    
          case gAmiZtcOpCode_Price_InterPanGetSheduledPricesReq_c:
                status = zclPrice_InterPanGetScheduledPricesReq((zclPrice_InterPanGetScheduledPricesReq_t *) &(pAnyReq->InterPanGetScheduledPricesReq));
           break;
#endif 
#if gASL_ZclPrice_InterPanPublishPriceRsp_d    
          case gAmiZtcOpCode_Price_InterPanPublishPriceRsp_c:
                status = zclPrice_InterPanPublishPriceRsp((zclPrice_InterPanPublishPriceRsp_t *) &(pAnyReq->InterPanPublishPriceRsp));
          break;    
#endif    
#endif

#if gASL_ZclPrepayment_d          
#if gASL_ZclPrepayment_SelAvailEmergCreditReq_d && gASL_ZclPrepayment_SupplyOptionals_d
	  case gAmiZtcOpCode_Prepayment_SelAvailEmergCreditReq_c:
	    status = zclPrepayment_Client_SelAvailEmergCreditReq((zclPrepayment_SelAvailEmergCreditReq_t *) &(pAnyReq->Prepayment_SelAvailEmergCreditReq));
	    break;	              
#endif
            
#if !gASL_ZclSE_12_Features_d            
#if gASL_ZclPrepayment_ChangeSupplyReq_d           
	  case gAmiZtcOpCode_Prepayment_ChangeSupplyReq_c:
	    status = zclPrepayment_Client_ChangeSupplyReq((zclPrepayment_ChangeSupplyReq_t *) &(pAnyReq->Prepayment_ChangeSupplyReq));
	    break;           
#endif
#endif
#endif
            
#if gASL_ZclKeyEstab_InitKeyEstabReq_d    
	  case gAmiZtcOpCode_KeyEstab_InitKeyEstabReq_c:
	    status = zclKeyEstab_InitKeyEstabReq((ZclKeyEstab_InitKeyEstabReq_t *) &(pAnyReq->KeyEstab_InitKeyEstabReq));
	    break;	  
#endif
#if gASL_ZclKeyEstab_EphemeralDataReq_d            
	  case gAmiZtcOpCode_KeyEstab_EphemeralDataReq_c:
	    status = zclKeyEstab_EphemeralDataReq((ZclKeyEstab_EphemeralDataReq_t *) &(pAnyReq->KeyEstab_EphemeralDataReq));
	    break;	  	  
#endif
#if gASL_ZclKeyEstab_ConfirmKeyDataReq_d            
	  case gAmiZtcOpCode_KeyEstab_ConfirmKeyDataReq_c:
	    status = zclKeyEstab_ConfirmKeyDataReq((ZclKeyEstab_ConfirmKeyDataReq_t *) &(pAnyReq->KeyEstab_ConfirmKeyDataReq));
	    break;	   
#endif            
#if gASL_ZclKeyEstab_TerminateKeyEstabServer_d            
	  case gAmiZtcOpCode_KeyEstab_TerminateKeyEstabServer_c:
	    status = zclKeyEstab_TerminateKeyEstabServer((ZclKeyEstab_TerminateKeyEstabServer_t *) &(pAnyReq->KeyEstab_TerminateKeyEstabServer));
	    break;	  
#endif
#if gASL_ZclKeyEstab_InitKeyEstabRsp_d            
	  case gAmiZtcOpCode_KeyEstab_InitKeyEstabRsp_c:
	    status = zclKeyEstab_InitKeyEstabRsp((ZclKeyEstab_InitKeyEstabRsp_t *) &(pAnyReq->KeyEstab_InitKeyEstabRsp));
	    break;	  	  
#endif
#if gASL_ZclKeyEstab_EphemeralDataRsp_d            
	  case gAmiZtcOpCode_KeyEstab_EphemeralDataRsp_c:
	    status = zclKeyEstab_EphemeralDataRsp((ZclKeyEstab_EphemeralDataRsp_t *) &(pAnyReq->KeyEstab_EphemeralDataRsp));
	    break;
#endif
#if gASL_ZclKeyEstab_ConfirmKeyDataRsp_d            
	  case gAmiZtcOpCode_KeyEstab_ConfirmKeyDataRsp_c:
	    status = zclKeyEstab_ConfirmKeyDataRsp((ZclKeyEstab_ConfirmKeyDataRsp_t *) &(pAnyReq->KeyEstab_ConfirmKeyDataRsp));
	    break;	  	  
#endif 
#if gASL_ZclKeyEstab_TerminateKeyEstabClient_d            
	  case gAmiZtcOpCode_KeyEstab_TerminateKeyEstabClient_c:
	    status = zclKeyEstab_TerminateKeyEstabClient((ZclKeyEstab_TerminateKeyEstabClient_t *) &(pAnyReq->KeyEstab_TerminateKeyEstabClient));
	    break;	  
#endif
#if gEccIncluded_d == 1
	  case gAmiZtcOpCode_KeyEstab_SetSecMaterial_c:
        status = zclKeyEstab_SetSecurityMaterial((ZclKeyEstab_SetSecurityMaterial_t *) &(pAnyReq->KeyEstab_SetSecurityMaterial));
	    break;
      case gAmiZtcOpCode_KeyEstab_InitKeyEstab_c:
        status = (uint8_t)!ZCL_InitiateKeyEstab(((ZclKeyEstab_InitiateKeyEstab_t *)(pMsg->data))->dstEndpoint, 
                   ((ZclKeyEstab_InitiateKeyEstab_t *)(pMsg->data))->srcEndpoint, ((ZclKeyEstab_InitiateKeyEstab_t *)(pMsg->data))->dstAddr);
        break;      
#endif	 
#if (gStandardSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d)         
#if gASL_ZclSE_RegisterDevice_d
        case gAmiZtcOpCode_SE_RegisterDeviceReq_c:
            if (pSE_ESPRegFunc != NULL)
              status = (*pSE_ESPRegFunc)((EspRegisterDevice_t*)pAnyReq);
          break;
        case gAmiZtcOpCode_SE_DeRegisterDeviceReq_c:
            if (pSE_ESPDeRegFunc != NULL)
              status = (*pSE_ESPDeRegFunc)((EspDeRegisterDevice_t*)pAnyReq);
          break;
#endif  
#endif          
#if gASL_ZclSE_InitTime_d
        case gGeneralZtcOpCode_InitTimeReq_c:
          ZCL_TimeInit(BeeAppDataInit(appEndPoint), (ZCLTimeConf_t *) pAnyReq);
          status = gZclSuccess_c;
        break;
#endif          

#if gZclEnableOTAServer_d
    case gOTAImageNotify_c:
      status = ZCL_OTAImageNotify((zclZtcOTAImageNotify_t *)pAnyReq, 0xFF);
    break;
    case gOTAQueryNextImageResponse_c:
      status = ZCL_OTANextImageResponse((zclZtcOTANextImageResponse_t *)pAnyReq, 0xFF);
      break;
    case gOTABlockResponse_c:
      status = ZCL_OTABlockResponse((zclZtcOTABlockResponse_t *)pAnyReq, 0xFF);
      break;
  case gOTAUpgradeEndResponse_c:
      status = ZCL_OTAUpgradeEndResponse((ztcZclOTAUpgdareEndResponse_t *)pAnyReq, 0xFF);
      break;
#endif	
#if (gZclEnableOTAServer_d == TRUE)||(gZclEnableOTAClient_d == TRUE)
  case gOTAInitiateProcess_c:
      status = OTA_InitiateOtaClusterProcess((zclInitiateOtaProcess_t *)pAnyReq);
      break; 
#endif      
#if gZclEnableOTAClient_d
    case gOTASetClientParams_c:
      status = ZCL_OTASetClientParams((zclOTAClientParams_t *)pAnyReq);
      break;
    case gOTABlockRequest_c:
      status = ZCL_OTABlockRequest((zclZtcOTABlockRequest_t *)pAnyReq);
      break; 
    case gOTAStartClientNextImageTransfer_c:
      status = OTA_OtaStartClientNextImageTransfer((zclStartClientNextImageTransfer_t *)pAnyReq); 
      break;  
    case gOTAQueryNextImageRequest_c:  
      status = ZCL_OTAImageRequest((zclZtcOTANextImageRequest_t*)pAnyReq);
      break;
    case gOTAUpgradeEndRequest_c:  
      status = ZCL_OTAUpgradeEndRequest((zclZtcOTAUpgradeEndRequest_t*)pAnyReq);
      break;
#endif

#if gZclEnablePollControlCluster_d      
   case gHaZtcOpCode_PollControl_SetClientInf_c:  
      status = ztcPollControl_SetClientInf((zclPollControlClientSetup_t * )pAnyReq);
      break;     
   case gHaZtcOpCode_PollControl_FastPollStop_c:   
      status = zclPollControl_FastPollStop((zclPollControl_FastPollStop_t *)pAnyReq);
      break;
   case gHaZtcOpCode_PollControl_SetLongPollInterval_c:   
      status = zclPollControl_SetLongPollInterval((zclPollControl_SetLongPollInterval_t *)pAnyReq);
      break;
   case gHaZtcOpCode_PollControl_SetShortPollInterval_c:   
      status = zclPollControl_SetShortPollInterval((zclPollControl_SetShortPollInterval_t *)pAnyReq);
      break;  
   case gHaZtcOpCode_PollControl_CheckIn_c:   
      status = zclPollControl_CheckIn((zclPollControl_CheckIn_t *)pAnyReq);
      break;         
#endif      

#if gASL_EnableZllTouchlinkCommissioning_d      
   case gZLlZtcOpCode_CommissioningUtility_SendEndpointInformation_c:   
      status = zclZllCommissioningUtility_SendEndpointInformation((zllCommissioning_SendEndpointInf_t *) &(pAnyReq->sendEndpointInf));
      break;           
   case gZLlZtcOpCode_CommissioningUtility_GetEndpointListReq_c:   
      status = zclZllCommissioningUtility_GetEndpointListReq((zllCommissioning_GetEndpointListReq_t *) &(pAnyReq->getEndpointList));
      break;  
   case gZLlZtcOpCode_CommissioningUtility_GetGroupIdReq_c:   
      status = zclZllCommissioningUtility_GetGroupIdReq((zllCommissioning_GetGroupIdReq_t *) &(pAnyReq->getGroupIdReq));
      break;   
#endif      
      
#if gASL_ZclSETunneling_d
          case gSeTunnel_Client_TunnelReq_c:
            status = ZCL_SETunnel_Client_RequestTunnelReq((zclSETunneling_RequestTunnelReq_t *)pAnyReq);
            break;
          case gSeTunnel_LoadFragment_c:
            status = ZCL_SETunnel_LoadFragment((zclSETunneling_LoadFragment_t *)pAnyReq);
            break;
          case gSeTunnel_TransferData_c:
            status = ZCL_SETunnel_ZTCTransferDataReq((zclSETunneling_ZTCTransferDataReq_t *)pAnyReq);
            break;
          case gSeTunnel_Server_SetNextTunnelID_c:
            status = ZCL_SETunnel_Server_SetNextTunnelID(pAnyReq->zclSETunneling_TunnelID);
            break;
#if gASL_ZclSETunnelingOptionals_d            
          case gSeTunnel_ReadyRx_c:
            status = ZCL_SETunnel_ReadyRx((zclSETunneling_ReadyRx_t *)pAnyReq);
            break;
#endif
#endif
    
#if gASL_ZclLevelControlReq_d          
    default:
      /* Send level cluster commands; they have a count of 8 and are similar, so 
       * they are processed based on opcode difference from MoveToLevel */
      if (opCode >= gHaZtcOpCode_LevelControlCmd_MoveToLevel_c && opCode <= gHaZtcOpCode_LevelControlCmd_StopOnOff_c)
        status = ASL_ZclLevelControlReq(&(pAnyReq->levelControlReq), gZclCmdLevelControl_MoveToLevel_c + (opCode - gHaZtcOpCode_LevelControlCmd_MoveToLevel_c));
      break;
#endif      
      	    
  }
  

  /* done, send confirm */
  /* ZTC data req */
#ifndef gHostApp_d  
  ZTCQueue_QueueToTestClient(&status, gHaZtcOpCodeGroup_c, opCode, sizeof(status));
#else
  ZTCQueue_QueueToTestClient(gpHostAppUart, &status, gHaZtcOpCodeGroup_c, opCode, sizeof(status));
#endif
}

void ZclSEReceiveZtcMessage
(
  ZTCMessage_t* pMsg    /* IN: message from ZTC/UART (Test Tool) */
)
{
  uint8_t opCode;
  zbStatus_t status;
  zclAnyReq_t *pAnyReq;
  
  /* opcodes */
  opCode = pMsg->opCodeId;
  status = gZclUnsupportedGeneralCommand_c;
  pAnyReq = (void *)pMsg->data;
  
  switch(opCode)
  {
#if (gStandardSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d)         
#if gASL_ZclSE_RegisterDevice_d
      case gAmiZtcOpCode_SE_ReRegisterDeviceReq_c:
            status = ZCL_ESIReRegisterDevice((EspReRegisterDevice_t*)pAnyReq);
        break;  
#endif  
#endif  

#if gASL_ZclSE_12_Features_d           
#if gASL_ZclPrice_GetTierLabelsReq_d            
      case gAmiZtcOpCode_Price_GetTierLabelsReq_c:
        status = zclPrice_GetTierLabelsReq((zclPrice_GetTierLabelsReq_t *) &(pAnyReq->Price_GetTierLabelsReq));
        break;
#endif 
#endif 

#if gASL_ZclSE_12_Features_d            
#if gASL_ZclPrice_PublishTierLabelsRsp_d            
      case gAmiZtcOpCode_Price_StoreServerTierLabels_c:
        status = ZCL_StoreServerTierLabels(( ztcCmdPrice_PublishTierLabelsRsp_t *)pAnyReq);
        break;
#endif
#endif

#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_Optionals_d            
#if gASL_ZclPrice_PublishCreditPaymentRsp_d            
	  case gAmiZtcOpCode_Price_StoreServerCreditPayment_c:
	    status = ZCL_StoreServerCreditPayment((ztcCmdPrice_PublishCreditPaymentRsp_t *)pAnyReq);
	    break;
#endif
#endif
#if gASL_ZclSE_12_Features_d && gASL_ZclPrice_Optionals_d            
#if gASL_ZclPrice_PublishCurrencyConversionRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleCurrencyConversion_c:
	    status = ZCL_ScheduleServerCurrencyConversion((ztcCmdPrice_PublishCurrencyConversionRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclPrice_GetCurrencyConversionReq_d 
          case gAmiZtcOpCode_Price_GetCurrencyConversionReq_c:
	    status = zclPrice_GetCurrencyConversionReq((zclPrice_GetCurrencyConversionReq_t *)pAnyReq);
	    break;
#endif

#if gASL_ZclPrice_PublishExtendedPriceRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerExtendedPrice_c:
	    status = ZCL_ScheduleServerPriceEvents((ztcCmdPrice_PublishPriceRsp_t *)pAnyReq, TRUE);
	    break;
#endif
            
#if gASL_ZclPrice_CancelTariff_d              
          case gAmiZtcOpCode_Price_CancelTariffReq_c:
            status = zclPrice_CancelTariffReq((zclPrice_CancelTariffReq_t *)pAnyReq);
            break;
#endif      

#if gASL_ZclMet_TakeSnapshotReq_d
          case gAmiZtcOpCode_Met_TakeSnapshotReq_c:
            status = ZclMet_TakeSnapshotRsp(&(pAnyReq->Met_TakeSnapshotReq));
          break;  

#if gASL_ZclSE_12_Features_d            
#if gASL_ZclPrepayment_ChangeDebtReq_d
          case gAmiZtcOpCode_Prepayment_ChangeDebtReq_c:
	    status = zclPrepayment_Client_ChangeDebtReq((zclPrepayment_ChangeDebtReq_t *) &(pAnyReq->Prepayment_ChangeDebtReq));
	    break;
#endif

#if gASL_ZclPrepayment_EmergencyCreditSetupReq_d
            case gAmiZtcOpCode_Prepayment_EmergencyCreditSetupReq_c:
	    status = zclPrepayment_Client_EmergencyCreditSetupReq((zclPrepayment_EmergencyCreditSetupReq_t *) &(pAnyReq->Prepayment_EmergencyCreditSetupReq));
	    break;
#endif

#if gASL_ZclPrepayment_ConsumerTopUpReq_d
            case gAmiZtcOpCode_Prepayment_ConsumerTopUpReq_c:
	    status = zclPrepayment_Client_ConsumerTopUpReq((zclPrepayment_ConsumerTopUpReq_t *) &(pAnyReq->Prepayment_ConsumerTopUpReq));
	    break;
#endif

#if gASL_ZclPrepayment_CreditAdjustmentReq_d
            case gAmiZtcOpCode_Prepayment_CreditAdjustmentReq_c:
	    status = zclPrepayment_Client_CreditAdjustmentReq((zclPrepayment_CreditAdjustmentReq_t *) &(pAnyReq->Prepayment_CreditAdjustmentReq));
	    break;
#endif

#if gASL_ZclPrepayment_ChangePaymentModeReq_d
            case gAmiZtcOpCode_Prepayment_ChangePaymentModeReq_c:
	    status = zclPrepayment_Client_ChangePaymentModeReq((zclPrepayment_ChangePaymentModeReq_t *) &(pAnyReq->Prepayment_ChangePaymentModeReq));
	    break;
#endif

#if gASL_ZclPrepayment_GetPrepaySnapshotReq_d
            case gAmiZtcOpCode_Prepayment_GetPrepaySnapshotReq_c:
	    status = zclPrepayment_Client_GetPrepaySnapshotReq((zclPrepayment_GetPrepaySnapshotReq_t *) &(pAnyReq->Prepayment_GetPrepaySnapshotReq));
	    break;
#endif

#if gASL_ZclPrepayment_GetTopUpLogReq_d
            case gAmiZtcOpCode_Prepayment_GetTopUpLogReq_c:
	    status = zclPrepayment_Client_GetTopUpLogReq((zclPrepayment_GetTopUpLogReq_t *) &(pAnyReq->Prepayment_GetTopUpLogReq));
	    break;
#endif

#if gASL_ZclPrepayment_SetLowCreditWarningLevelReq_d
            case gAmiZtcOpCode_Prepayment_SetLowCreditWarningLevelReq_c:
	    status = zclPrepayment_Client_SetLowCreditWarningLevelReq((zclPrepayment_SetLowCreditWarningLevelReq_t *) &(pAnyReq->Prepayment_SetLowCreditWarningLevelReq));
	    break;
#endif

#if gASL_ZclPrepayment_GetDebtRepaymentLogReq_d
            case gAmiZtcOpCode_Prepayment_GetDebtRepaymentLogReq_c:
	    status = zclPrepayment_Client_GetDebtRepaymentLogReq((zclPrepayment_GetDebtRepaymentLogReq_t *) &(pAnyReq->Prepayment_GetDebtRepaymentLogReq));
	    break;
#endif
            
          case gAmiZtcOpCode_Prepayment_GetCommandReq_c:
            status = zclPrepayment_Server_GetCommandRsp((zclPrepayment_GetCommandRsp_t *) &(pAnyReq->Prepayment_GetCommandRsp));
            break;
#endif /* gASL_ZclSE_12_Features_d */
#endif /* #if gASL_ZclPrepayment_d */
          
#if gASL_ZclSE_InitTime_d
      
        case gAmiZtcOpCode_Time_GetUTCTime_c:
        {
          ZCLTime_t currentTime = ZCL_GetUTCTime(BeeAppDataInit(appEndPoint));  
#ifndef gHostApp_d  
          ZTCQueue_QueueToTestClient((uint8_t*)&currentTime, gSeZtcOpCodeGroup_c, opCode, sizeof(ZCLTime_t));
#else
          ZTCQueue_QueueToTestClient(gpHostAppUart, &currentTime, gSeZtcOpCodeGroup_c, opCode, sizeof(ZCLTime_t));
#endif 
          return; 
        }
#endif /* gASL_ZclSE_InitTime_d */

#if gASL_ZclSE_12_Features_d || gASL_ZclTouCalendar_Optionals_d
        case gAmiZtcOpCode_TouCalendar_StoreServerActivityCalendarInfo_c:
           status = ZtcTouCalendar_StoreServerActivityCalendarInfo((uint8_t *)pAnyReq);
        break;   
        case gAmiZtcOpCode_TouCalendar_StoreServerSpecialDayInfo_c:
           status = ZtcTouCalendar_StoreServerSpecialDayInfo((zclTouCalendar_SpecialDayInfo_t *)pAnyReq);
        break;  
        case gAmiZtcOpCode_TouCalendar_GetCalendar_c:
           status = zclTouCalendar_GetCalendar((zclTouCalendar_GetCalendar_t *)pAnyReq);
        break;
#endif        

#if (gStandardSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d)
#if gZclSETrustCenterSwapOutServer_d            
          case gAmiZtcOpCode_TCSwapOut_GetDeviceData_c:
            status = ZCL_SE_TCSwapOut_GetDeviceData(*(uint8_t*)pAnyReq);
            if (status == gZbSuccess_c) 
              return;
            break;
            
          case gAmiZtcOpCode_TCSwapOut_SetDeviceData_c:
            status = ZCL_SE_TCSwapOut_SetDeviceData((seRegDev_t *)pAnyReq);
            break;
#endif            
#endif     
#if gZclSE_MDUPairing_d  
  case gAmiZtcOpCode_MDUPairing_PairingReq_c:
    status = zclMDUPairing_PairingReq(&pAnyReq->zclMDUPairing_PairingReq);
    break;
  case gAmiZtcOpCode_MDUPairing_AddVHAN_c:
    status = zclMDUPairing_AddVHAN(&pAnyReq->zclMDUPairing_AddVHAN);
    break;
  case gAmiZtcOpCode_MDUPairing_UpdateVHAN_c:
    status = zclMDUPairing_UpdateVHAN(&pAnyReq->zclMDUPairing_UpdateVHAN);
    break;
  case gAmiZtcOpCode_MDUPairing_DeleteVHANs_c:
    status = zclMDUPairing_DeleteVHANs();
    break;
#endif     
#if gZclSE_DevMgmt_d
  case gAmiZtcOpCode_DevMgmt_GetChangeOfTenancyReq_c:
    status = zclDevMgmt_GetChangeOfTenancyReq(&pAnyReq->zclDevMgmt_GetChangeOfTenancyReq);
    break;
  case gAmiZtcOpCode_DevMgmt_GetChangeOfSupplierReq_c:
    status = zclDevMgmt_GetChangeOfSupplierReq(&pAnyReq->zclDevMgmt_GetChangeOfSupplierReq);
    break;
  case gAmiZtcOpCode_DevMgmt_GetChangeSupplyReq_c:
    status = zclDevMgmt_GetChangeSupplyReq(&pAnyReq->zclDevMgmt_GetChangeSupplyReq);
    break;
  case gAmiZtcOpCode_DevMgmt_RequestNewPasswordReq_c:
    status = zclDevMgmt_RequestNewPasswordReq(&pAnyReq->zclDevMgmt_ReqNewPasswordReq);
    break;
  case gAmiZtcOpCode_DevMgmt_LocalChangeSupplyReq_c:
    status = zclDevMgmt_LocalChangeSupplyRsp(&pAnyReq->zclDevMgmt_LocalChangeSupplyRsp);
    break;
  case gAmiZtcOpCode_DevMgmt_UpdateSiteIDReq_c:
    status = zclDevMgmt_UpdateSiteIDRsp(&pAnyReq->zclDevMgmt_UpdateSiteRsp);
    break;
  case gAmiZtcOpCode_DevMgmt_SetSupplyStatusReq_c:
    status = zclDevMgmt_SetSupplyStatusRsp(&pAnyReq->zclDevMgmt_SetSupplyStatusRsp);
    break;
  case gAmiZtcOpCode_DevMgmt_SetEvtCfgReq_c:
    status = zclDevMgmt_SetEvtCfgRsp(&pAnyReq->zclDevMgmt_SetEvtCfgRsp);
    break;
  case gAmiZtcOpCode_DevMgmt_GetEvtCfgReq_c:
	status = zclDevMgmt_GetEvtCfgRsp(&pAnyReq->zclDevMgmt_GetEvtCfgRsp);
    break;
  case gAmiZtcOpCode_DevMgmt_ChangeTenancy_c:
    status = zclDevMgmt_ChangeTenancy(&pAnyReq->zclDevMgmt_ChangeTenancy);
    break;
  case gAmiZtcOpCode_DevMgmt_ChangeSupplier_c:
    status = zclDevMgmt_ChangeSupplier(&pAnyReq->zclDevMgmt_ChangeSupplier);
    break;
#endif
  }
  
  /* done, send confirm */
  /* ZTC data req */
#ifndef gHostApp_d  
  ZTCQueue_QueueToTestClient(&status, gSeZtcOpCodeGroup_c, opCode, sizeof(status));
#else
  ZTCQueue_QueueToTestClient(gpHostAppUart, &status, gSeZtcOpCodeGroup_c, opCode, sizeof(status));
#endif
  (void)pAnyReq;
}

/*
	ZclReceiveZtcMessage

	Receives a message from ZTC (Test Tool). Used to control the ZCL and HA
	application through the serial port.
*/
void ZclReceiveZtcMessage
(
  ZTCMessage_t* pMsg    /* IN: message from ZTC/UART (Test Tool) */
)
{
  /* ignore invalid opcodes */
  if(pMsg->opCode == gHaZtcOpCodeGroup_c)
    ZclHAReceiveZtcMessage(pMsg);
  else if(pMsg->opCode == gSeZtcOpCodeGroup_c)
    ZclSEReceiveZtcMessage(pMsg);
}

zbStatus_t ZCL_SE_TCSwapOut_GetDeviceData(uint8_t startIdx)
{
  uint8_t i;
  zclZtcSE_RegDataCnf_t *pMsg;
  uint8_t numEntries = 0;
  
  pMsg = MSG_Alloc(gMaxRxTxDataLength_c);
 
  if (pMsg == NULL)
    return gZbFailed_c;
                                                 
  for (i = 0; (i + startIdx) < RegistrationTableEntries_c; i++)
  {
    if (ZCL_CreateRegDevBackupEntry(i + startIdx, &pMsg->devList[i]))
      numEntries++;
    
    if (numEntries >= (gMaxRxTxDataLength_c / sizeof(seRegDev_t)))
      break;
  }
  
   pMsg->numDevInList = numEntries;
   pMsg->startIdx = startIdx;
   pMsg->totalRegDev = ZclSE_GetNumOfRegisteredDevices();
  
#ifndef gHostApp_d  
  ZTCQueue_QueueToTestClient((const uint8_t*)pMsg, gSeZtcOpCodeGroup_c, gAmiZtcOpCode_TCSwapOut_GetDeviceData_c, numEntries * sizeof(seRegDev_t) + 3);
#else
  ZTCQueue_QueueToTestClient(gpHostAppUart, pMsg, gSeZtcOpCodeGroup_c, gAmiZtcOpCode_TCSwapOut_GetDeviceData_c, numEntries * sizeof(seRegDev_t) + 3);
#endif
  
  MSG_Free(pMsg);
  return gZbSuccess_c;
}
