/*! @file 	ZllCommissioning.h
 *
 * @brief	Types, definitions and prototypes for Zll Commissioning cluster implementition(touchlink commissioning).
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
 *			list of conditions and the following disclaimer in the documentation and/or
 *			other materials provided with the distribution.
 *
 *			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *			contributors may be used to endorse or promote products derived from this
 *			software without specific prior written permission.
 *
 *			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *			ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *			WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 *			ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *			(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *			LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *			ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *			(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *			SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
 /* 
 *  [R1] - docs-11-0037-10-0zll-zigbee-light-link-zll-profile-specification.pdf
 *  [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf 
 */
#ifndef _ZLL_COMMISSIONING_H
#define _ZLL_COMMISSIONING_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZclOptions.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/
/* Zll Profile constants */
#define gZllAplcInterPanTransIdLifeTime_d               8000    /* miliseconds */
#define gZllAplcMaxPermitJoinDuration_d                 60000   /* miliseconds */
#define gZllAplcMaxLostParentRetryAttempts_d            10000   /* miliseconds */
#define gZllAplcMaxPollInterval_d                       3600    /* seconds */
#define gZllAplcMinStartupDelayTime_d                   2000    /* miliseconds */
#define gZllAplcRxWindowDuration_d                      5000    /* miliseconds */
#define gZllAplcScanTimeBaseDuration_d                  250     /* miliseconds */
#define gZllAplcMinChildPersistenceTime_d               4*gZllAplcMaxPollInterval_d /* seconds */

/* ZLl Profile attributes default values */
#define gZllAplFreeNwkAddRangeBegin_d                   0x0001
#define gZllAplFreeNwkAddRangeEnd_d                     0xFFF7
#define gZllAplGroupIdRangeBegin_d                      0x0001
#define gZllAplGroupIdRangeEnd_d                        0xFEFF


#if(gInstantiableStackEnabled_d == 0)
  #define ZllTouchlinkConfigData(val)     val
#else
  #define ZllTouchlinkConfigData(val)      pZllTouchlinkData->val
#endif

/**********************************************
      [R1] 7.1. ZLl commissioning Cluster 
**********************************************/
/* Zll commissioning Attributes Sets [R1] - 7.1.2.1 */
    
    /* no attributes*/


/* Zll commissioning cluster commands:  */

/* [R1] - 7.1.2.2 Commands Received:  */
/* touchlink commands: */
#define gZclCmdZllCommissioning_ScanReq_c                   0x00    /* M - scan request */
#define gZclCmdZllCommissioning_DeviceInformationReq_c      0x02    /* M - device information request */
#define gZclCmdZllCommissioning_IdentifyReq_c               0x06    /* M - identify request */
#define gZclCmdZllCommissioning_ResetToFactoryNewReq_c      0x07    /* M - reset to factory new request */
#define gZclCmdZllCommissioning_NetworkStartReq_c           0x10    /* M - network start request */
#define gZclCmdZllCommissioning_NetworkJoinRouterReq_c      0x12    /* M - network join router request */
#define gZclCmdZllCommissioning_NetworkJoinEndDeviceReq_c   0x14    /* M - network join endDevice request */
#define gZclCmdZllCommissioning_NetworkUpdateReq_c          0x16    /* M - network update request */
                                                /* all other values in the range 0x00 - 0x3F = reserved */
/* utility commands: */
#define gZclCmdZllCommissioning_GetGroupIdReq_c             0x41    /* M - get group identifiers request */
#define gZclCmdZllCommissioning_GetEndpointListReq_c        0x42    /* M - get endpoint list request */
                                                /* all other values in the range 0x40 - 0xFF = reserved */

/* [R1] - 7.1.2.3 Commands Generated:  */
/* touchlink commands: */
#define gZclCmdZllCommissioning_ScanRsp_c                   0x01    /* M - scan response */
#define gZclCmdZllCommissioning_DeviceInformationRsp_c      0x03    /* M - device information response */
#define gZclCmdZllCommissioning_NetworkStartRsp_c           0x11    /* M - network start response */
#define gZclCmdZllCommissioning_NetworkJoinRouterRsp_c      0x13    /* M - network join router response */
#define gZclCmdZllCommissioning_NetworkJoinEndDeviceRsp_c   0x15    /* M - network join endDevice response */
                                                /* all other values in the range 0x00 - 0x3F = reserved */
/* utility commands: */
#define gZclCmdZllCommissioning_EndpointInformation_c       0x40    /* M - endpoint information */
#define gZclCmdZllCommissioning_GetGroupIdRsp_c             0x41    /* M - get group identifiers response */
#define gZclCmdZllCommissioning_GetEndpointListRsp_c        0x42    /* M - get endpoint list response */
                                                /* all other values in the range 0x40 - 0xFF = reserved */


/* Zll commissioning cluster informations: */

/* [R1] 7.1.2.2.3.2 Identify Duration Field */
#define gZllCommissioning_ExitIdentifyMode_d            0x0000 /* exit identify mode */
#define gZllCommissioning_KeepIdentifyMode_d            0xFFFF /* keep identify mode for a default time known by the receiver */

/* ZLL security */

/* development key (key index 0): = "Phli", transaction identifier from scanRequest, "CLSN", transaction identifier of the scan response 
                                  - please update the 0xFF's with the transaction identifiers */
#define gZllCommissioning_DevelopmentKey_d              0x50, 0x68, 0x4c, 0x69, 0xff, 0xff, 0xff, 0xff, 0x43, 0x4c, 0x53, 0x4e, 0xff, 0xff, 0xff, 0xff  
#define gZllCommissioning_DevelopmentKeyIndex_d         0x00

/* master key (key index 4): secret shared by all certified Zll devices. Distributed only to certified manufacturers  */
#define gZllCommissioning_MasterKey_d                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff 
#define gZllCommissioning_MasterKeyIndex_d              0x04

/* certification key (key index 15) */
#define gZllCommissioning_CertificationKey_d            0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
#define gZllCommissioning_CertificationKeyIndex_d       0x0F 

