/*!
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file PWRLib.c
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

/*****************************************************************************
 *                               INCLUDED HEADERS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the headers that this module needs to include.    *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#include "EmbeddedTypes.h"
#include "PhyTypes.h"
#include "PhyTime.h"
#include "PWRLib.h"
#include "PWR_Configuration.h"
#include "TimersManager.h"
#include "Keyboard.h"
#include "fsl_osa_ext.h"

#include "TMR_Adapter.h"

#include "fsl_lptmr_hal.h"
#include "fsl_lptmr_driver.h"
#include "fsl_llwu_hal.h"

#if (cPWR_UsePowerModuleStandAlone == 0)
#include "MacInterface.h"
#endif
/*****************************************************************************
 *                               PRIVATE VARIABLES                           *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have local       *
 * (file) scope.                                                             *
 * Each of this declarations shall be preceded by the 'static' keyword.      *
 * These variables / constants cannot be accessed outside this module.       *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/* LPTMR variables */
   
#if (cPWR_UsePowerDownMode==1)
static uint32_t mPWRLib_RTIElapsedTicks;
#endif /* #if (cPWR_UsePowerDownMode==1) */


/* For LVD function */ 

#if (cPWR_LVD_Enable == 2)
tmrTimerID_t               PWRLib_LVD_PollIntervalTmrID;
PWRLib_LVD_VoltageLevel_t    PWRLib_LVD_SavedLevel;
#endif  /* #if (cPWR_LVD_Enable == 2) */





/*****************************************************************************
 *                               PUBLIC VARIABLES                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have global      *
 * (project) scope.                                                          *
 * These variables / constants can be accessed outside this module.          *
 * These variables / constants shall be preceded by the 'extern' keyword in  *
 * the interface header.                                                     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/* Zigbee STACK status */ 
PWRLib_StackPS_t PWRLib_StackPS;
volatile PWRLib_WakeupReason_t PWRLib_MCU_WakeupReason;
#if (cPWR_UsePowerDownMode)
static uint32_t   gPhyCalibrationTicks; 
#endif
#if (cPWR_UsePowerDownMode==1)

/*****************************************************************************
 *                           PRIVATE FUNCTIONS PROTOTYPES                    *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions prototypes that have local (file)   *
 * scope.                                                                    *
 * These functions cannot be accessed outside this module.                   *
 * These declarations shall be preceded by the 'static' keyword.             *
 *---------------------------------------------------------------------------*
 *****************************************************************************/


/*****************************************************************************
 *                                PRIVATE FUNCTIONS                          *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions that have local (file) scope.       *
 * These functions cannot be accessed outside this module.                   *
 * These definitions shall be preceded by the 'static' keyword.              *
 *---------------------------------------------------------------------------*
*****************************************************************************/

/*****************************************************************************
 *                             PUBLIC FUNCTIONS                              *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions that have global (project) scope.   *
 * These functions can be accessed outside this module.                      *
 * These functions shall have their declarations (prototypes) within the     *
 * interface header file and shall be preceded by the 'extern' keyword.      *
 *---------------------------------------------------------------------------*
 *****************************************************************************/


