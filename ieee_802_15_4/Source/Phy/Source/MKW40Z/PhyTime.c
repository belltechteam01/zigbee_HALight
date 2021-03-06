/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file PhyTime.c
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


/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "Phy.h"
#include "EmbeddedTypes.h"
#include "FunctionLib.h"

#include "fsl_os_abstraction.h"
#include "fsl_device_registers.h"

/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */
#define gPhyTimeMinSetupTime_c (4) /* symbols */

#define BM_ZLL_IRQSTS_TMRxMSK (ZLL_IRQSTS_TMR1MSK_MASK | \
                               ZLL_IRQSTS_TMR2MSK_MASK | \
                               ZLL_IRQSTS_TMR3MSK_MASK | \
                               ZLL_IRQSTS_TMR4MSK_MASK )

/*! *********************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
void (*gpfPhyTimeNotify)(void) = NULL;


/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */
static phyTimeEvent_t  mPhyTimers[gMaxPhyTimers_c];
static phyTimeEvent_t *pNextEvent;
volatile uint64_t      gPhyTimerOverflow;


/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
********************************************************************************** */
static void PhyTime_OverflowCB( uint32_t param );
static phyTimeEvent_t* PhyTime_GetNextEvent( void );


/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
* \brief  Sets the start time of a sequence
*
* \param[in]  startTime  the start time for a sequence
*
********************************************************************************** */
void PhyTimeSetEventTrigger
(
phyTime_t startTime
)
{
    uint32_t irqSts;

    OSA_EnterCritical(kCriticalDisableInt);

    // disable TMR2 compare
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_TMR2CMP_EN_MASK;

    ZLL_T2PRIMECMP = startTime;

    // unmask TMR2 interrupt (do not change other IRQ status)
    irqSts  = ZLL_IRQSTS & BM_ZLL_IRQSTS_TMRxMSK;
    irqSts &= ~(ZLL_IRQSTS_TMR2MSK_MASK);
    // aknowledge TMR2 IRQ
    irqSts |= ZLL_IRQSTS_TMR2IRQ_MASK;
    ZLL_IRQSTS = irqSts;

    // TC2PRIME_EN must be enabled
    // enable TMR2 compare, enable autosequence start by TC2 match
    ZLL_PHY_CTRL |= ZLL_PHY_CTRL_TMR2CMP_EN_MASK | ZLL_PHY_CTRL_TMRTRIGEN_MASK;

    OSA_ExitCritical(kCriticalDisableInt);
}

/*! *********************************************************************************
* \brief  Disable the time trigger for a sequence.
*
* \remarks The sequence will start asap
*
********************************************************************************** */
void PhyTimeDisableEventTrigger
(
void
)
{
    uint32_t irqSts;

    OSA_EnterCritical(kCriticalDisableInt);

    // disable autosequence start by TC2 match
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_TMRTRIGEN_MASK;
    // disable TMR2 compare
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_TMR2CMP_EN_MASK;
    // mask TMR2 interrupt (do not change other IRQ status)
    irqSts  = ZLL_IRQSTS & BM_ZLL_IRQSTS_TMRxMSK;
    irqSts |= ZLL_IRQSTS_TMR2MSK_MASK;
    // aknowledge TMR2 IRQ
    irqSts |= ZLL_IRQSTS_TMR2IRQ_MASK;
    ZLL_IRQSTS = irqSts;

    OSA_ExitCritical(kCriticalDisableInt);
}

/*! *********************************************************************************
* \brief  Sets the timeout value for a sequence
*
* \param[in]  pEndTime the absolute time when a sequence should terminate
*
* \remarks If the sequence does not finish until the timeout, it will be aborted
*
********************************************************************************** */
void PhyTimeSetEventTimeout
(
phyTime_t *pEndTime
)
{
    uint32_t irqSts;

#ifdef PHY_PARAMETERS_VALIDATION
    if(NULL == pEndTime)
    {
        return;
    }
#endif // PHY_PARAMETERS_VALIDATION

    OSA_EnterCritical(kCriticalDisableInt);

    // disable TMR3 compare
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_TMR3CMP_EN_MASK;

    ZLL_T3CMP = *pEndTime & 0x00FFFFFF;

    // aknowledge TMR3 IRQ
    irqSts  = ZLL_IRQSTS & BM_ZLL_IRQSTS_TMRxMSK;
    irqSts |= ZLL_IRQSTS_TMR3IRQ_MASK;
    ZLL_IRQSTS = irqSts;
    // enable TMR3 compare
    ZLL_PHY_CTRL |= ZLL_PHY_CTRL_TMR3CMP_EN_MASK;
    // enable autosequence stop by TC3 match
    ZLL_PHY_CTRL |= ZLL_PHY_CTRL_TC3TMOUT_MASK;

    OSA_ExitCritical(kCriticalDisableInt);
}

