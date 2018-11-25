/*! @file 	ZclSEKeyEstab.h
 *
 * @brief	Types, definitions and prototypes for the Key Establishment cluster implementation.
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
 *			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   		contributors may be used to endorse or promote products derived from this
 *   		software without specific prior written permission.
 *
 *			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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

#ifndef _ZCLSEKEYESTAB_H
#define _ZCLSEKEYESTAB_H

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/* Value of the KeyEstabSuite Attribute */

#if gBigEndian_c
#define gKeyEstabSuite_CBKE_ECMQV_c 0x0100      /* 0x0001, little endian order */
#else
#define gKeyEstabSuite_CBKE_ECMQV_c 0x0001
#endif

/* Information set */
#define gZclAttrKeyEstabInfoSet_c    0x00
#define gZclAttrKeyEstabSecSuite_c   0x00 /*M*/

#define gZclCmdKeyEstab_CompressedPubKeySize_c    22
#define gZclCmdKeyEstab_UncompressedPubKeySize_c  43
#define gZclCmdKeyEstab_PrivateKeySize_c          21
#define gZclCmdKeyEstab_CertSize_c                sizeof(IdentifyCert_t)
#define gZclCmdKeyEstab_SharedSecretSize_c        21
#define gZclCmdKeyEstab_PointOrderSize_c          21
#define gZclCmdKeyEstab_AesMMOHashSize_c          16


/*command Ids for client commands (send)*/
#define gZclCmdKeyEstab_InitKeyEstabReq_c         0x00
#define gZclCmdKeyEstab_EphemeralDataReq_c        0x01
#define gZclCmdKeyEstab_ConfirmKeyDataReq_c       0x02
#define gZclCmdKeyEstab_TerminateKeyEstabServer_c 0x03

/*command Ids for server commands (send)*/
#define gZclCmdKeyEstab_InitKeyEstabRsp_c         0x00
#define gZclCmdKeyEstab_EphemeralDataRsp_c        0x01
#define gZclCmdKeyEstab_ConfirmKeyDataRsp_c       0x02
#define gZclCmdKeyEstab_TerminateKeyEstabClient_c 0x03

/*status values for the Terminate Key Establishment command (client and server)*/
#define gZclCmdKeyEstab_TermUnknownIssuer_c     0x01
#define gZclCmdKeyEstab_TermBadKeyConfirm_c     0x02
#define gZclCmdKeyEstab_TermBadMessage_c        0x03
#define gZclCmdKeyEstab_TermNoResources_c       0x04
#define gZclCmdKeyEstab_TermUnsupportedSuite_c  0x05

/*status values for the Initiate Key Establishment response*/
#define gZclCmdKeyEstab_InitSuccess_c           0x00
#define gZclCmdKeyEstab_InitBadCertificate_c    0x01
#define gZclCmdKeyEstab_InitBadMessage_c        0x02
#define gZclCmdKeyEstab_Timeout_c               0x03

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef uint8_t SecuritySuite_t[2];

typedef PACKED_STRUCT IdentityCert_tag
{
  uint8_t PublicReconstrKey[22];
  zbIeeeAddr_t Subject;
  uint8_t Issuer[8];
  uint8_t ProfileAttributeData[10];
} IdentifyCert_t;

/*Client commands*/
typedef PACKED_STRUCT ZclCmdKeyEstab_InitKeyEstabReq_tag {
  SecuritySuite_t SecuritySuite;
  uint8_t EphemeralDataGenerateTime;
  uint8_t ConfirmKeyGenerateTime;
  uint8_t IdentityIDU[gZclCmdKeyEstab_CertSize_c];
} ZclCmdKeyEstab_InitKeyEstabReq_t;

typedef PACKED_STRUCT ZclCmdKeyEstab_EphemeralDataReq_tag {
  uint8_t EphemeralDataQEU[gZclCmdKeyEstab_CompressedPubKeySize_c];
} ZclCmdKeyEstab_EphemeralDataReq_t;


