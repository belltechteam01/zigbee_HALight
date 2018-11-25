/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file ASP.c
* This is the source file for the ASP module.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */

#include "EmbeddedTypes.h"
#include "fsl_os_abstraction.h"
#include "fsl_device_registers.h"

#include "Phy.h"
#include "PhyInterface.h"
#include "AspInterface.h"
#include "MemManager.h"
#include "FunctionLib.h"
#include "Panic.h"

#if gFsciIncluded_c
#include "FsciInterface.h"
#include "FsciCommands.h"
#include "FsciCommunication.h"
#endif


#if gAspCapability_d

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#define mFAD_THR_ResetValue         0x82
#define mANT_AGC_CTRL_ResetValue    0x40


/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */
enum {
  gDftNormal_c,
  gDftTxPattern_c,
  gDftTxRandom_c,
  gDftTxPnChipData_c,
  gDftTxExternalSrc_c,
  gDftTxNoMod_Carrier_c,
  gDftTxNoMod_ToneLFSR_c,
  gDftTxNoMod_ToneSelect_c,
};


/*! *********************************************************************************
*************************************************************************************
* Private functions prototype
*************************************************************************************
********************************************************************************** */
#if gFsciIncluded_c
static void fsciAspReqHandler(void *pData, void* param, uint32_t interfaceId);
static void AspSapMonitor(void *pData, void* param, uint32_t interfaceId);
#endif

void ASP_PRBS9_Load (void);

/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */

#if gFsciIncluded_c
static uint8_t mAspFsciBinding[gPhyInstancesCnt_c];
#endif

/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
* \brief  Initialize the ASP module
*
* \param[in]  phyInstance The instance of the PHY
* \param[in]  interfaceId The FSCI interface used
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 0
void ASP_Init( instanceId_t phyInstance, uint8_t interfaceId )
{
#if gFsciIncluded_c
    if( phyInstance < gPhyInstancesCnt_c )
    {
        mAspFsciBinding[phyInstance] = interfaceId;
        FSCI_RegisterOpGroup( gFSCI_AppAspOpcodeGroup_c, gFsciMonitorMode_c, fsciAspReqHandler, NULL, interfaceId);
        FSCI_RegisterOpGroup( gFSCI_AspSapId_c,          gFsciMonitorMode_c, AspSapMonitor,     NULL, interfaceId);
    }
#endif
}

