/************************************************************************************
* This header file is for LCD Driver Interface.
*
*
* (c) Copyright 2012, Freescale, Inc.  All rights reserved.
*
*
* No part of this document may be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"
//#include "AppToPlatformConfig.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/ 
  #define gLCDSupported_d FALSE

  #define LCD_ClearDisplay()
  #define LCD_Init()
#ifdef __IAR_SYSTEMS_ICC__
  #define LCD_WriteString(line, pstr) //((void)pstr)
#else
  #define LCD_WriteString(line, pstr)
#endif
  #define LCD_WriteBitmap(pstr, len, line, bitmap)
  #define LCD_WriteStringValue(pstr, value, line, display)
  #define LCD_WriteBytes(pstr, value, line, length)
  #define LCD_WriteCommand(command)
  #define LCD_WriteData(data)
  #define LCD_SetFont(font) TRUE
  #define LCD_CheckError()  FALSE
#ifdef __cplusplus
}
#endif
#endif /* _DISPLAY_H_ */
