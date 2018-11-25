/******************************************************************************
 * Filename: NV_Data.h
 *
 * Description: Declarations for the application client of the NV  
 *              storage module (NVM)
 * 
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

#ifndef _NV_DATA_H_
#define _NV_DATA_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"
#include "NVM_Interface.h"   


/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/*****************************************************************************/
/* Unique Nvm Data  Id's */
/* WARNING WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING*/
/* For instantiable stacks  the most significant nibble is used for pan Id */ 

/*PAN0*/
#define nvmId_NwkData_c                      0x0000
#define nvmId_BeeStackConfig_c               0x0001
#define nvmId_RouteTable_c                   0x0002
#define nvmId_NeighborTable_c                0x0003
#define nvmId_ApsAddressMapBitMask_c         0x0004
#define nvmId_ApsAddressMap_c                0x0005
#define nvmId_ApsKeySet_c                    0x0006
#define nvmId_ApsDeviceKeyPairSet_c          0x0007          
#define nvmId_IncomingFrameCounterSet1_c     0x0008
#define nvmId_IncomingFrameCounterSet2_c     0x0009
#define nvmId_OutgoingFrameCounter1_c        0x000A
#define nvmId_OutgoingFrameCounter2_c        0x000B
     
     /* Nvm Aps Data  Id's */
#define nvmId_BeeStackParameters_c           0x000C
#define nvmId_SAS_Ram_c                      0x000D
#define nvmId_ApsBindingTable_c              0x000E
#define nvmId_ApsGroupTable_c                0x000F

#define nvmId_AslData_c                      0x0010
#define nvmId_AppData_c                      0x0011  /* used for cluster attributes....*/
#define nvmId_AppSceneData_c                 0x0012  /* used for Scene Cluster ....*/
#define nvmId_ZclSE_KeyEstablished           0x0013     
#define nvmId_ZclSE_RegistrationTable        0x0014   
#define nvmId_AppTouchLinkData_c             0x0015  /* used for touchlink procedure */
#define nvmId_AppTouchLinkRemoteDevTable_c   0x0016  /* used for touchlink procedure to store commissioned devices */

/*  PAN1 */
#define nvmId_NwkDataPan1_c                  0x1000        
#define nvmId_ApsGroupTablePan1_c            0x100F

#define nvmId_AslDataPan1_c                  0x1010
#define nvmId_AppDataPan1_c                  0x1011  /* used for cluster attributes....*/
#define nvmId_AppSceneDataPan1_c             0x1012  /* used for Scene Cluster ....*/
#define nvmId_AppTouchLinkDataPan1_c         0x1015  /* used for touchlink procedure */
#define nvmId_AppTouchLinkRemoteDevTablePan1_c   0x1016  /* used for touchlink procedure to store commissioned devices */
/*****************************************************************************/

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/* None */

/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/

/* None */
extern void NVM_RegisterBeeStackDataSet(uint8_t beeStackInstanceId);
/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/

#if !gInstantiableStackEnabled_d
extern const NVM_DataEntry_t NVM_DataTable[];
#else
extern NVM_DataEntry_t NVM_DataTable[];
#endif

#ifdef __cplusplus
}
#endif

#endif //_NV_DATA_H_
