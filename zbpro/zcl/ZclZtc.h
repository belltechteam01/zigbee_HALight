/******************************************************************************
* ZclZtc.h
*
* Definitions for interacting with the ZigBee Test Client (ZTC), allowing
* Freescale Test Tool to control the application.
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
* [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf
******************************************************************************/
#ifndef _ZCLZTC_H_
#define _ZCLZTC_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "ZCL.h"
#include "SEProfile.h"
#include "ZclProtocInterf.h"
#include "ZclOTA.h"
#include "ZclSETunneling.h"
#include "ZCLSEPrepayment.h"
#include "ZCLSEMessaging.h"
#include "ZCLSEmetering.h"
#include "ZclSECalendar.h"
#include "zclSEMDUPairing.h"
#include "zclSEDevMgmt.h"
#include "HaProfile.h"
#ifdef gZclEnablePartition_d    
#if gZclEnablePartition_d
#include "ZclPartition.h"
#endif
#endif
/******************************************************************************
*******************************************************************************
* Public macros & types
*******************************************************************************
******************************************************************************/
void ZclReceiveZtcMessage(ZTCMessage_t* pMsg);
void InitZtcForZcl(void);

/******************************************
	ZTC Interface to HA/SE/HC
*******************************************/

/* only used with ZTC interface (optional) */
#define gHaZtcOpCodeGroup_c                 0x70 /* opcode group used for HA commands/events */
#define gSeZtcOpCodeGroup_c                 0x72 /* opcode group used for new SE commands/events */

/* general ZCL commands only used with ZTC interface (optional) */
#define gHaZtcOpCode_ReadAttr_c             0x00 /* Read attributes 2.4.1 */
#define gHaZtcOpCode_ReadAttrRsp_c          0x01 /* Read attributes response 2.4.2 (event only) */
#define gHaZtcOpCode_WriteAttr_c            0x02 /* Write attributes 2.4.3 */
#define gHaZtcOpCode_WriteAttrUndivided_c   0x03 /* Write attributes undivided 2.4.4 */
#define gHaZtcOpCode_WriteAttrRsp_c         0x04 /* Write attributes response 2.4.5 (event only) */
#define gHaZtcOpCode_WriteAttrNoRsp_c       0x05 /* Write attributes no response 2.4.6 */
#define gHaZtcOpCode_CfgReporting_c         0x06 /* Configure reporting 2.4.7 */
#define gHaZtcOpCode_CfgReportingRsp_c      0x07 /* Configure reporting response 2.4.8 (event only) */
#define gHaZtcOpCode_ReadReportingCfg_c     0x08 /* Read reporting configuration 2.4.9 */
#define gHaZtcOpCode_ReadReportingCfgRsp_c  0x09 /* Read reporting configuration response 2.4.10 (event only) */
#define gHaZtcOpCode_ReportAttr_c           0x0a /* Report attributes 2.4.11 */
#define gHaZtcOpCode_DefaultRsp_c           0x0b /* Default response 2.4.12 (event only) */
#define gHaZtcOpCode_DiscoverAttr_c         0x0c /* Discover attributes 2.4.13 */
#define gHaZtcOpCode_DiscoverAttrRsp_c      0x0d /* Discover attributes response (event only) 2.4.14 */
#define gHaZtcOpCode_DiscoverCmds_c         0x0e /* Discover commands 2.4.18, 2.4.20 */
#define gHaZtcOpCode_DiscoverExtendedAttr_c 0x0f /* Discover attributes extended 2.4.22 */

/* General ZCL commands and cluster specific used with ZTC interface (optional) */
#define gHaZtcOpCode_BasicCmd_Reset_c                                   0x10    /* basic cluster */
#define gAmiZtcOpCode_SmplMet_AcceptFastPollModeReq_c                   0x11
#define gAmiZtcOpCode_SmplMet_GetSnapshotReq_c                  		0x12
#define gAmiZtcOpCode_Met_RequestMirrorReq_c                            0x13
#define gAmiZtcOpCode_Met_RemoveMirrorReq_c                             0x14
#define gHaZtcOpCode_ReadAttrDirected_c                                 0x15    /* Read attributes 2.4.1 */
#define gHaZtcOpCode_WriteAttrDirected_c                                0x16    /* Write attributes 2.4.3 */
#define gHaZtcOpCode_DiscoverAttrDirected_c                             0x17    /* Discover attributes 2.4.13 */
                                                                        //0x18
                                                                        //0x19
#define gSeTunnel_Client_TunnelReq_c                                    0x1A
#define gSeTunnel_TransferData_c                                        0x1B
                                                                        //0x1C
#define gSeTunnel_LoadFragment_c                                        0x1D
#define gSeTunnel_Server_SetNextTunnelID_c                              0x1E
#define gSeTunnel_ReadyRx_c                                             0x1F

#define gHaZtcOpCode_IdentifyCmd_Identify_c                             0x20    /* identify cluster opcodes */
#define gHaZtcOpCode_IdentifyCmd_IdentifyQuery_c                        0x21    
#define gAmiZtcOpCode_Price_GetBillingPeriodReq_c                       0x22    /* price cluster opcodes */
#define gAmiZtcOpCode_Price_ScheduleServerBillingPeriod_c               0x23
#define gAmiZtcOpCode_Price_GetConsolidatedBillReq_c                    0x24
#define gAmiZtcOpCode_Price_ScheduleServerConsolidatedBill_c            0x25
#define gAmiZtcOpCode_Price_GetCO2ValueReq_c                            0x26
#define gAmiZtcOpCode_Price_ScheduleServerCO2Value_c                    0x27
#define gAmiZtcOpCode_Price_GetPriceMatrixReq_c                         0x28
#define gAmiZtcOpCode_Price_StoreServerPriceMatrix_c                    0x29
#define gAmiZtcOpCode_Price_GetTariffInformationReq_c                   0x2A
#define gAmiZtcOpCode_Price_ScheduleServerTariffInformation_c           0x2B
#define gAmiZtcOpCode_Price_GetBlockThresholdsReq_c                     0x2C
#define gAmiZtcOpCode_Price_StoreServerBlockThresholds_c                0x2D
#define gAmiZtcOpCode_Price_ScheduleServerCPPEvent_c                    0x2E 
#define gHaZtcOpCode_IdentifyCmd_EzModeInvoke_c		                	0x2F    /* identify cluster opcode */

