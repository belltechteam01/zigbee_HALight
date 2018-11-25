/*! *********************************************************************************
* \file RtosZigbeeTask.c
* This is the header file for the Zigbee stack internal timer module.
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

#ifndef __ZIGBEE_TMR_INTERFACE_H__
#define __ZIGBEE_TMR_INTERFACE_H__

#ifdef __cplusplus
    extern "C" {
#endif

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "EmbeddedTypes.h"

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

/*
 * NAME: gZbTMR_EnableMinutesSecondsTimers_d
 * DESCRIPTION:  Enable/Disable Minutes and Seconds Timers
 * VALID RANGE: TRUE/FALSE
 */
#ifndef gZbTMR_EnableMinutesSecondsTimers_d
#define gZbTMR_EnableMinutesSecondsTimers_d	1
#endif

/*
 * NAME: ZbTmrMilliseconds()
 * DESCRIPTION: Typecast the macro argument into milliseconds
 * VALID RANGE: -
 */
#define ZbTmrMilliseconds( n )	( (zbTmrTimeInMilliseconds_t) (n) )

/*
 * NAME: ZbTmrSeconds()
 * DESCRIPTION: Converts the macro argument (i.e. seconds) into milliseconds
 * VALID RANGE: - 
 */
#define ZbTmrSeconds( n )	         ( (zbTmrTimeInMilliseconds_t) (ZbTmrMilliseconds(n) * 1000) )

/*
 * NAME: ZbTmrMinutes()
 * DESCRIPTION: Converts the macro argument (i.e. minutes) into milliseconds
 * VALID RANGE: -
 */
#define ZbTmrMinutes( n )		  ( (zbTmrTimeInMilliseconds_t) (ZbTmrSeconds(n) * 60) )

/*
 * NAME: gTmrInvalidTimerID_c
 * DESCRIPTION: Reserved for invalid timer id
 * VALID RANGE: 0xFF
 */
#define gTmrInvalidTimerID_c	0xFF

/*
 * NAME: gTmrSingleShotTimer_c, gTmrIntervalTimer_c,
 *       gTmrSetMinuteTimer_c, gTmrSetSecondTimer_c,
 *       gTmrLowPowerTimer_c
 * DESCRIPTION: Timer types coded values
 * VALID RANGE: see definitions below
 */
#define gTmrSingleShotTimer_c	0x01
#define gTmrIntervalTimer_c     0x02
#define gTmrSetMinuteTimer_c	0x04
#define gTmrSetSecondTimer_c	0x08
#define gTmrLowPowerTimer_c     0x10

/*
 * NAME: gTmrMinuteTimer_c
 * DESCRIPTION: Minute timer definition
 * VALID RANGE: see definition below
 */
#define gTmrMinuteTimer_c       ( gTmrSetMinuteTimer_c )

/*
 * NAME: gTmrSecondTimer_c
 * DESCRIPTION: Second timer definition
 * VALID RANGE: see definition below
 */
#define gTmrSecondTimer_c       ( gTmrSetSecondTimer_c )

/*
 * NAME: See below
 * DESCRIPTION: LP minute/second/millisecond timer definitions
 * VALID VALUES: See definitions below
 */
#define gTmrLowPowerMinuteTimer_c	     ( gTmrMinuteTimer_c     | gTmrLowPowerTimer_c )
#define gTmrLowPowerSecondTimer_c	     ( gTmrSecondTimer_c     | gTmrLowPowerTimer_c )
#define gTmrLowPowerSingleShotMillisTimer_c  ( gTmrSingleShotTimer_c | gTmrLowPowerTimer_c )
#define gTmrLowPowerIntervalMillisTimer_c    ( gTmrIntervalTimer_c   | gTmrLowPowerTimer_c )

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/ 
/*
 * NAME: tmrTimerTicks_t
 * DESCRIPTION: 16-bit timer ticks type definition
 * VALID VALUES: see definition
 */
typedef uint16_t zbTmrTimerTicks16_t;

/*
 * NAME: tmrTimerTicks_t
 * DESCRIPTION: 32-bit timer ticks type definition
 * VALID VALUES: see definition
 */
typedef uint32_t zbTmrTimerTicks32_t;

