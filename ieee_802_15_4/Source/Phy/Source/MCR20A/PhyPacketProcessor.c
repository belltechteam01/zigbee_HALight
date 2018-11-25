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
#include "board.h"
#include "MCR20Drv.h"
#include "MCR20Reg.h"
#include "MCR20Overwrites.h"

#include "Phy.h"
#include "MpmInterface.h"

#include "fsl_os_abstraction.h"
#include "fsl_gpio_driver.h"


/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */
#define PHY_MIN_RNG_DELAY 4  /* [symbols] */


/*! *********************************************************************************
*************************************************************************************
* Private variables
*************************************************************************************
********************************************************************************** */
const  uint8_t gPhyIdlePwrState = gPhyDefaultIdlePwrMode_c;
const  uint8_t gPhyActivePwrState = gPhyDefaultActivePwrMode_c;

const uint8_t gPhyIndirectQueueSize_c = 12;
static uint8_t mPhyCurrentSamLvl = 12;
static uint8_t mPhyPwrState = gPhyPwrIdle_c;
#if gPhyUseReducedSpiAccess_d
uint8_t mPhySeqState;
#endif

#if gPhyNeighborTableSize_d
uint16_t mPhyNeighborTable[gPhyNeighborTableSize_d];
static uint32_t mPhyNeighbotTableUsage = 0;
#endif

#if gPhyRxPBTransferThereshold_d
extern uint8_t mPhyWatermarkLevel;
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
    uint8_t index;
    uint8_t phyReg;

    /* Initialize the transceiver SPI driver */
    MCR20Drv_Init();
    /* Configure the transceiver IRQ_B port */
    MCR20Drv_IRQ_PortConfig();
    /* Initialize the SPI driver and install PHY ISR */
    PHY_InstallIsr();

    /* Disable Tristate on COCO MISO for SPI reads */
    MCR20Drv_IndirectAccessSPIWrite((uint8_t) MISC_PAD_CTRL, (uint8_t) 0x02);

    /* PHY_CTRL4 unmask global TRX interrupts, enable 16 bit mode for TC2 - TC2 prime EN */
    MCR20Drv_DirectAccessSPIWrite(PHY_CTRL4, (uint8_t) (cPHY_CTRL4_TC2PRIME_EN | \
        (gCcaCCA_MODE1_c << cPHY_CTRL4_CCATYPE_Shift_c)));
    
    /* Clear all PP IRQ bits to avoid unexpected interrupts immediately after init, 
       disable all timer interrupts */
    MCR20Drv_DirectAccessSPIWrite(IRQSTS1,   (uint8_t) (cIRQSTS1_PLL_UNLOCK_IRQ | \
                                                        cIRQSTS1_FILTERFAIL_IRQ | \
                                                        cIRQSTS1_RXWTRMRKIRQ | \
                                                        cIRQSTS1_CCAIRQ | \
                                                        cIRQSTS1_RXIRQ | \
                                                        cIRQSTS1_TXIRQ | \
                                                        cIRQSTS1_SEQIRQ));
    
    MCR20Drv_DirectAccessSPIWrite(IRQSTS2,   (uint8_t) (cIRQSTS2_ASM_IRQ | \
                                                        cIRQSTS2_PB_ERR_IRQ | \
                                                        cIRQSTS2_WAKE_IRQ));
    
    MCR20Drv_DirectAccessSPIWrite(IRQSTS3,   (uint8_t) (cIRQSTS3_TMR4MSK | \
                                                        cIRQSTS3_TMR3MSK | \
                                                        cIRQSTS3_TMR2MSK | \
                                                        cIRQSTS3_TMR1MSK | \
                                                        cIRQSTS3_TMR4IRQ | \
                                                        cIRQSTS3_TMR3IRQ | \
                                                        cIRQSTS3_TMR2IRQ | \
                                                        cIRQSTS3_TMR1IRQ));
    
    /* PHY_CTRL1 default HW settings  + AUTOACK enabled */
    MCR20Drv_DirectAccessSPIWrite(PHY_CTRL1, (uint8_t) (cPHY_CTRL1_AUTOACK));
    
    /* PHY_CTRL2 : disable all interrupts */
    MCR20Drv_DirectAccessSPIWrite(PHY_CTRL2, (uint8_t) (cPHY_CTRL2_CRC_MSK | \
                                                        cPHY_CTRL2_PLL_UNLOCK_MSK | \
                                                        cPHY_CTRL2_FILTERFAIL_MSK | \
                                                        cPHY_CTRL2_RX_WMRK_MSK | \
                                                        cPHY_CTRL2_CCAMSK | \
                                                        cPHY_CTRL2_RXMSK | \
                                                        cPHY_CTRL2_TXMSK | \
                                                        cPHY_CTRL2_SEQMSK));
    
    /* PHY_CTRL3 : disable all timers and remaining interrupts */
    MCR20Drv_DirectAccessSPIWrite(PHY_CTRL3, (uint8_t) (cPHY_CTRL3_ASM_MSK    | \
                                                        cPHY_CTRL3_PB_ERR_MSK | \
                                                        cPHY_CTRL3_WAKE_MSK
#if gPhyUseReducedSpiAccess_d
                                                        /* Enable all TMR comparators */
                                                      | cPHY_CTRL3_TMR1CMP_EN  | \
                                                        cPHY_CTRL3_TMR2CMP_EN  | \
                                                        cPHY_CTRL3_TMR3CMP_EN  | \
                                                        cPHY_CTRL3_TMR4CMP_EN
#endif
                                                        ));
    /* SRC_CTRL */
    MCR20Drv_DirectAccessSPIWrite(SRC_CTRL,  (uint8_t) (cSRC_CTRL_ACK_FRM_PND | \
                                                        (cSRC_CTRL_INDEX << cSRC_CTRL_INDEX_Shift_c)));
    /*  RX_FRAME_FILTER
        FRM_VER[1:0] = b11. Accept FrameVersion 0 and 1 packets, reject all others */
    MCR20Drv_IndirectAccessSPIWrite(RX_FRAME_FILTER, (uint8_t)(cRX_FRAME_FLT_FRM_VER | \
                                                               cRX_FRAME_FLT_BEACON_FT | \
                                                               cRX_FRAME_FLT_DATA_FT | \
                                                               cRX_FRAME_FLT_CMD_FT ));
    /* Direct register overwrites */
    for (index = 0; index < sizeof(overwrites_direct)/sizeof(overwrites_t); index++)
        MCR20Drv_DirectAccessSPIWrite(overwrites_direct[index].address, overwrites_direct[index].data);

    /* Indirect register overwrites */
    for (index = 0; index < sizeof(overwrites_indirect)/sizeof(overwrites_t); index++)
        MCR20Drv_IndirectAccessSPIWrite(overwrites_indirect[index].address, overwrites_indirect[index].data);
    
    /* Clear HW indirect queue */
    for( index = 0; index < gPhyIndirectQueueSize_c; index++ )
        PhyPp_RemoveFromIndirect( index, 0 );
    
    PhyPlmeSetCurrentChannelRequest(0x0B, 0); /* 2405 MHz */
