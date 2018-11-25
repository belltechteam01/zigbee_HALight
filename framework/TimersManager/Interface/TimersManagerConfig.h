/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file TimersManagerConfig.h
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


#ifndef _TIMERSMANAGER_CONFIG_H_
#define _TIMERSMANAGER_CONFIG_H_

#ifndef gTMR_Enabled_d
#define gTMR_Enabled_d                     1
#endif
#ifndef gTimestamp_Enabled_d
#define gTimestamp_Enabled_d               1
#endif
#ifndef gTMR_PIT_Timestamp_Enabled_d
#define gTMR_PIT_Timestamp_Enabled_d       1
#endif
#ifndef gTMR_PIT_FreqMultipleOfMHZ_d
#define gTMR_PIT_FreqMultipleOfMHZ_d       1
#endif
#ifndef gTMR_EnableLowPowerTimers_d
#define gTMR_EnableLowPowerTimers_d        1
#endif
#ifndef gTMR_EnableHWLowPowerTimers_d
#define gTMR_EnableHWLowPowerTimers_d      0
#endif
#ifndef gTMR_EnableMinutesSecondsTimers_d
#define gTMR_EnableMinutesSecondsTimers_d  1
#endif
#ifndef gTmrApplicationTimers_c
#define gTmrApplicationTimers_c            4
#endif
#ifndef gTmrStackTimers_c
#define gTmrStackTimers_c                  4
#endif
#ifndef gTmrTaskStackSize_c
#define gTmrTaskStackSize_c                600
#endif
#ifndef gTmrTaskPriority_c
#define gTmrTaskPriority_c                 2
#endif

#endif /* _TIMERSMANAGER_CONFIG_H_ */