#define gHaZtcOpCode_GroupCmd_AddGroup_c                                0x30    /* group cluster opcodes */
#define gHaZtcOpCode_GroupCmd_ViewGroup_c                               0x31
#define gHaZtcOpCode_GroupCmd_GetGroupMembership_c                      0x32
#define gHaZtcOpCode_GroupCmd_RemoveGroup_c                             0x33
#define gHaZtcOpCode_GroupCmd_RemoveAllGroups_c                         0x34
#define gHaZtcOpCode_GroupCmd_AddGroupIfIdentifying_c                   0x35
#define gZclZtcOpCode_Alarms_GetAlarm_c                                 0x36    /* alarms cluster opcodes */
#define gZclZtcOpCode_Alarms_ResetAlarm_c                               0x37 
#define gZclZtcOpCode_Alarms_ResetAllAlarms_c                           0x38
#define gZclZtcOpCode_Alarms_Alarm_c                                    0x39
                                                                        //0x3A
#define gZclZtcOpCode_ColorControl_MoveToHue_c                          0x3B    /* Color Control opcodes*/
#define gZclZtcOpCode_ColorControl_MoveHue_c                            0x3C
                                                                        //0x3D
#define gHaZtcOpCode_IdentifyCmd_TriggerEffect_c	                	0x3E    /* Identify Cluster opcodes - ZLL */
#define gHaZtcOpCode_IdentifyCmd_UpdateCommissioningState_c	        	0x3F

#define gHaZtcOpCode_SceneCmd_AddScene_c                                0x40    /* scene cluster opcodes */
#define gHaZtcOpCode_SceneCmd_ViewScene_c                               0x41
#define gHaZtcOpCode_SceneCmd_RemoveScene_c                             0x42
#define gHaZtcOpCode_SceneCmd_RemoveAllScenes_c                         0x43
#define gHaZtcOpCode_SceneCmd_StoreScene_c                              0x44
#define gHaZtcOpCode_SceneCmd_RecallScene_c                             0x45
#define gHaZtcOpCode_SceneCmd_GetSceneMembership_c                      0x46
#define gHaZtcOpCode_SceneCmd_EnhancedAddScene_c                        0x47    
#define gHaZtcOpCode_SceneCmd_EnhancedViewScene_c                       0x48    
#define gZclZtcOpCode_Alarms_ResetAlarmLog_c                            0x49    /* alarms cluster opcode */
#define gHaZtcOpCode_SceneCmd_CopyScene_c                               0x4A    
                                                                        //0x4B
#define gZclZtcOpCode_ColorControl_StepHue_c                            0x4C    /* Color Control*/
#define gZclZtcOpCode_ColorControl_MoveToSaturation_c                   0x4D
#define gZclZtcOpCode_ColorControl_MoveSaturation_c                     0x4E
#define gZclZtcOpCode_ColorControl_StepSaturation_c                     0x4F

#define gHaZtcOpCode_OnOffCmd_Off_c                                     0x50    /* on/off cluster opcodes */
#define gHaZtcOpCode_OnOffCmd_OffWithEffect_c                           0x51    
#define gHaZtcOpCode_OnOffCmd_OnWithRecallGlobalScene_c                 0x52    
#define gHaZtcOpCode_OnOffCmd_OnWithTimedOff_c                          0x53    
                                                                        //0x54
#define gHaZtcOpCode_Thermostat_SetWeeklyScheduleReq                    0x55    /* Thermostat cluster opcodes */
#define gHaZtcOpCode_Thermostat_GetWeeklyScheduleReq                    0x56
#define gHaZtcOpCode_Thermostat_ClearWeeklyScheduleReq                  0x57
#define gHaZtcOpCode_Thermostat_SetpointRaiseLowerReq                   0x58
                                                                        //0x59
                                                                        //0x5A
#define gZclZtcOpCode_ColorControl_MoveToHueSaturation_c                0x5B    /* Color Control cluster opcodes */
#define gZclZtcOpCode_ColorControl_MoveToColor_c                        0x5C
#define gZclZtcOpCode_ColorControl_MoveColor_c                          0x5D
#define gZclZtcOpCode_ColorControl_StepColor_c                          0x5E
#define gZclZtcOpCode_ColorControl_MoveToColorTemperature_c             0x5F

#define gHaZtcOpCode_LevelControlCmd_MoveToLevel_c                      0x60    /* level control cluster opcodes */
#define gHaZtcOpCode_LevelControlCmd_Move_c                             0x61
#define gHaZtcOpCode_LevelControlCmd_Step_c                             0x62
#define gHaZtcOpCode_LevelControlCmd_Stop_c                             0x63
#define gHaZtcOpCode_LevelControlCmd_MoveToLevelOnOff_c                 0x64 
#define gHaZtcOpCode_LevelControlCmd_MoveOnOff_c                        0x65
#define gHaZtcOpCode_LevelControlCmd_StepOnOff_c                        0x66
#define gHaZtcOpCode_LevelControlCmd_StopOnOff_c                        0x67
#define gHaZtcOpCode_DoorLockCmds_c     		                0x68   
#define gHaZtcOpCode_DoorLockClearCmds_c     		                0x69  
                                                                        //0x6A
                                                                        //0x6B
                                                                        //0x6C
                                                                        //0x6D
                                                                        //0x6E
                                                                        //0x6F

