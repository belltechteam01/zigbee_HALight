/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file PhyPlmeData.c
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
#include "fsl_os_abstraction.h"
#include "MCR20Drv.h"
#include "MCR20Reg.h"
#include "Phy.h"
#include "PhyTypes.h"
#include "PhyInterface.h"


/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */
#define PHY_PARAMETERS_VALIDATION 1


/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */
                                    /* 2405   2410    2415    2420    2425    2430    2435    2440    2445    2450    2455    2460    2465    2470    2475    2480 */
static const uint8_t  pll_int[16] =  {0x0B,   0x0B,   0x0B,   0x0B,   0x0B,   0x0B,   0x0C,   0x0C,   0x0C,   0x0C,   0x0C,   0x0C,   0x0D,   0x0D,   0x0D,   0x0D};
static const uint16_t pll_frac[16] = {0x2800, 0x5000, 0x7800, 0xA000, 0xC800, 0xF000, 0x1800, 0x4000, 0x6800, 0x9000, 0xB800, 0xE000, 0x0800, 0x3000, 0x5800, 0x8000};

extern Phy_PhyLocalStruct_t     phyLocal[];
extern volatile phyTime_t mPhySeqTimeout;
static uint8_t gPhyCurrentChannelPAN0 = 0x0B;
static uint8_t gPhyCurrentChannelPAN1 = 0x0B;
#if gPhyUseReducedSpiAccess_d
extern uint8_t mPhySeqState;
#endif
#if gPhyRxRetryInterval_c
uint8_t gRxRetryTimer = gInvalidTimerId_c;
#endif


/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
********************************************************************************** */
#if gPhyRxRetryInterval_c
static void PhyRxRetry( uint32_t param );
#endif


