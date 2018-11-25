/*!
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
* \file NV_FlashHAL.h
* Description: Header file of the non-volatile storage module flash
*              controller HAL for Cortex-M4 processor     
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


#ifndef _NV_FLASHHAL_H_
#define _NV_FLASHHAL_H_

#include "IO_Map.h"
#include "EmbeddedTypes.h"

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/***************************************************************** 
 *                  MCU SPECIFIC DEFINITIONS                     *
 *****************************************************************/	
#if defined(MCU_MK21D5)

/* FTFL clock gating register */
#define gFTFL_SIM_SCGC_c                         SIM_SCGC6
/* FTFL clock gating mask */
#define gSIM_SCGC_FTFL_MASK_c                    SIM_SCGC6_FTFL_MASK
/* FTFL clock divider register */
#define gFTFL_SIM_CLKDIV_c                       SIM_CLKDIV1
#define gSIM_CLKDIV_OUTDIV                       SIM_CLKDIV1_OUTDIV4
#define gCLKDIV_VALUE_c                          16
/* FTFX registers base address */
#define gNV_FTFX_REG_BASE_c                      0x40020000

#elif defined(MCU_MKL26Z4) || defined(MCU_MKL46Z4)

#define gFTFL_SIM_SCGC_c                         SIM_SCGC6
/* FTFA clock gating mask */
#define gSIM_SCGC_FTFL_MASK_c                    SIM_SCGC6_FTF_MASK
/* FTFA clock divider register */
#define gFTFL_SIM_CLKDIV_c                       SIM_CLKDIV1
#define gSIM_CLKDIV_OUTDIV                       SIM_CLKDIV1_OUTDIV4
#define gCLKDIV_VALUE_c                          16
/* FTFX registers base address */
#define gNV_FTFX_REG_BASE_c                      0x40020000

#endif


/*
 * Name: gNVM_Pblocksize_Dblocksize_EERAMsize_Psectorsize_Dsectorsize
 * Description: Define flash derivatives
 */
#define gNV_256K_256K_4K_2K_2K_c        1
#define gNV_512K_0K_0K_2K_0K_c          2
#define gNV_512K_512K_16K_4K_4K_c       5
#define gNV_1024K_0K_16K_4K_0K_c        6
#define gNV_32K_0K_0K_1K_0K_c           7
#define gNV_32K_32K_2K_1K_1K_c          8
#define gNV_64K_0K_0K_1K_0K_c           9
#define gNV_64K_32K_2K_1K_1K_c          10
#define gNV_128K_0K_0K_1K_0K_c          11
#define gNV_128K_32K_2K_1K_1K_c         12
#define gNV_64K_32K_2K_2K_1K_c          13
#define gNV_128K_32K_2K_2K_1K_c         14
#define gNV_256K_32K_2K_2K_1K_c         15
#define gNV_256K_64K_4K_2K_1K_c         16
#define gNV_256K_0K_0K_1K_0K_c          17

/*
 * Name: gNV_Derivative_c
 * Description: derivative used by the target system
 */

#if defined(MCU_MK21D5)
#define gNV_Derivative_c                gNV_512K_0K_0K_2K_0K_c

#elif defined(MCU_MKL26Z4)
#define gNV_Derivative_c                gNV_128K_0K_0K_1K_0K_c

#elif defined(MCU_MKL46Z4)
#define gNV_Derivative_c                gNV_256K_0K_0K_1K_0K_c
#endif
   
   
/*
 * Return Code Definition for NVM
 */
#define gNV_OK_c                          0x0000
#define gNV_ERR_SIZE_c                    0x0001
#define gNV_ERR_RANGE_c                   0x0002
#define gNV_ERR_ACCERR_c                  0x0004
#define gNV_ERR_PVIOL_c                   0x0008
#define gNV_ERR_MGSTAT0_c                 0x0010
#define gNV_ERR_CHANGEPROT_c              0x0020
#define gNV_ERR_EEESIZE_c                 0x0040
#define gNV_ERR_EFLASHSIZE_c              0x0080
#define gNV_ERR_ADDR_c                    0x0100
#define gNV_ERR_NOEEE_c                   0x0200
#define gNV_ERR_EFLASHONLY_c              0x0400
#define gNV_ERR_DFLASHONLY_c              0x0800
#define gNV_ERR_RDCOLERR_c                0x1000
#define gNV_ERR_RAMRDY_c                  0x2000

/* Flash security status */
#define gNV_FLASH_SECURITY_STATE_KEYEN_c         0x80
#define gNV_FLASH_SECURITY_STATE_UNSECURED_c     0x02

#define gNV_FLASH_NOT_SECURE_c                   0x01
#define gNV_FLASH_SECURE_BACKDOOR_ENABLED_c      0x02
#define gNV_FLASH_SECURE_BACKDOOR_DISABLED_c     0x04

/* macro for flash configuration field offset */
#define gNV_FLASH_CNFG_START_ADDRESS_c           0x400
#define gNV_FLASH_CNFG_END_ADDRESS_c             0x40F

/* EERAM Function Control Code */
#define gNV_EEE_ENABLE_c                         0x00
#define gNV_EEE_DISABLE_c                        0xFF

/*--------------- NV Flash Module Memory Offset Map -----------------*/
        
/* Flash Status Register (FSTAT)*/
#define gNV_FSTAT_OFFSET_c               0x00000000
#define gNV_FSTAT_CCIF_c                 0x80
#define gNV_FSTAT_RDCOLERR_c             0x40
#define gNV_FSTAT_ACCERR_c               0x20
#define gNV_FSTAT_FPVIOL_c               0x10
#define gNV_FSTAT_MGSTAT0_c              0x01

/* Flash configuration register (FCNFG)*/
#define gNV_FCNFG_OFFSET_c               0x00000001
#define gNV_FCNFG_CCIE_c                 0x80
#define gNV_FCNFG_RDCOLLIE_c             0x40
#define gNV_FCNFG_ERSAREQ_c              0x20
#define gNV_FCNFG_ERSSUSP_c              0x10
#define gNV_FCNFG_RAMRDY_c               0x02
#define gNV_FCNFG_EEERDY_c               0x01

/* Flash security register (FSEC) */
#define gNV_FSEC_OFFSET_c                0x00000002
#define gNV_FSEC_KEYEN_c                 0xC0
#define gNV_FSEC_FSLACC_c                0x0C
#define gNV_FSEC_SEC_c                   0x03

/* Flash Option Register (FOPT) */
#define gNV_FOPT_OFFSET_c                0x00000003

