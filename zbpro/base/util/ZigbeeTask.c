/*! *********************************************************************************
* \file RtosZigbeeTask.c
* This is the source file for the RTOS ZbPro Task.
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

#include "SerialManager.h"
#include "LED.h"

#include "FsciInterface.h"
#include "FsciCommunication.h"
#include "FsciCommands.h"
#include "FsciMacCommands.h"
#include "Panic.h"

#include "AppInit.h"
#include "ZigbeeTask.h"

#include "TS_Interface.h"
#include "TMR_Interface.h"
#include "MsgSystem.h"
#include "ZtcInterface.h"


#include "BeeStack_Globals.h"
#include "NwkVariables.h"
#include "ApsVariables.h"
#include "AfVariables.h"
#include "ZDOVariables.h"
#include "BeeStackRamAlloc.h"

#include "BeeStackInit.h"
#include "BeeStackUtil.h"

#include "BeeAppInit.h"
#include "BeeApp.h"
#include "EndPointConfig.h"

#include "ZclFoundation.h"

#include "ASL_UserInterface.h"
/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/* Number of zigbee pro instances to be created */
#ifndef gZbProInstancesCnt_d
  #define gZbProInstancesCnt_d 1
#endif

/* Maximum number of private timers for a ZigbeePro instance */
#define gBeeStackMaxTimers_d 50

/* Maximum number of private for a ZigbeePro instance */
#define gTsMaxTasks_c   10

/* ZigbeePro instanciable reserved stack ram */
#if(gInstantiableStackEnabled_d == 1)
  #define gZbInstReservedRam_d 5000
#else
  #define gZbInstReservedRam_d 0
#endif

#define gZbProRtosStackSize_d (gZbInstReservedRam_d + 3364) 

#if gFsciIncluded_c
  /* ZigbeePro fsci opcodes counter */
  #define gZbFsciOpcodeCnt_d sizeof(aZbFsciOpcodes)/sizeof(uint8_t)
#endif


#define gHaZtcZclOpCodeGroup_c                 0x70 /* opcode group used for HA commands/events */

/*! *********************************************************************************
*************************************************************************************
* Private type declarations
*************************************************************************************
********************************************************************************** */

/* Zigbee Instance_Task params */ 
typedef struct zbTaskParams_tag
{
  /* Beestack thread   Id */
  osaTaskId_t    zbThreadId;
  /* Mac instance Id */
  uint32_t      macInstanceId;
  /* Upper layer instance thread Id */
  osaTaskId_t    upperThreadId;
  /* FSCI serial interface */
  uint8_t       serialInterfaceId;
}zbTaskParams_t;


/* MQX BeeStack events */
#define gZbStartEvent_d       1<<0
#define gZbTimerEvent_d       1<<1
#define gZbMacEvent_d         1<<2
#define gZbGenericEvent_d     1<<3
#define gZbFsciEvent_d        1<<4
#define gZbKeyboardEvent_d    1<<5

/* Zigbee task event mask */
#if gFsciIncluded_c

  #define gZbTaskEventMask_d (gZbStartEvent_d|gZbTimerEvent_d|gZbMacEvent_d|gZbFsciEvent_d)

#else

  #define gZbTaskEventMask_d (gZbStartEvent_d|gZbTimerEvent_d|gZbMacEvent_d)

#endif



/* Struct used by timer callbacks to post
   an event for beestackMqx timer */
typedef struct zbCallBackParams_tag
{
  /* Event group of the task */
  osaEventId_t        eventId;
  osaEventFlags_t     event;
}zbCallBackParams_t;


/* Serial interface's id's */
typedef enum
{
  gZbSerialInterface0_c,
  gZbSerialInterface1_c
} zbSerialInterfaceId_t;

/* Serial interface's id's */
typedef enum
{
  gZbVirtualSerialInterface0_c,
  gZbVirtualSerialInterface1_c
} beestackVirtualSerialInterfaceId_t; 

/* Serial interface params */
typedef struct zbSerialParams_tag
{
  uint8_t                serialId;
  serialInterfaceType_t  interfaceType;
  uint8_t                channel;
  uint32_t               baudRate;
}zbSerialParams_t;


