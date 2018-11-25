/*! @file 	  ZclOta.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for ZCL OTA Cluster.
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
 *  [R1] - docs-05-3474-20-0csg-zigbee-specification
 *  [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf
 */

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZclOptions.h"
#include "OtaSupport.h"
#include "ZclOta.h"
#include "ApsMgmtInterface.h"
#include "ASL_UserInterface.h"
#include "ASL_ZdpInterface.h"
#include "EndPointConfig.h"
#include "ZTCInterface.h"


#ifdef PROCESSOR_KINETIS
#include "PWR_Interface.h"
#include "Eeprom.h"
#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d || gZclOtaServerImgIntegrityCodeValidation_d)
#include "mmcau_interface.h"
#endif
#else
#include "PwrLib.h"
#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d || gZclOtaServerImgIntegrityCodeValidation_d)
#include "ASM_interface.h"
#endif
#endif

#if gZclEnableOTAServer_d && (defined(PROCESSOR_HCS08)||defined(PROCESSOR_MC13233C)||defined(PROCESSOR_MC13234C))
#include "BootloaderFlashUtils.h"
#endif

#if gZclEnableOTAClientECCLibrary_d
#include "eccapi.h"
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define gHaZtcOpCodeGroup_c                 0x70 /* opcode group used for HA commands/events */
#define gOTAImageProgressReport_c           0xDA /* for sending to external app the image progress report */

#if gZclEnableOTAServer_d
#if defined(PROCESSOR_HCS08)||defined(PROCESSOR_MC13233C)||defined(PROCESSOR_MC13234C)
  #define gExtFlash_TotalSize_c   gFlashParams_MaxImageLength_c
#else
  #ifdef PROCESSOR_KINETIS      
    #define gExtFlash_TotalSize_c   gEepromParams_TotalSize_c
  #else 
    #define gExtFlash_TotalSize_c   0x20000  
  #endif
#endif
#endif /* gZclEnableOTAServer_d */

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

#if gZclEnableOTAServer_d
/* ota server query next image command handler */
static zbStatus_t OTAServerNextImageRequestHandler(zbApsdeDataIndication_t *pIndication);

/* ota server procedure functions */
static zbStatus_t OTAServerStartImageProcess(uint8_t operationMode);
static zbStatus_t OTAServerNewImageReceived(ztcOtaServerImageNotify_t imgNotify);
static bool_t     OTAServerCheckAbortBlockRspList(zbNwkAddr_t   deviceID);
static uint8_t    OTAServerGetIndexFromOtaSetupTable(uint8_t endpoint, uint8_t tmrId);
static uint8_t    OTAServerGetClientEndpointFromTempQueryList(zbNwkAddr_t   aNwkAddr);
static void       OTAServerAddToTemporaryQueryReqList(zbNwkAddr_t   aNwkAddr, zbEndPoint_t  endPoint);

#if gZclOtaExternalMemorySupported_d
static void       OTAServerBlockRequestTmrCallback(uint8_t tmr);
static void       OTAServerQueryImageRequestTmrCallback(uint8_t tmr);
static uint8_t    OTAServerSearchImage(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion, uint8_t instanceId);
static uint8_t    OTAServerReadAndTestOtaHeader(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion, uint8_t instanceId);
#endif

/* ota server integrity code function*/
#if gZclOtaServerImgIntegrityCodeValidation_d
static void OTAServerProcessAesMmoHash(uint8_t *pImageBlock, uint8_t blockLength, uint8_t instanceId);
#endif

/* ota server serial protocol callbacks */
static otaResult_t ZtcOtaServerImageNotifyCnf(uint8_t* pBuffer, uint8_t len);
static otaResult_t ZtcOtaServerSetModeCnf(uint8_t* pBuffer, uint8_t len);
static otaResult_t ZtcOtaServerQueryImageRspCnf(uint8_t* pBuffer, uint8_t len);
static otaResult_t ZtcOtaServerBlockReceivedCnf(uint8_t* pBuffer, uint8_t len);
static otaResult_t ZtcOtaServerCancelImgCnf(uint8_t* pBuffer, uint8_t len);
static otaResult_t ZtcOtaServerAbortProcessCnf(uint8_t* pBuffer, uint8_t len);
static otaResult_t ZtcOtaServerSetFileVersPoliciesCnf(uint8_t* pBuffer, uint8_t len);
#endif  /* gZclEnableOTAServer_d */


#if gZclEnableOTAClient_d
/* ota client command handler */
static zbStatus_t ZCL_OTAClusterClient_ImageNotify_Handler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_OTAClusterClient_NextImageResponse_Handler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_OTAClusterClient_BlockResponse_Handler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_OTAClusterClient_UpgradeEndResponse_Handler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_OTAClusterClient_DefaultResponse_Handler(zbApsdeDataIndication_t *pIndication);

/* ota client procedure functions */
static zbStatus_t OTAClusterClientStartSession(uint8_t endpoint, uint32_t fileLength, uint32_t fileVersion);
static zbStatus_t OTAClusterClientRunImageProcessStateMachine();
static zbStatus_t OTAClusterClientProcessBlock(uint8_t *pImageBlock, uint8_t blockLength);

static zbStatus_t OTAClusterClientAbortSession(void);
static void OTAClusterClientProcessBlockTimerCallback(uint8_t tmr);
static void OTAClientRetransmitLastBlockTmrCallback(uint8_t tmr);
static void OTAClusterCPUResetCallback(uint8_t tmr);
static void OTAClusterDelayedUpgradeCallback(uint8_t tmr);
static void OTAClusterClientNextImageReqTimerCallback(uint8_t tmr);
static uint8_t OTAClusterClientGetIndexFromOtaSetupTable(uint8_t endpoint, uint8_t tmrId);

static zbStatus_t ZCL_OTAClusterClient_RetransmitBlockRequest(zbIeeeAddr_t  ieeeAddr, zbEndPoint_t  endPoint);
static zbStatus_t ZCL_OTAClusterClient_QueryNextImageRequest(zbIeeeAddr_t  ieeeAddr,  zbEndPoint_t  endPoint, uint8_t instanceOtaId);
static zbStatus_t ZCL_OTABlockRequestCmd(afAddrInfo_t addrInfo, zclOTABlockRequest_t  zclOTABlockRequest);
static zbStatus_t ZCL_OTAImageRequestCmd(afAddrInfo_t addrInfo, zclOTANextImageRequest_t zclOTANextImageRequest);

#if gEndDevCapability_d || gComboDeviceCapability_d 
static void OTAClient_UpdatePollRateCallBack(uint8_t tmrId);
#endif

/* ota client: server discovery functions */
static void OtaClient_ServerDiscoveryMatchDescReqTmrCallback(uint8_t tmr);
#endif

/* security functions */
#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d || gZclOtaServerImgIntegrityCodeValidation_d) && defined(PROCESSOR_KINETIS)
void encryptAES(uint8_t* key,  uint8_t* input_data, uint8_t* output_data, uint8_t data_length, uint8_t* init_vector);
#endif 

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/
#if (gZclEnableOTAServer_d == TRUE) && (gEnableOTAServer_d == 0)
#error "gEnableOTAServer_d from OtaSupport.h is not enabled"
#endif
/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/*  Size optimization. The values represent  should represent the size of the Image Notify Command
 based on the payload type.*/
const uint8_t mOtaImgNotifySize[gPayloadMax_c] = {2, 4, 6, 10};

#if gZclEnableOTAServer_d
/* ota server local information */
static otaServerSetup_t         mOtaServerSetup[gNoOfOtaClusterInstances_d];    /* one per Zigbee Cluster instance */
static otaServerCmdParams_t     mOtaServerCmdParams[gNoOfOtaClusterInstances_d];/* one per Zigbee Cluster instance */  

/* ota server temporary tables */
static otaServerQueryReqList_t         mOtaServerTempQueryReqList[gOtaServer_MaxTemporaryReqList_c];    /* one per Zigbee device*/
static otaServerAbortBlockRspList_t    mOtaServerAbortBlockRspList[gOtaServer_MaxTemporaryReqList_c];   /* one per Zigbee device*/


#if gZclOtaExternalMemorySupported_d   
/* ota server image informations */
static uint8_t mOtaServerCurrentImageIndex = 0;
static otaServerImageList_t mOtaServerImageList[gOtaServer_MaxOtaImages_c]; /* one per Zigbee device*/
#endif

/* ota server protocol (callbacks) */
otaServer_AppCB_t mOtaServerCb = 
{
    ZtcOtaServerImageNotifyCnf,
    ZtcOtaServerSetModeCnf,
    ZtcOtaServerQueryImageRspCnf,
    ZtcOtaServerBlockReceivedCnf,
    ZtcOtaServerCancelImgCnf,
    ZtcOtaServerAbortProcessCnf,
    ZtcOtaServerSetFileVersPoliciesCnf,
};
#endif /* gZclEnableOTAServer_d */


#if gZclEnableOTAClient_d
/* OTA client local informations */
static otaClientSetup_t      mOtaClienSetup[gNoOfOtaClusterInstances_d]; /* one per Zigbee Cluster instance */
static zclOTAClientSession_t mOtaClienSession;                           /* one per Zigbee device */    

/* OTA client timers */
zbTmrTimerID_t gOtaClientBlockProcessTimer = gTmrInvalidTimerID_c;
zbTmrTimerID_t gOtaClientRetransmissionBlockTimer = gTmrInvalidTimerID_c; 

/* OTA Client: block callback inf.*/
zclOTABlockCallbackState_t* gpBlockCallbackState = NULL;
/* OTA client: app callbacks:*/
otaClient_AppCB_t mOtaClientAppCb = 
{
  ZCL_OTAClusterClient_ImageNotify_Handler,
  ZCL_OTAClusterClient_NextImageResponse_Handler,
  ZCL_OTAClusterClient_BlockResponse_Handler,
  ZCL_OTAClusterClient_UpgradeEndResponse_Handler,
  ZCL_OTAClusterClient_DefaultResponse_Handler
};

/* OTA client: Server Discovery parameters */
const uint8_t OtaUpgrade_OutputClusterList[2] = {gaZclClusterOTA_c};
zbSimpleDescriptor_t otaCluster_simpleDescriptor = 
{
    8, /* Endpoint number */
    0x0,0x0, /* Application profile ID */
    0x0, 0x0, /* Application device ID */
    0, /* Application version ID */
    1, /* Number of input clusters */
    (uint8_t *)OtaUpgrade_OutputClusterList, /* Input cluster list */
    0, /* Number of output clusters */
    NULL, /* Output cluster list */
}; 
#endif /* gZclEnableOTAClient_d */


/******************************
  OTA Cluster Data
  See ZCL Specification Section 6.3
*******************************/

/* OTA cluster attributes: */
const zclAttrDef_t gaZclOTAClusterAttrDef[] = 
{
  { gZclAttrIdOTA_UpgradeServerId_c,                gZclDataTypeIeeeAddr_c,     gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(IEEEaddress_t), (void *)MbrOfs(zclOTAAttrsRAM_t, UpgradeServerId)},
  { gZclAttrIdOTA_ImageUpgradeStatusId_c,           gZclDataTypeEnum8_c,        gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(uint8_t), (void *)MbrOfs(zclOTAAttrsRAM_t, ImageUpgradeStatus)},
  { gZclAttrIdOTA_FileOffsetId_c,                   gZclDataTypeUint32_c,       gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(uint32_t), (void *)MbrOfs(zclOTAAttrsRAM_t, FileOffset)},
  { gZclAttrIdOTA_CurrentFileVersionId_c,           gZclDataTypeUint32_c,       gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(uint32_t), (void *)MbrOfs(zclOTAAttrsRAM_t, CurrentFileVersion)},
  { gZclAttrIdOTA_CurrentZigBeeStackVersionId_c,    gZclDataTypeUint16_c,       gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(uint16_t), (void *)MbrOfs(zclOTAAttrsRAM_t, CurrentZigBeeStackVersion)},
  { gZclAttrIdOTA_DownloadedFileVersionId_c,        gZclDataTypeUint32_c,       gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(uint32_t), (void *)MbrOfs(zclOTAAttrsRAM_t, DownloadedFileVersion)},
  { gZclAttrIdOTA_DownloadedZigBeeStackVersionId_c, gZclDataTypeUint16_c,       gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(uint16_t), (void *)MbrOfs(zclOTAAttrsRAM_t, DownloadedZigBeeStackVersion)},
  { gZclAttrIdOTA_ManufacturerId_c,                 gZclDataTypeUint16_c,       gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(uint16_t), (void *)MbrOfs(zclOTAAttrsRAM_t, ManufacturerId)},
  { gZclAttrIdOTA_ImageTypeId_c,                    gZclDataTypeUint16_c,       gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(uint16_t), (void *)MbrOfs(zclOTAAttrsRAM_t, ImageType)},
  { gZclAttrIdOTA_MinimumBlockRequestDelayId_c,     gZclDataTypeUint16_c,       gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsClientAttribute_c, sizeof(uint16_t), (void *)MbrOfs(zclOTAAttrsRAM_t, MinimumBlockReqDelay)}
};

const zclAttrSet_t gaZclOTAClusterAttrSet[] = 
{
  {gZclAttrOTASet_c, (void *)&gaZclOTAClusterAttrDef, NumberOfElements(gaZclOTAClusterAttrDef)}
};

const zclAttrSetList_t gZclOTAClusterAttrSetList = 
{
  NumberOfElements(gaZclOTAClusterAttrSet),
  gaZclOTAClusterAttrSet
};

/* OTA cluster commands supported: */
const zclCmd_t gaZclOTAClusterCmdReceivedDef[]=
{
  /* commands received */ 
  gZclCmdOTA_QueryNextImageRequest_c,
  gZclCmdOTA_ImageBlockRequest_c,
  gZclCmdOTA_UpgradeEndRequest_c
};

const zclCmd_t gaZclOTAClusterCmdGeneratedDef[]=
{
  /* commands generated */
  gZclCmdOTA_ImageNotify_c,
  gZclCmdOTA_QueryNextImageResponse_c,
  gZclCmdOTA_ImageBlockResponse_c,
  gZclCmdOTA_UpgradeEndResponse_c
};

const zclCommandsDefList_t gZclOTAClusterCommandsDefList =
{
   NumberOfElements(gaZclOTAClusterCmdReceivedDef), gaZclOTAClusterCmdReceivedDef,
   NumberOfElements(gaZclOTAClusterCmdGeneratedDef), gaZclOTAClusterCmdGeneratedDef
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/


/*****************************************************************************
******************************************************************************
*
* ZCL OTA SERVER FUNCTIONS
*
******************************************************************************
*****************************************************************************/

#if gZclEnableOTAServer_d
/*!
 * @fn 		zbStatus_t ZCL_OTAClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the OTA Upgrade Cluster Server. 
 *
 */
zbStatus_t ZCL_OTAClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    zclFrame_t *pFrame;
    zclOTABlockRequest_t          blockRequest;
    zclOTAUpgradeEndRequest_t     upgradeEndRequest;
    ztcZclOTAUpgdareEndResponse_t upgradeEndResponse;
    zbStatus_t status = gZclSuccess_c;    
    uint8_t instanceId;
    
     /* Parameter not used, avoid compiler warning */
    (void)pDevice;
    
    /*get ota server instance */
    instanceId= OTAServerGetIndexFromOtaSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
    if(instanceId == gZclCluster_InvalidDataIndex_d)
      return gZclFailure_c;
     
    
    /* ZCL frame */
    pFrame = (zclFrame_t*)pIndication->pAsdu;
    mOtaServerSetup[instanceId].tsqId = pIndication->pAsdu[1];
    
    /* Handle the commands */
    switch(pFrame->command)
    {
      case gZclCmdOTA_QueryNextImageRequest_c:  
        status = OTAServerNextImageRequestHandler(pIndication);   
         break;
  
      case gZclCmdOTA_ImageBlockRequest_c:
        if(mOtaServerSetup[instanceId].blockResponseInProgress == FALSE){
          mOtaServerSetup[instanceId].blockResponseInProgress = TRUE;
          FLib_MemCpy(&blockRequest, (pFrame+1), sizeof(zclOTABlockRequest_t));
        if(!mOtaServerCmdParams[instanceId].pZtcBlockResponse)
        {
          mOtaServerCmdParams[instanceId].pZtcBlockResponse = MSG_Alloc(sizeof(zclZtcOTABlockResponse_t)+ blockRequest.maxDataSize); 
        }
    	  if(!mOtaServerCmdParams[instanceId].pZtcBlockResponse)
    	  {
            mOtaServerSetup[instanceId].blockResponseInProgress = FALSE; 
            return gZclNoMem_c;	
    	  }
    	  Copy2Bytes(&mOtaServerCmdParams[instanceId].pZtcBlockResponse->aNwkAddr, &pIndication->aSrcAddr);
    	  mOtaServerCmdParams[instanceId].pZtcBlockResponse->endPoint = pIndication->srcEndPoint;
          if(mOtaServerSetup[instanceId].operationMode == gUseExternalMemoryForOtaUpdate_c)
          {      
#if gZclOtaExternalMemorySupported_d           
          if(mOtaServerSetup[instanceId].imageLoadingState == gOtaServerLoadingImageStart_c)
            mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.status = gZclOTAAbort_c;
          else
            mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.status = (mOtaServerSetup[instanceId].imageLoadingState == gOtaServerLoadingImageProcess_c)?gZclOTAWaitForData_c:gZclOTASuccess_c;   
#endif
          }
          else
            mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.status = gZclOTASuccess_c;
          mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.msgData.success.fileOffset = blockRequest.fileOffset;  		
    	  mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.msgData.success.dataSize = blockRequest.maxDataSize;
          mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.msgData.success.manufacturerCode = blockRequest.manufacturerCode;
          mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.msgData.success.imageType = blockRequest.imageType;
          mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.msgData.success.fileVersion = blockRequest.fileVersion;
          if(mOtaServerSetup[instanceId].operationMode == gUseExternalMemoryForOtaUpdate_c)
          {
#if gZclOtaExternalMemorySupported_d             
    	    status = ZCL_OTABlockResponse(mOtaServerCmdParams[instanceId].pZtcBlockResponse, instanceId);
            mOtaServerSetup[instanceId].blockResponseInProgress = FALSE;
            MSG_Free(mOtaServerCmdParams[instanceId].pZtcBlockResponse);
            mOtaServerCmdParams[instanceId].pZtcBlockResponse = NULL;
#endif  /* gZclOtaExternalMemorySupported_d  */          
          }
          else
          {         
            if(mOtaServerSetup[instanceId].otaServerIsActive)
            {
              uint16_t deviceId;
              Copy2Bytes(&deviceId, &pIndication->aSrcAddr);             
              OTA_ImageChunkReq(blockRequest.fileOffset, blockRequest.maxDataSize, deviceId);             
              status = gZclSuccess_c; 
            }
            else
            {
              mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.status = gZclOTAAbort_c;
              status = ZCL_OTABlockResponse(mOtaServerCmdParams[instanceId].pZtcBlockResponse, instanceId);
              mOtaServerSetup[instanceId].blockResponseInProgress = FALSE;
              MSG_Free(mOtaServerCmdParams[instanceId].pZtcBlockResponse);
              mOtaServerCmdParams[instanceId].pZtcBlockResponse = NULL;
            }          
          }  
        }
        else
        {
          zclZtcOTABlockResponse_t  *pZtcBlockResponseWaitForData;
          if((mOtaServerCmdParams[instanceId].pZtcBlockResponse->aNwkAddr[0] == pIndication->aSrcAddr[0])&&
             (mOtaServerCmdParams[instanceId].pZtcBlockResponse->aNwkAddr[1] == pIndication->aSrcAddr[1]))
               return gZclSuccess_c;
          pZtcBlockResponseWaitForData = MSG_Alloc(sizeof(zclZtcOTABlockResponse_t)); 
    	  if(!pZtcBlockResponseWaitForData)
    	  {
    		 mOtaServerSetup[instanceId].blockResponseInProgress = FALSE; 
    		 return gZclNoMem_c;	
    	  }
          Copy2Bytes(&pZtcBlockResponseWaitForData->aNwkAddr, &pIndication->aSrcAddr);
    	  pZtcBlockResponseWaitForData->endPoint = pIndication->srcEndPoint;
          pZtcBlockResponseWaitForData->zclOTABlockResponse.status = gZclOTAWaitForData_c;
          status = ZCL_OTABlockResponse(pZtcBlockResponseWaitForData, instanceId);
          MSG_Free(pZtcBlockResponseWaitForData);
        }
        break;
    case gZclCmdOTA_UpgradeEndRequest_c:
        FLib_MemCpy(&upgradeEndRequest, (pFrame+1), sizeof(zclOTAUpgradeEndRequest_t));
        if(upgradeEndRequest.status == gZclOTASuccess_c)
        {          
          Copy2Bytes(&upgradeEndResponse.aNwkAddr, &pIndication->aSrcAddr);
          upgradeEndResponse.endPoint = pIndication->srcEndPoint;
          upgradeEndResponse.zclOTAUpgradeEndResponse.manufacturerCode = upgradeEndRequest.manufacturerCode;
          upgradeEndResponse.zclOTAUpgradeEndResponse.imageType = upgradeEndRequest.imageType;
          upgradeEndResponse.zclOTAUpgradeEndResponse.fileVersion = upgradeEndRequest.fileVersion;
          upgradeEndResponse.zclOTAUpgradeEndResponse.currentTime = Native2OTA32(mOtaServerCmdParams[instanceId].currentTime);
          upgradeEndResponse.zclOTAUpgradeEndResponse.upgradeTime = Native2OTA32(mOtaServerCmdParams[instanceId].upgradeTime);                          
          status = ZCL_OTAUpgradeEndResponse(&upgradeEndResponse, instanceId);          
        }
        else{
        	status = gZclSuccessDefaultRsp_c;
        }
        if(mOtaServerSetup[instanceId].operationMode == gUseExternalMemoryForOtaUpdate_c)
        {   
#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)    
            zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
            /*send a report message with status TRUE or FALSE: imageLength field 
            *(TRUE - image successfully transmitted to client , FALSE - otherwise)*/
            ztcOtaProgressReportInf.currentOffset = 0xFFFFFFFF;
            if(status == gZclSuccess_c)
              ztcOtaProgressReportInf.imageLength = Native2OTA32(TRUE);
            else
              ztcOtaProgressReportInf.imageLength = Native2OTA32(FALSE);
            Copy2Bytes(&ztcOtaProgressReportInf.deviceAddr,&pIndication->aSrcAddr); 
#ifndef gHostApp_d  
            ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
            ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
#endif    
        }
        break;
        /* command not supported on this cluster */
    default:
        return gZclUnsupportedClusterCommand_c;
    }
    return status;
}

