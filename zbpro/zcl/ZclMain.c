/******************************************************************************
* ZclMain.c
*
* This source file contains commands the task used by the ZigBee cluster
* library. This task handles all ZCL events (which may in turn end up in the
* application through the ASL interface).
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
*
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
#include "zclSE.h"

#include "ZdoApsInterface.h"
#if gZclEnableOTAServer_d
#include "ZigbeeTask.h"
#endif

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


/******************************************************************************
*******************************************************************************
* Private data
*******************************************************************************
******************************************************************************/

#if gZclEnableReporting_c

//index_t gZclReportDeviceIndex;  /* for state machine, sending a report */
//index_t gZclReportClusterIndex; /*  */
//index_t gAsynchronousClusterIndex=0;
//bool_t  gfZclReportRestart;

#endif

/******************************************************************************
*******************************************************************************
* Public data
*******************************************************************************
******************************************************************************/

tsTaskID_t gZclTaskId;
void ZCL_ReportingTimeout(zbTmrTimerID_t iTimerId);

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

void TS_ZclTask(uint16_t events);
void ZCL_SendReport(void);
uint8_t ZCL_BuildAttrReport
  (
  zclCmdReportAttr_t *pAttrRecord,  /* IN: buffer to build report */
  afDeviceDef_t      *pDevice,      /* IN: device for building report */
  afClusterDef_t     *pCluster      /* IN: cluster definition */
  );


bool_t gApsAckConfirmStatus;  //TRUE = Succes, FALSE = Otherwise
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* ZCL_Init
* 
* Initialize each endpoint which is a ZCL endpoint.
******************************************************************************/
void ZCL_Init
  (
  void
  )
{
  /* allocate a timer for identify */
  ZbZclFoundationGlobals(gZclIdentifyTimerID) = ZbTMR_AllocateTimer();


  /* initialize ZTC if enabled */
  InitZtcForZcl();

  /* reset each cluster to defaults */
  ZCL_Reset();
  
  gZclTaskId = TS_CreateTask(gTsZclTaskPriority_c, TS_ZclTask);
#if gZclEnableReporting_c
  /* initialize the ZCL task */
  ZbZclFoundationGlobals(gZclReportingTimerID) = ZbTMR_AllocateTimer();
#endif
  
  /* init clusters: */
  
  /* init OTA upgrade cluster */
#if gZclEnableOTAClient_d
  OTAClientInit();
#endif   
  
#if gZclEnableOTAServer_d
  OTAServerInit(ZbGetFsciInstance());
#endif  
 
  /* init poll control cluster */
#if gZclEnablePollControlCluster_d && (gEndDevCapability_d || gComboDeviceCapability_d)     
  if (!IsLocalDeviceReceiverOnWhenIdle())  
      zclPollControl_ClusterServerInit();
#endif    
  
  /* init partition cluster */  
#if gZclEnablePartition_d
  ZCL_PartitionInit();
#if gZclEnableApplianceStatistics_d || gZclEnablePwrProfileClusterServer_d || gZclEnablePwrProfileClusterClient_d
  #if gZclEnableApplianceStatistics_d  
    ZCL_ApplianceStatisticsInit();
  #endif
  #if (gZclEnablePwrProfileClusterServer_d || gZclEnablePwrProfileClusterClient_d) && gZclEnablePartitionPwrProfile_d
    ZCL_PowerProfile_PartitionInit();
  #endif
#else
  ZCL_11073Init();
#endif /* gZclEnableApplianceStatistics_d || gZclEnablePwrProfileClusterServer_d || gZclEnablePwrProfileClusterClient_d */
#endif /* gZclEnablePartition_d */
 
  
}


/*******************************************************************************
* Task for ZigBee Cluster Library. Used to signal ourselves of events.
*******************************************************************************/
void TS_ZclTask
(
uint16_t events  /* IN: the ID to shoose from which to which sap handler the message should flow */
)
{
/*prevent compiler warnings*/
(void) events;

#ifndef gHostApp_d
if(!ZDO_IsRunningState())
{
  return;
}
#endif

#if gZclEnableReporting_c  
  /* time to send a report */
  if(events & gZclEventReportTimeout_c)
    ZCL_SendReport();
  
  if(events & gZclEventReportTick_c) {
    
    /*Start a 1 second timer*/
    ZbTMR_StartSingleShotTimer(ZbZclFoundationGlobals(gZclReportingTimerID), 
                             (zbTmrTimeInMilliseconds_t)1000,  
                             ZCL_ReportingTimeout);
  }
#endif 

}

