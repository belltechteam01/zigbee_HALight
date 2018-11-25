/******************************************************************************
* This is a Network Manager file that invokes the Network discovery,
* Formation,Permit Join,Joining to network,start router request
* and sync requests
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#include "EmbeddedTypes.h"




#include "BeeStackConfiguration.h"
#include "BeeStackUtil.h"
#include "BeeStackInterface.h"
#include "BeeStack_Globals.h"
#include "NwkCommon.h"

#include "NwkVariables.h"

#include "ZdoApsInterface.h"
#include "ZdoNwkManager.h"
#include "ZDOStateMachineHandler.h"
#include "ZDOMain.h"
#include "ZdoSecurityManager.h"
#include "ZdpManager.h"
#include "ZdpUtils.h"
#include "ZdoVariables.h"

#include "ApsMgmtInterface.h"
#include "AppZdoInterface.h"
/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

#if gStandardSecurity_d || gHighSecurity_d
extern void SSP_NwkSetRelationship
(
  zbIeeeAddr_t  aSourceAddress,  /* IN: The address of the remote device (Source address). */
  uint8_t  relationShip          /* IN: The relationship Id to be set in the NT. */
);
#endif


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public Memory
*******************************************************************************
******************************************************************************/

#if(gInstantiableStackEnabled_d == 1)
extern zbStackTablesSizes_t* pZbStackTablesSizes; 
#endif

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

/*
  The network layer has sent something to the ZDO layer (usually in response to a 
  request).
*/

