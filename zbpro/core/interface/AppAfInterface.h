/*! @file 	  AppAfInterface.h
 *
 * @brief	  This header file includes the type definitions and function prototypes used 
 *          for AF-APP interface.
 *
 * @copyright Copyright(c) 2014, Freescale, Inc. All rights reserved.
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

#ifndef _APP_AF_INTERFACE_H_
#define _APP_AF_INTERFACE_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "BeeStackFunctionality.h"
#include "BeeStack_Globals.h"

#include "zigbee.h"
#include "nwkcommon.h"
/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/* AF SapHandlers events */
#define gAPSDE_AF_c  (1<<0)

/* AF SapHandlers events */
#define gApp_AF_c    (1<<1)

/* AF reset */
#define gAfReset_c  (1<<2)

/* default radius = 2 * maxdepth from stack profile */
#define afDefaultRadius_c gDefaultRadiusCounter

/*
  AF_Payload

  Use to find the payload location 
*/
#define AF_Payload(pMsg) (void *)(((uint8_t *)(pMsg)) + gAsduOffset_c)

/*  AF_MaxPayloadLen  Returns the maximum payload length */
#define AF_MaxPayloadLen(pAddrInfo)  GetMaxApplicationPayloadByAddrInfo(pAddrInfo)

/*  AF APIs used for preparing and sending data (with or w/o fragmentation) */
#if (gFragmentationCapability_d)
  #define AF_DataRequest(pAddrInfo, payloadLen,pPayload,pConfirmId) \
        AF_ActualDataRequestFragmented(pAddrInfo, payloadLen,pPayload,pConfirmId)
  #define AF_DataRequestNoCopy(pAddrInfo, payloadLen, pMsg, pConfirmId) \
        AF_ActualDataRequestFragmentedNoCopy(pAddrInfo, payloadLen, pMsg, pConfirmId)
  #define AF_MsgFreeRequest(pMsg) \
        AF_ActualMsgFreeRequest(pMsg)
  #define AF_FreeDataIndicationMsg(pMsg) \
        AF_ActualMsgFreeIndication(pMsg)
  #define AF_GetSizeOfIndication(pIndication) \
        AF_ActualGetSizeOfIndication(pIndication)
  #define AF_GetSizeOfDataRequest(pDataReq) \
        AF_ActualGetSizeOfDataRequest(pDataReq)
  #define AF_IsFragmentedDataIndication(pIndication) \
        AF_ActualIsFragmentedDataIndication(pIndication)
#else
  #define AF_DataRequest(pAddrInfo, payloadLen,pPayload,pConfirmId) \
          AF_ActualDataRequest(pAddrInfo, payloadLen,pPayload,pConfirmId)
  #define AF_DataRequestNoCopy(pAddrInfo, payloadLen, pMsg, pConfirmId) \
          AF_ActualDataRequestNoCopy(pAddrInfo, payloadLen, pMsg, pConfirmId) 
  #define AF_MsgFreeRequest(pMsg) MSG_Free(pMsg)
  #define AF_FreeDataIndicationMsg(pMsg) MSG_Free(pMsg)
  #define AF_GetSizeOfDataRequest(pDataReq) (pDataReq)->asduLength
  #define AF_GetSizeOfIndication(pIndication) (pIndication)->asduLength
  #define AF_IsFragmentedDataIndication(pIndication) FALSE
#endif
            
/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
enum{
  gFrameUnsecured_c,
  gFrameSecuredNetworkkey_c,
  gFrameSecuredLinkkey_c  
};

/* enumeration for end point registration status */
enum {
  gRegSuccess_c,
  gInvalidDeRegReq_c,
  gSimpleDescTooLong    = 254,
  gRegFailure_c         = 255
};  


/* Enumeration for invalid request */
enum {
  gInvalidRequest_c       = 255
};

/* for address mode in the enumeration */
enum {		                
  /* 0x00 = Indirect data request, Device Address and Endpoint fields are 
  ignored.*/
  gAddrNotPresent_c,		
  /* 0x01 = Direct data request with short address of destination Device */
  gShortAndEPAddrPresent_c,
  /* 0x02 = Direct data request with extended address of destination Device */
  gExtAndEPAddrPresent_c
};


/* describes a single indication handler callback */
typedef zbStatus_t (*pfnIndication_t)(zbApsdeDataIndication_t *pIndication, void *pData);

