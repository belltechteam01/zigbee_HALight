/******************************************************************************
* SEProfile.h
*
* Types, definitions and prototypes for the ZigBee SE Profile.
*
* Copyright (c) 2006, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/
/*
Acronyms:

DemandResponseLoadControl... DmndRspLdCtrl
LoadControl...LdCtrl
SimpleMetering...SmplMet
KeyEstablishmentSecuritySuite ...KeyEstabSecSuite
KeyEstablishment...KeyEstab
DeviceClass...DevCls
CriticalityLevel...CritLev
UtilityDefined...UtilDef
Criticality or Critical...Crit; 
Messaging or Message...Msg
CoolingTemperatureSetPointApplied... CoolTempSetPntApplied(HeatTempSetPntApplied)
Simple....Smpl;       Load...Ld;        Demand...Dmnd;          Device...Dev;
Level...Lev;          Response...Rsp;   Control.......Ctrl;     Received...Rcvd;
Event....Evt;         Rejected...Rjctd  Delivered...Dlvrd       Summation...Summ
Metering...Met        Profile...Prof    Partial...Prtl          Interval...Intrv
Undefined...Undef     Defined...Def     Consumption...Consmp    Current...Curr
Command...Cmd         Anonymous... Anon Confirmation...Conf;
Alternate..... Alt    Register....Rgstr Number....Num           Information...Info
*/

#ifndef _SEPROFILE_H
#define _SEPROFILE_H

#ifdef __cplusplus
    extern "C" {
#endif

/* header files needed by home automation */
#include "EmbeddedTypes.h"
#include "AfApsInterface.h"
#include "ZclOptions.h"
#include "ZCL.h"
#include "ZCLGeneral.h"
#include "ZclHVAC.h"
#include "ZCLSensing.h"
#include "ZCLSEKeyEstab.h"

/******************************************************************************
*******************************************************************************
* Public macros & type definitions
*******************************************************************************
******************************************************************************/
/*Include if Certicom ECC library is included*/
#define gEccIncluded_d TRUE
#define gFullEcc_d FALSE

/* Number of ESIs supported by the device */
#ifndef gMaxNoOfESISupported_c
#define gMaxNoOfESISupported_c 2
#endif

/* Interval in minutes between 2 Service Discovery procedures */
#ifndef gSE_ServiceDiscoveryInterval_c
#define gSE_ServiceDiscoveryInterval_c 15
#endif

/* Interval in seconds between 2 time synchronization requests. It is also used
   as a keep alive mechanism */
#ifndef gTimeSyncInterval_c
#define gTimeSyncInterval_c 300
#endif

/*Wait time set in terminate key establishment commands*/
#ifndef gKeyEstab_DefaultWaitTime_c
#define gKeyEstab_DefaultWaitTime_c 5
#endif 

#ifndef gKeyEstab_DefaultEphemeralDataGenerateTime_c
#define gKeyEstab_DefaultEphemeralDataGenerateTime_c 0x03
#endif

#ifndef gKeyEstab_DefaultConfirmKeyGenerateTime_c
#define gKeyEstab_DefaultConfirmKeyGenerateTime_c 0x03
#endif

#ifndef  gKeyEstab_ConfirmKeyMessageTimeout_c
#define gKeyEstab_ConfirmKeyMessageTimeout_c 3
#endif

#ifndef gKeyEstab_EphemeralDataMessageTimeout_c
#define gKeyEstab_EphemeralDataMessageTimeout_c 3
#endif

#ifndef gKeyEstab_MinimumTimeout_c
#define gKeyEstab_MinimumTimeout_c 0x05
#endif

/* The poll rate used by an end device during ECC procedure*/
#ifndef gKeyEstab_EndDevicePollRate_c
#define gKeyEstab_EndDevicePollRate_c 1000
#endif

/* Number of virtual networks supported by the Federated TC in an MDU environment */
#ifndef gNumOfVirtualHANs_c
#define gNumOfVirtualHANs_c 2
#endif 

/* Number of nodes of a virtual network in an MDU environment */
#ifndef gNumOfVirtualHANDevices_c
#define gNumOfVirtualHANDevices_c 4
#endif 

/* Number of entries in  Message Table*/
#ifndef gNumOfMsgTableEntry_c
#define gNumOfMsgTableEntry_c 2
#endif 

/* Number of entries in  DRLC event Table*/
#ifndef gNumOfEventsTableEntry_c
#define gNumOfEventsTableEntry_c 4 
#endif 

/* Number of entries stored on the DRLC Server */
#ifndef gNumOfServerEventsTableEntry_c
#define gNumOfServerEventsTableEntry_c 2
#endif

/*Size of ESP registation table */
#ifndef RegistrationTableEntries_c
#define RegistrationTableEntries_c 2
#endif 

/* Number of InterPan addr used for Price Update(for Non-SE devices) */
#ifndef gNumOfInterPanAddr_c
#define gNumOfInterPanAddr_c 1
#endif
/* Number of addr information used for Price Update(for Se devices) */
#ifndef gNumOfPriceAddr_c
#define gNumOfPriceAddr_c 4
#endif

/* Number of Prices stored in the Server (min. value is 5) */
#ifndef gNumofServerPrices_c
#define gNumofServerPrices_c 2
#endif

/* Number of Prices Handled by the Client(min. value is 2) */
#ifndef gNumofClientPrices_c
#define gNumofClientPrices_c 2
#endif

/* Number of Prices stored in the Server (min. value is 2) */
#ifndef gNumofServerBlockPeriods_c
#define gNumofServerBlockPeriods_c 2
#endif

/* Number of Prices Handled by the Client(min. value is 2) */
#ifndef gNumofClientBlockPeriods_c
#define gNumofClientBlockPeriods_c 2
#endif

/* Number of Price Matrix stored in the Server (min. value is 2) */
#ifndef gNumofServerPriceMatrix_c
#define gNumofServerPriceMatrix_c 2
#endif

/* Number of Price Matrix Handled by the Client(min. value is 2) */
#ifndef gNumofClientPriceMatrix_c
#define gNumofClientPriceMatrix_c 2
#endif

/* Number of Block Thresholds stored in the Server (min. value is 2) */
#ifndef gNumofServerBlockThresholds_c
#define gNumofServerBlockThresholds_c 2
#endif

/* Number of Block Thresholds Handled by the Client(min. value is 2) */
#ifndef gNumofClientBlockThresholds_c
#define gNumofClientBlockThresholds_c 2
#endif

/* Number of Tier Labels stored in the Server (min. value is 2) */
#ifndef gNumofServerTierLabels_c
#define gNumofServerTierLabels_c 2
#endif

/* Number of Tier Labels handled by the Client(min. value is 2) */
#ifndef gNumofClientTierLabels_c
#define gNumofClientTierLabels_c 2
#endif

/* Number of Conversion Factor stored in the Server (min. value is 2) */
#ifndef gNumofServerConversionFactors_c
#define gNumofServerConversionFactors_c 2
#endif

/* Number of Conversion Factor Handled by the Client(min. value is 2) */
#ifndef gNumofClientConversionFactors_c
#define gNumofClientConversionFactors_c 2
#endif

/* Number of CO2 Value stored in the Server (min. value is 3) */
#ifndef gNumofServerCO2Value_c
#define gNumofServerCO2Value_c 3
#endif

/* Number of CO2 Value Handled by the Client(min. value is 2) */
#ifndef gNumofClientCO2Value_c
#define gNumofClientCO2Value_c 2
#endif

/* Number of Calorific Value stored in the Server (min. value is 2) */
#ifndef gNumofServerCalorificValue_c
#define gNumofServerCalorificValue_c 2
#endif

/* Number of Calorific Value Handled by the Client(min. value is 2) */
#ifndef gNumofClientCalorificValue_c
#define gNumofClientCalorificValue_c 2
#endif

/* Number of Tariff Information stored in the Server (min. value is 2) */
#ifndef gNumofServerTariffInformation_c
#define gNumofServerTariffInformation_c 2
#endif

/* Number of Tariff Information stored on the Client (min. value is 2) */
#ifndef gNumofClientTariffInformation_c
#define gNumofClientTariffInformation_c 2
#endif

/* Number of Billing Periods stored in the Server (min. value is 2) */
#ifndef gNumofServerBillingPeriods_c
#define gNumofServerBillingPeriods_c 2
#endif

/* Number of Billing Periods Handled by the Client(min. value is 2) */
#ifndef gNumofClientBillingPeriods_c
#define gNumofClientBillingPeriods_c 2
#endif

/* Number of CPP Events stored in the Server*/
#ifndef gNumofServerCPPEvents_c
#define gNumofServerCPPEvents_c 2
#endif

/* Number of CPP Events Handled by the Client */
#ifndef gNumofClientCPPEvents_c
#define gNumofClientCPPEvents_c 2
#endif

/* Number of Consolidated Bill stored in the Server (min. value is 5) */
#ifndef gNumofServerConsolidatedBill_c
#define gNumofServerConsolidatedBill_c 5
#endif

/* Number of Consolidated Bill Handled by the Client(min. value is 2) */
#ifndef gNumofClientConsolidatedBill_c
#define gNumofClientConsolidatedBill_c 2
#endif

/* Number of Credit Payment events stored in the Server (min. value is 5) */
#ifndef gNumofServerCreditPayment_c
#define gNumofServerCreditPayment_c 5
#endif

/* Number of Credit Payment events Handled by the Client(min. value is 2) */
#ifndef gNumofClientCreditPayment_c
#define gNumofClientCreditPayment_c 2
#endif

/* Maximum length for messages stored in the memory*/
#ifndef gZclMaxRcvdMsgLength
#define gZclMaxRcvdMsgLength   32
#endif

/* When responding to a request via
the Inter-PAN SAP, the Publish Price command should be broadcast to the PAN of
the requester after a random delay between 0 and 0.5 seconds, to avoid a potential
broadcast storm of packets */
#ifndef gInterPanMaxJitterTime_c
#define gInterPanMaxJitterTime_c 500
#endif

/* Maximum incoming transfer size that that can be supported with fragmentation using SE(128 bytes)*/
#ifndef gSEMaxIncomingTransferSize
#define gSEMaxIncomingTransferSize  0x80,0x00  
#endif
                                                 
/* Fragmenation window size, can be 1 - 8 */
#ifndef gSEMaxWindowSize_c
#define gSEMaxWindowSize_c          1        
#endif

/*Fragmentation interframe delay, can be 1 - 255ms */
#ifndef gSEInterframeDelay_c
#define gSEInterframeDelay_c        50    
#endif

/* Fragmentation maximum length, can be 1 - ApsmeGetMaxAsduLength() */
#ifndef gSEMaxFragmentLength_c
#define gSEMaxFragmentLength_c      64   
#endif

/* Minimum value for the maximum reporting interval (in seconds)*/
#ifndef gSEMaxReportIntMin_c
#define gSEMaxReportIntMin_c      0x003C   
#endif



/* Smart Energy Profile - little endian (0x0109) */
#if gBigEndian_c
#define gSEProfId_c	0x0901      /* 0x0109, little endian order */
#else
#define gSEProfId_c	0x0109      
#endif

#define gSEProfIdb_c	0x09,0x01   /* in byte form */




/******************************************
device definitions 
*******************************************/

typedef uint16_t SEDevId_t;

#if gBigEndian_c
/* Generic */
#define gSERangeExtender_c            0x0800  /* Range Extender */

/* SE */
#define gSEEnergyServicePortal_c      0x0005 /* Energy Service Portal */
#define gSEMeter_c                    0x0105 /* Meter */
#define gInPremiseDisplay_c           0x0205 /* In-Premise Display */
#define gPCT_c                        0x0305 /* Programmable Controller thermostat */
#define gLdCtrlDevice_c               0x0405 /* Load Control */
#define gSmartAppliance_c             0x0605 /* Smart Appliance */
#define gPrepaymentTerminal_c         0x0705 /* Prepayment Terminal */

#else /* #if gBigEndian_c */
/* Generic */
#define gSERangeExtender_c            0x0008  /* Range Extender */

/* SE */
#define gSEEnergyServicePortal_c      0x0500 /* Energy Service Portal */
#define gSEMeter_c                    0x0501 /* Meter */
#define gInPremiseDisplay_c           0x0502 /* In-Premise Display */
#define gPCT_c                        0x0503 /* Programmable Controller thermostat */
#define gLdCtrlDevice_c               0x0504 /* Load Control */
#define gSmartAppliance_c             0x0506 /* Smart Appliance */
#define gPrepaymentTerminal_c         0x0507 /* Prepayment Terminal */
#endif /* #if gBigEndian_c */

/* devices in byte form */
#define gSERangeExtenderb_c            0x08,0x00  /* Range Extender */

/* SE */
#define gSEEnergyServicePortalb_c      0x00,0x05 /* Energy Service Portal */
#define gSEMeterb_c                    0x01,0x05 /* Meter */
#define gInPremiseDisplayb_c           0x02,0x05 /* In-Premise Display */
#define gPCTb_c                        0x03,0x05 /* Programmable Controller thermostat */
#define gLdCtrlDeviceb_c               0x04,0x05 /* Load Control  */
#define gSmartApplianceb_c             0x06,0x05 /* Smart Appliance */
#define gPrepaymentTerminalb_c         0x07,0x05 /* Prepayment Terminal */




/**********************************************
Stuff that should be added to ZCL.h
***********************************************/
#if gBigEndian_c
/*SE cluster ID's*/
/* cluster IDs (for endpoints... for now) */
#define gZclClusterPrice_c                       0x0007
#define gZclClusterDmndRspLdCtrl_c               0x0107
#define gZclClusterSmplMet_c                     0x0207
#define gZclClusterMsg_c                         0x0307
#define gZclClusterSETunneling_c                 0x0407
#define gZclClusterPrepayment_c                  0x0507
#define gZclClusterTouCalendar_c                 0x0707  
#define gZclClusterDevMgmt_c                     0x0807  
#define gZclClusterMDUPairing_c                  0x0A07  
#define gKeyEstabCluster_c                       0x0008
#else /* #if gBigEndian_c */
#define gZclClusterPrice_c                       0x0700
#define gZclClusterDmndRspLdCtrl_c               0x0701
#define gZclClusterSmplMet_c                     0x0702
#define gZclClusterMsg_c                         0x0703
#define gZclClusterSETunneling_c                 0x0704
#define gZclClusterPrepayment_c                  0x0705
#define gZclClusterTouCalendar_c                 0x0707  
#define gZclClusterDevMgmt_c                     0x0708      
#define gZclClusterMDUPairing_c                  0x070A      
#define gKeyEstabCluster_c                       0x0800
#endif /* #if gBigEndian_c  */

#define gaZclClusterPrice_c                       0x00,0x07
#define gaZclClusterDmndRspLdCtrl_c               0x01,0x07
#define gaZclClusterSmplMet_c                     0x02,0x07
#define gaZclClusterMsg_c                         0x03,0x07
#define gaZclClusterSETunneling_c                 0x04,0x07
#define gaZclClusterPrepayment_c                  0x05,0x07
#define gaZclClusterTouCalendar_c                 0x07,0x07  
#define gaZclClusterDevMgmt_c                     0x08,0x07 
#define gaZclClusterMDUPairing_c                  0x0A,0x07
#define gaKeyEstabCluster_c                       0x00,0x08


typedef PACKED_UNION addrInfoType_tag
{
  afAddrInfo_t addrInfo; 
  InterPanAddrInfo_t addrInterPan;
}addrInfoType_t;

typedef PACKED_STRUCT storedInterPanAddrInfo_tag
{
InterPanAddrInfo_t addrInfo;
uint8_t entryStatus;
}storedInterPanAddrInfo_t;

typedef PACKED_STRUCT storedPriceAddrInfo_tag
{
zbIeeeAddr_t iEEEaddr;
afAddrInfo_t addrInfo;
uint8_t      entryStatus;
}storedPriceAddrInfo_t;

typedef PACKED_STRUCT EspRegisterDevice_tag 
{
  zbIeeeAddr_t Addr;
  uint8_t InstallCodeLength;  /*Install code length includes CRC also*/
  uint8_t InstallCode[18]; /*18 bytes is the maximum length */
} EspRegisterDevice_t;

typedef PACKED_STRUCT EspDeRegisterDevice_tag 
{
  zbIeeeAddr_t Addr;
} EspDeRegisterDevice_t;

typedef PACKED_STRUCT EspReRegisterDevice_tag 
{
  zbIeeeAddr_t Addr;
} EspReRegisterDevice_t;

/*Install code length includes CRC also*/
#define gZclSEInstallCode48bit_c  (6+2)
#define gZclSEInstallCode64bit_c  (8+2)
#define gZclSEInstallCode96bit_c  (12+2)
#define gZclSEInstallCode128bit_c (16+2)

typedef zbStatus_t ESPRegisterDevFunc(EspRegisterDevice_t *devinfo);
typedef zbStatus_t ESPDeRegisterDevFunc(EspDeRegisterDevice_t *devinfo);

typedef PACKED_STRUCT zclSEGenericRxCmd_tag
{  
  void *pSECmd; /* point to received SE Cmd (point in first fragment ) */
  zbRxFragmentedHdr_t *pRxFrag; /* used when a fragmented message is received;  */
}zclSEGenericRxCmd_t;

typedef uint8_t MsgId_t[4];
typedef uint8_t SEEvtId_t[4];
typedef uint16_t LCDRDevCls_t;
typedef uint16_t Duration_t;
typedef uint16_t LCDRSetPoint_t;
typedef uint8_t IntrvForProfiling_t[3];
typedef uint8_t Pressure_t[6];
typedef uint8_t Demmand_t[3];
typedef uint8_t Profile_t[3];
typedef uint8_t Consmp[3];
typedef uint8_t Consmp32_t[3];
typedef uint8_t ProviderID_t[4];
typedef uint8_t Currency_t[2];
typedef uint8_t Price_t[4];
typedef uint8_t Signature_t[42];
typedef uint8_t Summ_t[6];
typedef uint8_t Charge_t[4];
typedef uint8_t BlockInf_t[4];
typedef uint8_t Bill_t[4];
typedef uint8_t CO2Value_t[4];
typedef uint8_t StandingCharge_t[4];
typedef uint8_t BlockThresholdMask_t[2];
typedef uint8_t BlockThresholdMultiplier_t[3];
typedef uint8_t BlockThresholdDivisor_t[3];
typedef uint8_t BlockThreshold_t[6];
typedef uint8_t Credit_t[4];
typedef uint8_t Duration16_t[2];
typedef uint8_t Duration24_t[3];
typedef uint8_t Multiplier24_t[3];
typedef uint8_t Divisor24_t[3];
typedef uint8_t CommodityCharge_t[4];
typedef uint8_t ConversionFactor_t[4];
typedef uint8_t CalorificValue_t[4];
typedef uint8_t Bitmap32_t[4];
typedef uint8_t Threshold_t[6];
typedef uint8_t ChangeCtrl_t[4];
typedef uint8_t Password_t[11];
typedef uint8_t ExtStatus_t[8];
typedef uint8_t SnapshotSchedule_t[3];
typedef uint8_t SnapshotCause_t[4];
typedef uint8_t Sample_t[3];
typedef uint8_t sePrepaymentTopUp_t[6];
typedef uint8_t sePrepaymentDebt_t[6];
typedef uint8_t uint48_t[6];
typedef uint8_t int48_t[6];

/* Max and min values for Command Fields Range for Load Control*/
#define gZclCmdDmndRspLdCtrl_MaxDurationInMinutes_c    1440  // One day 
#define gZclCmdDmndRspLdCtrl_MaxCritLev_c               0x0F
#define gZclCmdDmndRspLdCtrl_MaxTempSetPoint_c          0x7FFF
#define gZclCmdDmndRspLdCtrl_MaxTempOffset_c            0xFE
#define gZclCmdDmndRspLdCtrl_MinAverageLdAdjustmentPercentage_c            (-100)
#define gZclCmdDmndRspLdCtrl_MaxAverageLdAdjustmentPercentage_c            100
#define gZclCmdDmndRspLdCtrl_MaxDutyCycle_c            100


/* Optional values for Command Fields Range for Load Control*/
#define gZclCmdDmndRspLdCtrl_OptionalTempSetPoint_c          0x8000
#define gZclCmdDmndRspLdCtrl_OptionalTempOffset_c            0xFF
#define gZclCmdDmndRspLdCtrl_OptionalAverageLdAdjustmentPercentage_c    (-128)
#define gZclCmdDmndRspLdCtrl_OptionalDutyCycle_c            0xFF


/* Load Control Event command payload */
typedef PACKED_STRUCT zclCmdDmndRspLdCtrl_LdCtrlEvtReq_tag
{
  SEEvtId_t         IssuerEvtID;
  LCDRDevCls_t      DevGroupClass;
  uint8_t           UtilityGroup;
  ZCLTime_t         StartTime;
  Duration_t        DurationInMinutes;
  uint8_t           CritLev;
  uint8_t           CoolingTempOffset;
  uint8_t           HeatingTempOffset;
  LCDRSetPoint_t    CoolingTempSetPoint;
  LCDRSetPoint_t    HeatingTempSetPoint;
  int8_t            AverageLdAdjustmentPercentage;
  uint8_t           DutyCycle;
  uint8_t           EvtCtrl;
} zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t;

/* Load Control Event command request */
typedef PACKED_STRUCT zclDmndRspLdCtrl_LdCtrlEvtReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t cmdFrame;
} zclDmndRspLdCtrl_LdCtrlEvtReq_t;

typedef PACKED_STRUCT zclLdCtrl_EventsTableEntry_tag
{
  afAddrInfo_t addrInfo;    /*where to send the Reports Event Status for this event */
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t cmdFrame; /* keep the Event */
  uint8_t           CurrentStatus;      /* the current event status; can be modify by the user */ 
  uint8_t           NextStatus;         /* keep track of next status which depend of current status (almost all the time ) */
  uint8_t           IsSuccessiveEvent;  /* TRUE - is a "back to back" event; else FAlSE */ 
  ZCLTime_t         CancelTime; /* When the event should be canceled; 0Xffffffff meens event not canceled; apply the random time stop if needed */
  uint8_t           CancelCtrl;   /* to check if the cancelling on CancelTime use the randomization information from cmdFrame */
  LCDRDevCls_t      CancelClass; /* 0xffff meens not used */
  LCDRDevCls_t      NotSupersededClass; /* used for supressing events; 0xffff - not used yet; 0x0000 - event is seperseded */
  ZCLTime_t         SupersededTime;
  uint8_t           EntryStatus; /* the tabel entry is in use (0xFF) or not (0x00) */
}zclLdCtrl_EventsTableEntry_t;


/*Entry used or not */
#define gEntryUsed_c 0xff
#define gEntryNotUsed_c 0x00

/* Device Class Field BitMap/Encoding */
#define gZclSELCDR_HVAC_DevCls_c             0x01,0x00
#define gZclSELCDR_StripHeat_DevCls_c        0x02,0x00
#define gZclSELCDR_WaterHeater_DevCls_c      0x04,0x00
#define gZclSELCDR_PoolPump_DevCls_c         0x08,0x00
#define gZclSELCDR_SmartAppliance_DevCls_c   0x10,0x00
#define gZclSELCDR_IrrigationPump_DevCls_c   0x20,0x00
#define gZclSELCDR_ManagedLds_DevCls_c       0x40,0x00
#define gZclSELCDR_SmplMiscLds_DevCls_c      0x80,0x00
#define gZclSELCDR_ExteriorLighting_DevCls_c 0x00,0x01
#define gZclSELCDR_InteriorLighting_DevCls_c 0x00,0x02
#define gZclSELCDR_ElectricVehicle_DevCls_c  0x00,0x04
#define gZclSELCDR_Generation_DevCls_c       0x00,0x08

/*Criticality levels*/
#define gZclSECritLev_Reserved_c 0x00
#define gZclSECritLev_Green_c    0x01
#define gZclSECritLev_1_c        0x02
#define gZclSECritLev_2_c        0x03
#define gZclSECritLev_3_c        0x04
#define gZclSECritLev_4_c        0x05
#define gZclSECritLev_5_c        0x06
#define gZclSECritLev_Emergency_c     0x07
#define gZclSECritLev_PlannedOutage_c 0x08
#define gZclSECritLev_ServiceDisconnect 0x09
#define gZclSECritLev_UtilDef1_c 0x0a
#define gZclSECritLev_UtilDef2_c 0x0b
#define gZclSECritLev_UtilDef3_c 0x0c
#define gZclSECritLev_UtilDef4_c 0x0d
#define gZclSECritLev_UtilDef5_c 0x0e
#define gZclSECritLev_UtilDef6_c 0x0f

/* Cancel Load Control Event command payload */
typedef PACKED_STRUCT zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_tag
{
  SEEvtId_t         IssuerEvtID;
  LCDRDevCls_t      DevGroupClass;
  uint8_t           UtilityGroup; 
  uint8_t           CancelCtrl; 
  ZCLTime_t         EffectiveTime;  
} zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_t;

/* Cancel Load Control Event command request */
typedef PACKED_STRUCT zclDmndRspLdCtrl_CancelLdCtrlEvtReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_t cmdFrame;
} zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t;

/* Cancel Control */
#define gCancelCtrlOverideRandomization_c 0x00 /* A value of Zero (0) indicates that randomization is overridden and the Evt
                                                   should be terminated immediately at the Effective Time. */
#define gCancelCtrlEndRandomization_c 0x01 /* A value of One (1) indicates the Evt should end using randomization settings in
                                                 the original Evt */

/* Cancel All Load Control Events command payload */
typedef PACKED_STRUCT zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_tag
{
  uint8_t           CancelCtrl; 
} zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t;

/* Cancel All Load Control Events command request */
typedef PACKED_STRUCT zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t cmdFrame;
} zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t;


/*Generated Command IDs for the Demand Response
  and Load Control Client*/
#define gZclCmdDmndRspLdCtrl_ReportEvtStatus_c           0x00 /* M */
#define gZclCmdDmndRspLdCtrl_GetScheduledEvts_c          0x01 /* M */

/* Report Event Status command payload */
typedef PACKED_STRUCT zclCmdDmndRspLdCtrl_ReportEvtStatus_tag
{
  SEEvtId_t        IssuerEvtID;
  uint8_t          EvtStatus;
  ZCLTime_t        EvtStatusTime;
  uint8_t          CritLevApplied;
  LCDRSetPoint_t   CoolTempSetPntApplied;
  LCDRSetPoint_t   HeatTempSetPntApplied;
  int8_t           AverageLdAdjustmentPercentage;
  uint8_t          DutyCycle;
  uint8_t          EvtCtrl; 
  uint8_t          SignatureType;
  Signature_t      Signature;
} zclCmdDmndRspLdCtrl_ReportEvtStatus_t;

/* Report Event Status command request */
typedef PACKED_STRUCT zclDmndRspLdCtrl_ReportEvtStatus_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdDmndRspLdCtrl_ReportEvtStatus_t cmdFrame;
} zclDmndRspLdCtrl_ReportEvtStatus_t;

typedef PACKED_STRUCT zclCmdDmndRspLdCtrl_GetScheduledEvts_tag
{
ZCLTime_t        EvtStartTime;
uint8_t          NumOfEvts;
}zclCmdDmndRspLdCtrl_GetScheduledEvts_t;

typedef PACKED_STRUCT zclDmndRspLdCtrl_GetScheduledEvts_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdDmndRspLdCtrl_GetScheduledEvts_t cmdFrame;
} zclDmndRspLdCtrl_GetScheduledEvts_t;

#define gSELCDR_SignatureType_c 0x01

/* [R1] Table D-111 Signature Type */
#define gZclSE_SignatureNotUsed_c	0x00
#define gZclSE_SignatureECDSA_c		0x01


/* Event Status Field Values */
#define gSELCDR_LdCtrlEvtCode_CmdRcvd_c                     0x01
#define gSELCDR_LdCtrlEvtCode_Started_c                     0x02
#define gSELCDR_LdCtrlEvtCode_Completed_c                   0x03
#define gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptOut_c      0x04
#define gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptIn_c       0x05
#define gSELCDR_LdCtrlEvtCode_EvtCancelled_c                0x06
#define gSELCDR_LdCtrlEvtCode_EvtSuperseded_c                0x07
#define gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptOut_c   0x08
#define gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptIn_c    0x09
#define gSELCDR_LdCtrlEvtCode_EvtCompletedWithNoUser_c           0x0A
#define gSELCDR_LdCtrlEvtCode_EvtInvalidCancelCmdDefault_c       0xF8
#define gSELCDR_LdCtrlEvtCode_EvtInvalidEffectiveTime_c          0xF9
#define gSELCDR_LdCtrlEvtCode_EvtReserved_c                      0xFA
#define gSELCDR_LdCtrlEvtCode_EvtHadExpired_c                    0xFB
#define gSELCDR_LdCtrlEvtCode_EvtUndef_c                         0xFD
#define gSELCDR_LdCtrlEvtCode_LdCtrlEvtCmdRjctd_c                0xFE

/* Client cluster attributes */
#define gZclAttrSetDRLCClientSet_c                  0x00         

#define gZclAttrIdDRLCClientUtilityGroup_c          0x00         /*M*/
#define gZclAttrIdDRLCClientStartRandomizeMin_c     0x01         /*M*/
#define gZclAttrIdDRLCClientStopRandomizeMin_c      0x02         /*M*/
#define gZclAttrIdDRLCClientDevCls_c                0x03         /*M*/

/* Client full attribute ids*/
#if ( TRUE == gBigEndian_c )
#define gZclAttrDRLCClientUtilityGroup_c          0x0000 
#define gZclAttrDRLCClientStartRandomizeMin_c     0x0100
#define gZclAttrDRLCClientStopRandomizeMin_c      0x0200
#define gZclAttrDRLCClientDevCls_c                0x0300
#else
#define gZclAttrDRLCClientUtilityGroup_c          0x0000 
#define gZclAttrDRLCClientStartRandomizeMin_c     0x0001
#define gZclAttrDRLCClientStopRandomizeMin_c      0x0002
#define gZclAttrDRLCClientDevCls_c                0x0003
#endif /* #if ( TRUE == gBigEndian_c ) */


/**********************************************
Simple Metering cluster definitions
***********************************************/

/* Metering Client */

#define gZclAttrMetNotificationSet_c            0x00
#define gZclAttrMetMirrorCfgSet_c               0x01

/* Notification Set*/
#define gZclAttrIdMetNotifCtlFlags_c            0x00 
#define gZclAttrIdMetNotifFlags_c               0x01 
#define gZclAttrIdMetPriceNotifCtlFlags_c       0x02 
#define gZclAttrIdMetCalendarNotifCtlFlags_c    0x03 
#define gZclAttrIdMetPrePayNotifCtlFlags_c      0x04 
#define gZclAttrIdMetDevMgmtNotifCtlFlags_c     0x05 

/* Mirroring Configuration Set*/
#define gZclAttrIdMetChangeReportingProfile_c   0x00 

/* Metering Server */

#define gZclAttrMetReadInfoSet_c                    0x00
#define gZclAttrMetTOUInfoSet_c                     0x01
#define gZclAttrMetMeterStatusSet_c                 0x02
#define gZclAttrMetConsmpFormatSet_c                0x03
#define gZclAttrMetESPHistoricalConsmpSet_c         0x04
#define gZclAttrMetLdProfConfigSet_c                0x05
#define gZclAttrMetSupplyLimitSet_c                 0x06
#define gZclAttrMetBlockInfoDlvrdSet_c              0x07
#define gZclAttrMetAlarmsSet_c                      0x08
#define gZclAttrMetBlockInfoRcvdSet_c               0x09
#define gZclAttrMetBillingSet_c                     0x0A
#define gZclAttrMetReportStatusSet_c                0xFF

