/*!
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file NV_Flash.c
* Implementation of the non-volatile storage module flash
*                  controller HAL for Cortex-M4 processor
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
#include "NV_FlashHAL.h"
#include "fsl_os_abstraction.h"


/*****************************************************************************
 *****************************************************************************
 * Private memory definitions
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 ******************************************************************************
 * Public memory definitions
 ******************************************************************************
 *****************************************************************************/

/*
 * Name: effective number of bytes written
 * Description: EffectiveWrBytesCnt;
 */
uint32_t EffectiveWrBytesCnt;

/*****************************************************************************
 *****************************************************************************
 * Private prototypes
 *****************************************************************************
 *****************************************************************************/

/************************************************************************
 *
 *  Function Name    : NvFlashCommandSequence
 *  Description      : Perform command write sequence for flash operation
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] index - command index
 *                     [IN] pCommandArray - command parameters
 *  Return Value     : uint32_t
 *
 *************************************************************************/
static uint32_t NvFlashCommandSequence
(
        pNvConfig_t pConfig,
        uint8_t index,
        uint8_t* pCommandArray
);

/*****************************************************************************
 *****************************************************************************
 * Private functions
 *****************************************************************************
 *****************************************************************************/

/************************************************************************
 *
 *  Function Name    : NvFlashCommandSequence
 *  Description      : Perform command write sequence for flash operation
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] index - command index
 *                     [IN] pCommandArray - command parameters
 *  Return Value     : uint32_t
 *
 *************************************************************************/
static uint32_t NvFlashCommandSequence
(
        pNvConfig_t pConfig,
        uint8_t index,
        uint8_t* pCommandArray
)
{
    uint8_t  counter;          /* for loop counter variable */
    uint8_t  registerValue;    /* store data read from flash register */
    uint32_t returnCode;       /* return code variable */    

    /* set the default return as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* check CCIF bit of the flash status register */
    while(FALSE == (gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c)))
    {
        /* wait till CCIF bit is set */
    };

    /* clear RDCOLERR & ACCERR & FPVIOL flag in flash status register */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, \
            (gNV_FSTAT_RDCOLERR_c | gNV_FSTAT_ACCERR_c | gNV_FSTAT_FPVIOL_c));

    /* load FCCOB registers */
    for(counter=0; counter<=index; counter++)
    {
        switch (counter)
        {
        case 0: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB0_OFFSET_c, pCommandArray[counter]);
        break;
        case 1: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB1_OFFSET_c, pCommandArray[counter]);
        break;
        case 2: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB2_OFFSET_c, pCommandArray[counter]);
        break;
        case 3: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB3_OFFSET_c, pCommandArray[counter]);
        break;
        case 4: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB4_OFFSET_c, pCommandArray[counter]);
        break;
        case 5: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB5_OFFSET_c, pCommandArray[counter]);
        break;
        case 6: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB6_OFFSET_c, pCommandArray[counter]);
        break;
        case 7: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB7_OFFSET_c, pCommandArray[counter]);
        break;
        case 8: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB8_OFFSET_c, pCommandArray[counter]);
        break;
        case 9: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB9_OFFSET_c, pCommandArray[counter]);
        break;
        case 10: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOBA_OFFSET_c, pCommandArray[counter]);
        break;
        case 11: gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOBB_OFFSET_c, pCommandArray[counter]);
        break;
        default: break;
        }
    }
#if gNvDisableIntCmdSeq_c    
     /* disable interrupts */
     OSA_EnterCritical(kCriticalDisableInt);;   
#endif

    /* clear CCIF bit */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c);

    /* check CCIF bit */
    while(FALSE == (gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c)))
    {
        /* wait till CCIF bit is set */
    }
    
#if gNvDisableIntCmdSeq_c
      /* enable interrupts */
      OSA_ExitCritical(kCriticalDisableInt);  
#endif

    /* Check error bits */

    /* Get flash status register value */
    registerValue = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c);

    /* checking access error */
    if(0 != (registerValue & gNV_FSTAT_ACCERR_c))
    {
        /* return an error code gNV_ERR_ACCERR_c */
        returnCode = gNV_ERR_ACCERR_c;
    }
    /* checking protection error */
    else if(0 != (registerValue & gNV_FSTAT_FPVIOL_c))
    {
        /* return an error code gNV_ERR_PVIOL_c */
        returnCode = gNV_ERR_PVIOL_c;
    }
    /* checking MGSTAT0 non-correctable error */
    else if(0 != (registerValue & gNV_FSTAT_MGSTAT0_c))
    {
        /* return an error code gNV_ERR_MGSTAT0_c */
        returnCode = gNV_ERR_MGSTAT0_c;
    }

    return(returnCode);
}


/*****************************************************************************
 *****************************************************************************
 * Public functions
 *****************************************************************************
 *****************************************************************************/

/************************************************************************
 *
 *  Function Name    : NV_FlashInit
 *  Description      : Initialize the Flash memory controller (FTFL)
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *  Return Value     : -
 *
 *************************************************************************/
void NV_FlashInit
(
        pNvConfig_t pConfig
)
{
#if (gNV_DEBLOCK_SIZE_c != 0)
    uint8_t  EEEDataSetSize;     /* store EEE Data Set Size */
    uint8_t  DEPartitionCode;    /* store D/E-Flash Partition Code */
#endif

    /* set the flash clock */
    //gFTFL_SIM_CLKDIV_c |= gSIM_CLKDIV_OUTDIV(gCLKDIV_VALUE_c);
    /* enable clock gating for the FTFL module */
    gFTFL_SIM_SCGC_c |= gSIM_SCGC_FTFL_MASK_c;
#if defined(MCU_MKL26Z4) || defined(MCU_MKL46Z4)
    MCM_PLACR |= MCM_PLACR_ESFC_MASK;
#endif
#if (gNV_DEBLOCK_SIZE_c != 0)
    /* check CCIF bit of the flash status register */
    while(FALSE == (gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c)))
    {
        /* wait till CCIF bit is set */
    }

    /* clear RDCOLERR & ACCERR & FPVIOL flag in flash status register */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, \
            (gNV_FSTAT_RDCOLERR_c | gNV_FSTAT_ACCERR_c | gNV_FSTAT_FPVIOL_c));

    /* Write Command Code to FCCOB0 */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB0_OFFSET_c, gNV_READ_RESOURCE_c);
    /* Write address to FCCOB1/2/3 */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB1_OFFSET_c, ((uint8_t)(gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c >> 16)));
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB2_OFFSET_c, ((uint8_t)((gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c >> 8) & 0xFF)));
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB3_OFFSET_c, ((uint8_t)(gNV_DFLASH_IFR_READRESOURCE_ADDRESS_c & 0xFF)));

    /* Write Resource Select Code of 0 to FCCOB8 to select IFR. Without this, */
    /* an access error may occur if the register contains data from a previous command. */
#if (gNV_512K_512K_16K_4K_4K_c == gNV_Derivative_c)
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB4_OFFSET_c, 0);
#else
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB8_OFFSET_c, 0);
#endif
    /* clear CCIF bit */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c);

    /* check CCIF bit */
    while((gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c)) == FALSE)
    {
        /* wait till CCIF bit is set */
    }

    /* Read returned value of FCCOB6/7 or FCCOBA,B to the variables */
#if (gNV_512K_512K_16K_4K_4K_c == gNV_Derivative_c)
    EEEDataSetSize = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOBA_OFFSET_c);
    DEPartitionCode = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOBB_OFFSET_c);
#else
    EEEDataSetSize = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB6_OFFSET_c);
    DEPartitionCode = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB7_OFFSET_c);