void ZDO_NwkManager
(
  nlmeZdoMessage_t *pZdoMsg  /*IN: Message received from ZDO*/
)
{

  nlmeJoinIndication_t  *pJoinIndication;
  uint8_t aNwkAddr[2];
  
  (void)aNwkAddr;
  ZbZdoPrivateData(gMemoryFreedByApplication) = FALSE;
  switch (pZdoMsg->msgType)
  {

    /*************************************************************************************
      NLME-Formation.confirm: Formation confirm is only available for PAN Coordinator
      devices or combo devices acting like coordinator.
    **************************************************************************************/
    case gNlmeNetworkFormationConfirm_c:

#if ( gCoordinatorCapability_d || gComboDeviceCapability_d)

#if gComboDeviceCapability_d
      /*
        If a combo device is not acting like a PAN Coordinator should not do any further
        processing.
      */
      if (NlmeGetRequest(gDevType_c) != gCoordinator_c)
      {
        break;
      }
#endif

      /* Send an event to the ZDO state machine informing the fromation status. */
      if (pZdoMsg->msgData.networkFormationConf.status == gZbSuccess_c)
      {
        ZDO_SendEvent(gZdoEvent_FormationSuccess_c);
      }
      else
      {
        ZDO_SendEvent(gZdoEvent_FormationFailure_c);
      }
#endif
    break;

    /*************************************************************************************
      NLME-Discovery.confirm
    **************************************************************************************/
    case gNlmeNetworkDiscoveryConfirm_c:
      /*
        If the device is trying to associate it self to a PAN, the state machine changes
        to be on gZdoDiscoveringNetworksState_c.
      */
      if (ZDO_GetState() == gZdoDiscoveringNetworksState_c)
      {
        ZdoEvent_t event;
        uint8_t pibValue = TRUE;
        /*
          The discovery information have already been stored by the NWK layer.
          Free up the confirm.
        */
        if(pZdoMsg->msgData.networkDiscoveryConf.pNetworkDescriptor != NULL)
        {
          MSG_Free( pZdoMsg->msgData.networkDiscoveryConf.pNetworkDescriptor );
        }
        
        /*
          Set the macAutoRequest PIB to TRUE. This was set to FALSE when the Network Discovery
          request was sent.
        */
        (void)SetPibAttributeValue(gMPibAutoRequest_c, &pibValue);

        /* send an event to state machine task depending on the confirmation */
        event = (pZdoMsg->msgData.networkDiscoveryConf.status == gZbSuccess_c)? gZdoEvent_DiscoverySuccess_c : gZdoEvent_DiscoveryFailure_c;
        ZDO_SendEvent(event);

        /*
          Setting this variable to FALSE means that ZDO will free the message and not the
          application.
        */
        ZbZdoPrivateData(gMemoryFreedByApplication) = FALSE;
        break;
      }
      
      /*
        If the device is on running state this means that it may be the application the
        one commanding the discovery.
      */
      if(ZDO_IsRunningState())
      {
        /* Reprot the discovery information to hte application. */
        ( void ) ZDP_APP_SapHandler((zdpToAppMessage_t *)pZdoMsg);

        /*
          Setting this variable to TRUE means that the application is responsible of
          freeing the list inside the discovery message.
        */
        ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
      }

      /*
        Management commands prevent the ZDO stae machine for doing anything but finishing
        the process of the remote command.
      */
      else if (ZDO_GetState() == gZdoRemoteCommandsState_c)
      {
#if gMgmt_NWK_Disc_rsp_d
        /* Generate the response through zdpmanager */
        Zdp_Mgmt_Send_Nwk_disc_conf( pZdoMsg );
#endif
        ZDO_SendEvent(gZdoEvent_ManagementResponse_c);

        /*
          The information was copied in ZDP so the list inside the packet is no longer
          needed.
        */
        if (pZdoMsg->msgData.networkDiscoveryConf.pNetworkDescriptor != NULL)
        {
          MSG_Free( pZdoMsg->msgData.networkDiscoveryConf.pNetworkDescriptor );
        }

        /*
          Setting this variable to FALSE means that all the buffers are free, no need to
          re-free it.
        */
        ZbZdoPrivateData(gMemoryFreedByApplication) = FALSE;
      }

      
      else
      {
        /* Clean up the discovery tables in the stack */
        NWK_FreeUpDiscoveryTables();
        if (pZdoMsg->msgData.networkDiscoveryConf.pNetworkDescriptor != NULL)
        {
          MSG_Free( pZdoMsg->msgData.networkDiscoveryConf.pNetworkDescriptor );
        }
        ZbZdoPrivateData(gMemoryFreedByApplication) = FALSE;
      }
      
      /***********************************************************************************
        For any of this cases the tables does not need to be arround so lets free
        up the memory. IMPORTANT: The tables need to be freed to avoid memory leaks.
      ************************************************************************************/
      NWK_FreeUpDiscoveryTables();
    break;

    /*************************************************************************************
      NLME-Join.confirm: We have join TO SOME ONE.
    **************************************************************************************/
    case gNlmeJoinConfirm_c:
#if gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d
      /*
        Make sure that the discovery tables have been freed after join request has been
        processed.
      */
      NWK_FreeUpDiscoveryTables();      

      /* If a combo device is not router or end device should not proceed. */
#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gCoordinator_c)
      {
        break;
      }
#endif

      /*
        If the device is already on running state the node may receive the unsolicited
        rejoin response because of and addressconflict, so ignore the join confirm.
      */
      if (ZDO_IsRunningState())
        break;

      /* If the device is not running then force it to parse the confirm. */
      ZDO_SetState(gZdoJoiningState_c);

      /* Send an event to the ZDO state machine informing about the joining state. */
      if( pZdoMsg->msgData.joinConf.status == gZbSuccess_c )
      {
        ZDO_SendEvent(gZdoEvent_JoinSuccess_c);
      }
      else
      {
        ZDO_SendEvent(gZdoEvent_JoinFailure_c);
      }
      ( void ) ZDP_APP_SapHandler((zdpToAppMessage_t *)pZdoMsg);
      ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
      
#endif
    break;

    /*************************************************************************************
      NLME-Join.indication: A device has join TO US.
    **************************************************************************************/
    case gNlmeJoinIndication_c:
    {
#if !(gStandardSecurity_d || gHighSecurity_d)      
      neighborTable_t  *pNeighborTableEntry = NULL;
#endif      
      /*
        Do the casting for the message one time and in one place, Code saving.
      */
      pJoinIndication = &pZdoMsg->msgData.joinIndication;

      /*
        To avoid reject packets when the node join and comunicate multi ple times,
        the devices should flush the APS Duplicate table.
      */
      APS_RemoveEntryFromApsDuplicateTable(pJoinIndication->aShortAddress);
#if !(gStandardSecurity_d || gHighSecurity_d)   
      /* Every child joined to us must be record, to avoid lossing it in case of reset. */
      pNeighborTableEntry = IsInNeighborTable( gZbAddrMode64Bit_c, pJoinIndication->aExtendedAddress );
#endif       

#if !(gStandardSecurity_d || gHighSecurity_d)
      if(pNeighborTableEntry)
      {
        ZdoNwkMng_SaveToNvm(zdoNvmObject_NeighborTable_c, pNeighborTableEntry);
      }
#endif

#if gRnplusCapability_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
      {
        /* Remove the old routes to the joining device */
        RemoveRouteEntry(pJoinIndication->aShortAddress);
      }
#endif
#endif


#if gStandardSecurity_d || gHighSecurity_d
      /* If the joining device is associated from scratch erase previous data. */
      if (pJoinIndication->rejoinNetwork == gAssociationJoin_c)
      {
        if (ApsmeGetRequest(gApsDefaultTCKeyType_c) == gTrustCenterLinkKey_c)
        {
          APS_ResetDeviceCounters(pJoinIndication->aExtendedAddress);
        }
        else
        {
          APS_RemoveSecurityMaterialEntry(pJoinIndication->aExtendedAddress);
        }

        /* Setting the last parameter to TRUE will erase APS secure material */
        ZDO_SecClearDeviceData(pJoinIndication->aShortAddress, pJoinIndication->aExtendedAddress, FALSE);
      }
      /*
        If security is active and the joining mode is other that association join,
        no further processing should be done to the join indication.
        Consider the unsecure join.
      */
      //else if ((pJoinIndication->rejoinNetwork == gNwkRejoin_c) && pJoinIndication->secureRejoin)
      //{
        /* Make sure the memory will be freed */
        //gMemoryFreedByApplication = FALSE;

        /* Do not process it further, no transport key will be needed. */
        //return;
      //}
      else if (pJoinIndication->rejoinNetwork == gNwkRejoin_c && !pJoinIndication->secureRejoin)
      {
#if gApsClearSecInfoOnUnsecRejoin_d
        /* Reset the Incoming frame counter of the dev that has left. */
        /* Setting the last parameter to TRUE will erase APS secure material */
        if (ApsmeGetRequest(gApsDefaultTCKeyType_c) == gTrustCenterLinkKey_c)
        {
          APS_ResetDeviceCounters(pJoinIndication->aExtendedAddress);
        }
        else
        {
          APS_RemoveSecurityMaterialEntry(pJoinIndication->aExtendedAddress);
        }
#endif
        /* Setting the last parameter to TRUE will erase APS secure material */
        ZDO_SecClearDeviceData(pJoinIndication->aShortAddress, pJoinIndication->aExtendedAddress, FALSE);
      }

      /*
        The device must be set as Unauthenticated child in the NT.
      */
      SSP_NwkSetRelationship(pJoinIndication->aExtendedAddress, gUnAuthenticatedChild_c);

      /* Keep the buffer around to be use in the state machine or the application. */
      ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;

/***************************************************
  Trust center specific join indication processing.
****************************************************/
#if gTrustCenter_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
      if (ZbBeeStackNwkGlobals(gTrustCenter))
#endif
      {
        /*
          Keep the information, we are going to start the joining state machine.
        */
        MSG_Queue(&ZbZdoPrivateData(gJoinIndicationQueue), pZdoMsg);

        /*
          Tell the ZDO Task that we got a joining device packet and ready for action.
        */
        TS_SendEvent(gZdoTaskID_c, gJoinIndicationArrive_c);
      }
#endif

#if !gTrustCenter_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
      if (!ZbBeeStackNwkGlobals(gTrustCenter))
#endif
      {
#if gASL_EnableZllTouchlinkCommissioning_d
        /*
          ZLL Specification[8.5.2] If apsTrustCenterAddress is equal to 0xffffffffffffffff , the router shall generate 
          and transmit an APS transport key command frame with a source address equal to 0xffffffffffffffff, 
          secured with the ZLL certification pre-installed link key (see 8.7.2), to the joining device
        */
        if ((pJoinIndication->rejoinNetwork == gAssociationJoin_c) && Cmp8BytesToFs(ApsmeGetRequest(gApsTrustCenterAddress_c)))
        {
            zbKeyType_t keyType = gStandardNetworkKey_c;
            zbNwkSecurityMaterialSet_t  *pSecurityMaterial;
            zbTransportKeyData_t  transportKeyData;
                     
             /* add the Device into the security material*/
            ZDO_SecAddDeviceInfo(pJoinIndication->aExtendedAddress, pJoinIndication->aShortAddress);           
            
            /* get network key from memory */
            transportKeyData.NetworkKey.keySeqNumber = NlmeGetRequest(gNwkActiveKeySeqNumber_c);
            pSecurityMaterial = SSP_NwkGetSecurityMaterialSet(transportKeyData.NetworkKey.keySeqNumber);            
            Copy16Bytes(transportKeyData.NetworkKey.aNetworkKey, pSecurityMaterial->nwkKey);
            Copy8Bytes(transportKeyData.NetworkKey.aParentAddress, ApsmeGetRequest(gApsTrustCenterAddress_c));
            transportKeyData.NetworkKey.useParent = FALSE;
            ZDO_APSME_Transport_Key_request(pJoinIndication->aExtendedAddress, keyType, &transportKeyData);
        }
        else
        {
#endif        
        uint8_t joiningType = gReserved_c;
        bool_t  isHighSecurity = FALSE;
          
        /*
          Every other device with joining capabilities should report the joining,
          using an APSME-UPDATE-DEVICE.request.
        */
        isHighSecurity = (pJoinIndication->capabilityInformation & gSecurityCapability_c)? TRUE : FALSE;
        if (pJoinIndication->rejoinNetwork == gAssociationJoin_c)
        {
          joiningType = (isHighSecurity)? gHighSecurityDeviceUnsecuredJoin_c : gStandardDeviceUnsecuredJoin_c;
        }
        else if ((pJoinIndication->rejoinNetwork == gNwkRejoin_c) || (pJoinIndication->rejoinNetwork == gOrphanJoin_c))
        {
          if (pJoinIndication->secureRejoin)
          {
            joiningType = (isHighSecurity)? gHighSecurityDeviceSecuredReJoin_c : gStandardDeviceSecuredReJoin_c;
          }
          else
          {
            joiningType = (isHighSecurity)? gHighSecurityDeviceUnsecuredRejoin_c : gStandardDeviceUnsecuredRejoin_c;
          }
        }

        // Always send the update device
        ZDO_APSME_Update_Device_request(pJoinIndication->aExtendedAddress,
                                        pJoinIndication->aShortAddress,
                                        joiningType,
                                        pJoinIndication->secureRejoin);
        (void)ZDP_APP_SapHandler((void *)pZdoMsg);
#if gASL_EnableZllTouchlinkCommissioning_d
        }
#endif        
      }
#endif

/* #endif gStandardSecurity_d */
#endif
    }
    break; /*break for join indication*/

    /*************************************************************************************
      NLME-StartRouter.confirm: Every time that a Coordinator or a Router starts, genrates
      a start router confirm.
    **************************************************************************************/
    case gNlmeStartRouterConfirm_c:
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d

      /* Combo devices acting like ZED don't need this confirm. */
#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      {
        break;
      }
#endif

      /* Indicate router has started (or not) */
      if (pZdoMsg->msgData.startRouterConf.status == gZbSuccess_c)
      {
        ZDO_SendEvent(gZdoEvent_StartRouterSuccess_c);
      }
      else
      {
        ZDO_SendEvent(gZdoEvent_StartRouterFailure_c);
      }
#endif
    break;

    /*************************************************************************************
      NLME-Sync.confirm
    **************************************************************************************/
    case gNlmeSyncConfirm_c:
#if gEndDevCapability_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
      {
        break;
      }
#endif
      pZdoMsg->msgType = gSyncConf_c;
      ( void ) ZDP_APP_SapHandler( (void *)pZdoMsg );
      /* The application is responsible of freen the buffers. */
      ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
#endif
    break;

    /*************************************************************************************
      NLME-PermitJoining.confirm
    **************************************************************************************/
    case gNlmePermitJoiningConfirm_c:
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      {
        break;
      }
#endif

      if(ZDO_IsRunningState())
      {
        /* let application know about the permit join */
        pZdoMsg->msgType = gPermitjoinConf_c;
        ( void ) ZDP_APP_SapHandler( (void *)pZdoMsg );

        /* indicate this SAP handler won't be freeing the memory */
        ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
      }

#endif
      break;

    /*************************************************************************************
      NLME-DirectJoin.confirm
    **************************************************************************************/
    case gNlmeDirectJoinConfirm_c:
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      {
        break;
      }
#endif

      /*
        The may come because the network layer start the direct join and the confirm must
        be pass up to the application.
      */
      if (ZDO_IsRunningState())
      {
        /* Send message on to app. Tell this SAP handler not to free it. */
        (void) ZDP_APP_SapHandler( (void *)pZdoMsg );
        ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
      }

      else if (ZDO_GetState() == gZdoRemoteCommandsState_c)
      {
        ZDO_SendEvent(gZdoEvent_ManagementResponse_c);
#if gMgmt_Direct_Join_rsp_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
        if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
        {
          break;
        }
#endif
        /* Generate the response through zdpmanager */
        Zdp_Mgmt_Direct_Join_conf( pZdoMsg );
        /* Update the global variable to free the menory */
        ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
#endif
      }

#endif
      break;

    /*************************************************************************************
      NLME-Leave.indication
    **************************************************************************************/
    case gNlmeLeaveIndication_c:

      /* The local node is the one leaving the network. */
      if (Cmp8BytesToZero(pZdoMsg->msgData.leaveIndication.aDeviceAddress))
      {
        /* Only important to the local leave. Pick an appropiated leaving event. */
        ZdoStopMode_t stopMode;

        /*
          Send event to ZDO state machine to initiate the reset procedure (r11, p272, ln31)
          The devices should not set the default mode to stop the node on a leave indication,
          the default mode send another leave.... and we just left.
        */
        stopMode = gZdoStopModeDefault_c |  ZbZdoPublicData(gZdoStopMode);
        if (pZdoMsg->msgData.leaveIndication.rejoin)
        {
          /* We will come back.! */
          stopMode |= gZdoStopMode_StopAndRestart_c;

          /* We are doing rejoin, we should not clear the tables. */
          stopMode &= ~gZdoStopMode_ResetTables_c;

        } /* End of if (pZdoMsg->msgData.leaveIndication.rejoin) */

        /* Tell ZDO to pull the breaks; */
        ZDO_StopEx(stopMode);
        break;
      } /* End of if (Cmp8BytesToZero(pZdoMsg->msgData.leaveIndication.aDeviceAddress)) */

      /* If we got the indiction and we are waiting for it, return to previous state. */
      if (ZDO_GetState() == gZdoRemoteCommandsState_c)
      {
        ZDO_SendEvent(gZdoEvent_ManagementResponse_c);
      }

#if gStandardSecurity_d || gHighSecurity_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      {
        break;
      }