/* ReadInformationSet */
#define gZclAttrIdMetRISCurrSummDlvrd_c                       0x00 /*M*/
#define gZclAttrIdMetRISCurrSummRcvd_c                        0x01
#define gZclAttrIdMetRISCurrMaxDmndDlvrd_c                    0x02
#define gZclAttrIdMetRISCurrMaxDmndRcvd_c                     0x03
#define gZclAttrIdMetRISDFTSumm_c                             0x04
#define gZclAttrIdMetRISDailyFreezeTime_c                     0x05
#define gZclAttrIdMetRISPowerFactor_c                         0x06
#define gZclAttrIdMetRISReadingSnapShotTime_c                 0x07
#define gZclAttrIdMetRISCurrMaxDmndDlvrdTimer_c               0x08
#define gZclAttrIdMetRISCurrMaxDmndRcvdTime_c                 0x09
#define gZclAttrIdMetRISDefaultUpdatePeriod_c                 0x0A
#define gZclAttrIdMetRISFastPollUpdatePeriod_c                0x0B
#define gZclAttrIdMetRISCurrBlockPeriodConsumpDlvrd_c         0x0C
#define gZclAttrIdMetRISDailyConsumptionTarget_c              0x0D
#define gZclAttrIdMetRISCurrBlock_c                           0x0E
#define gZclAttrIdMetRISProfileIntPeriod_c                    0x0F
#define gZclAttrIdMetRISIntReadReportingPeriod_c              0x10
#define gZclAttrIdMetRISPresetReadingTime_c                   0x11
#define gZclAttrIdMetRISVolumePerReport_c                     0x12
#define gZclAttrIdMetRISFlowRestriction_c                     0x13
#define gZclAttrIdMetRISSupplyStatus_c                        0x14
#define gZclAttrIdMetRISCurrInletEnergyCarrierSummation_c     0x15
#define gZclAttrIdMetRISCurrOutletEnergyCarrierSummation_c    0x16
#define gZclAttrIdMetRISInletTemp_c                           0x17
#define gZclAttrIdMetRISOutletTemp_c                          0x18
#define gZclAttrIdMetRISCtrlTemp_c                            0x19
#define gZclAttrIdMetRISCurrInletEnergyCarrierDemand_c        0x1A
#define gZclAttrIdMetRISCurrOutletEnergyCarrierDemand_c       0x1B
#define gZclAttrIdMetRISPrevBlockPeriodConsumptionDlvrd_c     0x1C
#define gZclAttrIdMetRISCurrBlockPeriodConsumptionRcvd_c      0x1D
#define gZclAttrIdMetRISCurrBlockRcvd_c                       0x1E
#define gZclAttrIdMetRISDFTSummationRcvd_c                    0x1F
#define gZclAttrIdMetRISActiveRegisterTierDlvrd_c             0x20
#define gZclAttrIdMetRISActiveRegisterTierRcvd_c              0x21
#define gZclAttrIdMetRISLastBlockSwitchTime_c                 0x22

/* Summation TOU Information Set */
#define gZclAttrIdMetTOUCurrTier1SummDlvrd_c  0x00
#define gZclAttrIdMetTOUCurrTier1SummRcvd_c   0x01
#define gZclAttrIdMetTOUCurrTier2SummDlvrd_c  0x02
#define gZclAttrIdMetTOUCurrTier2SummRcvd_c   0x03
#define gZclAttrIdMetTOUCurrTier3SummDlvrd_c  0x04
#define gZclAttrIdMetTOUCurrTier3SummRcvd_c   0x05
#define gZclAttrIdMetTOUCurrTier4SummDlvrd_c  0x06
#define gZclAttrIdMetTOUCurrTier4SummRcvd_c   0x07
#define gZclAttrIdMetTOUCurrTier5SummDlvrd_c  0x08
#define gZclAttrIdMetTOUCurrTier5SummRcvd_c   0x09
#define gZclAttrIdMetTOUCurrTier6SummDlvrd_c  0x0A
#define gZclAttrIdMetTOUCurrTier6SummRcvd_c   0x0B
#define gZclAttrIdMetTOUCurrTier7SummDlvrd_c  0x0C
#define gZclAttrIdMetTOUCurrTier7SummRcvd_c   0x0D
#define gZclAttrIdMetTOUCurrTier8SummDlvrd_c  0x0E
#define gZclAttrIdMetTOUCurrTier8SummRcvd_c   0x0F
#define gZclAttrIdMetTOUCurrTier9SummDlvrd_c  0x10
#define gZclAttrIdMetTOUCurrTier9SummRcvd_c   0x11
#define gZclAttrIdMetTOUCurrTier10SummDlvrd_c 0x12
#define gZclAttrIdMetTOUCurrTier10SummRcvd_c  0x13
#define gZclAttrIdMetTOUCurrTier11SummDlvrd_c 0x14
#define gZclAttrIdMetTOUCurrTier11SummRcvd_c  0x15
#define gZclAttrIdMetTOUCurrTier12SummDlvrd_c 0x16
#define gZclAttrIdMetTOUCurrTier12SummRcvd_c  0x17
#define gZclAttrIdMetTOUCurrTier13SummDlvrd_c 0x18
#define gZclAttrIdMetTOUCurrTier13SummRcvd_c  0x19
#define gZclAttrIdMetTOUCurrTier14SummDlvrd_c 0x1A
#define gZclAttrIdMetTOUCurrTier14SummRcvd_c  0x1B
#define gZclAttrIdMetTOUCurrTier15SummDlvrd_c 0x1C
#define gZclAttrIdMetTOUCurrTier15SummRcvd_c  0x1D
#define gZclAttrIdMetTOUCPP1SummDlvrd_c       0xFC
#define gZclAttrIdMetTOUCPP2SummDlvrd_c       0xFE

/* Consumption Formating Set*/
#define gZclAttrIdMetCFSUnitofMeasure_c                    0x00  /* M */
#define gZclAttrIdMetCFSMultiplier_c                       0x01
#define gZclAttrIdMetCFSDivisor_c                          0x02
#define gZclAttrIdMetCFSSummFormat_c                       0x03  /* M */
#define gZclAttrIdMetCFSDmndFormat_c                       0x04
#define gZclAttrIdMetCFSHistoricalConsmpFormat_c           0x05
#define gZclAttrIdMetCFSMetDevType_c                       0x06 /* M */
#define gZclAttrIdMetCFSSiteID_c                           0x07 
#define gZclAttrIdMetCFSMeterSerialNumber_c                0x08 
#define gZclAttrIdMetCFSEnergyCarrierUnitOfMeasure_c       0x09
#define gZclAttrIdMetCFSEnergyCarrierSummationFormatting_c 0x0A
#define gZclAttrIdMetCFSEnergyCarrierDemandFormatting_c    0x0B
#define gZclAttrIdMetCFSTempUnitOfMeasure_c                0x0C
#define gZclAttrIdMetCFSTempFormatting_c                   0x0D
#define gZclAttrIdMetCFSModuleSerialNum_c                  0x0E
#define gZclAttrIdMetCFSOpTariffLabelDlvrd_c               0x0F
#define gZclAttrIdMetCFSOpTariffLabelRcvd_c                0x10

/* ESP Historical Consumption */
#define gZclAttrIdMetEHCInstantaneousDmnd_c                0x00
#define gZclAttrIdMetEHCCurrDayConsmpDlvrd_c               0x01
#define gZclAttrIdMetEHCCurrDayConsmpRcvd_c                0x02
#define gZclAttrIdMetEHCPreviousDayConsmpDlvrd_c           0x03
#define gZclAttrIdMetEHCPreviousDayConsmpRcvd_c            0x04
#define gZclAttrIdMetEHCCurrPrtlProfIntrvStartTimeDlvrd_c  0x05
#define gZclAttrIdMetEHCCurrPrtlProfIntrvStartTimeRcvd_c   0x06
#define gZclAttrIdMetEHCCurrPrtlProfIntrvValueDlvrd_c      0x07
#define gZclAttrIdMetEHCCurrPrtlProfIntrvValueRcvd_c       0x08
#define gZclAttrIdMetEHCCurrDayMaxPressure_c               0x09
#define gZclAttrIdMetEHCCurrDayMinPressure_c               0x0A
#define gZclAttrIdMetEHCPrevDayMaxPressure_c               0x0B
#define gZclAttrIdMetEHCPrevDayMinPressure_c               0x0C
#define gZclAttrIdMetEHCCurrDayMaxDemand_c                 0x0D
#define gZclAttrIdMetEHCPrevDayMaxDemand_c                 0x0E
#define gZclAttrIdMetEHCCurrMonthMaxDemand_c               0x0F
#define gZclAttrIdMetEHCCurrYearMaxDemand_c                0x10
#define gZclAttrIdMetEHCCurrDayMaxEnergyCarrierDemand_c    0x11
#define gZclAttrIdMetEHCPrevDayMaxEnergyCarrierDemand_c    0x12
#define gZclAttrIdMetEHCCurrMonthMaxEnergyCarrierDemand_c  0x13
#define gZclAttrIdMetEHCCurrMonthMinEnergyCarrierDemand_c  0x14
#define gZclAttrIdMetEHCCurrYearMaxEnergyCarrierDemand_c   0x15
#define gZclAttrIdMetEHCCurrYearMinEnergyCarrierDemand_c   0x16
#define gZclAttrIdMetEHCDayConsump_c                       0x20
#define gZclAttrIdMetEHCWeekConsump_c                      0x30
#define gZclAttrIdMetEHCMonthConsump_c                     0x40

/*Meter Status Set*/
#define gZclAttrIdMetMSStatus_c                 0x00  /* M */
#define gZclAttrIdMetMSSRemainingBatteryLife_c  0x01  
#define gZclAttrIdMetMSSHoursInOperation_c      0x02  
#define gZclAttrIdMetMSSHoursInFault_c          0x03  
#define gZclAttrIdMetMSSExtStatus_c             0x04
#define gZclAttrIdMetMSSRemainingBatteryLifeInDays_c 0x05
#define gZclAttrIdMetMSSCurrMetID_c             0x06
#define gZclAttrIdMetMSSAmbientConsumpInd_c     0x07

/*Load Profile Configuration */
#define gZclAttrIdMetLPCMaxNumOfPeriodsDlvrd_c       0x00

/* Supply Limit Attribute Set */
#define gZclAttrIdMetSLSCurrDmndDlvrd_c              0x00
#define gZclAttrIdMetSLSDmndLimit_c                  0x01
#define gZclAttrIdMetSLSDmndIntegrationPeriod_c      0x02
#define gZclAttrIdMetSLSNumOfDmndSubIntrvs_c         0x03

/* Block Information Attribute Set Delivered */
#define gZclAttrIdMetBISCurrNoTierBlock1SummationDelivered_c            0x00
#define gZclAttrIdMetBISCurrNoTierBlock2SummationDelivered_c            0x01
#define gZclAttrIdMetBISCurrNoTierBlock3SummationDelivered_c            0x02
#define gZclAttrIdMetBISCurrNoTierBlock4SummationDelivered_c            0x03
#define gZclAttrIdMetBISCurrNoTierBlock5SummationDelivered_c            0x04
#define gZclAttrIdMetBISCurrNoTierBlock6SummationDelivered_c            0x05
#define gZclAttrIdMetBISCurrNoTierBlock7SummationDelivered_c            0x06
#define gZclAttrIdMetBISCurrNoTierBlock8SummationDelivered_c            0x07
#define gZclAttrIdMetBISCurrNoTierBlock9SummationDelivered_c            0x08
#define gZclAttrIdMetBISCurrNoTierBlock10SummationDelivered_c           0x09
#define gZclAttrIdMetBISCurrNoTierBlock11SummationDelivered_c           0x0A
#define gZclAttrIdMetBISCurrNoTierBlock12SummationDelivered_c           0x0B
#define gZclAttrIdMetBISCurrNoTierBlock13SummationDelivered_c           0x0C
#define gZclAttrIdMetBISCurrNoTierBlock14SummationDelivered_c           0x0D
#define gZclAttrIdMetBISCurrNoTierBlock15SummationDelivered_c           0x0E
#define gZclAttrIdMetBISCurrNoTierBlock16SummationDelivered_c           0x0F
#define gZclAttrIdMetBISCurrNoTier1Block1SummationDelivered_c           0x10
#define gZclAttrIdMetBISCurrNoTier1Block2SummationDelivered_c           0x11
#define gZclAttrIdMetBISCurrNoTier1Block3SummationDelivered_c           0x12
#define gZclAttrIdMetBISCurrNoTier1Block4SummationDelivered_c           0x13
#define gZclAttrIdMetBISCurrNoTier1Block5SummationDelivered_c           0x14
#define gZclAttrIdMetBISCurrNoTier1Block6SummationDelivered_c           0x15
#define gZclAttrIdMetBISCurrNoTier1Block7SummationDelivered_c           0x16
#define gZclAttrIdMetBISCurrNoTier1Block8SummationDelivered_c           0x17
#define gZclAttrIdMetBISCurrNoTier1Block9SummationDelivered_c           0x18
#define gZclAttrIdMetBISCurrNoTier1Block10SummationDelivered_c          0x19
#define gZclAttrIdMetBISCurrNoTier1Block11SummationDelivered_c          0x1A
#define gZclAttrIdMetBISCurrNoTier1Block12SummationDelivered_c          0x1B
#define gZclAttrIdMetBISCurrNoTier1Block13SummationDelivered_c          0x1C
#define gZclAttrIdMetBISCurrNoTier1Block14SummationDelivered_c          0x1D
#define gZclAttrIdMetBISCurrNoTier1Block15SummationDelivered_c          0x1E
#define gZclAttrIdMetBISCurrNoTier1Block16SummationDelivered_c          0x1F
#define gZclAttrIdMetBISCurrNoTier2Block1SummationDelivered_c           0x20
#define gZclAttrIdMetBISCurrNoTier2Block2SummationDelivered_c           0x21
#define gZclAttrIdMetBISCurrNoTier2Block3SummationDelivered_c           0x22
#define gZclAttrIdMetBISCurrNoTier2Block4SummationDelivered_c           0x23
#define gZclAttrIdMetBISCurrNoTier2Block5SummationDelivered_c           0x24
#define gZclAttrIdMetBISCurrNoTier2Block6SummationDelivered_c           0x25
#define gZclAttrIdMetBISCurrNoTier2Block7SummationDelivered_c           0x26
#define gZclAttrIdMetBISCurrNoTier2Block8SummationDelivered_c           0x27
#define gZclAttrIdMetBISCurrNoTier2Block9SummationDelivered_c           0x28
#define gZclAttrIdMetBISCurrNoTier2Block10SummationDelivered_c          0x29
#define gZclAttrIdMetBISCurrNoTier2Block11SummationDelivered_c          0x2A
#define gZclAttrIdMetBISCurrNoTier2Block12SummationDelivered_c          0x2B
#define gZclAttrIdMetBISCurrNoTier2Block13SummationDelivered_c          0x2C
#define gZclAttrIdMetBISCurrNoTier2Block14SummationDelivered_c          0x2D
#define gZclAttrIdMetBISCurrNoTier2Block15SummationDelivered_c          0x2E
#define gZclAttrIdMetBISCurrNoTier2Block16SummationDelivered_c          0x2F
#define gZclAttrIdMetBISCurrNoTier3Block1SummationDelivered_c           0x30
#define gZclAttrIdMetBISCurrNoTier3Block2SummationDelivered_c           0x31
#define gZclAttrIdMetBISCurrNoTier3Block3SummationDelivered_c           0x32
#define gZclAttrIdMetBISCurrNoTier3Block4SummationDelivered_c           0x33
#define gZclAttrIdMetBISCurrNoTier3Block5SummationDelivered_c           0x34
#define gZclAttrIdMetBISCurrNoTier3Block6SummationDelivered_c           0x35
#define gZclAttrIdMetBISCurrNoTier3Block7SummationDelivered_c           0x36
#define gZclAttrIdMetBISCurrNoTier3Block8SummationDelivered_c           0x37
#define gZclAttrIdMetBISCurrNoTier3Block9SummationDelivered_c           0x38
#define gZclAttrIdMetBISCurrNoTier3Block10SummationDelivered_c          0x39
#define gZclAttrIdMetBISCurrNoTier3Block11SummationDelivered_c          0x3A
#define gZclAttrIdMetBISCurrNoTier3Block12SummationDelivered_c          0x3B
#define gZclAttrIdMetBISCurrNoTier3Block13SummationDelivered_c          0x3C
#define gZclAttrIdMetBISCurrNoTier3Block14SummationDelivered_c          0x3D
#define gZclAttrIdMetBISCurrNoTier3Block15SummationDelivered_c          0x3E
#define gZclAttrIdMetBISCurrNoTier3Block16SummationDelivered_c          0x3F
#define gZclAttrIdMetBISCurrNoTier4Block1SummationDelivered_c           0x40
#define gZclAttrIdMetBISCurrNoTier4Block2SummationDelivered_c           0x41
#define gZclAttrIdMetBISCurrNoTier4Block3SummationDelivered_c           0x42
#define gZclAttrIdMetBISCurrNoTier4Block4SummationDelivered_c           0x43
#define gZclAttrIdMetBISCurrNoTier4Block5SummationDelivered_c           0x44
#define gZclAttrIdMetBISCurrNoTier4Block6SummationDelivered_c           0x45
#define gZclAttrIdMetBISCurrNoTier4Block7SummationDelivered_c           0x46
#define gZclAttrIdMetBISCurrNoTier4Block8SummationDelivered_c           0x47
#define gZclAttrIdMetBISCurrNoTier4Block9SummationDelivered_c           0x48
#define gZclAttrIdMetBISCurrNoTier4Block10SummationDelivered_c          0x49
#define gZclAttrIdMetBISCurrNoTier4Block11SummationDelivered_c          0x4A
#define gZclAttrIdMetBISCurrNoTier4Block12SummationDelivered_c          0x4B
#define gZclAttrIdMetBISCurrNoTier4Block13SummationDelivered_c          0x4C
#define gZclAttrIdMetBISCurrNoTier4Block14SummationDelivered_c          0x4D
#define gZclAttrIdMetBISCurrNoTier4Block15SummationDelivered_c          0x4E
#define gZclAttrIdMetBISCurrNoTier4Block16SummationDelivered_c          0x4F
#define gZclAttrIdMetBISCurrNoTier5Block1SummationDelivered_c           0x50
#define gZclAttrIdMetBISCurrNoTier5Block2SummationDelivered_c           0x51
#define gZclAttrIdMetBISCurrNoTier5Block3SummationDelivered_c           0x52
#define gZclAttrIdMetBISCurrNoTier5Block4SummationDelivered_c           0x53
#define gZclAttrIdMetBISCurrNoTier5Block5SummationDelivered_c           0x54
#define gZclAttrIdMetBISCurrNoTier5Block6SummationDelivered_c           0x55
#define gZclAttrIdMetBISCurrNoTier5Block7SummationDelivered_c           0x56
#define gZclAttrIdMetBISCurrNoTier5Block8SummationDelivered_c           0x57
#define gZclAttrIdMetBISCurrNoTier5Block9SummationDelivered_c           0x58
#define gZclAttrIdMetBISCurrNoTier5Block10SummationDelivered_c          0x59
#define gZclAttrIdMetBISCurrNoTier5Block11SummationDelivered_c          0x5A
#define gZclAttrIdMetBISCurrNoTier5Block12SummationDelivered_c          0x5B
#define gZclAttrIdMetBISCurrNoTier5Block13SummationDelivered_c          0x5C
#define gZclAttrIdMetBISCurrNoTier5Block14SummationDelivered_c          0x5D
#define gZclAttrIdMetBISCurrNoTier5Block15SummationDelivered_c          0x5E
#define gZclAttrIdMetBISCurrNoTier5Block16SummationDelivered_c          0x5F
#define gZclAttrIdMetBISCurrNoTier6Block1SummationDelivered_c           0x60
#define gZclAttrIdMetBISCurrNoTier6Block2SummationDelivered_c           0x61
#define gZclAttrIdMetBISCurrNoTier6Block3SummationDelivered_c           0x62
#define gZclAttrIdMetBISCurrNoTier6Block4SummationDelivered_c           0x63
#define gZclAttrIdMetBISCurrNoTier6Block5SummationDelivered_c           0x64
#define gZclAttrIdMetBISCurrNoTier6Block6SummationDelivered_c           0x65
#define gZclAttrIdMetBISCurrNoTier6Block7SummationDelivered_c           0x66
#define gZclAttrIdMetBISCurrNoTier6Block8SummationDelivered_c           0x67
#define gZclAttrIdMetBISCurrNoTier6Block9SummationDelivered_c           0x68
#define gZclAttrIdMetBISCurrNoTier6Block10SummationDelivered_c          0x69
#define gZclAttrIdMetBISCurrNoTier6Block11SummationDelivered_c          0x6A
#define gZclAttrIdMetBISCurrNoTier6Block12SummationDelivered_c          0x6B
#define gZclAttrIdMetBISCurrNoTier6Block13SummationDelivered_c          0x6C
#define gZclAttrIdMetBISCurrNoTier6Block14SummationDelivered_c          0x6D
#define gZclAttrIdMetBISCurrNoTier6Block15SummationDelivered_c          0x6E
#define gZclAttrIdMetBISCurrNoTier6Block16SummationDelivered_c          0x6F
#define gZclAttrIdMetBISCurrNoTier7Block1SummationDelivered_c           0x70
#define gZclAttrIdMetBISCurrNoTier7Block2SummationDelivered_c           0x71
#define gZclAttrIdMetBISCurrNoTier7Block3SummationDelivered_c           0x72
#define gZclAttrIdMetBISCurrNoTier7Block4SummationDelivered_c           0x73
#define gZclAttrIdMetBISCurrNoTier7Block5SummationDelivered_c           0x74
#define gZclAttrIdMetBISCurrNoTier7Block6SummationDelivered_c           0x75
#define gZclAttrIdMetBISCurrNoTier7Block7SummationDelivered_c           0x76
#define gZclAttrIdMetBISCurrNoTier7Block8SummationDelivered_c           0x77
#define gZclAttrIdMetBISCurrNoTier7Block9SummationDelivered_c           0x78
#define gZclAttrIdMetBISCurrNoTier7Block10SummationDelivered_c          0x79
#define gZclAttrIdMetBISCurrNoTier7Block11SummationDelivered_c          0x7A
#define gZclAttrIdMetBISCurrNoTier7Block12SummationDelivered_c          0x7B
#define gZclAttrIdMetBISCurrNoTier7Block13SummationDelivered_c          0x7C
#define gZclAttrIdMetBISCurrNoTier7Block14SummationDelivered_c          0x7D
#define gZclAttrIdMetBISCurrNoTier7Block15SummationDelivered_c          0x7E
#define gZclAttrIdMetBISCurrNoTier7Block16SummationDelivered_c          0x7F
#define gZclAttrIdMetBISCurrNoTier8Block1SummationDelivered_c           0x80
#define gZclAttrIdMetBISCurrNoTier8Block2SummationDelivered_c           0x81
#define gZclAttrIdMetBISCurrNoTier8Block3SummationDelivered_c           0x82
#define gZclAttrIdMetBISCurrNoTier8Block4SummationDelivered_c           0x83
#define gZclAttrIdMetBISCurrNoTier8Block5SummationDelivered_c           0x84
#define gZclAttrIdMetBISCurrNoTier8Block6SummationDelivered_c           0x85
#define gZclAttrIdMetBISCurrNoTier8Block7SummationDelivered_c           0x86
#define gZclAttrIdMetBISCurrNoTier8Block8SummationDelivered_c           0x87
#define gZclAttrIdMetBISCurrNoTier8Block9SummationDelivered_c           0x88
#define gZclAttrIdMetBISCurrNoTier8Block10SummationDelivered_c          0x89
#define gZclAttrIdMetBISCurrNoTier8Block11SummationDelivered_c          0x8A
#define gZclAttrIdMetBISCurrNoTier8Block12SummationDelivered_c          0x8B
#define gZclAttrIdMetBISCurrNoTier8Block13SummationDelivered_c          0x8C
#define gZclAttrIdMetBISCurrNoTier8Block14SummationDelivered_c          0x8D
#define gZclAttrIdMetBISCurrNoTier8Block15SummationDelivered_c          0x8E
#define gZclAttrIdMetBISCurrNoTier8Block16SummationDelivered_c          0x8F
#define gZclAttrIdMetBISCurrNoTier9Block1SummationDelivered_c           0x90
#define gZclAttrIdMetBISCurrNoTier9Block2SummationDelivered_c           0x91
#define gZclAttrIdMetBISCurrNoTier9Block3SummationDelivered_c           0x92
#define gZclAttrIdMetBISCurrNoTier9Block4SummationDelivered_c           0x93
#define gZclAttrIdMetBISCurrNoTier9Block5SummationDelivered_c           0x94
#define gZclAttrIdMetBISCurrNoTier9Block6SummationDelivered_c           0x95
#define gZclAttrIdMetBISCurrNoTier9Block7SummationDelivered_c           0x96
#define gZclAttrIdMetBISCurrNoTier9Block8SummationDelivered_c           0x97
#define gZclAttrIdMetBISCurrNoTier9Block9SummationDelivered_c           0x98
#define gZclAttrIdMetBISCurrNoTier9Block10SummationDelivered_c          0x99
#define gZclAttrIdMetBISCurrNoTier9Block11SummationDelivered_c          0x9A
#define gZclAttrIdMetBISCurrNoTier9Block12SummationDelivered_c          0x9B
#define gZclAttrIdMetBISCurrNoTier9Block13SummationDelivered_c          0x9C
#define gZclAttrIdMetBISCurrNoTier9Block14SummationDelivered_c          0x9D
#define gZclAttrIdMetBISCurrNoTier9Block15SummationDelivered_c          0x9E
#define gZclAttrIdMetBISCurrNoTier9Block16SummationDelivered_c          0x9F
#define gZclAttrIdMetBISCurrNoTier10Block1SummationDelivered_c          0xA0
#define gZclAttrIdMetBISCurrNoTier10Block2SummationDelivered_c          0xA1
#define gZclAttrIdMetBISCurrNoTier10Block3SummationDelivered_c          0xA2
#define gZclAttrIdMetBISCurrNoTier10Block4SummationDelivered_c          0xA3
#define gZclAttrIdMetBISCurrNoTier10Block5SummationDelivered_c          0xA4
#define gZclAttrIdMetBISCurrNoTier10Block6SummationDelivered_c          0xA5
#define gZclAttrIdMetBISCurrNoTier10Block7SummationDelivered_c          0xA6
#define gZclAttrIdMetBISCurrNoTier10Block8SummationDelivered_c          0xA7
#define gZclAttrIdMetBISCurrNoTier10Block9SummationDelivered_c          0xA8
#define gZclAttrIdMetBISCurrNoTier10Block10SummationDelivered_c         0xA9
#define gZclAttrIdMetBISCurrNoTier10Block11SummationDelivered_c         0xAA
#define gZclAttrIdMetBISCurrNoTier10Block12SummationDelivered_c         0xAB
#define gZclAttrIdMetBISCurrNoTier10Block13SummationDelivered_c         0xAC
#define gZclAttrIdMetBISCurrNoTier10Block14SummationDelivered_c         0xAD
#define gZclAttrIdMetBISCurrNoTier10Block15SummationDelivered_c         0xAE
#define gZclAttrIdMetBISCurrNoTier10Block16SummationDelivered_c         0xAF
#define gZclAttrIdMetBISCurrNoTier11Block1SummationDelivered_c          0xB0
#define gZclAttrIdMetBISCurrNoTier11Block2SummationDelivered_c          0xB1
#define gZclAttrIdMetBISCurrNoTier11Block3SummationDelivered_c          0xB2
#define gZclAttrIdMetBISCurrNoTier11Block4SummationDelivered_c          0xB3
#define gZclAttrIdMetBISCurrNoTier11Block5SummationDelivered_c          0xB4
#define gZclAttrIdMetBISCurrNoTier11Block6SummationDelivered_c          0xB5
#define gZclAttrIdMetBISCurrNoTier11Block7SummationDelivered_c          0xB6
#define gZclAttrIdMetBISCurrNoTier11Block8SummationDelivered_c          0xB7
#define gZclAttrIdMetBISCurrNoTier11Block9SummationDelivered_c          0xB8
#define gZclAttrIdMetBISCurrNoTier11Block10SummationDelivered_c         0xB9
#define gZclAttrIdMetBISCurrNoTier11Block11SummationDelivered_c         0xBA
#define gZclAttrIdMetBISCurrNoTier11Block12SummationDelivered_c         0xBB
#define gZclAttrIdMetBISCurrNoTier11Block13SummationDelivered_c         0xBC
#define gZclAttrIdMetBISCurrNoTier11Block14SummationDelivered_c         0xBD
#define gZclAttrIdMetBISCurrNoTier11Block15SummationDelivered_c         0xBE
#define gZclAttrIdMetBISCurrNoTier11Block16SummationDelivered_c         0xBF
#define gZclAttrIdMetBISCurrNoTier12Block1SummationDelivered_c          0xC0
#define gZclAttrIdMetBISCurrNoTier12Block2SummationDelivered_c          0xC1
#define gZclAttrIdMetBISCurrNoTier12Block3SummationDelivered_c          0xC2
#define gZclAttrIdMetBISCurrNoTier12Block4SummationDelivered_c          0xC3
#define gZclAttrIdMetBISCurrNoTier12Block5SummationDelivered_c          0xC4
#define gZclAttrIdMetBISCurrNoTier12Block6SummationDelivered_c          0xC5
#define gZclAttrIdMetBISCurrNoTier12Block7SummationDelivered_c          0xC6
#define gZclAttrIdMetBISCurrNoTier12Block8SummationDelivered_c          0xC7
#define gZclAttrIdMetBISCurrNoTier12Block9SummationDelivered_c          0xC8
#define gZclAttrIdMetBISCurrNoTier12Block10SummationDelivered_c         0xC9
#define gZclAttrIdMetBISCurrNoTier12Block11SummationDelivered_c         0xCA
#define gZclAttrIdMetBISCurrNoTier12Block12SummationDelivered_c         0xCB
#define gZclAttrIdMetBISCurrNoTier12Block13SummationDelivered_c         0xCC
#define gZclAttrIdMetBISCurrNoTier12Block14SummationDelivered_c         0xCD
#define gZclAttrIdMetBISCurrNoTier12Block15SummationDelivered_c         0xCE
#define gZclAttrIdMetBISCurrNoTier12Block16SummationDelivered_c         0xCF
#define gZclAttrIdMetBISCurrNoTier13Block1SummationDelivered_c          0xD0
#define gZclAttrIdMetBISCurrNoTier13Block2SummationDelivered_c          0xD1
#define gZclAttrIdMetBISCurrNoTier13Block3SummationDelivered_c          0xD2
#define gZclAttrIdMetBISCurrNoTier13Block4SummationDelivered_c          0xD3
#define gZclAttrIdMetBISCurrNoTier13Block5SummationDelivered_c          0xD4
#define gZclAttrIdMetBISCurrNoTier13Block6SummationDelivered_c          0xD5
#define gZclAttrIdMetBISCurrNoTier13Block7SummationDelivered_c          0xD6
#define gZclAttrIdMetBISCurrNoTier13Block8SummationDelivered_c          0xD7
#define gZclAttrIdMetBISCurrNoTier13Block9SummationDelivered_c          0xD8
#define gZclAttrIdMetBISCurrNoTier13Block10SummationDelivered_c         0xD9
#define gZclAttrIdMetBISCurrNoTier13Block11SummationDelivered_c         0xDA
#define gZclAttrIdMetBISCurrNoTier13Block12SummationDelivered_c         0xDB
#define gZclAttrIdMetBISCurrNoTier13Block13SummationDelivered_c         0xDC
#define gZclAttrIdMetBISCurrNoTier13Block14SummationDelivered_c         0xDD
#define gZclAttrIdMetBISCurrNoTier13Block15SummationDelivered_c         0xDE
#define gZclAttrIdMetBISCurrNoTier13Block16SummationDelivered_c         0xDF
#define gZclAttrIdMetBISCurrNoTier14Block1SummationDelivered_c          0xE0
#define gZclAttrIdMetBISCurrNoTier14Block2SummationDelivered_c          0xE1
#define gZclAttrIdMetBISCurrNoTier14Block3SummationDelivered_c          0xE2
#define gZclAttrIdMetBISCurrNoTier14Block4SummationDelivered_c          0xE3
#define gZclAttrIdMetBISCurrNoTier14Block5SummationDelivered_c          0xE4
#define gZclAttrIdMetBISCurrNoTier14Block6SummationDelivered_c          0xE5
#define gZclAttrIdMetBISCurrNoTier14Block7SummationDelivered_c          0xE6
#define gZclAttrIdMetBISCurrNoTier14Block8SummationDelivered_c          0xE7
#define gZclAttrIdMetBISCurrNoTier14Block9SummationDelivered_c          0xE8
#define gZclAttrIdMetBISCurrNoTier14Block10SummationDelivered_c         0xE9
#define gZclAttrIdMetBISCurrNoTier14Block11SummationDelivered_c         0xEA
#define gZclAttrIdMetBISCurrNoTier14Block12SummationDelivered_c         0xEB
#define gZclAttrIdMetBISCurrNoTier14Block13SummationDelivered_c         0xEC
#define gZclAttrIdMetBISCurrNoTier14Block14SummationDelivered_c         0xED
#define gZclAttrIdMetBISCurrNoTier14Block15SummationDelivered_c         0xEE
#define gZclAttrIdMetBISCurrNoTier14Block16SummationDelivered_c         0xEF
#define gZclAttrIdMetBISCurrNoTier15Block1SummationDelivered_c          0xF0
#define gZclAttrIdMetBISCurrNoTier15Block2SummationDelivered_c          0xF1
#define gZclAttrIdMetBISCurrNoTier15Block3SummationDelivered_c          0xF2
#define gZclAttrIdMetBISCurrNoTier15Block4SummationDelivered_c          0xF3
#define gZclAttrIdMetBISCurrNoTier15Block5SummationDelivered_c          0xF4
#define gZclAttrIdMetBISCurrNoTier15Block6SummationDelivered_c          0xF5
#define gZclAttrIdMetBISCurrNoTier15Block7SummationDelivered_c          0xF6
#define gZclAttrIdMetBISCurrNoTier15Block8SummationDelivered_c          0xF7
#define gZclAttrIdMetBISCurrNoTier15Block9SummationDelivered_c          0xF8
#define gZclAttrIdMetBISCurrNoTier15Block10SummationDelivered_c         0xF9
#define gZclAttrIdMetBISCurrNoTier15Block11SummationDelivered_c         0xFA
#define gZclAttrIdMetBISCurrNoTier15Block12SummationDelivered_c         0xFB
#define gZclAttrIdMetBISCurrNoTier15Block13SummationDelivered_c         0xFC
#define gZclAttrIdMetBISCurrNoTier15Block14SummationDelivered_c         0xFD
#define gZclAttrIdMetBISCurrNoTier15Block15SummationDelivered_c         0xFE
#define gZclAttrIdMetBISCurrNoTier15Block16SummationDelivered_c         0xFF