#endif
    /* Calculate D-Flash size and EEE size */
    switch (DEPartitionCode & 0x0F)
    {
    case 0: pConfig->DFlashBlockSize = gNV_DEPART_0000_c; break;
    case 1: pConfig->DFlashBlockSize = gNV_DEPART_0001_c; break;
    case 2: pConfig->DFlashBlockSize = gNV_DEPART_0010_c; break;
    case 3: pConfig->DFlashBlockSize = gNV_DEPART_0011_c; break;
    case 4: pConfig->DFlashBlockSize = gNV_DEPART_0100_c; break;
    case 5: pConfig->DFlashBlockSize = gNV_DEPART_0101_c; break;
    case 6: pConfig->DFlashBlockSize = gNV_DEPART_0110_c; break;
    case 7: pConfig->DFlashBlockSize = gNV_DEPART_0111_c; break;
    case 8: pConfig->DFlashBlockSize = gNV_DEPART_1000_c; break;
    case 9: pConfig->DFlashBlockSize = gNV_DEPART_1001_c; break;
    case 10: pConfig->DFlashBlockSize = gNV_DEPART_1010_c; break;
    case 11: pConfig->DFlashBlockSize = gNV_DEPART_1011_c; break;
    case 12: pConfig->DFlashBlockSize = gNV_DEPART_1100_c; break;
    case 13: pConfig->DFlashBlockSize = gNV_DEPART_1101_c; break;
    case 14: pConfig->DFlashBlockSize = gNV_DEPART_1110_c; break;
    case 15: pConfig->DFlashBlockSize = gNV_DEPART_1111_c; break;
    default: break;
    }

    switch (EEEDataSetSize & 0x0F)
    {
    case 0: pConfig->EEEBlockSize = gNV_EEESIZE_0000_c; break;
    case 1: pConfig->EEEBlockSize = gNV_EEESIZE_0001_c; break;
    case 2: pConfig->EEEBlockSize = gNV_EEESIZE_0010_c; break;
    case 3: pConfig->EEEBlockSize = gNV_EEESIZE_0011_c; break;
    case 4: pConfig->EEEBlockSize = gNV_EEESIZE_0100_c; break;
    case 5: pConfig->EEEBlockSize = gNV_EEESIZE_0101_c; break;
    case 6: pConfig->EEEBlockSize = gNV_EEESIZE_0110_c; break;
    case 7: pConfig->EEEBlockSize = gNV_EEESIZE_0111_c; break;
    case 8: pConfig->EEEBlockSize = gNV_EEESIZE_1000_c; break;
    case 9: pConfig->EEEBlockSize = gNV_EEESIZE_1001_c; break;
    case 10: pConfig->EEEBlockSize = gNV_EEESIZE_1010_c; break;
    case 11: pConfig->EEEBlockSize = gNV_EEESIZE_1011_c; break;
    case 12: pConfig->EEEBlockSize = gNV_EEESIZE_1100_c; break;
    case 13: pConfig->EEEBlockSize = gNV_EEESIZE_1101_c; break;
    case 14: pConfig->EEEBlockSize = gNV_EEESIZE_1110_c; break;
    case 15: pConfig->EEEBlockSize = gNV_EEESIZE_1111_c; break;
    default: break;
    }
#else /* gNV_DEBLOCK_SIZE_c == 0 */
/* If size of D/E-Flash = 0 */
    pConfig->DFlashBlockSize = 0;
    pConfig->EEEBlockSize = 0;
#endif /* of gNV_DEBLOCK_SIZE_c */
}


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
#if (defined(__IAR_SYSTEMS_ICC__))
__ramfunc uint32_t NV_DEFlashPartition
(
        pNvConfig_t pConfig,
        uint8_t EEEDataSizeCode,
        uint8_t DEPartitionCode
)
#else
uint32_t NV_DEFlashPartition
(
        pNvConfig_t pConfig,
        uint8_t EEEDataSizeCode,
        uint8_t DEPartitionCode
)
#endif
{
    uint32_t returnCode;      /* return code variable */
    uint8_t registerValue;

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* check CCIF bit of the flash status register */
    while(FALSE == (gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c)))
    {
        /* wait till CCIF bit is set */
    };

    /* clear RDCOLERR & ACCERR & FPVIOL flag in flash status register */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, \
            (gNV_FSTAT_RDCOLERR_c | gNV_FSTAT_ACCERR_c | gNV_FSTAT_FPVIOL_c));

    /* load the FCCOB registers */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB0_OFFSET_c, gNV_PROGRAM_PARTITION_c);
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB1_OFFSET_c, 0);
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB2_OFFSET_c, 0);
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB3_OFFSET_c, 0);
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB4_OFFSET_c, EEEDataSizeCode);
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCCOB5_OFFSET_c, DEPartitionCode);

    /* clear CCIF bit */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c);

    /* check CCIF bit */
    while(FALSE == (gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c)))
    {
        /* wait till CCIF bit is set */
    };

    /* Check error bits */
    /* Get flash status register value */
    registerValue = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c);

    /* checking access error */
    if(0 != (registerValue & gNV_FSTAT_ACCERR_c))
    {
        /* return an error code gNV_ERR_ACCERR_c */
        returnCode = gNV_ERR_ACCERR_c;
    }
    /* checking protection error */
    else if(0 != (registerValue & gNV_FSTAT_FPVIOL_c))
    {
        /* return an error code gNV_ERR_PVIOL_c */
        returnCode = gNV_ERR_PVIOL_c;
    }
    /* checking MGSTAT0 non-correctable error */
    else if(0 != (registerValue & gNV_FSTAT_MGSTAT0_c))
    {
        /* return an error code gNV_ERR_MGSTAT0_c */
        returnCode = gNV_ERR_MGSTAT0_c;
    }

    return(returnCode);
}


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
uint32_t NV_DFlashGetProtection
(
        pNvConfig_t pConfig,
        uint8_t* protectStatus
)
{
    /* Check if size of DFlash = 0 */
    if(pConfig->DFlashBlockSize == 0)
    {
        return(gNV_ERR_EFLASHONLY_c);
    }

    *protectStatus = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FDPROT_OFFSET_c);

    return(gNV_OK_c);
}


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
uint32_t NV_DFlashSetProtection
(
        pNvConfig_t pConfig,
        uint8_t protectStatus
)
{
    uint8_t  registerValue;    /* store data read from flash register */
    uint8_t  temp;

    /* check CCIF bit of the flash status register, it's impossible to write to DPROT if command is in progress */
    if(FALSE == (gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c)))
    {
        return(gNV_ERR_CHANGEPROT_c);
    }
    /* Check if size of DFlash = 0 */
    if(pConfig->DFlashBlockSize == 0)
    {
        return(gNV_ERR_EFLASHONLY_c);
    }

    /* Read the value of FDPROT register */
    registerValue = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FDPROT_OFFSET_c);

    /* Check if normal mode */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FDPROT_OFFSET_c, 0xFF);

    if(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FDPROT_OFFSET_c) != 0xFF)
    {
        /* if normal mode */
        temp = (uint8_t)(protectStatus ^ registerValue);
        if(!(temp & protectStatus))
        {
            gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FDPROT_OFFSET_c, protectStatus);
        }
        else
        {
            return(gNV_ERR_CHANGEPROT_c);
        }
    }
    /* if unprotected or special mode */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FDPROT_OFFSET_c, protectStatus);

    return(gNV_OK_c);
}


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
uint32_t NV_EEEWrite
(
        pNvConfig_t pConfig,
        uint32_t source,
        uint32_t destination,
        uint32_t size
)
{
    uint32_t returnCode;           /* Return code variable */
    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;
    /* Check if EEE is enabled */
    if(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCNFG_OFFSET_c) & gNV_FCNFG_EEERDY_c)
    {
        if(destination < pConfig->EERAMBlockBase || \
                ((destination + size) > (pConfig->EERAMBlockBase + pConfig->EEEBlockSize)))
        {
            returnCode = gNV_ERR_RANGE_c;
        }
        else
        {
            while(size > 0)
            {
                while(FALSE == gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FCNFG_OFFSET_c, gNV_FCNFG_EEERDY_c))
                {
                    /* wait till EEERDY bit is set */
                }
                gNV_WRITE8(destination,gNV_READ8(source));

                /* Check for protection violation error */
                if(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c) & gNV_FSTAT_FPVIOL_c)
                {
                    returnCode = gNV_ERR_PVIOL_c;
                    break;
                }

                /* update destination address for next iteration */
                destination += 1;
                /* update size for next iteration */
                size -= 1;
                /* increment the source address by 1 */
                source += 1;
            }
        }
    }
    else
    {
        returnCode = gNV_ERR_NOEEE_c;
    }

    return(returnCode);
}


