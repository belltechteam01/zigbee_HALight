/*! @file 	ZclEnergyHome.h
 *
 * @brief	Types, definitions and prototypes for the Power Pofile and Appliance Control Cluster Implementation.
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
 
/* 
 *  [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
 *  [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf
*/

#ifndef _ZCL_ENERGYHOME_H
#define _ZCL_ENERGYHOME_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "ZclOptions.h"
#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "EmbeddedTypes.h"
#include "ZCL.h"
#include "ZCLGeneral.h"


/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/

/******************************************
	Power Profile Cluster
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.6
*******************************************/
#if ( TRUE == gBigEndian_c )
/* power profile cluster attributes */
#define gZclAttrPowerProfile_TotalProfileNumId_c        0x0000 /* M - TotalProfileNum */
#define gZclAttrPowerProfile_MultipleSchedulingId_c     0x0100 /* M - MultipleScheduling */
#define gZclAttrPowerProfile_EnergyFormattingId_c       0x0200 /* M - EnergyFormatting */
#define gZclAttrPowerProfile_EnergyRemoteId_c           0x0300 /* M - EnergyRemote */
#define gZclAttrPowerProfile_ScheduleModeId_c           0x0400 /* M - Schedule Mode */
#else
/* power profile cluster attributes */
#define gZclAttrPowerProfile_TotalProfileNumId_c        0x0000 /* M - TotalProfileNum */
#define gZclAttrPowerProfile_MultipleSchedulingId_c     0x0001 /* M - MultipleScheduling */
#define gZclAttrPowerProfile_EnergyFormattingId_c       0x0002 /* M - EnergyFormatting */
#define gZclAttrPowerProfile_EnergyRemoteId_c           0x0003 /* M - EnergyRemote */
#define gZclAttrPowerProfile_ScheduleModeId_c           0x0004 /* M - Schedule Mode */
#endif /* #if ( TRUE == gBigEndian_c ) */

#define gZclAttrSetPowerProfileInformation_c    0x00

/* power profile cluster attributes */
#define gZclAttrIdPowerProfile_TotalProfileNumId_c        0x00 /* M - TotalProfileNum */
#define gZclAttrIdPowerProfile_MultipleSchedulingId_c     0x01 /* M - MultipleScheduling */
#define gZclAttrIdPowerProfile_EnergyFormattingId_c       0x02 /* M - EnergyFormatting */
#define gZclAttrIdPowerProfile_EnergyRemoteId_c           0x03 /* M - EnergyRemote */
#define gZclAttrIdPowerProfile_ScheduleModeId_c           0x04 /* M - Schedule Mode */


/* power profile cluster commands */
/* [R1] 9.6.5 Server Commands Received  */
#define gZclCmdPowerProfile_PowerProfileRequest_c                           0x00  /* M - Power Profile Request                            */
#define gZclCmdPowerProfile_PowerProfileStateRequest_c                      0x01  /* M - Power Profile State Request                      */
#define gZclCmdPowerProfile_GetPowerProfilePriceResponse_c                  0x02  /* M - Get Power Profile Price Response                 */
#define gZclCmdPowerProfile_GetOverallSchedulePriceResponse_c               0x03  /* M - Get Overall Schedule Price Response              */
#define gZclCmdPowerProfile_EnergyPhasesScheduleNotification_c              0x04  /* M - Energy Phases Schedule Notification              */
#define gZclCmdPowerProfile_EnergyPhasesScheduleResponse_c                  0x05  /* M - Energy Phases Schedule Response                  */
#define gZclCmdPowerProfile_PowerProfileScheduleConstraintsRequest_c        0x06  /* M - Power Profile Schedule Constraints Request       */
#define gZclCmdPowerProfile_EnergyPhasesScheduleStateRequest_c              0x07  /* M - Energy Phases Schedule State Request             */
#define gZclCmdPowerProfile_GetPwrProfilePriceExtendedRsp_c                 0x08  /* M - Get Power Profile Price Extended Response        */

/* [R1] 9.6.6 Server Commands Generated  */
#define gZclCmdPowerProfile_PowerProfileNotification_c                      0x00  /* M - Power Profile Notification                       */
#define gZclCmdPowerProfile_PowerProfileResponse_c                          0x01  /* M - Power Profile Response                           */
#define gZclCmdPowerProfile_PowerProfileStateResponse_c                     0x02  /* M - Power Profile State Response                     */
#define gZclCmdPowerProfile_GetPowerProfilePrice_c                          0x03  /* O - Get Power Profile Price                          */
#define gZclCmdPowerProfile_PowerProfileStateNotification_c                 0x04  /* M - Power Profiles State Notification                */
#define gZclCmdPowerProfile_GetOverallSchedulePrice_c                       0x05  /* O - Get Overall Schedule Price                       */
#define gZclCmdPowerProfile_EnergyPhasesScheduleRequest_c                   0x06  /* M - Energy Phases Schedule Request                   */
#define gZclCmdPowerProfile_EnergyPhasesScheduleStateResponse_c             0x07  /* M - Energy Phases Schedule State Response            */
#define gZclCmdPowerProfile_EnergyPhasesScheduleStateNotification_c         0x08  /* M - Energy Phases Schedule State Notification        */
#define gZclCmdPowerProfile_PowerProfileScheduleConstraintsNotification_c   0x09  /* M - Power Profile Schedule Constraints Notification  */
#define gZclCmdPowerProfile_PowerProfileScheduleConstraintsResponse_c       0x0A  /* M - Power Profile Schedule Constraints Response      */
#define gZclCmdPowerProfile_GetPowerProfilePriceExtended_c                  0x0B  /* O - Get Power Profile Price Extended                 */

#define gPwrProfile_AllAvailablePwrProfiles_d      0x00 /* Match all PowerProfileId */
#define gPwrProfile_CurrentPwrProfileId_d          0x01 /* Current Pwr ProfileId*/
#define gPwrProfile_MaxNumberOfPwrProfile_d        0x01 /* Should have the same value as TotalProfileNum */
#define gPwrProfile_MaxEnergyPhaseInf_d            0x02 /* Maxim number of energy and phase information*/
#define gPwrProfile_StartTimeFieldPresent_d        0x01 /* Start Time Fiedl present - see command GetPowerProfilePriceExtended*/ 