typedef zbStatus_t (*pfnInterPanIndication_t)(zbInterPanDataIndication_t *pIndication, void *pData);

/* Pointer to an attribute validation function */
typedef bool_t (*pfnValidationFunc_t)(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pAttrDef);

/* Used by BeeKit to pull in only the handling code needed for a set of clusters */
typedef PACKED_STRUCT afClusterDef_tag
{
  zbClusterId_t    aClusterId;    /* cluster ID */
  pfnIndication_t  pfnServerIndication; /* server indication handler for this cluster */
  pfnIndication_t  pfnClientIndication; /* client indication handler for this cluster */
  void            *pAttrSetList;     /* ptr to attribute set list (zclAttrSetList_t) */
  uint16_t         dataOffset;    /* offset of this cluster's data within instance data */
#if gAddValidationFuncPtrToClusterDef_c  
  pfnValidationFunc_t pfnValidationFunc; /* ptr to the attribute validation function */
#endif  
  void            *pCommandsDefList;  /* ptr to commands def list (zclCommandsDefList_t) */  
} afClusterDef_t;

/* one per endpoint, this structure defines cluster handlers and attributes for HA */
typedef PACKED_STRUCT afDeviceDef_tag
{
  pfnIndication_t     pfnZCL;        /* non-cluster specific handling, ZigBee Cluster Library style */
  uint8_t             clusterCount;  /* # of clusters in the list */
  afClusterDef_t      *pClusterDef;  /* ptr to cluster definitions (a list of clusters) */
  uint8_t             reportCount;   /* # of items in the report */
  void                *pReportList;  /* a list of reportable attributes (see haReportAttrList_t) */
  void                *pReportSetup; /* pointer to report setup data */  
  void                *pData;        /* describes the data for this device (profile specific) */
  void                *pSceneData;   /* pointer to scene data and table */ 
} afDeviceDef_t;

/* Address Info, used to generate data requests (transmit data over the air) */
/* Note: see also zbApsdeDataReq_t */
typedef PACKED_STRUCT afAddrInfo_tag
{
  zbAddrMode_t    dstAddrMode;    /* indirect, group, direct-16, direct-64 */
  zbApsAddr_t     dstAddr;        /* short address, long address or group (ignored on indirect mode) */
  zbEndPoint_t    dstEndPoint;    /* destination endpoint (ignored if group mode or indirect) */
  zbClusterId_t   aClusterId;     /* what cluster to send from/to? */
  zbEndPoint_t    srcEndPoint;    /* what source endpoint to send from? */
  zbApsTxOption_t txOptions;      /* to ACK or not to ACK, that is the question... */
  uint8_t         radiusCounter;  /* radius */
} afAddrInfo_t;
/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/

/* work-around for BeeKit not being able to output NULLs */
extern const afDeviceDef_t gNoDeviceDef;

/* used for BeeKit autogeneration of HA endpoints */
extern const afDeviceDef_t gHaOnOffLightDeviceDef;
extern const afDeviceDef_t gHaOnOffLightDevice2Def;
extern const afDeviceDef_t gHaDimmableLightDeviceDef;
extern const afDeviceDef_t gHaDimmableLight2DeviceDef;
extern const afDeviceDef_t gHaOnOffSwitchDeviceDef;
extern const afDeviceDef_t gHaDimmerSwitchDeviceDef;
extern const afDeviceDef_t gHaDoorLockDeviceDef;
extern const afDeviceDef_t gHaDoorLockControllerDeviceDef;
extern const afDeviceDef_t gHaShadeDeviceDef;
extern const afDeviceDef_t gHaShadeControllerDeviceDef;
extern const afDeviceDef_t gHaRangeExtenderDeviceDef;
extern const afDeviceDef_t gHaThermostatDeviceDef;
extern const afDeviceDef_t gHaConsumptionAwernessDeviceDeviceDef;
extern const afDeviceDef_t gHaTemperatureSensorDeviceDef;
extern const afDeviceDef_t gHaHeatingCoolingUnitDeviceDef;
extern const afDeviceDef_t gHaIASZoneDeviceDef;
extern const afDeviceDef_t gHaIASACEDeviceDef;
extern const afDeviceDef_t gHaIASCIEDeviceDef;
extern const afDeviceDef_t gHaIASWDDeviceDef;
extern const afDeviceDef_t gTp2DeviceDef;
extern const afDeviceDef_t gHcThermometerDeviceDef;
extern const afDeviceDef_t gHcDataCollectionUnitDeviceDef;
extern const afDeviceDef_t gHcBpMonitorDeviceDef;
extern const afDeviceDef_t gHcGlucoseMeterDeviceDef;
extern const afDeviceDef_t gHcWeightscaleDeviceDef;
extern const afDeviceDef_t gHcGenericManagerDeviceDef;
extern const afDeviceDef_t gHcGenericAgentDeviceDef;
extern const afDeviceDef_t gHaGenericDeviceDef;
extern const afDeviceDef_t gHaConfigurationToolDeviceDef;
extern const afDeviceDef_t gHaCombinedInterfaceDeviceDef;
extern const afDeviceDef_t gHaSimpleSensorDeviceDef;
extern const afDeviceDef_t gHaWhiteGoodsDeviceDef;
extern const afDeviceDef_t gHaHomeGatewayDeviceDef;
extern const afDeviceDef_t gZllColorControllerDeviceDef;
extern const afDeviceDef_t gZllColorLightDeviceDef;
extern const afDeviceDef_t gZllColorLight2DeviceDef;