/************************************************************************
 *
 *  Function Name    : NV_EERAMGetProtection.c
 *  Description      : This function retrieves current EERAM protection status.
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] protectStatus - current protection status
 *  Return Value     : uint32_t
 *
 *************************************************************************/
uint32_t NV_EERAMGetProtection
(
        pNvConfig_t pConfig,
        uint8_t* protectStatus
)
{
    /* Check if EERAM is set for EEE */
    if(gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FCNFG_OFFSET_c, gNV_FCNFG_EEERDY_c))
    {
        /* EERAM is set for EEE */
        *protectStatus = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FEPROT_OFFSET_c);
    }
    else
    {
        return(gNV_ERR_NOEEE_c);
    }

    return(gNV_OK_c);
}


/************************************************************************
 *
 *  Function Name    : NV_EERAMSetProtection
 *  Description      : This function sets EERAM protection status.
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] protectStatus - protection status to be set
 *  Return Value     : uint32_t
 *
 *************************************************************************/
uint32_t NV_EERAMSetProtection
(
        pNvConfig_t pConfig,
        uint8_t protectStatus
)
{
    uint8_t  registerValue;    /* store data read from flash register */
    uint32_t returnCode;       /* return code variable */
    uint8_t  temp;

    /* set the default return as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* check CCIF bit of the flash status register, it's impossible to write to FEPROT if command is in progress */
    if(FALSE == (gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c)))
    {
        return(gNV_ERR_CHANGEPROT_c);

    }
    /* Check if EERAM is set for EEE */
    if(!(gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FCNFG_OFFSET_c, gNV_FCNFG_EEERDY_c)))
    {
        /* EERAM is not set for EEE */
        return(gNV_ERR_NOEEE_c);
    }

    /* Read the value of FDPROT register */
    registerValue = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FEPROT_OFFSET_c);

    /* Check if normal mode */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FEPROT_OFFSET_c, 0xFF);

    if(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FEPROT_OFFSET_c) != 0xFF)
    {
        /* if normal mode */
        temp = (uint8_t)(protectStatus ^ registerValue);
        if(!(temp & protectStatus))
        {
            gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FEPROT_OFFSET_c, protectStatus);
        }
        else
        {
            returnCode = gNV_ERR_CHANGEPROT_c;
        }
        return(returnCode);
    }
    /* if unprotected or special mode */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FEPROT_OFFSET_c, protectStatus);

    return(returnCode);
}


/***********************************************************************
 *
 *  Function Name    : NV_FlashCheckSum
 *  Description      : This function is used to calculate checksum value
 *                     for the specified flash range.
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] destination - destination address
 *                     [IN] size - length of data
 *                     [OUT] pSum - pointer to the location where the
 *                                  calculated checksum will be place
 *  Return Value     : uint32_t
 *
 ************************************************************************/
uint32_t NV_FlashCheckSum
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint32_t size,
        uint32_t* pSum
)
{
    uint32_t data;             /* Data read from Flash address */
    uint32_t endAddress;       /* P Flash end address */

    /* calculating Flash end address */
    endAddress = destination + size;

    /* check for valid range of the target addresses */
    if((destination < pConfig->PFlashBlockBase) || \
            (endAddress > (pConfig->PFlashBlockBase + pConfig->PFlashBlockSize)))
    {
        if((destination < pConfig->DFlashBlockBase) || \
                (endAddress > (pConfig->DFlashBlockBase + pConfig->DFlashBlockSize)))
        {
            /* return an error code gNV_ERR_RANGE_c */
            return(gNV_ERR_RANGE_c);
        }
    }

    *pSum = 0;
    /* doing sum operation */
    while(destination < endAddress)
    {
        data = gNV_READ8(destination);
        *pSum += (uint32_t)data;
        destination += 1;
        size -= 1;
    }

    return(gNV_OK_c);
}


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
uint32_t NV_FlashEraseAllBlock
(
        pNvConfig_t pConfig
)
{
    uint8_t commandArray[1];     /* command sequence array */

    /* preparing passing parameter to erase all flash blocks */
    /* 1st element for the FCCOB register */
    commandArray[0] = gNV_ERASE_ALL_BLOCK_c;

    /* calling flash command sequence function to execute the command */
    return(NvFlashCommandSequence(pConfig, 0, commandArray));
}


/************************************************************************
 *
 *  Function Name    : NV_FlashEraseBlock
 *  Description      : The Erase Flash Block operation will erase all
 *                     addresses in a single P-Flash or D-Flash block.
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] destination - destination address
 *  Return Value     : uint32_t
 *
 *************************************************************************/
uint32_t NV_FlashEraseBlock
(
        pNvConfig_t pConfig,
        uint32_t destination
)
{
    uint8_t pCommandArray[4]; /* command sequence array */

    /* check if the destination is longword aligned or not */
    if (0 != (destination % gNV_ERSBLK_ALIGN_SIZE_c))
    {
        /* return an error code gNV_ERR_ADDR */
        return(gNV_ERR_ADDR_c);
    }

    /* check for valid range of the target addresses */
    if((destination < pConfig->PFlashBlockBase) || \
            (destination >= (pConfig->PFlashBlockBase + pConfig->PFlashBlockSize)))
    {
        if((destination < pConfig->DFlashBlockBase) || \
                (destination >= (pConfig->DFlashBlockBase + pConfig->DFlashBlockSize)))
        {
            /* return an error code gNV_ERR_RANGE_c */
            return(gNV_ERR_RANGE_c);
        }
        else
        {
            /* Convert System memory address to FTFx internal memory address */
            destination = destination - pConfig->DFlashBlockBase + 0x800000;
        }
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
        destination -= pConfig->PFlashBlockBase;
    }

    /* preparing passing parameter to erase a flash block */
    pCommandArray[0] = gNV_ERASE_BLOCK_c;
    pCommandArray[1] = (uint8_t)(destination >> 16);
    pCommandArray[2] = (uint8_t)((destination >> 8) & 0xFF);
    pCommandArray[3] = (uint8_t)(destination & 0xFF);

    /* calling flash command sequence function to execute the command */
    return(NvFlashCommandSequence(pConfig, 3, pCommandArray));
}


/************************************************************************
 *
 *  Function Name    : NV_FlashEraseSuspend
 *  Description      : This function is used to suspend a current operation
 *                     of flash erase sector command.
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *  Return Value     : uint32_t
 *
 *************************************************************************/
