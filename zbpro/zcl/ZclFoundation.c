/******************************************************************************
* ZclFoundation.c
*
* This source file contains commands defined in the ZigBee Cluster Library,
* Foundation document.
*
* Copyright (c) 2007, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
* [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf
******************************************************************************/
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "EndPointConfig.h"
#include "TMR_Interface.h"
#include "ZCL.h"
#include "ASL_ZCLInterface.h"
#include "ZdpManager.h"
#include "ZdoApsInterface.h"
#include "SeProfile.h"
#include "zclSE.h"    
#ifdef gZclEnablePartition_d    
#if (gZclEnablePartition_d == TRUE)
#include "ZclPartition.h"
#include "ZclProtocInterf.h"
#endif
#endif
#if gZclEnableApplianceStatistics_d
#include "ZclHomeAutomation.h"
#endif //gZclEnableApplianceStatistics_d
#if gZclEnablePwrProfileClusterServer_d || gZclEnablePwrProfileClusterClient_d
#include "ZclEnergyHome.h"
#endif

#include "beeapp.h"
#include "ZclFoundation.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

//zclAttrDef_t gCurrentAttr;

#if gZclEnablePartition_d
extern bool_t gSendPartitionData;
#endif
/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
uint8_t ZCL_IsFrameType(zclFrameControl_t frameControl);
void ZCL_InterpretResponse(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
bool_t ZCL_IsZclEndPoint(afDeviceDef_t *pDevice);
afClusterDef_t *ZCL_FindCluster(afDeviceDef_t *pDevice, zbClusterId_t aClusterId);
zclAttrDef_t *ZCL_FindAttr(afClusterDef_t *pCluster, zclAttrId_t attrId, uint8_t direction);
bool_t ZCL_IsStrType(zclAttrType_t attrType);
bool_t ZCL_IsDataTypeDiscrete(uint8_t dataType);
void ZCL_GetInvalidValue(uint8_t dataType, uint8_t attrLen, uint8_t *value);

#if gZclEnableLongStringTypes_c      
bool_t ZCL_IsLongStrType(zclAttrType_t attrType);
#endif

afToApsdeMessage_t *ZCL_CreateFragmentsFrame
(
zclCmd_t command,               /* IN: frame command */
zclFrameControl_t frameControl, /* IN: frame control field */
uint8_t *pTransactionId,
uint16_t *pPayloadLen,          /* IN/OUT: length of payload (then adjusted to length of asdu) */
void *pPayload                  /* IN: payload for entire frame*/
);

/*
  Used to generate the ZCL read attribute response. Takes as input a read 
  attribute request.
*/
uint8_t ZCL_BuildReadAttrResponse
  (
  afDeviceDef_t  *pDeviceDef, /* IN: device definition for this endpoint  */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint8_t count,              /* IN: # of responses */
  zclFrame_t *pFrame,         /* IN: read attr request */
  uint8_t maxAsduLen,         /* IN: maximum ASDU length */ 
  zclFrame_t *pFrameRsp       /* IN/OUT: response frame, allocated above, filled in here...*/
  );

/*
  Used to generate the ZCL write attribute response. Takes as input a write 
  attribute request.
*/
uint8_t ZCL_BuildWriteAttrResponse
  (
  afDeviceDef_t  *pDeviceDef, /* IN: device definition for this endpoint  */
  afClusterDef_t *pCluster,   /* IN: cluster definition to write attrs */
  uint8_t iTotalLen,          /* IN: length of input frame */
  zclFrame_t *pFrame,         /* IN: input frame */
  zclFrame_t *pFrameRsp       /* IN/OUT: response to write attribute */
  );

uint8_t ZCL_BuildDiscoverAttrResponse
  (
  afClusterDef_t *pCluster,   /* IN: cluster definition */
  zclFrame_t *pFrame,         /* IN: input frame */
  uint8_t maxAsduLen,         /* IN: maximum ASDU length */ 
  zclFrame_t *pFrameRsp       /* IN/OUT: response to discover attributes */
  );

uint8_t ZCL_BuildDiscoverAttrExtendedResponse
  (
  afAddrInfo_t   *pAddrInfo,  /* IN: address info */
  afClusterDef_t *pCluster,   /* IN: cluster definition */
  zclFrame_t *pFrame,         /* IN: input frame */
  zclFrame_t *pFrameRsp       /* IN/OUT: response to discover attributes */
  );

uint8_t ZCL_BuildDiscoverCommandsResponse
  (
  afAddrInfo_t   *pAddrInfo,  /* IN: address info */
  afClusterDef_t *pCluster,   /* IN: cluster definition */
  zclFrame_t *pFrame,         /* IN: input frame */
  zclFrame_t *pFrameRsp       /* IN/OUT: response to discover attributes */
  );


zbStatus_t ZCL_SendDefaultResponse
  (
    zbApsdeDataIndication_t *pIndication, /* IN: OTA indication from which to get reply addr info */
    zclStatus_t status                    /* IN: status indicated in the default response */
  );

/* is this attribute reportable? */
bool_t ZCL_IsReportableAttr
  (
  afDeviceDef_t  *pDevice,    /* IN: assumes pDevice is non-null */
  afClusterDef_t *pCluster,   /* IN: assume pCluster exists and is non-null */
  zclAttrId_t attrId,         /* IN: any attr ID */
  uint8_t *pIndex             /* OUT: index bit used for  */
  );

/* is this attribute both reporting and has changed enough? Used for change by type */
bool_t ZCL_IsReportingAndChanged
  (
  afDeviceDef_t   *pDeviceDef, 
  afClusterDef_t  *pClusterDef, 
  zclAttrId_t     attrId,
  uint8_t         *pReportIndex  
  );

/*
  ZCL_FindCluster

  Given a cluster ID, find a pointer to the cluster structure.
*/
afClusterDef_t *ZCL_FindCluster
  (
  afDeviceDef_t *pDevice,     /* IN: device definition */
  zbClusterId_t aClusterId    /* IN: cluster ID to find in that device definition */
  );

/******************************************************************************
* ZCL_GetAttributePtr
*
* Determine the pointer to the attribute. Returns NULL if attribute not found
* on this cluster. Optionally return size of attribute i pSize is non-null.
*******************************************************************************/
void *ZCL_GetAttributePtr
    (
    afDeviceDef_t  *pDeviceDef, /* IN: pointer to device definition */
    afClusterDef_t *pCluster,   /* IN: pointer to cluster definition */
    zclAttrId_t attrId,         /* IN: attribute ID (in little endian) */
    uint8_t direction,            /* IN: 0x00 for Server Attributes and 0x08 for Client Attributes*/      
    uint8_t *pAttrLen           /* IN/OUT: return size of attribute (actual not max). */
    );



/*******************************************************************************
* ZCL_BuildConfigureReportingResponse
*
* Build a response to the configure attribute(s) command. Works across clusters and 
* understands multiple instances of an endpoint.
*
* Returns total length of payload
*******************************************************************************/
STATIC uint8_t ZCL_BuildConfigureReportingResponse
  (
  afDeviceDef_t *pDevice,     /* IN: data to be acted on by this cluster */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint8_t asduLen,            /* IN: length of input frame */
  zclFrame_t *pFrame,         /* IN: read attr request */
  zclFrame_t *pFrameRsp       /* IN/OUT: response frame, allocated above, filled in here... */
  );

/*******************************************************************************
* ZCL_ReportingClientSetup
*
* used to update client configure reporting table
*******************************************************************************/
STATIC zbStatus_t ZCL_ReportingClientSetup
  (
   zbClusterId_t aClusterId, 
   zclAttrId_t   attrId,  
   uint16_t  reportTimeout
   );

/*******************************************************************************
* ZCL_BuildReadReportingCfgResponse
*
* Build a response to the read reporting configuration command. Works across clusters and 
* understands multiple instances of an endpoint.
*
* Returns total length of payload
*******************************************************************************/
STATIC uint8_t ZCL_BuildReadReportingCfgResponse
  (
  afDeviceDef_t *pDevice,     /* IN: data to be acted on by this cluster */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint8_t asduLen,            /* IN: length of input frame */
  zclFrame_t *pFrame,         /* IN: read attr request */
  zclFrame_t *pFrameRsp       /* IN/OUT: response frame, allocated above, filled in here... */
  );


bool_t ZCL_CmpUint
  (
  uint8_t *pAttrData,
  uint8_t dataSize
  );


/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

/* default zbApsTxOption */
zbApsTxOption_t const gZclTxOptions = afTxOptionsDefault_c;  /* send them simply */


/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
//uint8_t gZclTransactionId = 0x42;  /* start on something other than 0 */
//fnZclResponseHandler_t gfnZclResponseHandler;

#if gZclEnableReporting_c
//zclReportingClientSetup_t gZclReportingClientSetup[gZClMaxAttrReportingReceivedTable_c];
#endif

/******************************************************************************
*******************************************************************************
* Public  memory declarations
*******************************************************************************
******************************************************************************/

#if(gInstantiableStackEnabled_d == 1)

  zclFoundationGlobals_t* pzclFoundationGlobals = NULL;
  zclFoundationGlobals_t  ZclFoundationGlobals[2];

#else /*(gInstantiableStackEnabled_d == 1)*/

  uint8_t gZclTransactionId = 0x42;/* start on something other than 0 */

  zclAttrDef_t gCurrentAttr;

  /* Used for identify */
  uint8_t gZclIdentifyTimerID;

#if gZclEnableReporting_c
  zclReportingClientSetup_t gZclReportingClientSetup[gZClMaxAttrReportingReceivedTable_c];
#endif

  /* Used for reporting attributes */
  uint8_t gZclReportingTimerID;

  fnZclResponseHandler_t gfnZclResponseHandler;
  
#if gZclEnableReporting_c
  index_t gZclReportDeviceIndex;  /* for state machine, sending a report */
  index_t gZclReportClusterIndex; /*  */
  index_t gAsynchronousClusterIndex;
  bool_t  gfZclReportRestart;
#endif
  
#endif
  
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* ZCL_Reset
*
* Reset the ZCL layer. May be reset at any time.
******************************************************************************/
void ZCL_Reset
  (
  void
  )
{
  
#if gNum_EndPoints_c != 0        
  index_t i;
  afDeviceDef_t *pDevice;
#endif

  /* stop the timer (if running ) */
  ZbTMR_StopTimer(ZbZclFoundationGlobals(gZclIdentifyTimerID));

  /* copy in default ROM data to RAM for each endpoint */
  #if gNum_EndPoints_c != 0      
  #if gInstantiableStackEnabled_d 
  for(i=0; i < EndPointConfigData(gNum_EndPoints); ++i) 
  #else
  for(i=0; i < gNum_EndPoints_c; ++i) 
  #endif  
  {
    pDevice = (afDeviceDef_t *) EndPointConfigData(endPointList[i].pDevice);
    if (pDevice && pDevice->pfnZCL)
      ZCL_ResetDevice(pDevice);
  }
#if gZclEnableReporting_c   
  ZbTMR_StopTimer(ZbZclFoundationGlobals(gZclReportingTimerID));    
#endif  
#endif   
}

/*****************************************************************************
* ZCL_Register
*
* Register a callback for sending responses from ZCL (includes attribute
* reporting). If not registers, attribute reports go nowhere.
* 
*****************************************************************************/
void ZCL_Register
  (
  fnZclResponseHandler_t fnResponseHandler  /* IN: pointer to a response handler */
  )
{
  ZbZclFoundationGlobals(gfnZclResponseHandler) = fnResponseHandler;
}


#if gInterPanCommunicationEnabled_c
zbStatus_t ZCL_InterpretInterPanFrame(zbInterPanDataIndication_t *pIndication)
{
#if gNum_EndPoints_c != 0        
  zbSimpleDescriptor_t *pSimpleDesc;
  afDeviceDef_t *pDevDef;
  afClusterDef_t *pClusterDef;
  uint8_t i;
#endif  
  zbStatus_t status = gZclUnsupportedClusterCommand_c;
  zclFrameControl_t frameControl;
  zclFrame_t *pFrame;
  
    /* also not for ZCL if frame if not at least big enough for a frame */
  if(pIndication->asduLength < sizeof(zclFrame_t))
    return gZclMfgSpecific_c;

  pFrame = (void *)pIndication->pAsdu;
  frameControl = pFrame->frameControl;
  
    /* if manufacturer specific frame, let application  handle it */
  if(ZCL_IsMfgSpecificFrame(frameControl))
    return gZclMfgSpecific_c;
#if gNum_EndPoints_c != 0      
  #if gInstantiableStackEnabled_d 
  for(i=0; i < EndPointConfigData(gNum_EndPoints); ++i) 
  #else
  for(i=0; i < gNum_EndPoints_c; ++i) 
  #endif 
  {
    if(EndPointConfigData(endPointList[i].pEndpointDesc)) 
    {
      zbProfileId_t   zllProfileId = {gZllProfileIdentifier_d};
      pSimpleDesc = EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc);
      if(IsEqual2Bytes(pSimpleDesc->aAppProfId, pIndication->aProfileId) || 
         IsEqual2Bytes(zllProfileId, pIndication->aProfileId) )
      {
        pDevDef = (afDeviceDef_t *)EndPointConfigData(endPointList[i].pDevice);
        /* find the cluster in the cluster list */
        pClusterDef = ZCL_FindCluster(pDevDef, pIndication->aClusterId);
        /* if the destination cluster is not defined on the EP, 
        then the frame is most probably not for ZCL */
        if(pClusterDef)
        {
           bool_t basicDeviceEnabled = 0;
           zbClusterId_t basicClusterId = {gaZclClusterBasic_c}; 
          (void)ZCL_GetAttribute(pSimpleDesc->endPoint,  basicClusterId, gZclAttrBasic_DeviceEnabledId_c, gZclServerAttr_c, &basicDeviceEnabled, NULL);
          
          /* check if cluster handler is available and device is enabled */
          /* filter for price and message cluster */
          if(basicDeviceEnabled)
          {
            if(frameControl & gZclFrameControl_DirectionRsp)
            {
#if gInterPanCommunicationEnabled_c
              if(pfnInterPanClientInd)
                status = pfnInterPanClientInd(pIndication, pDevDef);
#endif

            }
            else
            {
#if gInterPanCommunicationEnabled_c
              if(pfnInterPanServerInd)  
                /* call the cluster # */
                status = pfnInterPanServerInd(pIndication, pDevDef);
#endif


            }   
          } 
        }/* if(pClusterDef) */
      }/* if(IsEqual2Bytes(pSimpleDesc->aAppProfId */ 
    } /*  if(endPointList[i].pEndpointDesc) */
  }/* end for(...) */
#endif  
  return status;
}
#endif



/*****************************************************************************
* ZCL_SendDefaultMfgResponse
*
* Send the default response to manufacturer specific commands.
* 
* Returns
*   gZclUnsupportedMfgGeneralCommand_c  if genereral command
*   gZclUnsupportedMfgClusterCommand_c  if cluster command
*****************************************************************************/
void ZCL_SendDefaultMfgResponse(zbApsdeDataIndication_t *pIndication)
{
  zclFrameControl_t frameControl;
  zbStatus_t status;
  
  frameControl = ((zclFrame_t*)pIndication->pAsdu)->frameControl;
  
  if(ZCL_IsFrameType(frameControl) == gZclFrameControl_FrameTypeGeneral)
    status = gZclUnsupportedMfgGeneralCommand_c;
  else
    status = gZclUnsupportedMfgClusterCommand_c;
  
  (void)ZCL_SendDefaultResponse(pIndication, status);
    
}


/*****************************************************************************
* ZCL_InterpretFrame
* 
* This is the entry point for all ZCL commands. Calls the appropriate function,
* as described by the pDevice. Note: this does NOT free the MSG buffer. It is
* expected that the calling application frees the MSG buffer.
*
* Returns 
*   gZclMfgSpecific_c       if the application must handle the frame (not ZCL 
*                           or ZCL msg specific frame)
*   gZbInvalidEndpoint_c    if no endpoint device information (private profile?)
*   gZclMalformedCommand_c  if malformed HA packet
*   gZbSuccess_c            if handled the frame.
*****************************************************************************/
zbStatus_t ZCL_InterpretFrame
  (
  zbApsdeDataIndication_t *pIndication  /* IN: data indication from application */
  )
{

  afDeviceDef_t *pDevice;
  afClusterDef_t *pClusterDef;
  zclFrameControl_t frameControl;
  zbStatus_t status;
  zclFrame_t *pFrame;

  /* verify dst endpoint has device data, and is for ZCL */
  pDevice = AF_GetEndPointDevice(pIndication->dstEndPoint);
  if(!pDevice || !pDevice->pfnZCL) 
    return gZclMfgSpecific_c; 
  
  /* also not for ZCL if frame if not at least big enough for a frame */
  if(pIndication->asduLength < sizeof(zclFrame_t))
    return gZclMfgSpecific_c;

  pFrame = (void *)pIndication->pAsdu;
  frameControl = pFrame->frameControl;

  /* if manufacturer specific frame, let application  handle it */
  if(ZCL_IsMfgSpecificFrame(frameControl))
    return gZclMfgSpecific_c;


#ifdef SmartEnergyApplication_d
  if (ZCl_SEClusterSecuritycheck(pIndication))
  {
#endif   
    
  if (pFrame->command ==  gZclCmdDefaultRsp_c)
  {
    zclCmdDefaultRsp_t * pDefaultRsp;
    pDefaultRsp = ZCL_GetPayload(pIndication);
    if((uint8_t)(pDefaultRsp->status) == gZclFailure_c)
      frameControl = frameControl|gZclFrameControl_FrameTypeSpecific;
    
#if gZclEnableOTAClient_d    
    if((uint8_t)(pDefaultRsp->status) == 0x95)  /* gZclOTAAbort_c  = 0x95*/
      frameControl = frameControl|gZclFrameControl_FrameTypeSpecific;
#endif 
    
#if gZclEnablePartition_d
    {
     zbClusterId_t clusterIdPartition;
     Set2Bytes(clusterIdPartition, gZclClusterPartition_c);
     if(IsEqual2Bytes(pIndication->aClusterId, clusterIdPartition))
     {
    	 if((uint8_t)(pDefaultRsp->status) != gZclSuccess_c)
   			 gSendPartitionData = FALSE;
    	 else
    		 gSendPartitionData = TRUE;
     } 
    }
#endif    
  }
  
  /* check for foundation (cross-cluster) functions (e.g. read attribute) */
  if(ZCL_IsFrameType(frameControl) == gZclFrameControl_FrameTypeGeneral)
  {
    status = ZCL_InterpretFoundationFrame(pIndication, pDevice);
  }
  else  /* ZCL command frame */ 
  {
    /* find the cluster in the cluster list */
    pClusterDef = ZCL_FindCluster(pDevice, pIndication->aClusterId);
  
    /* if the destination cluster is not defined on the EP, 
       then the frame is most probably not for ZCL */
    if(!pClusterDef)
    {
      return gZclInvalidValue_c;
    }
    /* cluster specific */
    else 
    {
      
       bool_t basicDeviceEnabled = 0;
       zbClusterId_t basicClusterId = {gaZclClusterBasic_c}; 
       
      (void)ZCL_GetAttribute(pIndication->dstEndPoint,  basicClusterId, gZclAttrBasic_DeviceEnabledId_c, gZclServerAttr_c, &basicDeviceEnabled, NULL);

      /* check if cluster handler is available and device is enabled */
      /* identify should work even if device is disabled */
      /* [R2] - 3.2.2.2.13 Device Enable Attribute */
        /* aandreescu warning*/
#ifndef SmartEnergyApplication_d      
      if(IsEqual2BytesInt(pClusterDef->aClusterId, gZclClusterIdentify_c) 
      || basicDeviceEnabled) 
#endif        
      {
        if(frameControl & gZclFrameControl_DirectionRsp)
        {
          if(pClusterDef->pfnClientIndication)
            status = pClusterDef->pfnClientIndication(pIndication, pDevice);
          else
            status = gZclUnsupportedClusterCommand_c;  
        }
        else  
        {  
          if(pClusterDef->pfnServerIndication)  
            /* call the cluster # */
            status = pClusterDef->pfnServerIndication(pIndication, pDevice);
          else
            status = gZclUnsupportedClusterCommand_c;
        }   
      } 
#ifndef SmartEnergyApplication_d        
      /* device disbled or no handler specified */
      else 
        status = gZclUnsupportedClusterCommand_c; 
#endif      
    }
  }
#ifdef SmartEnergyApplication_d
  }
  else
  {
    status = gZclFailure_c;
  }
#endif   

  
  /* try to send a default response if there was a ZCL error */
  /* or if the custom handler does not send a success response by default*/
  if ((status != gZbSuccess_c) && (status != gZclDontFreePacket_c)) {
    if (status == gZclSuccessDefaultRsp_c)
      status = gZclSuccess_c;
    (void)ZCL_SendDefaultResponse(pIndication, status);
  }  
  
  return status;
}





