/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : pin_mux.h
**     Project     : mrbkw01
**     Processor   : MKW01Z128CHN4
**     Component   : PinSettings
**     Version     : Component 01.002, Driver 1.1, CPU db: 3.00.000
**     Compiler    : IAR ARM C Compiler
**     Date/Time   : 2013-12-10, 14:58, # CodeGen: 7
**     Abstract    :
**
**     Settings    :
**
**     Contents    :
**         GPIO                - void pin_mux_GPIO(uint32_t instance);
**         I2C                 - void pin_mux_I2C(uint32_t instance);
**         RTC                 - void pin_mux_RTC(uint32_t instance);
**         SPI                 - void pin_mux_SPI(uint32_t instance);
**         UART                - void pin_mux_UART(uint32_t instance);
**
**     Copyright : 1997 - 2013 Freescale Semiconductor, Inc. All Rights Reserved.
**     SOURCE DISTRIBUTION PERMISSIBLE as directed in End User License Agreement.
**
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/
/*!
** @file pin_mux.h
** @version 1.1
** @brief
**
*/
/*!
**  @addtogroup pin_mux_module pin_mux module documentation
**  @{
*/

#ifndef pin_mux_H_
#define pin_mux_H_

/* MODULE pin_mux. */

/*
** ===================================================================
**     Method      :  configure_gpio_pins (component PinSettings)
*/
/*!
**     @brief
**         GPIO method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         PTA, PTB, PTC, PTD, PTE
**         peripherals.
**     @param
**         uint32_t instance - GPIO instance number 0..4
*/
/* ===================================================================*/
void configure_gpio_pins(uint32_t instance);
/*
** ===================================================================
**     Method      :  configure_i2c_pins(component PinSettings)
*/
/*!
**     @brief
**         I2C method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         I2C0, I2C1, I2C2
**         peripherals.
**     @param
**         uint32_t instance - I2C instance number 0..2
*/
/* ===================================================================*/
void configure_i2c_pins(uint32_t instance);
/*
** ===================================================================
**     Method      :  configure_rtc_pins(component PinSettings)
*/
/*!
**     @brief
**         RTC method sets registers according routing settings. Call
**         this method code to route desired pins into RTC periphery.
**     @param
**         uint32_t instance - RTC instance number (0 is expected)
*/
/* ===================================================================*/

void configure_rtc_pins(uint32_t instance);
/*
** ===================================================================
**     Method      :  configure_uart_pins(component PinSettings)
*/
/*!
**     @brief
**         UART method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         UART0, UART1, UART2, UART3, UART4, UART5
**         peripherals.
**     @param
**         uint32_t instance - UART instance number 0..5
*/
/* ===================================================================*/
void configure_uart_pins(uint32_t instance);
void configure_gpio_i2c_pins(uint32_t instance);
void configure_lpsci_pins(uint32_t instance);
void configure_uart_pins(uint32_t instance);
void configure_spi_pins(uint32_t instance);
void configure_spi_pins_for_modem(uint32_t instance);
void configure_tpm_pins(uint32_t instance);
void configure_cmp_pins(uint32_t instance);
void configure_tsi_pins(uint32_t instance);

/*
** ===================================================================
**     Method      :  configure_dac_pins(component PinSettings)
*/
/*!
**     @brief
**         This function enable analog function for the DAC_OUT
**         pin to be able to achieve the full range of the DAC.
**         
**     @param
**         uint32_t instance - DAC instance number 0
*/
/* ===================================================================*/

void configure_dac_pins(uint32_t instance);

void configure_xcvr_pins(void);

/* END pin_mux. */
#endif /* #ifndef __pin_mux_H_ */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