/*! *********************************************************************************
*************************************************************************************
* Private memory definitions
*************************************************************************************
********************************************************************************** */
#if gFsciIncluded_c
/* ZbPro Fsci opcodegroups */
const uint8_t aZbFsciOpcodes[]=
{
  gZdoNlmeOpcodeGroup_c,     gNlmeZdoOpcodeGroup_c,      gApsmeZdoOpcodeGroup_c, gZdoApsmeOpcodeGroup_c, 
  gApsNldeOpcodeGroup_c,     gNldeApsOpcodeGroup_c,      gAfApsdeOpcodeGroup_c,  gApsdeAfOpcodeGroup_c,
  gAfdeAppOpcodeGroup_c,     gAppAfdeOpcodeGroup_c,      gZdpAppOpcodeGroup_c,   gAppZdpOpcodeGroup_c,
  gAppInterPanOpcodeGroup_c, gInterPanAppOpcodeGroup_c,  gHcAppOpcodeGroup_c,    gAppHcOpcodeGroup_c,
  gHaZtcZclOpCodeGroup_c
  #if TestProfileApp_d
  ,    gTp2OpCodeGroup_c 
  #endif  
};
#endif

/* Queue for mlme messagess */
anchor_t aZbTaskMlmeNwkQueue[gZbProInstancesCnt_d];

/* Queue for mcps messagess */
anchor_t aZbTaskMcpsNwkQueue[gZbProInstancesCnt_d];

/* Queue for ztc messagess */
anchor_t aZbTaskZtcQueue[gZbProInstancesCnt_d];

/* ZbPro Instances semaphore - used for protection to context switch */
osaSemaphoreId_t  zbSemaphore = NULL;

/* ZbPro Instances eventd groups */
osaEventId_t mZbTaskEventId[gZbProInstancesCnt_d];
/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
********************************************************************************** */
static void       ZbPro_Task(osaTaskParam_t param);
static uint32_t   ZbGetMacInstance(void);
static uint8_t    MLME_NWK_SapHandler(nwkMessage_t *pMsg, uint8_t zbInstanceId);
static uint8_t    MCPS_NWK_SapHandler(nwkMessage_t *pMsg, uint8_t zbInstanceId);
static void       ZbTimerCallBack  (void *param);
static uint32_t   ZbGetMacInstance(void);
#if gFsciIncluded_c
  static uint32_t ZbFsciGetZbInstance(uint32_t commInterfaceId);
  static void     ZbRegisterToFsci(uint32_t zbInstanceId);
  static void     ZbFsciOpcodesHandler(void *pData, void* param, uint32_t commInterfaceId);
  static void     ZbProcessFsciPacket(clientPacket_t* pMsg);
  static bool_t   ZbFsciIsOtaPacket(opCode_t opCode);
#endif  
  
/*! *********************************************************************************
*************************************************************************************
* Public memory definitions
*************************************************************************************
********************************************************************************** */

/* ZbPro RTOS Task declaration */
OSA_EXT_TASK_DEFINE( ZbPro_Task,  (OSA_PRIORITY_IDLE+1),   gZbProInstancesCnt_d,  gZbProRtosStackSize_d,0);


/* ZbPro instances parameters */ 
zbTaskParams_t aZbTasksParams[]=
{
  /* beestackThreadId,  macInstanceId,        upperThreadId,  serialInterfaceId   */
  {     NULL,          gInvalidInstanceId_c,      NULL,           gZbSerialInterface0_c}
#if gZbProInstancesCnt_d > 1
  ,{     NULL ,         gInvalidInstanceId_c,     NULL,          gZbSerialInterface1_c}
#endif
};


key_event_t lastKeyCode;

uint32_t zbProInstance=0;
/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
* \brief  This function  initiallize the ZbPro Layer - create rtos task
*
* \param[in]  None.
*
* \return  None.
*
* \pre
*
* \post
*
* \remarks
*
********************************************************************************** */
void ZbPro_Init( void )
{
    uint32_t i;
    for( i=0 ; i < gZbProInstancesCnt_d; i++ )
    { 
      mZbTaskEventId[i] = OSA_EXT_EventCreate(TRUE);
      if( mZbTaskEventId[i] == NULL )
      {
        panic( 0, (uint32_t)ZbPro_Init, 0, 0 );
      }  
      if(NULL == OSA_EXT_TaskCreate( OSA_EXT_TASK(ZbPro_Task), (osaTaskParam_t)i))
      {
        panic( 0, (uint32_t)ZbPro_Init, 0, 0 ); 
      }
    }
}

