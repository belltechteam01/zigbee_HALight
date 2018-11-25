/*! @file 	  ZclLighting.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for the lighting domain(also look at ZclGeneral for generic things like
 *			  on/off or level control which is used for dimming lights).
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
 *  [R4] - docs-14-0062-00-0plo-zll-1-0-errata-test-specification-draft.pdf
 */
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZclFoundation.h"
#include "HaProfile.h"
#include "ZCLOptions.h"
#include "ZclLighting.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
/* color Control command handler: */
static zbStatus_t ColorCtrlServer_MoveToHueHandler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ColorCtrlServer_StepHueHandler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ColorCtrlServer_MoveToSaturationHandler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ColorCtrlServer_StepSaturationHandler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ColorCtrlServer_MoveToHueSaturationHandler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ColorCtrlServer_MoveToColorHandler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ColorCtrlServer_MoveColorHandler(zbApsdeDataIndication_t *pIndication);
#if gColorCtrlEnableZllFunctionality_c
static zbStatus_t ColorCtrlServer_ColorLoopSetHandler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ColorCtrlServer_StopMoveStepHandler(zbApsdeDataIndication_t *pIndication);
#endif

/* color Control utility functions: */
static void ColorCotrolServer_GetHueDirection(uint8_t indexSetup, uint16_t requiredHue, uint16_t currentHue, uint8_t direction, uint16_t maxValue);
static void ColorCtrlServer_UpdateRemainingTime(uint8_t indexSetup, uint16_t remainingTime);
static void ColorCtrl_ResetColorXyLocalTable(uint8_t index);
static void ColorCtrl_ResetHueSaturationLocalTable(uint8_t index);
static void ColorCtrlServer_TimerCallBack(uint8_t tmrId);
static zbStatus_t ColorCtrlServer_SetTimeout(uint8_t indexSetup, uint32_t duration);
static void ColorCtrlServer_ColorLoopCallBack(uint8_t tmrId);
static zbStatus_t ColorCtrlServer_ActivateColorLoop(uint8_t indexSetup);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
zclColorCtrlServerSetup_t gColorCtrlServerSetup[gNoOfColorCtrlServerInstances_d];

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

/* Tables generated for Hue = variable: 0..254, saturation = 254 (color fully saturated), value = 254 
   Resulting color varies from red, through yellow, green, cyan, blue, magenata and back to red */
const uint16_t gColorCtrlHueToXTable[] =
{
0xA0F0, 0xA0AD, 0xA068, 0xA011, 0x9FA1, 0x9F17, 0x9E72, 0x9DB2, 0x9CD8, 0x9BE4, 0x9AD7, 0x99B2, 
0x9877, 0x9728, 0x95C6, 0x9454, 0x92D3, 0x9147, 0x8FB0, 0x8E12, 0x8C6F, 0x8AC7, 0x891E, 0x8776, 
0x85CF, 0x842B, 0x828B, 0x80F2, 0x7F60, 0x7DD4, 0x7C52, 0x7AD8, 0x7968, 0x7802, 0x76A6, 0x7555, 
0x740D, 0x72D1, 0x719F, 0x7077, 0x6F59, 0x6E46, 0x6D3D, 0x6C3B, 0x6B37, 0x6A33, 0x692D, 0x6827, 
0x6721, 0x661A, 0x6514, 0x640F, 0x630C, 0x620A, 0x610B, 0x600F, 0x5F16, 0x5E21, 0x5D31, 0x5C45, 
0x5B5F, 0x5A7F, 0x59A5, 0x58D3, 0x5807, 0x5744, 0x5689, 0x55D7, 0x552D, 0x548D, 0x53F7, 0x536A, 
0x52E8, 0x526F, 0x5201, 0x519D, 0x5143, 0x50F4, 0x50AE, 0x5072, 0x503F, 0x5015, 0x4FF4, 0x4FDB, 
0x4FC6, 0x4FB3, 0x4FA4, 0x4F94, 0x4F7F, 0x4F65, 0x4F44, 0x4F1D, 0x4EF0, 0x4EBC, 0x4E81, 0x4E3F, 
0x4DF7, 0x4DA9, 0x4D53, 0x4CF8, 0x4C96, 0x4C2F, 0x4BC2, 0x4B4F, 0x4AD8, 0x4A5C, 0x49DB, 0x4957, 
0x48CF, 0x4844, 0x47B6, 0x4726, 0x4694, 0x4600, 0x456B, 0x44D5, 0x443E, 0x43A7, 0x4310, 0x427A, 
0x41E4, 0x414E, 0x40BB, 0x4028, 0x3F97, 0x3F07, 0x3E7A, 0x3DEE, 0x3D61, 0x3CD0, 0x3C3C, 0x3BA3, 
0x3B06, 0x3A65, 0x39C0, 0x3918, 0x386C, 0x37BD, 0x370B, 0x3656, 0x359F, 0x34E6, 0x342B, 0x336F, 
0x32B2, 0x31F6, 0x313A, 0x307F, 0x2FC6, 0x2F10, 0x2E5D, 0x2DAE, 0x2D04, 0x2C60, 0x2BC2, 0x2B2B, 
0x2A9C, 0x2A15, 0x2997, 0x2923, 0x28B8, 0x2857, 0x2801, 0x27B5, 0x2774, 0x273D, 0x2710, 0x26ED, 
0x26D2, 0x26B9, 0x26CC, 0x26F6, 0x2727, 0x2768, 0x27BA, 0x281D, 0x2893, 0x291B, 0x29B6, 0x2A64, 
0x2B26, 0x2BFA, 0x2CE2, 0x2DDC, 0x2EE9, 0x3007, 0x3135, 0x3274, 0x33C3, 0x351F, 0x3689, 0x3800, 
0x3982, 0x3B0E, 0x3CA4, 0x3E42, 0x3FE7, 0x4192, 0x4341, 0x44F5, 0x46AC, 0x4864, 0x4A1E, 0x4BD7, 
0x4D90, 0x4F48, 0x50FD, 0x52AF, 0x545E, 0x5609, 0x57AF, 0x5950, 0x5AEC, 0x5C91, 0x5E43, 0x6001, 
0x61CC, 0x63A3, 0x6586, 0x6776, 0x6971, 0x6B77, 0x6D87, 0x6FA1, 0x71C3, 0x73EE, 0x761E, 0x7854, 
0x7A8E, 0x7CCA, 0x7F06, 0x8141, 0x8378, 0x85AA, 0x87D4, 0x89F3, 0x8C06, 0x8E0A, 0x8FFD, 0x91DC, 
0x93A6, 0x9557, 0x96EF, 0x986B, 0x99C9, 0x9B09, 0x9C2A, 0x9D2B, 0x9E0C, 0x9ECC, 0x9F6D, 0x9FEF, 
0xA053, 0xA0A2, 0xA0F0
};
const uint16_t gColorCtrlHueToYTable[] =
{
0x521D, 0x5254, 0x528C, 0x52D3, 0x532E, 0x539F, 0x5426, 0x54C2, 0x5574, 0x563B, 0x5717, 0x5806, 
0x5907, 0x5A18, 0x5B39, 0x5C67, 0x5DA1, 0x5EE4, 0x6030, 0x6181, 0x62D8, 0x6431, 0x658C, 0x66E6, 
0x6840, 0x6996, 0x6AE9, 0x6C37, 0x6D7F, 0x6EC2, 0x6FFD, 0x7131, 0x725D, 0x7382, 0x749D, 0x75B1, 
0x76BC, 0x77BE, 0x78B8, 0x79A9, 0x7A92, 0x7B73, 0x7C4B, 0x7D1E, 0x7DF1, 0x7EC6, 0x7F9B, 0x8071, 
0x8147, 0x821D, 0x82F3, 0x83C8, 0x849C, 0x856E, 0x863E, 0x870C, 0x87D7, 0x889F, 0x8963, 0x8A23, 
0x8ADF, 0x8B96, 0x8C47, 0x8CF3, 0x8D99, 0x8E38, 0x8ED1, 0x8F62, 0x8FEC, 0x906F, 0x90EA, 0x915C, 
0x91C7, 0x9229, 0x9283, 0x92D5, 0x931E, 0x935F, 0x9398, 0x93C9, 0x93F2, 0x9414, 0x942F, 0x9444, 
0x9455, 0x9451, 0x9422, 0x93EF, 0x93AC, 0x9356, 0x92ED, 0x926F, 0x91DD, 0x9136, 0x9079, 0x8FA6, 
0x8EBE, 0x8DC1, 0x8CAF, 0x8B89, 0x8A4F, 0x8902, 0x87A3, 0x8633, 0x84B3, 0x8324, 0x8187, 0x7FDD, 
0x7E28, 0x7C68, 0x7AA0, 0x78D0, 0x76F9, 0x751E, 0x733E, 0x715B, 0x6F76, 0x6D91, 0x6BAB, 0x69C7, 
0x67E4, 0x6605, 0x6429, 0x6251, 0x607E, 0x5EB0, 0x5CE9, 0x5B27, 0x5963, 0x5791, 0x55B2, 0x53C6, 
0x51CE, 0x4FC8, 0x4DB7, 0x4B99, 0x4971, 0x473E, 0x4500, 0x42BB, 0x406D, 0x3E19, 0x3BC0, 0x3964, 
0x3705, 0x34A7, 0x324A, 0x2FF1, 0x2D9E, 0x2B54, 0x2915, 0x26E2, 0x24C0, 0x22AF, 0x20B3, 0x1ECD, 
0x1D01, 0x1B4F, 0x19BA, 0x1843, 0x16EC, 0x15B5, 0x14A0, 0x13AC, 0x12D9, 0x1229, 0x1198, 0x1126, 
0x10CF, 0x107F, 0x1073, 0x1089, 0x10A4, 0x10C7, 0x10F3, 0x1128, 0x1167, 0x11B0, 0x1204, 0x1261, 
0x12C9, 0x133C, 0x13B8, 0x143F, 0x14CF, 0x1569, 0x160C, 0x16B7, 0x176B, 0x1826, 0x18E9, 0x19B2, 
0x1A82, 0x1B57, 0x1C31, 0x1D10, 0x1DF2, 0x1ED7, 0x1FBF, 0x20AA, 0x2195, 0x2282, 0x2370, 0x245D, 
0x254A, 0x2636, 0x2721, 0x280B, 0x28F3, 0x29D8, 0x2ABB, 0x2B9B, 0x2C79, 0x2D5B, 0x2E44, 0x2F34,
0x302B, 0x3128, 0x322C, 0x3336, 0x3447, 0x355D, 0x3679, 0x379B, 0x38C0, 0x39EA, 0x3B18, 0x3C48, 
0x3D7A, 0x3EAE, 0x3FE2, 0x4114, 0x4245, 0x4373, 0x449D, 0x45C1, 0x46DF, 0x47F4, 0x4900, 0x4A02, 
0x4AF8, 0x4BE1, 0x4CBC, 0x4D88, 0x4E45, 0x4EF1, 0x4F8C, 0x5016, 0x508F, 0x50F6, 0x514D, 0x5193, 
0x51C9, 0x51F3, 0x521D
};
/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
/******************************
  Color Control Cluster 
  See ZCL Specification Section 5.2 [R2], 
  ZLL Specification Section 6.8 [R3]
*******************************/
/* Color Control Cluster Attribute Definitions */
const zclAttrDef_t gaZclColorCtrlClusterAttrDef[] = {
  { gZclAttrIdColorCtrlCurrentHue_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, currentHue)},
  { gZclAttrIdColorCtrlCurrentSaturation_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, currentSaturation)},
  { gZclAttrIdColorCtrlRemainingTime_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, remainingTime)},
  { gZclAttrIdColorCtrlCurrentX_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c  | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c | gZclAttrFlagsInSceneTable_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, currentX)},
  { gZclAttrIdColorCtrlCurrentY_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c  | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c | gZclAttrFlagsInSceneTable_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, currentY)},
