/***************************************************************************
* This is the header file for the ApsVariables
*
* (c) Copyright 2013, Freescale, Inc. All rights reserved.
*
* Freescale Semiconductor Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/
#ifndef _APS_INTERNAL_VARIABLES_H_
#define _APS_INTERNAL_VARIABLES_H_

#ifdef __cplusplus
    extern "C" {
#endif
/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

#if(gInstantiableStackEnabled_d == 0)
  /* APS */
  #define ZbApsPrivateData(val)          val
  #define ZbApsPublicData(val)           val
#else
  /* APS */
  #define ZbApsPrivateData(val)            pZbApsPrivateData->val
  #define ZbApsPublicData(val)             pZbApsPublicData->val
#endif

/*****************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
/* Fragmentation Rx state machine variables. all pointers are NULL if unused */
typedef struct apsRxTable_tag
{
  apsRxState_t        state;          /* what state are we in? init*/
  uint8_t             keyUsed;        /* remember key used for security init*/
  uint8_t             blockNumber;    /* the block # currently being processed */
  zbApsCounter_t      apsCounter;     /* counter ID of this transaction */
  zbNwkAddr_t         aSrcAddr;       /* source addr for this transaction */  
  zbExtHdr_t          iAckBitField;   /* receive ACK bits */
  apsdeToAfMessage_t *pDataIndMsg;    /* ptr to data indication we're building */
  nldeApsMessage_t   *pMsg;           /* pointer to new message from NWK layer */
  nldeDataIndication_t *pOrgNldeReq;  /* pointer to original nlde-data.req from NWK layer, in packed form, for ACKs */
} apsRxTable_t;

/* used for ZCP negative testing R20 */
typedef struct overridenApsFrameEncryption_tag
{
  uint8_t overrideUpdateDeviceEncryption     :1;
  uint8_t updateDeviceEncryption             :1;
  uint8_t overrideSecUpdateDeviceProcessing  :1;    
  uint8_t reserved                           :5;
} overridenApsFrameEncryption_t;


/* Aps module internal used ram global structure */
#if(gInstantiableStackEnabled_d == 1)
typedef struct zbApsPrivateData_tag
{
#endif
  /* Initialized  part of structure */
  
   /* ApsBindManagement*/
EXTERN  zbNwkAddr_t    gaNotCommissioned;
  
  /*ApsDataPrimitiveProcessor*/
  /* fragmented reception variable */
  /* Used to store the current rx fragmented packet in process*/
EXTERN  uint8_t iCurrentRxFragTableEntryIndex; 
EXTERN  bool_t gApsProfileCheck_c;
  
  /* ApsDataQueue */
EXTERN  bool_t gApsOk2Tx;
  /*********************************************************************/
  /* Uninitialized  part of structure */ 

  /* ApsDataAckTx*/
  /* used to test Fragmentation, by "pre-ACK" */
EXTERN  uint8_t         giFragmentedPreAcked;
EXTERN  zbApsCounter_t  giBlockCounter;
EXTERN  uint8_t         giApsMaxWindowSize;
EXTERN  uint8_t         miThisWindowStart;
  
  /*ApsDataPrimitiveProcessor*/
  /* Queue to queue the Data request from the upper layer */
EXTERN  anchor_t gApsdeQueue;
  /* Queue to queue the Data confirmation and indication from the NWK layer */
EXTERN  anchor_t gNldeQueue;
  /* fragmented reception variable */
  #if gEndDevCapability_d || gComboDeviceCapability_d
EXTERN  uint16_t gApsPrevToFragmentationPollRate;
  #endif
EXTERN  apsRxTable_t  gApsRxEntry;
  /* this array keeps track of messages already heard by APS */
EXTERN  index_t giApsHeardItCount; 
  
  /* ApsDataQueue */
EXTERN  bool_t IsThereAError;
  /* Mask to check which state machine has a pending event */
EXTERN  uint32_t gApsTxStateMachineIndex;
  
  /* ApsMain */
EXTERN  zbTmrTimerID_t gApsMinuteTimerID;
  
  /* Removed from SSP */
  /* used for ZCP negative testing R20 */
EXTERN  overridenApsFrameEncryption_t gOverrideApsFrameEncryptionSettings_c;
EXTERN  uint8_t gUpdateDeviceSecUnsecTransmit_c;
#if(gInstantiableStackEnabled_d == 1)
}zbApsPrivateData_t;
#endif

#if(gInstantiableStackEnabled_d == 1)
/* Aps module public ram global structure */
typedef struct zbApsPublicData_tag
{
#endif  
  /* Initialized  part of structure */
  
  /*********************************************************************/
  /* Uninitialized  part of structure */ 

  /* ApsDataPrimitiveProcessor*/
  /* used to uniquely identify an APS frame - incremented by APS layer */
EXTERN  zbApsCounter_t  giApsCounter;
EXTERN  zbApsConfirmId_t  giApsConfirmId;
  /* ApsMain */
  /* queue for APSME-xxxx security functions (see ZDO_APSME_SAPHandler() */
  #if gStandardSecurity_d || gHighSecurity_d
EXTERN    msgQueue_t mApsmeQueue;
  #endif
#if(gInstantiableStackEnabled_d == 1)
}zbApsPublicData_t;
#endif
/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
#if(gInstantiableStackEnabled_d == 1)
/* pointer used to access instance id variables */
extern zbApsPrivateData_t* pZbApsPrivateData;

/* pointer used to access instance id variables */
extern zbApsPublicData_t* pZbApsPublicData;
#endif
/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

void ApsInitVariables(uint8_t instId);
#ifdef __cplusplus
}
#endif
#endif /* _APS_INTERNAL_VARIABLES_H_ */
