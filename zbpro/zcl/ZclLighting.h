/*! @file 	ZclLighting.h
 *
 * @brief	Types, definitions and prototypes for lighting domain(also look at ZclGeneral for generic things like
 *			on/off or level Ctrl which is used for dimming lights).
 *
 * @copyright Copyright(c) 2013, Freescale, Inc. All rights reserved.
 *
 * @license	Redistribution and use in source and binary forms, with or without modification,
 *			are permitted provided that the following conditions are met:
 *
 *			o Redistributions of source code must retain the above copyright notice, this list
 *			of conditions and the following disclaimer.
 *
 *			o Redistributions in binary form must reproduce the above copyright notice, this
 *			list of conditions and the following disclaimer in the documentation and/or
 *			other materials provided with the distribution.
 *
 *			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *			contributors may be used to endorse or promote products derived from this
 *			software without specific prior written permission.
 *
 *			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *			ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *			WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 *			ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *			(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *			LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *			ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *			(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *			SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 /* 
 *  [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
 *  [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf 
 *  [R3] - docs-11-0037-10-0Zll-zigbee-light-link-zll-profile specification.pdf 
 */

#ifndef _ZCL_LIGHTING_H
#define _ZCL_LIGHTING_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"

#include "ZclOptions.h"
#include "HaProfile.h"


/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************
	Color Control Cluster
*******************************************/

/* [R3] 6.8.1.1 Color Ctrl Attributes, [R2] - 5.2.2.2.1  */
#if ( TRUE == gBigEndian_c )
/* mandatory attributes (Table 39, [R3])*/
/* color information */
#define gZclAttrColorCtrlCurrentHue_c                0x0000    /* O - Current HUE */
#define gZclAttrColorCtrlCurrentSaturation_c         0x0100    /* O - Current Saturation */
#define gZclAttrColorCtrlRemainingTime_c             0x0200    /* O - Remaining Time */
#define gZclAttrColorCtrlCurrentX_c                  0x0300    /* M - Current X */
#define gZclAttrColorCtrlCurrentY_c                  0x0400    /* M - Current Y */
#if gColorCtrlEnableDriftCompensation_c
#define gZclAttrColorCtrlDriftCompensation_c         0x0500    /* O - Drift Compesation */
#define gZclAttrColorCtrlCompensationText_c          0x0600    /* O - Compesation Text */
#endif
#if gColorCtrlEnableColorTemperature_c
#define gZclAttrColorCtrlColorTemperature_c          0x0700    /* O - Color Temperature */
#endif
#define gZclAttrColorCtrlColorMode_c                 0x0800    /* O - Color Mode */
   
#if gColorCtrlEnablePrimariesInformation_c   
/* primaries information */
#define gZclAttrColorCtrlNumberOfPrimaries_c         0x1000    /* O - Number of Primaries */
#define gZclAttrColorCtrlPrimary1X_c                 0x1100    /* O - Primary 1X */
#define gZclAttrColorCtrlPrimary1Y_c                 0x1200    /* O - Primary 1Y*/
#define gZclAttrColorCtrlPrimary1Intensity_c         0x1300    /* O - Primary 1 Intensity*/
#define gZclAttrColorCtrlPrimary2X_c                 0x1500    /* O - Primary 2X */
#define gZclAttrColorCtrlPrimary2Y_c                 0x1600    /* O - Primary 2Y*/
#define gZclAttrColorCtrlPrimary2Intensity_c         0x1700    /* O - Primary 2 Intensity*/
#define gZclAttrColorCtrlPrimary3X_c                 0x1900    /* O - Primary 3X */
#define gZclAttrColorCtrlPrimary3Y_c                 0x1A00    /* O - Primary 3Y*/
#define gZclAttrColorCtrlPrimary3Intensity_c         0x1B00    /* O - Primary 3 Intensity*/
#if gColorCtrlEnablePrimariesAdditionalInf_c
#define gZclAttrColorCtrlPrimary4X_c                 0x2000    /* O - Primary 4X */
#define gZclAttrColorCtrlPrimary4Y_c                 0x2100    /* O - Primary 4Y*/
#define gZclAttrColorCtrlPrimary4Intensity_c         0x2200    /* O - Primary 4 Intensity*/
#define gZclAttrColorCtrlPrimary5X_c                 0x2400    /* O - Primary 5X */
#define gZclAttrColorCtrlPrimary5Y_c                 0x2500    /* O - Primary 5Y*/
#define gZclAttrColorCtrlPrimary5Intensity_c         0x2600    /* O - Primary 5 Intensity*/
#define gZclAttrColorCtrlPrimary6X_c                 0x2800    /* O - Primary 6X */
#define gZclAttrColorCtrlPrimary6Y_c                 0x2900    /* O - Primary 6Y*/
#define gZclAttrColorCtrlPrimary6Intensity_c         0x2A00    /* O - Primary 6 Intensity*/
#endif
#endif /* gColorCtrlEnablePrimariesInformation_c */   
   
#if gColorCtrlEnableColorPointSettings_c
/* color point settings */
#define gZclAttrColorCtrlWhitePointX_c               0x3000    /* O - White Point X*/
#define gZclAttrColorCtrlWhitePointY_c               0x3100    /* O - White Point Y*/
#define gZclAttrColorCtrlColorPointRX_c              0x3200    /* O - Color Point Rx*/
#define gZclAttrColorCtrlColorPointRY_c              0x3300    /* O - Color Point Ry*/
#define gZclAttrColorCtrlColorPointRIntensity_c      0x3400    /* O - Color Point R Intensity */
#define gZclAttrColorCtrlColorPointGX_c              0x3600    /* O - Color Point Gx*/
#define gZclAttrColorCtrlColorPointGY_c              0x3700    /* O - Color Point Gy*/
#define gZclAttrColorCtrlColorPointGIntensity_c      0x3800    /* O - Color Point G Intensity */
#define gZclAttrColorCtrlColorPointBX_c              0x3600    /* O - Color Point Bx*/
#define gZclAttrColorCtrlColorPointBY_c              0x3700    /* O - Color Point By*/
#define gZclAttrColorCtrlColorPointBIntensity_c      0x3800    /* O - Color Point B Intensity */
#endif /* gColorCtrlEnableColorPointSettings_c */
   