/* invalid key */
#define gZllCommissioning_InvalidKey_d                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
#define gZllCommissioning_InvalidKeyIndex_d             0x04

/* Zll Device Discovery */
#define gZllTouchlink_StartCommissioning_c             (0x01<<0)
#define gZllTouchlink_DeviceInformationReq_c           (0x01<<1)
#define gZllTouchlink_IdentifyReq_c                    (0x01<<2)                      
#define gZllTouchlink_FactoryNewReq_c                  (0x01<<3)
#define gZllTouchlink_ClasicalCommissioning_c          (0x01<<4)
#define gZllTouchlink_FactoryFresh_c                   (0x01<<5)
                                    
#define gZllTouchlink_DefaultFunctionality_c           gZllTouchlink_IdentifyReq_c|gZllTouchlink_StartCommissioning_c                              
                                    
/* RSSI informations */
#define gZllCommissioning_MinRssiValue_d                0x80     /* Rssi min value accepted */
#define gZllCommissioning_RssiCorrection_d              0x00     /* pre-programmed Rssi correction offset: 0x00-0x20 */
#define gZLLCommissioning_MaxRemoteDevices_c            0x08     /* max no of remote devices supported  */       
#define gZLLCommissioning_MaxNoOfGroups_c              gApsMaxGroups_c     /* max no of goups supported */
                                    
/* channels inf */
#define gZllCommissioningDiscoveryCh11ScanReq_d         0x05  
#define gZllCommissioningDiscoveryPrimaryChScanReq_d    gZllCommissioningDiscoveryCh11ScanReq_d + 3 /* primaryChannels(5* ch11, 1*ch15, 1*ch20, 1*ch25) */     

#define gZllCommissioningDefaultIdentifyDuration_d      0x05
#define gZllCommissioningMaxEndpointCount_c             0x02    /* used to store endpoint information*/
                                    
#if gCoordinatorCapability_d
#define gZllCommissioningDefaultDeviceType_d            gZdoStartMode_Zc_c
#else 
#if gEndDevCapability_d
#define gZllCommissioningDefaultDeviceType_d            gZdoStartMode_Zed_c
#else  
#if gRouterCapability_d  
#define gZllCommissioningDefaultDeviceType_d            gZdoStartMode_Zr_c
#else
#define gZllCommissioningDefaultDeviceType_d            gZdoStartMode_Zr_c
#endif /* gRouterCapability_d */
#endif /* gEndDevCapability_d */
#endif /* gCoordinatorCapability_d */

#if(gInstantiableStackEnabled_d == 1)                                 
                                    
#if gCoordinatorCapability_d
#define gZllCommissioningDefaultDeviceTypePan1_d        gZdoStartMode_Zc_c
#else 
#if gEndDevCapability_d
#define gZllCommissioningDefaultDeviceTypePan1_d        gZdoStartMode_Zed_c
#else  
#if gRouterCapability_d  
#define gZllCommissioningDefaultDeviceTypePan1_d        gZdoStartMode_Zr_c
#else
#define gZllCommissioningDefaultDeviceTypePan1_d        gZdoStartMode_Zr_c
#endif /* gRouterCapability_d */
#endif /* gEndDevCapability_d */
#endif /* gCoordinatorCapability_d */                                   
 
#endif                                                                       
                                    
/* ZLL primary channels mask */
#ifndef gZllCommissioningPrimaryChannelsMask_c
  #define gZllCommissioningPrimaryChannelsMask_c        0x02108800
#endif

                                    
/* ZLL primary channels mask array */
#ifndef gaZllCommissioningPrimaryChannelsMask_c
  #define gaZllCommissioningPrimaryChannelsMask_c        0x00, 0x88, 0x10, 0x02  
#endif

                                                                        
/* ZLL secondary channels mask array */
#ifndef gaZllCommissioningSecondaryChannelsMask_c
  #define gaZllCommissioningSecondaryChannelsMask_c      0x00, 0x70, 0xEF, 0x05
#endif
                                   

#if gASL_EnableZllTouchlinkCommissioning_d
#ifndef gASL_EnableZllCommissioning_Initiator_d
#define gASL_EnableZllCommissioning_Initiator_d         gASL_EnableEZCommissioning_Initiator_d
#endif
#else
#ifndef gASL_EnableZllCommissioning_Initiator_d
#define gASL_EnableZllCommissioning_Initiator_d         FALSE
#endif
#endif

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/***********************************************************/
/* touchlink commands received format (interpan commands): */
/***********************************************************/

/* [R1] 7.1.2.2.1.2 zigbee information field*/
typedef PACKED_STRUCT  zllCommissioning_ZigbeInf_tag
{  
  unsigned char logicalType     :2;
  unsigned char rxOnIdle        :1;
  unsigned char reserved        :5;
} zllCommissioning_ZigbeInf_t;

/* [R1] 7.1.2.2.1.2 zll information field*/
typedef PACKED_STRUCT  zllCommissioning_ZllInf_tag
{  
  unsigned char factoryNew      :1;
  unsigned char addrAssignment  :1;
  unsigned char reserved1       :2;
  unsigned char initiator       :1;
  unsigned char priorityReq     :1;
  unsigned char reserved2       :2;  
} zllCommissioning_ZllInf_t;

/* [R1] 7.1.2.2.1 payload format for the ScanRequest command*/
typedef PACKED_STRUCT  zllCmdCommissioning_ScanRequest_tag
{  
  uint32_t                          interPanTransactionId;
  zllCommissioning_ZigbeInf_t       zbInf;
  zllCommissioning_ZllInf_t         zllInf;
} zllCmdCommissioning_ScanRequest_t;

/* [R1] 7.1.2.2.1 ScanRequest command*/
typedef PACKED_STRUCT  zllCommissioning_ScanRequest_tag
{  
  InterPanAddrInfo_t                    addrInfo;
  uint8_t                               zclTransactionId;
  zllCmdCommissioning_ScanRequest_t     cmdFrame;
} zllCommissioning_ScanRequest_t;

