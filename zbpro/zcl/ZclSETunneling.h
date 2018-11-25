/*! @file 	ZclSETunneling.h
 *
 * @brief	Types, definitions and prototypes for the Tunneling cluster implementation.
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
#ifndef _ZCLSETUNNELING_H
#define _ZCLSETUNNELING_H

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

#define gZclSETunnelMaxDataHandlingCapacity_c 2
#define gZclSETunnelRxBufferSize_c  300 // also used for maximum incoming transfer size
#define gZclSETunnelNumberOfSupportedProtocolIDs_c  1
#define gZclSETunnelNumberOfProtocolIDsInSTPR_c		16

#if gBigEndian_c
#define gZclAttrTunneling_CloseTunnelTimeout_c   0x1E00
#else
#define gZclAttrTunneling_CloseTunnelTimeout_c   0x001E
#endif

// ProtocolIDs
#define DLMS_COSEM    0x00
#define IEC61107      0x01
#define ANSI_C12      0x02
#define M_BUS         0x03
#define SML           0x04
#define ClimateTalk   0x05
#define TestProtocol  0xC7

// Manufacturer codes
#define gZclSETunnel_NoManufacturerCode_c	0xFFFF

// Tunnel IDs
#define gZclSETunnel_TunnelFail_c    0xFFFF

#define gZclCluster_High_Tunneling_c  0x07
#define gZclCluster_Low_Tunneling_c   0x04

// Tunnel status
#define gZclSETunnel_Success_c                    0x00
#define gZclSETunnel_Busy_c                       0x01
#define gZclSETunnel_NoMoreTunnelIDs_c            0x02
#define gZclSETunnel_ProtocolNotSupported_c       0x03
#define gZclSETunnel_FlowControlNotSupported_c    0x04
/* not in the spec but sent when no appropiate status was available */
#define gZclSETunnel_GeneralFailiure_c            0xf0

// Tunnel entry status
#define gZclSETunnel_TTE_Free_c                         0x00
#define gZclSETunnel_TTE_AwaitingTunnelRequestRsp_c     0x01
#define gZclSETunnel_TTE_Active_c                       0x02

// Transfer data status
#define gZclSETunnel_NoSuchTunnel_c				  0x00
#define gZclSETunnel_WrongDevice_c				  0x01
#define gZclSETunnel_DataOverflow_c				  0x02
#define gZclSETunnel_NoError_c                                    0xFD

// ZCL SE Tunneling status
#define gZclSETunnel_TableFull_c                  0xfe
#define gZclSETunnel_Fail_c                       0xff

/*----------Client commands---------*/
#define gZclCmdSETunneling_Client_RequestTunnelReq_c              0x00 /* M */
#define gZclCmdSETunneling_Client_CloseTunnelReq_c                0x01 /* M */
#define gZclCmdSETunneling_Client_TransferData_c                  0x02 /* M */
#define gZclCmdSETunneling_Client_TransferDataError_c          	  0x03 /* M */
#define gZclCmdSETunneling_Client_AckTransferDataRsp_c            0x04 /* O */
#define gZclCmdSETunneling_Client_ReadyDataReq_c                  0x05 /* O */
#define gZclCmdSETunneling_Client_GetSupportedTunnelProtocols_c   0x06 /* O */
/*----------Server commands---------*/
#define gZclCmdSETunneling_Server_RequestTunnelRsp_c              0x00 /* M */
#define gZclCmdSETunneling_Server_TransferData_c                  0x01 /* M */
#define gZclCmdSETunneling_Server_TransferDataError_c             0x02 /* M */
#define gZclCmdSETunneling_Server_AckTransferDataRsp_c            0x03 /* O */
#define gZclCmdSETunneling_Server_ReadyDataReq_c                  0x04 /* O */
#define gZclCmdSETunneling_Server_SupportedTunnelProtocolsRsp_c	  0x05 /* O */
#define gZclCmdSETunneling_Server_TunnelClosureNotification_c     0x06 /* O */

/* Server attributes*/
#define gZclAttrIdSETunnelingCloseTunnelTimeout_c         0x00