/* aditional attributes (Table 40, [R3])*/
#define gZclAttrColorCtrlEnhancedCurrentHue_c        0x0040    /* O - Enhanced Current HUE */
#define gZclAttrColorCtrlEnhancedColorMode_c         0x0140    /* O - Enhanced Color Mode */
#define gZclAttrColorCtrlColorLoopActive_c            0x0240    /* O - Color Loop Active */
#define gZclAttrColorCtrlColorLoopDirection_c        0x0340    /* O - Color Loop Direction */
#define gZclAttrColorCtrlColorLoopTime_c             0x0440    /* O - Color Loop Time */
#define gZclAttrColorCtrlColorLoopStartEnhancedHue_c     0x0540    /* O - Color Loop Start Enhanced HUE */
#define gZclAttrColorCtrlColorLoopStoredEnhancedHue_c    0x0640    /* O - Color Loop Stored Enhanced HUE */
#define gZclAttrColorCtrlColorCapabilities_c         0x0A40    /* O - Color Capabilities*/
#if gColorCtrlEnableColorTemperature_c
#define gZclAttrColorCtrlColorTempPhysicalMin_c      0x0B40    /* O - Color Temp Physical Min*/
#define gZclAttrColorCtrlColorTempPhysicalMax_c      0x0C40    /* O - Color Temp Physical Max*/
#endif

#else
/* mandatory attributes (Table 39, [R3])*/
/* color information */
#define gZclAttrColorCtrlCurrentHue_c                0x0000    /* O - Current HUE */
#define gZclAttrColorCtrlCurrentSaturation_c         0x0001    /* O - Current Saturation */
#define gZclAttrColorCtrlRemainingTime_c             0x0002    /* O - Remaining Time */
#define gZclAttrColorCtrlCurrentX_c                  0x0003    /* M - Current X */
#define gZclAttrColorCtrlCurrentY_c                  0x0004    /* M - Current Y */
#define gZclAttrColorCtrlDriftCompensation_c         0x0005    /* O - Drift Compesation */
#define gZclAttrColorCtrlCompensationText_c          0x0006    /* O - Compesation Text */
#define gZclAttrColorCtrlColorTemperature_c          0x0007    /* O - Color Temperature */
#define gZclAttrColorCtrlColorMode_c                 0x0008    /* O - Color Mode */
   
#if gColorCtrlEnablePrimariesInformation_c  
/* primaries information */
#define gZclAttrColorCtrlNumberOfPrimaries_c         0x0010    /* O - Number of Primaries */
#define gZclAttrColorCtrlPrimary1X_c                 0x0011    /* O - Primary 1X */
#define gZclAttrColorCtrlPrimary1Y_c                 0x0012    /* O - Primary 1Y*/
#define gZclAttrColorCtrlPrimary1Intensity_c         0x0013    /* O - Primary 1 Intensity*/
#define gZclAttrColorCtrlPrimary2X_c                 0x0015    /* O - Primary 2X */
#define gZclAttrColorCtrlPrimary2Y_c                 0x0016    /* O - Primary 2Y*/
#define gZclAttrColorCtrlPrimary2Intensity_c         0x0017    /* O - Primary 2 Intensity*/
#define gZclAttrColorCtrlPrimary3X_c                 0x0019    /* O - Primary 3X */
#define gZclAttrColorCtrlPrimary3Y_c                 0x001A    /* O - Primary 3Y*/
#define gZclAttrColorCtrlPrimary3Intensity_c         0x001B    /* O - Primary 3 Intensity*/
#define gZclAttrColorCtrlPrimary4X_c                 0x0020    /* O - Primary 4X */
#define gZclAttrColorCtrlPrimary4Y_c                 0x0021    /* O - Primary 4Y*/
#define gZclAttrColorCtrlPrimary4Intensity_c         0x0022    /* O - Primary 4 Intensity*/
#define gZclAttrColorCtrlPrimary5X_c                 0x0024    /* O - Primary 5X */
#define gZclAttrColorCtrlPrimary5Y_c                 0x0025    /* O - Primary 5Y*/
#define gZclAttrColorCtrlPrimary5Intensity_c         0x0026    /* O - Primary 5 Intensity*/
#define gZclAttrColorCtrlPrimary6X_c                 0x0028    /* O - Primary 6X */
#define gZclAttrColorCtrlPrimary6Y_c                 0x0029    /* O - Primary 6Y*/
#define gZclAttrColorCtrlPrimary6Intensity_c         0x002A    /* O - Primary 6 Intensity*/
#endif /* gColorCtrlEnablePrimariesInformation_c  */   
   
#if gColorCtrlEnableColorPointSettings_c
/* color point settings */
#define gZclAttrColorCtrlWhitePointX_c               0x0030    /* O - White Point X*/
#define gZclAttrColorCtrlWhitePointY_c               0x0031    /* O - White Point Y*/
#define gZclAttrColorCtrlColorPointRX_c              0x0032    /* O - Color Point Rx*/
#define gZclAttrColorCtrlColorPointRY_c              0x0033    /* O - Color Point Ry*/
#define gZclAttrColorCtrlColorPointRIntensity_c      0x0034    /* O - Color Point R Intensity */
#define gZclAttrColorCtrlColorPointGX_c              0x0036    /* O - Color Point Gx*/
#define gZclAttrColorCtrlColorPointGY_c              0x0037    /* O - Color Point Gy*/
#define gZclAttrColorCtrlColorPointGIntensity_c      0x0038    /* O - Color Point G Intensity */
#define gZclAttrColorCtrlColorPointBX_c              0x003A    /* O - Color Point Bx*/
#define gZclAttrColorCtrlColorPointBY_c              0x003B    /* O - Color Point By*/
#define gZclAttrColorCtrlColorPointBIntensity_c      0x003C    /* O - Color Point B Intensity */
#endif /* gColorCtrlEnableColorPointSettings_c */