/*!
 * @fn 		void OTAServerInit(uint32_t fsciInterface)
 *
 * @brief	Init Ota server application
 *
 */
void OTAServerInit(uint32_t fsciInterface)
{  
  uint8_t i;
  uint8_t nextStartIndex = 0;
  
  
  for(i = 0; i< gNoOfOtaClusterInstances_d; i++)
  {
      uint8_t endpoint;
      zbClusterId_t clusterId = {gaZclClusterOTA_c};
      uint8_t startIndex = nextStartIndex;
     
      mOtaServerSetup[i].serverEndpoint = gZclCluster_InvalidDataIndex_d;
      endpoint = ZCL_GetEndPointForSpecificCluster(clusterId, TRUE, startIndex, &nextStartIndex);
      if(endpoint != gZclCluster_InvalidDataIndex_d)  
      {
        /* set ota endpoint */
        mOtaServerSetup[i].serverEndpoint = endpoint;
  
        /* init basic server inf */
        #if gZclOtaExternalMemorySupported_d  
        mOtaServerSetup[i].currentAddress = 0;
        mOtaServerSetup[i].operationMode = gUseExternalMemoryForOtaUpdate_c;
        mOtaServerSetup[i].imageLoadingState = gOtaServerLoadingImageComplete_c;
        mOtaServerSetup[i].localFileVersionPolicies = gOtaFileVersionPolicies_Upgrade_c | gOtaFileVersionPolicies_Downgrade_c;
        #else
        mOtaServerSetup[i].operationMode = gDoNotUseExternalMemoryForOtaUpdate_c;
        #endif
        mOtaServerSetup[i].otaServerIsActive = FALSE;
        mOtaServerSetup[i].blockResponseInProgress = FALSE; 
        mOtaServerSetup[i].otaServerBlockReqTimer = gTmrInvalidTimerID_c;
  
        /* init cmd params */
        mOtaServerCmdParams[i].currentTime = gOtaServer_DefaultTimeValue_c;
        mOtaServerCmdParams[i].upgradeTime = gOtaServer_DefaultTimeValue_c;
        mOtaServerCmdParams[i].blockRequestDelay = gOtaServer_DefaultTimeValue_c;
        mOtaServerCmdParams[i].upgradeRequestTime = gOtaServer_DefaultUpgradeRequestTime_c;
        mOtaServerCmdParams[i].querryJitter = gOtaClientMaxRandomJitter_c;
      }
  }
  
  /* init temporary table */
  for(uint8_t i=0;i<gOtaServer_MaxTemporaryReqList_c; i++)
    mOtaServerAbortBlockRspList[i].status = TRUE;
  
  /* init ota server protocol callbacks */
  (void)OTA_RegisterToFsci(fsciInterface, &mOtaServerCb);
}

/*!
 * @fn 		static uint8_t OTAServerGetIndexFromOtaSetupTable(uint8_t endpoint, uint8_t tmrId)
 *
 * @brief	* Return - index in the ota setup table if succes
                          - invalid data - otherwise
 *
 */
static uint8_t OTAServerGetIndexFromOtaSetupTable(uint8_t endpoint, uint8_t tmrId)
{
  if(endpoint != gZclCluster_InvalidDataIndex_d) 
  {  
    for(uint8_t i=0;i<gNoOfOtaClusterInstances_d; i++)
      if(endpoint == mOtaServerSetup[i].serverEndpoint)
        return i;
  }
  if(tmrId != gTmrInvalidTimerID_c)
  {
     for(uint8_t i=0;i<gNoOfOtaClusterInstances_d; i++)
      if(tmrId == mOtaServerSetup[i].otaServerBlockReqTimer)
        return i;
  }
  
  return gZclCluster_InvalidDataIndex_d;
}

/*!
 * @fn 		zbStatus_t ZCL_OTAImageNotify(zclZtcOTAImageNotify_t* pZtcImageNotifyParams, uint8_t instanceOtaClusterId)
 *
 * @brief	Sends over-the-air an Image Notify command from Ota Server
 *
 */
zbStatus_t ZCL_OTAImageNotify(zclZtcOTAImageNotify_t* pZtcImageNotifyParams, uint8_t instanceOtaClusterId)
{
  afToApsdeMessage_t *pMsg;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionNone_c, 1};
  zclFrameControl_t frameControl;
  uint8_t len;  
  
  if(instanceOtaClusterId == 0xFF)
  {/* select the instance based on current BeeAppDataInit(appEndPoint)  */
    instanceOtaClusterId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
    if(instanceOtaClusterId == gZclCluster_InvalidDataIndex_d)
      return gZclFailure_c;
  }
  
  
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcImageNotifyParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcImageNotifyParams->endPoint;
  addrInfo.srcEndPoint = mOtaServerSetup[instanceOtaClusterId].serverEndpoint; 
 
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  /* Initialize the lenght of the command based on the payload type.*/
  if(pZtcImageNotifyParams->zclOTAImageNotify.payloadType < gPayloadMax_c)
  {
    len = mOtaImgNotifySize[pZtcImageNotifyParams->zclOTAImageNotify.payloadType];
  }
  else
  {
    return gZclFailure_c;
  }
  if(!IsValidNwkUnicastAddr(addrInfo.dstAddr.aNwkAddr))
  {
	
    frameControl = gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp;
  }
  else
  {
	addrInfo.txOptions |=  gApsTxOptionSecEnabled_c;
    frameControl = gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp;
  }

  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_ImageNotify_c,
                          frameControl, 
                          &ZbZclFoundationGlobals(gZclTransactionId), 
                          &len,
                          &pZtcImageNotifyParams->zclOTAImageNotify);
  ZbZclFoundationGlobals(gZclTransactionId)++;
  if(!pMsg)
    return gZclNoMem_c;
 return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/*!
 * @fn 		zbStatus_t ZCL_OTANextImageResponse(zclZtcOTANextImageResponse_t* pZtcNextImageResponseParams, uint8_t instanceOtaClusterId)
 *
 * @brief	Sends over-the-air an Query next image response command from Ota Server
 *
 */
zbStatus_t ZCL_OTANextImageResponse(zclZtcOTANextImageResponse_t* pZtcNextImageResponseParams, uint8_t instanceOtaClusterId)
{
  afToApsdeMessage_t *pMsg;
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif  
  uint8_t len = 0;  
  
  if(instanceOtaClusterId == 0xFF)
  {/* select the instance based on current BeeAppDataInit(appEndPoint)  */
    instanceOtaClusterId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
    if(instanceOtaClusterId == gZclCluster_InvalidDataIndex_d)
      return gZclFailure_c;
  }
  
  switch(pZtcNextImageResponseParams->zclOTANextImageResponse.status)
    {
  	  case gZclOTASuccess_c:
  	    len = sizeof(zclOTANextImageResponse_t);
  	    break;
  	  case gZclOTANoImageAvailable_c:
  	  case gZclOTANotAuthorized_c:	  
  	    len = sizeof(zclOTAStatus_t);
  	    break;
  	  default:
  	    /* Invalid status */
  	    return gZclMalformedCommand_c;
    }
  
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcNextImageResponseParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcNextImageResponseParams->endPoint;
  addrInfo.srcEndPoint = mOtaServerSetup[instanceOtaClusterId].serverEndpoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_QueryNextImageResponse_c,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp, 
                          &(mOtaServerSetup[instanceOtaClusterId].tsqId), 
                          &len,
                          &pZtcNextImageResponseParams->zclOTANextImageResponse);
  if(!pMsg)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/*!
 * @fn 		zbStatus_t ZCL_OTABlockResponse(zclZtcOTABlockResponse_t *pZtcBlockResponseParams, uint8_t instanceId)
 *
 * @brief	Sends over-the-air an Image Block Response command from Ota Server
 *
 */
zbStatus_t ZCL_OTABlockResponse(zclZtcOTABlockResponse_t *pZtcBlockResponseParams, uint8_t instanceOtaClusterId)
{
 uint8_t len = 0; 
 afToApsdeMessage_t *pMsg;
 
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif   
  
  if(instanceOtaClusterId == 0xFF)
  {/* select the instance based on current BeeAppDataInit(appEndPoint)  */
    instanceOtaClusterId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
    if(instanceOtaClusterId == gZclCluster_InvalidDataIndex_d)
      return gZclFailure_c;
  }
  
  
  if(OTAServerCheckAbortBlockRspList(pZtcBlockResponseParams->aNwkAddr))
    pZtcBlockResponseParams->zclOTABlockResponse.status = gZclOTAAbort_c;

  if(mOtaServerSetup[instanceOtaClusterId].operationMode == gUseExternalMemoryForOtaUpdate_c)
  {
#if(gZclOtaExternalMemorySupported_d)
    uint32_t addr;
    uint32_t offsetImage = 0;

    uint16_t manufacturerCode = OTA2Native16(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.manufacturerCode);
    uint16_t imageType = OTA2Native16(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.imageType);
    uint32_t fileVersion = OTA2Native32(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.fileVersion);
    
    if(pZtcBlockResponseParams->zclOTABlockResponse.status == gZclOTASuccess_c)
    {
      uint8_t index = OTAServerSearchImage(manufacturerCode,imageType,fileVersion, TRUE, instanceOtaClusterId); 
      if(index == gOtaServer_MaxOtaImages_c)
    	 return gZclOTANoImageAvailable_c; 
      else
      {
        offsetImage = OTA2Native32(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.fileOffset);
        len = pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.dataSize;
      
        if(mOtaServerImageList[index].totalImageSize - offsetImage < len)
			len =mOtaServerImageList[index].totalImageSize - offsetImage;
        addr = offsetImage + mOtaServerImageList[index].addressOfCurrentImage;
        if (OTA_ReadExternalMemory(&pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.data[0], len, addr) != gOtaSucess_c )
	 	     pZtcBlockResponseParams->zclOTABlockResponse.status = gZclOTAAbort_c; 
        pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.dataSize = len;	

#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)    
        {
          zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
          if((offsetImage == 0)||((offsetImage+len)%(len*100) == 0)||(offsetImage+len == mOtaServerImageList[index].totalImageSize))
          {
            ztcOtaProgressReportInf.currentOffset = Native2OTA32(offsetImage+len);
            ztcOtaProgressReportInf.imageLength =  Native2OTA32(mOtaServerImageList[index].totalImageSize);
            Copy2Bytes(&ztcOtaProgressReportInf.deviceAddr,&pZtcBlockResponseParams->aNwkAddr); 
#ifndef gHostApp_d  
            ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
            ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
          }
        }
#endif  /* gZclEnableOTAProgressReportToExternApp_d  */    
      }
    }
#endif /* gZclOtaExternalMemorySupported_d */
  }
  /* The lenght of the frame is status dependant */
  switch(pZtcBlockResponseParams->zclOTABlockResponse.status)
  {
	  case gZclOTASuccess_c:
	    len = sizeof(zclOTABlockResponse_t) + pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.dataSize - 1;
	    break;
	  case gZclOTAAbort_c:
	    len = sizeof(zclOTAStatus_t);
	    break;
	  case gZclOTAWaitForData_c:
            {
              pZtcBlockResponseParams->zclOTABlockResponse.msgData.wait.currentTime = Native2OTA32(mOtaServerCmdParams[instanceOtaClusterId].currentTime); 
              pZtcBlockResponseParams->zclOTABlockResponse.msgData.wait.requestTime = Native2OTA32(mOtaServerCmdParams[instanceOtaClusterId].upgradeRequestTime);     /* seconds */
	      pZtcBlockResponseParams->zclOTABlockResponse.msgData.wait.blockRequestDelay = Native2OTA16(mOtaServerCmdParams[instanceOtaClusterId].blockRequestDelay); /* Miliseconds*/
              len = sizeof(zclOTABlockResponseStatusWait_t) + 1;
            }
	    break;
	  default:
	    /* Invalid status */
	    return gZclMalformedCommand_c;
  }
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcBlockResponseParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcBlockResponseParams->endPoint;
  addrInfo.srcEndPoint = mOtaServerSetup[instanceOtaClusterId].serverEndpoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
	                      gZclCmdOTA_ImageBlockResponse_c,
	                      gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp, 
	                      &(mOtaServerSetup[instanceOtaClusterId].tsqId), 
	                      &len,
	                      &pZtcBlockResponseParams->zclOTABlockResponse);
  if(!pMsg)
	   return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);    
}

/*!
 * @fn 		zbStatus_t ZCL_OTAUpgradeEndResponse(ztcZclOTAUpgdareEndResponse_t* pZtcOTAUpgdareEndResponse, uint8_t instanceId)
 *
 * @brief	Sends over-the-air an Upgrade End Response command from Ota Server
 *
 */
zbStatus_t ZCL_OTAUpgradeEndResponse(ztcZclOTAUpgdareEndResponse_t* pZtcOTAUpgdareEndResponse, uint8_t instanceOtaClusterId)
{
  afToApsdeMessage_t *pMsg;
  uint8_t len = sizeof(zclOTAUpgradeEndResponse_t);  
  
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif  
  
  if(instanceOtaClusterId == 0xFF)
  {/* select the instance based on current BeeAppDataInit(appEndPoint)  */
    instanceOtaClusterId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
    if(instanceOtaClusterId == gZclCluster_InvalidDataIndex_d)
      return gZclFailure_c;
  }
  
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcOTAUpgdareEndResponse->aNwkAddr);
  addrInfo.dstEndPoint = pZtcOTAUpgdareEndResponse->endPoint;
  addrInfo.srcEndPoint = mOtaServerSetup[instanceOtaClusterId].serverEndpoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_UpgradeEndResponse_c,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp, 
                          &(mOtaServerSetup[instanceOtaClusterId].tsqId), 
                          &len,
                          &pZtcOTAUpgdareEndResponse->zclOTAUpgradeEndResponse);
  if(!pMsg)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}
 
/*!
 * @fn 		static zbStatus_t OTAServerNextImageRequestHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Process Query Next Image Request command. Sends back a next image response 
 *
 */
static zbStatus_t OTAServerNextImageRequestHandler
(
  zbApsdeDataIndication_t *pIndication  
)
{
   zclFrame_t *pFrame;
   zbStatus_t status = gZclSuccess_c;  
   zclOTANextImageRequest_t   ztcNextImageReq;
   uint8_t instanceId;
   
   /*get ota server instance */
   instanceId= OTAServerGetIndexFromOtaSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
   if(instanceId == gZclCluster_InvalidDataIndex_d)
      return gZclFailure_c;
   
   /*ZCL frame*/
   pFrame = (zclFrame_t*)pIndication->pAsdu;
   FLib_MemCpy(&ztcNextImageReq, (pFrame+1), sizeof(zclOTANextImageRequest_t));
   ztcNextImageReq.manufacturerCode = OTA2Native16(ztcNextImageReq.manufacturerCode);
   ztcNextImageReq.imageType = OTA2Native16(ztcNextImageReq.imageType);
   ztcNextImageReq.fileVersion = OTA2Native32(ztcNextImageReq.fileVersion);
   if(mOtaServerSetup[instanceId].operationMode == gDoNotUseExternalMemoryForOtaUpdate_c)
   {   
     if(mOtaServerSetup[instanceId].otaServerIsActive)
     {
        uint16_t devId;
        Copy2Bytes(&devId, &pIndication->aSrcAddr);
        /* wait the host application to confirm (see OTAServerQueryImageRsp) */
        OTA_QueryImageReq(devId, Native2OTA16(ztcNextImageReq.manufacturerCode), Native2OTA16(ztcNextImageReq.imageType), Native2OTA32(ztcNextImageReq.fileVersion));
        OTAServerAddToTemporaryQueryReqList(pIndication->aSrcAddr, pIndication->srcEndPoint);
     }
      else  
      {
          zclZtcOTANextImageResponse_t  ztcNextImageRsp;
          ztcNextImageRsp.zclOTANextImageResponse.status = gZclOTANoImageAvailable_c;
          Copy2Bytes(&ztcNextImageRsp.aNwkAddr,&pIndication->aSrcAddr);
          ztcNextImageRsp.endPoint = pIndication->srcEndPoint;
          ztcNextImageRsp.zclOTANextImageResponse.manufacturerCode = Native2OTA16(ztcNextImageReq.manufacturerCode);
          ztcNextImageRsp.zclOTANextImageResponse.imageType = Native2OTA16(ztcNextImageReq.imageType);
          status = ZCL_OTANextImageResponse(&ztcNextImageRsp, instanceId);
      }    
   }
   else
   {
      zclZtcOTANextImageResponse_t  ztcNextImageRsp;
#if gZclOtaExternalMemorySupported_d
      uint8_t index = OTAServerSearchImage(ztcNextImageReq.manufacturerCode,ztcNextImageReq.imageType, ztcNextImageReq.fileVersion, FALSE, instanceId); 
      if(index == gOtaServer_MaxOtaImages_c)
         ztcNextImageRsp.zclOTANextImageResponse.status = gZclOTANoImageAvailable_c;
      else
      {
        ztcNextImageRsp.zclOTANextImageResponse.status = gZclSuccess_c;
        ztcNextImageRsp.zclOTANextImageResponse.fileVersion = Native2OTA32(mOtaServerImageList[index].fileVersion);
        ztcNextImageRsp.zclOTANextImageResponse.imageSize = Native2OTA32(mOtaServerImageList[index].totalImageSize);  
      }      
      ztcNextImageRsp.zclOTANextImageResponse.manufacturerCode = Native2OTA16(mOtaServerImageList[index].manufacturerCode);
      ztcNextImageRsp.zclOTANextImageResponse.imageType = Native2OTA16(mOtaServerImageList[index].imageType);
#else
      ztcNextImageRsp.zclOTANextImageResponse.status = gZclOTANoImageAvailable_c;      
#endif   
      Copy2Bytes(&ztcNextImageRsp.aNwkAddr,&pIndication->aSrcAddr);
      ztcNextImageRsp.endPoint = pIndication->srcEndPoint;    
      status = ZCL_OTANextImageResponse(&ztcNextImageRsp, instanceId);     
   }
   return status;
}

/*!
 * @fn 		static zbStatus_t OTAServerStartImageProcess(uint8_t operationMode)
 *
 * @brief	Used to start an OTA Server process
 *
 */
static zbStatus_t OTAServerStartImageProcess(uint8_t operationMode)
{  
   /* select the instance based on current BeeAppDataInit(appEndPoint)  */
   uint8_t instanceId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
   
   if(instanceId == gZclCluster_InvalidDataIndex_d)
      return gZclFailure_c;
  
   /* set the operation mode */
   mOtaServerSetup[instanceId].operationMode = operationMode;
  
   /* start ota server procedure */
   if(mOtaServerSetup[instanceId].operationMode == gUseExternalMemoryForOtaUpdate_c)
   {
#if (gZclOtaExternalMemorySupported_d)  
      if(mOtaServerSetup[instanceId].imageLoadingState == gOtaServerLoadingImageProcess_c)
       return gOtaInvalidParam_c;
      mOtaServerSetup[instanceId].currentAddress = 0x00;
      mOtaServerCurrentImageIndex = 0;

      /* Init external memory */
#if TWR_KW24D512 || TWR_KW22D512 || TWR_KW21D256 || TWR_KW21D512
#if !gUseInternalFlashForOta_c  
  /* de-Init the keyboard module: for the specified platforms the keyboard and external EEprom are on the same Bus */
      KBD_Deinit();
#endif /* !gUseInternalFlashForOta_c */  
#endif /* gTargetTWR_KW24D512_d || gTargetTWR_KW22D512_d || gTargetTWR_KW21D256_d || gTargetTWR_KW21D512_d */        
      OTA_InitExternalMemory();
      
#if !(TWR_KW24D512 || TWR_KW22D512 || TWR_KW21D256 || TWR_KW21D512)  
      /* Erase external memory */
      if(gOtaSucess_c != OTA_EraseExternalMemory())
        return gOtaExternalFlashError_c; 
#endif      

#if gZclOtaServerImgIntegrityCodeValidation_d      
      FLib_MemSet(&mOtaServerSetup[instanceId].integrityCode[0], 0x00, AES_MMO_SIZE);
#endif  
      
      /*send a query image request */
      if(mOtaServerSetup[instanceId].otaServerBlockReqTimer == gTmrInvalidTimerID_c)
      {
        mOtaServerSetup[instanceId].otaServerBlockReqTimer = ZbTMR_AllocateTimer(); 
        if(mOtaServerSetup[instanceId].otaServerBlockReqTimer == gTmrInvalidTimerID_c)
          return gZclNoMem_c;
      }      
      ZbTMR_StartTimer(mOtaServerSetup[instanceId].otaServerBlockReqTimer, gTmrSingleShotTimer_c,
                         gOtaServerProtocolBlockReqDelay_c, OTAServerQueryImageRequestTmrCallback);
      return gOtaSucess_c;      
#else      
      return gOtaInvalidParam_c; 
#endif      
   }   
   else
   {
      if(mOtaServerSetup[instanceId].operationMode == gOtaTestingProcess)
      {
	/* switch the operation mode */
	 mOtaServerSetup[instanceId].operationMode = (mOtaServerSetup[instanceId].operationMode==gUseExternalMemoryForOtaUpdate_c)?gDoNotUseExternalMemoryForOtaUpdate_c:gUseExternalMemoryForOtaUpdate_c;			 
      }
      if(mOtaServerSetup[instanceId].operationMode == gDoNotUseExternalMemoryForOtaUpdate_c)
      {
	 uint8_t i;
	 for(i=0; i< gOtaServer_MaxTemporaryReqList_c; i++)
	 {
            mOtaServerTempQueryReqList[i].endPoint = 0xFF;
            Set2Bytes(mOtaServerTempQueryReqList[i].aNwkAddr, 0xFFFF);
	 }   
         mOtaServerSetup[instanceId].otaServerIsActive = TRUE;
      }
   }
   return gOtaSucess_c;
}

