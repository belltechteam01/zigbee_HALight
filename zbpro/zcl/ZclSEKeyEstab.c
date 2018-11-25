/*! @file	ZclSEKeyEstab.c
 *
 * @brief	This source file describes specific functionality implemented
 *			for the Key Establishment cluster.
 *
 * @copyright Copyright(c) 2013, Freescale, Inc. All rights reserved.
 *
 * @license	Redistribution and use in source and binary forms, with or without modification,
 *			are permitted provided that the following conditions are met:
 *
 *			o Redistributions of source code must retain the above copyright notice, this list
 *			of conditions and the following disclaimer.
 *
 *			o Redistributions in binary form must reproduce the above copyright notice, this
 *   		list of conditions and the following disclaimer in the documentation and/or
 *   		other materials provided with the distribution.
 *
 * 			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   		contributors may be used to endorse or promote products derived from this
 *   		software without specific prior written permission.
 *
 * 			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * 			ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * 			WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * 			ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * 			(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 			LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * 			ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * 			(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * 			SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "EmbeddedTypes.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "ZdoApsInterface.h"
#include "ApsMgmtInterface.h"

#include "ZclFoundation.h"
#include "eccapi.h"
#include "SEProfile.h"
#include "ZclSE.h"
#include "ZclSEKeyEstab.h"


#include "ZDOVariables.h"
#include "ZdoApsInterface.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/* key estab state machine states */
#define KeyEstab_InitState_c             0x00
#define KeyEstab_KeyEstabInitatedState_c 0x01
#define KeyEstab_EphemeralState_c        0x02
#define KeyEstab_ConfirmDataState_c      0x03

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