/* aditional attributes (Table 40, [R3])*/
#define gZclAttrColorCtrlEnhancedCurrentHue_c        0x4000    /* O - Enhanced Current HUE */
#define gZclAttrColorCtrlEnhancedColorMode_c         0x4001    /* O - Enhanced Color Mode */
#define gZclAttrColorCtrlColorLoopActive_c           0x4002    /* O - Color Loop Active */
#define gZclAttrColorCtrlColorLoopDirection_c        0x4003    /* O - Color Loop Direction */
#define gZclAttrColorCtrlColorLoopTime_c             0x4004    /* O - Color Loop Time */
#define gZclAttrColorCtrlColorLoopStartEnhancedHue_c         0x4005    /* O - Color Loop Start Enhanced HUE */
#define gZclAttrColorCtrlColorLoopStoredEnhancedHue_c        0x4006    /* O - Color Loop Stored Enhanced HUE */
#define gZclAttrColorCtrlColorCapabilities_c         0x400A    /* O - Color Capabilities*/
#define gZclAttrColorCtrlColorTempPhysicalMin_c      0x400B    /* O - Color Temp Physical Min*/
#define gZclAttrColorCtrlColorTempPhysicalMax_c      0x400C    /* O - Color Temp Physical Max*/
   
#endif 

/* color color - Color information attr set */
#define gZclAttrColorCtrl_ColorInformationSet_c        0x00    /* color information Set */      
#define gZclAttrIdColorCtrlCurrentHue_c                0x00    /* O - Current HUE */
#define gZclAttrIdColorCtrlCurrentSaturation_c         0x01    /* O - Current Saturation */
#define gZclAttrIdColorCtrlRemainingTime_c             0x02    /* O - Remaining Time */
#define gZclAttrIdColorCtrlCurrentX_c                  0x03    /* M - Current X */
#define gZclAttrIdColorCtrlCurrentY_c                  0x04    /* M - Current Y */
#define gZclAttrIdColorCtrlDriftCompensation_c         0x05    /* O - Drift Compesation */
#define gZclAttrIdColorCtrlCompensationText_c          0x06    /* O - Compesation Text */
#define gZclAttrIdColorCtrlColorTemperature_c          0x07    /* O - Color Temperature */
#define gZclAttrIdColorCtrlColorMode_c                 0x08    /* O - Color Mode */
   
#if gColorCtrlEnablePrimariesInformation_c 
 /* primaries information */
#define gZclAttrIdColorCtrlNumberOfPrimaries_c         0x10    /* O - Number of Primaries */
#define gZclAttrIdColorCtrlPrimary1X_c                 0x11    /* O - Primary 1X */
#define gZclAttrIdColorCtrlPrimary1Y_c                 0x12    /* O - Primary 1Y*/
#define gZclAttrIdColorCtrlPrimary1Intensity_c         0x13    /* O - Primary 1 Intensity*/
#define gZclAttrIdColorCtrlPrimary2X_c                 0x15    /* O - Primary 2X */
#define gZclAttrIdColorCtrlPrimary2Y_c                 0x16    /* O - Primary 2Y*/
#define gZclAttrIdColorCtrlPrimary2Intensity_c         0x17    /* O - Primary 2 Intensity*/
#define gZclAttrIdColorCtrlPrimary3X_c                 0x19    /* O - Primary 3X */
#define gZclAttrIdColorCtrlPrimary3Y_c                 0x1A    /* O - Primary 3Y*/
#define gZclAttrIdColorCtrlPrimary3Intensity_c         0x1B    /* O - Primary 3 Intensity*/
#define gZclAttrIdColorCtrlPrimary4X_c                 0x20    /* O - Primary 4X */
#define gZclAttrIdColorCtrlPrimary4Y_c                 0x21    /* O - Primary 4Y*/
#define gZclAttrIdColorCtrlPrimary4Intensity_c         0x22    /* O - Primary 4 Intensity*/
#define gZclAttrIdColorCtrlPrimary5X_c                 0x24    /* O - Primary 5X */
#define gZclAttrIdColorCtrlPrimary5Y_c                 0x25    /* O - Primary 5Y*/
#define gZclAttrIdColorCtrlPrimary5Intensity_c         0x26    /* O - Primary 5 Intensity*/
#define gZclAttrIdColorCtrlPrimary6X_c                 0x28    /* O - Primary 6X */
#define gZclAttrIdColorCtrlPrimary6Y_c                 0x29    /* O - Primary 6Y*/
#define gZclAttrIdColorCtrlPrimary6Intensity_c         0x2A    /* O - Primary 6 Intensity*/
#endif /* gColorCtrlEnablePrimariesInformation_c */
   
#if gColorCtrlEnableColorPointSettings_c
/* color point settings */
#define gZclAttrIdColorCtrlWhitePointX_c               0x30    /* O - White Point X*/
#define gZclAttrIdColorCtrlWhitePointY_c               0x31    /* O - White Point Y*/
#define gZclAttrIdColorCtrlColorPointRX_c              0x32    /* O - Color Point Rx*/
#define gZclAttrIdColorCtrlColorPointRY_c              0x33    /* O - Color Point Ry*/
#define gZclAttrIdColorCtrlColorPointRIntensity_c      0x34    /* O - Color Point R Intensity */
#define gZclAttrIdColorCtrlColorPointGX_c              0x36    /* O - Color Point Gx*/
#define gZclAttrIdColorCtrlColorPointGY_c              0x37    /* O - Color Point Gy*/
#define gZclAttrIdColorCtrlColorPointGIntensity_c      0x38    /* O - Color Point G Intensity */
#define gZclAttrIdColorCtrlColorPointBX_c              0x36    /* O - Color Point Bx*/
#define gZclAttrIdColorCtrlColorPointBY_c              0x37    /* O - Color Point By*/
#define gZclAttrIdColorCtrlColorPointBIntensity_c      0x38    /* O - Color Point B Intensity */
#endif /* gColorCtrlEnableColorPointSettings_c */

#if gColorCtrlEnableZllFunctionality_c
/* ZLL aditional attributes (Table 40, [R3])*/
#define gZclAttrColorCtrl_ZllAditionalSet_c            0x40    /* zll aditional attributes ID*/
#define gZclAttrIdColorCtrlEnhancedCurrentHue_c        0x00    /* O - Enhanced Current HUE */
#define gZclAttrIdColorCtrlEnhancedColorMode_c         0x01    /* O - Enhanced Color Mode */
#define gZclAttrIdColorCtrlColorLoopActive_c           0x02    /* O - Color Loop Active */
#define gZclAttrIdColorCtrlColorLoopDirection_c        0x03    /* O - Color Loop Direction */
#define gZclAttrIdColorCtrlColorLoopTime_c             0x04    /* O - Color Loop Time */
#define gZclAttrIdColorCtrlColorLoopStartEnhancedHue_c     0x05    /* O - Color Loop Start Enhanced HUE */
#define gZclAttrIdColorCtrlColorLoopStoredEnhancedHue_c    0x06    /* O - Color Loop Stored Enhanced HUE */
#define gZclAttrIdColorCtrlColorCapabilities_c         0x0A    /* O - Color Capabilities*/
#define gZclAttrIdColorCtrlColorTempPhysicalMin_c      0x0B    /* O - Color Temp Physical Min*/
#define gZclAttrIdColorCtrlColorTempPhysicalMax_c      0x0C    /* O - Color Temp Physical Max*/
#endif /* gColorCtrlEnableZllFunctionality_c */