/* used for BeeKit autogeneration of private profile endpoints */
extern const afDeviceDef_t gAccelerometerDeviceDef;    /* accelerometer */
extern const afDeviceDef_t gAccelDisplayDeviceDef;     /* accelerometer display */

extern afDeviceDef_t const gSEEnergyServicePortalDeviceDef;
extern afDeviceDef_t const gSEEnergyServiceInterfaceMirrorDeviceDef;
extern afDeviceDef_t const gSEInPremiseDisplayDeviceDef;
extern afDeviceDef_t const gSELoadControlDeviceDef;
extern afDeviceDef_t const gSEMeteringDeviceDef;
extern afDeviceDef_t const gSEPCTDeviceDef;
extern afDeviceDef_t const gSEPrepaymentTerminalDeviceDef;
extern afDeviceDef_t const gSERangeExtenderDeviceDef;
extern afDeviceDef_t const gSESmartApplianceDeviceDef;
/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		  zbStatus_t AF_RegisterEndPoint(const endPointDesc_t *pEndPoint)
 *
 * @brief	  Register the endpoint. Data can only be recieved on registered endpoints. 
 *          Call this once for each endpoint upon application task init. 
 *
 * @param   [in] pEndPoint   Endpoint description to register
 *
 * @return  gZbSuccess_c    worked
 *          gZbFailed_c     already registered or problem with the parameters
 *          gZbTableFull_c  Table is full
 *          gZbTooLarge_c   Means there are too many clusters in the simple descriptor. 
 *                          Reduce # of input or output clusters in the list.
 */
zbStatus_t AF_RegisterEndPoint
(
  const endPointDesc_t *pEndPoint  /* IN: endpoint description to register */
);

/*!
 * @fn 		  zbStatus_t AF_DeRegisterEndPoint(zbEndPoint_t endPoint)
 *
 * @brief	  Remove an endpoint from the registration table. Doesn't free any memory.
 *
 * @param   [in] endPoint   Endpoint number to de-register
 *
 * @return  gZbSuccess_c    worked
 *          gZbFailed_c     if endpoint not present in the table
 */
zbStatus_t AF_DeRegisterEndPoint
  (
  zbEndPoint_t endPoint /* IN: endpoint number */
  );

/*!
 * @fn 		  afDeviceDef_t *AF_GetEndPointDevice(zbEndPoint_t endPoint)
 *
 * @brief	  Returns the definition of the device residing on the endpoint.
 *          Use only for application endpoints (1-240).
 *
 * @param   [in] endPoint   Endpoint number to de-register #(1-240)
 *
 * @return  afDeviceDef_t*  Pointer to the device definition
 */
afDeviceDef_t *AF_GetEndPointDevice(zbEndPoint_t endPoint);

/*!
 * @fn 		  zbEndPoint_t AF_DeviceDefToEndPoint(afDeviceDef_t *pDeviceDef)
 *
 * @brief	  Look through endpoint descriptors for this device definition. 
 *
 * @param   [in] pDeviceDef  Device definition
 *
 * @return  zbEndPoint_t    Endpoint number
 */
zbEndPoint_t AF_DeviceDefToEndPoint(afDeviceDef_t *pDeviceDef);