static zbStatus_t ZCL_SendInitiatKeyEstReq(IdentifyCert_t *Cert, zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr);
static zbStatus_t ZCL_SendInitiatKeyEstRsp(IdentifyCert_t *Cert,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid);
static zbStatus_t ZCL_SendEphemeralDataReq(uint8_t *EphemeralPubKey,  zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SendEphemeralDataRsp(uint8_t *EphemeralPubKey,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid);
static zbStatus_t ZCL_SendConfirmKeyDataReq(uint8_t *MACU,  zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SendConfirmKeyDataRsp(uint8_t *MACV,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid);
static zbStatus_t ZCL_SendTerminateKeyEstabServerReq(uint8_t Status,uint8_t WaitCode,  zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SendTerminateKeyEstabClientReq(uint8_t Status, uint8_t WaitCode,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid);
static void GenerateKeyMACUMACVResponder(void);
static void GenerateKeyMACUMACVInitiator(void);
static void KeyEstabStopTimeout(void);
static void ResetKeyEstabTimeout(void);
static void InitAndStartKeyEstabTimeout(void);
static bool_t KeyEstabAllocateMemory(void);
static bool_t InitKeyEstabStateMachine(void);
static zbStatus_t ZCL_TerminateKeyEstabServer(zbApsdeDataIndication_t *pIndication, uint8_t ErrorCode,bool_t ResetStateMachine);
static zbStatus_t ZCL_TerminateKeyEstabClient(zbApsdeDataIndication_t *pIndication, uint8_t ErrorCode,bool_t ResetStateMachine);
static bool_t ValidateKeyEstabMessage(zbApsdeDataIndication_t *pIndication, IdentifyCert_t *pCert) ;
static void KeyEstabFreeMemory(void);
static void KeyEstabTimeoutCallback(zbTmrTimerID_t tmrid);
#if gEndDevCapability_d || gComboDeviceCapability_d
static void KeyEstabRestorePollRateCallBack(zbTmrTimerID_t tmrid);
#endif

#if gZclKeyEstabDebugTimer_d
static void KeyEstabDebugTimerCallBack(zbTmrTimerID_t tmrID);
#endif
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

typedef struct KeyEstab_KeyData_tag
{
  uint8_t ephemeralPrivateKey[gZclCmdKeyEstab_PrivateKeySize_c];
  uint8_t u_ephemeralPublicKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
  uint8_t v_ephemeralPublicKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
  uint8_t SharedSecret[gZclCmdKeyEstab_SharedSecretSize_c];
  uint8_t MACU[gZclCmdKeyEstab_AesMMOHashSize_c]; 
  uint8_t MACV[gZclCmdKeyEstab_AesMMOHashSize_c]; 
  uint8_t mackey[gZclCmdKeyEstab_SharedSecretSize_c]; /*only 16 bytes is used, but must be same length as Shared secret*/  
} KeyEstab_KeyData_t;

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

static uint8_t KeyEstabState = KeyEstab_InitState_c;
static IdentifyCert_t *pOppositeImplicitCert = NULL;
static KeyEstab_KeyData_t *KeyData = NULL;
static zbTmrTimerID_t KeyEstabTimerId = gTmrInvalidTimerID_c;
#if gEndDevCapability_d || gComboDeviceCapability_d
static zbTmrTimerID_t KeyEstabPollTimerId = gTmrInvalidTimerID_c;
#endif
static uint8_t EphemeralDataGenerateTime;
static uint8_t ConfirmKeyGenerateTime;

#if gZclKeyEstabDebugTimer_d
static zbTmrTimerID_t KeyEstabDebugTimerId = gTmrInvalidTimerID_c;
static zbApsdeDataIndication_t *pKeyEstabDataIndication  = NULL;
#endif


const zclAttrDef_t gaZclKeyEstabServerAttrDef[] = {
  { gZclAttrKeyEstabSecSuite_c, gZclDataTypeEnum16_c,  gZclAttrFlagsInRAMRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zclAttrKeyEstabAttr_t,SecuritySuite)}
};

const zclAttrDef_t gaZclKeyEstabClientAttrDef[] = {
  { gZclAttrKeyEstabSecSuite_c, gZclDataTypeEnum16_c,  gZclAttrFlagsInRAMClientRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zclAttrKeyEstabAttr_t,SecuritySuite)}
};

const zclAttrSet_t gaZclKeyEstabAttrSet[] = {
  {gZclAttrKeyEstabInfoSet_c, (void *)&gaZclKeyEstabClientAttrDef, NumberOfElements(gaZclKeyEstabClientAttrDef)},
  {gZclAttrKeyEstabInfoSet_c, (void *)&gaZclKeyEstabServerAttrDef, NumberOfElements(gaZclKeyEstabServerAttrDef)}
};

const zclAttrSetList_t gZclKeyEstabAttrSetList = {
  NumberOfElements(gaZclKeyEstabAttrSet),
  gaZclKeyEstabAttrSet
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		zbStatus_t ZCL_KeyEstabClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Key Establishment Cluster server, 
 *			only one session at a time is supported.
 *
 */
zbStatus_t ZCL_KeyEstabClusterServer
(
zbApsdeDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zclTSQ_t TransactionId;
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;

  (void) pDev; 
  pFrame = (void *)pIndication->pAsdu;
  Cmd = pFrame->command;
  
  /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;   
  
  TransactionId = pFrame->transactionId;
  
  switch(Cmd)
  {   
  case gZclCmdKeyEstab_InitKeyEstabReq_c:
    {
      ZclCmdKeyEstab_InitKeyEstabReq_t *pReq;
      IdentifyCert_t *pCert;
      pReq = (ZclCmdKeyEstab_InitKeyEstabReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));         
      pCert = (IdentifyCert_t *) pReq->IdentityIDU;
      if (KeyEstabState == KeyEstab_InitState_c)
      {
        // check whether request orginate from coordiantor, if device is not a coordinator.
        if (((NlmeGetRequest(gDevType_c) != gCoordinator_c) && (Cmp2BytesToZero(pIndication->aSrcAddr))) ||
            (NlmeGetRequest(gDevType_c) == gCoordinator_c))
        {
          // certificate validation function
          if(ValidateKeyEstabMessage(pIndication,pCert))
          {
            // validate whether the securitysuite is the correct type
            if(IsEqual2BytesInt(pReq->SecuritySuite, gKeyEstabSuite_CBKE_ECMQV_c))
            {
              // validate whether issuer ID is correct:
              if(FLib_MemCmp(pCert->Issuer, (void *)CertAuthIssuerID, sizeof(CertAuthIssuerID)))
              { 
                if (KeyEstabAllocateMemory()) {
                  FLib_MemCpy((uint8_t*) pOppositeImplicitCert,(void *)pCert, sizeof(IdentifyCert_t));
                  // Issuer ID is correct, send response back.
                  EphemeralDataGenerateTime = pReq->EphemeralDataGenerateTime;
                  ConfirmKeyGenerateTime    = pReq->ConfirmKeyGenerateTime;
                  InitAndStartKeyEstabTimeout();
#if !gZclKeyEstabDebugInhibitInitRsp_d                  
                  status = ZCL_SendInitiatKeyEstRsp(&DeviceImplicitCert, pIndication,TransactionId);
#endif                  
                  KeyEstabState = KeyEstab_EphemeralState_c;
                }
                else
                { // Memory coult not be allocated.
                  status = ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermNoResources_c, TRUE);
                }
              }
              else
              { // Issuer ID was not the correct, send terminate message
                status = ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermUnknownIssuer_c,TRUE);
              }          
            }
            else
            {
              //only CBKE-ECMQV supported
              status = ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermUnsupportedSuite_c,FALSE);          
            }
          }
          else
          {
            //invalid InitKeyEstabReq/certificate
            status = ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermBadMessage_c, TRUE);
          }
        }
        else
        {
          // discard, message when initate message is not from coordinator. 
          status = ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermNoResources_c,TRUE);    
        }               
      }
      else
      {
        //Device is in the middle of key establishment with another device, send terminate message with status: no resources.
        status = ZCL_SendTerminateKeyEstabClientReq(gZclCmdKeyEstab_TermNoResources_c, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
      }
    }
    break;      
  case gZclCmdKeyEstab_EphemeralDataReq_c:
    {
      
      if (KeyEstabState == KeyEstab_EphemeralState_c)
      {
        uint8_t aNwkAddrLocalCpy[2];
        zbIeeeAddr_t clientIeee;
        FLib_MemCpyReverseOrder(clientIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
        //check if the originator of the ephemeral data request is the originator of the init key estab req
        if(IsEqual2Bytes(APS_GetNwkAddress(clientIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr))
        {
          ZclCmdKeyEstab_EphemeralDataReq_t *pReq;
      
          pReq = (ZclCmdKeyEstab_EphemeralDataReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));         
          FLib_MemCpy(KeyData->u_ephemeralPublicKey,pReq->EphemeralDataQEU, gZclCmdKeyEstab_CompressedPubKeySize_c);
          if (ZSE_ECCGenerateKey(KeyData->ephemeralPrivateKey, KeyData->v_ephemeralPublicKey, ECC_GetRandomDataFunc, NULL, 0) == 0x00) // 0x00 = MCE_SUCCESS
          {
            ResetKeyEstabTimeout();
          }
          // Send response:
          // Generate keybits
          if(ZSE_ECCKeyBitGenerate(DevicePrivateKey, KeyData->ephemeralPrivateKey, KeyData->v_ephemeralPublicKey,
                                   (uint8_t *)pOppositeImplicitCert, KeyData->u_ephemeralPublicKey, (uint8_t *)CertAuthPubKey,KeyData->SharedSecret, ECC_HashFunc, NULL, 0)==0x00)
          {
            GenerateKeyMACUMACVResponder();
#if gZclKeyEstabDebugTimer_d
            KeyEstabDebugTimerId = ZbTMR_AllocateTimer();
            pKeyEstabDataIndication = MSG_Alloc(sizeof(ZclCmdKeyEstab_EphemeralDataReq_t));
            FLib_MemCpy(pKeyEstabDataIndication, pIndication, sizeof(zbApsdeDataIndication_t));
            ZbTMR_StartTimer(KeyEstabDebugTimerId, gTmrSingleShotTimer_c, gZclKeyEstabDebugTimeInterval_d, KeyEstabDebugTimerCallBack);   
#else

            KeyEstabState = KeyEstab_ConfirmDataState_c;
            status = ZCL_SendEphemeralDataRsp(KeyData->v_ephemeralPublicKey, pIndication,TransactionId);
#endif            
          }
          else
          {
            status = ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermBadMessage_c, TRUE);
          }
        }
        else
        {
          status = ZCL_SendTerminateKeyEstabClientReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
        }
      }
      else
      { // command is out of sequence... send terminate message
        status = ZCL_SendTerminateKeyEstabClientReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
      }
    }
    break;      
  case gZclCmdKeyEstab_ConfirmKeyDataReq_c:
    if (KeyEstabState == KeyEstab_ConfirmDataState_c)
    {
      uint8_t aNwkAddrLocalCpy[2];
      zbIeeeAddr_t clientIeee;
      FLib_MemCpyReverseOrder(clientIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
      //check if the originator of the confirm key data request is the originator of the init key estab req
      if(IsEqual2Bytes(APS_GetNwkAddress(clientIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr))
      {
        ZclCmdKeyEstab_ConfirmKeyDataReq_t *pReq;
        pReq = (ZclCmdKeyEstab_ConfirmKeyDataReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));         
        
        if (FLib_MemCmp(pReq->SecureMsgAuthCodeMACU, KeyData->MACU, gZclCmdKeyEstab_AesMMOHashSize_c))
        {
          uint8_t keytype = gApplicationLinkKey_c;
          zbIeeeAddr_t addr;
          KeyEstabStopTimeout();
#if gZclKeyEstabDebugTimer_d
          KeyEstabDebugTimerId = ZbTMR_AllocateTimer();
          pKeyEstabDataIndication = MSG_Alloc(sizeof(ZclKeyEstab_ConfirmKeyDataRsp_t));
          FLib_MemCpy(pKeyEstabDataIndication, pIndication, sizeof(zbApsdeDataIndication_t));
          ZbTMR_StartTimer(KeyEstabDebugTimerId, gTmrSingleShotTimer_c, gZclKeyEstabDebugTimeInterval_d, KeyEstabDebugTimerCallBack);   
#else          
          status = ZCL_SendConfirmKeyDataRsp(KeyData->MACV,pIndication,TransactionId);
          KeyEstabState = KeyEstab_InitState_c;
#endif          
          // call BeeAppDeviceJoined to tell that device has been registered.
          // install new key:
          // Ieee address in certificate is Big endian, convert to little endian before adding it.
          Copy8Bytes(addr,pOppositeImplicitCert->Subject);
          Swap8Bytes(addr);                
          if (IsEqual8Bytes(addr, ApsmeGetRequest(gApsTrustCenterAddress_c)))          
          {
            keytype = gTrustCenterLinkKey_c;
          }        
          APS_RegisterLinkKeyData(addr,keytype,(uint8_t *) KeyData->mackey);        
          BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabSuccesful_c, 0, 0, addr);
          ZCL_SetKeyEstabComplete(pIndication->srcEndPoint, addr);
#if !gZclKeyEstabDebugTimer_d          
          KeyEstabFreeMemory();
#endif          
        }
        else
        {
#if gZclKeyEstabDebugBypassMACCheck_d      
          status = ZCL_SendConfirmKeyDataRsp(KeyData->MACV,pIndication,TransactionId);
#else          
          // call BeeAppDeviceJoined to tell that a MACU failed to validate.
          status = ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermBadKeyConfirm_c, TRUE);
#endif          
        }
      }
      else
      {
        status = ZCL_SendTerminateKeyEstabClientReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
      }
    }
    else 
    {  // command is out of sequence... send terminate message
      status = ZCL_SendTerminateKeyEstabClientReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
    }
    break;      
  case gZclCmdKeyEstab_TerminateKeyEstabServer_c:
    {
      uint8_t aNwkAddrLocalCpy[2];
      zbIeeeAddr_t clientIeee;
      FLib_MemCpyReverseOrder(clientIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
      //accept terminate key estab server only from the device involved in key estab
      if(IsEqual2Bytes(APS_GetNwkAddress(clientIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr))
      {
        // call BeeAppDeviceJoined to tell that device failed to register.
        KeyEstabState = KeyEstab_InitState_c;
        KeyEstabFreeMemory();
        BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabFailed_c, 0, 0, pIndication);  
      }
    }
    break;          
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
  }
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_KeyEstabClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Key Establishment Cluster client, 
 *			only one session at a time is supported.
 *
 */