/* Flash common command object registers (FCCOB0-B) */
#define gNV_FCCOB0_OFFSET_c              0x00000007
#define gNV_FCCOB1_OFFSET_c              0x00000006
#define gNV_FCCOB2_OFFSET_c              0x00000005
#define gNV_FCCOB3_OFFSET_c              0x00000004
#define gNV_FCCOB4_OFFSET_c              0x0000000B
#define gNV_FCCOB5_OFFSET_c              0x0000000A
#define gNV_FCCOB6_OFFSET_c              0x00000009
#define gNV_FCCOB7_OFFSET_c              0x00000008
#define gNV_FCCOB8_OFFSET_c              0x0000000F
#define gNV_FCCOB9_OFFSET_c              0x0000000E
#define gNV_FCCOBA_OFFSET_c              0x0000000D
#define gNV_FCCOBB_OFFSET_c              0x0000000C

/* P-Flash protection registers (FPROT0-3) */
#define gNV_FPROT0_OFFSET_c              0x00000013
#define gNV_FPROT1_OFFSET_c              0x00000012
#define gNV_FPROT2_OFFSET_c              0x00000011
#define gNV_FPROT3_OFFSET_c              0x00000010

/* D-Flash protection registers (FDPROT) */
#define gNV_FDPROT_OFFSET_c              0x00000017

/* EERAM Protection Register (FEPROT)  */
#define gNV_FEPROT_OFFSET_c              0x00000016
        
/*------------- Flash hardware algorithm operation commands -------------*/
#define gNV_VERIFY_BLOCK_c               0x00
#define gNV_VERIFY_SECTION_c             0x01
#define gNV_PROGRAM_CHECK_c              0x02
#define gNV_READ_RESOURCE_c              0x03
#define gNV_PROGRAM_LONGWORD_c           0x06
#define gNV_PROGRAM_PHRASE_c             0x07
#define gNV_ERASE_BLOCK_c                0x08
#define gNV_ERASE_SECTOR_c               0x09
#define gNV_PROGRAM_SECTION_c            0x0B
#define gNV_VERIFY_ALL_BLOCK_c           0x40
#define gNV_READ_ONCE_c                  0x41
#define gNV_PROGRAM_ONCE_c               0x43
#define gNV_ERASE_ALL_BLOCK_c            0x44
#define gNV_SECURITY_BY_PASS_c           0x45
#define gNV_PFLASH_SWAP_c                0x46
#define gNV_PROGRAM_PARTITION_c          0x80
#define gNV_SET_EERAM_c                  0x81

/* Flash margin read settings */
#define gNV_MARGIN_NORMAL_c              0x0000
#define gNV_USER_MARGIN_LEVEL1_c         0x0001
#define gNV_USER_MARGIN_LEVEL0_c         0x0002
#define gNV_FACTORY_MARGIN_LEVEL0_c      0x0003
#define gNV_FACTORY_MARGIN_LEVEL1_c      0x0004

/*------------------- Read/Write/Set/Clear Operation Macros ----------------------*/

#define gNV_REG_BIT_SET(address, mask)      (*(vuint8_t*)(address) |= (mask))
#define gNV_REG_BIT_CLEAR(address, mask)    (*(vuint8_t*)(address) &= ~(mask))
#define gNV_REG_BIT_TEST(address, mask)     (*(vuint8_t*)(address) & (mask))
#define gNV_REG_WRITE(address, value)       (*(vuint8_t*)(address) = (value))
#define gNV_REG_READ(address)               ((uint8_t)(*(vuint8_t*)(address)))
#define gNV_REG_WRITE32(address, value)     (*(vuint32_t*)(address) = (value))
#define gNV_REG_READ32(address)             ((uint32_t)(*(vuint32_t*)(address)))

#define gNV_WRITE8(address, value)          (*(vuint8_t*)(address) = (value))
#define gNV_READ8(address)                  ((uint8_t)(*(vuint8_t*)(address)))
#define gNV_SET8(address, value)            (*(vuint8_t*)(address) |= (value))
#define gNV_CLEAR8(address, value)          (*(vuint8_t*)(address) &= ~(value))
#define gNV_TEST8(address, value)           (*(vuint8_t*)(address) & (value))

#define gNV_WRITE16(address, value)         (*(vuint16_t*)(address) = (value))
#define gNV_READ16(address)                 ((uint16_t)(*(vuint16_t*)(address)))
#define gNV_SET16(address, value)           (*(vuint16_t*)(address) |= (value))
#define gNV_CLEAR16(address, value)         (*(vuint16_t*)(address) &= ~(value))
#define gNV_TEST16(address, value)          (*(vuint16_t*)(address) & (value))

#define gNV_WRITE32(address, value)         (*(vuint32_t*)(address) = (value))
#define gNV_READ32(address)                 ((uint32_t)(*(vuint32_t*)(address)))
#define gNV_SET32(address, value)           (*(vuint32_t*)(address) |= (value))
#define gNV_CLEAR32(address, value)         (*(vuint32_t*)(address) &= ~(value))
#define gNV_TEST32(address, value)          (*(vuint32_t*)(address) & (value))

/*--------------------------------------------------------------------------------*/

/* Longword size */
#define gNV_LONGWORD_SIZE_c              0x0004     /* 4 bytes */

/* Phrase size */
#define gNV_PHRASE_SIZE_c                0x0008     /* 8 bytes */

/* Double-phrase size */
#define gNV_DPHRASE_SIZE_c               0x0010     /* 16 bytes */

/*--------------------------------------------------------------------------------*/