uint32_t NV_FlashEraseSuspend
(
        pNvConfig_t pConfig
)
{

    /* check CCIF bit of the flash status register */
    if(FALSE == (gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c)))
    {
        /* If the command write sequence in progressing, */
        /* Set ERSSUSP bit in FCNFG register */
        gNV_REG_BIT_SET(pConfig->ftfxRegBase + gNV_FCNFG_OFFSET_c, gNV_FCNFG_ERSSUSP_c);
    }

    return(gNV_OK_c);
}


/************************************************************************
 *
 *  Function Name    : NV_FlashEraseResume
 *  Description      : This function is used to resume a previous suspended
 *                     operation of flash erase sector command.
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *  Return Value     : uint32_t
 *
 *************************************************************************/
uint32_t NV_FlashEraseResume
(
        pNvConfig_t pConfig
)
{

    /* check ERSSUSP bit of the flash configuration register */
    if(TRUE == (gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FCNFG_OFFSET_c, gNV_FCNFG_ERSSUSP_c)))
    {
        /* If the ERSSUSP bit is set, */
        /* Clear CCIF bit to resumimg the previous suspended Erasing */
        gNV_REG_BIT_CLEAR(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c);
    }

    return(gNV_OK_c);
}


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
uint32_t NV_FlashEraseSector
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint32_t size
)
{
    uint8_t pCommandArray[4]; /* command sequence array */
    uint32_t returnCode;      /* return code variable */
    uint32_t endAddress;      /* storing end address */
    uint32_t sectorSize;      /* size of one sector */
    
   // extern unsigned long __region_ROM_end__[];

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* calculating Flash end address */
    endAddress = destination + size;

    /* check for valid range of the target addresses */
    if((destination < pConfig->PFlashBlockBase) || \
            (endAddress > (pConfig->PFlashBlockBase + pConfig->PFlashBlockSize)))
    {
        if((destination < pConfig->DFlashBlockBase) || \
                (endAddress > (pConfig->DFlashBlockBase + pConfig->DFlashBlockSize)))
        {
            /* return an error code gNV_ERR_RANGE_c */
            return(gNV_ERR_RANGE_c);
        }
        else
        {
            /* Convert System memory address to FTFx internal memory address */
            destination = destination - pConfig->DFlashBlockBase + 0x800000;
            sectorSize = gNV_DSECTOR_SIZE_c;
        }
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
        destination -= pConfig->PFlashBlockBase;
        sectorSize = gNV_PSECTOR_SIZE_c;
    }

    /* check if the destination is sector aligned or not */
    if (0 != (destination % sectorSize))
    {
        /* return an error code gNV_ERR_ADDR */
        return(gNV_ERR_ADDR_c);
    }

    /* check if the size is sector alignment or not */
    if(0 != (size % sectorSize))
    {
        /* return an error code gNV_ERR_SIZE */
        return(gNV_ERR_SIZE_c);
    }

    while(size > 0)
    {
        /* preparing passing parameter to erase a flash block */
        pCommandArray[0] = gNV_ERASE_SECTOR_c;
        pCommandArray[1] = (uint8_t)(destination >> 16);
        pCommandArray[2] = (uint8_t)((destination >> 8) & 0xFF);
        pCommandArray[3] = (uint8_t)(destination & 0xFF);
                
        /* calling flash command sequence function to execute the command */
        returnCode = NvFlashCommandSequence(pConfig, 3, pCommandArray);

        /* checking the success of command execution */
        if(gNV_OK_c != returnCode)
        {
            break;
        }
        else
        {
            /* update size and destination address */
            size -= sectorSize;
            destination += sectorSize;
        }
    }

    return(returnCode);
}


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
uint32_t NV_FlashGetInterruptEnable
(
        pNvConfig_t pConfig,
        uint8_t* interruptState
)
{
    /* store data read from flash register */
    uint8_t  registerValue;

    /* Initial interrupt state value */
    *interruptState = 0;

    /*Get flash configuration register value */
    registerValue = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCNFG_OFFSET_c);

    /* Add command complete interrupt enable bit value */
    /* CCIE bit of the FCNFG register */
    *interruptState |= (registerValue & (gNV_FCNFG_CCIE_c | gNV_FCNFG_RDCOLLIE_c));

    return(gNV_OK_c);
}


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
uint32_t NV_FlashGetSecurityState
(
        pNvConfig_t pConfig,
        uint8_t* securityState
)
{
    /* store data read from flash register */
    uint8_t  registerValue;

    /*Get flash security register value */
    registerValue = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FSEC_OFFSET_c);

    /* check the status of the flash security bits in the security register */
    if(gNV_FLASH_SECURITY_STATE_UNSECURED_c == (registerValue & gNV_FSEC_SEC_c))
    {
        /* Flash in unsecured state */
        *securityState = gNV_FLASH_NOT_SECURE_c;
    }
    else
    {
        /* Flash in secured state */
        /* check for backdoor key security enable bit */
        if(gNV_FLASH_SECURITY_STATE_KEYEN_c == (registerValue & gNV_FSEC_KEYEN_c))
        {
            /* Backdoor key security enabled */
            *securityState = gNV_FLASH_SECURE_BACKDOOR_ENABLED_c;
        }
        else
        {
            /* Backdoor key security disabled */
            *securityState = gNV_FLASH_SECURE_BACKDOOR_DISABLED_c;
        }
    }

    return(gNV_OK_c);
}


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
uint32_t NV_FlashProgramCheck
(
        pNvConfig_t pConfig,
        uint32_t  destination,
        uint32_t  size,
        uint8_t*  pExpectedData,
        uint8_t   marginLevel
)
{
    uint8_t pCommandArray[12]; /* command sequence array */
    uint32_t returnCode;       /* return code variable */

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* check if the destination is Longword aligned or not */
    if (0 != (destination % gNV_PGMCHK_ALIGN_SIZE_c))
    {
        /* return an error code gNV_ERR_ADDR */
        return(gNV_ERR_ADDR_c);
    }

    /* check if the size is Longword aligned or not */
    if (0 != size % gNV_PGMCHK_ALIGN_SIZE_c)
    {
        /* return an error code gNV_ERR_ADDR */
        return(gNV_ERR_SIZE_c);
    }

    /* check for valid range of the target addresses */
    if((destination < pConfig->PFlashBlockBase) || \
            ((destination+size) > (pConfig->PFlashBlockBase + pConfig->PFlashBlockSize)))
    {
        if((destination < pConfig->DFlashBlockBase) || \
                ((destination+size) > (pConfig->DFlashBlockBase + pConfig->DFlashBlockSize)))
        {
            /* return an error code gNV_ERR_RANGE_c */
            return(gNV_ERR_RANGE_c);
        }
        else
        {
            /* Convert System memory address to FTFx internal memory address */
            destination = destination - pConfig->DFlashBlockBase + 0x800000;
        }
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
        destination -= pConfig->PFlashBlockBase;
    }
    while (size)
    {

        /* preparing passing parameter to program check the flash block */
        pCommandArray[0] = gNV_PROGRAM_CHECK_c;
        pCommandArray[1] = (uint8_t)(destination >> 16);
        pCommandArray[2] = (uint8_t)((destination >> 8) & 0xFF);
        pCommandArray[3] = (uint8_t)(destination & 0xFF);
        pCommandArray[4] = marginLevel;

        pCommandArray[8] = *(pExpectedData+3);
        pCommandArray[9] = *(pExpectedData+2);
        pCommandArray[10] = *(pExpectedData+1);
        pCommandArray[11] = *pExpectedData;

        /* calling flash command sequence function to execute the command */
        returnCode = NvFlashCommandSequence(pConfig, 11, pCommandArray);

        /* checking for the success of command execution */
        if(gNV_OK_c != returnCode)
        {
            return(returnCode);
        }

        size-=gNV_LONGWORD_SIZE_c;
        pExpectedData+=gNV_LONGWORD_SIZE_c;
        destination+= gNV_LONGWORD_SIZE_c;
    }

    return(returnCode);
}


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
uint32_t NV_FlashProgramLongword
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint32_t size,
        uint32_t source
)
{
    uint8_t pCommandArray[8]; /* command sequence array */
    uint32_t returnCode;      /* return code variable */
    uint32_t endAddress;      /* storing end address */
    uint8_t loopCnt;          /* loop counter */

    uint8_t paddingBytes[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t paddingBytesCnt = 0;
    
 //   extern unsigned long __region_ROM_end__[];

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* calculating Flash end address */
    endAddress = destination + size;

    /* check if the destination is longword aligned or not */
    if((destination & (~0x03uL)) != destination)
    {
        /* align to next 32 bit boundary */
        destination &= ~0x03uL;
        destination += gNV_LONGWORD_SIZE_c;
    }

    paddingBytesCnt = size % gNV_LONGWORD_SIZE_c;

    /* check if the size is Longword alignment or not */

    if(0 != paddingBytesCnt)
    {
        size -= paddingBytesCnt;
        EffectiveWrBytesCnt = size + gNV_LONGWORD_SIZE_c;
    }
    else
    {
        EffectiveWrBytesCnt = size;
    }

    /* check for valid range of the target addresses */
    if((destination < pConfig->PFlashBlockBase) || \
            (endAddress > (pConfig->PFlashBlockBase + pConfig->PFlashBlockSize)))
    {
        if((destination < pConfig->DFlashBlockBase) || \
                (endAddress > (pConfig->DFlashBlockBase + pConfig->DFlashBlockSize)))
        {
            /* return an error code gNV_ERR_RANGE_c */
            return(gNV_ERR_RANGE_c);
        }
        else
        {
            /* Convert System memory address to FTFx internal memory address */
            destination = destination - pConfig->DFlashBlockBase + 0x800000;
        }
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
        destination -= pConfig->PFlashBlockBase;
    }

    while(size > 0)
    {
        /* preparing passing parameter to program the flash block */
        pCommandArray[0] = gNV_PROGRAM_LONGWORD_c;
        pCommandArray[1] = (uint8_t)(destination >> 16);
        pCommandArray[2] = (uint8_t)((destination >> 8) & 0xFF);
        pCommandArray[3] = (uint8_t)(destination & 0xFF);

        pCommandArray[4] = gNV_READ8(source + 3);
        pCommandArray[5] = gNV_READ8(source + 2);
        pCommandArray[6] = gNV_READ8(source + 1);
        pCommandArray[7] = gNV_READ8(source);
              
        /* calling flash command sequence function to execute the command */
        returnCode = NvFlashCommandSequence(pConfig, 7, pCommandArray);

        /* checking for the success of command execution */
        if(gNV_OK_c != returnCode)
        {
            break;
        }
        else
        {
            /* update destination address for next iteration */
            destination += gNV_LONGWORD_SIZE_c;
            /* update size for next iteration */
            size -= gNV_LONGWORD_SIZE_c;
            /* increment the source address */
            source += gNV_LONGWORD_SIZE_c;
        }
    }

    if(0 != paddingBytesCnt)
    {
        /* add padding bytes */

        for(loopCnt = 0; loopCnt < paddingBytesCnt; loopCnt++)
        {
            paddingBytes[loopCnt] = gNV_READ8(source++);
        }

        /* preparing passing parameter to program the flash block */
        pCommandArray[0] = gNV_PROGRAM_LONGWORD_c;
        pCommandArray[1] = (uint8_t)(destination >> 16);
        pCommandArray[2] = (uint8_t)((destination >> 8) & 0xFF);
        pCommandArray[3] = (uint8_t)(destination & 0xFF);

        pCommandArray[4] = paddingBytes[3];
        pCommandArray[5] = paddingBytes[2];
        pCommandArray[6] = paddingBytes[1];
        pCommandArray[7] = paddingBytes[0];
        
        /* calling flash command sequence function to execute the command */
        returnCode = NvFlashCommandSequence(pConfig, 7, pCommandArray);
    }

    return(returnCode);
}

/************************************************************************
 *
 *  Function Name    : NV_FlashProgramUnalignedLongword
 *  Description      : Program data into Flash. The address can be unaligned
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] destination - destination address
 *                     [IN] size - data set size
 *                     [IN] source - source address
 *  Return Value     : uint32_t
 *
 *************************************************************************/
uint32_t NV_FlashProgramUnalignedLongword
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint32_t size,
        uint32_t source
)
{
  uint8_t i, bytes = destination & 0x03;
  uint32_t tmp;
  uint8_t alignWord[4] = {0xFF, 0xFF, 0xFF, 0xFF};

  if (bytes)
  {
	uint32_t *pAddress;  
	  
    /* Go to previous aligned address */
    destination &= ~((uint32_t)0x03);

    /* Read LongWord from flash */
    pAddress = (uint32_t*)alignWord; 
    *pAddress = gNV_READ32(destination); // to suppress GCC compiler warning
   
    /* Add the unaligned bytes */
    for (i = (gNV_LONGWORD_SIZE_c - bytes); i < gNV_LONGWORD_SIZE_c; i++)
      alignWord[i] = gNV_READ8(source++);

    /* Write the new LongWord */
    tmp = NV_FlashProgramLongword(pConfig, destination, gNV_LONGWORD_SIZE_c, (uint32_t)&alignWord);

    if (gNV_OK_c != tmp)
      return tmp;

    destination += gNV_LONGWORD_SIZE_c;
    size -= bytes;
  }

  return NV_FlashProgramLongword(pConfig, destination, size, source);
}