zbStatus_t ZCL_KeyEstabClusterClient
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;

  (void) pDev; 
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  pFrame = (void *)pIndication->pAsdu;
  
  /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;   

  switch(Cmd)
  {   
  case gZclCmdKeyEstab_InitKeyEstabRsp_c:
    {
      ZclCmdKeyEstab_InitKeyEstabRsp_t *pReq;
      IdentifyCert_t *pCert;
      pReq = (ZclCmdKeyEstab_InitKeyEstabRsp_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));         
      pCert = (IdentifyCert_t *) pReq->IdentityIDV;
      if(KeyEstabState == KeyEstab_KeyEstabInitatedState_c)
      {
        if (ValidateKeyEstabMessage(pIndication,pCert))
        {
          // validate whether issuer ID is correct:
          if(FLib_MemCmp(pCert->Issuer, (void *)CertAuthIssuerID, sizeof(CertAuthIssuerID)) == TRUE)         
          {           
            FLib_MemCpy((void *)pOppositeImplicitCert,(void *)pCert, sizeof(IdentifyCert_t));          
            EphemeralDataGenerateTime = pReq->EphemeralDataGenerateTime;
            ConfirmKeyGenerateTime    = pReq->ConfirmKeyGenerateTime;
            // Issuer ID is correct, send ephemerial Data request back.
            if(ZSE_ECCGenerateKey(KeyData->ephemeralPrivateKey, KeyData->u_ephemeralPublicKey, ECC_GetRandomDataFunc, NULL, 0) == 0x00)//0x00 = MCE_SUCCESS
            {
              ResetKeyEstabTimeout();
              
#if gZclKeyEstabDebugTimer_d
              KeyEstabDebugTimerId = ZbTMR_AllocateTimer();
              pKeyEstabDataIndication = MSG_Alloc(sizeof(ZclCmdKeyEstab_InitKeyEstabRsp_t));
              FLib_MemCpy(pKeyEstabDataIndication, pIndication, sizeof(zbApsdeDataIndication_t));
              ZbTMR_StartTimer(KeyEstabDebugTimerId, gTmrSingleShotTimer_c, gZclKeyEstabDebugTimeInterval_d, KeyEstabDebugTimerCallBack);   
#else
              status = ZCL_SendEphemeralDataReq(KeyData->u_ephemeralPublicKey,pIndication);
              KeyEstabState = KeyEstab_EphemeralState_c;
#endif              
            }                            
          }
          else
          {
            // send terminate key estab. with UNKNOWN_ISSUER status           
            status = ZCL_TerminateKeyEstabServer(pIndication, gZclCmdKeyEstab_TermUnknownIssuer_c, TRUE);                 
          }                  
        }
        else
        {
          // cluster is not participating in key establishment
          // send terminate key estab. with BAD_MESSAGE status
          status = ZCL_TerminateKeyEstabServer(pIndication, gZclCmdKeyEstab_TermBadMessage_c, TRUE);                 
        }
      }
      else
      {
        // command is out of sequence... send terminate message
        status = ZCL_SendTerminateKeyEstabServerReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication);
      }
    }
    break;      
  case gZclCmdKeyEstab_EphemeralDataRsp_c:
    {
      if (KeyEstabState == KeyEstab_EphemeralState_c)
      {
        uint8_t aNwkAddrLocalCpy[2];
        zbIeeeAddr_t serverIeee;
        FLib_MemCpyReverseOrder(serverIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
        //check that the originator of the ephemeral data response is correct
        if(IsEqual2Bytes(APS_GetNwkAddress(serverIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr))
        {
          ZclCmdKeyEstab_EphemeralDataRsp_t *pReq;
          pReq = (ZclCmdKeyEstab_EphemeralDataRsp_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));
          FLib_MemCpy(KeyData->v_ephemeralPublicKey,pReq->EphemeralDataQEV, gZclCmdKeyEstab_CompressedPubKeySize_c);
          // Generate keybits 
          if (ZSE_ECCKeyBitGenerate(DevicePrivateKey, KeyData->ephemeralPrivateKey, KeyData->u_ephemeralPublicKey,
                                    (uint8_t *)pOppositeImplicitCert, KeyData->v_ephemeralPublicKey, (uint8_t *)CertAuthPubKey,KeyData->SharedSecret, ECC_HashFunc, NULL, 0)==0x00)
          {
            // Generate MAC key and MAC data  
            GenerateKeyMACUMACVInitiator();        
            ResetKeyEstabTimeout();
            
#if gZclKeyEstabDebugTimer_d
            KeyEstabDebugTimerId = ZbTMR_AllocateTimer();
            pKeyEstabDataIndication = MSG_Alloc(sizeof(ZclCmdKeyEstab_EphemeralDataRsp_t));
            FLib_MemCpy(pKeyEstabDataIndication, pIndication, sizeof(zbApsdeDataIndication_t));
            ZbTMR_StartTimer(KeyEstabDebugTimerId, gTmrSingleShotTimer_c, gZclKeyEstabDebugTimeInterval_d, KeyEstabDebugTimerCallBack);   
#else
            status = ZCL_SendConfirmKeyDataReq(KeyData->MACU,pIndication);        
            KeyEstabState = KeyEstab_ConfirmDataState_c;
#endif            
          }
          else
          {
            status = ZCL_TerminateKeyEstabServer(pIndication, gZclCmdKeyEstab_TermBadMessage_c, TRUE);
          }
        }
        else
        {
          //Originator of the ephemeral data response is different than the originator of the init key estab rsp
          status = ZCL_SendTerminateKeyEstabServerReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication);
        }
      }
      else
      {
        // command is out of sequence... send terminate message
        status = ZCL_TerminateKeyEstabServer(pIndication, gZclCmdKeyEstab_TermBadMessage_c, TRUE);
      }
    }
    break;      
  case gZclCmdKeyEstab_ConfirmKeyDataRsp_c:
    {
      ZclCmdKeyEstab_ConfirmKeyDataRsp_t *pReq;
      pReq = (ZclCmdKeyEstab_ConfirmKeyDataRsp_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));
      if (KeyEstabState == KeyEstab_ConfirmDataState_c)
      {
        uint8_t aNwkAddrLocalCpy[2];
        zbIeeeAddr_t serverIeee;
        FLib_MemCpyReverseOrder(serverIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
        //check that the originator of the confirm key data response is correct
        if(IsEqual2Bytes(APS_GetNwkAddress(serverIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr))
        {
          if (FLib_MemCmp(pReq->SecureMsgAuthCodeMACV, KeyData->MACV,gZclCmdKeyEstab_AesMMOHashSize_c))
          {
            // Call BeeAppDeviceUpdate and tell that device has been registered successfully
            // install key:
            // Ieee address in certificate is Big endian, convert to little endian before adding it.
            uint8_t keytype = gApplicationLinkKey_c;
            zbIeeeAddr_t addr;
            KeyEstabStopTimeout();
            Copy8Bytes(addr,pOppositeImplicitCert->Subject);
            Swap8Bytes(addr);
            if (IsEqual8Bytes(addr, ApsmeGetRequest(gApsTrustCenterAddress_c)))          
            {
              keytype = gTrustCenterLinkKey_c;
            }        
            APS_RegisterLinkKeyData(addr,keytype,(uint8_t *) KeyData->mackey);
            BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabSuccesful_c, 0, 0, addr);  
            ZCL_SetKeyEstabComplete(pIndication->srcEndPoint, addr);
            TS_SendEvent(gSETaskID, gZclEvtKeyEstablishmentSuccess_c);
            KeyEstabFreeMemory();
            KeyEstabState = KeyEstab_InitState_c;
          }
          else
          {
            status = ZCL_TerminateKeyEstabServer(pIndication, gZclCmdKeyEstab_TermBadKeyConfirm_c, TRUE);
          }
        }
        else
        {
          //Originator of the confirm key data response different than the originator of the init key estab rsp
          status = ZCL_SendTerminateKeyEstabServerReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication);
        }
      }
      else
      {
        // command is out of sequence... send terminate message
        status = ZCL_SendTerminateKeyEstabServerReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication);
      }
    }
    break;      
  case gZclCmdKeyEstab_TerminateKeyEstabClient_c:
    {
      ZclCmdKeyEstab_TerminateKeyEstabServer_t *pReq = (ZclCmdKeyEstab_TerminateKeyEstabServer_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));         
      uint8_t aNwkAddrLocalCpy[2];
      zbIeeeAddr_t serverIeee;
      FLib_MemCpyReverseOrder(serverIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
      //accept terminate key estab client only in KeyEstab_KeyEstabInitatedState_c or from the device involved in key estab.
      if((KeyEstabState == KeyEstab_KeyEstabInitatedState_c) || (IsEqual2Bytes(APS_GetNwkAddress(serverIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr)))
      {
#if gEndDevCapability_d || gComboDeviceCapability_d
        ZbTMR_StartSingleShotTimer(KeyEstabPollTimerId, gKeyEstab_EndDevicePollRate_c, KeyEstabRestorePollRateCallBack);
#endif     

        // call BeeAppDeviceUpdate and tell that device failed to register.
        KeyEstabState = KeyEstab_InitState_c;
        KeyEstabFreeMemory();
        BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabFailed_c, 0, 0, pIndication);
         /* Try again this procedure after the specified period */
        ZbTMR_StartSingleShotTimer(KeyEstabTimerId, ZbTmrSeconds(pReq->WaitCode), KeyEstabTimeoutCallback);        
        
        /* 5.4.7.1 If Key Establishment fails with a result of UNKNOWN_ISSUER the device should leave the network */
        if (pReq->StatusCode == gZclCmdKeyEstab_TermUnknownIssuer_c)
        {
          ZDO_SetState(gZdoLeavingState_c);
          ZDO_SendEvent(gZdoEvent_LeaveNetwork_c);
        }
      }
    }
    break;          
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
  }
 return status;
}

