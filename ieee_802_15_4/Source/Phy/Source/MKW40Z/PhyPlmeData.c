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
#include "Phy.h"
#include "PhyInterface.h"
#include "EmbeddedTypes.h"
#include "FunctionLib.h"

#include "fsl_os_abstraction.h"
#include "fsl_device_registers.h"


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
extern Phy_PhyLocalStruct_t     phyLocal[];


/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
********************************************************************************** */
static void PhyRxRetry( uint32_t param );


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
    uint8_t xcvseq;
    uint8_t *pPB;

#ifdef PHY_PARAMETERS_VALIDATION
    // null pointer
    if(NULL == pTxPacket)
    {
        return gPhyInvalidParameter_c;
    }

    // if CCA required ...
    if( (pTxPacket->CCABeforeTx == gPhyCCAMode3_c) || (pTxPacket->CCABeforeTx == gPhyEnergyDetectMode_c))
    { // ... cannot perform other types than MODE1 and MODE2
        return gPhyInvalidParameter_c;
    }

#endif // PHY_PARAMETERS_VALIDATION

    if( gIdle_c != PhyPpGetState() )
    {
        return gPhyBusy_c;
    }

    // load data into PB
    pPB = (uint8_t*)&ZLL_PKT_BUFFER0;
    pPB[0] = pTxPacket->psduLength + 2; /* including 2 bytes of FCS */
    FLib_MemCpy( &pPB[1], pTxPacket->pPsdu, pTxPacket->psduLength );

    // perform CCA before TX if required
    if( pTxPacket->CCABeforeTx != gPhyNoCCABeforeTx_c )
    {
        ZLL_BWR_PHY_CTRL_CCABFRTX(ZLL, 1);
        ZLL_BWR_PHY_CTRL_CCATYPE( ZLL, pTxPacket->CCABeforeTx );
    }
    else
    {
        ZLL_BWR_PHY_CTRL_CCABFRTX(ZLL, 0);
    }
    
    //gcapraru:
    ZLL_BWR_PHY_CTRL_CCABFRTX(ZLL, 0);

    // slotted operation
    if( pTxPacket->slottedTx == gPhySlottedMode_c )
    {
        ZLL_BWR_PHY_CTRL_SLOTTED(ZLL, 1);
    }
    else
    {
        ZLL_BWR_PHY_CTRL_SLOTTED(ZLL, 0);
    }

    // perform TxRxAck sequence if required by phyTxMode
    if(pTxPacket->ackRequired == gPhyRxAckRqd_c)
    {
        PhyIsrPassRxParams(pRxParams);
        ZLL_BWR_PHY_CTRL_RXACKRQD(ZLL, 1);
        xcvseq = gTR_c;
    }
    else
    {
        PhyIsrPassRxParams(NULL);
        ZLL_BWR_PHY_CTRL_RXACKRQD(ZLL, 0);
        xcvseq = gTX_c;
    }

    // ensure that no spurious interrupts are raised(do not change TMR1 and TMR4 IRQ status)
    ZLL_IRQSTS &= ~(ZLL_IRQSTS_TMR1IRQ_MASK | ZLL_IRQSTS_TMR4IRQ_MASK);
    ZLL_IRQSTS |= ZLL_IRQSTS_TMR3MSK_MASK;

    // start the TX / TRX / CCA sequence
    ZLL_BWR_PHY_CTRL_XCVSEQ( ZLL, xcvseq );
    // unmask SEQ interrupt
    ZLL_BWR_PHY_CTRL_SEQMSK(ZLL, 0);

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
#ifdef PHY_PARAMETERS_VALIDATION
    if(NULL == pRxParams)
    {
        return gPhyInvalidParameter_c;
    }
#endif // PHY_PARAMETERS_VALIDATION

    if( gIdle_c != PhyPpGetState() )
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
        phyTimeEvent_t event = {
            .timestamp = PhyTime_GetTimestamp() + gPhyRxRetryInterval_c,
            .parameter = (uint32_t)pRxParams,
            .callback  = PhyRxRetry,
        };

        PhyTime_ScheduleEvent( &event );
        return gPhyTRxOff_c;   
    }

    PhyIsrPassRxParams(pRxParams);

    pRxParams->pRxData->msgData.dataInd.pPsdu = 
        (uint8_t*)&pRxParams->pRxData->msgData.dataInd.pPsdu +
        sizeof(pRxParams->pRxData->msgData.dataInd.pPsdu);

    // slotted operation
    if(gPhySlottedMode_c == phyRxMode)
    {
        ZLL_BWR_PHY_CTRL_SLOTTED(ZLL, 1);
    }
    else
    {
        ZLL_BWR_PHY_CTRL_SLOTTED(ZLL, 0);
    }

    // ensure that no spurious interrupts are raised, but do not change TMR1 and TMR4 IRQ status
    ZLL_IRQSTS &= ~(ZLL_IRQSTS_TMR1IRQ_MASK | ZLL_IRQSTS_TMR4IRQ_MASK);
    ZLL_IRQSTS |= ZLL_IRQSTS_TMR3MSK_MASK;

    // start the RX sequence
    ZLL_BWR_PHY_CTRL_XCVSEQ( ZLL, gRX_c );
    // unmask SEQ interrupt
    ZLL_BWR_PHY_CTRL_SEQMSK(ZLL, 0);

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
#ifdef PHY_PARAMETERS_VALIDATION
    // illegal CCA type
    if( (ccaParam != gPhyCCAMode1_c) && (ccaParam != gPhyCCAMode2_c) && (ccaParam != gPhyCCAMode3_c) && (ccaParam != gPhyEnergyDetectMode_c))
    {
        return gPhyInvalidParameter_c;
    }

    // cannot perform Continuous CCA using ED type
    if( (ccaParam == gPhyEnergyDetectMode_c) && (cccaMode == gPhyContCcaEnabled) )
    {
        return gPhyInvalidParameter_c;
    }