#endif

      /*
        Reset the Incoming frame counter of the dev that has left.
      */
      SSP_NwkResetDeviceFrameCounter( pZdoMsg->msgData.leaveIndication.aDeviceAddress );
#if gApsRemoveLinkKeyOnDeviceLeave_d
      /*
        If the device is Link capable remove the link key information if it exists.
      */
      APS_RemoveSecurityMaterialEntry(pZdoMsg->msgData.leaveIndication.aDeviceAddress);
#endif
      /*
        Let the trust center know about the leaving device.
      */
//      if (!IsSelfIeeeAddress(ApsmeGetRequest(gApsTrustCenterAddress_c)))
#if !gTrustCenter_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
       if (!ZbBeeStackNwkGlobals(gTrustCenter))
#endif
      
      {
	  	neighborTable_t  *pNeighborTableEntry;
        pNeighborTableEntry = IsInNeighborTable(gZbAddrMode64Bit_c, pZdoMsg->msgData.leaveIndication.aDeviceAddress);
        
        if (pNeighborTableEntry && (pNeighborTableEntry->deviceProperty.bits.relationship == gNeighborIsChild_c))
        
        ZDO_APSME_Update_Device_request(pZdoMsg->msgData.leaveIndication.aDeviceAddress,
                                        APS_GetNwkAddress(pZdoMsg->msgData.leaveIndication.aDeviceAddress, aNwkAddr),
                                        gDeviceLeft_c, 0x00);
      }