#if gMpmIncluded_d
    PhyPlmeSetCurrentChannelRequest(0x0B, 1); /* 2405 MHz */
    
    /* Split the HW Indirect hash table in two */
    PhyPpSetDualPanSamLvl( gPhyIndirectQueueSize_c/2 );
#else
    /* Assign HW Indirect hash table to PAN0 */
    PhyPpSetDualPanSamLvl( gPhyIndirectQueueSize_c );
#endif

    /* Set the power level to 0dBm */
    PhyPlmeSetPwrLevelRequest(0x17);
    /* Set CCA threshold to -75 dBm */
    PhyPpSetCcaThreshold(0x4B);
    /* Set prescaller to obtain 1 symbol (16us) timebase */
    MCR20Drv_IndirectAccessSPIWrite(TMR_PRESCALE, 0x05);
    /* Write default Rx watermark level */
    MCR20Drv_IndirectAccessSPIWrite(RX_WTR_MARK, 0);
#if gPhyRxPBTransferThereshold_d
    mPhyWatermarkLevel = 0;
#endif

    /* Enable the RxWatermark IRQ and FilterFail IRQ */
    phyReg = MCR20Drv_DirectAccessSPIRead(PHY_CTRL2);
    phyReg &= (uint8_t)~(cPHY_CTRL2_FILTERFAIL_MSK);
    phyReg &= (uint8_t)~(cPHY_CTRL2_RX_WMRK_MSK);
    MCR20Drv_DirectAccessSPIWrite(PHY_CTRL2, phyReg);

    /* enable autodoze mode. */
    phyReg = MCR20Drv_DirectAccessSPIRead( (uint8_t) PWR_MODES);
    phyReg |= (uint8_t) cPWR_MODES_AUTODOZE;
    MCR20Drv_DirectAccessSPIWrite( (uint8_t) PWR_MODES, phyReg);
    MCR20Drv_Set_CLK_OUT_Freq(gMCR20_ClkOutFreq_d);

#if gPhyUseReducedSpiAccess_d
    mPhySeqState = gIdle_c;
#endif

#if gPhyNeighborTableSize_d
    mPhyNeighbotTableUsage = 0;
#endif

    // Clear IRQn Pending Status
    MCR20Drv_IRQ_Clear();
    NVIC_ClearPendingIRQ(g_portIrqId[GPIO_EXTRACT_PORT(kGpioXcvrIrq)]);
    /* enable the transceiver IRQ_B interrupt request */
    MCR20Drv_IRQ_Enable();
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
#if gPhyUseReducedSpiAccess_d
    uint8_t phyRegs[5];
#else
    uint8_t phyRegs[8];
#endif
    volatile uint8_t currentTime = 0;

    /* Mask XCVR irq */
    ProtectFromMCR20Interrupt();

#if gPhyUseReducedSpiAccess_d
    mPhySeqState = gIdle_c;