#endif // PHY_PARAMETERS_VALIDATION

    if( gIdle_c != PhyPpGetState() )
    {
        return gPhyBusy_c;
    }

    // write in PHY CTRL4 the desired type of CCA
    ZLL_BWR_PHY_CTRL_CCATYPE( ZLL, ccaParam );

    // ensure that no spurious interrupts are raised(do not change TMR1 and TMR4 IRQ status)
    ZLL_IRQSTS &= ~(ZLL_IRQSTS_TMR1IRQ_MASK | ZLL_IRQSTS_TMR4IRQ_MASK);
    ZLL_IRQSTS |= ZLL_IRQSTS_TMR3MSK_MASK;

    // unmask SEQ interrupt
    ZLL_BWR_PHY_CTRL_SEQMSK(ZLL, 0);

    // continuous CCA
    if(cccaMode == gPhyContCcaEnabled)
    {
        // start the continuous CCA sequence
        // immediately or by TC2', depending on a previous PhyTimeSetEventTrigger() call)
        ZLL_BWR_PHY_CTRL_XCVSEQ( ZLL, gCCCA_c );
    }
    // normal CCA (not continuous)
    else
    {
        // start the CCA or ED sequence (this depends on CcaType used)
        // immediately or by TC2', depending on a previous PhyTimeSetEventTrigger() call)
        ZLL_BWR_PHY_CTRL_XCVSEQ( ZLL, gCCA_c );
    }
    // at the end of the scheduled sequence, an interrupt will occur:
    // CCA , SEQ or TMR3

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
#endif // PHY_PARAMETERS_VALIDATION

  if( !pan )
      ZLL_WR_CHANNEL_NUM0( ZLL, channel );
  else
      ZLL_WR_CHANNEL_NUM1( ZLL, channel );

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
      return ZLL_CHANNEL_NUM0;
  else
      return ZLL_CHANNEL_NUM1;
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
  if((pwrStep < 3) || (pwrStep > 31)) //-40 dBm to 16 dBm
  {
    return gPhyInvalidParameter_c;
  }
#endif // PHY_PARAMETERS_VALIDATION

  ZLL_PA_PWR = pwrStep;
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
    return (uint8_t)ZLL_PA_PWR;
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
        result = PhyPlmeSetCurrentChannelRequest((uint8_t) pibValue, phyRegistrySet);
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
          *((uint8_t*)pibValue) = PhyPlmeGetPwrLevelRequest();
      }
      break;
      case gPhyPibLongAddress_c:
      {
          if( !phyRegistrySet )
          {
              *pibValue = ZLL_RD_MACLONGADDRS0_MSB(ZLL);
              *pibValue <<= 32;
              *pibValue = ZLL_RD_MACLONGADDRS0_LSB(ZLL);
          }
          else
          {
              *pibValue = ZLL_RD_MACLONGADDRS1_MSB(ZLL);
              *pibValue <<= 32;
              *pibValue = ZLL_RD_MACLONGADDRS1_LSB(ZLL);
          }
      }
      break;
      case gPhyPibShortAddress_c:
      {
          if( !phyRegistrySet )
              *((uint16_t*)pibValue) = ZLL_RD_MACSHORTADDRS0_MACSHORTADDRS0(ZLL);
          else
              *((uint16_t*)pibValue) = ZLL_RD_MACSHORTADDRS1_MACSHORTADDRS1(ZLL);
      }
      break;
      case gPhyPibPanId_c:
      {
          if( !phyRegistrySet )
              *((uint16_t*)pibValue) = ZLL_RD_MACSHORTADDRS0_MACPANID0(ZLL);
          else
              *((uint16_t*)pibValue) = ZLL_RD_MACSHORTADDRS1_MACPANID1(ZLL);
      }
      break;
      case gPhyPibPanCoordinator_c:
      {
          if( !phyRegistrySet )
              *((uint8_t*)pibValue) = ZLL_RD_PHY_CTRL_PANCORDNTR0(ZLL);
          else
              *((uint8_t*)pibValue) = ZLL_RD_DUAL_PAN_CTRL_PANCORDNTR1(ZLL);
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
static void PhyRxRetry( uint32_t param )
{
    PhyPlmeRxRequest( ((phyRxParams_t*)param)->phyRxMode, (phyRxParams_t*)param );
}