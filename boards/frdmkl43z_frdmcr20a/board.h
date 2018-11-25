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

#if !defined(__BOARD_H__)
#define __BOARD_H__

#include <stdint.h>

#include "gpio_pins.h"
#include "pin_mux.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/
/* The board name */
#define BOARD_NAME                      "FRDM-KL43Z_FRDM-MCR20A"

#define CLOCK_VLPR 1U
#define CLOCK_RUN  2U
#define CLOCK_NUMBER_OF_CONFIGURATIONS 3U

#ifndef CLOCK_INIT_CONFIG
#define CLOCK_INIT_CONFIG CLOCK_RUN
#endif

/* OSC0 configuration. */
#define OSC0_XTAL_FREQ 32768U
#define OSC0_SC2P_ENABLE_CONFIG  false
#define OSC0_SC4P_ENABLE_CONFIG  false
#define OSC0_SC8P_ENABLE_CONFIG  false
#define OSC0_SC16P_ENABLE_CONFIG false
#define MCG_HGO0   kOscGainLow
#define MCG_RANGE0 kOscRangeLow
#define MCG_EREFS0 kOscSrcOsc

/* NMI pin */
#define NMI_PORT   PORTA
#define NMI_PIN    4

/* EXTAL0 PTA18 */
#define EXTAL0_PORT   PORTA
#define EXTAL0_PIN    18
#define EXTAL0_PINMUX kPortPinDisabled

/* XTAL0 PTA19 */
#define XTAL0_PORT   PORTA
#define XTAL0_PIN    19
#define XTAL0_PINMUX kPortPinDisabled

/* RTC external clock configuration. */
#define RTC_XTAL_FREQ   0U
#define RTC_SC2P_ENABLE_CONFIG       false
#define RTC_SC4P_ENABLE_CONFIG       false
#define RTC_SC8P_ENABLE_CONFIG       false
#define RTC_SC16P_ENABLE_CONFIG      false
#define RTC_OSC_ENABLE_CONFIG        false
#define RTC_CLK_OUTPUT_ENABLE_CONFIG false

/* RTC_CLKIN PTC1 */
#define RTC_CLKIN_PORT   PORTC
#define RTC_CLKIN_PIN    1
#define RTC_CLKIN_PINMUX kPortMuxAsGpio

/*! The LPUART to use for debug messages. */
#define BOARD_DEBUG_UART_INSTANCE   0
#define BOARD_DEBUG_UART_BASEADDR   LPUART0
#if !defined (BOARD_DEBUG_UART_BAUD)
#define BOARD_DEBUG_UART_BAUD       115200
#endif

/* This define to use for power manager demo */
#define BOARD_LOW_POWER_UART_BAUD       9600

#define BOARD_USE_LPUART
#define PM_DBG_UART_IRQ_HANDLER         MODULE_IRQ_HANDLER(LPUART0)
#define PM_DBG_UART_IRQn                LPUART0_IRQn

#define BOARD_DAC_DEMO_DAC_INSTANCE     0U

/* Connectivity */
#define gXcvrSpiInstance_c      1

#ifndef APP_SERIAL_INTERFACE_TYPE
#define APP_SERIAL_INTERFACE_TYPE (gSerialMgrLpuart_c)
#endif

#ifndef APP_SERIAL_INTERFACE_INSTANCE
#define APP_SERIAL_INTERFACE_INSTANCE (0)
#endif

#define BOARD_DAC_DEMO_ADC_INSTANCE     0U
#define BOARD_DAC_DEMO_ADC_CHANNEL      23U

#define BOARD_TPM_INSTANCE               0
#define BOARD_TPM_CHANNEL                5

/* Define print statement to inform user which switch to press for
 * power_manager_hal_demo and power_manager_rtos_demo
 */
#define PRINT_LLWU_SW_NUM \
  PRINTF("SW3")

 /* Defines the llwu pin number for board switch which is used in power_manager_demo. */
#define BOARD_SW_HAS_LLWU_PIN        1
#define BOARD_SW_LLWU_EXT_PIN        7
/* Switch port base address and IRQ handler name. Used by power_manager_demo */
#define BOARD_SW_LLWU_PIN            3
#define BOARD_SW_LLWU_BASE           PORTC
#define BOARD_SW_LLWU_IRQ_HANDLER    PORTCD_IRQHandler
#define BOARD_SW_LLWU_IRQ_NUM        PORTCD_IRQn

/* The rtc instance used for rtc_func */
#define BOARD_RTC_FUNC_INSTANCE         0

#define HWADC_INSTANCE               0
#define ADC_IRQ_N                    ADC0_IRQn
#if (defined FSL_RTOS_MQX)
#define MQX_ADC_IRQHandler           MQX_ADC0_IRQHandler
#endif

/* Isn't supported now to use DAC as sine generator in adc_hw_trigger demo */
//#define USE_DAC_OUT_AS_SOURCE

#define BOARD_CMP_INSTANCE             0
#define BOARD_CMP_CHANNEL              0

#define BOARD_I2C_COMM_INSTANCE     1

#define BOARD_I2C_GPIO_SCL          GPIO_MAKE_PIN(GPIOE_IDX, 24)
#define BOARD_I2C_GPIO_SDA          GPIO_MAKE_PIN(GPIOE_IDX, 25)



/******************************************************************************
 * Externs
 ******************************************************************************/

/******************************************************************************
 * Prototypes
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void hardware_init(void);
void dbg_uart_init(void);
/*This function to used for power manager demo*/
void disable_unused_pins(void);
void enable_unused_pins(void);
/* Function to initialize clock base on board configuration. */
void BOARD_ClockInit(void);

/* Function to initialize OSC0 base on board configuration. */
void BOARD_InitOsc0(void);

/* Function to initialize RTC external clock base on board configuration. */
void BOARD_InitRtcOsc(void);

/* Function to initialize the MCR20 XCVR */
void BOARD_InitXCVR(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __BOARD_H__ */
/******************************************************************************
 * EOF
 ******************************************************************************/
