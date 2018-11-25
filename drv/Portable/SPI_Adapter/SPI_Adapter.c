/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file SPI_adapter.c
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
#include "SPI_Adapter.h"
#include "pin_mux.h"
#include "panic.h"

/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */
#ifndef gXcvrSpiInstance_c
#define gXcvrSpiInstance_c (0xFF)
#endif

#if BOARD_USE_DSPI
    #define SPI_HAL_IsMaster                 DSPI_HAL_IsMaster
    #define SPI_DRV_IRQHandler               DSPI_DRV_IRQHandler
    #define SPI_DRV_MasterTransferBlocking   DSPI_DRV_MasterTransferBlocking
    #define SPI_DRV_MasterTransfer           DSPI_DRV_MasterTransfer
    #define SPI_DRV_MasterAbortTransfer      DSPI_DRV_MasterAbortTransfer
    #define SPI_DRV_MasterGetTransferStatus  DSPI_DRV_MasterGetTransferStatus
    #define SPI_DRV_SlaveTransferBlocking    DSPI_DRV_SlaveTransferBlocking
    #define SPI_DRV_SlaveTransfer            DSPI_DRV_SlaveTransfer
    #define SPI_DRV_SlaveAbortTransfer       DSPI_DRV_SlaveAbortTransfer
    #define g_spiIrqId                       g_dspiIrqId
    #define g_spiStatePtr                    g_dspiStatePtr
    #define g_spiBase                        g_dspiBase
    #define kStatus_SPI_Busy                 kStatus_DSPI_Busy
#endif

/*! *********************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
********************************************************************************** */
typedef void (*pfSPIx_ISR_t)(void);


/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
********************************************************************************** */
static void SPIx_ISR(void);
extern void SPI_DRV_IRQHandler(uint32_t instance);


/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */
#if BOARD_USE_DSPI
static const dspi_master_user_config_t defaultSpiMasterCfg = {
    .isChipSelectContinuous = false,
    .isSckContinuous = false,
    .pcsPolarity = kDspiPcs_ActiveLow,
    .whichCtar = kDspiCtar0,
    .whichPcs = kDspiPcs0,
};
#endif

static pfSPIx_TRxCB_t mSpiTRxCb[SPI_INSTANCE_COUNT];
extern void * g_spiStatePtr[SPI_INSTANCE_COUNT];

/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */
uint32_t SpiMaster_Init(uint32_t instance, spiMasterState_t* pSpiState, pfSPIx_TRxCB_t pfCallback)
{
    IRQn_Type spiIrq = g_spiIrqId[instance];

    if( instance == gXcvrSpiInstance_c )
    {
        panic(0,(uint32_t)SpiMaster_Init,0,0);
        return 1;
    }

#if BOARD_USE_DSPI
    DSPI_DRV_MasterInit(instance, pSpiState, &defaultSpiMasterCfg);
#else
    SPI_DRV_MasterInit (instance, pSpiState);
#endif

    configure_spi_pins(instance);
    mSpiTRxCb[instance] = pfCallback;
    /* Overwrite old ISR */
    (void)OSA_InstallIntHandler(spiIrq, SPIx_ISR);
    /* set interrupt priority */
    NVIC_SetPriority(spiIrq, gSpi_IsrPrio_c >> (8 - __NVIC_PRIO_BITS));
    NVIC_ClearPendingIRQ(spiIrq);
    NVIC_EnableIRQ(spiIrq);
    return 0;
}

/*! *********************************************************************************
********************************************************************************** */
void SpiMaster_Configure(uint32_t instance, spiBusConfig_t* pConfig)
{
    uint32_t temp;

#if BOARD_USE_DSPI
    dspi_device_t dspiCfg = {
        .bitsPerSec                 = pConfig->bitsPerSec,
        .dataBusConfig.bitsPerFrame = pConfig->bitsPerFrame,
        .dataBusConfig.clkPhase     = (dspi_clock_phase_t)pConfig->clkPhase,
        .dataBusConfig.clkPolarity  = (dspi_clock_polarity_t)pConfig->clkPolarity,
        .dataBusConfig.direction    = (dspi_shift_direction_t )pConfig->direction
    };

    DSPI_DRV_MasterConfigureBus(instance, &dspiCfg, &temp);
    temp = 100; /* [ns] */
    DSPI_DRV_MasterSetDelay(instance, kDspiAfterTransfer, temp, &temp);
#else
    spi_master_user_config_t spiCfg = {
        .bitsPerSec = pConfig->bitsPerSec,
        .polarity   = (spi_clock_polarity_t)pConfig->clkPolarity,
        .phase      = (spi_clock_phase_t)pConfig->clkPhase,
        .direction  = (spi_shift_direction_t)pConfig->direction
    };

    SPI_DRV_MasterConfigureBus (instance, &spiCfg, &temp);
#endif
    (void)temp;
}