#endif      
#endif
#endif

     /* ZEDs dont porcess the removing of the children and removing of the routes. */
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      {
        break;
      }
#endif

#if gRnplusCapability_d
      /* Remove the old routes for the leaving device */
      RemoveNextHopRouteEntry(APS_GetNwkAddress(pZdoMsg->msgData.leaveIndication.aDeviceAddress, aNwkAddr));
#endif

      /* Clean up address map if there is no bindings referencing address map entry
         consider deleting all bindings first.
       */
      (void)APS_RemoveDeviceFromAddressMap(pZdoMsg->msgData.leaveIndication.aDeviceAddress);

      /* Tells the network layer to remove the leaving device from its tables. */
      NWK_RemoveChildRecords(pZdoMsg->msgData.leaveIndication.aDeviceAddress);
#endif

      /*
        Save the network data, to preserve that the leaving device is gone. Plus the
        counters for security (NWK and APS)that got reset.
      */
      ZdoNwkMng_SaveToNvm(zdoNvmObject_All_c, NULL);
    break;

    /*************************************************************************************
      NLME-Leave.confirm
    **************************************************************************************/
    case gNlmeLeaveConfirm_c:
      /* If we have received a management leave request, send the response */
      if (ZDO_GetState() == gZdoRemoteCommandsState_c)
      {
#if gMgmt_Leave_rsp_d
        Zdp_Mgmt_Send_leave_conf(pZdoMsg);
        /* The momery has been handled by the next higher layer. */
        ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
#endif
        ZDO_SendEvent(gZdoEvent_ManagementResponse_c);
      }

      /* The local node is the one leaving the network. */
      if (Cmp8BytesToZero(pZdoMsg->msgData.leaveConf.aDeviceAddress))
      {
        /* The self leave is handle in the indication. */
        break;
      }