/* [R1] 7.1.2.2.2 payload format for the Device information request command*/
typedef PACKED_STRUCT  zllCmdCommissioning_DeviceInformationReq_tag
{  
  uint32_t      interPanTransactionId;
  uint8_t       startIndex;
} zllCmdCommissioning_DeviceInformationReq_t;

/* [R1] 7.1.2.2.2 Device information request command*/
typedef PACKED_STRUCT  zllCommissioning_DeviceInformationReq_tag
{  
  InterPanAddrInfo_t                            addrInfo;
  uint8_t                                       zclTransactionId;
  zllCmdCommissioning_DeviceInformationReq_t    cmdFrame;
} zllCommissioning_DeviceInformationReq_t;

/* [R1] 7.1.2.2.3 payload format for the Identify request command*/
typedef PACKED_STRUCT  zllCmdCommissioning_IdentifyReq_tag
{  
  uint32_t      interPanTransactionId;
  uint16_t      identifyDuration;
} zllCmdCommissioning_IdentifyReq_t;

/* [R1] 7.1.2.2.3 Identify request command*/
typedef PACKED_STRUCT  zllCommissioning_IdentifyReq_tag
{  
  InterPanAddrInfo_t                    addrInfo;
  uint8_t                               zclTransactionId;
  zllCmdCommissioning_IdentifyReq_t     cmdFrame;
} zllCommissioning_IdentifyReq_t;

/* [R1] 7.1.2.2.4 payload format for the Reset to Factory New Request command*/
typedef PACKED_STRUCT  zllCmdCommissioning_ResetFactoryReq_tag
{  
  uint32_t      interPanTransactionId;
} zllCmdCommissioning_ResetFactoryReq_t;

/* [R1] 7.1.2.2.4  Reset to Factory New Request command*/
typedef PACKED_STRUCT  zllCommissioning_ResetFactoryReq_tag
{  
  InterPanAddrInfo_t                    addrInfo;
  uint8_t                               zclTransactionId;
  zllCmdCommissioning_ResetFactoryReq_t cmdFrame;
} zllCommissioning_ResetFactoryReq_t;

/* [R1] 7.1.2.2.5 payload format for the Network Start Request command*/
typedef PACKED_STRUCT  zllCmdCommissioning_NetworkStartReq_tag
{  
  uint32_t      interPanTransactionId;
  zbIeeeAddr_t  extendedPanId;
  uint8_t       keyIndex;
  zbAESKey_t    encryptedNtwKey;
  uint8_t       logicalChannel;
  zbPanId_t     panId;
  zbNwkAddr_t   ntwAddress;
  uint16_t      groupIdBegin;
  uint16_t      groupIdEnd; 
  zbNwkAddr_t   freeNtwAddrRangeBegin;  
  zbNwkAddr_t   freeNtwAddrRangeEnd;  
  uint16_t      freeGroupIdRangeBegin;
  uint16_t      freeGroupIdRangeEnd;  
  zbIeeeAddr_t  initiatorIeeeAddress;  
  zbNwkAddr_t   initiatorNtwAddress;  
} zllCmdCommissioning_NetworkStartReq_t;

/* [R1] 7.1.2.2.5  Network Start Request command*/
typedef PACKED_STRUCT  zllCommissioning_NetworkStartReq_tag
{  
  InterPanAddrInfo_t                    addrInfo;
  uint8_t                               zclTransactionId;
  zllCmdCommissioning_NetworkStartReq_t cmdFrame;
} zllCommissioning_NetworkStartReq_t;

/* [R1] 7.1.2.2.6 payload format for the Network Join Router Request command*/
typedef PACKED_STRUCT  zllCmdCommissioning_NtwJoinRouterReq_tag
{  
  uint32_t      interPanTransactionId;
  zbIeeeAddr_t  extendedPanId; 
  uint8_t       keyIndex;
  zbAESKey_t    encryptedNtwKey;
  uint8_t       nwkUpdateId;
  uint8_t       logicalChannel;
  zbNwkAddr_t   panId;
  zbNwkAddr_t   ntwAddress;
  uint16_t      groupIdBegin;
  uint16_t      groupIdEnd; 
  zbNwkAddr_t   freeNtwAddrRangeBegin;  
  zbNwkAddr_t   freeNtwAddrRangeEnd;  
  uint16_t      freeGroupIdRangeBegin;
  uint16_t      freeGroupIdRangeEnd;   
} zllCmdCommissioning_NtwJoinRouterReq_t;

/* [R1] 7.1.2.2.6  Network Join Router Request command*/
typedef PACKED_STRUCT  zllCommissioning_NtwJoinRouterReq_tag
{  
  InterPanAddrInfo_t                     addrInfo;
  uint8_t                                zclTransactionId;
  zllCmdCommissioning_NtwJoinRouterReq_t cmdFrame;
} zllCommissioning_NtwJoinRouterReq_t;

/* [R1] 7.1.2.2.7 payload format for the Network Join EndDevice Request command*/
typedef PACKED_STRUCT  zllCmdCommissioning_NtwJoinEndDeviceReq_tag
{  
  uint32_t      interPanTransactionId;
  zbIeeeAddr_t  extendedPanId; 
  uint8_t       keyIndex;
  zbAESKey_t    encryptedNtwKey;
  uint8_t       nwkUpdateId;
  uint8_t       logicalChannel;
  zbNwkAddr_t   panId;
  zbNwkAddr_t   ntwAddress;
  uint16_t      groupIdBegin;
  uint16_t      groupIdEnd; 
  zbNwkAddr_t   freeNtwAddrRangeBegin;  
  zbNwkAddr_t   freeNtwAddrRangeEnd;  
  uint16_t      freeGroupIdRangeBegin;
  uint16_t      freeGroupIdRangeEnd;   
} zllCmdCommissioning_NtwJoinEndDeviceReq_t;

