/******************************************************************************
* Ztc access to the NWK/APS Information Base
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#include "EmbeddedTypes.h"

#if gBeeStackIncluded_d
  #include "ZigBee.h"
  #include "BeeStackFunctionality.h"
  #include "BeeStackConfiguration.h"
#endif

#include "ZtcInterface.h"

#if gZtcIncluded_d

#include "FunctionLib.h"
#if gBeeStackIncluded_d
#include "BeeStackInterface.h"
#include "BeeStack_Globals.h"

#include "AfApsInterface.h"
#include "ZdoApsInterface.h"
#include "ApsMgmtInterface.h"
#include "BeeStackUtil.h"
#endif

#include "ZtcPrivate.h"
#include "ZtcClientCommunication.h"
#include "ZtcIBManagement.h"

#include "BeeStackIBManagement.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

/* None */



/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* Retrieve information from the Information Base.
 *
 * On entry
 *  pRequest->iId       ID of the IB entry to get,
 *  pRequest->iIndex    Index of the first element to get, and
 *  pRequest->iEntries  Number of elements to get.
 *  pRequest->pData     Ignored
 *
 * Return
 *  pRequest->iEntries  Actual number of elements returned,
 *  pRequest->pData     Reused as the total data returned, in bytes.
 *
 * Note that this code treats everything as an array. Scalar values are
 * considered arrays that have a maximum of one element.
 */
void ZtcMsgGetIBReqFunc(void) 
{
  zbIBRequest_t ibReq = zbIBApsmeGetReq_c;
  #define DataSizeFieldLengthInBytes 2 

  index_t totalDataSize;
  zbIBReqParams_t *pResponse = ((zbIBReqParams_t *)&gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)]);
  zbIBReqParams_t* pRequest  = (zbIBReqParams_t *)gpZtcPacketFromClient->structured.payload;
  
        
  pRequest->pData = ((uint8_t *) &pResponse->pData) + DataSizeFieldLengthInBytes;  
    
  /* points to the "To" client packet. They don't overlap. */
  FLib_MemCpy((uint8_t *) pResponse, (void *) pRequest, sizeof(zbIBReqParams_t));
  
  /* Check if this is a Nlme or Apsme request. */
  if(gpZtcPacketFromClient->headerAndStatus.header.msgType == mZtcMsgNlmeGetIBReq_c)
  {
    ibReq = zbIBNlmeGetReq_c;
  }
  
  /* get attrib value */
  if (gZbSuccess_c != BeeStack_GetSetAttrValue(ibReq,ztcBeeStackInstId,pRequest)) 
  {
    ZtcError(gZtcUnknownIBIdentifier_c);
    return;
  }
  
  totalDataSize = (index_t)(pRequest->iEntries * pRequest->iEntrySize);
  if (totalDataSize > sizeof(gpZtcPacketFromClient->structured.payload) - sizeof(getIBRequest_t)) 
  {
    gZtcPacketToClient.structured.payload[0] = gZtcTooBig_c;
    return;
  }
  
  pResponse->iEntries = pRequest->iEntries;
  pResponse->iEntrySize = pRequest->iEntrySize;
  
  /* Note that pRequest points to the "From" client packet, and pResponse */
  *((uint16_t *) &pResponse->pData) = totalDataSize;  
  gZtcPacketToClient.structured.header.len = sizeof(getIBRequest_t) + totalDataSize + sizeof(clientPacketStatus_t);
  
  #undef DataSizeFieldLengthInBytes /*  As this is not final, get rid of it ASAP */
}/* ZtcMsgGetIBReqFunc() */

/*****************************************************************************/

/* Store information to the Information Base.
 *
 * On entry
 *  pRequest->iId       ID of the IB entry to set,
 *  pRequest->iIndex    Index of the first element to set, and
 *  pRequest->iEntries  Number of elements to set.
 *  pRequest->pData     Not actually a pointer; replaced by the start of the
 *                          data.
 *
 * Note that this code treats everything as an array. Scalar values are
 * considered arrays that have a maximum of one element.
 */
