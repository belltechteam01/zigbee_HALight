/******************************************************************************
* In this file are the functions need it for the processing in 
*  frequency agility.
*
* (c) Copyright 2013, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
******************************************************************************/

#ifndef _ZdpFrequencyAgility_INTERFACE_H_
#define _ZdpFrequencyAgility_INTERFACE_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/
uint8_t FA_GetChannelFromList
(
  uintn32_t *pChannelList  /* IN: the 32-bits channel list, the list shall have only
                                  one valid channel. */
);

#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
/*------------- FA_Process_Mgmt_NWK_Update_notify --------------*/
void FA_Process_Mgmt_NWK_Update_notify
(
  zbMgmtNwkUpdateNotify_t  *pMessageComingIn  /* IN: The package with the request information. */
);

void FA_StateMachine
(
  tsEvent_t events
);

void FA_StateMachineInit
(
  void
);

void FA_ProcessNlmeTxReport
(
  nlmeNwkTxReport_t  *pNlmeNwkTxReport
);

void FA_ProcessEnergyScanCnf
(
  nlmeEnergyScanCnf_t *pScanCnf
);

uint32_t FA_BuildListFromChannel
(
  uint8_t  channelNumber
);

void FA_SelectChannelAndChange
(
  void
);

#define FA_SendChangeChannelEvt() TS_SendEvent(ZbZdoPrivateData(gFATaskId), gChangeChannel_c)
#else
#define FA_Process_Mgmt_NWK_Update_notify(pMessageComingIn)
#define FA_StateMachine(events)
#define FA_StateMachineInit()
/* Reset the counter to avoid sending too many once the first report has been sent.*/
#define FA_ProcessNlmeTxReport(pNlmeNwkTxReport) ResetTxCounters();
#define FA_ProcessEnergyScanCnf(pScanCnf)
#define FA_BuildListFromChannel(channelNumber)
#define FA_SendChangeChannelEvt()
#define FA_SelectChannelAndChange()
#endif

#if gFrequencyAgilityCapability_d
uint8_t FA_Process_Mgmt_NWK_Update_request
(
  zbMgmtNwkUpdateRequest_t  *pMessageComingIn,  /* IN: The package with the request information. */
  void                      *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be build. */
  zbNwkAddr_t               aSrcAddrr
);
#else
#define FA_Process_Mgmt_NWK_Update_request(pMessageComingIn,pMessageGoingOut, aSrcAddrr) 0
#endif
/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif  /*  _ZdpFrequencyAgility_INTERFACE_H_  */