#define gColorCtrlSaturationMaxValue_c       0xFE
#define gColorCtrlSaturationMinValue_c       0x00

#if ( TRUE != gBigEndian_c )
#define gColorCtrlHueMaxValue_c              0x00FE
#define gColorCtrlHueMinValue_c              0x0000
#define gColorCtrlColorMaxValue_c            0xFEFF
#define gColorCtrlColorMinValue_c            0x0000
#define gColorCtrlEnhancedHueMaxValue_c      0xFEFF
#define gColorCtrlEnhancedHueMinValue_c      0x0000

#define gColorCtrlDefaultWhiteX_c            0x55CD        
#define gColorCtrlDefaultWhiteY_c            0x58F9
#define gColorCtrlDefaultRedX_c              0xA0F0        
#define gColorCtrlDefaultRedY_c              0x521D
#define gColorCtrlDefaultGreenX_c            0x4FB8        
#define gColorCtrlDefaultGreenY_c            0x9461
#define gColorCtrlDefaultBlueX_c             0x26B0       
#define gColorCtrlDefaultBlueY_c             0x1064

#else
#define gColorCtrlHueMaxValue_c              0xFE00
#define gColorCtrlHueMinValue_c              0x0000
#define gColorCtrlColorMaxValue_c            0xFFFE
#define gColorCtrlColorMinValue_c            0x0000
#define gColorCtrlEnhancedHueMaxValue_c      0xFFFE
#define gColorCtrlEnhancedHueMinValue_c      0x0000

#define gColorCtrlDefaultWhiteX_c            0xCD55        
#define gColorCtrlDefaultWhiteY_c            0xF958
#define gColorCtrlDefaultRedX_c              0xF0A0        
#define gColorCtrlDefaultRedY_c              0x1D52
#define gColorCtrlDefaultGreenX_c            0xB84F        
#define gColorCtrlDefaultGreenY_c            0x6194
#define gColorCtrlDefaultBlueX_c             0xB026       
#define gColorCtrlDefaultBlueY_c             0x6410
#endif

/* drift compensation attribute values: */
enum{
 gColorCtrlDriftCompensation_None_c = 0x00,
 gColorCtrlDriftCompensation_Unknown_c,
 gColorCtrlDriftCompensation_TempMonitoring_c,
 gColorCtrlDriftCompensation_OpticalLuminanceMonitoring_c,
 gColorCtrlDriftCompensation_OpticalColorMonitoring_c
   /* 0x05 - 0xFF reserved */
};

/* color mode attribute values: */
enum{
 gColorCtrlColorMode_CurrentModeSaturation_c = 0x00,
 gColorCtrlColorMode_CurrentXY_c,
 gColorCtrlColorMode_ColorTemperature_c
   /* 0x03 - 0xFF reserved */
};

/* enhanced ColorMode attribute values: */
enum{
 gColorCtrlEnhancedColorMode_CurrentModeSaturation_c = 0x00,
 gColorCtrlEnhancedColorMode_CurrentXY_c,
 gColorCtrlEnhancedColorMode_ColorTemperature_c,
 gColorCtrlEnhancedColorMode_EnhHueSaturation_c,
   /* 0x04 - 0xFF reserved */
};

/* color Ctrl capabilities attribute (bitmap) */
typedef PACKED_STRUCT gColorCtrlCapabilities_tag
{
    uint16_t hueSaturationSupported     :1;                                 
    uint16_t enhancedHueSupported       :1;                                 
    uint16_t colorLoopSupported         :1;  
    uint16_t xyAttributeSupported       :1;
    uint16_t colorTemperatureSupported  :1;
    uint16_t reserved                   :11;
}gColorCtrlCapabilities_t;     


/* color Ctrl attr RAM structure*/
typedef PACKED_STRUCT zclColorCtrlAttrsRAM_tag
{
  uint8_t       currentHue[zclReportableCopies_c];
  uint8_t       currentSaturation[zclReportableCopies_c];
  uint16_t      remainingTime;
  uint16_t      currentX[zclReportableCopies_c];
  uint16_t      currentY[zclReportableCopies_c];
#if gColorCtrlEnableDriftCompensation_c   
  uint8_t       driftCompensation;
  zclStr16Oct_t compensationText;
#endif  
#if gColorCtrlEnableColorTemperature_c  
  uint16_t      colorTemperature[zclReportableCopies_c];
#endif  
  uint8_t       colorMode;
#if gColorCtrlEnablePrimariesInformation_c   
  uint8_t       noOfPrimaries;
  uint16_t      primary1X;
  uint16_t      primary1Y;
  uint8_t       primary1Intensity;
  uint16_t      primary2X;
  uint16_t      primary2Y;
  uint8_t       primary2Intensity;
  uint16_t      primary3X;
  uint16_t      primary3Y;
  uint8_t       primary3Intensity;
#if gColorCtrlEnablePrimariesAdditionalInf_c  
  uint16_t      primary4X;
  uint16_t      primary4Y;
  uint8_t       primary4Intensity; 
  uint16_t      primary5X;
  uint16_t      primary5Y;
  uint8_t       primary5Intensity;
  uint16_t      primary6X;
  uint16_t      primary6Y;
  uint8_t       primary6Intensity;  
#endif /* gColorCtrlEnablePrimariesAdditionalInf_c*/  
#endif /* gColorCtrlEnablePrimariesInformation_c */  
#if gColorCtrlEnableColorPointSettings_c  
  uint16_t      whitePointX;
  uint16_t      whitePointY;
  uint16_t      colorPointRx;
  uint16_t      colorPointRy;
  uint8_t       colorPointRIntensity; 
  uint16_t      colorPointGx;
  uint16_t      colorPointGy;
  uint8_t       colorPointGIntensity;   
  uint16_t      colorPointBx;
  uint16_t      colorPointBy;
  uint8_t       colorPointBIntensity;   
#endif /* gColorCtrlEnableColorPointSettings_c */ 
#if gColorCtrlEnableZllFunctionality_c  
  uint16_t      enhancedCurrentHue;
  uint8_t       enhancedColorMode;
  uint8_t       colorLoopActive;
  uint8_t       colorLoopDirection;
  uint16_t      colorLoopTime;
  uint16_t      colorLoopStartEnhancedHue;
  uint16_t      colorLoopStoredEnhancedHue;
  gColorCtrlCapabilities_t   colorCapabilities;
#if gColorCtrlEnableColorTemperature_c   
  uint16_t      colorTempPhysicalMin;
  uint16_t      colorTempPhysicalMax;
#endif  
#endif /* gColorCtrlEnableZllFunctionality_c */  
}zclColorCtrlAttrsRAM_t;

