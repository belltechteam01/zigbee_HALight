/*! *********************************************************************************
* \file InternalTimers.c
* This is the source file for Beestack Internal timers.
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
#include "TMR_Interface.h"
#include "FunctionLib.h"   

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/
/*
 * NAME: mTmrStatusFree_c
 * DESCRIPTION: self explanatory
 */
#define mTmrStatusFree_c    0x00

/*
 * NAME: mTmrStatusActive_c
 * DESCRIPTION: self explanatory
 */
#define mTmrStatusActive_c    0x20

/*
 * NAME: mTmrStatusReady_c
 * DESCRIPTION: self explanatory
 */
#define mTmrStatusReady_c    0x40

/*
 * NAME: mTmrStatusInactive_c
 * DESCRIPTION: self explanatory
 */
#define mTmrStatusInactive_c    0x80

/*
 * NAME: mTimerStatusMask_c
 * DESCRIPTION: timer status mask
 */
#define mTimerStatusMask_c      ( mTmrStatusActive_c \
                                | mTmrStatusReady_c \
                                | mTmrStatusInactive_c)

/*
 * NAME: TMR_IsTimerAllocated()
 * DESCRIPTION: checks if a specified timer is allocated
 */
#define TMR_IsTimerAllocated(timerID)   (pmMaTmrTimerTable[(timerID)].tmrTimerStatusTable)

/*
 * NAME: TMR_MarkTimerFree()
 * DESCRIPTION: marks the specified timer as free
 */
#define TMR_MarkTimerFree(timerID)       pmMaTmrTimerTable[(timerID)].tmrTimerStatusTable = 0

/*
 * NAME: IsLowPowerTimer()
 * DESCRIPTION: Detect if the timer is a low-power timer
 */
#define IsLowPowerTimer(type)           ((type) & gTmrLowPowerTimer_c)

/*
 * NAME: mTimerType_c
 * DESCRIPTION: timer types
 */
#define mTimerType_c            ( gTmrSingleShotTimer_c \
	| gTmrSetSecondTimer_c \
	| gTmrSetMinuteTimer_c \
	| gTmrIntervalTimer_c \
	| gTmrLowPowerTimer_c )

	
/* MQX resolution is 5 ms */                            
#define gRTOSResolutionInMs_d    5

/* MQX Timer events */
#define gTmrTickEvt_d       1<<0
#define gTmrEnableTmrEvt_d  1<<1

#define gTmrTaskEventMask_d (gTmrTickEvt_d | gTmrEnableTmrEvt_d)

/*****************************************************************************
 *****************************************************************************
 * Private prototypes
 *****************************************************************************
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_GetTimerStatus
 * DESCRIPTION: RETURNs the timer status
 * PARAMETERS:  IN: timerID - the timer ID
 * RETURN: see definition of zbTmrStatus_t
 * NOTES: none
 *---------------------------------------------------------------------------*/
static zbTmrStatus_t ZbTMR_GetTimerStatus 
( 
    zbTmrTimerID_t timerID 
);

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_SetTimerStatus
 * DESCRIPTION: Set the timer status
 * PARAMETERS:  IN: timerID - the timer ID
 * 			   IN: status - the status of the timer
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
static void ZbTMR_SetTimerStatus
( 
    zbTmrTimerID_t timerID,
    zbTmrStatus_t status
);

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_SetTimerType
 * DESCRIPTION: Set the timer type
 * PARAMETERS:  IN: timerID - the timer ID
 * 			    IN: type - timer type
 * RETURN: none
 * NOTES: none
 *---------------------------------------------------------------------------*/
static void ZbTMR_SetTimerType
(
  zbTmrTimerID_t timerID,
  zbTmrTimerType_t type
);
/*---------------------------------------------------------------------------
 * NAME: ZbTMR_GetTimerType
 * DESCRIPTION: RETURNs the timer type
 * PARAMETERS:  IN: timerID - the timer ID
 * RETURN: see definition of zbTmrTimerType_t
 * NOTES: none
 *---------------------------------------------------------------------------*/
static zbTmrTimerType_t ZbTMR_GetTimerType 
( 
  zbTmrTimerID_t timerID 
);
/*****************************************************************************
 *****************************************************************************
 * Private memory definitions
 *****************************************************************************
 *****************************************************************************/

/*
 * NAME: maTmrTimerTable
 * DESCRIPTION:  Main timer table. All allocated timers are stored here.
 *               A timer's ID is it's index in this table.
 * VALUES: see definition
 */

   