#if ( gBigEndian_c != TRUE )
#define gPwrProfile_DefaultExpectedDuration_d       0x000A  	/* Estimated Duration of the specific phase */
#define gPwrProfile_DefaulPriceValue_d		    0x00000073 	/* Default Price Value */
#define gPwrProfile_DefaulOverallPriceValue_d	    0x01000000 	/* Default Overall Price Value */
#else
#define gPwrProfile_DefaultExpectedDuration_d       0x0A00   	/* Estimated Duration of the specific phase */
#define gPwrProfile_DefaulPriceValue_d		    0x73000000 	/* Default Price Value */
#define gPwrProfile_DefaulOverallPriceValue_d	    0x00000001 	/* Default Overalll Price Value */
#endif
#define gPwrProfile_DefaultPeakPower_d             0x01   /* Estimated EnergyConsumption of the specific phase */  
#define gPwrProfile_DefaultMaxActivationDelay_d    0xFFFF /* Maximum interruption time between the end of the previous phase and the begininning of the specific phase */      
#define gPwrProfile_DefaultActivationDelay_d       180    /* Default interruption time between the end of the previous phase and the begininning of the specific phase -  in seconds*/           

#define gPwrProfileClient_MaxNoServerDevices_d          0x01 /* max no. of server devices*/
#define gPwrProfileClient_MaxPwrProfileInfSupported_d   0x02 /* max no. of pwr profile information supported for each device */
#define gPriceDefaultValueForStart_d                    0x0A /* default price value that can start the appliance cycle, also gStartApplianceIfPrice should be TRUE*/

/*currency symbol defined by ISO 4217 */
#if ( gBigEndian_c != TRUE )
  #define gISO4217Currency_EUR_c  0x03D2  /* 978 - Euro Symbol*/
  #define gISO4217Currency_USD_c  0x0348  /* 840 - US Dollar Symbol*/
#else
  #define gISO4217Currency_EUR_c  0xD203  /* 978 - Euro Symbol*/
  #define gISO4217Currency_USD_c  0x4803  /* 840 - US Dollar Symbol */
#endif /* #if ( TRUE == gBigEndian_c ) */


typedef PACKED_STRUCT zclPowerProfileAttrsRAM_tag
{
  uint8_t    totalProfileNum; 
  bool_t     multipleScheduling;
  uint8_t    energyFormatting;
  bool_t     energyRemote[zclReportableCopies_c];
  uint8_t    scheduleMode[zclReportableCopies_c];
} zclPowerProfileAttrsRAM_t;

/* Schedule Mode Field Bitmap */
enum 
{
  gPwrProfile_ScheduleModeNone_c = 0x00,     /* No Schedule Mode          */
  gPwrProfile_ScheduleModeCheapest_c = 0x01, /* Schedule mode Cheapest     */
  gPwrProfile_ScheduleModeGreenest_c = 0x02  /* Schedule mode Greenest     */  
};

/* Energy Phase IDs*/
enum
{
  gPwrProfile_EnergyPhaseId1_c= 0x01,
  gPwrProfile_EnergyPhaseId2_c,
  gPwrProfile_EnergyPhaseId3_c
};

/* payload format for PowerProfileRequest, PowerProfileScheduleConstraintsRequest, 
   EnergyPhasesScheduleStateRequest, EnergyPhasesScheduleRequest and GetPowerProfilePrice Commands */
typedef PACKED_STRUCT zclCmdPwrProfile_PwrProfileReq_tag 
{
  uint8_t  pwrProfileId;
}zclCmdPwrProfile_PwrProfileReq_t; 

/* [R1] 9.6.5.1  PowerProfileRequest Command */
typedef PACKED_STRUCT zclPwrProfile_PwrProfileReq_tag 
{
   afAddrInfo_t                     addrInfo;
   uint8_t                          zclTransactionId;
   zclCmdPwrProfile_PwrProfileReq_t cmdFrame;
} zclPwrProfile_PwrProfileReq_t;

/* [R1] 9.6.5.2  PowerProfileStateRequest Command */
typedef PACKED_STRUCT zclPwrProfile_PwrProfileStateReq_tag 
{
   afAddrInfo_t                             addrInfo;
   uint8_t                                  zclTransactionId;
}zclPwrProfile_PwrProfileStateReq_t;

/* payload format for GetOverallSchedulePriceResponse Command */
typedef PACKED_STRUCT zclCmdPwrProfile_GetPriceInfRsp_tag 
{
  uint16_t currency;
  uint32_t price;
  uint8_t  priceTrailingDigit;
}zclCmdPwrProfile_GetPriceInfRsp_t; 

/* payload format for GetPowerProfilePriceResponse Command */
typedef PACKED_STRUCT zclCmdPwrProfile_GetPwrProfilePriceRsp_tag 
{
  uint8_t  pwrProfileId;
  zclCmdPwrProfile_GetPriceInfRsp_t priceInf;
}zclCmdPwrProfile_GetPwrProfilePriceRsp_t; 

/* [R1] 9.6.5.3  GetPowerProfilePriceResponse Command */
typedef PACKED_STRUCT zclPwrProfile_GetPwrProfilePriceRsp_tag 
{
   afAddrInfo_t                             addrInfo;
   uint8_t                                  zclTransactionId;
   zclCmdPwrProfile_GetPwrProfilePriceRsp_t cmdFrame;
} zclPwrProfile_GetPwrProfilePriceRsp_t;

/* GetPowerProfilePriceExtendedResponse Command*/
typedef PACKED_STRUCT zclPwrProfile_GetPwrProfilePriceExtendedRsp_tag 
{
   afAddrInfo_t                             addrInfo;
   uint8_t                                  zclTransactionId;
   zclCmdPwrProfile_GetPwrProfilePriceRsp_t cmdFrame;
} zclPwrProfile_GetPwrProfilePriceExtendedRsp_t;

/* [R1] 9.6.5.4  GetOverallSchedulePriceResponse Command */
typedef PACKED_STRUCT zclPwrProfile_GetOverallSchedulePriceRsp_tag 
{
   afAddrInfo_t                       addrInfo;
   uint8_t                            zclTransactionId;
   zclCmdPwrProfile_GetPriceInfRsp_t  cmdFrame;
} zclPwrProfile_GetOverallSchedulePriceRsp_t;