/*****************************************************************************
* ZCL_InterpretResponse
* 
* Got a response. Send to the caller (if registered).
*****************************************************************************/
void ZCL_InterpretResponse
  (
  zbApsdeDataIndication_t *pIndication,   /* IN */
  afDeviceDef_t *pDevice                  /* IN */
  )
{
  /* send response to user if they want it */
  if(ZbZclFoundationGlobals(gfnZclResponseHandler))
    (ZbZclFoundationGlobals(gfnZclResponseHandler))(pIndication, pDevice);

}


/*****************************************************************************
* ZCL_IsFrameType
*
* Return the frame type
*****************************************************************************/
uint8_t ZCL_IsFrameType
  (
  zclFrameControl_t frameControl  /* IN */
  )
{
  return (frameControl & gZclFrameControl_FrameTypeMask);
}

/*****************************************************************************
* ZCL_IsZclEndPoint
*
* Does this Device Definition point to a ZCL endpoint?
*
* Return TRUE if so.
*****************************************************************************/
bool_t ZCL_IsZclEndPoint
  (
  afDeviceDef_t *pDevice    /* IN */
  )
{
  return (pDevice->pfnZCL == (pfnIndication_t)ZCL_InterpretFoundationFrame);
}


/*****************************************************************************
* ZCL_InCommandList
*
* Returns TRUE if this command is in the list.
*****************************************************************************/
bool_t ZCL_InCommandList
  (
  zclCmd_t command,       /* IN */
  zclCmdList_t *pCmdList  /* IN */
  )
{
  if(FLib_MemChr(pCmdList->pCmds, command, pCmdList->count))
    return TRUE;
  return FALSE;
}


/*****************************************************************************
* ZCL_InterpretFoundationFrame
*
* Process a cross-cluster command frame for this endpoint.
*
* Returns 
*   gZbSuccess_                       if worked
*   gZclNoMem_c                       if not enough memory for the request
*   gZclUnsupportedClusterCommand_c   if bad command
*****************************************************************************/
zbStatus_t ZCL_InterpretFoundationFrame
  (
  zbApsdeDataIndication_t *pIndication, /* IN: OTA indication */
  afDeviceDef_t *pDevice                /* IN: device data */
  )
{
  zclCmd_t   command;
  zclFrame_t *pFrame;
  zclFrame_t *pFrameRsp;
  afClusterDef_t *pCluster;
  afToApsdeMessage_t *pMsgRsp;
  afAddrInfo_t addrInfo;
  uint16_t iPayloadLen = 0;
  bool_t fSendResponse = TRUE;
  zbStatus_t status = gZbSuccess_c;
  zclAttrDef_t *pAttrDef = NULL;   /* pointer to attribute definition */
  uint8_t maxAsduLen = 0;      
  bool_t basicDeviceEnabled = 0;
  zbEndPoint_t endpoint;
  zbClusterId_t basicClusterId = {gaZclClusterBasic_c}; 
  
  pFrame = (void *)pIndication->pAsdu;
  zbStatus_t getAttrStatus;
  /* what command is this? */
  command = pFrame->command;
  
  getAttrStatus = ZCL_GetAttribute(pIndication->dstEndPoint, basicClusterId, gZclAttrBasic_DeviceEnabledId_c, gZclServerAttr_c, &basicDeviceEnabled, NULL);
  
  /* The Device Enabled attribute is not available */
  if (getAttrStatus != gZbSuccess_c)
  {
    /* Search other endpoints for the Basic cluster*/
    endpoint = ZCL_GetEndPointForSpecificCluster(basicClusterId, TRUE, 0, NULL);
    getAttrStatus = ZCL_GetAttribute(endpoint, basicClusterId, gZclAttrBasic_DeviceEnabledId_c, gZclServerAttr_c, &basicDeviceEnabled, NULL);
  }
  
  /* if device is disabled; should send/respond only 
      to read/write attr and identiy cluster command */
  /* [R2] - 3.2.2.2.13 Device Enable Attribute */
  if(!IsEqual2BytesInt(pIndication->aClusterId, gZclClusterIdentify_c))
  	  if(!basicDeviceEnabled)
    	  if(!(command == gZclCmdWriteAttr_c || command == gZclCmdReadAttr_c))
      		  return gZclSuccess_c;
  
  /* It is a ZCL Foundation response or report ??? */  
  /* Interpret here the Responses */
  switch(command) 
  {
  case gZclCmdReadAttrRsp_c:
  case gZclCmdWriteAttrRsp_c:
  case gZclCmdCfgReportingRsp_c:
  case gZclCmdReadReportingCfgRsp_c:
  case gZclCmdDefaultRsp_c:
  case gZclCmdDiscoverAttrRsp_c:
  case gZclCmdReportAttr_c:  
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_ZCLResponse_c, 0, NULL, pIndication);
    ZCL_InterpretResponse(pIndication, pDevice);
    status = gZclSuccessDefaultRsp_c;
    return status;
  case gZclCmdDiscoverCmdReceivedRsp_c:
  case gZclCmdDiscoverCmdGeneratedRsp_c:
  case gZclCmdDiscoverAttrExtendedRsp_c:
    /* process response */
    return status;
  default:
    break;
  }
  
  /* find the cluster */
  pCluster = ZCL_FindCluster(pDevice, pIndication->aClusterId);
  if(!pCluster)
  {
    if((command == gZclCmdReadAttr_c)||(command == gZclCmdWriteAttr_c))
      return gZclUnsupportedAttribute_c;
    else
      return gZclUnsupportedClusterCommand_c;
  }
  /* prepare for response in the address info (back to sender) */
  AF_PrepareForReply(&addrInfo, pIndication);
  /* Initialize the radius */
  addrInfo.radiusCounter = gDefaultRadiusCounter;
  
  
  /* allocate a response buffer (large enough for any response) */
  pMsgRsp = AF_MsgAlloc();
  if(!pMsgRsp)
    return gZclNoMem_c;
      
  pFrameRsp = (void *)(((uint8_t *)pMsgRsp) + gAsduOffset_c);
  pFrameRsp->frameControl = gZclFrameControl_FrameTypeGeneral | 
    ( (pFrame->frameControl & gZclFrameControl_DirectionRsp)?0:gZclFrameControl_DirectionRsp ) 
      | gZclFrameControl_DisableDefaultRsp;
  pFrameRsp->transactionId = pFrame->transactionId;

  /* Get the maximum ASDU length for the response */
  maxAsduLen = AF_MaxPayloadLen(&addrInfo);
  
  switch(command)
  {    
  /* read attribute command (1 or more records) */
    case gZclCmdReadAttr_c:
    {
      uint16_t attrCount;        /* number of attributtes in the list */
      
#if !gFragmentationCapability_d
      zclAttrId_t *pAttrId;     /* attribute IDs to read from this cluster */
      zclAttrId_t attrId = 0;       /* attr ID */
      bool_t bAsyncAttr = FALSE;        /* there's an async attribute in the list */
      uint8_t i;
#endif
      
      attrCount = (AF_GetSizeOfIndication(pIndication) - sizeof(zclFrame_t)) / sizeof(zclAttrId_t);

#if !gFragmentationCapability_d
      /* Look for async attributes */
      pAttrId = (void *)(pFrame + 1);
      for (i = 0; (i < attrCount) && !bAsyncAttr; i++)
      {
        attrId = pAttrId[i];
        pAttrDef = ZCL_FindAttr(pCluster, attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp);
        bAsyncAttr =  (NULL !=pAttrDef) && (ZclAttrIsAsynchronous_c(pAttrDef->flags));
      }
      
      /* If there's at least one asynchronous attribute then call BeeAppUpdateDevice to handle the attributes lists */
      if(bAsyncAttr)
      {      
        BeeAppUpdateDevice(pIndication->srcEndPoint, gZclUI_SendReadAttributeRequest_c, attrId, pCluster->aClusterId, pIndication);
      }
      else
#endif      
      {
#if gFragmentationCapability_d  && (gZclEnablePartition_d != TRUE)       

       /* Use buffer allocated by ZCL_CreateFrame */
       MSG_Free(pMsgRsp);
       pMsgRsp = ZCL_CreateFrame( &addrInfo, 
                            gZclCmdReadAttrRsp_c,
                            gZclFrameControl_FrameTypeGeneral | 
                            ((pFrame->frameControl & gZclFrameControl_DirectionRsp)?0:gZclFrameControl_DirectionRsp ) 
                            | gZclFrameControl_DisableDefaultRsp,          /* IN: frame control field */
                            &pFrame->transactionId, 
                            (uint8_t*)&iPayloadLen,
                            NULL);
       if(!pMsgRsp)
          return gZclNoMem_c;
       
       iPayloadLen = ZCL_BuildReadAttrResponseWithFragmentation(
                                          pDevice,                  /* IN: ptr to device, for instance data */
                                          pCluster,                 /* IN: cluster to read from */
                                          attrCount,                /* IN: read attr count */
                                          pIndication,              /* IN: request input frame */
                                          pMsgRsp);               /* OUT: response output frame */

#else
      iPayloadLen = ZCL_BuildReadAttrResponse(
                                            pDevice,                  /* IN: ptr to device, for instance data */
                                            pCluster,                 /* IN: cluster to read from */
                                            attrCount,                /* IN: read attr count */
                                            pFrame,                   /* IN: request input frame */
                                            maxAsduLen,               /* IN: maximum ASDU length */ 
                                            pFrameRsp);              /* OUT: response output frame */      
#endif      
    }
   }
    break;
    
    /* same as write attr, but don't do it unless ALL of the records can be written */
  case gZclCmdWriteAttrUndivided_c:
    /* fall through... */
    
    /* write attribute command (1 or more records) */
  case gZclCmdWriteAttr_c:
    /* fall through... */
    
    /* same as write attr, but don't respond */
  case gZclCmdWriteAttrNoRsp_c:
    iPayloadLen = ZCL_BuildWriteAttrResponse(
                                             pDevice,                  /* IN: RAM data if writing endpoint specific data */
                                             pCluster,                 /* IN: cluster to write to */
                                             (uint8_t)pIndication->asduLength,  /* IN: length of write frame */
                                             pFrame,                   /* IN: request input frame */
                                             pFrameRsp);               /* OUT: response output frame */
    
    if(iPayloadLen == sizeof(zclFrame_t)+1) /* succes status*/
    {
      zclAttrId_t *pAttrId; 
      pAttrId = (void *)(pFrame + 1);
      /* send event to the application */
      BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_WriteAttributeSuccess_c, pAttrId[0], pCluster->aClusterId, pIndication);
    }
    
    /* no response needed/wanted */
    if(command == gZclCmdWriteAttrNoRsp_c)
      fSendResponse = FALSE;
    break;
    
#if gZclDiscoverAttrRsp_d      
  case gZclCmdDiscoverAttr_c:
    iPayloadLen = ZCL_BuildDiscoverAttrResponse(
                                              pCluster,                 /* IN: cluster to write to */
                                              pFrame,                   /* IN: request input frame */
                                              maxAsduLen,                /* IN: maximum ASDU length */ 
                                              pFrameRsp);               /* OUT: response output frame */
    break;
#endif      
    
#if gZclDiscoverAttrExtendedRsp_d     
  case gZclCmdDiscoverAttrExtended_c:
    iPayloadLen = ZCL_BuildDiscoverAttrExtendedResponse(    
                                                &addrInfo,                /* IN: address info */        
                                                pCluster,                 /* IN: cluster to write to */
                                                pFrame,                   /* IN: request input frame */
                                                pFrameRsp);               /* OUT: response output frame */

    break;
#endif     

#if gZclDiscoverCommandsRsp_d
  case gZclCmdDiscoverCmdGenerated_c:  
  case gZclCmdDiscoverCmdReceived_c:
    iPayloadLen = ZCL_BuildDiscoverCommandsResponse(    
                                                &addrInfo,                /* IN: address info */        
                                                pCluster,                 /* IN: cluster to write to */
                                                pFrame,                   /* IN: request input frame */
                                                pFrameRsp);               /* OUT: response output frame */

    break;    
#endif    
    
#if gZclEnableReporting_c
    /* configure this device for reporting */
  case gZclCmdCfgReporting_c:
    iPayloadLen = ZCL_BuildConfigureReportingResponse(
                                                      pDevice,                    /* IN: RAM data if writing endpoint specific data */
                                                      pCluster,                   /* IN: cluster to write to */
                                                      pIndication->asduLength,    /* IN: length of write frame */
                                                      pFrame,                     /* IN: frame with write attr commands */
                                                      pFrameRsp);                 /* OUT: response frame */
    break;
  case gZclCmdReadReportingCfg_c:
    iPayloadLen = ZCL_BuildReadReportingCfgResponse(
                                                      pDevice,                    /* IN: RAM data if writing endpoint specific data */
                                                      pCluster,                   /* IN: cluster to write to */
                                                      pIndication->asduLength,    /* IN: length of write frame */
                                                      pFrame,                     /* IN: frame with write attr commands */
                                                      pFrameRsp);                 /* OUT: response frame */
    break;
#endif
    
    
    /* unsupported command */
  default:
    fSendResponse = FALSE;  
    status = gZclUnsupportedGeneralCommand_c;
    break;
  }
  
  /* no cmd specific response specified, try default rsp, free it and we're done */
  if(!fSendResponse){
    MSG_Free(pMsgRsp);
    if (status == gZbSuccess_c) 
      status = gZclSuccessDefaultRsp_c;
  }
  else {    
    /* send the response if the attribute not are a analog device */
    if(pAttrDef == NULL || !ZclAttrIsAsynchronous_c(pAttrDef->flags))
      (void)ZCL_DataRequestNoCopy(&addrInfo, iPayloadLen, pMsgRsp);
    else
      MSG_Free(pMsgRsp);    
  }
(void)maxAsduLen;
return status;
}

/*****************************************************************************
* ZCL_SetupFrame
*
* Sets up a short frame from the original frame
*****************************************************************************/
void ZCL_SetupFrame
  (
  zclFrame_t *pDstFrame, 
  zclFrame_t *pSrcFrame
  )
{
  uint8_t offset;
  offset = (pSrcFrame->frameControl & gZclFrameControl_MfgSpecific) ?  sizeof(zclMfgFrame_t) :  sizeof(zclFrame_t);  
    
  FLib_MemCpy(pDstFrame, pSrcFrame, offset);
  pDstFrame->frameControl |= ((pSrcFrame->frameControl & gZclFrameControl_DirectionRsp)?0 : gZclFrameControl_DirectionRsp) 
                             | gZclFrameControl_DisableDefaultRsp;
}

/*******************************************************************************
* ZCL_ReadAttrReq
* 
* Send a read attribute command to another node.
*******************************************************************************/
zclStatus_t ZCL_ReadAttrReq
  (
  zclReadAttrReq_t *pReq
  )
{
  uint8_t iPayloadLen;
  afToApsdeMessage_t *pMsg;

  /* create the frame and copy in payload */
  iPayloadLen = pReq->count * sizeof(zclAttrId_t);
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),gZclCmdReadAttr_c,gZclFrameControl_FrameTypeGeneral,
    NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZbNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/*******************************************************************************
* ZCL_ReadAttrDirectedReq
* 
* Send a read attribute command to another node.
*******************************************************************************/
zclStatus_t ZCL_ReadAttrDirectedReq
  (
  zclReadAttrDirectedReq_t *pReq
  )
{
  uint8_t iPayloadLen;
  afToApsdeMessage_t *pMsg;
  zclFrameControl_t frameCtrl = gZclFrameControl_FrameTypeGeneral;
  
  if (pReq->direction)
    frameCtrl |= gZclFrameControl_DirectionRsp;

  /* create the frame and copy in payload */
  iPayloadLen = pReq->count * sizeof(zclAttrId_t);
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),gZclCmdReadAttr_c,frameCtrl,
    NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZbNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/*******************************************************************************
* ZCL_WriteAttrReq
* 
* Send a write attribute command to another node.
*******************************************************************************/
zclStatus_t ZCL_WriteAttrReq
  (
  zclWriteAttrReq_t *pReq
  )
{
  afToApsdeMessage_t *pMsg;
  uint8_t payloadLen;
  zclCmdWriteAttr_t *pWriteReqRecord;
  zclCmdWriteAttrRecord_t *pOtaWriteRecord;
  index_t i;
  uint8_t attrLen;
 
  /* nothing to do! */
  if(!pReq->count)
    return gZbSuccess_c;

  /* create the frame and copy in payload */
  payloadLen = 0;
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), pReq->reqType, gZclFrameControl_FrameTypeGeneral, NULL, &payloadLen,NULL);
  if(!pMsg)
    return gZclNoMem_c;

  /* pointer to input command frames */
  pWriteReqRecord = pReq->aCmdFrame;

  /* were to write the write attr request OTA frame */
  pOtaWriteRecord = (void *)(&((uint8_t *)pMsg)[ApsmeGetAsduOffset() + sizeof(zclFrame_t)]);

  /* attribute length */
  for(i=0; i<pReq->count; ++i) {

    /*
      note: no checks are made to see whether this attribute exists (the other side 
      will check), or whether the attribute is formatted properly
    */
    pOtaWriteRecord->attrId = pWriteReqRecord->attrId;
    pOtaWriteRecord->attrType = pWriteReqRecord->attrType;
    
    attrLen = pWriteReqRecord->attrLen;
    FLib_MemCpy(pOtaWriteRecord->aData, pWriteReqRecord->aData, attrLen);

    /* on to next attribute in OTA frame */
    attrLen += sizeof(zclAttrId_t) + sizeof(zclAttrType_t);
    pOtaWriteRecord = ( zclCmdWriteAttrRecord_t* )( ( (uint8_t *)pOtaWriteRecord ) + attrLen );
    payloadLen += attrLen;

    /* on to next request */
    ++attrLen;  /* one extra byte for length in request structure */
    pWriteReqRecord = ( zclCmdWriteAttr_t* )( ( (uint8_t *)pWriteReqRecord ) + attrLen );
  }
  
  pMsg->msgData.dataReq.fragmentHdr.iDataSize = payloadLen;

  /* send the packet */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), payloadLen, pMsg);
}