#if gColorCtrlEnableDriftCompensation_c  
  { gZclAttrIdColorCtrlDriftCompensation_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, driftCompensation)},
  { gZclAttrIdColorCtrlCompensationText_c, gZclDataTypeStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr16Oct_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, compensationText)},
#endif  
#if gColorCtrlEnableColorTemperature_c  
  { gZclAttrIdColorCtrlColorTemperature_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorTemperature)},
#endif  
  { gZclAttrIdColorCtrlColorMode_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorMode)}
#if gColorCtrlEnablePrimariesInformation_c    
  ,{ gZclAttrIdColorCtrlNumberOfPrimaries_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, noOfPrimaries)},
  { gZclAttrIdColorCtrlPrimary1X_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary1X)},
  { gZclAttrIdColorCtrlPrimary1Y_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary1Y)},
  { gZclAttrIdColorCtrlPrimary1Intensity_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary1Intensity)},
  { gZclAttrIdColorCtrlPrimary2X_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary2X)},
  { gZclAttrIdColorCtrlPrimary2Y_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary2Y)},
  { gZclAttrIdColorCtrlPrimary2Intensity_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary2Intensity)},
  { gZclAttrIdColorCtrlPrimary3X_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary3X)},
  { gZclAttrIdColorCtrlPrimary3Y_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary3Y)},
  { gZclAttrIdColorCtrlPrimary3Intensity_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary3Intensity)}
#if gColorCtrlEnablePrimariesAdditionalInf_c  
  ,{ gZclAttrIdColorCtrlPrimary4X_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary4X)},
  { gZclAttrIdColorCtrlPrimary4Y_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary4Y)},
  { gZclAttrIdColorCtrlPrimary4Intensity_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary4Intensity)},
  { gZclAttrIdColorCtrlPrimary5X_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary5X)},
  { gZclAttrIdColorCtrlPrimary5Y_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary5Y)},
  { gZclAttrIdColorCtrlPrimary5Intensity_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary5Intensity)},
  { gZclAttrIdColorCtrlPrimary6X_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary6X)},
  { gZclAttrIdColorCtrlPrimary6Y_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary6Y)},
  { gZclAttrIdColorCtrlPrimary6Intensity_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, primary6Intensity)}
#endif
#endif /* gColorCtrlEnablePrimariesInformation_c  */
#if gColorCtrlEnableColorPointSettings_c  
  ,{ gZclAttrIdColorCtrlWhitePointX_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, whitePointX)},
  { gZclAttrIdColorCtrlWhitePointY_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, whitePointY)},
  { gZclAttrIdColorCtrlColorPointRX_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorPointRx)},
  { gZclAttrIdColorCtrlColorPointRY_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorPointRy)},
  { gZclAttrIdColorCtrlColorPointRIntensity_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorPointRIntensity)},
  { gZclAttrIdColorCtrlColorPointGX_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorPointGx)},
  { gZclAttrIdColorCtrlColorPointGY_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorPointGy)},
  { gZclAttrIdColorCtrlColorPointGIntensity_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorPointGIntensity)},
  { gZclAttrIdColorCtrlColorPointBX_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorPointBx)},
  { gZclAttrIdColorCtrlColorPointBY_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorPointBy)},
  { gZclAttrIdColorCtrlColorPointBIntensity_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorPointBIntensity)}
#endif /* gColorCtrlEnableColorPointSettings_c */
};

#if gColorCtrlEnableZllFunctionality_c  
/* ZLL Color Ctrl Cluster Aditional Attribute Definitions */
const zclAttrDef_t gaZclColorCtrlClusterZllAditionalAttrDef[] = {
  { gZclAttrIdColorCtrlEnhancedCurrentHue_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, enhancedCurrentHue)},
  { gZclAttrIdColorCtrlEnhancedColorMode_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, enhancedColorMode)},
  { gZclAttrIdColorCtrlColorLoopActive_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorLoopActive)},
  { gZclAttrIdColorCtrlColorLoopDirection_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorLoopDirection)},
  { gZclAttrIdColorCtrlColorLoopTime_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c  | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorLoopTime)},
  { gZclAttrIdColorCtrlColorLoopStartEnhancedHue_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c  | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorLoopStartEnhancedHue)},
  { gZclAttrIdColorCtrlColorLoopStoredEnhancedHue_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorLoopStoredEnhancedHue)},
  { gZclAttrIdColorCtrlColorCapabilities_c, gZclDataTypeBitmap16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorCapabilities)}
#if gColorCtrlEnableColorTemperature_c    
  ,{ gZclAttrIdColorCtrlColorTempPhysicalMin_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorTempPhysicalMin)},
  { gZclAttrIdColorCtrlColorTempPhysicalMax_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclColorCtrlAttrsRAM_t, colorTempPhysicalMax)}
#endif
};
#endif

const zclAttrSet_t gaZclColorCtrlClusterAttrSet[] = {
  {gZclAttrColorCtrl_ColorInformationSet_c, (void *)&gaZclColorCtrlClusterAttrDef, NumberOfElements(gaZclColorCtrlClusterAttrDef)}
#if gColorCtrlEnableZllFunctionality_c    
  ,{gZclAttrColorCtrl_ZllAditionalSet_c, (void *)&gaZclColorCtrlClusterZllAditionalAttrDef, NumberOfElements(gaZclColorCtrlClusterZllAditionalAttrDef)}
#endif
};

const zclAttrSetList_t gZclColorCtrlClusterAttrSetList = {
  NumberOfElements(gaZclColorCtrlClusterAttrSet),
  gaZclColorCtrlClusterAttrSet
};

const zclCmd_t gaZclColorCtrlClusterCmdReceivedDef[]={
  // commands received 
  gZclCmdColorCtrl_MoveToHue_c, gZclCmdColorCtrl_StepHue_c, 
  gZclCmdColorCtrl_MoveToSaturation_c, gZclCmdColorCtrl_StepSaturation_c, 
  gZclCmdColorCtrl_MoveToColor_c, gZclCmdColorCtrl_MoveColor_c 
#if gColorCtrlEnableZllFunctionality_c
  ,gZclCmdColorCtrl_EnhancedMoveToHue_c, 
  gZclCmdColorCtrl_EnhancedStepHue_c, gZclCmdColorCtrl_EnhancedMoveToHueSat_c,
  gZclCmdColorCtrl_ColorLoopSet_c, gZclCmdColorCtrl_StopMoveStep_c
#endif /* gColorCtrlEnableZllFunctionality_c */    
};

const zclCommandsDefList_t gZclColorCtrlClusterCommandsDefList =
{
   NumberOfElements(gaZclColorCtrlClusterCmdReceivedDef),  gaZclColorCtrlClusterCmdReceivedDef,
   0, NULL
};

/******************************
  Color Control Cluster
  See ZCL Specification Section 5.2 [R2], 
  ZLL Specification Section 6.8 [R3]
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_ColorControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ColorCtrl Cluster Server. 
 *
 */
zbStatus_t ZCL_ColorControlClusterServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  uint8_t status = gZclSuccessDefaultRsp_c;
  zclFrame_t *pFrame;
  zbClusterId_t clusterIdOnOff = {gaZclClusterOnOff_c};
  uint8_t onOffValue;
#if gColorCtrlEnableZllFunctionality_c
  uint8_t colorLoopActive = FALSE;
#endif  
  
  /* not used in this function */
  (void)pDevice;
  
  pFrame = (void *)(pIndication->pAsdu);
  
  /* verify OnOff attribute */
  if((ZCL_GetAttribute(pIndication->dstEndPoint,  clusterIdOnOff, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffValue, NULL)!= gZbSuccess_c)||
     (onOffValue != gZclCmdOnOff_On_c))
  {
    /* ignore the command if the device is Off*/
    return gZclFailure_c;
  }
  
#if gColorCtrlEnableZllFunctionality_c
  /* verify Color Loop Active attribute */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorLoopActive_c, gZclServerAttr_c, &onOffValue, NULL);
  if((colorLoopActive == TRUE) && (pFrame->command != gZclCmdColorCtrl_ColorLoopSet_c))
  {
    /* ignore the command if the color loop is active */
    return status;
  }
#endif  
  status = gZclSuccess_c;
    
  switch (pFrame->command) 
  {
    case gZclCmdColorCtrl_MoveToHue_c:
      status = ColorCtrlServer_MoveToHueHandler(pIndication);
      break;
    case gZclCmdColorCtrl_StepHue_c:    
      status = ColorCtrlServer_StepHueHandler(pIndication);
      break;
    case gZclCmdColorCtrl_MoveToSaturation_c:  
      status = ColorCtrlServer_MoveToSaturationHandler(pIndication);
      break;
    case gZclCmdColorCtrl_StepSaturation_c:  
      status = ColorCtrlServer_StepSaturationHandler(pIndication);
      break;
    case gZclCmdColorCtrl_MoveToHueSaturation_c:  
      status = ColorCtrlServer_MoveToHueSaturationHandler(pIndication);
      break;      
    case gZclCmdColorCtrl_MoveToColor_c:  
      status = ColorCtrlServer_MoveToColorHandler(pIndication);
      break;
    case gZclCmdColorCtrl_MoveColor_c:  
      status = ColorCtrlServer_MoveColorHandler(pIndication);
      break;      
#if gColorCtrlEnableZllFunctionality_c
    case gZclCmdColorCtrl_EnhancedMoveToHue_c:
      status = ColorCtrlServer_MoveToHueHandler(pIndication);
      break;
    case gZclCmdColorCtrl_EnhancedStepHue_c:
      status = ColorCtrlServer_StepHueHandler(pIndication);
      break;      
    case gZclCmdColorCtrl_EnhancedMoveToHueSat_c: 
      status = ColorCtrlServer_MoveToHueSaturationHandler(pIndication);
      break; 
    case gZclCmdColorCtrl_ColorLoopSet_c:
      status = ColorCtrlServer_ColorLoopSetHandler(pIndication);
      break;     
    case gZclCmdColorCtrl_StopMoveStep_c:   
      status = ColorCtrlServer_StopMoveStepHandler(pIndication);
      break;
#endif       
    default:
    	status = gZclUnsupportedClusterCommand_c;   
    	break;
        
  } 
  
  if((!(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)) && 
     (status == gZclSuccess_c))
    status = gZclSuccessDefaultRsp_c;
  
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_ColorControlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the ColorCtrl Cluster Client. 
 *
 */
zbStatus_t ZCL_ColorControlClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  /* avoid compiler warnings */
  (void)pIndication;
  (void)pDevice;
  
  /* The comand use for this cluster are the read/write attributes */
  return gZclUnsupportedClusterCommand_c;
}