/*
 * NAME: zbTmrTimeInMilliseconds_t
 * DESCRIPTION: Times specified in milliseconds (max 0x3ffff)
 */
typedef uint32_t	zbTmrTimeInMilliseconds_t;

/*
 * NAME: zbTmrTimeInMinutes_t
 * DESCRIPTION: Times specified in minutes (up to 40 days)
 */
typedef uint32_t	zbTmrTimeInMinutes_t;

/*
 * NAME: zbTmrTimeInSeconds_t
 * DESCRIPTION: Times specified in seconds (up to 65535)
 */
typedef uint32_t	zbTmrTimeInSeconds_t;

/*
 * NAME: zbTmrTimerID_t
 * DESCRIPTION: Timer type
 */
typedef uint8_t		zbTmrTimerID_t;

/*
 * NAME: zbTmrTimerType_t
 * DESCRIPTION: Timer type
 */
typedef uint8_t		zbTmrTimerType_t;

/*
 * NAME: pfZbTmrCallBack_t
 * DESCRIPTION: Timer callback function
 */
typedef void ( *pfZbTmrCallBack_t ) ( zbTmrTimerID_t );

/*
 * Type name: tmrTimerTableEntry_tag
 * Type description: One entry in the main timer table.
 * Members: intervalInTicks - The timer's original duration, in ticks.
 *                            Used to reset intervnal timers.
 *
 *          countDown - When a timer is started, this is set to the duration.
 *                      The timer task decrements this value. When it reaches
 *                      zero, the timer has expired.
 *          pfCallBack - Pointer to the callback function
 */

/*
 * Type name: zbTmrTimerStatus_t
 * Type description: The status and type are bitfields, to save RAM.
 *                   This costs some code space, though.
 * Members: N/A
 */
typedef uint8_t zbTmrTimerStatus_t;

/*
 * NAME: zbTmrStatus_t
 * DESCRIPTION: timer status - see the status macros.
 *              If none of these flags are on, the timer is not allocated.
 *              For allocated timers, exactly one of these flags will be set.
 *              mTmrStatusActive_c - Timer has been started and has not yet expired.
 *              mTmrStatusReady_c - ZbTMR_StartTimer() has been called for this timer, but
 *                                  the timer task has not yet actually started it. The
 *                                  timer is considered to be active.
 *              mTmrStatusInactive_c Timer is allocated, but is not active.
 */
typedef uint8_t zbTmrStatus_t;

typedef struct tmrTimerTableEntry_tag 
{
  zbTmrTimeInMilliseconds_t     intervalInMs;
  zbTmrTimeInMilliseconds_t     remainingMs;
  pfZbTmrCallBack_t             pfCallBack;
  zbTmrStatus_t                 tmrTimerStatusTable;
} tmrTimerTableEntry_t;

/*! *********************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/ 

/* None */

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/*---------------------------------------------------------------------------
 * NAME: TMR_ChangeInstance
 * DESCRIPTION: Beestack instance change for timers 
 *              Called from beestack rtos task before using timer module.
 * PARAMETERS:  IN: pointer to timers table and timer number
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
void ZbTMR_SetInstance
(
  tmrTimerTableEntry_t* pTmrTimerTable,
  zbTmrTimerID_t osTimer,
  uint8_t noOfTimers,
  zbTmrTimeInMilliseconds_t* pNextIntTime,
  void (*zbTmrSystemCallBack)(void *),
  void *param
 );

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_Handler
 * DESCRIPTION: Beestack instance timer handler. 
 *              Called by the rtos beestack task  when the RTOS timer callback 
 *              posts a timer event.
 * PARAMETERS:  IN: events - timer events mask
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
void ZbTMR_Handler(void);

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_Init
 * DESCRIPTION: initialize the timer module
 * PARAMETERS: -
 * RETURN: -
 *---------------------------------------------------------------------------*/
extern void ZbTMR_Init 
(
    void
);



/*---------------------------------------------------------------------------
 * NAME: ZbTMR_AllocateTimer
 * DESCRIPTION: allocate a timer
 * PARAMETERS: -
 * RETURN: timer ID
 *---------------------------------------------------------------------------*/
extern zbTmrTimerID_t ZbTMR_AllocateTimer
(
    void
);
                                     