#define gZclAttrSETunnelingCloseTunnelTimeout_c 0x0000

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef PACKED_STRUCT zclCmdSETunneling_RequestTunnelReq_tag
{
  uint8_t protocolID;
  uint16_t manufacturerCode;
  bool_t flowControlSupport;
  uint16_t maximumIncomingtransferSize;
} zclCmdSETunneling_RequestTunnelReq_t;


typedef PACKED_STRUCT  zclSETunneling_RequestTunnelReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSETunneling_RequestTunnelReq_t cmdFrame;
} zclSETunneling_RequestTunnelReq_t;

typedef PACKED_STRUCT zclCmdSETunneling_CloseTunnelReq_tag
{
  uint16_t tunnelId;
}zclCmdSETunneling_CloseTunnelReq_t;

typedef PACKED_STRUCT zclSETunneling_CloseTunnelReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSETunneling_CloseTunnelReq_t cmdFrame;
}zclSETunneling_CloseTunnelReq_t;

typedef PACKED_STRUCT zclCmdSETunneling_TunnelClosureNotification_tag
{
  uint16_t tunnelId;
}zclCmdSETunneling_TunnelClosureNotification_t;

typedef PACKED_STRUCT zclSETunneling_TunnelClosureNotification_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSETunneling_TunnelClosureNotification_t cmdFrame;
}zclSETunneling_TunnelClosureNotification_t;

typedef PACKED_STRUCT zclCmdSETunneling_TransferDataReq_tag
{
  uint16_t      tunnelId;
  uint8_t       data[1];
}zclCmdSETunneling_TransferDataReq_t;

typedef PACKED_STRUCT zclSETunneling_TransferDataReq_tag
{
  zclCmdSETunneling_TransferDataReq_t   cmdFrame;
}zclSETunneling_TransferDataReq_t;

typedef PACKED_STRUCT zclSETunneling_ZTCTransferDataReq_tag
{
  uint16_t    tunnelId;
}zclSETunneling_ZTCTransferDataReq_t;

typedef PACKED_STRUCT zclSETunneling_LoadFragment_tag
{
  uint16_t          tunnelId;
  uint8_t           length;
  uint8_t           data[1];
}zclSETunneling_LoadFragment_t;

typedef PACKED_STRUCT zclCmdSETunneling_TransferDataErrorReq_tag
{
  uint16_t tunnelId;
  uint8_t transferDataStatus;
}zclCmdSETunneling_TransferDataErrorReq_t;

typedef PACKED_STRUCT zclSETunneling_TransferDataErrorReq_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdSETunneling_TransferDataErrorReq_t   cmdFrame;
}zclSETunneling_TransferDataErrorReq_t;

typedef PACKED_STRUCT zclCmdSETunneling_GetSupportedTunnelProtocolsReq_tag
{
  uint8_t	protocolOffset;
}zclCmdSETunneling_GetSupportedTunnelProtocolsReq_t;

typedef PACKED_STRUCT zclSETunneling_GetSupportedTunnelProtocolsReq_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdSETunneling_GetSupportedTunnelProtocolsReq_t	cmdFrame;
}zclSETunneling_GetSupportedTunnelProtocolsReq_t;

typedef PACKED_STRUCT zclCmdSETunneling_AckTransferDataRsp_tag
{
  uint16_t tunnelId;
  uint16_t numberOfBytesLeft;
}zclCmdSETunneling_AckTransferDataRsp_t;

typedef PACKED_STRUCT zclSETunneling_AckTransferDataRsp_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdSETunneling_AckTransferDataRsp_t   cmdFrame;
}zclSETunneling_AckTransferDataRsp_t;

typedef PACKED_STRUCT zclCmdSETunneling_ReadyDataReq_tag
{
  uint16_t tunnelId;
  uint16_t numberOfBytesLeft;
}zclCmdSETunneling_ReadyDataReq_t;

typedef PACKED_STRUCT zclSETunneling_ReadyDataReq_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdSETunneling_ReadyDataReq_t         cmdFrame;
}zclSETunneling_ReadyDataReq_t;