typedef PACKED_STRUCT ZclCmdKeyEstab_ConfirmKeyDataReq_tag {
  uint8_t SecureMsgAuthCodeMACU[gZclCmdKeyEstab_AesMMOHashSize_c];
} ZclCmdKeyEstab_ConfirmKeyDataReq_t;

typedef PACKED_STRUCT ZclCmdKeyEstab_TerminateKeyEstabServer_tag {
  uint8_t StatusCode;
  uint8_t WaitCode;
  SecuritySuite_t SecuritySuite;
} ZclCmdKeyEstab_TerminateKeyEstabServer_t;


/*Server commmands*/
typedef PACKED_STRUCT ZclCmdKeyEstab_InitKeyEstabRsp_tag {
  SecuritySuite_t SecuritySuite;
  uint8_t EphemeralDataGenerateTime;
  uint8_t ConfirmKeyGenerateTime;
  uint8_t IdentityIDV[gZclCmdKeyEstab_CertSize_c];
} ZclCmdKeyEstab_InitKeyEstabRsp_t;

typedef PACKED_STRUCT ZclCmdKeyEstab_EphemeralDataRsp_tag {
  uint8_t EphemeralDataQEV[gZclCmdKeyEstab_CompressedPubKeySize_c];
} ZclCmdKeyEstab_EphemeralDataRsp_t;

typedef PACKED_STRUCT ZclCmdKeyEstab_ConfirmKeyDataRsp_tag {
  uint8_t SecureMsgAuthCodeMACV[gZclCmdKeyEstab_AesMMOHashSize_c];
} ZclCmdKeyEstab_ConfirmKeyDataRsp_t;

typedef PACKED_STRUCT ZclCmdKeyEstab_TerminateKeyEstabClient_tag {
  uint8_t StatusCode;
  uint8_t WaitCode;
  SecuritySuite_t SecuritySuite;
} ZclCmdKeyEstab_TerminateKeyEstabClient_t;

/*--------------------------------------*/

typedef PACKED_STRUCT  ZclKeyEstab_InitKeyEstabReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_InitKeyEstabReq_t cmdFrame;
} ZclKeyEstab_InitKeyEstabReq_t;

typedef PACKED_STRUCT  ZclKeyEstab_EphemeralDataReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_EphemeralDataReq_t cmdFrame;
} ZclKeyEstab_EphemeralDataReq_t;

typedef PACKED_STRUCT  ZclKeyEstab_ConfirmKeyDataReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_ConfirmKeyDataReq_t cmdFrame;
} ZclKeyEstab_ConfirmKeyDataReq_t;

typedef PACKED_STRUCT  ZclKeyEstab_TerminateKeyEstabServer_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_TerminateKeyEstabServer_t cmdFrame;
} ZclKeyEstab_TerminateKeyEstabServer_t;

typedef PACKED_STRUCT  ZclKeyEstab_InitKeyEstabRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_InitKeyEstabRsp_t cmdFrame;
} ZclKeyEstab_InitKeyEstabRsp_t;

typedef PACKED_STRUCT  ZclKeyEstab_EphemeralDataRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_EphemeralDataRsp_t cmdFrame;
} ZclKeyEstab_EphemeralDataRsp_t;

typedef PACKED_STRUCT  ZclKeyEstab_ConfirmKeyDataRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_ConfirmKeyDataRsp_t cmdFrame;
} ZclKeyEstab_ConfirmKeyDataRsp_t;


typedef PACKED_STRUCT  ZclKeyEstab_TerminateKeyEstabClient_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_TerminateKeyEstabClient_t cmdFrame;
} ZclKeyEstab_TerminateKeyEstabClient_t;

typedef PACKED_STRUCT ZclKeyEstab_SetSecurityMaterial_tag
{
  IdentifyCert_t deviceImplicitCert;
  uint8_t devicePrivateKey[gZclCmdKeyEstab_PrivateKeySize_c];
  uint8_t devicePublicKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
}ZclKeyEstab_SetSecurityMaterial_t;

typedef PACKED_STRUCT ZclKeyEstab_InitiateKeyEstab_tag
{
  zbEndPoint_t dstEndpoint;
  zbEndPoint_t srcEndpoint;
  zbNwkAddr_t dstAddr;
}ZclKeyEstab_InitiateKeyEstab_t;

