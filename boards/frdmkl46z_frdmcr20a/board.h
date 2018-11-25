/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
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
#include "pin_mux.h"
#include "gpio_pins.h"

/* The board name */
#define BOARD_NAME                      "FRDM-KL46Z_FRDM-MCR20A"

#define CLOCK_VLPR 1U
#define CLOCK_RUN  2U
#define CLOCK_NUMBER_OF_CONFIGURATIONS 3U

#ifndef CLOCK_INIT_CONFIG
#define CLOCK_INIT_CONFIG CLOCK_RUN
#endif

/* OSC0 configuration. */
#define OSC0_XTAL_FREQ 8000000U
#define OSC0_SC2P_ENABLE_CONFIG  false
#define OSC0_SC4P_ENABLE_CONFIG  false
#define OSC0_SC8P_ENABLE_CONFIG  false
#define OSC0_SC16P_ENABLE_CONFIG false
#define MCG_HGO0   kOscGainLow
#define MCG_RANGE0 kOscRangeVeryHigh
#define MCG_EREFS0 kOscSrcOsc

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

/* The UART to use for debug messages. */
#ifndef BOARD_DEBUG_UART_INSTANCE
    #define BOARD_DEBUG_UART_INSTANCE   0
    #define BOARD_DEBUG_UART_BASEADDR   UART0
#endif
#ifndef BOARD_DEBUG_UART_BAUD
    #define BOARD_DEBUG_UART_BAUD       115200
#endif

#define BOARD_USE_LPSCI

/* Connectivity */
#define gXcvrSpiInstance_c      1

#ifndef APP_SERIAL_INTERFACE_TYPE
#define APP_SERIAL_INTERFACE_TYPE (gSerialMgrLpsci_c)
#endif

#ifndef APP_SERIAL_INTERFACE_INSTANCE
#define APP_SERIAL_INTERFACE_INSTANCE (0)
#endif

#define PM_DBG_UART_IRQ_HANDLER         MODULE_IRQ_HANDLER(UART0)
#define PM_DBG_UART_IRQn                UART0_IRQn

/* Define the port interrupt number for the board switches */
#define BOARD_SW_GPIO               kGpioSW1
#define BOARD_SW_IRQ_NUM            PORTC_PORTD_IRQn
#define BOARD_SW_IRQ_HANDLER        PORTC_PORTD_IRQHandler

/* Define print statement to inform user which switch to press for
 * power_manager_hal_demo and power_manager_rtos_demo
 */
#define PRINT_LLWU_SW_NUM \
  PRINTF("SW1")

 /* Defines the llwu pin number for board switch which is used in power_manager_demo. */
#define BOARD_SW_HAS_LLWU_PIN           1
#define BOARD_SW_LLWU_EXT_PIN           kLlwuWakeupPin7                 /* PTC3 */
#define BOARD_SW_LLWU_WakeupPins_c      {BOARD_SW_LLWU_EXT_PIN}
#define BOARD_SW_LLWU_KeyboardMask_c    (1<<BOARD_SW_LLWU_EXT_PIN)   
/* Switch port base address and IRQ handler name. Used by power_manager_demo */
#define BOARD_SW_LLWU_PIN            3
#define BOARD_SW_LLWU_BASE           PORTC
#define BOARD_SW_LLWU_IRQ_HANDLER    PORTC_PORTD_IRQHandler
#define BOARD_SW_LLWU_IRQ_NUM        PORTC_PORTD_IRQn

#define BOARD_I2C_GPIO_SCL              GPIO_MAKE_PIN(GPIOE_IDX, 24)
#define BOARD_I2C_GPIO_SDA              GPIO_MAKE_PIN(GPIOE_IDX, 25)

/* The instances of peripherals used for dac_adc_demo */
#define BOARD_DAC_DEMO_DAC_INSTANCE     0U
#define BOARD_DAC_DEMO_ADC_INSTANCE     0U
#define BOARD_DAC_DEMO_ADC_CHANNEL      0U

/* The i2c instance used for i2c DAC demo */
#define BOARD_DAC_I2C_INSTANCE          1

/* The i2c instance used for i2c communication demo */
#define BOARD_I2C_COMM_INSTANCE         1

/* The TPM instance/channel used for board */
#define BOARD_TPM_INSTANCE              0
#define BOARD_TPM_CHANNEL               5

/* The CMP instance used for board. */
#define BOARD_CMP_INSTANCE              0
/* The CMP channel used for board. */
#define BOARD_CMP_CHANNEL               0

/* The rtc instance used for rtc_func */
#define BOARD_RTC_FUNC_INSTANCE         0


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void hardware_init(void);
void dbg_uart_init(void);
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