/*! *********************************************************************************
* \brief  Return the timeout value for the current sequence
*
* \return  uint32_t the timeout value
*
********************************************************************************** */
phyTime_t PhyTimeGetEventTimeout( void )
{
    return ZLL_T3CMP;
}

/*! *********************************************************************************
* \brief  Disables the sequence timeout
*
********************************************************************************** */
void PhyTimeDisableEventTimeout
(
void
)
{
    uint32_t irqSts;

    OSA_EnterCritical(kCriticalDisableInt);

    // disable TMR3 compare
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_TMR3CMP_EN_MASK;
    // disable autosequence stop by TC3 match
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_TC3TMOUT_MASK;
    // mask TMR3 interrupt (do not change other IRQ status)
    irqSts  = ZLL_IRQSTS & BM_ZLL_IRQSTS_TMRxMSK;
    irqSts |= ZLL_IRQSTS_TMR3MSK_MASK;
    // aknowledge TMR3 IRQ
    irqSts |= ZLL_IRQSTS_TMR3IRQ_MASK;
    ZLL_IRQSTS = irqSts;

    OSA_ExitCritical(kCriticalDisableInt);
}

/*! *********************************************************************************
* \brief  Reads the absolute clock from the radio
*
* \param[out]  pRetClk pointer to a location where the current clock will be stored
*
********************************************************************************** */
void PhyTimeReadClock
(
phyTime_t *pRetClk
)
{
#ifdef PHY_PARAMETERS_VALIDATION
    if(NULL == pRetClk)
    {
        return;
    }
#endif // PHY_PARAMETERS_VALIDATION

    *pRetClk = (phyTime_t)ZLL_EVENT_TMR;
}

/*! *********************************************************************************
* \brief  Initialize the Event Timer
*
* \param[in]  pAbsTime  pointer to the location where the new time is stored
*
********************************************************************************** */
void PhyTimeInitEventTimer
(
uint32_t *pAbsTime
)
{
#ifdef PHY_PARAMETERS_VALIDATION
    if(NULL == pAbsTime)
    {
        return;
    }
#endif // PHY_PARAMETERS_VALIDATION

    OSA_EnterCritical(kCriticalDisableInt);

    ZLL_T1CMP = *pAbsTime;

    ZLL_PHY_CTRL |= ZLL_PHY_CTRL_TMRLOAD_MASK; // self clearing bit

    OSA_ExitCritical(kCriticalDisableInt);
}

/*! *********************************************************************************
* \brief  Set TMR1 timeout value
*
* \param[in]  pWaitTimeout the timeout value
*
********************************************************************************** */
void PhyTimeSetWaitTimeout
(
phyTime_t *pWaitTimeout
)
{
    uint32_t irqSts;

    OSA_EnterCritical(kCriticalDisableInt);

    // disable TMR1 compare
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_TMR1CMP_EN_MASK;

    ZLL_T1CMP = *pWaitTimeout;

    // unmask TMR1 interrupt (do not change other IRQ status)
    irqSts  = ZLL_IRQSTS & BM_ZLL_IRQSTS_TMRxMSK;
    irqSts &= ~(ZLL_IRQSTS_TMR1MSK_MASK);
    // aknowledge TMR1 IRQ
    irqSts |= ZLL_IRQSTS_TMR1IRQ_MASK;
    ZLL_IRQSTS = irqSts;
    // enable TMR1 compare
    ZLL_PHY_CTRL |= ZLL_PHY_CTRL_TMR1CMP_EN_MASK;

    OSA_ExitCritical(kCriticalDisableInt);
}

/*! *********************************************************************************
* \brief  Disable the TMR1 timeout
*
********************************************************************************** */
void PhyTimeDisableWaitTimeout
(
void
)
{
    uint32_t irqSts;

    OSA_EnterCritical(kCriticalDisableInt);

    // disable TMR1 compare
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_TMR1CMP_EN_MASK;
    // mask TMR1 interrupt (do not change other IRQ status)
    irqSts  = ZLL_IRQSTS & BM_ZLL_IRQSTS_TMRxMSK;
    irqSts |= ZLL_IRQSTS_TMR1MSK_MASK;
    // aknowledge TMR1 IRQ
    irqSts |= ZLL_IRQSTS_TMR1IRQ_MASK;
    ZLL_IRQSTS = irqSts;

    OSA_ExitCritical(kCriticalDisableInt);
}