/*! *********************************************************************************
* \brief  ASP SAP handler.
*
* \param[in]  pMsg        Pointer to the request message
* \param[in]  instanceId  The instance of the PHY
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 1
AspStatus_t APP_ASP_SapHandler(AppToAspMessage_t *pMsg, instanceId_t phyInstance)
{
    AspStatus_t status = gAspSuccess_c;
#if gFsciIncluded_c
    FSCI_Monitor( gFSCI_AspSapId_c,
                  pMsg,
                  NULL,
                  mAspFsciBinding[phyInstance] );
#endif
    switch( pMsg->msgType )
    {
    case aspMsgTypeGetTimeReq_c:
        Asp_GetTimeReq((uint64_t*)&pMsg->msgData.aspGetTimeReq.time);
        break;
    case aspMsgTypeXcvrWriteReq_c:
        status = Asp_XcvrWriteReq( pMsg->msgData.aspXcvrData.mode,
                                   pMsg->msgData.aspXcvrData.addr,
                                   pMsg->msgData.aspXcvrData.len,
                                   pMsg->msgData.aspXcvrData.data);
        break;
    case aspMsgTypeXcvrReadReq_c:
        status = Asp_XcvrReadReq( pMsg->msgData.aspXcvrData.mode,
                                  pMsg->msgData.aspXcvrData.addr,
                                  pMsg->msgData.aspXcvrData.len,
                                  pMsg->msgData.aspXcvrData.data);
        break;
    case aspMsgTypeSetFADState_c:
        status = Asp_SetFADState(pMsg->msgData.aspFADState);
        break;
    case aspMsgTypeSetFADThreshold_c:
        status = Asp_SetFADThreshold(pMsg->msgData.aspFADThreshold);
        break;
    case aspMsgTypeSetANTXState_c:
        status = Asp_SetANTXState(pMsg->msgData.aspANTXState);
        break;
    case aspMsgTypeGetANTXState_c:
        *((uint8_t*)&status) = Asp_GetANTXState();
        break;
    case aspMsgTypeSetPowerLevel_c:
        status = Asp_SetPowerLevel(pMsg->msgData.aspSetPowerLevelReq.powerLevel);
        break;
    case aspMsgTypeGetPowerLevel_c:
        *((uint8_t*)&status) = Asp_GetPowerLevel(); //remove compiler warning
        break;
    case aspMsgTypeTelecSetFreq_c:
        status = ASP_TelecSetFreq(pMsg->msgData.aspTelecsetFreq.channel);
        break;
    case aspMsgTypeTelecSendRawData_c:
        status = ASP_TelecSendRawData((uint8_t*)&pMsg->msgData.aspTelecSendRawData);
        break;
    case aspMsgTypeTelecTest_c:
        status = ASP_TelecTest(pMsg->msgData.aspTelecTest.mode);
        break;
    case aspMsgTypeSetLQIMode_c:
        status = Asp_SetLQIMode(pMsg->msgData.aspLQIMode);
        break;
    case aspMsgTypeGetRSSILevel_c:
        *((uint8_t*)&status) = Asp_GetRSSILevel(); //remove compiler warning
        break;
#if gMpmIncluded_d
    case aspMsgTypeSetMpmConfig_c:
        MPM_SetConfig(&pMsg->msgData.MpmConfig);
        break;
    case aspMsgTypeGetMpmConfig_c:
        MPM_GetConfig(&pMsg->msgData.MpmConfig);
        break;
#endif
    default:
        status = gAspInvalidRequest_c;// OR gAspInvalidParameter_c
        break;
    }
#if gFsciIncluded_c
    FSCI_Monitor( gFSCI_AspSapId_c,
                  pMsg,
                  (void*)&status,
                  gAspInterfaceId );
#endif
    return status;
}

/*! *********************************************************************************
* \brief  Returns the current PHY time
*
* \param[in]  time  location where the PHY time will be stored
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 2
void Asp_GetTimeReq(uint64_t *time)
{
    PhyTimeReadClock( time );
}

/*! *********************************************************************************
* \brief  Write XCVR registers
*
* \param[in]  mode   ZLL/XCVR access
* \param[in]  addr   address
* \param[in]  len    number of bytes to write
* \param[in]  pData  data o be written
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 3
AspStatus_t Asp_XcvrWriteReq (uint8_t mode, uint16_t addr, uint8_t len, uint8_t* pData)
{
    if (mode)
    {
        //Indirect access: XCVR regs
        FLib_MemCpy((void*)(XCVR + addr), pData, len);
    }
    else
    {
        //Direct access: ZLL regs
        FLib_MemCpy((void*)(ZLL + addr), pData, len);
    }

    return gAspSuccess_c;
}

/*! *********************************************************************************
* \brief  Read XCVR registers
*
* \param[in]  mode   Direct/Indirect access
* \param[in]  addr   XCVR address
* \param[in]  len    number of bytes to read
* \param[in]  pData  location where data will be stored
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 4
AspStatus_t Asp_XcvrReadReq  (uint8_t mode, uint16_t addr, uint8_t len, uint8_t* pData)
{
    if (mode)
    {
        //Indirect access: XCVR regs
        FLib_MemCpy(pData, (void*)(XCVR + addr), len);
    }
    else
    {
        //Direct access: ZLL regs
        FLib_MemCpy(pData, (void*)(ZLL + addr), len);
    }

    return gAspSuccess_c;
}

/*! *********************************************************************************
* \brief  Set Tx output power level
*
* \param[in]  powerLevel   The new power level: 0x03-0x1F (see documentation for details)
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 5
AspStatus_t Asp_SetPowerLevel( uint8_t powerLevel )
{
    if(powerLevel > gAspPowerLevel_16dBm)
        return gAspInvalidParameter_c;

    {
        uint8_t res;

        res = PhyPlmeSetPwrLevelRequest(powerLevel);

        if( res == gPhySuccess_c )
        {
            return gAspSuccess_c;
        }
        else
        {
            return gAspDenied_c;
        }
    }
}

/*! *********************************************************************************
* \brief  Read the current Tx power level
*
* \return  power level
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 6
uint8_t Asp_GetPowerLevel()
{
    return PhyPlmeGetPwrLevelRequest();
}

/*! *********************************************************************************
* \brief  Set the state of Active Promiscuous functionality
*
* \param[in]  state  new state 
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 7
AspStatus_t Asp_SetActivePromState(bool_t state)
{
    PhySetActivePromiscuous(state);
    return gAspSuccess_c;
}

/*! *********************************************************************************
* \brief  Set the state of Fast Antenna Diversity functionality
*
* \param[in]  state  new state 
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 8
AspStatus_t Asp_SetFADState(bool_t state)
{
    if( gPhySuccess_c != PhyPlmeSetFADStateRequest(state) )
    {
        return gAspDenied_c;
    }
    return gAspSuccess_c;
}

/*! *********************************************************************************
* \brief  Set the Fast Antenna Diversity threshold
*
* \param[in]  threshold 
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 9
AspStatus_t Asp_SetFADThreshold(uint8_t threshold)
{
    if( gPhySuccess_c != PhyPlmeSetFADThresholdRequest(threshold) )
    {
        return gAspDenied_c;
    }
    return gAspSuccess_c;
}

/*! *********************************************************************************
* \brief  Set the ANTX functionality
*
* \param[in]  state 
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 10
AspStatus_t Asp_SetANTXState(bool_t state)
{
    if( gPhySuccess_c != PhyPlmeSetANTXStateRequest(state) )
    {
        return gAspDenied_c;
    }
    return gAspSuccess_c;
}

/*! *********************************************************************************
* \brief  Get the ANTX functionality
*
* \return  current state
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 11
uint8_t Asp_GetANTXState(void)
{
  return PhyPlmeGetANTXStateRequest();
}

/*! *********************************************************************************
* \brief  Set the ANTX pad state
*
* \param[in]  antAB_on 
* \param[in]  rxtxSwitch_on 
*
* \return  status
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 12
uint8_t Asp_SetANTPadStateRequest(bool_t antAB_on, bool_t rxtxSwitch_on)
{
    return PhyPlmeSetANTPadStateRequest(antAB_on, rxtxSwitch_on);
}

/*! *********************************************************************************
* \brief  Set the ANTX pad strength
*
* \param[in]  hiStrength 
*
* \return  status
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 13
uint8_t Asp_SetANTPadStrengthRequest(bool_t hiStrength)
{
    return PhyPlmeSetANTPadStrengthRequest(hiStrength);
}

/*! *********************************************************************************
* \brief  Set the ANTX inverted pads
*
* \param[in]  invAntA  invert Ant_A pad
* \param[in]  invAntB  invert Ant_B pad
* \param[in]  invTx    invert Tx pad
* \param[in]  invRx    invert Rx pad
*
* \return  status
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 14
uint8_t Asp_SetANTPadInvertedRequest(bool_t invAntA, bool_t invAntB, bool_t invTx, bool_t invRx)
{
    return PhyPlmeSetANTPadInvertedRequest(invAntA, invAntB, invTx, invRx);
}

/*! *********************************************************************************
* \brief  Set the LQI mode
*
* \param[in]  mode 
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 15
AspStatus_t Asp_SetLQIMode(bool_t mode)
{
    if( gPhySuccess_c != PhyPlmeSetLQIModeRequest(mode) )
    {
        return gAspDenied_c;
    }
    return gAspSuccess_c;
}

/*! *********************************************************************************
* \brief  Get the last RSSI level
*
* \return  RSSI
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 16
uint8_t Asp_GetRSSILevel(void)
{
  return PhyPlmeGetRSSILevelRequest();
}

/*! *********************************************************************************
* \brief  Set current channel
*
* \param[in]  channel  channel number (11-26)
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 17
AspStatus_t ASP_TelecSetFreq(uint8_t channel)
{
    PhyPlmeForceTrxOffRequest();
    if( gPhySuccess_c != PhyPlmeSetCurrentChannelRequest(channel,0) )
    {
        return gAspInvalidParameter_c;
    }

    return gAspSuccess_c;
}

/*! *********************************************************************************
* \brief  Send a raw data frame OTA
*
* \param[in]  dataPtr  raw data
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 18
AspStatus_t ASP_TelecSendRawData(uint8_t* dataPtr)
{
    dataPtr[0] += 2; /* Add FCS length to PSDU Length*/

    // Validate the length
    if(dataPtr[0] > gMaxPHYPacketSize_c)
        return gAspTooLong_c;

    //Force Idle
    PhyPlmeForceTrxOffRequest();
    XCVR_BWR_TX_DIG_CTRL_DFT_MODE(XCVR, gDftNormal_c);
    ZLL_BWR_SEQ_CTRL_STS_CONTINUOUS_EN(ZLL, 0);

    // Load the TX PB: load the PSDU Lenght byte but not the FCS bytes
    FLib_MemCpy( (void*)&ZLL_PKT_BUFFER0, dataPtr, dataPtr[0] + 1 - 2);
    // Program a Tx sequence
    ZLL_BWR_PHY_CTRL_XCVSEQ( ZLL, gTX_c );
    return gAspSuccess_c;
}