/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_UpdateWakeupReason
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LLWU_UpdateWakeupReason
(
void
)
{
  uint32_t i = 0;
  uint32_t llwuFlags = 0;
  
  for(i=0; i<15; i++)
  {
      if( LLWU_HAL_GetExternalPinWakeupFlag(LLWU, (llwu_wakeup_pin_t)i) )
      {
          llwuFlags |= 1<<i;
      }
  }
  
  if(llwuFlags & gPWRLib_LLWU_KeyboardMask_c)
  {
    PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1; 
  }
  
  if( LLWU_HAL_GetInternalModuleWakeupFlag(LLWU, (llwu_wakeup_module_t)gPWRLib_LLWU_WakeupModule_LPTMR_c) )
  {
    PWRLib_MCU_WakeupReason.Bits.FromLPTMR = 1;
    PWRLib_MCU_WakeupReason.Bits.DeepSleepTimeout = 1;
  }
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_GetCounterValue
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t  PWRLib_LPTMR_GetCounterValue
(
)
{
  return LPTMR_HAL_GetCounterValue(g_lptmrBase[gLptmrInstance_c]); 
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_IsWakeUpTimeExpired
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t  PWRLib_LPTMR_IsWakeUpTimeExpired
(
)
{
  if( LPTMR_HAL_IsIntPending(g_lptmrBase[gLptmrInstance_c]) )
  {
    return TRUE;
  }
  return FALSE; 
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_PhyTicksToLPOTicks
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t  PWRLib_LPTMR_PhyTicksToLPOTicks
(
 phyTime_t phyTicks
)
{
  uint32_t lpoTicks = (uint32_t)(-1) ;
  phyTicks *= gLPOCalibrationTicks_c;
  phyTicks /= gPhyCalibrationTicks;
  if( phyTicks < lpoTicks)
  {
    lpoTicks = (uint32_t)phyTicks;
  } 
  return lpoTicks; 
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_LPOTicksToPhyTicks
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
phyTime_t  PWRLib_LPTMR_LPOTicksToPhyTicks
(
 uint32_t lpoTicks
)
{
  phyTime_t phyTicks;
  phyTicks = lpoTicks;
  phyTicks *= gPhyCalibrationTicks;
  phyTicks += (gLPOCalibrationTicks_c>>1);
  phyTicks /= gLPOCalibrationTicks_c;
  return phyTicks; 
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_CalibrateLPOvsPhyTimer
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
static void PWRLib_LPTMR_CalibrateLPOvsPhyTimer
(
)
{
  LPTMR_Type *baseAddr = g_lptmrBase[gLptmrInstance_c];
  uint8_t offset = 1;
  phyTime_t phyTimeClockTicks1, phyTimeClockTicks2;
  lptmr_prescaler_user_config_t prescaler_config =
  {
    .prescalerBypass = 1,
    .prescalerClockSelect = (lptmr_prescaler_clock_select_t)cPWR_LPTMRClockSource,
    .prescalerValue = (lptmr_prescaler_value_t)0, /* bypass */
  };                             
  
  LPTMR_HAL_Disable(baseAddr);
  /* Set compare value */
  LPTMR_HAL_SetCompareValue(baseAddr, gLPOCalibrationOffest_c + gLPOCalibrationTicks_c + offset + 1 ); 
  /* Use specified tick count */
  /* Configure prescaler, bypass prescaler and clck source */
  /* Disable LPTMR prescaller: clock divider = 1 */
  LPTMR_HAL_SetPrescalerMode(baseAddr, prescaler_config);  
  /* Start counting */
  LPTMR_HAL_Enable(baseAddr); 
  while(gLPOCalibrationOffest_c != LPTMR_HAL_GetCounterValue(baseAddr));
  OSA_EXT_InterruptDisable();
  while( (offset + gLPOCalibrationOffest_c) !=  LPTMR_HAL_GetCounterValue(baseAddr));
  PhyTimeReadClockTicks(&phyTimeClockTicks1);
  OSA_EXT_InterruptEnable();
  while(( gLPOCalibrationOffest_c + gLPOCalibrationTicks_c )!= LPTMR_HAL_GetCounterValue(baseAddr));
  OSA_EXT_InterruptDisable();
  while(( offset + gLPOCalibrationOffest_c + gLPOCalibrationTicks_c )!= LPTMR_HAL_GetCounterValue(baseAddr));
  PhyTimeReadClockTicks(&phyTimeClockTicks2);
  OSA_EXT_InterruptEnable();
  LPTMR_HAL_Disable(baseAddr); 
  gPhyCalibrationTicks = phyTimeClockTicks2 - phyTimeClockTicks1;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ClockStart
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LPTMR_ClockStart
(
uint8_t  ClkMode,
uint32_t Ticks
)
{
  LPTMR_Type *baseAddr = g_lptmrBase[gLptmrInstance_c];
  lptmr_prescaler_user_config_t prescaler_config;
  
  OSA_EXT_InterruptDisable();
  LPTMR_HAL_Disable(baseAddr);
  /* Set compare value */
  LPTMR_HAL_SetCompareValue(baseAddr, Ticks); 
  /* Use specified tick count */
  mPWRLib_RTIElapsedTicks = 0;
  /* Configure prescaler, bypass prescaler and clck source */
  if( ClkMode == cLPTMR_PRS_00001ms )
  {
    /* Disable LPTMR prescaler: clock divider = 1 */
    prescaler_config.prescalerBypass = 1;
  }
  else
  {
    /* Enable LPTMR prescaler: clock divider is between 2 and 65536 */
    prescaler_config.prescalerBypass = 0;
    prescaler_config.prescalerValue = (lptmr_prescaler_value_t)ClkMode;
  }
  prescaler_config.prescalerClockSelect = (lptmr_prescaler_clock_select_t)cPWR_LPTMRClockSource;
  LPTMR_HAL_SetPrescalerMode(baseAddr, prescaler_config);  
  /* Start counting */
  LPTMR_HAL_Enable(baseAddr); 
  OSA_EXT_InterruptEnable();
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ClockCheck
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PWRLib_LPTMR_ClockCheck
(
void
)
{
  LPTMR_Type *baseAddr = g_lptmrBase[gLptmrInstance_c];
  
  OSA_EXT_InterruptDisable();
  /* LPTMR is still running */
  if( LPTMR_BRD_CSR_TEN(baseAddr) )
  {
    mPWRLib_RTIElapsedTicks = LPTMR_HAL_GetCounterValue(baseAddr);
    /* timer compare flag is set */
    if( LPTMR_HAL_IsIntPending(baseAddr) )
    {
      uint32_t compareReg;
      compareReg = LPTMR_HAL_GetCompareValue(baseAddr);
      if(mPWRLib_RTIElapsedTicks < compareReg )
      {
        mPWRLib_RTIElapsedTicks += 0x10000;
      }
    }
  }
  OSA_EXT_InterruptEnable();
  return mPWRLib_RTIElapsedTicks;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_LPTMR_ClockStop
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_LPTMR_ClockStop
(
void
)
{
  LPTMR_Type *baseAddr = g_lptmrBase[gLptmrInstance_c];
  
  OSA_EXT_InterruptDisable();
  /* LPTMR is still running */
  if( LPTMR_BRD_CSR_TEN(baseAddr) )
  {
    mPWRLib_RTIElapsedTicks = LPTMR_HAL_GetCounterValue(baseAddr);
    /* timer compare flag is set */
    if( LPTMR_HAL_IsIntPending(baseAddr) )
    {
      uint32_t compareReg;
      compareReg = LPTMR_HAL_GetCompareValue(baseAddr);
      if(mPWRLib_RTIElapsedTicks < compareReg )
      {
        mPWRLib_RTIElapsedTicks += 0x10000;
      }
    }
  }
  /* Stop LPTMR */
  LPTMR_HAL_Disable(baseAddr); 
  OSA_EXT_InterruptEnable();
}


#if (cPWR_UsePowerModuleStandAlone == 0)

/******************************************************************************
 * Name: PWRLib_GetMacStateReq
 * Description: Get status from MAC. Functions just as Asp_GetMacStateReq().
 *
 * Parameter(s): - none
 * Return: - gAspMacStateIdle_c     : MAC ready for Sleep or DeepSleep
 *           gAspMacStateBusy_c     : Don't sleep
 *           gAspMacStateNotEmpty_c : MAC allows Wait
 ******************************************************************************/

uint8_t PWRLib_GetMacStateReq
(
  void
)
{
  return Mac_GetState();
}

#endif /* (cPWR_UsePowerModuleStandAlone == 0) */

/*---------------------------------------------------------------------------
 * Name: PWRLib_LLWU_Isr
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/

void PWRLib_LLWU_Isr
(
void
)
{  
  /* Clear external pins wakeup interrupts */
  LLWU_F1 = LLWU_F1; 
  LLWU_F2 = LLWU_F2; 
  
  /* LPTMR is wakeup source */
  if( LLWU_HAL_GetInternalModuleWakeupFlag(LLWU, (llwu_wakeup_module_t)gPWRLib_LLWU_WakeupModule_LPTMR_c) )
  {
    /* Clear LPTMR interrupt */
    LPTMR_HAL_ClearIntFlag(g_lptmrBase[gLptmrInstance_c]);
  }
}

#endif /* #if (cPWR_UsePowerDownMode==1) */

/*---------------------------------------------------------------------------
* Name: PWRLib_LVD_CollectLevel
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
PWRLib_LVD_VoltageLevel_t PWRLib_LVD_CollectLevel
(
void
)
{
#if ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2))
  
  /* Check low detect voltage 1.6V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(0);
  PMC_LVDSC1 = PMC_LVDSC1_LVDACK_MASK;
  if(PMC_LVDSC1 & PMC_LVDSC1_LVDF_MASK)
  {
    /* Low detect voltage reached */
    PMC_LVDSC1 = PMC_LVDSC1_LVDACK_MASK;
    return(PWR_LEVEL_CRITICAL);
  }
  
  /* Check low trip voltage 1.8V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(0);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_1_8V);
  }
  
  /* Check low trip voltage 1.9V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(1);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_1_9V);
  }
  /* Check low trip voltage 2.0V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(2);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_0V);
  }
  
  /* Check low trip voltage 2.1V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(3);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_1V);
  }
  
  /* Check low detect voltage (high range) 2.56V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(1); /* Set high trip voltage and clear warning flag */
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(0);
  PMC_LVDSC1 |= PMC_LVDSC1_LVDACK_MASK;
  if(PMC_LVDSC1 & PMC_LVDSC1_LVDF_MASK)
  {
    /* Low detect voltage reached */
    PMC_LVDSC1 = PMC_LVDSC1_LVDACK_MASK; /* Set low trip voltage and clear warning flag */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_56V);
  }
  
  /* Check high trip voltage 2.7V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(1);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(0);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_7V);
  }
  
  /* Check high trip voltage 2.8V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(1);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(1);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_8V);
  }
  
  /* Check high trip voltage 2.9V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(1);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(2);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_2_9V);
  }
  
  /* Check high trip voltage 3.0V */
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(1);
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(3);
  PMC_LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
  if(PMC_LVDSC2 & PMC_LVDSC2_LVWF_MASK)
  {
    /* Low trip voltage reached */
    PMC_LVDSC2 = PMC_LVDSC2_LVWACK_MASK; /* Clear flag (and set low trip voltage) */
    PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
    return(PWR_BELOW_LEVEL_3_0V);
  }
  
  PMC_LVDSC2 = PMC_LVDSC2_LVWV(0);
  PMC_LVDSC1 = PMC_LVDSC1_LVDV(0); /* Set low trip voltage */
#endif  /* #if ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2)) */
  
  /*--- Voltage level is okay > 3.0V */
  return(PWR_ABOVE_LEVEL_3_0V);
}

/******************************************************************************
 * Name: PWRLib_LVD_PollIntervalCallback
 * Description:
 *
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if (cPWR_LVD_Enable == 2)
static void PWRLib_LVD_PollIntervalCallback
(
void* param
)
{
  (void)param;
  PWRLib_LVD_SavedLevel = PWRLib_LVD_CollectLevel();
}
#endif



/*---------------------------------------------------------------------------
 * Name: PWRLib_GetSystemResetStatus
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint16_t PWRLib_GetSystemResetStatus
(
  void
)
{
  uint16_t resetStatus = 0;
  resetStatus = (uint16_t) (RCM_SRS0);
  resetStatus |= (uint16_t)(RCM_SRS1 << 8);
  return resetStatus;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_Init
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/


void PWRLib_Init
(
void
)
{
  
#if (cPWR_UsePowerDownMode == 1)
  /* enable clock to LLWU module */

  
#if ((cPWR_DeepSleepMode == 1) || (cPWR_DeepSleepMode == 2))  
  PhyTimerInit();
  LPTMR_Init(NULL);
  PWRLib_LPTMR_CalibrateLPOvsPhyTimer();
  PWR_SetDeepSleepTimeInMs(cPWR_DeepSleepDurationMs);
#endif
  
#if ( cPWR_DeepSleepMode == 1)
  /* install LLWU Isr and validate it in NVIC */
  OSA_EXT_InstallIntHandler(LLWU_IRQn, PWRLib_LLWU_Isr);
  NVIC_SetPriority(LLWU_IRQn, 0x80 >> (8 - __NVIC_PRIO_BITS));
  NVIC_EnableIRQ(LLWU_IRQn);
  
  /* enable LPTMR as wakeup source for LLWU module */
  LLWU_HAL_SetInternalModuleCmd(LLWU, (llwu_wakeup_module_t)gPWRLib_LLWU_WakeupModule_LPTMR_c, true);
  LLWU_HAL_SetExternalInputPinMode(LLWU, kLlwuExternalPinChangeDetect, gPWRLib_LLWU_WakeupPin_PTD6_c);
#endif
#endif /* #if (cPWR_UsePowerDownMode==1) */
  
  /* LVD_Init TODO */
#if (cPWR_LVD_Enable == 0)
  PMC_LVDSC1 &= (uint8_t) ~( PMC_LVDSC1_LVDIE_MASK  | PMC_LVDSC1_LVDRE_MASK);
  PMC_LVDSC2 &= (uint8_t) ~( PMC_LVDSC2_LVWIE_MASK );
#elif ((cPWR_LVD_Enable == 1) || (cPWR_LVD_Enable == 2))
  PMC_LVDSC1 &= (uint8_t) ~( PMC_LVDSC1_LVDIE_MASK | PMC_LVDSC1_LVDRE_MASK);
  PMC_LVDSC2 &= (uint8_t) ~( PMC_LVDSC2_LVWIE_MASK );
#elif (cPWR_LVD_Enable==3)
  PMC_LVDSC1 = (PMC_LVDSC1 | (uint8_t)PMC_LVDSC1_LVDRE_MASK) & (uint8_t)(~PMC_LVDSC1_LVDIE_MASK );
  PMC_LVDSC2 &= (uint8_t) ~( PMC_LVDSC2_LVWIE_MASK );
#endif /* #if (cPWR_LVD_Enable) */
  
  
#if (cPWR_LVD_Enable == 2)
#if ((cPWR_LVD_Ticks == 0) || (cPWR_LVD_Ticks > 71582))
#error  "*** ERROR: cPWR_LVD_Ticks invalid value"
#endif 
  
  PWRLib_LVD_SavedLevel = PWRLib_LVD_CollectLevel(); 
  /* Allocate a platform timer */
  PWRLib_LVD_PollIntervalTmrID = TMR_AllocateTimer();	
  if(gTmrInvalidTimerID_c != PWRLib_LVD_PollIntervalTmrID)
  {	
    /* start the timer */
    TMR_StartLowPowerTimer(PWRLib_LVD_PollIntervalTmrID, gTmrIntervalTimer_c,TmrMinutes(cPWR_LVD_Ticks) , PWRLib_LVD_PollIntervalCallback, NULL); 
  }
#endif  /* #if (cPWR_LVD_Enable==2) */
  
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_Reset
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_Reset
(
  void
)
{
  NVIC_SystemReset();
  while(1);
}
