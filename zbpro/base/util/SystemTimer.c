/*! *********************************************************************************
* \file SystemTimer.c
* This is the source file for the TimerTaskUtilTask. Call Fwk Timer Manager functions used 
* in zigbee internal timer module
*
* Copyright (c) 2013, Freescale Semiconductor, Inc.
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
********************************************************************************** */

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "EmbeddedTypes.h"
#include "TimersManager.h"
   
/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/* None */

/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/

 /* None */

/*! *********************************************************************************
*************************************************************************************
* Private memory definitions
*************************************************************************************
************************************************************************************/

 /* None */

/*! *********************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

 /* None */

/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*---------------------------------------------------------------------------
 * NAME: ZbTmrAllocateSystemTimer
 * DESCRIPTION: Reserve a system timer
 * PARAMETERS: -
 * RETURN: gTmrInvalidTimerID_c if there are no timers available
 * NOTES: none
 *---------------------------------------------------------------------------*/
tmrTimerID_t ZbTmrAllocateSystemTimer(void)
{
  return TMR_AllocateTimer();
}

/*---------------------------------------------------------------------------
 * NAME: ZbTmrStopSystemTimer
 * DESCRIPTION: Stop a system timer
 * PARAMETERS:  IN: timerID - the ID of the tsystem imer
 * RETURN: None
 * NOTES: Associated timer callback function is not called, even if the timer
 *        expires. Does not frees the timer. Safe to call anytime, regardless
 *        of the state of the timer.
 *---------------------------------------------------------------------------*/

void ZbTmrStopSystemTimer(tmrTimerID_t timerID)
{
  TMR_StopTimer(timerID);
}

/*---------------------------------------------------------------------------
 * NAME: ZbTMRStartSystemTimer 
 * DESCRIPTION: Start a specified system timer
 * PARAMETERS: IN: timerId - the ID of the timer
 *             IN: timerType - the type of the timer
 *             IN: timeInMilliseconds - time expressed in millisecond units
 *             IN: pfTmrCallBack - callback function
  *            IN: param - parameter to callback function
 * RETURN: -
 * NOTES: When the timer expires, the callback function is called in
 *        non-interrupt context. If the timer is already running when
 *        this function is called, it will be stopped and restarted.
 *---------------------------------------------------------------------------*/
void ZbTMRStartSystemTimer
(
    tmrTimerID_t timerID,
    tmrTimerType_t timerType,
    tmrTimeInMilliseconds_t timeInMilliseconds,
    void (*zbTmrSystemCallBack)(void *),
    void *param
)
{
  TMR_StartTimer(timerID,timerType,timeInMilliseconds,zbTmrSystemCallBack,param);
}

/*---------------------------------------------------------------------------
 * NAME: ZbTMRGetRemainingTimeSystemTimer
 * DESCRIPTION: Returns the remaining time until timeout, for the specified
 *              system timer
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: remaining time in milliseconds until next timer timeout
 *---------------------------------------------------------------------------*/
uint32_t ZbTMRGetRemainingTimeSystemTimer
(
  tmrTimerID_t tmrID
)
{
  return TMR_GetRemainingTime(tmrID);
}