/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
* \brief  This function will start a TX sequence. The packet will be sent OTA
*
* \param[in]  pTxPacket   pointer to the TX packet structure
* \param[in]  pRxParams   pointer to RX parameters
* \param[in]  pTxParams   pointer to TX parameters
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPdDataRequest( pdDataReq_t *pTxPacket,
                              volatile phyRxParams_t *pRxParams,
                              volatile phyTxParams_t *pTxParams )
{
    uint8_t phyRegs[5], phyCtrl4Reg;
    uint8_t newSeq;

#ifdef PHY_PARAMETERS_VALIDATION
    if(NULL == pTxPacket)
    {
        return gPhyInvalidParameter_c;
    }

    /* if CCA required ... */
    if( (pTxPacket->CCABeforeTx == gPhyCCAMode3_c) || (pTxPacket->CCABeforeTx == gPhyEnergyDetectMode_c))
    { /* ... cannot perform other types than MODE1 and MODE2 */
        return gPhyInvalidParameter_c;
    }

#endif /* PHY_PARAMETERS_VALIDATION */

    if( gIdle_c != PhyPpGetState() )
    {
        return gPhyBusy_c;
    }
#if !gPhyDeferTxPBwrite_c
    {
        uint8_t *pTmpPsdu, *tmp;
        /* Load data into PB */
        tmp = pTxPacket->pPsdu;
        pTmpPsdu = (uint8_t *) ((&pTxPacket->pPsdu[0])-1);
        *pTmpPsdu = pTxPacket->psduLength + 2; /* including 2 bytes of FCS */
        MCR20Drv_PB_SPIBurstWrite( pTmpPsdu, (uint8_t) (pTxPacket->psduLength + 1)); /* including psduLength */
        pTxPacket->pPsdu = tmp;
    }
#endif

    /* Read XCVR registers */
    phyRegs[0] = MCR20Drv_DirectAccessSPIMultiByteRead(IRQSTS2, &phyRegs[1], 4);

    /* Slotted operation? */
    if( pTxPacket->slottedTx == gPhySlottedMode_c )
    {
        phyRegs[PHY_CTRL1] |= (uint8_t) (cPHY_CTRL1_SLOTTED);
    }
    else
    {
        phyRegs[PHY_CTRL1] &= (uint8_t) ~(cPHY_CTRL1_SLOTTED);
    }

    /* Perform TxRxAck sequence if required by phyTxMode */
    pTxParams->ackRequired = pTxPacket->ackRequired;

    if(pTxPacket->ackRequired == gPhyRxAckRqd_c)
    {
        PhyIsrPassRxParams(pRxParams);
        phyRegs[PHY_CTRL1] |= (uint8_t) (cPHY_CTRL1_RXACKRQD);
        newSeq =  gTR_c;
    }
    else
    {
        PhyIsrPassRxParams(NULL);
        phyRegs[PHY_CTRL1] &= (uint8_t) ~(cPHY_CTRL1_RXACKRQD);
        newSeq = gTX_c;
    }

    /* Perform CCA before TX if required */
    phyRegs[PHY_CTRL1] &= (uint8_t) ~(cPHY_CTRL1_CCABFRTX);
#if gUseStandaloneCCABeforeTx_d
    if( pTxPacket->CCABeforeTx != gPhyNoCCABeforeTx_c )
    {
        /* Start the CCA or ED sequence (this depends on CcaType used)
           immediately or by TC2', depending on a previous PhyTimeSetEventTrigger() call) */
        if( pTxPacket->slottedTx == gPhySlottedMode_c )
            pTxParams->numOfCca = 2;
        else
            pTxParams->numOfCca = 1;

        newSeq = gCCA_c;
        /* At the end of the scheduled sequence, an interrupt will occur: CCA , SEQ or TMR3 */
    }
    else
    {
        pTxParams->numOfCca = 0;
    }
#endif

    /* Set CCA type */
#if gUseStandaloneCCABeforeTx_d    
    if( newSeq == gCCA_c )
#endif
    {
        phyCtrl4Reg  = MCR20Drv_DirectAccessSPIRead(PHY_CTRL4);
        phyCtrl4Reg &= (uint8_t) ~(cPHY_CTRL4_CCATYPE << cPHY_CTRL4_CCATYPE_Shift_c);
        
        if( pTxPacket->CCABeforeTx != gPhyNoCCABeforeTx_c )
        {
#if (gUseStandaloneCCABeforeTx_d == 0)
            phyRegs[PHY_CTRL1] |= (cPHY_CTRL1_CCABFRTX);
#endif
            phyCtrl4Reg |= ((cPHY_CTRL4_CCATYPE & pTxPacket->CCABeforeTx) << (cPHY_CTRL4_CCATYPE_Shift_c));
        }
        /* Write CCA type into XCVR */
        MCR20Drv_DirectAccessSPIWrite(PHY_CTRL4, phyCtrl4Reg);
    }

    /* Start the TX or TRX sequence */
    phyRegs[PHY_CTRL1] &= ~(cPHY_CTRL1_XCVSEQ);
    phyRegs[PHY_CTRL1] |= newSeq;
#if gPhyUseReducedSpiAccess_d
    mPhySeqState = newSeq;
#endif
    
    /* Ensure that no spurious interrupts are raised */
    phyRegs[IRQSTS3] &= 0xF0; /* do not change other IRQ status */
    phyRegs[IRQSTS3] |= (cIRQSTS3_TMR3MSK | cIRQSTS3_TMR2IRQ | cIRQSTS3_TMR3IRQ);
    MCR20Drv_DirectAccessSPIMultiByteWrite(IRQSTS1, phyRegs, 3);
    /* Unmask SEQ interrupt */
    phyRegs[PHY_CTRL2] &= ~(cPHY_CTRL2_SEQMSK);
    MCR20Drv_DirectAccessSPIWrite(PHY_CTRL2, phyRegs[PHY_CTRL2]);

    /* Write XCVR settings */
    MCR20Drv_DirectAccessSPIWrite(PHY_CTRL1, phyRegs[PHY_CTRL1]);

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will start a RX sequence
*
* \param[in]  phyRxMode   slotted/unslotted
* \param[in]  pRxParams   pointer to RX parameters
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPlmeRxRequest( phySlottedMode_t phyRxMode, phyRxParams_t *  pRxParams )
{
    uint8_t phyRegs[5];

#ifdef PHY_PARAMETERS_VALIDATION
    if(NULL == pRxParams)
    {
        return gPhyInvalidParameter_c;
    }
#endif /* PHY_PARAMETERS_VALIDATION */

    if( gIdle_c != PhyGetSeqState() )
    {
        return gPhyBusy_c;
    }

    pRxParams->phyRxMode = phyRxMode;

    if( NULL == pRxParams->pRxData )
    {
        pRxParams->pRxData = MEM_BufferAlloc(sizeof(pdDataToMacMessage_t) + gMaxPHYPacketSize_c);
    }

    if( NULL == pRxParams->pRxData )
    {
#if gPhyRxRetryInterval_c
        if( gRxRetryTimer == gInvalidTimerId_c )
        {
            phyTimeEvent_t event = {
                .timestamp = PhyTime_GetTimestamp() + gPhyRxRetryInterval_c,
                .parameter = 0,
                .callback  = PhyRxRetry,
            };
            
            gRxRetryTimer = PhyTime_ScheduleEvent( &event );
        }
#endif
        return gPhyTRxOff_c;   
    }

    /* Set pPsdu to NULL to signal that the PacketBuffer was not read! */
    pRxParams->pRxData->msgData.dataInd.pPsdu = NULL;
    PhyIsrPassRxParams(pRxParams);

    phyRegs[0] = MCR20Drv_DirectAccessSPIMultiByteRead(IRQSTS2, &phyRegs[1], 4);

    /* Slotted operation */
    if(gPhySlottedMode_c == phyRxMode)
    {
        phyRegs[PHY_CTRL1] |= (uint8_t) (cPHY_CTRL1_SLOTTED);
    }
    else
    {
        phyRegs[PHY_CTRL1] &= (uint8_t) ~(cPHY_CTRL1_SLOTTED);
    }

    /* Program the RX sequence */
    phyRegs[PHY_CTRL1] &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
    phyRegs[PHY_CTRL1] |=  gRX_c;
#if gPhyUseReducedSpiAccess_d
    mPhySeqState = gRX_c;
#endif

    phyRegs[PHY_CTRL2] &= (uint8_t) ~(cPHY_CTRL2_SEQMSK); /* unmask SEQ interrupt */

    /* Ensure that no spurious interrupts are raised */
    phyRegs[IRQSTS3] &= 0xF0;                     /* do not change IRQ status */
    phyRegs[IRQSTS3] |= (uint8_t) (cIRQSTS3_TMR3MSK |
                                   cIRQSTS3_TMR2IRQ |
                                   cIRQSTS3_TMR3IRQ);   /* mask TMR3 interrupt */

    MCR20Drv_DirectAccessSPIMultiByteWrite(IRQSTS1, phyRegs, 3);    
    MCR20Drv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL2, phyRegs[PHY_CTRL2]);

    /* Start the RX sequence */
    MCR20Drv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL1, phyRegs[PHY_CTRL1]);

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will start a timmed RX sequence
*         The Rx may have an absolute start time and/or a fixed duration!
*
* \param[in]  phyRxMode   slotted/unslotted
* \param[in]  pRxParams   pointer to RX parameters
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPlmeTimmedRxRequest( phyRxParams_t *  pRxParams )
{
#if gPhyUseReducedSpiAccess_d
    uint8_t phyRegs[5];
#else
    uint8_t phyRegs[6];
#endif
    phyTime_t time;
#if !gPhyUseReducedSpiAccess_d
    uint8_t phyCtrl4;
#endif

#ifdef PHY_PARAMETERS_VALIDATION
    if(NULL == pRxParams)
    {
        return gPhyInvalidParameter_c;
    }
#endif /* PHY_PARAMETERS_VALIDATION */

    if( gIdle_c != PhyPpGetState() )
    {
        return gPhyBusy_c;
    }

    if( NULL == pRxParams->pRxData )
    {
        pRxParams->pRxData = MEM_BufferAlloc(sizeof(pdDataToMacMessage_t) + gMaxPHYPacketSize_c);
    }

    if( NULL == pRxParams->pRxData )
    {
#if gPhyRxRetryInterval_c
        if( gRxRetryTimer == gInvalidTimerId_c )
        {
            phyTimeEvent_t event = {
                .timestamp = PhyTime_GetTimestamp() + gPhyRxRetryInterval_c,
                .parameter = 0,
                .callback  = PhyRxRetry,
            };
            
            gRxRetryTimer = PhyTime_ScheduleEvent( &event );
        }
#endif
        return gPhyTRxOff_c;   
    }

    /* Set pPsdu to NULL to signal that the PacketBuffer was not read! */
    pRxParams->pRxData->msgData.dataInd.pPsdu = NULL;
    PhyIsrPassRxParams(pRxParams);

    phyRegs[IRQSTS1] = MCR20Drv_DirectAccessSPIMultiByteRead(IRQSTS2, &phyRegs[IRQSTS2], sizeof(phyRegs) - 1);

    /* Slotted operation */
    if( gPhySlottedMode_c == pRxParams->phyRxMode )
    {
        phyRegs[PHY_CTRL1] |= (uint8_t) (cPHY_CTRL1_SLOTTED);
    }
    else
    {
        phyRegs[PHY_CTRL1] &= (uint8_t) ~(cPHY_CTRL1_SLOTTED);
    }

    /* Program the RX sequence */
    phyRegs[PHY_CTRL1] &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);
    phyRegs[PHY_CTRL1] |=  gRX_c;

    phyRegs[PHY_CTRL2] &= (uint8_t) ~(cPHY_CTRL2_SEQMSK); /* unmask SEQ interrupt */

    /* Wnsure that no spurious interrupts are raised */
    phyRegs[IRQSTS3] &= 0xF0;                     /* do not change IRQ status */
    phyRegs[IRQSTS3] |= (uint8_t) (cIRQSTS3_TMR2IRQ | 
                                   cIRQSTS3_TMR3IRQ |
                                   cIRQSTS3_TMR2MSK |
                                   cIRQSTS3_TMR3MSK);