/* [R1] 7.1.2.2.7  Network Join EndDevice Request command*/
typedef PACKED_STRUCT  zllCommissioning_NtwJoinEndDeviceReq_tag
{  
  InterPanAddrInfo_t                            addrInfo;
  uint8_t                                       zclTransactionId;
  zllCmdCommissioning_NtwJoinEndDeviceReq_t     cmdFrame;
} zllCommissioning_NtwJoinEndDeviceReq_t;


/* [R1] 7.1.2.2.8 payload format for the Network Update Request command*/
typedef PACKED_STRUCT  zllCmdCommissioning_NtwUpdateReq_tag
{  
  uint32_t      interPanTransactionId;
  zbIeeeAddr_t  extendedPanId; 
  uint8_t       nwkUpdateId;
  uint8_t       logicalChannel;
  zbNwkAddr_t   panId;
  zbNwkAddr_t   ntwAddress;  
} zllCmdCommissioning_NtwUpdateReq_t;

/* [R1] 7.1.2.2.8  Network Update Request command*/
typedef PACKED_STRUCT  zllCommissioning_NtwUpdateReq_tag
{  
  InterPanAddrInfo_t                     addrInfo;
  uint8_t                                zclTransactionId;
  zllCmdCommissioning_NtwUpdateReq_t     cmdFrame;
} zllCommissioning_NtwUpdateReq_t;


/***********************************************************/
/* touchlink commands generated format (interpan commands): */
/***********************************************************/

/* [R1] 7.1.2.3.1 payload format for the Scan Response command */
typedef PACKED_STRUCT  zllCmdCommissioning_ScanResponse_tag
{  
  uint32_t                              interPanTransactionId;
  uint8_t                               rssiCorrection;
  zllCommissioning_ZigbeInf_t           zbInf;
  zllCommissioning_ZllInf_t             zllInf;
  uint16_t                              keyBitmask;
  uint32_t                              responseId;
  zbIeeeAddr_t                          extendedPanId;
  uint8_t                               nwkUpdateId;
  uint8_t                               logicalChannel;
  zbPanId_t                             panId;
  zbNwkAddr_t                           nwkAddr;
  uint8_t                               noOfSubdevices;
  uint8_t                               totalGroups;
  uint8_t                               endPoint;
  zbProfileId_t                         profileId;
  zbProfileId_t                         deviceId;
  uint8_t                               version;
  uint8_t                               groupIdCount;
} zllCmdCommissioning_ScanResponse_t;

/* [R1] 7.1.2.3.1  Scan Response command */
typedef PACKED_STRUCT  zllCommissioning_ScanResponse_tag
{  
  InterPanAddrInfo_t                     addrInfo;
  uint8_t                                zclTransactionId;
  zllCmdCommissioning_ScanResponse_t     cmdFrame;
} zllCommissioning_ScanResponse_t;

/* [R1] 7.1.2.3.2 format of the device information record field */
typedef PACKED_STRUCT  zllCommissioning_DeviceInfRecord_tag
{  
  zbIeeeAddr_t  ieeeAddress;
  uint8_t       endpointId;
  zbProfileId_t profileId;
  zbProfileId_t deviceId;
  uint8_t       version;
  uint8_t       groupIdCount;
  uint8_t       sort;
} zllCommissioning_DeviceInfRecord_t;

/* [R1] 7.1.2.3.2 payload format for the Device Information Response command */
typedef PACKED_STRUCT  zllCmdCommissioning_DeviceInfRsp_tag
{  
  uint32_t                              interPanTransactionId;
  uint8_t                               noOfSubDevices;
  uint8_t                               startIndex;
  uint8_t                               deviceInfRecordCount;
  zllCommissioning_DeviceInfRecord_t    deviceInfRecord[1];
} zllCmdCommissioning_DeviceInfRsp_t;

/* [R1] 7.1.2.3.2 Device Information Response command */
typedef PACKED_STRUCT  zllCommissioning_DeviceInfRsp_tag
{  
  InterPanAddrInfo_t                     addrInfo;
  uint8_t                                zclTransactionId;
  zllCmdCommissioning_DeviceInfRsp_t     cmdFrame;
} zllCommissioning_DeviceInfRsp_t;

/* [R1] 7.1.2.3.3 payload format for the Network Start Response command */
typedef PACKED_STRUCT  zllCmdCommissioning_NtwStartRsp_tag
{  
  uint32_t      interPanTransactionId;
  uint8_t       status;
  zbIeeeAddr_t  extendedPanId;
  uint8_t       nwkUpdateId;
  uint8_t       logicalChannel;
  zbPanId_t     panId; 
} zllCmdCommissioning_NtwStartRsp_t;

/* [R1] 7.1.2.3.3 Network Start Response command */
typedef PACKED_STRUCT  zllCommissioning_NtwStartRsp_tag
{  
  InterPanAddrInfo_t                     addrInfo;
  uint8_t                                zclTransactionId;
  zllCmdCommissioning_NtwStartRsp_t      cmdFrame;
} zllCommissioning_NtwStartRsp_t;

/* [R1] 7.1.2.3.4 payload format for the Network Join Router Response command */
typedef PACKED_STRUCT  zllCmdCommissioning_NtwJoinRouterRsp_tag
{  
  uint32_t      interPanTransactionId;
  uint8_t       status;
} zllCmdCommissioning_NtwJoinRouterRsp_t;

/* [R1] 7.1.2.3.4 Network Join Router Responsecommand */
typedef PACKED_STRUCT  zllCommissioning_NtwJoinRouterRsp_tag
{  
  InterPanAddrInfo_t                            addrInfo;
  uint8_t                                       zclTransactionId;
  zllCmdCommissioning_NtwJoinRouterRsp_t        cmdFrame;
} zllCommissioning_NtwJoinRouterRsp_t;

/* [R1] 7.1.2.3.5 payload format for the Network Join End Device Response command */
typedef PACKED_STRUCT  zllCmdCommissioning_NtwJoinEndDeviceRsp_tag
{  
  uint32_t      interPanTransactionId;
  uint8_t       status;
} zllCmdCommissioning_NtwJoinEndDeviceRsp_t;