/*******************************************************************************
* ZCL_WriteAttrDirectedReq
* 
* Send a write attribute command to another client or server node.
*******************************************************************************/
zclStatus_t ZCL_WriteAttrDirectedReq
  (
  zclWriteAttrDirectedReq_t *pReq
  )
{
  afToApsdeMessage_t *pMsg;
  uint8_t payloadLen;
  zclCmdWriteAttr_t *pWriteReqRecord;
  zclCmdWriteAttrRecord_t *pOtaWriteRecord;
  index_t i;
  uint8_t attrLen;
  zclFrameControl_t frameCtrl = gZclFrameControl_FrameTypeGeneral;

  /* nothing to do! */
  if(!pReq->count)
    return gZbSuccess_c;

  if (pReq->direction)
    frameCtrl |= gZclFrameControl_DirectionRsp;

  /* create the frame and copy in payload */
  payloadLen = 0;
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), pReq->reqType, frameCtrl, NULL, &payloadLen,NULL);
  if(!pMsg)
    return gZclNoMem_c;

  /* pointer to input command frames */
  pWriteReqRecord = pReq->aCmdFrame;

  /* were to write the write attr request OTA frame */
  pOtaWriteRecord = (void *)(&((uint8_t *)pMsg)[ApsmeGetAsduOffset() + sizeof(zclFrame_t)]);

  /* attribute length */
  for(i=0; i<pReq->count; ++i) {

    /*
      note: no checks are made to see whether this attribute exists (the other side 
      will check), or whether the attribute is formatted properly
    */
    pOtaWriteRecord->attrId = pWriteReqRecord->attrId;
    pOtaWriteRecord->attrType = pWriteReqRecord->attrType;
    
    attrLen = pWriteReqRecord->attrLen;
    FLib_MemCpy(pOtaWriteRecord->aData, pWriteReqRecord->aData, attrLen);

    /* on to next attribute in OTA frame */
    attrLen += sizeof(zclAttrId_t) + sizeof(zclAttrType_t);
    pOtaWriteRecord = ( zclCmdWriteAttrRecord_t* )( ( (uint8_t *)pOtaWriteRecord ) + attrLen );
    payloadLen += attrLen;

    /* on to next request */
    ++attrLen;  /* one extra byte for length in request structure */
    pWriteReqRecord = ( zclCmdWriteAttr_t* )( ( (uint8_t *)pWriteReqRecord ) + attrLen );
  }
  
  pMsg->msgData.dataReq.fragmentHdr.iDataSize = payloadLen;

  /* send the packet */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), payloadLen, pMsg);
}

/********************************************************************************
* ZCL_ConfigureReportingReq
* 
* Create an over-the-air ZCL frame for configuring reporting from another node.
* Note: that reporting is done automatically through binding, using indirect
* mode in the AF_DataRequest().
* 
* Returns
*   gZbSuccess_c  if worked
*   gZbNoMem_c    if not enough memory for request
*********************************************************************************/
zclStatus_t ZCL_ConfigureReportingReq
  (
  zclConfigureReportingReq_t *pReq  /* IN: the request */
  )
{
  uint8_t payloadLen;
  uint8_t i;
  uint8_t recordLen, currentOffset = 0;
  afToApsdeMessage_t *pMsg;
  uint8_t *pCfgReqRecords;
  uint8_t *pOtaCfgRecord;

  /* determine length of reporting cfg payload */
  if(!pReq->count)
    return gZbSuccess_c;  /* nothing to do! */

  /* create the frame (will copy in payload later) */
  payloadLen = 0;
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),gZclCmdCfgReporting_c,gZclFrameControl_FrameTypeGeneral,
    NULL, &payloadLen, NULL);
  if(!pMsg)
    return gZbNoMem_c;

  /* create the OTA frame from the request */
  pCfgReqRecords = (void *)pReq->aCmdFrame;
  pOtaCfgRecord = (void *)(&((uint8_t *)pMsg)[ApsmeGetAsduOffset() + sizeof(zclFrame_t)]);
  for(i=0; i<pReq->count; ++i) 
  {  
    zclCmdReportingCfgServer_t *pCfgReqRecord; 
    /* on to next response record */
    pCfgReqRecord = (void *)&pCfgReqRecords[currentOffset];
    
    recordLen = sizeof(zclCmdReportingCfgClientRecord_t);
    if(!pCfgReqRecord->direction) {
      /* determine size of fixed size part of record */
      recordLen = sizeof(zclCmdReportingCfgServerRecord_t) - 1;
      
      /* copy the attribute reportable change: For attributes of 'discrete' data type, ReportableChange is omitted.
          [R2] - 2.4.7.1.7 Reportable Change Field */
        if(!ZCL_IsDataTypeDiscrete(pCfgReqRecord->attrType))
          FLib_MemCpy(&pOtaCfgRecord[recordLen], 
            pCfgReqRecord->aReportableChange, pCfgReqRecord->attrLen);
    }

    /* copy the record */
    FLib_MemCpy(pOtaCfgRecord, pCfgReqRecord, recordLen);

    /* record is longer by the attribute */
    if(!pCfgReqRecord->direction)
    {
      currentOffset += 1;
      /* update the record len */
      if(!ZCL_IsDataTypeDiscrete(pCfgReqRecord->attrType))
      {
        recordLen += pCfgReqRecord->attrLen;
      }
      else
       currentOffset += pCfgReqRecord->attrLen;
    }

    /* update OTA fields (on to next record) */
    payloadLen += recordLen;
    pOtaCfgRecord = (uint8_t*)( ( (uint8_t *)pOtaCfgRecord ) + recordLen );
    
    currentOffset+=recordLen;
  }
  
  pMsg->msgData.dataReq.fragmentHdr.iDataSize = payloadLen;

  /* send the packet */
  (void)ZCL_DataRequestNoCopy(&(pReq->addrInfo), payloadLen, pMsg);
  return gZbSuccess_c;
}

/*******************************************************************************
* ZCL_ReadReportingCfgReq
* 
* Send a read reporting configuration command to another node.
*******************************************************************************/
zclStatus_t ZCL_ReadReportingCfgReq
  (
  zclReadReportingCfgReq_t *pReq
  )
{
  uint8_t iPayloadLen;
  afToApsdeMessage_t *pMsg;

  /* create the frame and copy in payload */
  iPayloadLen = pReq->count * sizeof(zclCmdReadReportingCfgRecord_t);
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), gZclCmdReadReportingCfg_c, gZclFrameControl_FrameTypeGeneral,
    NULL, &iPayloadLen,&(pReq->aCmdFrame));
  if(!pMsg)
    return gZbNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}



/*******************************************************************************
* ZCL_CreateFrame
*******************************************************************************/
afToApsdeMessage_t *ZCL_CreateFrame
(
afAddrInfo_t *pAddrInfo,           /* IN: source and destination information */
zclCmd_t command, 	           /* IN: frame  command */
zclFrameControl_t frameControl,    /* IN: frame control field */
uint8_t *pTransactionId,
uint8_t *pPayloadLen, 		   /* IN/OUT: length of payload (then adjusted to length of asdu) */
void *pPayload		           /* IN: payload for entire frame */
)
{
  afToApsdeMessage_t *pMsg;
  zclFrame_t *pFrame;

#if !gFragmentationCapability_d   
  /* Avoid sending lonager data than possible to avoid buffer overflow */
  if (*pPayloadLen > (AF_MaxPayloadLen(pAddrInfo) - sizeof(zclFrame_t)))
    return NULL;
#endif
  
  /* allocate space for the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return NULL;  
    
  /* set up the frame */
  pFrame = (void *)(((uint8_t *)pMsg) + gAsduOffset_c);
  pFrame->frameControl = frameControl;
  if(pTransactionId)	
    pFrame->transactionId = *pTransactionId;
  else 
    if(!(frameControl & gZclFrameControl_DirectionRsp))
      pFrame->transactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
  pFrame->command = command;

  pMsg->msgData.dataReq.fragmentHdr.iDataSize = sizeof(zclFrame_t);
  
#if gFragmentationCapability_d  
  if(pPayload)
    (void)AF_MsgAppendData(pMsg, pPayload, (*pPayloadLen));  
#else
  if(pPayload)
    FLib_MemCpy((pFrame + 1), pPayload, (*pPayloadLen));
#endif  
  
  /* convert payload len to asdu len */
  *pPayloadLen += sizeof(zclFrame_t); 
  
  /* return ptr to msg buffer */
  return pMsg;
}