#define gHaZtcOpCode_CommissioningRestartDeviceRequest_c                0x70
#define gHaZtcOpCode_CommissioningSaveStartupParametersRequest_c        0x71
#define gHaZtcOpCode_CommissioningRestoreStartupParametersRequest_c     0x72
#define gHaZtcOpCode_CommissioningResetStartupParametersRequest_c       0x73
#define gHaZtcOpCode_IASZoneCmd_EnrollReq_c                             0x74    /* IAS Zone cluster opcodes */  
#define gHaZtcOpCode_IASZoneCmd_ChangeNotif_c                           0x75
#define gHaZtcOpCode_IASACECmd_Arm_c                                    0x76    /* IAS ACE cluster opcodes */
#define gHaZtcOpCode_IASACECmd_Bypass_c                                 0x77
#define gHaZtcOpCode_IASACECmd_Emergency_c                              0x78
#define gHaZtcOpCode_IASACECmd_Fire_c                                   0x79
#define gHaZtcOpCode_IASACECmd_Panic_c                                  0x7A
#define gHaZtcOpCode_IASACECmd_GetZoneIDMap_c                           0x7B
#define gHaZtcOpCode_IASACECmd_GetZoneInf_c                             0x7C
#define gHaZtcOpCode_IASWDCmd_StartWarning_c                            0x7D    /* IAS WD cluster opcodes */ 
#define gHaZtcOpCode_IASZone_AddEntryInClientTable_c                    0x7E
#define gHaZtcOpCode_IASWDCmd_Squawk_c                                  0x7F

#define gAmiZtcOpCode_Msg_DisplayMsgReq_c                               0x81    /* Msg cluster opcodes */
#define gAmiZtcOpCode_Msg_CancelMsgReq_c                                0x82
#define gAmiZtcOpCode_Msg_GetLastMsgRequest_c                           0x83
#define gAmiZtcOpCode_Msg_MsgConfReq                                    0x84
#define gAmiZtcOpCode_SmplMet_GetProfReq_c                              0x85    /* Metering cluster opcodes */
#define gAmiZtcOpCode_SmplMet_GetProfRsp_c                              0x86
#define gAmiZtcOpCode_DmndRspLdCtrl_ReportEvtStatusReq_c                0x87    /* DmndRspLdCtrl Cluster opcodes */
#define gAmiZtcOpCode_DmndRspLdCtrl_LdCtrlEvtReq_c                      0x88
#define gAmiZtcOpCode_DmndRspLdCtrl_CancelLdCtrlEvtReq_c                0x89
#define gAmiZtcOpCode_DmndRspLdCtrl_CancelAllLdCtrlEvtReq_c             0x8a
#define gAmiZtcOpCode_Price_GetCurrPriceReq_c                           0x8b    /* price cluster opcodes */
#define gAmiZtcOpCode_Price_GetSheduledPricesReq_c                      0x8c
#define gAmiZtcOpCode_Price_PublishPriceRsp_c                           0x8d
#define gAmiZtcOpCode_KeyEstab_InitKeyEstabReq_c                        0x8e    /* key establishment cluster opcodes */
#define gAmiZtcOpCode_KeyEstab_EphemeralDataReq_c                       0x8f

#define gAmiZtcOpCode_KeyEstab_ConfirmKeyDataReq_c                      0x90
#define gAmiZtcOpCode_KeyEstab_TerminateKeyEstabServer_c                0x91
#define gAmiZtcOpCode_KeyEstab_InitKeyEstabRsp_c                        0x92
#define gAmiZtcOpCode_KeyEstab_EphemeralDataRsp_c                       0x93
#define gAmiZtcOpCode_KeyEstab_ConfirmKeyDataRsp_c                      0x94
#define gAmiZtcOpCode_KeyEstab_TerminateKeyEstabClient_c                0x95
#define gAmiZtcOpCode_DmndRspLdCtrl_GetScheduledEvtsReq_c               0x96    /* DmndRspLdCtrl Cluster opcodes */
#define gAmiZtcOpCode_DmndRspLdCtrl_ScheduleServerEvt_c                 0x97
#define gAmiZtcOpCode_SmplMet_FastPollModeReq_c                         0x98    /* Metering cluster opcodes */
#define gAmiZtcOpCode_SmplMet_FastPollModeRsp_c                         0x99
#define gZclZtcOpCode_ColorControl_EnhancedMoveToHue_c                  0x9A    /* Color control opcodes*/
#define gZclZtcOpCode_ColorControl_EnhancedMoveHue_c                    0x9B
#define gZclZtcOpCode_ColorControl_EnhancedStepHue_c                    0x9C
#define gZclZtcOpCode_ColorControl_EnhancedMoveToHueSaturation_c        0x9D
#define gZclZtcOpCode_ColorControl_ColorLoopSet_c                       0x9E
#define gZclZtcOpCode_ColorControl_StopMoveStep_c                       0x9F


#define gAmiZtcOpCode_KeyEstab_SetSecMaterial_c	                        0xa0    /* key establishment cluster opcode */
#define gAmiZtcOpCode_Msg_InterPanDisplayMsgReq_c                       0xa1    /* Msg cluster opcodes */
#define gAmiZtcOpCode_Msg_InterPanCancelMsgReq_c                        0xa2
#define gAmiZtcOpCode_Msg_InterPanGetLastMsgRequest_c                   0xa3
#define gAmiZtcOpCode_Msg_InterPanMsgConfReq                            0xa4
#define gAmiZtcOpCode_KeyEstab_InitKeyEstab_c	                        0xa5    /* key establishment cluster opcodes */
#define gZclZtcOpCode_ColorControl_MoveColorTemperature_c               0xA6    /* Color control cluster opcodes */
#define gZclZtcOpCode_ColorControl_StepColorTemperature_c               0xA7
#define gAmiZtcOpCode_Price_ScheduleServerPrice_c                       0xa8    /* price cluster opcodes */
#define gAmiZtcOpCode_Price_UpdateServerPrice_c                         0xa9
#define gAmiZtcOpCode_Price_DeleteServerScheduledPrices_c               0xaa
#define gAmiZtcOpCode_Price_InterPanGetCurrPriceReq_c                   0xab
#define gAmiZtcOpCode_Price_InterPanGetSheduledPricesReq_c              0xac
#define gAmiZtcOpCode_Price_InterPanPublishPriceRsp_c                   0xad
#define gAmiZtcOpCode_SE_RegisterDeviceReq_c                            0xae
#define gAmiZtcOpCode_SE_DeRegisterDeviceReq_c                          0xaf