#if gZclEnableReporting_c  
/*******************************************************************************
* State machine to send the next report. If it can't get the memory, tries
* again later.
*
* This will concatinate all of the reportable attributes in a single cluster
*******************************************************************************/
void ZCL_SendReport(void)
{
  afDeviceDef_t   *pDevice;
  uint8_t payloadLen;
  uint8_t reportLen;
  afToApsdeMessage_t *pMsg; /* a message for sending the report */
  afAddrInfo_t addrInfo;
  afClusterDef_t *pCluster;
  zclFrame_t  *pFrame;
  uint8_t i;
  zclReportAttr_t *pReportList;
  bool_t restartReporting = FALSE;
  
  BeeUtilZeroMemory(&addrInfo, sizeof(addrInfo));
  /* starting over? reset indexes */
  if(ZbZclFoundationGlobals(gfZclReportRestart)) {
    ZbZclFoundationGlobals(gZclReportDeviceIndex) = ZbZclFoundationGlobals(gZclReportClusterIndex) = ZbZclFoundationGlobals(gAsynchronousClusterIndex) = 0;
    ZbZclFoundationGlobals(gfZclReportRestart) = FALSE;
  }

  /* get a buffer to build the next report */
  pMsg = AF_MsgAlloc();
  if(!pMsg) {
    ZbZclFoundationGlobals(gfZclReportRestart) = FALSE;
    ZCL_StartReportingTimer();
    return; 
  }

  /* walk through all devices */
  #if gInstantiableStackEnabled_d
  while(ZbZclFoundationGlobals(gZclReportDeviceIndex) < EndPointConfigData(gNum_EndPoints)) {
  #else
  while(ZbZclFoundationGlobals(gZclReportDeviceIndex) < gNum_EndPoints_c) {
  #endif
    /* make sure this app endpoint is a ZCL device */
    pDevice = (afDeviceDef_t *) EndPointConfigData(endPointList[ZbZclFoundationGlobals(gZclReportDeviceIndex)].pDevice);
    
    /* report list */
    pReportList = pDevice->pReportList;
    
    if(!pDevice || !pDevice->pfnZCL) {
      ++ZbZclFoundationGlobals(gZclReportDeviceIndex);
      continue;
    }
 
    /* Find if any attribute are a asynchronous, then update the attribute */ 
    do
    {
      for(i=0; i<pDevice->reportCount; ++i)
      {
        zclAttrDef_t *pAttrDef = NULL;
        zclReportAttr_t *pCurrentReportList = &pReportList[i];
        /* only looking for this one cluster, to see if it's in the reportinglist */
        if(!IsEqual2Bytes(pCurrentReportList->aClusterId, pDevice->pClusterDef[ZbZclFoundationGlobals(gAsynchronousClusterIndex)].aClusterId))
          continue;    
        
        if (ZbZclFoundationGlobals(gAsynchronousClusterIndex) < pDevice->clusterCount)
          // If the cluster is on the reporting list, find the reporting attribute
          pAttrDef = ZCL_FindAttr(&pDevice->pClusterDef[ZbZclFoundationGlobals(gAsynchronousClusterIndex)], pCurrentReportList->attrId, 0);       
        
        if(pAttrDef)
        {
          /* It's a asynchronous attribute, update before reporting */
          if(ZclAttrIsAsynchronous_c(pAttrDef->flags))
          {
            ++ZbZclFoundationGlobals(gAsynchronousClusterIndex);
            BeeAppUpdateDevice(0, gZclUI_SendReportingAttributeRequest_c, pAttrDef->id, pCurrentReportList->aClusterId, NULL);
            //Free the message, because we return from here
            MSG_Free(pMsg);
            return;
          }
       }
        
      }
      ++ZbZclFoundationGlobals(gAsynchronousClusterIndex);
    }while(ZbZclFoundationGlobals(gAsynchronousClusterIndex) < pDevice->clusterCount);

    /* check each cluster for reporting attributes */
    while(ZbZclFoundationGlobals(gZclReportClusterIndex) < pDevice->clusterCount) 
    {
      bool_t basicDeviceEnabled = 0;
      zbClusterId_t basicClusterId = {gaZclClusterBasic_c};  
      
      pCluster = &pDevice->pClusterDef[ZbZclFoundationGlobals(gZclReportClusterIndex)];

      pFrame = (void *)(&((uint8_t *)pMsg)[ApsmeGetAsduOffset()]);

      /* build the report for the next cluster */
      reportLen = ZCL_BuildAttrReport(
        (zclCmdReportAttr_t *)(pFrame + 1),   /* ptr to report frame */
        pDevice,                              /* ptr to device */
        pCluster
        );            
      
      /* no reporting attributes this cluster */
      if(!reportLen) {
        ++ZbZclFoundationGlobals(gZclReportClusterIndex);
        continue;
      }
      
      (void)ZCL_GetAttribute(EndPointConfigData(endPointList[ZbZclFoundationGlobals(gZclReportDeviceIndex)].pEndpointDesc->pSimpleDesc->endPoint),  basicClusterId, gZclAttrBasic_DeviceEnabledId_c, gZclServerAttr_c, &basicDeviceEnabled, NULL);

      /* send the report only if the device is enabled */
      /* [R2] - 3.2.2.2.13 Device Enable Attribute or is Identify cluster */
      if(basicDeviceEnabled || IsEqual2BytesInt(pCluster->aClusterId, gZclClusterIdentify_c))
      {
        
        /* set up the address info */
        addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
        addrInfo.srcEndPoint = EndPointConfigData(endPointList[ZbZclFoundationGlobals(gZclReportDeviceIndex)].pEndpointDesc->pSimpleDesc->endPoint);
        addrInfo.txOptions = gZclTxOptions;
#if gZclMirroring_d      
        addrInfo.txOptions |= gApsTxOptionAckTx_c;
#endif      
        addrInfo.radiusCounter = afDefaultRadius_c;

        /* determine which cluster to send it to */
        Copy2Bytes(addrInfo.aClusterId, pCluster->aClusterId);

        /* set up frame */
        pFrame->frameControl = gZclFrameControl_FrameTypeGeneral | gZclFrameControl_DisableDefaultRsp | gZclFrameControl_DirectionRsp;
        pFrame->transactionId = ZbZclFoundationGlobals(gZclTransactionId)++;
        pFrame->command = gZclCmdReportAttr_c;

        /* send the report */
        payloadLen = sizeof(zclFrame_t) + reportLen;
        (void)ZCL_DataRequestNoCopy(&addrInfo, payloadLen, pMsg);
      }
      else
      {      
        if(pMsg)
          MSG_Free(pMsg);        
      } 

      ++ZbZclFoundationGlobals(gZclReportClusterIndex);

      /* start a short timer between reporting clusters */
      ZbZclFoundationGlobals(gfZclReportRestart) = FALSE;
      TS_SendEvent(gZclTaskId, gZclEventReportTimeout_c);
      
      return;
    }
    
    /* try next device */
    ++ZbZclFoundationGlobals(gZclReportDeviceIndex);
    ZbZclFoundationGlobals(gZclReportClusterIndex)=0;
    ZbZclFoundationGlobals(gAsynchronousClusterIndex)=0;
    
    /* start up a new timer if needed */
    for(i=0; i< pDevice->reportCount; i++)
    {
      zclReportingSetup_t *zclReportingSetup;
      
      /* get reporting Setup information */        
      zclReportingSetup = &((zclReportingSetup_t *)pDevice->pReportSetup)[0];  
      
      if(zclReportingSetup[i].reportTimeout != 0 && zclReportingSetup[i].reportTimeout != 0xFFFF)
      {
        restartReporting = TRUE;
      }
      if(((zclReportingSetup[i].sendReportMask & gZCLReportingAttrValueChanged_c)&&(!zclReportingSetup[i].minTimeCounter)) || 
        (zclReportingSetup[i].sendReportMask & gZCLReportingAttrTimeout_c))
      {
        zclReportingSetup[i].sendReportMask = gZCLReportingAttrValueEnabled_c;
        zclReportingSetup[i].reportCounter = zclReportingSetup[i].reportTimeout;
        zclReportingSetup[i].minTimeCounter = zclReportingSetup[i].reportMin;
      }
    }
    
  } /* end of while(gZclReportDeviceIndex < gNum_EndPoints_c) */

  if(pMsg)
    MSG_Free(pMsg);

  if(restartReporting)
    ZCL_StartReportingTimer();
}

/*******************************************************************************
* Build the report for this cluster. May be empty.
*
* Returns 0 if nothing to report. Returns length of report if one or more
* attributes to report.
*******************************************************************************/
uint8_t ZCL_BuildAttrReport
  (
  zclCmdReportAttr_t *pAttrRecord,  /* IN: buffer to build report */
  afDeviceDef_t      *pDevice,      /* IN: device for building report */
  afClusterDef_t     *pCluster      /* IN: cluster definition */
  )
{
  uint8_t i;
  uint8_t attrLen;
  uint8_t reportLen;
  zclReportAttr_t *pReportList;
  uint8_t * pAttrData;
  zclAttrDef_t *  pAttrDef;

  /* no reportable attributes on this endpoint */
  if(!pDevice->reportCount)
    return 0;

  /* report list */
  pReportList = pDevice->pReportList;

  /* prepare for building the attribute list from the clusters */ 
  /* look through attributes on this cluster */
  reportLen = 0;
  for(i=0; i<pDevice->reportCount; ++i) {
    zclReportAttr_t *pCurrentReportList = &pReportList[i];

    /* only looking for this one cluster */
    if(!IsEqual2Bytes(pCurrentReportList->aClusterId, pCluster->aClusterId))
      continue;

    /* report the attribute if being reported */

    /* although it's reportable, is it being reported? */
    if(BeeUtilGetIndexedBit(pDevice->pData, i)) 
    {
      zclReportingSetup_t *zclReportingSetup;
      /* get reporting Setup information */        
      zclReportingSetup = &((zclReportingSetup_t *)pDevice->pReportSetup)[i];  
      
      /* check the report status */
      if((!zclReportingSetup->sendReportMask)||(zclReportingSetup->sendReportMask == gZCLReportingAttrValueEnabled_c)||
    		  ((zclReportingSetup->sendReportMask & gZCLReportingAttrValueChanged_c)&&(zclReportingSetup->minTimeCounter)))
        continue;
      
      /* add the attribute to the list */
      pAttrRecord->attrId = pCurrentReportList->attrId;

      pAttrDef = ZCL_FindAttr(pCluster, pAttrRecord->attrId, gZclServerAttr_c);
      if (pAttrDef)
        pAttrRecord->attrType = pAttrDef->type;

        /* get a pointer to the data */
        pAttrData = ZCL_GetAttributePtr(pDevice, pCluster, pCurrentReportList->attrId, gZclServerAttr_c, &attrLen);
        /* get the report value */
        FLib_MemCpy(pAttrRecord->aData, pAttrData, attrLen);
        
        /* update the old Value */
        FLib_MemCpy(pAttrData + attrLen, pAttrData, attrLen);

        /* on to next record */
        attrLen += (sizeof(zclCmdReportAttr_t) - MbrSizeof(zclCmdReportAttr_t,aData));
        pAttrRecord = (zclCmdReportAttr_t *)(((uint8_t *)pAttrRecord) + attrLen); 
        reportLen += attrLen;

    } /* end of if being reported */
  } /* end of for loop */

  return reportLen;
}
/*******************************************************************************
* ZCL_CheckAndStartAttrReporting
*
* check and start attribute reporting if configured
*******************************************************************************/
void ZCL_CheckAndStartAttrReporting(void)
{
  #if gInstantiableStackEnabled_d
  for(uint8_t i=0; i< EndPointConfigData(gNum_EndPoints); i++)
  #else
  for(uint8_t i=0; i< gNum_EndPoints_c; i++)  
  #endif
  {
    afDeviceDef_t  *pDeviceDef;
    zclReportingSetup_t *zclReportingSetup;
    
    pDeviceDef = AF_GetEndPointDevice(EndPointConfigData(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint));
    
    zclReportingSetup = &((zclReportingSetup_t *)pDeviceDef->pReportSetup)[0];  
    
    for(uint8_t j=0; j<pDeviceDef->reportCount; j++)
    {
        if(zclReportingSetup[j].sendReportMask & gZCLReportingAttrValueEnabled_c)
        { 
            /* Start reporting timer with the interval period  */
            if ((!ZbTMR_IsTimerActive(ZbZclFoundationGlobals(gZclReportingTimerID))) && (zclReportingSetup[j].reportTimeout != 0))
            {
              ZCL_StartReportingTimer();
              return;
            }
        }   
    }
  }
}

/*******************************************************************************
* ZCL_StopAttrReporting
*
* clear reporting setup 
*       pDevice -  IN: device definition 
*******************************************************************************/
void ZCL_StopAttrReporting(afDeviceDef_t *pDevice)
{
  zclReportingSetup_t *zclReportingSetup;
   
  zclReportingSetup = &((zclReportingSetup_t *)pDevice->pReportSetup)[0];  
    
  for(uint8_t i=0; i<pDevice->reportCount; i++)
  {
      zclReportingSetup[i].reportTimeout = 0;
      zclReportingSetup[i].reportMin = 0;
      zclReportingSetup[i].sendReportMask = 0x00;
  }
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

#endif  /* gZclEnableReporting_c */
