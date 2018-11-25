/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file PhyISR.c
* PHY ISR Functions
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
#include "Phy.h"
#include "PhyInterface.h"
#include "FunctionLib.h"

#include "fsl_os_abstraction.h"
#include "fsl_device_registers.h"

/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */
#define gPhyIrqPriority_c     (0x80)

#if gUsePBTransferThereshold_d
  #define mPhyGetPBTransferThreshold(len) ((len) - 2)
#endif



/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */
extern Phy_PhyLocalStruct_t     phyLocal[];
static volatile phyRxParams_t * mpRxParams = NULL;
static uint32_t                 mPhyTaskInstance;
uint8_t                         mPhyLastRxLQI = 0;
uint8_t                         mPhyLastRxRSSI = 0;
uint8_t                         mPhyIrqDisableCnt = 1;

void (*gpfPhyPreprocessData)(uint8_t *pData, bool_t checkFP) = NULL;

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
void ProtectFromXcvrInterrupt(void)
{
    OSA_EnterCritical(kCriticalDisableInt);
   
    if( mPhyIrqDisableCnt == 0 )
    {
        ZLL_BWR_PHY_CTRL_TRCV_MSK(ZLL, 1);
    }
    
    mPhyIrqDisableCnt++;
    
    OSA_ExitCritical(kCriticalDisableInt);
}

void UnprotectFromXcvrInterrupt(void)
{
    OSA_EnterCritical(kCriticalDisableInt);
    
    if( mPhyIrqDisableCnt )
    {
        mPhyIrqDisableCnt--;
        
        if( mPhyIrqDisableCnt == 0 )
        {
            ZLL_BWR_PHY_CTRL_TRCV_MSK(ZLL, 0);
        }
    }

    OSA_ExitCritical(kCriticalDisableInt);
}

/*! *********************************************************************************
* \brief  Sets the current PHY instance waiting for an IRQ
*
* \param[in]  instanceId instance of the PHY
*
********************************************************************************** */
void PhyIsrPassTaskParams
(
  instanceId_t instanceId
)
{
    mPhyTaskInstance = instanceId;
}

/*! *********************************************************************************
* \brief  Sets the location of the Rx parameters
*
* \param[in]  pRxParam pointer to Rx parameters
*
********************************************************************************** */
void PhyIsrPassRxParams
(
  volatile phyRxParams_t * pRxParam
)
{
    mpRxParams = pRxParam;
}

/*! *********************************************************************************
* \brief  Clear and mask PHY IRQ, set sequence to Idle
*
********************************************************************************** */
void PhyIsrSeqCleanup
(
  void
)
{
    // mask SEQ, RX, TX and CCA interrupts
    ZLL_PHY_CTRL |= ZLL_PHY_CTRL_CCAMSK_MASK |
                    ZLL_PHY_CTRL_RXMSK_MASK  |
                    ZLL_PHY_CTRL_TXMSK_MASK  |
                    ZLL_PHY_CTRL_SEQMSK_MASK;
    // set the PHY sequencer back to IDLE
    ZLL_PHY_CTRL &= ~(ZLL_PHY_CTRL_XCVSEQ_MASK);

    while( ZLL_SEQ_STATE & ZLL_SEQ_STATE_SEQ_STATE_MASK );

    //mask TMR3 interrupt
    ZLL_IRQSTS |= ZLL_IRQSTS_TMR3MSK_MASK;
    // clear transceiver interrupts except TMRxIRQ
    ZLL_IRQSTS &= ~( ZLL_IRQSTS_TMR1IRQ_MASK |
                     ZLL_IRQSTS_TMR2IRQ_MASK |
                     ZLL_IRQSTS_TMR3IRQ_MASK |
                     ZLL_IRQSTS_TMR4IRQ_MASK );
}

/*! *********************************************************************************
* \brief  Clear and mask PHY IRQ, disable timeout, set sequence to Idle
*
********************************************************************************** */
void PhyIsrTimeoutCleanup
(
  void
)
{
    // mask SEQ, RX, TX and CCA interrupts
    ZLL_PHY_CTRL |= ZLL_PHY_CTRL_CCAMSK_MASK |
                    ZLL_PHY_CTRL_RXMSK_MASK  |
                    ZLL_PHY_CTRL_TXMSK_MASK  |
                    ZLL_PHY_CTRL_SEQMSK_MASK;
    // disable TMR3 comparator and timeout
    ZLL_PHY_CTRL &= ~(ZLL_PHY_CTRL_TMR3CMP_EN_MASK | ZLL_PHY_CTRL_TC3TMOUT_MASK);
    // set the PHY sequencer back to IDLE
    ZLL_PHY_CTRL &= ~(ZLL_PHY_CTRL_XCVSEQ_MASK);

    while( ZLL_SEQ_STATE & ZLL_SEQ_STATE_SEQ_STATE_MASK );

    //mask TMR3 interrupt
    ZLL_IRQSTS |= ZLL_IRQSTS_TMR3MSK_MASK;
    // clear transceiver interrupts except TMR1IRQ, TMR2IRQ and TMR4IRQ.
    ZLL_IRQSTS &= ~( ZLL_IRQSTS_TMR1IRQ_MASK |
                     ZLL_IRQSTS_TMR2IRQ_MASK |
                     ZLL_IRQSTS_TMR4IRQ_MASK );
}