#define gGeneralZtcOpCode_InitTimeReq_c                                 0xb0
#define gGt_AdvertiseProtocolAddr_c                                     0xb1
#define gGt_MatchProtocolAddr_c                                         0xb2
#define gGt_MatchProtocolAddrRes_c                                      0xb3
#define g11073_TransferApdu_c                                           0xb4
#define g11073_ConnectRequest_c                                         0xb5
#define g11073_DisconnectRequest_c                                      0xb6
#define g11073_ConnectStatusNotif_c                                     0xb7
#define g11073_SetPartitionThreshold_c                                  0xb8
#define g11073_GetIEEE11073MessageProcStatus_c                          0xb9
#define gAmiZtcOpCode_Price_GetBlockPeriodsReq_c                        0xba
#define gAmiZtcOpCode_Price_PublishBlockPeriodsRsp_c                    0xbb
#define gAmiZtcOpCode_Price_ScheduleServerBlockPeriods_c                0xbc
#define gAmiZtcOpCode_Price_UpdateServerBlockPeriods_c                  0xbd
#define gAmiZtcOpCode_Price_DeleteServerScheduledBlockPeriods_c         0xbe
#define gAmiZtcOpCode_Price_ScheduelServeCalorificValueStore_c          0xbf

#define gPartitionZtcOpCode_ReadHandshakeParamReq_c                     0xC0    /* partition cluster opcodes */
#define gPartitionZtcOpCode_WriteHandshakeParamReq_c                    0xC1
#define gPartitionZtcOpCode_ZtcDataEvent_c                              0xC2
#define gPartitionZtcOpCode_ZtcDataTransfer_c                           0xC3
#define gPartitionZtcOpCode_SetDefaultAttrs_c                           0xC4
#define gAmiZtcOpCode_Price_GetCalorificValueReq_c                      0xC5    /* price cluster opcodes */
#define gAmiZtcOpCode_Price_GetConversionFactorReq_c                    0xC6
#define gAmiZtcOpCode_Price_ScheduelServeConversionFactorStore_c        0xC7
#define gPartitionZtcOpCode_RegisterClusterWithTestDataGenerator_c      0xC8
#define gPartitionZtcOpCode_ConfigureTestDataGenerator_c                0xC9
#define gPartitionZtcOpCode_RegisterClusterWithZtcData_c                0xCA
#define gPartitionZtcOpCode_UnRegisterCluster_c                         0xCB
#define gPartitionZtcOpCode_RegisterOutgoingFrame_c                     0xCC
#define gPartitionZtcOpCode_CancelOutgoingFrame_c                       0xCD
#define gPartitionZtcOpCode_RegisterIncomingFrame_c                     0xCE
#define gPartitionZtcOpCode_CancelIncomingFrame_c                       0xCF

#define gOTAImageNotify_c                                               0xD0    /* OTA cluster opcodes */
#define gOTAQueryNextImageRequest_c                                     0xD1
#define gOTAQueryNextImageResponse_c                                    0xD2
#define gOTAUpgradeEndRequest_c                                         0xD3
#define gOTAUpgradeEndResponse_c                                        0xD4
#define gOTASetClientParams_c                                           0xD5
#define gOTABlockRequest_c                                              0xD6
#define gOTABlockResponse_c                                             0xD7
#define gOTAInitiateProcess_c                                           0xD8    /* for initiate Client(for Service Discovery)/Server OTA */
#define gOTAStartClientNextImageTransfer_c                              0xD9    /* to start an image transfer */
#define gOTAImageProgressReport_c                                       0xDA                
#define gHaZtcOpCode_IASZoneCmd_InitNormalOpMode_c                      0xDB    /* IAS Zone Cluster opcodes - HA1.2 Errata */
#define gHaZtcOpCode_IASZoneCmd_InitTestMode_c                          0xDC
#define gHaZtcOpCode_IASACECmd_PanelStatusChanged_c                     0xDD    /* IAS ACE Cluster opcodes - HA1.2 Errata */
#define gHaZtcOpCode_IASACECmd_GetBypassedZoneList_c                    0xDE    
#define gHaZtcOpCode_IASACECmd_GetPanelStatus_c                         0xDF

/* Prepayment cluster specific */
#define gAmiZtcOpCode_Prepayment_SelAvailEmergCreditReq_c       0xE0
#define gAmiZtcOpCode_Prepayment_ChangeSupplyReq_c              0xE1
#define gHaZtcOpCode_PwrProfile_GetPwrProfilePrice_c         		0xE2    /* Power profile cluster opcodes */
#define gHaZtcOpCode_PwrProfile_GetPwrProfilePriceExtended_c     	0xE3
#define gHaZtcOpCode_PwrProfile_GetOverallSchedulePrice_c	    	0xE4
#define gHaZtcOpCode_PwrProfile_PwrProfileScheduleConstraintsReq_c	0xE5	
#define gHaZtcOpCode_PwrProfile_EnergyPhsScheduleStateReq_c		0xE6
#define gHaZtcOpCode_PwrProfile_EnergyPhsScheduleReq_c			0xE7
#define gHaZtcOpCode_PwrProfile_SetCurrentPwrProfileInf_c    		0xE8
#define gHaZtcOpCode_ApplianceCtrl_ExecutionCommand_c			0xE9    /* Appliance control opcodes */
#define gHaZtcOpCode_ApplianceCtrl_SignalState_c			0xEA
#define gHaZtcOpCode_ApplianceCtrl_WriteFunctions_c			0xEB
#define gHaZtcOpCode_ApplianceCtrl_OverloadCommand_c			0xEC
                                                                        //0xED
                                                                        //0xEE
#define gHaZtcOpCode_ApplianceEventsAndAlerts_GetAlerts			0xEF

	
#define gHaZtcOpCode_SetValueAttr_c                                     0xF0
#define gHaZtcOpCode_ProcessUnsolicitedCommand_c                        0xF1
#define gHaZtcOpCode_ApplianceStatistics_LogReq_c		        0xF2    /* ZTC - Appliance Statistics opcodes */
#define gHaZtcOpCode_ApplianceStatistics_LogQueueReq_c		        0xF3
#define gHaZtcOpCode_PwrProfile_PwrProfileReq_c                         0xF4    /* ZTC - Power profile cluster opcodes */
#define gHaZtcOpCode_PwrProfile_PwrProfileStateReq_c         	        0xF5
#define gHaZtcOpCode_PollControl_SetClientInf_c                         0xF6
#define gHaZtcOpCode_PollControl_FastPollStop_c                         0xF7
#define gHaZtcOpCode_PollControl_SetLongPollInterval_c                  0xF8
#define gHaZtcOpCode_PollControl_SetShortPollInterval_c                 0xF9
#define gHaZtcOpCode_PollControl_CheckIn_c                              0xFA
                                                                        //0xFB