/* [R1] 7.1.2.3.5 Network Join End Device Responsecommand */
typedef PACKED_STRUCT  zllCommissioning_NtwJoinEndDeviceRsp_t
{  
  InterPanAddrInfo_t                            addrInfo;
  uint8_t                                       zclTransactionId;
  zllCmdCommissioning_NtwJoinEndDeviceRsp_t     cmdFrame;
} zllCommissioning_NtwJoinEndDeviceRsp_t;

/*************************************/
/* utility commands received format: */
/*************************************/
/* [R1] 7.1.2.2.9 payload format for the Get group identifiers Request command*/
typedef PACKED_STRUCT  zllCmdCommissioning_GetGroupIdReq_tag
{  
  uint8_t startIndex;
} zllCmdCommissioning_GetGroupIdReq_t;

/* [R1] 7.1.2.2.9  Get group identifiers Request command*/
typedef PACKED_STRUCT  zllCommissioning_GetGroupIdReq_tag
{  
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zllCmdCommissioning_GetGroupIdReq_t   cmdFrame;
} zllCommissioning_GetGroupIdReq_t;

/* [R1] 7.1.2.2.10 payload format for the Get endpoint list Request command*/
typedef PACKED_STRUCT  zllCmdCommissioning_GetEndpointListReq_tag
{  
  uint8_t startIndex;
} zllCmdCommissioning_GetEndpointListReq_t;

/* [R1] 7.1.2.2.10  Get endpoint list Request command*/
typedef PACKED_STRUCT  zllCommissioning_GetEndpointListReq_tag
{  
  afAddrInfo_t                                  addrInfo;
  uint8_t                                       zclTransactionId;
  zllCmdCommissioning_GetEndpointListReq_t      cmdFrame;
} zllCommissioning_GetEndpointListReq_t;


/**************************************/
/* utility commands generated format: */
/**************************************/

/* [R1] 7.1.2.3.6 payload format for the endpoint information command*/
typedef PACKED_STRUCT  zllCmdCommissioning_EndpointInformation_tag
{  
  zbIeeeAddr_t  ieeeAddress;
  zbNwkAddr_t   nwkAddress;
  uint8_t       endpointId;
  zbProfileId_t profileId;
  zbProfileId_t deviceId;
  uint8_t       version;
} zllCmdCommissioning_EndpointInformation_t;

/* [R1] 7.1.2.3.6  endpoint information command*/
typedef PACKED_STRUCT  zllCommissioning_EndpointInformation_tag
{  
  afAddrInfo_t                                  addrInfo;
  uint8_t                                       zclTransactionId;
  zllCmdCommissioning_EndpointInformation_t     cmdFrame;
} zllCommissioning_EndpointInformation_t;

typedef PACKED_STRUCT  zllCommissioning_SendEndpointInf_tag
{  
  afAddrInfo_t                                  addrInfo;
  uint8_t                                       zclTransactionId;
} zllCommissioning_SendEndpointInf_t;


/* [R1] 7.1.2.3.7 group information record*/
typedef PACKED_STRUCT  zllCommissioning_GroupInfRecord_tag
{  
  uint16_t      groupId;
  uint8_t       groupType;
} zllCommissioning_GroupInfRecord_t;

/* [R1] 7.1.2.3.7 payload format for the Get group identifiers response command*/
typedef PACKED_STRUCT  zllCmdCommissioning_GetGroupIdRsp_tag
{  
  uint8_t                               total;
  uint8_t                               startIndex;
  uint8_t                               count;
  zllCommissioning_GroupInfRecord_t     groupInfRecord[1];
} zllCmdCommissioning_GetGroupIdRsp_t;

/* [R1] 7.1.2.3.7  Get group identifiers Response command*/
typedef PACKED_STRUCT  zllCommissioning_GetGroupIdRsp_tag
{  
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zllCmdCommissioning_GetGroupIdRsp_t   cmdFrame;
} zllCommissioning_GetGroupIdRsp_t;

/* [R1] 7.1.2.3.8 endpoint information record*/
typedef PACKED_STRUCT  zllCommissioning_EndpointInfRecord_tag
{  
  zbNwkAddr_t   nwkAddress;
  uint8_t       endpointId;
  zbProfileId_t profileId;
  zbProfileId_t deviceId;
  uint8_t       version;
} zllCommissioning_EndpointInfRecord_t;

/* [R1] 7.1.2.3.8 payload format for the Get endpoint list Response command*/
typedef PACKED_STRUCT  zllCmdCommissioning_GetEndpointListRsp_tag
{  
  uint8_t                                       total;
  uint8_t                                       startIndex;
  uint8_t                                       count;
  zllCommissioning_EndpointInfRecord_t          endpointInfRecord[1];
} zllCmdCommissioning_GetEndpointListRsp_t;

/* [R1] 7.1.2.3.8  Get endpoint list Response command*/
typedef PACKED_STRUCT  zllCommissioning_GetEndpointListRsp_tag
{  
  afAddrInfo_t                                  addrInfo;
  uint8_t                                       zclTransactionId;
  zllCmdCommissioning_GetEndpointListRsp_t      cmdFrame;
} zllCommissioning_GetEndpointListRsp_t;


/* [R1] 7.1.2.3.3.2 Status Field */
enum
{
  gZllCommissioning_StatusSuccess_d = 0x00,     /* succes */
  gZllCommissioning_StatusFailed_d              /* failed */
};


/* zll commissioning touchlink  initiator session */
typedef PACKED_STRUCT zllCommissioning_TouchlinkInitiatorSession_tag
{
  uint8_t       ntwLogicalChannel;
  uint8_t       zllDeviceDiscoveryCounter;
  uint8_t       keyIndexSent;
  zbAESKey_t    tempEncrNtwKey;
  uint8_t       functionality;
  uint8_t       deviceConfigStartIndex;
}zllCommissioning_TouchlinkInitiatorSession_t;