#if gStandardSecurity_d || gHighSecurity_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      {
        break;
      }
#endif
     
      /* Tables should be cleared and the update device indication should be sent
      when the leave indication is received (spec R19 4.6.3.6.2). If the remove
      device command is used by the trust center 2 update device commands would
      be sent ota, one for the indication and one for the confirm. This is not ok.
      
      If some scenario relies on the leave confirm(eg. when silent leave is used)
      the tables should be cleared when the neighbor expires.
      */
#if 0
      if(pZdoMsg->msgData.leaveConf.status != gZbSuccess_c)
      {
        /*
          The device has not left the network, do nothing
        */
        break;
      }

      /*
        Reset the Incoming frame counter of the dev that has left.
      */
      SSP_NwkResetDeviceFrameCounter( pZdoMsg->msgData.leaveIndication.aDeviceAddress );

      /*
        If the device is Link capable remove the link key information if it exists.
      */
      APS_RemoveSecurityMaterialEntry(pZdoMsg->msgData.leaveIndication.aDeviceAddress);

      /*
        Let the trust center know about the leaving device.
      */
      ZDO_APSME_Update_Device_request(pZdoMsg->msgData.leaveIndication.aDeviceAddress,
                                      APS_GetNwkAddress(pZdoMsg->msgData.leaveIndication.aDeviceAddress, aNwkAddr),
                                      gDeviceLeft_c, 0x00);

      /*
        Tells the network layer to remove the leaving device from its tables.
      */
      NWK_RemoveChildRecords(pZdoMsg->msgData.leaveIndication.aDeviceAddress);
#endif//if 0
#endif

/* #endif gStandardSecurity_d */
#endif
    break;

    /*************************************************************************************
      NLME-RouteDiscovery.confirm
    **************************************************************************************/
    case  gNlmeNetworkStatusCnf_c:
    case  gNlmeRouteDiscoveryCnf_c:
      /* This packet will be freed by the application*/
      ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
      /* Pass the packet to the App */
      ( void ) ZDP_APP_SapHandler( (void *)pZdoMsg );
    break;

    /*************************************************************************************
      NLME-EnergyScan.confirm: An Energy Scan Confirm has come back. Was it due to the
      forming process, or due to FA/ZDP.
    **************************************************************************************/
    case  gNlmeEnergyScanConfirm_c:           
      
      if(ZDO_GetState() == gZdoDiscoveringNetworksState_c)
      {
        if (pZdoMsg->msgData.EnergyScanConf.status == gZbSuccess_c)
        {
          ZDO_SendEvent(gZdoEvent_EnergyScanComplete_c);
        }
        else
        {
          ZDO_StopEx(gZdoStopModeDefault_c);
        }
      }

      /* Send to application layer */
      ( void ) ZDP_APP_SapHandler( (void *)pZdoMsg );

      /* Update the global variable to free the memory */
      ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
    break;

    case gNlmeNetworkStatusInd_c:
    case gNlmeTxReport_c:
      /* Send to application layer */
      ( void ) ZDP_APP_SapHandler( (void *)pZdoMsg );

      /* Update the global variable to free the memory */
      ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
    break;

    case gNwkProcessFrameConf_c:
#if gStandardSecurity_d || gHighSecurity_d
      /* Send to application layer */
      ( void ) ZDP_APP_SapHandler( (void *)pZdoMsg );

      /* Update the global variable to free the memory */
      ZbZdoPrivateData(gMemoryFreedByApplication) = TRUE;
#else
      ZbZdoPrivateData(gMemoryFreedByApplication) = FALSE;
#endif
    break;

    default:
    break;
  } /* End of switch */
}

/*****************************************************************************/

/************************************************************************************
* Using the default values of the ApsChannelMask and ScanDuration, generates an
* Nlme command to discover the ZigBee networks that may exist on the current
* channel list (issues beacon requests and listens to beacon responses).
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
************************************************************************************/
void ZdoNwkMng_GenerateNwkDiscoveryReq
(
  void  /* IN: No Input Parameters. */
)
{
  zdoNlmeMessage_t *pNwkMsg;
  nlmeNetworkDiscoveryReq_t *pNwkDiscoveryReq;
  uint8_t *pChannelList = (uint8_t *)ApsmeGetRequest(gApsChannelMask_c);

  /*
    We need to generate a message to Nwk layer, a buffer needs to be allocated.
  */
  pNwkMsg = MSG_AllocType( zdoNlmeMessage_t );
  /*
    For some reason we fail to allocate the memory, no further
    processing can be done.
  */
  if (!pNwkMsg)
    return;

  /*
    We are about to do what in the network layer is named active scan, we need the right
    list depending on the device type.
  */
#if gCoordinatorCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) == gCoordinator_c)
#endif
  {
    /*
      We will use the list from the ED if and only if it is a valid list other wise use
      the current APS channel mask.
    */
    if ((*(uint32_t *)ZbNwkPublicData(gaScanChannels)) != 0)
    {
      /* Use only the channels with the best energy levels for the active scan. */
      pChannelList = ZbNwkPublicData(gaScanChannels);
    }
  }
#endif

  /*
    Create the network descovery request and sent to the network Layer.
  */
  pNwkMsg->msgType = gNlmeNetworkDiscoveryRequest_c ;

  /*
    Fast access to the memory where the message is build, also is less code.
  */
  pNwkDiscoveryReq =  &pNwkMsg->msgData.networkDiscoveryReq;

  /*
    Set the current and active ApsChannelMask, remember this can be updated
    during the FA process.
  */
  FLib_MemCpy(pNwkDiscoveryReq->aScanChannels,
              pChannelList,
              sizeof(ApsmeGetRequest(gApsChannelMask_c)));

  /*
    Get the default duration to use during the scan, remember that is trasnformed
    to a milliseconds time, down in the Nwk Layer.
  */
  pNwkDiscoveryReq->scanDuration = gScanDuration_c;
  
  if (ZDO_GetState() == gZdoDiscoveringNetworksState_c)
  {
  	uint8_t pibValue = FALSE;
    /*
      Set the macAutoRequest PIB to FALSE for the mac scans performed during ZDO Network
      Discovery to avoid the MAC layer stopping the active scan when the result list is
      full. The information relevant to joining is collected from the Beacon Notify
      indications in AppStackImpl.c by ParseBeaconNotifyIndication().
    */
    (void)SetPibAttributeValue(gMPibAutoRequest_c, &pibValue);
  }

  /*
    Pass the command down to the Nwk layer.
  */
  if (ZDO_NLME_SapHandler( pNwkMsg ))
  {
    /*
      Catch the error if needed.
    */
  }

}


