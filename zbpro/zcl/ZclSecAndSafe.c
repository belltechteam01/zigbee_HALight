/*! @file 	  ZclSecAndSafe.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for ZCL Security and Safety domain.
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
 *  [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
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
#include "ApsMgmtInterface.h"
#include "ZclFoundation.h"
#include "zcl.h"
#include "ZclSecAndSafe.h"
#include "HaProfile.h"


/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
static void IASZone_TestModeCallback(uint8_t tmrId);
static void IASACE_PanelStatusCallback(uint8_t tmrId);
static zbStatus_t IASZone_SetDeviceMode(afAddrInfo_t addrInfo, bool_t testMode, 
                                        uint8_t duration, uint8_t transactionId);
static zbStatus_t IASACE_BypassHandler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t IASACE_GetBypassZoneListHandler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t IASACE_GetZoneStatusHandler(zbApsdeDataIndication_t *pIndication);
static zbStatus_t IASACE_GetPanelStatusHandler(zbApsdeDataIndication_t *pIndication);
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

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
uint16_t gIasZoneTypeTable[gIASApp_ZoneTypeId_d] = 
{
  gZclZoneType_StandardCIE_c, gZclZoneType_MotionSensor_c, gZclZoneType_ContactSwitch_c,
  gZclZoneType_FireSensor_c, gZclZoneType_WaterSensor_c, gZclZoneType_GasSensor_c,
  gZclZoneType_PersonalEmergencyDevice_c, gZclZoneType_VibrationMovement_c,
  gZclZoneType_RemoteControl_c, gZclZoneType_KeyFob_c, gZclZoneType_Keypad_c,
  gZclZoneType_StandardWarning_c, gZclZoneType_GlassBreakSensor_c,
  gZclZoneType_CarbonMonoxideSensor_c
};

zbTmrTimerID_t gIasZoneTmrId = gTmrInvalidTimerID_c;
zbTmrTimerID_t gIasPanelStatusTmr = gTmrInvalidTimerID_c;
zbTmrTimerID_t gIasWarningDeviceClientTmr = gTmrInvalidTimerID_c;

zclCmdIASWD_StartWarning_t gIasStartWarningData;
zclCmdIASWD_WarningInf_t gIasWarningInf;
zclCmdIASWD_Squawk_t gIasSquawkInf;
uint8_t gCIEApp_CurrentArmMode = SquawkMode_SystemArmed;

uint8_t gZclIasAcePanelStatus = gZclPanelStatus_Disarmed_c;
uint8_t gZclIasAcePanelSecondsRemaining = 0x00;
/******************************
  IAS Zone Cluster 
  See ZCL Specification Section 8.2
*******************************/
/* IAS Zone Cluster Attribute Definitions */
const zclAttrDef_t gaZclIASZoneClusterAttrDef[] = {
  /*Attributes of the Zone Information attribute set */
  { gZclAttrIdZoneInformationZoneState_c,  gZclDataTypeEnum8_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneState)},
  { gZclAttrIdZoneInformationZoneType_c,   gZclDataTypeEnum16_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneType)},
  { gZclAttrIdZoneInformationZoneStatus_c, gZclDataTypeBitmap16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneStatus)},
  /*Attributes of the Zone Settings attribute set */
  { gZclAttrIdZoneSettingsIASCIEAddress_c, gZclDataTypeIeeeAddr_c,   gZclAttrFlagsInRAM_c, sizeof(IEEEaddress_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, IASCIEAddress)},
  { gZclAttrIdZoneSettingsZoneId_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneId)}
#if gIASZoneEnableSensitivityLevels_d  
  ,{ gZclAttrIdNoOfZoneSensitivityLevels_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, noZoneSensLevel)}
  ,{ gZclAttrIdCurrentZoneSensitivityLevel_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, curentZoneSensLevel)}
#endif
};

const zclAttrSet_t gaZclIASZoneClusterAttrSet[] = {
  {gZclAttrSetIASZone_c, (void *)&gaZclIASZoneClusterAttrDef, NumberOfElements(gaZclIASZoneClusterAttrDef)}
};

const zclAttrSetList_t gZclIASZoneClusterAttrSetList = {
  NumberOfElements(gaZclIASZoneClusterAttrSet),
  gaZclIASZoneClusterAttrSet
};


const zclCmd_t gaZclIASZoneClusterCmdReceivedDef[]={
  // commands received   
  gZclCmdRxIASZone_ZoneEnrollResponse_c,
  gZclCmdRxIASZone_InitiateNormalOperationMode_c,
  gZclCmdRxIASZone_InitiateTestMode_c
};

const zclCmd_t gaZclIASZoneClusterCmdGeneratedDef[]={
  // commands generated 
  gZclCmdTxIASZone_ZoneStatusChange_c,
  gZclCmdTxIASZone_ZoneEnrollRequest_c
};

const zclCommandsDefList_t gZclIASZoneClusterCommandsDefList =
{
   NumberOfElements(gaZclIASZoneClusterCmdReceivedDef), gaZclIASZoneClusterCmdReceivedDef,
   NumberOfElements(gaZclIASZoneClusterCmdGeneratedDef), gaZclIASZoneClusterCmdGeneratedDef
};


/******************************
  IAS Warning Device Cluster 
  See ZCL Specification Section 8.4
*******************************/
/* IAS Warning Device Cluster Attribute Definitions */
const zclAttrDef_t gaZclIASWDClusterAttrDef[] = {
  { gZclAttrIASWDIdMaxDuration_c, gZclDataTypeUint16_c,  gZclAttrFlagsInRAM_c, sizeof(uint16_t), (void *) MbrOfs(zclIASWDAttrsRAM_t, maxDuration)},
  };

const zclAttrSet_t gaZclIASWDClusterAttrSet[] = {
  {gZclAttrIASWDSet_c, (void *)&gaZclIASWDClusterAttrDef, NumberOfElements(gaZclIASWDClusterAttrDef)}
};

const zclAttrSetList_t gZclIASWDClusterAttrSetList = {
  NumberOfElements(gaZclIASWDClusterAttrSet),
  gaZclIASWDClusterAttrSet
};


const zclCmd_t gaZclIASWDClusterCmdReceivedDef[]={
  // commands received   
  gZclCmdRxIASWD_StartWarning_c,
  gZclCmdRxIASWD_Squawk_c
};


const zclCommandsDefList_t gZclIASWDClusterCommandsDefList =
{
   NumberOfElements(gaZclIASWDClusterCmdReceivedDef), gaZclIASWDClusterCmdReceivedDef,
   0, NULL
};

/******************************
  IAS ACE Cluster 
  See ZCL Specification Section 8.3
*******************************/

const zclCmd_t gaZclIASACEClusterCmdReceivedDef[]={
  // commands received   
  gZclCmdRxIASACE_Arm_c,
  gZclCmdRxIASACE_Bypass_c,
  gZclCmdRxIASACE_Emergency_c,
  gZclCmdRxIASACE_Fire_c,
  gZclCmdRxIASACE_Panic_c,
  gZclCmdRxIASACE_GetZoneIDMap_c,
  gZclCmdRxIASACE_GetZoneInformation_c,
  gZclCmdRxIASACE_GetPanelStatus_c,
  gZclCmdRxIASACE_GetBypassedZoneList_c,
  gZclCmdRxIASACE_GetZoneStatus_c
};

const zclCmd_t gaZclIASACEClusterCmdGeneratedDef[]={
  // commands generated 
  gZclCmdTxIASACE_ArmRsp_c,
  gZclCmdTxIASACE_GetZoneIDMApRsp_c,
  gZclCmdTxIASACE_GetZoneInfRsp_c,
  gZclCmdTxIASACE_ZoneStatusChanged_c,
  gZclCmdTxIASACE_PanelStatusChanged_c,
  gZclCmdTxIASACE_GetPanelStatusRsp_c,
  gZclCmdTxIASACE_SetBypassedZoneList_c,
  gZclCmdTxIASACE_BypassResponse_c,
  gZclCmdTxIASACE_GetZoneStatusRsp_c
};


const zclCommandsDefList_t gZclIASACEClusterCommandsDefList =
{
   NumberOfElements(gaZclIASACEClusterCmdReceivedDef), gaZclIASACEClusterCmdReceivedDef,
   NumberOfElements(gaZclIASACEClusterCmdGeneratedDef), gaZclIASACEClusterCmdGeneratedDef
};

/******************************
  IAS Zone Cluster 
  See ZCL Specification Section 8.2
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_IASZoneClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS Zone Cluster Client. 
 *
 */