/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveToHueReq(zclColorCtrl_MoveToHue_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to HUE  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveToHueReq
(
  zclColorCtrl_MoveToHue_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_MoveToHue_c, sizeof(zclCmdColorCtrl_MoveToHue_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveHueReq(zclColorCtrl_MoveToHue_t *pReq) 
 *
 * @brief	Sends over-the-air a Move  HUE  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveHueReq
(
  zclColorCtrl_MoveHue_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_MoveHue_c, sizeof(zclCmdColorCtrl_MoveHue_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclColorCtrl_StepHueReq(zclColorCtrl_StepHue_t *pReq) 
 *
 * @brief	Sends over-the-air a Step  HUE  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_StepHueReq
(
  zclColorCtrl_StepHue_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_StepHue_c, sizeof(zclCmdColorCtrl_StepHue_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveToSaturationReq(zclColorCtrl_MoveToSaturation_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to Saturation  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveToSaturationReq
(
  zclColorCtrl_MoveToSaturation_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_MoveToSaturation_c, sizeof(zclCmdColorCtrl_MoveToSaturation_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveSaturationReq(zclColorCtrl_MoveSaturation_t *pReq) 
 *
 * @brief	Sends over-the-air a Move Saturation  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveSaturationReq
(
  zclColorCtrl_MoveSaturation_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_MoveSaturation_c, sizeof(zclCmdColorCtrl_MoveSaturation_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_StepSaturationReq(zclColorCtrl_StepSaturation_t *pReq) 
 *
 * @brief	Sends over-the-air a Step Saturation  request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_StepSaturationReq
(
  zclColorCtrl_StepSaturation_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_StepSaturation_c, sizeof(zclCmdColorCtrl_StepSaturation_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveToHueSaturationReq(zclColorCtrl_MoveToHueSaturation_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to Hue and Saturation request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveToHueSaturationReq
(
  zclColorCtrl_MoveToHueSaturation_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_MoveToHueSaturation_c, sizeof(zclCmdColorCtrl_MoveToHueSaturation_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveToColorReq(zclColorCtrl_MoveToColor_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to Color request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveToColorReq
(
  zclColorCtrl_MoveToColor_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_MoveToColor_c, sizeof(zclCmdColorCtrl_MoveToColor_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveColorReq(zclColorCtrl_MoveColor_t *pReq) 
 *
 * @brief	Sends over-the-air a Move  Color request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveColorReq
(
  zclColorCtrl_MoveColor_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_MoveColor_c, sizeof(zclCmdColorCtrl_MoveColor_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_StepColorReq(zclColorCtrl_StepColor_t *pReq) 
 *
 * @brief	Sends over-the-air a Step  Color request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_StepColorReq
(
  zclColorCtrl_StepColor_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_StepColor_c, sizeof(zclCmdColorCtrl_StepColor_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveToColorTemperatureReq(zclColorCtrl_MoveToColorTemperature_t *pReq) 
 *
 * @brief	Sends over-the-air a Move to Color Temperature request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveToColorTemperatureReq
(
  zclColorCtrl_MoveToColorTemperature_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_MoveToColorTemperature_c, sizeof(zclCmdColorCtrl_MoveToColorTemperature_t), (zclGenericReq_t *)pReq);
}
  
  
#if gColorCtrlEnableZllFunctionality_c
/*!
 * @fn 		zbStatus_t zclColorCtrl_EnhancedMoveToHueReq(zclColorCtrl_EnhancedMoveToHue_t *pReq) 
 *
 * @brief	Sends over-the-air an Enhanced Move to HUE request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_EnhancedMoveToHueReq
(
  zclColorCtrl_EnhancedMoveToHue_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_EnhancedMoveToHue_c, sizeof(zclCmdColorCtrl_EnhancedMoveToHue_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_EnhancedMoveHueReq(zclColorCtrl_EnhancedMoveHue_t *pReq) 
 *
 * @brief	Sends over-the-air an Enhanced Move HUE request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_EnhancedMoveHueReq
(
  zclColorCtrl_EnhancedMoveHue_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_EnhancedMoveHue_c, sizeof(zclCmdColorCtrl_EnhancedMoveHue_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_EnhancedStepHueReq(zclColorCtrl_EnhancedStepHue_t *pReq) 
 *
 * @brief	Sends over-the-air an Enhanced Step HUE request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_EnhancedStepHueReq
(
  zclColorCtrl_EnhancedStepHue_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_EnhancedStepHue_c, sizeof(zclCmdColorCtrl_EnhancedStepHue_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclColorCtrl_EnhancedMoveToHueSaturationReq(zclColorCtrl_EnhancedMoveToHueSaturation_t *pReq) 
 *
 * @brief	Sends over-the-air an Enhanced Move To HUE  and Saturation request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_EnhancedMoveToHueSaturationReq
(
  zclColorCtrl_EnhancedMoveToHueSaturation_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_EnhancedMoveToHueSat_c, sizeof(zclCmdColorCtrl_EnhancedMoveToHueSaturation_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclColorCtrl_ColorLoopSetReq(zclColorCtrl_ColorLoopSet_t *pReq) 
 *
 * @brief	Sends over-the-air a Color Loop Set request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_ColorLoopSetReq
(
  zclColorCtrl_ColorLoopSet_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_ColorLoopSet_c, sizeof(zclCmdColorCtrl_ColorLoopSet_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_StopMoveStepReq(zclColorCtrl_StopMoveStep_t *pReq) 
 *
 * @brief	Sends over-the-air a StopMoveStep request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_StopMoveStepReq
(
  zclColorCtrl_StopMoveStep_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_StopMoveStep_c, 0, (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_MoveColorTemperatureReq(zclColorCtrl_MoveColorTemperature_t *pReq) 
 *
 * @brief	Sends over-the-air a Move color temperature request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_MoveColorTemperatureReq
(
  zclColorCtrl_MoveColorTemperature_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_MoveColorTemperature_c, sizeof(zclCmdColorCtrl_MoveColorTemperature_t), (zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t zclColorCtrl_StepColorTemperatureReq(zclColorCtrl_StepColorTemperature_t *pReq) 
 *
 * @brief	Sends over-the-air a Step color temperature request from the Color Ctrl Cluster Client. 
 *
 */
zbStatus_t zclColorCtrl_StepColorTemperatureReq
(
  zclColorCtrl_StepColorTemperature_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterColorControl_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdColorCtrl_StepColorTemperature_c, sizeof(zclCmdColorCtrl_StepColorTemperature_t), (zclGenericReq_t *)pReq);
}
#endif /* gColorCtrlEnableZllFunctionality_c */



/*!
 * @fn 		static zbStatus_t ColorCtrlServer_MoveToHueHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle MoveToHUE/EnhancedMoveToHue command received from Color Ctrl Client
 *
 */
static zbStatus_t ColorCtrlServer_MoveToHueHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclColorCtrl_MoveToHueData_t *pMoveToHue;
  uint8_t indexSetup, colorMode, direction;
  uint16_t currentHue, requiredHue, transitionTime;
  uint16_t maxHue = gColorCtrlHueMaxValue_c;
  uint16_t colorModeAttrId = gZclAttrColorCtrlColorMode_c;
  
  /* verify color Ctrl server table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
    
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pMoveToHue = (void *)(pFrame+1);  
  
  if(pFrame->command == gZclCmdColorCtrl_EnhancedMoveToHue_c)
  {
#if gColorCtrlEnableZllFunctionality_c  
    maxHue = gColorCtrlEnhancedHueMaxValue_c;
    colorModeAttrId = gZclAttrColorCtrlEnhancedColorMode_c;
    requiredHue = OTA2Native16(pMoveToHue->msgData.enhMoveToHue.enhancedHue);
    direction = pMoveToHue->msgData.enhMoveToHue.direction;
    transitionTime = OTA2Native16(pMoveToHue->msgData.enhMoveToHue.transitionTime);
    /* get Enhanced current HUE */
    (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedCurrentHue_c, gZclServerAttr_c, &currentHue, NULL);
#else
    return gZclUnsupportedClusterCommand_c;
#endif	
  }
  else
  {
    uint8_t hueValue;
    requiredHue = pMoveToHue->msgData.moveToHue.hue;
    direction = pMoveToHue->msgData.moveToHue.direction;
    transitionTime = OTA2Native16(pMoveToHue->msgData.moveToHue.transitionTime);
    /* get current HUE */
    (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentHue_c, gZclServerAttr_c, &hueValue, NULL);
    currentHue = hueValue;
  }
   
  /* verify direction */
  if((direction > gColorCtrlDirection_Down_c)||
     (requiredHue > maxHue))
    return gZclInvalidValue_c;
  
  /* get current color mode */
  ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, colorModeAttrId, gZclServerAttr_c, &colorMode, NULL);
  if(colorMode != gColorCtrlEnhancedColorMode_EnhHueSaturation_c || colorMode != gColorCtrlColorMode_CurrentModeSaturation_c)
  {
    uint16_t remainingTime = 0x00;
    /* reset remaining time */
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTime);
    ColorCtrl_ResetColorXyLocalTable(indexSetup);
  }
  
  /* set color mode */
  colorMode = gColorCtrlColorMode_CurrentModeSaturation_c;
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode);

#if gColorCtrlEnableZllFunctionality_c  
  /* set enhanced color mode attribute */
  colorMode = (pFrame->command == gZclCmdColorCtrl_EnhancedMoveToHue_c)?gColorCtrlEnhancedColorMode_EnhHueSaturation_c:gColorCtrlColorMode_CurrentModeSaturation_c;
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedColorMode_c, gZclServerAttr_c, &colorMode);
#endif
  
  /* get hue direction */ 
  ColorCotrolServer_GetHueDirection(indexSetup, requiredHue, currentHue, direction, maxHue);  
  
  /* update required hue value (value = value*65536): */
  gColorCtrlServerSetup[indexSetup].hueReqValue = requiredHue << 16;
 
  /* update transition time*/
  gColorCtrlServerSetup[indexSetup].hueTransitionTime = transitionTime;
    
  /* update remaining time attribute */
  ColorCtrlServer_UpdateRemainingTime(indexSetup, transitionTime);
  
  /* set timeout */
  status = ColorCtrlServer_SetTimeout(indexSetup, (transitionTime > 0x01)?100:0); /* 100 miliseconds */  
 
  return status;
}

/*!
 * @fn 		static zbStatus_t ColorCtrlServer_StepHueHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle StepHUE/EnhancedStepHue command received from Color Ctrl Client
 *
 */
static zbStatus_t ColorCtrlServer_StepHueHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclColorCtrl_StepHueData_t *pStepHue;
  uint8_t indexSetup, colorMode, stepMode;
  uint16_t currentHue, stepSize, transitionTime;
  uint16_t maxHue = gColorCtrlHueMaxValue_c;
  uint16_t minHue = gColorCtrlHueMinValue_c;
  uint16_t colorModeAttrId = gZclAttrColorCtrlColorMode_c;
  
  /* verify color Ctrl server table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
  
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pStepHue = (void *)(pFrame+1);
  
  if(pFrame->command == gZclCmdColorCtrl_EnhancedMoveToHue_c)
  {
#if gColorCtrlEnableZllFunctionality_c  
    maxHue = gColorCtrlEnhancedHueMaxValue_c;
    minHue = gColorCtrlEnhancedHueMinValue_c;
    colorModeAttrId = gZclAttrColorCtrlEnhancedColorMode_c;
    stepSize = OTA2Native16(pStepHue->msgData.enhStepHue.stepSize);
    stepMode = pStepHue->msgData.enhStepHue.stepMode;
    transitionTime = OTA2Native16(pStepHue->msgData.enhStepHue.transitionTime);
    /* get current HUE */
    (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedCurrentHue_c , gZclServerAttr_c, &currentHue, NULL);
#else
    return gZclUnsupportedClusterCommand_c;
#endif	
  }
  else
  {
    uint8_t hueValue;
    stepSize = pStepHue->msgData.stepHue.stepSize;
    stepMode = pStepHue->msgData.stepHue.stepMode;
    transitionTime =  pStepHue->msgData.stepHue.transitionTime;
    /* get current HUE */
    (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentHue_c, gZclServerAttr_c, &hueValue, NULL);
    currentHue = hueValue;
  }
  
  /* verify stepMode */
  if(!(stepMode == gColorCtrlStepMode_Up_c || stepMode == gColorCtrlStepMode_Down_c))
    return gZclInvalidField_c;

  /* get current color mode */
  ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, colorModeAttrId, gZclServerAttr_c, &colorMode, NULL);
  if(colorMode != gColorCtrlEnhancedColorMode_EnhHueSaturation_c || colorMode != gColorCtrlColorMode_CurrentModeSaturation_c)
  {
    uint16_t remainingTime = 0x00;
    /* reset remaining time */
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTime);
    ColorCtrl_ResetColorXyLocalTable(indexSetup);
  }
  
  /* set color mode */
  colorMode = gColorCtrlColorMode_CurrentModeSaturation_c;
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode);

#if gColorCtrlEnableZllFunctionality_c  
  /* set enhanced color mode attribute */
  colorMode = (pFrame->command == gZclCmdColorCtrl_EnhancedStepHue_c)?gColorCtrlEnhancedColorMode_EnhHueSaturation_c:gColorCtrlColorMode_CurrentModeSaturation_c;
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedColorMode_c, gZclServerAttr_c, &colorMode);
#endif
  
  /* update hue table */
  if(stepMode == gColorCtrlStepMode_Up_c)
  {
    gColorCtrlServerSetup[indexSetup].hueDirection = gColorCtrlStepMode_Up_c;
    if(currentHue + stepSize <= maxHue)
    {
      gColorCtrlServerSetup[indexSetup].hueReqValue = currentHue + stepSize;
    }
    else
    {
      gColorCtrlServerSetup[indexSetup].hueReqValue = minHue + stepSize - (maxHue - currentHue)-1;
    }
  }
  else
  {
    gColorCtrlServerSetup[indexSetup].hueDirection = gColorCtrlStepMode_Down_c;
    if(currentHue >= stepSize + minHue)
    {
      gColorCtrlServerSetup[indexSetup].hueReqValue = currentHue - stepSize;
    }
    else
    {
      gColorCtrlServerSetup[indexSetup].hueReqValue = maxHue - stepSize + currentHue - minHue + 1;
    }
  }
    
  /* update required hue value (value = value*65536): */
  gColorCtrlServerSetup[indexSetup].hueReqValue = (gColorCtrlServerSetup[indexSetup].hueReqValue << 16);
 
  /* update transition time*/
  gColorCtrlServerSetup[indexSetup].hueTransitionTime = transitionTime;
  
  /* update remaining time attribute */
  ColorCtrlServer_UpdateRemainingTime(indexSetup, transitionTime);
  
  /* set timeout */
  status = ColorCtrlServer_SetTimeout(indexSetup, (transitionTime > 0x01)?100:0); /* 100 miliseconds */  
 
  return status;
}

/*!
 * @fn 		static zbStatus_t ColorCtrlServer_MoveToSaturationHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle Move to Saturation command received from Color Ctrl Client
 *
 */
static zbStatus_t ColorCtrlServer_MoveToSaturationHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclCmdColorCtrl_MoveToSaturation_t *pMoveToSat;
  uint8_t indexSetup, colorMode;
  uint8_t currentSaturation;
  
  /* verify color Ctrl server table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
  
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pMoveToSat = (void *)(pFrame+1);
  pMoveToSat->transitionTime = OTA2Native16(pMoveToSat->transitionTime);
  
  /* verify saturation */
  if(pMoveToSat->saturation > gColorCtrlSaturationMaxValue_c)
    return gZclInvalidValue_c;
  
  /* get current saturation */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentSaturation_c, gZclServerAttr_c, &currentSaturation, NULL);
  
  /* get current color mode */
  ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode, NULL);
  if(colorMode != gColorCtrlColorMode_CurrentModeSaturation_c)
  {
    uint16_t remainingTime = 0x00;
    /* reset remaining time */
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTime);

    /* set color mode attribute to 0x00 */
    colorMode = gColorCtrlColorMode_CurrentModeSaturation_c;
    if(ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode) == gZclSuccess_c)
      ColorCtrl_ResetColorXyLocalTable(indexSetup);
  }
  
#if gColorCtrlEnableZllFunctionality_c
  /* set enhanced color mode */
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedColorMode_c, gZclServerAttr_c, &colorMode);
#endif  
  
  /* update required saturation value (value = value*65536): */
  gColorCtrlServerSetup[indexSetup].satReqValue = (pMoveToSat->saturation<<16);
  
  /* update transition time*/
  gColorCtrlServerSetup[indexSetup].satTransitionTime =  pMoveToSat->transitionTime;
  
  /* update remaining time attribute */
  ColorCtrlServer_UpdateRemainingTime(indexSetup, pMoveToSat->transitionTime);
  
  /* set timeout */
  status = ColorCtrlServer_SetTimeout(indexSetup, (pMoveToSat->transitionTime > 0x01)?100:0); /* 100 miliseconds */  

  return status;
}

/*!
 * @fn 		static zbStatus_t ColorCtrlServer_StepSaturationHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle StepSaturation command received from Color Ctrl Client
 *
 */
static zbStatus_t ColorCtrlServer_StepSaturationHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclCmdColorCtrl_StepSaturation_t *pStepSat;
  uint8_t indexSetup, colorMode;
  uint8_t currentSaturation;
  
  /* verify color Ctrl server table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
  
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pStepSat = (void *)(pFrame+1);
  pStepSat->transitionTime = OTA2Native16(pStepSat->transitionTime);
  
  if(!(pStepSat->stepMode == gColorCtrlStepMode_Up_c || pStepSat->stepMode == gColorCtrlStepMode_Down_c))
    return gZclInvalidField_c;
  
  /* get current saturation */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentSaturation_c, gZclServerAttr_c, &currentSaturation, NULL);
  
  /* get current color mode */
  ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode, NULL);
  if(colorMode != gColorCtrlColorMode_CurrentModeSaturation_c)
  {
    uint16_t remainingTime = 0x00;
    /* reset remaining time */
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTime);

    /* set color mode attribute to 0x00 */
    colorMode = gColorCtrlColorMode_CurrentModeSaturation_c;
    if(ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode) == gZclSuccess_c)
      ColorCtrl_ResetColorXyLocalTable(indexSetup);
  }