typedef PACKED_STRUCT zclCmdPwrProfile_EnegyPhsScheduledTime_tag 
{
  uint8_t   energyPhaseId;
  uint16_t  scheduledTime;
}zclCmdPwrProfile_EnegyPhsScheduledTime_t; 

/* payload format for EnergyPhasesScheduleNotification,EnergyPhasesScheduleStateResponse
   and EnergyPhasesScheduleResponse Commands */
typedef PACKED_STRUCT zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_tag 
{
  uint8_t                                   pwrProfileId;
  uint8_t                                   numOfScheduledPhases;
  zclCmdPwrProfile_EnegyPhsScheduledTime_t  energyPhsScheduledTime[1];
}zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_t; 

/* [R1] 9.6.5.8  EnergyPhasesScheduleNotification Command  or  9.6.5.13  EnergyPhasesScheduleResponse Command */
typedef PACKED_STRUCT zclPwrProfile_EnergyPhasesScheduleRspOrNotif_tag 
{
   afAddrInfo_t                                   addrInfo;
   uint8_t                                        zclTransactionId;
   zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_t   cmdFrame;
} zclPwrProfile_EnergyPhasesScheduleRspOrNotif_t;


/* [R1] 9.6.5.14 PowerProfileScheduleConstraintsRequest Command */
typedef PACKED_STRUCT zclPwrProfile_PwrProfileScheduleConstraintsReq_tag 
{
   afAddrInfo_t                     addrInfo;
   uint8_t                          zclTransactionId;
   zclCmdPwrProfile_PwrProfileReq_t cmdFrame;
} zclPwrProfile_PwrProfileScheduleConstraintsReq_t;

/* [R1] 9.6.5.15  EnergyPhasesScheduleStateRequest Command */
typedef PACKED_STRUCT zclPwrProfile_EnergyPhsScheduleStateReq_tag 
{
   afAddrInfo_t                     addrInfo;
   uint8_t                          zclTransactionId;
   zclCmdPwrProfile_PwrProfileReq_t cmdFrame;
} zclPwrProfile_EnergyPhsScheduleStateReq_t;

typedef PACKED_STRUCT zclPowerProfile_EnergyPhaseInf_tag {
  uint8_t  energyPhaseID;
  uint8_t  macroPhaseID;
  uint16_t expectedDuration;
  uint16_t peakPower;
  uint16_t energy;
  uint16_t maxActivationDelay;
} zclPwrProfile_EnergyPhaseInf_t;

/* payload format for PowerProfileNotification and PowerProfileResponse commands*/
typedef PACKED_STRUCT zclCmdPwrProfile_PwrProfileRspOrNotifification_tag {
  uint8_t                           totalProfileNum;
  uint8_t                           powerProfileID;
  uint8_t                           numOfTransferredPhases;
  zclPwrProfile_EnergyPhaseInf_t    energyPhaseInf[1];
} zclCmdPwrProfile_PwrProfileRspOrNotification_t;

typedef PACKED_STRUCT zclPwrProfileInf_tag
{
  uint8_t                           pwrProfileId;
  uint8_t                           numOfTransferredPhases;
  zclPwrProfile_EnergyPhaseInf_t    energyPhaseInf[gPwrProfile_MaxEnergyPhaseInf_d];
  uint8_t                           currentEnergyPhaseId;
  bool_t                            pwrProfileRemoteControl;
  uint8_t                           pwrProfileState;
  uint16_t                          currentDuration;
}zclPwrProfileInf_t;


typedef PACKED_STRUCT zclPwrProfileClientInf_tag
{
  bool_t						slotState;	 /*FALSE -  slot is free, FALSE - otherwise*/
  zbNwkAddr_t                   aNwkAddr;
  bool_t                        remoteControlInf;
  uint8_t                       multipleSchedulingInf; /* 0xFF -  don't have information, 0x01 =True, 0x00 -False*/
  uint8_t                       currentEnergyId;
  zclPwrProfileInf_t            pwrProfileInf[gPwrProfileClient_MaxPwrProfileInfSupported_d];
}zclPwrProfileClientInf_t;


/* [R1] 9.6.6.1 PowerProfileNotification Command and 9.6.6.2 PowerProfileResponse Command */
typedef PACKED_STRUCT zclPwrProfile_PwrProfileRspOrNotification_tag
{
  afAddrInfo_t                        addrInfo;
  uint8_t                             zclTransactionId;
  zclCmdPwrProfile_PwrProfileRspOrNotification_t  cmdFrame;
} zclPwrProfile_PwrProfileRspOrNotification_t;


typedef PACKED_STRUCT zclPwrProfile_PwrProfileRecord_tag
{
  uint8_t pwrProfileId;
  uint8_t energyPhaseId;
  bool_t  remoteControl;
  uint8_t pwrProfileState;
}zclPwrProfile_PwrProfileRecord_t;


/* payload format for PowerProfileStateResponse and PowerProfileStateNotification commands*/
typedef PACKED_STRUCT zclCmdPwrProfile_PwrProfileStateRspOrNotification_tag {
  uint8_t                           pwrProfileCount;
  zclPwrProfile_PwrProfileRecord_t  pwrProfileRecord[1];
} zclCmdPwrProfile_PwrProfileStateRspOrNotification_t;

/* [R1] 9.6.6.3 PowerProfileStateResponse Command and  9.6.6.5 PowerProfileStateNotification Command */
typedef PACKED_STRUCT zclPwrProfile_PwrProfileStateRspOrNotification_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdPwrProfile_PwrProfileStateRspOrNotification_t cmdFrame;
} zclPwrProfile_PwrProfileStateRspOrNotification_t;


/* Power Profile State */
enum{
  gPwrProfileState_PwrProfileIdle_c = 0,            /* The Power Profile is not defined in its parameters */
  gPwrProfileState_PwrProfileProgrammed_c,          /* The Power Profile is defined in its parameters but without a scheduled time reference */
  gPwrProfileState_Reserved_c,                      /* Reserved- 0x02 */
  gPwrProfileState_EnergyPhsRunning_c,              /* An energy phase is running */
  gPwrProfileState_EnergyPhsPaused_c,               /* The current energy phase is paused */
  gPwrProfileState_EnergyPhsWaitingToStart_c,       /* The Power Profile is in between two energy phases */
  gPwrProfileState_EnergyPhsWaitingPaused_c,        /* The Power Profile is set to Pause when being in the ENERGY_PHASE_WAITING_TO_START state*/
  gPwrProfileState_PwrProfileEnded_c,               /* The whole Power profile is terminated */
  gPwrProfileState_Reserved2_c                      /* Reserved 0x08-0xff */
};