zbStatus_t ZCL_IASZoneClusterClient
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    uint8_t event = 0, i = 0, j = 0;
    uint8_t *pIEEE, aExtAddr[8];
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;   
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;    
    
    /* handle the command */
    switch( pFrame->command)
    {
        case gZclCmdTxIASZone_ZoneStatusChange_c:
          {
           zclCmdIASZone_ZoneStatusChange_t changeNotif;
           FLib_MemCpy(&changeNotif ,(pFrame + 1), sizeof(zclCmdIASZone_ZoneStatusChange_t));   
           pIEEE = APS_GetIeeeAddress((uint8_t*)&pIndication->aSrcAddr, aExtAddr);
           /* zoneId field represent the index in the CIE's zone table */
           if(FLib_MemCmp(&gIASZoneTable[changeNotif.ZoneId].ZoneAddress, pIEEE, 8) == TRUE)
           {
               /* update zone status information */
              gIASZoneTable[changeNotif.ZoneId].ZoneStatus = changeNotif.ZoneStatus;
              event = gZclUI_ChangeNotification;
           }
           BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
           return status;
          }
        case gZclCmdTxIASZone_ZoneEnrollRequest_c:  
          {    
           zclCmdIASZone_ZoneEnrollRequest_t cmdPayload;
           zclIASZone_ZoneEnrollResponse_t *pReq; 
           FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASZone_ZoneEnrollRequest_t));
           pReq = MSG_Alloc(sizeof(zclIASZone_ZoneEnrollResponse_t)); 
           pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, pIndication->aSrcAddr); 
           pReq->addrInfo.dstEndPoint = pIndication->srcEndPoint;
           pReq->addrInfo.srcEndPoint = pIndication->dstEndPoint;
           pReq->addrInfo.txOptions = 0;
           pReq->addrInfo.radiusCounter = afDefaultRadius_c;
           pIEEE = APS_GetIeeeAddress((uint8_t*)&pIndication->aSrcAddr, aExtAddr);
           /* verify if the IEEEAddress exist already in table */
           for(i=0; i <=gIndexInIASZoneTable; i++)
           {
             if(FLib_MemCmp(&gIASZoneTable[i].ZoneAddress, pIEEE, 8) == TRUE)
             {
                pReq->cmdFrame.ZoneID = gIASZoneTable[i].ZoneID;
                pReq->cmdFrame.EnrollResponseCode =  gEnrollResponseCode_Succes_c;
                event = gZclUI_EnrollSuccesfull_c; 
                break;
             }
             else
             {
                if(i == gIndexInIASZoneTable)
                {
                  pReq->cmdFrame.ZoneID = gIndexInIASZoneTable;
                  /* verify the index in the zone table */
                  if(gIndexInIASZoneTable < gIASApp_MaxSupportedZones_d)
                  {
                      /*verify the zone_type*/
                      for(j=0; j< gIASApp_ZoneTypeId_d; j++)
                      
                        if(gIasZoneTypeTable[j] == cmdPayload.ZoneType)
                        {
                            pReq->cmdFrame.EnrollResponseCode =  gEnrollResponseCode_Succes_c;
                            /*store the device in the gZoneTable*/
                            gIASZoneTable[gIndexInIASZoneTable].ZoneID = gIndexInIASZoneTable;
                            gIASZoneTable[gIndexInIASZoneTable].ZoneType = cmdPayload.ZoneType;  
                            Copy8Bytes((uint8_t *)&gIASZoneTable[gIndexInIASZoneTable].ZoneAddress, aExtAddr);
                            gIASZoneTable[gIndexInIASZoneTable].Endpoint = pIndication->srcEndPoint;
                            if(cmdPayload.ZoneType != gZclZoneType_StandardCIE_c)
                              gIASZoneTable[gIndexInIASZoneTable].BypassStatus = gBypass_ZoneNotBypassed;
                            else
                              gIASZoneTable[gIndexInIASZoneTable].BypassStatus = gBypass_NotAllowed;
                            /*update index for table zone*/
                            gIndexInIASZoneTable++;
                            event = gZclUI_EnrollSuccesfull_c; 
                            break;
                        }
                        else
                        {
                            if(j == gIASApp_ZoneTypeId_d - 1)
                            {
                                pReq->cmdFrame.EnrollResponseCode =  gEnrollResponseCode_NoEnrollPermit_c;
                                event = gZclUI_EnrollFailed_c; 
                            }
                        }
                    
                  }            
                  else
                  {
                      pReq->cmdFrame.EnrollResponseCode = gEnrollResponseCode_TooManyZones_c; 
                      event = gZclUI_EnrollFailed_c; 
                  }
                  break;
              }
            }
           }
           status = IASZone_ZoneEnrollResponse(pReq);       
           BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
           MSG_Free(pReq);       
           return status;
          }
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*!
 * @fn 		zbStatus_t ZCL_IASZoneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS Zone Cluster Server. 
 *
 */
zbStatus_t ZCL_IASZoneClusterServer
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    uint8_t event = gZclUI_NoEvent_c;
    zbClusterId_t aClusterId;
    afAddrInfo_t addrInfo;
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    Copy2Bytes(aClusterId, pIndication->aClusterId);
    
    /* Create the destination address */
    AF_PrepareForReply(&addrInfo, pIndication); 
    
    switch( pFrame->command)
    {
        case gZclCmdRxIASZone_ZoneEnrollResponse_c:  
        {
          zclCmdIASZone_ZoneEnrollResponse_t cmdPayload;  
          uint8_t state;
          FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASZone_ZoneEnrollResponse_t)); 
          if(cmdPayload.EnrollResponseCode == gZclSuccess_c)
          {
            event = gZclUI_EnrollSuccesfull_c;
            state = gZclZoneState_Enrolled_c;
            (void)ZCL_SetAttribute(pIndication->dstEndPoint, aClusterId, gZclAttrZoneSettingsZoneId_c, gZclServerAttr_c, &cmdPayload.ZoneID);
          }
          else 
          {
             event = gZclUI_EnrollFailed_c;
             state = gZclZoneState_NotEnrolled_c;    
          }
          (void)ZCL_SetAttribute(pIndication->dstEndPoint, aClusterId, gZclAttrZoneInformationZoneState_c, gZclServerAttr_c, &state);
          BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
          return (pFrame->frameControl & gZclFrameControl_DisableDefaultRsp)?gZclSuccess_c:gZclSuccessDefaultRsp_c;        
          
        }
        case gZclCmdRxIASZone_InitiateNormalOperationMode_c:
        { 
          return IASZone_SetDeviceMode(addrInfo, FALSE, 0, pFrame->transactionId);
        }
        case gZclCmdRxIASZone_InitiateTestMode_c:
        {
          zclCmdIASZone_InitTestMode_t *pCmdPayload;       
          uint8_t noOfZoneSensLevels = 0x00;    
#if gIASZoneEnableSensitivityLevels_d            
          /* get number of zoneSensitivity levels */
          if(ZCL_GetAttribute(pIndication->dstEndPoint, pIndication->aClusterId, gZclAttrNoOfZoneSensitivityLevels_c,  gZclServerAttr_c, &noOfZoneSensLevels, NULL) != gZbSuccess_c)
            noOfZoneSensLevels = 0x00;
#endif
          pCmdPayload = (zclCmdIASZone_InitTestMode_t *)(pFrame+1);
          /* verify sensitivity level */
          if(pCmdPayload->currentZoneSensLevel > noOfZoneSensLevels)
            return gZclInvalidValue_c;
#if gIASZoneEnableSensitivityLevels_d 
          (void)ZCL_SetAttribute(pIndication->dstEndPoint, aClusterId, gZclAttrCurrentZoneSensitivityLevel_c, gZclServerAttr_c, &pCmdPayload->currentZoneSensLevel);
#endif          
          return IASZone_SetDeviceMode(addrInfo, TRUE, pCmdPayload->testModeDuration, pFrame->transactionId);
        }
        default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*!
 * @fn 		zbStatus_t IASZone_ZoneStatusChange(zclIASZone_ZoneStatusChange_t *pReq) 
 *
 * @brief	Sends over-the-air a ZoneStatusChange command from the IAS Zone Cluster Server. 
 *
 */