#if gColorCtrlEnableZllFunctionality_c
  /* set enhanced color mode */
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedColorMode_c, gZclServerAttr_c, &colorMode);
#endif  
      
  /* update saturation table */
  if(pStepSat->stepMode == gColorCtrlStepMode_Up_c)
  {
     gColorCtrlServerSetup[indexSetup].satReqValue = gColorCtrlSaturationMaxValue_c;
     if(currentSaturation + pStepSat->stepSize <= gColorCtrlSaturationMaxValue_c)
     {
       gColorCtrlServerSetup[indexSetup].satReqValue = currentSaturation + pStepSat->stepSize;
     }
  }
  else
  {
     gColorCtrlServerSetup[indexSetup].satReqValue = gColorCtrlSaturationMinValue_c;
     if(currentSaturation >= gColorCtrlSaturationMinValue_c + pStepSat->stepSize)
     {
       gColorCtrlServerSetup[indexSetup].satReqValue = currentSaturation - pStepSat->stepSize;
     }
  }

  /* update required saturation value (value = value*65536): */
  gColorCtrlServerSetup[indexSetup].satReqValue = (gColorCtrlServerSetup[indexSetup].satReqValue<<16);
  
  /* update transition time*/
  gColorCtrlServerSetup[indexSetup].satTransitionTime =  pStepSat->transitionTime;
  
  /* update remaining time attribute */
  ColorCtrlServer_UpdateRemainingTime(indexSetup, pStepSat->transitionTime);
  
  /* set timeout */
  status = ColorCtrlServer_SetTimeout(indexSetup, (pStepSat->transitionTime > 0x01)?100:0); /* 100 miliseconds */  
  return status;
}

/*!
 * @fn 		static zbStatus_t ColorCtrlServer_MoveToHueSaturationHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle MoveToHueSaturation/EnhancedMoveToHueSaturation command received from Color Ctrl Client
 *
 */
static zbStatus_t ColorCtrlServer_MoveToHueSaturationHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclColorCtrl_MoveToHueSatData_t *pMoveToHueSat;
  uint8_t indexSetup, colorMode, saturation;
  uint16_t currentHue, requiredHue, transitionTime;
  uint16_t maxHue = gColorCtrlHueMaxValue_c;
  uint16_t colorModeAttrId = gZclAttrColorCtrlColorMode_c;
  
  /* verify color Ctrl server table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
    
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pMoveToHueSat = (void *)(pFrame+1);  
  
  if(pFrame->command == gZclCmdColorCtrl_EnhancedMoveToHueSat_c)
  {
#if gColorCtrlEnableZllFunctionality_c  
    maxHue = gColorCtrlEnhancedHueMaxValue_c;
    colorModeAttrId = gZclAttrColorCtrlEnhancedColorMode_c;
    requiredHue = OTA2Native16(pMoveToHueSat->msgData.enhMoveToHueSat.enhancedHue);
    transitionTime = OTA2Native16(pMoveToHueSat->msgData.enhMoveToHueSat.transitionTime);
    saturation = pMoveToHueSat->msgData.enhMoveToHueSat.saturation;
    /* get current HUE */
    (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedCurrentHue_c, gZclServerAttr_c, &currentHue, NULL);
  
#else
    return gZclUnsupportedClusterCommand_c;
#endif	
  }
  else
  {
    uint8_t hueValue;
    requiredHue = pMoveToHueSat->msgData.moveToHueSat.hue; 
    transitionTime = OTA2Native16(pMoveToHueSat->msgData.moveToHueSat.transitionTime);
    saturation = pMoveToHueSat->msgData.moveToHueSat.saturation;
    (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentHue_c, gZclServerAttr_c, &hueValue, NULL);
    currentHue = hueValue;
  }
    
  if(requiredHue > maxHue || saturation > gColorCtrlSaturationMaxValue_c)
    return gZclInvalidValue_c;
  

  /* get current color mode */
  ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, colorModeAttrId, gZclServerAttr_c, &colorMode, NULL);
  if(colorMode != gColorCtrlEnhancedColorMode_EnhHueSaturation_c || colorMode != gColorCtrlColorMode_CurrentModeSaturation_c)
  {
    uint16_t remainingTime = 0x00;
    /* reset remaining time */
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTime);
    ColorCtrl_ResetColorXyLocalTable(indexSetup);
  }
  
  /* set color mode */
  colorMode = gColorCtrlColorMode_CurrentModeSaturation_c;
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode);