/*! *********************************************************************************
* \brief  Set Telec test mode
*
* \param[in]  mode  Telec test mode
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 19
AspStatus_t ASP_TelecTest(uint8_t mode)
{
    uint8_t channel;
    static bool_t fracSet = FALSE;

    // Get current channel number
    channel = PhyPlmeGetCurrentChannelRequest(0);

    if( fracSet )
    {
        ASP_TelecSetFreq(channel);
        fracSet = FALSE;
    }

    switch( mode )
    {
    case gTestForceIdle_c:  //ForceIdle();
        PhyPlmeForceTrxOffRequest();
        XCVR_BWR_TX_DIG_CTRL_DFT_MODE(XCVR, gDftNormal_c);
        XCVR_BWR_TSM_CTRL_FORCE_TX_EN(XCVR, 0);
        XCVR_BWR_DTEST_CTRL_DTEST_EN(XCVR, 0);
        XCVR_BWR_TX_DIG_CTRL_DFT_EN(XCVR, 0);
        XCVR_BWR_TX_DIG_CTRL_LFSR_EN(XCVR, 0);
        ZLL_BWR_SEQ_CTRL_STS_CONTINUOUS_EN(ZLL, 0);
        break;

    case gTestPulseTxPrbs9_c:  // Continuously transmit a PRBS9 pattern.
        XCVR_BWR_TX_DIG_CTRL_DFT_MODE(XCVR, gDftNormal_c);
        ASP_PRBS9_Load(); // Load the TX RAM
        //Enable continuous TX mode
        ZLL_BWR_SEQ_CTRL_STS_CONTINUOUS_EN(ZLL, 1);
        ZLL_BWR_PHY_CTRL_XCVSEQ( ZLL, gTX_c );
        break;

    case gTestContinuousRx_c:  // Sets the device into continuous RX mode
        XCVR_BWR_TX_DIG_CTRL_DFT_MODE(XCVR,gDftNormal_c);
        // Set length of data in DUAL_PAN_DWELL register
        ZLL_BWR_DUAL_PAN_CTRL_DUAL_PAN_DWELL(ZLL, 127);
        //Enable continuous RX mode
        ZLL_BWR_SEQ_CTRL_STS_CONTINUOUS_EN(ZLL, 1);
        ZLL_BWR_PHY_CTRL_XCVSEQ( ZLL, gRX_c );
        break;

    case gTestContinuousTxMod_c:  // Sets the device to continuously transmit a 10101010 pattern
        XCVR_BWR_TX_DIG_CTRL_DFT_MODE(XCVR,gDftTxPattern_c);
        XCVR_BWR_TX_DIG_CTRL_DFT_EN(XCVR, 1);
        XCVR_WR_TX_DFT_MOD_PAT(XCVR,0xAAAAAAAA);
        XCVR_BWR_DTEST_CTRL_DTEST_EN(XCVR, 1);
        ZLL_BWR_SEQ_CTRL_STS_CONTINUOUS_EN(ZLL, 1);
        XCVR_BWR_TSM_CTRL_FORCE_TX_EN(XCVR, 1);
        break;

    case gTestContinuousTxNoMod_c: // Sets the device to continuously transmit an unmodulated CW
        //Enable unmodulated TX
        XCVR_BWR_TX_DIG_CTRL_DFT_MODE(XCVR,gDftTxNoMod_Carrier_c);
        //Enable continuous TX mode
        XCVR_BWR_DTEST_CTRL_DTEST_EN(XCVR, 1);
        ZLL_BWR_SEQ_CTRL_STS_CONTINUOUS_EN(ZLL, 1);
        fracSet = TRUE;
        // Program a Tx sequence
        ZLL_BWR_PHY_CTRL_XCVSEQ( ZLL, gTX_c );
        break;

    case gTestContinuousTx2Mhz_c:
    case gTestContinuousTx200Khz_c:
        /* Not available for this IC */
        return gAspInvalidRequest_c;

    case gTestContinuousTx1MbpsPRBS9_c:
        XCVR_BWR_TX_DIG_CTRL_DFT_MODE(XCVR,gDftTxPnChipData_c);
        XCVR_BWR_TX_DIG_CTRL_DFT_LFSR_LEN(XCVR, 0); //length 9
        XCVR_BWR_TX_DIG_CTRL_LFSR_EN(XCVR,1);
        XCVR_BWR_DTEST_CTRL_DTEST_EN(XCVR, 1);
        ZLL_BWR_SEQ_CTRL_STS_CONTINUOUS_EN(ZLL, 1);
        XCVR_BWR_TSM_CTRL_FORCE_TX_EN(XCVR, 1);
        break;

    case gTestContinuousTxExternalSrc_c:
        XCVR_BWR_TX_DIG_CTRL_DFT_MODE(XCVR,gDftTxExternalSrc_c);
        //Enable continuous TX mode
        XCVR_BWR_DTEST_CTRL_DTEST_EN(XCVR, 1);
        ZLL_BWR_SEQ_CTRL_STS_CONTINUOUS_EN(ZLL, 1);
        // Program a Tx sequence
        ZLL_BWR_PHY_CTRL_XCVSEQ( ZLL, gTX_c );
        break;

    case gTestContinuousTxModZero_c:
        //Enable unmodulated TX
        XCVR_BWR_TX_DIG_CTRL_DFT_MODE(XCVR,gDftTxPattern_c);
        XCVR_BWR_TX_DIG_CTRL_DFT_EN(XCVR, 1);
        XCVR_WR_TX_DFT_MOD_PAT(XCVR,0x00000000);

        XCVR_BWR_DTEST_CTRL_DTEST_EN(XCVR, 1);
        ZLL_BWR_SEQ_CTRL_STS_CONTINUOUS_EN(ZLL, 1);
        XCVR_BWR_TSM_CTRL_FORCE_TX_EN(XCVR, 1);
        break;

    case gTestContinuousTxModOne_c:
        //Enable unmodulated TX
        XCVR_BWR_TX_DIG_CTRL_DFT_MODE(XCVR,gDftTxPattern_c);
        XCVR_BWR_TX_DIG_CTRL_DFT_EN(XCVR, 1);
        XCVR_WR_TX_DFT_MOD_PAT(XCVR,0xFFFFFFFF);

        XCVR_BWR_DTEST_CTRL_DTEST_EN(XCVR, 1);
        ZLL_BWR_SEQ_CTRL_STS_CONTINUOUS_EN(ZLL, 1);
        XCVR_BWR_TSM_CTRL_FORCE_TX_EN(XCVR, 1);
        break;
    default:
        return gAspInvalidParameter_c;
    }

    return gAspSuccess_c;
}

