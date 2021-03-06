/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file Flash_Adapter.h
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

#ifndef __FLASH_ADAPTER_H__
#define __FLASH_ADAPTER_H__

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"
#include "SSD_FTFx.h"


/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
/*
 * Name: gNvDisableIntCmdSeq_c
 * Description: this macro is used to enable/disable interrupts when the
 *              FTFL controller executes a command sequence. This macro
 *              has to be set according to NVM configuration. Therefore,
 *              if the FLASH region used by the NVM is placed in the same
 *              program block as the ISR's executable code, the interrupts 
 *              MUST be disabled (because no code pre-fetching can be performed
 *              while FTFL controller executes a command sequence, i.e. 
 *              program/erase). If the interrupts are not disabled, the 
 *              system will assert a hard fault when the flash controller 
 *              executes a command sequnce and an IRQ is about to be handled.
 *              If the NVM region is placed in a different program block than
 *              the ISR's code, this macro shall be set to FALSE (recommended).
 */      
#ifndef gNvDisableIntCmdSeq_c
#define gNvDisableIntCmdSeq_c           (1)
#endif

/* size of array to copy__Launch_Command function to.*/
/* It should be at least equal to actual size of __Launch_Command func */
/* User can change this value based on RAM size availability and actual size of __Launch_Command function */
#define LAUNCH_CMD_SIZE         64
/* Program Flash block information */
#define P_FLASH_SIZE            (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT)
#define P_BLOCK_NUM             FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT
#define P_SECTOR_SIZE           FSL_FEATURE_FLASH_PFLASH_BLOCK_SECTOR_SIZE
/* Data Flash block information */
#define FLEXNVM_BASE            FSL_FEATURE_FLASH_FLEX_NVM_START_ADDRESS
#define FLEXNVM_SECTOR_SIZE     FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SECTOR_SIZE
#define FLEXNVM_BLOCK_SIZE      FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SIZE
#define FLEXNVM_BLOCK_NUM       FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_COUNT
/* Other defines */
#define DEBUGENABLE             0x00
#define FTFx_REG_BASE           0x40020000
#define P_FLASH_BASE            0x00000000

/* Flex Ram block information */
#define EERAM_BASE              FSL_FEATURE_FLASH_FLEX_RAM_START_ADDRESS
#define EERAM_SIZE              FSL_FEATURE_FLASH_FLEX_RAM_SIZE

#define READ_NORMAL_MARGIN        0x00
#define READ_USER_MARGIN          0x01
#define READ_FACTORY_MARGIN       0x02

#define NV_FlashRead(pSrc, pDest, size) FLib_MemCpy((void*)(pDest), (void*)(pSrc), size);


/*! *********************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
extern FLASH_SSD_CONFIG gFlashConfig;
extern pFLASHCOMMANDSEQUENCE gFlashLaunchCommand;


/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */


/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */
void NV_Init(void);

uint32_t NV_FlashProgramUnaligned(PFLASH_SSD_CONFIG pSSDConfig,
                                  uint32_t dest,
                                  uint32_t size,
                                  uint8_t* pData,
                                  pFLASHCOMMANDSEQUENCE pFlashCommandSequence);

uint32_t NV_FlashEraseSector(PFLASH_SSD_CONFIG pSSDConfig,
                                  uint32_t dest,
                                  uint32_t size,
                                  pFLASHCOMMANDSEQUENCE pFlashCommandSequence);

#endif /* __FLASH_ADAPTER_H__ */