#if gColorCtrlEnableZllFunctionality_c  
  /* set enhanced color mode attribute */
  colorMode = (pFrame->command == gZclCmdColorCtrl_EnhancedMoveToHueSat_c)?gColorCtrlEnhancedColorMode_EnhHueSaturation_c:gColorCtrlColorMode_CurrentModeSaturation_c;
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedColorMode_c, gZclServerAttr_c, &colorMode);
#endif
  
  /* get hue direction */ 
  gColorCtrlServerSetup[indexSetup].hueDirection = (requiredHue > currentHue)?gColorCtrlDirection_Up_c:gColorCtrlDirection_Down_c;
  
  /* update required hue value (value = value*65536): */
  gColorCtrlServerSetup[indexSetup].hueReqValue = requiredHue << 16;
 
  /* update hue transition time*/
  gColorCtrlServerSetup[indexSetup].hueTransitionTime = transitionTime;
    
  /* update required saturation value (value = value*65536): */
  gColorCtrlServerSetup[indexSetup].satReqValue = (saturation<<16);
  
  /* update saturation transition time*/
  gColorCtrlServerSetup[indexSetup].satTransitionTime =  transitionTime;
  
  /* update remaining time attribute */
  ColorCtrlServer_UpdateRemainingTime(indexSetup, transitionTime);
  
  
  /* set timeout */
  status = ColorCtrlServer_SetTimeout(indexSetup, (transitionTime > 0x01)?100:0); /* 100 miliseconds */  
  
  return status;
}


/*!
 * @fn 		static zbStatus_t ColorCtrlServer_MoveToColorHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle Move to Color command received from Color Ctrl Client
 *
 */
static zbStatus_t ColorCtrlServer_MoveToColorHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclCmdColorCtrl_MoveToColor_t *pMoveToColor;
  uint8_t indexSetup, colorMode;
  uint16_t colorX, colorY;
  
  /* verify color Ctrl server table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
  
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pMoveToColor = (void *)(pFrame+1);
  
  pMoveToColor->colorX = OTA2Native16(pMoveToColor->colorX);
  pMoveToColor->colorY = OTA2Native16(pMoveToColor->colorY);
  pMoveToColor->transitionTime = OTA2Native16(pMoveToColor->transitionTime);
  
  /* get current color mode */
  ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode, NULL);
  if(colorMode != gColorCtrlColorMode_CurrentXY_c)
  {
    uint16_t remainingTime = 0x00;
    /* reset remaining time */
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTime);
 
    /* set color mode attribute to 0x01 */
    colorMode = gColorCtrlColorMode_CurrentXY_c;
    if(ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode) == gZbSuccess_c)
    {
      ColorCtrl_ResetHueSaturationLocalTable(indexSetup);
    }
  }
#if gColorCtrlEnableZllFunctionality_c
  /* set enhanced color mode */
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedColorMode_c, gZclServerAttr_c, &colorMode);
#endif  
  
  /* get current colorX, colorY values */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentX_c, gZclServerAttr_c, &colorX, NULL);
  (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentY_c, gZclServerAttr_c, &colorY, NULL);
  
  /* update colorX,Y values (value = value*65536) */
  gColorCtrlServerSetup[indexSetup].colorxReqValue = (pMoveToColor->colorX<<16);
  gColorCtrlServerSetup[indexSetup].coloryReqValue = (pMoveToColor->colorY<<16);
  /* set transitition time */
  gColorCtrlServerSetup[indexSetup].colorxTransitionTime = pMoveToColor->transitionTime;
  gColorCtrlServerSetup[indexSetup].coloryTransitionTime = pMoveToColor->transitionTime; 
  
  /* update remaining time attribute */
  ColorCtrlServer_UpdateRemainingTime(indexSetup, pMoveToColor->transitionTime);
  
  /* set timeout */
  status = ColorCtrlServer_SetTimeout(indexSetup, (pMoveToColor->transitionTime > 0x01)?100:0); /* 100 miliseconds */  
 
 
  return status;
}

/*!
 * @fn 		static zbStatus_t ColorCtrlServer_MoveColorHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle Move Color command received from Color Ctrl Client
 *
 */
static zbStatus_t ColorCtrlServer_MoveColorHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclCmdColorCtrl_MoveColor_t *pMoveColor;
  uint8_t indexSetup, colorMode;
  uint16_t colorX, colorY;
  uint32_t colorxDiff, coloryDiff;
  uint32_t tempTimeValue;
  uint16_t rateX, rateY, transitionTime;
  
  /* verify color Ctrl server table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
  
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pMoveColor = (void *)(pFrame+1);
  
  rateX = OTA2Native16(pMoveColor->rateX);
  rateY = OTA2Native16(pMoveColor->rateY);
   
  if(rateX == 0x00 && rateY == 0x00)
  {
     uint16_t remainingTime = 0x00;
     /* reset remaining time */
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTime);
     ZbTMR_StopTimer(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId);
     ColorCtrl_ResetHueSaturationLocalTable(indexSetup);
     ColorCtrl_ResetColorXyLocalTable(indexSetup);
     return status;
  }
  
  /* get current color mode */
  ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode, NULL);
  if(colorMode != gColorCtrlColorMode_CurrentXY_c)
  {
    uint16_t remainingTime = 0x00;
    /* reset remaining time */
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTime);
 
    /* set color mode attribute to 0x01 */
    colorMode = gColorCtrlColorMode_CurrentXY_c;
    if(ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorMode_c, gZclServerAttr_c, &colorMode) == gZbSuccess_c)
      ColorCtrl_ResetHueSaturationLocalTable(indexSetup);
  }
#if gColorCtrlEnableZllFunctionality_c
  /* set enhanced color mode */
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedColorMode_c, gZclServerAttr_c, &colorMode);
#endif  
 
  /* get current colorX, colorY values */
  (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentX_c, gZclServerAttr_c, &colorX, NULL);
  (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentY_c, gZclServerAttr_c, &colorY, NULL);
  

  if(rateX < 0x8000)
  {
    /* move up */
    colorxDiff = gColorCtrlColorMaxValue_c-colorX;
    gColorCtrlServerSetup[indexSetup].colorxReqValue = gColorCtrlColorMaxValue_c;
  }
  else
  {
    /* move Down */
    colorxDiff = colorX - gColorCtrlColorMinValue_c;
    gColorCtrlServerSetup[indexSetup].colorxReqValue = gColorCtrlColorMinValue_c;
    rateX = 0xFFFF - rateX + 1;
  }
  

  if(rateY < 0x8000)
  {
    /* move up */
    coloryDiff = gColorCtrlColorMaxValue_c-colorY;
    gColorCtrlServerSetup[indexSetup].coloryReqValue = gColorCtrlColorMaxValue_c;
  } 
  else
  {
    /* move down */
    coloryDiff = colorY - gColorCtrlColorMinValue_c;
    gColorCtrlServerSetup[indexSetup].coloryReqValue = gColorCtrlColorMinValue_c;
    rateY = (uint16_t)(0x10000 - rateY) ;
  }
  
  /* update colorX,Y values (value = value*65536) */
  gColorCtrlServerSetup[indexSetup].colorxReqValue = (gColorCtrlServerSetup[indexSetup].colorxReqValue<<16);
  gColorCtrlServerSetup[indexSetup].coloryReqValue = (gColorCtrlServerSetup[indexSetup].coloryReqValue<<16);
  
  /* set transitition time */
  tempTimeValue = ((colorxDiff<<16)/rateX)*10;
  gColorCtrlServerSetup[indexSetup].colorxTransitionTime = (tempTimeValue>>16);
  if( gColorCtrlServerSetup[indexSetup].colorxTransitionTime == 0x00 )
    gColorCtrlServerSetup[indexSetup].colorxTransitionTime = 0x01;
  tempTimeValue = ((coloryDiff<<16)/rateY)*10;  
  gColorCtrlServerSetup[indexSetup].coloryTransitionTime = (tempTimeValue>>16);
  if( gColorCtrlServerSetup[indexSetup].coloryTransitionTime == 0x00 )
    gColorCtrlServerSetup[indexSetup].coloryTransitionTime = 0x01;
    
  /* update remaining time attribute */
  transitionTime = (gColorCtrlServerSetup[indexSetup].colorxTransitionTime > gColorCtrlServerSetup[indexSetup].coloryTransitionTime)?gColorCtrlServerSetup[indexSetup].colorxTransitionTime:gColorCtrlServerSetup[indexSetup].coloryTransitionTime;
  ColorCtrlServer_UpdateRemainingTime(indexSetup, transitionTime);
  
  /* set timeout */
  status = ColorCtrlServer_SetTimeout(indexSetup, (transitionTime > 0x01)?100:0); /* 100 miliseconds */  
  
  return status;
}

#if gColorCtrlEnableZllFunctionality_c
/*!
 * @fn 		static zbStatus_t ColorCtrlServer_StopMoveStepHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle StopMoveStep command received from Color Ctrl Client
 *
 */
static zbStatus_t ColorCtrlServer_ColorLoopSetHandler(zbApsdeDataIndication_t *pIndication)
{
  zbStatus_t status = gZclSuccess_c;
  zclFrame_t *pFrame;
  zclCmdColorCtrl_ColorLoopSet_t *pColorLoopSet;
  uint8_t indexSetup, colorMode, hue;
  uint16_t transitionTime, enhancedHue;
  bool_t activeLoop = FALSE;
  
  /* verify color Ctrl server table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
  
  /* get payload data */
  pFrame = (void *)(pIndication->pAsdu);
  pColorLoopSet = (void *)(pFrame+1);  
  pColorLoopSet->time = OTA2Native16(pColorLoopSet->time);
  pColorLoopSet->startHue = OTA2Native16(pColorLoopSet->startHue);
  
  /* reset previous actions: */
  ColorCtrl_ResetHueSaturationLocalTable(indexSetup);
  ColorCtrl_ResetColorXyLocalTable(indexSetup);
  
  /* get color loop active attribute*/
  ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorLoopActive_c, gZclServerAttr_c, &activeLoop, NULL);
 
  /* verify update flags field */
  if(pColorLoopSet->updateFlags.updateDirection)
  {
    if(pColorLoopSet->direction > gColorCtrlDirection_IncrementHue_c)
      return gZclInvalidField_c;
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorLoopDirection_c, gZclServerAttr_c, &pColorLoopSet->direction);
  }
  if(pColorLoopSet->updateFlags.updateTime)
  {
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorLoopTime_c, gZclServerAttr_c, &pColorLoopSet->time);
  }
  if(pColorLoopSet->updateFlags.updateStartHue)
  {
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorLoopStartEnhancedHue_c, gZclServerAttr_c, &pColorLoopSet->startHue);
  }
  if(pColorLoopSet->updateFlags.updateAction)
  {
    if(pColorLoopSet->action > gColorCtrlAction_ActivateColorLoopUsingEnhancedCurrentHue_c)
      return gZclInvalidField_c;
    if(pColorLoopSet->action == gColorCtrlAction_DeactivateColorLoop_c)
    {
      if(activeLoop == TRUE)
      {
        /* de-activate the color loop */
        activeLoop = FALSE;
        (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorLoopActive_c, gZclServerAttr_c, &activeLoop);
        (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorLoopStoredEnhancedHue_c, gZclServerAttr_c, &enhancedHue, NULL);
        /* update enhanced current HUE */
        (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedCurrentHue_c, gZclServerAttr_c, &enhancedHue);
        /* update current hue */
        hue = (uint8_t)(enhancedHue>>8);
        (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentHue_c, gZclServerAttr_c, &hue);
        ZbTMR_StopTimer(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId);
        /* send event to the application */
        BeeAppUpdateDevice(gColorCtrlServerSetup[indexSetup].endpoint, gZclUI_GoToLevel_c, 0, 0, NULL);       
      }
      return status; 
    }
    activeLoop = TRUE;
    (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedCurrentHue_c, gZclServerAttr_c, &enhancedHue, NULL);
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorLoopStoredEnhancedHue_c, gZclServerAttr_c, &enhancedHue);
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorLoopActive_c, gZclServerAttr_c, &activeLoop); 
    
     /* set enhanced color mode */
    colorMode = gColorCtrlEnhancedColorMode_EnhHueSaturation_c;
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedColorMode_c, gZclServerAttr_c, &colorMode);
   
    if(pColorLoopSet->action == gColorCtrlAction_ActivateColorLoopUsingStartHue_c)
    {
      (void)ZCL_GetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlColorLoopStartEnhancedHue_c, gZclServerAttr_c, &enhancedHue, NULL);
      (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlEnhancedCurrentHue_c, gZclServerAttr_c, &enhancedHue);
      hue = (uint8_t)(enhancedHue>>8);  
      (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlCurrentHue_c, gZclServerAttr_c, &hue);
    }
    
    /* set transition time to 0xFFFF */
     transitionTime = 0xFFFF;
    (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &transitionTime);
 
    /* activate color loop */ 
    status = ColorCtrlServer_ActivateColorLoop(indexSetup);
    
  }
  return status;
}
/*!
 * @fn 		static zbStatus_t ColorCtrlServer_StopMoveStepHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Handle StopMoveStep command received from Color Ctrl Client
 *
 */