/* [R2] 5.2.2.3 Commands received */
#define gZclCmdColorCtrl_MoveToHue_c                    0x00    /* O - move to hue*/
#define gZclCmdColorCtrl_MoveHue_c                      0x01    /* O - move hue - not certifiable [R3] - Table 10.7 */
#define gZclCmdColorCtrl_StepHue_c                      0x02    /* O - step hue*/
#define gZclCmdColorCtrl_MoveToSaturation_c             0x03    /* O - move to saturation */
#define gZclCmdColorCtrl_MoveSaturation_c               0x04    /* O - move saturation - not certifiable [R3] - Table 10.7 */
#define gZclCmdColorCtrl_StepSaturation_c               0x05    /* O - step saturation */
#define gZclCmdColorCtrl_MoveToHueSaturation_c          0x06    /* O - move to hue and saturation - not certifiable [R3] - Table 10.7 */
#define gZclCmdColorCtrl_MoveToColor_c                  0x07    /* M - move to color */
#define gZclCmdColorCtrl_MoveColor_c                    0x08    /* M - move color */
#define gZclCmdColorCtrl_StepColor_c                    0x09    /* M - Step color - not certifiable [R3] - Table 10.7 */
#define gZclCmdColorCtrl_MoveToColorTemperature_c       0x0A    /* O - move to color temperature - not certifiable [R3] - Table 10.7 */

/* [R3] 6.8.1.3 Table 45 */
#define gZclCmdColorCtrl_EnhancedMoveToHue_c            0x40    /* O - enhanced move to hue */
#define gZclCmdColorCtrl_EnhancedMoveHue_c              0x41    /* O - enhanced move hue - not certifiable [R3] - Table 10.7 */
#define gZclCmdColorCtrl_EnhancedStepHue_c              0x42    /* O - enhanced step hue */
#define gZclCmdColorCtrl_EnhancedMoveToHueSat_c         0x43    /* O - enhanced move to hue and saturation */
#define gZclCmdColorCtrl_ColorLoopSet_c                 0x44    /* O - color loop set */
#define gZclCmdColorCtrl_StopMoveStep_c                 0x47    /* O - stop move step */
#define gZclCmdColorCtrl_MoveColorTemperature_c         0x4B    /* O - move color temperature - not certifiable [R3] - Table 10.7 */
#define gZclCmdColorCtrl_StepColorTemperature_c         0x4C    /* O - step color temperature - not certifiable [R3] - Table 10.7 */

/* [R2] 5.2.2.3.2 payload format for Move to HUE Command */
typedef PACKED_STRUCT zclCmdColorCtrl_MoveToHue_tag
{
  uint8_t       hue;
  uint8_t       direction;
  uint16_t      transitionTime;
}zclCmdColorCtrl_MoveToHue_t;

/* direction field - values */
enum
{
  gColorCtrlDirection_ShortestDistance_c = 0x00,
  gColorCtrlDirection_LongestDistance_c,
  gColorCtrlDirection_Up_c,
  gColorCtrlDirection_Down_c
      /* 0x04-0xFF reserved */
};

/* [R2] 5.2.2.3.2 Move to HUE Command */
typedef PACKED_STRUCT zclColorCtrl_MoveToHue_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_MoveToHue_t        cmdFrame;
}zclColorCtrl_MoveToHue_t;

/* [R2] 5.2.2.3.3 payload format for Move HUE Command */
typedef PACKED_STRUCT zclCmdColorCtrl_MoveHue_tag
{
  uint8_t       moveMode;
  uint8_t       rate;
}zclCmdColorCtrl_MoveHue_t;

/* move mode field - values */
enum
{
  gColorCtrlMoveMode_Stop_c = 0x00,
  gColorCtrlMoveMode_Up_c,
  gColorCtrlMoveMode_Reserved,
  gColorCtrlMoveMode_Down_c,
      /* 0x04-0xFF reserved */
};

/* [R2] 5.2.2.3.3 Move HUE Command */
typedef PACKED_STRUCT zclColorCtrl_MoveHue_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_MoveHue_t        cmdFrame;
}zclColorCtrl_MoveHue_t;

/* [R2] 5.2.2.3.4 payload format for Step HUE Command */
typedef PACKED_STRUCT zclCmdColorCtrl_StepHue_tag
{
  uint8_t       stepMode;
  uint8_t       stepSize;
  uint8_t       transitionTime;
}zclCmdColorCtrl_StepHue_t;

/* step mode field - values */
enum
{
  gColorCtrlStepMode_Reserved1_c = 0x00,
  gColorCtrlStepMode_Up_c,
  gColorCtrlStepMode_Reserved2,
  gColorCtrlStepMode_Down_c,
      /* 0x04-0xFF reserved */
};

/* [R2] 5.2.2.3.4 Step HUE Command */
typedef PACKED_STRUCT zclColorCtrl_StepHue_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_StepHue_t        cmdFrame;
}zclColorCtrl_StepHue_t;

/* [R2] 5.2.2.3.5 payload format for Move to Saturation Command */
typedef PACKED_STRUCT zclCmdColorCtrl_MoveToSaturation_tag
{
  uint8_t       saturation;
  uint16_t      transitionTime;
}zclCmdColorCtrl_MoveToSaturation_t;

/* [R2] 5.2.2.3.5 Move to Saturation Command */
typedef PACKED_STRUCT zclColorCtrl_MoveToSaturation_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_MoveToSaturation_t        cmdFrame;
}zclColorCtrl_MoveToSaturation_t;