static tmrTimerTableEntry_t* pmMaTmrTimerTable; 

static uint8_t mNoOfTimers;

static zbTmrTimerID_t rtosTimer;

static zbTmrTimeInMilliseconds_t* pNextTimeInMs;

/*
 * NAME: numberOfActiveTimers
 * DESCRIPTION: Number of Active timers (without low power capability)
 *              the MCU can not enter low power if numberOfActiveTimers!=0
 * VALUES: 0..255
 */
static uint8_t numberOfActiveTimers = 0;

/*
 * NAME: numberOfLowPowerActiveTimers
 * DESCRIPTION: Number of low power active timer.
 *              The MCU can enter in low power if more low power timers are active
 * VALUES:
 */
static uint8_t numberOfLowPowerActiveTimers = 0;

#define IncrementActiveTimerNumber(type)  (((type) & gTmrLowPowerTimer_c) \
                                          ?(++numberOfLowPowerActiveTimers) \
                                          :(++numberOfActiveTimers) )                                   
#define DecrementActiveTimerNumber(type)  (((type) & gTmrLowPowerTimer_c) \
                                          ?(--numberOfLowPowerActiveTimers) \
                                          :(--numberOfActiveTimers) ) 
                                          

volatile uint32_t systemCounter=0;

void* pfsystemTimerCallBack = NULL;
void*  systemTimerParams = NULL;

/* Protection to recursively call of ZbTMR_Handle */
static bool_t timerHandlerRunning = FALSE;
/*****************************************************************************
******************************************************************************
* Private functions
******************************************************************************
*****************************************************************************/

/*---------------------------------------------------------------------------
* NAME: ZbTMR_GetTimerStatus
* DESCRIPTION: Returns the timer status
* PARAMETERS:  IN: timerID - the timer ID
* RETURN: see definition of zbTmrStatus_t
* NOTES: none
*---------------------------------------------------------------------------*/
static zbTmrStatus_t ZbTMR_GetTimerStatus
(
    zbTmrTimerID_t timerID
)
{
    return pmMaTmrTimerTable[timerID].tmrTimerStatusTable & mTimerStatusMask_c;
}

/*---------------------------------------------------------------------------
* NAME: ZbTMR_SetTimerStatus
* DESCRIPTION: Set the timer status
* PARAMETERS:  IN: timerID - the timer ID
* 			   IN: status - the status of the timer	
* RETURN: None
* NOTES: none
*---------------------------------------------------------------------------*/
static void ZbTMR_SetTimerStatus
(
    zbTmrTimerID_t timerID, 
    zbTmrStatus_t status
)
{
    pmMaTmrTimerTable[timerID].tmrTimerStatusTable = (pmMaTmrTimerTable[timerID].tmrTimerStatusTable & ~mTimerStatusMask_c) | status;
}

/*---------------------------------------------------------------------------
* NAME: ZbTMR_GetTimerType
* DESCRIPTION: Returns the timer type
* PARAMETERS:  IN: timerID - the timer ID
* RETURN: see definition of zbTmrTimerType_t
* NOTES: none
*---------------------------------------------------------------------------*/
static zbTmrTimerType_t ZbTMR_GetTimerType
(
    zbTmrTimerID_t timerID
)
{
    return pmMaTmrTimerTable[timerID].tmrTimerStatusTable & mTimerType_c;
}

/*---------------------------------------------------------------------------
* NAME: ZbTMR_SetTimerType
* DESCRIPTION: Set the timer type
* PARAMETERS:  IN: timerID - the timer ID
* 			   IN: type - timer type	
* RETURN: none
* NOTES: none
*---------------------------------------------------------------------------*/
static void ZbTMR_SetTimerType
(
    zbTmrTimerID_t timerID, 
    zbTmrTimerType_t type
)
{
    pmMaTmrTimerTable[timerID].tmrTimerStatusTable = (pmMaTmrTimerTable[timerID].tmrTimerStatusTable & ~mTimerType_c) | type;
} 

/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_Init
 * DESCRIPTION: initialize the timer module
 * PARAMETERS: -
 * RETURN: -
 *---------------------------------------------------------------------------*/
void ZbTMR_Init 
(
    void
)
{
  
  FLib_MemSet16(pmMaTmrTimerTable,0,sizeof(tmrTimerTableEntry_t)*mNoOfTimers);

}

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_AllocateTimer
 * DESCRIPTION: allocate a timer
 * PARAMETERS: -
 * RETURN: timer ID
 *---------------------------------------------------------------------------*/