#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
/************************************************************************************
* Does a local energy scan, pass the confirm to the application using the ZDP_APP
* Sap handler. Not available to ZEDs
*
* Interface assumptions:
*   The parameter aChannelList, is a valid channel list and is on little endian.
*   The parameter duration, is a non zero value.
*
* Return value:
*   NONE.
*
************************************************************************************/
void ZdoNwkMng_EnergyScanRequest
(
  zbChannels_t  aChannelList,
  uint8_t  duration
)
{
  uintn32_t  localList = 0;
  zdoNlmeMessage_t  *pEnergyScan;
  nlmeEnergyScanReq_t  *pEnergyRequest;

  pEnergyScan = MSG_Alloc(sizeof(nlmeEnergyScanReq_t) + sizeof(zbMsgId_t));

  if (!pEnergyScan)
    return;
  // Protection for new mac 
  FLib_MemSet(pEnergyScan,0,sizeof(nlmeEnergyScanReq_t) + sizeof(zbMsgId_t));
  /*
    NOTE: this function is used on FA and on regular ZDO state machine some times it
    receives the parameter on little endian and some times on big endian.
  */

  /* Check if it's a valid channel list */
  if( aChannelList[0] & 0xFF ||
      aChannelList[1] & 0x07 ||
      aChannelList[3] & 0xF8 )
  {
#ifdef PROCESSOR_KINETIS
    FLib_MemCpyReverseOrder(&localList, aChannelList, sizeof(uint32_t));
#else
    localList = *((uint32_t *)aChannelList);
    localList = Native2OTA32(localList);
#endif
  }


  pEnergyScan->msgType = gNlmeEnergyScanRequest_c;
  pEnergyRequest = &pEnergyScan->msgData.EnergyScanReq;
  if (localList)
  {
    FLib_MemCpy(pEnergyRequest->aScanChannels, &localList, sizeof(localList));
  }
  else
  {
    FLib_MemCpy(pEnergyRequest->aScanChannels, aChannelList, sizeof(localList));
  }
  pEnergyRequest->scanDuration = duration;

  if(ZDO_NLME_SapHandler(pEnergyScan))
  {
  }
}
#endif /*( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)*/

/*****************************************************************************/

#if ( gCoordinatorCapability_d ) || gComboDeviceCapability_d

/************************************************************************************
* Using the default values of the ApsCahnnelMask and ScandDuration, Generates an
* Nlme command to discover the ZigBee networks that may exist on the current
* channel list.
*
* Interface assumptions:
*   Uses APS channel mask (gApsChannelMask_c)
*
* Return value:
*   NONE.
*
************************************************************************************/
void ZdoNwkMng_GenerateNetworkFormation
(
  void  /*IN: No Input Parameters*/
)
{
  nlmeNetworkFormationReq_t  *pNwkFormationReq;
  zdoNlmeMessage_t *pNwkMsg;
  uint8_t *pChannelList = (uint8_t *)ApsmeGetRequest(gApsChannelMask_c);

#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) != gCoordinator_c)
    return;
#endif

  /*
    every single command needs it own buffer to be passed down to next layer.
  */
  pNwkMsg = MSG_AllocType(zdoNlmeMessage_t);
  /*
    If the buffer allocation fails for any reason, that is it, no further
    processing is needed.
  */
  if ( !pNwkMsg )
    return;

  /*
    Create the network formation request and sent to the network Layer.
  */
  pNwkMsg->msgType = gNlmeNetworkFormationRequest_c;

  /*
    Fast access to the memory where the 
  */
  pNwkFormationReq = &pNwkMsg->msgData.networkFormationReq;

#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) == gCoordinator_c)
#endif
  {
    /*
      We will use the list from the ED if and only if it is a valid list other wise use
      the current APS channel mask.
    */
    if ((*(uint32_t *)ZbNwkPublicData(gaScanChannels)) != 0)
    {
      /* Use only the channels with the best energy levels for the active scan. */
      pChannelList = ZbNwkPublicData(gaScanChannels);
    }
  }

  /*
    Using the default channel mask, generate the Formation command.
  */
  FLib_MemCpy(pNwkFormationReq->aScanChannels,
              pChannelList,
              sizeof(ApsmeGetRequest(gApsChannelMask_c)));

  /*
    Get all the parameters neede for this command.
  */
  pNwkFormationReq->scanDuration = gScanDuration_c;
  FLib_MemCpy(&pNwkFormationReq->beaconOrder, &ZbBeeStackGlobalsParams(gBeeStackConfig.beaconOrder), 2);
  Copy2Bytes(pNwkFormationReq->aPanId, NlmeGetRequest(gNwkPanId_c));
  pNwkFormationReq->batteryLifeExtension = ZbBeeStackGlobalsParams(gBeeStackConfig.batteryLifeExtn);

  if (ZDO_NLME_SapHandler( pNwkMsg ))
  {
    /*
      Catch the error if needed.
    */
  }

}