/*! *********************************************************************************
* \brief  Nwk to Mcps Sap called internal in zigbee stack, add mac istance
*
* \param[in]  nwkToMcpsMessage_t 
*
* \return  mac resultType_t
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
resultType_t ZbNWK_MCPS_SapHandler( nwkToMcpsMessage_t* pMsg)
{
    #if(gInstantiableStackEnabled_d == 1)
        return NWK_MCPS_SapHandler((nwkToMcpsMessage_t*) pMsg, ZbGetMacInstance());
    #else
        return NWK_MCPS_SapHandler((nwkToMcpsMessage_t*) pMsg, aZbTasksParams[0].macInstanceId);
    #endif
}

/*! *********************************************************************************
* \brief  Nwk to Mlme Sap called internal in zigbee stack, add mac istance
*
* \param[in]  mlmeMessage_t 
*
* \return  mac resultType_t
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/

resultType_t ZbNWK_MLME_SapHandler( mlmeMessage_t*      pMsg)
{
    #if(gInstantiableStackEnabled_d == 1)
        return NWK_MLME_SapHandler( (mlmeMessage_t* )pMsg, ZbGetMacInstance());
    #else
        return NWK_MLME_SapHandler( (mlmeMessage_t* )pMsg, aZbTasksParams[0].macInstanceId);
    #endif
}


/*! *********************************************************************************
* \brief  Keyboard module callback
*
* \param[in]  keycode
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

void ZbKeyboardCallback(key_event_t keyCode)
{
  #if !gInstantiableStackEnabled_d
  lastKeyCode = keyCode;
  (void)OSA_EXT_EventSet(mZbTaskEventId[0], gZbKeyboardEvent_d);
  #else
  switch(keyCode)
  {
    case gKBD_EventPB1_c:             
    case gKBD_EventPB2_c:             
    case gKBD_EventLongPB1_c:         
    case gKBD_EventLongPB2_c:         
      lastKeyCode = keyCode;
      (void)OSA_EXT_EventSet(mZbTaskEventId[0], gZbKeyboardEvent_d);
      break;
    case gKBD_EventPB3_c:             
    case gKBD_EventPB4_c:             
    case gKBD_EventLongPB3_c:         
    case gKBD_EventLongPB4_c:
      lastKeyCode = keyCode-2;
      (void)OSA_EXT_EventSet(mZbTaskEventId[1], gZbKeyboardEvent_d);
      break;
    default:
      break;
  }
  #endif
  
}

/*! *********************************************************************************
* \brief Set mac instance extendded address 
*
* \param[in]  pointer to extendded address string 
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
void MacPhyInit_WriteExtAddress(uint8_t *pExtendedAddress)
{
  
    FLib_MemCpy(ZbBeeStackGlobalsParams(aExtendedAddress), pExtendedAddress, 8);
  
    (void)SetPibAttributeValue(gMPibExtendedAddress_c,(void *)ZbBeeStackGlobalsParams(aExtendedAddress));
}

/*! *********************************************************************************
* \brief  Zigbee FSCI generic opcodes handler
*
* \param[in]  pointer to memory block received, comm interface, virtual com interface 
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
bool_t ZbFsciHandler(void *pData, uint32_t commInterfaceId)
{
#if gFsciIncluded_c  
  uint8_t inst = ZbFsciGetZbInstance(commInterfaceId);
  if(inst < gZbProInstancesCnt_d)
  {
    MSG_Queue( &aZbTaskZtcQueue[inst], pData );
    (void)OSA_EXT_EventSet(mZbTaskEventId[inst], gZbFsciEvent_d);
  }
#else
  (void)pData;
  (void)commInterfaceId;
#endif  
  return FALSE;
}

/*! *********************************************************************************
*************************************************************************************
* Private Functions
*************************************************************************************
********************************************************************************** */
/*! *********************************************************************************
* \brief  This function implements Zigbee Pro rtos task
*
* \param[in]  task param - instance id.
*
* \return  None.
*
* \pre
*
* \post
*
* \remarks
*
********************************************************************************** */

