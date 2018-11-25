/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file PhyPacketProcessor.c
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


/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"
#include "MpmInterface.h"

#include "Phy.h"
#include "overwrites.h"
#include "KW4xXcvrDrv.h"
#include "ifr_apache_radio.h"

#include "fsl_os_abstraction.h"
#include "fsl_device_registers.h"


/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */
// Address mode indentifiers. Used for both network and MAC interfaces
#define gPhyAddrModeNoAddr_c        (0)
#define gPhyAddrModeInvalid_c       (1)
#define gPhyAddrMode16BitAddr_c     (2)
#define gPhyAddrMode64BitAddr_c     (3)

#define gPhyHwIndQueueSize_d        (128)

/*! *********************************************************************************
*************************************************************************************
* Private functions prototype
*************************************************************************************
********************************************************************************** */
extern void ProtectFromXcvrInterrupt(void);
extern void UnprotectFromXcvrInterrupt(void);


/*! *********************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
#if gPhyEnableSAA_c
/* Limit HW indirect queue size to ~10% */
const uint8_t gPhyIndirectQueueSize_c = gPhyHwIndQueueSize_d/10;
#else
const uint8_t gPhyIndirectQueueSize_c = gPhyHwIndQueueSize_d;
#endif


/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
* \brief  Initialize the 802.15.4 Radio registers
*
********************************************************************************** */
void PhyHwInit
(
void
)
{
    uint32_t i;

    XcvrInit( ZIGBEE );

    // enable 16 bit mode for TC2 - TC2 prime EN, disable all timers,
    // enable AUTOACK, mask all interrupts
    ZLL_PHY_CTRL = (gCcaCCA_MODE1_c << ZLL_PHY_CTRL_CCATYPE_SHIFT) |
                   ZLL_PHY_CTRL_TC2PRIME_EN_MASK    |
                   ZLL_PHY_CTRL_PB_ERR_MSK_MASK     |
                   ZLL_PHY_CTRL_CRC_MSK_MASK        |
                   ZLL_PHY_CTRL_PLL_UNLOCK_MSK_MASK |
                   ZLL_PHY_CTRL_FILTERFAIL_MSK_MASK |
                   ZLL_PHY_CTRL_RX_WMRK_MSK_MASK    |
                   ZLL_PHY_CTRL_CCAMSK_MASK         |
                   ZLL_PHY_CTRL_RXMSK_MASK          |
                   ZLL_PHY_CTRL_TXMSK_MASK          |
                   ZLL_PHY_CTRL_SEQMSK_MASK         |
                   ZLL_PHY_CTRL_AUTOACK_MASK        |
                   ZLL_PHY_CTRL_TRCV_MSK_MASK;

    // clear all PP IRQ bits to avoid unexpected interrupts immediately after init
    // disable all timer interrupts
    ZLL_IRQSTS = ZLL_IRQSTS;

    // enable Source Addresing Match module
    ZLL_BWR_SAM_CTRL_SAP0_EN(ZLL, 1);
#if (gMpmIncluded_d)
    ZLL_BWR_SAM_CTRL_SAP1_EN(ZLL, 1);
    ZLL_BWR_SAM_CTRL_SAP1_START(ZLL, gPhyHwIndQueueSize_d/2);
#if gPhyEnableSAA_c
    ZLL_BWR_SAM_CTRL_SAA0_EN(ZLL, 1);
    ZLL_BWR_SAM_CTRL_SAA0_START(ZLL, gPhyIndirectQueueSize_c/2);
    ZLL_BWR_SAM_CTRL_SAA1_EN(ZLL, 1);
    ZLL_BWR_SAM_CTRL_SAA1_START(ZLL, gPhyHwIndQueueSize_d/2 + gPhyIndirectQueueSize_c/2);
#endif

#elif (gPhyEnableSAA_c)
    ZLL_BWR_SAM_CTRL_SAA0_EN(ZLL, 1);
    ZLL_BWR_SAM_CTRL_SAA0_START(ZLL, gPhyIndirectQueueSize_c);
#endif

    // Clear HW indirect queue
    ZLL_SAM_TABLE |= ZLL_SAM_TABLE_INVALIDATE_ALL_MASK;

    //  Frame Filtering
    //  FRM_VER[7:6] = b11. Accept FrameVersion 0 and 1 packets, reject all others
    ZLL_RX_FRAME_FILTER = ZLL_RX_FRAME_FILTER_FRM_VER_MASK |
                          ZLL_RX_FRAME_FILTER_CMD_FT_MASK  |
                          ZLL_RX_FRAME_FILTER_DATA_FT_MASK |
                          ZLL_RX_FRAME_FILTER_BEACON_FT_MASK;

    // Set 802.15.4 register overwrites
    //gcapraru: ignore for now
//    for( i=0; i<NumberOfElements(overwrites_common); i++ )
//        *((uint32_t*)overwrites_common[i].address) = overwrites_common[i].data;
//
//    for( i=0; i<NumberOfElements(overwrites_802p15p4); i++ )
//        *((uint32_t*)overwrites_802p15p4[i].address) = overwrites_802p15p4[i].data;

    // Set prescaller to obtain 1 symbol (16us) timebase
   ZLL_TMR_PRESCALE = 0x05;

    // set CCA threshold to -75 dBm
    ZLL_BWR_CCA_LQI_CTRL_CCA1_THRESH(ZLL, 0x4B);

    // set the default power level to -2dBm
    ZLL_PA_PWR = 0x08; //gcapraru ???

    // enable the RxWatermark IRQ and FilterFail IRQ
//    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_FILTERFAIL_MSK_MASK;
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_RX_WMRK_MSK_MASK;
    // set Rx watermark level
    ZLL_WR_RX_WTR_MARK(ZLL, 0 );

    // set default channels
    PhyPlmeSetCurrentChannelRequest(0x0B, 0); //2405 MHz
    PhyPlmeSetCurrentChannelRequest(0x0B, 1); //2405 MHz

    // install PHY ISR
    PHY_InstallIsr();
}

