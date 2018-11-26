/*! *********************************************************************************
* \file   AppInit.c
* This is a source file for the Application Initialization File.
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
#include "fsl_osa_ext.h"
#include "AppInit.h"
#include "PhyInterface.h"
#include "MacInterface.h"
#include "SerialManager.h"
#include "FsciInterface.h"
#include "MsgSystem.h"
#include "RNG_Interface.h"
#include "MemManager.h"
#include "TimersManager.h"
#include "PWR_Interface.h"
#include "NVM_Interface.h"
#include "LED.h"
#include "ZigbeeTask.h"
#include "board.h"

/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/* NONE */

/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/

#if (gLpmIncluded_d || gNvStorageIncluded_d) 
static void AppIdleHandler(void const *argument);
#endif

/*! *********************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/* NONE */

/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
#if gFsciIncluded_c

#ifndef gInterfaceChannel_c
#define gInterfaceChannel_c 1
#endif

/* FSCI Interface Configuration structure */
static const gFsciSerialConfig_t mFsciSerials[] = {
    /* Baudrate,            interface type,   channel No, virtual interface */
    #if gSerialMgrUseUSB_c
       {gUARTBaudRate115200_c, gSerialMgrUSB_c, 1,           0}
      #if (gFsciMaxInterfaces_c > 1)
      ,{gUARTBaudRate115200_c, gSerialMgrUSB_c, 1,           1}
      #endif
    #elif gSerialMgrUseUart_c
       {gUARTBaudRate115200_c, APP_SERIAL_INTERFACE_TYPE, APP_SERIAL_INTERFACE_INSTANCE,           0}
       #if (gFsciMaxInterfaces_c > 1)
       ,{gUARTBaudRate115200_c, APP_SERIAL_INTERFACE_TYPE, APP_SERIAL_INTERFACE_INSTANCE,           1}
       #endif
    #else
      #warning Please configure the serial used for FSCI 
    #endif
};
#endif


/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/* NONE */

/*! *********************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  Cmsis main thread - init fwk modules, starts conectivity stacks
*         Used for low power module and non volatile module asynchronous operations
* \param[in]  main task param 
*
* \return  None.
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
void main_task(uint32_t param)
{
#if FSL_RTOS_FREE_RTOS
    /* Initialize framework and platform drivers */
    hardware_init();
#endif  
  /* Init memory blocks manager */
  MEM_Init();
  
  /* Init  timers module */
  TMR_Init();
  
  /* Init Led module */
  LED_Init();
  
  /* Init serial manager module*/
  SerialManager_Init();
  
  /* Init phy module */  
  Phy_Init();
  
  /* RNG must be initialized after the PHY is Initialized */
  RNG_Init();
  
  /* Init Keyboard module  */
  KBD_Init(ZbKeyboardCallback);
  
  /* Init mac module */
  MAC_Init();

  /* Init FSCI module */
#if gFsciIncluded_c    
  FSCI_Init( (void*)mFsciSerials );
#endif
  
  /* Init zigbeepro module */
  ZbPro_Init();
  
  /* Call after ZbPro_Init - because of the dynamic dataset registration */
  /* Non volatile memory module init */
  NvModuleInit();
    
  /*All LED's are switched OFF*/
  Led1Off();
  Led2Off();
  Led3Off();
  Led4Off();

#if TestProfileApp_d  
  Led1On();
#endif     
  
  /* Call after Nvm module init - to be able to restore NVM data */
  ZbApplicationInit();
  
#if (gLpmIncluded_d || gNvStorageIncluded_d) 
  AppIdleHandler(&param);
#else  
  /* The main thread is not used anymore*/
  OSA_EXT_TaskDestroy(OSA_EXT_TaskGetId());
#endif  
}
/*! *********************************************************************************
* \brief  Used for Low power module and non volatile module operation
*         
* \param[in]  main task param 
*
* \return  None.
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
#if (gLpmIncluded_d || gNvStorageIncluded_d)
static void AppIdleHandler(void const *argument)
{
    
    while(1)
    {

      #if gNvStorageIncluded_d
        /* Process NV Storage save-on-idle, save-on-count and save-on-interval requests */
        NvIdle();
      #endif
      
      #if gLpmIncluded_d  
      if( PWR_CheckIfDeviceCanGoToSleep() )
      {
            //PWRLib_WakeupReason_t wakeupReason;
            //wakeupReason = PWR_EnterLowPower();
            (void)PWR_EnterLowPower();
            PWR_DisallowDeviceToSleep();
            #if 0
            if( wakeupReason.Bits.FromKeyBoard )
            {
                App_HandleKeys( gKBD_EventSW1_c );
            }
            #endif
      }
      #endif
    }
}
#endif
/*************************************************************************************/