static zbStatus_t ColorCtrlServer_StopMoveStepHandler(zbApsdeDataIndication_t *pIndication)
{
  uint16_t remainingTime = 0x00;
  uint8_t indexSetup;
   
  /* verify color Ctrl server table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return gZclFailure_c;
   
   /* reset remaining time */
  (void)ZCL_SetAttribute(pIndication->dstEndPoint,  pIndication->aClusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTime);
 
  /* set color mode attribute to 0x01 */
  ColorCtrl_ResetHueSaturationLocalTable(indexSetup);
  ColorCtrl_ResetColorXyLocalTable(indexSetup);
  
  return gZclSuccess_c;
}
#endif
/*!
 * @fn 		zbStatus_t ZCL_ColorControlServerInit(void)
 *
 * @brief	Init Color Control Cluster Server
 *
 */
zbStatus_t ZCL_ColorControlServerInit(void)
{ 
  zbStatus_t status = gZclFailure_c;
  uint8_t nextStartIndex = 0, i = 0;
  zbClusterId_t colorControlClusterId = {gaZclClusterColorControl_c};
    
  for(i = 0; i< gNoOfColorCtrlServerInstances_d; i++)
  {
      uint8_t endpoint;
      uint8_t startIndex = nextStartIndex;
     
      gColorCtrlServerSetup[i].endpoint = gZclCluster_InvalidDataIndex_d;
      gColorCtrlServerSetup[i].colorTransitionTmrId = gTmrInvalidTimerID_c;
      
      endpoint = ZCL_GetEndPointForSpecificCluster(colorControlClusterId, TRUE, startIndex, &nextStartIndex);
      if(endpoint != gZclCluster_InvalidDataIndex_d)  
      {
        FLib_MemSet(&gColorCtrlServerSetup[i], 0, sizeof(zclColorCtrlServerSetup_t));
        gColorCtrlServerSetup[i].endpoint = endpoint;
        /* Initializa the timer for the Level Control Cluster */
        gColorCtrlServerSetup[i].colorTransitionTmrId = ZbTMR_AllocateTimer(); 
        if(gColorCtrlServerSetup[i].colorTransitionTmrId == gTmrInvalidTimerID_c)
          return gZclFailure_c;
        status = gZclSuccess_c;
      }
  }

  return status;
}

/*!
 * @fn 		uint8_t ColorCtrlServer_GetIndexFromColorSetupTable(uint8_t endpoint, uint8_t tmrId)
 *
 * @brief	return - index in the color Ctrl setup table if succes
 *                     - invalid data - otherwise
 */
uint8_t ColorCtrlServer_GetIndexFromColorSetupTable(uint8_t endpoint, uint8_t tmrId)
{
  uint8_t i;
  if(endpoint != gZclCluster_InvalidDataIndex_d) 
  { 
    for(i=0;i<gNoOfColorCtrlServerInstances_d; i++)
      if(endpoint == gColorCtrlServerSetup[i].endpoint)
        return i;
  }
  if(tmrId != gTmrInvalidTimerID_c)
  {
     for(i=0;i<gNoOfColorCtrlServerInstances_d; i++)
      if(tmrId == gColorCtrlServerSetup[i].colorTransitionTmrId)
        return i;
  }
  return gZclCluster_InvalidDataIndex_d;
}

/*!
 * @fn 		void ColorCtrlServer_SatToXyValue(zclColorCtrl_colorXY_t currentColorXy, uint8_t saturation, zclColorCtrl_colorXY_t *colorXy)
 *
 * @brief	Get color X,Y values based on Saturation level 
 */
void ColorCtrlServer_SatToXyValue(zclColorCtrl_colorXY_t currentColorXy, uint8_t saturation, zclColorCtrl_colorXY_t *colorXY)
{
  uint32_t tempX, tempY;
  uint32_t tempxDiff, tempyDiff;
  uint32_t tempSat = (gColorCtrlSaturationMaxValue_c + 1 - saturation)<<16;
  
  tempSat /= (gColorCtrlSaturationMaxValue_c + 1);
  tempX = currentColorXy.colorX;
  tempY = currentColorXy.colorY;
  
  /* update XY values, based on Saturation Value */
  if(tempX > gColorCtrlDefaultWhiteX_c)
  {
    tempxDiff = ((tempX - gColorCtrlDefaultWhiteX_c)*tempSat)>>16;
    colorXY->colorX = (uint16_t)(tempX - tempxDiff);
  }
  else
  {
    tempxDiff = ((gColorCtrlDefaultWhiteX_c - tempX)*tempSat)>>16;
    colorXY->colorX = (uint16_t)(tempX + tempxDiff);
  }
  
  if(tempY > gColorCtrlDefaultWhiteY_c)
  {
    tempyDiff = ((tempY - gColorCtrlDefaultWhiteY_c)*tempSat)>>16;
    colorXY->colorY = (uint16_t)(tempY - tempyDiff);
  }
  else
  {
    tempyDiff = ((gColorCtrlDefaultWhiteY_c - tempY)*tempSat)>>16;
    colorXY->colorY = (uint16_t)(tempY + tempyDiff);
  }
}

/*!
 * @fn 		void ColorCtrlServer_RefreshXyValues(uint8_t hue, uint8_t saturation, zclColorCtrl_colorXY_t *colorXy)
 *
 * @brief	Get color X,Y values based on HUE and Saturation level 
 */
void ColorCtrlServer_RefreshXyValues(uint8_t hue, uint8_t saturation, zclColorCtrl_colorXY_t *colorXY)
{
  zclColorCtrl_colorXY_t hueTocolorXY;
  
  /* get x, y values from table, based on HUE Value */
  hueTocolorXY.colorX = gColorCtrlHueToXTable[hue];
  hueTocolorXY.colorY = gColorCtrlHueToYTable[hue];
  if(saturation < gColorCtrlSaturationMaxValue_c)
  {
    ColorCtrlServer_SatToXyValue(hueTocolorXY, saturation, colorXY);
  }
  else
  {
    colorXY->colorX = hueTocolorXY.colorX;
    colorXY->colorY = hueTocolorXY.colorY;
  }
}

/*!
 * @fn 		void ColorCtrlServer_RefreshHueSatValues(zclColorCtrl_colorXY_t colorXy, uint8_t *hue, uint8_t *saturation)
 *
 * @brief	Get color X,Y values based on HUE and Saturation level 
 */
void ColorCtrlServer_RefreshHueSatValues(zclColorCtrl_colorXY_t colorXy, uint8_t *hue, uint8_t *saturation)
{
  uint8_t i;
  uint32_t minDiffX, minDiffY, currentDiffX, currentDiffY;
  uint16_t tempX = gColorCtrlHueToXTable[gColorCtrlHueMinValue_c];
  uint16_t tempY = gColorCtrlHueToYTable[gColorCtrlHueMinValue_c];
  
  /* get nearest Hue value based on gColorCtrlHueToXTable, gColorCtrlHueToYTable */
  minDiffX = (colorXy.colorX > tempX)?(colorXy.colorX - tempX):(tempX - colorXy.colorX);
  minDiffY = (colorXy.colorY > tempY)?(colorXy.colorY - tempY):(tempY - colorXy.colorY);
  *hue = gColorCtrlHueMinValue_c;
  
  for(i = gColorCtrlHueMinValue_c+1; i<gColorCtrlHueMaxValue_c+1; i++)
  {
    tempX = gColorCtrlHueToXTable[i];
    tempY = gColorCtrlHueToYTable[i];
    
    currentDiffX = (colorXy.colorX > tempX)?(colorXy.colorX - tempX):(tempX - colorXy.colorX);
    currentDiffY = (colorXy.colorY > tempY)?(colorXy.colorY - tempY):(tempY - colorXy.colorY);
     /* verify the min */
    if(currentDiffX < minDiffX && currentDiffY < minDiffY)
    {
        minDiffX = currentDiffX;
        minDiffY = currentDiffY;
        *hue = i;
    }
  }
  
  /* get saturation based on White X, gColorCtrlHueToXTable[hue]  values */
  tempX = gColorCtrlDefaultWhiteX_c;
  minDiffX = (gColorCtrlHueToXTable[*hue] > tempX)?(gColorCtrlHueToXTable[*hue] - tempX):(tempX - gColorCtrlHueToXTable[*hue]);
  currentDiffX = (gColorCtrlHueToXTable[*hue] > colorXy.colorX)?(gColorCtrlHueToXTable[*hue]- colorXy.colorX):(colorXy.colorX - gColorCtrlHueToXTable[*hue]);
  
  *saturation = (uint8_t)(gColorCtrlSaturationMaxValue_c - ((gColorCtrlSaturationMaxValue_c*currentDiffX)/minDiffX)); 
}

/*!
 * @fn 		static void ColorCotrolServer_GetHueDirection(uint8_t indexSetup, uint16_t requiredHue, uint16_t currentHue, uint8_t direction)
 *
 * @brief	update level difference and direction mode
 */
static void ColorCotrolServer_GetHueDirection(uint8_t indexSetup, uint16_t requiredHue, uint16_t currentHue, uint8_t direction, uint16_t maxValue)
{
  uint16_t hueDiff;
  if(requiredHue > currentHue)
  {
    hueDiff = requiredHue - currentHue;      
    if(((direction == gColorCtrlDirection_ShortestDistance_c) && (hueDiff > maxValue/2))||
      ((direction == gColorCtrlDirection_LongestDistance_c) && (hueDiff < maxValue/2))||
        (direction == gColorCtrlDirection_Down_c))
    {
        gColorCtrlServerSetup[indexSetup].hueDirection = gColorCtrlDirection_Down_c;
    }
    else
    {
      if(((direction == gColorCtrlDirection_LongestDistance_c) && (hueDiff > maxValue/2))||
          ((direction == gColorCtrlDirection_ShortestDistance_c) && (hueDiff< maxValue/2))||
           (direction == gColorCtrlDirection_Up_c))
      {
        gColorCtrlServerSetup[indexSetup].hueDirection = gColorCtrlDirection_Up_c;
      }
    }
  }
  else
  {
    hueDiff = currentHue - requiredHue;
    gColorCtrlServerSetup[indexSetup].hueDirection = (direction == gColorCtrlDirection_Up_c)?gColorCtrlDirection_Up_c:gColorCtrlDirection_Down_c;
    if(hueDiff > 0x00)
    {
      if(((direction == gColorCtrlDirection_ShortestDistance_c) && (hueDiff > maxValue/2))||
          ((direction == gColorCtrlDirection_LongestDistance_c) && (hueDiff < maxValue/2))||
            (direction == gColorCtrlDirection_Up_c))
      {
         gColorCtrlServerSetup[indexSetup].hueDirection = gColorCtrlDirection_Up_c;
      }
      else
      {
        if(((direction == gColorCtrlDirection_LongestDistance_c) && (hueDiff > maxValue/2))||
          ((direction == gColorCtrlDirection_ShortestDistance_c) && (hueDiff < maxValue/2))||
            (direction == gColorCtrlDirection_Down_c))
        {
            gColorCtrlServerSetup[indexSetup].hueDirection = gColorCtrlDirection_Down_c;
        }
      }
    }
  }
}