#if gPhyUseReducedSpiAccess_d
    mPhySeqState = gRX_c;

    /* Mask TMR2 and TMR3 IRQ */
    MCR20Drv_DirectAccessSPIWrite(IRQSTS3, phyRegs[IRQSTS3]);
#endif
 
    /* Program sequence start time */
    if( pRxParams->timeStamp != gPhySeqStartAsap_c )
    {
        phyRegs[PHY_CTRL1] |= cPHY_CTRL1_TMRTRIGEN;
#if !gPhyUseReducedSpiAccess_d
        phyRegs[PHY_CTRL3] |= cPHY_CTRL3_TMR2CMP_EN;
#endif
        //phyRegs[IRQSTS3] &= ~(cIRQSTS3_TMR2MSK); /* unmask TMR2IRQ to generate a StartIndication */
        MCR20Drv_DirectAccessSPIMultiByteWrite( T2PRIMECMP_LSB, (uint8_t *) &pRxParams->timeStamp, 2);
    }
    else
    {
        phyRegs[PHY_CTRL1] &= ~cPHY_CTRL1_TMRTRIGEN;
#if !gPhyUseReducedSpiAccess_d
        phyRegs[PHY_CTRL3] &= ~cPHY_CTRL3_TMR2CMP_EN;
#endif
        PhyTimeReadClock( &pRxParams->timeStamp );
    }

    /* Program sequence end time */
    if( pRxParams->duration != 0xFFFFFFFF )
    {
#if gPhyUseReducedSpiAccess_d
        phyRegs[IRQSTS3] &= ~(cIRQSTS3_TMR3MSK);
#else
        /* enable autosequence stop by TC3 match */
        phyCtrl4 = MCR20Drv_DirectAccessSPIRead(PHY_CTRL4);
        phyCtrl4 |= cPHY_CTRL4_TC3TMOUT;
        MCR20Drv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL4, phyCtrl4);

        /* enable TMR3 comparator */
        phyRegs[PHY_CTRL3] |= cPHY_CTRL3_TMR3CMP_EN;