/*!
 * @fn 		static void OTAServerQueryImageRequestTmrCallback(uint8_t tmr) 
 *
 * @brief	Callback used to initiate Ota server block request
 *
 */
static void OTAServerQueryImageRequestTmrCallback(uint8_t tmr) 
{
    uint8_t instanceId = OTAServerGetIndexFromOtaSetupTable(gZclCluster_InvalidDataIndex_d, tmr);
    if(instanceId == gZclCluster_InvalidDataIndex_d)
      return;
    ZbTMR_FreeTimer(mOtaServerSetup[instanceId].otaServerBlockReqTimer);
    mOtaServerSetup[instanceId].otaServerBlockReqTimer = gTmrInvalidTimerID_c;
    /* wait the host application to confirm (see OTAServerQueryImageRsp) */
    OTA_QueryImageReq(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFFFFF);
    
    (void)tmr;
    
}

/*!
 * @fn 		zbStatus_t OTAServerNewImageReceived(ztcOtaServerImageNotify_t imgNotify) 
 *
 * @brief	sends over the air an Image Notify command when a new image is available
 *
 */
zbStatus_t OTAServerNewImageReceived(ztcOtaServerImageNotify_t imgNotify)
{
    zclZtcOTAImageNotify_t imageNotify;
    
    /* send a broadcast or unicast image notify */
    Copy2Bytes(&imageNotify.aNwkAddr, &imgNotify.deviceID);
    imageNotify.endPoint = 0xFF;
 
    /* see 6.10.3- ImageNotify Command - Ota cluster specification (r17)*/
    if(IsValidNwkUnicastAddr(imageNotify.aNwkAddr) == FALSE)
      imageNotify.zclOTAImageNotify.payloadType = gJitter_c;
    else
      imageNotify.zclOTAImageNotify.payloadType = gJitterMfgImageFile_c;
    imageNotify.zclOTAImageNotify.queryJitter = gOtaClientMaxRandomJitter_c;
    /*inform clients:  new image(s) are available*/  
    imageNotify.zclOTAImageNotify.manufacturerCode = imgNotify.manufacturerCode;
    imageNotify.zclOTAImageNotify.imageType = imgNotify.imageType;   
    imageNotify.zclOTAImageNotify.fileVersion = imgNotify.fileVersion; 
    return ZCL_OTAImageNotify(&imageNotify, 0xFF);      
}


#if (gZclOtaExternalMemorySupported_d) 
/*!
 * @fn 		static uint8_t OTAServerSearchImage(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion, uint8_t instanceId)
 *
 * @brief	Return required image Index,  otherwise return gMaxNumberOfOTAImage
 *
 */
static uint8_t OTAServerSearchImage(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion, uint8_t instanceId)
{
  uint8_t i;
  
   if( mOtaServerSetup[instanceId].imageLoadingState != gOtaServerLoadingImageComplete_c)
     return gOtaServer_MaxOtaImages_c;
  
   /*test if the image exist in the list*/
   for(i=0;i<gOtaServer_MaxOtaImages_c;i++)
     if(((imageType == mOtaServerImageList[i].imageType)||(mOtaServerImageList[i].imageType == gOtaImageTypeMatchAll_c))&&
    	 ((manufacturerCode == mOtaServerImageList[i].manufacturerCode)||(mOtaServerImageList[i].manufacturerCode == gOtaManufacturerCodeMatchAll_c)))
     {
    	 if(testFileVersion == TRUE)
    	 {
            if((fileVersion == mOtaServerImageList[i].fileVersion)||(mOtaServerImageList[i].fileVersion==gOtaFileVersionMatchAll_c))
    		return i;
    	 }
    	 else
         {
            if(((mOtaServerSetup[instanceId].localFileVersionPolicies & gOtaFileVersionPolicies_Upgrade_c)&& (fileVersion < mOtaServerImageList[i].fileVersion))||
                ((mOtaServerSetup[instanceId].localFileVersionPolicies & gOtaFileVersionPolicies_Reinstall_c)&& (fileVersion == mOtaServerImageList[i].fileVersion))||
                 ((mOtaServerSetup[instanceId].localFileVersionPolicies & gOtaFileVersionPolicies_Downgrade_c)&& (fileVersion > mOtaServerImageList[i].fileVersion)))
                  return i;
         }
     }
      
   return OTAServerReadAndTestOtaHeader(manufacturerCode, imageType, fileVersion, testFileVersion, instanceId); 
}

/*!
 * @fn 		static uint8_t OTAServerReadAndTestOtaHeader(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion, uint8_t instanceId) 
 *
 * @brief	Read the ota header and compare part of it with Image Request Parameters
 *
 */
static uint8_t OTAServerReadAndTestOtaHeader(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion, uint8_t instanceId) 
{
    uint8_t buffer[sizeof(zclOTAFileHeader_t)]; 
    uint8_t *pData;
    zclOTAFileHeader_t otaHeader; /* to store the ota header information */
    uint32_t currentAddress = 0x00; 
    uint8_t index = 0; /* image index */
    
    /* Initialize External Memory */
    OTA_InitExternalMemory();
    pData = &buffer[0];
    while((currentAddress < gExtFlash_TotalSize_c)&&(index < gOtaServer_MaxOtaImages_c))
    {
        /* Read OTA Header */
        (void)OTA_ReadExternalMemory(pData, sizeof(zclOTAFileHeader_t), currentAddress);
        FLib_MemCpy(&otaHeader, &pData[0], sizeof(zclOTAFileHeader_t));
        otaHeader.fileIdentifier = OTA2Native32( otaHeader.fileIdentifier);
        if(otaHeader.fileIdentifier != gOtaFileIdentifier_c)
          return gOtaServer_MaxOtaImages_c;
        mOtaServerImageList[index].addressOfCurrentImage  = currentAddress;
        mOtaServerImageList[index].manufacturerCode = OTA2Native16(otaHeader.manufacturerCode);
        mOtaServerImageList[index].imageType = OTA2Native16(otaHeader.imageType);
        mOtaServerImageList[index].fileVersion = OTA2Native32(otaHeader.fileVersion);
        mOtaServerImageList[index].totalImageSize = OTA2Native32(otaHeader.totalImageSize);
       
        if(((imageType == mOtaServerImageList[index].imageType)||(mOtaServerImageList[index].imageType == gOtaImageTypeMatchAll_c))&&
    	 ((manufacturerCode == mOtaServerImageList[index].manufacturerCode)||(mOtaServerImageList[index].manufacturerCode == gOtaManufacturerCodeMatchAll_c)))

        {
          	 if(testFileVersion == TRUE)
          	 {
                    if((fileVersion == mOtaServerImageList[index].fileVersion)||(mOtaServerImageList[index].fileVersion==gOtaFileVersionMatchAll_c))
                    {
                      mOtaServerSetup[instanceId].imageLoadingState = gOtaServerLoadingImageComplete_c;
                      return index;
                    }
                    else
                    {
                      currentAddress+= mOtaServerImageList[index].totalImageSize;
                      index++;
                    }
           	 }
           	 else
           	 {
                    if(((mOtaServerSetup[instanceId].localFileVersionPolicies & gOtaFileVersionPolicies_Upgrade_c)&& (fileVersion < mOtaServerImageList[index].fileVersion))||
                        ((mOtaServerSetup[instanceId].localFileVersionPolicies & gOtaFileVersionPolicies_Reinstall_c)&& (fileVersion == mOtaServerImageList[index].fileVersion))||
                          ((mOtaServerSetup[instanceId].localFileVersionPolicies & gOtaFileVersionPolicies_Downgrade_c)&& (fileVersion > mOtaServerImageList[index].fileVersion)))
                    {
                      mOtaServerSetup[instanceId].imageLoadingState = gOtaServerLoadingImageComplete_c;
                      return index;
                    }
                    else
                    {
                      currentAddress+= mOtaServerImageList[index].totalImageSize;
                      index++;
                    }
           	 }
        }
        else
        {
        	currentAddress+= mOtaServerImageList[index].totalImageSize;
        	index++;
        }
    }
   mOtaServerSetup[instanceId].imageLoadingState = gOtaServerLoadingImageComplete_c;
   return gOtaServer_MaxOtaImages_c;
}

/*!
 * @fn 		static void OTAServerBlockRequestTmrCallback(uint8_t tmr) 
 *
 * @brief	Request an Image block(if the device contain external memory)
 *
 */
static void OTAServerBlockRequestTmrCallback(uint8_t tmr) 
{
    uint16_t    deviceID;
    uint32_t    currentOffset;
    uint8_t     blockSize;
    /* select the instance based on current tmrId */
    uint8_t instanceId = OTAServerGetIndexFromOtaSetupTable(gZclCluster_InvalidDataIndex_d, tmr);
    if(instanceId == gZclCluster_InvalidDataIndex_d)
      return;
   
    Copy2Bytes(&deviceID, NlmeGetRequest(gNwkShortAddress_c));
    if(mOtaServerSetup[instanceId].operationMode == gUseExternalMemoryForOtaUpdate_c)
    {	
      currentOffset = (mOtaServerCurrentImageIndex ==0)?Native2OTA32(mOtaServerSetup[instanceId].currentAddress):Native2OTA32(mOtaServerSetup[instanceId].currentAddress - mOtaServerImageList[mOtaServerCurrentImageIndex-1].totalImageSize);
      /* block size = 200 or rest of image data*/
      blockSize = (mOtaServerImageList[mOtaServerCurrentImageIndex].totalImageSize -
                     mOtaServerSetup[instanceId].currentAddress + mOtaServerImageList[mOtaServerCurrentImageIndex].addressOfCurrentImage > 200)?
                     200:(uint8_t)(mOtaServerImageList[mOtaServerCurrentImageIndex].totalImageSize - mOtaServerSetup[instanceId].currentAddress +
                     mOtaServerImageList[mOtaServerCurrentImageIndex].addressOfCurrentImage);  

      OTA_ImageChunkReq(currentOffset, blockSize, deviceID);
    }
    (void)tmr;  
}

#endif

/*!
 * @fn 		static void OTAServerAddToTemporaryQueryReqList(zbNwkAddr_t   aNwkAddr, zbEndPoint_t  endPoint)
 *
 * @brief       Store temporary client inf(address, endpoint) (only for dongle solution = without external memory)
 *
 */
static void OTAServerAddToTemporaryQueryReqList(zbNwkAddr_t   aNwkAddr, zbEndPoint_t  endPoint)
{
  uint8_t i = 0;
  for(i=0; i< gOtaServer_MaxTemporaryReqList_c; i++)
    if(mOtaServerTempQueryReqList[i].endPoint == 0xFF)
    {
      mOtaServerTempQueryReqList[i].endPoint = endPoint;
      Copy2Bytes(&mOtaServerTempQueryReqList[i].aNwkAddr, aNwkAddr);
      break;
    }
}

/*!
 * @fn 		static uint8_t OTAServerGetClientEndpointFromTempQueryList(zbNwkAddr_t   aNwkAddr)
 *
 * @brief       Get client endpoint (only for dongle solution = without external memory)
 *
 */
static uint8_t OTAServerGetClientEndpointFromTempQueryList(zbNwkAddr_t   aNwkAddr)
{
  uint8_t i = 0;
  zbEndPoint_t endpoint = 0xFF;
  for(i=0; i< gOtaServer_MaxTemporaryReqList_c; i++)
    if(IsEqual2Bytes(aNwkAddr, mOtaServerTempQueryReqList[i].aNwkAddr))
    {
      endpoint =  mOtaServerTempQueryReqList[i].endPoint;
      /* slot free */
      mOtaServerTempQueryReqList[i].endPoint = 0xFF;
      Set2Bytes(mOtaServerTempQueryReqList[i].aNwkAddr, 0xFFFF);
      break;
    }
  return endpoint;
}


/*!
 * @fn 	       static bool_t OTAServerCheckAbortBlockRspList(zbNwkAddr_t   deviceID) 
 *
 * @brief      Check abort list
 *
 */
static bool_t OTAServerCheckAbortBlockRspList(zbNwkAddr_t   deviceID) 
{
  uint8_t i = 0;
  for(i = 0; i<gOtaServer_MaxTemporaryReqList_c; i++)
  {
    if(mOtaServerAbortBlockRspList[i].status == FALSE)
    {
      if(IsEqual2Bytes(mOtaServerAbortBlockRspList[i].aNwkAddr, deviceID))  
      {
        mOtaServerAbortBlockRspList[i].status = TRUE;
        return TRUE;
      }
    }
  }
  return FALSE;
}

#if gZclOtaServerImgIntegrityCodeValidation_d
/*!
 * @fn 	       static void OTAServerProcessAesMmoHash(uint8_t *pImageBlock, uint8_t blockLength, uint8_t instanceId)
 *
 * @brief      This function computes the AES MMO hash for OTA Server Application using blocks of image 
 *             and stores the result in mOtaServerSetup[instanceId].integrityCode.
 *
 */
static void OTAServerProcessAesMmoHash(uint8_t *pImageBlock, uint8_t blockLength, uint8_t instanceId)
{
  uint8_t bytesToCopy, bytesCopied = 0;
  uint8_t sizeBlock = 2*AES_MMO_SIZE;
  static uint8_t aesMmoBlock[2*AES_MMO_SIZE];
  static uint8_t mPosition = 0;
  static bool_t lastBlockForHash = FALSE;
  
  if(mOtaServerImageList[mOtaServerCurrentImageIndex].totalImageSize + mOtaServerImageList[mOtaServerCurrentImageIndex].addressOfCurrentImage > (mOtaServerSetup[instanceId].currentAddress + blockLength + AES_MMO_SIZE))
  {
    while(bytesCopied < blockLength)
    {
        lastBlockForHash=FALSE;
        bytesToCopy = sizeBlock - mPosition;
        if(bytesToCopy > (blockLength - bytesCopied))
        {
            bytesToCopy = (blockLength - bytesCopied);
        }
        FLib_MemCpy(aesMmoBlock+mPosition, pImageBlock+bytesCopied, bytesToCopy);
        bytesCopied +=bytesToCopy;
        mPosition+=bytesToCopy;
        if(mPosition == sizeBlock)
        {
              OTAClusterAesMMO_hash(aesMmoBlock, sizeBlock, lastBlockForHash, mOtaServerImageList[mOtaServerCurrentImageIndex].totalImageSize-AES_MMO_SIZE, mOtaServerSetup[instanceId].integrityCode);
              mPosition = 0;
        }
      }
  }
  else
  {
    if(lastBlockForHash==FALSE)
    {
      uint8_t *lastAesMmoBlock;
      lastBlockForHash = TRUE;
      bytesToCopy = mOtaServerImageList[mOtaServerCurrentImageIndex].totalImageSize + mOtaServerImageList[mOtaServerCurrentImageIndex].addressOfCurrentImage - mOtaServerSetup[instanceId].currentAddress - AES_MMO_SIZE;
      sizeBlock = mPosition + bytesToCopy;
      lastAesMmoBlock = AF_MsgAlloc(); 
      if(!lastAesMmoBlock)
    	  return;
      FLib_MemCpy(lastAesMmoBlock, aesMmoBlock, mPosition);
      FLib_MemCpy(lastAesMmoBlock+mPosition, pImageBlock, bytesToCopy);
      OTAClusterAesMMO_hash(lastAesMmoBlock, sizeBlock, lastBlockForHash, mOtaServerImageList[mOtaServerCurrentImageIndex].totalImageSize-AES_MMO_SIZE, mOtaServerSetup[instanceId].integrityCode);
      mPosition = 0;
      MSG_Free(lastAesMmoBlock);
    }
  }
}
#endif /* gZclOtaServerImgIntegrityCodeValidation_d */  

/*****************************************************************************
*
*  OTA Server Serial Protocol callbacks
*
*****************************************************************************/
/*!
 * @fn 		otaResult_t ZtcOtaServerImageNotifyCnf (uint8_t* pBuffer, uint8_t len)
 *
 * @brief	Process image notification received from external application. 
 *              If operation Mode = gUseExternalMemoryForOtaUpdate_c: test the image parameters. If no match start download it
*                                 = gNotUseExternalMemoryForOtaUpdate_c: send the ImageNotify command over the air;      
 */
static otaResult_t ZtcOtaServerImageNotifyCnf (uint8_t* pBuffer, uint8_t len)
{
  ztcOtaServerImageNotify_t imageNotify;  
  /* select the instance based on current BeeAppDataInit(appEndPoint)  */
  uint8_t instanceId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
  
  if(instanceId == gZclCluster_InvalidDataIndex_d)
      return gOtaError_c;
  
  /* get image notification parameters: */
  FLib_MemCpy(&imageNotify, &pBuffer[0], sizeof(ztcOtaServerImageNotify_t));
  imageNotify.manufacturerCode = OTA2Native16(imageNotify.manufacturerCode);
  imageNotify.imageType = OTA2Native16(imageNotify.imageType);
  imageNotify.fileVersion = OTA2Native32(imageNotify.fileVersion);
  
  /* check operation mode: */
  if(mOtaServerSetup[instanceId].operationMode == gUseExternalMemoryForOtaUpdate_c)
  {
#if gZclOtaExternalMemorySupported_d    
    uint8_t index = OTAServerSearchImage(imageNotify.manufacturerCode, imageNotify.imageType, imageNotify.fileVersion, TRUE, instanceId);
    if(index == gOtaServer_MaxOtaImages_c)
    {
      /* start downloading process */
      if(mOtaServerSetup[instanceId].otaServerBlockReqTimer == gTmrInvalidTimerID_c)
      {
        mOtaServerSetup[instanceId].otaServerBlockReqTimer = ZbTMR_AllocateTimer();
        if(mOtaServerSetup[instanceId].otaServerBlockReqTimer == gTmrInvalidTimerID_c)
          return gOtaError_c;
      }
      ZbTMR_StartTimer(mOtaServerSetup[instanceId].otaServerBlockReqTimer, gTmrSingleShotTimer_c,
                         gOtaServerProtocolBlockReqDelay_c, OTAServerQueryImageRequestTmrCallback);
      return gOtaSucess_c;
    }
    else
      return gOtaError_c;
#endif    
  }
  /* avoid compiler warning */  
  (void)len;
  
  return (OTAServerNewImageReceived(imageNotify) == gZclSuccess_c)?gOtaSucess_c:gOtaError_c;
}

/*!
 * @fn 		otaResult_t ZtcOtaServerSetModeCnf (uint8_t* pBuffer, uint8_t len)
 *
 * @brief	Process set ota mode confirm
 *
 */
static otaResult_t ZtcOtaServerSetModeCnf(uint8_t* pBuffer, uint8_t len)
{
  /* avoid compiler warning */  
  (void)len;
  
  return (OTAServerStartImageProcess(pBuffer[0]) == gZclSuccess_c)?gOtaSucess_c:gOtaError_c;
}

/*!
 * @fn 		otaResult_t ZtcOtaServerQueryImageRspCnf (uint8_t* pBuffer, uint8_t len)
 *
 * @brief	Receive Image information from an external application
 *
 */