#if (gZclClusterOptionals_d)
/* [R1] 9.6.6.4 GetPowerProfilePrice Command */
typedef PACKED_STRUCT zclPwrProfile_GetPwrProfilePrice_tag
{
  afAddrInfo_t                     addrInfo;
  uint8_t                          zclTransactionId;
  zclCmdPwrProfile_PwrProfileReq_t cmdFrame;
} zclPwrProfile_GetPwrProfilePrice_t;
#endif

#if (gZclClusterOptionals_d)
/* [R1] 9.6.6.6 GetOverallSchedulePrice Command */
typedef PACKED_STRUCT zclPwrProfile_GetOverallSchedulePrice_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
} zclPwrProfile_GetOverallSchedulePrice_t;
#endif

/* [R1] 9.6.6.7 EnergyPhasesScheduleRequest Command */
typedef PACKED_STRUCT zclPwrProfile_EnergyPhsScheduleReq_tag
{
  afAddrInfo_t                     addrInfo;
  uint8_t                          zclTransactionId;
  zclCmdPwrProfile_PwrProfileReq_t cmdFrame;
} zclPwrProfile_EnergyPhsScheduleReq_t;

/* [R1] 9.6.6.8 EnergyPhasesScheduleStateResponse Command and 9.6.6.9 EnergyPhasesScheduleStateNotification Command*/
typedef PACKED_STRUCT zclPwrProfile_EnergyPhsScheduleStateRsp_tag 
{
   afAddrInfo_t                                        addrInfo;
   uint8_t                                             zclTransactionId;
   zclCmdPwrProfile_EnergyPhasesScheduleRspOrNotif_t   cmdFrame;
} zclPwrProfile_EnergyPhsScheduleStateRspOrNotif_t;

/* payload format for PowerProfileScheduleConstraintsNotification and PowerProfileScheduleConstraintsResponse Commands */
typedef PACKED_STRUCT zclCmdPwrProfile_PwrProfileScheduleConstraintsRspOrNotif_tag
{
  uint8_t   powerProfileId;
  uint16_t  startAfter;
  uint16_t  stopBefore;
}zclCmdPwrProfile_PwrProfileScheduleConstraintsRspOrNotif_t;

/* [R1] 9.6.6.10 PowerProfileScheduleConstraintsNotification Command and 9.6.6.11 PowerProfileScheduleConstraintsResponseCommand */
typedef PACKED_STRUCT zclPwrProfile_PwrProfileScheduleConstraintsRspOrNotif_tag
{
   afAddrInfo_t                                             addrInfo;
   uint8_t                                                  zclTransactionId;
   zclCmdPwrProfile_PwrProfileScheduleConstraintsRspOrNotif_t    cmdFrame;
} zclPwrProfile_PwrProfileScheduleConstraintsRspOrNotif_t;


/* payload format for GetPowerProfilePriceExtended Command */
typedef PACKED_STRUCT zclCmdPwrProfile_GetPwrProfilePriceExtended_tag
{
  uint8_t   options;
  uint8_t   pwrProfileId;
  uint16_t  pwrProfileStartTime;
}zclCmdPwrProfile_GetPwrProfilePriceExtended_t;

/* GetPowerProfilePriceExtended Command */
typedef PACKED_STRUCT zclPwrProfile_GetPwrProfilePriceExtended_tag
{
   afAddrInfo_t                                     addrInfo;
   uint8_t                                          zclTransactionId;
   zclCmdPwrProfile_GetPwrProfilePriceExtended_t    cmdFrame;
}zclPwrProfile_GetPwrProfilePriceExtended_t;


/******************************************
	EN50523 Appliance Control Cluster
  See Zigbee Home Automation profile 1.2(053520r29) Section 9.7
*******************************************/
/* Appliance control attribute set */
#if ( TRUE == gBigEndian_c )
/* Appliance functions attribute set */
#define gZclAttrApplianceControl_StartTimeId_c              0x0000 /* M - StartTime */
#define gZclAttrApplianceControl_FinishTimeId_c             0x0100 /* M - FinishTime */
#define gZclAttrApplianceControl_RemainingTimeId_c          0x0200 /* M - RemainingTime */
#else
/* Appliance functions attribute set */
#define gZclAttrApplianceControl_StartTimeId_c              0x0000 /* M - StartTime */
#define gZclAttrApplianceControl_FinishTimeId_c             0x0001 /* M - FinishTime */
#define gZclAttrApplianceControl_RemainingTimeId_c          0x0002 /* O - RemainingTime */
#endif /* #if ( TRUE == gBigEndian_c ) */


#define gZclApplianceControl_ApplianceFunctionsSet_c      0x00 /* Appliance Functions Set */

/* Appliance control cluster attributes */

/* Appliance functions attribute set */
#define gZclAttrIdApplianceControl_StartTimeId_c              0x00 /* M - StartTime */
#define gZclAttrIdApplianceControl_FinishTimeId_c             0x01 /* M - FinishTime */
#define gZclAttrIdApplianceControl_RemainingTimeId_c          0x02 /* O - RemainingTime */



/* Appliance control cluster commands */
/* [R1] 9.7.5 Server Commands Received  */
#define gZclCmdApplianceControl_ExecutionCommand_c            0x00   /* M - Execution of a Command */
#define gZclCmdApplianceControl_SignalState_c                 0x01   /* M - Signal State */
#define gZclCmdApplianceControl_WriteFunctions_c              0x02   /* O - Write Functions */
#define gZclCmdApplianceControl_OverloadPauseResume_c         0x03   /* O - Overload Pause Resume */
#define gZclCmdApplianceControl_OverloadPause_c               0x04   /* O - Overload Pause */
#define gZclCmdApplianceControl_OverloadWarning_c             0x05   /* O - Overload Warning */