void ZtcMsgSetIBReqFunc(void) 
{
  zbIBRequest_t ibReq = zbIBApsmeSetReq_c;
  zbStatus_t status;
  zbIBReqParams_t  ibRequest;
  zbIBReqParams_t* pRequest  = (zbIBReqParams_t *)gpZtcPacketFromClient->structured.payload;
  
  FLib_MemCpy((void*)&ibRequest,(void*)pRequest,sizeof(zbIBReqParams_t));
  
  ibRequest.pData = (void*)&pRequest->pData;
  /* Check if this is a Nlme or Apsme request. */
  if(gpZtcPacketFromClient->headerAndStatus.header.msgType == mZtcMsgNlmeSetIBReq_c)
  {
    ibReq = zbIBNlmeSetReq_c;
  }
  /* set attrib value */
  status = BeeStack_GetSetAttrValue(ibReq,ztcBeeStackInstId,&ibRequest);
  
  switch(status)
  {
    case gZbUnsupportedAttribute_c:
      ZtcError(gZtcUnknownIBIdentifier_c);
      return;
    case gZbNotPermitted_c:
      gZtcPacketToClient.structured.payload[0] = gZtcReadOnly_c;
      return;
    case gZbInvalidParameter_c:
      gZtcPacketToClient.structured.payload[0] = gZtcTooBig_c;
      return;  
    default:
     break; 
  }
  
  if (pRequest->attId == gNwkPanId_c)
    (void)SetPibAttributeValue(gMPibPanId_c, (void *)&pRequest->pData);
  else if (pRequest->attId == gNwkShortAddress_c)
    (void)SetPibAttributeValue(gMPibShortAddress_c, (void *)&pRequest->pData);
} /* ZtcMsgSetIBReqFunc() */

/*****************************************************************************
 This Function store  to the Information Base in BlackBox and Host.
 *****************************************************************************/
void ZtcMsgSetIBReq(uint8_t msgType, uint8_t attrId, uint8_t index, uint8_t *pValue) 
{

(void)msgType;
(void)attrId;
(void)index;
(void)pValue;
#if 0  
  
  index_t i;
  uint8_t entrySize;
  
  /* Search attribute in table */
  for (i = 0; i < NumberOfElements(maZtcIBData); ++i) {
    if (attrId == maZtcIBData[i].id) {
      break;
    }
  } 
  
  if (i == NumberOfElements(maZtcIBData)) {
    return;
  }

  entrySize = maZtcIBData[i].entrySize;
  if((maZtcIBData[i].access == mZtcIBRWUseFunc))
  {
    ztcIBAccessTbl_t  const *pZtcIBAccessFuncTbl = NULL;
    uint8_t j;
      /* Find the entry in maZtcIBAccessFuncTbl */
    for (j = 0; j < NumberOfElements(maZtcIBAccessFuncTbl); ++j) {
      if (maZtcIBData[i].id == maZtcIBAccessFuncTbl[j].id) {
        pZtcIBAccessFuncTbl = &maZtcIBAccessFuncTbl[j];
        break;
      }
    }
    if (pZtcIBAccessFuncTbl)
     (void)(pZtcIBAccessFuncTbl)->pSetTblEntry(index, pValue);
    
  }
  else
  {
      /* Set the IB attribute locally and sent it over the UART (to BlackBox) */
    FLib_MemCpy((uint8_t *) maZtcIBData[i].pTable + (uint16_t) (entrySize * index),
                pValue,
                entrySize);
  }
  gZtcPacketToClient.structured.header.opcodeGroup    = gZtcReqOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType        = msgType;
  gZtcPacketToClient.structured.payload[0] = attrId;
  gZtcPacketToClient.structured.payload[1] = index;
  gZtcPacketToClient.structured.payload[2] = 0x01; /* Update one entry each time */
  gZtcPacketToClient.structured.payload[3] = entrySize;
  FLib_MemCpy(&gZtcPacketToClient.structured.payload[4],
              pValue,
              entrySize);
  gZtcPacketToClient.structured.header.len = 4 + entrySize;
  ZtcComm_WritePacketToClient((gZtcPacketToClient.structured.header.len +
                         sizeof(gZtcPacketToClient.structured.header)));
  
#endif 
}
#endif /* #if gZtcIncluded_d */