/*---------------------------------------------------------------------------
 * NAME: ZbTMR_AreAllTimersOff
 * DESCRIPTION: Check if all timers except the LP timers are OFF.
 * PARAMETERS: -
 * RETURN: TRUE if there are no active non-low power timers, FALSE otherwise
 *---------------------------------------------------------------------------*/
extern bool_t ZbTMR_AreAllTimersOff
(
    void
);                                      

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_FreeTimer
 * DESCRIPTION: Free a timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: -
 * NOTES: Safe to call even if the timer is running.
 *        Harmless if the timer is already free.
 *---------------------------------------------------------------------------*/
extern void ZbTMR_FreeTimer
(
    zbTmrTimerID_t timerID
);                                       

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_IsTimerActive
 * DESCRIPTION: Check if a specified timer is active
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is active,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
extern bool_t ZbTMR_IsTimerActive
(
    zbTmrTimerID_t timerID
);

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_IsTimerReady
 * DESCRIPTION: Check if a specified timer is ready
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is ready,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
extern bool_t ZbTMR_IsTimerReady
(
    zbTmrTimerID_t timerID
);

/*---------------------------------------------------------------------------
 * NAME: TMR_GetRemainingTime
 * DESCRIPTION: Returns the remaining time until timeout, for the specified
 *              timer
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: remaining time in milliseconds until timer timeouts.
 *---------------------------------------------------------------------------*/
extern uint32_t TMR_GetRemainingTime
(
    zbTmrTimerID_t tmrID
);

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_StartTimer (BeeStack or application)
 * DESCRIPTION: Start a specified timer
 * PARAMETERS: IN: timerId - the ID of the timer
 *             IN: timerType - the type of the timer
 *             IN: timeInMilliseconds - time expressed in millisecond units
 *             IN: pfTmrCallBack - callback function
 * RETURN: -
 * NOTES: When the timer expires, the callback function is called in
 *        non-interrupt context. If the timer is already running when
 *        this function is called, it will be stopped and restarted.
 *---------------------------------------------------------------------------*/
extern void ZbTMR_StartTimer
(
    zbTmrTimerID_t timerID,                       
    zbTmrTimerType_t timerType,                   
    zbTmrTimeInMilliseconds_t timeInMilliseconds, 
    void (*pfZbTimerCallBack)(zbTmrTimerID_t)       
);

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_StartLowPowerTimer
 * DESCRIPTION: Start a low power timer. When the timer goes off, call the 
 *              callback function in non-interrupt context. 
 *              If the timer is running when this function is called, it will 
 *              be stopped and restarted. 
 *              Start the timer with the following timer types:
 *                          - gTmrLowPowerMinuteTimer_c
 *                          - gTmrLowPowerSecondTimer_c
 *                          - gTmrLowPowerSingleShotMillisTimer_c
 *                          - gTmrLowPowerIntervalMillisTimer_c
 *              The MCU can enter in low power if there are only active low 
 *              power timers.
 * PARAMETERS: IN: timerId - the ID of the timer
 *             IN: timerType - the type of the timer
 *             IN: timeIn - time in ticks
 *             IN: pfTmrCallBack - callback function
 * RETURN: type/DESCRIPTION
 *---------------------------------------------------------------------------*/
extern void ZbTMR_StartLowPowerTimer
(
    zbTmrTimerID_t timerId,
    zbTmrTimerType_t timerType,
    uint32_t timeIn,
    void (*pfZbTmrCallBack)(zbTmrTimerID_t)
);

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_StartMinuteTimer
 * DESCRIPTION: Starts a minutes timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMinutes - time expressed in minutes
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of ZbTMR_StartTimer(). This is a single shot timer.
 *        There are no interval minute timers.
 *---------------------------------------------------------------------------*/

#if gZbTMR_EnableMinutesSecondsTimers_d
extern void ZbTMR_StartMinuteTimer
(
    zbTmrTimerID_t timerId, 
    zbTmrTimeInMinutes_t timeInMinutes, 
    void (*pfZbTmrCallBack)(zbTmrTimerID_t)
);
#endif
  