/*!
 * @fn 		  zbSimpleDescriptor_t* AF_FindEndPointDesc(uint8_t endPoint)
 *
 * @brief	  Find the endpoint simple descriptor from the endpoint ID (0x00-0xff)
 *          Returns ptr to the simple descriptor (simpleDescriptor_t), or NULL if
 *          not a registered endpoint.
 *
 * @param   [in] endPoint           endpoint number
 *
 * @return  zbSimpleDescriptor_t*   Ptr to the simple descriptor
 */
zbSimpleDescriptor_t* AF_FindEndPointDesc(uint8_t endPoint);

/*!
 * @fn 		  bool_t ApsIsValidAppEndpoint(zbEndPoint_t endPoint)
 *
 * @brief	  Returns TRUE if the given EP is on the range of 0x01 - 0xf0, FALSE otherwise.
 *
 * @param   [in] endPoint   endpoint number
 *
 * @return  TRUE    endpoint in valid range
 *          FALSE   endpoint not in valid range     
 */
bool_t ApsIsValidAppEndpoint(zbEndPoint_t endPoint);

/*!
 * @fn 		  void *AF_MsgAlloc (void)
 *
 * @brief	  Allocate a message buffer for building a packet to be sent via 
 *          AF_DataRequestNoCopy(). Generally used when building larger payloads in-place.
 *          Alternately, use AF_DataRequest(), where it will make a copy of the payload.
 *
 * @return  void*   Pointer to the allocated buffer. 
 */
void *AF_MsgAlloc
  (
  void
  );

/*!
 * @fn 		  void AF_ActualMsgFreeRequest(afToApsdeMessage_t* pMsg)
 *
 * @brief	  Free the entire data request, including all fragmented blocks. 
 *          This function is used when gFragmentationCapability_d is set to TRUE.
 *          Note: the input is not the pointer to the data request, but ptr to 
 *          the message itself that was DeQueued.
 *
 * @param   [in] pMsg   Pointer to the AF to APSDE message
 */
void AF_ActualMsgFreeRequest
(
  afToApsdeMessage_t* pMsg
);

/*!
 * @fn 		  void AF_ActualMsgFreeIndication(apsdeToAfMessage_t* pMsg)
 *
 * @brief	  Free the entire data indication, including all fragmented blocks. 
 *          This function is used when gFragmentationCapability_d is set to TRUE.
 *          Note: the input is not the pointer to the data indication, but ptr to 
 *          the message itself that was DeQueued.
 *
 * @param   [in] pMsg   Pointer to the APSDE to AF indication message
 */
void AF_ActualMsgFreeIndication
(
  apsdeToAfMessage_t * pMsg
);

/*!
 * @fn 		  uint8_t GetMaxApplicationPayloadByAddrInfo(afAddrInfo_t *pAddrInfo)
 *
 * @brief	  Returns the maximum application payload (ASDU) that can fit
 *          into one single ZigBee packet, given the address information.  
 *
 * @param   [in] pAddrInfo   Pointer to the addressing info
 *
 * @return  uint8_t          Payload length     
 */
uint8_t GetMaxApplicationPayloadByAddrInfo
  (
  afAddrInfo_t *pAddrInfo /* IN: address info */
  );

/*!
 * @fn 		  zbStatus_t AF_ActualDataRequest (
 *                             afAddrInfo_t *pAddrInfo, 
 *                             uint8_t payloadLen, 
 *                             void *pPayload, 
 *                             zbApsConfirmId_t *pConfirmId
 *                             )
 *
 * @brief	  Send data out the radio. Will allocate a message and copy the payload. 
 *          This function is used when gFragmentationCapability_d is set to TRUE.
 *          Best Practice: The user should call in code AF_DataRequest.
 *          Use AfDataRequestNoCopy if creating a long message in place.  
 *
 * @param   [in] pAddrInfo    Pointer to the addressing info
 *          [in] payloadLen   Length of the payload
 *          [in] pPayload     Pointer to the data payload
 *          [in] pConfirmId   Pointer to the Confirm Id
 *
 * @return  zbSuccess_c             If worked.
 *          zbNoMem_c               If not enough memory to process request.
 *          gZbInvalidParameter_c   If invalid parameter.
 */
zbStatus_t AF_ActualDataRequest
  (
  afAddrInfo_t *pAddrInfo, 
  uint8_t payloadLen, 
  void *pPayload, 
  zbApsConfirmId_t *pConfirmId
  );