#if (gNV_256K_256K_4K_2K_2K_c == gNV_Derivative_c)

    /* PFlash sector size */
    #define gNV_PSECTOR_SIZE_c            0x00000800      /* 2 KB size */
    /* DFlash sector size */
    #define gNV_DSECTOR_SIZE_c            0x00000800      /* 2 KB size */
    #define gNV_DEBLOCK_SIZE_c            0x00040000      /* 256 KB size */

    /* EEE Data Set Size Field Description */
    #define gNV_EEESIZE_0000_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_0001_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_0010_c            0x00001000
    #define gNV_EEESIZE_0011_c            0x00000800
    #define gNV_EEESIZE_0100_c            0x00000400
    #define gNV_EEESIZE_0101_c            0x00000200
    #define gNV_EEESIZE_0110_c            0x00000100
    #define gNV_EEESIZE_0111_c            0x00000080
    #define gNV_EEESIZE_1000_c            0x00000040
    #define gNV_EEESIZE_1001_c            0x00000020
    #define gNV_EEESIZE_1010_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1011_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1100_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1101_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1110_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1111_c            0x00000000      /* Default value */

    /* D/E-Flash Partition Code Field Description */
    #define gNV_DEPART_0000_c             0x00040000
    #define gNV_DEPART_0001_c             0x00040000      /* Reserved */
    #define gNV_DEPART_0010_c             0x0003C000
    #define gNV_DEPART_0011_c             0x00038000
    #define gNV_DEPART_0100_c             0x00030000
    #define gNV_DEPART_0101_c             0x00020000
    #define gNV_DEPART_0110_c             0x00000000
    #define gNV_DEPART_0111_c             0x00040000      /* Reserved */
    #define gNV_DEPART_1000_c             0x00000000
    #define gNV_DEPART_1001_c             0x00040000      /* Reserved */
    #define gNV_DEPART_1010_c             0x00004000
    #define gNV_DEPART_1011_c             0x00008000
    #define gNV_DEPART_1100_c             0x00010000
    #define gNV_DEPART_1101_c             0x00020000
    #define gNV_DEPART_1110_c             0x00040000
    #define gNV_DEPART_1111_c             0x00040000      /* Reserved */
    /*destination to read Dflash IFR area*/
    #define gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c   0x8000FC

    /* Address offset and size of PFlash IFR and DFlash IFR */
    #define gNV_PFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_PFLASH_IFR_SIZE_c                   0x00000100
    #define gNV_DFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_DFLASH_IFR_SIZE_c                   0x00000100

    /* Size for checking alignment of a section */
    #define gNV_ERSBLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of erase block function */
    #define gNV_PGMCHK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of program check function */
    #define gNV_PPGMSEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of program section function */
    #define gNV_DPGMSEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of program section function */
    #define gNV_RD1BLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of verify block function */
    #define gNV_PRD1SEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of verify section function */
    #define gNV_DRD1SEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of verify section function */
    #define gNV_SWAP_ALIGN_SIZE_c         gNV_LONGWORD_SIZE_c      /* check align of swap function */
    #define gNV_RDRSRC_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of read resource function */
    #define gNV_RDONCE_INDEX_MAX_c        0xF                      /* maximum index in read once command */

#elif (gNV_512K_0K_0K_2K_0K_c  == gNV_Derivative_c)

    #define gNV_PFLASH_BLOCK_BASE_c      0x00000000
    #define gNV_PFLASH_BLOCK_SIZE_c      0x00080000
    #define gNV_DFLASH_BLOCK_BASE_c      0x00000000
    #define gNV_DFLASH_BLOCK_SIZE_c      0x00000000
    #define gNV_EERAM_BLOCK_BASE_c       0x00000000
    #define gNV_EERAM_BLOCK_SIZE_c       0x00000000
    #define gNV_EEE_BLOCK_SIZE_c         0x00000000

    /* PFlash sector size */
    #define gNV_PSECTOR_SIZE_c       0x00000800      /* 2 KB size */
    /* DFlash sector size */
    #define gNV_DSECTOR_SIZE_c       0x00000000      /* 0 KB size */
    #define gNV_DEBLOCK_SIZE_c       0x00000000      /* 0 KB size */

    /*destination to read Dflash IFR area*/
    #define gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c   0x8000FC

    /* Address offset and size of PFlash IFR and DFlash IFR */
    #define gNV_PFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_PFLASH_IFR_SIZE_c                   0x00000100
    #define gNV_DFLASH_IFR_OFFSET_c                 0xFFFFFFFF /* reserved */
    #define gNV_DFLASH_IFR_SIZE_c                   0xFFFFFFFF /* reserved */

    /* Size for checking alignment of a section */
    #define gNV_ERSBLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of erase block function */
    #define gNV_PGMCHK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of program check function */
    #define gNV_PPGMSEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of program section function */
    #define gNV_DPGMSEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of program section function */
    #define gNV_RD1BLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of verify block function */
    #define gNV_PRD1SEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of verify section function */
    #define gNV_DRD1SEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of verify section function */
    #define gNV_SWAP_ALIGN_SIZE_c         gNV_LONGWORD_SIZE_c      /* check align of swap function */
    #define gNV_RDRSRC_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of read resource function */
    #define gNV_RDONCE_INDEX_MAX_c        0xF                      /* maximum index in read once command */

#elif ((gNV_32K_0K_0K_1K_0K_c == gNV_Derivative_c) \
        ||(gNV_64K_0K_0K_1K_0K_c == gNV_Derivative_c))

    /* PFlash sector size */
    #define gNV_PSECTOR_SIZE_c       0x00000400      /* 1 KB size */
    /* DFlash sector size */
    #define gNV_DSECTOR_SIZE_c       0x00000000      /* 0 KB size */
    #define gNV_DEBLOCK_SIZE_c       0x00000000      /* 0 KB size */
    /*destination to read Dflash IFR area*/
    #define gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c   0x8000FC

    /* Address offset and size of PFlash IFR and DFlash IFR */
    #define gNV_PFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_PFLASH_IFR_SIZE_c                   0x00000100
    #define gNV_DFLASH_IFR_OFFSET_c                 0xFFFFFFFF /* reserved */
    #define gNV_DFLASH_IFR_SIZE_c                   0xFFFFFFFF /* reserved */

    /* Size for checking alignment of a section */
    #define gNV_ERSBLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of erase block function */
    #define gNV_PGMCHK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of program check function */
    #define gNV_PPGMSEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of program section function */
    #define gNV_DPGMSEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of program section function */
    #define gNV_RD1BLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of verify block function */
    #define gNV_PRD1SEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of verify section function */
    #define gNV_DRD1SEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of verify section function */
    #define gNV_SWAP_ALIGN_SIZE_c         gNV_LONGWORD_SIZE_c      /* check align of swap function */
    #define gNV_RDRSRC_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of read resource function */
    #define gNV_RDONCE_INDEX_MAX_c        0xF                      /* maximum index in read once command */
#elif(gNV_128K_0K_0K_1K_0K_c == gNV_Derivative_c)

    #define gNV_PFLASH_BLOCK_BASE_c      0x00000000
    #define gNV_PFLASH_BLOCK_SIZE_c      0x00020000
    #define gNV_DFLASH_BLOCK_BASE_c      0x00000000
    #define gNV_DFLASH_BLOCK_SIZE_c      0x00000000
    #define gNV_EERAM_BLOCK_BASE_c       0x00000000
    #define gNV_EERAM_BLOCK_SIZE_c       0x00000000
    #define gNV_EEE_BLOCK_SIZE_c         0x00000000
    /* PFlash sector size */
    #define gNV_PSECTOR_SIZE_c       0x00000400      /* 1 KB size */
    /* DFlash sector size */
    #define gNV_DSECTOR_SIZE_c       0x00000000      /* 0 KB size */
    #define gNV_DEBLOCK_SIZE_c       0x00000000      /* 0 KB size */
    /*destination to read Dflash IFR area*/
    #define gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c   0x8000FC

    /* Address offset and size of PFlash IFR and DFlash IFR */
    #define gNV_PFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_PFLASH_IFR_SIZE_c                   0x00000100
    #define gNV_DFLASH_IFR_OFFSET_c                 0xFFFFFFFF /* reserved */
    #define gNV_DFLASH_IFR_SIZE_c                   0xFFFFFFFF /* reserved */

    /* Size for checking alignment of a section */
    #define gNV_ERSBLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of erase block function */
    #define gNV_PGMCHK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of program check function */
    #define gNV_PPGMSEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of program section function */
    #define gNV_DPGMSEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of program section function */
    #define gNV_RD1BLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of verify block function */
    #define gNV_PRD1SEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of verify section function */
    #define gNV_DRD1SEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of verify section function */
    #define gNV_SWAP_ALIGN_SIZE_c         gNV_LONGWORD_SIZE_c      /* check align of swap function */
    #define gNV_RDRSRC_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of read resource function */
    #define gNV_RDONCE_INDEX_MAX_c        0xF                      /* maximum index in read once command */