/*! *********************************************************************************
* \brief  Return the instance of the PHY associated with the FSCI interface
*
* \param[in]  interfaceId  FSCI interface
*
* \return  insance
*
********************************************************************************** */
#if gFsciIncluded_c
#undef mFuncId_c
#define mFuncId_c 20
static uint32_t getPhyInstance( uint32_t interfaceId )
{
    uint32_t i;

    for( i=0; i<gPhyInstancesCnt_c; i++ )
        if( mAspFsciBinding[i] == interfaceId )
            return i;

    return 0;
}

/*! *********************************************************************************
* \brief  Handle ASP requests received from FSCI
*
* \param[in]  pData        monitored message
* \param[in]  param        
* \param[in]  interfaceId  FSCI interface 
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 21
static void fsciAspReqHandler(void *pData, void* param, uint32_t interfaceId)
{
    clientPacket_t *pClientPacket = ((clientPacket_t*)pData);
    uint8_t *pMsg = pClientPacket->structured.payload;

    pMsg -= sizeof(AppAspMsgType_t);
    ((AppToAspMessage_t*)pMsg)->msgType = (AppAspMsgType_t)pClientPacket->structured.header.opCode;

    APP_ASP_SapHandler( (AppToAspMessage_t*)pMsg, getPhyInstance( interfaceId ) );
    MEM_BufferFree(pData);
}

/*! *********************************************************************************
* \brief  Monitor the ASP Requests and Responses
*
* \param[in]  pData        monitored message
* \param[in]  param        
* \param[in]  interfaceId  FSCI interface 
*
* \return  AspStatus_t
*
********************************************************************************** */
#undef mFuncId_c
#define mFuncId_c 22
static void AspSapMonitor(void *pData, void* param, uint32_t interfaceId)
{
    clientPacket_t *pFsciPacket = MEM_BufferAlloc( sizeof(clientPacket_t) );
    AppToAspMessage_t *pReq = (AppToAspMessage_t*)pData;
    uint8_t *p;

    if( NULL == pFsciPacket )
    {
        FSCI_Error( gFsciOutOfMessages_c, interfaceId );
        return;
    }

    p = pFsciPacket->structured.payload;

    if( NULL == param ) // Requests
    {
        pFsciPacket->structured.header.opGroup = gFSCI_AppAspOpcodeGroup_c;
        pFsciPacket->structured.header.opCode = pReq->msgType;

        switch( pReq->msgType )
        {
        case aspMsgTypeGetTimeReq_c:
            break;
        case aspMsgTypeXcvrWriteReq_c:
        case aspMsgTypeXcvrReadReq_c:
            *p++ = pReq->msgData.aspXcvrData.mode;
            *((uint16_t*)p) = pReq->msgData.aspXcvrData.addr;
            p += sizeof(uint16_t);
            *p++ = pReq->msgData.aspXcvrData.len;
            if( pReq->msgType == aspMsgTypeXcvrWriteReq_c )
            {
                FLib_MemCpy( p, pReq->msgData.aspXcvrData.data,
                             pReq->msgData.aspXcvrData.len );
                p += pReq->msgData.aspXcvrData.len;
            }
            break;
        case aspMsgTypeSetFADState_c:
            FLib_MemCpy( p, &pReq->msgData.aspFADState, sizeof(pReq->msgData.aspFADState) );
            p += sizeof(pReq->msgData.aspFADState);
            break;
        case aspMsgTypeSetFADThreshold_c:
            FLib_MemCpy( p, &pReq->msgData.aspFADThreshold, sizeof(pReq->msgData.aspFADThreshold) );
            p += sizeof(pReq->msgData.aspFADThreshold);
            break;
        case aspMsgTypeSetANTXState_c:
            FLib_MemCpy( p, &pReq->msgData.aspANTXState, sizeof(pReq->msgData.aspANTXState) );
            p += sizeof(pReq->msgData.aspANTXState);
            break;
        case aspMsgTypeGetANTXState_c:
            /* Nothing to do here */
            break;

        case aspMsgTypeSetPowerLevel_c:
            FLib_MemCpy( p, &pReq->msgData.aspSetPowerLevelReq, sizeof(pReq->msgData.aspSetPowerLevelReq) );
            p += sizeof(pReq->msgData.aspSetPowerLevelReq);
            break;
        case aspMsgTypeGetPowerLevel_c:
            /* Nothing to do here */
            break;
        case aspMsgTypeTelecSetFreq_c:
            FLib_MemCpy( p, &pReq->msgData.aspTelecsetFreq, sizeof(pReq->msgData.aspTelecsetFreq) );
            p += sizeof(pReq->msgData.aspTelecsetFreq);
            break;
        case aspMsgTypeTelecSendRawData_c:
            FLib_MemCpy( p, &pReq->msgData.aspTelecSendRawData, sizeof(pReq->msgData.aspTelecSendRawData) );
            p += sizeof(pReq->msgData.aspTelecSendRawData);
            break;
        case aspMsgTypeTelecTest_c:
            FLib_MemCpy( p, &pReq->msgData.aspTelecTest, sizeof(pReq->msgData.aspTelecTest) );
            p += sizeof(pReq->msgData.aspTelecTest);
            break;
        case aspMsgTypeSetLQIMode_c:
            FLib_MemCpy(p, &pReq->msgData.aspLQIMode, sizeof(pReq->msgData.aspLQIMode) );
            p += sizeof(pReq->msgData.aspLQIMode);
            break;
        case aspMsgTypeGetRSSILevel_c:
            /* Nothing to do here */
            break;
        }
    }
    else // Confirms / Indications
    {
        pFsciPacket->structured.header.opGroup = gFSCI_AspAppOpcodeGroup_c;
        pFsciPacket->structured.header.opCode = pReq->msgType;

        *p++ = *((uint8_t*)param);/* copy status */

        switch( pReq->msgType )
        {
        case aspMsgTypeGetTimeReq_c:
            FLib_MemCpy( p, &pReq->msgData.aspGetTimeReq.time , sizeof(aspGetTimeReq_t) );
            p += sizeof(aspGetTimeReq_t);
            break;
        case aspMsgTypeGetMpmConfig_c:
            FLib_MemCpy( p, &pReq->msgData.MpmConfig , sizeof(mpmConfig_t) );
            p += sizeof(mpmConfig_t);
            break;
        case aspMsgTypeXcvrReadReq_c:
            *p++ = pReq->msgData.aspXcvrData.len; /* copy length */
            FLib_MemCpy( p, pReq->msgData.aspXcvrData.data, pReq->msgData.aspXcvrData.len );
            p += pReq->msgData.aspXcvrData.len;
            break;
        }

    }

    /* Send data over the serial interface */
    pFsciPacket->structured.header.len = (fsciLen_t)(p - pFsciPacket->structured.payload);

    if ( pFsciPacket->structured.header.len )
        FSCI_transmitFormatedPacket( pFsciPacket, interfaceId );
    else
        MEM_BufferFree( pFsciPacket );
}