/* Alarm Attribute Set */
#define gZclAttrIdMetASGenericAlarmMask_c                  0x00
#define gZclAttrIdMetASElectricityAlarmMask_c              0x01
#define gZclAttrIdMetASGenericFlowPressureAlarmMask_c      0x02
#define gZclAttrIdMetASWaterSpecificAlarmMask_c            0x03
#define gZclAttrIdMetASHeatAndCoolingSpecificAlarmMask_c   0x04
#define gZclAttrIdMetASGasSpecificAlarmMask_c              0x05

/* Block Information Attribute Set Received */
#define gZclAttrIdMetBISCurrNoTierBlock1SummationReceived_c            0x00
#define gZclAttrIdMetBISCurrNoTierBlock2SummationReceived_c            0x01
#define gZclAttrIdMetBISCurrNoTierBlock3SummationReceived_c            0x02
#define gZclAttrIdMetBISCurrNoTierBlock4SummationReceived_c            0x03
#define gZclAttrIdMetBISCurrNoTierBlock5SummationReceived_c            0x04
#define gZclAttrIdMetBISCurrNoTierBlock6SummationReceived_c            0x05
#define gZclAttrIdMetBISCurrNoTierBlock7SummationReceived_c            0x06
#define gZclAttrIdMetBISCurrNoTierBlock8SummationReceived_c            0x07
#define gZclAttrIdMetBISCurrNoTierBlock9SummationReceived_c            0x08
#define gZclAttrIdMetBISCurrNoTierBlock10SummationReceived_c           0x09
#define gZclAttrIdMetBISCurrNoTierBlock11SummationReceived_c           0x0A
#define gZclAttrIdMetBISCurrNoTierBlock12SummationReceived_c           0x0B
#define gZclAttrIdMetBISCurrNoTierBlock13SummationReceived_c           0x0C
#define gZclAttrIdMetBISCurrNoTierBlock14SummationReceived_c           0x0D
#define gZclAttrIdMetBISCurrNoTierBlock15SummationReceived_c           0x0E
#define gZclAttrIdMetBISCurrNoTierBlock16SummationReceived_c           0x0F
#define gZclAttrIdMetBISCurrNoTier1Block1SummationReceived_c           0x10
#define gZclAttrIdMetBISCurrNoTier1Block2SummationReceived_c           0x11
#define gZclAttrIdMetBISCurrNoTier1Block3SummationReceived_c           0x12
#define gZclAttrIdMetBISCurrNoTier1Block4SummationReceived_c           0x13
#define gZclAttrIdMetBISCurrNoTier1Block5SummationReceived_c           0x14
#define gZclAttrIdMetBISCurrNoTier1Block6SummationReceived_c           0x15
#define gZclAttrIdMetBISCurrNoTier1Block7SummationReceived_c           0x16
#define gZclAttrIdMetBISCurrNoTier1Block8SummationReceived_c           0x17
#define gZclAttrIdMetBISCurrNoTier1Block9SummationReceived_c           0x18
#define gZclAttrIdMetBISCurrNoTier1Block10SummationReceived_c          0x19
#define gZclAttrIdMetBISCurrNoTier1Block11SummationReceived_c          0x1A
#define gZclAttrIdMetBISCurrNoTier1Block12SummationReceived_c          0x1B
#define gZclAttrIdMetBISCurrNoTier1Block13SummationReceived_c          0x1C
#define gZclAttrIdMetBISCurrNoTier1Block14SummationReceived_c          0x1D
#define gZclAttrIdMetBISCurrNoTier1Block15SummationReceived_c          0x1E
#define gZclAttrIdMetBISCurrNoTier1Block16SummationReceived_c          0x1F
#define gZclAttrIdMetBISCurrNoTier2Block1SummationReceived_c           0x20
#define gZclAttrIdMetBISCurrNoTier2Block2SummationReceived_c           0x21
#define gZclAttrIdMetBISCurrNoTier2Block3SummationReceived_c           0x22
#define gZclAttrIdMetBISCurrNoTier2Block4SummationReceived_c           0x23
#define gZclAttrIdMetBISCurrNoTier2Block5SummationReceived_c           0x24
#define gZclAttrIdMetBISCurrNoTier2Block6SummationReceived_c           0x25
#define gZclAttrIdMetBISCurrNoTier2Block7SummationReceived_c           0x26
#define gZclAttrIdMetBISCurrNoTier2Block8SummationReceived_c           0x27
#define gZclAttrIdMetBISCurrNoTier2Block9SummationReceived_c           0x28
#define gZclAttrIdMetBISCurrNoTier2Block10SummationReceived_c          0x29
#define gZclAttrIdMetBISCurrNoTier2Block11SummationReceived_c          0x2A
#define gZclAttrIdMetBISCurrNoTier2Block12SummationReceived_c          0x2B
#define gZclAttrIdMetBISCurrNoTier2Block13SummationReceived_c          0x2C
#define gZclAttrIdMetBISCurrNoTier2Block14SummationReceived_c          0x2D
#define gZclAttrIdMetBISCurrNoTier2Block15SummationReceived_c          0x2E
#define gZclAttrIdMetBISCurrNoTier2Block16SummationReceived_c          0x2F
#define gZclAttrIdMetBISCurrNoTier3Block1SummationReceived_c           0x30
#define gZclAttrIdMetBISCurrNoTier3Block2SummationReceived_c           0x31
#define gZclAttrIdMetBISCurrNoTier3Block3SummationReceived_c           0x32
#define gZclAttrIdMetBISCurrNoTier3Block4SummationReceived_c           0x33
#define gZclAttrIdMetBISCurrNoTier3Block5SummationReceived_c           0x34
#define gZclAttrIdMetBISCurrNoTier3Block6SummationReceived_c           0x35
#define gZclAttrIdMetBISCurrNoTier3Block7SummationReceived_c           0x36
#define gZclAttrIdMetBISCurrNoTier3Block8SummationReceived_c           0x37
#define gZclAttrIdMetBISCurrNoTier3Block9SummationReceived_c           0x38
#define gZclAttrIdMetBISCurrNoTier3Block10SummationReceived_c          0x39
#define gZclAttrIdMetBISCurrNoTier3Block11SummationReceived_c          0x3A
#define gZclAttrIdMetBISCurrNoTier3Block12SummationReceived_c          0x3B
#define gZclAttrIdMetBISCurrNoTier3Block13SummationReceived_c          0x3C
#define gZclAttrIdMetBISCurrNoTier3Block14SummationReceived_c          0x3D
#define gZclAttrIdMetBISCurrNoTier3Block15SummationReceived_c          0x3E
#define gZclAttrIdMetBISCurrNoTier3Block16SummationReceived_c          0x3F
#define gZclAttrIdMetBISCurrNoTier4Block1SummationReceived_c           0x40
#define gZclAttrIdMetBISCurrNoTier4Block2SummationReceived_c           0x41
#define gZclAttrIdMetBISCurrNoTier4Block3SummationReceived_c           0x42
#define gZclAttrIdMetBISCurrNoTier4Block4SummationReceived_c           0x43
#define gZclAttrIdMetBISCurrNoTier4Block5SummationReceived_c           0x44
#define gZclAttrIdMetBISCurrNoTier4Block6SummationReceived_c           0x45
#define gZclAttrIdMetBISCurrNoTier4Block7SummationReceived_c           0x46
#define gZclAttrIdMetBISCurrNoTier4Block8SummationReceived_c           0x47
#define gZclAttrIdMetBISCurrNoTier4Block9SummationReceived_c           0x48
#define gZclAttrIdMetBISCurrNoTier4Block10SummationReceived_c          0x49
#define gZclAttrIdMetBISCurrNoTier4Block11SummationReceived_c          0x4A
#define gZclAttrIdMetBISCurrNoTier4Block12SummationReceived_c          0x4B
#define gZclAttrIdMetBISCurrNoTier4Block13SummationReceived_c          0x4C
#define gZclAttrIdMetBISCurrNoTier4Block14SummationReceived_c          0x4D
#define gZclAttrIdMetBISCurrNoTier4Block15SummationReceived_c          0x4E
#define gZclAttrIdMetBISCurrNoTier4Block16SummationReceived_c          0x4F
#define gZclAttrIdMetBISCurrNoTier5Block1SummationReceived_c           0x50
#define gZclAttrIdMetBISCurrNoTier5Block2SummationReceived_c           0x51
#define gZclAttrIdMetBISCurrNoTier5Block3SummationReceived_c           0x52
#define gZclAttrIdMetBISCurrNoTier5Block4SummationReceived_c           0x53
#define gZclAttrIdMetBISCurrNoTier5Block5SummationReceived_c           0x54
#define gZclAttrIdMetBISCurrNoTier5Block6SummationReceived_c           0x55
#define gZclAttrIdMetBISCurrNoTier5Block7SummationReceived_c           0x56
#define gZclAttrIdMetBISCurrNoTier5Block8SummationReceived_c           0x57
#define gZclAttrIdMetBISCurrNoTier5Block9SummationReceived_c           0x58
#define gZclAttrIdMetBISCurrNoTier5Block10SummationReceived_c          0x59
#define gZclAttrIdMetBISCurrNoTier5Block11SummationReceived_c          0x5A
#define gZclAttrIdMetBISCurrNoTier5Block12SummationReceived_c          0x5B
#define gZclAttrIdMetBISCurrNoTier5Block13SummationReceived_c          0x5C
#define gZclAttrIdMetBISCurrNoTier5Block14SummationReceived_c          0x5D
#define gZclAttrIdMetBISCurrNoTier5Block15SummationReceived_c          0x5E
#define gZclAttrIdMetBISCurrNoTier5Block16SummationReceived_c          0x5F
#define gZclAttrIdMetBISCurrNoTier6Block1SummationReceived_c           0x60
#define gZclAttrIdMetBISCurrNoTier6Block2SummationReceived_c           0x61
#define gZclAttrIdMetBISCurrNoTier6Block3SummationReceived_c           0x62
#define gZclAttrIdMetBISCurrNoTier6Block4SummationReceived_c           0x63
#define gZclAttrIdMetBISCurrNoTier6Block5SummationReceived_c           0x64
#define gZclAttrIdMetBISCurrNoTier6Block6SummationReceived_c           0x65
#define gZclAttrIdMetBISCurrNoTier6Block7SummationReceived_c           0x66
#define gZclAttrIdMetBISCurrNoTier6Block8SummationReceived_c           0x67
#define gZclAttrIdMetBISCurrNoTier6Block9SummationReceived_c           0x68
#define gZclAttrIdMetBISCurrNoTier6Block10SummationReceived_c          0x69
#define gZclAttrIdMetBISCurrNoTier6Block11SummationReceived_c          0x6A
#define gZclAttrIdMetBISCurrNoTier6Block12SummationReceived_c          0x6B
#define gZclAttrIdMetBISCurrNoTier6Block13SummationReceived_c          0x6C
#define gZclAttrIdMetBISCurrNoTier6Block14SummationReceived_c          0x6D
#define gZclAttrIdMetBISCurrNoTier6Block15SummationReceived_c          0x6E
#define gZclAttrIdMetBISCurrNoTier6Block16SummationReceived_c          0x6F
#define gZclAttrIdMetBISCurrNoTier7Block1SummationReceived_c           0x70
#define gZclAttrIdMetBISCurrNoTier7Block2SummationReceived_c           0x71
#define gZclAttrIdMetBISCurrNoTier7Block3SummationReceived_c           0x72
#define gZclAttrIdMetBISCurrNoTier7Block4SummationReceived_c           0x73
#define gZclAttrIdMetBISCurrNoTier7Block5SummationReceived_c           0x74
#define gZclAttrIdMetBISCurrNoTier7Block6SummationReceived_c           0x75
#define gZclAttrIdMetBISCurrNoTier7Block7SummationReceived_c           0x76
#define gZclAttrIdMetBISCurrNoTier7Block8SummationReceived_c           0x77
#define gZclAttrIdMetBISCurrNoTier7Block9SummationReceived_c           0x78
#define gZclAttrIdMetBISCurrNoTier7Block10SummationReceived_c          0x79
#define gZclAttrIdMetBISCurrNoTier7Block11SummationReceived_c          0x7A
#define gZclAttrIdMetBISCurrNoTier7Block12SummationReceived_c          0x7B
#define gZclAttrIdMetBISCurrNoTier7Block13SummationReceived_c          0x7C
#define gZclAttrIdMetBISCurrNoTier7Block14SummationReceived_c          0x7D
#define gZclAttrIdMetBISCurrNoTier7Block15SummationReceived_c          0x7E
#define gZclAttrIdMetBISCurrNoTier7Block16SummationReceived_c          0x7F
#define gZclAttrIdMetBISCurrNoTier8Block1SummationReceived_c           0x80
#define gZclAttrIdMetBISCurrNoTier8Block2SummationReceived_c           0x81
#define gZclAttrIdMetBISCurrNoTier8Block3SummationReceived_c           0x82
#define gZclAttrIdMetBISCurrNoTier8Block4SummationReceived_c           0x83
#define gZclAttrIdMetBISCurrNoTier8Block5SummationReceived_c           0x84
#define gZclAttrIdMetBISCurrNoTier8Block6SummationReceived_c           0x85
#define gZclAttrIdMetBISCurrNoTier8Block7SummationReceived_c           0x86
#define gZclAttrIdMetBISCurrNoTier8Block8SummationReceived_c           0x87
#define gZclAttrIdMetBISCurrNoTier8Block9SummationReceived_c           0x88
#define gZclAttrIdMetBISCurrNoTier8Block10SummationReceived_c          0x89
#define gZclAttrIdMetBISCurrNoTier8Block11SummationReceived_c          0x8A
#define gZclAttrIdMetBISCurrNoTier8Block12SummationReceived_c          0x8B
#define gZclAttrIdMetBISCurrNoTier8Block13SummationReceived_c          0x8C
#define gZclAttrIdMetBISCurrNoTier8Block14SummationReceived_c          0x8D
#define gZclAttrIdMetBISCurrNoTier8Block15SummationReceived_c          0x8E
#define gZclAttrIdMetBISCurrNoTier8Block16SummationReceived_c          0x8F
#define gZclAttrIdMetBISCurrNoTier9Block1SummationReceived_c           0x90
#define gZclAttrIdMetBISCurrNoTier9Block2SummationReceived_c           0x91
#define gZclAttrIdMetBISCurrNoTier9Block3SummationReceived_c           0x92
#define gZclAttrIdMetBISCurrNoTier9Block4SummationReceived_c           0x93
#define gZclAttrIdMetBISCurrNoTier9Block5SummationReceived_c           0x94
#define gZclAttrIdMetBISCurrNoTier9Block6SummationReceived_c           0x95
#define gZclAttrIdMetBISCurrNoTier9Block7SummationReceived_c           0x96
#define gZclAttrIdMetBISCurrNoTier9Block8SummationReceived_c           0x97
#define gZclAttrIdMetBISCurrNoTier9Block9SummationReceived_c           0x98
#define gZclAttrIdMetBISCurrNoTier9Block10SummationReceived_c          0x99
#define gZclAttrIdMetBISCurrNoTier9Block11SummationReceived_c          0x9A
#define gZclAttrIdMetBISCurrNoTier9Block12SummationReceived_c          0x9B
#define gZclAttrIdMetBISCurrNoTier9Block13SummationReceived_c          0x9C
#define gZclAttrIdMetBISCurrNoTier9Block14SummationReceived_c          0x9D
#define gZclAttrIdMetBISCurrNoTier9Block15SummationReceived_c          0x9E
#define gZclAttrIdMetBISCurrNoTier9Block16SummationReceived_c          0x9F
#define gZclAttrIdMetBISCurrNoTier10Block1SummationReceived_c          0xA0
#define gZclAttrIdMetBISCurrNoTier10Block2SummationReceived_c          0xA1
#define gZclAttrIdMetBISCurrNoTier10Block3SummationReceived_c          0xA2
#define gZclAttrIdMetBISCurrNoTier10Block4SummationReceived_c          0xA3
#define gZclAttrIdMetBISCurrNoTier10Block5SummationReceived_c          0xA4
#define gZclAttrIdMetBISCurrNoTier10Block6SummationReceived_c          0xA5
#define gZclAttrIdMetBISCurrNoTier10Block7SummationReceived_c          0xA6
#define gZclAttrIdMetBISCurrNoTier10Block8SummationReceived_c          0xA7
#define gZclAttrIdMetBISCurrNoTier10Block9SummationReceived_c          0xA8
#define gZclAttrIdMetBISCurrNoTier10Block10SummationReceived_c         0xA9
#define gZclAttrIdMetBISCurrNoTier10Block11SummationReceived_c         0xAA
#define gZclAttrIdMetBISCurrNoTier10Block12SummationReceived_c         0xAB
#define gZclAttrIdMetBISCurrNoTier10Block13SummationReceived_c         0xAC
#define gZclAttrIdMetBISCurrNoTier10Block14SummationReceived_c         0xAD
#define gZclAttrIdMetBISCurrNoTier10Block15SummationReceived_c         0xAE
#define gZclAttrIdMetBISCurrNoTier10Block16SummationReceived_c         0xAF
#define gZclAttrIdMetBISCurrNoTier11Block1SummationReceived_c          0xB0
#define gZclAttrIdMetBISCurrNoTier11Block2SummationReceived_c          0xB1
#define gZclAttrIdMetBISCurrNoTier11Block3SummationReceived_c          0xB2
#define gZclAttrIdMetBISCurrNoTier11Block4SummationReceived_c          0xB3
#define gZclAttrIdMetBISCurrNoTier11Block5SummationReceived_c          0xB4
#define gZclAttrIdMetBISCurrNoTier11Block6SummationReceived_c          0xB5
#define gZclAttrIdMetBISCurrNoTier11Block7SummationReceived_c          0xB6
#define gZclAttrIdMetBISCurrNoTier11Block8SummationReceived_c          0xB7
#define gZclAttrIdMetBISCurrNoTier11Block9SummationReceived_c          0xB8
#define gZclAttrIdMetBISCurrNoTier11Block10SummationReceived_c         0xB9
#define gZclAttrIdMetBISCurrNoTier11Block11SummationReceived_c         0xBA
#define gZclAttrIdMetBISCurrNoTier11Block12SummationReceived_c         0xBB
#define gZclAttrIdMetBISCurrNoTier11Block13SummationReceived_c         0xBC
#define gZclAttrIdMetBISCurrNoTier11Block14SummationReceived_c         0xBD
#define gZclAttrIdMetBISCurrNoTier11Block15SummationReceived_c         0xBE
#define gZclAttrIdMetBISCurrNoTier11Block16SummationReceived_c         0xBF
#define gZclAttrIdMetBISCurrNoTier12Block1SummationReceived_c          0xC0
#define gZclAttrIdMetBISCurrNoTier12Block2SummationReceived_c          0xC1
#define gZclAttrIdMetBISCurrNoTier12Block3SummationReceived_c          0xC2
#define gZclAttrIdMetBISCurrNoTier12Block4SummationReceived_c          0xC3
#define gZclAttrIdMetBISCurrNoTier12Block5SummationReceived_c          0xC4
#define gZclAttrIdMetBISCurrNoTier12Block6SummationReceived_c          0xC5
#define gZclAttrIdMetBISCurrNoTier12Block7SummationReceived_c          0xC6
#define gZclAttrIdMetBISCurrNoTier12Block8SummationReceived_c          0xC7
#define gZclAttrIdMetBISCurrNoTier12Block9SummationReceived_c          0xC8
#define gZclAttrIdMetBISCurrNoTier12Block10SummationReceived_c         0xC9
#define gZclAttrIdMetBISCurrNoTier12Block11SummationReceived_c         0xCA
#define gZclAttrIdMetBISCurrNoTier12Block12SummationReceived_c         0xCB
#define gZclAttrIdMetBISCurrNoTier12Block13SummationReceived_c         0xCC
#define gZclAttrIdMetBISCurrNoTier12Block14SummationReceived_c         0xCD
#define gZclAttrIdMetBISCurrNoTier12Block15SummationReceived_c         0xCE
#define gZclAttrIdMetBISCurrNoTier12Block16SummationReceived_c         0xCF
#define gZclAttrIdMetBISCurrNoTier13Block1SummationReceived_c          0xD0
#define gZclAttrIdMetBISCurrNoTier13Block2SummationReceived_c          0xD1
#define gZclAttrIdMetBISCurrNoTier13Block3SummationReceived_c          0xD2
#define gZclAttrIdMetBISCurrNoTier13Block4SummationReceived_c          0xD3
#define gZclAttrIdMetBISCurrNoTier13Block5SummationReceived_c          0xD4
#define gZclAttrIdMetBISCurrNoTier13Block6SummationReceived_c          0xD5
#define gZclAttrIdMetBISCurrNoTier13Block7SummationReceived_c          0xD6
#define gZclAttrIdMetBISCurrNoTier13Block8SummationReceived_c          0xD7
#define gZclAttrIdMetBISCurrNoTier13Block9SummationReceived_c          0xD8
#define gZclAttrIdMetBISCurrNoTier13Block10SummationReceived_c         0xD9
#define gZclAttrIdMetBISCurrNoTier13Block11SummationReceived_c         0xDA
#define gZclAttrIdMetBISCurrNoTier13Block12SummationReceived_c         0xDB
#define gZclAttrIdMetBISCurrNoTier13Block13SummationReceived_c         0xDC
#define gZclAttrIdMetBISCurrNoTier13Block14SummationReceived_c         0xDD
#define gZclAttrIdMetBISCurrNoTier13Block15SummationReceived_c         0xDE
#define gZclAttrIdMetBISCurrNoTier13Block16SummationReceived_c         0xDF
#define gZclAttrIdMetBISCurrNoTier14Block1SummationReceived_c          0xE0
#define gZclAttrIdMetBISCurrNoTier14Block2SummationReceived_c          0xE1
#define gZclAttrIdMetBISCurrNoTier14Block3SummationReceived_c          0xE2
#define gZclAttrIdMetBISCurrNoTier14Block4SummationReceived_c          0xE3
#define gZclAttrIdMetBISCurrNoTier14Block5SummationReceived_c          0xE4
#define gZclAttrIdMetBISCurrNoTier14Block6SummationReceived_c          0xE5
#define gZclAttrIdMetBISCurrNoTier14Block7SummationReceived_c          0xE6
#define gZclAttrIdMetBISCurrNoTier14Block8SummationReceived_c          0xE7
#define gZclAttrIdMetBISCurrNoTier14Block9SummationReceived_c          0xE8
#define gZclAttrIdMetBISCurrNoTier14Block10SummationReceived_c         0xE9
#define gZclAttrIdMetBISCurrNoTier14Block11SummationReceived_c         0xEA
#define gZclAttrIdMetBISCurrNoTier14Block12SummationReceived_c         0xEB
#define gZclAttrIdMetBISCurrNoTier14Block13SummationReceived_c         0xEC
#define gZclAttrIdMetBISCurrNoTier14Block14SummationReceived_c         0xED
#define gZclAttrIdMetBISCurrNoTier14Block15SummationReceived_c         0xEE
#define gZclAttrIdMetBISCurrNoTier14Block16SummationReceived_c         0xEF
#define gZclAttrIdMetBISCurrNoTier15Block1SummationReceived_c          0xF0
#define gZclAttrIdMetBISCurrNoTier15Block2SummationReceived_c          0xF1
#define gZclAttrIdMetBISCurrNoTier15Block3SummationReceived_c          0xF2
#define gZclAttrIdMetBISCurrNoTier15Block4SummationReceived_c          0xF3
#define gZclAttrIdMetBISCurrNoTier15Block5SummationReceived_c          0xF4
#define gZclAttrIdMetBISCurrNoTier15Block6SummationReceived_c          0xF5
#define gZclAttrIdMetBISCurrNoTier15Block7SummationReceived_c          0xF6
#define gZclAttrIdMetBISCurrNoTier15Block8SummationReceived_c          0xF7
#define gZclAttrIdMetBISCurrNoTier15Block9SummationReceived_c          0xF8
#define gZclAttrIdMetBISCurrNoTier15Block10SummationReceived_c         0xF9
#define gZclAttrIdMetBISCurrNoTier15Block11SummationReceived_c         0xFA
#define gZclAttrIdMetBISCurrNoTier15Block12SummationReceived_c         0xFB
#define gZclAttrIdMetBISCurrNoTier15Block13SummationReceived_c         0xFC
#define gZclAttrIdMetBISCurrNoTier15Block14SummationReceived_c         0xFD
#define gZclAttrIdMetBISCurrNoTier15Block15SummationReceived_c         0xFE
#define gZclAttrIdMetBISCurrNoTier15Block16SummationReceived_c         0xFF

/* Billing Attribute Set */
#define gZclAttrIdMetBillingBillToDate_c                               0x00
#define gZclAttrIdMetBillingBillToDateTimeStamp_c                      0x01
#define gZclAttrIdMetBillingProjectedBill_c                            0x02
#define gZclAttrIdMetBillingProjectedBillTimeStamp_c                   0x03

/* Metering full attribute ids */
#if ( TRUE == gBigEndian_c )
#define gZclAttrMetMSStatus_c                 0x0002
#define gZclAttrMetRISCurrSummDlvrd_c         0x0000
#define gZclAttrMetRISCurrSummRcvd_c          0x0100
#define gZclAttrMetRISSupplyStatus_c          0x1400
#define gZclAttrMetASGenericAlarmMask_c       0x0008
#define gZclAttrMetCFSUnitofMeasure_c         0x0003
#define gZclAttrMetCFSSummFormat_c            0x0303
#define gZclAttrMetCFSMetDevType_c            0x0603
#define gZclAttrMetRISFastPollUpdatePeriod_c  0x0B00
#define gZclAttrMetNotifCtlFlags_c            0x0000  
#define gZclAttrMetNotifFlags_c               0x0100  
#define gZclAttrMetPriceNotifCtlFlags_c       0x0200  
#define gZclAttrMetCalendarNotifCtlFlags_c    0x0300  
#define gZclAttrMetPrePayNotifCtlFlags_c      0x0400  
#define gZclAttrMetDevMgmtNotifCtlFlags_c     0x0500  
#else
#define gZclAttrMetMSStatus_c                 0x0200
#define gZclAttrMetRISCurrSummDlvrd_c         0x0000
#define gZclAttrMetRISCurrSummRcvd_c          0x0001
#define gZclAttrMetRISSupplyStatus_c          0x0014
#define gZclAttrMetASGenericAlarmMask_c       0x0800
#define gZclAttrMetCFSUnitofMeasure_c         0x0300
#define gZclAttrMetCFSSummFormat_c            0x0303
#define gZclAttrMetCFSMetDevType_c            0x0306
#define gZclAttrMetRISFastPollUpdatePeriod_c  0x000B
#define gZclAttrMetNotifCtlFlags_c            0x0000  
#define gZclAttrMetNotifFlags_c               0x0001  
#define gZclAttrMetPriceNotifCtlFlags_c       0x0002  
#define gZclAttrMetCalendarNotifCtlFlags_c    0x0003  
#define gZclAttrMetPrePayNotifCtlFlags_c      0x0004 
#define gZclAttrMetDevMgmtNotifCtlFlags_c     0x0005 
#endif /* #if ( TRUE == gBigEndian_c ) */

/* Mapping of the Meter Status Attribute */
#define gZclMSStatusCheckMeter_c              0x01
#define gZclMSStatusLowBattery_c              0x02 
#define gZclMSStatusTamperDetect_c            0x04 
#define gZclMSStatusPowerFailure_c            0x08 
#define gZclMSStatusPowerQuality_c            0x10 
#define gZclMSStatusLeakDetect_c              0x20 
#define gZclMSStatusServiceDisconnectOpen_c   0x40 