typedef PACKED_STRUCT zclAttrKeyEstabAttr_tag
{
  SecuritySuite_t SecuritySuite;
}zclAttrKeyEstabAttr_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/*!
 * @fn 		zbStatus_t ZCL_KeyEstabClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Key Establishment Cluster client, 
 *			only one session at a time is supported.
 *
 */
zbStatus_t ZCL_KeyEstabClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZCL_KeyEstabClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Key Establishment Cluster server, 
 *			only one session at a time is supported.
 *
 */
zbStatus_t ZCL_KeyEstabClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		bool_t ZCL_InitiateKeyEstab(zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr) 
 *
 * @brief	Initiates the CBKE process with a remote node
 *
 */
bool_t ZCL_InitiateKeyEstab(zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr);

/*!
 * @fn 		zbStatus_t zclKeyEstab_InitKeyEstabReq(ZclKeyEstab_InitKeyEstabReq_t *pReq) 
 *
 * @brief	Sends a Initiate Key Establishment Request over-the-air
 *
 */
zbStatus_t zclKeyEstab_InitKeyEstabReq(ZclKeyEstab_InitKeyEstabReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_EphemeralDataReq(ZclKeyEstab_EphemeralDataReq_t *pReq) 
 *
 * @brief	Sends over-the-air an Ephemeral Data Request from the client
 *
 */
zbStatus_t zclKeyEstab_EphemeralDataReq(ZclKeyEstab_EphemeralDataReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_ConfirmKeyDataReq(ZclKeyEstab_ConfirmKeyDataReq_t *pReq) 
 *
 * @brief	Sends over-the-air an Confirm Key Data Request from the client
 *
 */
zbStatus_t zclKeyEstab_ConfirmKeyDataReq(ZclKeyEstab_ConfirmKeyDataReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_TerminateKeyEstabServer(ZclKeyEstab_TerminateKeyEstabServer_t *pReq) 
 *
 * @brief	Sends over-the-air a Terminate Key Establishment from the client
 *
 */
zbStatus_t zclKeyEstab_TerminateKeyEstabServer(ZclKeyEstab_TerminateKeyEstabServer_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_InitKeyEstabRsp(ZclKeyEstab_InitKeyEstabRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an Initiate Key Establishment Response from the server
 *
 */
zbStatus_t zclKeyEstab_InitKeyEstabRsp(ZclKeyEstab_InitKeyEstabRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_EphemeralDataRsp(ZclKeyEstab_EphemeralDataRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an Ephemeral Data Response from the server
 *
 */
zbStatus_t zclKeyEstab_EphemeralDataRsp(ZclKeyEstab_EphemeralDataRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_ConfirmKeyDataRsp(ZclKeyEstab_ConfirmKeyDataRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Confirm Key Data Response from the server
 *
 */
zbStatus_t zclKeyEstab_ConfirmKeyDataRsp(ZclKeyEstab_ConfirmKeyDataRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_TerminateKeyEstabClient(ZclKeyEstab_TerminateKeyEstabClient_t *pReq) 
 *
 * @brief	Sends over-the-air a Terminate Key Establishment from the server
 *
 */
zbStatus_t zclKeyEstab_TerminateKeyEstabClient(ZclKeyEstab_TerminateKeyEstabClient_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_SetSecurityMaterial(ZclKeyEstab_SetSecurityMaterial_t *pReq) 
 *
 * @brief	Sets the CBKE Security Material (Certificate, Private Key, Public Key)
 *
 */
zbStatus_t zclKeyEstab_SetSecurityMaterial(ZclKeyEstab_SetSecurityMaterial_t *pReq);

/*******************************************************************************
********************************************************************************
* Public memory declarations
********************************************************************************
*******************************************************************************/
extern const zclAttrDef_t gZclKeyEstabServerAttrDef[];
#ifdef __cplusplus
}
#endif

#endif /* _ZCLSEKEYESTAB_H */