/******************************************************************************
  ZCL_DataRequestNoCopy()
*******************************************************************************/
zclStatus_t ZCL_DataRequestNoCopy
(
afAddrInfo_t *pAddrInfo,      /* IN: source and destination information */
uint16_t payloadLen,           /* IN: length of payload */
afToApsdeMessage_t *pMsg     /* IN: allocated msg structure */
)
{
  zclFrame_t *pFrame;
  pFrame = (zclFrame_t *)pMsg->msgData.dataReq.pAsdu; 

  bool_t basicDeviceEnabled = 0;
  zbClusterId_t basicClusterId = {gaZclClusterBasic_c}; 
       
  (void)ZCL_GetAttribute(pAddrInfo->srcEndPoint,  basicClusterId, gZclAttrBasic_DeviceEnabledId_c, gZclServerAttr_c, &basicDeviceEnabled, NULL);

    /* if device is disabled; should send/respond only to read/write attr and identiy cluster command */
    /* [R2] - 3.2.2.2.13 Device Enable Attribute */  	
    if(!IsEqual2BytesInt(pAddrInfo->aClusterId, gZclClusterIdentify_c))
    {
	  if(!basicDeviceEnabled)
	  {
    	if((!(ZCL_IsFrameType(pFrame->frameControl) == gZclFrameControl_FrameTypeGeneral)) || ((ZCL_IsFrameType(pFrame->frameControl) == gZclFrameControl_FrameTypeGeneral) &&
			(pFrame->command != gZclCmdWriteAttrRsp_c && pFrame->command != gZclCmdReadAttrRsp_c && pFrame->command != gZclCmdWriteAttr_c && pFrame->command != gZclCmdReadAttr_c)))
        {
        	AF_MsgFreeRequest(pMsg);
			return gZbSuccess_c;
        }
	  }
    }	
      return AF_DataRequestNoCopy(pAddrInfo, payloadLen, pMsg, NULL);
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/


/*******************************************************************************
* ZCL_BuildReadAttrResponseWithFragmentation
*  
* Build a response to the read attribute(s) command. Works across clusters and 
* understands multiple instances of an endpoint. This function is used when 
* fragmentation is enabled so the node can interpret a large request and also
* respond with a message longer than one buffer.
* 
* Returns total length of payload
*******************************************************************************/
#if (gZclEnablePartition_d != TRUE)
#if gFragmentationCapability_d
uint16_t ZCL_BuildReadAttrResponseWithFragmentation
  (
  afDeviceDef_t  *pDevice,    /* IN: device definition for this endpoint */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint16_t count,              /* IN: # of responses */
  zbApsdeDataIndication_t *pIndication,         /* IN: data indication */
  afToApsdeMessage_t *pMsg       /* IN/OUT: response frame, allocated above, filled in here... */
  )
{
  zclCmdReadAttrRspRecord_t rspRecord;  /* building the response record */
  zclAttrDef_t *pAttrDef;    /* pointer to attribute definition */
  uint8_t   *pAttrData = NULL;
  uint8_t   *pAttrDst;
  uint8_t   attrLen;
  uint16_t    i;
  uint16_t   iTotalLen;
  uint16_t   iSizeLeft;
  zclFrame_t *pFrame;
  uint16_t iOffset;

  /* Current offset in the read attribute request */
  iOffset = sizeof(zclFrame_t);
  
  /* Length of the read attribute response */
  iTotalLen = sizeof(zclFrame_t);
  
  pFrame = (zclFrame_t *)pIndication->pAsdu;
  
  /* Limit payload to maximum transfer size */
  iSizeLeft = GetMaxOutTransferSize(pIndication->aSrcAddr) - sizeof(zclFrame_t);
    
  for(i=0; i < count && iSizeLeft >= sizeof(zclCmdReadAttrRspError_t); ++i) 
  {
    /* Get next attributeId from the received payload */
    if (AF_MsgGetOffsetData(pIndication, iOffset,&rspRecord.attrId, sizeof(zclAttrId_t)))
      break;
    
    rspRecord.status = gZclUnsupportedAttribute_c;
    attrLen = sizeof(zclCmdReadAttrRspError_t);

    /* if attribute does exist, return it (all attributes can be read) */
    pAttrDef = ZCL_FindAttr(pCluster, rspRecord.attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp);
    if(pAttrDef)
    {
      /* check for write only attribute */
      if(ZclAttrIsWriteOnly(pAttrDef->flags)) 
      {
        rspRecord.status = gZclWriteOnly_c;  
        break;
      }

      /* don't allow too long packet */
      attrLen = ZCL_GetDataTypeLen(pAttrDef->type, pAttrDef->data.pData, pAttrDef);
      if(iSizeLeft < (sizeof(zclCmdReadAttrRspRecord_t) - sizeof(rspRecord.aData)) + attrLen)
        break;

      /* copy in attribute type */
      rspRecord.attrType = pAttrDef->type;
      rspRecord.status = gZbSuccess_c;

      /* copy in the data */
      pAttrData = ZCL_GetAttributePtr(pDevice, pCluster, rspRecord.attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp, &attrLen);

      /* ROM strings do not have the length byte in front of them, generate it */
      if(ZCL_IsStrType(pAttrDef->type) && (pAttrDef->flags & gZclAttrFlagsInROM_c)) {
        *pAttrDst++ = attrLen;
        ++attrLen;
      }
#if gZclEnableLongStringTypes_c      
      else if (ZCL_IsLongStrType(pAttrDef->type) && (pAttrDef->flags & gZclAttrFlagsInROM_c)) {
        *((uint16_t*)pAttrDst) = Native2OTA16(attrLen);
        pAttrDst += sizeof(uint16_t);
        attrLen += 2;
      }
#endif      
    }    
    
    if (pAttrDef)
    {
      /* Set the attribute response record payload */
      if (AF_MsgAppendData(pMsg, &rspRecord, sizeof(zclCmdReadAttrRspRecord_t)- sizeof(rspRecord.aData)))
        break;

      /* copy in the data */
      if (AF_MsgAppendData(pMsg, pAttrData, attrLen))
        break;

      /* add in size of response record */
      attrLen += (sizeof(zclCmdReadAttrRspRecord_t) - sizeof(rspRecord.aData));
    }
    else
    {
      /* Set the attribute response record payload */
      if (AF_MsgAppendData(pMsg, &rspRecord, attrLen))
        break;
    }
    
    /* on to next attribute */
    iTotalLen += attrLen;
    iSizeLeft -= attrLen;
    iOffset += sizeof(zclAttrId_t);
   
  } /* end of for loop */

  return iTotalLen;
}

#else

uint8_t ZCL_BuildReadAttrResponse
  (
  afDeviceDef_t  *pDevice,    /* IN: device definition for this endpoint */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint8_t count,              /* IN: # of responses */
  zclFrame_t *pFrame,         /* IN: read attr request */
  uint8_t maxAsduLen,         /* IN: maximum ASDU length */ 
  zclFrame_t *pFrameRsp       /* IN/OUT: response frame, allocated above, filled in here... */
  )
{
  zclCmdReadAttrRspRecord_t *pRspRecord;  /* building the response record */
  zclAttrId_t *pAttrId;   /* attribute IDs to read from this cluster */
  zclAttrDef_t *pAttrDef;    /* pointer to attribute definition */
  uint8_t   *pAttrData;
  uint8_t   *pAttrDst;
  uint8_t   attrLen;
  index_t    i;
  uint8_t   iTotalLen;
  uint8_t   iSizeLeft;
  zclAttrId_t attrId; /* attr ID */

  /* build response frame */
  pFrameRsp->command = gZclCmdReadAttrRsp_c;

  /* get pointers past the HA frame header */
  pRspRecord = (void *)(pFrameRsp + 1);
  pAttrId = (void *)(pFrame + 1);

  iTotalLen = sizeof(zclFrame_t);
  iSizeLeft = maxAsduLen - sizeof(zclFrame_t) ;  
  
  for(i=0; i<count && iSizeLeft >= sizeof(zclCmdReadAttrRspError_t); ++i) {

    /* assume attribute doesn't exist */
#if ( gBigEndian_c != TRUE)
    {
      //ARM7 memory aligment handling
      volatile uint8_t* pTemp8 = (uint8_t*)(pAttrId + i);
      attrId = *pTemp8++;
      attrId = attrId + ((*pTemp8) << 8);
    }
#else
    attrId = pAttrId[i];      
#endif
              
    pRspRecord->attrId = attrId;
    pRspRecord->status = gZclUnsupportedAttribute_c;
    attrLen = sizeof(zclCmdReadAttrRspError_t);

    /* where to put the data */
    pAttrDst = pRspRecord->aData;

    /* if attribute does exist, return it (all attributes can be read) */
    pAttrDef = ZCL_FindAttr(pCluster, attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp);
    if(pAttrDef) {

      /* check for write only attribute */
      if(ZclAttrIsWriteOnly(pAttrDef->flags)) 
      {
        pRspRecord->status = gZclWriteOnly_c;  
        break;
      }

      /* don't allow too long packet */
      attrLen = ZCL_GetDataTypeLen(pAttrDef->type, pAttrDef->data.pData, pAttrDef);
      if(iSizeLeft < (sizeof(zclCmdReadAttrRspRecord_t) - sizeof(pRspRecord->aData)) + attrLen)
        break;

      /* copy in attribute type */
      pRspRecord->attrType = pAttrDef->type;
      pRspRecord->status = gZbSuccess_c;

      /* copy in the data */
      pAttrData = ZCL_GetAttributePtr(pDevice, pCluster, attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp, &attrLen);

      /* ROM strings do not have the length byte in front of them, generate it */
      if(ZCL_IsStrType(pAttrDef->type) && (pAttrDef->flags & gZclAttrFlagsInROM_c)) {
        *pAttrDst++ = attrLen;
        ++attrLen;
      }
#if gZclEnableLongStringTypes_c      
      else if (ZCL_IsLongStrType(pAttrDef->type) && (pAttrDef->flags & gZclAttrFlagsInROM_c)) {
        *((uint16_t*)pAttrDst) = Native2OTA16(attrLen);
        pAttrDst += sizeof(uint16_t);
        attrLen += 2;
      }
#endif      

      /* copy in the data */
      FLib_MemCpy(pAttrDst, pAttrData, attrLen);

      /* add in size of response record */
      attrLen += (sizeof(zclCmdReadAttrRspRecord_t) - sizeof(pRspRecord->aData));
    }

    /* on to next attribute */
    iTotalLen += attrLen;
    iSizeLeft -= attrLen;
    pRspRecord = (zclCmdReadAttrRspRecord_t*)( ( (uint8_t *)pRspRecord ) + attrLen );

  } /* end of for loop */

  return iTotalLen;
}

#endif

#else

// partition enabled
uint8_t ZCL_BuildReadAttrResponseWithPartition
  (
  afDeviceDef_t  *pDevice,    /* IN: device definition for this endpoint */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint8_t count,              /* IN: # of responses */
  zclFrame_t *pFrame,         /* IN: read attr request */
  uint8_t maxAsduLen,         /* IN: maximum ASDU length */ 
  zclFrame_t *pFrameRsp,       /* IN/OUT: response frame, allocated above, filled in here... */
  bool_t partitionReadAttr
  ) 
{
  
  zclCmdReadAttrRspRecord_t *pRspRecord;  /* building the response record */
  zclAttrId_t *pAttrId;   /* attribute IDs to read from this cluster */
  zclAttrDef_t *pAttrDef;    /* pointer to attribute definition */
  uint8_t   *pAttrData;
  uint8_t   *pAttrDst;
  uint8_t   attrLen;
  index_t    i;
  uint8_t   iTotalLen;
  uint8_t   iSizeLeft;
  zclAttrId_t attrId; /* attr ID */
  zbClusterId_t partitionedClusterId;

   /* get pointers past the HA frame header */
  pRspRecord = (void *)(pFrameRsp + 1);
  pAttrId = (void *)(pFrame + 1);

  /* build response frame */
   
  if (!partitionReadAttr) {
    (void) partitionedClusterId;
    pFrameRsp->command = gZclCmdReadAttrRsp_c;  
    iTotalLen = sizeof(zclFrame_t);
    iSizeLeft = maxAsduLen - sizeof(zclFrame_t);    
  }
  else {
    pFrameRsp->command = gZclPartitionCmdTx_ReadHandshakeParamRsp_c;  
    Copy2Bytes(pRspRecord, pAttrId);
    Copy2Bytes(&partitionedClusterId, pAttrId);
    pRspRecord = (void *)((uint8_t*)pRspRecord + sizeof(zbClusterId_t));      
    pAttrId =  (void *)((uint8_t*)pAttrId + sizeof(zbClusterId_t));      
    iTotalLen = sizeof(zclFrame_t) + sizeof(zbClusterId_t);
    iSizeLeft = maxAsduLen - iTotalLen;        
  }
  
  for(i=0; i<count && iSizeLeft >= sizeof(zclCmdReadAttrRspError_t); ++i) {

    /* assume attribute doesn't exist */
#if ( gBigEndian_c != TRUE)
    {
      //ARM7 memory aligment handling
      volatile uint8_t* pTemp8 = (uint8_t*)(pAttrId + i);
      attrId = *pTemp8++;
      attrId = attrId + ((*pTemp8) << 8);
    }
#else
    attrId = pAttrId[i];      
#endif
    pRspRecord->attrId = attrId;
    pRspRecord->status = gZclUnsupportedAttribute_c;
    attrLen = sizeof(zclCmdReadAttrRspError_t);

    /* where to put the data */
    pAttrDst = pRspRecord->aData;

    /* if attribute does exist, return it (all attributes can be read) */
    if (!partitionReadAttr)    
      pAttrDef = ZCL_FindAttr(pCluster, attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp);
    else
      pAttrDef = ZCL_FindPartitionAttr(attrId, partitionedClusterId, 
                                        pFrame->transactionId);
    
    if(pAttrDef) {

      /* check for write only attribute */
      if(ZclAttrIsWriteOnly(pAttrDef->flags)) 
      {
        pRspRecord->status = gZclWriteOnly_c;  
        break;
      }

      /* don't allow too long packet */
      attrLen = ZCL_GetDataTypeLen(pAttrDef->type, pAttrDef->data.pData, pAttrDef);
      if(iSizeLeft < (sizeof(zclCmdReadAttrRspRecord_t) - sizeof(pRspRecord->aData)) + attrLen)
        break;

      /* copy in attribute type */
      pRspRecord->attrType = pAttrDef->type;
      pRspRecord->status = gZbSuccess_c;

      /* copy in the data */
      if (!partitionReadAttr)       
        pAttrData = ZCL_GetAttributePtr(pDevice, pCluster, attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp, &attrLen);
      else
        pAttrData = ZCL_GetPartitionAttributePtr(attrId, partitionedClusterId, 
                                        pFrame->transactionId, &attrLen);

       /* ROM strings do not have the length byte in front of them, generate it */
      if(ZCL_IsStrType(pAttrDef->type) && (pAttrDef->flags & gZclAttrFlagsInROM_c)) {
        *pAttrDst++ = attrLen;
        ++attrLen;
      }
#if gZclEnableLongStringTypes_c      
      else if (ZCL_IsLongStrType(pAttrDef->type) && (pAttrDef->flags & gZclAttrFlagsInROM_c)) {
        *((uint16_t*)pAttrDst) = Native2OTA16(attrLen);
        pAttrDst += sizeof(uint16_t);
        attrLen += 2;
      }
#endif      

      /* copy in the data */
      FLib_MemCpy(pAttrDst, pAttrData, attrLen);

      /* add in size of response record */
      attrLen += (sizeof(zclCmdReadAttrRspRecord_t) - sizeof(pRspRecord->aData));
    }

    /* on to next attribute */
    iTotalLen += attrLen;
    iSizeLeft -= attrLen;
    pRspRecord = (zclCmdReadAttrRspRecord_t*)( ( (uint8_t *)pRspRecord ) + attrLen );

  } /* end of for loop */

  return iTotalLen;
}

uint8_t ZCL_BuildReadAttrResponse
  (
  afDeviceDef_t  *pDevice,    /* IN: device definition for this endpoint */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint8_t count,              /* IN: # of responses */
  zclFrame_t *pFrame,         /* IN: read attr request */
  uint8_t maxAsduLen,         /* IN: maximum ASDU length */ 
  zclFrame_t *pFrameRsp       /* IN/OUT: response frame, allocated above, filled in here... */
  )
{
  return ZCL_BuildReadAttrResponseWithPartition(pDevice, pCluster, count, pFrame, maxAsduLen, pFrameRsp, FALSE);
}
#endif


/*******************************************************************************
* ZCL_SendDefaultResponse (2.4.12)
*  
* Builds and sends a response in case there is no other predefined response
* for a certain ZCL frame.
* 
*******************************************************************************/
zbStatus_t ZCL_SendDefaultResponse
  (
    zbApsdeDataIndication_t *pIndication, /* IN: OTA indication */
    zclStatus_t status
  )
{
  zclFrame_t *pFrame;
  zclMfgFrame_t *pMfgFrame;
  afToApsdeMessage_t *pMsgRsp;
  zclCmdDefaultRsp_t *pRspRecord;
  uint8_t command;
  uint8_t *pCmd;
  uint8_t offset;

  /* default response is sent only for unicast incoming frames */
  if (pIndication->fWasBroadcast || pIndication->dstAddrMode == gZbAddrModeGroup_c)
    return gZbSuccess_c;
  
  /* incoming ZCL frame */
  pFrame = (zclFrame_t *) pIndication->pAsdu;
  pMfgFrame = (zclMfgFrame_t *) pIndication->pAsdu;
  
  if (pFrame->frameControl & gZclFrameControl_MfgSpecific)
  {
    pCmd = &(pMfgFrame->command);
    offset = sizeof(zclMfgFrame_t);
  }
  else
  {
    pCmd = &(pFrame->command);
    offset = sizeof(zclFrame_t);
  }

  /* save the incoming command to put in default rsp payload*/
  command = *pCmd;

  /* don't send default rsp if corresponding flag is set in frame control or
   * incoming frame was a default rsp too */
  if (((pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) && (status == gZclSuccess_c)) 
      || ((command == gZclCmdDefaultRsp_c) && ((pFrame->frameControl & gZclFrameControl_FrameTypeMask) == gZclFrameControl_FrameTypeGeneral) ))
    return gZbSuccess_c;
  
  /* else begin to build frame for default rsp based on incoming frame; */
  /* set this in payload frame  so that ZCL_ReplyNoCopy will generate the
   * same command type as for indication */    
  *pCmd = gZclCmdDefaultRsp_c;
  
  /* Make sure there will be no default rsp as reply to this default rsp */  
  pFrame->frameControl |= gZclFrameControl_DisableDefaultRsp;
  
  /* Set the frame type bits */  
  pFrame->frameControl &= ~gZclFrameControl_FrameTypeMask;

  /* Allocate message for default rsp */  
  pMsgRsp = AF_MsgAlloc();
  if(!pMsgRsp)
    return gZclNoMem_c;
  
  /* Get the address of the actual ZCL payload of default rsp */  
  pRspRecord = (void *)(((uint8_t *)pMsgRsp) + gAsduOffset_c + offset);
  
  /* Use the previously stored command from incoming frame */  
  pRspRecord->cmd = command;
  
  /* And the status received as parameter */  
  pRspRecord->status = status;
  
  /* Hand it over to ZCL_ReplyNoCopy to build reply addr info and send */
  return ZCL_ReplyNoCopy(pIndication, sizeof(zclCmdDefaultRsp_t), pMsgRsp);
}

/*******************************************************************************
* ZCL_WriteAttrUndividedValidateFields
*
* Validate fields for Write Attribute Undivided Command
*
* Returns TRUE - all fields are valid, FALSE - otherwise
*******************************************************************************/
bool_t ZCL_WriteAttrUndividedValidateFields(afClusterDef_t *pCluster, zclCmdWriteAttrRecord_t  *pRecord, 
                                   zclFrame_t *pFrame, uint8_t asduLen, uint8_t endPoint)
{
  zclAttrDef_t  *pAttrDef;
  uint8_t attrLen;
  
  /* walk through list of attribtes to write */
  while(asduLen >= sizeof(zclAttrId_t) + 2) 
  {
    /* does this attribute exist? */
    pAttrDef = ZCL_FindAttr(pCluster, pRecord->attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp);
    if(!pAttrDef)
      return FALSE;
    else
    {
      bool_t isStrType = ZCL_IsStrType(pAttrDef->type);
#if gZclEnableLongStringTypes_c    
      bool_t isLongStrType = ZCL_IsLongStrType(pAttrDef->type);      
#endif
      
#if gAddValidationFuncPtrToClusterDef_c
      bool_t bValueIsValid = (NULL != pCluster->pfnValidationFunc) ? 
                              pCluster->pfnValidationFunc(endPoint, pCluster->aClusterId, pRecord) : TRUE;
#endif
      
      attrLen = ZCL_GetDataTypeLen(pAttrDef->type, pAttrDef->data.pData, pAttrDef);

      /* determine length of attribute */
      if(ZCL_IsStrType(pAttrDef->type))
        attrLen = pRecord->aData[0] + 1;
#if gZclEnableLongStringTypes_c          
      else if (isLongStrType)
        attrLen = OTA2Native16(*(uint16_t*)(pRecord->aData)) + 2;
#endif      
       
      /* invalid data type, read only, Defined Out Of Band */
      if((pAttrDef->type != pRecord->attrType) || (ZclAttrIsReadOnly(pAttrDef->flags)) ||
        (ZclAttrIsDefinedOutOfBand(pAttrDef->flags)))
        return FALSE;
      
#if gZclEnableLongStringTypes_c    
      else if((isStrType || isLongStrType) && attrLen > pAttrDef->size)
        return FALSE;
#else
      else if(isStrType && attrLen > pAttrDef->size)
        return FALSE;      
#endif
      /* value is out of range */
#if gAddValidationFuncPtrToClusterDef_c
      else if (!bValueIsValid)
        return FALSE;  
#endif  
    }
        /* add in size of identifier (attrLen now means size of this write request record) */
    attrLen = 1+ sizeof(zclAttrId_t) + ZCL_GetDataTypeLen(pRecord->attrType, pRecord->aData, pAttrDef);

    /* point to next write request record */
    pRecord = (zclCmdWriteAttrRecord_t*)( ( (uint8_t *)pRecord ) + attrLen );
    
    if(attrLen >= asduLen)
      asduLen = 0;
    else
      asduLen -= attrLen;
  }
  return TRUE;
}

/*******************************************************************************
* ZCL_BuildWriteAttrResponse
*
* Build a response to the write attribute(s) command. Works across clusters and 
* understands multiple instances of an endpoint.
*
* Also does the actual write to the attribute
*
* Returns total length of payload
*******************************************************************************/
#if (gZclEnablePartition_d != TRUE)
STATIC uint8_t ZCL_BuildWriteAttrResponse
  (
  afDeviceDef_t  *pDevice,    /* IN: data to be acted on by this cluster */
  afClusterDef_t *pCluster,   /* IN: cluster definition to write attrs */
  uint8_t asduLen,            /* IN: length of input frame */
  zclFrame_t *pFrame,         /* IN: input frame */
  zclFrame_t *pFrameRsp       /* IN/OUT: response to write attribute */
  )
{
  zclCmdWriteAttrRecord_t  *pRecord;      /* input records (received from over-the-air) */
  zclCmdWriteAttrRspRecord_t *pRspRecord; /* output records (to be sent over-the-air) */
  uint8_t iRspLen;
  uint8_t attrLen;
  zclAttrDef_t  *pAttrDef;
  uint8_t *pDstAttrData;
  bool_t fIsIdentifyCluster;
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)
  bool_t fIsPollControlCluster;
#endif 
  zbEndPoint_t endPoint;
  bool_t writeAttrValue = TRUE; /* used for write attribute undivided (see ZCL - 2.4.4) */

  /* build response frame */
  pFrameRsp->command = gZclCmdWriteAttrRsp_c;

  /* write each attribute */
  pRecord = (void *)(pFrame + 1);
  asduLen -= sizeof(zclFrame_t);
  pRspRecord = (void *)(pFrameRsp + 1);
  iRspLen = sizeof(zclFrame_t);

  /* special case: check if identify, then writing to the cluster is a command */
  fIsIdentifyCluster = IsEqual2BytesInt(pCluster->aClusterId, gZclClusterIdentify_c);
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)
  /* special case: check if poll control cluster */
  fIsPollControlCluster = IsEqual2BytesInt(pCluster->aClusterId, gZclClusterPollControl_c);
#endif  
  
  endPoint = AF_DeviceDefToEndPoint(pDevice);
  
  if(pFrame->command == gZclCmdWriteAttrUndivided_c)
  {
    writeAttrValue = ZCL_WriteAttrUndividedValidateFields(pCluster, pRecord, pFrame, asduLen, endPoint);
  }
  

  /* walk through list of attribtes to write */
  while(asduLen >= sizeof(zclAttrId_t) + 2) 
  {

    /* assume unsupported attribute */
    pRspRecord->attrId = pRecord->attrId;
    pRspRecord->status = gZclUnsupportedAttribute_c;

    /* does this attribute exist? */
    pAttrDef = ZCL_FindAttr(pCluster, pRecord->attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp);
    attrLen = 1;     /* assume 1-byte attribute if we know nothing about it */

    /* found the attribute, write it */
    if(pAttrDef) 
    {
      bool_t isStrType = ZCL_IsStrType(pAttrDef->type);
#if gZclEnableLongStringTypes_c    
      bool_t isLongStrType = ZCL_IsLongStrType(pAttrDef->type);      
#endif
      
#if gAddValidationFuncPtrToClusterDef_c
      bool_t bValueIsValid = (NULL != pCluster->pfnValidationFunc) ? 
                              pCluster->pfnValidationFunc(endPoint, pCluster->aClusterId, pRecord) : TRUE;
#endif
      
      attrLen = ZCL_GetDataTypeLen(pAttrDef->type, pAttrDef->data.pData, pAttrDef);

      /* determine length of attribute */
      if(ZCL_IsStrType(pAttrDef->type))
        attrLen = pRecord->aData[0] + 1;
#if gZclEnableLongStringTypes_c          
      else if (isLongStrType)
        attrLen = OTA2Native16(*(uint16_t*)(pRecord->aData)) + 2;
#endif      
       
      /* invalid data type */
      if (pAttrDef->type != pRecord->attrType)
        pRspRecord->status = gZclInvalidDataType_c;
      
      /* read only */
      else if (ZclAttrIsReadOnly(pAttrDef->flags))
        pRspRecord->status = gZclReadOnly_c;
      /* Defined Out Of Band */      
      
      else if (ZclAttrIsDefinedOutOfBand(pAttrDef->flags))
        pRspRecord->status = gZclDefinedOutOfBand_c;
#if gZclEnableLongStringTypes_c    
      else if((isStrType || isLongStrType) && attrLen > pAttrDef->size)
        pRspRecord->status = gZclInvalidValue_c;
#else
      else if(isStrType && attrLen > pAttrDef->size)
        pRspRecord->status = gZclInvalidValue_c;      
#endif
      /* value is out of range */
#if gAddValidationFuncPtrToClusterDef_c
      else if (!bValueIsValid)
        pRspRecord->status = gZclInvalidValue_c;  
#endif
      /* write to the attribute */
      else 
      {
        if(writeAttrValue)
        {
          /* determine where to put the thing */        
          if(ZclAttrIsInstance(pAttrDef->flags))
          {
            if (ZclAttrIsInSceneData(pAttrDef->flags))
              pDstAttrData = &((uint8_t *)pDevice->pSceneData)[pCluster->dataOffset + pAttrDef->data.offset];
            else
              pDstAttrData = &((uint8_t *)pDevice->pData)[pCluster->dataOffset + pAttrDef->data.offset];
          }
          else
            pDstAttrData = pAttrDef->data.pData;
               
          /* special case: writing to identify is also a command */
          if(fIsIdentifyCluster) 
          {
            ZCL_SetIdentifyMode(endPoint, (*((uint16_t *)(pRecord->aData))));
          }

          /* copy the attribute data from indication to true location */
          else 
          {
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)          
            if(fIsPollControlCluster)
            {
              /* special case: poll control cluster;  the Server returns Success Status but the attribute will be updated only if pass some conditions */
              ZclPollControl_WriteAttribute(endPoint, pRecord);
            }
            else
            {
#endif       
            FLib_MemCpy(pDstAttrData, pRecord->aData, attrLen);

            /* should check for reporting here */
#if gZclEnableReporting_c
            {
              uint8_t reportIndex = 0;
              /* report this attribute if attribute is reporting (and has changed enough) */
              if(ZCL_IsReportingAndChanged(pDevice, pCluster, pRecord->attrId,&reportIndex)) 
              {
                  zclReportingSetup_t *zclReportingSetup;
                  zclReportingSetup = &((zclReportingSetup_t *)pDevice->pReportSetup)[0];   
        
                  if(zclReportingSetup == NULL)
                  continue;
        
                  /* do the report */  
                  if (zclReportingSetup[reportIndex].minTimeCounter == 0)
                  {      
                      ZbZclFoundationGlobals(gfZclReportRestart) = TRUE;
                      zclReportingSetup[reportIndex].sendReportMask |= gZCLReportingAttrValueChanged_c;
                      TS_SendEvent(gZclTaskId, gZclEventReportTimeout_c);     
                      zclReportingSetup[reportIndex].minTimeCounter = zclReportingSetup[reportIndex].reportMin;
                  }
                  else 
                  {  
                      if (zclReportingSetup[reportIndex].reportMin)
    		 	 zclReportingSetup[reportIndex].sendReportMask |= gZCLReportingAttrValueChanged_c;  
                  }

                  /* reset the timer if needed */      
                  if((zclReportingSetup[reportIndex].reportTimeout  || zclReportingSetup[reportIndex].reportMin) && !zclReportingSetup[reportIndex].reportCounter)
                  {      
                      /*Initialize the counter with the number of seconds we want to report*/
                      zclReportingSetup[reportIndex].reportCounter = zclReportingSetup[reportIndex].reportTimeout; 
                      ZCL_StartReportingTimer();
                  }
              }
            }
#endif          
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)
            }
#endif
          }
        } /* if(writeAttrValue) */
        
        /* worked */
        pRspRecord->status = gZbSuccess_c;
      }
    }

    /* add in size of identifier (attrLen now means size of this write request record) */
    attrLen = 1+ sizeof(zclAttrId_t) + ZCL_GetDataTypeLen(pRecord->attrType, pRecord->aData, pAttrDef);

    /* point to next write request record */
    pRecord = (zclCmdWriteAttrRecord_t*)( ( (uint8_t *)pRecord ) + attrLen );
    
    if(attrLen >= asduLen)
      asduLen = 0;
    else
      asduLen -= attrLen;

    /* point to next output record , if there is an error*/
    if(pRspRecord->status != gZbSuccess_c) {
      ++pRspRecord;       /* include attr IDs for failed ones */
      iRspLen += sizeof(zclCmdWriteAttrRspRecord_t);
    }
  }

  /*If all write attribute records were a success then only return a success status
  we check whether the length has been increased in order to determine whether response
  records with error codes have been added.
  */
  if (iRspLen == sizeof(zclFrame_t))
  { /* add size of status */
    iRspLen +=  sizeof (uint8_t);
  }
  return iRspLen;
}
#else
uint8_t ZCL_BuildWriteAttrResponseWithPartition
  (
  afDeviceDef_t  *pDevice,    /* IN: data to be acted on by this cluster */
  afClusterDef_t *pCluster,   /* IN: cluster definition to write attrs */
  uint8_t asduLen,            /* IN: length of input frame */
  zclFrame_t *pFrame,         /* IN: input frame */
  zclFrame_t *pFrameRsp,      /* IN/OUT: response to write attribute */
  bool_t partitionedWriteAttr,/* IN: write to a partition table instead of main atttr table */
  bool_t forceWritingROAttr   /* IN: write data even if the attribute is readonly */
  )
{
  zclCmdWriteAttrRecord_t  *pRecord;      /* input records (received from over-the-air) */
  zclCmdWriteAttrRspRecord_t *pRspRecord; /* output records (to be sent over-the-air) */
  uint8_t iRspLen;
  uint8_t attrLen;
  zclAttrDef_t  *pAttrDef;
  uint8_t *pDstAttrData;
  bool_t fIsIdentifyCluster;
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d) 
  bool_t fIsPollControlCluster;