#endif

    phyRegs[0] = MCR20Drv_DirectAccessSPIMultiByteRead(IRQSTS2, &phyRegs[1], sizeof(phyRegs) - 1);

    /* Disable timer trigger (for scheduled XCVSEQ) */
    if( phyRegs[PHY_CTRL1] & cPHY_CTRL1_TMRTRIGEN )
    {
        phyRegs[PHY_CTRL1] &= (uint8_t) ~(cPHY_CTRL1_TMRTRIGEN );
        MCR20Drv_DirectAccessSPIWrite(PHY_CTRL1, phyRegs[PHY_CTRL1]);
        
        /* Give the FSM enough time to start if it was triggered */
        currentTime = (uint8_t) ( MCR20Drv_DirectAccessSPIRead(EVENT_TMR_LSB) + 2 );
        while(MCR20Drv_DirectAccessSPIRead(EVENT_TMR_LSB) != (uint8_t) (currentTime));
        
        phyRegs[PHY_CTRL1] = MCR20Drv_DirectAccessSPIRead(PHY_CTRL1);
    }

    if( (phyRegs[PHY_CTRL1] & cPHY_CTRL1_XCVSEQ) != gIdle_c )
    {
        /* Abort current SEQ */
        phyRegs[PHY_CTRL1] &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
        MCR20Drv_DirectAccessSPIWrite(PHY_CTRL1, phyRegs[PHY_CTRL1]);
        
        /* Wait for Sequence Idle (if not already) */
        while ((MCR20Drv_DirectAccessSPIRead(SEQ_STATE) & 0x1F) != 0);
    }

    /* Mask SEQ interrupt */
    phyRegs[PHY_CTRL2] |= (uint8_t) (cPHY_CTRL2_SEQMSK);
#if !gPhyUseReducedSpiAccess_d
    /* Stop timers */
    phyRegs[PHY_CTRL3] &= (uint8_t) ~(cPHY_CTRL3_TMR2CMP_EN | cPHY_CTRL3_TMR3CMP_EN);
    phyRegs[PHY_CTRL4] &= (uint8_t) ~(cPHY_CTRL4_TC3TMOUT);

    MCR20Drv_DirectAccessSPIMultiByteWrite(PHY_CTRL2, &phyRegs[PHY_CTRL2], 4);
#endif
    /* Clear all PP IRQ bits to avoid unexpected interrupts and mask TMR3 interrupt.
       Do not change TMR IRQ status. */
    phyRegs[IRQSTS3] &= 0xF0;
    phyRegs[IRQSTS3] |= (uint8_t) (cIRQSTS3_TMR3MSK |
                                   cIRQSTS3_TMR2MSK |
                                   cIRQSTS3_TMR2IRQ |
                                   cIRQSTS3_TMR3IRQ);
#if gPhyUseReducedSpiAccess_d
    /* write all registers with a single SPI burst write */
    MCR20Drv_DirectAccessSPIMultiByteWrite(IRQSTS1, phyRegs, 5);
#else
    MCR20Drv_DirectAccessSPIMultiByteWrite(IRQSTS1, phyRegs, 3);
#endif

    PhyIsrPassRxParams(NULL);

#if gPhyRxPBTransferThereshold_d
    mPhyWatermarkLevel = 0;
    MCR20Drv_IndirectAccessSPIWrite(RX_WTR_MARK, mPhyWatermarkLevel);
#endif

    /* Unmask XCVR irq */
    UnprotectFromMCR20Interrupt();
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
#if gPhyUseReducedSpiAccess_d
    return mPhySeqState;
#else
    return (uint8_t)( MCR20Drv_DirectAccessSPIRead( (uint8_t) PHY_CTRL1) & cPHY_CTRL1_XCVSEQ );