/* UnitofMeasure Attribute Enumerations */
#define  gZclAttrMetCFS_kWInPureBinary_c 0x00 /* kW (kilo-Watts) & kWh (kilo-WattHours) in pure Binary format.*/
#define  gZclAttrMetCFS_m3InPureBinary_c 0x01 /*m3 (Cubic Meter) & m3/h (Cubic Meter per Hour) in pure Binary format.*/
#define  gZclAttrMetCFS_ft3InPureBinary_c 0x02 /*ft3 (Cubic Feet) & ft3/h (Cubic Feet per Hour) in pure Binary format.*/
#define  gZclAttrMetCFS_ccfInPureBinary_c 0x03 /*ccf ((100 or Centum) Cubic Feet) & ccf/h ((100 or Centum) Cubic Feet per Hour) in pure Binary format.*/
#define  gZclAttrMetCFS_USglInPureBinary_c 0x04 /*US gl (US Gallons) & US gl/h (US Gallons per Hour) in pure Binary format.*/
#define  gZclAttrMetCFS_IMPglInPureBinary_c 0x05 /*IMP gl (Imperial Gallons) & IMP gl/h (Imperial Gallons per Hour) in pure Binary format.*/
#define  gZclAttrMetCFS_BTUsInPureBinary_c 0x06 /*BTUs & BTU/h in pure Binary format.*/
#define  gZclAttrMetCFS_LitersInPureBinary_c 0x07 /*Liters & l/h (Liters per Hour) in pure Binary format*/
#define  gZclAttrMetCFS_kPAGaugeInPureBinary_c 0x08 /*kPA(gauge) in pure Binary format.*/
#define  gZclAttrMetCFS_kPAAbsoluteInPureBinary_c 0x09 /*kPA(absolute) in pure Binary format.*/
#define  gZclAttrMetCFS_kWInBCD_c 0x80 /*kW (kilo-Watts) & kWh (kilo-WattHours) in BCD format*/
#define  gZclAttrMetCFS_m3InBCD_c 0x81 /*m3 (Cubic Meter) & m3/h (Cubic Meter per Hour) in BCD format*/
#define  gZclAttrMetCFS_ft3InBCD_c 0x82 /*ft3 (Cubic Feet) & ft3/h (Cubic Feet per Hour) in BCD format*/
#define  gZclAttrMetCFS_ccfInBCD_c 0x83 /*ccf ((100 or Centum) Cubic Feet) & ccf/h ((100 or Centum) Cubic Feet per Hour) in BCD format*/
#define  gZclAttrMetCFS_USglInBCD_c 0x84 /* US gl (US Gallons) & US gl/h (US Gallons per Hour) in BCD format*/
#define  gZclAttrMetCFS_IMPglInBCD_c 0x85 /* IMP gl (Imperial Gallons) & IMP gl/h (Imperial Gallons per Hour) in BCD format */
#define  gZclAttrMetCFS_BTUsInBCD_c 0x86 /* BTUs & BTU/h in BCD format */
#define  gZclAttrMetCFS_LitersInBCD_c 0x87 /* Liters & l/h (Liters per Hour) in BCD format */
#define  gZclAttrMetCFS_kPAGaugeInBCD_c 0x88 /* kPA(gauge) in BCD format. */
#define  gZclAttrMetCFS_kPAAbsoluteInBCD_c 0x89 /* kPA(absolute) in BCD format. */

/* Summation Formatting Attribute */
#define gZclAttrMetFormat_c      gMetFormat_c  

/* TemperatureUnitOfMeasure Enumeration  */
#define gZclAttrMetTUMKelvin_InPureBinary_c      0x00  /* K (Degrees Kelvin) in pure Binary format. */
#define gZclAttrMetTUMCelsius_InPureBinary_c     0x01  /* ?C (Degrees Celsius) in pure Binary format. */ 
#define gZclAttrMetTUMFahrenheit_InPureBinary_c  0x02  /* ?F (Degrees Fahrenheit) in pure Binary format. */ 
#define gZclAttrMetTUMKelvin_InBCD_c             0x80  /* K (Degrees Kelvin) in BCD format. */ 
#define gZclAttrMetTUMCelsius_InBCD_c            0x81  /* ?C (Degrees Celsius) in BCD format. */ 
#define gZclAttrMetTUMFahrenheit_InBCD_c         0x82  /* ?F (Degrees Fahrenheit) in BCD format. */ 

/* MeteringDeviceType Attribute Enumerations */
#define  gZclAttrMetCFSElectricMet_c         0x00
#define  gZclAttrMetCFSGasMet_C              0x01
#define  gZclAttrMetCFSWaterMet_c            0x02
#define  gZclAttrMetCFSThermalMet_c          0x03
#define  gZclAttrMetCFSPressureMet_c         0x04
#define  gZclAttrMetCFSHeatMet_c             0x05
#define  gZclAttrMetCFSCoolingMet_c          0x06
#define  gZclAttrMetCFSMirroredGasMet_c      0x80
#define  gZclAttrMetCFSMirroredWaterMet_c    0x81
#define  gZclAttrMetCFSMirroredThermalMet_c  0x82
#define  gZclAttrMetCFSMirroredPressureMet_c 0x83
#define  gZclAttrMetCFSMirroredHeatMet_c     0x84
#define  gZclAttrMetCFSMirroredCoolingMet_c  0x85

/* ProfileIntervalPeriod Timeframes */
#define gZclSEProfIntrvPeriod_Daily_c  0x00
#define gZclSEProfIntrvPeriod_60mins_c 0x01
#define gZclSEProfIntrvPeriod_30mins_c 0x02
#define gZclSEProfIntrvPeriod_15mins_c 0x03
#define gZclSEProfIntrvPeriod_10mins_c 0x04
#define gZclSEProfIntrvPeriod_7dot5mins_c 0x05
#define gZclSEProfIntrvPeriod_5mins_c     0x06
#define gZclSEProfIntrvPeriod_2dot5mins_c     0x07

/* Block Threshold Field Enumerations */
#define gBTFNoBlocksInUse_c                 0x00
#define gBTFNoBlock1_c                      0x01
#define gBTFNoBlock2_c                      0x02
#define gBTFNoBlock3_c                      0x03
#define gBTFNoBlock4_c                      0x04
#define gBTFNoBlock5_c                      0x05
#define gBTFNoBlock6_c                      0x06
#define gBTFNoBlock7_c                      0x07
#define gBTFNoBlock8_c                      0x08
#define gBTFNoBlock9_c                      0x09
#define gBTFNoBlock10_c                     0x0A
#define gBTFNoBlock11_c                     0x0B
#define gBTFNoBlock12_c                     0x0C
#define gBTFNoBlock13_c                     0x0D
#define gBTFNoBlock14_c                     0x0E
#define gBTFNoBlock15_c                     0x0F
#define gBTFNoBlock16_c                     0x10

/* Generic Alarm Group */
#define gGAGCheckMeter_c                     0x00 
#define gGAGLowBatery_c                      0x01 
#define gGAGTamperDetect_c                   0x02 
#define gGAGPowerFailure_c                   0x03
#define gGAGPipeEmpty_c                      0x03
#define gGAGTempSensor_c                     0x03
#define gGAGPowerQuality_c                   0x04
#define gGAGLowPressure_c                    0x04
#define gGAGBurstDetect_c                    0x04
#define gGAGLeakDetect_c                     0x05
#define gGAGServiceDisconnect_c              0x06
#define gGAGFlow_c                           0x07
#define gGAGFlowSensor_c                     0x07

/* Electricity Alarm Group */
#define gEAGLowVoltageL1_c                   0x10 
#define gEAGHighVoltageL1_c                  0x11
#define gEAGLowVoltageL2_c                   0x12 
#define gEAGHighVoltageL2_c                  0x13
#define gEAGLowVoltageL3_c                   0x14 
#define gEAGHighVoltageL3_c                  0x15
#define gEAGOverCurrentL1_c                  0x16 
#define gEAGOverCurrentL2_c                  0x17
#define gEAGOverCurrentL3_c                  0x18 
#define gEAGFrequencyTooLowL1_c              0x19
#define gEAGFrequencyTooHighL1_c             0x1A
#define gEAGFrequencyTooLowL2_c              0x1B
#define gEAGFrequencyTooHighL2_c             0x1C
#define gEAGFrequencyTooLowL3_c              0x1D
#define gEAGFrequencyTooHighL3_c             0x1E
#define gEAGGroundFault_c                    0x1F
#define gEAGElectricTamperDetect_c           0x20

/* Generic Flow/Presure Alarm Group */
#define gFPABurstDetect_c                    0x30
#define gFPAPresureTooLow_c                  0x31
#define gFPAPresureTooHigh_c                 0x32
#define gFPAFlowSensorCommError_c            0x33
#define gFPAFlowSensorMeasurementFault_c     0x34
#define gFPAFlowSensorReverseFlow_c          0x35
#define gFPAFlowSensorAirDetect_c            0x36
#define gFPAPipeEmpty_c                      0x37

/* Heat and Cooling Specific Alarm Group */
#define gWSAInletTemperatureSensorFault_c    0x50
#define gWSAOutletTemperatureSensorFault_c   0x51

/* Supply Status Attribute */
#define gSSASupplyOff_c         0x00
#define gSSASupplyOffArmed_c    0x01
#define gSSASupplyOn_c          0x02

/**************************************************
* Met Cluster 
**************************************************/

/* Table D7– Demand Response Client Cluster Attributes */
#define gZclAttrSetMetClient__c    0

/* Table D10– Metering Cluster Server Attribute Sets */
#define gZclAttrSetMetServer_ReadingInfo_c 0x00
#define gZclAttrSetMetServer_TOUInfo_c 0x01
#define gZclAttrSetMetServer_MeterStatus_c 0x02
#define gZclAttrSetMetServer_Formatting_c 0x03
#define gZclAttrSetMetServer_HistoricalConsmp_c 0x04
#define gZclAttrSetMetServer_LoadProfileCfg_c 0x05
#define gZclAttrSetMetServer_SupplyLimit_c 0x06
#define gZclAttrSetMetServer_BlockInfoDlvrd_c 0x07
#define gZclAttrSetMetServer_Alarms_c 0x08
#define gZclAttrSetMetServer_BlockInfoRcvd_c 0x09
#define gZclAttrSetMetServer_MeterBilling_c 0x0A

/* Table D55 – Metering Cluster Client Attribute Sets */
#define gZclAttrSetMetClient_Notif_c 0x00
#define gZclAttrSetMetClient_MirrorCfg_c 0x01

/* Table D7– Demand Response Client Cluster Attributes */
#define gZclAttrMet_UtilityEnrollmentGroup_c 0x0000
#define gZclAttrMet_StartRandomizeMinutes_c 0x0001
#define gZclAttrMet_StopRandomizeMnutes_c 0x0002
#define gZclAttrMet_DeviceClassValue_c 0x0003


/* Table D11– Reading Information Attribute Set */
#define gZclAttrMet_CurrSummDlvrd_c 0x0000
#define gZclAttrMet_CurrSummRcvd_c 0x0001
#define gZclAttrMet_CurrMaxDemandDlvrd_c 0x0002
#define gZclAttrMet_CurrMaxDemandRcvd_c 0x0003
#define gZclAttrMet_DFTSumm_c 0x0004
#define gZclAttrMet_DailyFreezeTime_c 0x0005
#define gZclAttrMet_PowerFactor_c 0x0006
#define gZclAttrMet_ReadingSnapShotTime_c 0x0007
#define gZclAttrMet_CurrMaxDemandDlvrdTime_c 0x0008
#define gZclAttrMet_CurrMaxDemandRcvdTime_c 0x0009
#define gZclAttrMet_DefaultUpdatePeriod_c 0x000A
#define gZclAttrMet_FastPollUpdatePeriod_c 0x000B
#define gZclAttrMet_CurrBlockPeriodConsmpDlvrd_c 0x000C
#define gZclAttrMet_DailyConsmpTarget_c 0x000D
#define gZclAttrMet_CurrBlock_c 0x000E
#define gZclAttrMet_ProfileIntervalPeriod_c 0x000F
#define gZclAttrMet_IntervalReadReportingPeriod_c 0x0010
#define gZclAttrMet_PresetReadingTime_c 0x0011
#define gZclAttrMet_VolumePerReport_c 0x0012
#define gZclAttrMet_FlowRestriction_c 0x0013
#define gZclAttrMet_SupplyStatus_c 0x0014
#define gZclAttrMet_CurrInletEnergyCarrierSumm_c 0x0015
#define gZclAttrMet_CurrOutletEnergyCarrierSumm_c 0x0016
#define gZclAttrMet_InletTemperature_c 0x0017
#define gZclAttrMet_OutletTemperature_c 0x0018
#define gZclAttrMet_ControlTemperature_c 0x0019
#define gZclAttrMet_CurrInletEnergyCarrierDemand_c 0x001A
#define gZclAttrMet_CurrOutletEnergyCarrierDemand_c 0x001B
#define gZclAttrMet_PrevBlockPeriodConsmpDlvrd_c 0x001C
#define gZclAttrMet_CurrBlockPeriodConsmpRcvd_c 0x001D
#define gZclAttrMet_CurrBlockRcvd_c 0x001E
#define gZclAttrMet_DFTSummRcvd_c 0x001F
#define gZclAttrMet_ActiveRegisterTierDlvrd_c 0x0020
#define gZclAttrMet_ActiveRegisterTierRcvd_c 0x0021
#define gZclAttrMet_LastBlockSwitchTime_c 0x0022


/* Table D14– TOU Information Attribute Set */
#define gZclAttrMet_CurrTier1SummDlvrd_c 0x0100
#define gZclAttrMet_CurrTier1SummRcvd_c 0x0101
#define gZclAttrMet_CurrTier2SummDlvrd_c 0x0102
#define gZclAttrMet_CurrTier2SummRcvd_c 0x0103
#define gZclAttrMet_CurrTier3SummDlvrd_c 0x0104
#define gZclAttrMet_CurrTier3SummRcvd_c 0x0105
#define gZclAttrMet_CurrTier4SummDlvrd_c 0x0106
#define gZclAttrMet_CurrTier4SummRcvd_c 0x0107
#define gZclAttrMet_CurrTier5SummDlvrd_c 0x0108
#define gZclAttrMet_CurrTier5SummRcvd_c 0x0109
#define gZclAttrMet_CurrTier6SummDlvrd_c 0x010A
#define gZclAttrMet_CurrTier6SummRcvd_c 0x010B
#define gZclAttrMet_CurrTier7SummDlvrd_c 0x010C
#define gZclAttrMet_CurrTier7SummRcvd_c 0x010D
#define gZclAttrMet_CurrTier8SummDlvrd_c 0x010E
#define gZclAttrMet_CurrTier8SummRcvd_c 0x010F
#define gZclAttrMet_CurrTier9SummDlvrd_c 0x0110
#define gZclAttrMet_CurrTier9SummRcvd_c 0x0111
#define gZclAttrMet_CurrTier10SummDlvrd_c 0x0112
#define gZclAttrMet_CurrTier10SummRcvd_c 0x0113
#define gZclAttrMet_CurrTier11SummDlvrd_c 0x0114
#define gZclAttrMet_CurrTier11SummRcvd_c 0x0115
#define gZclAttrMet_CurrTier12SummDlvrd_c 0x0116
#define gZclAttrMet_CurrTier12SummRcvd_c 0x0117
#define gZclAttrMet_CurrTier13SummDlvrd_c 0x0118
#define gZclAttrMet_CurrTier13SummRcvd_c 0x0119
#define gZclAttrMet_CurrTier14SummDlvrd_c 0x011A
#define gZclAttrMet_CurrTier14SummRcvd_c 0x011B
#define gZclAttrMet_CurrTier15SummDlvrd_c 0x011C
#define gZclAttrMet_CurrTier15SummRcvd_c 0x011D
#define gZclAttrMet_CurrTier16SummDlvrd_c 0x011E
#define gZclAttrMet_CurrTier16SummRcvd_c 0x011F
#define gZclAttrMet_CurrTier17SummDlvrd_c 0x0120
#define gZclAttrMet_CurrTier17SummRcvd_c 0x0121
#define gZclAttrMet_CurrTier48SummDlvrd_c 0x015E
#define gZclAttrMet_CurrTier48SummRcvd_c 0x015F
#define gZclAttrMet_CPP1SummDlvrd_c 0x01FC
#define gZclAttrMet_CPP2SummDlvrd_c 0x01FE


/* Table D15– Meter Status Attribute Set */
#define gZclAttrMet_Status_c 0x0200
#define gZclAttrMet_RemainingBatteryLife_c 0x0201
#define gZclAttrMet_HoursInOperation_c 0x0202
#define gZclAttrMet_HoursInFault_c 0x0203
#define gZclAttrMet_ExtendedStatus_c 0x0204
#define gZclAttrMet_RemainingBatteryLifeinDays_c 0x0205
#define gZclAttrMet_CurrMeterID_c 0x0206
#define gZclAttrMet_AmbientConsmpIndicator_c 0x0207


/* Table D26– Formatting Attribute Set */
#define gZclAttrMet_UnitofMeasure_c 0x0300
#define gZclAttrMet_Multiplier_c 0x0301
#define gZclAttrMet_Divisor_c 0x0302
#define gZclAttrMet_SummFormatting_c 0x0303
#define gZclAttrMet_DemandFormatting_c 0x0304
#define gZclAttrMet_HistoricalConsmpFormatting_c 0x0305
#define gZclAttrMet_MeteringDeviceType_c 0x0306
#define gZclAttrMet_SiteID_c 0x0307
#define gZclAttrMet_MeterSerialNumber_c 0x0308
#define gZclAttrMet_EnergyCarrierUnitOfMeasure_c 0x0309
#define gZclAttrMet_EnergyCarrierSummFormatting_c 0x030A
#define gZclAttrMet_EnergyCarrierDemandFormatting_c 0x030B
#define gZclAttrMet_TemperatureUnitOfMeasure_c 0x030C
#define gZclAttrMet_TemperatureFormatting_c 0x030D
#define gZclAttrMet_ModuleSerialNumber_c 0x030E
#define gZclAttrMet_OperatingTariffLabelDlvrd_c 0x030F
#define gZclAttrMet_OperatingTariffLabelRcvd_c 0x0310


/* Table D30– Historical Attribute Set */
#define gZclAttrMet_InstantaneousDemand_c 0x0400
#define gZclAttrMet_CurrDayConsmpDlvrd_c 0x0401
#define gZclAttrMet_CurrDayConsmpRcvd_c 0x0402
#define gZclAttrMet_PrevDayConsmpDlvrd_c 0x0403
#define gZclAttrMet_PrevDayConsmpRcvd_c 0x0404
#define gZclAttrMet_CurrPartialProfileIntervalStartTimeDlvrd_c 0x0405
#define gZclAttrMet_CurrPartialProfileIntervalStartTimeRcvd_c 0x0406
#define gZclAttrMet_CurrPartialProfileIntervalValueDlvrd_c 0x0407
#define gZclAttrMet_CurrPartialProfileIntervalValueRcvd_c 0x0408
#define gZclAttrMet_CurrDayMaxPressure_c 0x0409
#define gZclAttrMet_CurrDayMinPressure_c 0x040A
#define gZclAttrMet_PrevDayMaxPresure_c 0x040B
#define gZclAttrMet_PrevDayMinPressure_c 0x040C
#define gZclAttrMet_CurrDayMaxDemand_c 0x040D
#define gZclAttrMet_PrevDayMaxDemand_c 0x040E
#define gZclAttrMet_CurrMonthMaxDemand_c 0x040F
#define gZclAttrMet_CurrYearMaxDemand_c 0x0410
#define gZclAttrMet_CurrDayMaxEnergyCarrierDemand_c 0x0411
#define gZclAttrMet_PrevDayMaxEnergyCarrierDemand_c 0x0412
#define gZclAttrMet_CurrMonthMaxEnergyCarrierDemand_c 0x0413
#define gZclAttrMet_CurrMonthMinEnergyCarrierDemand_c 0x0414
#define gZclAttrMet_CurrYearMaxEnergyCarrierDemand_c 0x0415
#define gZclAttrMet_CurrYearMinEnergyCarrierDemand_c 0x0416
#define gZclAttrMet_PrevDay2ConsmpDlvrd_c 0x0420
#define gZclAttrMet_PrevDay2ConsmpRcvd_c 0x0421
#define gZclAttrMet_PrevDay3ConsmpDlvrd_c 0x0422
#define gZclAttrMet_PrevDay3ConsmpRcvd_c 0x0423
#define gZclAttrMet_PrevDay4ConsmpDlvrd_c 0x0424
#define gZclAttrMet_PrevDay4ConsmpRcvd_c 0x0425
#define gZclAttrMet_PrevDay5ConsmpDlvrd_c 0x0426
#define gZclAttrMet_PrevDay5ConsmpRcvd_c 0x0427
#define gZclAttrMet_PrevDay6ConsmpDlvrd_c 0x0428
#define gZclAttrMet_PrevDay6ConsmpRcvd_c 0x0429
#define gZclAttrMet_PrevDay7ConsmpDlvrd_c 0x042A
#define gZclAttrMet_PrevDay7ConsmpRcvd_c 0x042B
#define gZclAttrMet_PrevDay8ConsmpDlvrd_c 0x042C
#define gZclAttrMet_PrevDay8ConsmpRcvd_c 0x042D
#define gZclAttrMet_CurrWeekConsmpDlvrd_c 0x0430
#define gZclAttrMet_CurrWeekConsmpRcvd_c 0x0431
#define gZclAttrMet_PrevWeekConsmpDlvrd_c 0x0432
#define gZclAttrMet_PrevWeekConsmpRcvd_c 0x0433
#define gZclAttrMet_PrevWeek2ConsmpDlvrd_c 0x0434
#define gZclAttrMet_PrevWeek2ConsmpRcvd_c 0x0435
#define gZclAttrMet_PrevWeek3ConsmpDlvrd_c 0x0436
#define gZclAttrMet_PrevWeek3ConsmpRcvd_c 0x0437
#define gZclAttrMet_PrevWeek4ConsmpDlvrd_c 0x0438
#define gZclAttrMet_PrevWeek4ConsmpRcvd_c 0x0439
#define gZclAttrMet_PrevWeek5ConsmpDlvrd_c 0x043A
#define gZclAttrMet_PrevWeek5ConsmpRcvd_c 0x043B
#define gZclAttrMet_CurrMonthConsmpDlvrd_c 0x0440
#define gZclAttrMet_CurrMonthConsmpRcvd_c 0x0441
#define gZclAttrMet_PrevMonthConsmpDlvrd_c 0x0442
#define gZclAttrMet_PrevMonthConsmpRcvd_c 0x0443
#define gZclAttrMet_PrevMonth2ConsmpDlvrd_c 0x0444
#define gZclAttrMet_PrevMonth2ConsmpRcvd_c 0x0445
#define gZclAttrMet_PrevMonth3ConsmpDlvrd_c 0x0446
#define gZclAttrMet_PrevMonth3ConsmpRcvd_c 0x0447
#define gZclAttrMet_PrevMonth4ConsmpDlvrd_c 0x0448
#define gZclAttrMet_PrevMonth4ConsmpRcvd_c 0x0449
#define gZclAttrMet_PrevMonth5ConsmpDlvrd_c 0x044A
#define gZclAttrMet_PrevMonth5ConsmpRcvd_c 0x044B
#define gZclAttrMet_PrevMonth6ConsmpDlvrd_c 0x044C
#define gZclAttrMet_PrevMonth6ConsmpRcvd_c 0x044D
#define gZclAttrMet_PrevMonth7ConsmpDlvrd_c 0x044E
#define gZclAttrMet_PrevMonth7ConsmpRcvd_c 0x044F
#define gZclAttrMet_PrevMonth8ConsmpDlvrd_c 0x0450
#define gZclAttrMet_PrevMonth8ConsmpRcvd_c 0x0451
#define gZclAttrMet_PrevMonth9ConsmpDlvrd_c 0x0452
#define gZclAttrMet_PrevMonth9ConsmpRcvd_c 0x0453
#define gZclAttrMet_PrevMonth10ConsmpDlvrd_c 0x0454
#define gZclAttrMet_PrevMonth10ConsmpRcvd_c 0x0455
#define gZclAttrMet_PrevMonth11ConsmpDlvrd_c 0x0456
#define gZclAttrMet_PrevMonth11ConsmpRcvd_c 0x0457
#define gZclAttrMet_PrevMonth12ConsmpDlvrd_c 0x0458
#define gZclAttrMet_PrevMonth12ConsmpRcvd_c 0x0459
#define gZclAttrMet_PrevMonth13ConsmpDlvrd_c 0x045A
#define gZclAttrMet_PrevMonth13ConsmpRcvd_c 0x045B


/* Table D31– Load Profile Configuration Attribute Set */
#define gZclAttrMet_MaxNumberOfPeriodsDlvrd_c 0x0500


/* Table D32– Supply Limit Attribute Set */
#define gZclAttrMet_CurrDemandDlvrd_c 0x0600
#define gZclAttrMet_DemandLimit_c 0x0601
#define gZclAttrMet_DemandIntegrationPeriod_c 0x0602
#define gZclAttrMet_NumberOfDemandSubintervals_c 0x0603
#define gZclAttrMet_DemandLimitArmDurationinminutes_c 0x0604
#define gZclAttrMet_LoadLimitSupplyState_c 0x0605
#define gZclAttrMet_LoadLimitCounter_c 0x0606


/* Table D33– Block Information Attribute Set (Delivered) */
#define gZclAttrMet_CurrNoTierBlock1SummDlvrd_c 0x0700
#define gZclAttrMet_CurrNoTierBlock2SummDlvrd_c 0x0701
#define gZclAttrMet_CurrNoTierBlock3SummDlvrd_c 0x0702
#define gZclAttrMet_CurrNoTierBlock16SummDlvrd_c 0x070F
#define gZclAttrMet_CurrTier1Block1SummDlvrd_c 0x0710
#define gZclAttrMet_CurrTier1Block2SummDlvrd_c 0x0711
#define gZclAttrMet_CurrTier1Block3SummDlvrd_c 0x0712
#define gZclAttrMet_CurrTier1Block16SummDlvrd_c 0x071F
#define gZclAttrMet_CurrTier2Block1SummDlvrd_c 0x0720
#define gZclAttrMet_CurrTier2Block16SummDlvrd_c 0x072F
#define gZclAttrMet_CurrTier3Block1SummDlvrd_c 0x0730
#define gZclAttrMet_CurrTier3Block16SummDlvrd_c 0x073F
#define gZclAttrMet_CurrTier4Block1SummDlvrd_c 0x0740
#define gZclAttrMet_CurrTier4Block16SummDlvrd_c 0x074F
#define gZclAttrMet_CurrTier5Block1SummDlvrd_c 0x0750
#define gZclAttrMet_CurrTier5Block16SummDlvrd_c 0x075F
#define gZclAttrMet_CurrTier6Block1SummDlvrd_c 0x0760
#define gZclAttrMet_CurrTier6Block16SummDlvrd_c 0x076F
#define gZclAttrMet_CurrTier7Block1SummDlvrd_c 0x0770
#define gZclAttrMet_CurrTier7Block16SummDlvrd_c 0x077F
#define gZclAttrMet_CurrTier8Block1SummDlvrd_c 0x0780
#define gZclAttrMet_CurrTier8Block16SummDlvrd_c 0x078F
#define gZclAttrMet_CurrTier9Block1SummDlvrd_c 0x0790
#define gZclAttrMet_CurrTier9Block16SummDlvrd_c 0x079F
#define gZclAttrMet_CurrTier10Block1SummDlvrd_c 0x07A0
#define gZclAttrMet_CurrTier10Block16SummDlvrd_c 0x07AF
#define gZclAttrMet_CurrTier11Block1SummDlvrd_c 0x07B0
#define gZclAttrMet_CurrTier11Block16SummDlvrd_c 0x07BF
#define gZclAttrMet_CurrTier12Block1SummDlvrd_c 0x07C0
#define gZclAttrMet_CurrTier12Block16SummDlvrd_c 0x07CF
#define gZclAttrMet_CurrTier13Block1SummDlvrd_c 0x07D0
#define gZclAttrMet_CurrTier13Block16SummDlvrd_c 0x07DF
#define gZclAttrMet_CurrTier14Block1SummDlvrd_c 0x07E0
#define gZclAttrMet_CurrTier14Block16SummDlvrd_c 0x07EF
#define gZclAttrMet_CurrTier15Block1SummDlvrd_c 0x07F0
#define gZclAttrMet_CurrTier15Block16SummDlvrd_c 0x07FF


/* Table D34– Alarm Attribute Set */
#define gZclAttrMet_GenericAlarmMask_c 0x0800
#define gZclAttrMet_ElectricityAlarmMask_c 0x0801
#define gZclAttrMet_GenericFlowPressureAlarmMask_c 0x0802
#define gZclAttrMet_WaterSpecificAlarmMask_c 0x0803
#define gZclAttrMet_HeatandCoolingSpecificAlarmMask_c 0x0804
#define gZclAttrMet_GasSpecificAlarmMask_c 0x0805
#define gZclAttrMet_ExtendedGenericAlarmMask_c 0x0806
#define gZclAttrMet_ManufacturerAlarmMask_c 0x0807


/* Table D44– Block Information Attribute Set (Received) */
#define gZclAttrMet_CurrNoTierBlock1SummRcvd_c 0x0900
#define gZclAttrMet_CurrNoTierBlock2SummRcvd_c 0x0901
#define gZclAttrMet_CurrNoTierBlock3SummRcvd_c 0x0902
#define gZclAttrMet_CurrNoTierBlock16SummRcvd_c 0x090F
#define gZclAttrMet_CurrTier1Block1SummRcvd_c 0x0910
#define gZclAttrMet_CurrTier1Block2SummRcvd_c 0x0911
#define gZclAttrMet_CurrTier1Block3SummRcvd_c 0x0912
#define gZclAttrMet_CurrTier1Block16SummRcvd_c 0x091F
#define gZclAttrMet_CurrTier2Block1SummRcvd_c 0x0920
#define gZclAttrMet_CurrTier2Block16SummRcvd_c 0x092F
#define gZclAttrMet_CurrTier3Block1SummRcvd_c 0x0930
#define gZclAttrMet_CurrTier3Block16SummRcvd_c 0x093F
#define gZclAttrMet_CurrTier4Block1SummRcvd_c 0x0940
#define gZclAttrMet_CurrTier4Block16SummRcvd_c 0x094F
#define gZclAttrMet_CurrTier5Block1SummRcvd_c 0x0950
#define gZclAttrMet_CurrTier5Block16SummRcvd_c 0x095F
#define gZclAttrMet_CurrTier6Block1SummRcvd_c 0x0960
#define gZclAttrMet_CurrTier6Block16SummRcvd_c 0x096F
#define gZclAttrMet_CurrTier7Block1SummRcvd_c 0x0970
#define gZclAttrMet_CurrTier7Block16SummRcvd_c 0x097F
#define gZclAttrMet_CurrTier8Block1SummRcvd_c 0x0980
#define gZclAttrMet_CurrTier8Block16SummRcvd_c 0x098F
#define gZclAttrMet_CurrTier9Block1SummRcvd_c 0x0990
#define gZclAttrMet_CurrTier9Block16SummRcvd_c 0x099F
#define gZclAttrMet_CurrTier10Block1SummRcvd_c 0x09A0
#define gZclAttrMet_CurrTier10Block16SummRcvd_c 0x09AF
#define gZclAttrMet_CurrTier11Block1SummRcvd_c 0x09B0
#define gZclAttrMet_CurrTier11Block16SummRcvd_c 0x09BF
#define gZclAttrMet_CurrTier12Block1SummRcvd_c 0x09C0
#define gZclAttrMet_CurrTier12Block16SummRcvd_c 0x09CF
#define gZclAttrMet_CurrTier13Block1SummRcvd_c 0x09D0
#define gZclAttrMet_CurrTier13Block16SummRcvd_c 0x09DF
#define gZclAttrMet_CurrTier14Block1SummRcvd_c 0x09E0
#define gZclAttrMet_CurrTier14Block16SummRcvd_c 0x09EF
#define gZclAttrMet_CurrTier15Block1SummRcvd_c 0x09F0
#define gZclAttrMet_CurrTier15Block16SummRcvd_c 0x09FF


/* Table D45– Meter Billing Attribute Set */
#define gZclAttrMet_BillToDateDlvrd_c 0x0A00
#define gZclAttrMet_BillToDateTimeStampDlvrd_c 0x0A01
#define gZclAttrMet_ProjectedBillDlvrd_c 0x0A02
#define gZclAttrMet_ProjectedBillTimeStampDlvrd_c 0x0A03
#define gZclAttrMet_BillDlvrdTrailingDigit_c 0x0A04
#define gZclAttrMet_BillToDateRcvd_c 0x0A10
#define gZclAttrMet_BillToDateTimeStampRcvd_c 0x0A11
#define gZclAttrMet_ProjectedBillRcvd_c 0x0A12
#define gZclAttrMet_ProjectedBillTimeStampRcvd_c 0x0A13
#define gZclAttrMet_BillRcvdTrailingDigit_c 0x0A14