/* [R2] 5.2.2.3.6 payload format for Move Saturation Command */
typedef PACKED_STRUCT zclCmdColorCtrl_MoveSaturation_tag
{
  uint8_t       moveMode;
  uint8_t       rate;
}zclCmdColorCtrl_MoveSaturation_t;

/* [R2] 5.2.2.3.6 Move  Saturation Command */
typedef PACKED_STRUCT zclColorCtrl_MoveSaturation_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_MoveSaturation_t        cmdFrame;
}zclColorCtrl_MoveSaturation_t;

/* [R2] 5.2.2.3.7 payload format for Step Saturation Command */
typedef PACKED_STRUCT zclCmdColorCtrl_StepSaturation_tag
{
  uint8_t       stepMode;
  uint8_t       stepSize;
  uint8_t       transitionTime;
}zclCmdColorCtrl_StepSaturation_t;

/* [R2] 5.2.2.3.7 Step  Saturation Command */
typedef PACKED_STRUCT zclColorCtrl_StepSaturation_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_StepSaturation_t        cmdFrame;
}zclColorCtrl_StepSaturation_t;

/* [R2] 5.2.2.3.8 payload format for Move to Hue and Saturation Command */
typedef PACKED_STRUCT zclCmdColorCtrl_MoveToHueSaturation_tag
{
  uint8_t       hue;
  uint8_t       saturation;
  uint16_t      transitionTime;
}zclCmdColorCtrl_MoveToHueSaturation_t;

/* [R2] 5.2.2.3.8 Move to Hue and  Saturation Command */
typedef PACKED_STRUCT zclColorCtrl_MoveToHueSaturation_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_MoveToHueSaturation_t        cmdFrame;
}zclColorCtrl_MoveToHueSaturation_t;

/* [R2] 5.2.2.3.9 payload format for Move to Color Command */
typedef PACKED_STRUCT zclCmdColorCtrl_MoveToColor_tag
{
  uint16_t      colorX;
  uint16_t      colorY;
  uint16_t      transitionTime;
}zclCmdColorCtrl_MoveToColor_t;

/* [R2] 5.2.2.3.9 Move to Color Command */
typedef PACKED_STRUCT zclColorCtrl_MoveToColor_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_MoveToColor_t        cmdFrame;
}zclColorCtrl_MoveToColor_t;

/* [R2] 5.2.2.3.10 payload format for Move Color Command */
typedef PACKED_STRUCT zclCmdColorCtrl_MoveColor_tag
{
  int16_t      rateX;
  int16_t      rateY;
}zclCmdColorCtrl_MoveColor_t;

/* [R2] 5.2.2.3.10 Move Color Command */
typedef PACKED_STRUCT zclColorCtrl_MoveColor_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_MoveColor_t        cmdFrame;
}zclColorCtrl_MoveColor_t;


/* [R2] 5.2.2.3.11 payload format for Step Color Command */
typedef PACKED_STRUCT zclCmdColorCtrl_StepColor_tag
{
  uint16_t      stepX;
  uint16_t      stepY;
  uint16_t      transitionTime;
}zclCmdColorCtrl_StepColor_t;

/* [R2] 5.2.2.3.11 Step Color Command */
typedef PACKED_STRUCT zclColorCtrl_StepColor_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_StepColor_t        cmdFrame;
}zclColorCtrl_StepColor_t;

/* [R2] 5.2.2.3.12 payload format for Move To Color Temperature Command */
typedef PACKED_STRUCT zclCmdColorCtrl_MoveToColorTemperature_tag
{
  uint16_t      colorTemperature;
  uint16_t      transitionTime;
}zclCmdColorCtrl_MoveToColorTemperature_t;

/* [R2] 5.2.2.3.12 Move To Color Temperature Command */
typedef PACKED_STRUCT zclColorCtrl_MoveToColorTemperature_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_MoveToColorTemperature_t        cmdFrame;
}zclColorCtrl_MoveToColorTemperature_t;

/* [R3] 6.8.1.3.2 payload format for Enhanced Move to HUE Command */
typedef PACKED_STRUCT zclCmdColorCtrl_EnhancedMoveToHue_tag
{
  uint16_t      enhancedHue;
  uint8_t       direction;
  uint16_t      transitionTime;
}zclCmdColorCtrl_EnhancedMoveToHue_t;

/* [R3] 6.8.1.3.2 Enhanced Move to HUE Command */
typedef PACKED_STRUCT zclColorCtrl_EnhancedMoveToHue_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_EnhancedMoveToHue_t        cmdFrame;
}zclColorCtrl_EnhancedMoveToHue_t;

/* [R3] 6.8.1.3.3 payload format for Enhanced Move HUE Command */
typedef PACKED_STRUCT zclCmdColorCtrl_EnhancedMoveHue_tag
{
  uint8_t       moveMode;
  uint16_t      rate;
}zclCmdColorCtrl_EnhancedMoveHue_t;

/* [R3] 6.8.1.3.3 Enhanced Move HUE Command */
typedef PACKED_STRUCT zclColorCtrl_EnhancedMoveHue_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_EnhancedMoveHue_t        cmdFrame;
}zclColorCtrl_EnhancedMoveHue_t;

/* [R3] 6.8.1.3.4  payload format for Enhanced Step HUE Command */
typedef PACKED_STRUCT zclCmdColorCtrl_EnhancedStepHue_tag
{
  uint8_t       stepMode;
  uint16_t      stepSize;
  uint16_t      transitionTime;
}zclCmdColorCtrl_EnhancedStepHue_t;

/* [R3] 6.8.1.3.4 Enhanced Step HUE Command */
typedef PACKED_STRUCT zclColorCtrl_EnhancedStepHue_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_EnhancedStepHue_t        cmdFrame;
}zclColorCtrl_EnhancedStepHue_t;

/* [R3] 6.8.1.3.5  payload format for Enhanced Move to Hue and Saturation Command */
typedef PACKED_STRUCT zclCmdColorCtrl_EnhancedMoveToHueSaturation_tag
{
  uint16_t      enhancedHue;
  uint8_t       saturation;
  uint16_t      transitionTime;
}zclCmdColorCtrl_EnhancedMoveToHueSaturation_t;

/* [R3] 6.8.1.3.5 Enhanced Move to Hue and  Saturation Command */
typedef PACKED_STRUCT zclColorCtrl_EnhancedMoveToHueSaturation_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_EnhancedMoveToHueSaturation_t        cmdFrame;
}zclColorCtrl_EnhancedMoveToHueSaturation_t;