/*!
 * @fn 		  zbStatus_t AF_ActualDataRequestNoCopy (
 *                             afAddrInfo_t *pAddrInfo, 
 *                             uint8_t payloadLen, 
 *                             afToApsdeMessage_t *pMsg, 
 *                             zbApsConfirmId_t *pConfirmId
 *                             )
 *
 * @brief	  Send data out the radio. Assumes pMsg has already been allocated, 
 *          and the payload is pointed to by the pAsdu field of the data request. 
 *          The pAsdu must point to the offset of the pMsg + ApsmeGetAsduOffset()
 *          This function is used only when fragmentation is not supported.
 *          Best Practice: The user should call in code AF_DataRequestNoCopy.
 *
 * @param   [in] pAddrInfo    Pointer to the addressing info
 *          [in] payloadLen   Length of the payload
 *          [in] pMsg         Pointer to the AfToApsde Message
 *          [in] pConfirmId   Pointer to the Confirm Id
 *
 * @return  zbSuccess_c             If worked.
 *          zbNoMem_c               If not enough memory to process request.
 *          gZbInvalidEndpoint_c    If src endpoint is not registered.
 *          gZbInvalidParameter_c   If invalid parameter.
 */
zbStatus_t AF_ActualDataRequestNoCopy
  (
  afAddrInfo_t *pAddrInfo,
  uint8_t payloadLen,
  afToApsdeMessage_t *pMsg, 
  zbApsConfirmId_t *pConfirmId
  );

/*!
 * @fn 		zbStatus_t AF_ActualDataRequestFragmented(
 *                             afAddrInfo_t *pAddrInfo, 
 *                             uint16_t payloadLen, 
 *                             void *pPayload, 
 *                             zbApsConfirmId_t *pConfirmId
 *                             )
 *
 * @brief	  Send a large (or small) array of bytes using the ZigBee fragmented method. 
 *          Will allocate a message and copy the payload. The total size than can be sent 
 *          depends on the field gApsMaxFragmentLength_c settable through  ApsmeSetRequest():
 *          a total of 256 fragments may be sent in a single data requst. If the 
 *          fragment size is 100, with would be 25,600 bytes. The packets are confirmed with
 *          gApsMaxRetries_c retries. 
 *
 *          Only available if gFragmentedCapability_d property is enabled.
 *          Best Practice: The user should call in code AF_DataRequest.
 *          Use AfDataRequestNoCopy if creating a long message in place. 
 *          
 *          Use the optional confirmId to track multiple in-the-air data requests.
 *
 * @param   [in] pAddrInfo    Pointer to the addressing info
 *          [in] payloadLen   Length of the payload
 *          [in] pPayload     Pointer to the data payload
 *          [in] pConfirmId   Pointer to the Confirm Id
 *
 * @return  zbSuccess_c             If request created and sent.
 *          zbNoMem_c               If not enough memory to process request.
 *          gZbInvalidParameter_c   If invalid parameter.
 *          gZbBusy_c 
 */
zbStatus_t AF_ActualDataRequestFragmented
  (
  afAddrInfo_t *pAddrInfo,      
  uint16_t payloadLen,
  void *pPayload,  
  zbApsConfirmId_t  *pConfirmId 
  );

/*!
 * @fn 		  zbStatus_t AF_ActualDataRequestFragmentedNoCopy (
 *                             afAddrInfo_t *pAddrInfo, 
 *                             uint16_t payloadLen, 
 *                             afToApsdeMessage_t *pMsg, 
 *                             zbApsConfirmId_t *pConfirmId
 *                             )
 *
 * @brief	  Send a large (or small) array of bytes using the ZigBee fragmented method. 
 *          Assumes pMsg has already been allocated and the payload is pointed to 
 *          by the pAsdu field of the data request. The total size than can be sent 
 *          depends on the field gApsMaxFragmentLength_c settable through  ApsmeSetRequest():
 *          a total of 256 fragments may be sent in a single data requst. If the 
 *          fragment size is 100, with would be 25,600 bytes. The packets are confirmed with
 *          gApsMaxRetries_c retries. 
 *
 *          Only available if gFragmentedCapability_d property is enabled.
 *          Best Practice: The user should call in code AF_DataRequestNoCopy.
 *          
 *          Use the optional confirmId to track multiple in-the-air data requests.
 *
 * @param   [in] pAddrInfo    Pointer to the addressing info
 *          [in] payloadLen   Length of the payload
 *          [in] pMsg         Pointer to the AfToApsde Message
 *          [in] pConfirmId   Pointer to the Confirm Id
 *
 * @return  zbSuccess_c             If worked.
 *          zbNoMem_c               If not enough memory to process request.
 *          gZbInvalidEndpoint_c    If src endpoint is not registered.
 *          gZbInvalidParameter_c   If invalid parameter.
 */