#endif
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
#endif /* PHY_PARAMETERS_VALIDATION */

    if( 0 == pan )
        MCR20Drv_IndirectAccessSPIMultiByteWrite((uint8_t) MACPANID0_LSB, pPanId, 2);
    else
        MCR20Drv_IndirectAccessSPIMultiByteWrite((uint8_t) MACPANID1_LSB, pPanId, 2);

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
#endif /* PHY_PARAMETERS_VALIDATION */

    if( pan == 0 )
    {
        MCR20Drv_IndirectAccessSPIMultiByteWrite((uint8_t) MACSHORTADDRS0_LSB, pShortAddr, 2);
    }
    else
    {
        MCR20Drv_IndirectAccessSPIMultiByteWrite((uint8_t) MACSHORTADDRS1_LSB, pShortAddr, 2);
    }

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
#endif /* PHY_PARAMETERS_VALIDATION */

    if( 0 == pan )
        MCR20Drv_IndirectAccessSPIMultiByteWrite((uint8_t) MACLONGADDRS0_0, pLongAddr, 8);
    else
        MCR20Drv_IndirectAccessSPIMultiByteWrite((uint8_t) MACLONGADDRS1_0, pLongAddr, 8);

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
  uint8_t phyReg;

  if( 0 == pan )
  {
      phyReg = MCR20Drv_DirectAccessSPIRead( (uint8_t) PHY_CTRL4);

      if(gMacRole_PanCoord_c == macRole)
      {
          phyReg |=  cPHY_CTRL4_PANCORDNTR0;
      }
      else
      {
          phyReg &= ~cPHY_CTRL4_PANCORDNTR0;
      }
      MCR20Drv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL4, phyReg);
  }
  else
  {
      phyReg = MCR20Drv_IndirectAccessSPIRead( (uint8_t) DUAL_PAN_CTRL);

      if(gMacRole_PanCoord_c == macRole)
      {
          phyReg |=  cDUAL_PAN_CTRL_PANCORDNTR1;
      }
      else
      {
          phyReg &= ~cDUAL_PAN_CTRL_PANCORDNTR1;
      }
      MCR20Drv_IndirectAccessSPIWrite( (uint8_t) DUAL_PAN_CTRL, phyReg);
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
  uint8_t rxFrameFltReg, phyCtrl4Reg;

  rxFrameFltReg = MCR20Drv_IndirectAccessSPIRead( (uint8_t) RX_FRAME_FILTER);
  phyCtrl4Reg = MCR20Drv_DirectAccessSPIRead( (uint8_t) PHY_CTRL4);

  if( mode )
  {
    /* FRM_VER[1:0] = b00. 00: Any FrameVersion accepted (0,1,2 & 3) */
    /* All frame types accepted*/
    phyCtrl4Reg |= cPHY_CTRL4_PROMISCUOUS;
    rxFrameFltReg &= ~(cRX_FRAME_FLT_FRM_VER);
    rxFrameFltReg |=   (cRX_FRAME_FLT_ACK_FT | cRX_FRAME_FLT_NS_FT);
  }
  else
  {
    phyCtrl4Reg &= ~cPHY_CTRL4_PROMISCUOUS;
    /* FRM_VER[1:0] = b11. Accept FrameVersion 0 and 1 packets, reject all others */
    /* Beacon, Data and MAC command frame types accepted */
    rxFrameFltReg &= ~(cRX_FRAME_FLT_FRM_VER);
    rxFrameFltReg |= (0x03 << cRX_FRAME_FLT_FRM_VER_Shift_c);
    rxFrameFltReg &= ~(cRX_FRAME_FLT_ACK_FT | cRX_FRAME_FLT_NS_FT);
  }

  MCR20Drv_IndirectAccessSPIWrite( (uint8_t) RX_FRAME_FILTER, rxFrameFltReg);
  MCR20Drv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL4, phyCtrl4Reg);
}

