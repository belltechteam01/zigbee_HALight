/*
 * Copyright (c) 2013-2014, Freescale Semiconductor, Inc.
 * All rights reserved.
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

#include "board.h"
#include "fsl_clock_manager.h"
#include "fsl_smc_hal.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"

/* Configuration for enter VLPR mode. Core clock = 4MHz. */
const clock_manager_user_config_t g_defaultClockConfigVlpr =
{
    .mcgConfig =
    {
        .mcg_mode           = kMcgModeBLPI,   // Work in BLPI mode.
        .irclkEnable        = true,  // MCGIRCLK enable.
        .irclkEnableInStop  = false, // MCGIRCLK disable in STOP mode.
        .ircs               = kMcgIrcFast, // Select IRC4M.
        .fcrdiv             = 0U,    // FCRDIV is 0.

        .frdiv   = 0U,
        .drs     = kMcgDcoRangeSelLow,  // Low frequency range
        .dmx32   = kMcgDmx32Default,    // DCO has a default range of 25%
        .oscsel  = kMcgOscselRtc,       // Select RTC

    },
    .simConfig =
    {
        .pllFllSel = kClockPllFllSelFll, // PLLFLLSEL select FLL.
        .er32kSrc  = kClockEr32kSrcLpo,     // ERCLK32K selection, use LPO.
        .outdiv1   = 0U,
        .outdiv4   = 4U,
    }
};

/* Configuration for enter RUN mode. Core clock = 32MHz. */
const clock_manager_user_config_t g_defaultClockConfigRun =
{
    .mcgConfig =
    {
        .mcg_mode           = kMcgModeBLPE, // Work in BLPE mode.
        .irclkEnable        = true,  // MCGIRCLK enable.
        .irclkEnableInStop  = false, // MCGIRCLK disable in STOP mode.
        .ircs               = kMcgIrcSlow, // Select IRC32k.
        .fcrdiv             = 0U,    // FCRDIV is 0.

        .frdiv   = 5U,
        .drs     = kMcgDcoRangeSelLow,  // Low frequency range
        .dmx32   = kMcgDmx32Default,    // DCO has a default range of 25%
        .oscsel  = kMcgOscselOsc,       // Select 
    },
    .simConfig =
    {
        .pllFllSel = kClockPllFllSelFll,    // PLLFLLSEL select FLL.
        .er32kSrc  = kClockEr32kSrcLpo,     // ERCLK32K selection, use LPO.
        .outdiv1   = 0U,
        .outdiv4   = 1U,
    }
};


/* Function to initialize OSC0 base on board configuration. */
void BOARD_InitOsc0(void)
{
    // OSC0 has not configuration register, only set frequency
    MCG_WR_C2_RANGE(MCG,kOscRangeVeryHigh);
    g_xtal0ClkFreq = 32000000U;
}

/* Function to initialize RTC external clock base on board configuration. */
void BOARD_InitRtcOsc(void)
{
    rtc_osc_user_config_t rtcOscConfig =
    {
        .freq                = RTC_XTAL_FREQ,
        .enableCapacitor2p   = RTC_SC2P_ENABLE_CONFIG,
        .enableCapacitor4p   = RTC_SC4P_ENABLE_CONFIG,
        .enableCapacitor8p   = RTC_SC8P_ENABLE_CONFIG,
        .enableCapacitor16p  = RTC_SC16P_ENABLE_CONFIG,
        .enableOsc           = RTC_OSC_ENABLE_CONFIG,
        .enableClockOutput   = RTC_CLK_OUTPUT_ENABLE_CONFIG,
    };

    CLOCK_SYS_RtcOscInit(0U, &rtcOscConfig);
}

void clockStableDelay(void)
{
}

/* Initialize clock. */
void BOARD_ClockInit(void)
{
    /* Set allowed power mode, allow all. */
    SMC_HAL_SetProtection(SMC, kAllowPowerModeAll);

    /* Setup board clock source. */
    // Setup OSC0 if used.
    // Configure OSC0 pin mux.
    PORT_HAL_SetMuxMode(EXTAL0_PORT, EXTAL0_PIN, EXTAL0_PINMUX);
    PORT_HAL_SetMuxMode(XTAL0_PORT, XTAL0_PIN, XTAL0_PINMUX);

    BOARD_InitOsc0();
    BOARD_InitRtcOsc();

    /* Set system clock configuration. */
#if (CLOCK_INIT_CONFIG == CLOCK_VLPR)
    CLOCK_SYS_SetConfiguration(&g_defaultClockConfigVlpr);
#else
    //CLOCK_SYS_SetConfiguration(&g_defaultClockConfigRun);

    uint32_t outClkFreq;
    
    /* Set outdiv for safe output clock frequency. */
    CLOCK_HAL_SetOutDiv(SIM, 2U, 0U, 0U, 5U);
    
    // Configure FLL in FBE mode then switch to FEE mode.
    CLOCK_HAL_SetFbeMode(MCG,
                         g_defaultClockConfigRun.mcgConfig.oscsel,
                         g_defaultClockConfigRun.mcgConfig.frdiv,
                         g_defaultClockConfigRun.mcgConfig.dmx32,
                         g_defaultClockConfigRun.mcgConfig.drs,
                         clockStableDelay,
                         &outClkFreq);
    
    // Enalbe low power mode.
    CLOCK_HAL_SetLowPowerModeCmd(MCG, true);
    
    /* Set SIM setting. */
    CLOCK_HAL_SetOutDiv(SIM,
                        g_defaultClockConfigRun.simConfig.outdiv1,
                        0U,
                        0U,
                        g_defaultClockConfigRun.simConfig.outdiv4);

#if FSL_FEATURE_MCG_HAS_PLL_FLL_SELECTION    
    CLOCK_HAL_SetPllfllSel(SIM, g_defaultClockConfigRun.simConfig.pllFllSel);
#endif
    
    CLOCK_HAL_SetExternalRefClock32kSrc(SIM, g_defaultClockConfigRun.simConfig.er32kSrc);
#endif
    
    CLOCK_SYS_SetTpmSrc(0, kClockTpmSrcOsc0erClk);
}

void dbg_uart_init(void)
{
    configure_lpuart_pins(BOARD_DEBUG_UART_INSTANCE);

    // Select different clock source for LPSCI. */
#if (CLOCK_INIT_CONFIG == CLOCK_VLPR)
    CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcMcgIrClk);
#else
    CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcMcgFllClk);
#endif

//    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUD, kDebugConsoleLPUART);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

 