/************************************************************************
 *
 *  Function Name    : NV_FlashRead
 *  Description      : Read data stored in flash memory
 *  Arguments        : [IN] address - flash memory address
 *                     [OUT] pData - pointer to a memory location
 *                                   where the data will be stored
 *                     [IN] len - data length to be read
 *  Return Value     : -
 *
 *************************************************************************/
void NV_FlashRead
(
        uint32_t address,
        uint8_t* pData,
        uint32_t len
)
{
    while(len--)
    {
        *pData++ = ((uint8_t)(*(vuint8_t*)(address++)));
    }
}

/************************************************************************
 *
 *  Function Name    : NV_FlashProgramOnce
 *  Description      : Program data into a dedicated 64 bytes region in
 *                     the P-Flash IFR which stores critical information
 *                     for the user
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] pDataArray - pointer to a memory location
 *                                       where the data is stored
 *  Return Value     : uint32_t
 *
 *************************************************************************/
uint32_t NV_FlashProgramOnce
(
        pNvConfig_t pConfig,
        uint8_t* pDataArray
)
{
    uint8_t  counter;          /* outer loop counter */
    uint8_t  pCommandArray[8]; /* command sequence array */
    uint32_t returnCode;       /* return code variable */
    uint8_t i;                 /* variable to store record number index */

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;
    /* set the counter value for 0 */
    counter = 0;
    /* calculate record number index */
    i = (64 / (gNV_RDONCE_INDEX_MAX_c + 1));

    /* preparing passing parameter for program once command */
    /* 1st element for the FCCOB register */
    pCommandArray[0] = gNV_PROGRAM_ONCE_c;

    while (counter <= gNV_RDONCE_INDEX_MAX_c)
    {
        /* preparing the rest of the parameters to be passed for the FCCOB registers */
        pCommandArray[1] = counter;
        pCommandArray[2] = 0x00;
        pCommandArray[3] = 0x00;
        pCommandArray[4] = pDataArray[counter*i];
        pCommandArray[5] = pDataArray[counter*i + 1];
        pCommandArray[6] = pDataArray[counter*i + 2];
        pCommandArray[7] = pDataArray[counter*i + 3];
#if ((gNV_512K_512K_16K_4K_4K_c == gNV_Derivative_c) || (gNV_1024K_0K_16K_4K_0K_c == gNV_Derivative_c))                 
        pCommandArray[8] = pDataArray[counter*i + 4];
        pCommandArray[9] = pDataArray[counter*i + 5];
        pCommandArray[10] = pDataArray[counter*i + 6];
        pCommandArray[11] = pDataArray[counter*i + 7];
#endif

        /* calling flash command sequence API to execute the command */
        returnCode = NvFlashCommandSequence(pConfig, i + 3, pCommandArray);

        /* checking for the success of command execution */
        if(gNV_OK_c != returnCode)
        {
            break;
        }

        /* decrement the counter value */
        counter++;
    }

    return(returnCode);
}


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
uint32_t NV_FlashProgramPhrase
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint32_t size,
        uint32_t source
)
{
    uint8_t pCommandArray[12]; /* command sequence array */
    uint32_t returnCode;       /* return code variable */
    uint32_t endAddress;       /* storing end address */

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* calculating Flash end address */
    endAddress = destination + size;

    /* check if the destination is Longword aligned or not */
    if (0 != (destination % gNV_PHRASE_SIZE_c))
    {
        /* return an error code gNV_ERR_ADDR */
        return(gNV_ERR_ADDR_c);
    }

    /* check if the size is Longword alignment or not */
    if(0 != (size % gNV_PHRASE_SIZE_c))
    {
        /* return an error code gNV_ERR_SIZE */
        return(gNV_ERR_SIZE_c);
    }

    /* check for valid range of the target addresses */
    if((destination < pConfig->PFlashBlockBase) || \
            (endAddress > (pConfig->PFlashBlockBase + pConfig->PFlashBlockSize)))
    {
        if((destination < pConfig->DFlashBlockBase) || \
                (endAddress > (pConfig->DFlashBlockBase + pConfig->DFlashBlockSize)))
        {
            /* return an error code gNV_ERR_RANGE_c */
            return(gNV_ERR_RANGE_c);
        }
        else
        {
            /* Convert System memory address to FTFx internal memory address */
            destination = destination - pConfig->DFlashBlockBase + 0x800000;
        }
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
        destination -= pConfig->PFlashBlockBase;
    }

    while(size > 0)
    {
        /* preparing passing parameter to program the flash block */
        pCommandArray[0] = gNV_PROGRAM_PHRASE_c;
        pCommandArray[1] = (uint8_t)(destination >> 16);
        pCommandArray[2] = (uint8_t)((destination >> 8) & 0xFF);
        pCommandArray[3] = (uint8_t)(destination & 0xFF);

        pCommandArray[4] = gNV_READ8(source + 3);
        pCommandArray[5] = gNV_READ8(source + 2);
        pCommandArray[6] = gNV_READ8(source + 1);
        pCommandArray[7] = gNV_READ8(source);
        pCommandArray[8] = gNV_READ8(source + 7);
        pCommandArray[9] = gNV_READ8(source + 6);
        pCommandArray[10] = gNV_READ8(source + 5);
        pCommandArray[11] = gNV_READ8(source + 4);

        /* calling flash command sequence function to execute the command */
        returnCode = NvFlashCommandSequence(pConfig, 11, pCommandArray);

        /* checking for the success of command execution */
        if(gNV_OK_c != returnCode)
        {
            break;
        }

        /* update destination address for next iteration */
        destination += gNV_PHRASE_SIZE_c;
        /* update size for next iteration */
        size -= gNV_PHRASE_SIZE_c;
        /* increment the source address by 1 */
        source += gNV_PHRASE_SIZE_c;
    }

    return(returnCode);
}