/* remote device information (initiator)*/
typedef PACKED_STRUCT  zllCommissioning_TouchlinkRemoteTarget_tag
{ 
  uint8_t       dstAddrMode;
  zbApsAddr_t   dstAddr; 
  zbNwkAddr_t   ntwAddress;
  zbPanId_t     destPanId;
  zbIeeeAddr_t  extendedPanId;
  uint8_t       endPoint[gZllCommissioningMaxEndpointCount_c];
  zbProfileId_t profileId;
  zbProfileId_t deviceId[gZllCommissioningMaxEndpointCount_c];
  uint8_t       ntwUpdateId;
  uint8_t       version;
  uint8_t       groupIdCount; 
  uint8_t       logicalChannel;
  uint8_t       noOfSubdevices;
  uint8_t       keyIndex;
  uint8_t       rssiValue;
  zllCommissioning_ZigbeInf_t   zbInf;
  zllCommissioning_ZllInf_t     zllInf;      
} zllCommissioning_TouchlinkRemoteTarget_t;

/* Zll commissioning target state */
enum
{
  gZllCommissioning_InitialState_d = 0x00,              /* Initial State */
  gZllCommissioning_ClassicalCommissioning_d,           /* Clasical Commissioning */
  gZllCommissioning_ClassicalCommissioningCombo_d,      /* Clasical Commissioning Combo Devices*/
  gZllCommissioning_JoinFormNtwState_d,                 /* Join/Form the network */
  gZllCommissioningTarget_NetworkStart_d,               /* Network Start */       
  gZllCommissioningTarget_FormNetwork_d,                /* Form the network */   
  gZllCommissioningTarget_JoinNetwork_d,                /* join the network */  
  gZllCommissioningTarget_CreateNeighTableEntry_d,      /* NLME-Direct-Join */
  gZllCommissioningInitiator_ScanReq_d,                 /* ScanReq */
  gZllCommissioningInitiator_DeviceInfReq_d,            /* Device Information Req */     
  gZllCommissioningInitiator_IdentifyReq_d,             /* Identify Req */   
  gZllCommissioningInitiator_NetworkStart_d,            /* Network Start */    
  gZllCommissioningInitiator_NetworkJoinReq_d,          /* NetworkJoinReq*/    
  gZllCommissioningInitiator_NetworkUpdateReq_d,        /* Network Update */
  gZllCommissioningInitiator_NetworkRejoin_d,           /* Network Rejoin */    
  gZllCommissioningInitiator_SendNetworkRejoin_d,       /* Process network rejoin */
  gZllCommissioningInitiator_ResetToFactoryNew_d        /* reset to factory new */     
};

/*  zll commissioning touchlink session */
typedef struct zllCommissioning_TouchlinkSession_tag
{
  uint8_t               state;
  uint8_t               keyIndex;
  zbIeeeAddr_t          extendedPanId;
  zbPanId_t             panId;
  zbAESKey_t            encryptedNtwKey;
  uint8_t               logicalChannel;
  uint8_t               ntwUpdateId;
  zbNwkAddr_t           ntwAddress;
  uint16_t              groupIdBegin;
  uint16_t              groupIdEnd;
  zbIeeeAddr_t          initiatorIeeeAddress;  
  zbNwkAddr_t           initiatorNtwAddress; 
  uint8_t               initiatorCapabilityInf;
  uint8_t               minRssi;
  uint16_t              identifyDuration;
  zbTmrTimerID_t          zllTouchlinkTmr;
  InterPanAddrInfo_t    remoteDevAddrInfo;
}zllCommissioning_TouchlinkSession_t;

/* zll commissioning touchlink - group table  */
typedef PACKED_STRUCT  zllTouchlink_GroupTable_tag
{  
  uint16_t      groupId;
} zllTouchlink_GroupTable_t;

/*  zll commissioning touchlink common(target/initiator) setup param */
typedef struct zllCommissioning_TouchlinkSetup_tag
{
  bool_t   factoryNewStatus;     
  uint16_t freeNwkAddrBegin;
  uint16_t freeNwkAddrEnd;
  uint16_t freeGroupIdBegin;
  uint16_t freeGroupIdEnd;  
#if gComboDeviceCapability_d
  uint8_t  deviceType;
#endif  
#if gASL_EnableZllCommissioning_Initiator_d  
  zllTouchlink_GroupTable_t groupTable[gZLLCommissioning_MaxNoOfGroups_c];
#endif  
}zllCommissioning_TouchlinkSetup_t;

/*  zll commissioning touchlink start param */
typedef PACKED_STRUCT zllTouchlink_Start_tag
{
  uint8_t  deviceType; 
  bool_t   deviceRole;
  uint8_t  functionality;
}zllTouchlink_Start_t;

/* zll commissioning touchlink configure param */
typedef PACKED_STRUCT zllTouchlink_Configure_tag
{
  uint8_t  minRssi;
  uint8_t  deviceConfigStartIndex;
  uint16_t idenfityDuration;
}zllTouchlink_Configure_t;

/* zll commissioning touchlink - remote table  */
typedef PACKED_STRUCT  zllTouchlink_RemoteDeviceTable_tag
{  
  zbNwkAddr_t   ntwAddress;
  zbIeeeAddr_t  ieeeAddress;
  uint8_t       endpointId[gZllCommissioningMaxEndpointCount_c];
  zbProfileId_t profileId;
  zbProfileId_t deviceId[gZllCommissioningMaxEndpointCount_c];
} zllTouchlink_RemoteDeviceTable_t;

/* zll commissioning touchlink - get commissioned devices*/
typedef PACKED_STRUCT zllTouchlink_GetCommDevices_tag
{
  uint8_t  startIndex;
  uint8_t  count;
}zllTouchlink_GetCommDevices_t;