/* [R1] 9.7.6 Server Commands Generated  */
#define gZclCmdApplianceControl_SignalStateResponse_c         0x00   /* M - Signal State Response */
#define gZclCmdApplianceControl_SignalStateNotification_c     0x01   /* M - Signal State Notification */

typedef uint8_t ApplianceStatus24_t[3];

typedef PACKED_STRUCT zclApplianceControlAttrsRAM_tag
{
  uint16_t               startTime[zclReportableCopies_c]; 
  uint16_t               finishTime[zclReportableCopies_c];
#if (gZclClusterOptionals_d)  
  uint16_t               remainingTime[zclReportableCopies_c];  
#endif  
} zclApplianceControlAttrsRAM_t;

#define gApplianceControlTimeValueMask_d 0x001F


typedef PACKED_STRUCT zclPairDeviceInf_tag
{
  zbNwkAddr_t  aDestAddress;
  uint8_t      endPoint;
}zclPairDeviceInf_t;


/* Appliance Status attribute */
typedef enum applianceControlStatus_tag
{
  gApplianceStatus_Reserved_c = 0x00,          /* Reserved */
  gApplianceStatus_Off_c,                      /* Appliance in off state */
  gApplianceStatus_StandBy_c,                  /* Appliance in stand-by */
  gApplianceStatus_Programmed_c,               /* Appliance already programmed */
  gApplianceStatus_ProgrammedWaitToStart_c,    /* Appliance already programmed and ready to start */
  gApplianceStatus_Running_c,                  /* Appliance is running */
  gApplianceStatus_Pause_c,                    /* Appliance is in pause */
  gApplianceStatus_EndProgrammed_c,            /* Appliance end programmed tasks */
  gApplianceStatus_Failure_c,                  /* Appliance is in a failure state */
  gApplianceStatus_ProgrammedInterrupted_c,    /* The appliance programmed tasks have been interrupted */
  gApplianceStatus_Idle_c,                     /* Appliance in idle state */
  gApplianceStatus_RinseHold_c,                /* Appliance rinse hold */
  gApplianceStatus_Service_c,                  /* Appliance in service state */
  gApplianceStatus_SuperFreezing_c,            /* Appliance in superfreezing state */
  gApplianceStatus_SuperCooling_c,             /* Appliance in supercooling state */
  gApplianceStatus_SuperHeating_c,             /* Appliance in superheating state */
  gApplianceStatus_Reserved2_c,                /* Reserved (0x10-0x3F) */
  gApplianceStatus_NonStandardized_c = 0x40,   /* Non Standardized (0x40-0x7F) */
  gApplianceStatus_Proprietary_c = 0x80,       /* Proprietary (0x80-0xFF) */
}applianceControlStatus_t;


/* Appliance Control - Remote Enable Flags attribute */
typedef PACKED_STRUCT zclApplCtrlRemoteEnableFlags_tag
{
 uint8_t   remoteEnableFlags      : 4; /* remote enable flags */
 uint8_t   deviceStatus2Structure : 4; /* device status 2 structure */
}zclApplCtrlRemoteEnableFlags_t;  

/*  Remote Enable Flags -> remoteEnableFlags, value list */
enum{
  gRemoteEnableFlags_Disabled_c = 0x0,                  /* Disabled */
  gRemoteEnableFlags_EnableRemoteEnergyControl_c = 0x1, /* Enable Remote and Energy Control */
  gRemoteEnableFlags_Reserved_c,                        /* Reserved (0x2-0x6) */
  gRemoteEnableFlags_TempLocked_c = 0x7,                /* Temporary Locked / Disabled */
  gRemoteEnableFlags_Reserved2_c,                       /* Reserved (0x8-0xE) */
  gRemoteEnableFlags_EnabledRemoteControl_c = 0xF,      /* Enabled Remote Control */
};

/*  Remote Enable Flags -> DeviceStatus2Structure, value list  */
enum {
  gDeviceStatus2Structure_Proprietary0_c = 0x0,      /* Proprietary */
  gDeviceStatus2Structure_Proprietary1_c,            /* Proprietary */
  gDeviceStatus2Structure_IrisSymptomCode_c,         /* Iris Symptom code */
  gDeviceStatus2Structure_Reserved_c                 /* Reserved (0x3-0xF) */
};

/* Start Time Attribute */
typedef PACKED_STRUCT zclApplCtrlTimeEncoding_tag
{
 unsigned   minutesRanging      : 6;  /* minutes ranging from 0 to 59 */
 unsigned   timeEncoding        : 2;  /* time encoding */
 unsigned   hoursRanging        : 8;  /* hour ranging from 0 to 255 (relative encoding) or 0 to 23 (absolute encoding)*/
}zclApplCtrlTimeEncoding_t;  

#define gApplCtrlTimeEncoding_MinutesMinValue_c      0    /* Minimum value of Minutes field */
#define gApplCtrlTimeEncoding_MinutesMaxValue_c      59   /* Maximum value of Minutes field */

#define gApplCtrlTimeEncoding_HoursMinValue_c        0    /* Minimum value of Hours field */
#define gApplCtrlTimeEncoding_HoursMaxAbsValue_c     23   /* Maximum value of Hours field (Absolute encoding) */
#define gApplCtrlTimeEncoding_HoursMaxRelValue_c     255  /* Maximum value of Hours field (Relative encoding) */


/* time encoding*/
enum{
  gTimeEncoding_Relative = 0x0, /* relative */
  gTimeEncoding_Absolute = 0x1, /* absolute */
  gTimeEncoding_Reserved        /* reserved 0x2, 0x3*/
};

/* payload format for ExecutionCommand Command */
typedef PACKED_STRUCT zclCmdApplCtrl_ExecutionCommand_tag 
{
  uint8_t  commandId;
}zclCmdApplCtrl_ExecutionCommand_t;

/* 9.7.5.1 Execution of a Command*/
typedef PACKED_STRUCT zclApplCtrl_ExecutionCommand_tag 
{
  afAddrInfo_t                         addrInfo;
  uint8_t                              zclTransactionId;
  zclCmdApplCtrl_ExecutionCommand_t    cmdFrame;
}zclApplCtrl_ExecutionCommand_t;