/************************************************************************
 *
 *  Function Name    : NV_FlashReadOnce
 *  Description      : This function is used to read access to a reserved
 *                     64 byte field located in the P-Flash IFR.
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] pDataArray - pointer to location where read data
 *                                       will be placed
 *  Return Value     : uint32_t
 *
 *************************************************************************/
uint32_t NV_FlashReadOnce
(
        pNvConfig_t pConfig,
        uint8_t* pDataArray
)
{
    uint8_t  counter;          /* outer loop counter */
    uint8_t  pCommandArray[2]; /* command sequence array */
    uint32_t returnCode;       /* return code variable */
    uint8_t  i;                /* variable to store record number index */

    /* initialize counter and returnCode */
    counter = 0;

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;
    /* calculate record number index */
    i = 64/(gNV_RDONCE_INDEX_MAX_c + 1);
    /* preparing passing parameter for read once command */
    /* 1st element for the FCCOB register */
    pCommandArray[0] = gNV_READ_ONCE_c;

    while (counter <= gNV_RDONCE_INDEX_MAX_c)
    {
        /* 2nd element for the FCCOB register */
        pCommandArray[1] = counter;

        /* calling flash command sequence API to execute the command */
        returnCode = NvFlashCommandSequence(pConfig, 1, pCommandArray);

        /* checking for the success of command execution */
        if(gNV_OK_c != returnCode)
        {
            break;
        }

        /* Read the data from the FCCOB registers into the pDataArray */
        pDataArray[counter*i] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB4_OFFSET_c);
        pDataArray[counter*i + 1] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB5_OFFSET_c);
        pDataArray[counter*i + 2] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB6_OFFSET_c);
        pDataArray[counter*i + 3] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB7_OFFSET_c);
#if ((gNV_512K_512K_16K_4K_4K_c == gNV_Derivative_c) || (gNV_1024K_0K_16K_4K_0K_c == gNV_Derivative_c))
        pDataArray[counter*i + 4] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB8_OFFSET_c);
        pDataArray[counter*i + 5] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB9_OFFSET_c);
        pDataArray[counter*i + 6] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOBA_OFFSET_c);
        pDataArray[counter*i + 7] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOBB_OFFSET_c);
#endif
        counter++;
    }

    return(returnCode);
}


/************************************************************************
 *
 *  Function Name    : NV_FlashReadResource
 *  Description      : This function is provided for the user to read data
 *                     from P-Flash IFR and D-Flash IFR space.
 *  Arguments        :[IN] pConfig - pointer to NV configuration
 *                    [IN] destination - destination address
 *                    [IN] pDataArray - pointer to location where read data
 *                                      will be placed
 *  Return Value     : uint32_t
 *
 *************************************************************************/