/*! *********************************************************************************
* \brief  Set TMR4 timeout value
*
* \param[in]  pWakeUpTime  absolute time
*
********************************************************************************** */
void PhyTimeSetWakeUpTime
(
uint32_t *pWakeUpTime
)
{
    uint32_t irqSts;

    OSA_EnterCritical(kCriticalDisableInt);

    // disable TMR4 compare
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_TMR4CMP_EN_MASK;

    ZLL_T4CMP = *pWakeUpTime;
    
    // unmask TMR4 interrupt (do not change other IRQ status)
    irqSts  = ZLL_IRQSTS & BM_ZLL_IRQSTS_TMRxMSK;
    irqSts &= ~(ZLL_IRQSTS_TMR4MSK_MASK);
    // aknowledge TMR4 IRQ
    irqSts |= ZLL_IRQSTS_TMR4IRQ_MASK;
    ZLL_IRQSTS = irqSts;
    // enable TMR4 compare
    ZLL_PHY_CTRL |= ZLL_PHY_CTRL_TMR4CMP_EN_MASK;

    OSA_ExitCritical(kCriticalDisableInt);
}

/*! *********************************************************************************
* \brief  Check if TMR4 IRQ occured, and aknowledge it
*
* \return  TRUE if TMR4 IRQ occured
*
********************************************************************************** */
bool_t PhyTimeIsWakeUpTimeExpired
(
void
)
{
    bool_t wakeUpIrq = FALSE;
    uint32_t irqSts;

    OSA_EnterCritical(kCriticalDisableInt);
    
    irqSts = ZLL_IRQSTS;

    // disable TMR4 compare
    ZLL_PHY_CTRL &= ~ZLL_PHY_CTRL_TMR4CMP_EN_MASK;

    if( irqSts & ZLL_IRQSTS_TMR4IRQ_MASK )
    {
        wakeUpIrq = TRUE;
    }

    // unmask TMR4 interrupt (do not change other IRQ status)
    irqSts |= BM_ZLL_IRQSTS_TMRxMSK;
    irqSts &= ~(ZLL_IRQSTS_TMR4MSK_MASK);
    // aknowledge TMR4 IRQ
    irqSts |= ZLL_IRQSTS_TMR4IRQ_MASK;
    ZLL_IRQSTS = irqSts;

    OSA_ExitCritical(kCriticalDisableInt);

    return wakeUpIrq;
}


/*! *********************************************************************************
* \brief  PHY Timer Interrupt Service Routine
*
********************************************************************************** */
void PhyTime_ISR(void)
{
    if( pNextEvent->callback == PhyTime_OverflowCB )
    {
        gPhyTimerOverflow++;
    }
    
    if( gpfPhyTimeNotify )
    {
        gpfPhyTimeNotify();
    }
    else
    {
        PhyTime_RunCallback();
        PhyTime_Maintenance();
    }
}

/*! *********************************************************************************
* \brief  Initialize the PHY Timer module
*
* \return  phyTimeStatus_t
*
********************************************************************************** */
phyTimeStatus_t PhyTime_TimerInit( void (*cb)(void) )
{
    if( gpfPhyTimeNotify )
        return gPhyTimeError_c;

    gpfPhyTimeNotify = cb;
    gPhyTimerOverflow = 0;
    FLib_MemSet( mPhyTimers, 0, sizeof(mPhyTimers) );

    /* Schedule Overflow Calback */
    pNextEvent = &mPhyTimers[0];
    pNextEvent->callback = PhyTime_OverflowCB;
    pNextEvent->timestamp = (gPhyTimerOverflow+1) << gPhyTimeShift_c;
    PhyTimeSetWaitTimeout( &pNextEvent->timestamp );

    return gPhyTimeOk_c;
}

/*! *********************************************************************************
* \brief  Returns a 64bit timestamp value to be used by the MAC Layer
*
* \return  phyTime_t PHY timestamp
*
********************************************************************************** */
phyTime_t PhyTime_GetTimestamp(void)
{
    phyTime_t time = 0;

    OSA_EnterCritical(kCriticalDisableInt);
    PhyTimeReadClock( &time );
    time |= (gPhyTimerOverflow << gPhyTimeShift_c);
    OSA_ExitCritical(kCriticalDisableInt);

    return time;
}

/*! *********************************************************************************
* \brief  Schedules an event
*
* \param[in]  pEvent  event to be scheduled
*
* \return  phyTimeTimerId_t  the id of the alocated timer
*
********************************************************************************** */
phyTimeTimerId_t PhyTime_ScheduleEvent( phyTimeEvent_t *pEvent )
{
    phyTimeTimerId_t tmr;

    /* Parameter validation */
    if( NULL == pEvent->callback )
    {
        return gInvalidTimerId_c;
    }

    /* Search for a free slot (slot 0 is reserved for the Overflow calback) */
    OSA_EnterCritical(kCriticalDisableInt);
    for( tmr=1; tmr<gMaxPhyTimers_c; tmr++ )
    {
        if( mPhyTimers[tmr].callback == NULL )
        {
            mPhyTimers[tmr] = *pEvent;
            break;
        }
    }
    OSA_ExitCritical(kCriticalDisableInt);

    if( tmr >= gMaxPhyTimers_c )
        return gInvalidTimerId_c;

    /* Program the next event */
    if((NULL == pNextEvent) ||
       (NULL != pNextEvent  && mPhyTimers[tmr].timestamp < pNextEvent->timestamp))
    {
        PhyTime_Maintenance();
    }

    return tmr;
}