/*! *********************************************************************************
* \brief  Set the PHY in ActivePromiscuous mode
*
* \param[in]  state
*
********************************************************************************** */
void PhySetActivePromiscuous(bool_t state)
{
    uint8_t phyCtrl4Reg;
    uint8_t phyFrameFilterReg;

    phyCtrl4Reg = MCR20Drv_DirectAccessSPIRead( (uint8_t) PHY_CTRL4);
    phyFrameFilterReg = MCR20Drv_IndirectAccessSPIRead(RX_FRAME_FILTER);

    /* if Prom is set */
    if( state )
    {
        if( phyCtrl4Reg & cPHY_CTRL4_PROMISCUOUS )
        {
            /* Disable Promiscuous mode */
            phyCtrl4Reg &= ~(cPHY_CTRL4_PROMISCUOUS);

            /* Enable Active Promiscuous mode */
            phyFrameFilterReg |= cRX_FRAME_FLT_ACTIVE_PROMISCUOUS;
        }
    }
    else
    {
        if( phyFrameFilterReg & cRX_FRAME_FLT_ACTIVE_PROMISCUOUS )
        {
            /* Disable Active Promiscuous mode */
            phyFrameFilterReg &= ~(cRX_FRAME_FLT_ACTIVE_PROMISCUOUS);

            /* Enable Promiscuous mode */
            phyCtrl4Reg |= cPHY_CTRL4_PROMISCUOUS;
        }
    }

    MCR20Drv_DirectAccessSPIWrite((uint8_t) PHY_CTRL4, phyCtrl4Reg);
    MCR20Drv_IndirectAccessSPIWrite(RX_FRAME_FILTER, phyFrameFilterReg);
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
    uint8_t phyReg = MCR20Drv_IndirectAccessSPIRead(RX_FRAME_FILTER);

    if( phyReg & cRX_FRAME_FLT_ACTIVE_PROMISCUOUS )
        return TRUE;

    return FALSE;
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
    uint8_t phyReg, newPhyReg;
    /* Disable/Enables the Source Address Matching feature */
    phyReg = MCR20Drv_DirectAccessSPIRead(SRC_CTRL);
    if( state )
        newPhyReg = phyReg | cSRC_CTRL_SRCADDR_EN;
    else
        newPhyReg = phyReg & ~(cSRC_CTRL_SRCADDR_EN);
    
    if( newPhyReg != phyReg )
        MCR20Drv_DirectAccessSPIWrite(SRC_CTRL, newPhyReg);
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
  uint16_t srcAddressCheckSum = checkSum;
  uint8_t  srcCtrlReg;

  if( index >= gPhyIndirectQueueSize_c )
      return gPhyInvalidParameter_c;

  srcCtrlReg = (uint8_t) ( (index & cSRC_CTRL_INDEX) << cSRC_CTRL_INDEX_Shift_c );
  MCR20Drv_DirectAccessSPIWrite( (uint8_t) SRC_CTRL, srcCtrlReg);

  MCR20Drv_DirectAccessSPIMultiByteWrite( (uint8_t) SRC_ADDRS_SUM_LSB, (uint8_t *) &srcAddressCheckSum, 2);

  srcCtrlReg |= ( cSRC_CTRL_SRCADDR_EN | cSRC_CTRL_INDEX_EN );
  MCR20Drv_DirectAccessSPIWrite( (uint8_t) SRC_CTRL, srcCtrlReg);

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
  uint8_t srcCtrlReg;

  if( index >= gPhyIndirectQueueSize_c )
      return gPhyInvalidParameter_c;

  srcCtrlReg = (uint8_t)( ( (index & cSRC_CTRL_INDEX) << cSRC_CTRL_INDEX_Shift_c )
                         |( cSRC_CTRL_SRCADDR_EN )
                         |( cSRC_CTRL_INDEX_DISABLE) );

  MCR20Drv_DirectAccessSPIWrite( (uint8_t) SRC_CTRL, srcCtrlReg);

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
  uint8_t irqsts2Reg;
  irqsts2Reg = MCR20Drv_DirectAccessSPIRead((uint8_t) IRQSTS2);
  if(irqsts2Reg & cIRQSTS2_PI)
  {
    return TRUE;
  }
  return FALSE;
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
  uint8_t irqsts1Reg;
  irqsts1Reg = MCR20Drv_DirectAccessSPIRead((uint8_t) IRQSTS1);
  if(irqsts1Reg & cIRQSTS1_RX_FRM_PEND)
  {
    return TRUE;
  }
  return FALSE;
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
    uint8_t srcCtrlReg;

    srcCtrlReg = MCR20Drv_DirectAccessSPIRead(SRC_CTRL);
    if( srcCtrlReg & cSRC_CTRL_SRCADDR_EN )
    {
        uint8_t irqsts2Reg;

        irqsts2Reg = MCR20Drv_DirectAccessSPIRead((uint8_t) IRQSTS2);

        if(irqsts2Reg & cIRQSTS2_SRCADDR)
            return TRUE;
        else
            return FALSE;
    }
    else
    {
        return ((srcCtrlReg & cSRC_CTRL_ACK_FRM_PND) == cSRC_CTRL_ACK_FRM_PND);
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
  MCR20Drv_IndirectAccessSPIWrite((uint8_t) CCA1_THRESH, (uint8_t) ccaThreshold);
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
  MCR20Drv_IndirectAccessSPIWrite(FAD_THR, FADThreshold);
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
  uint8_t phyReg;

  phyReg = MCR20Drv_IndirectAccessSPIRead(ANT_AGC_CTRL);
  state ? (phyReg |= cANT_AGC_CTRL_FAD_EN_Mask_c) : (phyReg &= (~((uint8_t)cANT_AGC_CTRL_FAD_EN_Mask_c)));
  MCR20Drv_IndirectAccessSPIWrite(ANT_AGC_CTRL, phyReg);

  phyReg = MCR20Drv_IndirectAccessSPIRead(ANT_PAD_CTRL);
  state ? (phyReg |= 0x02) : (phyReg &= ~cANT_PAD_CTRL_ANTX_EN);
  MCR20Drv_IndirectAccessSPIWrite(ANT_PAD_CTRL, phyReg);

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
  uint8_t currentMode;

  currentMode = MCR20Drv_IndirectAccessSPIRead(CCA_CTRL);
  lqiMode ? (currentMode |= cCCA_CTRL_LQI_RSSI_NOT_CORR) : (currentMode &= (~((uint8_t)cCCA_CTRL_LQI_RSSI_NOT_CORR)));
  MCR20Drv_IndirectAccessSPIWrite(CCA_CTRL, currentMode);

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
  return MCR20Drv_IndirectAccessSPIRead(RSSI);
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
  uint8_t phyReg;

  phyReg = MCR20Drv_IndirectAccessSPIRead(ANT_AGC_CTRL);
  state ? (phyReg |= cANT_AGC_CTRL_ANTX_Mask_c) : (phyReg &= (~((uint8_t)cANT_AGC_CTRL_ANTX_Mask_c)));
  MCR20Drv_IndirectAccessSPIWrite(ANT_AGC_CTRL, phyReg);

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
  uint8_t phyReg;

  phyReg = MCR20Drv_IndirectAccessSPIRead(ANT_AGC_CTRL);

  return ((phyReg & cANT_AGC_CTRL_ANTX_Mask_c) == cANT_AGC_CTRL_ANTX_Mask_c);
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
  uint8_t phyReg, phyReg2;

  phyReg = MCR20Drv_IndirectAccessSPIRead( (uint8_t) DUAL_PAN_CTRL);

  if( mode )
  {
    phyReg2 = phyReg | (cDUAL_PAN_CTRL_DUAL_PAN_AUTO);
  }
  else
  {
    phyReg2 = phyReg & (~cDUAL_PAN_CTRL_DUAL_PAN_AUTO);
  }

  /* Write the new value only if it has changed */
  if (phyReg2 != phyReg)
    MCR20Drv_IndirectAccessSPIWrite( (uint8_t) DUAL_PAN_CTRL, phyReg2);
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
  uint8_t phyReg = MCR20Drv_IndirectAccessSPIRead(DUAL_PAN_CTRL);
  return  (phyReg & cDUAL_PAN_CTRL_DUAL_PAN_AUTO) == cDUAL_PAN_CTRL_DUAL_PAN_AUTO;
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
  MCR20Drv_IndirectAccessSPIWrite( (uint8_t) DUAL_PAN_DWELL, dwell);
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
  return MCR20Drv_IndirectAccessSPIRead( (uint8_t) DUAL_PAN_DWELL);
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
  return (MCR20Drv_IndirectAccessSPIRead(DUAL_PAN_STS) & cDUAL_PAN_STS_DUAL_PAN_REMAIN);
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
  uint8_t phyReg, phyReg2;

  phyReg = MCR20Drv_IndirectAccessSPIRead( (uint8_t) DUAL_PAN_CTRL);

  if( 0 == nwk )
  {
      phyReg2 = phyReg & (~cDUAL_PAN_CTRL_ACTIVE_NETWORK);
  }
  else
  {
      phyReg2 = phyReg | cDUAL_PAN_CTRL_ACTIVE_NETWORK;
  }

  /* Write the new value only if it has changed */
  if( phyReg2 != phyReg )
  {
      MCR20Drv_IndirectAccessSPIWrite( (uint8_t) DUAL_PAN_CTRL, phyReg2);
  }
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
  uint8_t phyReg;

  phyReg = MCR20Drv_IndirectAccessSPIRead( (uint8_t)DUAL_PAN_CTRL );

  return (phyReg & cDUAL_PAN_CTRL_CURRENT_NETWORK) > 0;
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
  uint8_t phyReg;
  uint8_t PanBitMask = 0;

  phyReg = MCR20Drv_IndirectAccessSPIRead( (uint8_t) DUAL_PAN_STS);

  if( phyReg & cDUAL_PAN_STS_RECD_ON_PAN0 )
      PanBitMask |= (1<<0);

  if( phyReg & cDUAL_PAN_STS_RECD_ON_PAN1 )
      PanBitMask |= (1<<1);

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
  return mPhyCurrentSamLvl;
}

/*! *********************************************************************************
* \brief  Set the indirect queue level at which the HW queue will be split between PANs
*
* \param[in]  level
*
********************************************************************************** */
void PhyPpSetDualPanSamLvl( uint8_t level )
{
  uint8_t phyReg;
#ifdef PHY_PARAMETERS_VALIDATION
  if( level > gPhyIndirectQueueSize_c )
    return;
#endif
  phyReg = MCR20Drv_IndirectAccessSPIRead( (uint8_t) DUAL_PAN_CTRL);

  phyReg &= ~cDUAL_PAN_CTRL_DUAL_PAN_SAM_LVL_MSK; /* Clear current lvl */
  phyReg |= level << cDUAL_PAN_CTRL_DUAL_PAN_SAM_LVL_Shift_c; /* Set new lvl */

  MCR20Drv_IndirectAccessSPIWrite( (uint8_t) DUAL_PAN_CTRL, phyReg);
  mPhyCurrentSamLvl = level;
}

/*! *********************************************************************************
* \brief  Change the XCVR power state
*
* \param[in]  state  the new XCVR power state
*
* \return  phyStatus_t
*
* \pre Before entering hibernate/reset states, the MCG clock source must be changed
*      to use an input other than the one generated by the XCVR!
*
* \post When XCVR is in hibernate, indirect registers cannot be accessed in burst mode
*       When XCVR is in reset, all registers are inaccessible!
*
* \remarks Putting the XCVR into hibernate/reset will stop the generated clock signal!
*
********************************************************************************** */
phyStatus_t PhyPlmeSetPwrState( uint8_t state )
{
    uint8_t phyPWR, xtalState;

    /* Parameter validation */
    if( state > gPhyPwrReset_c )
        return gPhyInvalidParameter_c;

    /* Check if the new power state = old power state */
    if( state == mPhyPwrState )
        return gPhyBusy_c;

    /* Check if the XCVR is in reset power mode */
    if( mPhyPwrState == gPhyPwrReset_c )
    {
        MCR20Drv_RST_B_Deassert();
        /* Wait for transceiver to deassert IRQ pin */
        while( MCR20Drv_IsIrqPending() );
        /* Wait for transceiver wakeup from POR iterrupt */
        while( !MCR20Drv_IsIrqPending() );
        /* After reset, the radio is in Idle state */
        mPhyPwrState = gPhyPwrIdle_c;
        /* Restore default radio settings */
        PhyHwInit();
    }

    if( state != gPhyPwrReset_c )
    {
        phyPWR = MCR20Drv_DirectAccessSPIRead( PWR_MODES );
        xtalState = phyPWR & cPWR_MODES_XTALEN;
    }

    switch( state )
    {
    case gPhyPwrIdle_c:
        phyPWR &= ~(cPWR_MODES_AUTODOZE);
        phyPWR |= (cPWR_MODES_XTALEN | cPWR_MODES_PMC_MODE);
        break;

    case gPhyPwrAutodoze_c:
        phyPWR |= (cPWR_MODES_XTALEN | cPWR_MODES_AUTODOZE | cPWR_MODES_PMC_MODE);
        break;

    case gPhyPwrDoze_c:
        phyPWR &= ~(cPWR_MODES_AUTODOZE | cPWR_MODES_PMC_MODE);
        phyPWR |= cPWR_MODES_XTALEN;
        break;

    case gPhyPwrHibernate_c:
        phyPWR &= ~(cPWR_MODES_XTALEN | cPWR_MODES_AUTODOZE | cPWR_MODES_PMC_MODE);
        break;

    case gPhyPwrReset_c:
        MCR20Drv_IRQ_Disable();
        mPhyPwrState = gPhyPwrReset_c;
        MCR20Drv_RST_B_Assert();
        return gPhySuccess_c;
    }

    mPhyPwrState = state;
    MCR20Drv_DirectAccessSPIWrite( PWR_MODES, phyPWR );

    if( !xtalState && (phyPWR & cPWR_MODES_XTALEN))
    {
        /* wait for crystal oscillator to complet its warmup */
        while( ( MCR20Drv_DirectAccessSPIRead(PWR_MODES) & cPWR_MODES_XTAL_READY ) != cPWR_MODES_XTAL_READY);
        /* wait for radio wakeup from hibernate interrupt */
        while( ( MCR20Drv_DirectAccessSPIRead(IRQSTS2) & (cIRQSTS2_WAKE_IRQ | cIRQSTS2_TMRSTATUS) ) != (cIRQSTS2_WAKE_IRQ | cIRQSTS2_TMRSTATUS) );

        MCR20Drv_DirectAccessSPIWrite(IRQSTS2, cIRQSTS2_WAKE_IRQ);
    }

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  Get a random number from the XCVR
*
* \param[in]  pRandomNo  pointer to a location where the random number will be stored
*
* \pre This function should be called only when the Radio is idle.
*      The function may take a long time to run (more than 16 symbols)!
*      It is recomended to use this function only to initializa a seed at startup!
*
********************************************************************************** */
void PhyGetRandomNo(uint32_t *pRandomNo)
{
  uint8_t i = 4, prevRN=0;
  uint8_t* ptr = (uint8_t *)pRandomNo;
  phyTime_t startTime, endTime;
  uint8_t phyReg;

  MCR20Drv_IRQ_Disable();
  
  if( PhyPpGetState() )
  {
      *pRandomNo = 0;
      MCR20Drv_IRQ_Enable();
      return;   
  }

  while (i--)
  {
    PhyTimeReadClock(&startTime);

    /* Program a new sequence */
    phyReg = MCR20Drv_DirectAccessSPIRead(PHY_CTRL1);
    MCR20Drv_DirectAccessSPIWrite( PHY_CTRL1, phyReg | gRX_c);

    /* Wwait a variable number of symbols */
    do
      PhyTimeReadClock(&endTime);
    while( ((endTime - startTime) & 0x00FFFFFF) < (PHY_MIN_RNG_DELAY + (prevRN>>5)));

    /* Abort the sequence */
    PhyAbort();

    /* Read new 8 bit random number */
    prevRN = MCR20Drv_IndirectAccessSPIRead((uint8_t)_RNG);
    *ptr++ = prevRN;
  }

  MCR20Drv_IRQ_Enable();
}

/*! *********************************************************************************
* \brief  Set the state of the FP bit of an outgoing ACK frame
*
* \param[in]  FP  the state of the FramePending bit
*
********************************************************************************** */
void PhyPpSetFpManually
(
  bool_t FP
)
{
    uint8_t phyReg;
    /* Disable the Source Address Matching feature and set FP manually */
    phyReg = MCR20Drv_DirectAccessSPIRead(SRC_CTRL);
    phyReg &= ~(cSRC_CTRL_SRCADDR_EN);
    if(FP)
        phyReg |= cSRC_CTRL_ACK_FRM_PND;
    else
        phyReg &= ~(cSRC_CTRL_ACK_FRM_PND);
    MCR20Drv_DirectAccessSPIWrite(SRC_CTRL, phyReg);
}

uint8_t PhyPlmeSetANTPadStateRequest(bool_t antAB_on, bool_t rxtxSwitch_on)
{
    uint8_t phyReg;

    phyReg = MCR20Drv_IndirectAccessSPIRead(ANT_PAD_CTRL);
    antAB_on ? (phyReg |= 0x02) : (phyReg &= ~0x02);
    rxtxSwitch_on ? (phyReg |= 0x01) : (phyReg &= ~0x01);
    MCR20Drv_IndirectAccessSPIWrite(ANT_PAD_CTRL, phyReg);

    return gPhySuccess_c;
}

uint8_t PhyPlmeSetANTPadStrengthRequest(bool_t hiStrength)
{
    uint8_t phyReg;

    phyReg = MCR20Drv_IndirectAccessSPIRead(MISC_PAD_CTRL);
    hiStrength ? (phyReg |= cMISC_PAD_CTRL_ANTX_CURR) : (phyReg &= ~cMISC_PAD_CTRL_ANTX_CURR);
    MCR20Drv_IndirectAccessSPIWrite(MISC_PAD_CTRL, phyReg);

    return gPhySuccess_c;
}

uint8_t PhyPlmeSetANTPadInvertedRequest(bool_t invAntA, bool_t invAntB, bool_t invTx, bool_t invRx)
{
    uint8_t phyReg;

    phyReg = MCR20Drv_IndirectAccessSPIRead(MISC_PAD_CTRL);
    invAntA ? (phyReg |= 0x10) : (phyReg &= ~0x10);
    invAntB ? (phyReg |= 0x20) : (phyReg &= ~0x20);
    invTx   ? (phyReg |= 0x40) : (phyReg &= ~0x40);
    invRx   ? (phyReg |= 0x80) : (phyReg &= ~0x80);
    MCR20Drv_IndirectAccessSPIWrite(MISC_PAD_CTRL, phyReg);

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function compute the hash code for an 802.15.4 device
*
* \param[in]  pAddr     Pointer to an 802.15.4 address
* \param[in]  addrMode  The 802.15.4 addressing mode
* \param[in]  PanId     The 802.15.2 PAN Id
*
* \return  hash code
*
********************************************************************************** */
uint16_t PhyGetChecksum(uint8_t *pAddr, uint8_t addrMode, uint16_t PanId)
{
    uint16_t checksum;
    
    /* Short address */
    checksum  = PanId;
    checksum += *pAddr++;
    checksum += (*pAddr++) << 8;
    
    if( addrMode == 3 )    
    {
        /* Extended address */
        checksum += *pAddr++;
        checksum += (*pAddr++) << 8;
        checksum += *pAddr++;
        checksum += (*pAddr++) << 8;
        checksum += *pAddr++;
        checksum += (*pAddr++) << 8;
    }

    return checksum;
}

/*! *********************************************************************************
* \brief  This function adds an 802.15.4 device to the neighbor table.
*         If a polling device is not in the neighbor table, the ACK will have FP=1
*
* \param[in]  pAddr     Pointer to an 802.15.4 address
* \param[in]  addrMode  The 802.15.4 addressing mode
* \param[in]  PanId     The 802.15.2 PAN Id
*
********************************************************************************** */
uint8_t PhyAddToNeighborTable(uint8_t *pAddr, uint8_t addrMode, uint16_t PanId)
{
#if gPhyNeighborTableSize_d
    uint16_t checksum = PhyGetChecksum(pAddr, addrMode, PanId);

    if( PhyCheckNeighborTable(checksum) )
    {
        /* Device is allready in the table */
        return 0;
    }

    if( mPhyNeighbotTableUsage < gPhyNeighborTableSize_d )
    {
        mPhyNeighborTable[mPhyNeighbotTableUsage++] = checksum;
        return 0;
    }
#endif
    return 1;
}

/*! *********************************************************************************
* \brief  This function removes an 802.15.4 device to the neighbor table.
*         If a polling device is not in the neighbor table, the ACK will have FP=1
*
* \param[in]  pAddr     Pointer to an 802.15.4 address
* \param[in]  addrMode  The 802.15.4 addressing mode
* \param[in]  PanId     The 802.15.2 PAN Id
*
********************************************************************************** */
uint8_t PhyRemoveFromNeighborTable(uint8_t *pAddr, uint8_t addrMode, uint16_t PanId)
{
#if gPhyNeighborTableSize_d
    uint16_t i, checksum = PhyGetChecksum(pAddr, addrMode, PanId);

    for( i=0; i< mPhyNeighbotTableUsage; i++ )
    {
        if( checksum == mPhyNeighborTable[i] )
        {
            mPhyNeighborTable[i] = mPhyNeighborTable[--mPhyNeighbotTableUsage];
            mPhyNeighborTable[mPhyNeighbotTableUsage] = 0;
            return 0;
        }
    }
#endif
    return 1;
}

/*! *********************************************************************************
* \brief  This function checks if an 802.15.4 device is in the neighbor table.
*         If a polling device is not in the neighbor table, the ACK will have FP=1
*
* \param[in]  pAddr     Pointer to an 802.15.4 address
* \param[in]  addrMode  The 802.15.4 addressing mode
* \param[in]  PanId     The 802.15.2 PAN Id
*
* \return  TRUE if the device is present in the neighbor table, FALSE if not.
*
********************************************************************************** */
bool_t PhyCheckNeighborTable(uint16_t checksum)
{
#if gPhyNeighborTableSize_d
    uint16_t i;

    for( i=0; i< mPhyNeighbotTableUsage; i++ )
    {
        if( checksum == mPhyNeighborTable[i] )
        {
            return TRUE;
        }
    }
#endif
    return FALSE;
}