static otaResult_t ZtcOtaServerQueryImageRspCnf(uint8_t* pBuffer, uint8_t len)
{
  ztcOtaServerQueryImageRsp_t queryResponse;
  /* select the instance based on current BeeAppDataInit(appEndPoint)  */
  uint8_t instanceId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
  
  if(instanceId == gZclCluster_InvalidDataIndex_d)
      return gOtaError_c;
  
  /* get queryImage parameters */
  FLib_MemCpy(&queryResponse, &pBuffer[0], sizeof(ztcOtaServerQueryImageRsp_t));  
  queryResponse.manufacturerCode = OTA2Native16(queryResponse.manufacturerCode);
  queryResponse.imageType = OTA2Native16(queryResponse.imageType);
  queryResponse.fileVersion = OTA2Native32(queryResponse.fileVersion);
  queryResponse.totalImageSize = OTA2Native32(queryResponse.totalImageSize);
  
  /* check operation mode: */
  if(mOtaServerSetup[instanceId].operationMode == gUseExternalMemoryForOtaUpdate_c)
  {
#if (gZclOtaExternalMemorySupported_d)  
      /* check queryRsp parameters: */
      if((queryResponse.imageStatus != gOtaSucess_c) || (mOtaServerCurrentImageIndex >= gOtaServer_MaxOtaImages_c))
        return gOtaInvalidParam_c;
      
      /* check available flash */
      if ((queryResponse.totalImageSize == 0) || 
            (queryResponse.totalImageSize + mOtaServerSetup[instanceId].currentAddress > gExtFlash_TotalSize_c)) 
        return gOtaInvalidParam_c;
      
      /* keep server information */
      mOtaServerImageList[mOtaServerCurrentImageIndex].imageType = queryResponse.imageType;
      mOtaServerImageList[mOtaServerCurrentImageIndex].totalImageSize = queryResponse.totalImageSize;
      mOtaServerImageList[mOtaServerCurrentImageIndex].manufacturerCode = queryResponse.manufacturerCode;
      mOtaServerImageList[mOtaServerCurrentImageIndex].fileVersion = queryResponse.fileVersion; 
      mOtaServerImageList[mOtaServerCurrentImageIndex].addressOfCurrentImage =  mOtaServerSetup[instanceId].currentAddress;
      mOtaServerSetup[instanceId].imageLoadingState = gOtaServerLoadingImageStart_c;
      
      /* send a first block request */
      if(mOtaServerSetup[instanceId].otaServerBlockReqTimer == gTmrInvalidTimerID_c)
      {
        mOtaServerSetup[instanceId].otaServerBlockReqTimer = ZbTMR_AllocateTimer(); 
        if(mOtaServerSetup[instanceId].otaServerBlockReqTimer == gTmrInvalidTimerID_c)
          return gOtaError_c;
      }
      ZbTMR_StartTimer(mOtaServerSetup[instanceId].otaServerBlockReqTimer, gTmrSingleShotTimer_c,
                         gOtaServerProtocolBlockReqDelay_c, OTAServerBlockRequestTmrCallback);    
#else
      return gOtaInvalidOperation_c;
#endif  
   }
   else  /* gNotUseExternalMemoryForOtaUpdate_c */
   {
     /* send a query next image response over the air */
     zclZtcOTANextImageResponse_t  ztcNextImageRsp;
     
     Copy2Bytes(&ztcNextImageRsp.aNwkAddr, &queryResponse.deviceID);
     ztcNextImageRsp.endPoint = OTAServerGetClientEndpointFromTempQueryList(queryResponse.deviceID);
     ztcNextImageRsp.zclOTANextImageResponse.status = queryResponse.imageStatus;
     ztcNextImageRsp.zclOTANextImageResponse.manufacturerCode = queryResponse.manufacturerCode;
     ztcNextImageRsp.zclOTANextImageResponse.imageType = queryResponse.imageType;
     ztcNextImageRsp.zclOTANextImageResponse.fileVersion = queryResponse.fileVersion;
     ztcNextImageRsp.zclOTANextImageResponse.imageSize = queryResponse.totalImageSize;   
     
     return (ZCL_OTANextImageResponse(&ztcNextImageRsp, instanceId) == gZclSuccess_c)?gOtaSucess_c:gOtaError_c;
   }  
   return gOtaSucess_c;  
}

/*!
 * @fn 		otaResult_t ZtcOtaServerBlockReceivedCnf (uint8_t* pBuffer, uint8_t len)
 *
 * @brief	called every time a new image block is received.
 *
 */
static otaResult_t ZtcOtaServerBlockReceivedCnf(uint8_t* pBuffer, uint8_t len)
{
  zbStatus_t status = gOtaSucess_c;
  /* select the instance based on current BeeAppDataInit(appEndPoint)  */
  uint8_t instanceId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
  
  if(instanceId == gZclCluster_InvalidDataIndex_d)
      return gOtaError_c;
  
  
  /* Validate parameters */
  if((len == 0) || (pBuffer == NULL)) 
  {
    return gOtaInvalidParam_c;
  }
  
  /* check operation mode */
  if(mOtaServerSetup[instanceId].operationMode == gDoNotUseExternalMemoryForOtaUpdate_c) 
  {
    if(mOtaServerCmdParams[instanceId].pZtcBlockResponse)
    {
    /* dongle mode(without external memory):  complete the image block response*/
    FLib_MemCpy(&mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.msgData.success.data[0], pBuffer, len);
    mOtaServerCmdParams[instanceId].pZtcBlockResponse->zclOTABlockResponse.msgData.success.dataSize = len;
    status = ZCL_OTABlockResponse(mOtaServerCmdParams[instanceId].pZtcBlockResponse, instanceId);
   
    mOtaServerSetup[instanceId].blockResponseInProgress = FALSE;
    MSG_Free(mOtaServerCmdParams[instanceId].pZtcBlockResponse);  
    mOtaServerCmdParams[instanceId].pZtcBlockResponse = NULL;
    }
    else
    {
      status = gZclNoMem_c;
    }
  }
  else
  { 
    /* with external memory */
#if (gZclOtaExternalMemorySupported_d)   
    
#if gZclOtaServerImgIntegrityCodeValidation_d      
    /* compute the Hash to check the ImageIntegrity */
    OTAServerProcessAesMmoHash(pBuffer, len, instanceId);
#endif     
    
    /* Write image to external memory */
    status = OTA_WriteExternalMemory(pBuffer, len, mOtaServerSetup[instanceId].currentAddress);
    if(status == gOtaSucess_c)
    { 
      mOtaServerSetup[instanceId].currentAddress+= len;
      /* send a request for next block */
      if(mOtaServerSetup[instanceId].currentAddress - mOtaServerImageList[mOtaServerCurrentImageIndex].addressOfCurrentImage < mOtaServerImageList[mOtaServerCurrentImageIndex].totalImageSize)
        ZbTMR_StartTimer(mOtaServerSetup[instanceId].otaServerBlockReqTimer, gTmrSingleShotTimer_c,
                          gOtaServerProtocolBlockReqDelay_c, OTAServerBlockRequestTmrCallback); 
      else  
      { 
        ztcOtaServerImageNotify_t imgNotify = {0xFF, 0xFF, 0, 0, 0};
        
#if gZclOtaServerImgIntegrityCodeValidation_d    
        zclOTaImageIntegritySubElement_t  subElemInf; 
        uint8_t buffer[sizeof(zclOTaImageIntegritySubElement_t)];
        uint8_t *pData;
        pData = &buffer[0];
        (void)OTA_ReadExternalMemory(pData, sizeof(zclOTaImageIntegritySubElement_t),  mOtaServerSetup[instanceId].currentAddress - sizeof(zclOTaImageIntegritySubElement_t));  
        FLib_MemCpy(&subElemInf, &pData[0], sizeof(zclOTaImageIntegritySubElement_t));
        ZbTMR_FreeTimer(mOtaServerSetup[instanceId].otaServerBlockReqTimer);    
   
        /*check the image integrity Code */
        if(subElemInf.id == gZclOtaImageIntegrityCodeTagId && 
           subElemInf.length == gImageIntegrityCodeLengthField)
        {
          if(!FLib_MemCmp(mOtaServerSetup[instanceId].integrityCode, subElemInf.hash, AES_MMO_SIZE))
          {
            /* clear the image fields*/
            mOtaServerImageList[mOtaServerCurrentImageIndex].manufacturerCode = 0x00;
            mOtaServerImageList[mOtaServerCurrentImageIndex].imageType = 0x00;  
            mOtaServerImageList[mOtaServerCurrentImageIndex].totalImageSize = 0x00;
            mOtaServerImageList[mOtaServerCurrentImageIndex].fileVersion = 0x00;
            /* clear the image header from memory */
            //(void)OTA_EraseBlock(mOtaServerImageList[mOtaServerCurrentImageIndex].addressOfCurrentImage);
            return gOtaCrcError_c;
          }
        }
#endif       
        mOtaServerSetup[instanceId].imageLoadingState = gOtaServerLoadingImageComplete_c;
#if TWR_KW24D512 || TWR_KW22D512 || TWR_KW21D256 || TWR_KW21D512
#if !gUseInternalFlashForOta_c  
  /* re-Init the keyboard module: for the specified platforms the keyboard and external EEprom are on the same Bus */
        KBD_Init(BeeAppHandleKeys);
#endif /* !gUseInternalFlashForOta_c */  
#endif /* TWR_KW24D512 || TWR_KW22D512 || TWR_KW21D256 || TWR_KW21D512 */ 
        status = OTAServerNewImageReceived(imgNotify);
        mOtaServerCurrentImageIndex++;
        
#if gZclOtaServerImgIntegrityCodeValidation_d     
         /* clear mOtaServerSetup.integrityCode Information */    
        FLib_MemSet(&mOtaServerSetup[instanceId].integrityCode[0], 0x00, AES_MMO_SIZE);
#endif         
      }
    }
#endif /*  gZclOtaExternalMemorySupported_d   */
  } 
  return (status==gZclSuccess_c)?gOtaSucess_c:gOtaError_c;
}

/*!
 * @fn 		otaResult_t ZtcOtaServerCancelImgCnf (uint8_t* pBuffer, uint8_t len)
 *
 * @brief	Terminate the upgrade process(server as a dongle) and clean abort device list
 *
 */
static otaResult_t ZtcOtaServerCancelImgCnf(uint8_t* pBuffer, uint8_t len)
{
  /* select the instance based on current BeeAppDataInit(appEndPoint)  */
  uint8_t instanceId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
  
  if(instanceId == gZclCluster_InvalidDataIndex_d)
      return gOtaError_c;
  
  if(mOtaServerSetup[instanceId].operationMode == gDoNotUseExternalMemoryForOtaUpdate_c)
    mOtaServerSetup[instanceId].otaServerIsActive = FALSE;
  mOtaServerSetup[instanceId].blockResponseInProgress = FALSE;

  /* free abort device list */ 
   for(uint8_t i = 0; i<gOtaServer_MaxTemporaryReqList_c; i++)
      mOtaServerAbortBlockRspList[i].status = TRUE;
   
   /* avoid compiler warning */  
  (void)len;
  
   return gOtaSucess_c;  
}

/*!
 * @fn 		otaResult_t ZtcOtaServerAbortProcessCnf (uint8_t* pBuffer, uint8_t len)
 *
 * @brief	Process set ota mode confirm
 *
 */
static otaResult_t ZtcOtaServerAbortProcessCnf(uint8_t* pBuffer, uint8_t len)
{
  for(uint8_t i = 0; i<gOtaServer_MaxTemporaryReqList_c; i++)
  {
    if(mOtaServerAbortBlockRspList[i].status == TRUE)
    {
      mOtaServerAbortBlockRspList[i].status = FALSE;
      Copy2Bytes(mOtaServerAbortBlockRspList[i].aNwkAddr, &pBuffer[0]);
      return gOtaSucess_c;
    }      
  }
  return gOtaError_c;  
}

/*!
 * @fn 		otaResult_t ZtcOtaServerSetFileVersPoliciesCnf (uint8_t* pBuffer, uint8_t len)
 *
 * @brief	set the local file version policies: allow upgrade/reinstall/downgrade
 *
 */
static otaResult_t ZtcOtaServerSetFileVersPoliciesCnf(uint8_t* pBuffer, uint8_t len)
{
  /* select the instance based on current BeeAppDataInit(appEndPoint)  */
  uint8_t instanceId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
  
  if(instanceId == gZclCluster_InvalidDataIndex_d)
      return gOtaError_c;
  
#if gZclOtaExternalMemorySupported_d
  /* set version policies */
  if(mOtaServerSetup[instanceId].operationMode == gUseExternalMemoryForOtaUpdate_c)
    mOtaServerSetup[instanceId].localFileVersionPolicies = pBuffer[0];
#endif
  
  /* avoid compiler warning */  
  (void)len;
  
  return gOtaSucess_c;
}


#endif

/*****************************************************************************
******************************************************************************
*
* ZCL OTA CLIENT FUNCTIONS
*
******************************************************************************
*****************************************************************************/

#if gZclEnableOTAClient_d
 
/*!
 * @fn 		zbStatus_t ZCL_OTAClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the OTA Upgrade Cluster Client. 
 *
 */
zbStatus_t ZCL_OTAClusterClient
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
)
{
    zclCmd_t            command;
    zclFrame_t          *pFrame;
    zbStatus_t          result = gZclSuccessDefaultRsp_c;    
    zbIeeeAddr_t aExtAddr;
    uint8_t instanceId;
    
    /* prevent compiler warning */
    (void)pDevice;
    pFrame = (void *)pIndication->pAsdu;
    
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    	result = gZclSuccess_c;
	
    instanceId = OTAClusterClientGetIndexFromOtaSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c); 
    if(instanceId == gZclCluster_InvalidDataIndex_d)
      return gZclUnsupportedClusterCommand_c;
    
    /* handle the command */
    command = pFrame->command;
    switch(command)
    {
      case gZclCmdOTA_ImageNotify_c:
      {
        zbIeeeAddr_t   upgradeServerId; 
       
        /*test the upgradeServerID attribute*/
        (void)APS_GetIeeeAddress(pIndication->aSrcAddr, aExtAddr); 
        (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_UpgradeServerId_c, gZclClientAttr_c, upgradeServerId, NULL);
        
        if(!FLib_MemCmp(aExtAddr,upgradeServerId, sizeof(zbIeeeAddr_t)))
        { 
          uint8_t instanceIndex = OTAClusterClientGetIndexFromOtaSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
          if(instanceIndex != gZclCluster_InvalidDataIndex_d)
          {
            otaCluster_simpleDescriptor.endPoint = pIndication->dstEndPoint;
            Copy2Bytes(&otaCluster_simpleDescriptor.aAppProfId, pIndication->aProfileId);     		  
            ASL_MatchDescriptor_req(NULL,pIndication->aSrcAddr, (zbSimpleDescriptor_t*)&otaCluster_simpleDescriptor); 
          
            mOtaClienSetup[instanceIndex].flags |= gOtaExpectedMatchDescRsp_c;
          }
          return result;
        }
        if(mOtaClienSetup[instanceId].pOtaClientAppCb.otaClientImgNotifyHandler)
        {
            result = mOtaClienSetup[instanceId].pOtaClientAppCb.otaClientImgNotifyHandler(pIndication); 
        }
        else
        {
            result = gZclUnsupportedClusterCommand_c;
        }
        break;
      } 
      case gZclCmdOTA_QueryNextImageResponse_c:
      {
    	if(pFrame->transactionId == mOtaClienSession.requestedTsq) 
        {
            if(mOtaClienSetup[instanceId].pOtaClientAppCb.otaClientNextImgRspHandler)
            {
                result = mOtaClienSetup[instanceId].pOtaClientAppCb.otaClientNextImgRspHandler(pIndication); 
            }
            else
            {
                result = gZclUnsupportedClusterCommand_c;
            }
        }
    	else
          result = gZclFailure_c;
        break;
      }
      case gZclCmdOTA_ImageBlockResponse_c:
      {
    	  
    	 if(pFrame->transactionId == mOtaClienSession.requestedTsq) 
    	 {
    	    /* process packed received */	
    	    ZbTMR_StopTimer(gOtaClientRetransmissionBlockTimer);
    	    mOtaClienSession.retransmissionCounter = 0x00;
            if(mOtaClienSetup[instanceId].pOtaClientAppCb.otaClientBlockRspHandler)
            {
                result = mOtaClienSetup[instanceId].pOtaClientAppCb.otaClientBlockRspHandler(pIndication); 
            }
            else
            {
                result = gZclUnsupportedClusterCommand_c;
            }
    	 }
    	 
    	 if(!ZbTMR_IsTimerActive(gOtaClientRetransmissionBlockTimer))
    	 {
            if(!ZbTMR_IsTimerReady(gOtaClientRetransmissionBlockTimer))
            {
              /* force an request if the client don't receive the BlockResponse */
               uint16_t blockDelay = (mOtaClienSession.blockRequestDelay > 50)? mOtaClienSession.blockRequestDelay:gOtaBlockProcessingCallbackDelay_c; 
    	       ZbTMR_StartIntervalTimer(gOtaClientRetransmissionBlockTimer, blockDelay + 4000, OTAClientRetransmitLastBlockTmrCallback); 
            }
    	 }
    	 break;
      }
      case gZclCmdOTA_UpgradeEndResponse_c:  
      {  
        if(mOtaClienSetup[instanceId].pOtaClientAppCb.otaClientUpgradeEndRspHandler)
        {
          result = mOtaClienSetup[instanceId].pOtaClientAppCb.otaClientUpgradeEndRspHandler(pIndication); 
        }
        else
        {
          result = gZclUnsupportedClusterCommand_c;
        }
    	break;
      }
      case gZclCmdDefaultRsp_c:
        if(mOtaClienSetup[instanceId].pOtaClientAppCb.otaClientDefaultRspHandler)
        {
           result = mOtaClienSetup[instanceId].pOtaClientAppCb.otaClientDefaultRspHandler(pIndication); 
        }
        else
          result = gZclFailure_c;
        break;
      default:
      {
    	result = gZclUnsupportedClusterCommand_c;
        break;
      }
    }
    
    if(result == gZclSuccess_c)
    {
    	result = (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c;
    }   
    return result;
}

/*!
 * @fn 		void OTAClientInit(void)
 *
 * @brief	Init Ota server application
 *
 */
void OTAClientInit(void)
{   
  zbClusterId_t clusterId = {gaZclClusterOTA_c};
  uint16_t      currentStackVersion = NlmeGetRequest(gNwkStackProfile_c);
  uint16_t      downloadedZigBeeStackVersion = NlmeGetRequest(gNwkStackProfile_c);
  uint32_t      fileOffset = 0;
  uint8_t       upgradeStatus = gOTAUpgradeStatusDownloadComplete_c;  
  uint16_t      minimumBlockDelay = 0;
  uint8_t       nextStartIndex = 0, i = 0;
  
  for(i = 0; i< gNoOfOtaClusterInstances_d; i++)
  {
      uint8_t endpoint;
      uint8_t startIndex = nextStartIndex;
     
      mOtaClienSetup[i].clientEndpoint = gZclCluster_InvalidDataIndex_d;
      mOtaClienSetup[i].imageRequestTimer = gTmrInvalidTimerID_c;
      
      endpoint = ZCL_GetEndPointForSpecificCluster(clusterId, FALSE, startIndex, &nextStartIndex);
      if(endpoint != gZclCluster_InvalidDataIndex_d)  
      {
          /* set OTA Attributes */
          (void)ZCL_SetAttribute(endpoint, clusterId, gZclAttrOTA_CurrentZigBeeStackVersionId_c, gZclClientAttr_c,&currentStackVersion);
          (void)ZCL_SetAttribute(endpoint, clusterId, gZclAttrOTA_DownloadedZigBeeStackVersionId_c, gZclClientAttr_c,&downloadedZigBeeStackVersion);
          (void)ZCL_SetAttribute(endpoint, clusterId, gZclAttrOTA_FileOffsetId_c, gZclClientAttr_c, &fileOffset);
          (void)ZCL_SetAttribute(endpoint, clusterId, gZclAttrOTA_ImageUpgradeStatusId_c, gZclClientAttr_c, &upgradeStatus);
          (void)ZCL_SetAttribute(endpoint, clusterId, gZclAttrOTA_MinimumBlockRequestDelayId_c, gZclClientAttr_c, &minimumBlockDelay);

          /* set client params */
          mOtaClienSetup[i].clientEndpoint = endpoint;
          mOtaClienSetup[i].serverEndpoint = gZclCluster_InvalidDataIndex_d;
          mOtaClienSetup[i].hardwareVersion = gOtaCurrentHwVersion_c;
          mOtaClienSetup[i].maxDataSize = gOtaClientMaxDataSize_c; 
          mOtaClienSetup[i].multipleUpgradeImg = 0x00;
          mOtaClienSetup[i].flags = 0x00;   
          
          mOtaClienSetup[i].pOtaClientAppCb = mOtaClientAppCb;
     }
  }
  /* init Ota client session*/
  FLib_MemSet(&mOtaClienSession, 0x00, sizeof(zclOTAClientSession_t));
  mOtaClienSession.pStateBuffer = NULL;
}

/******************************************************************************
* ZCL_OTAClusterClient_EndUpgradeRequest
*
* Sends upgrade end request to server
******************************************************************************/  
zbStatus_t ZCL_OTAClusterClient_EndUpgradeRequest
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint,
  zclOTAStatus_t status
) 
{
  zclZtcOTAUpgradeEndRequest_t upgradeEndRequest;
  zbClusterId_t clusterOtaId = {gaZclClusterOTA_c};  
  uint8_t clientEndPoint = mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint;
  
  Copy2Bytes(&upgradeEndRequest.aNwkAddr, aNwkAddr);
  upgradeEndRequest.endPoint = endPoint;
  upgradeEndRequest.zclOTAUpgradeEndRequest.status = status;
  (void)ZCL_GetAttribute(clientEndPoint, clusterOtaId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &upgradeEndRequest.zclOTAUpgradeEndRequest.manufacturerCode, NULL);
  (void)ZCL_GetAttribute(clientEndPoint, clusterOtaId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &upgradeEndRequest.zclOTAUpgradeEndRequest.imageType, NULL);
           
  upgradeEndRequest.zclOTAUpgradeEndRequest.fileVersion = Native2OTA32(mOtaClienSession.downloadingFileVersion);
  
#if  (gZclEnableOTAProgressReport_d == TRUE) 
  if(status == gZclOTASuccess_c)
  {
      mOtaClienSession.progressReport = otaProgress100_c;
      BeeAppUpdateDevice(clientEndPoint, gOTAProgressReportEvent_c, 0, 0, NULL);
  }
#endif 

#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)  
  {
    /*send a report message with status TRUE or FALSE in the imageLength field 
    *(TRUE - image successfully transmitted to client , FALSE - otherwise)*/
    zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
    if(status == gZclOTASuccess_c)
      ztcOtaProgressReportInf.imageLength = Native2OTA32(TRUE);
    else
      ztcOtaProgressReportInf.imageLength = Native2OTA32(FALSE);
    ztcOtaProgressReportInf.currentOffset = 0xFFFFFFFF;
    Copy2Bytes(ztcOtaProgressReportInf.deviceAddr,NlmeGetRequest(gNwkShortAddress_c));   
#ifndef gHostApp_d  
    ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
    ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
  }
#endif   /* gZclEnableOTAProgressReportToExternApp_d */

  return ZCL_OTAUpgradeEndRequest(&upgradeEndRequest);
}