#endif /* gFsciIncluded_c */

/*! *********************************************************************************
* \brief  Generate and load a PRBS9 pattern into Tx buffer
*
********************************************************************************** */
void ASP_PRBS9_Load (void)
{
  uint8_t c1; /* Byte counter */
  uint8_t c2; /* Bit counter */
  uint16_t t1; /* LFSR */
  uint16_t t2; /* LFSR output */
  uint16_t t3; /* LFSR feedback tap */
  uint8_t t4; /* Assembled transmit byte */
  uint8_t *pTxBuffer = (uint8_t*)&ZLL_PKT_BUFFER0;

  pTxBuffer[0] = 64;
    t1 = 0x01FF; /* Initialize the LFSR */
    for (c1=1; c1<=64; c1++) /* Byte counter */
    {
        t4 = 0x00; /* Initialize the byte */
        for (c2=0; c2<8; c2++) /* Bit counter */
        {
            t2 = (t1 & 0x0001); /* LFSR output */
            if (t2 == 0x0001)
            {
                t4 = t4 | 0x80; /* Set/Clear byte based on LFSR output */
            }
            if (c2 != 7)
            {
                t4 = t4 >> 1; /* LSBit will be first bit out of LFSR */
            }
            t3 = ((t1 & 0x0010) >> 4); /* LFSR tap */
            t1 = t1 >> 1; /* Now shift the LFSR */
            if (t2 == t3) /* Set/Clr the LFSR MSBit */
            {
                t1 = t1 & 0xFEFF;
            }
            else
            {
                t1 = t1 | 0x0100;
            }
        }
      pTxBuffer[c1] = t4;
    }
}

#endif /* gAspCapability_d */