/*! *********************************************************************************
* \brief  Scales energy level to 0-255
*
* \param[in]  energyLevel  the energ level reported by HW
*
* \return  uint8_t  the energy level scaled in 0x00-0xFF
*
********************************************************************************** */
uint8_t Phy_GetEnergyLevel
(
uint8_t energyLevel /* db */
)
{
    if(energyLevel >= 90)
    {
        /* ED value is below minimum. Return 0x00. */
        energyLevel = 0x00;
    }
    else if(energyLevel <= 26)
    {
        /* ED value is above maximum. Return 0xFF. */
        energyLevel = 0xFF;
    }
    else
    {
        /* Energy level (-90 dBm to -26 dBm ) --> varies form 0 to 64 */
        energyLevel = (90 - energyLevel);
        /* Rescale the energy level values to the 0x00-0xff range (0 to 64 translates in 0 to 255) */
        /* energyLevel * 3.9844 ~= 4 */
        /* Multiply with 4=2^2 by shifting left.
        The multiplication will not overflow beacause energyLevel has values between 0 and 63 */
        energyLevel <<= 2;
    }

    return energyLevel;
}

/*! *********************************************************************************
* \brief  Scales LQI to 0-255
*
* \param[in]  hwLqi  the LQI reported by HW
*
* \return  uint8_t  the LQI scaled in 0x00-0xFF
*
********************************************************************************** */
static uint8_t Phy_LqiConvert
(
uint8_t hwLqi
)
{
    uint32_t tmpLQI;
    
    /* LQI Saturation Level */
    if (hwLqi >= 230)
    {
        return 0xFF;
    }
    else
    {
        /* Rescale the LQI values from min to saturation to the 0x00 - 0xFF range */
        /* The LQI value mst be multiplied by ~1.1087 */
        /* tmpLQI =  hwLqi * 7123 ~= hwLqi * 65536 * 0.1087 = hwLqi * 2^16 * 0.1087*/
        tmpLQI = ((uint32_t)hwLqi * (uint32_t)7123 );
        /* tmpLQI =  (tmpLQI / 2^16) + hwLqi */
        tmpLQI = (uint32_t)(tmpLQI >> 16) + (uint32_t)hwLqi;
        
        return (uint8_t)tmpLQI;
    }
}

/*! *********************************************************************************
* \brief  This function returns the LQI for the las received packet
*
* \return  uint8_t  LQI value
*
********************************************************************************** */
uint8_t PhyGetLastRxLqiValue(void)
{
    return mPhyLastRxLQI;
}

/*! *********************************************************************************
* \brief  This function returns the RSSI for the las received packet
*
* \return  uint8_t  RSSI value
*
********************************************************************************** */
uint8_t PhyGetLastRxRssiValue(void)
{
  return mPhyLastRxRSSI;
}