#elif ((gNV_512K_512K_16K_4K_4K_c == gNV_Derivative_c) || (gNV_1024K_0K_16K_4K_0K_c == gNV_Derivative_c))


#if (gNV_512K_512K_16K_4K_4K_c == gNV_Derivative_c)

    /* PFlash sector size */
    #define gNV_PSECTOR_SIZE_c       0x00001000      /* 4 KB size */
    /* DFlash sector size */
    #define gNV_DSECTOR_SIZE_c       0x00001000      /* 4 KB size */
    #define gNV_DEBLOCK_SIZE_c       0x00080000      /* 512 KB size */

    /* EEE Data Set Size Field Description */
    #define gNV_EEESIZE_0000_c            0x00004000      /* 16386 bytes */
    #define gNV_EEESIZE_0001_c            0x00002000      /* 8192 bytes */
    #define gNV_EEESIZE_0010_c            0x00001000      /* 4096 bytes */
    #define gNV_EEESIZE_0011_c            0x00000800      /* 2048 bytes */
    #define gNV_EEESIZE_0100_c            0x00000400      /* 1024 bytes */
    #define gNV_EEESIZE_0101_c            0x00000200      /* 512 bytes */
    #define gNV_EEESIZE_0110_c            0x00000100      /* 256 bytes */
    #define gNV_EEESIZE_0111_c            0x00000080      /* 128 bytes */
    #define gNV_EEESIZE_1000_c            0x00000040      /* 64 bytes */
    #define gNV_EEESIZE_1001_c            0x00000020      /* 32 bytes */
    #define gNV_EEESIZE_1010_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1011_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1100_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1101_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1110_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1111_c            0x00000000      /* 0 byte */

    /* D/E-Flash Partition Code Field Description */
    #define gNV_DEPART_0000_c             0x00080000      /* 512 KB */
    #define gNV_DEPART_0001_c             0x00080000      /* Reserved */
    #define gNV_DEPART_0010_c             0x00080000      /* Reserved */
    #define gNV_DEPART_0011_c             0x00080000      /* Reserved */
    #define gNV_DEPART_0100_c             0x00070000      /* 448 KB */
    #define gNV_DEPART_0101_c             0x00060000      /* 384 KB */
    #define gNV_DEPART_0110_c             0x00040000      /* 256 KB */
    #define gNV_DEPART_0111_c             0x00000000      /* 0 KB */
    #define gNV_DEPART_1000_c             0x00000000      /* 0 KB */
    #define gNV_DEPART_1001_c             0x00080000      /* Reserved */
    #define gNV_DEPART_1010_c             0x00080000      /* Reserved */
    #define gNV_DEPART_1011_c             0x00080000      /* Reserved */
    #define gNV_DEPART_1100_c             0x00010000      /* 64 KB */
    #define gNV_DEPART_1101_c             0x00020000      /* 128 KB */
    #define gNV_DEPART_1110_c             0x00040000      /* 256 KB */
    #define gNV_DEPART_1111_c             0x00080000      /* 512 KB */

    /* Address offset (compare to start addr of P/D flash) and size of PFlash IFR and DFlash IFR */
    #define gNV_PFLASH_IFR_OFFSET_c       0x00000000
    #define gNV_PFLASH_IFR_SIZE_c         0x00000400
    #define gNV_DFLASH_IFR_OFFSET_c       0x00000000
    #define gNV_DFLASH_IFR_SIZE_c         0x00000400

#else /* gNV_1024K_0K_16K_4K_0K_c == gNV_Derivative_c */
    /* PFlash sector size */
    #define gNV_PSECTOR_SIZE_c       0x00001000      /* 4 KB size */
    /* DFlash sector size */
    #define gNV_DSECTOR_SIZE_c       0x00000000      /* 0 KB size */
    #define gNV_DEBLOCK_SIZE_c       0x00000000      /* 0 KB size */

    /* Address offset (compare to start addr of P/D flash) and size of PFlash IFR and DFlash IFR */
    #define gNV_PFLASH_IFR_OFFSET_c       0x00000000
    #define gNV_PFLASH_IFR_SIZE_c         0x00000400
    #define gNV_DFLASH_IFR_OFFSET_c       0xFFFFFFFF      /* reserved */
    #define gNV_DFLASH_IFR_SIZE_c         0xFFFFFFFF      /* reserved */
#endif    /* of gNV_512K_512K_16K_4K_4K_c == gNV_Derivative_c */

    /*destination to read Dflash IFR area*/
    #define gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c       0x8003F8

    /* Size for checking alignment of a section */
    #define gNV_ERSBLK_ALIGN_SIZE_c       gNV_DPHRASE_SIZE_c       /* check align of erase block function */
    #define gNV_PGMCHK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of program check function */
    #define gNV_PPGMSEC_ALIGN_SIZE_c      gNV_DPHRASE_SIZE_c       /* check align of program section function */
    #define gNV_DPGMSEC_ALIGN_SIZE_c      gNV_DPHRASE_SIZE_c       /* check align of program section function */
    #define gNV_RD1BLK_ALIGN_SIZE_c       gNV_DPHRASE_SIZE_c       /* check align of verify block function */
    #define gNV_PRD1SEC_ALIGN_SIZE_c      gNV_DPHRASE_SIZE_c       /* check align of verify section function */
    #define gNV_DRD1SEC_ALIGN_SIZE_c      gNV_DPHRASE_SIZE_c       /* check align of verify section function */
    #define gNV_SWAP_ALIGN_SIZE_c         gNV_DPHRASE_SIZE_c       /* check align of swap function */
    #define gNV_RDRSRC_ALIGN_SIZE_c       gNV_PHRASE_SIZE_c        /* check align of read resource function */
    #define gNV_RDONCE_INDEX_MAX_c        0x7                      /* maximum index in read once command */