/* Table D56 – Notification Attribute Set */
#define gZclAttrMet_FunctionalNotifFlags_c 0x0000
#define gZclAttrMet_NotifFlags2_c 0x0001
#define gZclAttrMet_NotifFlags3_c 0x0002
#define gZclAttrMet_NotifFlags4_c 0x0003
#define gZclAttrMet_NotifFlags5_c 0x0004
#define gZclAttrMet_NotifFlags6_c 0x0005
#define gZclAttrMet_NotifFlags7_c 0x0006
#define gZclAttrMet_NotifFlags8_c 0x0007


/* Table D61 – Mirror Configuration Attribute Set */
#define gZclAttrMet_ReportProfile_c 0x0100
#define gZclAttrMet_MirrorReporting_c 0x0101
#define gZclAttrMet_NotifScheme_c 0x0102

#define gZclAttrIdMet_UtilityEnrollmentGroup_c 0x00
#define gZclAttrIdMet_StartRandomizeMinutes_c 0x01
#define gZclAttrIdMet_StopRandomizeMnutes_c 0x02
#define gZclAttrIdMet_DeviceClassValue_c 0x03
#define gZclAttrIdMet_CurrSummDlvrd_c 0x00
#define gZclAttrIdMet_CurrSummRcvd_c 0x01
#define gZclAttrIdMet_CurrMaxDemandDlvrd_c 0x02
#define gZclAttrIdMet_CurrMaxDemandRcvd_c 0x03
#define gZclAttrIdMet_DFTSumm_c 0x04
#define gZclAttrIdMet_DailyFreezeTime_c 0x05
#define gZclAttrIdMet_PowerFactor_c 0x06
#define gZclAttrIdMet_ReadingSnapShotTime_c 0x07
#define gZclAttrIdMet_CurrMaxDemandDlvrdTime_c 0x08
#define gZclAttrIdMet_CurrMaxDemandRcvdTime_c 0x09
#define gZclAttrIdMet_DefaultUpdatePeriod_c 0x0A
#define gZclAttrIdMet_FastPollUpdatePeriod_c 0x0B
#define gZclAttrIdMet_CurrBlockPeriodConsmpDlvrd_c 0x0C
#define gZclAttrIdMet_DailyConsmpTarget_c 0x0D
#define gZclAttrIdMet_CurrBlock_c 0x0E
#define gZclAttrIdMet_ProfileIntervalPeriod_c 0x0F
#define gZclAttrIdMet_IntervalReadReportingPeriod_c 0x10
#define gZclAttrIdMet_PresetReadingTime_c 0x11
#define gZclAttrIdMet_VolumePerReport_c 0x12
#define gZclAttrIdMet_FlowRestriction_c 0x13
#define gZclAttrIdMet_SupplyStatus_c 0x14
#define gZclAttrIdMet_CurrInletEnergyCarrierSumm_c 0x15
#define gZclAttrIdMet_CurrOutletEnergyCarrierSumm_c 0x16
#define gZclAttrIdMet_InletTemperature_c 0x17
#define gZclAttrIdMet_OutletTemperature_c 0x18
#define gZclAttrIdMet_ControlTemperature_c 0x19
#define gZclAttrIdMet_CurrInletEnergyCarrierDemand_c 0x1A
#define gZclAttrIdMet_CurrOutletEnergyCarrierDemand_c 0x1B
#define gZclAttrIdMet_PrevBlockPeriodConsmpDlvrd_c 0x1C
#define gZclAttrIdMet_CurrBlockPeriodConsmpRcvd_c 0x1D
#define gZclAttrIdMet_CurrBlockRcvd_c 0x1E
#define gZclAttrIdMet_DFTSummRcvd_c 0x1F
#define gZclAttrIdMet_ActiveRegisterTierDlvrd_c 0x20
#define gZclAttrIdMet_ActiveRegisterTierRcvd_c 0x21
#define gZclAttrIdMet_LastBlockSwitchTime_c 0x22
#define gZclAttrIdMet_CurrTier1SummDlvrd_c 0x00
#define gZclAttrIdMet_CurrTier1SummRcvd_c 0x01
#define gZclAttrIdMet_CurrTier2SummDlvrd_c 0x02
#define gZclAttrIdMet_CurrTier2SummRcvd_c 0x03
#define gZclAttrIdMet_CurrTier3SummDlvrd_c 0x04
#define gZclAttrIdMet_CurrTier3SummRcvd_c 0x05
#define gZclAttrIdMet_CurrTier4SummDlvrd_c 0x06
#define gZclAttrIdMet_CurrTier4SummRcvd_c 0x07
#define gZclAttrIdMet_CurrTier5SummDlvrd_c 0x08
#define gZclAttrIdMet_CurrTier5SummRcvd_c 0x09
#define gZclAttrIdMet_CurrTier6SummDlvrd_c 0x0A
#define gZclAttrIdMet_CurrTier6SummRcvd_c 0x0B
#define gZclAttrIdMet_CurrTier7SummDlvrd_c 0x0C
#define gZclAttrIdMet_CurrTier7SummRcvd_c 0x0D
#define gZclAttrIdMet_CurrTier8SummDlvrd_c 0x0E
#define gZclAttrIdMet_CurrTier8SummRcvd_c 0x0F
#define gZclAttrIdMet_CurrTier9SummDlvrd_c 0x10
#define gZclAttrIdMet_CurrTier9SummRcvd_c 0x11
#define gZclAttrIdMet_CurrTier10SummDlvrd_c 0x12
#define gZclAttrIdMet_CurrTier10SummRcvd_c 0x13
#define gZclAttrIdMet_CurrTier11SummDlvrd_c 0x14
#define gZclAttrIdMet_CurrTier11SummRcvd_c 0x15
#define gZclAttrIdMet_CurrTier12SummDlvrd_c 0x16
#define gZclAttrIdMet_CurrTier12SummRcvd_c 0x17
#define gZclAttrIdMet_CurrTier13SummDlvrd_c 0x18
#define gZclAttrIdMet_CurrTier13SummRcvd_c 0x19
#define gZclAttrIdMet_CurrTier14SummDlvrd_c 0x1A
#define gZclAttrIdMet_CurrTier14SummRcvd_c 0x1B
#define gZclAttrIdMet_CurrTier15SummDlvrd_c 0x1C
#define gZclAttrIdMet_CurrTier15SummRcvd_c 0x1D
#define gZclAttrIdMet_CurrTier16SummDlvrd_c 0x1E
#define gZclAttrIdMet_CurrTier16SummRcvd_c 0x1F
#define gZclAttrIdMet_CurrTier17SummDlvrd_c 0x20
#define gZclAttrIdMet_CurrTier17SummRcvd_c 0x21
#define gZclAttrIdMet_CurrTier48SummDlvrd_c 0x5E
#define gZclAttrIdMet_CurrTier48SummRcvd_c 0x5F
#define gZclAttrIdMet_CPP1SummDlvrd_c 0xFC
#define gZclAttrIdMet_CPP2SummDlvrd_c 0xFE
#define gZclAttrIdMet_Status_c 0x00
#define gZclAttrIdMet_RemainingBatteryLife_c 0x01
#define gZclAttrIdMet_HoursInOperation_c 0x02
#define gZclAttrIdMet_HoursInFault_c 0x03
#define gZclAttrIdMet_ExtendedStatus_c 0x04
#define gZclAttrIdMet_RemainingBatteryLifeinDays_c 0x05
#define gZclAttrIdMet_CurrMeterID_c 0x06
#define gZclAttrIdMet_AmbientConsmpIndicator_c 0x07
#define gZclAttrIdMet_UnitofMeasure_c 0x00
#define gZclAttrIdMet_Multiplier_c 0x01
#define gZclAttrIdMet_Divisor_c 0x02
#define gZclAttrIdMet_SummFormatting_c 0x03
#define gZclAttrIdMet_DemandFormatting_c 0x04
#define gZclAttrIdMet_HistoricalConsmpFormatting_c 0x05
#define gZclAttrIdMet_MeteringDeviceType_c 0x06
#define gZclAttrIdMet_SiteID_c 0x07
#define gZclAttrIdMet_MeterSerialNumber_c 0x08
#define gZclAttrIdMet_EnergyCarrierUnitOfMeasure_c 0x09
#define gZclAttrIdMet_EnergyCarrierSummFormatting_c 0x0A
#define gZclAttrIdMet_EnergyCarrierDemandFormatting_c 0x0B
#define gZclAttrIdMet_TemperatureUnitOfMeasure_c 0x0C
#define gZclAttrIdMet_TemperatureFormatting_c 0x0D
#define gZclAttrIdMet_ModuleSerialNumber_c 0x0E
#define gZclAttrIdMet_OperatingTariffLabelDlvrd_c 0x0F
#define gZclAttrIdMet_OperatingTariffLabelRcvd_c 0x10
#define gZclAttrIdMet_InstantaneousDemand_c 0x00
#define gZclAttrIdMet_CurrDayConsmpDlvrd_c 0x01
#define gZclAttrIdMet_CurrDayConsmpRcvd_c 0x02
#define gZclAttrIdMet_PrevDayConsmpDlvrd_c 0x03
#define gZclAttrIdMet_PrevDayConsmpRcvd_c 0x04
#define gZclAttrIdMet_CurrPartialProfileIntervalStartTimeDlvrd_c 0x05
#define gZclAttrIdMet_CurrPartialProfileIntervalStartTimeRcvd_c 0x06
#define gZclAttrIdMet_CurrPartialProfileIntervalValueDlvrd_c 0x07
#define gZclAttrIdMet_CurrPartialProfileIntervalValueRcvd_c 0x08
#define gZclAttrIdMet_CurrDayMaxPressure_c 0x09
#define gZclAttrIdMet_CurrDayMinPressure_c 0x0A
#define gZclAttrIdMet_PrevDayMaxPresure_c 0x0B
#define gZclAttrIdMet_PrevDayMinPressure_c 0x0C
#define gZclAttrIdMet_CurrDayMaxDemand_c 0x0D
#define gZclAttrIdMet_PrevDayMaxDemand_c 0x0E
#define gZclAttrIdMet_CurrMonthMaxDemand_c 0x0F
#define gZclAttrIdMet_CurrYearMaxDemand_c 0x10
#define gZclAttrIdMet_CurrDayMaxEnergyCarrierDemand_c 0x11
#define gZclAttrIdMet_PrevDayMaxEnergyCarrierDemand_c 0x12
#define gZclAttrIdMet_CurrMonthMaxEnergyCarrierDemand_c 0x13
#define gZclAttrIdMet_CurrMonthMinEnergyCarrierDemand_c 0x14
#define gZclAttrIdMet_CurrYearMaxEnergyCarrierDemand_c 0x15
#define gZclAttrIdMet_CurrYearMinEnergyCarrierDemand_c 0x16
#define gZclAttrIdMet_PrevDay2ConsmpDlvrd_c 0x20
#define gZclAttrIdMet_PrevDay2ConsmpRcvd_c 0x21
#define gZclAttrIdMet_PrevDay3ConsmpDlvrd_c 0x22
#define gZclAttrIdMet_PrevDay3ConsmpRcvd_c 0x23
#define gZclAttrIdMet_PrevDay4ConsmpDlvrd_c 0x24
#define gZclAttrIdMet_PrevDay4ConsmpRcvd_c 0x25
#define gZclAttrIdMet_PrevDay5ConsmpDlvrd_c 0x26
#define gZclAttrIdMet_PrevDay5ConsmpRcvd_c 0x27
#define gZclAttrIdMet_PrevDay6ConsmpDlvrd_c 0x28
#define gZclAttrIdMet_PrevDay6ConsmpRcvd_c 0x29
#define gZclAttrIdMet_PrevDay7ConsmpDlvrd_c 0x2A
#define gZclAttrIdMet_PrevDay7ConsmpRcvd_c 0x2B
#define gZclAttrIdMet_PrevDay8ConsmpDlvrd_c 0x2C
#define gZclAttrIdMet_PrevDay8ConsmpRcvd_c 0x2D
#define gZclAttrIdMet_CurrWeekConsmpDlvrd_c 0x30
#define gZclAttrIdMet_CurrWeekConsmpRcvd_c 0x31
#define gZclAttrIdMet_PrevWeekConsmpDlvrd_c 0x32
#define gZclAttrIdMet_PrevWeekConsmpRcvd_c 0x33
#define gZclAttrIdMet_PrevWeek2ConsmpDlvrd_c 0x34
#define gZclAttrIdMet_PrevWeek2ConsmpRcvd_c 0x35
#define gZclAttrIdMet_PrevWeek3ConsmpDlvrd_c 0x36
#define gZclAttrIdMet_PrevWeek3ConsmpRcvd_c 0x37
#define gZclAttrIdMet_PrevWeek4ConsmpDlvrd_c 0x38
#define gZclAttrIdMet_PrevWeek4ConsmpRcvd_c 0x39
#define gZclAttrIdMet_PrevWeek5ConsmpDlvrd_c 0x3A
#define gZclAttrIdMet_PrevWeek5ConsmpRcvd_c 0x3B
#define gZclAttrIdMet_CurrMonthConsmpDlvrd_c 0x40
#define gZclAttrIdMet_CurrMonthConsmpRcvd_c 0x41
#define gZclAttrIdMet_PrevMonthConsmpDlvrd_c 0x42
#define gZclAttrIdMet_PrevMonthConsmpRcvd_c 0x43
#define gZclAttrIdMet_PrevMonth2ConsmpDlvrd_c 0x44
#define gZclAttrIdMet_PrevMonth2ConsmpRcvd_c 0x45
#define gZclAttrIdMet_PrevMonth3ConsmpDlvrd_c 0x46
#define gZclAttrIdMet_PrevMonth3ConsmpRcvd_c 0x47
#define gZclAttrIdMet_PrevMonth4ConsmpDlvrd_c 0x48
#define gZclAttrIdMet_PrevMonth4ConsmpRcvd_c 0x49
#define gZclAttrIdMet_PrevMonth5ConsmpDlvrd_c 0x4A
#define gZclAttrIdMet_PrevMonth5ConsmpRcvd_c 0x4B
#define gZclAttrIdMet_PrevMonth6ConsmpDlvrd_c 0x4C
#define gZclAttrIdMet_PrevMonth6ConsmpRcvd_c 0x4D
#define gZclAttrIdMet_PrevMonth7ConsmpDlvrd_c 0x4E
#define gZclAttrIdMet_PrevMonth7ConsmpRcvd_c 0x4F
#define gZclAttrIdMet_PrevMonth8ConsmpDlvrd_c 0x50
#define gZclAttrIdMet_PrevMonth8ConsmpRcvd_c 0x51
#define gZclAttrIdMet_PrevMonth9ConsmpDlvrd_c 0x52
#define gZclAttrIdMet_PrevMonth9ConsmpRcvd_c 0x53
#define gZclAttrIdMet_PrevMonth10ConsmpDlvrd_c 0x54
#define gZclAttrIdMet_PrevMonth10ConsmpRcvd_c 0x55
#define gZclAttrIdMet_PrevMonth11ConsmpDlvrd_c 0x56
#define gZclAttrIdMet_PrevMonth11ConsmpRcvd_c 0x57
#define gZclAttrIdMet_PrevMonth12ConsmpDlvrd_c 0x58
#define gZclAttrIdMet_PrevMonth12ConsmpRcvd_c 0x59
#define gZclAttrIdMet_PrevMonth13ConsmpDlvrd_c 0x5A
#define gZclAttrIdMet_PrevMonth13ConsmpRcvd_c 0x5B
#define gZclAttrIdMet_MaxNumberOfPeriodsDlvrd_c 0x00
#define gZclAttrIdMet_CurrDemandDlvrd_c 0x00
#define gZclAttrIdMet_DemandLimit_c 0x01
#define gZclAttrIdMet_DemandIntegrationPeriod_c 0x02
#define gZclAttrIdMet_NumberOfDemandSubintervals_c 0x03
#define gZclAttrIdMet_DemandLimitArmDurationinminutes_c 0x04
#define gZclAttrIdMet_LoadLimitSupplyState_c 0x05
#define gZclAttrIdMet_LoadLimitCounter_c 0x06
#define gZclAttrIdMet_CurrNoTierBlock1SummDlvrd_c 0x00
#define gZclAttrIdMet_CurrNoTierBlock2SummDlvrd_c 0x01
#define gZclAttrIdMet_CurrNoTierBlock3SummDlvrd_c 0x02
#define gZclAttrIdMet_CurrNoTierBlock16SummDlvrd_c 0x0F
#define gZclAttrIdMet_CurrTier1Block1SummDlvrd_c 0x10
#define gZclAttrIdMet_CurrTier1Block2SummDlvrd_c 0x11
#define gZclAttrIdMet_CurrTier1Block3SummDlvrd_c 0x12
#define gZclAttrIdMet_CurrTier1Block16SummDlvrd_c 0x1F
#define gZclAttrIdMet_CurrTier2Block1SummDlvrd_c 0x20
#define gZclAttrIdMet_CurrTier2Block16SummDlvrd_c 0x2F
#define gZclAttrIdMet_CurrTier3Block1SummDlvrd_c 0x30
#define gZclAttrIdMet_CurrTier3Block16SummDlvrd_c 0x3F
#define gZclAttrIdMet_CurrTier4Block1SummDlvrd_c 0x40
#define gZclAttrIdMet_CurrTier4Block16SummDlvrd_c 0x4F
#define gZclAttrIdMet_CurrTier5Block1SummDlvrd_c 0x50
#define gZclAttrIdMet_CurrTier5Block16SummDlvrd_c 0x5F
#define gZclAttrIdMet_CurrTier6Block1SummDlvrd_c 0x60
#define gZclAttrIdMet_CurrTier6Block16SummDlvrd_c 0x6F
#define gZclAttrIdMet_CurrTier7Block1SummDlvrd_c 0x70
#define gZclAttrIdMet_CurrTier7Block16SummDlvrd_c 0x7F
#define gZclAttrIdMet_CurrTier8Block1SummDlvrd_c 0x80
#define gZclAttrIdMet_CurrTier8Block16SummDlvrd_c 0x8F
#define gZclAttrIdMet_CurrTier9Block1SummDlvrd_c 0x90
#define gZclAttrIdMet_CurrTier9Block16SummDlvrd_c 0x9F
#define gZclAttrIdMet_CurrTier10Block1SummDlvrd_c 0xA0
#define gZclAttrIdMet_CurrTier10Block16SummDlvrd_c 0xAF
#define gZclAttrIdMet_CurrTier11Block1SummDlvrd_c 0xB0
#define gZclAttrIdMet_CurrTier11Block16SummDlvrd_c 0xBF
#define gZclAttrIdMet_CurrTier12Block1SummDlvrd_c 0xC0
#define gZclAttrIdMet_CurrTier12Block16SummDlvrd_c 0xCF
#define gZclAttrIdMet_CurrTier13Block1SummDlvrd_c 0xD0
#define gZclAttrIdMet_CurrTier13Block16SummDlvrd_c 0xDF
#define gZclAttrIdMet_CurrTier14Block1SummDlvrd_c 0xE0
#define gZclAttrIdMet_CurrTier14Block16SummDlvrd_c 0xEF
#define gZclAttrIdMet_CurrTier15Block1SummDlvrd_c 0xF0
#define gZclAttrIdMet_CurrTier15Block16SummDlvrd_c 0xFF
#define gZclAttrIdMet_GenericAlarmMask_c 0x00
#define gZclAttrIdMet_ElectricityAlarmMask_c 0x01
#define gZclAttrIdMet_GenericFlowPressureAlarmMask_c 0x02
#define gZclAttrIdMet_WaterSpecificAlarmMask_c 0x03
#define gZclAttrIdMet_HeatandCoolingSpecificAlarmMask_c 0x04
#define gZclAttrIdMet_GasSpecificAlarmMask_c 0x05
#define gZclAttrIdMet_ExtendedGenericAlarmMask_c 0x06
#define gZclAttrIdMet_ManufacturerAlarmMask_c 0x07
#define gZclAttrIdMet_CurrNoTierBlock1SummRcvd_c 0x00
#define gZclAttrIdMet_CurrNoTierBlock2SummRcvd_c 0x01
#define gZclAttrIdMet_CurrNoTierBlock3SummRcvd_c 0x02
#define gZclAttrIdMet_CurrNoTierBlock16SummRcvd_c 0x0F
#define gZclAttrIdMet_CurrTier1Block1SummRcvd_c 0x10
#define gZclAttrIdMet_CurrTier1Block2SummRcvd_c 0x11
#define gZclAttrIdMet_CurrTier1Block3SummRcvd_c 0x12
#define gZclAttrIdMet_CurrTier1Block16SummRcvd_c 0x1F
#define gZclAttrIdMet_CurrTier2Block1SummRcvd_c 0x20
#define gZclAttrIdMet_CurrTier2Block16SummRcvd_c 0x2F
#define gZclAttrIdMet_CurrTier3Block1SummRcvd_c 0x30
#define gZclAttrIdMet_CurrTier3Block16SummRcvd_c 0x3F
#define gZclAttrIdMet_CurrTier4Block1SummRcvd_c 0x40
#define gZclAttrIdMet_CurrTier4Block16SummRcvd_c 0x4F
#define gZclAttrIdMet_CurrTier5Block1SummRcvd_c 0x50
#define gZclAttrIdMet_CurrTier5Block16SummRcvd_c 0x5F
#define gZclAttrIdMet_CurrTier6Block1SummRcvd_c 0x60
#define gZclAttrIdMet_CurrTier6Block16SummRcvd_c 0x6F
#define gZclAttrIdMet_CurrTier7Block1SummRcvd_c 0x70
#define gZclAttrIdMet_CurrTier7Block16SummRcvd_c 0x7F
#define gZclAttrIdMet_CurrTier8Block1SummRcvd_c 0x80
#define gZclAttrIdMet_CurrTier8Block16SummRcvd_c 0x8F
#define gZclAttrIdMet_CurrTier9Block1SummRcvd_c 0x90
#define gZclAttrIdMet_CurrTier9Block16SummRcvd_c 0x9F
#define gZclAttrIdMet_CurrTier10Block1SummRcvd_c 0xA0
#define gZclAttrIdMet_CurrTier10Block16SummRcvd_c 0xAF
#define gZclAttrIdMet_CurrTier11Block1SummRcvd_c 0xB0
#define gZclAttrIdMet_CurrTier11Block16SummRcvd_c 0xBF
#define gZclAttrIdMet_CurrTier12Block1SummRcvd_c 0xC0
#define gZclAttrIdMet_CurrTier12Block16SummRcvd_c 0xCF
#define gZclAttrIdMet_CurrTier13Block1SummRcvd_c 0xD0
#define gZclAttrIdMet_CurrTier13Block16SummRcvd_c 0xDF
#define gZclAttrIdMet_CurrTier14Block1SummRcvd_c 0xE0
#define gZclAttrIdMet_CurrTier14Block16SummRcvd_c 0xEF
#define gZclAttrIdMet_CurrTier15Block1SummRcvd_c 0xF0
#define gZclAttrIdMet_CurrTier15Block16SummRcvd_c 0xFF
#define gZclAttrIdMet_BillToDateDlvrd_c 0x00
#define gZclAttrIdMet_BillDlvrdTrailingDigit_c 0x04
#define gZclAttrIdMet_BillRcvdTrailingDigit_c 0x14
#define gZclAttrIdMet_FunctionalNotifFlags_c 0x00
#define gZclAttrIdMet_NotifFlags2_c 0x01
#define gZclAttrIdMet_NotifFlags3_c 0x02
#define gZclAttrIdMet_NotifFlags4_c 0x03
#define gZclAttrIdMet_NotifFlags5_c 0x04
#define gZclAttrIdMet_NotifFlags6_c 0x05
#define gZclAttrIdMet_NotifFlags7_c 0x06
#define gZclAttrIdMet_NotifFlags8_c 0x07
#define gZclAttrIdMet_ReportProfile_c 0x00
#define gZclAttrIdMet_MirrorReporting_c 0x01
#define gZclAttrIdMet_NotifScheme_c 0x02

/* Table D9– Event Status Field Values */
#define gZclMet_LoadControlEventCommandRcvd_c 0x01
#define gZclMet_EventStarted_c 0x02
#define gZclMet_EventCompleted_c 0x03
#define gZclMet_UserHasChosenTooptoutUserWillNotParticipateInThisEvent_c 0x04
#define gZclMet_UserHasChosenTooptinUserWillParticipateInThisEvent_c 0x05
#define gZclMet_TheEventHasBeenCancelled_c 0x06
#define gZclMet_TheEventHasBeenSuperseded_c 0x07
#define gZclMet_EventPartiallyCompletedWithUseroptout_c 0x08
#define gZclMet_EventPartiallyCompletedDueToUseroptin_c 0x09
#define gZclMet_EventCompletedNoUserParticipationpreviousoptout_c 0x0A
#define gZclMet_RejectedInvalidCancelCommanddefault_c 0xF8
#define gZclMet_RejectedInvalidCancelCommandinvalidEffectiveTime_c 0xF9
#define gZclMet_RejectedEventWasRcvdAfterItHadExpiredcurrentTimeStartTimeDuration_c 0xFB
#define gZclMet_RejectedInvalidCancelCommandundefinedEvent_c 0xFD
#define gZclMet_LoadControlEventCommandRejected_c 0xFE

/* Signature Type (mandatory): An 8-bit Unsigned integer enumerating the type of algorithm use to create the Signature. The enumerated values are: */
#define gZclMet_NoSignature_c 0x00
#define gZclMet_Ecdsa_c 0x01

/* Table D12– Block Enumerations */
#define gZclMet_NoBlocksInUse_c 0x00
#define gZclMet_Block1_c 0x01
#define gZclMet_Block2_c 0x02
#define gZclMet_Block3_c 0x03
#define gZclMet_Block4_c 0x04
#define gZclMet_Block5_c 0x05
#define gZclMet_Block6_c 0x06
#define gZclMet_Block7_c 0x07
#define gZclMet_Block8_c 0x08
#define gZclMet_Block9_c 0x09
#define gZclMet_Block10_c 0x0A
#define gZclMet_Block11_c 0x0B
#define gZclMet_Block12_c 0x0C
#define gZclMet_Block13_c 0x0D
#define gZclMet_Block14_c 0x0E
#define gZclMet_Block15_c 0x0F
#define gZclMet_Block16_c 0x10

/* Table D13– Supply Status Attribute Enumerations */
#define gZclMet_SupplyOff_c 0x00
#define gZclMet_SupplyOffarmed_c 0x01
#define gZclMet_SupplyOn_c 0x02

/* Table D24– LowMediumHighStatus Attribute */
#define gZclMet_LowEnergyUsage_c 0x00
#define gZclMet_MediumEnergyUsage_c 0x01
#define gZclMet_HighEnergyUsage_c 0x02

/* Table D27– UnitofMeasure Attribute Enumerations */
#define gZclMet_KwhkilowattHoursKwkilowattsInPureBinaryFormat_c 0x00
#define gZclMet_M3cubicMeterM3hcubicMeterPerHourInPureBinaryFormat_c 0x01
#define gZclMet_Ft3cubicFeetFt3hcubicFeetPerHourInPureBinaryFormat_c 0x02
#define gZclMet_Ccf100OrCentumCubicFeetCcfh100OrCentumCubicFeetPerhourInPureBinaryFormat_c 0x03
#define gZclMet_UsGlusGallonsUsGlhusGallonsPerHourInPureBinaryFormat_c 0x04
#define gZclMet_ImpGlimperialGallonsImpGlhimperialGallonsPerHourInPureBinaryFormat_c 0x05
#define gZclMet_BtusBtuhInPureBinaryFormat_c 0x06
#define gZclMet_LitersLhlitersPerHourInPureBinaryFormat_c 0x07
#define gZclMet_KpagaugeInPureBinaryFormat_c 0x08
#define gZclMet_KpaabsoluteInPureBinaryFormat_c 0x09
#define gZclMet_Mcf1000CubicFeetMcfh1000CubicFeetPerHourInPureBinaryFormat_c 0x0A
#define gZclMet_UnitlessInPureBinaryFormat_c 0x0B
#define gZclMet_MjmegaJouleAndMjsmegaJoulePerSecondmwInPureBinaryFormat_c 0x0C
#define gZclMet_KvarKvarhInBinaryFormat_c 0x0D
#define gZclMet_KwhkilowattHoursKwkilowattsInBcdFormat_c 0x80
#define gZclMet_M3cubicMeterM3hcubicMeterPerHourInBcdFormat_c 0x81
#define gZclMet_Ft3cubicFeetFt3hcubicFeetPerHourInBcdFormat_c 0x82
#define gZclMet_Ccf100OrCentumCubicFeetCcfh100OrCentumCubicFeetPerhourInBcdFormat_c 0x83
#define gZclMet_UsGlusGallonsUsGlhusGallonsPerHourInBcdFormat_c 0x84
#define gZclMet_ImpGlimperialGallonsImpGlhimperialGallonsPerHourInBcdformat_c 0x85
#define gZclMet_BtusBtuhInBcdFormat_c 0x86
#define gZclMet_LitersLhlitersPerHourInBcdFormat_c 0x87
#define gZclMet_KpagaugeInBcdFormat_c 0x88
#define gZclMet_KpaabsoluteInBcdFormat_c 0x89
#define gZclMet_Mcf1000CubicFeetMcfh1000CubicFeetPerHourInBcdFormat_c 0x8A
#define gZclMet_UnitlessInBcdFormat_c 0x8B
#define gZclMet_MjmegaJouleAndMjsmegaJoulePerSecondmwInBcdFormat_c 0x8C
#define gZclMet_KvarKvarhInBcdFormat_c 0x8D

/* Table D28– MeteringDeviceType Attribute */
#define gZclMet_ElectricMetering_c 0x0
#define gZclMet_GasMetering_c 0x1
#define gZclMet_WaterMetering_c 0x2
#define gZclMet_ThermalMeteringdeprecated_c 0x3
#define gZclMet_PressureMetering_c 0x4
#define gZclMet_HeatMetering_c 0x5
#define gZclMet_CoolingMetering_c 0x6
#define gZclMet_EndUseMeasurementDeviceeumdForMeteringElectricVehicleCharging_c 0x7
#define gZclMet_PvGenerationMetering_c 0x8
#define gZclMet_WindTurbineGenerationMetering_c 0x9
#define gZclMet_WaterTurbineGenerationMetering_c 0xA
#define gZclMet_MicroGenerationMetering_c 0xB
#define gZclMet_SolarHotWaterGenerationMetering_c 0xC
#define gZclMet_ElectricMeteringElementphase1_c 0xD
#define gZclMet_ElectricMeteringElementphase2_c 0xE
#define gZclMet_ElectricMeteringElementphase3_c 0xF
#define gZclMet_MirroredElectricMetering_c 0x7F
#define gZclMet_MirroredGasMetering_c 0x80
#define gZclMet_MirroredWaterMetering_c 0x81
#define gZclMet_MirroredThermalMeteringdeprecated_c 0x82
#define gZclMet_MirroredPressureMetering_c 0x83
#define gZclMet_MirroredHeatMetering_c 0x84
#define gZclMet_MirroredCoolingMetering_c 0x85
#define gZclMet_MirroredEndUseMeasurementDeviceeumdForMeteringElectricVehicleCharging_c 0x86
#define gZclMet_MirroredPvGenerationMetering_c 0x87
#define gZclMet_MirroredWindTurbineGenerationMetering_c 0x88
#define gZclMet_MirroredWaterTurbineGenerationMetering_c 0x89
#define gZclMet_MirroredMicroGenerationMetering_c 0x8A
#define gZclMet_MirroredSolarHotWaterGenerationMetering_c 0x8B
#define gZclMet_MirroredElectricMeteringElementphase1_c 0x8C
#define gZclMet_MirroredElectricMeteringElementphase2_c 0x8D
#define gZclMet_MirroredElectricMeteringElementphase3_c 0x8E

