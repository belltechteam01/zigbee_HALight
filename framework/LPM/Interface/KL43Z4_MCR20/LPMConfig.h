/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file LPMConfig.h
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

#ifndef _LPM_CONFIG_H_
#define _LPM_CONFIG_H_

#ifndef cPWR_LVD_Enable
#define cPWR_LVD_Enable                        0
#endif
#ifndef cPWR_LVD_Ticks
#define cPWR_LVD_Ticks                         60
#endif
#ifndef cPWR_UsePowerDownMode
#define cPWR_UsePowerDownMode                  1
#endif
#ifndef gPWR_EnsureOscStabilized_d
#define gPWR_EnsureOscStabilized_d             0
#endif
#ifndef cPWR_DeepSleepMode
#define cPWR_DeepSleepMode                     9
#endif
#ifndef cPWR_SleepMode
#define cPWR_SleepMode                         1
#endif
#ifndef cPWR_LPTMRTickTimeSource_ERCLK32K
#define cPWR_LPTMRTickTimeSource_ERCLK32K      cLPTMR_PRS_1000ms
#endif
#ifndef cPWR_TMRTicks
#define cPWR_TMRTicks                          3
#endif
#ifndef cPWR_DeepSleepDurationMs
#define cPWR_DeepSleepDurationMs               3000
#endif
#ifndef cPWR_CallWakeupStackProcAfterDeepSleep
#define cPWR_CallWakeupStackProcAfterDeepSleep 0
#endif

#endif //_LPM_CONFIG_H_