/*!
 * @fn 		static void ColorCtrlServer_UpdateValues(uint32_t *currentValue, uint32_t *transitionTime, uint32_t requiredValue, uint32_t max, uint32_t min, uint8_t direction)
 *
 * @brief	Update color Ctrl values
 *
 */
static void ColorCtrlServer_UpdateValues(uint32_t *currentValue, uint32_t *transitionTime, uint32_t requiredValue, uint32_t max, uint32_t min, uint8_t direction)
{
  uint32_t step, diff;	

  /* verify transition time */
  if( *transitionTime == 1)
  {
    *currentValue = requiredValue;
    *transitionTime = *transitionTime - 1;
    return;
  }
  
  /* get level difference */
  if(requiredValue > *currentValue)
  { 
     diff = requiredValue - *currentValue;
     if(direction  == gColorCtrlDirection_Down_c)
       diff = max-min-diff;
  }
  else
  {
      diff =  *currentValue - requiredValue;
      if((direction  == gColorCtrlDirection_Up_c) || (diff == 0x00))
          diff = max-min-diff;
  }

  /* get step */
  step = diff/(*transitionTime);
  
  /* update currentValue */
  if(direction  == gColorCtrlDirection_Down_c)
  {
      *currentValue = (*currentValue >= step + min)?(*currentValue-step):(max-min-step+(*currentValue));		
  }
  else
  {
      *currentValue = (*currentValue + step <= max)?(*currentValue+step):(max-min -(*currentValue) + step);	
  }
  *transitionTime = *transitionTime - 1;
}
/*!
 * @fn 		static void ColorCtrlServer_UpdateHueValue(uint8_t indexSetup, uint8_t colorMode)
 *
 * @brief	Update color Ctrl values
 *
 */
static void ColorCtrlServer_UpdateHueValue(uint8_t indexSetup, uint8_t colorMode)
{ 
  uint32_t tempHue, tempTransitionTime;
  uint16_t currentHueValue;
  zbClusterId_t clusterId = {gaZclClusterColorControl_c};
  uint32_t maxValue = gColorCtrlHueMaxValue_c;
  uint32_t minValue = gColorCtrlHueMinValue_c;
  uint8_t hueValue;
  
  if(colorMode == gColorCtrlEnhancedColorMode_EnhHueSaturation_c)
  {
#if gColorCtrlEnableZllFunctionality_c    
    maxValue = gColorCtrlEnhancedHueMaxValue_c;
    minValue = gColorCtrlEnhancedHueMinValue_c;
   (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlEnhancedCurrentHue_c, gZclServerAttr_c, &currentHueValue, NULL);  
#endif  
  }
  else
  {
    (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentHue_c, gZclServerAttr_c, &hueValue, NULL);  
    currentHueValue = hueValue;
  }
  
  tempHue = currentHueValue<<16;
  tempTransitionTime = gColorCtrlServerSetup[indexSetup].hueTransitionTime;
  if(tempTransitionTime)
  {
    ColorCtrlServer_UpdateValues(&tempHue, &tempTransitionTime, gColorCtrlServerSetup[indexSetup].hueReqValue,(maxValue<<16), 
                                  (minValue<<16), gColorCtrlServerSetup[indexSetup].hueDirection);
  
    gColorCtrlServerSetup[indexSetup].hueTransitionTime = tempTransitionTime;
  }
  tempHue = tempHue>>16;
  if(currentHueValue != (uint16_t)tempHue)
  {
    zclColorCtrl_colorXY_t colorXy;
    uint8_t currentSaturation; 
    
    currentHueValue = (uint16_t)tempHue;
    /* set current HUE*/
    if(colorMode == gColorCtrlEnhancedColorMode_EnhHueSaturation_c)
    {
      hueValue = (uint8_t)(currentHueValue>>8);      
    }
    else
    {
      hueValue = (uint8_t)currentHueValue; 
#if gColorCtrlEnableZllFunctionality_c  
      uint16_t tempHueValue;   
      (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlEnhancedCurrentHue_c, gZclServerAttr_c, &tempHueValue, NULL);
      /* update Enahanced value: upper 8 bits = Hue value, lower 8 bits = interpolate btw hue Values*/
      currentHueValue = (currentHueValue<<8) + (tempHueValue & 0x00FF);
#endif
      
    }
    
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentHue_c, gZclServerAttr_c, &hueValue);
#if gColorCtrlEnableZllFunctionality_c  
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlEnhancedCurrentHue_c, gZclServerAttr_c, &currentHueValue);
#endif
    
    /* refresh XY values */
    (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint,  clusterId, gZclAttrColorCtrlCurrentSaturation_c, gZclServerAttr_c, &currentSaturation, NULL); 
    ColorCtrlServer_RefreshXyValues(hueValue, currentSaturation, &colorXy);
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentX_c, gZclServerAttr_c, &colorXy.colorX);
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentY_c, gZclServerAttr_c, &colorXy.colorY);
  }
}

/*!
 * @fn 		static void ColorCtrlServer_UpdateSaturationValue(uint8_t indexSetup)
 *
 * @brief	Update Saturation Attribute value
 *
 */
static void ColorCtrlServer_UpdateSaturationValue(uint8_t indexSetup)
{
  uint8_t  currentSat, satDirection;    
  uint32_t tempSat, tempTransitionTime;
  zbClusterId_t clusterId = {gaZclClusterColorControl_c};
    
  
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentSaturation_c, gZclServerAttr_c, &currentSat, NULL);
  tempSat = currentSat<<16;
  
  satDirection = (tempSat<gColorCtrlServerSetup[indexSetup].satReqValue)?gColorCtrlStepMode_Up_c:gColorCtrlStepMode_Down_c;
  tempTransitionTime = gColorCtrlServerSetup[indexSetup].satTransitionTime;
  if(tempTransitionTime)
  {
    ColorCtrlServer_UpdateValues(&tempSat, &tempTransitionTime, gColorCtrlServerSetup[indexSetup].satReqValue,(uint32_t)(gColorCtrlSaturationMaxValue_c<<16), 
                                  (uint32_t)(gColorCtrlSaturationMinValue_c<<16), satDirection);
    gColorCtrlServerSetup[indexSetup].satTransitionTime = tempTransitionTime;
  }
  tempSat = tempSat>>16;
  
  if(currentSat != (uint8_t)tempSat)
  {
    zclColorCtrl_colorXY_t colorXy;
    uint8_t currentHue = 0x00;
    
    /* set current saturation*/
    currentSat = (uint8_t)tempSat;
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentSaturation_c, gZclServerAttr_c, &currentSat);
    
    /* refresh XY values */
    (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint,  clusterId, gZclAttrColorCtrlCurrentHue_c, gZclServerAttr_c, &currentHue, NULL); 
    ColorCtrlServer_RefreshXyValues(currentHue, currentSat, &colorXy);
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentX_c, gZclServerAttr_c, &colorXy.colorX);
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentY_c, gZclServerAttr_c, &colorXy.colorY);
  }
}

/*!
 * @fn 		static void ColorCtrlServer_UpdateColorXyValues(uint8_t indexSetup)
 *
 * @brief	Update ColorX, ColorY Attribute values
 *
 */
static void ColorCtrlServer_UpdateColorXyValues(uint8_t indexSetup)
{
  uint32_t tempColorX, tempColorY, tempTransitionTime;
  zclColorCtrl_colorXY_t colorXY;
  uint8_t colorxDir, coloryDir;
  zbClusterId_t clusterId = {gaZclClusterColorControl_c};
  uint8_t refreshHueSat = FALSE;
    
  /* get current values */
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentX_c, gZclServerAttr_c, &colorXY.colorX, NULL);
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentY_c, gZclServerAttr_c, &colorXY.colorY, NULL);
  tempColorX = colorXY.colorX<<16;
  tempColorY = colorXY.colorY<<16;
 
  colorxDir = (tempColorX < gColorCtrlServerSetup[indexSetup].colorxReqValue)?gColorCtrlStepMode_Up_c:gColorCtrlStepMode_Down_c;
  coloryDir = (tempColorY < gColorCtrlServerSetup[indexSetup].coloryReqValue)?gColorCtrlStepMode_Up_c:gColorCtrlStepMode_Down_c;
  
  tempTransitionTime = gColorCtrlServerSetup[indexSetup].colorxTransitionTime;
  if(tempTransitionTime)
  {
    ColorCtrlServer_UpdateValues(&tempColorX, &tempTransitionTime, gColorCtrlServerSetup[indexSetup].colorxReqValue,(uint32_t)(gColorCtrlColorMaxValue_c<<16), 
                                  (uint32_t)(gColorCtrlColorMinValue_c<<16), colorxDir);
    gColorCtrlServerSetup[indexSetup].colorxTransitionTime = tempTransitionTime;
  }
  
  tempTransitionTime = gColorCtrlServerSetup[indexSetup].coloryTransitionTime;
  if(tempTransitionTime)
  {
    ColorCtrlServer_UpdateValues(&tempColorY, &tempTransitionTime, gColorCtrlServerSetup[indexSetup].coloryReqValue,(uint32_t)(gColorCtrlColorMaxValue_c<<16), 
                                  (uint32_t)(gColorCtrlColorMinValue_c<<16), coloryDir);
    gColorCtrlServerSetup[indexSetup].coloryTransitionTime = tempTransitionTime;
  }
  
  tempColorX = tempColorX>>16;
  tempColorY = tempColorY>>16;
  
  if(colorXY.colorX != (uint16_t)tempColorX)
  {  
    /* set currentX value*/
    colorXY.colorX = (uint16_t)tempColorX;
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentX_c, gZclServerAttr_c, &colorXY.colorX);
    refreshHueSat = TRUE;
  }
  if(colorXY.colorY != (uint16_t)tempColorY)
  {  
    /* set currentY value*/
    colorXY.colorY = (uint16_t)tempColorY;
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentY_c, gZclServerAttr_c, &colorXY.colorY);
    refreshHueSat = TRUE;
  }
  
  if(refreshHueSat)
  {
    uint8_t hue, saturation;
#if gASL_EnableZLLClustersData_d      
    uint16_t enhancedHue;
#endif    
    /* refresh hue and saturation values */
     ColorCtrlServer_RefreshHueSatValues(colorXY, &hue, &saturation);
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentHue_c, gZclServerAttr_c, &hue);
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlCurrentSaturation_c, gZclServerAttr_c, &saturation);
#if gASL_EnableZLLClustersData_d    
    enhancedHue = hue<<8;
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlEnhancedCurrentHue_c, gZclServerAttr_c, &enhancedHue);
#endif  
  }
}