/* Table D29– TemperatureUnitOfMeasure Enumeration */
#define gZclMet_KdegreesKelvinInPureBinaryFormat_c 0x00
#define gZclMet_cdegreesCelsiusInPureBinaryFormat_c 0x01
#define gZclMet_fdegreesFahrenheitInPureBinaryFormat_c 0x02
#define gZclMet_KdegreesKelvinInBcdFormat_c 0x80
#define gZclMet_cdegreesCelsiusInBcdFormat_c 0x81
#define gZclMet_fdegreesFahrenheitInBcdFormat_c 0x82

/* Table D47– Status Field Values */
#define gZclMet_Success_c 0x00
#define gZclMet_UndefinedIntervalChannelRequested_c 0x01
#define gZclMet_IntervalChannelNotSupported_c 0x02
#define gZclMet_InvalidEndTime_c 0x03
#define gZclMet_MorePeriodsRequestedThanCanBeReturned_c 0x04
#define gZclMet_NoIntervalsAvailableForTheRequestedTime_c 0x05

/* Table D48– ProfileIntervalPeriod Timeframes */
#define gZclMet_Daily_c 0x0
#define gZclMet_60Minutes_c 0x1
#define gZclMet_30Minutes_c 0x2
#define gZclMet_15Minutes_c 0x3
#define gZclMet_10Minutes_c 0x4
#define gZclMet_75Minutes_c 0x5
#define gZclMet_5Minutes_c 0x6
#define gZclMet_25Minutes_c 0x7

/* Table D49– Snapshot Schedule Confirmation */
#define gZclMet_Accepted_c 0x00
#define gZclMet_SnapshotTypeNotSupported_c 0x01
#define gZclMet_SnapshotCnfCauseNotSupported_c 0x02
#define gZclMet_SnapshotScheduleNotCurrlyAvailable_c 0x03
#define gZclMet_SnapshotSchedulesNotSupportedByDevice_c 0x04
#define gZclMet_InsufficientSpaceForSnapshotSchedule_c 0x05

/* Table D50– Snapshot Confirmation */
#define gZclMet_Accepted_c 0x00
#define gZclMet_SnapshotCauseNotSupported_c 0x01

/* Table D52– Snapshot Payload Type */
#define gZclMet_TouInfoSetDlvrdregisters_c 0x0
#define gZclMet_TouInfoSetRcvdRegisters_c 0x1
#define gZclMet_BlockTierInfoSetDlvrd_c 0x2
#define gZclMet_BlockTierInfoSetRcvd_c 0x3
#define gZclMet_DataUnavailable_c 0x80

/* SampleType (mandatory): An 8 bit enumeration that identifies the type of data being sampled. Possible values are defined in the following table:Table D53– Log Id Enumerations */
#define gZclMet_ConsmpDlvrd_c 0x0

/* Table D54– Notification Flags Order */
#define gZclMet_Notifflag1_c 0x0
#define gZclMet_Notifflag2_c 0x1
#define gZclMet_Notifflag3_c 0x2
#define gZclMet_Notifflag4_c 0x3
#define gZclMet_Notifflag5_c 0x4
#define gZclMet_Notifflag6_c 0x5
#define gZclMet_Notifflag7_c 0x6
#define gZclMet_Notifflag8_c 0x7
#define gZclMet_BlankNoNotifFlag_c 0

/* The NotificationScheme attribute allows for the pre-loading of the Notification Flags bit mapping to ZCL or Smart Energy Standard commands. The following schemes are currently supported within the Smart Energy Standard:- */
#define gZclMet_NoNotifSchemeDefined_c 0x00
#define gZclMet_PredefinedNotifSchemeA_c 0x01
#define gZclMet_PredefinedNotifSchemeB_c 0x02
#define gZclMet_ForMspRequirements_c 0x81

/* Table D63– Interval Channel Values */
#define gZclMet_ConsmpDlvrd_c 0x0
#define gZclMet_ConsmpRcvd_c 0x1
#define gZclMet_NotUsed_c 0

/* Table D131– Debt Recovery Method Enumerations */
#define gZclMet_TimeBased_c 0x00
#define gZclMet_PercentageBased_c 0x01
#define gZclMet_CatchupBasedfixedPeriod_c 0x02

/* Table D132–Recovery Frequency Field Enumerations */
#define gZclMet_PerHour_c 0x00
#define gZclMet_PerDay_c 0x01
#define gZclMet_PerWeek_c 0x02
#define gZclMet_PerMonth_c 0x03
#define gZclMet_PerQuarter_c 0x04

/**********************************************
Price cluster definitions
***********************************************/

#define gZclAttrClientPriceSet_c   0x00
#define gZclAttrIdClientPrice_PriceIncreaseRandomizeMinutes_c   0x00
#define gZclAttrIdClientPrice_PriceDecreaseRandomizeMinutes_c   0x01
#define gZclAttrIdClientPrice_CommodityType_c                   0x02

/* Price Server Attribute Sets */
#define gZclAttrPrice_TierLabelSet_c 		        0x00
#define gZclAttrPrice_BlockThresholdSet_c 	        0x01
#define gZclAttrPrice_BlockPeriodSet_c 		        0x02
#define gZclAttrPrice_CommodityTypeSet_c 	        0x03
#define gZclAttrPrice_BlockPriceInfoSet_c 	        0x04
#define gZclAttrPrice_ExtendedPriceInfoSet_c 	        0x05
#define gZclAttrPrice_TariffInfoSet_c 	                0x06
#define gZclAttrPrice_BillingInfoSet_c 	                0x07
#define gZclAttrPrice_CreditPaymentSet_c 	        0x08
#define gZclAttrPrice_TaxControlSet_c 	                0x09
#define gZclAttrPrice_ExportTierLabelSet_c 	        0x80
#define gZclAttrPrice_ExportBlockThresholdSet_c         0x81
#define gZclAttrPrice_ExportBlockPeriodSet_c 	        0x82
#define gZclAttrPrice_ExportBlockPriceInfoSet_c         0x84
#define gZclAttrPrice_ExportExtendedPriceInfoSet_c      0x85
#define gZclAttrPrice_ExportTariffInfoSet_c 	        0x86
#define gZclAttrPrice_ExportBillingInfoSet_c 	        0x87

/* Price Server Tier Label Set */
#define gZclAttrIdPrice_Tier1PriceLabel_c   0x00
#define gZclAttrIdPrice_Tier2PriceLabel_c   0x01
#define gZclAttrIdPrice_Tier3PriceLabel_c   0x02
#define gZclAttrIdPrice_Tier4PriceLabel_c   0x03
#define gZclAttrIdPrice_Tier5PriceLabel_c   0x04
#define gZclAttrIdPrice_Tier6PriceLabel_c   0x05
#define gZclAttrIdPrice_Tier7PriceLabel_c   0x06
#define gZclAttrIdPrice_Tier8PriceLabel_c   0x07
#define gZclAttrIdPrice_Tier9PriceLabel_c   0x08
#define gZclAttrIdPrice_Tier10PriceLabel_c  0x09
#define gZclAttrIdPrice_Tier11PriceLabel_c  0x0A
#define gZclAttrIdPrice_Tier12PriceLabel_c  0x0B
#define gZclAttrIdPrice_Tier13PriceLabel_c  0x0C
#define gZclAttrIdPrice_Tier14PriceLabel_c  0x0D
#define gZclAttrIdPrice_Tier15PriceLabel_c  0x0E
#define gZclAttrIdPrice_Tier16PriceLabel_c  0x0F
#define gZclAttrIdPrice_Tier17PriceLabel_c  0x10
#define gZclAttrIdPrice_Tier18PriceLabel_c  0x11
#define gZclAttrIdPrice_Tier19PriceLabel_c  0x12
#define gZclAttrIdPrice_Tier20PriceLabel_c  0x13
#define gZclAttrIdPrice_Tier21PriceLabel_c  0x14
#define gZclAttrIdPrice_Tier22PriceLabel_c  0x15
#define gZclAttrIdPrice_Tier23PriceLabel_c  0x16
#define gZclAttrIdPrice_Tier24PriceLabel_c  0x17
#define gZclAttrIdPrice_Tier25PriceLabel_c  0x18
#define gZclAttrIdPrice_Tier26PriceLabel_c  0x19
#define gZclAttrIdPrice_Tier27PriceLabel_c  0x1A
#define gZclAttrIdPrice_Tier28PriceLabel_c  0x1B
#define gZclAttrIdPrice_Tier29PriceLabel_c  0x1C
#define gZclAttrIdPrice_Tier30PriceLabel_c  0x1D
#define gZclAttrIdPrice_Tier31PriceLabel_c  0x1E
#define gZclAttrIdPrice_Tier32PriceLabel_c  0x1F
#define gZclAttrIdPrice_Tier33PriceLabel_c  0x20
#define gZclAttrIdPrice_Tier34PriceLabel_c  0x21
#define gZclAttrIdPrice_Tier35PriceLabel_c  0x22
#define gZclAttrIdPrice_Tier36PriceLabel_c  0x23
#define gZclAttrIdPrice_Tier37PriceLabel_c  0x24
#define gZclAttrIdPrice_Tier38PriceLabel_c  0x25
#define gZclAttrIdPrice_Tier39PriceLabel_c  0x26
#define gZclAttrIdPrice_Tier40PriceLabel_c  0x27
#define gZclAttrIdPrice_Tier41PriceLabel_c  0x28
#define gZclAttrIdPrice_Tier42PriceLabel_c  0x29
#define gZclAttrIdPrice_Tier43PriceLabel_c  0x2A
#define gZclAttrIdPrice_Tier44PriceLabel_c  0x2B
#define gZclAttrIdPrice_Tier45PriceLabel_c  0x2C
#define gZclAttrIdPrice_Tier46PriceLabel_c  0x2D
#define gZclAttrIdPrice_Tier47PriceLabel_c  0x2E
#define gZclAttrIdPrice_Tier48PriceLabel_c  0x2F


/* Price Server Block Threshold Set */
#define gZclAttrIdPrice_Block1Threshold_c   0x00
#define gZclAttrIdPrice_Block2Threshold_c   0x01
#define gZclAttrIdPrice_Block3Threshold_c   0x02
#define gZclAttrIdPrice_Block4Threshold_c   0x03
#define gZclAttrIdPrice_Block5Threshold_c   0x04
#define gZclAttrIdPrice_Block6Threshold_c   0x05
#define gZclAttrIdPrice_Block7Threshold_c   0x06
#define gZclAttrIdPrice_Block8Threshold_c   0x07
#define gZclAttrIdPrice_Block9Threshold_c   0x08
#define gZclAttrIdPrice_Block10Threshold_c  0x09
#define gZclAttrIdPrice_Block11Threshold_c  0x0A
#define gZclAttrIdPrice_Block12Threshold_c  0x0B
#define gZclAttrIdPrice_Block13Threshold_c  0x0C
#define gZclAttrIdPrice_Block14Threshold_c  0x0D
#define gZclAttrIdPrice_Block15Threshold_c  0x0E
#define gZclAttrIdPrice_BlockThresholdCount_c  0x0F
#define gZclAttrIdPrice_Tier1Block1Threshold_c          0x10
#define gZclAttrIdPrice_Tier1BlockThresholdCount_c      0x1F
#define gZclAttrIdPrice_Tier2Block1Threshold_c          0x20
#define gZclAttrIdPrice_Tier2BlockThresholdCount_c      0x2F
#define gZclAttrIdPrice_Tier3Block1Threshold_c          0x30
#define gZclAttrIdPrice_Tier3BlockThresholdCount_c      0x3F
#define gZclAttrIdPrice_Tier4Block1Threshold_c          0x40
#define gZclAttrIdPrice_Tier4BlockThresholdCount_c      0x4F
#define gZclAttrIdPrice_Tier5Block1Threshold_c          0x50
#define gZclAttrIdPrice_Tier5BlockThresholdCount_c      0x5F
#define gZclAttrIdPrice_Tier6Block1Threshold_c          0x60
#define gZclAttrIdPrice_Tier6BlockThresholdCount_c      0x6F
#define gZclAttrIdPrice_Tier7Block1Threshold_c          0x70
#define gZclAttrIdPrice_Tier7BlockThresholdCount_c      0x7F
#define gZclAttrIdPrice_Tier8Block1Threshold_c          0x80
#define gZclAttrIdPrice_Tier8BlockThresholdCount_c      0x8F
#define gZclAttrIdPrice_Tier9Block1Threshold_c          0x90
#define gZclAttrIdPrice_Tier9BlockThresholdCount_c      0x9F
#define gZclAttrIdPrice_Tier10Block1Threshold_c          0xA0
#define gZclAttrIdPrice_Tier10BlockThresholdCount_c      0xAF
#define gZclAttrIdPrice_Tier11Block1Threshold_c          0xB0
#define gZclAttrIdPrice_Tier11BlockThresholdCount_c      0xBF
#define gZclAttrIdPrice_Tier12Block1Threshold_c          0xC0
#define gZclAttrIdPrice_Tier12BlockThresholdCount_c      0xCF
#define gZclAttrIdPrice_Tier13Block1Threshold_c          0xD0
#define gZclAttrIdPrice_Tier13BlockThresholdCount_c      0xDF
#define gZclAttrIdPrice_Tier14Block1Threshold_c          0xE0
#define gZclAttrIdPrice_Tier14BlockThresholdCount_c      0xEF
#define gZclAttrIdPrice_Tier15Block1Threshold_c          0xF0
#define gZclAttrIdPrice_Tier15BlockThresholdCount_c      0xFF

/* Price Server Block Period Set */
#define gZclAttrIdPrice_StartofBlockPeriod_c    0x00
#define gZclAttrIdPrice_BlockPeriodDuration_c   0x01
#define gZclAttrIdPrice_ThresholdMultiplier_c   0x02
#define gZclAttrIdPrice_ThresholdDivisor_c      0x03

/* Price Server Commodity Type Set */
#define gZclAttrIdPrice_CommodityType_c             0x00
#define gZclAttrIdPrice_StandingCharge_c            0x01
#define gZclAttrIdPrice_ConversionFactor_c          0x02
#define gZclAttrIdPrice_ConversionFactorTrlDigit_c  0x03
#define gZclAttrIdPrice_CalorificValue_c            0x04
#define gZclAttrIdPrice_CalorificValueUnit_c        0x05
#define gZclAttrIdPrice_CalorificValueTrlDigit_c    0x06

/* Price Server Block Information Set */
#define gZclAttrIdPrice_NoTierBlock1Price_c  0x00
#define gZclAttrIdPrice_NoTierBlock2Price_c  0x01
#define gZclAttrIdPrice_NoTierBlock3Price_c  0x02
#define gZclAttrIdPrice_NoTierBlock4Price_c  0x03
#define gZclAttrIdPrice_NoTierBlock5Price_c  0x04
#define gZclAttrIdPrice_NoTierBlock6Price_c  0x05
#define gZclAttrIdPrice_NoTierBlock7Price_c  0x06
#define gZclAttrIdPrice_NoTierBlock8Price_c  0x07
#define gZclAttrIdPrice_NoTierBlock9Price_c  0x08
#define gZclAttrIdPrice_NoTierBlock10Price_c  0x09
#define gZclAttrIdPrice_NoTierBlock11Price_c  0x0A
#define gZclAttrIdPrice_NoTierBlock12Price_c  0x0B
#define gZclAttrIdPrice_NoTierBlock13Price_c  0x0C
#define gZclAttrIdPrice_NoTierBlock14Price_c  0x0D
#define gZclAttrIdPrice_NoTierBlock15Price_c  0x0E
#define gZclAttrIdPrice_NoTierBlock16Price_c  0x0F
#define gZclAttrIdPrice_Tier1Block1Price_c  0x10
#define gZclAttrIdPrice_Tier1Block2Price_c  0x11
#define gZclAttrIdPrice_Tier1Block3Price_c  0x12
#define gZclAttrIdPrice_Tier1Block4Price_c  0x13
#define gZclAttrIdPrice_Tier1Block5Price_c  0x14
#define gZclAttrIdPrice_Tier1Block6Price_c  0x15
#define gZclAttrIdPrice_Tier1Block7Price_c  0x16
#define gZclAttrIdPrice_Tier1Block8Price_c  0x17
#define gZclAttrIdPrice_Tier1Block9Price_c  0x18
#define gZclAttrIdPrice_Tier1Block10Price_c  0x19
#define gZclAttrIdPrice_Tier1Block11Price_c  0x1A
#define gZclAttrIdPrice_Tier1Block12Price_c  0x1B
#define gZclAttrIdPrice_Tier1Block13Price_c  0x1C
#define gZclAttrIdPrice_Tier1Block14Price_c  0x1D
#define gZclAttrIdPrice_Tier1Block15Price_c  0x1E
#define gZclAttrIdPrice_Tier1Block16Price_c  0x1F
#define gZclAttrIdPrice_Tier2Block1Price_c  0x20
#define gZclAttrIdPrice_Tier2Block2Price_c  0x21
#define gZclAttrIdPrice_Tier2Block3Price_c  0x22
#define gZclAttrIdPrice_Tier2Block4Price_c  0x23
#define gZclAttrIdPrice_Tier2Block5Price_c  0x24
#define gZclAttrIdPrice_Tier2Block6Price_c  0x25
#define gZclAttrIdPrice_Tier2Block7Price_c  0x26
#define gZclAttrIdPrice_Tier2Block8Price_c  0x27
#define gZclAttrIdPrice_Tier2Block9Price_c  0x28
#define gZclAttrIdPrice_Tier2Block10Price_c  0x29
#define gZclAttrIdPrice_Tier2Block11Price_c  0x2A
#define gZclAttrIdPrice_Tier2Block12Price_c  0x2B
#define gZclAttrIdPrice_Tier2Block13Price_c  0x2C
#define gZclAttrIdPrice_Tier2Block14Price_c  0x2D
#define gZclAttrIdPrice_Tier2Block15Price_c  0x2E
#define gZclAttrIdPrice_Tier2Block16Price_c  0x2F
#define gZclAttrIdPrice_Tier3Block1Price_c  0x30
#define gZclAttrIdPrice_Tier3Block2Price_c  0x31
#define gZclAttrIdPrice_Tier3Block3Price_c  0x32
#define gZclAttrIdPrice_Tier3Block4Price_c  0x33
#define gZclAttrIdPrice_Tier3Block5Price_c  0x34
#define gZclAttrIdPrice_Tier3Block6Price_c  0x35
#define gZclAttrIdPrice_Tier3Block7Price_c  0x36
#define gZclAttrIdPrice_Tier3Block8Price_c  0x37
#define gZclAttrIdPrice_Tier3Block9Price_c  0x38
#define gZclAttrIdPrice_Tier3Block10Price_c  0x39
#define gZclAttrIdPrice_Tier3Block11Price_c  0x3A
#define gZclAttrIdPrice_Tier3Block12Price_c  0x3B
#define gZclAttrIdPrice_Tier3Block13Price_c  0x3C
#define gZclAttrIdPrice_Tier3Block14Price_c  0x3D
#define gZclAttrIdPrice_Tier3Block15Price_c  0x3E
#define gZclAttrIdPrice_Tier3Block16Price_c  0x3F
#define gZclAttrIdPrice_Tier4Block1Price_c  0x40
#define gZclAttrIdPrice_Tier4Block2Price_c  0x41
#define gZclAttrIdPrice_Tier4Block3Price_c  0x42
#define gZclAttrIdPrice_Tier4Block4Price_c  0x43
#define gZclAttrIdPrice_Tier4Block5Price_c  0x44
#define gZclAttrIdPrice_Tier4Block6Price_c  0x45
#define gZclAttrIdPrice_Tier4Block7Price_c  0x46
#define gZclAttrIdPrice_Tier4Block8Price_c  0x47
#define gZclAttrIdPrice_Tier4Block9Price_c  0x48
#define gZclAttrIdPrice_Tier4Block10Price_c  0x49
#define gZclAttrIdPrice_Tier4Block11Price_c  0x4A
#define gZclAttrIdPrice_Tier4Block12Price_c  0x4B
#define gZclAttrIdPrice_Tier4Block13Price_c  0x4C
#define gZclAttrIdPrice_Tier4Block14Price_c  0x4D
#define gZclAttrIdPrice_Tier4Block15Price_c  0x4E
#define gZclAttrIdPrice_Tier4Block16Price_c  0x4F
#define gZclAttrIdPrice_Tier5Block1Price_c  0x50
#define gZclAttrIdPrice_Tier5Block2Price_c  0x51
#define gZclAttrIdPrice_Tier5Block3Price_c  0x52
#define gZclAttrIdPrice_Tier5Block4Price_c  0x53
#define gZclAttrIdPrice_Tier5Block5Price_c  0x54
#define gZclAttrIdPrice_Tier5Block6Price_c  0x55
#define gZclAttrIdPrice_Tier5Block7Price_c  0x56
#define gZclAttrIdPrice_Tier5Block8Price_c  0x57
#define gZclAttrIdPrice_Tier5Block9Price_c  0x58
#define gZclAttrIdPrice_Tier5Block10Price_c  0x59
#define gZclAttrIdPrice_Tier5Block11Price_c  0x5A
#define gZclAttrIdPrice_Tier5Block12Price_c  0x5B
#define gZclAttrIdPrice_Tier5Block13Price_c  0x5C
#define gZclAttrIdPrice_Tier5Block14Price_c  0x5D
#define gZclAttrIdPrice_Tier5Block15Price_c  0x5E
#define gZclAttrIdPrice_Tier5Block16Price_c  0x5F
#define gZclAttrIdPrice_Tier6Block1Price_c  0x60
#define gZclAttrIdPrice_Tier6Block2Price_c  0x61
#define gZclAttrIdPrice_Tier6Block3Price_c  0x62
#define gZclAttrIdPrice_Tier6Block4Price_c  0x63
#define gZclAttrIdPrice_Tier6Block5Price_c  0x64
#define gZclAttrIdPrice_Tier6Block6Price_c  0x65
#define gZclAttrIdPrice_Tier6Block7Price_c  0x66
#define gZclAttrIdPrice_Tier6Block8Price_c  0x67
#define gZclAttrIdPrice_Tier6Block9Price_c  0x68
#define gZclAttrIdPrice_Tier6Block10Price_c  0x69
#define gZclAttrIdPrice_Tier6Block11Price_c  0x6A
#define gZclAttrIdPrice_Tier6Block12Price_c  0x6B
#define gZclAttrIdPrice_Tier6Block13Price_c  0x6C
#define gZclAttrIdPrice_Tier6Block14Price_c  0x6D
#define gZclAttrIdPrice_Tier6Block15Price_c  0x6E
#define gZclAttrIdPrice_Tier6Block16Price_c  0x6F
#define gZclAttrIdPrice_Tier7Block1Price_c  0x70
#define gZclAttrIdPrice_Tier7Block2Price_c  0x71
#define gZclAttrIdPrice_Tier7Block3Price_c  0x72
#define gZclAttrIdPrice_Tier7Block4Price_c  0x73
#define gZclAttrIdPrice_Tier7Block5Price_c  0x74
#define gZclAttrIdPrice_Tier7Block6Price_c  0x75
#define gZclAttrIdPrice_Tier7Block7Price_c  0x76
#define gZclAttrIdPrice_Tier7Block8Price_c  0x77
#define gZclAttrIdPrice_Tier7Block9Price_c  0x78
#define gZclAttrIdPrice_Tier7Block10Price_c  0x79
#define gZclAttrIdPrice_Tier7Block11Price_c  0x7A
#define gZclAttrIdPrice_Tier7Block12Price_c  0x7B
#define gZclAttrIdPrice_Tier7Block13Price_c  0x7C
#define gZclAttrIdPrice_Tier7Block14Price_c  0x7D
#define gZclAttrIdPrice_Tier7Block15Price_c  0x7E
#define gZclAttrIdPrice_Tier7Block16Price_c  0x7F
#define gZclAttrIdPrice_Tier8Block1Price_c  0x80
#define gZclAttrIdPrice_Tier8Block2Price_c  0x81
#define gZclAttrIdPrice_Tier8Block3Price_c  0x82
#define gZclAttrIdPrice_Tier8Block4Price_c  0x83
#define gZclAttrIdPrice_Tier8Block5Price_c  0x84
#define gZclAttrIdPrice_Tier8Block6Price_c  0x85
#define gZclAttrIdPrice_Tier8Block7Price_c  0x86
#define gZclAttrIdPrice_Tier8Block8Price_c  0x87
#define gZclAttrIdPrice_Tier8Block9Price_c  0x88
#define gZclAttrIdPrice_Tier8Block10Price_c  0x89
#define gZclAttrIdPrice_Tier8Block11Price_c  0x8A
#define gZclAttrIdPrice_Tier8Block12Price_c  0x8B
#define gZclAttrIdPrice_Tier8Block13Price_c  0x8C
#define gZclAttrIdPrice_Tier8Block14Price_c  0x8D
#define gZclAttrIdPrice_Tier8Block15Price_c  0x8E
#define gZclAttrIdPrice_Tier8Block16Price_c  0x8F
#define gZclAttrIdPrice_Tier9Block1Price_c  0x90
#define gZclAttrIdPrice_Tier9Block2Price_c  0x91
#define gZclAttrIdPrice_Tier9Block3Price_c  0x92
#define gZclAttrIdPrice_Tier9Block4Price_c  0x93
#define gZclAttrIdPrice_Tier9Block5Price_c  0x94
#define gZclAttrIdPrice_Tier9Block6Price_c  0x95
#define gZclAttrIdPrice_Tier9Block7Price_c  0x96
#define gZclAttrIdPrice_Tier9Block8Price_c  0x97
#define gZclAttrIdPrice_Tier9Block9Price_c  0x98
#define gZclAttrIdPrice_Tier9Block10Price_c  0x99
#define gZclAttrIdPrice_Tier9Block11Price_c  0x9A
#define gZclAttrIdPrice_Tier9Block12Price_c  0x9B
#define gZclAttrIdPrice_Tier9Block13Price_c  0x9C
#define gZclAttrIdPrice_Tier9Block14Price_c  0x9D
#define gZclAttrIdPrice_Tier9Block15Price_c  0x9E
#define gZclAttrIdPrice_Tier9Block16Price_c  0x9F
#define gZclAttrIdPrice_Tier10Block1Price_c  0xA0
#define gZclAttrIdPrice_Tier10Block2Price_c  0xA1
#define gZclAttrIdPrice_Tier10Block3Price_c  0xA2
#define gZclAttrIdPrice_Tier10Block4Price_c  0xA3
#define gZclAttrIdPrice_Tier10Block5Price_c  0xA4
#define gZclAttrIdPrice_Tier10Block6Price_c  0xA5
#define gZclAttrIdPrice_Tier10Block7Price_c  0xA6
#define gZclAttrIdPrice_Tier10Block8Price_c  0xA7
#define gZclAttrIdPrice_Tier10Block9Price_c  0xA8
#define gZclAttrIdPrice_Tier10Block10Price_c  0xA9
#define gZclAttrIdPrice_Tier10Block11Price_c  0xAA
#define gZclAttrIdPrice_Tier10Block12Price_c  0xAB
#define gZclAttrIdPrice_Tier10Block13Price_c  0xAC
#define gZclAttrIdPrice_Tier10Block14Price_c  0xAD
#define gZclAttrIdPrice_Tier10Block15Price_c  0xAE
#define gZclAttrIdPrice_Tier10Block16Price_c  0xAF
#define gZclAttrIdPrice_Tier11Block1Price_c  0xB0
#define gZclAttrIdPrice_Tier11Block2Price_c  0xB1
#define gZclAttrIdPrice_Tier11Block3Price_c  0xB2
#define gZclAttrIdPrice_Tier11Block4Price_c  0xB3
#define gZclAttrIdPrice_Tier11Block5Price_c  0xB4
#define gZclAttrIdPrice_Tier11Block6Price_c  0xB5
#define gZclAttrIdPrice_Tier11Block7Price_c  0xB6
#define gZclAttrIdPrice_Tier11Block8Price_c  0xB7
#define gZclAttrIdPrice_Tier11Block9Price_c  0xB8
#define gZclAttrIdPrice_Tier11Block10Price_c  0xB9
#define gZclAttrIdPrice_Tier11Block11Price_c  0xBA
#define gZclAttrIdPrice_Tier11Block12Price_c  0xBB
#define gZclAttrIdPrice_Tier11Block13Price_c  0xBC
#define gZclAttrIdPrice_Tier11Block14Price_c  0xBD
#define gZclAttrIdPrice_Tier11Block15Price_c  0xBE
#define gZclAttrIdPrice_Tier11Block16Price_c  0xBF
#define gZclAttrIdPrice_Tier12Block1Price_c  0xC0
#define gZclAttrIdPrice_Tier12Block2Price_c  0xC1
#define gZclAttrIdPrice_Tier12Block3Price_c  0xC2
#define gZclAttrIdPrice_Tier12Block4Price_c  0xC3
#define gZclAttrIdPrice_Tier12Block5Price_c  0xC4
#define gZclAttrIdPrice_Tier12Block6Price_c  0xC5
#define gZclAttrIdPrice_Tier12Block7Price_c  0xC6
#define gZclAttrIdPrice_Tier12Block8Price_c  0xC7
#define gZclAttrIdPrice_Tier12Block9Price_c  0xC8
#define gZclAttrIdPrice_Tier12Block10Price_c  0xC9
#define gZclAttrIdPrice_Tier12Block11Price_c  0xCA
#define gZclAttrIdPrice_Tier12Block12Price_c  0xCB
#define gZclAttrIdPrice_Tier12Block13Price_c  0xCC
#define gZclAttrIdPrice_Tier12Block14Price_c  0xCD
#define gZclAttrIdPrice_Tier12Block15Price_c  0xCE
#define gZclAttrIdPrice_Tier12Block16Price_c  0xCF
#define gZclAttrIdPrice_Tier13Block1Price_c  0xD0
#define gZclAttrIdPrice_Tier13Block2Price_c  0xD1
#define gZclAttrIdPrice_Tier13Block3Price_c  0xD2
#define gZclAttrIdPrice_Tier13Block4Price_c  0xD3
#define gZclAttrIdPrice_Tier13Block5Price_c  0xD4
#define gZclAttrIdPrice_Tier13Block6Price_c  0xD5
#define gZclAttrIdPrice_Tier13Block7Price_c  0xD6
#define gZclAttrIdPrice_Tier13Block8Price_c  0xD7
#define gZclAttrIdPrice_Tier13Block9Price_c  0xD8
#define gZclAttrIdPrice_Tier13Block10Price_c  0xD9
#define gZclAttrIdPrice_Tier13Block11Price_c  0xDA
#define gZclAttrIdPrice_Tier13Block12Price_c  0xDB
#define gZclAttrIdPrice_Tier13Block13Price_c  0xDC
#define gZclAttrIdPrice_Tier13Block14Price_c  0xDD
#define gZclAttrIdPrice_Tier13Block15Price_c  0xDE
#define gZclAttrIdPrice_Tier13Block16Price_c  0xDF
#define gZclAttrIdPrice_Tier14Block1Price_c  0xE0
#define gZclAttrIdPrice_Tier14Block2Price_c  0xE1
#define gZclAttrIdPrice_Tier14Block3Price_c  0xE2
#define gZclAttrIdPrice_Tier14Block4Price_c  0xE3
#define gZclAttrIdPrice_Tier14Block5Price_c  0xE4
#define gZclAttrIdPrice_Tier14Block6Price_c  0xE5
#define gZclAttrIdPrice_Tier14Block7Price_c  0xE6
#define gZclAttrIdPrice_Tier14Block8Price_c  0xE7
#define gZclAttrIdPrice_Tier14Block9Price_c  0xE8
#define gZclAttrIdPrice_Tier14Block10Price_c  0xE9
#define gZclAttrIdPrice_Tier14Block11Price_c  0xEA
#define gZclAttrIdPrice_Tier14Block12Price_c  0xEB
#define gZclAttrIdPrice_Tier14Block13Price_c  0xEC
#define gZclAttrIdPrice_Tier14Block14Price_c  0xED
#define gZclAttrIdPrice_Tier14Block15Price_c  0xEE
#define gZclAttrIdPrice_Tier14Block16Price_c  0xEF
#define gZclAttrIdPrice_Tier15Block1Price_c  0xF0
#define gZclAttrIdPrice_Tier15Block2Price_c  0xF1
#define gZclAttrIdPrice_Tier15Block3Price_c  0xF2
#define gZclAttrIdPrice_Tier15Block4Price_c  0xF3
#define gZclAttrIdPrice_Tier15Block5Price_c  0xF4
#define gZclAttrIdPrice_Tier15Block6Price_c  0xF5
#define gZclAttrIdPrice_Tier15Block7Price_c  0xF6
#define gZclAttrIdPrice_Tier15Block8Price_c  0xF7
#define gZclAttrIdPrice_Tier15Block9Price_c  0xF8
#define gZclAttrIdPrice_Tier15Block10Price_c  0xF9
#define gZclAttrIdPrice_Tier15Block11Price_c  0xFA
#define gZclAttrIdPrice_Tier15Block12Price_c  0xFB
#define gZclAttrIdPrice_Tier15Block13Price_c  0xFC
#define gZclAttrIdPrice_Tier15Block14Price_c  0xFD
#define gZclAttrIdPrice_Tier15Block15Price_c  0xFE
#define gZclAttrIdPrice_Tier15Block16Price_c  0xFF