#endif   
  zbEndPoint_t endPoint;
  zbClusterId_t partitionedClusterId;

  /* build response frame */
  pFrameRsp->command = gZclCmdWriteAttrRsp_c;

  /* write each attribute */
  pRecord = (void *)(pFrame + 1);
  asduLen -= sizeof(zclFrame_t);
  pRspRecord = (void *)(pFrameRsp + 1);
  iRspLen = sizeof(zclFrame_t);
  
  (void) partitionedClusterId;
  
  if (partitionedWriteAttr) {
    Copy2Bytes(&partitionedClusterId, pRecord);
    pRecord = (void*)((uint8_t*)pRecord + sizeof(zbClusterId_t));
  }

  /* special case: check if identify, then writing to the cluster is a command */
  fIsIdentifyCluster = IsEqual2BytesInt(pCluster->aClusterId, gZclClusterIdentify_c);
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d) 
  /* special case: check if poll control cluster */
  fIsPollControlCluster = IsEqual2BytesInt(pCluster->aClusterId, gZclClusterPollControl_c);
#endif   
  endPoint = AF_DeviceDefToEndPoint(pDevice);

  /* walk through list of attribtes to write */
  while(asduLen >= sizeof(zclAttrId_t) + 2) {

    /* assume unsupported attribute */
    pRspRecord->attrId = pRecord->attrId;
    pRspRecord->status = gZclUnsupportedAttribute_c;

    /* does this attribute exist? */
    if (!partitionedWriteAttr)
      pAttrDef = ZCL_FindAttr(pCluster, pRecord->attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp);
    else
      pAttrDef = ZCL_FindPartitionAttr(pRecord->attrId, partitionedClusterId, 
                                        pFrame->transactionId);
    
    attrLen = 1;     /* assume 1-byte attribute if we know nothing about it */

    /* found the attribute, write it */
    if(pAttrDef) {
      bool_t isStrType = ZCL_IsStrType(pAttrDef->type);
#if gZclEnableLongStringTypes_c    
      bool_t isLongStrType = ZCL_IsLongStrType(pAttrDef->type);      
#endif

#if gAddValidationFuncPtrToClusterDef_c
      bool_t bValueIsValid = (NULL != pCluster->pfnValidationFunc) ? 
                              pCluster->pfnValidationFunc(endPoint, pCluster->aClusterId, pRecord) : TRUE;
#endif
      
      attrLen = pAttrDef->size;

      /* determine length of attribute */
      if(ZCL_IsStrType(pAttrDef->type))
        attrLen = pRecord->aData[0] + 1;
#if gZclEnableLongStringTypes_c          
      else if (isLongStrType)
        attrLen = OTA2Native16(*(uint16_t*)(pRecord->aData)) + 2;
#endif      
       
      /* invalid data type */
      if (pAttrDef->type != pRecord->attrType)
        pRspRecord->status = gZclInvalidDataType_c;
      
      /* read only */
      else if (ZclAttrIsReadOnly(pAttrDef->flags) && !forceWritingROAttr)
        pRspRecord->status = gZclReadOnly_c;
      /* Defined Out Of Band */      
      
      else if (ZclAttrIsDefinedOutOfBand(pAttrDef->flags))
        pRspRecord->status = gZclDefinedOutOfBand_c;
#if gZclEnableLongStringTypes_c    
      else if((isStrType || isLongStrType) && attrLen > pAttrDef->size)
        pRspRecord->status = gZclInvalidValue_c;
#else
      else if(isStrType && attrLen > pAttrDef->size)
        pRspRecord->status = gZclInvalidValue_c;      
#endif
      /* value is out of range */
#if gAddValidationFuncPtrToClusterDef_c
      else if (!bValueIsValid)
        pRspRecord->status = gZclInvalidValue_c;  
#endif
      /* write to the attribute */
      else {

        /* determine where to put the thing */  
        if (partitionedWriteAttr)      
          pDstAttrData = ZCL_GetPartitionAttributePtr(pRecord->attrId, partitionedClusterId, 
                                        pFrame->transactionId, &attrLen);
        else if(ZclAttrIsInstance(pAttrDef->flags))
	{
          if (ZclAttrIsInSceneData(pAttrDef->flags))
            pDstAttrData = &((uint8_t *)pDevice->pSceneData)[pCluster->dataOffset + pAttrDef->data.offset];
          else
            pDstAttrData = &((uint8_t *)pDevice->pData)[pCluster->dataOffset + pAttrDef->data.offset];
        }
	else
          pDstAttrData = pAttrDef->data.pData;
        /* special case: writing to identify is also a command */
        if(fIsIdentifyCluster) {
          ZCL_SetIdentifyMode(endPoint, (*((uint16_t *)(pRecord->aData))));
        }

        /* copy the attribute data from indication to true location */
        else {
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)        
          if(fIsPollControlCluster){
            /* special case: poll control cluster;  the Server returns Success Status but the attribute will be updated only if pass some conditions */
            ZclPollControl_WriteAttribute(endPoint, pRecord);
          }
          else{
#endif
          FLib_MemCpy(pDstAttrData, pRecord->aData, attrLen);

          /* should check for reporting here */
#if gZclEnableReporting_c
#endif
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)
          }
#endif 
        }
        /* worked */
        pRspRecord->status = gZbSuccess_c;
      }
    }

    /* add in size of identifier (attrLen now means size of this write request record) */
    attrLen = 1 + sizeof(zclAttrId_t) + ZCL_GetDataTypeLen(pRecord->attrType, pRecord->aData, pAttrDef);

    /* point to next write request record */
    pRecord = (zclCmdWriteAttrRecord_t*)( ( (uint8_t *)pRecord ) + attrLen );
    
    if(attrLen >= asduLen)
      asduLen = 0;
    else
      asduLen -= attrLen;

    /* point to next output record , if there is an error*/
    if(pRspRecord->status != gZbSuccess_c) {
      ++pRspRecord;       /* include attr IDs for failed ones */
      iRspLen += sizeof(zclCmdWriteAttrRspRecord_t);
    }
  }

  /*If all write attribute records were a success then only return a success status
  we check whether the length has been increased in order to determine whether response
  records with error codes have been added.
  */
  if (iRspLen == sizeof(zclFrame_t))
  { /* add size of status */
    iRspLen +=  sizeof (uint8_t);
  }
  return iRspLen;
}

STATIC uint8_t ZCL_BuildWriteAttrResponse
  (
  afDeviceDef_t  *pDevice,    /* IN: data to be acted on by this cluster */
  afClusterDef_t *pCluster,   /* IN: cluster definition to write attrs */
  uint8_t asduLen,            /* IN: length of input frame */
  zclFrame_t *pFrame,         /* IN: input frame */
  zclFrame_t *pFrameRsp       /* IN/OUT: response to write attribute */
  
  ) 
{
  return ZCL_BuildWriteAttrResponseWithPartition(pDevice, pCluster, asduLen, pFrame, pFrameRsp, FALSE, FALSE);
}
#endif


/*******************************************************************************
* ZCL_DiscoverAttrReq
* 
* Send a discover attributes request command to another node.
*******************************************************************************/
#if gZclDiscoverAttrReq_d
zclStatus_t ZCL_DiscoverAttrReq
  (
    zclDiscoverAttrReq_t *pReq
  )
{
  uint8_t iPayloadLen;
  afToApsdeMessage_t *pMsg;
  
  /* create the frame and copy in payload */
  iPayloadLen = sizeof(zclCmdDiscoverAttr_t);
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),gZclCmdDiscoverAttr_c, gZclFrameControl_FrameTypeGeneral,
    NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZbNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}
#endif

/*******************************************************************************
* ZCL_DiscoverAttrExtendedReq
* 
* Send a discover attributes request command to another node.
*******************************************************************************/
#if gZclDiscoverAttrExtendedReq_d
zclStatus_t ZCL_DiscoverAttrExtendedReq
  (
    zclDiscoverAttrExtendedReq_t *pReq
  )
{
  uint8_t iPayloadLen;
  afToApsdeMessage_t *pMsg;
  uint8_t direction = (pReq->direction==0)?gZclFrameControl_FrameTypeGeneral:gZclFrameControl_DirectionRsp;

  /* create the frame and copy in payload */
  iPayloadLen = sizeof(zclCmdDiscoverAttr_t);
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),gZclCmdDiscoverAttrExtended_c,gZclFrameControl_FrameTypeGeneral|direction,
    NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZbNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}
#endif


/*******************************************************************************
* ZCL_DiscoverCommandsReq
* 
* Send a discover commands(received/generated) request command to another node.
*******************************************************************************/
#if gZclDiscoverCommandsReg_d
zclStatus_t ZCL_DiscoverCommandsReq
  (
    zclDiscoverCommandsReq_t *pReq
  )
{
  uint8_t iPayloadLen;
  afToApsdeMessage_t *pMsg;
  uint8_t direction = (pReq->direction==0)?gZclFrameControl_FrameTypeGeneral:gZclFrameControl_DirectionRsp;

  /* create the frame and copy in payload */
  iPayloadLen = sizeof(zclCmdDiscoverCommands_t);
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),pReq->commandId, gZclFrameControl_FrameTypeGeneral|direction,
    NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZbNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}
#endif

/*******************************************************************************
* ZCL_DiscoverAttrDirectedReq
* 
* Send a discover attributes request command to another client or server node.
*******************************************************************************/
#if gZclDiscoverAttrReq_d
zclStatus_t ZCL_DiscoverAttrDirectedReq
  (
    zclDiscoverAttrDirectedReq_t *pReq
  )
{
  uint8_t iPayloadLen;
  afToApsdeMessage_t *pMsg;
  zclFrameControl_t frameCtrl = gZclFrameControl_FrameTypeGeneral;
  
  if (pReq->direction)
    frameCtrl |= gZclFrameControl_DirectionRsp;

  /* create the frame and copy in payload */
  iPayloadLen = sizeof(zclCmdDiscoverAttr_t);
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),gZclCmdDiscoverAttr_c, frameCtrl,
    NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZbNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}
#endif


/*******************************************************************************
* ZCL_BuildDiscoverAttrResponse
*
* Build a response to the discover attribute(s) command. 
*
* Returns total length of payload
*******************************************************************************/
STATIC uint8_t ZCL_BuildDiscoverAttrResponse
  (
  afClusterDef_t *pCluster,   /* IN: cluster definition */
  zclFrame_t *pFrame,         /* IN: input frame */
  uint8_t maxAsduLen,         /* IN: maximum ASDU length */ 
  zclFrame_t *pFrameRsp       /* IN/OUT: response to discover attributes */
  )
{
  zclCmdDiscoverAttr_t *pDiscoverReq;         /* input request */
  zclCmdDiscoverAttrRsp_t *pDiscoverRsp;      /* output request */
  zclCmdDiscoverAttrRspRecord_t *pRspRecord;  
 
  uint8_t iRspLen;
  zclAttrSet_t const *pAttrSet;
  zclAttrSetList_t  *pAttrSetList;
  uint8_t maxIds, direction; 
  index_t i, j, k;
   
  /* build response frame */
  pFrameRsp->command = gZclCmdDiscoverAttrRsp_c;
  pDiscoverRsp = (void *)(pFrameRsp + 1);
  /* initialize discovery complete attribute */  
  pDiscoverRsp->header.discoveryComplete = zclDiscoverAttrNotComplete;
  /* pointer to the response records */  
  pRspRecord = pDiscoverRsp->aRsp;
  iRspLen = sizeof(zclFrame_t) + sizeof(zclCmdDiscoverAttrRspHeader_t);

  pDiscoverReq = (void *)(pFrame + 1);
  maxIds = pDiscoverReq->maxAttrIds;
  direction = pFrame->frameControl & gZclFrameControl_DirectionRsp;

  /* Pointer to attribute set list on the cluster */  
  pAttrSetList = pCluster->pAttrSetList;

  if(pAttrSetList && maxIds != 0) 
  {
    pAttrSet = pAttrSetList->pAttrSet;
    /* Search for the attribute set */
    for(i = 0; (i < pAttrSetList->count) && (maxIds > 0); ++i)
    {
      zclAttrSet_t const *pCurrentAttrSet = &(pAttrSet[i]);
      
      /* Search for the attribute */
      for(j = 0; (j < pCurrentAttrSet->size) && (maxIds > 0); ++j)
      {
        zclAttrDef_t const *pCurrentAttrDef = &(pCurrentAttrSet->pAttrDefList[j]);
        uint16_t attrId = (pCurrentAttrSet->id << 8) +  pCurrentAttrDef->id;
        
         if ((ZclAttrIsOnClient(pCurrentAttrDef->flags) && !direction) ||
            (!ZclAttrIsOnClient(pCurrentAttrDef->flags) && direction))
          continue;
        
        if (!ZclAttrIsVector(pCurrentAttrDef->flags))
        {
          if(attrId >= pDiscoverReq->startAttr)
          {
            /* attribute id and type are written to the response */
            pRspRecord->attrId = attrId;
            pRspRecord->attrType = pCurrentAttrDef->type;
            
            /* go to the next response record */
            pRspRecord++;
            iRspLen += sizeof(zclCmdDiscoverAttrRspRecord_t);
            
            if(iRspLen + sizeof(zclCmdDiscoverAttrRspRecord_t) > maxAsduLen - sizeof(zclFrame_t))
              return iRspLen;
        
            /* no more than maxIds attributes are written */
            if (--maxIds == 0)
              break;
          }
        }
        else
          if((attrId + pCurrentAttrDef->size) >= pDiscoverReq->startAttr)
          {
            uint8_t noAttr, startOffset = 0;
            
            if (pDiscoverReq->startAttr > attrId)                       
              startOffset = pDiscoverReq->startAttr - (pCurrentAttrSet->id << 8);
            noAttr = FLib_GetMin(pCurrentAttrDef->size - startOffset + 1, maxIds);
            
            for (k = 0; k < noAttr; k++)  
            {
              /* attribute id and type are written to the response */
              pRspRecord->attrId = attrId + k + startOffset;
              pRspRecord->attrType = pCurrentAttrDef->type;
              /* go to the next response record */
              pRspRecord++;
              iRspLen +=  sizeof(zclCmdDiscoverAttrRspRecord_t);
              maxIds -= 1; 
              if(iRspLen + 3 > maxAsduLen - sizeof(zclFrame_t))
                return iRspLen;
            }
           
          }
      }
    }
 }
 
  pDiscoverRsp->header.discoveryComplete = zclDiscoverAttrComplete;
  return iRspLen;
}
/*******************************************************************************
* ZCL_BuildDiscoverAttrExtendedResponse
*
* Build a response to the discover attribute(s) extended command. 
*
* Returns total length of payload
*******************************************************************************/
STATIC uint8_t ZCL_BuildDiscoverAttrExtendedResponse
  (
  afAddrInfo_t *pAddrInfo,    /* IN: address info */ 
  afClusterDef_t *pCluster,   /* IN: cluster definition */
  zclFrame_t *pFrame,         /* IN: input frame */
  zclFrame_t *pFrameRsp       /* IN/OUT: response to discover attributes */
  )
{
  zclCmdDiscoverAttr_t *pDiscoverReq;                       /* input request */
  zclCmdDiscoverAttrExtendedRsp_t       *pDiscoverRsp;      /* output request */
  zclCmdDiscoverAttrExtendedRspRecord_t *pRspRecord;  
 
  uint8_t iRspLen;
  zclAttrSet_t const *pAttrSet = NULL;
  zclAttrSetList_t  *pAttrSetList;
  uint8_t maxIds, direction; 
  index_t i, j, k;
  uint16_t totalNoOfAttr = 0;

  zbApsdeDataReq_t dataReq;
  uint8_t maxPayload;
  zclAttrId_t startAttr;
  /* get max payload */  
  FLib_MemCpy(&dataReq, pAddrInfo, sizeof(zbAddrMode_t)+ sizeof(zbApsAddr_t));
  dataReq.txOptions = pAddrInfo->txOptions;
  
  maxPayload = ApsmeGetMaxAsduLength(&dataReq);
  
  /* build response frame */
  pFrameRsp->command = gZclCmdDiscoverAttrExtendedRsp_c;
  pDiscoverRsp = (void *)(pFrameRsp + 1);
  /* initialize discovery complete attribute */  
  pDiscoverRsp->header.discoveryComplete = zclDiscoverAttrNotComplete;
  /* pointer to the response records */  
  pRspRecord = pDiscoverRsp->aRsp;
  iRspLen = sizeof(zclFrame_t) + sizeof(zclCmdDiscoverAttrRspHeader_t);

  pDiscoverReq = (void *)(pFrame + 1);
  maxIds = pDiscoverReq->maxAttrIds;
  direction = pFrame->frameControl & gZclFrameControl_DirectionRsp;
  startAttr = OTA2Native16(pDiscoverReq->startAttr);

  /* Pointer to attribute set list on the cluster */  
  pAttrSetList = pCluster->pAttrSetList;

  if(pAttrSetList && maxIds != 0) 
  {
    pAttrSet = pAttrSetList->pAttrSet;
    /* Search for the attribute set */
    for(i = 0; (i < pAttrSetList->count) && (maxIds > 0); ++i)
    {
      zclAttrSet_t const *pCurrentAttrSet = &(pAttrSet[i]);
      
      /* Search for the attribute */
      for(j = 0; (j < pCurrentAttrSet->size) && (maxIds > 0); ++j)
      {
        zclAttrDef_t const *pCurrentAttrDef = &(pCurrentAttrSet->pAttrDefList[j]);
        uint16_t attrId = (pCurrentAttrSet->id << 8) +  pCurrentAttrDef->id;
        
         if ((ZclAttrIsOnClient(pCurrentAttrDef->flags) && !direction) ||
            (!ZclAttrIsOnClient(pCurrentAttrDef->flags) && direction))
          continue;
        
        if (!ZclAttrIsVector(pCurrentAttrDef->flags))
        {
          if(attrId >= startAttr)
          {
            /* attribute id and type are written to the response */
            pRspRecord->attrId = Native2OTA16(attrId);
            pRspRecord->attrType = pCurrentAttrDef->type;
            /* check the attribute flags */
            pRspRecord->attrAccessCtrl = 0;
            if(!ZclAttrIsWriteOnly(pCurrentAttrDef->flags))
              pRspRecord->attrAccessCtrl |= gZclAttrReadable_c;
            if(!ZclAttrIsReadOnly(pCurrentAttrDef->flags))
              pRspRecord->attrAccessCtrl|= gZclAttrWriteable_c;
            if(ZclAttrIsReportable(pCurrentAttrDef->flags))
              pRspRecord->attrAccessCtrl|= gZclAttrReportable_c;            
            
            if(iRspLen + sizeof(zclCmdDiscoverAttrExtendedRspRecord_t)>maxPayload)
              return iRspLen;
            
            /* go to the next response record */
            pRspRecord++;
            iRspLen += sizeof(zclCmdDiscoverAttrExtendedRspRecord_t);
            
        
            /* no more than maxIds attributes are written */
            if (--maxIds == 0)
              break;
          }
        }
        else
          if((attrId + pCurrentAttrDef->size) >= startAttr)
          {
            uint16_t noAttr, startOffset = 0;
            
            if (startAttr > attrId)                       
              startOffset = startAttr - (((zclAttrId_t)pCurrentAttrSet->id) << 8);
            noAttr = FLib_GetMin(pCurrentAttrDef->size - startOffset + 1, maxIds);
            
            for (k = 0; k < noAttr; k++)  
            {
              /* attribute id and type are written to the response */
              pRspRecord->attrId = attrId + k + startOffset;
              pRspRecord->attrType = pCurrentAttrDef->type;
              /* check the attribute flags  - should be tested for vector flags*/ 
              if(!ZclAttrIsWriteOnly(pCurrentAttrDef->flags))
                pRspRecord->attrAccessCtrl |= gZclAttrReadable_c;
              if(!ZclAttrIsReadOnly(pCurrentAttrDef->flags))
                pRspRecord->attrAccessCtrl|= gZclAttrWriteable_c;
              if(ZclAttrIsReportable(pCurrentAttrDef->flags))
                pRspRecord->attrAccessCtrl|= gZclAttrReportable_c;                

              if(iRspLen + sizeof(zclCmdDiscoverAttrExtendedRspRecord_t)>maxPayload)
                return iRspLen;
              
              /* go to the next response record */
              pRspRecord++;      
              iRspLen +=  sizeof(zclCmdDiscoverAttrRspRecord_t);
              
              /* no more than maxIds attributes are written */
              if (--maxIds == 0)
                break;

            }
           
          }
      }
    }
 }
  if(pAttrSetList)
  {
    for(j = 0; j < pAttrSetList->count; j++)
    {
      zclAttrSet_t const *pCurrentAttrInfSet = &(pAttrSet[j]);
      for(i = 0; i < pCurrentAttrInfSet->size; i++) 
      {
         zclAttrDef_t const *pCurrentAttrDef = &(pCurrentAttrInfSet->pAttrDefList[j]);
        if ((ZclAttrIsOnClient(pCurrentAttrDef->flags) && !direction) ||
            (!ZclAttrIsOnClient(pCurrentAttrDef->flags) && direction))
          continue; 
        totalNoOfAttr ++; 
      }
    }
    if(pDiscoverReq->maxAttrIds >= totalNoOfAttr)
      pDiscoverRsp->header.discoveryComplete = zclDiscoverAttrComplete;
  }
  else
    pDiscoverRsp->header.discoveryComplete = zclDiscoverAttrComplete;
  return iRspLen;
}