uint32_t NV_FlashReadResource
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint8_t* pDataArray
)
{
    uint8_t  pCommandArray[9]; /* command sequence array */
    uint32_t returnCode;       /* return code variable */

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* check if the destination is longword aligned or not */
    if (0 != destination % gNV_RDRSRC_ALIGN_SIZE_c)
    {
        /* return an error code gNV_ERR_ADDR */
        return(gNV_ERR_ADDR_c);
    }

    /* check for valid range of the target addresses */
    if((destination < ( pConfig->PFlashBlockBase + gNV_PFLASH_IFR_OFFSET_c )) || \
            (destination >= ( pConfig->PFlashBlockBase + gNV_PFLASH_IFR_SIZE_c )))
    {
        if((destination < ( pConfig->DFlashBlockBase + gNV_DFLASH_IFR_OFFSET_c )) || \
                (destination >= ( pConfig->DFlashBlockBase + gNV_DFLASH_IFR_SIZE_c )))
        {
            /* return an error code gNV_ERR_RANGE_c */
            return(gNV_ERR_RANGE_c);
        }
        else
        {
            /* Convert System memory address to FTFx internal memory address */
            destination = destination - pConfig->DFlashBlockBase + 0x800000;
        }
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
        destination -= pConfig->PFlashBlockBase;
    }

    /* preparing passing parameter for read resource command */
    /* 1st element for the FCCOB register */
    pCommandArray[0] = gNV_READ_RESOURCE_c;
    pCommandArray[1] = (uint8_t)(destination >> 16);
    pCommandArray[2] = (uint8_t)((destination >> 8) & 0xFF);
    pCommandArray[3] = (uint8_t)(destination & 0xFF);
#if ((gNV_512K_512K_16K_4K_4K_c == gNV_Derivative_c) || (gNV_1024K_0K_16K_4K_0K_c == gNV_Derivative_c))
    pCommandArray[4] = 0x00;
    /* calling flash command sequence API to execute the command */
    returnCode = NvFlashCommandSequence(pConfig, 4, pCommandArray);
#else
    pCommandArray[8] = 0x00;
    /* calling flash command sequence API to execute the command */
    returnCode = NvFlashCommandSequence(pConfig, 8, pCommandArray);
#endif

    /* checking for the success of command execution */
    if(gNV_OK_c != returnCode)
    {
        return(returnCode);
    }

    /* Read the data from the FCCOB registers into the pDataArray */
    pDataArray[0] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB4_OFFSET_c);
    pDataArray[1] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB5_OFFSET_c);
    pDataArray[2] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB6_OFFSET_c);
    pDataArray[3] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB7_OFFSET_c);
#if ((gNV_512K_512K_16K_4K_4K_c == gNV_Derivative_c) || (gNV_1024K_0K_16K_4K_0K_c == gNV_Derivative_c))
    pDataArray[4] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB8_OFFSET_c);
    pDataArray[5] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOB9_OFFSET_c);
    pDataArray[6] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOBA_OFFSET_c);
    pDataArray[7] = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCCOBB_OFFSET_c);
#endif

    return(returnCode);
}


/************************************************************************
 *
 *  Function Name    : NV_FlashSecurityBypass
 *  Description      : If the MCU is secured state, this function will
 *                     unsecure the MCU by comparing the provided backdoor
 *                     key with ones in the Flash Configuration Field.
 *  Arguments        : [IN] pConfig - pointer to NV configuration
 *                     [IN] keyBuffer - pointer to a location where the key
 *                                      is stored
 *  Return Value     : uint32_t
 *
 *************************************************************************/