/* Price Server Extended Price Information Set */
#define gZclAttrIdPrice_PriceTier16_c   0x0F

/* Price Server Tariff Information Set */
#define gZclAttrIdPrice_TariffLabel_c             0x10
#define gZclAttrIdPrice_NoOfPriceTiersInUse_c     0x11
#define gZclAttrIdPrice_NoOfBlockTholdInUse_c     0x12
#define gZclAttrIdPrice_TierBlockMode_c           0x13
#define gZclAttrIdPrice_BlockTholdMask_c          0x14
#define gZclAttrIdPrice_UnitOfMeasure_c           0x15
#define gZclAttrIdPrice_Currency_c                0x16
#define gZclAttrIdPrice_PriceTrlDigit_c           0x17

/* Price Server Billing Information Attribute Set */
#define gZclAttrIdPrice_CurrBillingPeriodStartUTCTime_c         0x00
#define gZclAttrIdPrice_CurrBillPeriodDuration_c                0x01
#define gZclAttrIdPrice_LastBillPeriodStart_c                   0x02
#define gZclAttrIdPrice_LastBillPeriodDuration_c                0x03
#define gZclAttrIdPrice_LastBillPeriodConsolidatedBill_c        0x04

/* Price Server Credit Payment Attribute Set */
#define gZclAttrIdPrice_CreditPaymentDueDate_c          0x00
#define gZclAttrIdPrice_CreditPaymentStatus_c           0x01
#define gZclAttrIdPrice_CreditPaymentOverDueAmount_c    0x02
#define gZclAttrIdPrice_PaymentDiscount_c               0x0A
#define gZclAttrIdPrice_PaymentDiscountPeriod_c         0x0B
#define gZclAttrIdPrice_CreditPayment1_c                0x10
#define gZclAttrIdPrice_CreditPaymentDate1_c            0x11
#define gZclAttrIdPrice_CreditPaymentRef1_c             0x12
#define gZclAttrIdPrice_CreditPayment2_c                0x20
#define gZclAttrIdPrice_CreditPaymentDate2_c            0x21
#define gZclAttrIdPrice_CreditPaymentRef2_c             0x22
#define gZclAttrIdPrice_CreditPayment3_c                0x30
#define gZclAttrIdPrice_CreditPaymentDate3_c            0x31
#define gZclAttrIdPrice_CreditPaymentRef3_c             0x32
#define gZclAttrIdPrice_CreditPayment4_c                0x40
#define gZclAttrIdPrice_CreditPaymentDate4_c            0x41
#define gZclAttrIdPrice_CreditPaymentRef4_c             0x42
#define gZclAttrIdPrice_CreditPayment5_c                0x50    
#define gZclAttrIdPrice_CreditPaymentDate5_c            0x51
#define gZclAttrIdPrice_CreditPaymentRef5_c             0x52

/* Price Server Tax Control Attribute Set*/
#define gZclAttrIdPrice_DomesticPercentage_c            0x00
#define gZclAttrIdPrice_DomesticFuelTaxRate_c           0x01
#define gZclAttrIdPrice_DomesticFuelTaxRateTrlDgt       0x02

/* Price Server Export Tariff Attribute Set*/
#define gZclAttrIdPrice_RcvdTariffResPeriod_d           0x15
#define gZclAttrIdPrice_RcvdCO2_d                       0x25
#define gZclAttrIdPrice_RcvdCO2Unit_d                   0x26
#define gZclAttrIdPrice_RcvdCO2TrlDigit_d               0x27

#if gBigEndian_c
#define gZclAttrPrice_NoOfPriceTiersInUse_c     0x1106
#define gZclAttrPrice_NoOfBlockTholdInUse_c     0x1206
#define gZclAttrPrice_TierBlockMode_c           0x1306
#else
#define gZclAttrPrice_NoOfPriceTiersInUse_c     0x0611
#define gZclAttrPrice_NoOfBlockTholdInUse_c     0x0612
#define gZclAttrPrice_TierBlockMode_c           0x0613
#endif

#if gBigEndian_c
#define gZclAttrPrice_StartofBlockPeriod_c    0x0002
#define gZclAttrPrice_BlockPeriodDuration_c   0x0102
#define gZclAttrPrice_ThresholdMultiplier_c   0x0202
#define gZclAttrPrice_ThresholdDivisor_c      0x0302
#else
#define gZclAttrPrice_StartofBlockPeriod_c    0x0200
#define gZclAttrPrice_BlockPeriodDuration_c   0x0201
#define gZclAttrPrice_ThresholdMultiplier_c   0x0202
#define gZclAttrPrice_ThresholdDivisor_c      0x0203
#endif


#define gZclCmdPrice_GetCurrPriceReq_c                 0x00 /* M */
#define gZclCmdPrice_GetScheduledPricesReq_c           0x01 /* O */
#define gZclCmdPrice_PriceAck_c                        0x02 /* M */
#define gZclCmdPrice_GetBlockPeriods_c                 0x03 /* O */
#define gZclCmdPrice_GetConversionFactor_c             0x04 /* O */
#define gZclCmdPrice_GetCalorificValue_c               0x05 /* O */
#define gZclCmdPrice_GetTariffInformation_c            0x06 /* O */
#define gZclCmdPrice_GetPriceMatrix_c                  0x07 /* O */
#define gZclCmdPrice_GetBlockThresholds_c              0x08 /* O */
#define gZclCmdPrice_GetCO2Value_c                     0x09 /* O */
#define gZclCmdPrice_GetTierLabels_c                   0x0A /* O */
#define gZclCmdPrice_GetBillingPeriod_c                0x0B /* O */
#define gZclCmdPrice_GetConsolidatedBill_c             0x0C /* O */
#define gZclCmdPrice_GetCPPEventResponse_c             0x0D /* O */
#define gZclCmdPrice_GetCreditPayment_c                0x0E /* O */
#define gZclCmdPrice_GetCurrencyConversion_c           0x0F /* O */

#define gZclCmdPrice_PublishPriceRsp_c                 0x00 /* M */
#define gZclCmdPrice_PublishBlockPeriodRsp_c           0x01 /* O */
#define gZclCmdPrice_PublishConversionFactorRsp_c      0x02 /* O */
#define gZclCmdPrice_PublishCalorificValueRsp_c        0x03 /* O */
#define gZclCmdPrice_PublishTariffInformationRsp_c     0x04 /* O */
#define gZclCmdPrice_PublishPriceMatrixRsp_c           0x05 /* O */
#define gZclCmdPrice_PublishBlockThresholdsRsp_c       0x06 /* O */
#define gZclCmdPrice_PublishCO2ValueRsp_c              0x07 /* O */
#define gZclCmdPrice_PublishTierLabelsRsp_c            0x08 /* O */
#define gZclCmdPrice_PublishBillingPeriodRsp_c         0x09 /* O */
#define gZclCmdPrice_PublishConsolidatedBillRsp_c      0x0A /* O */
#define gZclCmdPrice_PublishCPPEventRsp_c              0x0B /* O */
#define gZclCmdPrice_PublishCreditPaymentRsp_c         0x0C /* O */
#define gZclCmdPrice_PublishCurrencyConversionRsp_c    0x0D /* O */
#define gZclCmdPrice_PublishExtendedPriceRsp_c         0x0E /* O */
#define gZclCmdPrice_CancelTariffReq_c                 0x0F /* O */

#define gGetCurrPrice_RequestorRxOnWhenIdle_c       0x01

#define gAltCostUnit_KgOfCO2perUnit_c 0x01 

/* Price Tier Sub-field Enumerations */
#define gPriceTierNoTierRelated_c 0x0 
#define gPriceTierTier1PriceLabel_c 0x1 
#define gPriceTierTier2PriceLabel_c 0x2 
#define gPriceTierTier3PriceLabel_c 0x3 
#define gPriceTierTier4PriceLabel_c 0x4 
#define gPriceTierTier5PriceLabel_c 0x5 
#define gPriceTierTier6PriceLabel_c 0x6 

/* Register Tier Sub-field Enumerations */
#define gRgstrTierNoTierRelated_c 0x0
#define gRgstrTierCurrTier1SummDlvrd 0x1 
#define gRgstrTierCurrTier2SummDlvrd 0x2 
#define gRgstrTierCurrTier3SummDlvrd 0x3 
#define gRgstrTierCurrTier4SummDlvrd 0x4 
#define gRgstrTierCurrTier5SummDlvrd 0x5 
#define gRgstrTierCurrTier6SummDlvrd 0x6
/* CPP Autorization Status*/
#define gPriceCPPEventPending   0x00
#define gPriceCPPEventAccepted  0x01
#define gPriceCPPEventRejected  0x02
#define gPriceCPPEventForced    0x03

typedef PACKED_STRUCT zclCmdPrice_GetCalorificValueReq_tag
{
  ZCLTime_t StartTime;
  SEEvtId_t      MinIssuerEvtId;
  uint8_t NumOfEvts;
} zclCmdPrice_GetCalorificValueReq_t;

typedef PACKED_STRUCT  zclPrice_GetCalorificValueReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetCalorificValueReq_t cmdFrame;
} zclPrice_GetCalorificValueReq_t;

typedef PACKED_STRUCT zclCmdPrice_GetConversionFactorReq_tag
{
  ZCLTime_t StartTime;
  SEEvtId_t      MinIssuerEvtId;
  uint8_t NumOfEvts;
} zclCmdPrice_GetConversionFactorReq_t;

typedef PACKED_STRUCT  zclPrice_GetConversionFactorReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetConversionFactorReq_t cmdFrame;
} zclPrice_GetConversionFactorReq_t;

typedef PACKED_STRUCT zclCmdPrice_GetBlockPeriodsReq_tag
{
  ZCLTime_t StartTime;
  uint8_t NumOfEvts;
  uint8_t TariffType;
} zclCmdPrice_GetBlockPeriodsReq_t;

typedef PACKED_STRUCT  zclPrice_GetBlockPeriodsReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetBlockPeriodsReq_t cmdFrame;
} zclPrice_GetBlockPeriodsReq_t;

typedef PACKED_STRUCT zclCmdPrice_GetTariffInformationReq_tag
{
  ZCLTime_t EarliestStartTime;
  uint32_t MinIssuerEvtId;
  uint8_t NumOfCmds;
  uint8_t TariffType;
} zclCmdPrice_GetTariffInformationReq_t;

typedef PACKED_STRUCT  zclPrice_GetTariffInformationReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetTariffInformationReq_t cmdFrame;
} zclPrice_GetTariffInformationReq_t;

typedef PACKED_STRUCT  zclPrice_GetCurrencyConversionReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
} zclPrice_GetCurrencyConversionReq_t;

typedef PACKED_STRUCT zclCmdPrice_GetBillingPeriodReq_tag
{
  ZCLTime_t EarliestStartTime;
  uint32_t MinIssuerEvtId;
  uint8_t NumOfCmds;
  uint8_t TariffType;
} zclCmdPrice_GetBillingPeriodReq_t;

typedef PACKED_STRUCT  zclPrice_GetBillingPeriodReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetBillingPeriodReq_t cmdFrame;
} zclPrice_GetBillingPeriodReq_t;

typedef PACKED_STRUCT zclCmdPrice_CPPEventRsp_tag
{
  uint32_t    IssuerEvtID;
  uint8_t     CPPAuth;
} zclCmdPrice_CPPEventRsp_t;

typedef PACKED_STRUCT  zclPrice_CPPEventRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_CPPEventRsp_t cmdFrame;
} zclPrice_CPPEventRsp_t;

typedef PACKED_STRUCT zclCmdPrice_GetPriceMatrixReq_tag
{
  SEEvtId_t         IssuerTariffID;
} zclCmdPrice_GetPriceMatrixReq_t;

typedef PACKED_STRUCT  zclPrice_GetPriceMatrixReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetPriceMatrixReq_t cmdFrame;
} zclPrice_GetPriceMatrixReq_t;

typedef PACKED_STRUCT zclCmdPrice_GetBlockThresholdsReq_tag
{
  SEEvtId_t         IssuerTariffID;
} zclCmdPrice_GetBlockThresholdsReq_t;

typedef PACKED_STRUCT  zclPrice_GetBlockThresholdsReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetBlockThresholdsReq_t cmdFrame;
} zclPrice_GetBlockThresholdsReq_t;

typedef PACKED_STRUCT zclCmdPrice_GetCO2ValueReq_tag
{
  ZCLTime_t EarliestStartTime;
  uint32_t MinIssuerEvtId;
  uint8_t NumOfCmds;
  uint8_t TariffType;
} zclCmdPrice_GetCO2ValueReq_t;

typedef PACKED_STRUCT  zclPrice_GetCO2ValueReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetCO2ValueReq_t cmdFrame;
} zclPrice_GetCO2ValueReq_t;

typedef PACKED_STRUCT zclCmdPrice_GetTierLabelsReq_tag
{
  SEEvtId_t         IssuerTariffID;
} zclCmdPrice_GetTierLabelsReq_t;

typedef PACKED_STRUCT  zclPrice_GetTierLabelsReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetTierLabelsReq_t cmdFrame;
} zclPrice_GetTierLabelsReq_t;


typedef PACKED_STRUCT zclCmdPrice_GetConsolidatedBillReq_tag
{
  ZCLTime_t EarliestStartTime;
  uint32_t MinIssuerEvtId;
  uint8_t NumOfCmds;
  uint8_t TariffType;
} zclCmdPrice_GetConsolidatedBillReq_t;

typedef PACKED_STRUCT  zclPrice_GetConsolidatedBillReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetConsolidatedBillReq_t cmdFrame;
} zclPrice_GetConsolidatedBillReq_t;

typedef PACKED_STRUCT zclCmdPrice_GetCurrPriceReq_tag
{
  uint8_t CmdOptions;
} zclCmdPrice_GetCurrPriceReq_t;

typedef PACKED_STRUCT  zclPrice_GetCurrPriceReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetCurrPriceReq_t cmdFrame;
} zclPrice_GetCurrPriceReq_t;

typedef PACKED_STRUCT  zclPrice_InterPanGetCurrPriceReq_tag
{  
  InterPanAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetCurrPriceReq_t cmdFrame;
} zclPrice_InterPanGetCurrPriceReq_t;

typedef PACKED_STRUCT  zclCmdPrice_GetScheduledPricesReq_tag
{  
  ZCLTime_t StartTime;
  uint8_t NumOfEvts;
} zclCmdPrice_GetScheduledPricesReq_t;

typedef PACKED_STRUCT  zclPrice_GetScheduledPricesReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetScheduledPricesReq_t cmdFrame;
} zclPrice_GetScheduledPricesReq_t;

typedef PACKED_STRUCT  zclPrice_InterPanGetScheduledPricesReq_tag
{  
  InterPanAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetScheduledPricesReq_t cmdFrame;
} zclPrice_InterPanGetScheduledPricesReq_t;

typedef PACKED_STRUCT zclCmdPrice_PublishPriceRsp_tag
{
  ProviderID_t ProviderID;
  zclStr12_t   RateLabel;
  SEEvtId_t    IssuerEvt;
  ZCLTime_t    CurrTime;
  uint8_t      UnitOfMeasure;
  Currency_t   Currency;
  uint8_t      PriceTrailingDigitAndPriceTier;
  uint8_t      NumOfPriceTiersAndRgstrTier;
  ZCLTime_t    StartTime;         
  Duration_t   DurationInMinutes;
  Price_t      Price;
  uint8_t      PriceRatio;  /* O */
  Price_t      GenerationPrice;   /* O */
  uint8_t      GenerationPriceRatio;  /* O */
  Price_t      AltCostDlvrd;  /* O */
  uint8_t      AltCostUnit;  /* O */
  uint8_t      AltCostTrailingDigit;   /* O */
  uint8_t      NumberOfBlocksThresholds;   /* O */
  uint8_t      PriceControl;   /* O */
#if gASL_ZclSE_12_Features_d  
  uint8_t      NumOfGenerationTiers;   /* O */
  uint8_t      GenerationTiers;   /* O */
#endif  
} zclCmdPrice_PublishPriceRsp_t;

typedef PACKED_STRUCT zclCmdPrice_PublishExtendedPriceRsp_tag
{
  ProviderID_t ProviderID;
  zclStr12_t   RateLabel;
  SEEvtId_t    IssuerEvt;
  ZCLTime_t    CurrTime;
  uint8_t      UnitOfMeasure;
  Currency_t   Currency;
  uint8_t      PriceTrailingDigitAndPriceCtl;
  uint8_t      PriceTier;
  uint8_t      NumOfPriceTiers;
  uint8_t      RegisterTiers;
  ZCLTime_t    StartTime;         
  Duration_t   DurationInMinutes;
  Price_t      Price;
  uint8_t      PriceRatio;  /* O */
  Price_t      GenerationPrice;   /* O */
  uint8_t      GenerationPriceRatio; /* O */ 
  Price_t      AltCostDlvrd;  /* O */
  uint8_t      AltCostUnit;  /* O */
  uint8_t      AltCostTrailingDigit;  /* O */ 
  uint8_t      NumOfGenerationTiers;   /* O */
  uint8_t      GenerationTier;   /* O */
} zclCmdPrice_PublishExtendedPriceRsp_t;

typedef PACKED_UNION zclCmdPrice_tag
{
  zclCmdPrice_PublishPriceRsp_t         Price;
  zclCmdPrice_PublishExtendedPriceRsp_t ExtendedPrice;
}zclCmdPrice_t;

typedef PACKED_STRUCT  zclPrice_PublishPriceRsp_tag
{  
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdPrice_t cmdFrame;
} zclPrice_PublishPriceRsp_t;

typedef PACKED_STRUCT zclCmdPrice_PublishBlockPeriodRsp_tag
{
  ProviderID_t          ProviderID;
  SEEvtId_t             IssuerEvtID;
  ZCLTime_t             BlockPeriodStartTime;
  Duration24_t          BlockPeriodDuration;
  uint8_t               NrOfPriceTiersNrOfBlockThresholds;
  uint8_t               BlockPeriodControl;
#if gASL_ZclSE_12_Features_d  
  uint8_t               TariffType;
  uint8_t               TariffResPer;
#endif  
} zclCmdPrice_PublishBlockPeriodRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishBlockPeriodRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishBlockPeriodRsp_t cmdFrame;
} zclPrice_PublishBlockPeriodRsp_t;

typedef PACKED_STRUCT zclCmdIdxTotalNumCmd_tag
{
#ifdef __IAR_SYSTEMS_ICC__    
  uint8_t cmdIdx                :4;
  uint8_t totalNoCmds           :4;
#else
  unsigned int cmdIdx           :4;
  unsigned int totalNoCmds      :4;
#endif  
}zclCmdIdxTotalNumCmd_t;

typedef PACKED_STRUCT zclTierBlock_tag
{
#ifdef __IAR_SYSTEMS_ICC__    
  uint8_t tierNum          :4;
  uint8_t blockNum         :4;
#else
  unsigned int tierNum      :4;
  unsigned int blockNum     :4;
#endif  
}zclTierBlock_t;

typedef PACKED_STRUCT TariffTypeScheme_tag
{
#ifdef __IAR_SYSTEMS_ICC__    
  uint8_t tariffType     :4;
  uint8_t scheme         :4;
#else
  unsigned int tariffType      :4;
  unsigned int scheme     :4;
#endif  
}TariffTypeScheme_t;

typedef PACKED_UNION zclTierBlockID_tag
{
  zclTierBlock_t      tierBlockID; 
  uint8_t             tierID;
}zclTierBlockID_t;

typedef PACKED_STRUCT TierBlockPrice_tag
{
  zclTierBlockID_t      id; 
  Price_t               price;
}TierBlockPrice_t;

typedef PACKED_STRUCT TierBlockThreshold_tag
{
  zclTierBlockID_t      id; 
  BlockThreshold_t      aBlockThresholds[1];
}TierBlockThreshold_t;

typedef PACKED_STRUCT zclCmdPrice_PublishPriceMatrixRsp_tag
{
  SEEvtId_t   ProviderID;
  SEEvtId_t   IssuerEvtID;
  ZCLTime_t   StartTime;
  SEEvtId_t   IssuerTariffID;
//  zclCmdIdxTotalNumCmd_t     CmdIdxTotalNumCmds;
  uint8_t     CmdIdx;  
  uint8_t     NoCmds;  
  uint8_t     SubPayloadCtl;
  TierBlockPrice_t     TierBlockPrice[1]; 
} zclCmdPrice_PublishPriceMatrixRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishPriceMatrixRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishPriceMatrixRsp_t cmdFrame;
} zclPrice_PublishPriceMatrixRsp_t;

typedef PACKED_STRUCT zclCmdPrice_PublishBlockThresholdsRsp_tag
{
  SEEvtId_t   ProviderID;
  SEEvtId_t   IssuerEvtID;
  ZCLTime_t   StartTime;
  SEEvtId_t   IssuerTariffID;
  //zclCmdIdxTotalNumCmd_t     CmdIdxTotalNumCmds;
  uint8_t     CmdIdx;
  uint8_t     NumCmds;
  uint8_t     SubPayloadCtl;
  TierBlockThreshold_t  BlockThresholds[1]; 
} zclCmdPrice_PublishBlockThresholdsRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishBlockThresholdsRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishBlockThresholdsRsp_t cmdFrame;
} zclPrice_PublishBlockThresholdsRsp_t;

typedef PACKED_STRUCT zclTierEntry_tag
{
  uint8_t       tierNumber;
  zclStr12_t    tierLabel;
}zclTierEntry_t;

typedef PACKED_STRUCT zclCmdPrice_PublishTierLabelsRsp_tag
{
  SEEvtId_t             ProviderID;
  SEEvtId_t             IssuerEvtID;
  SEEvtId_t             IssuerTariffID;
  index_t               CmdIdx;
  uint8_t               TotalNumOfCmds;
  uint8_t               NumOfLabels;
  zclTierEntry_t        TierLabel[1]; 
} zclCmdPrice_PublishTierLabelsRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishTierLabelsRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishTierLabelsRsp_t cmdFrame;
} zclPrice_PublishTierLabelsRsp_t;


typedef PACKED_STRUCT zclCmdPrice_PublishCalorificValueRsp_tag
{
  SEEvtId_t             IssuerEvtID;
  ZCLTime_t             StartTime;
  CalorificValue_t      CalorificValue;
  uint8_t               CalorificValueUnit;
  uint8_t               CalorificValueTrailingDigit;
} zclCmdPrice_PublishCalorificValueRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishCalorificValueRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishCalorificValueRsp_t cmdFrame;
} zclPrice_PublishCalorificValueRsp_t;

typedef PACKED_STRUCT zclCmdPrice_PublishConversionFactorRsp_tag
{
  SEEvtId_t             IssuerEvt;
  ZCLTime_t             StartTime;
  ConversionFactor_t    ConversionFactor;
  uint8_t               ConversionFactorTrailingDigit;
} zclCmdPrice_PublishConversionFactorRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishConversionFactorRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishConversionFactorRsp_t cmdFrame;
} zclPrice_PublishConversionFactorRsp_t;


typedef PACKED_STRUCT zclCmdPrice_PublishTariffInformationRsp_tag
{
  ProviderID_t                ProviderID;
  uint32_t                    IssuerEvtID;
  SEEvtId_t                   IssuerTariffID;
  ZCLTime_t                   StartTime;
  TariffTypeScheme_t          TariffType;
  zclStr24Oct_t               TariffLabel;
  uint8_t                     NumberPriceTiersUse;
  uint8_t                     NumberBlockThreshholdsUse;
  uint8_t                     UnitOfMeasure;
  Currency_t                  Currency;
  uint8_t                     PriceTrailingDigit;
  StandingCharge_t            StandingCharge;
  uint8_t                     TierBlockMode;
  BlockThresholdMask_t        BlockThresholdMask;
  BlockThresholdMultiplier_t  BlockThresholdMultiplier;
  BlockThresholdDivisor_t     BlockThresholdDivisor;
} zclCmdPrice_PublishTariffInformationRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishTariffInformationRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishTariffInformationRsp_t cmdFrame;
} zclPrice_PublishTariffInformationRsp_t;

typedef PACKED_STRUCT zclCmdPrice_CancelTariffReq_tag
{
  ProviderID_t     ProviderID;
  SEEvtId_t        IssuerTariffID;
  uint8_t          TariffType;  
} zclCmdPrice_CancelTariffReq_t;

typedef PACKED_STRUCT  zclPrice_CancelTariffReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_CancelTariffReq_t cmdFrame;
} zclPrice_CancelTariffReq_t;

typedef PACKED_STRUCT zclCmdPrice_PublishCurrencyConversionRsp_tag
{
  ProviderID_t                ProviderID;
  uint32_t                    IssuerEvtID;
  ZCLTime_t                   StartTime;
  Currency_t                  OldCurrency;
  Currency_t                  NewCurrency;
  ConversionFactor_t          ConversionFactor;
  uint8_t                     ConversionFactorTrailingDigit;
  ChangeCtrl_t                CurrencyChangeControlFlags;
} zclCmdPrice_PublishCurrencyConversionRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishCurrencyConversionRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishCurrencyConversionRsp_t cmdFrame;
} zclPrice_PublishCurrencyConversionRsp_t;

typedef PACKED_STRUCT zclCmdPrice_PublishBillingPeriodRsp_tag
{
  SEEvtId_t         ProviderID;
  uint32_t          IssuerEvtID;
  ZCLTime_t         BillingPeriodStartTime;
  Duration24_t      BillingPeriodDuration;
  uint8_t           TariffType;
} zclCmdPrice_PublishBillingPeriodRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishBillingPeriodRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishBillingPeriodRsp_t cmdFrame;
} zclPrice_PublishBillingPeriodRsp_t;

typedef PACKED_STRUCT zclCmdPrice_PublishCPPEventRsp_tag
{
  SEEvtId_t         ProviderID;
  uint32_t          IssuerEvtID;
  ZCLTime_t         CPPEventStartTime;
  Duration16_t      CPPEventDurationInMinutes;
  uint8_t           TariffType;
  uint8_t           CPPPriceTier;
  uint8_t           CPPAuth;
} zclCmdPrice_PublishCPPEventRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishCPPEventRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishCPPEventRsp_t cmdFrame;
} zclPrice_PublishCPPEventRsp_t;

typedef PACKED_STRUCT zclCmdPrice_PublishCO2ValueRsp_tag
{
  SEEvtId_t         ProviderID;
  uint32_t          IssuerEvtID;
  ZCLTime_t         StartTime;
  uint8_t           TariffType;
  CO2Value_t        CO2Value;
  uint8_t           CO2ValueUnit;
  uint8_t           CO2ValueTrailingDigit;
} zclCmdPrice_PublishCO2ValueRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishCO2ValueRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishCO2ValueRsp_t cmdFrame;
} zclPrice_PublishCO2ValueRsp_t;

typedef PACKED_STRUCT zclCmdPrice_PublishConsolidatedBillRsp_tag
{
  SEEvtId_t         ProviderID;
  uint32_t          IssuerEvtID;
  ZCLTime_t         BillingPeriodStartTime;
  Duration24_t      BillingPeriodDuration;
  uint8_t           TariffType;
  Bill_t            ConsolidatedBill;
  Currency_t        Currency;
  uint8_t           BillTrailingDigit;
} zclCmdPrice_PublishConsolidatedBillRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishConsolidatedBillRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishConsolidatedBillRsp_t cmdFrame;
} zclPrice_PublishConsolidatedBillRsp_t;

typedef PACKED_STRUCT zclCmdPrice_PublishCreditPaymentRsp_tag
{
  SEEvtId_t         ProviderID;
  uint32_t          IssuerEvtID;
  ZCLTime_t         CreditPaymentDueDate;
  Credit_t          CreditPaymentOverdueAmount;
  uint8_t           CreditPaymentStatus;
  Credit_t          CreditPayment;
  ZCLTime_t         CreditPaymentDate;
  zclStr20_t        CreditPaymentRef;
} zclCmdPrice_PublishCreditPaymentRsp_t;

typedef PACKED_STRUCT  zclPrice_PublishCreditPaymentRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishCreditPaymentRsp_t cmdFrame;
} zclPrice_PublishCreditPaymentRsp_t;

typedef PACKED_STRUCT zclCmdPrice_PriceAck_tag
{
  ProviderID_t ProviderID;
  SEEvtId_t    IssuerEvt;
  ZCLTime_t    PriceAckTime;
  uint8_t      PriceControl;
}zclCmdPrice_PriceAck_t;

typedef PACKED_STRUCT zclPrice_PriceAck_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PriceAck_t cmdFrame;
}zclPrice_PriceAck_t;

typedef PACKED_STRUCT zclPrice_InterPriceAck_tag
{
  InterPanAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PriceAck_t cmdFrame;
}zclPrice_InterPriceAck_t;

typedef PACKED_STRUCT  zclPrice_InterPanPublishPriceRsp_tag
{  
  InterPanAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishPriceRsp_t cmdFrame;
} zclPrice_InterPanPublishPriceRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishBlockPeriodRsp_tag
{
  zclCmdPrice_PublishBlockPeriodRsp_t publishBlockPeriodRsp;
  bool_t SendUnsolicited;
} ztcCmdPrice_PublishBlockPeriodRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishBillingPeriodRsp_tag
{
  zclCmdPrice_PublishBillingPeriodRsp_t publishBillingPeriodRsp;
  bool_t SendUnsolicited;
} ztcCmdPrice_PublishBillingPeriodRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishTariffInformationRsp_tag
{
  zclCmdPrice_PublishTariffInformationRsp_t publishTariffInformationRsp;
  bool_t SendUnsolicited;
} ztcCmdPrice_PublishTariffInformationRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishCurrencyConversionRsp_tag
{
  zclCmdPrice_PublishCurrencyConversionRsp_t publishCurrencyConversionRsp;
  bool_t SendUnsolicited;
} ztcCmdPrice_PublishCurrencyConversionRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishCO2ValueRsp_tag
{
  zclCmdPrice_PublishCO2ValueRsp_t publishCO2ValueRsp;
  bool_t SendUnsolicited;
} ztcCmdPrice_PublishCO2ValueRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishConsolidatedBillRsp_tag
{
  zclCmdPrice_PublishConsolidatedBillRsp_t publishConsolidatedBillRsp;
  bool_t SendUnsolicited;
} ztcCmdPrice_PublishConsolidatedBillRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishCreditPaymentRsp_tag
{
  zclCmdPrice_PublishCreditPaymentRsp_t publishCreditPaymentRsp;
  bool_t SendUnsolicited;
} ztcCmdPrice_PublishCreditPaymentRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishConversionFactorRsp_tag
{
  zclCmdPrice_PublishConversionFactorRsp_t publishConversionFactorRsp;
  bool_t SendUnsolicited;
} ztcCmdPrice_PublishConversionFactorRsp_t;