typedef PACKED_STRUCT zclCmdSETunneling_RequestTunnelRsp_tag
{
  uint16_t tunnelId;
  uint8_t  tunnelStatus;
  uint16_t maximumIncomingtransferSize;
}zclCmdSETunneling_RequestTunnelRsp_t;

typedef PACKED_STRUCT zclSETunneling_RequestTunnelRsp_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdSETunneling_RequestTunnelRsp_t  cmdFrame;
}zclSETunneling_RequestTunnelRsp_t;

typedef PACKED_STRUCT zclSETunneling_Protocol_tag
{
  uint16_t 	manufacturerCode;
  uint8_t	protocolID;
}zclSETunneling_Protocol_t;

typedef PACKED_STRUCT zclCmdSETunneling_SupportedTunnelingProtocolsRsp_tag
{
  bool_t                      protocolListComplete;
  uint8_t                     protocolCount;
  zclSETunneling_Protocol_t   protocolList[1];
}zclCmdSETunneling_SupportedTunnelingProtocolsRsp_t;

typedef PACKED_STRUCT zclSETunneling_SupportedTunnelingProtocolsRsp_tag
{
  afAddrInfo_t                          				addrInfo;
  uint8_t                               				zclTransactionId;
  zclCmdSETunneling_SupportedTunnelingProtocolsRsp_t	cmdFrame;
}zclSETunneling_SupportedTunnelingProtocolsRsp_t;

typedef PACKED_STRUCT zclSETunneling_ReadyRx_tag
{
  uint16_t tunnelID;
  uint16_t numberOfBytesLeft;
}zclSETunneling_ReadyRx_t;

typedef PACKED_STRUCT tunnelingTable_tag
{
  /* common */
  uint16_t tunnelID;
  uint8_t protocolId;
  uint16_t manufacturerCode;
  bool_t flowControlSupport;
  bool_t isClient;
  uint8_t tunnelStatus;
  
  /* partner */
  uint16_t maximumIncomingTransferSize;
  uint16_t numberOfOctetsLeft;
  zbIeeeAddr_t partnerIeeeAddr;
  afAddrInfo_t partnerAddrInfo;
  /* local */
  zbTmrTimerID_t closeTunnelTmr;
  uint16_t rxDataLength;
  uint16_t txDataLength;
  zbTxFragmentedHdr_t *pTxBuffers;
  /* unused*/
  uint8_t transferDataStatus;
} tunnelingTable_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Client_RequestTunnelReq(zclSETunneling_RequestTunnelReq_t *pReq)
 *
 * @brief	Sends over-the-air a Request Tunnel frame from the Tunneling client and 
 *			creates a new entry in the tunneling table.
 *
 */