#elif ((gNV_32K_32K_2K_1K_1K_c == gNV_Derivative_c) \
        ||(gNV_64K_32K_2K_1K_1K_c == gNV_Derivative_c) \
        ||(gNV_128K_32K_2K_1K_1K_c == gNV_Derivative_c))

    /* User configuration of EEPROM */
    #define gNV_EEPROM_DATASET_SIZE_CODE_c     0x33   /* 2048 bytes */
    #define gNV_EEPROM_BACKUP_SIZE_CODE_c      0x03   /* 32 KBytes */
    #define gNV_FLEXRAM_FNC_CTRL_CODE          0x00   /* make FlexRAM available for EEPROM */

    #define gNV_PFLASH_BLOCK_BASE_c      0x00000000
    #define gNV_PFLASH_BLOCK_SIZE_c      0x00020000
    #define gNV_DFLASH_BLOCK_BASE_c      0x10000000
    #define gNV_DFLASH_BLOCK_SIZE_c      0x00008000
    #define gNV_EERAM_BLOCK_BASE_c       0x14000000
    #define gNV_EERAM_BLOCK_SIZE_c       0x00000800
    #define gNV_EEE_BLOCK_SIZE_c         0x00008000

    /* PFlash sector size */
    #define gNV_PSECTOR_SIZE_c           0x00000400     /* 1 KB size */
    /* DFlash sector size */
    #define gNV_DSECTOR_SIZE_c           0x00000400     /* 1 KB size */
    #define gNV_DEBLOCK_SIZE_c           0x00008000     /* 32 KB size */

    /* EEE Data Set Size Field Description */
    #define gNV_EEESIZE_0000_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_0001_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_0010_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_0011_c            0x00000800
    #define gNV_EEESIZE_0100_c            0x00000400
    #define gNV_EEESIZE_0101_c            0x00000200
    #define gNV_EEESIZE_0110_c            0x00000100
    #define gNV_EEESIZE_0111_c            0x00000080
    #define gNV_EEESIZE_1000_c            0x00000040
    #define gNV_EEESIZE_1001_c            0x00000020
    #define gNV_EEESIZE_1010_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1011_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1100_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1101_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1110_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1111_c            0x00000000      /* Default value */

    /* D/E-Flash Partition Code Field Description */
    #define gNV_DEPART_0000_c             0x00008000
    #define gNV_DEPART_0001_c             0x00006000
    #define gNV_DEPART_0010_c             0x00004000
    #define gNV_DEPART_0011_c             0x00000000
    #define gNV_DEPART_0100_c             0x00008000      /* Reserved */
    #define gNV_DEPART_0101_c             0x00008000      /* Reserved */
    #define gNV_DEPART_0110_c             0x00008000      /* Reserved */
    #define gNV_DEPART_0111_c             0x00008000      /* Reserved */
    #define gNV_DEPART_1000_c             0x00000000
    #define gNV_DEPART_1001_c             0x00002000
    #define gNV_DEPART_1010_c             0x00004000
    #define gNV_DEPART_1011_c             0x00008000
    #define gNV_DEPART_1100_c             0x00008000      /* Reserved */
    #define gNV_DEPART_1101_c             0x00008000      /* Reserved */
    #define gNV_DEPART_1110_c             0x00008000      /* Reserved */
    #define gNV_DEPART_1111_c             0x00008000      /* Default value */

    /*destination to read Dflash IFR area*/
    #define gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c   0x8000FC

    /* Address offset and size of PFlash IFR and DFlash IFR */
    #define gNV_PFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_PFLASH_IFR_SIZE_c                   0x00000100
    #define gNV_DFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_DFLASH_IFR_SIZE_c                   0x00000100

    /* Size for checking alignment of a section */
    #define gNV_ERSBLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c        /* check align of erase block function */
    #define gNV_PGMCHK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c        /* check align of program check function */
    #define gNV_PPGMSEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c        /* check align of program section function */
    #define gNV_DPGMSEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c        /* check align of program section function */
    #define gNV_RD1BLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c        /* check align of verify block function */
    #define gNV_PRD1SEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c        /* check align of verify section function */
    #define gNV_DRD1SEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c        /* check align of verify section function */
    #define gNV_SWAP_ALIGN_SIZE_c         gNV_LONGWORD_SIZE_c        /* check align of swap function */
    #define gNV_RDRSRC_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c        /* check align of read resource function */
    #define gNV_RDONCE_INDEX_MAX_c        0xF                        /* maximum index in read once command */

#elif ((gNV_64K_32K_2K_2K_1K_c == gNV_Derivative_c) \
        ||(gNV_128K_32K_2K_2K_1K_c == gNV_Derivative_c) \
        ||(gNV_256K_32K_2K_2K_1K_c == gNV_Derivative_c))

    /* PFlash sector size */
    #define gNV_PSECTOR_SIZE_c            0x00000800      /* 2 KB size */
    /* DFlash sector size */
    #define gNV_DSECTOR_SIZE_c            0x00000400      /* 1 KB size */
    #define gNV_DEBLOCK_SIZE_c            0x00008000      /* 32 KB size */

    /* EEE Data Set Size Field Description */
    #define gNV_EEESIZE_0000_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_0001_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_0010_c            0x00000000
    #define gNV_EEESIZE_0011_c            0x00000800
    #define gNV_EEESIZE_0100_c            0x00000400
    #define gNV_EEESIZE_0101_c            0x00000200
    #define gNV_EEESIZE_0110_c            0x00000100
    #define gNV_EEESIZE_0111_c            0x00000080
    #define gNV_EEESIZE_1000_c            0x00000040
    #define gNV_EEESIZE_1001_c            0x00000020
    #define gNV_EEESIZE_1010_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1011_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1100_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1101_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1110_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1111_c            0x00000000      /* Default value */

    /* D/E-Flash Partition Code Field Description */
    #define gNV_DEPART_0000_c             0x00008000
    #define gNV_DEPART_0001_c             0x00006000
    #define gNV_DEPART_0010_c             0x00004000
    #define gNV_DEPART_0011_c             0x00000000
    #define gNV_DEPART_0100_c             0x00008000      /* Reserved */
    #define gNV_DEPART_0101_c             0x00008000      /* Reserved */
    #define gNV_DEPART_0110_c             0x00008000      /* Reserved */
    #define gNV_DEPART_0111_c             0x00008000      /* Reserved */
    #define gNV_DEPART_1000_c             0x00000000
    #define gNV_DEPART_1001_c             0x00002000
    #define gNV_DEPART_1010_c             0x00004000
    #define gNV_DEPART_1011_c             0x00008000
    #define gNV_DEPART_1100_c             0x00008000      /* Reserved */
    #define gNV_DEPART_1101_c             0x00008000      /* Reserved */
    #define gNV_DEPART_1110_c             0x00008000      /* Reserved */
    #define gNV_DEPART_1111_c             0x00008000      /* Default value */

    /*destination to read Dflash IFR area*/
    #define gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c   0x8000FC

    /* Address offset and size of PFlash IFR and DFlash IFR */
    #define gNV_PFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_PFLASH_IFR_SIZE_c                   0x00000100
    #define gNV_DFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_DFLASH_IFR_SIZE_c                   0x00000100

    /* Size for checking alignment of a section */
    #define gNV_ERSBLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of erase block function */
    #define gNV_PGMCHK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of program check function */
    #define gNV_PPGMSEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of program section function */
    #define gNV_DPGMSEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of program section function */
    #define gNV_RD1BLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of verify block function */
    #define gNV_PRD1SEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of verify section function */
    #define gNV_DRD1SEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of verify section function */
    #define gNV_SWAP_ALIGN_SIZE_c         gNV_LONGWORD_SIZE_c      /* check align of swap function*/
    #define gNV_RDRSRC_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of read resource function */
    #define gNV_RDONCE_INDEX_MAX_c        0xF                     /* maximum index in read once command */