zbStatus_t IASZone_ZoneStatusChange
(
    zclIASZone_ZoneStatusChange_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c);	
    return ZCL_SendServerReqSeqPassed(gZclCmdTxIASZone_ZoneStatusChange_c, sizeof(zclCmdIASZone_ZoneStatusChange_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendStatusChangeNotification(zbEndPoint_t SrcEndpoint, uint16_t zoneStatusChangeNotif) 
 *
 * @brief	Helper function to create and send ZoneStatusChange frame over the air
 *
 */
void ZCL_SendStatusChangeNotification(zbEndPoint_t SrcEndpoint, uint16_t zoneStatusChangeNotif) 
{
    zclIASZone_ZoneStatusChange_t *pReq;
    zbClusterId_t aClusterId={gaZclClusterIASZone_c}; 
    uint8_t zoneId;
    pReq = MSG_Alloc(sizeof(zclIASZone_ZoneStatusChange_t)); 
     
    if(pReq) 
    {
        pReq->addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.ZoneStatus = zoneStatusChangeNotif;
        pReq->cmdFrame.ExtendedStatus = 0x00;
        (void)ZCL_SetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneInformationZoneStatus_c, gZclServerAttr_c, &zoneStatusChangeNotif);
        (void)ZCL_GetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneSettingsZoneId_c,  gZclServerAttr_c, &zoneId, NULL);
        pReq->cmdFrame.ZoneId = zoneId;
        pReq->cmdFrame.Delay = (zoneStatusChangeNotif & Mask_IASZone_SupervisionReports)?12:0; /* 12 = 3 seconds - shoud check the gIntervalReportStatus*/
        (void)IASZone_ZoneStatusChange(pReq);
        MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASZone_ZoneEnrollRequest(zclIASZone_ZoneEnrollRequest_t *pReq) 
 *
 * @brief	Sends over-the-air a ZoneEnrollRequest command from the IAS Zone Cluster Server. 
 *
 */
zbStatus_t IASZone_ZoneEnrollRequest
(
    zclIASZone_ZoneEnrollRequest_t *pReq
)
{ 
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c); 
    return ZCL_SendServerReqSeqPassed(gZclCmdTxIASZone_ZoneEnrollRequest_c,sizeof(zclCmdIASZone_ZoneEnrollRequest_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendZoneEnrollRequest(uint8_t endpoint) 
 *
 * @brief	Helper function to create and send(using binding table inf.) enrollRequest frame over the air
 *
 */
void ZCL_SendZoneEnrollRequest(uint8_t endpoint) 
{
    zclIASZone_ZoneEnrollRequest_t *pReq;
    uint16_t zoneType;
    zbClusterId_t aClusterId={gaZclClusterIASZone_c}; 
   
    pReq = MSG_Alloc(sizeof(zclIASZone_ZoneEnrollRequest_t)); 
     
    if(pReq) 
    {
        pReq->addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
        pReq->addrInfo.srcEndPoint = endpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        (void)ZCL_GetAttribute(endpoint, aClusterId, gZclAttrZoneInformationZoneType_c , gZclServerAttr_c, &zoneType, NULL);
        pReq->cmdFrame.ZoneType = zoneType;
        pReq->cmdFrame.ManufacturerCode = gZclIasZoneManufacturerCode_c;
        (void) IASZone_ZoneEnrollRequest(pReq);
        MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASZone_ZoneEnrollResponse(zclIASZone_ZoneEnrollResponse_t *pReq) 
 *
 * @brief	Sends over-the-air a ZoneEnrollResponse command from the IAS Zone Cluster Client. 
 *
 */
zbStatus_t IASZone_ZoneEnrollResponse
(
    zclIASZone_ZoneEnrollResponse_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c);	
    return ZCL_SendClientRspSeqPassed(gZclCmdRxIASZone_ZoneEnrollResponse_c,sizeof(zclCmdIASZone_ZoneEnrollResponse_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASZone_InitNormalOpModeRequest(zclIASZone_InitNormalOpMode_t *pReq) 
 *
 * @brief	Sends over-the-air a InitNormalOperationMode command from the IAS Zone Cluster Client. 
 *
 */
zbStatus_t IASZone_InitNormalOpModeRequest
(
    zclIASZone_InitNormalOpMode_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c);	
    return ZCL_SendClientRspSeqPassed(gZclCmdRxIASZone_InitiateNormalOperationMode_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASZone_InitNormalOpModeRequest(zclIASZone_InitTestMode_t *pReq) 
 *
 * @brief	Sends over-the-air a InitTestMode command from the IAS Zone Cluster Client. 
 *
 */
zbStatus_t IASZone_InitTestModeRequest
(
    zclIASZone_InitTestMode_t *pReq
)  
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c);	
    return ZCL_SendClientRspSeqPassed(gZclCmdRxIASZone_InitiateTestMode_c,sizeof(zclCmdIASZone_InitTestMode_t),(zclGenericReq_t *)pReq);
}
/*!
 * @fn 		static zbStatus_t IASZone_SetDeviceMode(afAddrInfo_t addrInfo, bool_t testMode, uint8_t duration, uint8_t transactionId)
 *
* @brief	Init Device operation mode: test mode or operation mode. 
 *
 */
static zbStatus_t IASZone_SetDeviceMode(afAddrInfo_t addrInfo, bool_t testMode, uint8_t duration, uint8_t transactionId)
{
  zbStatus_t status = gZclSuccess_c;
  zbClusterId_t aClusterIdZone = {gaZclClusterIASZone_c}; 
  uint16_t zoneStatus;
  
  /* stop the timer */
  if(gIasZoneTmrId != gTmrInvalidTimerID_c)
    ZbTMR_StopTimer(gIasZoneTmrId);
  
  /* get test Mode */
  (void)ZCL_GetAttribute(addrInfo.srcEndPoint, aClusterIdZone, gZclAttrZoneInformationZoneStatus_c, gZclServerAttr_c, &zoneStatus, NULL);
  if(((zoneStatus & MASK_IASZone_Test) && (!testMode))||
     ((!(zoneStatus & MASK_IASZone_Test)) && testMode))
  {
      /* set zone status */
      zoneStatus ^= MASK_IASZone_Test;
  }
  
  if(testMode)
  {
    /* start timeout */
    if(gIasZoneTmrId == gTmrInvalidTimerID_c)
    {
      gIasZoneTmrId = ZbTMR_AllocateTimer();
      if(gIasZoneTmrId == gTmrInvalidTimerID_c)
        return gZclNoMem_c;
    }
    ZbTMR_StartSecondTimer(gIasZoneTmrId, duration, IASZone_TestModeCallback);
  }

  /* set zone Status */
  (void)ZCL_SetAttribute(addrInfo.srcEndPoint, aClusterIdZone, gZclAttrZoneInformationZoneStatus_c, gZclServerAttr_c, &zoneStatus);
  /* send the status change notification command */
  ZCL_SendStatusChangeNotification(addrInfo.srcEndPoint, zoneStatus);
  
  return status;
}
/*!
 * @fn 		static void IASZone_TestModeCallback(uint8_t tmrId)
 *
* @brief	callback for the test mode -> next status = operation mode
 *
 */
static void IASZone_TestModeCallback(uint8_t tmrId)
{
  zbClusterId_t aClusterIdZone = {gaZclClusterIASZone_c}; 
  uint8_t endpoint =  ZCL_GetEndPointForSpecificCluster(aClusterIdZone, TRUE, 0, NULL);
  uint16_t zoneStatus;
  
  ZbTMR_StopTimer(gIasZoneTmrId);
  
  /* update zone status attribute */
  (void)ZCL_GetAttribute(endpoint, aClusterIdZone, gZclAttrZoneInformationZoneStatus_c, gZclServerAttr_c, &zoneStatus, NULL);
  if(zoneStatus & MASK_IASZone_Test)
  {
    /* clear test mode flag */
    zoneStatus ^= MASK_IASZone_Test;
    (void)ZCL_SetAttribute(endpoint, aClusterIdZone, gZclAttrZoneInformationZoneStatus_c, gZclServerAttr_c, &zoneStatus);
    /* send the status change notification command */
    ZCL_SendStatusChangeNotification(endpoint, zoneStatus);
  }
}

/*!
 * @fn 		zbStatus_t IASZone_AddEntryInDeviceClientTable(gZclZoneTable_t *pIasZoneEntry)
 *
 * @brief	Add an Entry in IAS zone client Table 
 *
 */
#if gASL_ZclIASZoneClientReq_d
zbStatus_t IASZone_AddEntryInDeviceClientTable(gZclZoneTable_t *pIasZoneEntry)
{
   if(gIndexInIASZoneTable >= gIASApp_MaxSupportedZones_d) 
     return gZclFailure_c;
   gIASZoneTable[gIndexInIASZoneTable].ZoneStatus = pIasZoneEntry->ZoneStatus;
   gIASZoneTable[gIndexInIASZoneTable].ZoneType = pIasZoneEntry->ZoneType;
   gIASZoneTable[gIndexInIASZoneTable].ZoneID = pIasZoneEntry->ZoneID;
   gIASZoneTable[gIndexInIASZoneTable].BypassStatus = pIasZoneEntry->BypassStatus;
   Copy8Bytes((uint8_t *)&gIASZoneTable[gIndexInIASZoneTable].ZoneAddress, (uint8_t *)&pIasZoneEntry->ZoneAddress);
   gIASZoneTable[gIndexInIASZoneTable].Endpoint = pIasZoneEntry->Endpoint;
   gIndexInIASZoneTable++;
   return gZclSuccess_c;
}
#endif

#if gAddValidationFuncPtrToClusterDef_c
/*!
 * @fn 		bool_t  ZCL_IASZoneValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
 *
 * @brief	Validation function for IAS Zone attributes
 *
 */
bool_t  ZCL_IASZoneValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
{  
  zclCmdWriteAttrRecord_t *pRecord = (zclCmdWriteAttrRecord_t*) pData;  
  zclAttrData_t *pAttrData = (zclAttrData_t*) pRecord->aData;
  bool_t status = TRUE;
    
  switch (pRecord->attrId)
  {        
#if gIASZoneEnableSensitivityLevels_d   
      case gZclAttrCurrentZoneSensitivityLevel_c:
      {
        uint8_t noOfZoneSensLevels;
        (void)ZCL_GetAttribute(endPoint, clusterId, gZclAttrNoOfZoneSensitivityLevels_c, gZclServerAttr_c, &noOfZoneSensLevels, NULL);
        status = (pAttrData->data8 > noOfZoneSensLevels)?FALSE:TRUE;
        break;
      }
#endif      
    default:
      {
        status = TRUE;
        break;
      }
  }
  
 /* to avoid ompiler warnings */
 (void) endPoint;
 (void) clusterId;

  return status;
} 
#endif /* gAddValidationFuncPtrToClusterDef_c */


/******************************
  IAS ACE Cluster 
  See ZCL Specification Section 8.3
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_IASACEClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS ACE Cluster Client. 
 *
 */
zbStatus_t ZCL_IASACEClusterClient
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;
    zbStatus_t status = gZclSuccessDefaultRsp_c;    
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
	status = gZclSuccess_c;    
    
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
       case gZclCmdTxIASACE_ArmRsp_c:
         {
          zclCmdIASACE_ArmRsp_t cmdPayload;
          FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASACE_ArmRsp_t)); 
          if(cmdPayload.ArmNotification <= gArmNotif_AllZonesArmed_c) /* gArmNotif_AllZoneDisarm_c, gArmNotif_DayHomeZoneArmed_c, gArmNotif_NightSleepZoneArmed_c, gArmNotif_AllZonesArmed_c*/        
            return status;
          else
            return gZbFailed_c;
         }
      case gZclCmdTxIASACE_GetPanelStatusRsp_c:  
        return status;
        
        
       case gZclCmdTxIASACE_GetZoneIDMApRsp_c:      
       case gZclCmdTxIASACE_GetZoneInfRsp_c:
         return status;
       case gZclCmdTxIASACE_ZoneStatusChanged_c: 
       case gZclCmdTxIASACE_PanelStatusChanged_c:
         return status;
       case gZclCmdTxIASACE_SetBypassedZoneList_c:
       case gZclCmdTxIASACE_BypassResponse_c:
       case gZclCmdTxIASACE_GetZoneStatusRsp_c:
         return status;
       default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*!
 * @fn 		zbStatus_t ZCL_IASACEClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS ACE Cluster Server. 
 *
 */
zbStatus_t ZCL_IASACEClusterServer
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    uint8_t i=0,j=0;
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;    
    /* handle the command */
    switch( pFrame->command)
    {
        case gZclCmdRxIASACE_Arm_c:
        {
           zclCmdIASACE_Arm_t cmdPayload;
           zclIASACE_ArmRsp_t *pReq;
           FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASACE_Arm_t));  /*Command = ArmMode */
           pReq = MSG_Alloc(sizeof(zclIASACE_ArmRsp_t));
           if(!pReq)
            return gZclNoMem_c; 
           pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr,pIndication->aSrcAddr); 
           pReq->addrInfo.dstEndPoint = pIndication->srcEndPoint;
           pReq->addrInfo.srcEndPoint = pIndication->dstEndPoint;
           pReq->addrInfo.txOptions = 0;
           pReq->addrInfo.radiusCounter = afDefaultRadius_c;
           /*send arm command to devices (armmode = arm notification)*/                  
           pReq->cmdFrame.ArmNotification = cmdPayload.ArmMode;    
           status = IASACE_ArmRsp(pReq);  
           MSG_Free(pReq); 
           
           gCIEApp_CurrentArmMode = cmdPayload.ArmMode;
           /* search a Warning device to send  squawk command */
           if(gIasWarningDeviceClientTmr == gTmrInvalidTimerID_c)
           {
             gIasWarningDeviceClientTmr = ZbTMR_AllocateTimer(); 
             if(gIasWarningDeviceClientTmr == gTmrInvalidTimerID_c) 
               return gZclNoMem_c;
             ZbTMR_StartTimer(gIasWarningDeviceClientTmr, gTmrSingleShotTimer_c, 100, ZCL_SendSquawkCmdCallback);
           }
           return status;
        }
        case gZclCmdRxIASACE_GetZoneIDMap_c: 
        {
           zclIASACE_GetZoneIDMApRsp_t *pReqZoneMap;
           pReqZoneMap = MSG_Alloc(sizeof(zclIASACE_GetZoneIDMApRsp_t)); 
           pReqZoneMap->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReqZoneMap->addrInfo.dstAddr.aNwkAddr,pIndication->aSrcAddr); 
           pReqZoneMap->addrInfo.dstEndPoint = pIndication->srcEndPoint;
           pReqZoneMap->addrInfo.srcEndPoint = pIndication->dstEndPoint;
           pReqZoneMap->addrInfo.txOptions = 0;
           pReqZoneMap->addrInfo.radiusCounter = afDefaultRadius_c;
           for(i=0;i<16;i++) /* for  zone section */
           {
             pReqZoneMap->cmdFrame.ZoneIDMapSection[i] = 0x0000;
             for(j=0;j<16;j++)/* for zone bit section */
             {
               if((gIndexInIASZoneTable-1) >= (i*16+j))
                  pReqZoneMap->cmdFrame.ZoneIDMapSection[i] |=  (0x0001 << j); 
               else
                  break;  
             }
#if (TRUE == gBigEndian_c)
               pReqZoneMap->cmdFrame.ZoneIDMapSection[i] = Swap2Bytes(pReqZoneMap->cmdFrame.ZoneIDMapSection[i]);
#endif
           }
           status = IASACE_GetZoneIDMapRsp(pReqZoneMap);   
           MSG_Free(pReqZoneMap);       
           return status;      
        }
        case gZclCmdRxIASACE_GetZoneInformation_c: 
          {  
           zclIASACE_GetZoneInfRsp_t *pReqZoneInf;
           zclCmdIASACE_GetZoneInformation_t cmdPayload;
           FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASACE_GetZoneInformation_t));  /*Command = ZoneID */
           pReqZoneInf = AF_MsgAlloc();
           if(pReqZoneInf)
           {
              pReqZoneInf->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
              Copy2Bytes(pReqZoneInf->addrInfo.dstAddr.aNwkAddr,pIndication->aSrcAddr); 
              pReqZoneInf->addrInfo.dstEndPoint = pIndication->srcEndPoint;
              pReqZoneInf->addrInfo.srcEndPoint = pIndication->dstEndPoint;
              pReqZoneInf->addrInfo.txOptions = 0;
              pReqZoneInf->addrInfo.radiusCounter = afDefaultRadius_c;
              /* obtain zone information from table zone */   
              if(cmdPayload.ZoneID < gIndexInIASZoneTable)
              {
        	   pReqZoneInf->cmdFrame.ZoneID       =   gIASZoneTable[cmdPayload.ZoneID].ZoneID;   
        	   pReqZoneInf->cmdFrame.ZoneType     =   gIASZoneTable[cmdPayload.ZoneID].ZoneType; 
        	   Copy8Bytes((uint8_t *)&pReqZoneInf->cmdFrame.IEEEAddress, (uint8_t *)&gIASZoneTable[cmdPayload.ZoneID].ZoneAddress);
              }
              else
              { 
        	   pReqZoneInf->cmdFrame.ZoneID       =   0xFF;   
        	   pReqZoneInf->cmdFrame.ZoneType     =   0xFFFF; 
        	   pReqZoneInf->cmdFrame.IEEEAddress.AddressHigh	=   0xFFFFFFFF;
        	   pReqZoneInf->cmdFrame.IEEEAddress.AddressLow	  	=   0xFFFFFFFF;     	   
              }
              pReqZoneInf->cmdFrame.zoneLabelLength = 0x04;
              for(i=0;i< pReqZoneInf->cmdFrame.zoneLabelLength; i++)
                pReqZoneInf->cmdFrame.zoneLabel[i]=0x00;

              status =  IASACE_GetZoneInformationRsp(pReqZoneInf);       
              MSG_Free(pReqZoneInf);
           }
           else
             status = gZclFailure_c;
           return status;
          }
        case gZclCmdRxIASACE_Bypass_c:
          {
            /* send back a Bypass rsp */
            return IASACE_BypassHandler(pIndication);  
          }  
        case gZclCmdRxIASACE_GetBypassedZoneList_c:
          {
            /* send back an Set Bypass Zone List command */
            return IASACE_GetBypassZoneListHandler(pIndication);  
          }
        case gZclCmdRxIASACE_GetZoneStatus_c:
          {
            /* send back an Get Zone Status Response command */
            return IASACE_GetZoneStatusHandler(pIndication);  
          }
        case gZclCmdRxIASACE_Fire_c:
        case gZclCmdRxIASACE_Emergency_c:
        case gZclCmdRxIASACE_Panic_c: 
         {
           if( pFrame->command == gZclCmdRxIASACE_Fire_c)
            gIasStartWarningData.WarningModeStrobeSirenLevel.WarningMode = WarningMode_Fire;
           else
            gIasStartWarningData.WarningModeStrobeSirenLevel.WarningMode = WarningMode_Emergency; 
           gIasStartWarningData.WarningModeStrobeSirenLevel.Strobe = Strobe_StrobeParallel;
           gIasStartWarningData.WarningModeStrobeSirenLevel.SirenLevel = SirenLevel_Medium;
           gIasStartWarningData.StrobeDutyCycle = 50;
           gIasStartWarningData.StrobeLevel = StrobeLevel_Medium;
           gIasStartWarningData.WarningDuration = 0x1F;
           gIasStartWarningData.WarningDuration = OTA2Native16(gIasStartWarningData.WarningDuration);
           /* search a Warning device to send  StartWarning command */
           if(gIasWarningDeviceClientTmr == gTmrInvalidTimerID_c)
           {
             gIasWarningDeviceClientTmr = ZbTMR_AllocateTimer(); 
             if(gIasWarningDeviceClientTmr == gTmrInvalidTimerID_c) 
               return gZclNoMem_c;
             ZbTMR_StartTimer(gIasWarningDeviceClientTmr, gTmrSingleShotTimer_c, 100, ZCL_SendWarningCmdCallback);
           }
           return status;
         }
        case gZclCmdRxIASACE_GetPanelStatus_c:
          {  
            /* send back a GetPanelStatus rsp */
            return IASACE_GetPanelStatusHandler(pIndication);  
          }
        default:
           return gZclUnsupportedClusterCommand_c;
    }

}