#endif

        /* write timeout value */
        time = pRxParams->timeStamp + pRxParams->duration;
        mPhySeqTimeout = time;
        MCR20Drv_DirectAccessSPIMultiByteWrite( T3CMP_LSB, (uint8_t *) &time, 3);

    }
#if !gPhyUseReducedSpiAccess_d
    else
    {
        phyRegs[PHY_CTRL3] &= ~cPHY_CTRL3_TMR3CMP_EN;
    }
#endif

    MCR20Drv_DirectAccessSPIMultiByteWrite(IRQSTS1, &phyRegs[IRQSTS1], 3);
#if gPhyUseReducedSpiAccess_d
    MCR20Drv_DirectAccessSPIWrite(PHY_CTRL2, phyRegs[PHY_CTRL2]);
#else
    MCR20Drv_DirectAccessSPIMultiByteWrite(PHY_CTRL2, &phyRegs[PHY_CTRL2], 2);
#endif

    /* Check if the Sequence Start Time has expired */
    OSA_EnterCritical(kCriticalDisableInt);
    if( phyRegs[PHY_CTRL1] & cPHY_CTRL1_TMRTRIGEN )
    {
        time = PhyTime_GetTimestamp();
        if( time+4 > pRxParams->timeStamp )
        {
            phyRegs[PHY_CTRL1] &= ~cPHY_CTRL1_TMRTRIGEN;
        }
    }
    /* start the RX sequence */
    MCR20Drv_DirectAccessSPIWrite( PHY_CTRL1, phyRegs[PHY_CTRL1]);
    OSA_ExitCritical(kCriticalDisableInt);

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will start a CCA / CCCA sequence
*
* \param[in]  ccaParam   the type of CCA
* \param[in]  cccaMode   continuous or single CCA
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPlmeCcaEdRequest( phyCCAType_t ccaParam, phyContCCAMode_t cccaMode )
{
    uint8_t phyRegs[5];

#ifdef PHY_PARAMETERS_VALIDATION
    /* Check for illegal CCA type */
    if( (ccaParam != gPhyCCAMode1_c) && (ccaParam != gPhyCCAMode2_c) && (ccaParam != gPhyCCAMode3_c) && (ccaParam != gPhyEnergyDetectMode_c))
    {
        return gPhyInvalidParameter_c;
    }

    /* Cannot perform Continuous CCA using ED type */
    if( (ccaParam == gPhyEnergyDetectMode_c) && (cccaMode == gPhyContCcaEnabled) )
    {
        return gPhyInvalidParameter_c;
    }
#endif /* PHY_PARAMETERS_VALIDATION */

    if( gIdle_c != PhyGetSeqState() )
    {
        return gPhyBusy_c;
    }

    /* Write in PHY CTRL4 the desired type of CCA */
    phyRegs[0] = MCR20Drv_DirectAccessSPIRead(PHY_CTRL4);
    phyRegs[0] &= (uint8_t) ~(cPHY_CTRL4_CCATYPE << cPHY_CTRL4_CCATYPE_Shift_c);
    phyRegs[0] |= (uint8_t) ((cPHY_CTRL4_CCATYPE & ccaParam) << (cPHY_CTRL4_CCATYPE_Shift_c));
    MCR20Drv_DirectAccessSPIWrite( (uint8_t)PHY_CTRL4, phyRegs[0]);
    
    phyRegs[0] = MCR20Drv_DirectAccessSPIMultiByteRead(IRQSTS2, &phyRegs[1], 4);
    phyRegs[PHY_CTRL1] &= (uint8_t) ~(cPHY_CTRL1_XCVSEQ);

    /* Continuous CCA */
    if(cccaMode == gPhyContCcaEnabled)
    {
        /* Start the continuous CCA sequence
           immediately or by TC2', depending on a previous PhyTimeSetEventTrigger() call) */
#if gPhyUseReducedSpiAccess_d
        mPhySeqState = gCCCA_c;
#endif
        phyRegs[PHY_CTRL1] |= gCCCA_c;
        /* At the end of the scheduled sequence, an interrupt will occur: CCA , SEQ or TMR3 */
    }
    /* Normal CCA (not continuous) */
    else
    {
        /* Start the CCA or ED sequence (this depends on CcaType used)
           immediately or by TC2', depending on a previous PhyTimeSetEventTrigger() call) */
#if gPhyUseReducedSpiAccess_d
        mPhySeqState = gCCA_c;
#endif
        phyRegs[PHY_CTRL1] |= gCCA_c;
        /* At the end of the scheduled sequence, an interrupt will occur: CCA , SEQ or TMR3 */
    }

    /* Unmask SEQ interrupt */
    phyRegs[PHY_CTRL2] &= (uint8_t) ~(cPHY_CTRL2_SEQMSK);
    
    /* Ensure that no spurious interrupts are raised */
    phyRegs[IRQSTS3] &= 0xF0;                     /* do not change IRQ status */
    phyRegs[IRQSTS3] |= (uint8_t) (cIRQSTS3_TMR3MSK |
                                   cIRQSTS3_TMR2IRQ |
                                   cIRQSTS3_TMR3IRQ);   /* mask TMR3 interrupt */
    
    MCR20Drv_DirectAccessSPIMultiByteWrite(IRQSTS1, phyRegs, 3);
    
    /* Start the CCA/ED or CCCA sequence */
    MCR20Drv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL2, phyRegs[PHY_CTRL2]);
    MCR20Drv_DirectAccessSPIWrite( (uint8_t) PHY_CTRL1, phyRegs[PHY_CTRL1]);

    return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will set the channel number for the specified PAN