#if(gInstantiableStackEnabled_d == 1)
/* Nwk module internal used ram global structure */
typedef struct zllTouchlinkData_tag
{
  zllCommissioning_TouchlinkSetup_t gTouchLinkSetup;    /* Zll commissioning touchlink setup: */
  bool_t gZllCommissioningDeviceInNonZllNtw ;    
  uint8_t gZllCommissioningDefaultDeviceType;
#if gInterPanCommunicationEnabled_c
#if gASL_EnableZllCommissioning_Initiator_d  
  bool_t mZllExtendedScanReq;
#endif   
  uint32_t mTouchLinkScanReqId;   
  uint32_t mTouchLinkScanRspId;
  zllCommissioning_TouchlinkSession_t mTouchLinkSession;
#if gComboDeviceCapability_d || gEndDevCapability_d 
  uint16_t gZllOrgPollRate;
#if gASL_EnableZllCommissioning_Initiator_d  
  bool_t gEndDevForcedTouchlink;
#endif  
#endif 
#endif /* gInterPanCommunicationEnabled_c */
#if gASL_EnableZllCommissioning_Initiator_d
  uint8_t *mpTouchLinkTempRemoteTarget;
  zllCommissioning_TouchlinkInitiatorSession_t mTouchLinkInitiatorSession;
  zllTouchlink_RemoteDeviceTable_t gZllRemoteDeviceTable[gZLLCommissioning_MaxRemoteDevices_c];
#endif /* gASL_EnableZllCommissioning_Initiator_d */
}zllTouchlinkData_t;
extern zllTouchlinkData_t *pZllTouchlinkData;
#endif 

/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/
#if gInterPanCommunicationEnabled_c
/*!
 * @fn 		zbStatus_t ZCL_ZllTouchlinkClusterServer(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the InterPAN Zll Commisisoning (Touchlink)Cluster server. 
 *
 */