/*******************************************************************************
* ZCL_BuildDiscoverCommandsResponse
*
* Build a response to the discover commands(s) received/generated command. 
*
* Returns total length of payload
*******************************************************************************/
STATIC uint8_t ZCL_BuildDiscoverCommandsResponse
  (
  afAddrInfo_t *pAddrInfo,    /* IN: address info */ 
  afClusterDef_t *pCluster,   /* IN: cluster definition */
  zclFrame_t *pFrame,         /* IN: input frame */
  zclFrame_t *pFrameRsp       /* IN/OUT: response to discover attributes */
  )
{
  zclCmdDiscoverCommands_t          *pDiscoverReq;  /* input request */
  zclCmdDiscoverCommandsRsp_t       *pDiscoverRsp;  /* output request */
  
  uint8_t iRspLen = 0;
  zclCmd_t const *pCmdDef;
  zclCommandsDefList_t  *pCmdList;
  
  uint8_t maxIds, flag; 
  uint16_t cmdCount;
  zbApsdeDataReq_t dataReq;
  uint8_t maxPayload;
  uint16_t i = 0, j;
  
  /* get max payload */  
  FLib_MemCpy(&dataReq, pAddrInfo, sizeof(zbAddrMode_t)+ sizeof(zbApsAddr_t));
  dataReq.txOptions = pAddrInfo->txOptions;
  
  maxPayload = ApsmeGetMaxAsduLength(&dataReq);
  
  /* build response frame */
  pFrameRsp->command = (pFrame->command == gZclCmdDiscoverCmdReceived_c)?gZclCmdDiscoverCmdReceivedRsp_c:gZclCmdDiscoverCmdGeneratedRsp_c;
  pDiscoverRsp = (void *)(pFrameRsp + 1);
  
  /* initialize discovery complete attribute */  
  pDiscoverRsp->discoveryComplete = zclDiscoverAttrNotComplete;
  
  /* pointer to the response records */  
  iRspLen = sizeof(zclFrame_t) + 1;

  pDiscoverReq = (void *)(pFrame + 1);
  maxIds = pDiscoverReq->maxCmdIds;
  if(pFrameRsp->command == gZclCmdDiscoverCmdGeneratedRsp_c)
    flag = (pFrame->frameControl & gZclFrameControl_DirectionRsp)? gZclCmdReceived_c :gZclCmdGenerated_c;
  else
    flag = (pFrame->frameControl & gZclFrameControl_DirectionRsp)? gZclCmdGenerated_c : gZclCmdReceived_c;  
  
  /* pointer to attribute list on the cluster */  
  pCmdList = pCluster->pCommandsDefList;
  
  if(!(pFrame->frameControl & gZclFrameControl_DirectionRsp))
  {
    if(!pCluster->pfnServerIndication)
    {
      pDiscoverRsp->discoveryComplete = zclDiscoverAttrComplete;
      return iRspLen;
    }
  }
  else  
  {
    if(!pCluster->pfnClientIndication)
    {
      pDiscoverRsp->discoveryComplete = zclDiscoverAttrComplete;
      return iRspLen;
    }
  }
 
  if(pCmdList && maxIds != 0) 
  {
    pCmdDef = (flag==gZclCmdReceived_c)?(pCmdList->pCommandReceivedDef):(pCmdList->pCommandGeneratedDef);

    cmdCount = (flag==gZclCmdReceived_c)?(pCmdList->countReceived):(pCmdList->countGenerated);
    /* go through all attributes */
    for(j=0;j<cmdCount; j++)
    {
    
      if (pCmdDef[j] >= pDiscoverReq->startCmd) 
      {
        if (j == cmdCount-1)
          pDiscoverRsp->discoveryComplete = zclDiscoverAttrComplete;
        
        pDiscoverRsp->cmdId[i] = pCmdDef[j];
        i++;
       
        if(iRspLen + 1 >maxPayload)         
          break;
     
        iRspLen += 1;
    
        /* no more than maxIds commands are written */
        if (--maxIds == 0)
            break;
      }
    }
   }
  else
      pDiscoverRsp->discoveryComplete = zclDiscoverAttrComplete;
  
  return iRspLen;
}


#if gZclEnableReporting_c

/*******************************************************************************
  ZCL_ReportingTimeout

  Time to report all reporting attributes.

*******************************************************************************/

/********************************************************************************
* 1 second timeout handler for reports
********************************************************************************/
void ZCL_ReportingTimeout
  (
  zbTmrTimerID_t iTimerId
  )
{
  uint8_t i = 0, j = 0;
  uint8_t event = 0xFF;
  
  (void)iTimerId;
 
  /* check endpoint list */
  #if gInstantiableStackEnabled_d
  for(j=0; j<EndPointConfigData(gNum_EndPoints); j++)
  #else
  for(j=0; j<gNum_EndPoints_c; j++)
  #endif
  {
    afDeviceDef_t *pDevice;
    zclReportingSetup_t *zclReportingSetup;
    
    /* verify dst endpoint has device data, and is for ZCL */
    pDevice = AF_GetEndPointDevice(EndPointConfigData(endPointList[j].pEndpointDesc->pSimpleDesc->endPoint));
    /* get reporting Setup information */        
    zclReportingSetup = &((zclReportingSetup_t *)pDevice->pReportSetup)[0];  
    
    if(zclReportingSetup == NULL)
      continue;
    
    /* Count down second counter (both min and max) */
    for(i=0; i<pDevice->reportCount; i++)
    {
      if(!(zclReportingSetup[i].sendReportMask&gZCLReportingAttrValueEnabled_c))
        continue;
    
      if(zclReportingSetup[i].reportCounter)
        --zclReportingSetup[i].reportCounter;
      
      if (zclReportingSetup[i].reportMin) 
      {       
        if(zclReportingSetup[i].minTimeCounter)
          --zclReportingSetup[i].minTimeCounter;
    
        if ((0 == zclReportingSetup[i].minTimeCounter) && (zclReportingSetup[i].sendReportMask & gZCLReportingAttrValueChanged_c))     
        {
          event = gZclEventReportTimeout_c;
        }
      }
      if(zclReportingSetup[i].reportCounter == 0)
      {
        zclReportingSetup[i].sendReportMask |= gZCLReportingAttrTimeout_c;
        event = gZclEventReportTimeout_c;
      }
    }
  }
  if(event == gZclEventReportTimeout_c )     
  {  
    TS_SendEvent(gZclTaskId, event);
    return;
  }
  else 
  {
      TS_SendEvent(gZclTaskId, gZclEventReportTick_c);  
  }    
}

/********************************************************************************
* Prepare the system for sending a timed response
********************************************************************************/
void ZCL_StartReportingTimer(void)
{
  ZbZclFoundationGlobals(gfZclReportRestart) = TRUE;
      

  /*Start a 1 second timer*/
  ZbTMR_StartTimer(ZbZclFoundationGlobals(gZclReportingTimerID), gTmrLowPowerSingleShotMillisTimer_c,
      (zbTmrTimeInMilliseconds_t)1000,  
      ZCL_ReportingTimeout);
  
}


/*******************************************************************************
* ZCL_BuildConfigureReportingResponse
*
* Build a response to the configure attribute(s) command. Works across clusters and 
* understands multiple instances of an endpoint.
*
* Returns total length of payload
*******************************************************************************/
STATIC uint8_t ZCL_BuildConfigureReportingResponse
  (
  afDeviceDef_t *pDevice,     /* IN: data to be acted on by this cluster */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint8_t asduLen,            /* IN: length of input frame */
  zclFrame_t *pFrame,         /* IN: read attr request */
  zclFrame_t *pFrameRsp       /* IN/OUT: response frame, allocated above, filled in here... */
  )
{
  uint8_t *pData = pDevice->pData; /* pointer to data for the device */
  uint8_t  count;              
  uint8_t rspLen;             /* length of response frame */
  uint8_t  cFailures = 0;      /* count # of failures. affects return */
  uint16_t recordLen, recordsLen = 0;          /* length of record */
  uint8_t attrLen;            /* length of the attribute */
  zbStatus_t status;

  uint8_t     *pCfgRecordReq; 
  zclCmdReportCfgRsp_t *pRspRecord;
  zclAttrDef_t *pAttrDef;     /* pointer to attribute definition */
  uint8_t *pAttrData;
  bool_t  fReportAttr, fStartReportingTimer = FALSE;
  uint8_t iReportIndex;
  uint8_t direction;
  uint16_t attrId;

  /* respond to config reporting command */
  pFrameRsp->command = gZclCmdCfgReportingRsp_c;

  /* prep for parsing input frame */
  pCfgRecordReq =   (void *)(pFrame + 1);  /* input records (req from ota) */
  pRspRecord = (void *)(pFrameRsp + 1); /* output records (rsp to ota) */
  rspLen = sizeof(zclFrame_t);
  asduLen -= sizeof(zclFrame_t);

  /* initialize response status */
  pRspRecord->status = gZbSuccess_c;

  /* parse input frame */
  count = 0;
  while(asduLen) {

    zclCmdReportingCfgServerRecord_t *pCfgRecord;
     /* on to next response record */
    pCfgRecord = (void *)&pCfgRecordReq[recordsLen];
    
    /* assume successful */
    status = gZbSuccess_c;

    /* skip config client records, nothing to do on direction 0x01! */
    recordLen = sizeof(zclCmdReportingCfgClientRecord_t);
    direction = pCfgRecord->direction;
    attrId = pCfgRecord->attrId;
    if(!direction)
    /* direction 0x00 */
    /* config server record (tell a node to report it's attribute when it changes) */
    {

      /* record length */
      recordLen = sizeof(zclCmdReportingCfgServerRecord_t) - 
        MbrSizeof(zclCmdReportingCfgServerRecord_t, aReportableChange);

      /* if partial record, then give up */
      if(asduLen < recordLen)
        status = gZclInvalidValue_c;

      else {

        /* does attribute exist? */
        pAttrDef = ZCL_FindAttr(pCluster, attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp);

        /* if so, get the attribute ptr and it's size */
        attrLen = 1;
        if(pAttrDef)
        {
          pAttrData = ZCL_GetAttributePtr(pDevice, pCluster, attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp, &attrLen);
          /* For attributes of 'discrete' data type, ReportableChange is omitted.
          [R2] - 2.4.7.1.7 Reportable Change Field */
          if(!ZCL_IsDataTypeDiscrete(pCfgRecord->attrType))
            recordLen += attrLen;
        }
        else
        {
          /* For attributes of 'discrete' data type, ReportableChange is omitted.
          [R2] - 2.4.7.1.7 Reportable Change Field */
          if(!ZCL_IsDataTypeDiscrete(pCfgRecord->attrType))
            recordLen += ZCL_GetDataTypeLen(pCfgRecord->attrType,pCfgRecord->aReportableChange, pAttrDef);
        }

        /* attribute doesn't exist */
        if(!pAttrDef)
          status = gZclUnsupportedAttribute_c;

#if gZclEnableReporting_c
        /* attribute exists, but is not reportable */
        else if(!ZCL_IsReportableAttr(pDevice, pCluster, attrId, &iReportIndex))
          status = gZclUnreportableAttribute_c;
#endif
        else if(pCfgRecord->attrType != pAttrDef->type)
          status = gZclInvalidDataType_c;
        
        else if(iReportIndex >= pDevice->reportCount)
          status = gZclInsufficientSpace_c;
      }

      /* found a reportable attribute, set it */
      if(status == gZbSuccess_c)
      {
         zclReportingSetup_t *zclReportingSetup;
        /* get reporting Setup information */        
        zclReportingSetup = &((zclReportingSetup_t *)pDevice->pReportSetup)[0];  
        
        /* assume we're turning off reporting for this attribute */
        fReportAttr = FALSE;

        /* Start reporting and set state */
        zclReportingSetup[iReportIndex].sendReportMask |= gZCLReportingAttrValueEnabled_c;
        
        /* set up reporting timeout */
        zclReportingSetup[iReportIndex].reportTimeout = OTA2Native16(pCfgRecord->maxReportingInterval);
        zclReportingSetup[iReportIndex].reportMin = OTA2Native16(pCfgRecord->minReportingInterval);

        /*if Max reporting Interval == 0xFFFF then disable interval reporting*/
        if (zclReportingSetup[iReportIndex].reportTimeout == 0xFFFF) {
          ZbZclFoundationGlobals(gfZclReportRestart) = TRUE;
          fReportAttr = TRUE;  
          zclReportingSetup[iReportIndex].reportTimeout = 0;        
        }
         
        /* no need for timer anymore */
        if(!zclReportingSetup[iReportIndex].reportTimeout) 
        {
          uint8_t i = 0, j = 0;
          uint8_t reportingIndex = 0;
          uint8_t maxReportingData = 0;
          
          zclReportingSetup[iReportIndex].sendReportMask = 0x00;
          #if gInstantiableStackEnabled_d
          for(j=0;j<EndPointConfigData(gNum_EndPoints); j++)
          #else  
          for(j=0;j<gNum_EndPoints_c; j++)
          #endif
          {
            afDeviceDef_t *pCurrentDevice;
            
            /* verify dst endpoint has device data, and is for ZCL */
            pCurrentDevice = AF_GetEndPointDevice(EndPointConfigData(endPointList[j].pEndpointDesc->pSimpleDesc->endPoint));
            
            /* get current reporting Setup information */        
            zclReportingSetup = &((zclReportingSetup_t *)pDevice->pReportSetup)[0];  
            
            for(i=0;i<pCurrentDevice->reportCount; i++)
            {
              if(!(zclReportingSetup[i].sendReportMask & gZCLReportingAttrValueEnabled_c))
                continue;
              else
                break; 
            }
            reportingIndex += i; 
            maxReportingData += pCurrentDevice->reportCount;
          }
          if(reportingIndex == maxReportingData)      
            ZbTMR_StopTimer(ZbZclFoundationGlobals(gZclReportingTimerID));
        }

        /* setting up timer for reporting (will start the timer state machine) */
        else {

          fReportAttr = TRUE;
          /*Initialize the counter with the number of seconds we want to report*/
          zclReportingSetup[iReportIndex].reportCounter = zclReportingSetup[iReportIndex].reportTimeout;
          fStartReportingTimer = TRUE;
        }

        /* copy current value of attribute and copy change in value */
        FLib_MemCpy(&pAttrData[attrLen], pAttrData, attrLen);
        /* For attributes of 'discrete' data type, ReportableChange is omitted.
          [R2] - 2.4.7.1.7 Reportable Change Field */
        if(!ZCL_IsDataTypeDiscrete(pCfgRecord->attrType))
        {
          FLib_MemCpy(&pAttrData[attrLen << 1], pCfgRecord->aReportableChange, attrLen);

          /* setting this to be reportable or not? */
          if(!BeeUtilArrayIsFilledWith(pCfgRecord->aReportableChange, 0, attrLen))
            fReportAttr = TRUE;
        }
        else
          fReportAttr = TRUE;

        /* set or clear the bit as requested */
        if((fReportAttr)&&(zclReportingSetup[iReportIndex].sendReportMask))
          BeeUtilSetIndexedBit(pData, iReportIndex);
        else
          (void)BeeUtilClearIndexedBit(pData, iReportIndex);


        /* save app data into nvm */
        ZCL_SaveNvmZclData();
      } /* end of if success */

    }
    else
    {
      uint16_t timeout;
      FLib_MemCpy(&timeout, &pCfgRecord->attrType, sizeof(uint16_t));
      status = ZCL_ReportingClientSetup(pCluster->aClusterId, attrId, OTA2Native16(timeout));
    }

    /* only record failures in rsp packet */
    if(status != gZbSuccess_c) {
      zclCmdReportCfgRsp_t* pRspCurrentRecord;
      ++cFailures;

      /* on to next response record */
      pRspCurrentRecord = &pRspRecord[count];
      
      
      pRspCurrentRecord->status = status;
      pRspCurrentRecord->direction = direction;
      pRspCurrentRecord->attrId = Native2OTA16(attrId);
      ++count;
      rspLen += sizeof(*pRspRecord);
    }

    recordsLen+= recordLen;
    if(asduLen <= recordLen)
      asduLen = 0;
    else
      asduLen -= recordLen;

  } /* end of while loop */

  if(fStartReportingTimer == TRUE)
     ZCL_StartReportingTimer();
  
  /* no failures, simple response is all that is needed */
  if(!cFailures)
    rspLen = sizeof(zclFrame_t)+1;

  return rspLen;
}