/*---------------------------------------------------------------------------
 * NAME: ZbTMR_StartSecondTimer
 * DESCRIPTION: Starts a seconds timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInSeconds - time expressed in seconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of ZbTMR_StartTimer(). This is a single shot timer.
 *        There are no interval seconds timers.
 *---------------------------------------------------------------------------*/
#if gZbTMR_EnableMinutesSecondsTimers_d
extern void ZbTMR_StartSecondTimer
(
    zbTmrTimerID_t timerId, 
    zbTmrTimeInSeconds_t timeInSeconds, 
    void (*pfZbTmrCallBack)(zbTmrTimerID_t)
);
#endif
 
/*---------------------------------------------------------------------------
 * NAME: ZbTMR_StartIntervalTimer
 * DESCRIPTION: Starts an interval count timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMilliseconds - time expressed in milliseconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of ZbTMR_StartTimer()
 *---------------------------------------------------------------------------*/
extern void ZbTMR_StartIntervalTimer
(
    zbTmrTimerID_t timerID,
    zbTmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfZbTimerCallBack)(zbTmrTimerID_t)
);

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_StartSingleShotTimer
 * DESCRIPTION: Starts an single-shot timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMilliseconds - time expressed in milliseconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of ZbTMR_StartTimer()
 *---------------------------------------------------------------------------*/
extern void ZbTMR_StartSingleShotTimer
(
    zbTmrTimerID_t timerID,
    zbTmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfZbTimerCallBack)(zbTmrTimerID_t)
);

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_StopTimer
 * DESCRIPTION: Stop a timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: None
 * NOTES: Associated timer callback function is not called, even if the timer
 *        expires. Does not frees the timer. Safe to call anytime, regardless
 *        of the state of the timer.
 *---------------------------------------------------------------------------*/
extern void ZbTMR_StopTimer
(
    zbTmrTimerID_t timerID
); 


/*---------------------------------------------------------------------------
 * NAME: ZbTMR_EnableTimer
 * DESCRIPTION: Enable the specified timer
 * PARAMETERS:  IN: tmrID - the timer ID
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
extern void ZbTMR_EnableTimer
(
    zbTmrTimerID_t tmrID
);


/*---------------------------------------------------------------------------
 * NAME: TMR_SyncLpmTimers
 * DESCRIPTION: This function is called by the Low Power module
 * each time the MCU wakes up.
 * PARAMETERS:  sleep duration in milliseconds
 * RETURN: none
 * NOTES: none
 *---------------------------------------------------------------------------*/                             
extern void TMR_SyncLpmTimers
(
    uint32_t sleepDurationTmrTicks
);


/*---------------------------------------------------------------------------
 * NAME: ZbTmrAllocateSystemTimer
 * DESCRIPTION: Reserve a system timer
 * PARAMETERS: -
 * RETURN: gTmrInvalidTimerID_c if there are no timers available
 * NOTES: none
 *---------------------------------------------------------------------------*/

extern zbTmrTimerID_t ZbTmrAllocateSystemTimer(void);

/*---------------------------------------------------------------------------
 * NAME: ZbTmrStopSystemTimer
 * DESCRIPTION: Stop a system timer
 * PARAMETERS:  IN: timerID - the ID of the tsystem imer
 * RETURN: None
 * NOTES: Associated timer callback function is not called, even if the timer
 *        expires. Does not frees the timer. Safe to call anytime, regardless
 *        of the state of the timer.
 *---------------------------------------------------------------------------*/

extern void ZbTmrStopSystemTimer(zbTmrTimerID_t timerID);

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
extern void ZbTMRStartSystemTimer
(
    zbTmrTimerID_t timerID,
    zbTmrTimerType_t timerType,
    zbTmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfTimerCallBack)(void *),
    void *param
);

/*---------------------------------------------------------------------------
 * NAME: ZbTMRGetRemainingTimeSystemTimer
 * DESCRIPTION: Returns the remaining time until timeout, for the specified
 *              system timer
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: remaining time in milliseconds until next timer timeout
 *---------------------------------------------------------------------------*/
extern uint32_t ZbTMRGetRemainingTimeSystemTimer
(
  zbTmrTimerID_t tmrID
);


#ifdef __cplusplus
}
#endif


#endif /* #ifndef __ZIGBEE_TMR_INTERFACE_H__ */