zbStatus_t AF_ActualDataRequestFragmentedNoCopy
  (
  afAddrInfo_t *pAddrInfo,      /* IN: source and destination address information */
  uint16_t payloadLen,          /* IN: length of payload */
  afToApsdeMessage_t *pMsg,     /* IN: linked list of fragments starting with data request msg */
  zbApsConfirmId_t  *pConfirmId   /* IN: point to a coinfirm ID if needed */
  );

/*!
 * @fn 		  uint16_t AF_ActualGetSizeOfIndication(zbApsdeDataIndication_t* pIndication)
 *
 * @brief	  Gets the length of the entire payload (including fragments) for a 
 *          APS data indication
 *
 * @param   [in] pMsg   Pointer to the APSDE data indication message
 *
 * @return  uint16_t    Length of ASDU payload
 */
uint16_t AF_ActualGetSizeOfIndication
  (
  zbApsdeDataIndication_t *pIndication
  );
  
/*!
 * @fn 		  uint16_t AF_ActualGetSizeOfDataRequest(zbApsdeDataReq_t* pDataReq)
 *
 * @brief	  Gets the length of the entire payload (including fragments) for a 
 *          zbApsdeDataReq_t
 *
 * @param   [in] pDataReq   Pointer to zbApsdeDataReq_t
 *
 * @return  uint16_t    Length of ASDU payload
 */
uint16_t AF_ActualGetSizeOfDataRequest
  (
  zbApsdeDataReq_t* pDataReq
  );

/*!
 * @fn 		  bool_t AF_ActualIsFragmentedDataIndication(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	  Checks if the APSDE data indication is fragmented
 *
 * @param   [in]    pIndication   Pointer to zbApsdeDataIndication_t
 *
 * @return  bool_t  True, if the data indication is fragmented
 */
bool_t AF_ActualIsFragmentedDataIndication
  (
  zbApsdeDataIndication_t *pIndication  
  );

/*!
 * @fn 		  void AF_PrepareForReply(afAddrInfo_t *pAddrInfo, zbApsdeDataIndication_t *pIndication)
 *
 * @brief	  Prepares the address information for a reply to a data indication. Helper 
 *          function for replies to data indications. Cannot fail.
 *
 * @param   [out]   pAddrInfo     Pointer to the reply addressing information
 *          [in]    pIndication   Pointer to zbApsdeDataIndication_t
 */
void AF_PrepareForReply
  (
  afAddrInfo_t *pAddrInfo,              
  zbApsdeDataIndication_t *pIndication  
  );

/*!
 * @fn 		  void AF_PrepareForUnicast(afAddrInfo_t *pAddrInfo, zbNwkAddr_t aNwkAddr, 
 *               zbEndPoint_t dstEndPoint, zbEndPoint_t srcEndPoint, zbClusterId_t  aClusterId)
 *
 * @brief	  Prepares the address information for a direct unicast data
 *          data transmission by using the 16-bit address. It uses link key
 *          security
 *
 * @param   [out]   pAddrInfo     Pointer to the reply addressing information
 *          [in]    aNwkAddr      Destination 16-bit address
 *          [in]    dstEndPoint   Destination endpoint
 *          [in]    srcEndPoint   Source endpoint
 *          [in]    aClusterId    Cluster Id
 */
void AF_PrepareForUnicast
  (
   afAddrInfo_t *pAddrInfo, 
   zbNwkAddr_t aNwkAddr, 
   zbEndPoint_t dstEndPoint, 
   zbEndPoint_t srcEndPoint,
   zbClusterId_t  aClusterId
  );

/*!
 * @fn 		  void AF_PrepareForIndirectUnicast(afAddrInfo_t *pAddrInfo, zbEndPoint_t srcEndPoint, zbClusterId_t  aClusterId)
 *
 * @brief	  Prepares the address information for a direct unicast data
 *          data transmission by using the 16-bit address. It uses link key
 *          security
 *
 * @param   [out]   pAddrInfo     Pointer to the reply addressing information
 *          [in]    srcEndPoint   Source endpoint
 *          [in]    aClusterId    Cluster Id
 */