/* command identification field*/
enum{
  gApplCtrl_CommandId_Reserved = 0x00,      /* reserved */ 
  gApplCtrl_CommandId_Start,                /* start appliance cycle */
  gApplCtrl_CommandId_Stop,                 /* stop appliance cycle */
  gApplCtrl_CommandId_Pause,                /* pause appliance cycle */
  gApplCtrl_CommandId_StartSuperFreezing,   /* start superfreezing cycle */
  gApplCtrl_CommandId_StopSuperFreezing,    /* stop superfreezing cycle */
  gApplCtrl_CommandId_StartSuperCooling,    /* start supercooling cycle */
  gApplCtrl_CommandId_StopSuperCooling,     /* stop supercooling cycle */  
  gApplCtrl_CommandId_DisableGas,           /* disable gas */
  gApplCtrl_CommandId_EnableGas,            /* enable gas */
  gApplCtrl_CommandId_Standardized,         /* stardardized (TBD) 0x0A..0x3F */
  gApplCtrl_CommandId_NonStandardized,      /* non stardardized  0x40..0x7F */  
  gApplCtrl_CommandId_Proprietary           /* proprietary */  
};    

/* [R1] 9.7.5.2 SignalState Command, 9.7.5.5 OverloadPause Command and 9.7.5.4 OverloadPauseResume Command */
typedef PACKED_STRUCT zclApplCtrl_CommandWithNoPayload_tag 
{
  afAddrInfo_t                         addrInfo;
  uint8_t                              zclTransactionId;
}zclApplCtrl_CommandWithNoPayload_t;

/* payload format for WriteFunctions Command */
typedef PACKED_STRUCT zclCmdApplCtrl_WriteFunction_tag 
{
  uint16_t functionId;
  uint8_t  functionDataType;
  uint8_t  functionData[1];    //variable octets
}zclCmdApplCtrl_WriteFunction_t;

/* [R1] 9.7.5.3 WriteFunctions Command */
typedef PACKED_STRUCT zclApplCtrl_WriteFunction_tag 
{
  afAddrInfo_t                         addrInfo;
  uint8_t                              zclTransactionId;
  zclCmdApplCtrl_WriteFunction_t       cmdFrame;
}zclApplCtrl_WriteFunction_t;

/* payload format for Overload Warning Command */
typedef PACKED_STRUCT zclCmdApplCtrl_OverloadWarning_tag 
{
   uint8_t warningEvent;
}zclCmdApplCtrl_OverloadWarning_t;

enum{
  gApplCtrl_Warning1 = 0x00,   /* warning 1: overall power above "available power" level */
  gApplCtrl_Warning2,          /* warning 2: overall power above "power threshold" level */
  gApplCtrl_Warning3,          /* warning 3: overall power back below the  "available power" level */ 
  gApplCtrl_Warning4,          /* warning 4: overall power back below the  "power threshold" level */ 
  gApplCtrl_Warning5           /* warning 5: overall power will be potentially above "available power" level if the appliance starts*/
};

/* [R1] 9.7.5.8 OverloadWarning Command */
typedef PACKED_STRUCT zclApplCtrl_OverloadWarning_tag 
{
  afAddrInfo_t                         addrInfo;
  uint8_t                              zclTransactionId;
  zclCmdApplCtrl_OverloadWarning_t     cmdFrame;
}zclApplCtrl_OverloadWarning_t;

/* payload format for SignalStateResponse and SignalStateNotification commands */
typedef PACKED_STRUCT zclCmdApplCtrl_SignalStateRspOrNotif_tag 
{
   uint8_t applianceStatus;                /* appliance status */
   uint8_t RemoteEnFlagsDevStatus2;        /* remote enable flags and device status 2 */
   ApplianceStatus24_t applianceStatus2;   /* appliance status 2*/ 
}zclCmdApplCtrl_SignalStateRspOrNotif_t;

/* [R1] 9.7.6.1 SignalStateResponse Command and 9.7.6.2 SignalStateNotification Command */
typedef PACKED_STRUCT zclApplCtrl_SignalStateRsp_tag 
{
  afAddrInfo_t                                addrInfo;
  uint8_t                                     zclTransactionId;
  zclCmdApplCtrl_SignalStateRspOrNotif_t      cmdFrame;
}zclApplCtrl_SignalStateRspOrNotif_t;


typedef PACKED_STRUCT zclApplCtrl_OverloadCommand_tag
{
  afAddrInfo_t                              addrInfo;
  uint8_t                                   zclTransactionId;
  uint8_t 									commandId;
  uint8_t									cmdData;	
}zclApplCtrl_OverloadCommand_t;

#if gZclEnablePwrProfileClusterServer_d || gZclEnablePwrProfileClusterClient_d
#if gZclEnablePartition_d
/* used for partition cluster */
#if ( TRUE == gBigEndian_c )
#define gMaxPwrProfilePartitionBuffer 0x0015
#else
#define gMaxPwrProfilePartitionBuffer 0x1500
#endif
typedef PACKED_STRUCT PwrPRofilePartitionTxFrameBuffer_tag {
  uint8_t *nextBuffer;
  uint32_t bufferLen;
  union 
  {
    uint8_t data[1];
    zclFrame_t zclHeader;
  } payload;
}PwrPRofilePartitionTxFrameBuffer_t;


typedef PACKED_STRUCT PwrProfilePartitionRxFrameBuffer_tag {
  bool_t    framePending;
  uint8_t   zclTransactionId;
  uint32_t  totalLen;
  uint8_t   commandId;
  uint8_t   *data;
} PwrProfilePartitionRxFrameBuffer_t;

typedef PACKED_STRUCT PwrProfile_TransferDataPtr_tag
{
  afAddrInfo_t      addrInfo;
  uint8_t           zclTransactionId;
  uint8_t           totalLength;
  uint8_t           data[1];  
}PwrProfile_TransferDataPtr_t;

#endif /* gZclEnablePartition_d */
#endif /* gZclEnablePwrProfileClusterServer_d || gZclEnablePwrProfileClusterClient_d */

/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/
/*!
 * @fn 		zbStatus_t ZCL_PowerProfileClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the PowerProfile Cluster Server. 
 *
 */
zbStatus_t ZCL_PowerProfileClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_PowerProfileClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the PowerProfile Cluster Client. 
 *
 */
zbStatus_t ZCL_PowerProfileClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_ApplianceControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ApplianceControl Cluster Server. 
 *
 */
