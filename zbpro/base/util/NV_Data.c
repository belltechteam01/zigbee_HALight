/******************************************************************************
 * Filename: NV_Data.c
 *
 * Description: Data definitions for the application client of the  NV  
 *              storage module (NVM)
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
 */

#include "EmbeddedTypes.h"
#include "NVM_Interface.h"
#include "NV_Data.h"
#include "FunctionLib.h"
#include "BeeStack_Globals.h"
#include "BeeApp.h"
#include "ASL_UserInterface.h"

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

#ifndef gAPP_DATA_SET_FOR_NVM
  #define gAPP_DATA_SET_FOR_NVM {NULL,0,0,gNvInvalidDataEntry_c}
#endif

#define gMaxNVDataTableEntries_c gNvTableEntriesCountMax_c
/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

#if gNvStorageIncluded_d && gInstantiableStackEnabled_d
static void ZbInitNvmTableEntry(void);
static void ZbRegisterNvmTableEntry(void* ptrData,uint16_t uniqueId,uint16_t elemCount, uint16_t elemSize);
#endif

/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/

#if(gInstantiableStackEnabled_d == 1)
extern zbStackTablesSizes_t* pZbStackTablesSizes; 
#endif

#if gNvStorageIncluded_d
/*
 * Name: NvDataTable
 * Description: NVM data table. Contains entries of datasets.
 *              Defined by appication.
 */
#if !gInstantiableStackEnabled_d
const NVM_DataEntry_t NVM_DataTable[] =
{
  #define NVM_TblEntry(adress,count,size,id) {adress,count,size,id},
  /* Zigbee NWK and APS data sets */
  #include "BeeStackNvmDataSets.h"
  #undef  NVM_TblEntry(adress,count,size,id)
  /* Application data sets */
  gAPP_DATA_SET_FOR_NVM,  
  /* Required end-of-table marker. */
  {NULL,0,0,gNvEndOfTableId_c}  
};
#else
NVM_DataEntry_t NVM_DataTable[gMaxNVDataTableEntries_c]= 
{
  gAPP_DATA_SET_FOR_NVM,
  /* Required end-of-table marker. */
  {NULL,0,0,gNvInvalidDataEntry_c} 
};

#endif
/*
 * Name: pNVM_DataTable
 * Description: Pointer to NVM table. The content of the table
 * is defined by the application code. See NvDataTable.
 */
NVM_DataEntry_t* pNVM_DataTable = (NVM_DataEntry_t*)NVM_DataTable;

#endif

#if gInstantiableStackEnabled_d

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
/*! *********************************************************************************
* \brief  Register beestack nvm data sets for all instances. Called from RTOS zigbee task.
*
* \param[in]  beestack instance id 
*
* \return  void
*
* \pre
*
* \post
*
* \remarks
*
************************************************************************************/
void NVM_RegisterBeeStackDataSet(uint8_t beeStackInstanceId)
{
   #if gNvStorageIncluded_d
     uint16_t idOffset = (uint16_t)(0x000F&beeStackInstanceId)<<12;
     ZbInitNvmTableEntry();
     #define NVM_TblEntry(adress,count,size,id) \
           ZbRegisterNvmTableEntry((void*)adress,\
                         (uint16_t)(id)|(uint16_t)idOffset,\
                         (count),\
                         (size));
     /* Expand in calls of NvRegisterTableEntry with beestack datasets */
     #include "BeeStackNvmDataSets.h"
     #undef  NVM_TblEntry(adress,count,size,id)
   #else
     (void)beeStackInstanceId;        
   #endif        
}
#if gNvStorageIncluded_d
/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
static void ZbInitNvmTableEntry(void)
{
  uint16_t indexNvmTable;
  bool_t startFlag=FALSE;
  for(indexNvmTable = 0; indexNvmTable < gMaxNVDataTableEntries_c-1; indexNvmTable++)
  {
    if(NVM_DataTable[indexNvmTable].DataEntryID == gNvInvalidDataEntry_c)
    {
      startFlag=TRUE;
    }
    if(FALSE == startFlag)
    { 
      continue;
    }
    NVM_DataTable[indexNvmTable].DataEntryID = gNvInvalidDataEntry_c;
    NVM_DataTable[indexNvmTable].pData = NULL; 
  }
  NVM_DataTable[indexNvmTable].DataEntryID = gNvEndOfTableId_c;
  NVM_DataTable[indexNvmTable].pData = NULL; 
}

static void ZbRegisterNvmTableEntry(void* ptrData,uint16_t uniqueId,uint16_t elemCount, uint16_t elemSize)
{
  uint16_t indexNvmTable;
  for(indexNvmTable = 0; indexNvmTable < gMaxNVDataTableEntries_c-1; indexNvmTable++)
  {
    if(NVM_DataTable[indexNvmTable].DataEntryID != gNvInvalidDataEntry_c)
    {
      continue;
    }
    NVM_DataTable[indexNvmTable].DataEntryID = uniqueId;
    NVM_DataTable[indexNvmTable].ElementsCount = elemCount;
    NVM_DataTable[indexNvmTable].ElementSize = elemSize;
    NVM_DataTable[indexNvmTable].pData = ptrData;
    break;
  }
} 
#endif /* gInstantiableStackEnabled_d */
#endif /*gNvStorageIncluded_d */