static void ZbPro_Task(osaTaskParam_t param)
{

#if(gInstantiableStackEnabled_d == 1)
  /* This variables will stay in task stack */
  /* Stack reserved for beestack internal variables */ 
  uint8_t  zbReservedRam[gZbInstReservedRam_d];
  
  /* beeapp data init  - application init data*/
  beeAppDataInit_t beeAppDataInit;

  #if TestProfileApp_d  
    /* beeapp data - application data*/
    beeAppData_t beeAppData;
  #endif
  
  #if !TestProfileApp_d   
    zclFoundationGlobals_t zclFoundationGlobals;
  #endif  
 
#endif /* (gInstantiableStackEnabled_d == 1) */
  
  /* Bestack timers table */ 
  tmrTimerTableEntry_t tmrTimerTable[gBeeStackMaxTimers_d];
  /* Events received by beestack task */
  osaEventFlags_t mZBEventFlags;
  /* BeeStack Instance timer handler */
  zbTmrTimerID_t instanceTimer;
  /* Previous timer execution in ticks */
  zbTmrTimeInMilliseconds_t zbPreviousTime = 0;
  /* put on task stack the threadId */
  osaTaskId_t threadId = OSA_EXT_TaskGetId();
      
  tsTaskTableEntry_t taskTable[gTsMaxTasks_c];
  
  tsTaskID_t taskIDsByPriority[gTsMaxTasks_c];   
  
  zbProInstance = (uint32_t)param;
  
  /*Rtos Timer callback params */  
  zbCallBackParams_t timerParams ={mZbTaskEventId[zbProInstance],(osaEventFlags_t)gZbTimerEvent_d};
  
  aZbTasksParams[zbProInstance].zbThreadId = threadId;
  
#if(gInstantiableStackEnabled_d == 1)
  
/* Create instances semaphore */
  if(NULL == zbSemaphore)
  {
    zbSemaphore = OSA_EXT_SemaphoreCreate(1);
    if( zbSemaphore == NULL )
    {
      panic( 0, (uint32_t)ZbPro_Task, 0, 0 );
    }
  }
    
  /* Set Ram to 0 */
  FLib_MemSet16(zbReservedRam, 0,gZbInstReservedRam_d);

  #if !TestProfileApp_d  
    /* Set zclFoundationGlobals to 0 */
    FLib_MemSet(&zclFoundationGlobals, 0, sizeof(zclFoundationGlobals_t));
   #endif  
  
/* Test code for ram usage */   
#if 0  
  {
      /* Test variable - remove later */     
      uint16_t ramSize;
      
      (void)BeeStackGetRamSpace(&zbStackDefaultInitTablesSizes,&ramSize);
   
      /* Check if is enough ram space for beestack dynamic data */  
      if(ramSize > gZbInstReservedRam_d)
      {
        panic( 0, (uint32_t)ZbPro_Task, 0, 0 );
      }
  }
#endif  
  
  if(gZbSuccess_c != BeeStackAllocRamSpace(&zbStackDefaultInitTablesSizes, 
                                           &zbReservedRam,
                                           gZbInstReservedRam_d,
                                           zbProInstance))
  {
    panic( 0, (uint32_t)ZbPro_Task, 0, 0 );
  }
  
  
  
#if !TestProfileApp_d   
  EZModeContextSwitch(zbProInstance);
  EZModeDataInitVariables(zbProInstance);
#endif
  
  BeeStackGlobalsSetCurrentInstance(zbProInstance);
   
  (void)BeeStackGlobalsInit(zbProInstance);
  /* Set application instance */
   
  pBeeAppInitData =  &beeAppDataInit;

#if TestProfileApp_d  
  /* beeapp data - application data*/
  pBeeAppData     =  &beeAppData;
#endif
  
  /* endpoints data */
  pEndPointData   =  &endPointDataTable[zbProInstance];
  
#if !TestProfileApp_d   
  pzclFoundationGlobals = &zclFoundationGlobals;
#endif  
  /* Init End point data */
    
  BeeAppDataInitVariables();
  
  NVM_RegisterBeeStackDataSet(zbProInstance);  

#endif /* (gInstantiableStackEnabled_d == 1) */ 
    
  /* Set tasks instance */
  TS_SetInstance(taskTable,taskIDsByPriority,gTsMaxTasks_c);
  
  /* Init Beestack internal tasksheduler */
  TS_Init();
   
  /* Create beestack timer */
  instanceTimer = ZbTmrAllocateSystemTimer();
  
  /* Set timer instance */
  ZbTMR_SetInstance(tmrTimerTable, instanceTimer, gBeeStackMaxTimers_d, &zbPreviousTime,ZbTimerCallBack, &timerParams);

  /* Initialize zbpro internal timer module */
  ZbTMR_Init();

  /* Init beestack componnents */
  BeeStackInit();
  
  /*initialize the application*/
  
  gAppTaskID = TS_CreateTask(gTsAppTaskPriority_c, BeeAppTask);
 
  /* Set ztc instance REMOVE after FSCI integration */
  ztcBeeStackInstId = zbProInstance;
   
   
  /* Start  Aplication task lines */
  /* Move later in AppTaskInit */
  
  
  /* Bind to mac */
  aZbTasksParams[zbProInstance].macInstanceId = 
                BindToMAC((instanceId_t)zbProInstance);
  
  if(gInvalidInstanceId_c == aZbTasksParams[zbProInstance].macInstanceId)
  {
    panic( 0, (uint32_t)ZbPro_Task, 0, 0 );
  }
  
  /* Register SAP Handlers */
  Mac_RegisterSapHandlers((MCPS_NWK_SapHandler_t)MCPS_NWK_SapHandler, 
                          (MLME_NWK_SapHandler_t)MLME_NWK_SapHandler, 
                           aZbTasksParams[zbProInstance].macInstanceId);
  #if gFsciIncluded_c
  
    /* Register mac fsci */
    fsciRegisterMac( (instanceId_t)aZbTasksParams[zbProInstance].macInstanceId, 
                      aZbTasksParams[zbProInstance].serialInterfaceId);
  
  
    /* Register ZbPro fsci */
    ZbRegisterToFsci(zbProInstance); 
    /* End  Aplication task lines */
    /* Move later in AppTaskInit */
  #endif  
    
  while(1)
  {
            
    /* Wait for events indefinite*/    
    (void)OSA_EXT_EventWait(mZbTaskEventId[zbProInstance],((osaEventFlags_t)(-1)),FALSE,osaWaitForever_c,&mZBEventFlags);
    
    #if(gInstantiableStackEnabled_d == 1)
      OSA_EXT_SemaphoreWait(zbSemaphore, osaWaitForever_c );
    #endif
        
     zbProInstance = (uint32_t)param;  
      
    /* Set timer instance */
    ZbTMR_SetInstance(tmrTimerTable, instanceTimer, gBeeStackMaxTimers_d, &zbPreviousTime,ZbTimerCallBack, &timerParams);
     
    /* Set tasks instance */
    TS_SetInstance(taskTable,taskIDsByPriority,gTsMaxTasks_c);
  
    /* Set current beestack instance */
    (void)BeeStackGlobalsSetCurrentInstance(zbProInstance);
    
    #if(gInstantiableStackEnabled_d == 1)
    
    /* Set application instance */
    #if !TestProfileApp_d 
      EZModeContextSwitch(zbProInstance);
    #endif  
      pBeeAppInitData =  &beeAppDataInit;
    #if TestProfileApp_d 
       /* beeapp data - application data*/
      pBeeAppData     =  &beeAppData;
    #endif
      /* endpoints data */
      pEndPointData   =  &endPointDataTable[zbProInstance];
    #if !TestProfileApp_d  
      pzclFoundationGlobals = &zclFoundationGlobals;
    #endif  
    #endif  
  
    
    /* Set ztc instance REMOVE after FSCI integration */
    ztcBeeStackInstId = zbProInstance;
        
#if !TestProfileApp_d
    /* Check keyboard */
    if( mZBEventFlags & gZbKeyboardEvent_d)
    {
      BeeAppHandleKeys(lastKeyCode);
      TS_Scheduler();
    }
#endif    
    
    /*check application init */
    if(mZBEventFlags & gZbStartEvent_d)
    {
      /* Call after Nvm module init - to be able to restore NVM data */
      BeeAppInit();
      TS_Scheduler();
    }
    
    
    /* Check timers */
    if( mZBEventFlags & gZbTimerEvent_d)
    {
       ZbTMR_Handler();
       TS_Scheduler();
    }
     
    /* Empty Mlme Queue */
    while(MSG_Pending(&aZbTaskMlmeNwkQueue[zbProInstance]))
    {
      (void)zbMLME_NWK_SapHandler(MSG_DeQueue(&aZbTaskMlmeNwkQueue[zbProInstance]));
      TS_Scheduler();
    }
     
    /* Empty Mcps Queue */
    while(MSG_Pending(&aZbTaskMcpsNwkQueue[zbProInstance]))
    {
      (void)zbMCPS_NWK_SapHandler(MSG_DeQueue(&aZbTaskMcpsNwkQueue[zbProInstance]));
       /* Run instance until all internal events are done*/
       /* Timer and MAC */ 
       TS_Scheduler();
    }
#if gFsciIncluded_c      
    /* Empty Fsci Queue */
    if(mZBEventFlags & gZbFsciEvent_d)
    {    
      while(MSG_Pending(&aZbTaskZtcQueue[zbProInstance]))
      {
         ZbProcessFsciPacket(MSG_DeQueue(&aZbTaskZtcQueue[zbProInstance]));
      } 
    }
#endif
    
    if(mZBEventFlags & gZbGenericEvent_d)
    {
       TS_Scheduler();
    }
    
    #if(gInstantiableStackEnabled_d == 1)
      OSA_EXT_SemaphorePost(zbSemaphore);
    #endif
  }

}