zbStatus_t ZCL_ZllTouchlinkClusterServer(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t ZCL_ZllTouchlinkClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the InterPAN Zll Commisisoning (Touchlink)Cluster Client. 
 *
 */
zbStatus_t ZCL_ZllTouchlinkClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_ScanReq(zllCommissioning_ScanRequest_t *pReq)
 *
 * @brief	Sends over-the-air a Scan Request command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_ScanReq(zllCommissioning_ScanRequest_t *pReq);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_DeviceInfReq(zllCommissioning_DeviceInformationReq_t *pReq)
 *
 * @brief	Sends over-the-air a Device Information request command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_DeviceInfReq(zllCommissioning_DeviceInformationReq_t *pReq);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_IdentifyReq(zllCommissioning_DeviceInformationReq_t *pReq)
 *
 * @brief	Sends over-the-air a Identify request command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_IdentifyReq(zllCommissioning_IdentifyReq_t *pReq);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_ResetFactoryReq(zllCommissioning_ResetFactoryReq_t *pReq)
 *
 * @brief	Sends over-the-air a ResetFactoryNewRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_ResetFactoryReq(zllCommissioning_ResetFactoryReq_t *pReq);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkStartReq(zllCommissioning_NetworkStartReq_t *pReq)
 *
 * @brief	Sends over-the-air a Network start Request from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkStartReq(zllCommissioning_NetworkStartReq_t *pReq);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkJoinRouterReq(zllCommissioning_NtwJoinRouterReq_t *pReq)
 *
 * @brief	Sends over-the-air a Network join router Request from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkJoinRouterReq(zllCommissioning_NtwJoinRouterReq_t *pReq);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkJoinEndDeviceReq(zllCommissioning_NtwJoinEndDeviceReq_t *pReq)
 *
 * @brief	Sends over-the-air a Network join router Request from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkJoinEndDeviceReq(zllCommissioning_NtwJoinEndDeviceReq_t *pReq);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkUpdateReq(zllCommissioning_NtwUpdateReq_t *pReq)
 *
 * @brief	Sends over-the-air a Network join router Request from the Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkUpdateReq(zllCommissioning_NtwUpdateReq_t *pReq);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_ScanRsp(zllCommissioning_ScanResponse_t *pReq)
 *
 * @brief	Sends over-the-air a Scan Response from the Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllTouchlink_ScanRsp(zllCommissioning_ScanResponse_t *pReq);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_DeviceInfRsp(zllCommissioning_DeviceInfRsp_t *pReq)
 *
 * @brief	Sends over-the-air DeviceInf response from the Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllTouchlink_DeviceInfRsp(zllCommissioning_DeviceInfRsp_t *pReq);
/*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkStartRsp(zllCommissioning_NtwStartRsp_t *pReq)
 *
 * @brief	Sends over-the-air NetworkStartResponse from the Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkStartRsp(zllCommissioning_NtwStartRsp_t *pReq);
 /*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkJoinRouterRsp(zllCommissioning_NtwJoinRouterRsp_t *pReq)
 *
 * @brief	Sends over-the-air NetworkJoinRouterResponse from the Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkJoinRouterRsp(zllCommissioning_NtwJoinRouterRsp_t *pReq);
 /*!
 * @fn 		zbStatus_t zclZllTouchlink_NetworkJoinEndDeviceRsp(zllCommissioning_NtwJoinEndDeviceRsp_t *pReq)
 *
 * @brief	Sends over-the-air NetworkJoinEndDeviceResponse from the Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllTouchlink_NetworkJoinEndDeviceRsp(zllCommissioning_NtwJoinEndDeviceRsp_t *pReq);

#endif /* gInterPanCommunicationEnabled_c */

/*!
 * @fn 		zbStatus_t ZCL_ZllCommissioningClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t ZCL_ZllCommissioningUtilityClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_ZllCommissioningUtilityClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Zll commissioning Cluster Client. 
 *
 */
zbStatus_t ZCL_ZllCommissioningUtilityClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_GetGroupIdReq(zllCommissioning_GetGroupIdReq_t *pReq) 
 *
 * @brief	Sends over-the-air a GetGroupId request from the Zll Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllCommissioningUtility_GetGroupIdReq(zllCommissioning_GetGroupIdReq_t *pReq); 
/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_GetEndpointListReq(zllCommissioning_GetEndpointListReq_t *pReq) 
 *
 * @brief	Sends over-the-air a GetEndpointList request from the Zll Commissioning Cluster Client. 
 *
 */
zbStatus_t zclZllCommissioningUtility_GetEndpointListReq(zllCommissioning_GetEndpointListReq_t *pReq); 
/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_EndpointInformation(zllCommissioning_EndpointInformation_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air an Endpoint Information command from the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllCommissioningUtility_EndpointInformation(zllCommissioning_EndpointInformation_t *pCommandRsp);
/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_GetGroupIdRsp(zllCommissioning_GetGroupIdRsp_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air a GetGroupId response from the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllCommissioningUtility_GetGroupIdRsp(zllCommissioning_GetGroupIdRsp_t *pCommandRsp);
/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_GetEndpointListRsp(zllCommissioning_GetEndpointListRsp_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air a GetEndpointList response from the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllCommissioningUtility_GetEndpointListRsp(zllCommissioning_GetEndpointListRsp_t *pCommandRsp);

/************************************************************************************************/
/*                      Helper Functions to send ZLL commands:                                  */ 
/************************************************************************************************/
#if gInterPanCommunicationEnabled_c
/*!
 * @fn 		zbStatus_t ZllTouchlink_SendScanReq(void)
 *
* @brief	Helper functions: Sends over-the-air a ScanRequest command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendScanReq(void);
/*!
 * @fn 		zbStatus_t ZllTouchlink_SendDeviceInfReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId, uint8_t startIndex)
 *
 * @brief	Helper functions: Sends over-the-air a SendDeviceInfReq command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendDeviceInfReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId, uint8_t startIndex);
/*!
 * @fn 		zbStatus_t ZllTouchlink_SendIdentifyReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId, uint16_t identifyDuration)
 *
 * @brief	Helper functions: Sends over-the-air a SendIdentifyReq command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendIdentifyReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId, uint16_t identifyDuration);
/*!
 * @fn 		zbStatus_t ZllTouchlink_SendResetFactoryNewReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId)
 *
 * @brief	Helper functions: Sends over-the-air a ResetToFacory command from the Commissioning Cluster Client. 
 *
 */
zbStatus_t ZllTouchlink_SendResetFactoryNewReq(zbIeeeAddr_t dstAddr, zbPanId_t dstPanId);
/*!
 * @fn 		void ZllTouchlink_Init(bool_t startWithNvm)
 *
 * @brief	init Zll commissioning procedure 
 *
 */
bool_t ZllTouchlink_Init(bool_t startWithNvm);
/*
 * @fn 		void ZllTouchlink_InitDevice(void)
 *
 * @brief	Init the ZLL Commissioning TouchLink Procedure  
 *
 */
void ZllTouchlink_InitDevice(void);   
/*!
 * @fn 		bool_t ZllTouchlink_ProcessEvent(uint8_t event, uint8_t *pData)
 *
 * @brief	Process Zll commissioning Touchlink events
 *
 */
bool_t ZllTouchlink_ProcessEvent(uint8_t event, uint8_t *pData);
/*
 * @fn 		void ZllTouchlink_DefaultNonZllNetworkData(zbChannels_t channelMask)
 *
 * @brief	Default network data
 *
 */
void ZllTouchlink_DefaultNonZllNetworkData(zbChannels_t channelMask);
/*
 * @fn 		void ZllTouchlink_RemoveEntryFromRemoteDeviceTable(zbIeeeAddr_t ieeeAddr)
 *
 * @brief	Update network address in remote device table 
 *
 */
void ZllTouchlink_RemoveEntryFromRemoteDeviceTable(zbIeeeAddr_t ieeeAddr);
#endif /* gInterPanCommunicationEnabled_c */

/*!
 * @fn 		zbStatus_t ZllTouchlinkComissioning_Start(zllTouchlink_Start_t *pTouchlinkStart)     
 *
 * @brief	start Zll Touchlink procedure using ZTC command
 *
 */
zbStatus_t ZllTouchlinkComissioning_Start(zllTouchlink_Start_t *pTouchlinkStart);
/*!
 * @fn 		zbStatus_t ZllTouchlinkComissioning_Configure(zllTouchlink_Configure_t *pTouchlinkConfigure)     
 *
 * @brief	configure Zll Touchlink procedure 
 *
 */
zbStatus_t ZllTouchlinkComissioning_Configure(zllTouchlink_Configure_t *pTouchlinkConfigure);
/*!
 * @fn 		zbStatus_t ZllTouchlinkComissioning_GetListOfCommDevices(zllTouchlink_GetCommDevices_t *pReq, uint8_t *plistOfDevices)     
 *
 * @brief	return the  list of commissioned Devices (pListOfDevices[0] = totalNoOfDevice, pListOfDevices[1] = count,  pListOfDevices[2....] = deviceInf 
 *
 */
zbStatus_t ZllTouchlinkComissioning_GetListOfCommDevices(zllTouchlink_GetCommDevices_t *pReq, uint8_t *plistOfDevices);
/*!
 * @fn 		zbStatus_t ZllTouchlinkComissioning_AddGroup(zbGroupId_t aGroupId)     
 *
 * @brief	Add a group in the touchlink commissioning table  
 *
 */
zbStatus_t ZllTouchlinkComissioning_AddGroup(zbGroupId_t aGroupId);
/*!
 * @fn 		zbStatus_t ZllTouchlinkComissioning_AddGroup(zbGroupId_t aGroupId)     
 *
 * @brief	Add a group in the touchlink commissioning table  
 *
 */
zbStatus_t ZllTouchlinkComissioning_RemoveGroup(zbGroupId_t aGroupId);
/*!
 * @fn 		zbStatus_t zclZllCommissioningUtility_SendEndpointInformation(zllCommissioning_SendEndpointInf_t *pCommandRsp) 
 *
 * @brief	Sends over-the-air an Endpoint Information command from the Zll Commissioning Cluster Server. 
 *
 */
zbStatus_t zclZllCommissioningUtility_SendEndpointInformation(zllCommissioning_SendEndpointInf_t *pCommandRsp);
#ifdef __cplusplus
}
#endif 
#endif