/*! *********************************************************************************
* \brief  Aborts the current sequence and force the radio to IDLE
*
********************************************************************************** */
void PhyAbort
(
void
)
{
    uint32_t temp;

    // Mask XCVR irq
    ProtectFromXcvrInterrupt();

    // Disable timer trigger (for scheduled XCVSEQ)
    if( ZLL_PHY_CTRL & ZLL_PHY_CTRL_TMRTRIGEN_MASK )
    {
        ZLL_BWR_PHY_CTRL_XCVSEQ(ZLL, 0);
        // give the FSM enough time to start if it was triggered
        temp = (ZLL_EVENT_TMR + 2) & 0x00FFFFFF;
        while( temp != ZLL_EVENT_TMR );
    }

    //gcapraru:
    /* while(XCVR_BRD_STATUS_TSM_COUNT(XCVR) == 0); */

    // If XCVR is not idle, abort current SEQ
    if( ZLL_PHY_CTRL & ZLL_PHY_CTRL_XCVSEQ_MASK )
    {
        ZLL_BWR_PHY_CTRL_XCVSEQ(ZLL, gIdle_c);
        // wait for Sequence Idle (if not already)
        while( ZLL_SEQ_STATE & ZLL_SEQ_STATE_SEQ_STATE_MASK );
    }

    // mask SEQ interrupt
    ZLL_BWR_PHY_CTRL_SEQMSK(ZLL, 1);
    // stop timers
    ZLL_BWR_PHY_CTRL_TMR2CMP_EN(ZLL, 0);
    ZLL_BWR_PHY_CTRL_TMR3CMP_EN(ZLL, 0);
    ZLL_BWR_PHY_CTRL_TC3TMOUT(ZLL, 0);
    // clear all PP IRQ bits to avoid unexpected interrupts( do not change TMR1 and TMR4 IRQ status )
    ZLL_IRQSTS &= ~(ZLL_IRQSTS_TMR1IRQ_MASK | ZLL_IRQSTS_TMR4IRQ_MASK);

    PhyIsrPassRxParams(NULL);

    // Unmask XCVR irq
    UnprotectFromXcvrInterrupt();
}