#endif /*( gCoordinatorCapability_d )*/

/*****************************************************************************/

#if ( !gCoordinatorCapability_d )

/*
  Call on the network layer to (re)join the network. Includes orphan join, 
  associate join, etc...
*/
void ZdoNwkMng_GenerateNwkJoinRequest(zbNwkJoinMode_t joinMode)
{
  zdoNlmeMessage_t *pMsg;
  nlmeJoinReq_t *pJoinReq;
  uint8_t *pExtendedPan = (uint8_t *)NlmeGetRequest(gNwkExtendedPanId_c);

  /* allocate the request */
  pMsg = MSG_AllocType(zdoNlmeMessage_t);

  if (!pMsg)
  {
    return;
  }

  /* Create the network Join request and sent to the network Layer */
  pMsg->msgType = gNlmeJoinRequest_c;
  pJoinReq = &pMsg->msgData.joinReq;

  FLib_MemCpy(pJoinReq->aScanChannels,
              ApsmeGetRequest(gApsChannelMask_c),
              sizeof(ApsmeGetRequest(gApsChannelMask_c)));

  /*
    By default the node will try to keep the behavior established by the SAS attribute,
    The SAS attribute canbe override by the application through the IB Set and Get.
  */
  pJoinReq->securityEnable = ApsmeGetRequest(gApsUseInsecureJoin_c)? FALSE : TRUE;
#if gStandardSecurity_d || gHighSecurity_d
  /* If we are trying to send a secure packet, then, check if we have a valid Nwk key. */
  if (pJoinReq->securityEnable)
  {
    pJoinReq->securityEnable = SSP_NwkGotValidKey();
  }

  /* If the rejoin is allowed unsecured then change the APSIB until authentication has arrive. */
  if ((!pJoinReq->securityEnable) && (joinMode != gZdoStartMode_SilentStart_c))
  {
    ApsmeSetRequest(gApsUseInsecureJoin_c, TRUE);
    BeeUtilZeroMemory(&ApsmeGetRequest(gApsTrustCenterAddress_c), 8);
  }
#endif

#if gComboDeviceCapability_d
  pJoinReq->joinAsRouter = (NlmeGetRequest( gDevType_c ) == gRouter_c)? TRUE : FALSE;
#elif ( gRouterCapability_d )
  pJoinReq->joinAsRouter = TRUE;
#else /*gRouterCapability_d*/
  pJoinReq->joinAsRouter = FALSE;
#endif /*gRouterCapability_d*/

  pJoinReq->rejoinNetwork = joinMode;
  if (joinMode == gZdoStartMode_NwkRejoin_c)
  {
    pExtendedPan = (uint8_t *)ApsmeGetRequest(gApsUseExtendedPANID_c);
  }

  Copy8Bytes( pJoinReq->aExtendedPanId, pExtendedPan);
  pJoinReq->scanDuration = gScanDuration_c;
  pJoinReq->powerSource = ((ZbBeeStackGlobalsParams(gBeeStackConfig.currPowerSourceAndLevel) & 0x01)? 0 : 1);

  pJoinReq->rxOnWhenIdle = ((IsLocalDeviceReceiverOnWhenIdle())? TRUE : FALSE);

  ( void ) ZDO_NLME_SapHandler( pMsg );
}

#endif /*( !gCoordinatorCapability_d )*/


/*****************************************************************************/
#if ( gRouterCapability_d || gCoordinatorCapability_d ) || (gComboDeviceCapability_d)

/*
  Start the ZC or ZR. Turns on the radio.
  
  Calls NLME-START-ROUTER.request
*/
void ZdoNwkMng_GenerateStartRouterReq
(
  void  /*IN: No Input Parameters*/
)
{
  zdoNlmeMessage_t *pMsg;
  nlmeStartRouterReq_t sStartRouterReq;

  /*Create the Start as Router request and sent to the network Layer*/
  pMsg = MSG_AllocType(zdoNlmeMessage_t);
  if(NULL == pMsg)
    return;

  pMsg->msgType = gNlmeStartRouterRequest_c;
  sStartRouterReq.beaconOrder = sStartRouterReq.superframeOrder = ZbBeeStackGlobalsParams(gBeeStackConfig.beaconOrder);
  sStartRouterReq.batteryLifeExtension = ZbBeeStackGlobalsParams(gBeeStackConfig.batteryLifeExtn);
	FLib_MemCpy(&pMsg->msgData.startRouterReq,&sStartRouterReq,sizeof(nlmeStartRouterReq_t));
  ( void ) ZDO_NLME_SapHandler(pMsg);

}

#endif /*( gRouterCapability_d || gCoordinatorCapability_d ) || (gComboDeviceCapability_d))*/

/*****************************************************************************/
#if ( !gCoordinatorCapability_d )
#if gStandardSecurity_d || gHighSecurity_d
/******************************************************************************
* This function is called when the time to establish a Nwk key expires.This funtion
* makes the device leave the Nwk.
*
* Interface assumptions:
*   None
*
* Return Value:
*   None 
*
* Effects on global data:
*   None
*
******************************************************************************/
void ZDO_SecEstablishKeyTimeExpired
(
  void /*IN: No iniput Parameter*/
)
{
  uint8_t disableSecurity = 0;

  /* Create the Leave request and sent to the network Layer, since authentication Failed. */
  NlmeSetRequest(gNwkSecureAllFrames_c, disableSecurity);
  NlmeSetRequest(gNwkSecurityLevel_c, disableSecurity);

  ZDO_Leave();
}
#endif
#endif