/*! *********************************************************************************
* \brief  MLME to NWK Sap handler - call in mac task
*
* \param[in]  mlme to nwk message, zigbee instance id
*
* \return  status 
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/

static uint8_t MLME_NWK_SapHandler(nwkMessage_t *pMsg,uint8_t zbInstanceId)
{

#if gFsciIncluded_c  
  FSCI_Monitor(gFSCI_MlmeSapId_c, 
                pMsg,
                gSuccess_c,
                fsciGetMacInterfaceId(aZbTasksParams[zbInstanceId].macInstanceId));
#endif
  MSG_Queue( &aZbTaskMlmeNwkQueue[zbInstanceId], pMsg );
  (void)OSA_EXT_EventSet(mZbTaskEventId[zbInstanceId], gZbMacEvent_d);
  if(zbInstanceId >= gZbProInstancesCnt_d)
  {
    panic( 0, (uint32_t)MLME_NWK_SapHandler, 0, 0 );
  }
  return gZbSuccess_c;
}

/*! *********************************************************************************
* \brief  MCPS to NWK Sap handler - call in mac task
*
* \param[in] mcps to  nwk message, zigbee instance id
*
* \return  status 
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
static uint8_t MCPS_NWK_SapHandler(nwkMessage_t *pMsg, uint8_t zbInstanceId)
{
 
#if gFsciIncluded_c
  FSCI_Monitor(gFSCI_McpsSapId_c, 
               pMsg,
               gSuccess_c,
               fsciGetMacInterfaceId(aZbTasksParams[zbInstanceId].macInstanceId));
#endif  
  
  MSG_Queue( &aZbTaskMcpsNwkQueue[zbInstanceId], pMsg );
  (void)OSA_EXT_EventSet(mZbTaskEventId[zbInstanceId], gZbMacEvent_d);
  if(zbInstanceId >= gZbProInstancesCnt_d)
  {
    panic( 0, (uint32_t)MCPS_NWK_SapHandler, 0, 0 );
    return gZbFailed_c;
  }
  return gZbSuccess_c;
}
/*! *********************************************************************************
* \brief  Get mac instance bind to zigbee current instance
*
* \param[in]  None.
*
* \return  mac instance
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
static uint32_t ZbGetMacInstance(void)
{
  uint8_t inst;
  
  /* Get current zigbee instance thread Id */
  osaTaskId_t threadId = OSA_EXT_TaskGetId();
  
  /* Look for mac bindded instance */
  for (inst = 0; inst < gZbProInstancesCnt_d; inst++)
  {
     if(threadId == aZbTasksParams[inst].zbThreadId)
     {
       return aZbTasksParams[inst].macInstanceId;
     }
  }
  if(inst == gZbProInstancesCnt_d)
  {
    panic( 0, (uint32_t)ZbGetMacInstance, 0, 0 );
  }
  return gInvalidInstanceId_c;
}