/*******************************************************************************
* ZCL_ReportingClientSetup
*
* used to update client configure reporting table
*******************************************************************************/
STATIC zbStatus_t ZCL_ReportingClientSetup(zbClusterId_t aClusterId, zclAttrId_t   attrId,  uint16_t  reportTimeout)
{
  uint8_t i = 0;
  
  for(i=0; i<gZClMaxAttrReportingReceivedTable_c; i++)
  {
     if(FLib_MemCmp(ZbZclFoundationGlobals(gZclReportingClientSetup[i].aClusterId), aClusterId, sizeof(zbClusterId_t))&&
           (ZbZclFoundationGlobals(gZclReportingClientSetup[i].attrId) == attrId)&&(ZbZclFoundationGlobals(gZclReportingClientSetup[i].aState)))
     {
        ZbZclFoundationGlobals(gZclReportingClientSetup[i].aState) = TRUE;
        ZbZclFoundationGlobals(gZclReportingClientSetup[i].reportTimeout) = reportTimeout;
        return gZbSuccess_c;
     }
  }
  for(i=0; i<gZClMaxAttrReportingReceivedTable_c; i++)
  { 
    if(ZbZclFoundationGlobals(gZclReportingClientSetup[i].aState) == FALSE)
    {
      ZbZclFoundationGlobals(gZclReportingClientSetup[i].aState) = TRUE;
      ZbZclFoundationGlobals(gZclReportingClientSetup[i].attrId) = attrId;
      FLib_MemCpy(ZbZclFoundationGlobals(gZclReportingClientSetup[i].aClusterId), aClusterId, sizeof(zbClusterId_t));
      ZbZclFoundationGlobals(gZclReportingClientSetup[i].reportTimeout) = reportTimeout;
      return gZbSuccess_c; 
     }      
  }
  return gZclUnsupportedAttribute_c;
}

/*******************************************************************************
* ZCL_BuildReadReportingCfgResponse
*
* Build a response to the read reporting configuration command. Works across clusters and 
* understands multiple instances of an endpoint.
*
* Returns total length of payload
*******************************************************************************/
STATIC uint8_t ZCL_BuildReadReportingCfgResponse
  (
  afDeviceDef_t *pDevice,     /* IN: data to be acted on by this cluster */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint8_t asduLen,            /* IN: length of input frame */
  zclFrame_t *pFrame,         /* IN: read reporting cfg request */
  zclFrame_t *pFrameRsp       /* IN/OUT: response frame, allocated above, filled in here... */
  )
{
  uint8_t *pData = pDevice->pData; /* pointer to data for the device */        
  uint8_t rspLen;             /* length of response frame */
  uint8_t attrLen;            /* length of the attribute */
  zclCmdReadReportingCfgRecord_t    *pReqRecord;
  uint8_t *pRspRecord;
  zclAttrDef_t *pAttrDef;     /* pointer to attribute definition */
  uint8_t *pAttrData = NULL;
  uint8_t iReportIndex;

  /* respond to config reporting command */
  pFrameRsp->command = gZclCmdReadReportingCfgRsp_c;

  /* prep for parsing input frame */
  pReqRecord =   (void *)(pFrame + 1);  /* input records (req from ota) */
  pRspRecord =   (void *)(pFrameRsp + 1); /* output records (rsp to ota) */
  rspLen = sizeof(zclFrame_t);
  asduLen -= sizeof(zclFrame_t);
  
  while(asduLen) 
  {

    zclCmdReadReportingCfgRspRecord_t* pRspCurrentRecord;
     /* on to next response record */
    pRspCurrentRecord = (void *)&pRspRecord[rspLen - sizeof(zclFrame_t)];

    if(asduLen < sizeof(zclCmdReadReportingCfgRecord_t))
      return rspLen;
    
     /* assume successful */
    pRspCurrentRecord->status = gZbSuccess_c;
    pRspCurrentRecord->direction = pReqRecord->direction;
    pRspCurrentRecord->attrId = pReqRecord->attrId;
    
    rspLen += sizeof(zclAttrId_t) + 2;
    
    if(!pReqRecord->direction)
    {
       /* direction 0x00 */
      
      /* does attribute exist? */
      pAttrDef = ZCL_FindAttr(pCluster, pRspCurrentRecord->attrId , pFrame->frameControl & gZclFrameControl_DirectionRsp);
      /* if so, get the attribute ptr and it's size */
      attrLen = 1;
      if(pAttrDef)
      {    
#if gZclEnableReporting_c
        /* attribute exists, but is not reportable */
        if(!ZCL_IsReportableAttr(pDevice, pCluster, pRspCurrentRecord->attrId, &iReportIndex))
          pRspCurrentRecord->status  = gZclUnreportableAttribute_c;
#endif
        pAttrData = ZCL_GetAttributePtr(pDevice, pCluster, pRspCurrentRecord->attrId, pFrame->frameControl & gZclFrameControl_DirectionRsp, &attrLen);
      }
      else 
      {
        /* attribute doesn't exist */
        pRspCurrentRecord->status  = gZclUnsupportedAttribute_c;
      }
      if(pRspCurrentRecord->status == gZbSuccess_c)
      {
        pRspCurrentRecord->recordData.reported.attrDataType = pAttrDef->type;
        if(!BeeUtilGetIndexedBit(pData, iReportIndex))
        {
          /* specified attribute has not been configured */
          pRspCurrentRecord->recordData.reported.maxReportInterval = 0xFFFF;
          pRspCurrentRecord->recordData.reported.minReportInterval = 0xFFFF;
          /* For attributes of 'discrete' data type, ReportableChange field is omitted.
          [R2] - 2.4.10.1.7 Reportable Change Field */
          if(!ZCL_IsDataTypeDiscrete(pAttrDef->type))
          {
            ZCL_GetInvalidValue(pAttrDef->type, attrLen, &pRspCurrentRecord->recordData.reported.reportableChange[0]);
          }
        }
        else
        {
          zclReportingSetup_t *zclReportingSetup;
          zclReportingSetup = &((zclReportingSetup_t *)pDevice->pReportSetup)[iReportIndex];   
          /* get attribute report configuration */
          pRspCurrentRecord->recordData.reported.maxReportInterval = Native2OTA16(zclReportingSetup->reportTimeout);
          pRspCurrentRecord->recordData.reported.minReportInterval = Native2OTA16(zclReportingSetup->reportMin);
          /* For attributes of 'discrete' data type, ReportableChange field is omitted.
          [R2] - 2.4.10.1.7 Reportable Change Field */
          if(!ZCL_IsDataTypeDiscrete(pAttrDef->type))
            FLib_MemCpy(pRspCurrentRecord->recordData.reported.reportableChange, &pAttrData[attrLen << 1], attrLen);
        }
        /* update record len */
        if(ZCL_IsDataTypeDiscrete(pAttrDef->type))
          attrLen = 0;
        rspLen += sizeof(zclCmdReadReportCfg_Reported_t) -1 + attrLen;
      }
    }
    else
    {
      /* direction 0x01 */
      uint8_t i = 0;
      for(i=0; i<gZClMaxAttrReportingReceivedTable_c; i++)
      {
        if((ZbZclFoundationGlobals(gZclReportingClientSetup[i].aState))&&
           (FLib_MemCmp(ZbZclFoundationGlobals(gZclReportingClientSetup[i].aClusterId), pCluster->aClusterId, sizeof(zbClusterId_t)))&&
           (ZbZclFoundationGlobals(gZclReportingClientSetup[i].attrId) == pRspCurrentRecord->attrId))
          {
            pRspCurrentRecord->recordData.received.timeoutPeriod = Native2OTA16(ZbZclFoundationGlobals(gZclReportingClientSetup[i].reportTimeout));
            rspLen += sizeof(uint16_t);
            break;
          }
        if(i+1 == gZClMaxAttrReportingReceivedTable_c)
          {
            pRspCurrentRecord->status = gZclUnreportableAttribute_c;
          }
      }
    }
       
    asduLen -= sizeof(zclCmdReadReportingCfgRecord_t);
    /* on to next request record */
    pReqRecord = (zclCmdReadReportingCfgRecord_t *)(((uint8_t *)pReqRecord) + sizeof(zclCmdReadReportingCfgRecord_t));
   } 
  return rspLen;
}
#endif

/*******************************************************************************
* ZCL_FindCluster
*
* Given a cluster ID, find a pointer to the cluster structure.
*******************************************************************************/
afClusterDef_t *ZCL_FindCluster
  (
  afDeviceDef_t *pDevice,     /* IN: device definition */
  zbClusterId_t aClusterId    /* IN: cluster ID to find in that device definition */
  )
{
  afClusterDef_t *pClusterDef;
  index_t i;

  /* no cluster def, so no clusters */
  if(!pDevice)
    return NULL;

  /* look through cluster list */
  pClusterDef = pDevice->pClusterDef;
  if(!pClusterDef)
    return  NULL;   /* no cluster list */

  /* does the cluster ID match? return it */
  for(i=0; i < pDevice->clusterCount; ++i) {
    if(IsEqual2Bytes(pClusterDef[i].aClusterId, aClusterId))
      return &pClusterDef[i];
  }

  /* not found */
  return NULL;
}

#if gZclEnableReporting_c

/*******************************************************************************
* ZCL_IsReportableAttr
* 
* Find a reportable attribute given this cluster and attr ID.
* 
* Returns TRUE if this attribute is reportable, and returns the index into the
* reportMask. Returns FALSE if not reportable.
*******************************************************************************/
bool_t ZCL_IsReportableAttr
  (
  afDeviceDef_t  *pDevice,    /* IN: assumes pDevice is non-null */
  afClusterDef_t *pCluster,   /* IN: assume pCluster exists and is non-null */
  zclAttrId_t attrId,         /* IN: any attr ID */
  uint8_t *pIndex             /* OUT: index into reportMask on the endpoint data */
  )
{
  uint8_t iReportMaskIndex;
  zclReportAttr_t *pReportList;
  zclAttrDef_t *pAttrDef;

  /* make sure the attribute exists (and there is a report list) */
  pReportList = pDevice->pReportList;
  if(!pReportList)
    return FALSE; /* no reportable attributes */

  /* is this attribute reportable? */
  pAttrDef = ZCL_FindAttr(pCluster, attrId, 0);
  if(!pAttrDef || !ZclAttrIsReportable(pAttrDef->flags))
    return FALSE;

  /* determine report mask index (entry # in report table) */
  for(iReportMaskIndex=0; iReportMaskIndex < pDevice->reportCount; ++iReportMaskIndex) {
    zclReportAttr_t *pCurrentReport = &pReportList[iReportMaskIndex];
    /* is there a match? */
    if(IsEqual2Bytes(pCurrentReport->aClusterId, pCluster->aClusterId) &&
      IsEqual2Bytes(&pCurrentReport->attrId, &attrId)) {
      if(pIndex)
        *pIndex = iReportMaskIndex;
      
      return TRUE;
    }
  }
  return FALSE;
}

/*******************************************************************************
* ZCL_IsReportingAndChanged
* 
* Find a reportable attribute given this cluster and attr ID.
* 
* Returns TRUE if this attribute is reportable, and returns the index into the
* reportMask. Returns FALSE if not reportable.
*******************************************************************************/
bool_t ZCL_IsReportingAndChanged
  (
  afDeviceDef_t   *pDeviceDef, 
  afClusterDef_t  *pClusterDef, 
  zclAttrId_t     attrId,
  uint8_t         *pReportIndex
  )
{
  zclAttrDef_t *pAttrDef;
  zclReportingSetup_t *zclReportingSetup;

  bool_t fChanged;
  uint8_t *pAttrData;
  uint8_t attrLen;
  uint8_t len = 0;
  uint8_t endPoint = AF_DeviceDefToEndPoint(pDeviceDef);


  /* check if it's reportable */
  if(!ZCL_IsReportableAttr(pDeviceDef, pClusterDef, attrId, pReportIndex))
    return FALSE;

  /* check if it's reporting */
  if(!BeeUtilGetIndexedBit(pDeviceDef->pData, pReportIndex[0]))
    return FALSE;

  /* get reporting Setup information */        
  zclReportingSetup = &((zclReportingSetup_t *)pDeviceDef->pReportSetup)[0];  
  
  if(zclReportingSetup[pReportIndex[0]].sendReportMask & gZCLReportingAttrValueChanged_c)
    return FALSE;
  
  /* setup for range checking */
  fChanged = FALSE;

  /* get pointer to data */
  pAttrData = ZCL_GetAttributePtr(pDeviceDef, pClusterDef,  attrId, gZclServerAttr_c, &attrLen);

  /* check if it has changed enough */
  /* note: pAttrDef cannot fail at this point, becuase ZCL_IsReportableAttr() already validated this attr */
  pAttrDef = ZCL_FindAttr(pClusterDef, attrId, 0);
  
  len = ZCL_GetDataTypeLen(pAttrDef->type, pAttrData, pAttrDef);
  
  /* For attributes of 'discrete' data type, ReportableChange field is omitted.
  [R2] - 2.4.7.1.7 Reportable Change Field */
  if(!ZCL_IsDataTypeDiscrete(pAttrDef->type))
  {
#if (gZclEnableOver32BitAttrsReporting_c == FALSE)
    if (len > 4)
      len = 0;
#endif  
 
    if (len == 0)
      fChanged = !FLib_MemCmp(pAttrData, pAttrData + attrLen, attrLen);
    else
      fChanged = ZCL_CmpUint(pAttrData, len);

  }
  else
     fChanged = !FLib_MemCmp(pAttrData, pAttrData + attrLen, attrLen);
  /* changed, record new value */
  if(fChanged) {

    /* copy in old value from present value */
    FLib_MemCpy(pAttrData + attrLen, pAttrData, attrLen);
    return TRUE;  /* yes, it's changed */
  }

  return FALSE;
}
#endif

/*******************************************************************************
* ZCL_FindAttr
* 
* Look through the cluster's attribute list to find this attribute.
* All mult-byte fields are little-endian.
* 
* Returns pointer to the attribute, or NULL if not found.
*******************************************************************************/
zclAttrDef_t *ZCL_FindAttr
  (
  afClusterDef_t *pCluster,   /* IN: */
  zclAttrId_t attrId,         /* IN: */
  uint8_t direction           /* IN: 0x00 for Server Attributes and 0x08 for Client Attributes*/
  )
{
  zclAttrSetList_t  *pAttrSetList;
  zclAttrSet_t const *pAttrSet;
  index_t            i, j;

  
  attrId = OTA2Native16(attrId);
  /* Look through attribute sets list */
  pAttrSetList = pCluster->pAttrSetList;
  
  if(!pAttrSetList)
    return NULL;  /* No attribute sets in this cluster */

  pAttrSet = pAttrSetList->pAttrSet;
  for(i = 0; i < pAttrSetList->count; ++i)
  {
    zclAttrSet_t const *pCurrentAttrSet = &(pAttrSet[i]);
    /* Check the attribute set id*/
    if (((attrId >> 8) & 0xff) == pCurrentAttrSet->id)
    for(j = 0; j < pCurrentAttrSet->size; ++j)
    {
      /* Check the attribute id*/
      zclAttrDef_t const *pCurrentAttrDef = &(pCurrentAttrSet->pAttrDefList[j]);
      uint8_t id = attrId & 0xff;
      
      if ((ZclAttrIsOnClient(pCurrentAttrDef->flags) && !direction) ||
        (!ZclAttrIsOnClient(pCurrentAttrDef->flags) && direction))
        continue;
      
      if (id == pCurrentAttrDef->id) 
        return (zclAttrDef_t *)(pCurrentAttrDef);
      
      /* Check if the attribute id is in the defined interval*/
      if (ZclAttrIsVector(pCurrentAttrDef->flags) && 
          ((pCurrentAttrDef->id + pCurrentAttrDef->size) >=  id) &&
           (pCurrentAttrDef->id <  id) )
      {
        uint8_t attrLen =  ZCL_GetDataTypeLen(pCurrentAttrDef->type, pCurrentAttrDef->data.pData, (zclAttrDef_t*)pCurrentAttrDef);
        FLib_MemCpy(&ZbZclFoundationGlobals(gCurrentAttr), (void *)pCurrentAttrDef, sizeof(zclAttrDef_t));
        ZbZclFoundationGlobals(gCurrentAttr.id) += (id - pCurrentAttrDef->id);
        ZbZclFoundationGlobals(gCurrentAttr.data.pData) = (uint8_t*)(ZbZclFoundationGlobals(gCurrentAttr.data.pData)) + attrLen * (id - pCurrentAttrDef->id);
        return (void *)(&ZbZclFoundationGlobals(gCurrentAttr));
      }
    }
  }

  /* not found */
  return NULL;
}


/*******************************************************************************
* ZCL_GetAttribute
*
* For applications to call. Get's an attribute based on endpoint/cluster/attr.
* For the pAttrLen parameter, pass NULL if no length is desired. The pAttrData
* parameter must be pointing to a buffer large enough to hold the attribute.
*
* Returns 
*   gZbSuccess_c                  if worked
*   gZclUnsupportedAttribute_c    if attribute doesn't exist
*******************************************************************************/
zbStatus_t ZCL_GetAttribute
  (
    zbEndPoint_t  ep,           /* IN: pointer to instance data (derived from endpoint) */
    zbClusterId_t aClusterId,   /* IN: pointer to cluster definition (derived from clusterID */
    zclAttrId_t   attrId,       /* IN: attribute ID (in little endian) */
    uint8_t       direction,     /* IN: 0x00 for Server Attribute and 0x08 for Client */
    void          *pAttrData,   /* IN/OUT: get a copy of the attribute */
    uint8_t       *pAttrLen     /* IN/OUT: (optional) ptr to attribute length */
  )
{
  afDeviceDef_t  *pDeviceDef;
  afClusterDef_t *pClusterDef;
  void           *pSrcAttrData;
  uint8_t         attrLen;


  /* does the endpoint exist? (and have a device definition?) */
  pDeviceDef = AF_GetEndPointDevice(ep);
  if(!pDeviceDef)
    return gZclUnsupportedAttribute_c;

  /* does the cluster exist on this endpoint? */
  pClusterDef = ZCL_FindCluster(pDeviceDef, aClusterId);
  if(!pClusterDef)
    return gZclUnsupportedAttribute_c;

  /* does the attribute exist on this cluster? If so, find it and return it. */
  pSrcAttrData = ZCL_GetAttributePtr(pDeviceDef, pClusterDef, attrId, direction, &attrLen);
  if(!pSrcAttrData)
    return gZclUnsupportedAttribute_c;

  /* read the attribute */
  FLib_MemCpy(pAttrData, pSrcAttrData, attrLen);
  
  if(pAttrLen)
    *pAttrLen = attrLen;

  return gZbSuccess_c;
}