/******************************************************************************
* ZCL_OTAClusterClient_EndUpgradeAbortRequest
*
* Aborts current client session and sends end request with Abort or Invalid Image
*(if security failed) status to server
******************************************************************************/  
zbStatus_t ZCL_OTAClusterClient_EndUpgradeAbortRequest
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint, 
  zclOTAStatus_t status
) 
{
  uint8_t result = ZCL_OTAClusterClient_EndUpgradeRequest(aNwkAddr, endPoint, status);
  if (result != gZclSuccess_c)
    return gZclOTAAbort_c;
  
  return OTAClusterClientAbortSession();
}


/******************************************************************************
* ZCL_OTAClusterClient_NextImageRequest
*
* Sends back a next image request (may be as a result of Image Notify)
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_NextImageRequest 
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint    
)
{
  zbClusterId_t clusterOtaId = {gaZclClusterOTA_c}; 
  zclZtcOTANextImageRequest_t nextImageRequest;     
  uint8_t clientEndPoint = mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint;
  
  /* Send back the image request. */
  Copy2Bytes(&nextImageRequest.aNwkAddr, aNwkAddr);
  nextImageRequest.endPoint = endPoint;
  nextImageRequest.zclOTANextImageRequest.fieldControl = gZclOTANextImageRequest_HwVersionPresent_c;
  (void)ZCL_GetAttribute(clientEndPoint, clusterOtaId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &nextImageRequest.zclOTANextImageRequest.manufacturerCode, NULL);
  (void)ZCL_GetAttribute(clientEndPoint, clusterOtaId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &nextImageRequest.zclOTANextImageRequest.imageType, NULL);
  (void)ZCL_GetAttribute(clientEndPoint, clusterOtaId, gZclAttrOTA_CurrentFileVersionId_c, gZclClientAttr_c, &nextImageRequest.zclOTANextImageRequest.fileVersion, NULL);

  nextImageRequest.zclOTANextImageRequest.hardwareVersion = Native2OTA16(mOtaClienSetup[mOtaClienSession.instanceIndex].hardwareVersion);
  return ZCL_OTAImageRequest(&nextImageRequest);  
}
 
/*!
 * @fn 		static zbStatus_t ZCL_OTAClusterClient_ImageNotify_Handler(zbApsdeDataIndication_t *pIndication) 
 *
 * @brief	Handles the receipt of an ImageNotify Command on the client
 *
 */
static zbStatus_t ZCL_OTAClusterClient_ImageNotify_Handler 
(
  zbApsdeDataIndication_t *pIndication
)
{
  zclOTAImageNotify_t *pImageNotify;
  uint8_t frameLen;
  uint8_t clientJitter;
  zclStatus_t result;
  bool_t  dropPacket = FALSE;
  bool_t isUnicast = (pIndication->fWasBroadcast == TRUE)?FALSE:TRUE;;
 
  /*ignore command if an ota session started */
  if(mOtaClienSession.sessionStarted == TRUE)
    return gZclSuccess_c;
  
  /* update transaction Sequence number */
  if(pIndication->pAsdu[1] == ZbZclFoundationGlobals(gZclTransactionId))
	  ZbZclFoundationGlobals(gZclTransactionId)++;
  pImageNotify =  (zclOTAImageNotify_t *)(pIndication->pAsdu + sizeof(zclFrame_t));
  frameLen = pIndication->asduLength - sizeof(zclFrame_t); 

  /* invalid payload type or invalid data length for specified payload type data */
  if (frameLen < sizeof(pImageNotify->payloadType) ||
      pImageNotify->payloadType >= gPayloadMax_c ||
      frameLen != mOtaImgNotifySize[pImageNotify->payloadType])
    return  gZclMalformedCommand_c;
    
  /* unicast */
  if (isUnicast) 
  {
      /* send back query next image */
      result = ZCL_OTAClusterClient_NextImageRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);
      if (result != gZclSuccess_c)
        result = gZclOTAAbort_c;
        return result;
  }

  /*  broadcast/multicast */
  
  /* validate jitter */ 
  if (pImageNotify->queryJitter > gOtaClientMaxRandomJitter_c)
    dropPacket = TRUE;
  else
    /* compute random jitter */
    clientJitter = GetRandomRange(0, gOtaClientMaxRandomJitter_c);
  
  /* validate manufacturer */
  if (pImageNotify->payloadType > gJitter_c) 
  {
    uint16_t manufacturerCode;
   (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &manufacturerCode, NULL);
    
    pImageNotify->manufacturerCode = OTA2Native16(pImageNotify->manufacturerCode);
    if (pImageNotify->manufacturerCode != manufacturerCode &&
        pImageNotify->manufacturerCode != gOtaManufacturerCodeMatchAll_c)
        dropPacket = TRUE;
  }
  
  /* validate imageType */
  if (pImageNotify->payloadType > gJitterMfg_c) 
  {
    uint16_t imageType;
   (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &imageType, NULL);

    pImageNotify->imageType = OTA2Native16(pImageNotify->imageType);
    if (pImageNotify->imageType != imageType &&
        pImageNotify->imageType != gOtaImageTypeMatchAll_c)
        dropPacket = TRUE;
  }
  
  /* validate fileVersion */
  if (pImageNotify->payloadType > gJitterMfgImage_c) 
  {
    uint32_t currentFileVersion; 
    (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_CurrentFileVersionId_c, gZclClientAttr_c, &currentFileVersion, NULL);
   
    pImageNotify->fileVersion = OTA2Native32(pImageNotify->fileVersion);
    if (pImageNotify->fileVersion == currentFileVersion)
        dropPacket = TRUE;
  }
  
  
  if (!dropPacket &&  clientJitter <= pImageNotify->queryJitter)
    /* send jitter message */
    {
      result = ZCL_OTAClusterClient_NextImageRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);   
      if (result != gZclSuccess_c)
        result = gZclOTAAbort_c;
        return result;      
    }
  else
    /* no further processing */
    return gZclSuccess_c; 
  
}


/******************************************************************************
* ZCL_OTAClusterClient_NextBlockRequest
*
* Sends back a next image request (may be as a result of Image Notify)
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_NextBlockRequest 
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint    
)
{ 
  zclZtcOTABlockRequest_t     blockRequest;
  zbClusterId_t clusterOtaId = {gaZclClusterOTA_c}; 
  uint8_t clientEndPoint = mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint;

  Copy2Bytes(&blockRequest.aNwkAddr,aNwkAddr);
  blockRequest.endPoint = endPoint;
  blockRequest.zclOTABlockRequest.fieldControl = 0x00;
  if(gZclOTABlockRequest_BlockRequestDelayPresent_c == 0x02)
  {
	    
    blockRequest.zclOTABlockRequest.fieldControl |= gZclOTABlockRequest_BlockRequestDelayPresent_c;
    blockRequest.zclOTABlockRequest.blockRequestDelay = Native2OTA16(mOtaClienSession.blockRequestDelay);
  }

 (void)ZCL_GetAttribute(clientEndPoint, clusterOtaId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &blockRequest.zclOTABlockRequest.manufacturerCode, NULL);
 (void)ZCL_GetAttribute(clientEndPoint, clusterOtaId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &blockRequest.zclOTABlockRequest.imageType, NULL);

  blockRequest.zclOTABlockRequest.fileVersion = Native2OTA32(mOtaClienSession.downloadingFileVersion);
  blockRequest.zclOTABlockRequest.fileOffset = Native2OTA32(mOtaClienSession.currentOffset);
  blockRequest.zclOTABlockRequest.maxDataSize = mOtaClienSetup[mOtaClienSession.instanceIndex].maxDataSize;
  return ZCL_OTABlockRequest(&blockRequest);
}
 
/*!
 * @fn 		static zbStatus_t ZCL_OTAClusterClient_NextImageResponse_Handler(zbApsdeDataIndication_t *pIndication) 
 *
 * @brief	 Sends back a next image request (may be as a result of Image Notify)
 *
 */
static zbStatus_t ZCL_OTAClusterClient_NextImageResponse_Handler
(
  zbApsdeDataIndication_t *pIndication
)
{
  zclOTANextImageResponse_t   *pNextImageResponse;
  uint16_t manufacturerCode, imageType;
  uint8_t result;
  uint8_t instanceIndex = OTAClusterClientGetIndexFromOtaSetupTable(pIndication->dstEndPoint, gTmrInvalidTimerID_c);
  if(instanceIndex == gZclCluster_InvalidDataIndex_d)
    return gZclOTAAbort_c;

  pNextImageResponse = (zclOTANextImageResponse_t *)(pIndication->pAsdu + sizeof(zclFrame_t));
  
  /* status is successful */
  if(pNextImageResponse->status == gZclOTASuccess_c)
  {
    /* validate frame length */
    if (pIndication->asduLength - sizeof(zclFrame_t) != sizeof(zclOTANextImageResponse_t))
      return gZclMalformedCommand_c;
    
    pNextImageResponse->manufacturerCode = OTA2Native16(pNextImageResponse->manufacturerCode);
    pNextImageResponse->imageType = OTA2Native16(pNextImageResponse->imageType);
    pNextImageResponse->fileVersion = OTA2Native32(pNextImageResponse->fileVersion);
    pNextImageResponse->imageSize = OTA2Native32(pNextImageResponse->imageSize);    
    
    (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &manufacturerCode, NULL);
    (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &imageType, NULL);

    
    /* validate frame params */
    if ((pNextImageResponse->manufacturerCode != manufacturerCode &&
        pNextImageResponse->manufacturerCode != gOtaManufacturerCodeMatchAll_c) ||
        (pNextImageResponse->imageType != imageType &&
        pNextImageResponse->imageType != gOtaImageTypeMatchAll_c ))   
      return gZclMalformedCommand_c;
    
    /* begin client session */
    result =  OTAClusterClientStartSession(pIndication->dstEndPoint, pNextImageResponse->imageSize, pNextImageResponse->fileVersion);
    if(result != gZbSuccess_c) 
        return gZclOTAAbort_c;
    
    /* stop timer that send query image request periodically */
    /* instance index it is set in the OTAClusterClientStartSession function */
    ZbTMR_FreeTimer(mOtaClienSetup[instanceIndex].imageRequestTimer); 
    mOtaClienSetup[instanceIndex].imageRequestTimer = gTmrInvalidTimerID_c;
    
    gOtaClientRetransmissionBlockTimer = ZbTMR_AllocateTimer(); 
    if(gOtaClientRetransmissionBlockTimer == gTmrInvalidTimerID_c)
       return gZclNoMem_c;	
    
    /* send the first block request */  
    result = ZCL_OTAClusterClient_NextBlockRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);
    if(result != gZclSuccess_c) {
      return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
    }
    return  gZclSuccess_c;
  }
  else
  {
     if(pNextImageResponse->status == gZclOTANoImageAvailable_c)
     {
        if(mOtaClienSetup[instanceIndex].imageRequestTimer == gTmrInvalidTimerID_c) 
        {
            mOtaClienSetup[instanceIndex].imageRequestTimer = ZbTMR_AllocateTimer(); 
            if(mOtaClienSetup[instanceIndex].imageRequestTimer == gTmrInvalidTimerID_c)
              return gZclNoMem_c;	
        }
	ZbTMR_StartMinuteTimer(mOtaClienSetup[instanceIndex].imageRequestTimer, gOtaMinTimeforNextImageRequest_c, OTAClusterClientNextImageReqTimerCallback);
     }
     else
     {        
          zclInitiateOtaProcess_t  initServerDiscovery = {0, 0};
          initServerDiscovery.isServer = FALSE;
          initServerDiscovery.clientEndpoint = mOtaClienSetup[instanceIndex].clientEndpoint;
          (void)OTA_InitiateOtaClusterProcess(&initServerDiscovery);	
      }
      return gZclSuccess_c;
  } 
}


/*!
 * @fn 		static zbStatus_t ZCL_OTAClusterClient_NextImageResponse_Handler(zbApsdeDataIndication_t *pIndication) 
 *
 * @brief	Handles the block response indication
 *
 */
static zbStatus_t ZCL_OTAClusterClient_BlockResponse_Handler
(
  zbApsdeDataIndication_t *pIndication
) 
{
  zclOTABlockResponse_t *pBlockResponse;
  uint16_t manufacturerCode, imageType;

  uint8_t result;
  
  /* Handle succes status received. Verify the received parameters */
  pBlockResponse = (zclOTABlockResponse_t *)(pIndication->pAsdu + sizeof(zclFrame_t));

  if(pBlockResponse->status == gZclOTASuccess_c)
  {
      /* validate frame length */
    if (pIndication->asduLength - sizeof(zclFrame_t) != 
        sizeof(zclOTAStatus_t) + MbrOfs(zclOTABlockResponseStatusSuccess_t, data) + 
        pBlockResponse->msgData.success.dataSize)
      return gZclMalformedCommand_c;
    
    /* command fields */
    pBlockResponse->msgData.success.manufacturerCode = OTA2Native16(pBlockResponse->msgData.success.manufacturerCode);
    pBlockResponse->msgData.success.imageType = OTA2Native16(pBlockResponse->msgData.success.imageType);
    pBlockResponse->msgData.success.fileVersion = OTA2Native32(pBlockResponse->msgData.success.fileVersion);
    pBlockResponse->msgData.success.fileOffset = OTA2Native32(pBlockResponse->msgData.success.fileOffset);
     
   (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &manufacturerCode, NULL);
   (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &imageType, NULL);

    /* Error cases. Send back malformed command indication - no abort of the current upgrade executed. */
    if((pBlockResponse->msgData.success.manufacturerCode != manufacturerCode)||
       (pBlockResponse->msgData.success.imageType != imageType)||
       (pBlockResponse->msgData.success.fileVersion != mOtaClienSession.downloadingFileVersion) ||
       (pBlockResponse->msgData.success.dataSize > mOtaClienSetup[mOtaClienSession.instanceIndex].maxDataSize ) ||
       (pBlockResponse->msgData.success.dataSize + mOtaClienSession.currentOffset > mOtaClienSession.fileLength)) 
    {  
      return gZclMalformedCommand_c;
    }
    /* handle out of sync packets by repeating the request (spec does not handle this as error) */
    else if ( pBlockResponse->msgData.success.fileOffset != mOtaClienSession.currentOffset ) 
    {
      /* send the first block request */   
      result = ZCL_OTAClusterClient_NextBlockRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);
      if(result != gZclSuccess_c) 
      {
        return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
      }
      return  gZclSuccess_c;
    }
    else 
    {
      /* Handle the received data chunk - push it to the image storage if we have a session started */
      if(mOtaClienSession.sessionStarted == TRUE)
      {
        if(gpBlockCallbackState)
        {
          MSG_Free(gpBlockCallbackState);
          gpBlockCallbackState = NULL;
        }
        /* do it on a timer to handle processing aps and 
          writing to external memory sync issues */
        if(gOtaClientBlockProcessTimer == gTmrInvalidTimerID_c)
        	gOtaClientBlockProcessTimer = ZbTMR_AllocateTimer();
        
        if(gpBlockCallbackState == NULL)
        	gpBlockCallbackState = AF_MsgAlloc();

        if (gpBlockCallbackState && (gOtaClientBlockProcessTimer != gTmrInvalidTimerID_c)) 
        {  
           uint16_t blockDelay = (mOtaClienSession.blockRequestDelay > 50)? mOtaClienSession.blockRequestDelay:gOtaBlockProcessingCallbackDelay_c;
          
           Copy2Bytes(&gpBlockCallbackState->dstAddr, &pIndication->aSrcAddr);
           gpBlockCallbackState->dstEndpoint = pIndication->srcEndPoint;
           gpBlockCallbackState->blockSize = pBlockResponse->msgData.success.dataSize;
           FLib_MemCpy(gpBlockCallbackState->blockData,
                       pBlockResponse->msgData.success.data, 
                       pBlockResponse->msgData.success.dataSize);
           
           
           ZbTMR_StartTimer(gOtaClientBlockProcessTimer, gTmrSingleShotTimer_c,
                          blockDelay, OTAClusterClientProcessBlockTimerCallback);
                          
           return gZclSuccess_c;
        } 
        else 
        {
          return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
        }
      }
      else 
      {
        return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
      }
    }
  }
    else if(pBlockResponse->status == gZclOTAWaitForData_c)
    {
    	uint32_t timeInSeconds = OTA2Native32(pBlockResponse->msgData.wait.requestTime) -
						OTA2Native32(pBlockResponse->msgData.wait.currentTime);
        if(gZclOTABlockRequest_BlockRequestDelayPresent_c == 0x02)
        {
          mOtaClienSession.blockRequestDelay = OTA2Native16(pBlockResponse->msgData.wait.blockRequestDelay);       
          /* sets the attribute and will report if needed */
          (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_MinimumBlockRequestDelayId_c, gZclClientAttr_c, &mOtaClienSession.blockRequestDelay);         
        }
        ZbTMR_StartSecondTimer(gOtaClientRetransmissionBlockTimer,(uint16_t)timeInSeconds, OTAClientRetransmitLastBlockTmrCallback); 
        return  gZclSuccess_c;
    }
    else if(pBlockResponse->status == gZclOTAAbort_c)
    {
        return OTAClusterClientAbortSession();
    }
    else /*unknown status */
      /* Error detected. Send back malformed command indication - no abort of the current upgrade executed. */
      return gZclMalformedCommand_c;

}
  
 
/*!
 * @fn 		static zbStatus_t ZCL_OTAClusterClient_NextImageResponse_Handler(zbApsdeDataIndication_t *pIndication) 
 *
 * @brief	Handles the upgrade end response indication
 *
 */
static zbStatus_t ZCL_OTAClusterClient_UpgradeEndResponse_Handler
(
  zbApsdeDataIndication_t *pIndication
) 

{
    zclOTAUpgradeEndResponse_t *pUpgradeEndResponse;
    uint16_t manufacturerCode, imageType;
    
    pUpgradeEndResponse = (zclOTAUpgradeEndResponse_t*)(pIndication->pAsdu + sizeof(zclFrame_t));
    pUpgradeEndResponse->manufacturerCode = OTA2Native16(pUpgradeEndResponse->manufacturerCode);
    pUpgradeEndResponse->imageType = OTA2Native16(pUpgradeEndResponse->imageType);
    pUpgradeEndResponse->fileVersion = OTA2Native32(pUpgradeEndResponse->fileVersion);
    pUpgradeEndResponse->currentTime = OTA2Native32(pUpgradeEndResponse->currentTime);
    pUpgradeEndResponse->upgradeTime = OTA2Native32(pUpgradeEndResponse->upgradeTime);
      
    (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &manufacturerCode, NULL);
    (void)ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &imageType, NULL);

    
    /* Verify the data received in the response  */
    if((pUpgradeEndResponse->manufacturerCode != manufacturerCode)||
       (pUpgradeEndResponse->imageType != imageType)||
       (pUpgradeEndResponse->fileVersion != mOtaClienSession.downloadingFileVersion))
    {
      return gZclMalformedCommand_c;
    }
    else
    {
      uint32_t offset = pUpgradeEndResponse->upgradeTime - pUpgradeEndResponse->currentTime;

      if(offset == 0)
      {   
        offset++;
        if(mOtaClienSession.steps >= 3)
        {
          /* allow at least 1 second before reset */	
          gOtaClientBlockProcessTimer = ZbTMR_AllocateTimer();
          /* Flash flags will be write in next instance of idle task  */
          OTA_SetNewImageFlag();
          /* OTA TODO this does not accept 32 bit second timers */
          ZbTMR_StartSecondTimer(gOtaClientBlockProcessTimer, (uint16_t)offset, OTAClusterCPUResetCallback);      
        }
        else
        {
        	(void)OTAClusterClientAbortSession();
        }
      }
      else
      {
        if(pUpgradeEndResponse->upgradeTime == 0xFFFFFFFF)
        {
            /* wait to upgrate after receiving the 2nd Upgrade End Response   */
            uint8_t upgradeStatus = gOTAUpgradeStatusWaitingToUpgrade_c;      
            /* sets the attribute and will report if needed */
            (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_ImageUpgradeStatusId_c, gZclClientAttr_c, &upgradeStatus);
        }
        else
        {
            uint8_t upgradeStatus = gOTAUpgradeStatusCountDown_c;      
            /* sets the attribute and will report if needed */
            (void)ZCL_SetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrOTA_ImageUpgradeStatusId_c, gZclClientAttr_c, &upgradeStatus);

            /* wait offset seconds after the client shall perform to apply the upgrade process; */
            gOtaClientBlockProcessTimer = ZbTMR_AllocateTimer();
            
            /* OTATODO this does not accept 32 bit second timers */
            ZbTMR_StartSecondTimer(gOtaClientBlockProcessTimer, (uint16_t)offset, OTAClusterDelayedUpgradeCallback);  
        }
      } 
      return gZclSuccess_c;
    }
}

/*!
 * @fn 		static zbStatus_t ZCL_OTAClusterClient_DefaultResponse_Handler(zbApsdeDataIndication_t *pIndication) 
 *
 * @brief	Handles the upgrade end response indication
 *
 */