zbStatus_t ZCL_SETunnel_Client_RequestTunnelReq(zclSETunneling_RequestTunnelReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Client_CloseTunnelReq(zclSETunneling_CloseTunnelReq_t *pReq)
 *
 * @brief	Sends over-the-air a Close Tunnel frame from the Tunneling client.
 *
 */
zbStatus_t ZCL_SETunnel_Client_CloseTunnelReq(zclSETunneling_CloseTunnelReq_t *pReq);

/*!
 * @fn 		ZCL_SETunnel_Server_SetNextTunnelID(uint16_t tunnelID)
 *
 * @brief	Sets the next available tunnel ID on the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_SetNextTunnelID(uint16_t tunnelID);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_RequestTunnelRsp(zclSETunneling_RequestTunnelRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Request Tunnel Response frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_RequestTunnelRsp(zclSETunneling_RequestTunnelRsp_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_TunnelClosureNotificationReq(zclSETunneling_TunnelClosureNotification_t *pReq)
 *
 * @brief	Sends over-the-air a Tunnel Closure Notification frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_TunnelClosureNotificationReq(zclSETunneling_TunnelClosureNotification_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_ReadyData(zclSETunneling_ReadyDataReq_t *pReq)
 *
 * @brief	Sends over-the-air a Ready Data frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_ReadyData(zclSETunneling_ReadyDataReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_SupportedTunnelProtocolsRsp(zclSETunneling_SupportedTunnelingProtocolsRsp_t *pReq)
 *
 * @brief	Sends over-the-air a Ready Data frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_SupportedTunnelProtocolsRsp(zclSETunneling_SupportedTunnelingProtocolsRsp_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Client_TransferDataErrorReq(zclSETunneling_TransferDataErrorReq_t *pReq)
 *
 * @brief	Sends over-the-air a Transfer Data Error frame from the Tunneling client.
 *
 */
zbStatus_t ZCL_SETunnel_Client_TransferDataErrorReq(zclSETunneling_TransferDataErrorReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Client_AckTransferDataRsp(zclSETunneling_AckTransferDataRsp_t *pReq)
 *
 * @brief	Sends over-the-air an Ack Transfer Data frame from the Tunneling client.
 *
 */
zbStatus_t ZCL_SETunnel_Client_AckTransferDataRsp(zclSETunneling_AckTransferDataRsp_t *pReq);

/*!
 * @fn 		void ZCL_SETunnel_InitData(void)
 *
 * @brief	Initializes the Tunneling server functionality.
 *
 */
void ZCL_SETunnel_InitData(void);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_TransferDataReq(uint16_t tunnelId)
 *
 * @brief	Sends the cached data to the tunnel partner.
 *
 */
zbStatus_t ZCL_SETunnel_TransferDataReq(uint16_t tunnelId);

zbStatus_t ZCL_SETunnel_ReadyRx(zclSETunneling_ReadyRx_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_LoadFragment(zclSETunneling_LoadFragment_t *pReq)
 *
 * @brief	Loads fragments of data to be tunneled.
 *
 */
zbStatus_t ZCL_SETunnel_LoadFragment(zclSETunneling_LoadFragment_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Client_GetSupportedProtocolsReq(zclSETunneling_GetSupportedTunnelProtocolsReq_t *pReq)
 *
 * @brief	Sends over-the-air a Get Supported Tunnel Protocols Data from the Tunneling client.
 *
 */
zbStatus_t ZCL_SETunnel_Client_GetSupportedProtocolsReq(zclSETunneling_GetSupportedTunnelProtocolsReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_ReadyDataReq(zclSETunneling_ReadyDataReq_t *pReq, bool_t client)
 *
 * @brief	Sends over-the-air a Ready Data frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_ReadyDataReq(zclSETunneling_ReadyDataReq_t *pReq, bool_t client);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_TransferDataErrorReq(zclSETunneling_TransferDataErrorReq_t *pReq)
 *
 * @brief	Sends over-the-air a Transfer Data Error frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_TransferDataErrorReq(zclSETunneling_TransferDataErrorReq_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnel_Server_AckTransferDataRsp(zclSETunneling_AckTransferDataRsp_t *pReq)
 *
 * @brief	Sends over-the-air an Ack Transfer Data frame from the Tunneling server.
 *
 */
zbStatus_t ZCL_SETunnel_Server_AckTransferDataRsp(zclSETunneling_AckTransferDataRsp_t *pReq);

/*!
 * @fn 		zbStatus_t ZCL_SETunnelingClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Tunneling Cluster client. 
 *
 */
zbStatus_t ZCL_SETunnelingClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZCL_SETunnelingServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Tunneling Cluster server. 
 *
 */
zbStatus_t ZCL_SETunnelingServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		uint8_t getTunnelTableEntry(uint16_t tunnelID)
 *
 * @brief	Finds the Tunneling table entry on the server, for a given tunnel id.
 *
 */
uint8_t getTunnelTableEntry(uint16_t tunnelID);

zbStatus_t ZCL_SETunnel_ZTCTransferDataReq(zclSETunneling_ZTCTransferDataReq_t *pReq);
/*******************************************************************************
********************************************************************************
* Public memory declarations
********************************************************************************
*******************************************************************************/
extern tunnelingTable_t gaZclSETunnelingTable[];
#ifdef __cplusplus
}
#endif
#endif /*_ZCLSETUNNELING_H */