/*! *********************************************************************************
* \brief  Get the state of the ZLL
*
* \return  uint8_t state
*
********************************************************************************** */
uint8_t PhyPpGetState
(
void
)
{
    return ZLL_RD_PHY_CTRL_XCVSEQ(ZLL);
}

/*! *********************************************************************************
* \brief  Set the value of the MAC PanId
*
* \param[in]  pPanId
* \param[in]  pan
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPpSetPanId
(
uint8_t *pPanId,
uint8_t pan
)
{
#ifdef PHY_PARAMETERS_VALIDATION
    if(NULL == pPanId)
    {
        return gPhyInvalidParameter_c;
    }
#endif // PHY_PARAMETERS_VALIDATION

    if( 0 == pan )
        ZLL_WR_MACSHORTADDRS0_MACPANID0( ZLL, *((uint16_t*)pPanId) );
    else
        ZLL_WR_MACSHORTADDRS1_MACPANID1( ZLL, *((uint16_t*)pPanId) );

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  Set the value of the MAC Short Address
*
* \param[in]  pShortAddr
* \param[in]  pan
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPpSetShortAddr
(
uint8_t *pShortAddr,
uint8_t pan
)
{
#ifdef PHY_PARAMETERS_VALIDATION
    if(NULL == pShortAddr)
    {
        return gPhyInvalidParameter_c;
    }
#endif // PHY_PARAMETERS_VALIDATION

    if( pan == 0 )
        ZLL_WR_MACSHORTADDRS0_MACSHORTADDRS0( ZLL, *((uint16_t*)pShortAddr) );
    else
        ZLL_WR_MACSHORTADDRS1_MACSHORTADDRS1( ZLL, *((uint16_t*)pShortAddr) );

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  Set the value of the MAC extended address
*
* \param[in]  pLongAddr
* \param[in]  pan
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPpSetLongAddr
(
uint8_t *pLongAddr,
uint8_t pan
)
{
#ifdef PHY_PARAMETERS_VALIDATION
    if(NULL == pLongAddr)
    {
        return gPhyInvalidParameter_c;
    }
#endif // PHY_PARAMETERS_VALIDATION

    if( 0 == pan )
    {
        ZLL_WR_MACLONGADDRS0_LSB(ZLL, *((uint32_t*)pLongAddr) );
        pLongAddr += sizeof(uint32_t);
        ZLL_WR_MACLONGADDRS0_MSB(ZLL, *((uint32_t*)pLongAddr) );
    }
    else
    {
        ZLL_WR_MACLONGADDRS1_LSB(ZLL, *((uint32_t*)pLongAddr) );
        pLongAddr += sizeof(uint32_t);
        ZLL_WR_MACLONGADDRS1_MSB(ZLL, *((uint32_t*)pLongAddr) );
    }

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  Set the MAC PanCoordinator role
*
* \param[in]  macRole
* \param[in]  pan
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPpSetMacRole
(
  bool_t macRole,
  uint8_t pan
)
{
    uint8_t panCoord;

    if(gMacRole_PanCoord_c == macRole)
        panCoord = 1;
    else
        panCoord = 0;

    if( 0 == pan )
    {
        ZLL_BWR_PHY_CTRL_PANCORDNTR0(ZLL, panCoord);
    }
    else
    {
        ZLL_BWR_DUAL_PAN_CTRL_PANCORDNTR1(ZLL, panCoord);
    }

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  Set the PHY in Promiscuous mode
*
* \param[in]  mode
*
********************************************************************************** */
void PhyPpSetPromiscuous
(
bool_t mode
)
{
    if( mode )
    {
        if( (ZLL_PHY_CTRL & ZLL_PHY_CTRL_PROMISCUOUS_MASK) ||
            (ZLL_RX_FRAME_FILTER & ZLL_RX_FRAME_FILTER_ACTIVE_PROMISCUOUS_MASK) )
            return;

        ZLL_BWR_PHY_CTRL_PROMISCUOUS(ZLL, 1);
    /* FRM_VER[7:6] = b00. Any FrameVersion accepted (0,1,2 & 3) */
    /* All frame types accepted*/
        ZLL_RX_FRAME_FILTER &= ~ZLL_RX_FRAME_FILTER_FRM_VER_MASK;
        ZLL_RX_FRAME_FILTER |= (ZLL_RX_FRAME_FILTER_ACK_FT_MASK |
                                ZLL_RX_FRAME_FILTER_NS_FT_MASK);
    }
    else
    {
        ZLL_BWR_PHY_CTRL_PROMISCUOUS(ZLL, 0);
        /* FRM_VER[7:6] = b11. Accept FrameVersion 0 and 1 packets, reject all others */
        /* Beacon, Data and MAC command frame types accepted */
        ZLL_RX_FRAME_FILTER &= ~(ZLL_RX_FRAME_FILTER_FRM_VER_MASK |
                                 ZLL_RX_FRAME_FILTER_ACK_FT_MASK  |
                                 ZLL_RX_FRAME_FILTER_NS_FT_MASK   |
                                 ZLL_RX_FRAME_FILTER_ACTIVE_PROMISCUOUS_MASK);
        ZLL_RX_FRAME_FILTER |= 0x03 << ZLL_RX_FRAME_FILTER_FRM_VER_SHIFT;
    }
}