static zbStatus_t ZCL_OTAClusterClient_DefaultResponse_Handler
(
  zbApsdeDataIndication_t *pIndication
) 
{
  zclCmdDefaultRsp_t * pDefaultRsp;
  
  pDefaultRsp = ZCL_GetPayload(pIndication);
  /* abort the session when receive an DefaultResponse with status 0x95 for End Request Command; */
  if(((uint8_t)(pDefaultRsp->status) == gZclOTAAbort_c)&&(pDefaultRsp->cmd==gZclCmdOTA_UpgradeEndRequest_c))
  {
      return OTAClusterClientAbortSession();
  }
  return gZclSuccess_c;
}



/******************************************************************************
* OTAClusterDelayedUpgradeCallback
*
* Resets the node to upgrade after Upgrade End Response
******************************************************************************/
static void OTAClusterDelayedUpgradeCallback(uint8_t tmr) 
{
  (void)tmr;  
  /* Flash flags will be write in next instance of idle task */
  if(mOtaClienSession.steps >= 3)
  { 
      OTA_SetNewImageFlag();
      /* allow at least 1 second before reset */
      ZbTMR_StartSecondTimer(gOtaClientBlockProcessTimer, 0x01, OTAClusterCPUResetCallback); 
  }
  else
  {
      (void)OTAClusterClientAbortSession();
  }
}

/******************************************************************************
* OTAClusterResetCallback
*
* Resets the node to upgrade after Upgrade End Response
******************************************************************************/
static void OTAClusterCPUResetCallback(uint8_t tmr) 
{
  (void)tmr;
#ifndef __IAR_SYSTEMS_ICC__
  /* S08 platform reset */
  PWRLib_Reset();               
#else
#ifdef PROCESSOR_KINETIS
  PWRLib_Reset(); 
#else  
  CRM_SoftReset();
#endif
#endif  
}


/******************************************************************************
* ZCL_OTASetClientParams
*
* Interface function to set client parameters
******************************************************************************/
zbStatus_t ZCL_OTASetClientParams(zclOTAClientParams_t* pClientParams)
{
  zbClusterId_t clusterId = {gaZclClusterOTA_c};  
  uint8_t i;  
  
  for(i=0; i< gNoOfOtaClusterInstances_d; i++)
  {
    if(mOtaClienSetup[i].clientEndpoint != gZclCluster_InvalidDataIndex_d)
    {
      /* update OTA attributes*/
      (void)ZCL_SetAttribute(mOtaClienSetup[i].clientEndpoint, clusterId, gZclAttrOTA_CurrentFileVersionId_c, gZclClientAttr_c, &pClientParams->currentFileVersion);
      (void)ZCL_SetAttribute(mOtaClienSetup[i].clientEndpoint, clusterId, gZclAttrOTA_DownloadedFileVersionId_c, gZclClientAttr_c, &pClientParams->downloadedFileVersion);
      (void)ZCL_SetAttribute(mOtaClienSetup[i].clientEndpoint, clusterId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &pClientParams->manufacturerCode);
      (void)ZCL_SetAttribute(mOtaClienSetup[i].clientEndpoint, clusterId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &pClientParams->imageType);
      (void)ZCL_SetAttribute(mOtaClienSetup[i].clientEndpoint, clusterId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &pClientParams->downloadedFileVersion);
 
      mOtaClienSetup[i].hardwareVersion = pClientParams->hardwareVersion;
      mOtaClienSetup[i].hardwareVersion = OTA2Native16(mOtaClienSetup[i].hardwareVersion);
    }
  }
  return gZclSuccess_c;
}

/******************************************************************************
* ZCL_OTAImageRequest
*
* Request to send a image request
******************************************************************************/
zbStatus_t ZCL_OTAImageRequest(zclZtcOTANextImageRequest_t* pZtcNextImageRequestParams)
{
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif  
  
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcNextImageRequestParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcNextImageRequestParams->endPoint;
  addrInfo.srcEndPoint = mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint; 
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  return ZCL_OTAImageRequestCmd(addrInfo, pZtcNextImageRequestParams->zclOTANextImageRequest);
}

/******************************************************************************
* ZCL_OTAImageRequestCmd
*
* Request to send a image request command
******************************************************************************/
static zbStatus_t ZCL_OTAImageRequestCmd(afAddrInfo_t addrInfo, zclOTANextImageRequest_t zclOTANextImageRequest)
{
  afToApsdeMessage_t *pMsg;	
  uint8_t len = sizeof(zclOTANextImageRequest_t);  
  
  if(!(zclOTANextImageRequest.fieldControl & gZclOTANextImageRequest_HwVersionPresent_c)) 
    len -=sizeof(uint16_t); /* remove hardware version from packet */   

  /* store the current TSQ */
  mOtaClienSession.requestedTsq = ZbZclFoundationGlobals(gZclTransactionId);
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_QueryNextImageRequest_c,
                          gZclFrameControl_FrameTypeSpecific, 
                          NULL, 
                          &len,
                          &zclOTANextImageRequest);
  if(!pMsg)
    return gZclNoMem_c;
  
 return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/******************************************************************************
* ZCL_OTABlockRequest
*
* Request to send an image block
******************************************************************************/
zbStatus_t ZCL_OTABlockRequest(zclZtcOTABlockRequest_t *pZtcBlockRequestParams)
{
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif  
  
  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcBlockRequestParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcBlockRequestParams->endPoint;
  addrInfo.srcEndPoint = mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint;  
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  return ZCL_OTABlockRequestCmd(addrInfo, pZtcBlockRequestParams->zclOTABlockRequest);
}

/******************************************************************************
* ZCL_OTABlockRequestCmd
*
* Request to send an image block request
******************************************************************************/
static zbStatus_t ZCL_OTABlockRequestCmd(afAddrInfo_t addrInfo, zclOTABlockRequest_t  zclOTABlockRequest)
{
   afToApsdeMessage_t *pMsg;	
   uint8_t len = sizeof(zclOTABlockRequest_t);
   
   if(!(zclOTABlockRequest.fieldControl & gZclOTABlockRequest_BlockRequestDelayPresent_c))
     len -=sizeof(uint16_t); /* remove block request delay from packet */ 
   
   /* store the current TSQ */
     mOtaClienSession.requestedTsq = ZbZclFoundationGlobals(gZclTransactionId);
   
   pMsg = ZCL_CreateFrame( &addrInfo, 
	                       gZclCmdOTA_ImageBlockRequest_c,
	                       gZclFrameControl_FrameTypeSpecific, 
	                       NULL, 
	                       &len,
	                       &zclOTABlockRequest);
   if(!pMsg)
	   return gZclNoMem_c;
   
   return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  	
}