*
* \param[in]   channel   new channel number
* \param[in]   pan       the PAN registers (0/1)
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPlmeSetCurrentChannelRequest
(
  uint8_t channel,
  uint8_t pan
)
{

#ifdef PHY_PARAMETERS_VALIDATION
  if((channel < 11) || (channel > 26))
  {
    return gPhyInvalidParameter_c;
  }
#endif /* PHY_PARAMETERS_VALIDATION */

  if( !pan )
  {
      gPhyCurrentChannelPAN0 = channel;
      MCR20Drv_DirectAccessSPIWrite(PLL_INT0, pll_int[channel - 11]);
      MCR20Drv_DirectAccessSPIMultiByteWrite(PLL_FRAC0_LSB, (uint8_t *) &pll_frac[channel - 11], 2);
  }
  else
  {
      gPhyCurrentChannelPAN1 = channel;
      MCR20Drv_IndirectAccessSPIWrite(PLL_INT1, pll_int[channel - 11]);
      MCR20Drv_IndirectAccessSPIMultiByteWrite(PLL_FRAC1_LSB, (uint8_t *) &pll_frac[channel - 11], 2);
  }
  return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will return the current channel for a specified PAN
*
* \param[in]   pan   the PAN registers (0/1)
*
* \return  uint8_t  current channel number
*
********************************************************************************** */
uint8_t PhyPlmeGetCurrentChannelRequest
(
  uint8_t pan
)
{
    if( !pan )
        return gPhyCurrentChannelPAN0;
    else
        return gPhyCurrentChannelPAN1;
}

/*! *********************************************************************************
* \brief  This function will set the radio Tx power
*
* \param[in]   pwrStep   the Tx power
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPlmeSetPwrLevelRequest
(
  uint8_t pwrStep
)
{
#ifdef PHY_PARAMETERS_VALIDATION
  if((pwrStep < 3) || (pwrStep > 31)) /* -40 dBm to 16 dBm */
  {
    return gPhyInvalidParameter_c;
  }
#endif /* PHY_PARAMETERS_VALIDATION */

  MCR20Drv_DirectAccessSPIWrite(PA_PWR, (uint8_t)(pwrStep & 0x1F));
  return gPhySuccess_c;
}