/*! *********************************************************************************
* \brief  Set the PHY in ActivePromiscuous mode
*
* \param[in]  state
*
********************************************************************************** */
void PhySetActivePromiscuous(bool_t state)
{
    if( state )
    {
        if( !(ZLL_PHY_CTRL & ZLL_PHY_CTRL_PROMISCUOUS_MASK) )
            return;

        /* Disable Promiscuous mode */
        ZLL_BWR_PHY_CTRL_PROMISCUOUS(ZLL, 0);
        ZLL_WR_RX_FRAME_FILTER_ACTIVE_PROMISCUOUS(ZLL, 1);
    }
    else
    {
        if( !(ZLL_RX_FRAME_FILTER & ZLL_RX_FRAME_FILTER_ACTIVE_PROMISCUOUS_MASK) )
            return;

        ZLL_WR_RX_FRAME_FILTER_ACTIVE_PROMISCUOUS(ZLL, 0);
        /* Enable Promiscuous mode */
        ZLL_BWR_PHY_CTRL_PROMISCUOUS(ZLL, 1);
    }
}

/*! *********************************************************************************
* \brief  Get the state of the ActivePromiscuous mode
*
* \return  bool_t state
*
********************************************************************************** */
bool_t PhyGetActivePromiscuous
(
void
)
{
    return ZLL_RD_RX_FRAME_FILTER_ACTIVE_PROMISCUOUS(ZLL);
}

/*! *********************************************************************************
* \brief  Set the state of the SAM HW module
*
* \param[in]  state
*
********************************************************************************** */
void PhyPpSetSAMState
(
  bool_t state
)
{
    ZLL_BWR_SAM_CTRL_SAP0_EN(ZLL, state);
#if gMpmIncluded_d
    ZLL_BWR_SAM_CTRL_SAP1_EN(ZLL, state);
#endif
}

/*! *********************************************************************************
* \brief  Add a new element to the PHY indirect queue
*
* \param[in]  index
* \param[in]  checkSum
* \param[in]  instanceId
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPp_IndirectQueueInsert
(
uint8_t  index,
uint16_t checkSum,
instanceId_t instanceId
)
{
    uint32_t temp;

    (void)instanceId;
    if( index >= gPhyHwIndQueueSize_d )
        return gPhyInvalidParameter_c;

    temp = ZLL_SAM_TABLE;
    temp &= ~(ZLL_SAM_TABLE_SAM_INDEX_MASK | ZLL_SAM_TABLE_SAM_CHECKSUM_MASK);

    temp |= (index << ZLL_SAM_TABLE_SAM_INDEX_SHIFT) |
            (checkSum << ZLL_SAM_TABLE_SAM_CHECKSUM_SHIFT) |
            ZLL_SAM_TABLE_SAM_INDEX_WR_MASK |
            ZLL_SAM_TABLE_SAM_INDEX_EN_MASK;
    ZLL_SAM_TABLE = temp;

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  Remove an eleent from the PHY indirect queue
*
* \param[in]  index
* \param[in]  instanceId
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPp_RemoveFromIndirect
(
uint8_t index,
instanceId_t instanceId
)
{
    uint32_t temp;
    if( index >= gPhyHwIndQueueSize_d )
        return gPhyInvalidParameter_c;

    temp = ZLL_SAM_TABLE;
    temp &= ~(ZLL_SAM_TABLE_SAM_INDEX_MASK);
    temp |= (index << ZLL_SAM_TABLE_SAM_INDEX_SHIFT) | ZLL_SAM_TABLE_SAM_INDEX_INV_MASK;
    ZLL_SAM_TABLE = temp;

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  Return TRUE if the received packet is a PollRequest
*
* \return  bool_t
*
********************************************************************************** */
bool_t PhyPpIsPollIndication
(
void
)
{
    return ZLL_RD_IRQSTS_PI(ZLL);
}

