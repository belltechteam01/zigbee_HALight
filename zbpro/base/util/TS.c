/*! *********************************************************************************
* \file TS_Kernel.c
* This is the source file for the TS_Kernel. Kernel / task handling
* internal zigbee tasks.
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
#include "FunctionLib.h"
#include "TS_Interface.h"
#include "ZigbeeTask.h"

/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/* Number of elements in an array. */
#ifndef NumberOfElements
  #define NumberOfElements(array)     ((sizeof(array) / (sizeof(array[0]))))
#endif


/*! *********************************************************************************
*************************************************************************************
* Private type declarations
*************************************************************************************
************************************************************************************/

/* None */

/*! *********************************************************************************
*************************************************************************************
* Private memory definitions
*************************************************************************************
************************************************************************************/

/* Pointer to active instance tasks table */
tsTaskTableEntry_t* mpTsTaskTable;

/* Pointer to sorted by task priority active instance table */
/* the highest priority tasks at lower index positions */
tsTaskID_t* mpTsTaskIDsByPriority;

/*Max No of tasks for active instance */
uint8_t mMaxNoOfTasks;

/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/

/* None */

/*! *********************************************************************************
*************************************************************************************
* Public memory definitions
*************************************************************************************
************************************************************************************/

/* None */

/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  Set active instance params 
*
* \param[in]  pointer to task table, pointer to priority table and max no of tasks 
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
void TS_SetInstance
(
  tsTaskTableEntry_t* pTaskTable,
  tsTaskID_t* pPriorityTaks,
  uint8_t maxNoTasks
)
{
  mpTsTaskTable = pTaskTable;
  mpTsTaskIDsByPriority = pPriorityTaks;
  mMaxNoOfTasks  = maxNoTasks;
}


/* Remove events from a task's event flags. */
void TS_ClearEvent
(
  tsTaskID_t taskID,
  tsEvent_t events
)
{
 
  mpTsTaskTable[taskID].events &= ~events;

}

/*! *********************************************************************************
* \brief  Add a task to the task table.
*         Return the task ID, or gTsInvalidTaskID_c if the task table is full.
*
*         taskPriority == 0 is reserved for the idle task, and must never be specified
*         for any other task. TS_CreateTask() does not check for this.
*
*         Note that TS_CreateTask() does not prevent a given event handler function
*         pointer from being added more than once to the task table.
*
*         Note that if TS_CreateTask() is called with a taskPriority that is the
*         same as the priority of a task that is already in the task table, the
*         two tasks will end up in adjacent slots in the table. Which one is
*         called first by the scheduler is not specified.
* \param[in]  task priority and pointer to task function
*
* \return  new task id
*
* \pre
*
* \post
*
* \remarks
*
********************************************************************************** */
tsTaskID_t TS_CreateTask
(
  tsTaskPriority_t taskPriority,  
  pfTsTaskEventHandler_t pfTaskEventHandler
)
{
  uint8_t i;
  uint8_t freeSlot;
  uint8_t sizeofTaskId = sizeof(tsTaskID_t);

  /* Try to find a free slot in the task table. */
  for (i = 0, freeSlot = gTsInvalidTaskID_c;
       i < mMaxNoOfTasks;
       ++i) {
    if (mpTsTaskTable[i].priority == gTsInvalidTaskPriority_c) {
      freeSlot = i;
      break;
    }
  }                                     /* for (i = 0, freeSlot = 0xFF; ... */

  if (freeSlot == gTsInvalidTaskID_c) {
    return gTsInvalidTaskID_c;
  }

  mpTsTaskTable[freeSlot].events = 0;
  mpTsTaskTable[freeSlot].pfTaskEventHandler = pfTaskEventHandler;
  mpTsTaskTable[freeSlot].priority = taskPriority;

  /* maTsTaskIDsByPriority is maintained in sorted order, so 1) find the */
  /* place where this new task should go; 2) move everything up; and 3) add */
  /* the new task. */
  for (i = 0; i < mMaxNoOfTasks; i++) {
    /* If the end of the table is reached, just add the new task. */
    if (mpTsTaskIDsByPriority[i] == gTsInvalidTaskPriority_c) {
      break;
    }

    /* If all tasks from this point on have lower priorities than the task */
    /* being added, move the rest up and insert the new one. */
    if (mpTsTaskTable[mpTsTaskIDsByPriority[i]].priority < taskPriority) {
      FLib_MemInPlaceCpy(&mpTsTaskIDsByPriority[i + 1],
                         &mpTsTaskIDsByPriority[i],
                         (mMaxNoOfTasks - i - 1) * sizeofTaskId);
      break;
    }
  }                                     /* for (i = 0; ... */
  mpTsTaskIDsByPriority[i] = freeSlot;

  return freeSlot;
}