/******************************************************************************
* Clears all routes, and walk through the NT remmoving siblings. The idea is
* to force the re-addtion of the secure matriasl, and re-route to any sibling
* destination
*
* Interface assumption:
*   startMode is a valid ZDO start up mode.
*
* Retrun value:
*   NONE.
*******************************************************************************/
#if (gStandardSecurity_d || gHighSecurity_d) && !gZigbeeProIncluded_d
void ZdoNwk_ClearSiblingOnNeighborTable(ZdoEvent_t startMode)
{
#if (gRnplusCapability_d && !gEndDevCapability_d)
  uint8_t iIndex;         /* Index to traverse through the neighbor table*/
  neighborTable_t* pNTE;  /* pointer to neighbor table entry */

  /*
    If the starting mode is silent the device should prented that it has been on the
    network all the time and avoid sending the device announce.
  */
  if (startMode == gZdoStartMode_SilentStart_c)
  {
    return;
  }

  /* End device dont have neither siblings or routes. */
  if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    return;
  }
  /*
    If SP1 is used then clean up the NT table and route table for siblings when a router rejoins
    this ensures that routes are re-established again.
  */
  ResetRouteTable();

  for (iIndex = 0; iIndex < ZbStackTablesSizes(gMaxNeighborTableEntries); iIndex++)
  {
    pNTE = &(ZbBeeStackNwkGlobals(gaNeighborTable [ iIndex ]));
    if(pNTE->deviceProperty.bits.relationship == gNeighborIsSibling_c )
    {
      ResetNeighborTableEntry(pNTE);
    }
  }
#endif
  (void)startMode;
}
#endif

/************************************************************************************
* Generates a local Nlme Leave command to let the nwk layer know that WE as a device
* are leaving the network, in other words, is a command to do a self-leave. Only called 
* from the ZDO state machine.
*
* NOTE: This function is meant to only be called byt the ZDO State machine on the
*       leaving state.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
************************************************************************************/
void ZdoNwkMng_GenerateNlmeLeaveRequest
(
  void  /*IN: No Input Parameters*/
)
{
  /*
    Pointer to the buffer where the Nlme command will be filled.
  */
  zdoNlmeMessage_t *pZdoNlmeMessage;

  /*
    The size for the curretn command to be passed down.
  */
  uint8_t size = (sizeof(zbMsgId_t) + sizeof(nlmeLeaveReq_t));

  /*
    Allocated the exact size memory for the Nlme command, remember
    this will passed down using a Sap hanlder, the memory will be freed by the next layer.
  */
  pZdoNlmeMessage = MSG_Alloc(size);

  /*
    If for any reason the message was unable to be allocated, no further
    processing is done.
  */
  if(!pZdoNlmeMessage)
    return;

  /*
    Most of the options are filled with a Zero value, lets fill the whole
    thing with Zeros, and just set the ones that are not Zero (less code).
  */
  BeeUtilZeroMemory(pZdoNlmeMessage, size);

  /*
    Build the command.
  */
  pZdoNlmeMessage->msgType = gNlmeLeaveRequest_c;

#if gMgmt_Leave_rsp_d
  /* this would only occur if management leave command came from OTA */
  if(zbMgmtOptionRemoveChildren_c & ZbZdoPrivateData(gMgmtSelfLeaveMask))
  {
    ZbZdoPrivateData(gMgmtSelfLeaveMask) &= ~(zbMgmtOptionRemoveChildren_c);
    pZdoNlmeMessage->msgData.leaveReq.removeChildren = TRUE;
  }

  /* If the mgmt command was sent with the  */
  if(zbMgmtOptionReJoin_c & ZbZdoPrivateData(gMgmtSelfLeaveMask))
  {
    /* don't turn off bit yet in gMgmtSelfLeaveMask, it will be turned off later */
    pZdoNlmeMessage->msgData.leaveReq.rejoin = TRUE;
    ZbZdoPrivateData(gMgmtSelfLeaveMask) &= ~(zbMgmtOptionReJoin_c);
  }
#endif

  /* check if the ZDO or the application needs to remove the children. */
  if ( ZbZdoPublicData(gZdoStopMode) & gZdoStopMode_RemoveChildren_c)
  {
    ZbZdoPublicData(gZdoStopMode) &= ~(gZdoStopMode_RemoveChildren_c);
    pZdoNlmeMessage->msgData.leaveReq.removeChildren = TRUE;
  }

  pZdoNlmeMessage->msgData.leaveReq.reuseAddress = mDefaultReuseAddressPolicy_c;

  if (ZDO_NLME_SapHandler( pZdoNlmeMessage ))
  {
    /*
      Catch the error message if needed.
    */
  }
}

/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/

#if gHighSecurity_d
void ZDO_StartLinkStatus(){
  
  /* Jitter time to avoid synchronization with link status of others devices.
   Time in milliseconds. */
  uint8_t gNwkLinkStatusJitterInterval = GetRandomRange(0,10);
  
  /* Start the timer */
  ZbTMR_StartSingleShotTimer(gLinkStatusTimerID,
                           (ZbTmrSeconds( NlmeGetRequest(gNwkLinkStatusPeriod_c) ) + gNwkLinkStatusJitterInterval),
                           CustomLinkStatusTimeOutCallBack);

}
#endif

/******************************************************************************
*******************************************************************************
* Private Debug Stuff
*******************************************************************************
******************************************************************************/

/* None */