typedef PACKED_STRUCT ztcCmdPrice_PublishPriceMatrixRsp_tag
{
  SEEvtId_t   ProviderID;
  SEEvtId_t   IssuerEvtID;
  ZCLTime_t   StartTime;
  SEEvtId_t   IssuerTariffID;
  uint8_t     SubPayloadCtl;
  uint8_t     Offset;
  uint8_t     Length;
  TierBlockPrice_t     TierBlockPrice[1]; 
} ztcCmdPrice_PublishPriceMatrixRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishTierLabelsRsp_tag
{
  SEEvtId_t             ProviderID;
  SEEvtId_t             IssuerEvtID;
  SEEvtId_t             IssuerTariffID;
  uint8_t               Offset; 
  uint8_t               Length;
  zclTierEntry_t        TierLabel[1]; 
  } ztcCmdPrice_PublishTierLabelsRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishBlockThresholdsRsp_tag
{
  SEEvtId_t   ProviderID;
  SEEvtId_t   IssuerEvtID;
  ZCLTime_t   StartTime;
  SEEvtId_t   IssuerTariffID;
  uint8_t     SubPayloadCtl;
  uint8_t     TierNumber;
  uint8_t     Length;
  BlockThreshold_t   aBlockThresholds[1];
} ztcCmdPrice_PublishBlockThresholdsRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishCalorificValueRsp_tag
{
  zclCmdPrice_PublishCalorificValueRsp_t publishCalorificValueRsp;
  bool_t SendUnsolicited;
} ztcCmdPrice_PublishCalorificValueRsp_t;

typedef PACKED_STRUCT ztcCmdPrice_PublishPriceRsp_tag
{
  zclCmdPrice_PublishPriceRsp_t publishPriceRsp;
  bool_t SendUnsolicited;
} ztcCmdPrice_PublishPriceRsp_t;

#define gPriceReceivedStatus_c 0x01
#define gPriceStartedStatus_c 0x02
#define gPriceUpdateStatus_c 0x03
#define gPriceCompletedStatus_c 0x04

//block period client status
#define gBlockPeriodReceivedStatus_c 0x01
#define gBlockPeriodStartedStatus_c 0x02
#define gBlockPeriodUpdateStatus_c 0x03
#define gBlockPeriodCompletedStatus_c 0x04

typedef PACKED_STRUCT publishPriceEntry_tag
{
  zclCmdPrice_t Price;
  ZCLTime_t     EffectiveStartTime;
  bool_t        IsExtended;    
  uint8_t       EntryStatus;
} publishPriceEntry_t;

typedef PACKED_STRUCT clientPriceTableEntry_tag
{
#if gMaxNoOfESISupported_c > 1
  zbNwkAddr_t aNwkAddr;
#endif  
  zclCmdPrice_PublishPriceRsp_t Price;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} clientPriceTableEntry_t;

typedef PACKED_STRUCT publishBlockPeriodEntry_tag
{  
  zclCmdPrice_PublishBlockPeriodRsp_t blockPeriod;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishBlockPeriodEntry_t;

typedef PACKED_STRUCT publishPriceMatrixEntry_tag
{
  SEEvtId_t   ProviderID;
  SEEvtId_t   IssuerEvtID;
  ZCLTime_t   StartTime;
  SEEvtId_t   IssuerTariffID;
  uint8_t     SubPayloadCtl;
  uint8_t     Length;
  uint8_t     EntryStatus;
  TierBlockPrice_t     TierBlockPrice[gASL_ZclSE_TiersNumber_d*gASL_ZclSE_BlocksNumber_d]; 
} publishPriceMatrixEntry_t;

typedef PACKED_STRUCT publishBlockThresholdsEntry_tag
{
  SEEvtId_t   ProviderID;
  SEEvtId_t   IssuerEvtID;
  ZCLTime_t   StartTime;
  SEEvtId_t   IssuerTariffID;
  uint8_t     SubPayloadCtl;
  uint8_t     Length;
  uint8_t      EntryStatus;
  TierBlockThreshold_t  BlockThresholds[gASL_ZclSE_TiersNumber_d]; 
} publishBlockThresholdsEntry_t;

typedef PACKED_STRUCT publishTierLabelsEntry_tag
{
  SEEvtId_t             ProviderID;
  SEEvtId_t             IssuerEvtID;
  SEEvtId_t             IssuerTariffID;
  uint8_t               Length;
  uint8_t               EntryStatus;
  zclTierEntry_t        TierLabel[gASL_ZclSE_TiersNumber_d]; 
} publishTierLabelsEntry_t;

typedef PACKED_STRUCT publishConversionFactorEntry_tag
{  
  zclCmdPrice_PublishConversionFactorRsp_t ConversionFactor;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishConversionFactorEntry_t;

typedef PACKED_STRUCT publishCalorificValueEntry_tag
{  
  zclCmdPrice_PublishCalorificValueRsp_t CalorificValue;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishCalorificValueEntry_t;

typedef PACKED_STRUCT publishTariffInformationEntry_tag
{  
  zclCmdPrice_PublishTariffInformationRsp_t TariffInformation;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishTariffInformationEntry_t;

typedef PACKED_STRUCT publishCurrencyConversionEntry_tag
{  
  zclCmdPrice_PublishCurrencyConversionRsp_t CurrencyConversion;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishCurrencyConversionEntry_t;

typedef PACKED_STRUCT publishBillingPeriodEntry_tag
{  
  zclCmdPrice_PublishBillingPeriodRsp_t BillingPeriod;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishBillingPeriodEntry_t;

typedef PACKED_STRUCT publishCPPEventEntry_tag
{  
  zclCmdPrice_PublishCPPEventRsp_t CPPEvent;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishCPPEventEntry_t;

typedef PACKED_STRUCT publishCO2ValueEntry_tag
{  
  zclCmdPrice_PublishCO2ValueRsp_t CO2Value;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishCO2ValueEntry_t;

typedef PACKED_STRUCT publishConsolidatedBillEntry_tag
{  
  zclCmdPrice_PublishConsolidatedBillRsp_t ConsolidatedBill;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishConsolidatedBillEntry_t;

typedef PACKED_STRUCT publishCreditPaymentEntry_tag
{  
  zclCmdPrice_PublishCreditPaymentRsp_t CreditPayment;
  uint8_t EntryStatus;
} publishCreditPaymentEntry_t;


/******************************************************************************
*******************************************************************************
* Internal Attributes types def
*******************************************************************************
******************************************************************************/


typedef PACKED_STRUCT zclDRLCClientAttr_tag {
  uint8_t UtilityGroup[zclReportableCopies_c];
  uint8_t StartRandomizeMin;
  uint8_t StopRandomizeMin;  
  LCDRDevCls_t DevCls;
} zclDRLCClientAttr_t;


/*Attributes of the Metering Cluster */
typedef PACKED_STRUCT zclMetAttrRAM_tag
{ 
   /*Attributes of the Reading information attribute set */
  Summ_t   RISCurrSummDlvrd[zclReportableCopies_c];
#if gASL_ZclMet_Optionals_d
  Summ_t    RISCurrSummRcvd; 
  Summ_t    RISCurrMaxDmndDlvrd;
  Summ_t    RISCurrMaxDmndRcvd;
  Summ_t    RISDFTSumm;
  uint16_t    RISDailyFreezeTime;
  int8_t    RISPowerFactor;
  ZCLTime_t RISReadingSnapShotTime;
  ZCLTime_t RISCurrMaxDmndDlvrdTimer;
  ZCLTime_t RISCurrMaxDmndRcvdTime;
  uint8_t   RISDefaultUpdatePeriod;
  uint8_t   RISFastPollUpdatePeriod;
  Summ_t    RISCurrBlockPeriodConsumptionDelivered;
  Consmp    RISDailyConsumptionTarget;
  uint8_t   RISCurrBlock;
  uint8_t   RISProfileIntPeriod;
  uint16_t  RISIntReadReportingPeriod;
  uint16_t  RISPresetReadingTime;
  uint16_t  RISVolumePerReport;
  uint8_t   RISFlowRestrictio;
  uint8_t   RISSupplyStatus;
  Summ_t    RISCurrInletEnergyCarrierSummation;
  Summ_t    RISCurrOutletEnergyCarrierSummation;
  int16_t   RISInletTemp;
  int16_t   RISOutletTemp;
  int16_t   RISCtrlTemp;
  uint8_t   RISCurrInletEnergyCarrierDemand;
  uint8_t   RISCurrOutletEnergyCarrierDemand;
  Summ_t    RISPrevBlockPeriodConsumptionDlvrd;     
  Summ_t    RISCurrBlockPeriodConsumptionRcvd;      
  uint8_t   RISCurrBlockRcvd;                       
  Summ_t    RISDFTSummationRcvd;                    
  uint8_t   RISActiveRegisterTierDlvrd;             
  uint8_t   RISActiveRegisterTierRcvd;              
  ZCLTime_t RISLastBlockSwitchTime;                 
#endif  

  /*Attributes of the TOU Information attribute set */
#if gASL_ZclSE_TiersNumber_d >=1 
  Summ_t  TOUCurrTierSumm[2 * (gASL_ZclSE_TiersNumber_d + gASL_ZclSE_ExtendedPriceTiersNumber_d)];
#endif
  
/*Attributes of the Meter Status attribute set */
  uint8_t MSStatus[zclReportableCopies_c];
#if gASL_ZclMet_Optionals_d
  uint8_t RemainingBatteryLife;
  Consmp  HoursInOperation;
  Consmp  HoursInFault;
#endif

/*Attributes of the Formatting attribute set */
  uint8_t   SmplMetCFSUnitofMeasure[zclReportableCopies_c];
#if gASL_ZclMet_Optionals_d 
  Consmp  Mult;
  Consmp  Div;
#endif  
  uint8_t   CFSSummFormat[zclReportableCopies_c];
#if gASL_ZclMet_Optionals_d 
  uint8_t   DmndFormat;
  uint8_t   HistoricalConsmpFormat;
#endif  
  uint8_t   CFSMetDevType[zclReportableCopies_c];
#if gASL_ZclMet_Optionals_d
  zclStr32Oct_t CFSSiteID;
  zclStr24Oct_t CFSMeterSerialNumber;
  uint8_t CFSEnergyCarrierUnitOfMeasure;
  uint8_t CFSEnergyCarrierSummationFormatting;
  uint8_t CFSEnergyCarrierDemandFormatting;
  uint8_t CFSTempUnitOfMeasure;
  uint8_t CFSTempFormatting;
  zclStr24Oct_t ModuleSerialNum;
  zclStr24Oct_t OpTariffLabelDlvrd;
  zclStr24Oct_t OpTariffLabelRcvd;
#endif

  /*Attributes of the ESP Historical Consumption */
  Consmp  InstantaneousDmnd;
  Consmp  CurrDayConsmpDlvrd;
  Consmp  CurrDayConsmpRcvd;
  Consmp  PreviousDayConsmpDlvrd;
  Consmp  PreviousDayConsmpRcvd;
  ZCLTime_t CurrPartialProfileIntrvStartTimeDlvrd;
  ZCLTime_t CurrPartialProfileIntrvStartTimeRcvd;
  IntrvForProfiling_t  CurrPartialProfileIntrvValueDlvrd;
  IntrvForProfiling_t  CurrPartialProfileIntrvValueRcvd;
  Pressure_t CurrDayMaxPressure;
  Pressure_t CurrDayMinPressure;
  Pressure_t PrevDayMaxPressure;
  Pressure_t PrevDayMinPressure;
  Demmand_t CurrDayMaxDemand;
  Demmand_t PrevDayMaxDemand;
  Demmand_t CurrMonthMaxDemand;
  Demmand_t CurrYearMaxDemand;
  Demmand_t CurrDayMaxEnergyCarrierDemand;
  Demmand_t PrevDayMaxEnergyCarrierDemand;
  Demmand_t CurrMonthMaxEnergyCarrierDemand;
  Demmand_t CurrMonthMinEnergyCarrierDemand;
  Demmand_t CurrYearMaxEnergyCarrierDemand;
  Demmand_t CurrYearMinEnergyCarrierDemand;
  Consmp  DayConsump[14];
  Consmp  WeekConsump[12];
  Consmp32_t  MonthConsump[28];

  /*Attributes of the Simple Metering Meter Load Profile Configuration attribute set */
  uint8_t MaxNumberOfPeriodsDlvrd;

/*Attributes of the Simple Metering Alarm attribute set */  
  uint16_t ASGenericAlarmMask;
  uint16_t ASElectricityAlarmMask;
  uint16_t ASGenericFlowPressureAlarmMask;
  uint16_t ASWaterSpecificAlarmMask;
  uint16_t ASHeatAndCoolingSpecificAlarmMask;
  uint16_t ASGasSpecificAlarmMask;
  Pressure_t ExtendedGenericAlarmMask;
  uint16_t ManufacturerAlarmMask;
  
/*Attributes of the Simple Metering Supply Limit Attribute Set */ 
  Consmp  CurrDmndDlvrd;
  Consmp  DmndLimit;
  uint8_t DmndIntegrationPeriod;
  uint8_t NumOfDmndSubIntrvs;
  uint16_t DemandLimitArmDuration;
  uint8_t LoadLimitSupplyState;
  uint8_t LoadLimitCounter;

/*Attributes of the Simple Metering Block Information Delivered Attribute Set */
  Summ_t    CurrNoTierBlockSummDlvrd[gASL_ZclSE_TiersNumber_d + 1][gASL_ZclSE_BlocksNumber_d];

/*Attributes of the Simple Metering Block Information Received Attribute Set */
  Summ_t    CurrNoTierBlockSummRcvd[gASL_ZclSE_TiersNumber_d + 1][gASL_ZclSE_BlocksNumber_d];
  
/*Attributes of the Simple Metering Billing Attribute Set */
  Bill_t    BillToDate;
  uint32_t  BillToDateTimeStamp;
  Bill_t    ProjectedBill;
  uint32_t  ProjectedBillTimeStamp;

  uint8_t	ReportStatus;
}zclMetAttrRAM_t;

typedef PACKED_STRUCT zclPriceClientAttrRAM_tag{
  uint8_t PriceIncreaseRandomizeMinutes;
  uint8_t PriceDecreaseRandomizeMinutes;
  uint8_t ClientCommodityType;
} zclPriceClientAttrRAM_t;

typedef PACKED_STRUCT zclPriceServerAttrRAM_tag{
#if gASL_ZclSE_TiersNumber_d + gASL_ZclSE_ExtendedPriceTiersNumber_d > 0
  zclStr12_t TierPriceLabel[gASL_ZclSE_TiersNumber_d + gASL_ZclSE_ExtendedPriceTiersNumber_d];
#endif
#if gASL_ZclPrice_BlockThresholdNumber_d > 0
  BlockThreshold_t BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
#if gASL_ZclSE_12_Features_d  
  uint8_t BlockThresholdCount;
  BlockThreshold_t Tier1BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier1BlockThresholdCount;
  BlockThreshold_t Tier2BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier2BlockThresholdCount;
  BlockThreshold_t Tier3BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier3BlockThresholdCount;
  BlockThreshold_t Tier4BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier4BlockThresholdCount;
  BlockThreshold_t Tier5BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier5BlockThresholdCount;
  BlockThreshold_t Tier6BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier6BlockThresholdCount;
  BlockThreshold_t Tier7BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier7BlockThresholdCount;
  BlockThreshold_t Tier8BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier8BlockThresholdCount;
  BlockThreshold_t Tier9BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier9BlockThresholdCount;
  BlockThreshold_t Tier10BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier10BlockThresholdCount;
  BlockThreshold_t Tier11BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier11BlockThresholdCount;
  BlockThreshold_t Tier12BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier12BlockThresholdCount;
  BlockThreshold_t Tier13BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier13BlockThresholdCount;
  BlockThreshold_t Tier14BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier14BlockThresholdCount;
  BlockThreshold_t Tier15BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
  uint8_t Tier15BlockThresholdCount;
#endif  
#endif

  ZCLTime_t StartofBlockPeriod;
  Duration24_t BlockPeriodDuration;
  Multiplier24_t ThresholdMultiplier;
  Divisor24_t ThresholdDivisor;

  uint8_t CommodityType;
  CommodityCharge_t StandingCharge;
  ConversionFactor_t ConversionFactor;
  uint8_t  ConversionFactorTrlDigit;
  CalorificValue_t CalorificValue;
  uint8_t CalorificValueUnit;
  uint8_t CalorificValueTrlDigit;

  uint32_t BlockPriceInfoPrice[gASL_ZclPrice_BlockPriceInfoNumber_d];
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 0
  uint32_t ExtendedPriceInfo[gASL_ZclSE_ExtendedPriceTiersNumber_d];
#endif
  zclStr12_t    TariffLabel;
  uint8_t       NoOfPriceTiersInUse;
  uint8_t       NoOfBlockThresholdsInUse;
  uint8_t       TierBlockMode;
  BlockThresholdMask_t  BlockThresholdMask;
  uint8_t               UnitOfMeasure;
  Currency_t            Currency;
  uint8_t               PriceTrailingDigit;

  ZCLTime_t     CurrBillingPeriodStartUTCTime;
  Duration24_t  CurrBillPeriodDuration;
  ZCLTime_t     LastBillPeriodStart;
  Duration24_t  LastBillPeriodDuration;
  Bill_t        LastBillPeriodConsolidatedBill;

  ZCLTime_t     CreditPaymentDueDate;
  uint8_t       CreditPaymentStatus;
  Credit_t      CreditPaymentOverDueAmount;
  Credit_t      PaymentDiscount;
  uint8_t       PaymentDiscountPeriod;
  Credit_t      CreditPayment1;
  ZCLTime_t     CreditPaymentDate1;
  zclStr20_t    CreditPaymentRef1;
  Credit_t      CreditPayment2;
  ZCLTime_t     CreditPaymentDate2;
  zclStr20_t    CreditPaymentRef2;
  Credit_t      CreditPayment3;
  ZCLTime_t     CreditPaymentDate3;
  zclStr20_t    CreditPaymentRef3;
  Credit_t      CreditPayment4;
  ZCLTime_t     CreditPaymentDate4;
  zclStr20_t    CreditPaymentRef4;
  Credit_t      CreditPayment5;
  ZCLTime_t     CreditPaymentDate5;
  zclStr20_t    CreditPaymentRef5;

  uint16_t DomesticPercentage;
  uint32_t DomesticFuelTaxRate;
  uint8_t DomesticFuelTaxRateTrlDgt;

#if gASL_ZclSE_12_Features_d
#if gASL_ZclSE_TiersNumber_d + gASL_ZclSE_ExtendedPriceTiersNumber_d > 0
  zclStr12_t RcvdTierPriceLabel[gASL_ZclSE_TiersNumber_d+gASL_ZclSE_ExtendedPriceTiersNumber_d];
#endif

#if gASL_ZclPrice_BlockThresholdNumber_d > 0
  BlockThreshold_t RcvdBlockThresholds[gASL_ZclPrice_BlockThresholdNumber_d];
#endif

  ZCLTime_t RcvdStartofBlockPeriod;
  Duration24_t RcvdBlockPeriodDuration;
  Multiplier24_t RcvdThresholdMultiplier;
  Divisor24_t RcvdThresholdDivisor;

  uint8_t RcvdCommodityType;
  CommodityCharge_t RcvdStandingCharge;
  ConversionFactor_t RcvdConversionFactor;
  uint8_t  RcvdConversionFactorTrlDigit;
  CalorificValue_t RcvdCalorificValue;
  uint8_t RcvdCalorificValueUnit;
  uint8_t RcvdCalorificValueTrlDigit;

  uint32_t RcvdBlockPriceInfo[gASL_ZclPrice_BlockPriceInfoNumber_d];
#if gASL_ZclSE_ExtendedPriceTiersNumber_d > 0
  uint32_t RcvdExtendedPriceInfo[gASL_ZclSE_ExtendedPriceTiersNumber_d];
#endif
  zclStr12_t    RcvdTariffLabel;
  uint8_t       RcvdNoOfPriceTiersInUse;
  uint8_t       RcvdNoOfBlockThresholdsInUse;
  uint8_t       RcvdTierBlockMode;
  BlockThresholdMask_t  RcvdBlockThresholdMask;
  uint8_t               RcvdUnitOfMeasure;
  Currency_t            RcvdCurrency;
  uint8_t               RcvdPriceTrailingDigit;

  ZCLTime_t     RcvdCurrBillingPeriodStartUTCTime;
  Duration24_t  RcvdCurrBillPeriodDuration;
  ZCLTime_t     RcvdLastBillPeriodStart;
  Duration24_t  RcvdLastBillPeriodDuration;
  Bill_t        RcvdLastBillPeriodConsolidatedBill;
#endif
}zclPriceServerAttrRAM_t;

#if gZcl_EnablePriceServer_d || gZcl_EnablePriceClient_d 
typedef PACKED_STRUCT zclPriceAttrRAM_tag {
#if gZcl_EnablePriceServer_d
  zclPriceServerAttrRAM_t  PriceServerAttrs;
#endif
#if gZcl_EnablePriceClient_d 
  zclPriceClientAttrRAM_t  PriceClientAttrs;
#endif
} zclPriceAttrRAM_t;
#endif

typedef PACKED_STRUCT zclPrepaymentAttrRAM_tag {
  uint16_t            paymentCtrl;
#if gASL_ZclPrepayment_Optionals_d
  uint32_t            creditRemaining;
  uint32_t            emergencyCreditRemaining;
  uint8_t             creditStatus;
  ZCLTime_t           creditRemainingTimeStamp;
  uint32_t            accumulatedDebt;
  uint32_t            overallDebtCap;
  int32_t             emergencyCreditLimitAllowance;
  int32_t             emergencyCreditThreshold;
  uint48_t            totalCreditAdded;
  uint32_t            maxCreditLimit;
  uint8_t             friendlyCreditWarning;
  uint32_t            lowCreditWarning;
  uint32_t            IHDLowCreditWarning;
  uint8_t             intSuspendTime;
  int32_t             cutOffValue;
  zclStr20_t          paymentCardID;
#endif

  ZCLTime_t           topUpTime1;
  sePrepaymentTopUp_t topUpAmount1;
  uint8_t             topUpOriginatingDevice1;
  zclStr25_t          topUpCode1;
  ZCLTime_t           topUpTime2;
  sePrepaymentTopUp_t topUpAmount2;
  uint8_t             topUpOriginatingDevice2;
  zclStr25_t          topUpCode2;
  ZCLTime_t           topUpTime3;
  sePrepaymentTopUp_t topUpAmount3;
  uint8_t             topUpOriginatingDevice3;
  zclStr25_t          topUpCode3;
  ZCLTime_t           topUpTime4;
  sePrepaymentTopUp_t topUpAmount4;
  uint8_t             topUpOriginatingDevice4;
  zclStr25_t          topUpCode4;
  ZCLTime_t           topUpTime5;
  sePrepaymentTopUp_t topUpAmount5;
  uint8_t             topUpOriginatingDevice5;
  zclStr25_t          topUpCode5;

  zclStr12_t       debtLabel1;
  uint32_t         debtAmount1;
  uint8_t          debtRecoveryMethod1;
  ZCLTime_t        debtRecoveryStartTime1;
  uint16_t         debtRecoveryCollectionTime1;
  uint8_t          debtRecoveryFequency1;
  uint32_t         debtRecoveryAmount1;
  uint16_t         debtRecoveryVendPerCent1;
  uint8_t          debtRecoveryRateCap1;
  zclStr12_t       debtLabel2;
  uint32_t         debtAmount2;
  uint8_t          debtRecoveryMethod2;
  ZCLTime_t        debtRecoveryStartTime2;
  uint16_t         debtRecoveryCollectionTime2;
  uint8_t          debtRecoveryFequency2;
  uint32_t         debtRecoveryAmount2;
  uint16_t         debtRecoveryVendPerCent2;
  uint8_t          debtRecoveryRateCap2;
  zclStr12_t       debtLabel3;
  uint32_t         debtAmount3;
  uint8_t          debtRecoveryMethod3;
  ZCLTime_t        debtRecoveryStartTime3;
  uint16_t         debtRecoveryCollectionTime3;
  uint8_t          debtRecoveryFequency3;
  uint32_t         debtRecoveryAmount3;
  uint16_t         debtRecoveryVendPerCent3;
  uint8_t          debtRecoveryRateCap3;

#if !gASL_ZclSE_12_Features_d
    uint32_t             proposedChangeProviderId;
    ZCLTime_t            proposedChangeImplementationTime;
    uint8_t              proposedChangeSupplyStatus;
    uint16_t             delayedSupplyIntValueRemaining;
    uint8_t              delayedSupplyIntValueType;
#else
    uint16_t prepaymentAlarmStatus;
    uint16_t genericAlarmMask;
    uint16_t prepaySwitchAlarmMask;
    uint16_t prepayEventAlarmMask;
#endif
} zclPrepaymentAttrRAM_t;

typedef PACKED_STRUCT seTunnelingAttrRAM_tag {
  uint16_t            closeTunnelTimeout;
} seTunnelingAttrRAM_t;

typedef PACKED_STRUCT devMeterAttrRAM_tag {
  uint8_t              reportMask[1];  /* allows up to 8 reportable endpoints in this node  */
  zclBasicAttrsRAM_t    basicAttrs;     /* RAM attributes from the Basic cluster */
#if gZclEnablePwrCfgServer_d 
  zclPowerCfgAttrsRAM_t         PwrCfgAttrs;
#endif   
  zclAttrKeyEstabAttr_t	KeyEstabAttrs;
  seTunnelingAttrRAM_t	TunnelingAttrs;
#if gZcl_EnablePriceServer_d || gZcl_EnablePriceClient_d  
  zclPriceAttrRAM_t  	PriceAttrs;
#endif  
  zclMetAttrRAM_t		MeteringAttr;
  zclPrepaymentAttrRAM_t  	PrepaymentAttrs;
}devMeterAttrRAM_t;

typedef PACKED_STRUCT devIPDAttrRAM_tag {
  uint8_t              reportMask[1];  /* allows up to 8 reportable endpoints in this node  */
  zclAttrKeyEstabAttr_t	KeyEstabAttrs;
  zclDRLCClientAttr_t  	DRLCAttrs;
  seTunnelingAttrRAM_t	TunnelingAttrs;
#if gZcl_EnablePriceServer_d || gZcl_EnablePriceClient_d  
  zclPriceAttrRAM_t  	PriceAttrs;
#endif  
}devIPDAttrRAM_t;

typedef PACKED_STRUCT devESIAttrRAM_tag {
  uint8_t                       reportMask[1];  /* allows up to 8 reportable endpoints in this node  */
  zclBasicAttrsRAM_t            BasicAttrs;     /* RAM attributes from the Basic cluster */
#if gZclEnablePwrCfgServer_d 
  zclPowerCfgAttrsRAM_t         PwrCfgAttrs;
#endif  
  ZCLTimeServerAttrsRAM_t       TimeAttrs;     /* RAM attributes from the Time cluster */
  zclAttrKeyEstabAttr_t	        KeyEstabAttrs;
  seTunnelingAttrRAM_t	        TunnelingAttrs;
#if gZcl_EnablePriceServer_d || gZcl_EnablePriceClient_d 
  zclPriceAttrRAM_t  	        PriceAttrs;
#endif  
#if gZclEnableMeteringServer_d
  zclMetAttrRAM_t		MeteringAttr;
#endif
  zclPrepaymentAttrRAM_t  	PrepaymentAttrs;
} devESIAttrRAM_t;

typedef PACKED_STRUCT zclBasicAttrMirrorRAM_tag {
  uint8_t    ZCLVersion;           /* 0x0000 ZCLVersion - 0x01 - RdOnly - M */
  uint8_t    iPowerSource;       /* 0x0007 PowerSource - 0x01(mains) - M */  
} zclBasicAttrMirrorRAM_t;

typedef PACKED_STRUCT zclMeteringAttrMirrorRAM_tag {
  uint32_t				NotificationFlags[8];
  IntrvForProfiling_t   ChangeReportProfile;
  uint8_t               NotificationReporting;
  uint8_t               NotificationScheme;

  Summ_t     RISCurrSummDlvrd;
#if gZclClusterOptionals_d  
  Summ_t    RISDFTSumm;
  uint16_t    RISDailyFreezeTime;
  ZCLTime_t RISReadingSnapShotTime;
#endif  
  uint8_t MSStatus;
  uint8_t   SmplMetCFSUnitofMeasure;
#if gZclClusterOptionals_d    
  Consmp  Mult;
  Consmp  Div;
#endif  
  uint8_t   CFSSummFormat;
  uint8_t   CFSMetDevType;  
#if gZclClusterOptionals_d    
  uint8_t MaxNumberOfPeriodsDlvrd;
#endif  
} zclMeteringAttrMirrorRAM_t;

typedef PACKED_STRUCT devESIMirrorAttrRAM_tag
{
  zclBasicAttrMirrorRAM_t		BasicAttrs;
  zclMeteringAttrMirrorRAM_t	MeteringAttrs;
} devESIMirrorAttrRAM_t;

typedef PACKED_STRUCT seESIDescriptor_tag {
  zbNwkAddr_t  NwkAddr;
  zbEndPoint_t EndPoint;
  uint8_t      TimeStatus;
  uint8_t      EntryStatus; /* the tabel entry is in use (0xFF) or not (0x00) */
} seESIDescriptor_t;

typedef PACKED_STRUCT seDeviceDescriptor_tag {
  zbNwkAddr_t   aNwkAddr;
  uint16_t      MaxOutTransferSize; /*Maximum Outgoing Transfer Size*/
  uint8_t       EntryStatus; /* the tabel entry is in use (0xFF) or not (0x00) */
} seDeviceDescriptor_t;

typedef PACKED_STRUCT seRegDev_tag {
  zbIeeeAddr_t aExtAddr;
  zbAESKey_t   aHashedKey;
  zbAESKey_t   aInstallCode;
} seRegDev_t;


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

extern const zclAttrSetList_t gZclKeyEstabAttrSetList;
extern const zclAttrSetList_t gZclMetAttrSetList;
extern const zclAttrSetList_t gZclMetMirrorAttrSetList;
extern const zclAttrSetList_t gZclDRLCClientClusterAttrSetList;
extern const zclAttrSetList_t gZclPriceAttrSetList;
extern const zclAttrSetList_t gZclPriceServerAttrSetList;
extern const zclAttrSetList_t gZclPriceClientAttrSetList;
extern const zclAttrSetList_t gZclSETunnelingServerAttrSetList;
extern const zclAttrSetList_t gZclPrepaymentAttrSetList;

extern IdentifyCert_t DeviceImplicitCert;
extern uint8_t DevicePrivateKey[gZclCmdKeyEstab_PrivateKeySize_c];
extern uint8_t DevicePublicKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
extern const uint8_t CertAuthPubKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
extern const uint8_t CertAuthIssuerID[8];
#ifdef __cplusplus
}
#endif
#endif 
/* _SEPROFILE_H */