#elif (gNV_256K_64K_4K_2K_2K_c == gNV_Derivative_c)
    #define gNV_PFLASH_BLOCK_BASE_c      0x00000000
    #define gNV_PFLASH_BLOCK_SIZE_c      0x00040000
    #define gNV_DFLASH_BLOCK_BASE_c      0x10000000
    #define gNV_DFLASH_BLOCK_SIZE_c      0x00010000
    #define gNV_EERAM_BLOCK_BASE_c       0x14000000
    #define gNV_EERAM_BLOCK_SIZE_c       0x00001000
    #define gNV_EEE_BLOCK_SIZE_c         0x00001000

    /* PFlash sector size */
    #define gNV_PSECTOR_SIZE_c            0x00000800      /* 2 KB size */
    /* DFlash sector size */
    #define gNV_DSECTOR_SIZE_c            0x00000800      /* 2 KB size */
    #define gNV_DEBLOCK_SIZE_c            0x00010000      /* 64 KB size */
	
	/* User configuration of EEPROM */
    #define gNV_EEPROM_DATASET_SIZE_CODE_c     0x22   /* 2048 + 2048 bytes */
    #define gNV_EEPROM_BACKUP_SIZE_CODE_c      0x04   /* 64 KBytes */
    #define gNV_FLEXRAM_FNC_CTRL_CODE          0x00   /* make FlexRAM available for EEPROM */


    /* EEE Data Set Size Field Description */
    #define gNV_EEESIZE_0000_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_0001_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_0010_c            0x00001000
    #define gNV_EEESIZE_0011_c            0x00000800
    #define gNV_EEESIZE_0100_c            0x00000400
    #define gNV_EEESIZE_0101_c            0x00000200
    #define gNV_EEESIZE_0110_c            0x00000100
    #define gNV_EEESIZE_0111_c            0x00000080
    #define gNV_EEESIZE_1000_c            0x00000040
    #define gNV_EEESIZE_1001_c            0x00000020
    #define gNV_EEESIZE_1010_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1011_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1100_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1101_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1110_c            0x00000000      /* Reserved */
    #define gNV_EEESIZE_1111_c            0x00000000      /* Default value */

    /* D/E-Flash Partition Code Field Description */
    #define gNV_DEPART_0000_c             0x00010000
    #define gNV_DEPART_0001_c             0x00000000
    #define gNV_DEPART_0010_c             0x00000000
    #define gNV_DEPART_0011_c             0x00008000
    #define gNV_DEPART_0100_c             0x00000000      /* Reserved */
    #define gNV_DEPART_0101_c             0x00000000      /* Reserved */
    #define gNV_DEPART_0110_c             0x00000000      /* Reserved */
    #define gNV_DEPART_0111_c             0x00000000      /* Reserved */
    #define gNV_DEPART_1000_c             0x00000000
    #define gNV_DEPART_1001_c             0x00000000
    #define gNV_DEPART_1010_c             0x00000000
    #define gNV_DEPART_1011_c             0x00008000
    #define gNV_DEPART_1100_c             0x00010000      /* Reserved */
    #define gNV_DEPART_1101_c             0x00000000      /* Reserved */
    #define gNV_DEPART_1110_c             0x00000000      /* Reserved */
    #define gNV_DEPART_1111_c             0x00010000      /* Default value */

    /*destination to read Dflash IFR area*/
    #define gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c   0x8000FC

    /* Address offset and size of PFlash IFR and DFlash IFR */
    #define gNV_PFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_PFLASH_IFR_SIZE_c                   0x00000100
    #define gNV_DFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_DFLASH_IFR_SIZE_c                   0x00000100

    /* Size for checking alignment of a section */
    #define gNV_ERSBLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of erase block function */
    #define gNV_PGMCHK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of program check function */
    #define gNV_PPGMSEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of program section function */
    #define gNV_DPGMSEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of program section function */
    #define gNV_RD1BLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of verify block function */
    #define gNV_PRD1SEC_ALIGN_SIZE_c      gNV_PHRASE_SIZE_c        /* check align of verify section function */
    #define gNV_DRD1SEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of verify section function */
    #define gNV_SWAP_ALIGN_SIZE_c         gNV_LONGWORD_SIZE_c      /* check align of swap function*/
    #define gNV_RDRSRC_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of read resource function */
    #define gNV_RDONCE_INDEX_MAX_c        0xF 

#elif (gNV_256K_0K_0K_1K_0K_c == gNV_Derivative_c)

    #define gNV_PFLASH_BLOCK_BASE_c      0x00000000
    #define gNV_PFLASH_BLOCK_SIZE_c      0x00040000
    #define gNV_DFLASH_BLOCK_BASE_c      0x00000000
    #define gNV_DFLASH_BLOCK_SIZE_c      0x00000000
    #define gNV_EERAM_BLOCK_BASE_c       0x00000000
    #define gNV_EERAM_BLOCK_SIZE_c       0x00000000
    #define gNV_EEE_BLOCK_SIZE_c         0x00000000
    /* PFlash sector size */
    #define gNV_PSECTOR_SIZE_c       0x00000400      /* 1 KB size */
    /* DFlash sector size */
    #define gNV_DSECTOR_SIZE_c       0x00000000      /* 0 KB size */
    #define gNV_DEBLOCK_SIZE_c       0x00000000      /* 0 KB size */
    /*destination to read Dflash IFR area*/
    #define gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c   0x8000FC

    /* Address offset and size of PFlash IFR and DFlash IFR */
    #define gNV_PFLASH_IFR_OFFSET_c                 0x00000000
    #define gNV_PFLASH_IFR_SIZE_c                   0x00000100
    #define gNV_DFLASH_IFR_OFFSET_c                 0xFFFFFFFF /* reserved */
    #define gNV_DFLASH_IFR_SIZE_c                   0xFFFFFFFF /* reserved */

    /* Size for checking alignment of a section */
    #define gNV_ERSBLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of erase block function */
    #define gNV_PGMCHK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of program check function */
    #define gNV_PPGMSEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of program section function */
    #define gNV_DPGMSEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of program section function */
    #define gNV_RD1BLK_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of verify block function */
    #define gNV_PRD1SEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of verify section function */
    #define gNV_DRD1SEC_ALIGN_SIZE_c      gNV_LONGWORD_SIZE_c      /* check align of verify section function */
    #define gNV_SWAP_ALIGN_SIZE_c         gNV_LONGWORD_SIZE_c      /* check align of swap function */
    #define gNV_RDRSRC_ALIGN_SIZE_c       gNV_LONGWORD_SIZE_c      /* check align of read resource function */
    #define gNV_RDONCE_INDEX_MAX_c        0xF                      /* maximum index in read once command */