/*! *********************************************************************************
* \brief  PHY ISR
*
********************************************************************************** */
void PHY_InterruptHandler(void)
{
    uint8_t xcvseqCopy;
    uint32_t irqSts;

    /* Mask XCVR interrupts */
    ProtectFromXcvrInterrupt();
    /* Read current XCVRSEQ and interrup status */
    xcvseqCopy = ZLL_PHY_CTRL & ZLL_PHY_CTRL_XCVSEQ_MASK;
    irqSts     = ZLL_IRQSTS;
    /* Clear all xcvr interrupts */
    ZLL_IRQSTS = irqSts;

    /* Flter Fail IRQ */
    if( (irqSts & ZLL_IRQSTS_FILTERFAIL_IRQ_MASK) &&
       !(ZLL_PHY_CTRL & ZLL_PHY_CTRL_FILTERFAIL_MSK_MASK) )
    {
        Radio_Phy_PlmeFilterFailRx(mPhyTaskInstance);
    }
    /* Rx Watermark IRQ */
    else if( (irqSts & ZLL_IRQSTS_RXWTRMRKIRQ_MASK) &&
            !(ZLL_PHY_CTRL & ZLL_PHY_CTRL_RX_WMRK_MSK_MASK) )
    {
        Radio_Phy_PlmeRxSfdDetect(mPhyTaskInstance, ZLL_RD_IRQSTS_RX_FRAME_LENGTH(ZLL));
    }

    /* Timer 1 Compare Match */
    if( (irqSts & ZLL_IRQSTS_TMR1IRQ_MASK) &&
       !(irqSts & ZLL_IRQSTS_TMR1MSK_MASK) )
    {
        PhyTimeDisableWaitTimeout();

        Radio_Phy_TimeWaitTimeoutIndication(mPhyTaskInstance);
    }

    /* Sequencer interrupt, the autosequence has completed */
    if( (irqSts & ZLL_IRQSTS_SEQIRQ_MASK) &&
       !(ZLL_PHY_CTRL & ZLL_PHY_CTRL_SEQMSK_MASK))
    {
        // PLL unlock, the autosequence has been aborted due to PLL unlock
        if( irqSts & ZLL_IRQSTS_PLL_UNLOCK_IRQ_MASK )
        {
            PhyIsrSeqCleanup();
            Radio_Phy_PlmeSyncLossIndication(mPhyTaskInstance);
            // unmask transceiver interrupt
            UnprotectFromXcvrInterrupt();
            return;
        }

        // TMR3 timeout, the autosequence has been aborted due to TMR3 timeout
        if( (irqSts & ZLL_IRQSTS_TMR3IRQ_MASK) &&
           !(irqSts & ZLL_IRQSTS_RXIRQ_MASK) &&
            (gTX_c != xcvseqCopy) )
        {
            PhyIsrTimeoutCleanup();
            Radio_Phy_TimeRxTimeoutIndication(mPhyTaskInstance);
            // unmask transceiver interrupt
            UnprotectFromXcvrInterrupt();
            return;
        }

        PhyIsrSeqCleanup();

        switch(xcvseqCopy)
        {
        case gTX_c:
            if( (irqSts & ZLL_IRQSTS_CCAIRQ_MASK ) &&
               (ZLL_PHY_CTRL & ZLL_PHY_CTRL_CCABFRTX_MASK) )
            {
                Radio_Phy_PlmeCcaConfirm(gPhyChannelBusy_c, mPhyTaskInstance);
            }
            else
            {
                Radio_Phy_PdDataConfirm(mPhyTaskInstance, FALSE);
            }
            break;

        case gTR_c:
            if( (irqSts & ZLL_IRQSTS_CCAIRQ_MASK ) &&
                (ZLL_PHY_CTRL & ZLL_PHY_CTRL_CCABFRTX_MASK) )
            {
                Radio_Phy_PlmeCcaConfirm(gPhyChannelBusy_c, mPhyTaskInstance);
            }
            else
            {
                if(NULL != mpRxParams)
                {
                    // reports value of 0x00 for -105 dBm of received input power and 0xFF for 0 dBm of received input power
                    mPhyLastRxRSSI = ZLL_RD_LQI_AND_RSSI_LQI_VALUE(ZLL);
                    mPhyLastRxLQI = Phy_LqiConvert(mPhyLastRxRSSI);
                    mpRxParams->linkQuality = mPhyLastRxLQI;
                    mpRxParams->timeStamp = ZLL_TIMESTAMP;
                    mpRxParams->psduLength = ZLL_RD_IRQSTS_RX_FRAME_LENGTH(ZLL); //Including FCS (2 bytes)
                    mpRxParams = NULL;
                }
                if( irqSts & ZLL_IRQSTS_RX_FRM_PEND_MASK )
                {
                    Radio_Phy_PdDataConfirm(mPhyTaskInstance, TRUE);
                }
                else
                {
                    Radio_Phy_PdDataConfirm(mPhyTaskInstance, FALSE);
                }
            }
            break;

        case gRX_c:
            //check SAA0 and SAA1 (source address absent)
            if( irqSts & ZLL_IRQSTS_PI_MASK )
            {
                /* Save the state of the FP bit sent in ACK frame */
                if( PhyPpIsTxAckDataPending() )
                {
                    phyLocal[mPhyTaskInstance].flags |= gPhyFlagTxAckFP_c;
                }
                else
                {
                    phyLocal[mPhyTaskInstance].flags &= ~gPhyFlagTxAckFP_c;
                }

                if( (gpfPhyPreprocessData != NULL) && 
                    (ZLL_SAM_MATCH & (ZLL_SAM_MATCH_SAA0_ADDR_ABSENT_MASK | ZLL_SAM_MATCH_SAA1_ADDR_ABSENT_MASK)) )
                {
                    gpfPhyPreprocessData((uint8_t*)&ZLL_PKT_BUFFER0, FALSE);
                }
            }
            
            if( NULL != mpRxParams )
            {
                // reports value of 0x00 for -105 dBm of received input power and 0xFF for 0 dBm of received input power
                mPhyLastRxRSSI = ZLL_RD_LQI_AND_RSSI_LQI_VALUE(ZLL);
                mPhyLastRxLQI = Phy_LqiConvert(mPhyLastRxRSSI);
                mpRxParams->linkQuality = mPhyLastRxLQI;
                mpRxParams->timeStamp = ZLL_TIMESTAMP;
                mpRxParams->psduLength = ZLL_RD_IRQSTS_RX_FRAME_LENGTH(ZLL); //Including FCS (2 bytes)
                mpRxParams = NULL;
            }
            Radio_Phy_PdDataIndication(mPhyTaskInstance);
            break;

        case gCCA_c:
            if( ZLL_RD_PHY_CTRL_CCATYPE(ZLL) == gCcaED_c )
            {
                // Ed
                Radio_Phy_PlmeEdConfirm( ZLL_RD_LQI_AND_RSSI_CCA1_ED_FNL(ZLL), mPhyTaskInstance );
            }
            else
            {
                // CCA
                if( irqSts & ZLL_IRQSTS_CCAIRQ_MASK )
                {
                    Radio_Phy_PlmeCcaConfirm(gPhyChannelBusy_c, mPhyTaskInstance);
                }
                else
                {
                    Radio_Phy_PlmeCcaConfirm(gPhyChannelIdle_c, mPhyTaskInstance);
                }
            }
            break;

        case gCCCA_c:
            Radio_Phy_PlmeCcaConfirm(gPhyChannelIdle_c, mPhyTaskInstance);
            break;

        default:
            Radio_Phy_PlmeSyncLossIndication(mPhyTaskInstance);
            break;
        }
    }
    // timers interrupt
    else
    {
        /* Timer 2 Compare Match */
        if( (irqSts & ZLL_IRQSTS_TMR2IRQ_MASK) &&
           !(irqSts & ZLL_IRQSTS_TMR2MSK_MASK) )
        {
            PhyTimeDisableEventTrigger();

            if( gIdle_c != xcvseqCopy )
            {
                Radio_Phy_TimeStartEventIndication(mPhyTaskInstance);
            }
        }

        /* Timer 3 Compare Match */
        if( (irqSts & ZLL_IRQSTS_TMR3IRQ_MASK) &&
           !(irqSts & ZLL_IRQSTS_TMR3MSK_MASK) )
        {
            PhyTimeDisableEventTimeout();

            /* Ensure that we're not issuing TimeoutIndication while the Automated sequence is still in progress */
            /* TMR3 can expire during R-T turnaround for example, case in which the sequence is not interrupted */
            if( gIdle_c == xcvseqCopy )
            {
                Radio_Phy_TimeRxTimeoutIndication(mPhyTaskInstance);
            }
        }

        /* Timer 4 Compare Match */
        if( (irqSts & ZLL_IRQSTS_TMR4IRQ_MASK) &&
           !(irqSts & ZLL_IRQSTS_TMR4MSK_MASK) )
        {
            // disable TMR4 comparator
            ZLL_BWR_PHY_CTRL_TMR4CMP_EN(ZLL, 0);
            // mask TMR4 interrupt (do not change other IRQ status)
            ZLL_BWR_IRQSTS_TMR4MSK(ZLL, 1);//gcapraru
        }
    }

    // unmask transceiver interrupt
    UnprotectFromXcvrInterrupt();
}

/*! *********************************************************************************
* \brief  This function installs the PHY ISR
*
********************************************************************************** */
void PHY_InstallIsr( void )
{
    OSA_InstallIntHandler(ZigBee_IRQn, PHY_InterruptHandler);

    /* enable transceiver SPI interrupt request */
    NVIC_ClearPendingIRQ(ZigBee_IRQn);
    NVIC_EnableIRQ(ZigBee_IRQn);
    
    /* set transceiver interrupt priority */
    NVIC_SetPriority(ZigBee_IRQn, gPhyIrqPriority_c >> (8 - __NVIC_PRIO_BITS));
    UnprotectFromXcvrInterrupt();
}