/*! *********************************************************************************
* \brief  Zigbee pro rtos timer callback
*
* \param[in]  callback params - thread and event
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
static void ZbTimerCallBack (void *param)
{
  /* Send event to the beestack mqx task */
  (void)OSA_EXT_EventSet(((zbCallBackParams_t*)param)->eventId,(osaEventFlags_t)((zbCallBackParams_t*)param)->event);
};

#if gFsciIncluded_c
/*! *********************************************************************************
* \brief  This function  register ZbPro Layer opcodes to FSCI 
*
* \param[in]  zigbee instance Id
*
* \return  None.
*
* \pre
*
* \post
*
* \remarks
*
********************************************************************************** */
static void ZbRegisterToFsci(uint32_t zbInstanceId)
{
  uint8_t regIndex;
  
  for(regIndex = 0; regIndex < gZbFsciOpcodeCnt_d ; regIndex++)
  {
    if(gFsciSuccess_c != FSCI_RegisterOpGroup( aZbFsciOpcodes[regIndex],    gFsciMonitorMode_c, ZbFsciOpcodesHandler, 
                                              (void*)zbInstanceId, aZbTasksParams[zbInstanceId].serialInterfaceId))
    {
      panic( 0, (uint32_t)ZbRegisterToFsci, 0, 0 );
    }
  }
}

