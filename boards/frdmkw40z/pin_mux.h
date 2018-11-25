/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : pin_mux.h
**     Project     : twrkl25z48m
**     Processor   : MKL25Z128VLK4
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
**         TPM                 - void pin_mux_TPM(uint32_t instance);
**         RTC                 - void pin_mux_RTC(uint32_t instance);
**         SPI                 - void pin_mux_SPI(uint32_t instance);
**         LPUART              - void pin_mux_LPUART(uint32_t instance);
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
**         PTA, PTB, PTC
**         peripherals.
**     @param
**         uint32_t instance - GPIO instance number 0..2
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
**         I2C0, I2C1
**         peripherals.
**     @param
**         uint32_t instance - I2C instance number 0..1
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
**     Method      :  configure_lpuart_pins(component PinSettings)
*/
/*!
**     @brief
**         LPUART method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         LPUART0
**         peripherals.
**     @param
**         uint32_t instance - LPUART instance number (0 is expected)
*/
/* ===================================================================*/
void configure_lpuart_pins(uint32_t instance);

/*
** ===================================================================
**     Method      :  configure_spi_pins (component PinSettings)
*/
/*!
**     @brief
**         SPI method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         SPI0,SPI1
**         peripheral.
**     @param
**         uint32_t instance - SPI instance number 0..1
*/
/* ===================================================================*/
void configure_spi_pins(uint32_t instance);

/*
** ===================================================================
**     Method      :  configure_tpm_pins (component PinSettings)
*/
/*!
**     @brief
**         TPM method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         TMP0,TPM1,TPM2
**         peripheral.
**     @param
**         uint32_t instance - TPM instance number 0..2
*/
/* ===================================================================*/
void configure_tpm_pins(uint32_t instance);

/*
** ===================================================================
**     Method      :  configure_tsi_pins (component PinSettings)
*/
/*!
**     @brief
**         TSI method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         TSI0
**         peripheral.
**     @param
**         uint32_t instance - TSI instance number (0 is expected)
*/
/* ===================================================================*/
void configure_tsi_pins(uint32_t instance);

/*
** ===================================================================
**     Method      :  configure_dac_pins (component PinSettings)
*/
/*!
**     @brief
**         DAC method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         DAC
**         peripheral.
**     @param
**         uint32_t instance - DAC instance number (0 is expected)
*/
/* ===================================================================*/
void configure_dac_pins(uint32_t instance);

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