void AF_PrepareForIndirectUnicast
  (
   afAddrInfo_t *pAddrInfo, 
   zbEndPoint_t srcEndPoint,
   zbClusterId_t  aClusterId
  );
    
/*!
 * @fn 		  zbStatus_t AF_MsgAppendData(afToApsdeMessage_t *pMsg, void* pData, uint16_t iDataSize)
 *
 * @brief	  Appends data to the payload of an existing afToApsde Message. It allocates 
 *          the buffers for the payload internally.
 *          This function is used when gFragmentationCapability_d is set to TRUE.
 *
 * @param   [in] pMsg       Pointer to the AfToApsde Message
 *          [in] pData      Pointer to the data payload 
 *          [in] iDataSize  Data payload length
 *
 * @return  zbSuccess_c             If worked.
 *          zbNoMem_c               If not enough memory to process request.
 */
zbStatus_t AF_MsgAppendData
(
  afToApsdeMessage_t *pMsg, 
  void* pData, 
  uint16_t iDataSize
);

/*!
 * @fn 		  zbStatus_t AF_MsgSetOffsetData(afToApsdeMessage_t *pMsg, uint16_t iMsgOffset, void* pData, uint16_t iDataSize)
 *
 * @brief	  Overwrites the data payload starting from a specified offset in the message.
 *          It is aware of link list created messages.
 *          This function is used when gFragmentationCapability_d is set to TRUE.
 *
 * @param   [in] pMsg       Pointer to the AfToApsde Message
 *          [in] iMsgOffset Offset in the message payload from where to start setting data
 *          [in] pData      Pointer to the data payload 
 *          [in] iDataSize  Data payload length
 *
 * @return  zbSuccess_c             If worked.
 *          zbNoMem_c               If not enough memory to process request.
 */              
zbStatus_t AF_MsgSetOffsetData
(
  afToApsdeMessage_t *pMsg, 
  uint16_t iMsgOffset, 
  void* pData, 
  uint16_t iDataSize
);

/*!
 * @fn 		  zbStatus_t AF_MsgGetOffsetData(zbApsdeDataIndication_t *pIndication, uint16_t iMsgOffset, void* pData, uint8_t iDataSize)
 *
 * @brief	  Overwrites the data payload starting from a specified offset in the message.
 *          It is aware of link list created messages.
 *          This function is used when gFragmentationCapability_d is set to TRUE.
 *
 * @param   [in] pIndication  Pointer to the ApsdeDataIndication
 *          [in] iMsgOffset   Offset in the message payload from where to start setting data
 *          [in] pData        Pointer to the data payload 
 *          [in] iDataSize    Data payload length
 *
 * @return  zbSuccess_c             If worked.
 *          zbNoMem_c               If not enough memory to process request.
 */              
zbStatus_t AF_MsgGetOffsetData
(
  zbApsdeDataIndication_t *pIndication, 
  uint16_t iMsgOffset, 
  void* pData, 
  uint8_t iDataSize
 );

/*!
 * @fn 		  zbStatus_t AF_InterPanDataRequest (
 *                             afAddrInfo_t *pAddrInfo, 
 *                             uint8_t payloadLen, 
 *                             void *pPayload, 
 *                             zbApsConfirmId_t *pConfirmId
 *                             )
 *
 * @brief	   Sends out an InterPan Data request out of the radio. 
 *           Note only 1 request pending  is supported.
 *
 * @param   [in] pAddrInfo    Pointer to the addressing info
 *          [in] payloadLen   Length of the payload
 *          [in] pPayload     Pointer to data payload
 *          [in] pConfirmId   Pointer to the Confirm Id
 *
 * @return  zbSuccess_c             If worked.
 *          zbNoMem_c               If not enough memory to process request.
 *          gZbBusy_c               Message can not be sent (1 request is already pending.
 *          gZbInvalidParameter_c   If invalid parameter.
 */
zbStatus_t AF_InterPanDataRequest
  (
  InterPanAddrInfo_t *pAddrInfo,/* IN: source and destination information */
  uint8_t payloadLen,           /* IN: length of payload */
  void *pPayload,               /* IN: payload (will be copied from here to a MSG) */
  zbApsConfirmId_t  *pConfirmId   /* IN: point to a confirm ID if needed */
  );

/* Application Framework's main task */  
void TS_AfTask(tsEvent_t events); 

/* Application Framework init code */
void TS_AfTaskInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _APP_AF_INTERFACE_H_ */