/*! *********************************************************************************
* \brief  Get current zigbee instance for fsci
*
* \param[in]  serial interfaceId, virtual serial interface Id
*
* \return  zigbee instance bind to comm interfaceId and comm virtual serial interface Id or gInvalidInstanceId_c
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
static uint32_t ZbFsciGetZbInstance(uint32_t commInterfaceId)
{
  uint8_t inst;
  
  for (inst = 0; inst < gZbProInstancesCnt_d; inst++)
  {
     if(commInterfaceId == aZbTasksParams[inst].serialInterfaceId)
     {
       return inst;
     }
  }
  if(inst == gZbProInstancesCnt_d)
  {
    panic( 0, (uint32_t)ZbFsciGetZbInstance, 0, 0 );
  }
  return gInvalidInstanceId_c;
}

/*! *********************************************************************************
* \brief  Get current fsci instance for zigbee
*
* \param[in] zigbee instance bind to comm interfaceId and comm virtual serial interface Id or gInvalidInstanceId_c
*
* \return  serial interfaceId, virtual serial interface Id 
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
uint32_t ZbGetFsciInstance(void)
{
  uint8_t inst;
    
  /* Get current zigbee instance thread Id */
  osaTaskId_t  threadId = OSA_EXT_TaskGetId();
  
  /* Look for zigbee serial and virtual instances */
  for (inst = 0; inst < gZbProInstancesCnt_d; inst++)
  {
     if(threadId == aZbTasksParams[inst].zbThreadId)
     {
       return aZbTasksParams[inst].serialInterfaceId;
     }
  }
  
  if(inst == gZbProInstancesCnt_d)
  {
    panic( 0, (uint32_t)ZbGetFsciInstance, 0, 0 );
  }
  return gInvalidInstanceId_c;
}