/*!
 * @fn 		zbStatus_t zclKeyEstab_InitKeyEstabReq(ZclKeyEstab_InitKeyEstabReq_t *pReq) 
 *
 * @brief	Sends over-the-air an Initiate Key Establishment Request from the client
 *
 */
zbStatus_t zclKeyEstab_InitKeyEstabReq
(
ZclKeyEstab_InitKeyEstabReq_t *pReq
) 
{
 	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendClientReqSeqPassed(gZclCmdKeyEstab_InitKeyEstabReq_c, sizeof(ZclCmdKeyEstab_InitKeyEstabReq_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclKeyEstab_EphemeralDataReq(ZclKeyEstab_EphemeralDataReq_t *pReq) 
 *
 * @brief	Sends over-the-air an Ephemeral Data Request from the client
 *
 */
zbStatus_t zclKeyEstab_EphemeralDataReq
(
ZclKeyEstab_EphemeralDataReq_t *pReq
) 
{
		Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendClientReqSeqPassed(gZclCmdKeyEstab_EphemeralDataReq_c, sizeof(ZclCmdKeyEstab_EphemeralDataReq_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclKeyEstab_ConfirmKeyDataReq(ZclKeyEstab_ConfirmKeyDataReq_t *pReq) 
 *
 * @brief	Sends over-the-air a Confirm Key Data Request from the client
 *
 */
zbStatus_t zclKeyEstab_ConfirmKeyDataReq
(
ZclKeyEstab_ConfirmKeyDataReq_t *pReq
) 
{
  	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendClientReqSeqPassed(gZclCmdKeyEstab_ConfirmKeyDataReq_c, sizeof(ZclCmdKeyEstab_ConfirmKeyDataReq_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclKeyEstab_TerminateKeyEstabServer(ZclKeyEstab_TerminateKeyEstabServer_t *pReq) 
 *
 * @brief	Sends over-the-air a Terminate Key Establishment from the client
 *
 */
zbStatus_t zclKeyEstab_TerminateKeyEstabServer
(
ZclKeyEstab_TerminateKeyEstabServer_t *pReq
) 
{
  	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendClientReqSeqPassed(gZclCmdKeyEstab_TerminateKeyEstabServer_c, sizeof(ZclCmdKeyEstab_TerminateKeyEstabServer_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclKeyEstab_InitKeyEstabRsp(ZclKeyEstab_InitKeyEstabRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an Initiate Key Establishment Response from the server
 *
 */
zbStatus_t zclKeyEstab_InitKeyEstabRsp
(
ZclKeyEstab_InitKeyEstabRsp_t *pReq
) 
{
  	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendServerRspSeqPassed(gZclCmdKeyEstab_InitKeyEstabRsp_c, sizeof(ZclCmdKeyEstab_InitKeyEstabRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclKeyEstab_EphemeralDataRsp(ZclKeyEstab_EphemeralDataRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an Ephemeral Data Response from the server
 *
 */
zbStatus_t zclKeyEstab_EphemeralDataRsp
(
ZclKeyEstab_EphemeralDataRsp_t *pReq
) 
{
  	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendServerRspSeqPassed(gZclCmdKeyEstab_EphemeralDataRsp_c, sizeof(ZclCmdKeyEstab_EphemeralDataRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclKeyEstab_ConfirmKeyDataRsp(ZclKeyEstab_ConfirmKeyDataRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Confirm Key Data Response from the server
 *
 */
zbStatus_t zclKeyEstab_ConfirmKeyDataRsp
(
ZclKeyEstab_ConfirmKeyDataRsp_t *pReq
) 
{ 
  	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendServerRspSeqPassed(gZclCmdKeyEstab_ConfirmKeyDataRsp_c, sizeof(ZclCmdKeyEstab_ConfirmKeyDataRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclKeyEstab_TerminateKeyEstabClient(ZclKeyEstab_TerminateKeyEstabClient_t *pReq) 
 *
 * @brief	Sends over-the-air a Terminate Key Establishment from the server
 *
 */
zbStatus_t zclKeyEstab_TerminateKeyEstabClient
(
ZclKeyEstab_TerminateKeyEstabClient_t *pReq
) 
{
	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendServerRspSeqPassed(gZclCmdKeyEstab_TerminateKeyEstabClient_c, sizeof(ZclCmdKeyEstab_TerminateKeyEstabClient_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclKeyEstab_SetSecurityMaterial(ZclKeyEstab_SetSecurityMaterial_t *pReq) 
 *
 * @brief	Sets the CBKE Security Material (Certificate, Private Key, Public Key)
 *
 */
zbStatus_t zclKeyEstab_SetSecurityMaterial
(
	ZclKeyEstab_SetSecurityMaterial_t *pReq
) 
{
	FLib_MemCpy(&DeviceImplicitCert, &pReq->deviceImplicitCert, sizeof(IdentifyCert_t));
	FLib_MemCpy(&DevicePrivateKey, &pReq->devicePrivateKey, gZclCmdKeyEstab_PrivateKeySize_c * sizeof(uint8_t));
	FLib_MemCpy(&DevicePublicKey, &pReq->devicePublicKey, gZclCmdKeyEstab_CompressedPubKeySize_c * sizeof(uint8_t));
	return gZbSuccess_c;
}

/*!
 * @fn 		bool_t ZCL_InitiateKeyEstab(zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr) 
 *
 * @brief	Initiates the CBKE process with a remote node
 *
 */
bool_t ZCL_InitiateKeyEstab(zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr) {
  // Init key estab. state machine. (key establishment inprogress)
   if (InitKeyEstabStateMachine())  
   {
/* Change the poll rate to the authentication specific value for the ECC procedure*/    
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
    {
      (void)ZDO_NLME_ChangePollRate(gKeyEstab_EndDevicePollRate_c);
    }
#endif      
#if !gZclKeyEstabDebugMode_d
    ZCL_SendInitiatKeyEstReq(&DeviceImplicitCert, DstEndpoint, SrcEndpoint,(uint8_t*) DstAddr);
#else
    (void)DstEndpoint;
    (void)SrcEndpoint;
    (void)DstAddr;
#endif 
    InitAndStartKeyEstabTimeout();
    return TRUE;
   } else {
     return FALSE;   
   }
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************/
/* Free memory used by key estab cluster                                     */
/*****************************************************************************/
static void KeyEstabFreeMemory(void)
{
  if(pOppositeImplicitCert)
  {
    MSG_Free(pOppositeImplicitCert);
    pOppositeImplicitCert = NULL;
  }
  if(KeyData)
  {
    MSG_Free(KeyData);
    KeyData = NULL;
  }
  
}

#if gZclKeyEstabDebugTimer_d
static void KeyEstabDebugTimerCallBack(zbTmrTimerID_t tmrID)
{
  zclCmd_t TransactionId;
  
  (void) tmrID;
  ZbTMR_FreeTimer(tmrID);
  TransactionId = ((zclFrame_t *)pKeyEstabDataIndication->pAsdu)->transactionId; 
  
  switch(KeyEstabState)
  {  
  case KeyEstab_KeyEstabInitatedState_c:
    (void) TransactionId;
    ZCL_SendEphemeralDataReq(KeyData->u_ephemeralPublicKey, pKeyEstabDataIndication);
    KeyEstabState = KeyEstab_EphemeralState_c;
    MSG_Free(pKeyEstabDataIndication);
    break;
  case KeyEstab_EphemeralState_c:
#if gCoordinatorCapability_d
    ZCL_SendEphemeralDataRsp(KeyData->v_ephemeralPublicKey, pKeyEstabDataIndication, TransactionId);
    KeyEstabState = KeyEstab_ConfirmDataState_c;
    MSG_Free(pKeyEstabDataIndication);
#else    
    ZCL_SendConfirmKeyDataReq(KeyData->MACU, pKeyEstabDataIndication);
    KeyEstabState = KeyEstab_ConfirmDataState_c;
    MSG_Free(pKeyEstabDataIndication);
#endif    
    break;
  case KeyEstab_ConfirmDataState_c:
#if gCoordinatorCapability_d    
    ZCL_SendConfirmKeyDataRsp(KeyData->MACV, pKeyEstabDataIndication, TransactionId);
    KeyEstabState = KeyEstab_InitState_c;
    MSG_Free(pKeyEstabDataIndication);
    KeyEstabFreeMemory();
#endif
    break;
  default:
    break;    
  }
}
#endif

/*****************************************************************************/
/* Timout timer callback function                                            */
/* It restores the original polling rate at the end of the CBKE procedure    */
/*****************************************************************************/
#if gEndDevCapability_d || gComboDeviceCapability_d
static void KeyEstabRestorePollRateCallBack(zbTmrTimerID_t tmrid)
{
  ZbTMR_FreeTimer(tmrid);
  /* Change the poll rate to its initial value after ECC procedure*/    
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
  if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
  {
    (void)ZDO_NLME_ChangePollRate(ZbZdoPublicData(gZdoOrgPollRate));
  }
#endif  
}
#endif

/*****************************************************************************/
/* Timout timer callback function                                            */
/* If timeout triggered this function will reset state machine and clean up  */
/*****************************************************************************/
static void KeyEstabTimeoutCallback(zbTmrTimerID_t tmrid)
{
  (void) tmrid;
  ZbTMR_FreeTimer(tmrid); 
  if (KeyEstabState != KeyEstab_InitState_c) 
  {    
    KeyEstabState = KeyEstab_InitState_c;
    KeyEstabFreeMemory();
    BeeAppUpdateDevice(0, gZclUI_KeyEstabTimeout_c, 0, 0, NULL);
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if( NlmeGetRequest(gDevType_c) == gEndDevice_c )
#endif
    {      
      ZbTMR_StartSingleShotTimer(KeyEstabPollTimerId, gKeyEstab_EndDevicePollRate_c, KeyEstabRestorePollRateCallBack);
    }
#endif     
  }
  else
  {
    /* Retry Key establishment */
    TS_SendEvent(gSETaskID, gZclEvtKeyEstablishmentFailed_c);
  }
}

/*****************************************************************************/
/* Stops and frees timer used for key estab timeout                          */
/*****************************************************************************/
static void KeyEstabStopTimeout(void)
{
  if (KeyEstabTimerId != gTmrInvalidTimerID_c)
  {
    ZbTMR_StopTimer(KeyEstabTimerId);
    ZbTMR_FreeTimer(KeyEstabTimerId); 
    KeyEstabTimerId = gTmrInvalidTimerID_c;
  }
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if( NlmeGetRequest(gDevType_c) == gEndDevice_c )
#endif
    {    
      ZbTMR_StartSingleShotTimer(KeyEstabPollTimerId, gKeyEstab_EndDevicePollRate_c, KeyEstabRestorePollRateCallBack);
    }
#endif   
}

/*****************************************************************************/
/* Resets timeout timer so timeout period is started again                   */
/*****************************************************************************/
static void ResetKeyEstabTimeout(void)
{
 // use the biggest timeout value reported, but minimum 2 seconds.
  uint8_t timeout = FLib_GetMax(EphemeralDataGenerateTime, ConfirmKeyGenerateTime );
  timeout = FLib_GetMax(gKeyEstab_MinimumTimeout_c , timeout );
  
  ZbTMR_StartSingleShotTimer(KeyEstabTimerId, ZbTmrSeconds(timeout) + gApsAckWaitDuration_c, KeyEstabTimeoutCallback);
}

/*****************************************************************************/
/* Initializes (allocates timer and start timeout timer                      */
/*****************************************************************************/
static void InitAndStartKeyEstabTimeout(void)
{
  if (KeyEstabTimerId == gTmrInvalidTimerID_c)
  {
    KeyEstabTimerId = ZbTMR_AllocateTimer();
  }
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if( NlmeGetRequest(gDevType_c) == gEndDevice_c )
#endif
      if (KeyEstabPollTimerId == gTmrInvalidTimerID_c)
      {
        KeyEstabPollTimerId = ZbTMR_AllocateTimer();
      }
#endif

  ResetKeyEstabTimeout();
}

/*****************************************************************************/
/* Allocates memory used during a key establishment session                  */
/*****************************************************************************/
static bool_t KeyEstabAllocateMemory(void)
{
  KeyData = MSG_Alloc(sizeof(KeyEstab_KeyData_t));
  if (KeyData)
  {
    BeeUtilZeroMemory(KeyData, sizeof(KeyEstab_KeyData_t));
    pOppositeImplicitCert = MSG_Alloc(sizeof(IdentifyCert_t));
    if (pOppositeImplicitCert)
      return TRUE;
  }
  // memory allocation failed - free memory
  KeyEstabFreeMemory();
  return FALSE;  
}

/*****************************************************************************/
/* Initializes Key estab. state machine and allocates memory needed          */
/*****************************************************************************/
static bool_t InitKeyEstabStateMachine(void) {
  if(KeyEstabState == KeyEstab_InitState_c) 
  {
    if( KeyEstabAllocateMemory()) {      
      KeyEstabState = KeyEstab_KeyEstabInitatedState_c;
      return TRUE;
    }    
  }
 return FALSE;  
}

/*****************************************************************************/
/* Sends terminate key estab to server and resets statemachine               */
/*****************************************************************************/
static zbStatus_t ZCL_TerminateKeyEstabServer(zbApsdeDataIndication_t *pIndication, uint8_t ErrorCode,bool_t ResetStateMachine)
{  
  zbStatus_t status;
  
  status = ZCL_SendTerminateKeyEstabServerReq(ErrorCode, gKeyEstab_DefaultWaitTime_c,pIndication);
  KeyEstabStopTimeout();
  if (ResetStateMachine) 
  {    
    KeyEstabState = KeyEstab_InitState_c;
    KeyEstabFreeMemory();
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabFailed_c, 0, 0, pIndication);
  }  
  return status;
}
/*****************************************************************************/
/* Sends terminate key estab to client and resets statemachine               */
/*****************************************************************************/
static zbStatus_t ZCL_TerminateKeyEstabClient(zbApsdeDataIndication_t *pIndication, uint8_t ErrorCode,bool_t ResetStateMachine) 
{
  zbStatus_t status;

  status = ZCL_SendTerminateKeyEstabClientReq(ErrorCode, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
  KeyEstabStopTimeout();
  if (ResetStateMachine) 
  {    
    KeyEstabState = KeyEstab_InitState_c;
    KeyEstabFreeMemory();
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabFailed_c, 0, 0, pIndication);    
  }
  return status;  
}

/*****************************************************************************/
/* calculates key, MACU, MAC for the server/responder side                   */
/*****************************************************************************/
static void GenerateKeyMACUMACVResponder(void)
{       // Generate MAC key and MAC data  
        SSP_HashKeyDerivationFunction(KeyData->SharedSecret,gZclCmdKeyEstab_SharedSecretSize_c, KeyData->mackey);
        //MACtag 1, initator Ieee, responer ieee, Initiator Ep pub key, responder ep pub key, shared secret Z, output.
        SSP_HashGenerateSecretTag(2,(uint8_t*) pOppositeImplicitCert->Subject,(uint8_t*) DeviceImplicitCert.Subject, KeyData->u_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->v_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->SharedSecret,KeyData->MACU,FALSE, NULL);
        //MACtag 2, responder Ieee, initiator ieee, responder Ep pub key, initiator ep pub key, shared secret Z, output.
        SSP_HashGenerateSecretTag(3,(uint8_t*) DeviceImplicitCert.Subject, (uint8_t*)pOppositeImplicitCert->Subject, KeyData->v_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->u_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->SharedSecret,KeyData->MACV,FALSE, NULL);          
        //MACtag 1, initator Ieee, responer ieee, Initiator Ep pub key, responder ep pub key, shared secret Z, output.

}
/*****************************************************************************/
/* calculates key, MACU, MAC for the client/initiator side                   */
/*****************************************************************************/
static void GenerateKeyMACUMACVInitiator(void)
{
// Generate MAC key and MAC data  
        SSP_HashKeyDerivationFunction(KeyData->SharedSecret, gZclCmdKeyEstab_SharedSecretSize_c, KeyData->mackey);
        //MACtag 1, initator Ieee, responer ieee, Initiator Ep pub key, responder ep pub key, shared secret Z, output.
        SSP_HashGenerateSecretTag(2,(uint8_t*) DeviceImplicitCert.Subject,(uint8_t*) pOppositeImplicitCert->Subject, KeyData->u_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->v_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->SharedSecret,KeyData->MACU, FALSE, NULL);
        //MACtag 2, responder Ieee, initiator ieee, responder Ep pub key, initiator ep pub key, shared secret Z, output.
        SSP_HashGenerateSecretTag(3,(uint8_t*) pOppositeImplicitCert->Subject,(uint8_t*) DeviceImplicitCert.Subject,KeyData->v_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->u_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->SharedSecret,KeyData->MACV, FALSE, NULL);                
}
/*****************************************************************************/
/* Check if the sender and the length of the InitKeyEstabReq is correct      */
/*****************************************************************************/
static bool_t ValidateKeyEstabMessage(zbApsdeDataIndication_t *pIndication, IdentifyCert_t *pCert) 
{
zbNwkAddr_t aSrcAddr; 
zbIeeeAddr_t aCertIeee;
#if gComboDeviceCapability_d || gCoordinatorCapability_d
zbNwkAddr_t aCertNwk;
#endif
uint8_t asduLen=pIndication->asduLength; 
FLib_MemCpyReverseOrder(aCertIeee,pCert->Subject,sizeof(zbIeeeAddr_t));
Copy2Bytes(aSrcAddr,pIndication->aSrcAddr);
#if gComboDeviceCapability_d 
  if (((NlmeGetRequest(gDevType_c) != gCoordinator_c) && (Cmp2BytesToZero(aSrcAddr) && IsEqual8Bytes(aCertIeee, ApsmeGetRequest(gApsTrustCenterAddress_c))))||((NlmeGetRequest(gDevType_c) == gCoordinator_c) && IsEqual2Bytes(APS_GetNwkAddress(aCertIeee,aCertNwk),aSrcAddr)))
#elif gCoordinatorCapability_d
  if(IsEqual2Bytes(APS_GetNwkAddress(aCertIeee,aCertNwk),aSrcAddr))
#else
  if(Cmp2BytesToZero(aSrcAddr) && IsEqual8Bytes(aCertIeee, ApsmeGetRequest(gApsTrustCenterAddress_c)))
#endif 
  {
#if gZclAcceptLongCert_d
    if((asduLen>=sizeof(ZclCmdKeyEstab_InitKeyEstabReq_t)+sizeof(zclFrame_t)))
#else
    if((asduLen==sizeof(ZclCmdKeyEstab_InitKeyEstabReq_t)+sizeof(zclFrame_t)))
#endif
      return TRUE;
  } 
  return FALSE; 
} 
  
static zbStatus_t ZCL_SendInitiatKeyEstReq(IdentifyCert_t *Cert, zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr) 
{
  ZclKeyEstab_InitKeyEstabReq_t *pReq;
  zbStatus_t status;
    
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_InitKeyEstabReq_t)); 
  
  if(pReq) {
    pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
    Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
    pReq->addrInfo.dstEndPoint = DstEndpoint;
    pReq->addrInfo.srcEndPoint = SrcEndpoint;
    pReq->addrInfo.txOptions = gApsTxOptionAckTx_c;
    pReq->addrInfo.radiusCounter = afDefaultRadius_c;
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
          
    pReq->cmdFrame.EphemeralDataGenerateTime = gKeyEstab_DefaultEphemeralDataGenerateTime_c;
    pReq->cmdFrame.ConfirmKeyGenerateTime = gKeyEstab_DefaultConfirmKeyGenerateTime_c;
    Set2Bytes(pReq->cmdFrame.SecuritySuite,gKeyEstabSuite_CBKE_ECMQV_c);
    
    FLib_MemCpy(pReq->cmdFrame.IdentityIDU,(uint8_t *)Cert ,sizeof(IdentifyCert_t)/sizeof(uint8_t));
    
    status = zclKeyEstab_InitKeyEstabReq(pReq);
    MSG_Free(pReq);
  }
  else
    status = gZclNoMem_c;
  
  return status;
}

static zbStatus_t ZCL_SendEphemeralDataReq(uint8_t *EphemeralPubKey, zbApsdeDataIndication_t *pIndication)
{
  ZclKeyEstab_EphemeralDataReq_t *pReq;
  zbStatus_t status;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_EphemeralDataReq_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    pReq->addrInfo.txOptions = gApsTxOptionAckTx_c;
    
    FLib_MemCpy(pReq->cmdFrame.EphemeralDataQEU,(uint8_t *)EphemeralPubKey ,gZclCmdKeyEstab_CompressedPubKeySize_c);
    
    status = zclKeyEstab_EphemeralDataReq(pReq);
    MSG_Free(pReq);
  }
  else
    status = gZclNoMem_c;

  return status;
}

static zbStatus_t ZCL_SendConfirmKeyDataReq(uint8_t *MACU, zbApsdeDataIndication_t *pIndication) 
{
  ZclKeyEstab_ConfirmKeyDataReq_t *pReq;
  zbStatus_t status;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_ConfirmKeyDataReq_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    pReq->addrInfo.txOptions = gApsTxOptionAckTx_c;
             
    FLib_MemCpy(pReq->cmdFrame.SecureMsgAuthCodeMACU,(uint8_t *)MACU ,gZclCmdKeyEstab_AesMMOHashSize_c);
    
    status = zclKeyEstab_ConfirmKeyDataReq(pReq);
    MSG_Free(pReq);
  }
  else
    status = gZclNoMem_c;

  return status;
}

static zbStatus_t ZCL_SendTerminateKeyEstabServerReq(uint8_t Status,uint8_t WaitCode, zbApsdeDataIndication_t *pIndication) 
{
  ZclKeyEstab_TerminateKeyEstabServer_t *pReq;
  zbStatus_t status;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_TerminateKeyEstabServer_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
  pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    pReq->addrInfo.txOptions = gApsTxOptionAckTx_c;
     
    pReq->cmdFrame.StatusCode  = Status;
    pReq->cmdFrame.WaitCode    = WaitCode;
    Set2Bytes(pReq->cmdFrame.SecuritySuite,gKeyEstabSuite_CBKE_ECMQV_c);  
   
    status = zclKeyEstab_TerminateKeyEstabServer(pReq);
    MSG_Free(pReq);
  
    /* Retry Key Establishment after a default wait time */
    ZbTMR_StartSingleShotTimer(KeyEstabTimerId, ZbTmrSeconds(gKeyEstab_DefaultWaitTime_c), KeyEstabTimeoutCallback);
  }
  else
    status = gZclNoMem_c;

  return status;
}

static zbStatus_t ZCL_SendInitiatKeyEstRsp(IdentifyCert_t *Cert,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid) 
{
  ZclKeyEstab_InitKeyEstabRsp_t *pReq;
  zbStatus_t status;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_InitKeyEstabReq_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
    pReq->zclTransactionId = transactionid;
    pReq->addrInfo.txOptions = gApsTxOptionAckTx_c;
          
    pReq->cmdFrame.EphemeralDataGenerateTime = gKeyEstab_DefaultEphemeralDataGenerateTime_c;
    pReq->cmdFrame.ConfirmKeyGenerateTime = gKeyEstab_DefaultConfirmKeyGenerateTime_c;
    Set2Bytes(pReq->cmdFrame.SecuritySuite,gKeyEstabSuite_CBKE_ECMQV_c);
    
    FLib_MemCpy(pReq->cmdFrame.IdentityIDV,(uint8_t *)Cert ,sizeof(IdentifyCert_t)/sizeof(uint8_t));
    
    status = zclKeyEstab_InitKeyEstabRsp(pReq);
    MSG_Free(pReq);
  }
  else
    status = gZclNoMem_c;

  return status;
}

static zbStatus_t ZCL_SendEphemeralDataRsp(uint8_t *EphemeralPubKey,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid) 
{
  ZclKeyEstab_EphemeralDataRsp_t *pReq;
  zbStatus_t status;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_EphemeralDataRsp_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
    pReq->zclTransactionId = transactionid;	
    pReq->addrInfo.txOptions = gApsTxOptionAckTx_c;

    FLib_MemCpy(pReq->cmdFrame.EphemeralDataQEV,(uint8_t *)EphemeralPubKey ,gZclCmdKeyEstab_CompressedPubKeySize_c);
    
    status = zclKeyEstab_EphemeralDataRsp(pReq);
    MSG_Free(pReq);
  }
  else
    status = gZclNoMem_c;

  return status;
}

static zbStatus_t ZCL_SendConfirmKeyDataRsp(uint8_t *MACV, zbApsdeDataIndication_t *pIndication, uint8_t transactionid)
{
  ZclKeyEstab_ConfirmKeyDataRsp_t *pReq;
  zbStatus_t status;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_ConfirmKeyDataRsp_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
    pReq->zclTransactionId = transactionid;	
    pReq->addrInfo.txOptions = gApsTxOptionAckTx_c;

    FLib_MemCpy(pReq->cmdFrame.SecureMsgAuthCodeMACV,(uint8_t *)MACV ,gZclCmdKeyEstab_AesMMOHashSize_c);
    
    status = zclKeyEstab_ConfirmKeyDataRsp(pReq);
    MSG_Free(pReq);
  }
  else
    status = gZclNoMem_c;

  return status;
}

static zbStatus_t ZCL_SendTerminateKeyEstabClientReq(uint8_t Status, uint8_t WaitCode,zbApsdeDataIndication_t *pIndication , uint8_t transactionid) 
{
  ZclKeyEstab_TerminateKeyEstabClient_t *pReq;
  zbStatus_t status;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_TerminateKeyEstabClient_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
    	
  pReq->zclTransactionId = transactionid;	
  pReq->addrInfo.txOptions = gApsTxOptionAckTx_c;

  pReq->cmdFrame.StatusCode  = Status;
  pReq->cmdFrame.WaitCode    = WaitCode;
  Set2Bytes(pReq->cmdFrame.SecuritySuite,gKeyEstabSuite_CBKE_ECMQV_c);  

  status = zclKeyEstab_TerminateKeyEstabClient(pReq);
  MSG_Free(pReq);
  }
  else
    status = gZclNoMem_c;

  return status;
}