/******************************************************************************
* ZCL_OTAUpgradeEndRequest
*
* Request to send an upgrade end request
******************************************************************************/
zbStatus_t ZCL_OTAUpgradeEndRequest(zclZtcOTAUpgradeEndRequest_t *pZtcUpgradeEndParams)
{
  afToApsdeMessage_t *pMsg;
  uint8_t len;
  
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif

  /* Create the destination address */
  Copy2Bytes(&addrInfo.dstAddr,&pZtcUpgradeEndParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcUpgradeEndParams->endPoint;
  addrInfo.srcEndPoint = mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  len = sizeof(zclOTAUpgradeEndRequest_t);

  /* store the current TSQ */
    mOtaClienSession.requestedTsq = ZbZclFoundationGlobals(gZclTransactionId);
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_UpgradeEndRequest_c,
                          gZclFrameControl_FrameTypeSpecific, 
                          NULL, 
                          &len,
                          &pZtcUpgradeEndParams->zclOTAUpgradeEndRequest);
  if(!pMsg)
    return gZclNoMem_c;
 return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/******************************************************************************
* OTAClusterClientAbortSession
*
* Private function. Abort a started image download session
******************************************************************************/
static zbStatus_t OTAClusterClientAbortSession()
{
  zbClusterId_t clusterId = {gaZclClusterOTA_c};
  uint8_t       upgradeStatus = gOTAUpgradeStatusNormal_c;  
  uint16_t      minimumBlockDelay = 0;
  uint32_t      downloadedFileVersion; 
  uint8_t       clientEndpoint = mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint;
    
  mOtaClienSession.sessionStarted = FALSE;
  mOtaClienSession.fileLength = 0;
  mOtaClienSession.currentOffset = 0;  
  mOtaClienSession.steps = 0;
  mOtaClienSession.requestedTsq = 0;
  
  /* clear OTA client session timer */
  if(gOtaClientBlockProcessTimer != gTmrInvalidTimerID_c)
  {
    ZbTMR_FreeTimer(gOtaClientBlockProcessTimer);
    gOtaClientBlockProcessTimer = gTmrInvalidTimerID_c;
  }
  
  /*clear OTA client session Callback */
  MSG_Free(gpBlockCallbackState);
  gpBlockCallbackState = NULL;
   
  /* restore OTA attributes */
  (void)ZCL_SetAttribute(clientEndpoint, clusterId, gZclAttrOTA_ImageUpgradeStatusId_c, gZclClientAttr_c, &upgradeStatus);
  (void)ZCL_SetAttribute(clientEndpoint, clusterId, gZclAttrOTA_FileOffsetId_c, gZclClientAttr_c, &mOtaClienSession.currentOffset);
  (void)ZCL_SetAttribute(clientEndpoint, clusterId, gZclAttrOTA_MinimumBlockRequestDelayId_c, gZclClientAttr_c, &minimumBlockDelay);
  /* get Current file version */
  (void)ZCL_GetAttribute(clientEndpoint, clusterId, gZclAttrOTA_CurrentFileVersionId_c, gZclClientAttr_c, &downloadedFileVersion, NULL);
  /* set downloaded file version */
  (void)ZCL_SetAttribute(clientEndpoint, clusterId, gZclAttrOTA_DownloadedFileVersionId_c, gZclClientAttr_c, &downloadedFileVersion);
 
#if (gZclEnableOTAProgressReport_d)
  mOtaClienSession.progressReport = otaStartProgress_c; /*4 states available:  0 = startProgress, 1 = 33%, 2 = 66%, 3 = 100% */
  BeeAppUpdateDevice(clientEndpoint, gOTAProgressReportEvent_c, 0, 0, NULL);
#endif 
  
  
  if(mOtaClienSession.pStateBuffer != NULL)
  {
    MSG_Free(mOtaClienSession.pStateBuffer);
    mOtaClienSession.pStateBuffer = NULL;
  }
  OTA_CancelImage();
  //(void)OTA_EraseExternalMemory();
  
#if TWR_KW24D512 || TWR_KW22D512 || TWR_KW21D256 || TWR_KW21D512
#if !gUseInternalFlashForOta_c  
  /* re-Init the keyboard module: for the specified platforms the keyboard and external EEprom are on the same Bus */
  KBD_Init(BeeAppHandleKeys);
#endif /* !gUseInternalFlashForOta_c */  
#endif /* TWR_KW24D512 || TWR_KW22D512 || TWR_KW21D256 || TWR_KW21D512 */ 
  
#if gEndDevCapability_d || gComboDeviceCapability_d 
  /* stop the OTA FastPoll Mode */
  if(gOtaClientRetransmissionBlockTimer == gTmrInvalidTimerID_c)
	 gOtaClientRetransmissionBlockTimer = ZbTMR_AllocateTimer(); 
  ZbTMR_StartSecondTimer(gOtaClientRetransmissionBlockTimer, 1, OTAClient_UpdatePollRateCallBack);
#else
  if(gOtaClientRetransmissionBlockTimer != gTmrInvalidTimerID_c)
  {
	  ZbTMR_FreeTimer(gOtaClientRetransmissionBlockTimer); 
	  gOtaClientRetransmissionBlockTimer = gTmrInvalidTimerID_c;
  }
#endif     
  
  /* start timer for the next QueryRequest */
  mOtaClienSetup[mOtaClienSession.instanceIndex].imageRequestTimer = ZbTMR_AllocateTimer(); 
  if(mOtaClienSetup[mOtaClienSession.instanceIndex].imageRequestTimer == gTmrInvalidTimerID_c)
     return gZclNoMem_c;
  ZbTMR_StartMinuteTimer(mOtaClienSetup[mOtaClienSession.instanceIndex].imageRequestTimer, gOtaMinTimeforNextImageRequest_c, OTAClusterClientNextImageReqTimerCallback);

  return gZclSuccess_c;
}

#if gEndDevCapability_d || gComboDeviceCapability_d 
/******************************************************************************
* OTAClient_UpdatePollRateCallBack
*
* stop OTA Client Fast Poll Mode  
******************************************************************************/
static void OTAClient_UpdatePollRateCallBack(uint8_t tmrId)
{  
	(void)tmrId;

	if (!IsLocalDeviceReceiverOnWhenIdle())
	{ 
           uint32_t longPollInterval = mDefaultValueOfIndirectPollRate_c;
#if gZclEnablePollControlCluster_d     
           longPollInterval = (ZclPollControl_GetMinLongPollIntervalValue())*1000/4;
#endif
	    
          if(NlmeGetRequest(gNwkIndirectPollRate_c) < longPollInterval)
	  {
	      (void)ZDO_NLME_ChangePollRate(longPollInterval);
#ifdef PROCESSOR_KINETIS
                    /* Save the new pollRate in NVM */
                    NvSaveOnIdle(&gSAS_Ram, TRUE);
#else		    
                    ZCL_SaveNvmZclData();
#endif                    
	   }
        }
        ZbTMR_FreeTimer(gOtaClientRetransmissionBlockTimer); 
        gOtaClientRetransmissionBlockTimer = gTmrInvalidTimerID_c;
}
#endif	

/******************************************************************************
* OTAClusterClientStartSession
*
* Private function. Starts an image download session.
******************************************************************************/
static zbStatus_t OTAClusterClientStartSession(uint8_t endpoint, uint32_t fileLength, uint32_t fileVersion)
{
  zbStatus_t status = gZbFailed_c; 
  
  mOtaClienSession.instanceIndex = OTAClusterClientGetIndexFromOtaSetupTable(endpoint, gTmrInvalidTimerID_c);
  if(mOtaClienSession.instanceIndex != gZclCluster_InvalidDataIndex_d)
  {
    zbClusterId_t clusterId = {gaZclClusterOTA_c};
    uint8_t upgradeStatus = gOTAUpgradeStatusDownloadInProgress_c;  
    
    /* init session parameters: */
    mOtaClienSession.sessionStarted = TRUE;
    mOtaClienSession.fileLength = fileLength;
    mOtaClienSession.downloadingFileVersion = fileVersion;
    mOtaClienSession.currentOffset = 0;  
    mOtaClienSession.state = OTAClient_InitState_c;
    mOtaClienSession.steps = 0;
    mOtaClienSession.requestedTsq = 0;
    mOtaClienSession.bytesNeededForState = gOtaHeaderLenOffset_c + sizeof(uint16_t);
    mOtaClienSession.stateBufferIndex = 0;
    mOtaClienSession.retransmissionCounter = 0;
  
    if(mOtaClienSession.pStateBuffer == NULL)
    {
        mOtaClienSession.pStateBuffer = MSG_Alloc(gOtaSessionBufferSize_c);
        if(mOtaClienSession.pStateBuffer == NULL) 
            return gZbNoMem_c;
    }

    #if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d)
      FLib_MemSet(&mOtaClienSession.msgDigest[0], 0x00, AES_MMO_SIZE);
    #endif  
    
    #if (gZclEnableOTAProgressReport_d)
      mOtaClienSession.progressReport = otaStartProgress_c; /*4 states available:  0 = startProgress, 1 = 33%, 2 = 66%, 3 = 100% */
    #endif  
  
    /* update upgrade status */
    (void)ZCL_SetAttribute(mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint, clusterId, gZclAttrOTA_ImageUpgradeStatusId_c, gZclClientAttr_c, &upgradeStatus);
   
    #if gEndDevCapability_d || gComboDeviceCapability_d
      if (!IsLocalDeviceReceiverOnWhenIdle())
      {     
          /* update poll rate during the OTA process: go to FastPollMode */ 	
          uint16_t pollRate = 250;
          #if gZclEnablePollControlCluster_d
              pollRate = (ZclPollControl_GetMaxShortPollIntervalValue())*1000/4;     
          #endif
          (void)ZDO_NLME_ChangePollRate(pollRate);
      }
    #endif 
    status = gZbSuccess_c;
  }
  return status;
}


/*****************************************************************************
* OTAClusterClientGetIndexFromOtaSetupTable Fuction
* -return - index in the ota setup table if succes
          - invalid data - otherwise
*****************************************************************************/
static uint8_t OTAClusterClientGetIndexFromOtaSetupTable(uint8_t endpoint, uint8_t tmrId)
{
  if(endpoint != gZclCluster_InvalidDataIndex_d) 
  {  
    for(uint8_t i=0;i<gNoOfOtaClusterInstances_d; i++)
      if(endpoint == mOtaClienSetup[i].clientEndpoint)
        return i;
  }
  if(tmrId != gTmrInvalidTimerID_c)
  {
     for(uint8_t i=0;i<gNoOfOtaClusterInstances_d; i++)
      if(tmrId == mOtaClienSetup[i].imageRequestTimer)
        return i;
  }
  return gZclCluster_InvalidDataIndex_d;
}


/******************************************************************************
* OTAClusterClientRunImageProcessStateMachine
*
* Private function. Process a block of received data.
******************************************************************************/
static zbStatus_t OTAClusterClientRunImageProcessStateMachine()
{
  zclOTAFileHeader_t* pHeader;
  zclOTAFileSubElement_t* pSubElement;
  static uint32_t subElementLen;
  static uint8_t *pBitmap;
  uint16_t headerLen;
  zbClusterId_t clusterOtaId = {gaZclClusterOTA_c};  
  
#if (!gZclEnableOTAClientECCLibrary_d)
  uint32_t crcReceived = 0;
  /* Current CRC value */
  static uint16_t mCrcCompute  = 0;
#else
  static IdentifyCert_t mCertReceived;
#endif
  
  switch(mOtaClienSession.state)
  {
    case OTAClient_InitState_c:  
        /* In the init state we only extract the header length and move to the next state.
         * The bytes received so far are not consumed */
        headerLen = *(uint16_t*)(mOtaClienSession.pStateBuffer + gOtaHeaderLenOffset_c);
        mOtaClienSession.bytesNeededForState = (uint8_t)OTA2Native16(headerLen);
        mOtaClienSession.state = OTAClient_ProcessHeaderState_c;
        #if (!gZclEnableOTAClientECCLibrary_d)
        mCrcCompute  = 0;
        #endif
        break;
    case OTAClient_ProcessHeaderState_c:
      {
        uint16_t manufacturerCode, imageType;
        
        pHeader = (zclOTAFileHeader_t*)mOtaClienSession.pStateBuffer;
        #if (!gZclEnableOTAClientECCLibrary_d)
        /* process image CRC */
        mCrcCompute = OTA_CrcCompute(mOtaClienSession.pStateBuffer, mOtaClienSession.bytesNeededForState, mCrcCompute);
        #endif    
        (void)ZCL_GetAttribute(mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint, clusterOtaId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &manufacturerCode, NULL);
        (void)ZCL_GetAttribute(mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint, clusterOtaId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &imageType, NULL);

        /* Check the header for consistency */
        if((pHeader->headerVersion != gZclOtaHeaderVersion_c)||((OTA2Native16(pHeader->imageType)!= gOtaImageTypeMatchAll_c)&&(OTA2Native16(pHeader->imageType) != imageType))||
          ((OTA2Native16(pHeader->manufacturerCode) != manufacturerCode)&&(OTA2Native16(pHeader->manufacturerCode) != gOtaManufacturerCodeMatchAll_c)))
        {
            return gZclOTAAbort_c;
        }
    
        /* check the field control for supported features - upgrade file destination and security credential version not supported */
        if((OTA2Native16(pHeader->fieldControl) & SECURITY_CREDENTIAL_VERSION_PRESENT)||(OTA2Native16(pHeader->fieldControl) & DEVICE_SPECIFIC_FILE))
            return gZclOTAAbort_c;
    
        /* If HW version is specified, verify it against our own */
        if(OTA2Native16(pHeader->fieldControl) & HARDWARE_VERSION_PRESENT)
        {
            if(!((OTA2Native16(pHeader->minHWVersion) <= mOtaClienSetup[mOtaClienSession.instanceIndex].hardwareVersion)&&(mOtaClienSetup[mOtaClienSession.instanceIndex].hardwareVersion <= OTA2Native16(pHeader->maxHWVersion))))
                return gZclOTAAbort_c;
        }
    
        /* If we got here it means we are ready for the upgrade image tag. 
        * All bytes from the buffer have been processed. The next state requires receiving a sub-element */
        mOtaClienSession.state = OTAClient_ProcessSubElementTagState_c;
        mOtaClienSession.bytesNeededForState =  sizeof(zclOTAFileSubElement_t);
        mOtaClienSession.stateBufferIndex = 0;
        mOtaClienSession.steps++;
        break;
      }
    case OTAClient_ProcessSubElementTagState_c:
        {
          pSubElement = (zclOTAFileSubElement_t*)mOtaClienSession.pStateBuffer;
          #if (!gZclEnableOTAClientECCLibrary_d)   
          /* process image CRC */ 
          if(pSubElement->id != gZclOtaUpgradeCRCTagId_c)
          {
              mCrcCompute = OTA_CrcCompute(mOtaClienSession.pStateBuffer, mOtaClienSession.bytesNeededForState, mCrcCompute);
          }
          #endif /* (!gZclEnableOTAClientECCLibrary_d) */
    
          switch(pSubElement->id)
          {      
              case gZclOtaUpgradeImageTagId_c:
                  mOtaClienSession.state = OTAClient_ProcessUpgradeImageState_c;
                  /* All OK, get the image length */
                  subElementLen = OTA2Native32(pSubElement->length);
                  /* Start the FLASH upgrade process */
                  if(OTA_StartImage(subElementLen) != gOtaSucess_c)
                      return gZclOTAAbort_c;   
                  mOtaClienSession.steps++;
                  mOtaClienSession.bytesNeededForState = (uint8_t)((subElementLen > gOtaSessionBufferSize_c) ? gOtaSessionBufferSize_c : subElementLen);
                  break;
              case gZclOtaSectorBitmapTagId_c:  
                  mOtaClienSession.state = OTAClient_ProcessBitmapState_c;
                  mOtaClienSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
                  break;
              #if (!gZclEnableOTAClientECCLibrary_d) 	          
              case gZclOtaUpgradeCRCTagId_c:
                  mOtaClienSession.state = OTAClient_ProcessCRCState_c;
                  mOtaClienSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
                  break;          
              case gZclOtaImageIntegrityCodeTagId:
                  mOtaClienSession.state = OTAClient_ProcessImgIntegrityCodeState_c;
                  mOtaClienSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
                  break;        
              #else        
              case gZclOtaECDSASigningCertTagId_c: 
                  mOtaClienSession.state = OTAClient_ProcessECDSASignCertState_c;
                  mOtaClienSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
                  break;
              case gZclOtaECDSASignatureTagId_c: 
                  mOtaClienSession.state = OTAClient_ProcessECDSASigningState_c;
                  mOtaClienSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
                break;
              #endif        
              default:
                  mOtaClienSession.state = OTAClient_StateMax_c;
                  mOtaClienSession.bytesNeededForState = 1;   
                  break;
          }     
          mOtaClienSession.stateBufferIndex = 0;
          break;
      }
    case OTAClient_ProcessUpgradeImageState_c:
        #if (!gZclEnableOTAClientECCLibrary_d) 
        /* process image CRC */
        mCrcCompute = OTA_CrcCompute(mOtaClienSession.pStateBuffer, mOtaClienSession.bytesNeededForState, mCrcCompute);
        #endif  
    
        /* New image chunk arrived. upgradeImageLen is updated by the OTA platform component. */
        if(OTA_PushImageChunk(mOtaClienSession.pStateBuffer, mOtaClienSession.bytesNeededForState, NULL) != gOtaSucess_c)
        {
            return gZclOTAAbort_c;
        }
        subElementLen-=mOtaClienSession.bytesNeededForState;
        /* Prepare for next chunk or next state if the image was downloaded */
        if(subElementLen != 0)
        {
            /* More chuncks to come */
            mOtaClienSession.bytesNeededForState =  (uint8_t)((subElementLen > gOtaSessionBufferSize_c) ? gOtaSessionBufferSize_c : subElementLen);
            mOtaClienSession.stateBufferIndex = 0;
        }
        else
        {
            /* We need to move to the next state */
            mOtaClienSession.state = OTAClient_ProcessSubElementTagState_c;
            mOtaClienSession.bytesNeededForState = sizeof(zclOTAFileSubElement_t);
            mOtaClienSession.stateBufferIndex = 0;
        }
        break;  
    case OTAClient_ProcessBitmapState_c:
        pBitmap = MSG_Alloc(mOtaClienSession.bytesNeededForState); 
        if(!pBitmap) 
            return gZclOTAAbort_c;
        FLib_MemCpy(pBitmap, mOtaClienSession.pStateBuffer, (mOtaClienSession.bytesNeededForState));   
        #if (!gZclEnableOTAClientECCLibrary_d)
        /* process image CRC */
        mCrcCompute = OTA_CrcCompute(mOtaClienSession.pStateBuffer, mOtaClienSession.bytesNeededForState, mCrcCompute);
        #endif    
        mOtaClienSession.state = OTAClient_ProcessSubElementTagState_c;
        mOtaClienSession.bytesNeededForState = sizeof(zclOTAFileSubElement_t);
        mOtaClienSession.stateBufferIndex = 0;
        mOtaClienSession.steps++;
        break;  
    #if (!gZclEnableOTAClientECCLibrary_d)   
    case OTAClient_ProcessImgIntegrityCodeState_c:
    case OTAClient_ProcessCRCState_c:
        if(mOtaClienSession.state == OTAClient_ProcessCRCState_c) 
        {
            /* check the CRC Value */
            FLib_MemCpy(&crcReceived, mOtaClienSession.pStateBuffer, sizeof(crcReceived));
            crcReceived = OTA2Native32(crcReceived);
            if(crcReceived != mCrcCompute)
                return gZclOTAInvalidImage_c; 
        }
        #if gZclOtaClientImgIntegrityCodeValidation_d    
        if(mOtaClienSession.state == OTAClient_ProcessImgIntegrityCodeState_c)
        {
            /* check the Image Integrity Code Value */
            uint8_t hashReceived[AES_MMO_SIZE];
            FLib_MemCpy(&hashReceived, mOtaClienSession.pStateBuffer, AES_MMO_SIZE);
            if(!FLib_MemCmp(mOtaClienSession.msgDigest, hashReceived, AES_MMO_SIZE))
                return gZclOTAInvalidImage_c;
        } 
        #endif     
    #else
    case OTAClient_ProcessECDSASigningState_c:
        /* ECDSA signature Validation */
        if(OtaSignatureVerification((uint8_t*)&mOtaClienSession.pStateBuffer[0], mCertReceived, (uint8_t*)&mOtaClienSession.pStateBuffer[sizeof(zbIeeeAddr_t)])!= gZbSuccess_c)
            return gZclOTAInvalidImage_c;  
    #endif  /* #if(gEccIncluded_d == FALSE)  */ 
        /* We need to close the written image here; commit image has different prototype on ARM7 vs S08 */
        if(mOtaClienSession.steps >= 3)
        {
            #if (gBigEndian_c != TRUE)
              #ifndef PROCESSOR_KINETIS     
                  if(OTA_CommitImage(FALSE, *(uint32_t *)(pBitmap)) != gOtaSucess_c) return gZclOTAAbort_c;
              #else
                  if(OTA_CommitImage(pBitmap) != gOtaSucess_c) return gZclOTAAbort_c;
              #endif      
            #else
                if(OTA_CommitImage(pBitmap) != gOtaSucess_c) return gZclOTAAbort_c;
            #endif    
            MSG_Free(pBitmap); 
        }
        /* Advance to an illegal state. This state machine should not be called again in this upgrade session. */
        mOtaClienSession.state = OTAClient_StateMax_c;
        mOtaClienSession.bytesNeededForState = 1; 
        mOtaClienSession.stateBufferIndex = 0;    
        break;
    #if (gZclEnableOTAClientECCLibrary_d == TRUE)	  
    case OTAClient_ProcessECDSASignCertState_c:  
        /* store the certificate */
        FLib_MemCpy(&mCertReceived, mOtaClienSession.pStateBuffer, sizeof(IdentifyCert_t));
        /* Prepare for next state */
        mOtaClienSession.state = OTAClient_ProcessSubElementTagState_c;
        mOtaClienSession.bytesNeededForState = sizeof(zclOTAFileSubElement_t);
        mOtaClienSession.stateBufferIndex = 0;
        break;    
    #endif /* #if(gEccIncluded_d == TRUE) */   
    case OTAClient_StateMax_c:
    default:
        return gZclOTAAbort_c;
    }  
 
  return gZbSuccess_c;
}

/******************************************************************************
* OTAClusterClientProcessBlock
*
* Private function. Process a block of received data.
******************************************************************************/
zbStatus_t OTAClusterClientProcessBlock(uint8_t *pImageBlock, uint8_t blockLength)
{
  uint8_t bytesToCopy;
  uint8_t bytesCopied = 0;
  zbStatus_t result = gZbSuccess_c; 
  
#if gZclEnableOTAClientECCLibrary_d 
  uint8_t signatureSize = 2*SECT163K1_SIGNATURE_ELEMENT_LENGTH;
#endif
#if  gZclOtaClientImgIntegrityCodeValidation_d
  uint8_t signatureSize = AES_MMO_SIZE;
#endif  
  
#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d) 
  uint8_t sizeBlock = 2*AES_MMO_SIZE;
  static uint8_t aesMmoBlock[2*AES_MMO_SIZE];
  static uint8_t mPosition = 0;
  static bool_t lastBlockForHash = FALSE;
  
  if(mOtaClienSession.fileLength > (mOtaClienSession.currentOffset + blockLength + signatureSize))
  {
    while(bytesCopied < blockLength)
    {
        lastBlockForHash=FALSE;
        bytesToCopy = sizeBlock - mPosition;
        if(bytesToCopy > (blockLength - bytesCopied))
        {
            bytesToCopy = (blockLength - bytesCopied);
        }
        FLib_MemCpy(aesMmoBlock+mPosition, pImageBlock+bytesCopied, bytesToCopy);
        bytesCopied +=bytesToCopy;
        mPosition+=bytesToCopy;
        if(mPosition == sizeBlock)
        {
              OTAClusterAesMMO_hash(aesMmoBlock, sizeBlock, lastBlockForHash, mOtaClienSession.fileLength-signatureSize, mOtaClienSession.msgDigest);
              mPosition = 0;
        }
      }
  }
  else
  {
    if(lastBlockForHash==FALSE)
    {
      uint8_t *lastAesMmoBlock;
      lastBlockForHash = TRUE;
      bytesToCopy = mOtaClienSession.fileLength - mOtaClienSession.currentOffset - signatureSize;
      sizeBlock = mPosition + bytesToCopy;
      
      lastAesMmoBlock = MSG_Alloc(sizeBlock);
      
      if(lastAesMmoBlock)
      {
        FLib_MemCpy(lastAesMmoBlock, aesMmoBlock, mPosition);
        FLib_MemCpy(lastAesMmoBlock+mPosition, pImageBlock, bytesToCopy);
        OTAClusterAesMMO_hash(lastAesMmoBlock, sizeBlock, lastBlockForHash, mOtaClienSession.fileLength-signatureSize, mOtaClienSession.msgDigest);
        mPosition = 0;
        MSG_Free(lastAesMmoBlock);
      }
    }
  }
#endif  
  
  bytesCopied = 0;
  while(bytesCopied < blockLength)
  {
    bytesToCopy = mOtaClienSession.bytesNeededForState - mOtaClienSession.stateBufferIndex;
    if(bytesToCopy > (blockLength - bytesCopied))
    {
      bytesToCopy = (blockLength - bytesCopied);
    }
    FLib_MemCpy(mOtaClienSession.pStateBuffer + mOtaClienSession.stateBufferIndex, pImageBlock + bytesCopied, bytesToCopy);
    bytesCopied +=bytesToCopy;
    mOtaClienSession.stateBufferIndex+=bytesToCopy;
    if(mOtaClienSession.stateBufferIndex == mOtaClienSession.bytesNeededForState)
    {
      result = OTAClusterClientRunImageProcessStateMachine();
      if(result != gZbSuccess_c) return result;
    }
  }
 
 #if  (gZclEnableOTAProgressReport_d == TRUE) 
  {
	  static uint8_t mSendReport = 0;
	  if((mOtaClienSession.currentOffset > mOtaClienSession.fileLength/3) && (mOtaClienSession.currentOffset < 2*mOtaClienSession.fileLength/3))
		  mOtaClienSession.progressReport = otaProgress33_c;
	  else
		  if((mOtaClienSession.currentOffset > 2*mOtaClienSession.fileLength/3) && (mOtaClienSession.currentOffset < mOtaClienSession.fileLength))
			  mOtaClienSession.progressReport = otaProgress66_c;
	  mSendReport++;
	  if(mSendReport%2 == 0)
	  {
		  mSendReport = 0;
		  BeeAppUpdateDevice(mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint, gOTAProgressReportEvent_c, 0, 0, NULL);
	  }
  }	
#endif  
   
#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)  
  {
    zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
    if((mOtaClienSession.currentOffset == 0)||((mOtaClienSession.currentOffset+blockLength)%(blockLength*100) == 0)||(mOtaClienSession.currentOffset+blockLength ==  mOtaClienSession.fileLength))
    {
      ztcOtaProgressReportInf.currentOffset = Native2OTA32(mOtaClienSession.currentOffset + blockLength);
      ztcOtaProgressReportInf.imageLength = Native2OTA32(mOtaClienSession.fileLength);
      Copy2Bytes(ztcOtaProgressReportInf.deviceAddr, NlmeGetRequest(gNwkShortAddress_c));   
#ifndef gHostApp_d  
        ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
        ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
    }
  }
#endif   //gZclEnableOTAProgressReportToExternApp_d
  return result;
}


/******************************************************************************
* OTAClusterClientProcessBlockTimerCallback
*
* Timer callback to process block indications
******************************************************************************/
static void OTAClusterClientProcessBlockTimerCallback(uint8_t tmr) 
{
  zbStatus_t          result = gZclUnsupportedClusterCommand_c;    
  zbClusterId_t clusterId = {gaZclClusterOTA_c};
  uint32_t fileOffset, downloadedFileVersion;
  
  (void) tmr;
                                                               
  result = OTAClusterClientProcessBlock(gpBlockCallbackState->blockData, gpBlockCallbackState->blockSize);
  ZbTMR_FreeTimer(gOtaClientBlockProcessTimer);
  gOtaClientBlockProcessTimer = gTmrInvalidTimerID_c;
  
  if(result != gZbSuccess_c)
  {
     (void)ZCL_OTAClusterClient_EndUpgradeAbortRequest(gpBlockCallbackState->dstAddr,  gpBlockCallbackState->dstEndpoint, result);
     return;
  }
  //Update the transfer info
  mOtaClienSession.currentOffset += gpBlockCallbackState->blockSize;
  fileOffset = Native2OTA32(mOtaClienSession.currentOffset);
 (void)ZCL_SetAttribute(mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint, clusterId, gZclAttrOTA_FileOffsetId_c, gZclClientAttr_c, &fileOffset);

    
  if(mOtaClienSession.currentOffset < mOtaClienSession.fileLength)
  {
    //More data to be received - send back a block request
    (void)ZCL_OTAClusterClient_NextBlockRequest(gpBlockCallbackState->dstAddr, gpBlockCallbackState->dstEndpoint);
  }
  else 
  {
     uint8_t upgradeStatus = gOTAUpgradeStatusDownloadComplete_c;  
    
    /* Save relevant data on the new image */
    downloadedFileVersion = Native2OTA32(mOtaClienSession.downloadingFileVersion);
    /* update downloaded File Version status */
    (void)ZCL_SetAttribute(mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint, clusterId, gZclAttrOTA_DownloadedFileVersionId_c, gZclClientAttr_c, &downloadedFileVersion);
        
    if(mOtaClienSetup[mOtaClienSession.instanceIndex].multipleUpgradeImg != gZclOTARequireMoreImage_c)
      /* All image data received. Issue upgrade end request. */
      (void)ZCL_OTAClusterClient_EndUpgradeRequest(gpBlockCallbackState->dstAddr, gpBlockCallbackState->dstEndpoint, gZclSuccess_c);    
    else
    {
      /* require more data */
      (void)ZCL_OTAClusterClient_EndUpgradeRequest(gpBlockCallbackState->dstAddr, gpBlockCallbackState->dstEndpoint, gZclOTARequireMoreImage_c);  
      
      /* update image upgrade status */
      upgradeStatus = gOTAUpgradeStatusWaitForMore_c;
      
      /* initiate new client session: init client params; */
      mOtaClienSetup[mOtaClienSession.instanceIndex].multipleUpgradeImg = 0x00;
      if(mOtaClienSetup[mOtaClienSession.instanceIndex].imageRequestTimer == gTmrInvalidTimerID_c)
          mOtaClienSetup[mOtaClienSession.instanceIndex].imageRequestTimer = ZbTMR_AllocateTimer(); 
      if(mOtaClienSetup[mOtaClienSession.instanceIndex].imageRequestTimer != gTmrInvalidTimerID_c)
      {
    	  /* wait 2 second and start new ota upgrade image process */
    	  ZbTMR_StartSecondTimer(mOtaClienSetup[mOtaClienSession.instanceIndex].imageRequestTimer, 2, OTAClusterClientNextImageReqTimerCallback);
      }
    }
    
    /* update upgrade status */
    (void)ZCL_SetAttribute(mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint, clusterId, gZclAttrOTA_ImageUpgradeStatusId_c, gZclClientAttr_c, &upgradeStatus);
 
#if gEndDevCapability_d || gComboDeviceCapability_d 
    /* stop the OTA FastPoll Mode */
    if(gOtaClientRetransmissionBlockTimer == gTmrInvalidTimerID_c)
    	gOtaClientRetransmissionBlockTimer = ZbTMR_AllocateTimer(); 
    ZbTMR_StartSecondTimer(gOtaClientRetransmissionBlockTimer, 1, OTAClient_UpdatePollRateCallBack);
#else    
    ZbTMR_FreeTimer(gOtaClientRetransmissionBlockTimer); 
    gOtaClientRetransmissionBlockTimer = gTmrInvalidTimerID_c;
#endif       
  }
 // MSG_Free(gpBlockCallbackState);             
 }

static void OTAClientRetransmitLastBlockTmrCallback(uint8_t tmr) 
{
  zbClusterId_t aClusterId={gaZclClusterOTA_c}; 
  uint8_t iEEEAddress[8];
	
   (void)tmr;
   mOtaClienSession.retransmissionCounter++;
   if(mOtaClienSession.retransmissionCounter == gOtaMaxRetransmisionCounter_c)
   {
    	 (void)OTAClusterClientAbortSession();
    	 return;
   }
    if(ZCL_GetAttribute(mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint, aClusterId, gZclAttrOTA_UpgradeServerId_c, gZclClientAttr_c, iEEEAddress, NULL)==gZbSuccess_c)  
    {
    	(void)ZCL_OTAClusterClient_RetransmitBlockRequest(iEEEAddress, mOtaClienSetup[mOtaClienSession.instanceIndex].serverEndpoint); 
    }
}


/******************************************************************************
* ZCL_OTAClusterClient_RetransmitBlockRequest
*
* retransmit last block Request
******************************************************************************/  
static zbStatus_t ZCL_OTAClusterClient_RetransmitBlockRequest 
(
  zbIeeeAddr_t  ieeeAddr,
  zbEndPoint_t  endPoint    
)
{ 
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode64Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode64Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif   
  zclOTABlockRequest_t  blockRequest;
  zbClusterId_t clusterOtaId = {gaZclClusterOTA_c}; 
  uint8_t clientEndpoint = mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint;

  blockRequest.fieldControl = 0x00;
  if(gZclOTABlockRequest_BlockRequestDelayPresent_c == 0x02)
  {
    blockRequest.fieldControl |= gZclOTABlockRequest_BlockRequestDelayPresent_c;
    blockRequest.blockRequestDelay = Native2OTA16(mOtaClienSession.blockRequestDelay);
  }

  (void)ZCL_GetAttribute(clientEndpoint, clusterOtaId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &blockRequest.manufacturerCode, NULL);
  (void)ZCL_GetAttribute(clientEndpoint, clusterOtaId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &blockRequest.imageType, NULL);

  blockRequest.fileVersion = Native2OTA32(mOtaClienSession.downloadingFileVersion);
  blockRequest.fileOffset = Native2OTA32(mOtaClienSession.currentOffset);
  blockRequest.maxDataSize = mOtaClienSetup[mOtaClienSession.instanceIndex].maxDataSize;
  
  /* Create the destination address */
  Copy8Bytes(addrInfo.dstAddr.aIeeeAddr, ieeeAddr);
  addrInfo.dstEndPoint = endPoint;
  addrInfo.srcEndPoint = clientEndpoint;  
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  return ZCL_OTABlockRequestCmd(addrInfo, blockRequest);
  
}

/******************************************************************************
ZCL_StartClientNextImageTransfer - used only if it is discovered a valid OTA Server
*****************************************************************************/
zbStatus_t OTA_OtaStartClientNextImageTransfer 
(
    zclStartClientNextImageTransfer_t* startClientNextImageTransfer
)
{
  zbStatus_t    status = gZbSuccess_c;
  zbIeeeAddr_t  upgradeServerId; 
  zbClusterId_t clusterId = {gaZclClusterOTA_c};          
  
  if(mOtaClienSession.sessionStarted)
    return gZbFailed_c;
           
  /* required EndRequest Status*/
  mOtaClienSetup[mOtaClienSession.instanceIndex].multipleUpgradeImg = startClientNextImageTransfer->multipleUpgradeImage;
  
  (void)ZCL_GetAttribute(mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint, clusterId, gZclAttrOTA_UpgradeServerId_c, gZclClientAttr_c, upgradeServerId, NULL);
  
  /* is valid upgradeServerId attribute? */
  if(Cmp8BytesToFs(upgradeServerId))
  {
      zclInitiateOtaProcess_t initServerDiscovery = {0, 0};
      initServerDiscovery.isServer = FALSE;
      initServerDiscovery.clientEndpoint = mOtaClienSetup[mOtaClienSession.instanceIndex].clientEndpoint;
      (void)OTA_InitiateOtaClusterProcess(&initServerDiscovery);
      return gZbFailed_c;
  }
 
  /* send a Query Next Image Request */ 
  status = ZCL_OTAClusterClient_QueryNextImageRequest(upgradeServerId, mOtaClienSetup[mOtaClienSession.instanceIndex].serverEndpoint, mOtaClienSession.instanceIndex);  
 
  return status;
}

static void OTAClusterClientNextImageReqTimerCallback(uint8_t tmr) 
{
  zbClusterId_t aClusterId={gaZclClusterOTA_c}; 
  uint8_t iEEEAddress[8];
  /* get instance index */
  uint8_t instanceIndex = OTAClusterClientGetIndexFromOtaSetupTable(gZclCluster_InvalidDataIndex_d, tmr);
 
  (void)tmr;
  
  if(instanceIndex != gZclCluster_InvalidDataIndex_d)
  {
    if(ZCL_GetAttribute(mOtaClienSetup[instanceIndex].clientEndpoint, aClusterId, gZclAttrOTA_UpgradeServerId_c, gZclClientAttr_c, iEEEAddress, NULL) == gZbSuccess_c)
    {
        (void)ZCL_OTAClusterClient_QueryNextImageRequest(iEEEAddress, mOtaClienSetup[instanceIndex].serverEndpoint, instanceIndex);  	
    }
    ZbTMR_StartMinuteTimer(mOtaClienSetup[instanceIndex].imageRequestTimer, gOtaMinTimeforNextImageRequest_c, OTAClusterClientNextImageReqTimerCallback);

  }
}


/******************************************************************************
* ZCL_OTAClusterClient_QueryNextImageRequest
*
* Sends back a next image request (may be as a result of Image Notify)
******************************************************************************/  
static zbStatus_t ZCL_OTAClusterClient_QueryNextImageRequest 
(
  zbIeeeAddr_t  ieeeAddr,
  zbEndPoint_t  endPoint,
  uint8_t       instanceOtaId
)
{
  zclOTANextImageRequest_t nextImageRequest;     
  zbClusterId_t clusterOtaId = {gaZclClusterOTA_c}; 
  uint8_t clientEndpoint = mOtaClienSetup[instanceOtaId].clientEndpoint;
  
#ifdef SmartEnergyApplication_d  
  afAddrInfo_t addrInfo = {gZbAddrMode64Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c | gApsTxOptionSecEnabled_c, 1};
#else  
  afAddrInfo_t addrInfo = {gZbAddrMode64Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
#endif   
  uint8_t len = sizeof(zclOTANextImageRequest_t);  
  
  nextImageRequest.fieldControl = gZclOTANextImageRequest_HwVersionPresent_c;
  
  (void)ZCL_GetAttribute(clientEndpoint, clusterOtaId, gZclAttrOTA_ManufacturerId_c, gZclClientAttr_c, &nextImageRequest.manufacturerCode, NULL);
  (void)ZCL_GetAttribute(clientEndpoint, clusterOtaId, gZclAttrOTA_ImageTypeId_c, gZclClientAttr_c, &nextImageRequest.imageType, NULL);
  (void)ZCL_GetAttribute(clientEndpoint, clusterOtaId, gZclAttrOTA_CurrentFileVersionId_c, gZclClientAttr_c, &nextImageRequest.fileVersion, NULL);

  nextImageRequest.hardwareVersion = Native2OTA16(mOtaClienSetup[mOtaClienSession.instanceIndex].hardwareVersion);
  
  
  if(!(nextImageRequest.fieldControl & gZclOTANextImageRequest_HwVersionPresent_c)) 
    len -=sizeof(uint16_t); /* remove hardware version from packet */ 
  
  /* Create the destination address */
  FLib_MemCpy(addrInfo.dstAddr.aIeeeAddr, ieeeAddr, sizeof(zbIeeeAddr_t));
  addrInfo.dstEndPoint = endPoint;
  addrInfo.srcEndPoint = clientEndpoint; 
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  return ZCL_OTAImageRequestCmd(addrInfo, nextImageRequest);
}


/******************************************************************************
* OTA Cluster client: Server discovery Procedure
*
******************************************************************************/ 
/*!
 * @fn 		static void OtaClient_ServerDiscoveryMatchDescReqTmrCallback(uint8_t tmr) 
 *
 * @brief	Sends over the air an MatchDescReq command
 *
 */
static void OtaClient_ServerDiscoveryMatchDescReqTmrCallback(uint8_t tmr) 
{
  zbNwkAddr_t  aDestAddress = {0xFD, 0xFF};

  uint8_t instanceIndex = OTAClusterClientGetIndexFromOtaSetupTable(gZclCluster_InvalidDataIndex_d, tmr);
  if(instanceIndex != gZclCluster_InvalidDataIndex_d)
  {
    ASL_MatchDescriptor_req(NULL,(uint8_t *)aDestAddress, (zbSimpleDescriptor_t*)&otaCluster_simpleDescriptor); 
    if(mOtaClienSession.sessionDiscoveryIndex == gNoOfOtaClusterInstances_d)
    {
      uint8_t i=0; 
      /* update flags for all instancess */
      for(i=0; i< gNoOfOtaClusterInstances_d; i++)
        mOtaClienSetup[i].flags |= gOtaExpectedMatchDescRsp_c;
    }
    else
      mOtaClienSetup[instanceIndex].flags |= gOtaExpectedMatchDescRsp_c;
    
    mOtaClienSession.sessionDiscoveryIndex = 0x00;
    ZbTMR_FreeTimer(mOtaClienSetup[instanceIndex].imageRequestTimer);
    mOtaClienSetup[instanceIndex].imageRequestTimer = gTmrInvalidTimerID_c;
  }
  (void)tmr; 
}

/*!
 * @fn 		bool_t OtaClient_ServerDiscoveryProcessMatchDesc(zbNwkAddr_t  aDestAddress, uint8_t endPoint)
 *
 * @brief	Process Match Descriptor Response
 *
 */
bool_t OtaClient_ServerDiscoveryProcessMatchDesc(zbNwkAddr_t  aDestAddress, uint8_t endPoint)
{  
  uint8_t i = 0;
  bool_t status = FALSE;
  
  for(i = 0; i<gNoOfOtaClusterInstances_d; i++)
  {
    if((mOtaClienSetup[i].flags & gOtaExpectedMatchDescRsp_c) && 
       (mOtaClienSetup[i].clientEndpoint != gZclCluster_InvalidDataIndex_d))
    {      
        /* clear OTA flag */
        mOtaClienSetup[i].flags &= (~gOtaExpectedMatchDescRsp_c);
        /*Send IEEE_Addr_Req*/
        mOtaClienSetup[i].serverEndpoint = endPoint;
        mOtaClienSetup[i].flags |= gOtaExpectedIeeeAddrRsp_c;     
        status = TRUE;
    }
  }
  if(status == TRUE)
     ASL_IEEE_addr_req(NULL, aDestAddress, aDestAddress, 0x00, 0x00);
  
  return status;
}
/*!
 * @fn 		bool_t OtaClient_ServerDiscoveryProcessIeeeAddrRsp(zbIeeeAddr_t  aIeeeAddr)
 *
 * @brief	Process Match Descriptor Response
 *
 */
bool_t OtaClient_ServerDiscoveryProcessIeeeAddrRsp(zbIeeeAddr_t  aIeeeAddr)
{
  uint8_t i = 0;
  bool_t status = FALSE;
  
  for(i=0; i<gNoOfOtaClusterInstances_d; i++)
  {
    if((mOtaClienSetup[i].flags & gOtaExpectedIeeeAddrRsp_c) && (mOtaClienSetup[i].clientEndpoint != gZclCluster_InvalidDataIndex_d))
    {
      zbClusterId_t clusterId = {gaZclClusterOTA_c};
      (void)ZCL_SetAttribute(mOtaClienSetup[i].clientEndpoint, clusterId, gZclAttrOTA_UpgradeServerId_c, gZclClientAttr_c, aIeeeAddr);
      /* clear flag */
      mOtaClienSetup[i].flags&=(~gOtaExpectedIeeeAddrRsp_c);
    
      if(mOtaClienSetup[i].imageRequestTimer == gTmrInvalidTimerID_c)
      {
        mOtaClienSetup[i].imageRequestTimer = ZbTMR_AllocateTimer(); 
        if(mOtaClienSetup[i].imageRequestTimer == gTmrInvalidTimerID_c)
          return FALSE;
      }
      ZbTMR_StartSecondTimer(mOtaClienSetup[i].imageRequestTimer, 3 + i, OTAClusterClientNextImageReqTimerCallback);       
      status = TRUE;
    }
  }
  return status;
}

/*!
 * @fn 		bool_t OtaClient_StartServerDiscoveryProcess(zbIeeeAddr_t  aIeeeAddr)
 *
 * @brief	start server discovery process
 *
 */

zbStatus_t OtaClient_StartServerDiscoveryProcess 
(
    uint8_t instanceIndex
)
{     
  uint8_t endpoint;
  uint8_t localInstanceIndex;
  uint8_t i;
  
  if(instanceIndex <= gNoOfOtaClusterInstances_d)
  {
    mOtaClienSession.sessionDiscoveryIndex = instanceIndex;
    if(instanceIndex == gNoOfOtaClusterInstances_d)
    {
      endpoint = mOtaClienSetup[0].clientEndpoint;
      localInstanceIndex = 0;
    }
    else
    {
      endpoint = mOtaClienSetup[instanceIndex].clientEndpoint;
      localInstanceIndex = instanceIndex;
    }
  
    if( endpoint == gZclCluster_InvalidDataIndex_d)
      return gZclFailure_c;
  
    otaCluster_simpleDescriptor.endPoint = endpoint;     
    
    #if gInstantiableStackEnabled_d 
    for(i=0; i< EndPointConfigData(gNum_EndPoints); ++i) 
    #else
    for(i=0; i< gNum_EndPoints_c; ++i)
    #endif
    {
      if(EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint) == endpoint) 
        Copy2Bytes(&otaCluster_simpleDescriptor.aAppProfId, EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->aAppProfId));     
    }  
  
    /*wait 1 second to send Match_Desc_req*/
    if(mOtaClienSetup[localInstanceIndex].imageRequestTimer == gTmrInvalidTimerID_c)
    {
      mOtaClienSetup[localInstanceIndex].imageRequestTimer = ZbTMR_AllocateTimer();	
      if(mOtaClienSetup[localInstanceIndex].imageRequestTimer == gTmrInvalidTimerID_c)
        return gZclFailure_c;
    }
  
    ZbTMR_StartSecondTimer(mOtaClienSetup[localInstanceIndex].imageRequestTimer, 1, OtaClient_ServerDiscoveryMatchDescReqTmrCallback);

    return gZclSuccess_c;
  }
  return gZclFailure_c;
}
#if gZclEnableOTAProgressReport_d  
/*!
 * @fn 		uint8_t OtaClient_GetProgressReport(void)
 *
 * @brief	return  0 = startProgress, 1 = 33%, 2 = 66%, 3 = 100% 
 *
 */