/*!
 * @fn 		static void ColorCtrlServer_UpdateRemainingTime(uint8_t indexSetup)
 *
 * @brief	Update Remaining Time Attribute value
 *
 */
static void ColorCtrlServer_UpdateRemainingTime(uint8_t indexSetup, uint16_t remainingTime)
{  
  uint16_t  currentRemainingTime;    
  zbClusterId_t clusterId = {gaZclClusterColorControl_c};
  
  /* get current remaining Time */
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint,  clusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &currentRemainingTime, NULL);
  
  if(currentRemainingTime < remainingTime)
  {
    /* update remaining time */
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint,  clusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTime);
  }
}

/*!
 * @fn 		static void ColorCtrl_ResetColorXyLocalTable(uint8_t index)
 *
 * @brief	reset color x, color y values from local table
 *
 */
static void ColorCtrl_ResetColorXyLocalTable(uint8_t index)
{ 
  gColorCtrlServerSetup[index].colorxReqValue = 0x00;
  gColorCtrlServerSetup[index].coloryReqValue = 0x00; 
  gColorCtrlServerSetup[index].colorxTransitionTime = 0x00;    
  gColorCtrlServerSetup[index].coloryTransitionTime = 0x00;    
}

/*!
 * @fn 		static void ColorCtrl_ResetHueSaturationLocalTable(uint8_t index)
 *
 * @brief	reset hue, saturation values from local table
 *
 */
static void ColorCtrl_ResetHueSaturationLocalTable(uint8_t index)
{
  
  gColorCtrlServerSetup[index].hueReqValue = 0x00;
  gColorCtrlServerSetup[index].hueTransitionTime = 0x00; 
  
  gColorCtrlServerSetup[index].satReqValue = 0x00;
  gColorCtrlServerSetup[index].satTransitionTime = 0x00; 
  
}

/*!
 * @fn 		static zbStatus_t ColorCtrlServer_SetTimeout(uint8_t indexSetup, uint32_t duration)
 *
 * @brief	Set color Ctrl timeout. Duration is in miliseconds
 *
 */
static zbStatus_t ColorCtrlServer_SetTimeout(uint8_t indexSetup, uint32_t duration)
{ 
  if(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId == gTmrInvalidTimerID_c)
  {
    gColorCtrlServerSetup[indexSetup].colorTransitionTmrId = ZbTMR_AllocateTimer(); 
    if(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId == gTmrInvalidTimerID_c)
      return gZclFailure_c;
  }
  
  ZbTMR_StartTimer(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId, gTmrSingleShotTimer_c, duration, ColorCtrlServer_TimerCallBack);
  return gZclSuccess_c;
}
 
/*!
 * @fn 		void ColorCtrlServer_StopActiveTimer(uint8_t endpoint)
 *
 * @brief	Stop active color control cluster action
 *
 */
void ColorCtrlServer_StopActiveTimer(uint8_t endpoint)
{
  uint8_t indexSetup;
  bool_t activeLoop = FALSE;
  zbClusterId_t aColorClusterId = {gaZclClusterColorControl_c}; 
  
  /* verify color Ctrl table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(endpoint, gTmrInvalidTimerID_c);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return;
  
   /* de-activate the color loop */
   (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint,  aColorClusterId, gZclAttrColorCtrlColorLoopActive_c, gZclServerAttr_c, &activeLoop);
   if(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId != gTmrInvalidTimerID_c)
      ZbTMR_StopTimer(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId); 
   
   /* reset color loop active table */
   ColorCtrl_ResetColorXyLocalTable(indexSetup);
   ColorCtrl_ResetHueSaturationLocalTable(indexSetup);
}

/*!
 * @fn 		static void ColorCtrlServer_TimerCallBack(uint8_t tmrId)
 *
 * @brief	color Ctrl timer callback
 *
 */
static void ColorCtrlServer_TimerCallBack(uint8_t tmrId)
{
  uint8_t indexSetup,colorMode;
  zbClusterId_t clusterId = {gaZclClusterColorControl_c};
  zbClusterId_t clusterIdOnOff = {gaZclClusterOnOff_c};
  uint8_t onOffStatus = 0xFF;
  uint16_t remainingTimeValue;
  uint16_t colorModeAttrId = gZclAttrColorCtrlColorMode_c;
  
  /* verify color Ctrl table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(gZclCluster_InvalidDataIndex_d, tmrId);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return;
   
  /* verify OnOff attribute */
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterIdOnOff, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffStatus, NULL);
  if(onOffStatus != 0x01)
  {
    /* ignore the command */
    ColorCtrl_ResetColorXyLocalTable(indexSetup);
    ColorCtrl_ResetHueSaturationLocalTable(indexSetup);
    /* reset remaining time attribute */
    remainingTimeValue = 0x00;
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTimeValue);
    ZbTMR_StopTimer(tmrId);
    return;
  }
  
  /* verify color Mode/enhanced color Mode */  
#if gColorCtrlEnableZllFunctionality_c  
  colorModeAttrId = gZclAttrColorCtrlEnhancedColorMode_c;
#endif  
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, colorModeAttrId, gZclServerAttr_c, &colorMode, NULL);
  if(colorMode == gColorCtrlEnhancedColorMode_ColorTemperature_c ||
     (colorMode > gColorCtrlEnhancedColorMode_EnhHueSaturation_c && colorModeAttrId == gZclAttrColorCtrlEnhancedColorMode_c) ||
       (colorMode > gColorCtrlColorMode_CurrentXY_c && colorModeAttrId == gZclAttrColorCtrlColorMode_c))
  {
    /* ignore the command */
    ZbTMR_StopTimer(tmrId);
    return;
  }
  
  if((colorMode == gColorCtrlColorMode_CurrentModeSaturation_c)||
     (colorMode == gColorCtrlEnhancedColorMode_EnhHueSaturation_c))
  {
    /* verify hue transition time */
    if(gColorCtrlServerSetup[indexSetup].hueTransitionTime)
    {
      ColorCtrlServer_UpdateHueValue(indexSetup, colorMode);
    }
  
    /* verify saturation transition time */
    if(gColorCtrlServerSetup[indexSetup].satTransitionTime)
    {
      ColorCtrlServer_UpdateSaturationValue(indexSetup);
    }
  }
  
  if(colorMode == gColorCtrlColorMode_CurrentXY_c)
  {
    /* verify color x,y transition time */
    if(gColorCtrlServerSetup[indexSetup].colorxTransitionTime || 
       gColorCtrlServerSetup[indexSetup].coloryTransitionTime)
    {
      ColorCtrlServer_UpdateColorXyValues(indexSetup);
    }
  }
  
  /* send event to the application */
  BeeAppUpdateDevice(gColorCtrlServerSetup[indexSetup].endpoint, gZclUI_GoToLevel_c, 0, 0, NULL);
  
   /* update the remaining time */
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTimeValue, NULL);
  if(remainingTimeValue)
  {
    remainingTimeValue -= 1;
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTimeValue);
  }
  
  if((remainingTimeValue) && (gColorCtrlServerSetup[indexSetup].hueTransitionTime || 
        gColorCtrlServerSetup[indexSetup].satTransitionTime || gColorCtrlServerSetup[indexSetup].colorxTransitionTime || 
          gColorCtrlServerSetup[indexSetup].coloryTransitionTime))
    ColorCtrlServer_SetTimeout(indexSetup, 100); /* 100 miliseconds*/
  else
    ZbTMR_StopTimer(tmrId);
}
/*!
 * @fn 		static zbStatus_t ColorCtrlServer_ActivateColorLoop(uint8_t indexSetup)
 *
 * @brief	activate color loop 
 *
 */
static zbStatus_t ColorCtrlServer_ActivateColorLoop(uint8_t indexSetup)
{
  zbClusterId_t clusterId = {gaZclClusterColorControl_c};
  uint16_t secondsTransitionTime, enhancedHue;
  
  if(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId == gTmrInvalidTimerID_c)
  {
    gColorCtrlServerSetup[indexSetup].colorTransitionTmrId = ZbTMR_AllocateTimer(); 
    if(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId == gTmrInvalidTimerID_c)
      return gZclFailure_c;
  }
  
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlColorLoopTime_c, gZclServerAttr_c, &secondsTransitionTime, NULL);
  gColorCtrlServerSetup[indexSetup].hueTransitionTime = secondsTransitionTime*10; /*  100 miliseconds */
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlColorLoopDirection_c, gZclServerAttr_c, &gColorCtrlServerSetup[indexSetup].hueDirection, NULL);
  gColorCtrlServerSetup[indexSetup].hueDirection =(gColorCtrlServerSetup[indexSetup].hueDirection == gColorCtrlDirection_DecrementHue_c)?gColorCtrlDirection_Down_c:gColorCtrlDirection_Up_c;
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlEnhancedCurrentHue_c, gZclServerAttr_c, &enhancedHue, NULL);
  gColorCtrlServerSetup[indexSetup].hueReqValue = enhancedHue<<16;

  ZbTMR_StartTimer(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId, gTmrSingleShotTimer_c, 100, ColorCtrlServer_ColorLoopCallBack);
  return gZclSuccess_c;
}
/*!
 * @fn 		static void ColorCtrlServer_ColorLoopCallBack(uint8_t tmrId)
 *
 * @brief	color Ctrl - color loop callback
 *
 */
static void ColorCtrlServer_ColorLoopCallBack(uint8_t tmrId)
{
  uint8_t indexSetup;
  zbClusterId_t clusterIdOnOff = {gaZclClusterOnOff_c};
  zbClusterId_t clusterId = {gaZclClusterColorControl_c};
  uint8_t onOffStatus = 0xFF;
  
    /* verify color Ctrl table setup*/
  indexSetup = ColorCtrlServer_GetIndexFromColorSetupTable(gZclCluster_InvalidDataIndex_d, tmrId);
  if(indexSetup == gZclCluster_InvalidDataIndex_d)
    return;
   
  /* verify OnOff attribute */
  (void)ZCL_GetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterIdOnOff, gZclAttrOnOff_OnOffId_c, gZclServerAttr_c, &onOffStatus, NULL);
  if(onOffStatus != 0x01)
  {
    uint16_t remainingTimeValue;
    
    /* ignore the command */
    ColorCtrl_ResetColorXyLocalTable(indexSetup);
    ColorCtrl_ResetHueSaturationLocalTable(indexSetup);
    /* reset remaining time attribute */
    remainingTimeValue = 0x00;
    (void)ZCL_SetAttribute(gColorCtrlServerSetup[indexSetup].endpoint, clusterId, gZclAttrColorCtrlRemainingTime_c, gZclServerAttr_c, &remainingTimeValue);
    ZbTMR_StopTimer(tmrId);
    return;
  }
  
  /* update color Ctrl atributes */
  ColorCtrlServer_UpdateHueValue(indexSetup, gColorCtrlEnhancedColorMode_EnhHueSaturation_c);
  
  /* send event to the application */
  BeeAppUpdateDevice(gColorCtrlServerSetup[indexSetup].endpoint, gZclUI_GoToLevel_c, 0, 0, NULL);
  
  /* verify remaining time */
  if(gColorCtrlServerSetup[indexSetup].hueTransitionTime)
    ZbTMR_StartTimer(gColorCtrlServerSetup[indexSetup].colorTransitionTmrId, gTmrSingleShotTimer_c, 100, ColorCtrlServer_ColorLoopCallBack);
  else
    (void)ColorCtrlServer_ActivateColorLoop(indexSetup);

}