/*! *********************************************************************************
* \brief  Generic FSCI opcodegroups handler
*
* \param[in]  mlmeMessage_t 
*
* \return  mac resultType_t
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
static void ZbFsciOpcodesHandler(void *pData, void* param, uint32_t commInterfaceId)
{
  uint8_t inst = ZbFsciGetZbInstance(commInterfaceId);
  if(inst < gZbProInstancesCnt_d)
  {
    MSG_Queue( &aZbTaskZtcQueue[inst], pData );
    (void)OSA_EXT_EventSet(mZbTaskEventId[inst], gZbFsciEvent_d);
  }
}

/*! *********************************************************************************
* \brief  ZbProcessFsciPacket
*
* \param[in]  clientPacket_t*
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
static void ZbProcessFsciPacket(clientPacket_t* pMsg)
{
  bool_t msgHandledByZtc = TRUE;
  
  if(pMsg->structured.header.opGroup == gFSCI_ReqOpcodeGroup_c || pMsg->structured.header.opGroup == gFSCI_CnfOpcodeGroup_c)
  {
    if(ZbFsciIsOtaPacket(pMsg->structured.header.opCode))
    {
      if(FSCI_OtaSupportHandlerFunc(pMsg, aZbTasksParams[zbProInstance].serialInterfaceId))
      {
        /* Reuse received message */
        ((clientPacket_t*)pMsg)->structured.header.opGroup = gFSCI_CnfOpcodeGroup_c;
        FSCI_transmitFormatedPacket( pMsg, aZbTasksParams[zbProInstance].serialInterfaceId );
      }
      
      msgHandledByZtc = FALSE;
    }
  }
  
  if(msgHandledByZtc)
  {
    Ztc_Task((uint8_t*)pMsg, zbProInstance);
    /* Run instance until all internal events are done*/
    TS_Scheduler();
  }
}

/*! *********************************************************************************
* \brief  ZbFsciIsOtaPacket
*
* \param[in]  opCode_t
*
* \return  bool_t
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
static bool_t ZbFsciIsOtaPacket(opCode_t opCode)
{
  return ((opCode == mFsciOtaSupportSetModeReq_c)
          || (opCode == mFsciOtaSupportStartImageReq_c)
            || (opCode == mFsciOtaSupportPushImageChunkReq_c)
              || (opCode == mFsciOtaSupportCommitImageReq_c)
                || (opCode == mFsciOtaSupportCancelImageReq_c)
                  || (opCode == mFsciOtaSupportSetFileVerPoliciesReq_c)
                    || (opCode == mFsciOtaSupportAbortOTAUpgradeReq_c)
                      || (opCode == mFsciOtaSupportImageChunkReq_c)
                        || (opCode == mFsciOtaSupportQueryImageRsp_c)
                          || (opCode == mFsciOtaSupportImageNotifyReq_c));
}

/*! *********************************************************************************
* \brief  Transmit fsci blocks from zigbee test client
*
* \param[in]  mlmeMessage_t 
*
* \return  mac resultType_t
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
void ZtcComm_TransmitBufferBlock(uint8_t *pSrc, uint8_t length)
{
  uint8_t inst;
  
  /* add 3 bytes for stx, virtual interface and checksum */
  uint8_t *pPacket = MSG_Alloc(length + 3);
  
  if(!pPacket)
  {
    return;
  }
  
  /* Copy payload in buffer */
  FLib_MemCpy(pPacket,pSrc,length);
  
  /* Get current zigbee instance thread Id */
  osaTaskId_t threadId = OSA_EXT_TaskGetId();
  
  /* Look for zigbee serial and virtual instances */
  for (inst = 0; inst < gZbProInstancesCnt_d; inst++)
  {
     if(threadId == aZbTasksParams[inst].zbThreadId)
     {
       FSCI_transmitFormatedPacket(pPacket,aZbTasksParams[inst].serialInterfaceId);
     }
  }
  
}
#endif

/*! *********************************************************************************
* \brief  ZbApplicationInit
*
* \param[in]  None
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
void ZbApplicationInit(void)
{
    for(uint8_t i=0 ; i < gZbProInstancesCnt_d; i++ )
    { 
      (void)OSA_EXT_EventSet(mZbTaskEventId[i], gZbStartEvent_d);
    }
}

/*! *********************************************************************************
* \brief  Trigger Zigbee RTOS task 
*
* \param[in]  zigbee instance
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
void ZbTriggerZigbeeRTOSTask(uint8_t instanceId)
{
    if (instanceId >= gZbProInstancesCnt_d)
    {
        return;
    }

    (void)OSA_EXT_EventSet(mZbTaskEventId[instanceId], gZbGenericEvent_d);
    
}