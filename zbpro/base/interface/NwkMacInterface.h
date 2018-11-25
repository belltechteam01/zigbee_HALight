/*! *********************************************************************************
* \file NwkMacInterface.h
* This header file is for backward compatibility with old baremetal mac implementation.
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

#ifndef _NWK_MAC_INTERFACE_H
#define _NWK_MAC_INTERFACE_H

#ifdef __cplusplus
    extern "C" {
#endif
/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "MacConfig.h"
#include "MacTypes.h"
#include "Messaging.h"
#include "MacInterface.h"

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef mlmeBeaconNotifyInd_t nwkBeaconNotifyInd_t;

typedef mlmeScanCnf_t nwkScanCnf_t;

typedef mlmeAssociateInd_t nwkAssociateInd_t;

typedef mlmeOrphanInd_t nwkOrphanInd_t;

typedef uint8_t primMlmeToNwk_t;

typedef uint8_t messageId_t;

/* TX option bit fields */
enum {
  gTxOptsAck_c       = 1<<0,
  gTxOptsGts_c       = 1<<1,
  gTxOptsIndirect_c  = 1<<2,
  gTxOptsSecurity_c  = 1<<3,
  gTxOptsCCA_Mode2_c = 1<<4
};

/*-----------------------------------------------------------------------------------
  -----------------------------------------------------------------------------------
                     Primitives in the MLME to NWK direction 
  -----------------------------------------------------------------------------------
  ----------------------------------------------------------------------------------- */

#define  gNwkAssociateInd_c         gMlmeAssociateInd_c
#define  gNwkAssociateCnf_c         gMlmeAssociateCnf_c
#define  gNwkDisassociateInd_c      gMlmeDisassociateInd_c
#define  gNwkDisassociateCnf_c      gMlmeDisassociateCnf_c
#define  gNwkBeaconNotifyInd_c      gMlmeBeaconNotifyInd_c
#define  gNwkGetCnf_c               gMlmeGetCnf_c
#define  gNwkGtsInd_c               gMlmeGtsInd_c
#define  gNwkGtsCnf_c               gMlmeGtsCnf_c
#define  gNwkOrphanInd_c            gMlmeOrphanInd_c
#define  gNwkResetCnf_c             gMlmeResetCnf_c
#define  gNwkRxEnableCnf_c          gMlmeRxEnableCnf_c
#define  gNwkScanCnf_c              gMlmeScanCnf_c
#define  gNwkCommStatusInd_c        gMlmeCommStatusInd_c
#define  gNwkSetCnf_c               gMlmeSetCnf_c
#define  gNwkStartCnf_c             gMlmeStartCnf_c
#define  gNwkSyncLossInd_c          gMlmeSyncLossInd_c
#define  gNwkPollCnf_c              gMlmePollCnf_c
     
// Proprietary primitive : Poll notify indication
#define  gNwkPollNotifyInd_c        gMlmePollNotifyInd_c    

#define  gNwkErrorCnf_c             gMlmePollNotifyInd_c+1
#define  gNwkMaxPrimitives_c        gNwkErrorCnf_c+1

/* Superframe specification bit fields */
enum {
  gSuperFrameSpecLsbBO_c            = 0x0F,
  gSuperFrameSpecLsbSO_c            = 0xF0,
  gSuperFrameSpecMsbFinalCapSlot_c  = 0x0F,
  gSuperFrameSpecMsbBattlifeExt_c   = 0x10,
  gSuperFrameSpecMsbReserved_c      = 0x20,
  gSuperFrameSpecMsbPanCoord_c      = 0x40,
  gSuperFrameSpecMsbAssocPermit_c   = 0x80
};


 /* Address modes used by mlmeAssociateReq_t, mlmePollReq_t, mcpsDataReq_t, 
    nwkCommStatusInd_t, mcpsDataInd_t, and panDescriptor_t */
enum {
  gAddrModeNone_c  = 0,
  gAddrModeShort_c = 2,
  gAddrModeLong_c  = 3
};

#define aScanResultsPerBlock gScanResultsPerBlock_c
/*! *********************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/* None */

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

/* None */

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/* This header is included in stack */
resultType_t ZbNWK_MCPS_SapHandler( nwkToMcpsMessage_t* pMsg);
resultType_t ZbNWK_MLME_SapHandler( mlmeMessage_t*      pMsg);

uint8_t zbMCPS_NWK_SapHandler(mcpsToNwkMessage_t *pMsg);
uint8_t zbMLME_NWK_SapHandler(nwkMessage_t       *pMsg);

void MacPhyInit_WriteExtAddress(uint8_t *pExtendedAddress);

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/* None */
#ifdef __cplusplus
}
#endif
#endif  /* _NWK_MAC_INTERFACE_H */