zbTmrTimerID_t ZbTMR_AllocateTimer
(
  void
)
{
  uint32_t i;
  for (i = 0; i < mNoOfTimers; ++i) 
  {
    if (!TMR_IsTimerAllocated(i)) 
    {
      ZbTMR_SetTimerStatus(i, mTmrStatusInactive_c);
      return i;
    }
  }
  return gTmrInvalidTimerID_c;
}                                      

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_AreAllTimersOff
 * DESCRIPTION: Check if all timers except the LP timers are OFF.
 * PARAMETERS: -
 * RETURN: TRUE if there are no active non-low power timers, FALSE otherwise
 *---------------------------------------------------------------------------*/
bool_t ZbTMR_AreAllTimersOff
(
    void
)
{
    return !numberOfActiveTimers;
}                                      

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_FreeTimer
 * DESCRIPTION: Free a timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: -
 * NOTES: Safe to call even if the timer is running.
 *        Harmless if the timer is already free.
 *---------------------------------------------------------------------------*/
void ZbTMR_FreeTimer
(
    zbTmrTimerID_t timerID
)
{
    ZbTMR_StopTimer(timerID);
    TMR_MarkTimerFree(timerID);
}                                       

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_IsTimerActive
 * DESCRIPTION: Check if a specified timer is active
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is active,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
bool_t ZbTMR_IsTimerActive
(
    zbTmrTimerID_t timerID
)
{
    return ZbTMR_GetTimerStatus(timerID) == mTmrStatusActive_c;
} 

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_IsTimerReady
 * DESCRIPTION: Check if a specified timer is ready
 * PARAMETERS: IN: timerID - the ID of the timer
 * RETURN: TRUE if the timer (specified by the timerID) is ready,
 *         FALSE otherwise
 *---------------------------------------------------------------------------*/
bool_t ZbTMR_IsTimerReady
(
    zbTmrTimerID_t timerID
)
{
    return ZbTMR_GetTimerStatus(timerID) == mTmrStatusReady_c;
} 

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
void ZbTMR_StartTimer
(
    zbTmrTimerID_t timerID,                       
    zbTmrTimerType_t timerType,                   
    zbTmrTimeInMilliseconds_t timeInMilliseconds, 
    void (*pfZbTimerCallBack)(zbTmrTimerID_t)       
)
{
    
    /* check if timer is not allocated or if it has an invalid ID (fix@ENGR223389) */
    if (!TMR_IsTimerAllocated(timerID) || (gTmrInvalidTimerID_c == timerID)) 
      return;

    /* Stopping an already stopped timer is harmless. */
    ZbTMR_StopTimer(timerID);
    
    if (timeInMilliseconds < gRTOSResolutionInMs_d) 
    {
        timeInMilliseconds = gRTOSResolutionInMs_d;
    }
    
    ZbTMR_SetTimerType(timerID, timerType);
    pmMaTmrTimerTable[timerID].intervalInMs = timeInMilliseconds;
    pmMaTmrTimerTable[timerID].remainingMs = timeInMilliseconds;
    pmMaTmrTimerTable[timerID].pfCallBack = pfZbTimerCallBack;
    
    /* Enable timer, the timer task will do the rest of the work. */
    ZbTMR_EnableTimer(timerID);
    
}

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
void ZbTMR_StartLowPowerTimer
(
    zbTmrTimerID_t timerId,
    zbTmrTimerType_t timerType,
    uint32_t timeIn,
    void (*pfTmrCallBack)(zbTmrTimerID_t)
) 
{

    ZbTMR_StartTimer(timerId, timerType | gTmrLowPowerTimer_c, timeIn, pfTmrCallBack);
}

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

void ZbTMR_StartMinuteTimer
(
    zbTmrTimerID_t timerId, 
    zbTmrTimeInMinutes_t timeInMinutes, 
    void (*pfZbTmrCallBack)(zbTmrTimerID_t)
)
{
    ZbTMR_StartTimer(timerId, gTmrMinuteTimer_c, ZbTmrMinutes(timeInMinutes), pfZbTmrCallBack);
}

  
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