zbStatus_t ZCL_ApplianceControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_ApplianceControlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t ZCL_ApplianceControlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);


#if gZclEnablePwrProfileClusterClient_d
/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileReq(zclPwrProfile_PwrProfileReq_t *pReq)
 *
 * @brief	Sends over-the-air a PowerProfileRequest from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_PwrProfileReq(zclPwrProfile_PwrProfileReq_t *pReq);  
/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileStateReq(zclPwrProfile_PwrProfileStateReq_t *pReq)
 *
 * @brief	Sends over-the-air a PowerProfileStateRequest from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_PwrProfileStateReq(zclPwrProfile_PwrProfileStateReq_t *pReq);  
/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileScheduleConstraintsReq(zclPwrProfile_PwrProfileScheduleConstraintsReq_t *pReq) 
 *
 * @brief	Sends over-the-air a PowerProfileScheduleConstraints Request from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_PwrProfileScheduleConstraintsReq(zclPwrProfile_PwrProfileScheduleConstraintsReq_t *pReq);  
/*!
 * @fn 		zbStatus_t zclPwrProfile_EnergyPhaseScheduleRspOrNotification(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId)
 *
 * @brief	Sends over-the-air a EnergyPhaseScheduleResponse/EnergyPhaseScheduleNotification Command from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_EnergyPhaseScheduleRspOrNotification(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId);
/*!
 * @fn 		zbStatus_t zclPwrProfile_EnergyPhsScheduleStateReq(zclPwrProfile_EnergyPhsScheduleStateReq_t *pReq) 
 *
 * @brief	Sends over-the-air a EnergyPhaseScheduleState Request from the PowerProfile Cluster Client. 
 *
 */
zbStatus_t zclPwrProfile_EnergyPhsScheduleStateReq(zclPwrProfile_EnergyPhsScheduleStateReq_t *pReq);  
#endif

#if gZclEnablePwrProfileClusterServer_d
/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileRspOrNotification(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId)
 *
 * @brief	Sends over-the-air a PowerProfileResponse/PowerProfileNotification command from the PowerProfile Cluster Server. 
 *
 */
zbStatus_t zclPwrProfile_PwrProfileRspOrNotification(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId);
/*!
 * @fn 		zbStatus_t zclPwrProfile_PwrProfileStateRspOrNotification(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t commandId)
 *
 * @brief	Sends over-the-air a PowerProfileStateResponse/PowerProfileStateNotification command from the PowerProfile Cluster Server. 
 *
 */  
zbStatus_t zclPwrProfile_PwrProfileStateRspOrNotification(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t commandId);
/*!
 * @fn 		zbStatus_t zclPwrProfile_EnergyPhsScheduleStateRspOrNotif(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId)
 *
 * @brief	Sends over-the-air a EnergyPhaseScheduleStateResponse/EnergyPhaseScheduleStateNotification command from the PowerProfile Cluster Server. 
 *
 */
zbStatus_t zclPwrProfile_EnergyPhsScheduleStateRspOrNotif(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t pwrProfileId, uint8_t commandId);
/*!
 * @fn 		zbStatus_t zclPwrProfile_EnergyPhsScheduleReq(zclPwrProfile_EnergyPhsScheduleReq_t *pReq)
 *
 * @brief	Sends over-the-air a EnergyPhaseScheduleRequest command from the PowerProfile Cluster Server. 
 *
 */
zbStatus_t zclPwrProfile_EnergyPhsScheduleReq(zclPwrProfile_EnergyPhsScheduleReq_t *pReq);
#if (gZclClusterOptionals_d)
/*!
 * @fn 		zbStatus_t ZtcPwrProfileServer_SetCurrentPwrProfileInformation(zclPwrProfileInf_t* pPwrProfileInf)
 *
 * @brief	Set current power profile information
 *
 */
zbStatus_t ZtcPwrProfileServer_SetCurrentPwrProfileInformation(zclPwrProfileInf_t* pPwrProfileInf);
/*!
 * @fn 		zbStatus_t zclPwrProfile_GetPwrProfilePrice(zclPwrProfile_GetPwrProfilePrice_t *pReq)
 *
 * @brief	Sends over-the-air a GetPowerProfilePriceRequest command from the PowerProfile Cluster Server. 
 *
 */ 
zbStatus_t zclPwrProfile_GetPwrProfilePrice(zclPwrProfile_GetPwrProfilePrice_t *pReq);
/*!
 * @fn 		zbStatus_t zclPwrProfile_GetOverallSchedulePrice(zclPwrProfile_GetOverallSchedulePrice_t *pReq)
 *
 * @brief	Sends over-the-air a GetOverallSchedulePriceRequest command from the PowerProfile Cluster Server. 
 *
 */
zbStatus_t zclPwrProfile_GetOverallSchedulePrice(zclPwrProfile_GetOverallSchedulePrice_t *pReq);
/*!
 * @fn 		zbStatus_t zclPwrProfile_GetPwrProfilePriceExtended(zclPwrProfile_GetPwrProfilePriceExtended_t *pReq)
 *
 * @brief	Sends over-the-air a GetPowerProfilePriceExtended command from the PowerProfile Cluster Server. 
 *
 */
zbStatus_t zclPwrProfile_GetPwrProfilePriceExtended( zclPwrProfile_GetPwrProfilePriceExtended_t *pReq);
#endif //gZclClusterOptionals_d
/*!
 * @fn 		void PwrProfile_InitPwrProfileServer(void)
 *
 * @brief	Init Power Profile Server
 *
 */