#endif


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/*
 * Name: NvConfig_t
 * Description: Flash NVM Configuration Structure
 */
typedef struct NvConfig_tag
{
    uint32_t      ftfxRegBase;        /* FTFx control register base */
    uint32_t      PFlashBlockBase;    /* base address of PFlash block */
    uint32_t      PFlashBlockSize;    /* size of PFlash block */
    uint32_t      DFlashBlockBase;    /* base address of DFlash block */
    uint32_t      DFlashBlockSize;    /* size of DFlash block */
    uint32_t      EERAMBlockBase;     /* base address of EERAM block */
    uint32_t      EERAMBlockSize;     /* size of EERAM block */
    uint32_t      EEEBlockSize;       /* size of EEE block */
} NvConfig_t, *pNvConfig_t;

/*****************************************************************************
******************************************************************************
* Public memory declarations
*****************************************************************************
*****************************************************************************/
/*
 * Name: EffectiveWrBytesCnt
 * Description: effective number of bytes written
 */   
extern uint32_t EffectiveWrBytesCnt;

/*****************************************************************************
 ******************************************************************************
 * Public prototypes
 ******************************************************************************
 *****************************************************************************/

/************************************************************************
*
*  Function Name    : NV_FlashInit
*  Description      : Initialize the Flash memory controller (FTFL)
*  Arguments        : [IN] pConfig - pointer to NV configuration
*  Return Value     : -
*
*************************************************************************/
extern void NV_FlashInit
(
        pNvConfig_t pConfig
);

/************************************************************************
*
*  Function Name    : NV_DEFlashPartition
*  Description      : This function prepares the D/E-Flash block for use
*                     as D-Flash, E-Flash or a combination of both and
*                     initializes the EERAM.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                   : [IN] EEEDataSizeCode - EEPROM data set size code
*                   : [IN] DEPartitionCode - FlexNVM partition code
*  Return Value     : see NV return codes
*
*************************************************************************/
extern uint32_t NV_DEFlashPartition
(
        pNvConfig_t pConfig,
        uint8_t EEEDataSizeCode,
        uint8_t DEPartitionCode
);

/*************************************************************************
*
*  Function Name    : NV_DFlashGetProtection
*  Description      : This function retrieves current D-Flash protection status.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                   : [OUT] protectStatus - pointer to a memory location where
*                                           the protection status will be stored.
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_DFlashGetProtection
(
        pNvConfig_t pConfig,
        uint8_t* protectStatus
);

/************************************************************************
*
*  Function Name    : NV_DFlashSetProtection
*  Description      : This function sets the D-Flash protection to the
*                     intended protection status
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] protectStatus - protection status to be set
*  Return Value     : uint32_t
*
*************************************************************************/

extern uint32_t NV_DFlashSetProtection
(
        pNvConfig_t pConfig,
        uint8_t protectStatus
);

/************************************************************************
*
*  Function Name    : NV_EEEWrite
*  Description      : This function is used to write data to EERAM
*                     when it is used as EEPROM emulator
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] source - source address
*                     [IN] destination - EERAM destination address
*                     [IN] size - how many bytes to be written
*
*  Return Value     : uint32_t
*
************************************************************************/
extern uint32_t NV_EEEWrite
(
        pNvConfig_t pConfig,
        uint32_t source,
        uint32_t destination,
        uint32_t size
);

/************************************************************************
*
*  Function Name    : NV_EERAMGetProtection.c
*  Description      : This function retrieves current EERAM protection status.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] protectStatus - current protection status
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_EERAMGetProtection
(
        pNvConfig_t pConfig,
        uint8_t* protectStatus
);

/************************************************************************
*
*  Function Name    : NV_EERAMSetProtection
*  Description      : This function sets EERAM protection status.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] protectStatus - protection status to be set
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_EERAMSetProtection
(
        pNvConfig_t pConfig,
        uint8_t protectStatus
);

/***********************************************************************
*
*  Function Name    : NV_FlashCheckSum
*  Description      : This function is used to calculate checksum value
*                     for the specified flash range.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] destination - destination address
*                     [IN] size - length of data
*                     [OUT] pSum - pointer to the location where the
*                           calculated checksum will be place
*  Return Value     : uint32_t
*
************************************************************************/
extern uint32_t NV_FlashCheckSum
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint32_t size,
        uint32_t* pSum
);

/************************************************************************
*
*  Function Name    : NV_FlashEraseAllBlock
*  Description      : The Erase All Blocks operation will erase all Flash
*                     memory, initialize the EERAM, verify all memory
*                     contents, then release MCU security.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashEraseAllBlock
(
        pNvConfig_t pConfig
);

/************************************************************************
*
*  Function Name    : NV_FlashEraseBlock
*  Description      : The Erase Flash Block operation will erase all addresses in a single P-Flash or D-Flash block.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] destination - destination address
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashEraseBlock
(
        pNvConfig_t pConfig,
        uint32_t destination
);

/************************************************************************
*
*  Function Name    : NV_FlashEraseSuspend
*  Description      : This function is used to suspend a current operation
*                     of flash erase sector command.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashEraseSuspend
(
        pNvConfig_t pConfig
);

/************************************************************************
*
*  Function Name    : NV_FlashEraseResume
*  Description      : This function is used to resume a previous suspended
*                     operation of flash erase sector command.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashEraseResume
(
        pNvConfig_t pConfig
);

/****************************************************************************
*
*  Function Name    : NV_FlashEraseSector
*  Description      : Perform erase operation on Flash
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] destination - destination address
*                     [IN] size - data size
*  Return Value     : uint32_t
*
*****************************************************************************/
extern uint32_t NV_FlashEraseSector
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint32_t size
);

