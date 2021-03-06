/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file Phy.h
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __PHY_H__
#define __PHY_H__


/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"
#include "PhyInterface.h"
#include "fsl_os_abstraction.h"


/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
 
#ifdef __cplusplus
    extern "C" {
#endif

#ifndef gSnifferCRCEnabled_d
#define gSnifferCRCEnabled_d          (0)
#endif      

#ifndef gUseStandaloneCCABeforeTx_d
#define gUseStandaloneCCABeforeTx_d   (1)
#endif

/* Reduce the number of XCVR SPI reads/writes to improve Phy response time */
#ifndef gPhyUseReducedSpiAccess_d
#define gPhyUseReducedSpiAccess_d     (0)
#endif

/* The gPhyRxPBTransferThereshold_d must be enabled in order to use the Phy neighbor table */
#ifndef gPhyNeighborTableSize_d
#define gPhyNeighborTableSize_d       (0)
#endif
        
/* Start the SPI transfer of the XCVR PacketBuffer when the RX reached the specified threshold */
#ifndef gPhyRxPBTransferThereshold_d
#define gPhyRxPBTransferThereshold_d  (0)
#endif

/* The interval at which the Phy will poll for a new buffer in order to start the RX */
#ifndef gPhyRxRetryInterval_c
#define gPhyRxRetryInterval_c         (60) /* [symbols] */
#endif

#ifndef gPhyDeferTxPBwrite_c
#define gPhyDeferTxPBwrite_c          (0)
#endif
        
#define PhyGetSeqState()              PhyPpGetState()
#define PhyPlmeForceTrxOffRequest()   PhyAbort()

#ifdef gPHY_802_15_4g_d
#ifndef gPhyIndirectQueueSize4g_c
#define gPhyIndirectQueueSize4g_c     (10)
#endif
#endif
      
/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */
/* PHY states */
enum {
  gIdle_c,
  gRX_c,
  gTX_c,
  gCCA_c,
  gTR_c,
  gCCCA_c,
#ifdef gPHY_802_15_4g_d
  gPN9_c,
  gED_c
#endif  /* gPHY_802_15_4g_d */
};

/* PHY channel state */
enum {
  gChannelIdle_c,
  gChannelBusy_c
};

/* PANCORDNTR bit in PP */
enum {
  gMacRole_DeviceOrCoord_c,
  gMacRole_PanCoord_c
};

/* Cca types */
enum {
  gCcaED_c,            /* energy detect - CCA bit not active, not to be used for T and CCCA sequences */
  gCcaCCA_MODE1_c,     /* energy detect - CCA bit ACTIVE */
  gCcaCCA_MODE2_c,     /* 802.15.4 compliant signal detect - CCA bit ACTIVE */
  gCcaCCA_MODE3_c,     /* 802.15.4 compliant signal detect and energy detect - CCA bit ACTIVE */
  gInvalidCcaType_c    /* illegal type */
};

enum {
  gNormalCca_c,
  gContinuousCca_c
};

typedef struct Phy_PhyLocalStruct_tag
{
    PD_MAC_SapHandler_t         PD_MAC_SapHandler;
    PLME_MAC_SapHandler_t       PLME_MAC_SapHandler;
    event_t                     phyTaskEventId;  
    msgQueue_t                  macPhyInputQueue;
    uint32_t                    maxFrameWaitTime;
    phyMessageHeader_t         *pReq;
    volatile phyTxParams_t      txParams;
    volatile phyRxParams_t      rxParams;
    volatile phyChannelParams_t channelParams;
#ifdef gPHY_802_15_4g_d
    volatile phyFlags_t         flags;
    phyTime_t                   startTime;
    uint16_t                    phyUnavailableQueuePos;
    uint16_t                    phyIndirectQueue[gPhyIndirectQueueSize4g_c];
    uint16_t                    fcs;
    uint8_t                     macPanID[2];
    uint8_t                     macShortAddress[2];
    uint8_t                     macLongAddress[8];
#else
    volatile uint8_t            flags;
#endif  /* gPHY_802_15_4g_d */
    uint8_t                     currentMacInstance;
}Phy_PhyLocalStruct_t;


/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */

/* PHY Packet Processor */

/*---------------------------------------------------------------------------
 * Name: PhyHwInit
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyHwInit
( 
  void 
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetPromiscuous
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetPromiscuous
(
  bool_t mode
);

/*---------------------------------------------------------------------------
* Name: PhySetActivePromState()
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
void PhySetActivePromiscuous
(
bool_t state
);

/*---------------------------------------------------------------------------
* Name: PhyGetActivePromiscuous()
* Description: -
* Parameters: -
* Return: - TRUE/FALSE
*---------------------------------------------------------------------------*/
bool_t PhyGetActivePromiscuous
(
void
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetPanId
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPpSetPanId
(
  uint8_t *pPanId,
  uint8_t pan
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetShortAddr
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPpSetShortAddr
(
  uint8_t *pShortAddr,
  uint8_t pan
);


/*---------------------------------------------------------------------------
 * Name: PhyPpSetLongAddr
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPpSetLongAddr
(
  uint8_t *pLongAddr,
  uint8_t pan
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetMacRole
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPpSetMacRole
(
  bool_t macRole,
  uint8_t pan
);


/*---------------------------------------------------------------------------
 * Name: PhyPpIsTxAckDataPending
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyPpIsTxAckDataPending
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyPpIsRxAckDataPending
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyPpIsRxAckDataPending
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetFpManually
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetFpManually
(
  bool_t FP
);

/*---------------------------------------------------------------------------
 * Name: PhyPpIsPollIndication
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyPpIsPollIndication
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetSAMState
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPpSetSAMState
(
  bool_t state
);

/*---------------------------------------------------------------------------
 * Name: Phy_IndirectQueueChecksum
 * Description: Function called to compute the checksum for a 16bit or 64bit address
 * in the same way as the transceiver
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
#ifdef gPHY_802_15_4g_d
uint16_t Phy_IndirectQueueChecksum
(
  bool_t addrType, 
  uint64_t address, 
  uint16_t panId
);
#endif  /* gPHY_802_15_4g_d */

/*---------------------------------------------------------------------------
 * Name: PhyPp_IndirectQueueInsert
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPp_IndirectQueueInsert
(
  uint8_t  index,
  uint16_t checkSum,
  instanceId_t instanceId
);

/*---------------------------------------------------------------------------
 * Name: PhyPp_RemoveFromIndirect
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPp_RemoveFromIndirect
(
  uint8_t index,
  instanceId_t instanceId
);

/*---------------------------------------------------------------------------
 * Name: PhyPpGetState
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPpGetState
(
  void
);

#ifdef gPHY_802_15_4g_d
/*---------------------------------------------------------------------------
 * Name: PhySetState
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhySetState
(
  uint8_t phyState
);

/*---------------------------------------------------------------------------
 * Name: PhyPrepareTx
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPrepareTx
(
  pdDataReq_t *pTxPacket
);
#endif  /* gPHY_802_15_4g_d */

/*---------------------------------------------------------------------------
 * Name: PhyAbort
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyAbort
(
  void
);

/* PHY PLME & DATA primitives */

/*---------------------------------------------------------------------------
 * Name: PhyPdDataRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPdDataRequest
(  
  pdDataReq_t *pTxPacket,  
  volatile phyRxParams_t *pRxParams, 
  volatile phyTxParams_t *pTxParams
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeRxRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPlmeRxRequest
(
  phySlottedMode_t phyRxMode,
  phyRxParams_t *  pRxParams
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeTimmedRxRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPlmeTimmedRxRequest( phyRxParams_t *  pRxParams );

/*---------------------------------------------------------------------------
 * Name: PhyPlmeCcaEdRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPlmeCcaEdRequest
(
  phyCCAType_t     ccaParam,
  phyContCCAMode_t cccaMode
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetCurrentChannelRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPlmeSetCurrentChannelRequest
(
  uint8_t channel,
  uint8_t pan
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeGetCurrentChannelRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeGetCurrentChannelRequest
(
  uint8_t pan
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetPwrLevelRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPlmeSetPwrLevelRequest
(
  uint8_t pwrStep
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeGetPwrLevelRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPlmeGetPwrLevelRequest
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetPIBRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPlmeSetPIBRequest
(
  phyPibId_t pibId,
  uint64_t pibValue,
  uint8_t phyRegistrySet,
  instanceId_t instanceId
);

/*---------------------------------------------------------------------------
 * Name: PhyPlmeGetPIBRequest
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyStatus_t PhyPlmeGetPIBRequest
(
  phyPibId_t pibId,
  uint64_t * pibValue,
  uint8_t phyRegistrySet, 
  instanceId_t instanceId
);

/*---------------------------------------------------------------------------
 * Name: PhyPpSetCcaThreshold
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/

phyStatus_t PhyPpSetCcaThreshold
(
  uint8_t ccaThreshold
);

/* PHY Time */

/*---------------------------------------------------------------------------
 * Name: PhyTimeSetEventTrigger
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetEventTrigger
(
  phyTime_t startTime
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeSetEventTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetEventTimeout
(
  phyTime_t *pEndTime
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeGetEventTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyTime_t PhyTimeGetEventTimeout( void );

#ifdef gPHY_802_15_4g_d
/*---------------------------------------------------------------------------
 * Name: PhyTimeSetRxTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetRxTimeout
(
  phyTime_t *pRxTimeout
);
#endif  /* gPHY_802_15_4g_d */

/*---------------------------------------------------------------------------
 * Name: PhyTimeReadClock
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeReadClock
(
  phyTime_t *pRetClk
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeDisableEventTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeDisableEventTimeout
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeDisableEventTrigger
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeDisableEventTrigger
(
  void
);


#ifdef gPHY_802_15_4g_d
/*---------------------------------------------------------------------------
 * Name: PhyTimeDisableRxTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeDisableRxTimeout
(
  void
);
#endif  /* gPHY_802_15_4g_d */

/*---------------------------------------------------------------------------
 * Name: PhyTimeSetWakeUpTime
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetWakeUpTime
(
  uint32_t *pWakeUpTime
);
/*---------------------------------------------------------------------------
 * Name: PhyTimeInitEventTimer
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeInitEventTimer
(
  uint32_t *pAbsTime
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeIsWakeUpTimeExpired
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PhyTimeIsWakeUpTimeExpired
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeSetWaitTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeSetWaitTimeout
(
  phyTime_t *pWaitTimeout
);

/*---------------------------------------------------------------------------
 * Name: PhyTimeDisableWaitTimeout
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyTimeDisableWaitTimeout
(
  void
);

/* PHY ISR */

/*---------------------------------------------------------------------------
 * Name: PHY_InstallIsr
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PHY_InstallIsr
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PHY_InterruptHandler
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PHY_InterruptHandler
(
  void
);


/*---------------------------------------------------------------------------
 * Name: PhyIsrPassRxParams()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyIsrPassRxParams
(
  volatile phyRxParams_t * pRxParam
);

#ifdef gPHY_802_15_4g_d
/*---------------------------------------------------------------------------
 * Name: PhyIsrPassRxParams()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PhyPassTxParams
(
  pdDataReq_t *pTxParam
);
#endif /* gPHY_802_15_4g_d */

/*---------------------------------------------------------------------------
 * Name: PhyIsrPassTaskParams()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyIsrPassTaskParams
(
  instanceId_t instanceId
);


/*---------------------------------------------------------------------------
 * Name: PhyIsrTimeoutCleanup
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyIsrTimeoutCleanup
(
  void
);

/*---------------------------------------------------------------------------
 * Name: PhyIsrSeqCleanup
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyIsrSeqCleanup
(
  void
);

/*****************************************************************************
* PhyGetRandomNo function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyGetRandomNo
(
  uint32_t *pRandomNo
);

/*****************************************************************************
* PhyPpSetDualPanAuto function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPpSetDualPanAuto
(
  bool_t mode
);

/*****************************************************************************
* PhyPpGetDualPanAuto function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
bool_t PhyPpGetDualPanAuto
(
   void
);

/*****************************************************************************
* PhyPpSetDualPanDwell function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPpSetDualPanDwell
(
  uint8_t
);

/*****************************************************************************
* PhyPpGetDualPanDwell function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
uint8_t PhyPpGetDualPanDwell
(
  void
);

/*****************************************************************************
* PhyPpGetDualPanRemain function
*
* Interface assumptions:
*
* Return Value:
* The remaining time until a channel switch will occure
*****************************************************************************/
uint8_t PhyPpGetDualPanRemain
(
  void
);

/*****************************************************************************
* PhyPpSetDualPanSamLvl function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPpSetDualPanSamLvl
(
  uint8_t
);

/*****************************************************************************
* PhyPpGetDualPanSamLvl function
*
* Interface assumptions:
*
* Return Value:
* The level at which the HW queue is split for the two PANs
*****************************************************************************/
uint8_t PhyPpGetDualPanSamLvl
(
  void
);

/*****************************************************************************
* PhyPpSetDualPanSamLvl function
*
* Interface assumptions:
*
* Return Value:
* None
*****************************************************************************/
void PhyPpSetDualPanActiveNwk
(
  uint8_t
);

/*****************************************************************************
* PhyPpGetDualPanActiveNwk function
*
* Interface assumptions:
*
* Return Value:
* The current NWK on which the PHY is operating
*****************************************************************************/
uint8_t PhyPpGetDualPanActiveNwk
(
  void
);

/*****************************************************************************
* PhyPpGetPanOfRxPacket function
*
* Interface assumptions:
*
* Return Value:
* The PAN on which the packet was received (can be receiced on both PANs)
*****************************************************************************/
uint8_t PhyPpGetPanOfRxPacket
(
  void
);

/*****************************************************************************
* PhyGetLastRxLqiValue function
*
* Interface assumptions:
*
* Return Value:
* The LQI value for the last received packet
*****************************************************************************/
uint8_t PhyGetLastRxLqiValue(void);

/*****************************************************************************
* PhyGetLastRxRssiValue function
*
* Interface assumptions:
*
* Return Value:
* The RSSI value for the last received packet
*****************************************************************************/
uint8_t PhyGetLastRxRssiValue(void);

/*! *********************************************************************************
* \brief  This function converts the LQI reported by the PHY into an signed RSSI value
*
* \param[in]  LQI  the LQI reported by the PHY
*
* \return  the RSSI value in dbm
*
********************************************************************************** */
int8_t PhyConvertLQIToRSSI(uint8_t LQI);

/*****************************************************************************
* PhyPlmeSetFADStateRequest function
*
* Interface assumptions: state
*
* Return Value: gPhySuccess
*
* Description: Enable the FAD function (FAD_EN bit)
*****************************************************************************/
uint8_t PhyPlmeSetFADStateRequest(bool_t state);

/*****************************************************************************
* PhyPlmeSetFADThresholdRequest function
*
* Interface assumptions: FADThreshold
*
* Return Value: gPhySuccess
*
* Description: Correlator threshold at which the FAD will select the antenna
*****************************************************************************/
uint8_t PhyPlmeSetFADThresholdRequest(uint8_t FADThreshold);

uint8_t PhyPlmeSetANTPadStateRequest(bool_t antAB_on, bool_t rxtxSwitch_on);
uint8_t PhyPlmeSetANTPadStrengthRequest(bool_t hiStrength);
uint8_t PhyPlmeSetANTPadInvertedRequest(bool_t invAntA, bool_t invAntB, bool_t invTx, bool_t invRx);

/*****************************************************************************
* PhyPlmeSetANTXStateRequest function
*
* Interface assumptions: state
*
* Return Value: gPhySuccess
*
* Description: ANTX_IN - FAD Antenna start when FAD_EN = 1 or antenna selected
*              when FAD_EN=0
*****************************************************************************/
uint8_t PhyPlmeSetANTXStateRequest(bool_t state);

/*****************************************************************************
* PhyPlmeGetANTXStateRequest function
*
* Interface assumptions: none
*
* Return Value: Chosen antenna by the FAD (FAD_EN = 1) or copy of ANTX_IN
*
* Description: Antenna selected in FAD of non-FAD mode
*****************************************************************************/
uint8_t PhyPlmeGetANTXStateRequest(void);

/*****************************************************************************
* PhyPlmeSetLQIModeRequest function
*
* Interface assumptions: none
*
* Return Value: gPhySuccess
*
* Description: Choose LQI Mode: 1 - LQI Based on RSSI, 
*                               0 - LQI Based on Correlation Peaks
*****************************************************************************/
uint8_t PhyPlmeSetLQIModeRequest(uint8_t lqiMode);

/*****************************************************************************
* PhyPlmeGetRSSILevelRequest function
*
* Interface assumptions: none
*
* Return Value: RSSI level
*
* Description: Returns the RSSI level value, refreshed every 125us
*****************************************************************************/
uint8_t PhyPlmeGetRSSILevelRequest(void);

#ifdef gPHY_802_15_4g_d

#ifdef gListenSupported_c
/*****************************************************************************
* PhySetRxToListen function
*
* Interface assumptions: none
*
* Return Value: 
*
* Description: 
*****************************************************************************/
phyStatus_t PhySetRxToListen(bool_t listenOn, instanceId_t instanceId );
#endif
/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetCslRxEnabled()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPlmeSetCslRxEnabled( bool_t cslRxEnabled, instanceId_t instanceId );

/*---------------------------------------------------------------------------
 * Name: PhyPlmeSetTschEnabled()
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PhyPlmeSetTschEnabled( bool_t tschEnabled, instanceId_t instanceId );
#endif   /* gPHY_802_15_4g_d */

/*****************************************************************************
* PhySetRxOnWhenIdle function
*
* Interface assumptions: none
*
* Return Value: None.
*
* Description: Informs the PHY if it should start an RX when entering IDLE or not
*****************************************************************************/
void PhyPlmeSetRxOnWhenIdle( bool_t state, instanceId_t instanceId );

/*****************************************************************************
* PhyPlmeSetFrameWaitTime function
*
* Interface assumptions: none
*
* Return Value: None.
*
* Description: Set the amount of time in symbols to wait for an data frame 
*              after receiving an ACK with FP=1
*****************************************************************************/
void PhyPlmeSetFrameWaitTime( uint32_t time, instanceId_t instanceId );

/*****************************************************************************
* Phy_SetSequenceTiming function
*
* Interface assumptions: none
*
* Return Value: None.
*
* Description:  
*              
*****************************************************************************/
#ifndef gPHY_802_15_4g_d
void Phy_SetSequenceTiming(phyTime_t startTime, uint32_t seqDuration);
#else
void Phy_SetSequenceTiming(phyTime_t startTime, uint32_t seqDuration, uint8_t nextState, instanceId_t instanceId);
#endif  /* gPHY_802_15_4g_d */

uint8_t Phy_GetEnergyLevel(uint8_t energyLeveldB);

uint16_t PhyGetChecksum(uint8_t *pAddr, uint8_t addrMode, uint16_t PanId);
uint8_t  PhyAddToNeighborTable(uint8_t *pAddr, uint8_t addrMode, uint16_t PanId);
uint8_t  PhyRemoveFromNeighborTable(uint8_t *pAddr, uint8_t addrMode, uint16_t PanId);
bool_t   PhyCheckNeighborTable(uint16_t checksum);

/* RADIO EVENTS */

void Radio_Phy_PdDataConfirm(instanceId_t instanceId, bool_t framePending);

void Radio_Phy_TimeWaitTimeoutIndication(instanceId_t instanceId);

void Radio_Phy_TimeRxTimeoutIndication(instanceId_t instanceId);

void Radio_Phy_PdDataIndication(instanceId_t instanceId);

void Radio_Phy_TimeStartEventIndication(instanceId_t instanceId);

void Radio_Phy_PlmeCcaConfirm(phyStatus_t phyChannelStatus, instanceId_t instanceId);

void Radio_Phy_PlmeEdConfirm(uint8_t energyLeveldB, instanceId_t instanceId);

void Radio_Phy_PlmeSyncLossIndication(instanceId_t instanceId);

void Radio_Phy_PlmeRxSfdDetect(instanceId_t instanceId, uint32_t param);

void Radio_Phy_PlmeFilterFailRx(instanceId_t instanceId);

void Radio_Phy_UnexpectedTransceiverReset(instanceId_t instanceId);

void Radio_Phy_DummyEvent(instanceId_t instanceId);

bool_t PhyIsIdleRx( instanceId_t instanceId );

#ifdef __cplusplus
}
#endif

#endif /* __PHY_H__ */