/* update flags field (bitmap) */
typedef PACKED_STRUCT gColorCtrlUpdateFlags_tag
{
    uint8_t updateAction        :1;                                 
    uint8_t updateDirection     :1;                                 
    uint8_t updateTime          :1;  
    uint8_t updateStartHue      :1;
    uint8_t reserved            :4;
}gColorCtrlUpdateFlags_t; 

/* [R3] 6.8.1.3.6  payload format for Color Loop Set Command */
typedef PACKED_STRUCT zclCmdColorCtrl_ColorLoopSet_tag
{
  gColorCtrlUpdateFlags_t       updateFlags;
  uint8_t       action;
  uint8_t       direction;
  uint16_t      time;
  uint16_t      startHue;
}zclCmdColorCtrl_ColorLoopSet_t;

/* action field values: */
enum{
  gColorCtrlAction_DeactivateColorLoop_c = 0x00,
  gColorCtrlAction_ActivateColorLoopUsingStartHue_c,
  gColorCtrlAction_ActivateColorLoopUsingEnhancedCurrentHue_c
    /* 0x03 - 0xFF reserved */
};

/*direction field values */
enum{
  gColorCtrlDirection_DecrementHue_c = 0x00,
  gColorCtrlDirection_IncrementHue_c,
    /* 0x02 - 0xFF reserved */
};

/* [R3] 6.8.1.3.6 Color Loop set Command */
typedef PACKED_STRUCT zclColorCtrl_ColorLoopSet_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_ColorLoopSet_t        cmdFrame;
}zclColorCtrl_ColorLoopSet_t;

/* [R3] 6.8.1.3.7 Stop Move Step Command */
typedef PACKED_STRUCT zclColorCtrl_StopMoveStep_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
}zclColorCtrl_StopMoveStep_t;

/* [R3] 6.8.1.3.8  payload format for Move color temperature Command  */
typedef PACKED_STRUCT zclCmdColorCtrl_MoveColorTemperature_tag
{
  uint8_t       moveMode;
  uint16_t      rate;
  uint16_t      colorTempMin;
  uint16_t      colorTempMax;
}zclCmdColorCtrl_MoveColorTemperature_t;

/* [R3] 6.8.1.3.8 Move Color Temperature Command */
typedef PACKED_STRUCT zclColorCtrl_MoveColorTemperature_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_MoveColorTemperature_t        cmdFrame;
}zclColorCtrl_MoveColorTemperature_t;

/* [R3] 6.8.1.3.9  payload format for Step color temperature Command  */
typedef PACKED_STRUCT zclCmdColorCtrl_StepColorTemperature_tag
{
  uint8_t       stepMode;
  uint16_t      stepSize;
  uint16_t      transitionTime;
  uint16_t      colorTempMin;
  uint16_t      colorTempMax;
}zclCmdColorCtrl_StepColorTemperature_t;

/* [R3] 6.8.1.3.9 Step Color Temperature Command */
typedef PACKED_STRUCT zclColorCtrl_StepColorTemperature_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t       zclTransactionId;
  zclCmdColorCtrl_StepColorTemperature_t        cmdFrame;
}zclColorCtrl_StepColorTemperature_t;

  
/* color Ctrl setup - keep a copy for each color Ctrl instance */
typedef PACKED_STRUCT zclColorCtrlServerSetup_tag
{
  zbEndPoint_t  endpoint;               /* endpoint */
  uint8_t       colorTransitionTmrId;   /* transition timer*/
  /* hue data */
  uint8_t       hueDirection;           /* hue direction: up , down */
  uint32_t      hueTransitionTime;      /* hue transistion time */
  uint32_t      hueReqValue;            /* hue required Value */
  /* saturation data */
  uint16_t      satTransitionTime;      /* saturation transistion time */ 
  uint32_t      satReqValue;            /* satuation required Value */
  /* colorXY data */
  uint32_t      colorxReqValue;         /* colorX required Value */
  uint32_t      coloryReqValue;         /* colorX required Value */
  uint32_t      colorxTransitionTime;   /* colorX transistion time */ 
  uint32_t      coloryTransitionTime;   /* colorX transistion time */ 
}zclColorCtrlServerSetup_t;

/* move to Hue data*/
typedef PACKED_STRUCT zclColorCtrl_MoveToHueData_tag
{
  PACKED_UNION {
    zclCmdColorCtrl_MoveToHue_t moveToHue;   
    zclCmdColorCtrl_EnhancedMoveToHue_t enhMoveToHue;
  } msgData;
}zclColorCtrl_MoveToHueData_t;

/* step Hue data*/
typedef PACKED_STRUCT zclColorCtrl_StepHueData_tag
{
  PACKED_UNION {
    zclCmdColorCtrl_StepHue_t stepHue;   
    zclCmdColorCtrl_EnhancedStepHue_t enhStepHue;
  } msgData;
}zclColorCtrl_StepHueData_t;

/* move to hue saturation data*/
typedef PACKED_STRUCT zclColorCtrl_MoveToHueSatData_tag
{
  PACKED_UNION {
    zclCmdColorCtrl_MoveToHueSaturation_t moveToHueSat;   
    zclCmdColorCtrl_EnhancedMoveToHueSaturation_t enhMoveToHueSat;
  } msgData;
}zclColorCtrl_MoveToHueSatData_t;

/* xy table */
typedef PACKED_STRUCT zclColorCtrl_colorXY_tag
{
  uint16_t colorX;
  uint16_t colorY;
}zclColorCtrl_colorXY_t;
/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/
/*!
 * @fn 		zbStatus_t ZCL_ColorControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ColorCtrl Cluster Server. 
 *
 */
zbStatus_t ZCL_ColorControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_ColorControlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ColorCtrl Cluster Client. 
 *
 */
zbStatus_t ZCL_ColorControlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_ColorControlServerInit(void)
 *
 * @brief	Init Color Control Cluster Server
 *
 */