uint8_t OtaClient_GetProgressReport(void)
{
  return mOtaClienSession.progressReport;
}
#endif
#endif //gZclEnableOTAClient_d

/*!
 * @fn 		zbStatus_t OTA_InitiateOtaClusterProcess(zclInitiateOtaProcess_t* initiateOtaProcess)
 *
 * @brief	Start an ServerDiscovery for specific instance(client side) or init server params
 *
 */
zbStatus_t OTA_InitiateOtaClusterProcess 
(
    zclInitiateOtaProcess_t* initiateOtaProcess
)
{
#if gZclEnableOTAClient_d && gNum_EndPoints_c     
  uint8_t instanceIndex = OTAClusterClientGetIndexFromOtaSetupTable(initiateOtaProcess->clientEndpoint, gTmrInvalidTimerID_c);
  if(instanceIndex != gZclCluster_InvalidDataIndex_d)  
    (void)OtaClient_StartServerDiscoveryProcess(instanceIndex);
#endif

#if gZclEnableOTAServer_d    
  { 
    /* select the instance based on current BeeAppDataInit(appEndPoint)  */
    uint8_t instanceId = OTAServerGetIndexFromOtaSetupTable(BeeAppDataInit(appEndPoint), gTmrInvalidTimerID_c);
    if(instanceId != gZclCluster_InvalidDataIndex_d)
    { 
      mOtaServerCmdParams[instanceId].querryJitter = initiateOtaProcess->querryJitter;
      mOtaServerCmdParams[instanceId].currentTime = OTA2Native32(initiateOtaProcess->currentTime); 
      mOtaServerCmdParams[instanceId].upgradeRequestTime = OTA2Native32(initiateOtaProcess->upgradeRequestTime);
      mOtaServerCmdParams[instanceId].upgradeTime = OTA2Native32(initiateOtaProcess->upgradeTime);
      mOtaServerCmdParams[instanceId].blockRequestDelay = OTA2Native16(initiateOtaProcess->blockRequestDelay);
    }
  }
#endif
 return gZbSuccess_c;
}


/******************************************************************************
* OTA Cluster: Security functions
*
******************************************************************************/ 
#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d || gZclOtaServerImgIntegrityCodeValidation_d)	
/*!
 * @fn 		void aesHashNextBlock(uint8_t* input, uint8_t *keyInit, uint8_t* output)
 *
 * @brief	This function computes the AES-128 of the <tt>input</tt> parameter using 
 *               the Key <tt>keyInit</tt>, and stores the result in <tt>output</tt>.
 *
 */
void aesHashNextBlock(uint8_t* input, uint8_t *keyInit, uint8_t* output)
{
  uint8_t i=0;
#ifdef __IAR_SYSTEMS_ICC__  
#ifdef PROCESSOR_KINETIS
  uint8_t tempIn[AES_MMO_SIZE],tempKey[AES_MMO_SIZE], tempOut[AES_MMO_SIZE]; 
  uint8_t init_vector[AES_MMO_SIZE];
  
  for(i=0;i<AES_MMO_SIZE; i++)
    init_vector[i] = 0;
  FLib_MemCpy(tempIn, input, AES_MMO_SIZE);
  FLib_MemCpy(tempKey, keyInit, AES_MMO_SIZE);
  FLib_MemCpy(tempOut, output, AES_MMO_SIZE);
#else  
  
  uint32_t tempIn[AES_MMO_SIZE/4],tempKey[AES_MMO_SIZE/4], tempOut[AES_MMO_SIZE/4];
  for(i=0;i<AES_MMO_SIZE/4;i++){
    FLib_MemCpyReverseOrder(&tempIn[i], &input[i*4], AES_MMO_SIZE/4);
    FLib_MemCpyReverseOrder(&tempKey[i], &keyInit[i*4], AES_MMO_SIZE/4);
    FLib_MemCpyReverseOrder(&tempOut[i], &output[i*4], AES_MMO_SIZE/4);
  }
#endif
  
#ifdef PROCESSOR_KINETIS
  encryptAES(tempKey, tempIn, tempOut, AES_MMO_SIZE, init_vector);
  FLib_MemCpy(output, tempOut, AES_MMO_SIZE);
#else  
  (void)Asm_CbcMacEncryptDecrypt(tempKey, NULL, tempIn, tempOut);
  for(i=0;i<AES_MMO_SIZE/4;i++)
    FLib_MemCpyReverseOrder(&output[i*4], &tempOut[i], AES_MMO_SIZE/4);
#endif
#else /*! __IAR_SYSTEMS_ICC__   */
  uint8_t tempIn[AES_MMO_SIZE],tempKey[AES_MMO_SIZE], tempOut[AES_MMO_SIZE]; 
  
  FLib_MemCpy(tempIn, input, AES_MMO_SIZE);
  FLib_MemCpy(tempKey, keyInit, AES_MMO_SIZE);
  FLib_MemCpy(tempOut, output, AES_MMO_SIZE); 
  
  (void)Asm_CbcMacEncryptDecrypt(tempKey, NULL, tempIn, tempOut);
  FLib_MemCpy(output, tempOut, AES_MMO_SIZE);
#endif /*__IAR_SYSTEMS_ICC__  */
  
  
  for(i=0; i<AES_MMO_SIZE; i++)
    output[i] ^=input[i];
}

#ifdef PROCESSOR_KINETIS
/*!
 * @fn 		static void encryptAES(uint8_t* key,  uint8_t* input_data, uint8_t* output_data, uint8_t data_length, uint8_t* init_vector)
 *
 * @brief	This function computes the AES CBC(Cipher Block Chaining) of the <tt>input_data</tt> parameter using 
 *              the Key <tt>key</tt> and the  <tt>init_vector</tt>, and stores the result in <tt>output_data</tt>.
 *
 */
static void encryptAES(uint8_t* key,  uint8_t* input_data, uint8_t* output_data, uint8_t data_length, uint8_t* init_vector)
{	
    uint8_t i;
    uint8_t blocks;
    uint8_t rounds;
    uint8_t temp_block[AES_MMO_SIZE];
    uint8_t temp_iv[AES_MMO_SIZE];

    uint8_t key_expansion[gOtaAes128KeyExpansion_c*4];
    
    /*validate data length*/
    if( data_length % AES_MMO_SIZE )
       return ;/*wrong length*/
       
    /*expand AES key*/
    mmcau_aes_set_key(key, AES_MMO_SIZE*8, key_expansion);
       
    /*get number of blocks*/
    blocks = data_length/AES_MMO_SIZE;
    
    /*copy init vector to temp storage*/
    FLib_MemCpy((void*)temp_iv,(void*)init_vector,AES_MMO_SIZE);
    
    do
    {
       /*copy to temp storage*/
       FLib_MemCpy((void*)temp_block,(void*)input_data,AES_MMO_SIZE);
       /*xor for CBC*/
       for (i = 0; i < AES_MMO_SIZE; i++)
           temp_block[i] ^= temp_iv[i];
            
       mmcau_aes_encrypt(temp_block, key_expansion, AES128_ROUNDS, output_data);
       FLib_MemCpy((void*)temp_iv,(void*)output_data,AES_MMO_SIZE);
       input_data += AES_MMO_SIZE;
       output_data += AES_MMO_SIZE;  
    }while(--blocks);

}
#endif

/*!
 * @fn 		void OTAClusterAesMMO_hash(uint8_t *blockToProcess, uint8_t length, bool_t lastBlock, uint32_t totalLength, uint8_t *hashValue)
 *
 * @brief	This function computes the AES MMO hash for OTA Application including also 
 *              the signature tag, signer IEEE for image and stores the result in <tt>output</tt>.
 *
 */
void OTAClusterAesMMO_hash(uint8_t *blockToProcess, uint8_t length, bool_t lastBlock, uint32_t totalLength, uint8_t *hashValue)
{
  uint8_t temp[AES_MMO_SIZE];
  uint8_t moreDataLength = length;
  
  for (;AES_MMO_SIZE <= moreDataLength; 
        blockToProcess += AES_MMO_SIZE, moreDataLength -= AES_MMO_SIZE)
      aesHashNextBlock(blockToProcess, hashValue, hashValue);
  
  if(lastBlock == TRUE){
      FLib_MemSet(&temp[0], 0x00, AES_MMO_SIZE);
      FLib_MemCpy(&temp[0], &blockToProcess[0], moreDataLength);
      temp[moreDataLength] = 0x80;
    
      if (AES_MMO_SIZE - moreDataLength < 3) {
        aesHashNextBlock(temp, hashValue, hashValue);
        FLib_MemSet(&temp[0], 0x00, AES_MMO_SIZE);
      }
      temp[AES_MMO_SIZE - 2] = (uint8_t)(totalLength >> 5);
      temp[AES_MMO_SIZE - 1] = (uint8_t)(totalLength << 3);
      aesHashNextBlock(temp, hashValue, hashValue); 
  }
  
}
#endif /* (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d)|| gZclOtaServerImgIntegrityCodeValidation_d*/


#if gZclEnableOTAClient_d &&  gZclEnableOTAClientECCLibrary_d
extern const uint8_t CertAuthPubKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
extern const uint8_t CertAuthIssuerID[8];
/*!
 * @fn 		zbStatus_t OtaSignatureVerification(uint8_t *signerIEEE,IdentifyCert_t certificate, uint8_t* signature)
 *
 * @brief	OTA signature Verification
 *
 */
zbStatus_t OtaSignatureVerification(uint8_t *signerIEEE,IdentifyCert_t certificate, uint8_t* signature)
{
   uint8_t devicePublicKey_rec[gZclCmdKeyEstab_CompressedPubKeySize_c];
   uint8_t signerIEEE_reverse[8];
   zbIeeeAddr_t signersIEEEList = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
   uint8_t i=0;
   
   for(i=0;i<8;i++)
      signerIEEE_reverse[i] = signerIEEE[8-i-1];
   /*first Step - determine if the signer of the image is an authorized signer*/
   if(Cmp8Bytes(&signersIEEEList[0], &signerIEEE_reverse[0])==FALSE)
     return gZbFailed_c;
   /*Verify the signer IEEE address within the subject field of the ECDSA Certificate */
   if(Cmp8Bytes(&certificate.Subject[0], &signerIEEE_reverse[0])==FALSE)
     return gZbFailed_c;
   /*Test the issuer field of the ECDSA Certificate with a specific list*/
   if(Cmp8Bytes(&certificate.Issuer[0], (uint8_t*)&CertAuthIssuerID[0])==FALSE)
     return gZbFailed_c;
   /*reconstruct public key using certificate and specific CertAuthPubKey*/
   (void)ZSE_ECQVReconstructPublicKey((uint8_t *)&certificate,(uint8_t *)CertAuthPubKey, devicePublicKey_rec,aesMmoHash, NULL, 0);
   /*ECDSA Verification*/
   if(ZSE_ECDSAVerify(devicePublicKey_rec, mOtaClienSession.msgDigest,  &signature[0], &signature[SECT163K1_SIGNATURE_ELEMENT_LENGTH],NULL,0)!=gZbSuccess_c)
     return gZbFailed_c;
   /*if all ok return gZbSuccess_c*/
   return gZbSuccess_c;
}
#endif