/******************************************************************************
* ZCL_SetAttribute
*
* Determine the pointer to the attribute. Returns NULL if attribute not found
* on this cluster. Optionally return size of attribute i pSize is non-null.
* Note: there is no checking on values for non-strings (that is, an app can set
* the temperature out of range). If range checking is desired, use the over-the-air
* ZCL commands rather than setting the values directly.
*
* Returns
*   zbSuccess_c                   if worked
*   gZclUnsupportedAttribute_c    if attribute (or cluster) doesn't exist
*   gZclReadOnly_c                attr is read only
*   gZclInvalidValue_c            invalid string length
*******************************************************************************/
zbStatus_t ZCL_SetAttribute
  (
    zbEndPoint_t  ep,           /* IN: pointer to instance data (derived from endpoint) */
    zbClusterId_t aClusterId,   /* IN: pointer to cluster definition (derived from clusterID */
    zclAttrId_t   attrId,       /* IN: attribute ID (in little endian) */
    uint8_t       direction,     /* IN: 0x00 for Server Attribute and 0x08 for Client */
    void          *pAttrData    /* IN: ptr to attribute data (length is implied by ID) */
  )
{
  afDeviceDef_t  *pDeviceDef;
  afClusterDef_t *pClusterDef;
  zclAttrDef_t   *pAttrDef;
  void           *pDstAttrData;
  uint8_t        attrLen;

  /* does the endpoint exist? (and have a device definition?) */
  pDeviceDef = AF_GetEndPointDevice(ep);
  if(!pDeviceDef)
    return gZclUnsupportedAttribute_c;

  /* does the cluster exist on this endpoint? */
  pClusterDef = ZCL_FindCluster(pDeviceDef, aClusterId);
  if(!pClusterDef)
    return gZclUnsupportedAttribute_c;

  /* does the attribute exist? */
  pAttrDef = ZCL_FindAttr(pClusterDef, attrId, direction);
  if(!pAttrDef)
    return gZclUnsupportedAttribute_c;

  /* the attribute must exist */
  pDstAttrData = ZCL_GetAttributePtr(pDeviceDef, pClusterDef, attrId, direction, &attrLen);

  /* handle string special */
  if(ZCL_IsStrType(pAttrDef->type)) {

    /* don't write too long of strings */
    attrLen = *(uint8_t *)pAttrData;  /* get count */
    if(attrLen > pAttrDef->size)
      return gZclInvalidValue_c;

    /* add one byte for length byte */
    ++attrLen;  
  }
#if gZclEnableLongStringTypes_c        
  else if (ZCL_IsLongStrType(pAttrDef->type)) {
    /* don't write too long of strings */
    uint16_t attrLen16 = OTA2Native16(*(uint16_t *)pAttrData);  /* get count */
    if(attrLen16 > pAttrDef->maxLen)
      return gZclInvalidValue_c;
    else
      attrLen = (uint8_t)attrLen16;

    /* add one byte for length byte */
    attrLen += 2;      
  }
#endif  
  

  /* write the attribute */
  FLib_MemCpy(pDstAttrData, pAttrData, attrLen);

#if gZclEnableReporting_c
  {
  uint8_t reportIndex = 0;
  /* report this attribute if attribute is reporting (and has changed enough) */
  if(ZCL_IsReportingAndChanged(pDeviceDef, pClusterDef, attrId, &reportIndex)) 
  {
    zclReportingSetup_t *zclReportingSetup;
    
    /* get reporting Setup information */        
    zclReportingSetup = &((zclReportingSetup_t *)pDeviceDef->pReportSetup)[0];  
    
    /* do the report */  
    if (zclReportingSetup[reportIndex].minTimeCounter == 0)
    {      
      ZbZclFoundationGlobals(gfZclReportRestart) = TRUE;
      zclReportingSetup[reportIndex].sendReportMask |= gZCLReportingAttrValueChanged_c;
      TS_SendEvent(gZclTaskId, gZclEventReportTimeout_c);     
      zclReportingSetup[reportIndex].minTimeCounter = zclReportingSetup[reportIndex].reportMin;
    } else {
      if (zclReportingSetup[reportIndex].reportMin)
        zclReportingSetup[reportIndex].sendReportMask |= gZCLReportingAttrValueChanged_c;    
    }

    /* reset the timer if needed */      
    if((zclReportingSetup[reportIndex].reportTimeout  || zclReportingSetup[reportIndex].reportMin) && !zclReportingSetup[reportIndex].reportCounter)
    {      
      /*Initialize the counter with the number of seconds we want to report*/
      zclReportingSetup[reportIndex].reportCounter = zclReportingSetup[reportIndex].reportTimeout; 
      ZCL_StartReportingTimer();
    }
            
  }
  }
#endif
  if (ZclAttrIsInSceneTable(pAttrDef->flags))
    ((zclSceneAttrs_t*)pDeviceDef->pSceneData)->sceneValid = gZclSceneInvalid_c;

  /* everthing worked */
  return gZbSuccess_c;
}

/******************************************************************************
* ZCL_GetAttributePtr
*
* Determine the pointer to the attribute. Returns NULL if attribute not found
* on this cluster. Optionally return size of attribute i pSize is non-null.
*******************************************************************************/


void *ZCL_GetAttributePtr
    (
    afDeviceDef_t  *pDeviceDef,   /* IN: pointer to device definition */
    afClusterDef_t *pClusterDef,  /* IN: pointer to cluster definition */
    zclAttrId_t attrId,           /* IN: attribute ID (in little endian) */
    uint8_t direction,            /* IN: 0x00 for Server Attributes and 0x08 for Client Attributes*/      
    uint8_t *pAttrLen             /* IN/OUT: return size of attribute (actual not max). */
    )
{
  uint8_t *pData;
  zclAttrDef_t *pAttrDef;
  void *pAttrPtr = NULL;
  uint16_t flags;
  uint8_t attrLen;

  /* make sure there is valid device data */

  /* get pointer to attribute structure */
  pAttrDef = ZCL_FindAttr(pClusterDef, attrId, direction);
  if(!pAttrDef)
    return NULL;  /* invalid attribute */

  /* most types just have a length */
  attrLen = ZCL_GetDataTypeLen(pAttrDef->type, pAttrDef->data.pData, pAttrDef);
  /* get pointer to attribute */
  flags = pAttrDef->flags;
 
  if (ZclAttrIsInSceneData(flags))
    pData = pDeviceDef->pSceneData;
  else
    pData = pDeviceDef->pData;
  if(ZclAttrIsInstance(flags))
  {
    if ((NULL != pDeviceDef) && (NULL != pData))
      pAttrPtr = &pData[pClusterDef->dataOffset + pAttrDef->data.offset];
  }
  else
    pAttrPtr = pAttrDef->data.pData;

  /* need the size too? */
  if((NULL != pAttrPtr) && (NULL != pAttrLen)) {

    /* return the size of the attribute */
    if (ZCL_IsStrType(pAttrDef->type) && !(flags & gZclAttrFlagsInROM_c))
      *pAttrLen = 1 + *((uint8_t *)pAttrPtr);      
#if gZclEnableLongStringTypes_c          
    else if (ZCL_IsLongStrType(pAttrDef->type) && !(flags & gZclAttrFlagsInROM_c))
      *pAttrLen = 2 + OTA2Native16(*((uint16_t *)pAttrPtr));
#endif    
    else if (pAttrDef->type == gZclDataTypeArray_c)
      *pAttrLen = 2 + OTA2Native16(*((uint16_t *)pAttrPtr));
    else
      *pAttrLen  = attrLen;
  }
  /* Return pointer to attribute */
  return pAttrPtr;
}

/******************************************************************************
* Returns TRUE if this type is a string (octet or char).
******************************************************************************/
bool_t ZCL_IsStrType
  (
  zclAttrType_t attrType  /* IN */
  )
{
  return (attrType == gZclDataTypeOctetStr_c || attrType == gZclDataTypeStr_c || attrType == gZclDataTypeLongOctetStr_c || attrType == gZclDataTypeLongStr_c); 
}

#if gZclEnableLongStringTypes_c      
/******************************************************************************
* Returns TRUE if this type is a long string (octet or char).
******************************************************************************/
bool_t ZCL_IsLongStrType
  (
  zclAttrType_t attrType  /* IN */
  )
{
  return (attrType == gZclDataTypeLongOctetStr_c || attrType == gZclDataTypeLongStr_c);
}

#endif


/*******************************************************************************
* ZCL_GetCommand
* 
* Helper function for ZCL callback. See ZCL_Register().
* Returns command on an indication.
*******************************************************************************/
zclCmd_t ZCL_GetCommand
  (
  zbApsdeDataIndication_t *pIndication   /* IN */
  )
{
  return ((zclFrame_t *)(pIndication->pAsdu))->command;
}

/*******************************************************************************
* ZCL_GetTransactionId
* 
* Helper function for ZCL callback. See ZCL_Register().
* Returns transaction ID for this command.
*******************************************************************************/
uint8_t ZCL_GetTransactionId
  (
  zbApsdeDataIndication_t *pIndication   /* IN */
  )
{
  return ((zclFrame_t *)(pIndication->pAsdu))->transactionId;
}

/*******************************************************************************
* ZCL_GetPayload
* 
* Helper function for ZCL callback. See ZCL_Register().
* Returns ptr to payload. Payload type depends on command.
*******************************************************************************/
void *ZCL_GetPayload
  (
  zbApsdeDataIndication_t *pIndication   /* IN */
  )
{
  return (void *)(pIndication->pAsdu + sizeof(zclFrame_t));
}


/********************************************************************************
* Has this changed enough? Works for both signed and unsigned #s, and handles wrap.
* ChangeValue must be < 1/2 max for the type.
********************************************************************************/
#if !gZclEnableOver32BitAttrsReporting_c

bool_t ZCL_CmpUint(uint8_t *pAttrData, uint8_t dataSize)
{
  uint32_t iPresentValue = 0;
  uint32_t iOldValue = 0;
  uint32_t iChangeValue = 0;
  uint32_t iRangeLow = 0;
  uint32_t iRangeHigh = 0;
  uint8_t i;
  
/* #if-s to prevent CodeWarrior warnings */  
#if zclReportValueIndex_c == 1  
  FLib_MemCpy((uint8_t *)&iPresentValue, (uint8_t *)&pAttrData[dataSize], dataSize);
#else
  FLib_MemCpy((uint8_t *)&iPresentValue, (uint8_t *)&pAttrData[zclReportValueIndex_c * dataSize], dataSize);
#endif  
#if zclReportOldValueIndex_c == 1  
  FLib_MemCpy((uint8_t *)&iOldValue, (uint8_t *)&pAttrData[dataSize], dataSize);
#else
  FLib_MemCpy((uint8_t *)&iOldValue, (uint8_t *)&pAttrData[zclReportOldValueIndex_c * dataSize], dataSize);
#endif
#if zclReportChangeIndex_c == 1
  FLib_MemCpy((uint8_t *)&iChangeValue, (uint8_t *)&pAttrData[dataSize], dataSize); 
#else  
  FLib_MemCpy((uint8_t *)&iChangeValue, (uint8_t *)&pAttrData[zclReportChangeIndex_c * dataSize], dataSize); 
#endif

  
  iPresentValue = OTA2Native32(iPresentValue);
  iOldValue = OTA2Native32(iOldValue);
  iChangeValue = OTA2Native32(iChangeValue);
  
  if(iChangeValue)
    --iChangeValue;

  /* determine range */
  if(iOldValue >= iChangeValue)
    iRangeLow = iOldValue - iChangeValue;
  
  for (i = 0; i < dataSize; i++)
    iRangeHigh |= (0xff << i);  
  
  if(iRangeHigh - iOldValue >= iChangeValue)
    iRangeHigh = iOldValue + iChangeValue;

  /* if (+/- change value), then report */
  if(iPresentValue < iRangeLow || iPresentValue > iRangeHigh)
    return TRUE;

  /* yes it's still in range (hasn't changed enough) */
  return FALSE;
}
#else

void ZCL_Add(uint8_t *pTerm1, uint8_t *pTerm2, uint8_t *pResult, uint8_t len)
{
  int8_t i;
  bool_t carry = FALSE;
  
  /* iterate little endian array */
  for (i = 0; i < len; i++)
  {
    uint16_t res = (uint16_t)(pTerm1[i] + pTerm2[i]);
    if (carry)
    {
      res++;
      carry = FALSE;
    }
    if (res > 0xFF)
    {
      carry = TRUE;
      pResult[i] = (uint8_t)(res - 0xFF);
    }
    else
      pResult[i] = (uint8_t)res;
  }

}

bool_t ZCL_SubstractCompare(uint8_t *pTerm1, uint8_t *pTerm2, uint8_t *pResult, uint8_t len)
{
  int8_t i;
  bool_t carry = FALSE;
  uint8_t status = FALSE;
  
  /* iterate little endian array */
  for (i = 0; i < len; i++)
  {
    uint8_t term1 = pTerm1[i];
    uint8_t term2 = pTerm2[i];
    
    if (term2 > term1)
    {
      status = FALSE;
      pResult[i] = 0xFF - (term2 - term1) + 1;

      if (carry)
      {
        pResult[i]--;
      }
      carry = TRUE;
    }
    else
    {
      pResult[i] = term1 - term2;
      if (pResult[i] == 0) 
      {
        if(carry)
        {
          pResult[i] = 0xFF;
          carry = TRUE;
        }
      }
      else
      {
        if(carry)
        {
          pResult[i]--;
          carry = FALSE;
        }
        status = TRUE;
      }
    }
  }
  return  status;
}

/********************************************************************************
* Has this changed enough? Works for both signed and unsigned #s, and handles wrap.
* ChangeValue must be < 1/2 max for the type.
********************************************************************************/
bool_t ZCL_CmpUint(uint8_t *pAttrData, uint8_t dataSize)
{
  uint8_t tempResult1[8];
  uint8_t tempResult2[8];
  bool_t lowRange = FALSE;
  bool_t highRange = FALSE;
  
/* #if-s to prevent CodeWarrior warnings */    
#if zclReportValueIndex_c == 1 
  uint8_t *pPresentValue = (uint8_t *)&pAttrData[dataSize];
#else  
  uint8_t *pPresentValue = (uint8_t *)&pAttrData[zclReportValueIndex_c * dataSize];
#endif  
#if zclReportOldValueIndex_c == 1 
  uint8_t *pOldValue = (uint8_t *)&pAttrData[dataSize];
#else
  uint8_t *pOldValue = (uint8_t *)&pAttrData[zclReportOldValueIndex_c * dataSize];  
#endif
#if zclReportChangeIndex_c == 1
  uint8_t *pChangeValue = (uint8_t *)&pAttrData[dataSize];
#else
  uint8_t *pChangeValue = (uint8_t *)&pAttrData[zclReportChangeIndex_c * dataSize];  
#endif  

  BeeUtilZeroMemory(tempResult1, sizeof(tempResult1));  
  BeeUtilZeroMemory(tempResult2, sizeof(tempResult2));
  
  if ((dataSize > 8)||(FLib_MemCmp(pPresentValue, pOldValue, dataSize)))
    return FALSE;  

  tempResult2[0] = 0x01;
  
  if(!BeeUtilArrayIsFilledWith(pChangeValue, 0, dataSize))
  {
     (void)ZCL_SubstractCompare(pChangeValue, tempResult2, tempResult1, dataSize);
     pChangeValue = &tempResult1[0];
  }

  if (!ZCL_SubstractCompare(pOldValue, pChangeValue, tempResult2, dataSize))
    BeeUtilZeroMemory(tempResult2, sizeof(tempResult2));    

  if (!ZCL_SubstractCompare(pPresentValue, tempResult2, tempResult2, dataSize))
    lowRange = TRUE;
  
  BeeUtilSetToF(tempResult2, sizeof(tempResult2));
  
  (void)ZCL_SubstractCompare(tempResult2, pOldValue, tempResult2, dataSize);
  if (ZCL_SubstractCompare(tempResult2, tempResult1, tempResult2, dataSize))
    ZCL_Add(pOldValue, tempResult1, tempResult2, dataSize);
  else
    BeeUtilSetToF(tempResult2, sizeof(tempResult2));
  
  if (!ZCL_SubstractCompare(tempResult2, pPresentValue, tempResult2, dataSize))
    highRange = TRUE;

  /* if (+/- change value), then report */
  if(lowRange || highRange)
    return TRUE;

  /* yes it's still in range (hasn't changed enough) */
  return FALSE;
}

#endif

/********************************************************************************
* Copies a length encoded string, [len]str
* Returns the length of the string.
********************************************************************************/
uint8_t ZCL_CopyStr(uint8_t *pDst, uint8_t *pSrc)
{
  uint8_t len = 1 + *pSrc;
  FLib_MemCpy(pDst, pSrc, len);
  return len;
}

/********************************************************************************
* ZCL_GetDataTypeLen
*
* Returns data type length
********************************************************************************/
uint8_t ZCL_GetDataTypeLen(uint8_t dataType, uint8_t *pAttrData, zclAttrDef_t *pAttrDef)
{
  uint8_t len = 0;
  
  /* If the attribute is stored on one of our endpoints set the default size
   the one declared in the attribute definition structure */
  if((pAttrDef) && !(pAttrDef->flags & gZclAttrFlagsIsVector_c))
  {
  
     len = pAttrDef->size;
     if(ZCL_IsStrType(pAttrDef->type))
      return len;
  }
    
  switch(dataType) 
  {
    /* 8-bit comparison */ 
    case gZclDataTypeInt8_c:
    case gZclDataType8Bit_c:
    case gZclDataTypeUint8_c:
    case gZclDataTypeBool_c:
    case gZclDataTypeEnum8_c:
    case gZclDataTypeBitmap8_c:  
      len = 1;
      break;
    /* 16-bit comparison */
    case gZclDataTypeInt16_c:
    case gZclDataType16Bit_c:
    case gZclDataTypeUint16_c:
    case gZclDataTypeBitmap16_c:  
    case gZclDataTypeEnum16_c:    
    case gZclDataTypeSemi_c:  
      len = 2;      
      break;
    /* 24-bit comparison */
    case gZclDataTypeInt24_c:
    case gZclDataType24Bit_c:
    case gZclDataTypeUint24_c:
    case gZclDataTypeBitmap24_c:        
      len = 3;      
      break;      
    /* 32-bit comparison */
    case gZclDataTypeInt32_c:
    case gZclDataType32Bit_c:
    case gZclDataTypeUint32_c:
    case gZclDataTypeBitmap32_c:   
    case gZclDataTypeSingle_c:    
    case gZclDataTypeTime_c:  
    case gZclDataTypeDate_c:    
    case gZclDataTypeUTCTime_c:       
      len = 4;
      break;
    /* 40-bit comparison*/  
    case gZclDataTypeInt40_c:
    case gZclDataType40Bit_c:
    case gZclDataTypeUint40_c:
    case gZclDataTypeBitmap40_c:    
      len = 5;
      break;            
    /* 48-bit comparison*/ 
    case gZclDataTypeInt48_c:
    case gZclDataType48Bit_c:
    case gZclDataTypeUint48_c:
    case gZclDataTypeBitmap48_c:        
      len = 6;
      break;                  
    /* 56-bit comparison*/   
    case gZclDataTypeInt56_c:
    case gZclDataType56Bit_c:
    case gZclDataTypeUint56_c:
    case gZclDataTypeBitmap56_c:        
      len = 7;
      break;                              
    /* 64-bit comparison*/   
    case gZclDataTypeInt64_c:
    case gZclDataType64Bit_c:
    case gZclDataTypeUint64_c:
    case gZclDataTypeBitmap64_c:   
    case gZclDataTypeDouble_c:   
    case gZclDataTypeIeeeAddr_c:  
      len = 8;
      break;    
    /* 128-bit comparison*/     
    case gZclDataTypeSecurityKey_c:
      len = 16;
      break;
    /* variable values*/  
    case gZclDataTypeStr_c:
    case gZclDataTypeOctetStr_c:		
    case gZclDataTypeLongOctetStr_c:
    case gZclDataTypeLongStr_c :    
      len = pAttrData[0]+1;
      break;	      
  }
  return len;
}

/********************************************************************************
* ZCL_IsDataTypeDiscrete
*
* Returns TRUE if dataType is discrete or FALSE if dataType is analog 
* ([R2] - Table 2.16 Data Types)
********************************************************************************/
bool_t ZCL_IsDataTypeDiscrete(uint8_t dataType)
{
  if ((dataType<=gZclDataTypeBitmap64_c)||
      (dataType>=gZclDataTypeEnum8_c && dataType<= 0x37)||
        (dataType>=0x40 && dataType<= 0x4F))
    return TRUE;
  
  return FALSE;
}

/********************************************************************************
* ZCL_GetInvalidValue
*
* ([R2] - Table 2.16 Data Types)
********************************************************************************/
void ZCL_GetInvalidValue(uint8_t dataType, uint8_t attrLen, uint8_t *value)
{
  uint8_t i=0;
  
  for(i=0; i<attrLen; i++ )
  {
    if((dataType >= gZclDataTypeInt8_c) && (dataType <= gZclDataTypeInt64_c))
     value[i] = (i==attrLen-1)? 0x80:0x00;
    else
     value[i] = 0xFF;
  }
  
}