void ZbTMR_StartSecondTimer
(
    zbTmrTimerID_t timerId, 
    zbTmrTimeInSeconds_t timeInSeconds, 
    void (*pfZbTmrCallBack)(zbTmrTimerID_t)
) 
{
    ZbTMR_StartTimer(timerId, gTmrSecondTimer_c, ZbTmrSeconds(timeInSeconds), pfZbTmrCallBack);
}

 
/*---------------------------------------------------------------------------
 * NAME: ZbTMR_StartIntervalTimer
 * DESCRIPTION: Starts an interval count timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMilliseconds - time expressed in milliseconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of ZbTMR_StartTimer()
 *---------------------------------------------------------------------------*/
void ZbTMR_StartIntervalTimer
(
    zbTmrTimerID_t timerID,
    zbTmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfZbTimerCallBack)(zbTmrTimerID_t)
)
{
    ZbTMR_StartTimer(timerID, gTmrIntervalTimer_c, timeInMilliseconds, pfZbTimerCallBack);
}

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_StartSingleShotTimer
 * DESCRIPTION: Starts an single-shot timer
 * PARAMETERS:  IN: timerId - the ID of the timer
 *              IN: timeInMilliseconds - time expressed in milliseconds
 *              IN: pfTmrCallBack - callback function
 * RETURN: None
 * NOTES: Customized form of ZbTMR_StartTimer()
 *---------------------------------------------------------------------------*/
void ZbTMR_StartSingleShotTimer
(
    zbTmrTimerID_t timerID,
    zbTmrTimeInMilliseconds_t timeInMilliseconds,
    void (*pfZbTimerCallBack)(zbTmrTimerID_t)
)
{
    ZbTMR_StartTimer(timerID, gTmrSingleShotTimer_c, timeInMilliseconds, pfZbTimerCallBack);
}

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_StopTimer
 * DESCRIPTION: Stop a timer
 * PARAMETERS:  IN: timerID - the ID of the timer
 * RETURN: None
 * NOTES: Associated timer callback function is not called, even if the timer
 *        expires. Does not frees the timer. Safe to call anytime, regardless
 *        of the state of the timer.
 *---------------------------------------------------------------------------*/