zbStatus_t ZCL_ColorControlServerInit(void);
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveToHueReq(zclColorCtrl_MoveToHue_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to HUE  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveToHueReq(zclColorCtrl_MoveToHue_t *pReq); 
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveHueReq(zclColorCtrl_MoveToHue_t *pReq) 
 *
 * @brief	Sends over-the-air a Move  HUE  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveHueReq(zclColorCtrl_MoveHue_t *pReq); 
/*!
 * @fn 		zbStatus_t zclColorCtrl_StepHueReq(zclColorCtrl_StepHue_t *pReq) 
 *
 * @brief	Sends over-the-air a Step  HUE  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_StepHueReq(zclColorCtrl_StepHue_t *pReq);  
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveToSaturationReq(zclColorCtrl_MoveToSaturation_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to Saturation  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveToSaturationReq(zclColorCtrl_MoveToSaturation_t *pReq); 
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveSaturationReq(zclColorCtrl_MoveSaturation_t *pReq) 
 *
 * @brief	Sends over-the-air a Move Saturation  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveSaturationReq(zclColorCtrl_MoveSaturation_t *pReq);  
/*!
 * @fn 		zbStatus_t zclColorCtrl_StepSaturationReq(zclColorCtrl_StepSaturation_t *pReq) 
 *
 * @brief	Sends over-the-air a Step Saturation  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_StepSaturationReq(zclColorCtrl_StepSaturation_t *pReq); 
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveToHueSaturationReq(zclColorCtrl_MoveToHueSaturation_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to Hue and Saturation request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveToHueSaturationReq(zclColorCtrl_MoveToHueSaturation_t *pReq);
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveToColorReq(zclColorCtrl_MoveToColor_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to Color request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveToColorReq(zclColorCtrl_MoveToColor_t *pReq);
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveColorReq(zclColorCtrl_MoveColor_t *pReq) 
 *
 * @brief	Sends over-the-air a Move  Color request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveColorReq( zclColorCtrl_MoveColor_t *pReq);
/*!
 * @fn 		zbStatus_t zclColorCtrl_StepColorReq(zclColorCtrl_StepColor_t *pReq) 
 *
 * @brief	Sends over-the-air a Step  Color request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_StepColorReq( zclColorCtrl_StepColor_t *pReq); 
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveToColorTemperatureReq(zclColorCtrl_MoveToColorTemperature_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to Color Temperature request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveToColorTemperatureReq( zclColorCtrl_MoveToColorTemperature_t *pReq); 
/*!
 * @fn 		uint8_t ColorCtrlServer_GetIndexFromColorSetupTable(uint8_t endpoint, uint8_t tmrId)
 *
 * @brief	return - index in the color Ctrl setup table if succes
 *                     - invalid data - otherwise
 */
uint8_t ColorCtrlServer_GetIndexFromColorSetupTable(uint8_t endpoint, uint8_t tmrId);

#if gColorCtrlEnableZllFunctionality_c
/*!
 * @fn 		zbStatus_t zclColorCtrl_EnhancedMoveToHueReq(zclColorCtrl_EnhancedMoveToHue_t *pReq) 
 *
 * @brief	Sends over-the-air an Enhanced Move to HUE request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_EnhancedMoveToHueReq( zclColorCtrl_EnhancedMoveToHue_t *pReq);
 /*!
 * @fn 		zbStatus_t zclColorCtrl_EnhancedMoveHueReq(zclColorCtrl_EnhancedMoveHue_t *pReq) 
 *
 * @brief	Sends over-the-air an Enhanced Move HUE request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_EnhancedMoveHueReq( zclColorCtrl_EnhancedMoveHue_t *pReq); 
/*!
 * @fn 		zbStatus_t zclColorCtrl_EnhancedStepHueReq(zclColorCtrl_EnhancedStepHue_t *pReq) 
 *
 * @brief	Sends over-the-air an Enhanced Step HUE request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_EnhancedStepHueReq( zclColorCtrl_EnhancedStepHue_t *pReq);
/*!
 * @fn 		zbStatus_t zclColorCtrl_EnhancedMoveToHueSaturationReq(zclColorCtrl_EnhancedMoveToHueSaturation_t *pReq) 
 *
 * @brief	Sends over-the-air an Enhanced Move To HUE  and Saturation request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_EnhancedMoveToHueSaturationReq( zclColorCtrl_EnhancedMoveToHueSaturation_t *pReq); 
/*!
 * @fn 		zbStatus_t zclColorCtrl_ColorLoopSetReq(zclColorCtrl_ColorLoopSet_t *pReq) 
 *
 * @brief	Sends over-the-air a Color Loop Set request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_ColorLoopSetReq( zclColorCtrl_ColorLoopSet_t *pReq); 
/*!
 * @fn 		zbStatus_t zclColorCtrl_StopMoveStepReq(zclColorCtrl_StopMoveStep_t *pReq) 
 *
 * @brief	Sends over-the-air a StopMoveStep request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_StopMoveStepReq(zclColorCtrl_StopMoveStep_t *pReq);
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveColorTemperatureReq(zclColorCtrl_MoveColorTemperature_t *pReq) 
 *
 * @brief	Sends over-the-air a Move color temperature request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveColorTemperatureReq( zclColorCtrl_MoveColorTemperature_t *pReq);
/*!
 * @fn 		zbStatus_t zclColorCtrl_StepColorTemperatureReq(zclColorCtrl_StepColorTemperature_t *pReq) 
 *
 * @brief	Sends over-the-air a Step color temperature request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_StepColorTemperatureReq( zclColorCtrl_StepColorTemperature_t *pReq); 
#endif /* gColorCtrlEnableZllFunctionality_c*/
/*!
 * @fn 		void ColorCtrlServer_StopActiveTimer(uint8_t endpoint)
 *
 * @brief	Stop active color control cluster action
 *
 */
void ColorCtrlServer_StopActiveTimer(uint8_t endpoint);
/*!
 * @fn 		void ColorCtrlServer_RefreshHueSatValues(zclColorCtrl_colorXY_t colorXy, uint8_t *hue, uint8_t *saturation)
 *
 * @brief	Get color X,Y values based on HUE and Saturation level 
 */
void ColorCtrlServer_RefreshHueSatValues(zclColorCtrl_colorXY_t colorXy, uint8_t *hue, uint8_t *saturation);
/*!
 * @fn 		void ColorCtrlServer_RefreshXyValues(uint8_t hue, uint8_t saturation, zclColorCtrl_colorXY_t *colorXy)
 *
 * @brief	Get color X,Y values based on HUE and Saturation level 
 */
void ColorCtrlServer_RefreshXyValues(uint8_t hue, uint8_t saturation, zclColorCtrl_colorXY_t *colorXY);
#ifdef __cplusplus
}
#endif
#endif