void PwrProfile_InitPwrProfileServer(uint8_t endpoint);
#endif
/*!
 * @fn 		zbStatus_t zclApplianceCtrl_ExecutionCommand(zclApplCtrl_ExecutionCommand_t *pReq) 
 *
 * @brief	Sends over-the-air an ExecutionOfCommand request from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_ExecutionCommand(zclApplCtrl_ExecutionCommand_t *pReq);
/*!
 * @fn 		zbStatus_t zclApplianceCtrl_SignalState(zclApplCtrl_CommandWithNoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air an SignalState request from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_SignalState(zclApplCtrl_CommandWithNoPayload_t *pReq); 
/*!
 * @fn 		zbStatus_t zclApplianceCtrl_WriteFunctions(zclApplCtrl_WriteFunction_t *pReq) 
 *
 * @brief	Sends over-the-air a WriteFunctions command from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_WriteFunctions(zclApplCtrl_WriteFunction_t *pReq); 
/*!
 * @fn 		zbStatus_t zclApplianceCtrl_OverloadPauseResume(zclApplCtrl_CommandWithNoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a OverloadPauseResume command from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_OverloadPauseResume(zclApplCtrl_CommandWithNoPayload_t *pReq);
/*!
 * @fn 		zbStatus_t zclApplianceCtrl_OverloadPause(zclApplCtrl_CommandWithNoPayload_t *pReq) 
 *
 * @brief	Sends over-the-air a OverloadPause command from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_OverloadPause(zclApplCtrl_CommandWithNoPayload_t *pReq);
/*!
 * @fn 		zbStatus_t zclApplianceCtrl_OverloadWarning(zclApplCtrl_OverloadWarning_t *pReq) 
 *
 * @brief	Sends over-the-air a OverloadWarning command from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_OverloadWarning(zclApplCtrl_OverloadWarning_t *pReq);
/*!
 * @fn 		zbStatus_t zclApplianceCtrl_SignalStateRspOrNotif(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t commandId)  
 *
 * @brief	Sends over-the-air an SignalStateResponse/SignalState Notification from the ApplianceControl Cluster Server. 
 *
 */
zbStatus_t zclApplianceCtrl_SignalStateRspOrNotif(afAddrInfo_t addrInfo, uint8_t transactionId, uint8_t commandId);
/*!
 * @fn 		zbStatus_t ApplianceControlProcessStateMachine(uint8_t applianceControlStatus)
 *
 * @brief	Process Appliance Control events 
 *
 */
zbStatus_t ApplianceControlProcessStateMachine(uint8_t applianceControlStatus);
/*!
 * @fn 		zbStatus_t zclApplianceCtrl_OverloadCommand(zclApplCtrl_OverloadCommand_t *pReq)
 *
 * @brief	Sends over-the-air an Overload Command(OverloadWarning/OverloadPause/OverloadPauseResume) from the ApplianceControl Cluster Client. 
 *
 */
zbStatus_t zclApplianceCtrl_OverloadCommand(zclApplCtrl_OverloadCommand_t *pReq);

#if gZclEnablePwrProfileClusterServer_d
/*!
 * @fn 		zbStatus_t Zcl_PowerProfile_UnsolicitedCommandHandler(uint8_t commandId, uint32_t data)
 *
 * @brief   Process Power Profile Unsolicited command received from external/internal app.
 *
 */
zbStatus_t Zcl_PowerProfile_UnsolicitedCommandHandler(uint8_t commandId,uint32_t data);
/*!
 * @fn 		uint16_t PwrProfile_GetProfileExpectedDuration(void)
 *
 * @brief	Get Power Profile Expected Duration for the current profile
 *
 */
uint16_t PwrProfile_GetProfileExpectedDuration(void);
/*!
 * @fn 		uint16_t PwrProfile_UpdateProfileDuration(uint8_t stateDuration)
 *
 * @brief   Update Power Profile Duration according with the device running state
 *
 */
uint16_t PwrProfile_UpdateProfileDuration(uint8_t stateDuration);
/*!
 * @fn 		void PwrProfile_ChangeEnergyPhaseInformation(bool_t increaseEnPhsInf) 
 *
 * @brief   Change default Configuration by increased or decreased energy phase information
 *
 */
void PwrProfile_ChangeEnergyPhaseInformation(bool_t increaseEnPhsInf);
#endif


#if (gZclEnablePwrProfileClusterServer_d || gZclEnablePwrProfileClusterClient_d) && gZclEnablePartitionPwrProfile_d
/*!
 * @fn 		void ZCL_PowerProfile_PartitionInit(void)
 *
 * @brief   Init PowerProfile - Partition configuration. In case of messages that will not fit into a single Zigbee payload, 
 *			the Partition Cluster will be used (managed by the application). This function will register the PowerProfile to Partition Cluster.
 *
 */
void ZCL_PowerProfile_PartitionInit(void);
#endif

#if gZclEnablePwrProfileClusterClient_d
/*!
 * @fn 		void zclPwrProfile_InitClient(void)
 *
 * @brief	Init Power Profile Client Cluster 
 *
 */
void zclPwrProfile_InitClient(uint8_t endpoint);
/*!
 * @fn 		void zclPwrProfileClient_StoreRemoteServerMultipleSchedulingInf(zbNwkAddr_t aNwkAddr, uint16_t attrId, uint8_t attrValue)
 *
 * @brief	Update client table. Store multiple schedule and remote control server informations 
 *
 */
void zclPwrProfileClient_StoreRemoteServerMultipleSchedulingInf(zbNwkAddr_t aNwkAddr, uint16_t attrId, uint8_t attrValue);
/*!
 * @fn 		void PwrProfileClient_StoreRemoteServerEnPhsInf(uint8_t index,zclCmdPwrProfile_PwrProfileRspOrNotification_t *pData)
 *
 * @brief	Update client table. Store the server energy phase inf.
 *
 */
void PwrProfileClient_StoreRemoteServerEnPhsInf(uint8_t index,zclCmdPwrProfile_PwrProfileRspOrNotification_t *pData);
/*!
 * @fn 		zclCmdPwrProfile_GetPriceInfRsp_t PwrProfileClient_GetPrice(uint8_t serverIndex, uint8_t powerProfileID)
 *
 * @brief	Based on duration set a demo price value.
 *
 */
zclCmdPwrProfile_GetPriceInfRsp_t PwrProfileClient_GetPrice(uint8_t serverIndex, uint8_t powerProfileID);
/*!
 * @fn 		void PwrProfileClient_StoreRemoteServerStateInf(uint8_t index, zclCmdPwrProfile_PwrProfileStateRspOrNotification_t *pData)
 *
 * @brief	Update client table. Store the current server state.
 *
 */
void PwrProfileClient_StoreRemoteServerStateInf(uint8_t index, zclCmdPwrProfile_PwrProfileStateRspOrNotification_t *pData);
#endif
#ifdef __cplusplus
}
#endif

#endif
