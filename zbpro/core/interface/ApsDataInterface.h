/******************************************************************************
* This file provides interface methods for data service module of APS
*
* Copyright (c) 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*******************************************************************************/
#ifndef _APS_DATA_INTERFACE_H_
#define _APS_DATA_INTERFACE_H_

#ifdef __cplusplus
    extern "C" {
#endif
/* needed for binding type */
#include "ApsMgmtInterface.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/*APS SapHandlers events*/
#define gAF_APSDE_c  (1<<0)
#define gNLDE_APS_c  (1<<1)
#define gZDO_APSME_c (1<<2)
#define gApsTxStateMachine_c (1<<3)


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

extern anchor_t gApsdeQueue;
extern anchor_t gNldeQueue;





/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* SAP handler to for APSDE-DATA.request */  
uint8_t AF_APSDE_SapHandler(afToApsdeMessage_t *pMsg);
  
/* SAP handler for APSDE-DATA.confirm and APSDE-DATA.indication */
uint8_t NLDE_APS_SapHandler(nldeApsMessage_t *pMsg);
      
/* actually processes the APSDE-DATA.request */
zbStatus_t APSDE_DATA_request(afToApsdeMessage_t * pMsg);

/* actually processes the APSDE-DATA.indication and APSDE-DATA.confirm */
void APS_ProcessNldeMessage(nldeApsMessage_t* pMsg);

/* called when an ACK times out, 
  or when time to send to the next binding entry on indirect data requests */
void APS_TxStateMachine(void);

/* initialize the retry */
void APS_ResetTxTable(void);

/* resets the fragmentation rx table and timers */
void APS_ResetRxFragTable(void);

/* resets the fragmentation RxState machine */
void APS_FreeRxStateMachine(index_t iIndexRxTable);

/* free the entire linked list of Tx fragments, except the head. Use FragHdr as input. */
void FreeSubsequentFragments
  (
  zbTxFragmentedHdr_t *pHead  /* IN: header for linked list of fragments */
  );

/* free the entire linked list of Tx fragments, except the head. Use dataReq as input */
void FreeTxFragments
  (
  zbApsdeDataReq_t *pDataReq /* IN: data request with linked list at end */
  );

/* free linked list of Rx fragments, */  
void FreeRxFragments
  (
  zbRxFragmentedHdr_t * pFirstBuffer
  );

uint16_t GetLengthOfAllFragments
  (
  zbTxFragmentedHdr_t *pHead
  );

uint16_t AF_GetTotalLength
  (
  zbApsdeDataIndication_t *pIndication
  );

void AllocateTxFragments(zbTxFragmentedHdr_t *pHead, uint16_t length);
#ifdef __cplusplus
}
#endif
#endif /* _APS_DATA_INTERFACE_H_ */