void ZbTMR_StopTimer
(
    zbTmrTimerID_t timerID
)
{
    zbTmrStatus_t status;
    
    status = ZbTMR_GetTimerStatus(timerID);
    
    if ( (status == mTmrStatusActive_c) || (status == mTmrStatusReady_c) ) 
    {
        ZbTMR_SetTimerStatus(timerID, mTmrStatusInactive_c);
        DecrementActiveTimerNumber(ZbTMR_GetTimerType(timerID));
        /* if no sw active timers are enabled, */
        /* call the TMR_Task() to countdown the ticks and stop the hw timer*/
        if (!numberOfActiveTimers && !numberOfLowPowerActiveTimers) 
            ZbTmrStopSystemTimer(rtosTimer);
    }	

}  

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_SetInstance
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
)
{
  pmMaTmrTimerTable = pTmrTimerTable;
  rtosTimer = osTimer;
  mNoOfTimers = noOfTimers;
  pNextTimeInMs = pNextIntTime;
  pfsystemTimerCallBack = (void*)zbTmrSystemCallBack;
  systemTimerParams = param;

  uint8_t timerID;
  
  numberOfActiveTimers = 0;
  numberOfLowPowerActiveTimers = 0;
  /* Count active timers */
  for (timerID = 0; timerID < mNoOfTimers; ++timerID)
  { 
    if (ZbTMR_GetTimerStatus(timerID) != mTmrStatusInactive_c)
    {      
      IncrementActiveTimerNumber(ZbTMR_GetTimerType(timerID));  
    }
  }
}

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_Handler
 * DESCRIPTION: Beestack instance timer handler. 
 *              Called by the rtos beestack task  when the RTOS timer callback posts a timer event.
 * PARAMETERS:  IN: events - timer events mask
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
void ZbTMR_Handler(void)
{
  zbTmrTimeInMilliseconds_t nextInterruptTime = 0x0000FFFF;
  pfZbTmrCallBack_t pfCallBack;
  zbTmrTimeInMilliseconds_t remainingTimeInMs;
  zbTmrTimeInMilliseconds_t msSinceLastHere;
  zbTmrTimerStatus_t status;
  uint8_t timerID;
  zbTmrTimerType_t timerType;
  bool_t bJustLPMTimersCheck=TRUE;  
  
  timerHandlerRunning = TRUE;
  
  remainingTimeInMs = ZbTMRGetRemainingTimeSystemTimer(rtosTimer);
  
  ZbTmrStopSystemTimer(rtosTimer);
    
  /* calculate difference between current and previous.  */ 
  msSinceLastHere = (*pNextTimeInMs - remainingTimeInMs);
  
  for (timerID = 0; timerID < mNoOfTimers; ++timerID) 
  {
    status = ZbTMR_GetTimerStatus(timerID);
    /* If ZbTMR_StartTimer() has been called for this timer, start it's count */
    /* down as of now. */
    if (status == mTmrStatusReady_c) 
    {
      ZbTMR_SetTimerStatus(timerID, mTmrStatusActive_c);
      continue;
    }

    /* Ignore any timer that is not active. */
    if (status != mTmrStatusActive_c) 
    {
      continue;
    }

    /* This timer is active. Decrement it's countdown.. */
    if (pmMaTmrTimerTable[timerID].remainingMs > msSinceLastHere) 
    {
      pmMaTmrTimerTable[timerID].remainingMs -= msSinceLastHere;
      if(pmMaTmrTimerTable[timerID].remainingMs > gRTOSResolutionInMs_d)
      { 
        continue;
      }
    }

    timerType = ZbTMR_GetTimerType(timerID);
    /* If this is an interval timer, restart it. Otherwise, mark it as inactive. */
    if ( (timerType & gTmrSingleShotTimer_c) ||
         (timerType & gTmrSetMinuteTimer_c) ||
         (timerType & gTmrSetSecondTimer_c)  ) 
    {
      pmMaTmrTimerTable[timerID].remainingMs = 0;
      ZbTMR_StopTimer(timerID);
    } 
    else 
    {
      pmMaTmrTimerTable[timerID].remainingMs = pmMaTmrTimerTable[timerID].intervalInMs;
    }
    /* This timer has expired. */
    pfCallBack = pmMaTmrTimerTable[timerID].pfCallBack;
    /*Call callback if it is not NULL
      This is done after the timer got updated,
      in case the timer gets stopped or restarted in the callback*/
      if (pfCallBack) 
      {
        pfCallBack(timerID);
      }
  }  /* for (timerID = 0; timerID < ... */
 

    for (timerID = 0; timerID < mNoOfTimers; ++timerID) 
    {
      if (ZbTMR_GetTimerStatus(timerID) == mTmrStatusActive_c) 
      {
        if(!IsLowPowerTimer(ZbTMR_GetTimerType(timerID)))
        {
          bJustLPMTimersCheck = FALSE;
        }
        if (nextInterruptTime > pmMaTmrTimerTable[timerID].remainingMs) 
        {
          nextInterruptTime = pmMaTmrTimerTable[timerID].remainingMs;
        }
      }
    }
    
    *pNextTimeInMs = nextInterruptTime;
    
    if (numberOfActiveTimers || numberOfLowPowerActiveTimers) 
    {
      ZbTMRStartSystemTimer(rtosTimer,
                            (bJustLPMTimersCheck?gTmrLowPowerTimer_c|gTmrSingleShotTimer_c:gTmrSingleShotTimer_c),
                            //gTmrSingleShotTimer_c,
                            nextInterruptTime,
                            (void(*)(void *))pfsystemTimerCallBack,
                            systemTimerParams);
    }
    
    timerHandlerRunning = FALSE;
}

/*---------------------------------------------------------------------------
 * NAME: ZbTMR_EnableTimer
 * DESCRIPTION: Enable the specified timer
 * PARAMETERS:  IN: tmrID - the timer ID
 * RETURN: None
 * NOTES: none
 *---------------------------------------------------------------------------*/
void ZbTMR_EnableTimer
(
    zbTmrTimerID_t tmrID
)
{    		
  
  if (ZbTMR_GetTimerStatus(tmrID) == mTmrStatusInactive_c)
  {      
    
    IncrementActiveTimerNumber(ZbTMR_GetTimerType(tmrID));    
    ZbTMR_SetTimerStatus(tmrID, mTmrStatusReady_c);
    
    /* Protection to recursively call of ZbTMR_Handle */
    if(FALSE == timerHandlerRunning)
    {
      ZbTMR_Handler();
    }
    else
    {
      ZbTMR_SetTimerStatus(tmrID, mTmrStatusActive_c);
    }
  }  	
}

/*****************************************************************************
 *                               <<< EOF >>>                                 *
 *****************************************************************************/