/*! *********************************************************************************
* \brief      Remove a task from the task table. 
*
* \param[in]  taskId
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

void TS_DestroyTask
(
  tsTaskID_t taskID
)
{
  uint8_t i;
  uint8_t sizeofTaskId = sizeof(tsTaskID_t);

  if (mpTsTaskTable[taskID].priority == gTsInvalidTaskPriority_c) {
    return;
  }

  /* Mark this slot in the task descriptor table as unused. */
  mpTsTaskTable[taskID].priority = gTsInvalidTaskPriority_c;

  /* Remove this task's ID from the priority table. Find it's position */
  /* in the table, and shift everything else down. */
  for (i = 0; i < mMaxNoOfTasks; i++) {
    if (mpTsTaskIDsByPriority[i] == taskID) {

      FLib_MemCpy(&mpTsTaskIDsByPriority[i],
                  &mpTsTaskIDsByPriority[i + 1],
                  (mMaxNoOfTasks - i - 1) * sizeofTaskId);

      /* Note that exactly one entry was removed. */
      mpTsTaskIDsByPriority[mMaxNoOfTasks - 1] = gTsInvalidTaskID_c;
      break;
    }
  }

  return;
} 
/*! *********************************************************************************
* \brief  Initialize the task scheduler
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
void TS_Init(void) 
{
  FLib_MemSet(mpTsTaskTable, gTsInvalidTaskPriority_c, mMaxNoOfTasks*sizeof(tsTaskTableEntry_t));
  FLib_MemSet(mpTsTaskIDsByPriority, gTsInvalidTaskID_c, mMaxNoOfTasks*sizeof(tsTaskID_t));
}                                       

/*! *********************************************************************************
* \brief  Returns true if there are any pending events for any task
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
uint8_t TS_PendingEvents(void) 
{
  uint8_t i;

  for (i = 0; i < mMaxNoOfTasks; i++) {
    if (( mpTsTaskTable[i].priority != gTsInvalidTaskPriority_c)
        && mpTsTaskTable[i].events) {
      return TRUE;
    }
  }

  return FALSE;
}

/*! *********************************************************************************
* \brief Send events to a task
*
* \param[in]  taskId , events mask
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
void TS_SendEvent
(
  tsTaskID_t taskID,
  tsEvent_t events
)
{
  
  mpTsTaskTable[taskID].events |= events;
#if !gInstantiableStackEnabled_d
  ZbTriggerZigbeeRTOSTask(0);
#endif
} /* TS_SendEvent() */

/*! *********************************************************************************
* \brief  BeeStack's main task loop. Never returns. This function is the center of
 *        the task system.
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
void TS_Scheduler(void) 
{
  uint8_t activeTask;
  tsEvent_t events;
  uint8_t i;
  uint8_t taskID;
  uint8_t taskRepeatCounter=0;
  
  tsEvent_t lastEvents = 0;
  pfTsTaskEventHandler_t pfLastTaskEventHandle = NULL;
  
  /* mpTsTaskIDsByPriority[] is maintained in task priority order. If there */
  /* are fewer than the maximum number of tasks, the first gInvalidTaskID_c */
  /* marks the end of the table. */
  for (;;) {
    /* Look for the highest priority task that has an event flag set. */
    activeTask = gTsInvalidTaskID_c;
    for (i = 0; i < mMaxNoOfTasks; ++i) {
      taskID = mpTsTaskIDsByPriority[i];
      if (taskID == gTsInvalidTaskID_c) {
        break;
      }

      if (mpTsTaskTable[taskID].events) {
        activeTask = taskID;
        break;
      }
    }

    /* If there are no outstanding events, go to MQX task. */
    if (gTsInvalidTaskID_c == activeTask )
    {
      break;
    }
    else
    {
      events = mpTsTaskTable[activeTask].events;
      mpTsTaskTable[activeTask].events = 0;

      (*mpTsTaskTable[activeTask].pfTaskEventHandler)(events);
      /* In case the sheduler is in a loop the leave because we expect an outside mqx task event */
      if((lastEvents == events) && (pfLastTaskEventHandle == mpTsTaskTable[activeTask].pfTaskEventHandler))
      {
        taskRepeatCounter++;
        if(taskRepeatCounter == 10)
        {
          break;
        }
      }
      else
      {
        taskRepeatCounter=0;
        lastEvents = events;
        pfLastTaskEventHandle = mpTsTaskTable[activeTask].pfTaskEventHandler;
      }
    }
  }                                     /* for (;;) */
}                                       /* TS_Scheduler() */