/*! *********************************************************************************
* \brief  This function will return the radio Tx power
*
* \return  Power level
*
********************************************************************************** */
uint8_t PhyPlmeGetPwrLevelRequest(void)
{
    return MCR20Drv_DirectAccessSPIRead(PA_PWR);
}

/*! *********************************************************************************
* \brief  This function will set the value of PHY PIBs
*
* \param[in]   pibId            the Id of the PIB
* \param[in]   pibValue         the new value of the PIB
* \param[in]   phyRegistrySet   the PAN registers (0/1)
* \param[in]   instanceId       the instance of the PHY
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPlmeSetPIBRequest(phyPibId_t pibId, uint64_t pibValue, uint8_t phyRegistrySet, instanceId_t instanceId)
{
  phyStatus_t result = gPhySuccess_c;

  switch(pibId)
  {
    case gPhyPibCurrentChannel_c:
    {
        bool_t value = !!(phyLocal[instanceId].flags & gPhyFlagRxOnWhenIdle_c);
        
        PhyPlmeSetRxOnWhenIdle(FALSE, instanceId);
        result = PhyPlmeSetCurrentChannelRequest((uint8_t) pibValue, phyRegistrySet);
        PhyPlmeSetRxOnWhenIdle(value, instanceId);
    }
    break;
    case gPhyPibTransmitPower_c:
    {
        result = PhyPlmeSetPwrLevelRequest((uint8_t) pibValue);
    }
    break;
    case gPhyPibLongAddress_c:
    {
        uint64_t longAddr = pibValue;
        result = PhyPpSetLongAddr((uint8_t *) &longAddr, phyRegistrySet);
    }
    break;
    case gPhyPibShortAddress_c:
    {
        uint16_t shortAddr = (uint16_t) pibValue;
        result = PhyPpSetShortAddr((uint8_t *) &shortAddr, phyRegistrySet);
    }
    break;
    case gPhyPibPanId_c:
    {
        uint16_t panId = (uint16_t) pibValue;
        result = PhyPpSetPanId((uint8_t *) &panId, phyRegistrySet);
    }
    break;
    case gPhyPibPanCoordinator_c:
    {
        bool_t macRole = (bool_t) pibValue;
        result = PhyPpSetMacRole(macRole, phyRegistrySet);
    }
    break;
    case gPhyPibCurrentPage_c:
    {
        /* Nothinh to do... */
    }
    break;
    case gPhyPibPromiscuousMode_c:
    {
        PhyPpSetPromiscuous((uint8_t)pibValue);
    }
    break;
    case gPhyPibRxOnWhenIdle:
    {
        PhyPlmeSetRxOnWhenIdle( (bool_t)pibValue, instanceId );
    }
    break;
    case gPhyPibFrameWaitTime_c:
    {
        PhyPlmeSetFrameWaitTime( (uint32_t)pibValue, instanceId );
    }
    break;
    case gPhyPibDeferTxIfRxBusy_c:
    {
        if( pibValue )
            phyLocal[instanceId].flags |= gPhyFlagDeferTx_c;
        else
            phyLocal[instanceId].flags &= ~gPhyFlagDeferTx_c;
    }
    break;
    case gPhyPibLastTxAckFP_c:
    {
        result = gPhyReadOnly_c;
    }
    break;
    default:
    {
        result = gPhyUnsupportedAttribute_c;
    }
    break;
  }

  return result;
}