/*! *********************************************************************************
* \brief  Return the state of the FP bit of the received ACK
*
* \return  bool_t
*
********************************************************************************** */
bool_t PhyPpIsRxAckDataPending
(
void
)
{
    return ZLL_RD_IRQSTS_RX_FRM_PEND(ZLL);
}

/*! *********************************************************************************
* \brief  Return TRUE if there is data pending for the Poling Device
*
* \return  bool_t
*
********************************************************************************** */
bool_t PhyPpIsTxAckDataPending
(
void
)
{
    if( ZLL_SAM_CTRL & (ZLL_SAM_CTRL_SAP0_EN_MASK | ZLL_SAM_CTRL_SAP1_EN_MASK) )
    {
        return ZLL_RD_IRQSTS_SRCADDR(ZLL);
    }
    else
    {
        return ZLL_RD_SAM_TABLE_ACK_FRM_PND(ZLL);
    }
}

/*! *********************************************************************************
* \brief  Set the value of the CCA threshold
*
* \param[in]  ccaThreshold
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPpSetCcaThreshold
(
uint8_t ccaThreshold
)
{
    ZLL_BWR_CCA_LQI_CTRL_CCA1_THRESH( ZLL, ccaThreshold );
    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will set the value for the FAD threshold
*
* \param[in]  FADThreshold   the FAD threshold
*
* \return  phyStatus_t
*
********************************************************************************** */
uint8_t PhyPlmeSetFADThresholdRequest(uint8_t FADThreshold)
{
    XCVR_WR_FAD_THR(XCVR, FADThreshold );
    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will enable/disable the FAD
*
* \param[in]  state   the state of the FAD
*
* \return  phyStatus_t
*
********************************************************************************** */
uint8_t PhyPlmeSetFADStateRequest(bool_t state)
{
    ZLL_BWR_FAD_CTRL_FAD_EN(ZLL, state);

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will set the LQI mode
*
* \return  uint8_t
*
********************************************************************************** */
uint8_t PhyPlmeSetLQIModeRequest(uint8_t lqiMode)
{
    ZLL_BWR_CCA_LQI_CTRL_CCA3_AND_NOT_OR(ZLL, (lqiMode>0));

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will return the RSSI level
*
* \return  uint8_t
*
********************************************************************************** */
uint8_t PhyPlmeGetRSSILevelRequest(void)
{
    return ZLL_RD_LQI_AND_RSSI_RSSI(ZLL);
}

/*! *********************************************************************************
* \brief  This function will enable/disable the ANTX
*
* \param[in]  state   the state of the ANTX
*
* \return  phyStatus_t
*
********************************************************************************** */
uint8_t PhyPlmeSetANTXStateRequest(bool_t state)
{
    ZLL_BWR_FAD_CTRL_ANTX_EN(ZLL, state);

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will retrn the state of the ANTX
*
* \return  uint8_t
*
********************************************************************************** */
uint8_t PhyPlmeGetANTXStateRequest(void)
{
    return ZLL_BRD_FAD_CTRL_ANTX_EN(ZLL);
}

/*! *********************************************************************************
* \brief  Set the state of the Dual Pan Auto mode
*
* \param[in]  mode TRUE/FALSE
*
********************************************************************************** */
void PhyPpSetDualPanAuto
(
bool_t mode
)
{
    ZLL_BWR_DUAL_PAN_CTRL_DUAL_PAN_AUTO(ZLL, mode);
}

/*! *********************************************************************************
* \brief  Get the state of the Dual Pan Auto mode
*
* \return  bool_t state
*
********************************************************************************** */
bool_t PhyPpGetDualPanAuto
(
void
)
{
    return ZLL_BRD_DUAL_PAN_CTRL_DUAL_PAN_AUTO(ZLL);
}

/*! *********************************************************************************
* \brief  Set the dwell for the Dual Pan Auto mode
*
* \param[in]  dwell
*
********************************************************************************** */
void PhyPpSetDualPanDwell
(
uint8_t dwell
)
{
    ZLL_BWR_DUAL_PAN_CTRL_DUAL_PAN_DWELL( ZLL, dwell );
}

/*! *********************************************************************************
* \brief  Get the dwell for the Dual Pan Auto mode
*
* \return  uint8_t PAN dwell
*
********************************************************************************** */
uint8_t PhyPpGetDualPanDwell
(
void
)
{
    return ZLL_BRD_DUAL_PAN_CTRL_DUAL_PAN_DWELL(ZLL);
}

/*! *********************************************************************************
* \brief  Get the remeining time before a PAN switch occures
*
* \return  uint8_t remaining time
*
********************************************************************************** */
uint8_t PhyPpGetDualPanRemain
(
void
)
{
  return ZLL_BRD_DUAL_PAN_CTRL_DUAL_PAN_REMAIN(ZLL);
}

/*! *********************************************************************************
* \brief  Set the current active Nwk
*
* \param[in]  nwk index of the nwk
*
********************************************************************************** */
void PhyPpSetDualPanActiveNwk
(
uint8_t nwk
)
{
    ZLL_BWR_DUAL_PAN_CTRL_ACTIVE_NETWORK(ZLL, (nwk > 0));
}

/*! *********************************************************************************
* \brief  Return the index of the Acive PAN
*
* \return  uint8_t index
*
********************************************************************************** */
uint8_t PhyPpGetDualPanActiveNwk
(
void
)
{
  return ZLL_BRD_DUAL_PAN_CTRL_ACTIVE_NETWORK(ZLL);
}

/*! *********************************************************************************
* \brief  Returns the PAN bitmask for the last Rx packet.
*         A packet can be received on multiple PANs
*
* \return  uint8_t bitmask
*
********************************************************************************** */
uint8_t PhyPpGetPanOfRxPacket(void)
{
  uint8_t PanBitMask = 0;

  if( ZLL_DUAL_PAN_CTRL & ZLL_DUAL_PAN_CTRL_DUAL_PAN_AUTO_MASK )
  {
      if( ZLL_DUAL_PAN_CTRL & ZLL_DUAL_PAN_CTRL_RECD_ON_PAN0_MASK )
          PanBitMask |= (1<<0);

      if( ZLL_DUAL_PAN_CTRL & ZLL_DUAL_PAN_CTRL_RECD_ON_PAN1_MASK )
          PanBitMask |= (1<<1);
  }
  else
  {
      if(ZLL_DUAL_PAN_CTRL & ZLL_DUAL_PAN_CTRL_ACTIVE_NETWORK_MASK )
          PanBitMask |= (1<<1);
      else
          PanBitMask |= (1<<0);
	  
  }

  return PanBitMask;
}

/*! *********************************************************************************
* \brief  Get the indirect queue level at which the HW queue will be split between PANs
*
* \return  uint8_t level
*
********************************************************************************** */
uint8_t PhyPpGetDualPanSamLvl(void)
{
    return ZLL_BRD_SAM_CTRL_SAP1_START(ZLL);
}

/*! *********************************************************************************
* \brief  Set the indirect queue level at which the HW queue will be split between PANs
*
* \param[in]  level
*
********************************************************************************** */
void PhyPpSetDualPanSamLvl( uint8_t level )
{
    ZLL_BWR_SAM_CTRL_SAP1_START(ZLL,level);
    ZLL_BWR_SAM_CTRL_SAP1_EN(ZLL, (level > 0) );
}