/*! *********************************************************************************
********************************************************************************** */
uint32_t SpiMaster_SyncTransfer(uint32_t instance, uint8_t* pTxData, uint8_t* pRxData, uint16_t size)
{
    SPI_DRV_MasterAbortTransfer(instance);
    return SPI_DRV_MasterTransferBlocking(instance, NULL, pTxData, pRxData, size, OSA_WAIT_FOREVER);
}

/*! *********************************************************************************
********************************************************************************** */
uint32_t SpiMaster_AsyncTransfer(uint32_t instance, uint8_t* pTxData, uint8_t* pRxData, uint16_t size)
{
    SPI_DRV_MasterAbortTransfer(instance);
    return SPI_DRV_MasterTransfer(instance, NULL, pTxData, pRxData, size);
}

/*! *********************************************************************************
********************************************************************************** */
uint8_t SpiMaster_GetStatus(uint32_t instance)
{
    if( kStatus_SPI_Busy == SPI_DRV_MasterGetTransferStatus(instance, NULL) )
        return 1;

    return 0;
}

/*! *********************************************************************************
********************************************************************************** */
uint32_t SpiSlave_Init(uint32_t instance, spiSlaveState_t* pSpiState, pfSPIx_TRxCB_t pfCallback)
{
    IRQn_Type spiIrq = g_spiIrqId[instance];
    
    if( instance == gXcvrSpiInstance_c )
    {
        panic(0,(uint32_t)SpiMaster_Init,0,0);
        return 1;
    }
    
#if BOARD_USE_DSPI
    dspi_slave_user_config_t spiSlaveCfg = {
        .dataConfig.bitsPerFrame = 8,
        .dataConfig.clkPhase     = gSpiClkPhase_FirstEdge_d,
        .dataConfig.clkPolarity  = gSpiClk_ActiveHigh_d,
        .dataConfig.direction    = gSpiMsbFirst_d,
    };

    DSPI_DRV_SlaveInit(instance, pSpiState, &spiSlaveCfg);
#else
    spi_slave_user_config_t spiSlaveCfg = {
        .phase                   = gSpiClkPhase_FirstEdge_d,
        .polarity                = gSpiClk_ActiveHigh_d,
        .direction               = gSpiMsbFirst_d
    };

    SPI_DRV_SlaveInit(instance, pSpiState, &spiSlaveCfg);
#endif

    configure_spi_pins(instance);
    mSpiTRxCb[instance] = pfCallback;
    /* Overwrite old ISR */
    (void)OSA_InstallIntHandler(spiIrq, SPIx_ISR);
    /* set interrupt priority */
    NVIC_SetPriority(spiIrq, gSpi_IsrPrio_c >> (8 - __NVIC_PRIO_BITS));
    NVIC_ClearPendingIRQ(spiIrq);
    NVIC_EnableIRQ(spiIrq);
    return 0;
}

/*! *********************************************************************************
********************************************************************************** */
uint32_t SpiSlave_SyncTransfer (uint32_t instance, uint8_t* pTxData, uint8_t* pRxData, uint16_t size)
{
    SPI_DRV_SlaveAbortTransfer(instance);
    return SPI_DRV_SlaveTransferBlocking(instance, pTxData, pRxData, size, OSA_WAIT_FOREVER);
}

/*! *********************************************************************************
********************************************************************************** */
uint32_t SpiSlave_AsyncTransfer(uint32_t instance, uint8_t* pTxData, uint8_t* pRxData, uint16_t size)
{
    SPI_DRV_SlaveAbortTransfer(instance);
    return SPI_DRV_SlaveTransfer(instance, pTxData, pRxData, size);
}

/*! *********************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
********************************************************************************* */
static void SPIx_ISR(void)
{
    uint32_t instance = (__get_IPSR() - 16) - g_spiIrqId[0];
    uint8_t idle;

    SPI_DRV_IRQHandler(instance);

    if( SPI_HAL_IsMaster(g_spiBase[instance]) )
    {
        idle = !((spiMasterState_t*)g_spiStatePtr[instance])->isTransferInProgress;
    }
    else
    {
        idle = !(( spiSlaveState_t *)g_spiStatePtr[instance])->isTransferInProgress;
    }

    if( mSpiTRxCb[instance] && idle )
    {
        mSpiTRxCb[instance](instance);
    }
}