/*!
 * @fn 		zbStatus_t IASACE_Arm(zclIASACE_Arm_t *pReq) 
 *
 * @brief	Sends over-the-air an Arm command  from the IAS ACE Cluster Client. 
 *
 */                       
zbStatus_t IASACE_Arm
(
    zclIASACE_Arm_t *pReq
)
{
    uint8_t payloadLength = sizeof(zclCmdIASACE_Arm_t) + pReq->cmdFrame.ArmDisarmCodeLength;
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Arm_c, payloadLength,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendArmCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t armMode) 
 *
 * @brief	Helper function to create and send an Arm command over the air
 *
 */
void ZCL_SendArmCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t armMode) 
{
    zclIASACE_Arm_t *pReq;
    uint8_t i, zoneId;
    uint8_t armDisarmCode[gIasDefaultLengthArmDisarmCode_d] = {gIasDefaultArmDisarmCode_d};
    zbClusterId_t aClusterId = {gaZclClusterIASZone_c};
    
    pReq = AF_MsgAlloc(); 
  
    if(pReq) 
    {
        pReq->addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.ArmMode = armMode;
        pReq->cmdFrame.ArmDisarmCodeLength = gIasDefaultLengthArmDisarmCode_d;
        for(i=0;i<gIasDefaultLengthArmDisarmCode_d;i++)
          pReq->cmdFrame.ArmDisarmCodeZoneId[i] = armDisarmCode[i];
        /* get zone Id */
        (void)ZCL_GetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneSettingsZoneId_c,  gZclServerAttr_c, &zoneId, NULL);
        pReq->cmdFrame.ArmDisarmCodeZoneId[i] = zoneId;
        (void) IASACE_Arm(pReq);
        MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASACE_ArmRsp(zclIASACE_ArmRsp_t *pReq)
 *
 * @brief	Sends over-the-air an ArmResponse command  from the IAS ACE Cluster Server. 
 *
 */ 
zbStatus_t IASACE_ArmRsp
(
    zclIASACE_ArmRsp_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_ArmRsp_c, sizeof(zclCmdIASACE_ArmRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_Bypass(zclIASACE_Bypass_t *pReq) 
 *
 * @brief	Sends over-the-air an Bypass command  from the IAS ACE Cluster Client. 
 *
 */   
zbStatus_t IASACE_Bypass
(
    zclIASACE_Bypass_t *pReq
)
{
    uint8_t payloadLength = 1 + pReq->cmdFrame.bypassData[0] + 1 + pReq->cmdFrame.bypassData[pReq->cmdFrame.bypassData[0]+1];
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Bypass_c, payloadLength,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendBypassCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t NrOfZones, uint8_t *pZoneId) 
 *
 * @brief	Helper function to create and send an Bypass command over the air
 *
 */
void ZCL_SendBypassCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t NrOfZones, uint8_t *pZoneId) 
{
    zclIASACE_Bypass_t *pReq;
    uint8_t i = 0, j = 0;
    
    pReq = AF_MsgAlloc(); 
    
    if(pReq) 
    {
        uint8_t armDisarmCode[gIasDefaultLengthArmDisarmCode_d] = {gIasDefaultArmDisarmCode_d};
        
        pReq->addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.bypassData[0] = NrOfZones;
        for(i=0; i < NrOfZones; i++)
          pReq->cmdFrame.bypassData[i+1] = pZoneId[i];
        i++;
        pReq->cmdFrame.bypassData[i] = gIasDefaultLengthArmDisarmCode_d;
        i++;
        for(j=0;j<gIasDefaultLengthArmDisarmCode_d; j++)
        {
          pReq->cmdFrame.bypassData[i+j] = armDisarmCode[j];
        }

        (void) IASACE_Bypass(pReq);
         MSG_Free(pReq);
    }
}

/*!
 * @fn 		void ZCL_SendEFPCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t CommandEFP)
 *
 * @brief	Helper function to create and send an Emergency/Fire/Panic command over the air. 
 *
 */  
void ZCL_SendEFPCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t CommandEFP) 
{
    zclIASACE_EFP_t *pReq;
    
    pReq = MSG_Alloc(sizeof(zclIASACE_EFP_t)); 
     
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
        pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    
        (void)ZCL_SendClientReqSeqPassed(CommandEFP,0,(zclGenericReq_t *)pReq);
        MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASACE_Emergency(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an Emergency command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_Emergency
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Emergency_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_Fire(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an Fire command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_Fire
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Fire_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_Panic(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an Panic command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_Panic
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Panic_c,0,(zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t IASACE_GetPanelStatus(zclIASACE_GetPanelStatus_t *pReq) 
 *
* @brief	Sends over-the-air an GetPanelStatus command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_GetPanelStatus
(
    zclIASACE_GetPanelStatus_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_GetPanelStatus_c, 0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_GetBypassedZoneList(zclIASACE_GetBypassedZoneList_t *pReq) 
 *
* @brief	Sends over-the-air an GetBypassedZoneList command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_GetBypassedZoneList
(
    zclIASACE_GetBypassedZoneList_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_GetBypassedZoneList_c, 0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_GetZoneStatus(zclIASACE_GetBypassedZoneList_t *pReq) 
 *
* @brief	Sends over-the-air an GetBypassedZoneList command  from the IAS ACE Cluster Client. 
 *
 */ 
zbStatus_t IASACE_GetZoneStatus
(
    zclIASACE_GetZoneStatus_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_GetZoneStatus_c, sizeof(zclCmdIASACE_GetZoneStatus_t),(zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t IASACE_GetZoneIDMap(zclIASACE_EFP_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneIdMap command  from the IAS ACE Cluster Client. 
 *
 */
zbStatus_t IASACE_GetZoneIDMap
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_GetZoneIDMap_c,0,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_GetZoneIDMapRsp(zclIASACE_GetZoneIDMApRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneIdMapResponse command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_GetZoneIDMapRsp
(
    zclIASACE_GetZoneIDMApRsp_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_GetZoneIDMApRsp_c, sizeof(zclCmdIASACE_GetZoneIDMApRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_GetZoneInformation(zclIASACE_GetZoneInformation_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneInformation command  from the IAS ACE Cluster Client. 
 *
 */
zbStatus_t IASACE_GetZoneInformation
(
    zclIASACE_GetZoneInformation_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_GetZoneInformation_c, sizeof(zclCmdIASACE_GetZoneInformation_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendGetZoneInformationCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t zoneId) 
 *
 * @brief	Helper function to create and send an GetZoneInformation command over the air. 
 *
 */ 
void ZCL_SendGetZoneInformationCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t zoneId) 
{
    zclIASACE_GetZoneInformation_t *pReq;
    
    pReq = MSG_Alloc(sizeof(zclIASACE_GetZoneInformation_t));   
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.ZoneID = zoneId;
             
        (void) IASACE_GetZoneInformation(pReq);
         MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASACE_GetZoneInformationRsp(zclIASACE_GetZoneInfRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneInformationResponse command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_GetZoneInformationRsp
(
    zclIASACE_GetZoneInfRsp_t *pReq
)
{
    uint8_t payloadLength = sizeof(zclCmdIASZone_GetZoneInfRsp_t)-1 + pReq->cmdFrame.zoneLabelLength;
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_GetZoneInfRsp_c, payloadLength,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_GetPanelStatusRsp(zclIASACE_GetPanelStatusRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an GetPanelStatusResponse command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_GetPanelStatusRsp
(
    zclIASACE_GetPanelStatusRsp_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_GetPanelStatusRsp_c, sizeof(zclCmdIASACE_GetPanelStatusRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_BypassRsp(zclIASACE_BypassRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an BypassResponse command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_BypassRsp
(
    zclIASACE_BypassRsp_t *pReq
)
{
    uint8_t payloadLength = 1+ pReq->cmdFrame.noOfZones;
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_BypassResponse_c, payloadLength,(zclGenericReq_t *)pReq);
}

    /*!
 * @fn 		zbStatus_t IASACE_SetBypassedZoneList(zclIASACE_BypassRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an BypassResponse command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_SetBypassedZoneList
(
    zclIASACE_SetBypassedZoneList_t *pReq
)
{
    uint8_t payloadLength = sizeof(zclCmdIASACE_SetBypassedZoneList_t) - 1 + pReq->cmdFrame.noOfZones;
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_SetBypassedZoneList_c, payloadLength,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_ZoneStatusChanged(zclIASACE_ZoneStatusChanged_t *pReq) 
 *
 * @brief	Sends over-the-air an ZoneStatusChanged command  from the IAS ACE Cluster Server. 
 *
 */                       
zbStatus_t IASACE_ZoneStatusChanged
(
    zclIASACE_ZoneStatusChanged_t *pReq
)
{
    uint8_t payloadLength =  sizeof(zclCmdIASACE_ZoneStatusChanged_t) + pReq->cmdFrame.zoneLabelLength -1;
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_ZoneStatusChanged_c, payloadLength,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t IASACE_PanelStatusChanged(zclIASACE_PanelStatusChanged_t *pReq) 
 *
 * @brief	Sends over-the-air an PanelStatusChanged command  from the IAS ACE Cluster Server. 
 *
 */                         
zbStatus_t IASACE_PanelStatusChanged
(
    zclIASACE_PanelStatusChanged_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    gZclIasAcePanelStatus = pReq->cmdFrame.panelStatus;
    
    if(gZclIasAcePanelStatus == gZclPanelStatus_ExitDelay_c || gZclIasAcePanelStatus == gZclPanelStatus_EntryDelay_c)
    {
        if(gIasPanelStatusTmr == gTmrInvalidTimerID_c)
        {
           gIasPanelStatusTmr = ZbTMR_AllocateTimer(); 
           gZclIasAcePanelSecondsRemaining = pReq->cmdFrame.secondsRemaining;
           if(gIasPanelStatusTmr != gTmrInvalidTimerID_c) 
              ZbTMR_StartTimer(gIasPanelStatusTmr, gTmrSingleShotTimer_c | gTmrLowPowerTimer_c, 1000, IASACE_PanelStatusCallback);
        }
    }
    else
    {
      gZclIasAcePanelSecondsRemaining = 0x00;
      if(gIasPanelStatusTmr != gTmrInvalidTimerID_c)
      {
        ZbTMR_FreeTimer(gIasPanelStatusTmr);
        gIasPanelStatusTmr = gTmrInvalidTimerID_c;
      }
          
    }
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_PanelStatusChanged_c, sizeof(zclCmdIASACE_PanelStatusChanged_t),(zclGenericReq_t *)pReq);
}
/*!
 * @fn 		zbStatus_t IASACE_GetZoneStatusRsp(zclIASACE_GetZoneStatusRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an GetZoneStatusRsp command  from the IAS ACE Cluster Server. 
 *
 */
zbStatus_t IASACE_GetZoneStatusRsp
(
    zclIASACE_GetZoneStatusRsp_t *pReq
)
{
    uint8_t payloadLength;
    if(pReq->cmdFrame.noOfZones > 0)
      payloadLength = sizeof(zclCmdIASACE_GetZoneStatusRsp_t) + (pReq->cmdFrame.noOfZones-1)*sizeof(zclGetZoneStatusRspRecord_t);
    else
      payloadLength = sizeof(zclCmdIASACE_GetZoneStatusRsp_t) - sizeof(zclGetZoneStatusRspRecord_t);
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_GetZoneStatusRsp_c, payloadLength,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		static zbStatus_t IASACE_BypassHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	sends a get panse status rsp over the air. 
 *
 */ 
static zbStatus_t IASACE_BypassHandler(zbApsdeDataIndication_t *pIndication)
{
    zbStatus_t status = gZclFailure_c;
    zclIASACE_BypassRsp_t *pCmdRsp;
    uint8_t i, j;
    afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterIASACE_c}, 0, gApsTxOptionNone_c, 1};

    /* check the data payload */
    pCmdRsp = AF_MsgAlloc();
    if(pCmdRsp)
    {
      uint8_t *pBypassReqPayload;
      uint8_t bypassCode[gIasDefaultLengthArmDisarmCode_d+1];
            
      pBypassReqPayload = (void *)(pIndication->pAsdu + sizeof(zclFrame_t));
      FLib_MemCpy(&bypassCode[0], &pBypassReqPayload[pBypassReqPayload[0]+1], gIasDefaultLengthArmDisarmCode_d+1);
        
      /* Create the destination address */
      AF_PrepareForReply(&addrInfo, pIndication); 
      FLib_MemCpy(&pCmdRsp->addrInfo, &addrInfo, sizeof(afAddrInfo_t));
      pCmdRsp->cmdFrame.noOfZones = pBypassReqPayload[0];

      
      for(i=0;i<pBypassReqPayload[0]; i++)
      {
        pCmdRsp->cmdFrame.bypassResult[i] = gBypass_UnknownZoneID;
        for(j=0; j<gIndexInIASZoneTable; j++)
        {
          if(gIASZoneTable[j].ZoneID == pBypassReqPayload[i+1])
          {
            uint8_t armDisarmCode[gIasDefaultLengthArmDisarmCode_d] = {gIasDefaultArmDisarmCode_d};
            if(bypassCode[0] == gIasDefaultLengthArmDisarmCode_d &&
               FLib_MemCmp(&bypassCode[1], &armDisarmCode[0], gIasDefaultLengthArmDisarmCode_d))
            {
              if(gIASZoneTable[j].BypassStatus != gBypass_NotAllowed)
                gIASZoneTable[j].BypassStatus = gBypass_ZoneBypassed;
              pCmdRsp->cmdFrame.bypassResult[i] = gIASZoneTable[j].BypassStatus;
            }
            else
            {
              pCmdRsp->cmdFrame.bypassResult[i] = gBypass_InvalidArmDisarmCode;
            }
            break;
          }
        }
      }
      
      status = IASACE_BypassRsp(pCmdRsp);
      MSG_Free(pCmdRsp);
    }
    return status;
}

/*!
 * @fn 		static zbStatus_t IASACE_BypassHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	sends a get panse status rsp over the air. 
 *
 */ 
static zbStatus_t IASACE_GetBypassZoneListHandler(zbApsdeDataIndication_t *pIndication)
{
    zclIASACE_SetBypassedZoneList_t *pCmdRsp;
    zbStatus_t status = gZclFailure_c;
    uint8_t i;
    afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterIASACE_c}, 0, gApsTxOptionNone_c, 1};

    pCmdRsp = AF_MsgAlloc();
    if(pCmdRsp)
    {
      /* Create the destination address */
      AF_PrepareForReply(&addrInfo, pIndication); 
      FLib_MemCpy(&pCmdRsp->addrInfo, &addrInfo, sizeof(afAddrInfo_t));
      //pCmdRsp->cmdFrame.noOfZones = gIndexInIASZoneTable;
      pCmdRsp->cmdFrame.noOfZones = 0x00;
      pCmdRsp->cmdFrame.zoneId[0] = 0x00;
      for(i=0; i< gIndexInIASZoneTable; i++)
      {
        if(gIASZoneTable[i].BypassStatus == gBypass_ZoneBypassed)
        {
          pCmdRsp->cmdFrame.zoneId[i] = gIASZoneTable[i].ZoneID;
          pCmdRsp->cmdFrame.noOfZones++;
        }
      }
      
      status = IASACE_SetBypassedZoneList(pCmdRsp);
      MSG_Free(pCmdRsp);
    }
    return status;
}


/*!
 * @fn 		static zbStatus_t IASACE_GetZoneStatusHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	sends a get panse status rsp over the air. 
 *
 */ 
static zbStatus_t IASACE_GetZoneStatusHandler(zbApsdeDataIndication_t *pIndication)
{
    zclIASACE_GetZoneStatusRsp_t *pCmdRsp;
    zbStatus_t status = gZclFailure_c;
    uint8_t i;
    afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterIASACE_c}, 0, gApsTxOptionNone_c, 1};

    pCmdRsp = AF_MsgAlloc();
    if(pCmdRsp)
    {
      uint8_t maxAsduLen, iRspLen;
      zclCmdIASACE_GetZoneStatus_t *pGetZoneStatusPayload;
      pGetZoneStatusPayload = ZCL_GetPayload(pIndication);
      
      /* Create the destination address */
      AF_PrepareForReply(&addrInfo, pIndication); 
      FLib_MemCpy(&pCmdRsp->addrInfo, &addrInfo, sizeof(afAddrInfo_t));
      
      /* Get the maximum ASDU length for the response */
      maxAsduLen = AF_MaxPayloadLen(&addrInfo);
      iRspLen = sizeof(zclFrame_t) + sizeof(zclCmdIASACE_GetZoneStatusRsp_t)-sizeof(zclGetZoneStatusRspRecord_t);
      
      pCmdRsp->cmdFrame.noOfZones = 0;
      pCmdRsp->cmdFrame.zoneStatusComplete = TRUE;
      
      if(pGetZoneStatusPayload->startingZoneId < gIndexInIASZoneTable)
      {
        for(i=pGetZoneStatusPayload->startingZoneId; i< gIndexInIASZoneTable; i++)
        {
          if((!pGetZoneStatusPayload->zoneStatusMaskFlag)||
             (gIASZoneTable[i].ZoneStatus & pGetZoneStatusPayload->zoneStatusMask))
             {
                pCmdRsp->cmdFrame.zoneRecord[i - pGetZoneStatusPayload->startingZoneId].zoneID = gIASZoneTable[i].ZoneID;
                pCmdRsp->cmdFrame.zoneRecord[i - pGetZoneStatusPayload->startingZoneId].status = gIASZoneTable[i].ZoneStatus;
                pCmdRsp->cmdFrame.noOfZones++;
                
                iRspLen += sizeof(zclCmdDiscoverAttrRspRecord_t);
                if((iRspLen + sizeof(zclGetZoneStatusRspRecord_t) > maxAsduLen - sizeof(zclFrame_t))&&
                   (i<gIndexInIASZoneTable))
                {
                  pCmdRsp->cmdFrame.zoneStatusComplete = FALSE;
                  break;
                }
             }
        }
      }

      status = IASACE_GetZoneStatusRsp(pCmdRsp);
      MSG_Free(pCmdRsp);
    }
    return status;
}
/*!
 * @fn 		static zbStatus_t IASACE_GetPanelStatusHandler(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	sends a get panse status rsp over the air. 
 *
 */ 
static zbStatus_t IASACE_GetPanelStatusHandler(zbApsdeDataIndication_t *pIndication)
{
    zclIASACE_GetPanelStatusRsp_t cmdRsp;
    afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterIASACE_c}, 0, gApsTxOptionNone_c, 1};

    /* Create the destination address */
    AF_PrepareForReply(&addrInfo, pIndication); 
    FLib_MemCpy(&cmdRsp.addrInfo, &addrInfo, sizeof(afAddrInfo_t));
      
    cmdRsp.cmdFrame.panelStatus = gZclIasAcePanelStatus;
    if(cmdRsp.cmdFrame.panelStatus == gZclPanelStatus_ExitDelay_c || cmdRsp.cmdFrame.panelStatus == gZclPanelStatus_EntryDelay_c  )
    {
        cmdRsp.cmdFrame.secondsRemaining = gZclIasAcePanelSecondsRemaining; 
        
    }
    else
    {
        cmdRsp.cmdFrame.secondsRemaining = 0x00;
    }
   
    /* default values */
    cmdRsp.cmdFrame.alarmStatus = 0x01; /* default Sound */ 
    cmdRsp.cmdFrame.audibleNotification = WarningMode_Stop;
  
    return IASACE_GetPanelStatusRsp(&cmdRsp);
}


/*!
 * @fn 		void ZCL_SendPanelStatusChangedCommand(uint8_t endpoint, uint8_t panelStatus, uint8_t secondsRemaining, uint8_t audibleNotification, uint8_t alarmStatus)
 *
 * @brief	sends a get panse status rsp over the air. 
 *
 */ 
void ZCL_SendPanelStatusChangedCommand(uint8_t endpoint, uint8_t panelStatus, uint8_t secondsRemaining, uint8_t audibleNotification, uint8_t alarmStatus)
{
  zclIASACE_PanelStatusChanged_t *pReq;
   
  pReq = MSG_Alloc(sizeof(zclIASACE_PanelStatusChanged_t)); 
     
  if(pReq) 
  {
    pReq->addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
    pReq->addrInfo.srcEndPoint = endpoint;
    pReq->addrInfo.txOptions = 0;
    pReq->addrInfo.radiusCounter = afDefaultRadius_c;    
    pReq->cmdFrame.panelStatus = panelStatus;
    pReq->cmdFrame.audibleNotification = (panelStatus != gZclPanelStatus_Disarmed_c)?WarningMode_Burglar:WarningMode_Stop;
    if(panelStatus == gZclPanelStatus_ExitDelay_c || panelStatus == gZclPanelStatus_EntryDelay_c  )
    {
       pReq->cmdFrame.secondsRemaining = secondsRemaining; 
    }
    else
    {
       pReq->cmdFrame.secondsRemaining = 0x00;
    }
    pReq->cmdFrame.audibleNotification = audibleNotification;
    pReq->cmdFrame.alarmStatus =  alarmStatus; /* default Sound */ 
 
    (void) IASACE_PanelStatusChanged(pReq);
     MSG_Free(pReq);
   }
} 

/*!
 * @fn 		static IASACE_PanelStatusCallback(uint8_t tmrId)
 *
 * @brief	Update panel status. 
 *
 */ 
static void IASACE_PanelStatusCallback(uint8_t tmrId)
{
  zbClusterId_t aClusterId = {gaZclClusterIASACE_c}; 
  uint8_t endpoint =  ZCL_GetEndPointForSpecificCluster(aClusterId, TRUE, 0, NULL);
  
  gZclIasAcePanelSecondsRemaining--;
   
  if(!gZclIasAcePanelSecondsRemaining)
  {
    if(gZclIasAcePanelStatus == gZclPanelStatus_ExitDelay_c)
      gZclIasAcePanelStatus = gZclPanelStatus_Disarmed_c;
    if(gZclIasAcePanelStatus == gZclPanelStatus_EntryDelay_c)
      gZclIasAcePanelStatus = gZclPanelStatus_ArmedStay_c;
    /* free timer */
    ZbTMR_FreeTimer(gIasPanelStatusTmr);
    gIasPanelStatusTmr = gTmrInvalidTimerID_c;
  }
  else
  {
    ZbTMR_StartTimer(gIasPanelStatusTmr, gTmrSingleShotTimer_c | gTmrLowPowerTimer_c, 1000, IASACE_PanelStatusCallback);
  }
  
  /* send status change notification */
  ZCL_SendPanelStatusChangedCommand(endpoint, gZclIasAcePanelStatus, gZclIasAcePanelSecondsRemaining, 0x01, 0x00);
}


/******************************
  IAS Warning Device Cluster 
  See ZCL Specification Section 8.4
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_IASWDClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS WarningDevice Cluster Client. 
 *
 */
zbStatus_t ZCL_IASWDClusterClient
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
       default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*!
 * @fn 		zbStatus_t ZCL_IASWDClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the IAS WarningDevice Cluster Server. 
 *
 */
zbStatus_t ZCL_IASWDClusterServer
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    uint8_t event;
    zbClusterId_t aClusterId;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
  	   status = gZclSuccess_c;
    
    Copy2Bytes(aClusterId, pIndication->aClusterId);
    
    /* handle the command */
    switch(pFrame->command)
    {
        case gZclCmdRxIASWD_StartWarning_c:
          {
            zclCmdIASWD_StartWarning_t  cmdPayload;
            uint16_t maxDurationAttr;
            (void)ZCL_GetAttribute(pIndication->dstEndPoint, aClusterId, gZclAttrIASWDMaxDuration_c, gZclServerAttr_c, &maxDurationAttr, NULL);
            FLib_MemCpy(&cmdPayload ,(pFrame + 1), sizeof(zclCmdIASWD_StartWarning_t));
            cmdPayload.WarningDuration = OTA2Native16( cmdPayload.WarningDuration);
            maxDurationAttr = OTA2Native16(maxDurationAttr);
            gIasWarningInf.WarningDuration = ((cmdPayload.WarningDuration > maxDurationAttr)?maxDurationAttr:cmdPayload.WarningDuration);  
            gIasWarningInf.StrobeDutyCycle = ((cmdPayload.StrobeDutyCycle > 10)?cmdPayload.StrobeDutyCycle:10);
            gIasWarningInf.WarningModeStrobeSirenLevel = cmdPayload.WarningModeStrobeSirenLevel;
            if(cmdPayload.WarningModeStrobeSirenLevel.WarningMode != 0)
            {
              if(cmdPayload.WarningModeStrobeSirenLevel.Strobe != 0)      
                event = gZclUI_StartWarningStrobe_c;
              else
              {
                event = gZclUI_StartWarning_c;
                gIasWarningInf.StrobeDutyCycle = 0;
              }
            }
            else
            {
              if(cmdPayload.WarningModeStrobeSirenLevel.Strobe != 0)
                event = gZclUI_StartStrobe_c;
              else
              {
                gIasWarningInf.WarningDuration = 0;
                event = gZclUI_NoEvent_c;
              }
            }
            BeeAppUpdateDevice(0, event, 0, 0, NULL);
            return status;  
          }       
        case gZclCmdRxIASWD_Squawk_c:
            FLib_MemCpy(&gIasSquawkInf ,(pFrame + 1), sizeof(zclCmdIASWD_Squawk_t));
            event = (gIasWarningInf.WarningDuration > 0)?gZclUI_NoEvent_c:gZclUI_Squawk_c;
            BeeAppUpdateDevice(0, event, 0, 0, NULL);
            return status;
            
        default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*!
 * @fn 		zbStatus_t IASWD_StartWarning(zclIASWD_StartWarning_t *pReq) 
 *
 * @brief	Sends over-the-air a StartWarning command from the IAS WD Cluster Client. 
 *
 */                       
zbStatus_t IASWD_StartWarning
(
   zclIASWD_StartWarning_t *pReq
)
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASWD_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASWD_StartWarning_c,sizeof(zclCmdIASWD_StartWarning_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendStartWarning(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_StartWarning_t startWarningPayload) 
 *
 * @brief	Helper function to create and send StartWarning command over the air
 *
 */
void ZCL_SendStartWarning(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_StartWarning_t startWarningPayload) 
{
    zclIASWD_StartWarning_t *pReq;
    pReq = MSG_Alloc(sizeof(zclIASWD_StartWarning_t)); 
  
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.WarningModeStrobeSirenLevel = startWarningPayload.WarningModeStrobeSirenLevel;
        pReq->cmdFrame.WarningDuration = startWarningPayload.WarningDuration;
        pReq->cmdFrame.StrobeLevel = startWarningPayload.StrobeLevel;
        pReq->cmdFrame.StrobeDutyCycle = startWarningPayload.StrobeDutyCycle;
          
        (void)IASWD_StartWarning(pReq);
        MSG_Free(pReq);
    }
}

/*!
 * @fn 		zbStatus_t IASWD_Squawk(zclIASWD_Squawk_t *pReq) 
 *
 * @brief	Sends over-the-air a Squawk command from the IAS WD Cluster Client. 
 *
 */  
zbStatus_t IASWD_Squawk
(
    zclIASWD_Squawk_t *pReq
)     
{
    pReq->zclTransactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASWD_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASWD_Squawk_c,sizeof(zclCmdIASWD_Squawk_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		void ZCL_SendSquawk(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_Squawk_t squawk)  
 *
 * @brief	Helper function to create and send Squawk command over the air
 *
 */
void ZCL_SendSquawk(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_Squawk_t squawk) 
{
    zclIASWD_Squawk_t *pReq;
    pReq = MSG_Alloc(sizeof(zclIASWD_StartWarning_t)); 
  
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame = squawk; 
       (void)IASWD_Squawk(pReq);
        MSG_Free(pReq);
    }
}


/*!
 * @fn 		void ZCL_SendSquawkCmdCallback(zbTmrTimerID_t tmrId) 
 *
 * @brief	Callback used to send the Squawk command to all Warning devices, according with the zoneTable
 *
 */
void ZCL_SendSquawkCmdCallback(zbTmrTimerID_t tmrId)
{
  static uint8_t currentIndex = 0;
  uint8_t aNwkAddrCpy[2];
  uint8_t i = 0;
  uint8_t pIEEE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00};
  uint8_t srcEndpoint;
  zbClusterId_t clusterId = {gaZclClusterIASWD_c};
  
  (void)tmrId;
  
  /* check zone table */ 
   for(i = currentIndex; i<gIndexInIASZoneTable; i++)
   {
    if(gIASZoneTable[i].ZoneType == gZclZoneType_StandardWarning_c)
    {
      zclCmdIASWD_Squawk_t squawk;
      Copy8Bytes(pIEEE, (uint8_t *)&gIASZoneTable[i].ZoneAddress);
      (void) APS_GetNwkAddress(pIEEE, aNwkAddrCpy);
      srcEndpoint = ZCL_GetEndPointForSpecificCluster(clusterId, FALSE, 0, NULL); 
      if(gCIEApp_CurrentArmMode == gArmNotif_AllZoneDisarm_c)
        squawk.SquawkMode = SquawkMode_SystemDisarmed;
      else
        squawk.SquawkMode = SquawkMode_SystemArmed;
      squawk.Strobe = STROBE_PARALLEL;
      squawk.SquawkLevel = SquawkLevel_LowLevel;
      ZCL_SendSquawk( gIASZoneTable[i].Endpoint, srcEndpoint, aNwkAddrCpy, squawk);
      currentIndex = i+1;
      if(currentIndex >= gIndexInIASZoneTable)
      {
        ZbTMR_FreeTimer(gIasWarningDeviceClientTmr);
        gIasWarningDeviceClientTmr = gTmrInvalidTimerID_c;
        currentIndex = 0;
      }
      else
        ZbTMR_StartSecondTimer(tmrId, 0x01, ZCL_SendSquawkCmdCallback);
      return;
     }
   }
   ZbTMR_FreeTimer(gIasWarningDeviceClientTmr);
   gIasWarningDeviceClientTmr = gTmrInvalidTimerID_c;
   currentIndex = 0;   
}


/*!
 * @fn 		void ZCL_SendWarningCmdCallback(zbTmrTimerID_t tmrId) 
 *
 * @brief	Callback used to send the StartWarning command to all Warning devices, according with the zoneTable
 *
 */
void ZCL_SendWarningCmdCallback(zbTmrTimerID_t tmrId)
{
  static uint8_t currentIndex = 0;
  uint8_t aNwkAddrCpy[2];
  uint8_t i = 0;
  uint8_t pIEEE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00};
  uint8_t srcEndpoint;
  zbClusterId_t clusterId = {gaZclClusterIASWD_c};

  /* check zone table */ 
   for(i = currentIndex; i< gIndexInIASZoneTable; i++)
   {
    if(gIASZoneTable[i].ZoneType == gZclZoneType_StandardWarning_c)
    {
      Copy8Bytes(pIEEE, (uint8_t *)&gIASZoneTable[i].ZoneAddress);
      (void) APS_GetNwkAddress(pIEEE, aNwkAddrCpy);
      srcEndpoint = ZCL_GetEndPointForSpecificCluster(clusterId, FALSE, 0, NULL); 
      ZCL_SendStartWarning(gIASZoneTable[i].Endpoint, srcEndpoint,  aNwkAddrCpy,  gIasStartWarningData); 

      currentIndex = i+1;
      if(currentIndex >= gIndexInIASZoneTable )
      {
        ZbTMR_FreeTimer(gIasWarningDeviceClientTmr);
        gIasWarningDeviceClientTmr = gTmrInvalidTimerID_c;
        currentIndex = 0;
      }
      else
        ZbTMR_StartSecondTimer(tmrId, 0x01, ZCL_SendWarningCmdCallback);
      return;
     }
   }
   ZbTMR_FreeTimer(gIasWarningDeviceClientTmr);
   gIasWarningDeviceClientTmr = gTmrInvalidTimerID_c;
   currentIndex = 0;   
}