/*! *********************************************************************************
* \brief  This function will return the value of PHY PIBs
*
* \param[in]   pibId            the Id of the PIB
* \param[out]  pibValue         pointer to a location where the value will be stored
* \param[in]   phyRegistrySet   the PAN registers (0/1)
* \param[in]   instanceId       the instance of the PHY
*
* \return  phyStatus_t
*
********************************************************************************** */
phyStatus_t PhyPlmeGetPIBRequest(phyPibId_t pibId, uint64_t * pibValue, uint8_t phyRegistrySet, instanceId_t instanceId)
{
    phyStatus_t result = gPhySuccess_c;
    switch(pibId)
    {
      case gPhyPibCurrentChannel_c:
      {
          *((uint8_t*)pibValue) = (uint64_t) PhyPlmeGetCurrentChannelRequest(phyRegistrySet);
      }
      break;
      case gPhyPibTransmitPower_c:
      {
          *((uint8_t*)pibValue) = MCR20Drv_DirectAccessSPIRead(PA_PWR);
      }
      break;
      case gPhyPibLongAddress_c:
      {
          if( !phyRegistrySet )
              MCR20Drv_IndirectAccessSPIMultiByteRead( MACLONGADDRS0_0, (uint8_t*)pibValue, 8);
          else
              MCR20Drv_IndirectAccessSPIMultiByteRead( MACLONGADDRS1_0, (uint8_t*)pibValue, 8);
      }
      break;
      case gPhyPibShortAddress_c:
      {
          if( !phyRegistrySet )
              MCR20Drv_IndirectAccessSPIMultiByteRead( MACSHORTADDRS0_LSB, (uint8_t*)pibValue, 2);
          else
              MCR20Drv_IndirectAccessSPIMultiByteRead( MACSHORTADDRS1_LSB, (uint8_t*)pibValue, 2);
      }
      break;
      case gPhyPibPanId_c:
      {
          if( !phyRegistrySet )
              MCR20Drv_IndirectAccessSPIMultiByteRead( MACPANID0_LSB, (uint8_t*)pibValue, 2);
          else
              MCR20Drv_IndirectAccessSPIMultiByteRead( MACPANID1_LSB, (uint8_t*)pibValue, 2);
      }
      break;
      case gPhyPibPanCoordinator_c:
      {
          uint8_t phyReg;

          if( !phyRegistrySet )
          {
              phyReg = MCR20Drv_DirectAccessSPIRead( PHY_CTRL4);
              phyReg = (phyReg & cPHY_CTRL4_PANCORDNTR0) == cPHY_CTRL4_PANCORDNTR0;
          }
          else
          {
              phyReg = MCR20Drv_IndirectAccessSPIRead( (uint8_t) DUAL_PAN_CTRL);
              phyReg = (phyReg & cDUAL_PAN_CTRL_PANCORDNTR1) == cDUAL_PAN_CTRL_PANCORDNTR1;
          }

          *((uint8_t*)pibValue) = phyReg;
      }
      break;
      case gPhyPibRxOnWhenIdle:
      {
          *((uint8_t*)pibValue) = !!(phyLocal[instanceId].flags & gPhyFlagRxOnWhenIdle_c);
      }
      break;
      case gPhyPibFrameWaitTime_c:
      {
          *((uint8_t*)pibValue) = phyLocal[instanceId].maxFrameWaitTime;
      }
      break;
      case gPhyPibDeferTxIfRxBusy_c:
      {
          *((uint8_t*)pibValue) = !!(phyLocal[instanceId].flags & gPhyFlagDeferTx_c);
      }
      break;
      case gPhyPibLastTxAckFP_c:
      {
          *((uint8_t*)pibValue) = !!(phyLocal[instanceId].flags & gPhyFlagTxAckFP_c);
      }
      break;
      default:
      {
          result = gPhyUnsupportedAttribute_c;
      }
      break;
    }

    return result;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  This function try to restart the Rx
*
* \param[in]   param  phy Rx params
*
********************************************************************************** */
#if gPhyRxRetryInterval_c
static void PhyRxRetry( uint32_t param )
{
    volatile phyRxParams_t *pRxParams = &phyLocal[param].rxParams;
    phyTime_t absEndTime = pRxParams->timeStamp;

    absEndTime += pRxParams->duration;
    gRxRetryTimer = gInvalidTimerId_c;

    if( PhyTime_GetTimestamp() < absEndTime )
    {
        PhyPlmeRxRequest( pRxParams->phyRxMode, (phyRxParams_t*)pRxParams );
    }
    else
    {
        Radio_Phy_TimeRxTimeoutIndication(param);
    }
}
#endif