/************************************************************************
*
*  Function Name    : NV_FlashGetInterruptEnable
*  Description      : This function will read the FCNFG register and
*                     return the interrupt enable states for Flash
*                     interrupt types.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [OUT] interruptState - pointer to a memory location
*                           where the interrupt state shall be placed
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashGetInterruptEnable
(
        pNvConfig_t pConfig,
        uint8_t* interruptState
);

/************************************************************************
*
*  Function Name    : NV_FlashGetSecurityState
*  Description      : This function retrieves the current Flash security
*                     status, including the security enabling state and
*                     the backdoor key enabling state.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [OUT] securityState - pointer to a memory location
*                           where the security state will be placed
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashGetSecurityState
(
        pNvConfig_t pConfig,
        uint8_t* securityState
);

/************************************************************************
*
*  Function Name    : NV_FlashProgramCheck
*  Description      : The Program Check command tests a previously
*                     programmed P-Flash or D-Flash longword to see
*                     if it reads correctly at the specified margin level.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] destination - destination address
*                     [IN] size - block size to be checked
*                     [IN] pExpectedData - pointer to a memory location where
*                          the expected data is stored
*                     [IN] pFailAddr - pointer to a memory location where the
*                          failed address will be stored
*                     [IN] pFailData - pointer to a memory location where
*                          failed data will be stored
*                     [IN] marginLevel - read margin level
*
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashProgramCheck
(
        pNvConfig_t pConfig,
        uint32_t  destination,
        uint32_t  size,
        uint8_t*  pExpectedData,
        uint8_t   marginLevel
);

/************************************************************************
*
*  Function Name    : NV_FlashProgramLongword
*  Description      : Program data into Flash
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] destination - destination address
*                     [IN] size - data set size
*                     [IN] source - source address
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashProgramLongword
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint32_t size,
        uint32_t source
);

/************************************************************************
*
*  Function Name    : NV_FlashProgramLongword
*  Description      : Program data into Flash
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] destination - destination address
*                     [IN] size - data set size
*                     [IN] source - source address
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashProgramUnalignedLongword
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint32_t size,
        uint32_t source
);
/************************************************************************
*
*  Function Name    : NV_FlashRead
*  Description      : Read data stored in flash memory
*  Arguments        : [IN] address - flash memory address
*             [OUT] pData - pointer to a memory location
*                  where the data will be stored
*                     [IN] len - data lenght to be read
*  Return Value     : -
*
*************************************************************************/
extern void NV_FlashRead
(
    uint32_t address,
    uint8_t* pData,
    uint32_t len
);

/************************************************************************
*
*  Function Name    : NV_FlashProgramOnce
*  Description      : Program data into a dedicated 64 bytes region in
*                     the P-Flash IFR which stores critical information
*                     for the user
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] pDataArray - pointer to a memory location
*                          where the data is stored
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashProgramOnce
(
        pNvConfig_t pConfig,
        uint8_t* pDataArray
);

/************************************************************************
*
*  Function Name    : NV_FlashProgramPhrase
*  Description      : Program data into Flash
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] destination - destination address
*                     [IN] size - size to be written
*                     [IN] source - source address
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashProgramPhrase
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint32_t size,
        uint32_t source
);

/************************************************************************
*
*  Function Name    : NV_FlashReadOnce
*  Description      : This function is used to read access to a reserved
*                     64 byte field located in the P-Flash IFR.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] pDataArray - pointer to location where read data
*                          will be placed
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashReadOnce
(
        pNvConfig_t pConfig,
        uint8_t* pDataArray
);

/************************************************************************
*
*  Function Name    : NV_FlashReadResource
*  Description      : This function is provided for the user to read data
*                     from P-Flash IFR and D-Flash IFR space.
*  Arguments        :[IN] pConfig - pointer to NV configuration
*                    [IN] destination - destination address
*                    [IN] pDataArray - pointer to location where read data
*                          will be placed
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashReadResource
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint8_t* pDataArray
);

/************************************************************************
*
*  Function Name    : NV_FlashSecurityBypass
*  Description      : If the MCU is secured state, this function will
*                     unsecure the MCU by comparing the provided backdoor
*                     key with ones in the Flash Configuration Field.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] keyBuffer - pointer to a location where the key
*                          is stored
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashSecurityBypass
(
        pNvConfig_t pConfig,
        uint8_t* keyBuffer
);

/************************************************************************
*
*  Function Name    : NV_FlashSetInterruptEnable
*  Description      : This function sets the Flash interrupt enable
*                     bits in the Flash module configuration register.
*                     Other bits in the FCNFG register won’t be affected.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] interruptState - interrupt state to be set
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashSetInterruptEnable
(
        pNvConfig_t pConfig,
        uint8_t interruptState
);

/************************************************************************
*
*  Function Name    : NV_FlashVerifyAllBlock
*  Description      : This function will check to see if the P-Flash
*                     and D-Flash blocks as well as EERAM, E-Flash records
*                     and D-Flash IFR have been erased to the specified read
*                     margin level, if applicable, and will release security
*                     if the readout passes.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] marginLevel - margin level
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashVerifyAllBlock
(
        pNvConfig_t pConfig,
        uint8_t marginLevel
);

/************************************************************************
*
*  Function Name    : NV_FlashVerifyBlock
*  Description      : This function will check to see if an entire
*                     P-Flash or D-Flash block has been erased to the
*                     specified margin level.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] destination - destination address
*                     [IN] marginLevel - margin level
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_FlashVerifyBlock
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint8_t marginLevel
);

/************************************************************************
 *
 *  Function Name    : NV_FlashVerifySection
 *  Description      : This function will check to see if a section of
 *                     P-Flash or D-Flash memory is erased to the specified
 *                     read margin level.
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] destination - destination address
 *                     [IN] Length - number of bytes to be verified
 *                     [IN] marginLevel - margin level
 *  Return Value     : uint32_t
 *
 *************************************************************************/
extern uint32_t NV_FlashVerifySection
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint16_t Length,
        uint8_t marginLevel
);

/************************************************************************
*
*  Function Name    : NV_PFlashSetProtection
*  Description      : This function sets the P-Flash protection to the
*                     intended protection status
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] protectStatus - protection status to be set
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_PFlashSetProtection
(
        pNvConfig_t pConfig,
        uint32_t protectStatus
);

/************************************************************************
*
*  Function Name    : NV_PFlashGetProtection
*  Description      : This function retrieves current P-Flash protection status.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] protectStatus - pointer to a memory location where
*                          the protection status will be stored
*
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_PFlashGetProtection
(
        pNvConfig_t pConfig,
        uint32_t* protectStatus
);

/************************************************************************
*
*  Function Name    : NV_SetEEEEnable
*  Description      : This function is used to change the function of
*                     the EERAM. When not partitioned for EEE, the EERAM
*                     is typically used as traditional RAM. When partitioned
*                     for EEE, the EERAM is typically used to store EEE data.
*  Arguments        : [IN] pConfig - pointer to NV configuration
*                     [IN] EEEEnable - EERAM Enable
*  Return Value     : uint32_t
*
*************************************************************************/
extern uint32_t NV_SetEEEEnable
(
        pNvConfig_t pConfig,
        uint8_t EEEEnable
);

#ifdef __cplusplus
}
#endif

#endif /* _NV_FLASHHAL_H_ */