uint32_t NV_FlashSecurityBypass
(
        pNvConfig_t pConfig,
        uint8_t* keyBuffer
)
{
    uint8_t pCommandArray[12]; /* command sequence array */
    uint8_t registerValue;    /* registerValue */
    uint32_t returnCode;      /* return code variable */

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /*Get flash security register value */
    registerValue = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FSEC_OFFSET_c);

    if (0x02 != (registerValue & 0x03))
    {
        /* preparing passing parameter to erase a flash block */
        pCommandArray[0] = gNV_SECURITY_BY_PASS_c;
        pCommandArray[4] = keyBuffer[0];
        pCommandArray[5] = keyBuffer[1];
        pCommandArray[6] = keyBuffer[2];
        pCommandArray[7] = keyBuffer[3];
        pCommandArray[8] = keyBuffer[4];
        pCommandArray[9] = keyBuffer[5];
        pCommandArray[10] = keyBuffer[6];
        pCommandArray[11] = keyBuffer[7];

        /* calling flash command sequence function to execute the command */
        returnCode = NvFlashCommandSequence(pConfig, 11, pCommandArray);
    }

    return(returnCode);
}


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
uint32_t NV_FlashSetInterruptEnable
(
        pNvConfig_t pConfig,
        uint8_t interruptState
)
{
    /* store data read from flash register */
    uint8_t  registerValue;

    /* Mask off unused bits in interruptState */
    interruptState &= (gNV_FCNFG_CCIE_c | gNV_FCNFG_RDCOLLIE_c);

    /*Get flash configuration register value */
    registerValue = gNV_REG_READ(pConfig->ftfxRegBase + gNV_FCNFG_OFFSET_c);

    /* Set register value with desired interrupt state */
    registerValue &= ~(gNV_FCNFG_CCIE_c | gNV_FCNFG_CCIE_c);
    registerValue |= interruptState;
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FCNFG_OFFSET_c, registerValue);

    return(gNV_OK_c);
}


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
uint32_t NV_FlashVerifyAllBlock
(
        pNvConfig_t pConfig,
        uint8_t marginLevel
)
{
    uint8_t pCommandArray[2]; /* command sequence array */
    uint32_t returnCode;      /* return code variable */

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* preparing passing parameter to verify all block command*/
    pCommandArray[0] = gNV_VERIFY_ALL_BLOCK_c;
    pCommandArray[1] = marginLevel;

    /* calling flash command sequence function to execute the command */
    returnCode = NvFlashCommandSequence(pConfig, 1, pCommandArray);

    return(returnCode);
}


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
uint32_t NV_FlashVerifyBlock
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint8_t marginLevel
)
{
    uint8_t pCommandArray[5]; /* command sequence array */
    uint32_t returnCode;      /* return code variable */

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* check if the destination is longword aligned or not */
    if (0 != (destination % gNV_RD1BLK_ALIGN_SIZE_c))
    {
        /* return an error code gNV_ERR_ADDR */
        return(gNV_ERR_ADDR_c);
    }

    /* check for valid range of the target addresses */
    if((destination < pConfig->PFlashBlockBase) || \
            (destination >= (pConfig->PFlashBlockBase + pConfig->PFlashBlockSize)))
    {
        if((destination < pConfig->DFlashBlockBase) || \
                (destination >= (pConfig->DFlashBlockBase + pConfig->DFlashBlockSize)))
        {
            /* return an error code gNV_ERR_RANGE_c */
            return(gNV_ERR_RANGE_c);
        }
        else
        {
            /* Convert System memory address to FTFx internal memory address */
            destination = destination - pConfig->DFlashBlockBase + 0x800000;
        }
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
        destination -= pConfig->PFlashBlockBase;
    }

    /* preparing passing parameter to verify block command */
    pCommandArray[0] = gNV_VERIFY_BLOCK_c;
    pCommandArray[1] = (uint8_t)(destination >> 16);
    pCommandArray[2] = (uint8_t)((destination >> 8) & 0xFF);
    pCommandArray[3] = (uint8_t)(destination & 0xFF);
    pCommandArray[4] = marginLevel;

    /* calling flash command sequence function to execute the command */
    returnCode = NvFlashCommandSequence(pConfig, 4, pCommandArray);

    return(returnCode);
}


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
uint32_t NV_FlashVerifySection
(
        pNvConfig_t pConfig,
        uint32_t destination,
        uint16_t Length,
        uint8_t marginLevel
)
{
    uint8_t pCommandArray[7]; /* command sequence array */
    uint32_t returnCode;      /* return code variable */
    uint32_t endAddress;      /* storing end address. This is the starting point of the next block */
    uint8_t misalignedBytes;
    uint16_t Number;
    uint8_t alignSize;

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* check for valid range of the destination */
    if((destination >= pConfig->PFlashBlockBase) && \
            (destination < (pConfig->PFlashBlockBase + pConfig->PFlashBlockSize)))
    {
        /* calculating Flash end address */
    	endAddress = destination + Length;
        alignSize = gNV_PRD1SEC_ALIGN_SIZE_c;
    }
    else if((destination >= pConfig->DFlashBlockBase) && \
            (destination < (pConfig->DFlashBlockBase + pConfig->DFlashBlockSize)))
    {
        /* calculating Flash end address */
    	endAddress = destination + Length;
        alignSize = gNV_DRD1SEC_ALIGN_SIZE_c;    	
    }
    else
    {
        /* return an error code gNV_ERR_RANGE_c */
        return(gNV_ERR_RANGE_c);
    }

    /* check for valid range of end address */
    if((endAddress >= pConfig->PFlashBlockBase) && \
            (endAddress <= (pConfig->PFlashBlockBase + pConfig->PFlashBlockSize)))
    {
        /* Convert System memory address to FTFx internal memory address */
        destination -= pConfig->PFlashBlockBase;
    }
    else if((endAddress >= pConfig->DFlashBlockBase) || \
            (endAddress <= (pConfig->DFlashBlockBase + pConfig->DFlashBlockSize)))
    {
        /* Convert System memory address to FTFx internal memory address */
        destination = destination - pConfig->DFlashBlockBase + 0x800000;
    }
    else
    {
        /* return an error code gNV_ERR_RANGE_c */
        return(gNV_ERR_RANGE_c);
    }

    /* align destination (if misaligned) and blank check the misaligned addresses */
    misalignedBytes = destination - (destination & (~0x07uL));
    while(misalignedBytes)
    {
      uint8_t pData;
      pData = ((uint8_t)(*(vuint8_t*)(destination)));
      if(pData != 0xFF)
      {
        return(gNV_ERR_ACCERR_c);
      }
      destination++;
      Length--;
      misalignedBytes--;
    }

    /* align end address (if misaligned) and blank check the misaligned addresses */
    misalignedBytes = endAddress - (endAddress & (~0x07uL));
    while(misalignedBytes)
    {
      uint8_t pData;
      pData = ((uint8_t)(*(vuint8_t*)(endAddress)));
      if(pData != 0xFF)
      {
        return(gNV_ERR_ACCERR_c);
      }
      endAddress--;
      Length--;
      misalignedBytes--;
    }

    /* calculate the number of phrases needed by the verify section command */
    Number = Length / alignSize;

    /* preparing passing parameter to verify section command */
    pCommandArray[0] = gNV_VERIFY_SECTION_c;
    pCommandArray[1] = (uint8_t)(destination >> 16);
    pCommandArray[2] = (uint8_t)((destination >> 8) & 0xFF);
    pCommandArray[3] = (uint8_t)(destination & 0xFF);
    pCommandArray[4] = (uint8_t)(Number >> 8);
    pCommandArray[5] = (uint8_t)(Number & 0xFF);
    pCommandArray[6] = marginLevel;

    /* calling flash command sequence function to execute the command */
    returnCode = NvFlashCommandSequence(pConfig, 6, pCommandArray);

    return(returnCode);
}


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
uint32_t NV_PFlashSetProtection
(
        pNvConfig_t pConfig,
        uint32_t protectStatus
)
{
    uint32_t registerValue;    /* store data read from flash register */
    uint32_t returnCode;       /* return code variable */
    uint32_t temp;
    uint32_t registerValue0;
    uint32_t registerValue1;
    uint32_t registerValue2;
    uint32_t registerValue3;

    /* set the default return as gNV_OK_c */
    returnCode = gNV_OK_c;
    /* check CCIF bit of the flash status register, it's impossible to write to FPROT if command is in progress */
    if(FALSE == (gNV_REG_BIT_TEST(pConfig->ftfxRegBase + gNV_FSTAT_OFFSET_c, gNV_FSTAT_CCIF_c)))
    {
        return(gNV_ERR_CHANGEPROT_c);
    }
    /* Read the value of FPPROT registers */
    registerValue0=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT0_OFFSET_c));
    registerValue1=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT1_OFFSET_c));
    registerValue2=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT2_OFFSET_c));
    registerValue3=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT3_OFFSET_c));

    registerValue = ((registerValue0 << 24 ) | \
            (registerValue1 << 16 ) | \
            (registerValue2 << 8 ) | \
            registerValue3);

    /* Check if normal mode */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT0_OFFSET_c, 0xFF);
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT1_OFFSET_c, 0xFF);
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT2_OFFSET_c, 0xFF);
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT3_OFFSET_c, 0xFF);

    /* Read the value of FPPROT registers */
    registerValue0=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT0_OFFSET_c));
    registerValue1=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT1_OFFSET_c));
    registerValue2=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT2_OFFSET_c));
    registerValue3=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT3_OFFSET_c));

    if((registerValue0 != 0xFF) | (registerValue1 != 0xFF) | \
            (registerValue2 != 0xFF) | (registerValue3 != 0xFF))
    {
        /* if normal mode */
        temp = protectStatus ^ registerValue;
        if(!(temp & protectStatus))
        {
            gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT0_OFFSET_c, (uint8_t)(protectStatus >> 24));
            gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT1_OFFSET_c, (uint8_t)(protectStatus >> 16));
            gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT2_OFFSET_c, (uint8_t)(protectStatus >> 8));
            gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT3_OFFSET_c, (uint8_t)protectStatus);
        }
        else
        {
            returnCode = gNV_ERR_CHANGEPROT_c;
        }
        return(returnCode);
    }
    /* if unprotected or special mode */
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT0_OFFSET_c, (uint8_t)(protectStatus >> 24));
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT1_OFFSET_c, (uint8_t)(protectStatus >> 16));
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT2_OFFSET_c, (uint8_t)(protectStatus >> 8));
    gNV_REG_WRITE(pConfig->ftfxRegBase + gNV_FPROT3_OFFSET_c, (uint8_t)protectStatus);

    return(returnCode);
}


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
uint32_t NV_PFlashGetProtection
(
        pNvConfig_t pConfig,
        uint32_t* protectStatus
)
{
    uint32_t registerValue0;
    uint32_t registerValue1;
    uint32_t registerValue2;
    uint32_t registerValue3;

    registerValue0=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT0_OFFSET_c));
    registerValue1=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT1_OFFSET_c));
    registerValue2=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT2_OFFSET_c));
    registerValue3=(uint32_t)(gNV_REG_READ(pConfig->ftfxRegBase + gNV_FPROT3_OFFSET_c));

    *protectStatus = ((registerValue0 << 24 ) | \
            (registerValue1 << 16 ) | \
            (registerValue2 << 8 ) | \
            registerValue3);

    return(gNV_OK_c);
}


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
uint32_t NV_SetEEEEnable
(
        pNvConfig_t pConfig,
        uint8_t EEEEnable
)
{
    uint8_t pCommandArray[2]; /* command sequence array */
    uint32_t returnCode;      /* return code variable */

    /* set the default return code as gNV_OK_c */
    returnCode = gNV_OK_c;

    /* preparing passing parameter to set EEPROM Enabled command */
    pCommandArray[0] = gNV_SET_EERAM_c;
    pCommandArray[1] = EEEEnable;

    /* calling flash command sequence function to execute the command */
    returnCode = NvFlashCommandSequence(pConfig, 1, pCommandArray);

    return(returnCode);
}