#define gZLlZtcOpCode_CommissioningUtility_SendEndpointInformation_c    0xFC
#define gZLlZtcOpCode_CommissioningUtility_GetGroupIdReq_c              0xFD
#define gZLlZtcOpCode_CommissioningUtility_GetEndpointListReq_c         0xFE
                                                                        //0xFF


/**************************************************************************/
/* New SE commands OpCode Group 0x72 */
/**************************************************************************/

#define gAmiZtcOpCode_SE_ReRegisterDeviceReq_c                          0x18
#define gAmiZtcOpCode_Time_GetUTCTime_c	                                0xa6
#define gAmiZtcOpCode_Met_TakeSnapshotReq_c                             0x2f
#define gAmiZtcOpCode_TouCalendar_StoreServerActivityCalendarInfo_c     0xDB
#define gAmiZtcOpCode_TouCalendar_StoreServerSpecialDayInfo_c           0xDC
#define gAmiZtcOpCode_TouCalendar_GetCalendar_c                         0xDD
#define gAmiZtcOpCode_Price_GetTierLabelsReq_c                          0xb1
#define gAmiZtcOpCode_Price_StoreServerTierLabels_c                     0xb2
#define gAmiZtcOpCode_Price_StoreServerCreditPayment_c                  0xb3   
#define gAmiZtcOpCode_Price_GetCurrencyConversionReq_c                  0xb5   
#define gAmiZtcOpCode_Price_ScheduleCurrencyConversion_c                0xb6   
#define gAmiZtcOpCode_Price_ScheduleServerExtendedPrice_c               0xb7   
#define gAmiZtcOpCode_Price_CancelTariffReq_c                           0xb8   
#define gAmiZtcOpCode_TCSwapOut_GetDeviceData_c                         0xF2
#define gAmiZtcOpCode_TCSwapOut_SetDeviceData_c                         0xF3
#define gAmiZtcOpCode_Prepayment_ChangeDebtReq_c                        0xE2
#define gAmiZtcOpCode_Prepayment_EmergencyCreditSetupReq_c              0xE3
#define gAmiZtcOpCode_Prepayment_ConsumerTopUpReq_c                     0xE4
#define gAmiZtcOpCode_Prepayment_CreditAdjustmentReq_c                  0xE5
#define gAmiZtcOpCode_Prepayment_ChangePaymentModeReq_c                 0xE6
#define gAmiZtcOpCode_Prepayment_GetPrepaySnapshotReq_c                 0xE7
#define gAmiZtcOpCode_Prepayment_GetTopUpLogReq_c                       0xE8
#define gAmiZtcOpCode_Prepayment_SetLowCreditWarningLevelReq_c          0xE9
#define gAmiZtcOpCode_Prepayment_GetDebtRepaymentLogReq_c               0xEA
#define gAmiZtcOpCode_Prepayment_GetCommandReq_c                        0xEB

/* MDU Pairing cluster specific */
#define gAmiZtcOpCode_MDUPairing_PairingReq_c                   0x60
#define gAmiZtcOpCode_MDUPairing_AddVHAN_c                      0x61
#define gAmiZtcOpCode_MDUPairing_UpdateVHAN_c                   0x62
#define gAmiZtcOpCode_MDUPairing_DeleteVHANs_c                  0x63

/* Device Management cluster specific */
#define gAmiZtcOpCode_DevMgmt_GetChangeOfTenancyReq_c           0x70
#define gAmiZtcOpCode_DevMgmt_GetChangeOfSupplierReq_c          0x71
#define gAmiZtcOpCode_DevMgmt_GetChangeSupplyReq_c              0x72
#define gAmiZtcOpCode_DevMgmt_RequestNewPasswordReq_c           0x73
#define gAmiZtcOpCode_DevMgmt_LocalChangeSupplyReq_c            0x74
#define gAmiZtcOpCode_DevMgmt_UpdateSiteIDReq_c         	0x75
#define gAmiZtcOpCode_DevMgmt_SetSupplyStatusReq_c         	0x76
#define gAmiZtcOpCode_DevMgmt_SetEvtCfgReq_c		        0x77
#define gAmiZtcOpCode_DevMgmt_GetEvtCfgReq_c		        0x78
#define gAmiZtcOpCode_DevMgmt_ChangeTenancy_c		        0x79
#define gAmiZtcOpCode_DevMgmt_ChangeSupplier_c		        0x7A