/*! *********************************************************************************
* \brief  Cancel an event
*
* \param[in]  timerId  the Id of the timer
*
* \return  phyTimeStatus_t
*
********************************************************************************** */
phyTimeStatus_t PhyTime_CancelEvent( phyTimeTimerId_t timerId )
{
    if( (timerId == 0) || (timerId >= gMaxPhyTimers_c) || (NULL == mPhyTimers[timerId].callback) )
    {
        return gPhyTimeNotFound_c;
    }

    OSA_EnterCritical(kCriticalDisableInt);
    if( pNextEvent == &mPhyTimers[timerId] )
        pNextEvent = NULL;

    mPhyTimers[timerId].callback = NULL;
    OSA_ExitCritical(kCriticalDisableInt);

    return gPhyTimeOk_c;
}

/*! *********************************************************************************
* \brief  Cancel all event with the specified paameter
*
* \param[in]  param  event parameter
*
* \return  phyTimeStatus_t
*
********************************************************************************** */
phyTimeStatus_t PhyTime_CancelEventsWithParam ( uint32_t param )
{
    uint32_t i;
    phyTimeStatus_t status = gPhyTimeNotFound_c;

    OSA_EnterCritical(kCriticalDisableInt);
    for( i=1; i<gMaxPhyTimers_c; i++ )
    {
        if( mPhyTimers[i].callback && (param == mPhyTimers[i].parameter) )
        {
            status = gPhyTimeOk_c;
            mPhyTimers[i].callback = NULL;
            if( pNextEvent == &mPhyTimers[i] )
                pNextEvent = NULL;
        }
    }
    OSA_ExitCritical(kCriticalDisableInt);

    return status;
}

/*! *********************************************************************************
* \brief  Run the callback for the recently expired event
*
********************************************************************************** */
void PhyTime_RunCallback( void )
{
    uint32_t param;
    phyTimeCallback_t cb;

    if( pNextEvent )
    {
        OSA_EnterCritical(kCriticalDisableInt);

        param = pNextEvent->parameter;
        cb = pNextEvent->callback;
        pNextEvent->callback = NULL;
        pNextEvent = NULL;

        OSA_ExitCritical(kCriticalDisableInt);

        cb(param);
    }
}

/*! *********************************************************************************
* \brief  Expire events too close to be scheduled.
*         Program the next event
*
********************************************************************************** */
void PhyTime_Maintenance( void )
{
    phyTime_t currentTime;
    phyTimeEvent_t *pEv;

    PhyTimeDisableWaitTimeout();

    while(1)
    {
        OSA_EnterCritical(kCriticalDisableInt);
        
        pEv = PhyTime_GetNextEvent();
        currentTime = PhyTime_GetTimestamp();
        
        /* Program next event if exists */
        if( pEv )
        {
            pNextEvent = pEv;
            
            if( pEv->timestamp > (currentTime + gPhyTimeMinSetupTime_c) )
            {
                PhyTimeSetWaitTimeout( &pEv->timestamp );
                pEv = NULL;
            }
        }

        OSA_ExitCritical(kCriticalDisableInt);

        if( !pEv )
            break;

        PhyTime_RunCallback();
    }
}


/*! *********************************************************************************
* \brief  Timer Overflow callback
*
* \param[in]  param
*
********************************************************************************** */
static void PhyTime_OverflowCB( uint32_t param )
{
    (void)param;

    /* Reprogram the next overflow callback */
    mPhyTimers[0].callback = PhyTime_OverflowCB;
    mPhyTimers[0].timestamp = (gPhyTimerOverflow+1) << 24;
}

/*! *********************************************************************************
* \brief  Search for the next event to be scheduled
*
* \return phyTimeEvent_t pointer to the next event to be scheduled
*
********************************************************************************** */
static phyTimeEvent_t* PhyTime_GetNextEvent( void )
{
    phyTimeEvent_t *pEv = NULL;
    uint32_t i;

    /* Search for the next event to be serviced */
    for( i=0; i<gMaxPhyTimers_c; i++ )
    {
        if( NULL != mPhyTimers[i].callback )
        {
            if( NULL == pEv )
            {
                pEv = &mPhyTimers[i];
            }
            /* Check which event expires first */
            else if( mPhyTimers[i].timestamp < pEv->timestamp )
            {
                pEv = &mPhyTimers[i];
            }
        }
    }

    return pEv;
}