/* Union of all requests */
typedef PACKED_UNION zclAnyReq_tag
{
  zclReadAttrReq_t                    readAttrReq;
  zclWriteAttrReq_t                   writeAttrReq;
  zclDiscoverAttrReq_t                discoverAttrReq;
  zclDiscoverAttrExtendedReq_t        discoverAttrExtendedReq;
  zclDiscoverCommandsReq_t            discoverCommandsReq; 
  zclReadAttrDirectedReq_t            readAttrDirectedReq;
  zclWriteAttrDirectedReq_t           writeAttrDirectedReq;
  zclDiscoverAttrDirectedReq_t        discoverAttrDirectedReq;
  zclOnOffReq_t                       onOffReq;
#if gASL_EnableZLLClustersData_d  
  zclOnOff_OnWithTimedOff_t           onWithTimedOff;
  zclOnOff_OnWithRecallGlobalScene_t  onWithRecallGlobalScene;
  zclOnOff_OffWithEffect_t            offWithEffect;
#endif  
  zclColorCtrl_MoveToHue_t            moveToHue;
  zclColorCtrl_MoveHue_t              moveHue;
  zclColorCtrl_StepHue_t              stepHue;
  zclColorCtrl_MoveToSaturation_t     moveToSaturation;
  zclColorCtrl_MoveSaturation_t       moveSaturation;
  zclColorCtrl_StepSaturation_t       stepSaturation;  
  zclColorCtrl_MoveToHueSaturation_t  moveToHueSaturation;
  zclColorCtrl_MoveToColor_t          moveToColor;  
  zclColorCtrl_MoveColor_t            moveColor;  
  zclColorCtrl_StepColor_t            stepColor;  
  zclColorCtrl_MoveToColorTemperature_t      moveToColorTemperature;
  zclColorCtrl_EnhancedMoveToHue_t    enhancedMoveToHue;            
  zclColorCtrl_EnhancedMoveHue_t      enhancedMoveHue; 
  zclColorCtrl_EnhancedStepHue_t      enhancedStepHue; 
  zclColorCtrl_EnhancedMoveToHueSaturation_t enhancedMoveToHueSaturation;         
  zclColorCtrl_ColorLoopSet_t         colorLoopSet;
  zclColorCtrl_StopMoveStep_t         stopMoveStep; 
  zclColorCtrl_MoveColorTemperature_t        moveColorTemperature;
  zclColorCtrl_StepColorTemperature_t        stepColorTemperature;
  zclConfigureReportingReq_t          cfgReporting;
  zclReadReportingCfgReq_t            readReportingCfg;  
  zclLevelControlReq_t                levelControlReq;
  zclBasicResetReq_t                  basicResetReq;
  zclIdentifyReq_t                    identifyReq;	
  zclIdentifyQueryReq_t               identifyQueryReq;
  zclEzModeInvokeReq_t                ezModeInvokeReq;
  zclUpdateCommissioningStateReq_t    updateCommissioningStateReq;
#if gASL_EnableZLLClustersData_d  
  zclIdentifyTriggerEffect_t          identifyTriggerEffect;
#endif  
  zclGroupAddGroupReq_t               addGroupReq;
  zclGroupAddGroupIfIdentifyingReq_t  addGroupIfIdentifyReq;
  zclGroupRemoveGroupReq_t            removeGroupReq;
  zclGroupRemoveAllGroupsReq_t        removeAllGroupsReq;
  zclGroupViewGroupReq_t              viewGroupReq;
  zclGroupGetGroupMembershipReq_t     getGroupMembershipReq;
  zclSceneAddSceneReq_t               addSceneReq;
  zclSceneViewSceneReq_t              viewSceneReq;
  zclSceneRemoveSceneReq_t            removeSceneReq;
  zclSceneRemoveAllScenesReq_t        removeAllScenesReq;
  zclSceneStoreSceneReq_t             storeSceneReq;
  zclSceneRecallSceneReq_t            recallSceneReq;
  zclSceneGetSceneMembershipReq_t     getSceneMembershipreq;
#if gASL_EnableZLLClustersData_d && gASL_ZclCopySceneReq_d 
  zclSceneCopyScene_t                 copyScene;  
#endif  
  zclDoorLockCmdReq_t                 doorLockReq;
  zclDoorLockClearCmdReq_t	      doorLockClearReq;
  
  zclAlarmInformation_ResetAlarm_t    resetAlarmReq;
  zclAlarmInformation_Alarm_t         alarmReq;
  zclAlarmInformation_NoPayload_t     resetAllAlarmsReq;
  zclAlarmInformation_NoPayload_t     getAlarmReq;
  zclAlarmInformation_NoPayload_t     resetAlarmLogReq;
  
  zclCmdCommissiong_RestartDeviceRequest_t RestartDeviceReq;
  zclCmdCommissiong_SaveStartupParametersRequest_t    SaveStartupParameterReq;
  zclCmdCommissiong_RestoreStartupParametersRequest_t RestoreStartupParameterReq;
  zclCmdCommissiong_ResetStartupParametersRequest_t   ResetStartupParameterReq;
  
  zclIASZone_ZoneStatusChange_t   statusChangeNotifReq;
  zclIASZone_ZoneEnrollRequest_t  zoneEnrollReq;
  zclIASZone_InitNormalOpMode_t   initNormalOpMode;
  zclIASZone_InitTestMode_t       initTestMode;
  gZclZoneTable_t iasZoneEntry;

  zclSetAttrValue_t         setAttrValue;
  zclProcessUnsolicitedCommand_t  processUnsolicitedCommand;
  
  zclIASACE_Arm_t arm;
  zclIASACE_Bypass_t bypass;
  zclIASACE_EFP_t emergency;
  zclIASACE_EFP_t panic;
  zclIASACE_EFP_t fire;
  zclIASACE_EFP_t zoneIdMap;
  zclIASACE_GetZoneInformation_t zoneInf;
  zclIASACE_SetBypassedZoneList_t getBypassedZoneList;
  zclIASACE_GetPanelStatus_t getPanelStatus;
  zclIASACE_PanelStatusChanged_t panelStatusChanged;
  
  zclIASWD_StartWarning_t startWarning;
  zclIASWD_Squawk_t  squawk;
  zclPwrProfile_PwrProfileReq_t  pwrProfileReq;
  zclPwrProfile_PwrProfileStateReq_t pwrProfileStateReq;
  zclPwrProfile_EnergyPhsScheduleStateReq_t energyPhsScheduleStateReq;
  zclPwrProfile_PwrProfileScheduleConstraintsReq_t pwrProfileScheduleConstraintsReq;
#if (gZclClusterOptionals_d)
  zclPwrProfile_GetPwrProfilePrice_t  getPwrProfilePrice;
  zclPwrProfile_GetOverallSchedulePrice_t  getOverallSchedulePrice;
  zclPwrProfileInf_t pwrProfileInf;
#endif  
  zclPwrProfile_GetPwrProfilePriceExtended_t  getPwrProfilePriceExtended;
  zclPwrProfile_EnergyPhsScheduleReq_t energyPhsScheduleReq;
  
  zclApplCtrl_ExecutionCommand_t executionCmd;
  zclApplCtrl_CommandWithNoPayload_t signalState;
  zclApplCtrl_WriteFunction_t writeFunction;
  zclApplCtrl_OverloadCommand_t overloadCmd;
  
#if gZclEnableApplianceEventsAlerts_d
  zclApplianceEventsAlerts_GetAlerts_t  getAlerts;
#endif
  
#if gZclEnableApplianceStatistics_d
  zclApplianceStatistics_LogReq_t 		loqRequest;
  zclApplianceStatistics_LogQueueReq_t 	logQueueReq;
#endif
  
  zclThermostat_SetWeeklySchedule_t     SetWeeklyScheduleReq;
  zclThermostat_GetWeeklySchedule_t     GetWeeklyScheduleReq;
  zclThermostat_ClearWeeklySchedule_t   ClearWeeklyScheduleReq;
  zclThermostat_SetpointRaiseLower_t    SetpointRaiseLowerReq;
  
  zclDisplayMsgReq_t              DisplayMsgReq;
  zclCancelMsgReq_t               CancelMsgReq;
  zclGetLastMsgReq_t              GetLastMsgReq;
  zclMsgConfReq_t                 MsgConfReq;
  zclInterPanDisplayMsgReq_t      InterPanDisplayMsgReq;
  zclInterPanCancelMsgReq_t       InterPanCancelMsgReq;
  zclInterPanGetLastMsgReq_t      InterPanGetLastMsgReq;
  zclInterPanMsgConfReq_t         InterPanMsgConfReq;  
  zclSmplMet_GetProfReq_t           SmplMet_GetProfReq;
  zclSmplMet_GetProfRsp_t           SmplMet_GetProfRsp;
  zclSmplMet_FastPollModeReq_t      SmplMet_FastPollModeReq;
  zclSmplMet_ReqFastPollModeRsp_t   SmplMet_FastPollModeRsp;
  zclMet_RequestMirrorReq_t         Met_RequestMirrorReq;
  zclMet_RemoveMirrorReq_t          Met_RemoveMirrorReq;
  bool_t                            SmplMet_AcceptFastPollModeReq;
  zclSmplMet_GetSnapshotReq_t       SmplMet_GetSnapShotReq;
  zclMet_TakeSnapshotReq_t          Met_TakeSnapshotReq;
  
  zclPrepayment_SelAvailEmergCreditReq_t       Prepayment_SelAvailEmergCreditReq;
  zclPrepayment_ChangeSupplyReq_t              Prepayment_ChangeSupplyReq;
  zclPrepayment_ChangeDebtReq_t                Prepayment_ChangeDebtReq;
  zclPrepayment_EmergencyCreditSetupReq_t      Prepayment_EmergencyCreditSetupReq;
  zclPrepayment_ConsumerTopUpReq_t             Prepayment_ConsumerTopUpReq;
  zclPrepayment_CreditAdjustmentReq_t          Prepayment_CreditAdjustmentReq;
  zclPrepayment_ChangePaymentModeReq_t         Prepayment_ChangePaymentModeReq;
  zclPrepayment_GetPrepaySnapshotReq_t         Prepayment_GetPrepaySnapshotReq;
  zclPrepayment_GetTopUpLogReq_t               Prepayment_GetTopUpLogReq;
  zclPrepayment_SetLowCreditWarningLevelReq_t  Prepayment_SetLowCreditWarningLevelReq;
  zclPrepayment_GetDebtRepaymentLogReq_t       Prepayment_GetDebtRepaymentLogReq;
  zclPrepayment_GetCommandRsp_t                Prepayment_GetCommandRsp;
  
  zclDmndRspLdCtrl_ReportEvtStatus_t DmndRspLdCtrl_ReportEvtStatus;
  zclDmndRspLdCtrl_GetScheduledEvts_t DmndRspLdCtrl_GetScheduledEvts;
  zclDmndRspLdCtrl_LdCtrlEvtReq_t DmndRspLdCtrl_LdCtrlEvtReq;
  zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t DmndRspLdCtrl_CancelLdCtrlEvtReq;
  zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t DmndRspLdCtrl_CancelAllLdCtrlEvtsReq;
  zclPrice_GetCurrPriceReq_t       Price_GetCurrPriceReq;
  zclPrice_GetScheduledPricesReq_t    Price_GetSheduledPricesReq;
  zclPrice_PublishPriceRsp_t      Price_PublishPriceRsp;
  zclCmdPrice_PublishConversionFactorRsp_t PublishConversionFactorRsp;
  zclCmdPrice_PublishCalorificValueRsp_t Price_PublishCalorificValueRsp;
  zclPrice_GetBlockPeriodsReq_t   Price_GetBlockPeriodsReq;
  zclPrice_GetBillingPeriodReq_t   Price_GetBillingPeriodReq;
  zclPrice_GetPriceMatrixReq_t   Price_GetPriceMatrixReq;
  zclPrice_GetBlockThresholdsReq_t   Price_GetBlockThresholdsReq;
  zclPrice_GetTierLabelsReq_t   Price_GetTierLabelsReq;
  zclPrice_GetTariffInformationReq_t   Price_GetTariffInformationReq;
  zclPrice_GetCO2ValueReq_t   Price_GetCO2ValueReq;
  zclPrice_GetConsolidatedBillReq_t   Price_GetConsolidatedBillReq;
  zclPrice_GetCalorificValueReq_t   Price_GetCalorificValueReq;
  zclPrice_GetConversionFactorReq_t Price_GetConversionFactor;
  zclPrice_PublishBlockPeriodRsp_t Price_BlockPeriodsRsp;
  zclPrice_InterPanPublishPriceRsp_t InterPanPublishPriceRsp;
  zclPrice_InterPanGetScheduledPricesReq_t InterPanGetScheduledPricesReq;
  zclPrice_InterPanGetCurrPriceReq_t InterPanGetCurrPriceReq;
  ztcCmdPrice_PublishConversionFactorRsp_t ztcPublishConversionFactorRsp;
  ztcCmdPrice_PublishCalorificValueRsp_t ztcPublishCalorificValueRsp;
  ZclKeyEstab_InitKeyEstabReq_t    KeyEstab_InitKeyEstabReq;
  ZclKeyEstab_EphemeralDataReq_t   KeyEstab_EphemeralDataReq;
  ZclKeyEstab_ConfirmKeyDataReq_t  KeyEstab_ConfirmKeyDataReq;
  ZclKeyEstab_TerminateKeyEstabServer_t KeyEstab_TerminateKeyEstabServer;
  ZclKeyEstab_InitKeyEstabRsp_t         KeyEstab_InitKeyEstabRsp;
  ZclKeyEstab_EphemeralDataRsp_t        KeyEstab_EphemeralDataRsp;
  ZclKeyEstab_ConfirmKeyDataRsp_t       KeyEstab_ConfirmKeyDataRsp;
  ZclKeyEstab_TerminateKeyEstabClient_t KeyEstab_TerminateKeyEstabClient;
  ZclKeyEstab_SetSecurityMaterial_t 	KeyEstab_SetSecurityMaterial;
  zclCmdGtAdvertiseProtoAddr_t        Gt_AdvertiseProtoAddr;
  zclCmdGtMpa_t                       Gt_MatchProtoAddr;
  zclCmdGtMpaRes_t                    Gt_MatchProtoAddrRes;
  zclCmd11073_TransferApdu_t          Ieee11073_TrsApdu;
  zclCmd11073_ConnectRequest_t        Ieee11073_ConnectRequest;
  zclCmd11073_DisconnectRequest_t     Ieee11073_DisconnectRequest;
  zclCmd11073_ConnectStatusNotif_t    Ieee11073_ConnectStatusNotif;
  uint8_t Ieee11073_SetPartitionThreshold;
  
  zclTouCalendar_SpecialDayInfo_t   specialDay;  
  zclTouCalendar_GetCalendar_t  getCalendar;
#ifdef gZclEnablePartition_d    
#if (gZclEnablePartition_d == TRUE)
  zclPartitionCmdReadHandshakeParamReq_t partitionReadHandshakeParamReq;
  zbClusterId_t partitionClusterId;
  zclPartitionedClusterFrameInfo_t partitionClusterFrameInfo;
  zclPartitionWriteHandshakeParamReq_t partitionWriteHandshakeParamReq;
  zclPartitionAttrs_t partitionSetDefaultAttrsReq;
#endif
#endif

#ifdef gZclEnableOTAServer_d
#if (gZclEnableOTAServer_d == TRUE)
  zclZtcOTAImageNotify_t  otaImageNotifyParam;
  zclZtcOTANextImageResponse_t  otaNextImageResponseParams;
#endif
#endif
#if (gZclEnableOTAServer_d == TRUE)||(gZclEnableOTAClient_d == TRUE)
    zclInitiateOtaProcess_t       initiateOtaProcess;
#endif
#ifdef gZclEnableOTAClient_d
#if (gZclEnableOTAClient_d == TRUE)
  zclOTAClientParams_t               otaClientParams;
  zclStartClientNextImageTransfer_t  startClientNextImageTransfer;
  zclZtcOTANextImageRequest_t        otaNextImageRequest;
  zclZtcOTAUpgradeEndRequest_t       otaUpgradeEndRequest;
#endif
#endif
  
#if gZclEnablePollControlCluster_d  
  zclPollControlClientSetup_t setClientInf;
  zclPollControl_FastPollStop_t fastPollStop;
  zclPollControl_SetLongPollInterval_t setLongPollInterval;
  zclPollControl_SetShortPollInterval_t setShortPollInterval;
#endif 
  
#if gASL_EnableZllTouchlinkCommissioning_d  
  zllCommissioning_SendEndpointInf_t sendEndpointInf;
  zllCommissioning_GetEndpointListReq_t getEndpointList;
  zllCommissioning_GetGroupIdReq_t getGroupIdReq;
#endif  
  
#if gASL_ZclSETunneling_d
  zclSETunneling_RequestTunnelReq_t   zclSETunneling_RequestTunnelReq;
  zclSETunneling_LoadFragment_t         zclSETunneling_LoadFragment;
  zclSETunneling_ZTCTransferDataReq_t   zclSETunneling_ZTCTransferDataReq;
  uint16_t                              zclSETunneling_TunnelID;
  zclSETunneling_ReadyRx_t              zclSETunneling_ReadyRx;
#endif
#if gZclSETrustCenterSwapOutServer_d  
  seRegDev_t                    seTCSwapOut_SetRegisterData;
#endif  
#if gZclSE_MDUPairing_d  
  zclMDUPairing_PairingReq_t    zclMDUPairing_PairingReq;
  zclMDUPairing_AddVHAN_t       zclMDUPairing_AddVHAN;
  zclMDUPairing_UpdateVHAN_t    zclMDUPairing_UpdateVHAN;
#endif  
#if gZclSE_DevMgmt_d
  zclDevMgmt_GetChangeOfTenancyReq_t    zclDevMgmt_GetChangeOfTenancyReq;
  zclDevMgmt_GetChangeOfSupplierReq_t	zclDevMgmt_GetChangeOfSupplierReq;
  zclDevMgmt_GetChangeSupplyReq_t		zclDevMgmt_GetChangeSupplyReq;
  zclDevMgmt_ReqNewPasswordReq_t		zclDevMgmt_ReqNewPasswordReq;
  zclDevMgmt_LocalChangeSupplyRsp_t		zclDevMgmt_LocalChangeSupplyRsp;
  zclDevMgmt_UpdateSiteIDRsp_t			zclDevMgmt_UpdateSiteRsp;
  zclDevMgmt_SetSupplyStatusRsp_t		zclDevMgmt_SetSupplyStatusRsp;
  zclDevMgmt_SetEvtCfgRsp_t				zclDevMgmt_SetEvtCfgRsp;
  zclDevMgmt_GetEvtCfgRsp_t				zclDevMgmt_GetEvtCfgRsp;
  DevMgmt_ChangeOfTenancy_t				zclDevMgmt_ChangeTenancy;
  DevMgmt_ChangeOfSupplier_t			zclDevMgmt_ChangeSupplier;
#endif
} zclAnyReq_t;

typedef PACKED_STRUCT zclZtcSE_RegDataCnf_tag {
  uint8_t totalRegDev;
  uint8_t startIdx;
  uint8_t numDevInList;
  seRegDev_t devList[1];
} zclZtcSE_RegDataCnf_t;

/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/
void InitZtcForZcl(void);
#ifdef __cplusplus
}
#endif
#endif /* _ZCLZTC_H_ */
