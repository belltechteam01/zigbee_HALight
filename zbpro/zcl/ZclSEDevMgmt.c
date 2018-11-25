/*! @file	ZclSEDevMgmt.c
 *
 * @brief	This source file describes specific functionality implemented
 *			for the Device Management cluster.
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
 * 			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   		contributors may be used to endorse or promote products derived from this
 *   		software without specific prior written permission.
 *
 * 			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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


#include "EmbeddedTypes.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"

#include "SEProfile.h"
#include "ZclSE.h"
#include "ZclSEDevMgmt.h"

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
static zbStatus_t zclDevMgmt_ProcessGetChangeOfTenancyReq(afAddrInfo_t *pAddrInfo, uint8_t zclTransactionId);
static zbStatus_t zclDevMgmt_ProcessGetChangeOfSupplierReq(afAddrInfo_t *pAddrInfo, uint8_t zclTransactionId);
static zbStatus_t zclDevMgmt_ProcessGetChangeSupplyReq(afAddrInfo_t *pAddrInfo, uint8_t zclTransactionId);
static zbStatus_t zclDevMgmt_ProcessReqNewPasswordReq(afAddrInfo_t *pAddrInfo, uint8_t zclTransactionId, zclCmdDevMgmt_ReqNewPasswordReq_t *pReq);
static zbStatus_t zclDevMgmt_ProcessChangeSupplyRsp(afAddrInfo_t *pAddrInfo, uint8_t zclTransactionId, zclCmdDevMgmt_ChangeSupplyRsp_t *pReq);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

typedef enum DevMgmtEvtType_tag{
	ChangeOfTenancy_c,
	ChangeOfSupplier_c,
	ChangeSupply_c
}DevMgmtEvtType_t;


/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

const zbClusterId_t maClusterId = {gaZclClusterDevMgmt_c};

//static zbTmrTimerID_t 	mDevMgmtEventTimerID;
//static DevMgmt_Evt_t	maDevMgmtEvtTable[2];


DevMgmt_ChangeOfTenancy_t	mZclDevMgmt_ChangeOfTenancy;
DevMgmt_ChangeOfSupplier_t	mZclDevMgmt_ChangeOfSupplier;
DevMgmt_ChangeSupply_t		mZclDevMgmt_ChangeSupply;
DevMgmt_Password_t   	    mZclDevMgmt_Password;

DevMgmt_SupplyCtlSetAttr_t               gZclDevMgmt_SupplyCtlSetAttr;
DevMgmt_SupplierCtlSetAttr_t             gZclDevMgmt_SupplierCtlSetAttr;
DevMgmt_TenancyCtlSetAttr_t              gZclDevMgmt_TenancyCtlSetAttr;
DevMgmt_BackhaulCtlSetAttr_t             gZclDevMgmt_BackhaulCtlSetAttr;
DevMgmt_HANCtlSetAttr_t                  gZclDevMgmt_HANCtlSetAttr;
DevMgmt_SupplierSetAttr_t                gZclDevMgmt_SupplierSetAttr;
DevMgmt_PriceEvtCfgSetAttr_t             gZclDevMgmt_PriceEvtCfgSetAttr;
DevMgmt_MeteringEvtCfgSetAttr_t          gZclDevMgmt_MeteringEvtCfgSetAttr;
DevMgmt_MessagingEvtCfgSetAttr_t         gZclDevMgmt_MessagingEvtCfgSetAttr;
DevMgmt_PrepayEvtCfgSetAttr_t            gZclDevMgmt_PrepayEvtCfgSetAttr;
DevMgmt_CalendarEvtCfgSetAttr_t          gZclDevMgmt_CalendarEvtCfgSetAttr;
DevMgmt_DevMgmtEvtCfgSetAttr_t           gZclDevMgmt_DevMgmtEvtCfgSetAttr;
DevMgmt_TunnelEvtCfgSetAttr_t            gZclDevMgmt_TunnelEvtCfgSetAttr;
DevMgmt_OTAEvtCfgSetAttr_t               gZclDevMgmt_OTAEvtCfgSetAttr;

 /* [R1] D.10.2.2.1 Supply Control Attribute Set */
const zclAttrDef_t gaZclDevMgmt_SupplyCtlSetAttrDef[] = {
  {gZclAttrIdDevMgmt_ProposedChangeSupplyImplTime_c,    gZclDataTypeUTCTime_c,  gZclAttrFlagsRdOnly_c, sizeof(ZCLTime_t), (void *) &gZclDevMgmt_SupplyCtlSetAttr.ProposedChangeSupplyImplTime},
  {gZclAttrIdDevMgmt_ProposedChangeSupplyStatus_c,      gZclDataTypeEnum8_c,    gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclDevMgmt_SupplyCtlSetAttr.ProposedChangeSupplyStatus}
};

 /* [R1] D.10.2.2.2 Supplier Control Attribute Set */
const zclAttrDef_t gaZclDevMgmt_SupplierCtlSetAttrDef[] = {
  {gZclAttrIdDevMgmt_ProviderID_c,    gZclDataTypeUint32_c,  gZclAttrFlagsRdOnly_c, sizeof(ProviderID_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ProviderID},
  {gZclAttrIdDevMgmt_ProviderName_c,  gZclDataTypeOctetStr_c,    gZclAttrFlagsRdOnly_c, sizeof(zclStr17_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ProviderName},
  {gZclAttrIdDevMgmt_ProposedProviderID_c,  gZclDataTypeUint32_c,  gZclAttrFlagsRdOnly_c, sizeof(ProviderID_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ProposedProviderID},
  {gZclAttrIdDevMgmt_ProposedProviderName_c,  gZclDataTypeOctetStr_c,    gZclAttrFlagsRdOnly_c, sizeof(zclStr17_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ProposedProviderName},
  {gZclAttrIdDevMgmt_ProposedProviderChangeDate_c,  gZclDataTypeUTCTime_c,    gZclAttrFlagsRdOnly_c, sizeof(ZCLTime_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ProposedProviderChangeDate},
  {gZclAttrIdDevMgmt_ProposedProviderChangeCtl_c,  gZclDataTypeBitmap32_c,    gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ProposedProviderChangeCtl},
  {gZclAttrIdDevMgmt_ExProviderID_c,    gZclDataTypeUint32_c,  gZclAttrFlagsRdOnly_c, sizeof(ProviderID_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ExProviderID},
  {gZclAttrIdDevMgmt_ExProviderName_c,  gZclDataTypeOctetStr_c,    gZclAttrFlagsRdOnly_c, sizeof(zclStr17_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ExProviderName},
  {gZclAttrIdDevMgmt_ExProposedProviderID_c,  gZclDataTypeUint32_c,  gZclAttrFlagsRdOnly_c, sizeof(ProviderID_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ExProposedProviderID},
  {gZclAttrIdDevMgmt_ExProposedProviderName_c,  gZclDataTypeOctetStr_c,    gZclAttrFlagsRdOnly_c, sizeof(zclStr17_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ExProposedProviderName},
  {gZclAttrIdDevMgmt_ExProposedProviderChangeDate_c,  gZclDataTypeUTCTime_c,    gZclAttrFlagsRdOnly_c, sizeof(ZCLTime_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ExProposedProviderChangeDate},
  {gZclAttrIdDevMgmt_ExProposedProviderChangeCtl_c,  gZclDataTypeBitmap32_c,    gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void *) &gZclDevMgmt_SupplierCtlSetAttr.ExProposedProviderChangeCtl}
};

 /* [R1] D.10.2.2.3 tenancy Control Attribute Set */
const zclAttrDef_t gaZclDevMgmt_TenancyCtlSetAttrDef[] = {
  {gZclAttrIdDevMgmt_ChangeOfTenancyTime_c,       gZclDataTypeUTCTime_c,    gZclAttrFlagsRdOnly_c, sizeof(ZCLTime_t), (void *) &gZclDevMgmt_TenancyCtlSetAttr.ChangeOfTenancyTime},
  {gZclAttrIdDevMgmt_ProposedTenancyChangeCtl_c,  gZclDataTypeBitmap32_c,   gZclAttrFlagsRdOnly_c, sizeof(uint32_t),  (void *) &gZclDevMgmt_TenancyCtlSetAttr.ProposedTenancyChangeCtl}
};

 /* [R1] D.10.2.2.4 Backhaul Control Attribute Set */
const zclAttrDef_t gaZclDevMgmt_BackhaulCtlSetAttrDef[] = {
  {gZclAttrIdDevMgmt_WANStatus_c,       gZclDataTypeEnum8_c,    gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclDevMgmt_BackhaulCtlSetAttr.WANStatus}
};

/* [R1] D.10.2.2.5 HAN Control Attribute Set */
const zclAttrDef_t gaZclDevMgmt_HANCtlSetAttrDef[] = {
  {gZclAttrIdDevMgmt_LowMedThreshold_c,       gZclDataTypeUint32_c,    gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void *) &gZclDevMgmt_HANCtlSetAttr.LowMedThreshold},
  {gZclAttrIdDevMgmt_MedHighThreshold_c,      gZclDataTypeUint32_c,    gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void *) &gZclDevMgmt_HANCtlSetAttr.MedHighThreshold}
};

/* [R1] D.10.3.2.1 Supplier Attribute Set */
const zclAttrDef_t gaZclDevMgmt_SupplierSetAttrDef[] = {
  {gZclAttrIdDevMgmt_ClientProviderID_c,    gZclDataTypeUint32_c,  gZclAttrFlagsClientRdOnly_c, sizeof(ProviderID_t), (void *) &gZclDevMgmt_SupplierSetAttr.ProviderID},
  {gZclAttrIdDevMgmt_ClientExProviderID_c,  gZclDataTypeUint32_c,  gZclAttrFlagsClientRdOnly_c, sizeof(ProviderID_t), (void *) &gZclDevMgmt_SupplierSetAttr.ExProviderID},
  {gZclAttrIdDevMgmt_SupplyTamperState_c,   gZclDataTypeBool_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 2, (void *) &gZclDevMgmt_SupplierSetAttr.SupplyTamperState}
};

/* [R1] D.10.3.2.2 Price Event Configuration Attribute Set */
const zclAttrDef_t gaZclDevMgmt_PriceEvtCfgSetAttrDef[] = {
   {gZclAttrIdDevMgmt_TOUTariffActivation_c,   gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 7, (void *) &gZclDevMgmt_PriceEvtCfgSetAttr.TOUTariffActivation},
   {gZclAttrIdDevMgmt_PublishPriceRcvd_c,      gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 67, (void *) &gZclDevMgmt_PriceEvtCfgSetAttr.PublishPriceReceived}
};

/* [R1] D.10.3.2.3 Metering Event Configuration Attribute Set */
const zclAttrDef_t gaZclDevMgmt_MeteringEvtCfgSetAttrDef[] = {
   {gZclAttrIdDevMgmt_CheckMeter_c,   gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 0x29, (void *) &gZclDevMgmt_MeteringEvtCfgSetAttr.CheckMeter},
   {gZclAttrIdDevMgmt_BurstDetect_c,  gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 7, (void *) &gZclDevMgmt_MeteringEvtCfgSetAttr.BurstDetect},
   {gZclAttrIdDevMgmt_InletTemperatureSensorFault_c,  gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c, sizeof(bool_t), (void *) &gZclDevMgmt_MeteringEvtCfgSetAttr.InletTemperatureSensorFault},
   {gZclAttrIdDevMgmt_OutletTemperatureSendorFault_c,  gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c,  sizeof(bool_t), (void *) &gZclDevMgmt_MeteringEvtCfgSetAttr.OutletTemperatureSendorFault},
   {gZclAttrIdDevMgmt_ReverseFlow_c,  gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 5, (void *) &gZclDevMgmt_MeteringEvtCfgSetAttr.ReverseFlow},
   {gZclAttrIdDevMgmt_MeasurementSystemError_c,  gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 39, (void *) &gZclDevMgmt_MeteringEvtCfgSetAttr.MeasurementSystemError},
   {gZclAttrIdDevMgmt_ManufacturerSpecificA_c,  gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 8, (void *) &gZclDevMgmt_MeteringEvtCfgSetAttr.ManufacturerSpecificA},
   {gZclAttrIdDevMgmt_MirrorResponseCommandManagement_c,  gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 5, (void *) &gZclDevMgmt_MeteringEvtCfgSetAttr.MirrorResponseCommandManagement}
};

/* [R1] D.10.3.2.4 Messaging Event Configuration Attribute Set */
const zclAttrDef_t gaZclDevMgmt_MessagingEvtCfgSetAttrDef[] = {
   {gZclAttrIdDevMgmt_DisplayMessageReceived_c,   gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 7, (void *) &gZclDevMgmt_MessagingEvtCfgSetAttr.DisplayMessageReceived},
   {gZclAttrIdDevMgmt_MessageConfirmationSent_c,  gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c, sizeof(bool_t), (void *) &gZclDevMgmt_MessagingEvtCfgSetAttr.DisplayMessageReceived}
};

/* [R1] D.10.3.2.5 Prepayment Event Configuration Attribute Set */
const zclAttrDef_t gaZclDevMgmt_PrepayEvtCfgSetAttrDef[] = {
   {gZclAttrIdDevMgmt_LowCredit_c,   gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 5, (void *) &gZclDevMgmt_PrepayEvtCfgSetAttr.LowCredit},
   {gZclAttrIdDevMgmt_SupplyON_c,   gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 6, (void *) &gZclDevMgmt_PrepayEvtCfgSetAttr.SupplyON},
   {gZclAttrIdDevMgmt_PhysicalAttackOnPrepayMeter_c,   gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 18, (void *) &gZclDevMgmt_PrepayEvtCfgSetAttr.PhysicalAttackonthePrepayMeter},
   {gZclAttrIdDevMgmt_ModeCredit2Prepay_c,   gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 2, (void *) &gZclDevMgmt_PrepayEvtCfgSetAttr.ModeCredit2Prepay},
   {gZclAttrIdDevMgmt_SelectAvailableEmergencyCreditReceived_c,   gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c, 41, (void *) &gZclDevMgmt_PrepayEvtCfgSetAttr.SelectAvailableEmergencyCreditReceived},
};

/* [R1] D.10.3.2.6 Calendar Event Configuration Attribute Set */
const zclAttrDef_t gaZclDevMgmt_CalendarEvtCfgSetAttrDef[] = {
    {gZclAttrIdDevMgmt_CalendarClusterNotFound_c,    gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c,    sizeof(uint8_t),    (void *) &gZclDevMgmt_CalendarEvtCfgSetAttr.CalendarClusterNotFound},
    {gZclAttrIdDevMgmt_PublishCalendarReceived_c,    gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c,    21,    (void *) &gZclDevMgmt_CalendarEvtCfgSetAttr.PublishCalendarReceived}
};

/* [R1] D.10.3.2.7 Dev Mgmt Event Configuration Attribute Set */
const zclAttrDef_t gaZclDevMgmt_DevMgmtEvtCfgSetAttrDef[] = {
    {gZclAttrIdDevMgmt_Password1Change_c,    gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c,   4,    (void *) &gZclDevMgmt_DevMgmtEvtCfgSetAttr.Password1Change},
    {gZclAttrIdDevMgmt_SupplyON_c,    gZclDataTypeBitmap8_c,    gZclAttrFlagsIsClientAttribute_c | gZclAttrFlagsIsVector_c,    9,    (void *) &gZclDevMgmt_DevMgmtEvtCfgSetAttr.SupplyON},
    {gZclAttrIdDevMgmt_PublishCoTReceived_c,    gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c,    29,    (void *) &gZclDevMgmt_DevMgmtEvtCfgSetAttr.PublishCoTReceived}
};

/* [R1] D.10.3.2.8 Tunnel Event Configuration Attribute Set */
const zclAttrDef_t gaZclDevMgmt_TunnelEvtCfgSetAttrDef[] = {
    {gZclAttrIdDevMgmt_TunnelingClusterNotFound_c,    gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c,    2,    (void *) &gZclDevMgmt_TunnelEvtCfgSetAttr.TunnelingClusterNotFound}
};

/* [R1] D.10.3.2.9 OTA Event Configuration Attribute Set */
const zclAttrDef_t gaZclDevMgmt_OTAEvtCfgSetAttrDef[] = {
    {gZclAttrIdDevMgmt_FirmwareReadyForActivation_c,    gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c,    5,    (void *) &gZclDevMgmt_OTAEvtCfgSetAttr.FirmwareReadyForActivation},
    {gZclAttrIdDevMgmt_OTAReceived_c,    gZclDataTypeBitmap8_c,    gZclAttrFlagsClientRdOnly_c | gZclAttrFlagsIsVector_c,    3,    (void *) &gZclDevMgmt_OTAEvtCfgSetAttr.OTAReceived}
};

 /* [R1] D.10.2.2 Server Attributes */
const zclAttrSet_t gaZclDevMgmtServerAttrSet[] = {
  {gZclAttrDevMgmt_SupplyCtlSet_c,      (void *)&gaZclDevMgmt_SupplyCtlSetAttrDef,      NumberOfElements(gaZclDevMgmt_SupplyCtlSetAttrDef)},
  {gZclAttrDevMgmt_SupplierCtlSet_c,    (void *)&gaZclDevMgmt_SupplierCtlSetAttrDef,    NumberOfElements(gaZclDevMgmt_SupplierCtlSetAttrDef)},
  {gZclAttrDevMgmt_TenancyCtlSet_c,     (void *)&gaZclDevMgmt_TenancyCtlSetAttrDef,     NumberOfElements(gaZclDevMgmt_TenancyCtlSetAttrDef)},
  {gZclAttrDevMgmt_BackhaulCtlSet_c,    (void *)&gaZclDevMgmt_BackhaulCtlSetAttrDef,    NumberOfElements(gaZclDevMgmt_BackhaulCtlSetAttrDef)},
  {gZclAttrDevMgmt_HANCtlSet_c,         (void *)&gaZclDevMgmt_HANCtlSetAttrDef,         NumberOfElements(gaZclDevMgmt_HANCtlSetAttrDef)}
};

const zclAttrSetList_t gZclDevMgmtServerAttrSetList = {
  NumberOfElements(gaZclDevMgmtServerAttrSet),
  gaZclDevMgmtServerAttrSet
};

 /* [R1] D.10.3.2 Client Attributes */
const zclAttrSet_t gaZclDevMgmtClientAttrSet[] = {
  {gZclAttrDevMgmt_SupplierAttrSet_c,           (void *)&gaZclDevMgmt_SupplierSetAttrDef,       NumberOfElements(gaZclDevMgmt_SupplierSetAttrDef)},
  {gZclAttrDevMgmt_PriceEvtCfgAttrSet_c,        (void *)&gaZclDevMgmt_PriceEvtCfgSetAttrDef,    NumberOfElements(gaZclDevMgmt_PriceEvtCfgSetAttrDef)},
  {gZclAttrDevMgmt_MeteringEvtCfgAttrSet_c,     (void *)&gaZclDevMgmt_MeteringEvtCfgSetAttrDef,     NumberOfElements(gaZclDevMgmt_MeteringEvtCfgSetAttrDef)},
  {gZclAttrDevMgmt_MessagingEvtCfgAttrSet_c,    (void *)&gaZclDevMgmt_MessagingEvtCfgSetAttrDef,    NumberOfElements(gaZclDevMgmt_MessagingEvtCfgSetAttrDef)},
  {gZclAttrDevMgmt_PrepayEvtCfgAttrSet_c,       (void *)&gaZclDevMgmt_PrepayEvtCfgSetAttrDef,         NumberOfElements(gaZclDevMgmt_PrepayEvtCfgSetAttrDef)},
  {gZclAttrDevMgmt_CalendarEvtCfgAttrSet_c,     (void *)&gaZclDevMgmt_CalendarEvtCfgSetAttrDef,         NumberOfElements(gaZclDevMgmt_CalendarEvtCfgSetAttrDef)},
  {gZclAttrDevMgmt_DevMgmtEvtCfgAttrSet_c,      (void *)&gaZclDevMgmt_DevMgmtEvtCfgSetAttrDef,         NumberOfElements(gaZclDevMgmt_DevMgmtEvtCfgSetAttrDef)},
  {gZclAttrDevMgmt_TunnelEvtCfgAttrSet_c,       (void *)&gaZclDevMgmt_TunnelEvtCfgSetAttrDef,         NumberOfElements(gaZclDevMgmt_TunnelEvtCfgSetAttrDef)},
  {gZclAttrDevMgmt_OTAEvtCfgAttrSet_c,          (void *)&gaZclDevMgmt_OTAEvtCfgSetAttrDef,         NumberOfElements(gaZclDevMgmt_OTAEvtCfgSetAttrDef)}
};

const zclAttrSetList_t gZclDevMgmtClientAttrSetList = {
  NumberOfElements(gaZclDevMgmtClientAttrSet),
  gaZclDevMgmtClientAttrSet
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*!
 * @fn 		void ZCLDegMgmtInit(void)
 *
 * @brief	Initializes functionality for the DeviceManagement server implementation.
 *
 */
void ZCLDegMgmtInit(void)
{
//  mDevMgmtEventTimerID = ZbTMR_AllocateTimer();
}

/*!
 * @fn 		zbStatus_t ZCL_DevMgmtServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Device Management Cluster server. 
 *
 */
zbStatus_t ZCL_DevMgmtServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo;
  zbStatus_t status = gZbSuccess_c;
  
  (void)pDevice;
  
   pFrame = (void *)pIndication->pAsdu;
   
   /*Create the destination address*/
   AF_PrepareForReply(&addrInfo, pIndication); 

   switch (pFrame->command) 
   {
      case gZclCmdDevMgmt_GetChangeOfTenancyReq_c:
        {
          status = zclDevMgmt_ProcessGetChangeOfTenancyReq(&addrInfo, pFrame->transactionId);
        }
        break;
        
      case gZclCmdDevMgmt_GetChangeOfSupplierReq_c:
	    {
		  status = zclDevMgmt_ProcessGetChangeOfSupplierReq(&addrInfo, pFrame->transactionId);
	    }
        break;

      case gZclCmdDevMgmt_GetChangeSupplyReq_c:
		{
	      status = zclDevMgmt_ProcessGetChangeSupplyReq(&addrInfo, pFrame->transactionId);
		}
		break;

      case gZclCmdDevMgmt_RequestNewPasswordReq_c:
		{
		  zclCmdDevMgmt_ReqNewPasswordReq_t *pCmdPayload = (zclCmdDevMgmt_ReqNewPasswordReq_t *)((uint8_t *)pIndication->pAsdu
																					+ sizeof(zclFrame_t));
	      status = zclDevMgmt_ProcessReqNewPasswordReq(&addrInfo, pFrame->transactionId, pCmdPayload);
		}
		break;

      case gZclCmdDevMgmt_SupplyStatusRsp_c:
    	break;
      case gZclCmdDevMgmt_ReportEvtConfigReq_c:
        break;
      default:
        status = gZclUnsupportedClusterCommand_c;
        break;
  }
  return status;
}

/*!
 * @fn 		zbStatus_t ZCL_DevMgmtClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Device Management Cluster client. 
 *
 */
zbStatus_t ZCL_DevMgmtClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  afAddrInfo_t addrInfo;
  zbStatus_t status = gZbSuccess_c;
 
  pFrame = (void *)pIndication->pAsdu;
  /* Create the destination address */
  AF_PrepareForReply(&addrInfo, pIndication); 
  
  (void)pDevice;
  
  pFrame = (void *)pIndication->pAsdu;
  /* Handle the command */
  switch(pFrame->command){
     case gZclCmdDevMgmt_PublishChangeOfTenancyRsp_c:
       {
    	 zclCmdDevMgmt_PublishChangeOfTenancyRsp_t *pCmdPayload = (zclCmdDevMgmt_PublishChangeOfTenancyRsp_t *)((uint8_t *)pIndication->pAsdu
    	  																					+ sizeof(zclFrame_t));
         (void)pCmdPayload;
       }
       break;

     case gZclCmdDevMgmt_PublishChangeOfSupplierRsp_c:
       {
    	 zclCmdDevMgmt_PublishChangeOfSupplierRsp_t *pCmdPayload = (zclCmdDevMgmt_PublishChangeOfSupplierRsp_t *)((uint8_t *)pIndication->pAsdu
  	  																					+ sizeof(zclFrame_t));
    	 (void)pCmdPayload;
       }
       break;

     case gZclCmdDevMgmt_ChangeSupplyRsp_c:
       {
    	   zclCmdDevMgmt_ChangeSupplyRsp_t *pCmdPayload = (zclCmdDevMgmt_ChangeSupplyRsp_t *)((uint8_t *)pIndication->pAsdu
	  																					+ sizeof(zclFrame_t));
    	   status = zclDevMgmt_ProcessChangeSupplyRsp(&addrInfo, pFrame->transactionId, pCmdPayload);
       }
       break;

     case gZclCmdDevMgmt_RequestNewPasswordRsp_c:
     case gZclCmdDevMgmt_LocalChangeSupplyRsp_c:
     case gZclCmdDevMgmt_UpdateSiteIDRsp_c:
     case gZclCmdDevMgmt_SetSupplyStatusRsp_c:
     case gZclCmdDevMgmt_SetEvtConfigRsp_c:
     case gZclCmdDevMgmt_GetEvtConfigRsp_c:
        break;
     default:
        status = gZclUnsupportedClusterCommand_c;
        break;
  }
  return status;
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_GetChangeOfTenancyReq(zclDevMgmt_GetChangeOfTenancyReq_t *pReq) 
 *
 * @brief	Sends over-the-air a Get Change of Tenancy frame from the Device Management client.
 *
 */
zbStatus_t zclDevMgmt_GetChangeOfTenancyReq
  (
  zclDevMgmt_GetChangeOfTenancyReq_t *pReq
  )
{
  return ZCL_SendClientReqSeqPassed(gZclCmdDevMgmt_GetChangeOfTenancyReq_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_GetChangeSupplyReq(zclDevMgmt_GetChangeSupplyReq_t *pReq) 
 *
 * @brief	Sends over-the-air a Get Change Supply frame from the Device Management client.
 *
 */
zbStatus_t zclDevMgmt_GetChangeSupplyReq
  (
  zclDevMgmt_GetChangeSupplyReq_t *pReq
  )
{
  return ZCL_SendClientReqSeqPassed(gZclCmdDevMgmt_GetChangeSupplyReq_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_GetChangeOfSupplierReq(zclDevMgmt_GetChangeOfSupplierReq_t *pReq) 
 *
 * @brief	Sends over-the-air a Get Change of Supplier frame from the Device Management client.
 *
 */
zbStatus_t zclDevMgmt_GetChangeOfSupplierReq
  (
  zclDevMgmt_GetChangeOfSupplierReq_t *pReq
  )
{
  return ZCL_SendClientReqSeqPassed(gZclCmdDevMgmt_GetChangeOfSupplierReq_c, 0, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_SupplyStatusRsp(zclDevMgmt_SupplyStatusRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Supply Status Response frame from the Device Management client.
 *
 */
zbStatus_t zclDevMgmt_SupplyStatusRsp
  (
  zclDevMgmt_SupplyStatusRsp_t *pReq
  )
{
  return ZCL_SendClientRspSeqPassed(gZclCmdDevMgmt_SupplyStatusRsp_c, sizeof(zclCmdDevMgmt_SupplyStatusRsp_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_RequestNewPasswordReq(zclDevMgmt_ReqNewPasswordReq_t *pReq) 
 *
 * @brief	Sends over-the-air a Request New Password frame from the Device Management client.
 *
 */
zbStatus_t zclDevMgmt_RequestNewPasswordReq
  (
  zclDevMgmt_ReqNewPasswordReq_t *pReq
  )
{
  return ZCL_SendClientReqSeqPassed(gZclCmdDevMgmt_RequestNewPasswordReq_c, sizeof(zclCmdDevMgmt_ReqNewPasswordReq_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_ReportEvtCfgReq(zclDevMgmt_ReportEvtCfgReq_t *pReq, uint8_t numEntries) 
 *
 * @brief	Sends over-the-air a Report Event Configuration frame from the Device Management client.
 *
 */
zbStatus_t zclDevMgmt_ReportEvtCfgReq
  (
  zclDevMgmt_ReportEvtCfgReq_t *pReq,
  uint8_t numEntries
  )
{
  return ZCL_SendClientRspSeqPassed(gZclCmdDevMgmt_ReportEvtConfigReq_c, sizeof(zclCmdDevMgmt_ReportEvtCfgReq_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_PublishChangeOfTenancyRsp(zclDevMgmt_PublishChangeOfTenancyRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Publish Change of Tenancy frame from the Device Management server.
 *
 */
zbStatus_t zclDevMgmt_PublishChangeOfTenancyRsp
  (
  zclDevMgmt_PublishChangeOfTenancyRsp_t *pRsp
  )
{
  uint8_t len = sizeof(zclCmdDevMgmt_PublishChangeOfTenancyRsp_t);

#if gZclSE_EnableCommandSignature_d
  {
	Signature_t signature;
	pRsp->cmdFrame.SignatureType = gZclSE_SignatureECDSA_c;

	/* Add ECDSA Signature*/
	ZCL_ApplyECDSASign((uint8_t*)&pRsp->cmdFrame, (sizeof(zclCmdDevMgmt_PublishChangeOfTenancyRsp_t) - sizeof(Signature_t)), (uint8_t*)&signature);
	FLib_MemCpy(pRsp->cmdFrame.Signature, &signature, sizeof(Signature_t));
    }
#else
  pRsp->cmdFrame.SignatureType = gZclSE_SignatureNotUsed_c;
  len -= sizeof(Signature_t);
#endif

  return ZCL_SendServerRspSeqPassed(gZclCmdDevMgmt_PublishChangeOfTenancyRsp_c, len, (zclGenericReq_t *)pRsp);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_PublishChangeOfSupplierRsp(zclDevMgmt_PublishChangeOfSupplierRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Publish Change of Supplier frame from the Device Management server.
 *
 */
zbStatus_t zclDevMgmt_PublishChangeOfSupplierRsp
  (
  zclDevMgmt_PublishChangeOfSupplierRsp_t *pRsp
  )
{
  uint8_t len = sizeof(zclCmdDevMgmt_PublishChangeOfSupplierRsp_t);

#if gZclSE_EnableCommandSignature_d
  {
	Signature_t signature;
	pRsp->cmdFrame.SignatureType = gZclSE_SignatureECDSA_c;

	/* Add ECDSA Signature*/
	ZCL_ApplyECDSASign((uint8_t*)&pRsp->cmdFrame, (sizeof(zclCmdDevMgmt_PublishChangeOfSupplierRsp_t) - sizeof(Signature_t)), (uint8_t*)&signature);
	FLib_MemCpy(pRsp->cmdFrame.Signature, &signature, sizeof(Signature_t));
    }
#else
  pRsp->cmdFrame.SignatureType = gZclSE_SignatureNotUsed_c;
  len -= sizeof(Signature_t);
#endif

  len -= (sizeof(zclStr17_t) -   pRsp->cmdFrame.ProviderName.length - 1);
  return ZCL_SendServerRspSeqPassed(gZclCmdDevMgmt_PublishChangeOfSupplierRsp_c, len, (zclGenericReq_t *)pRsp);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_ChangeSupplyRsp(zclDevMgmt_ChangeSupplyRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Publish Change Supply Response frame from the Device Management server.
 *
 */
zbStatus_t zclDevMgmt_ChangeSupplyRsp
  (
  zclDevMgmt_ChangeSupplyRsp_t *pRsp
  )
{
  uint8_t len = sizeof(zclCmdDevMgmt_ChangeSupplyRsp_t);

#if gZclSE_EnableCommandSignature_d
  {
	Signature_t signature;
	pRsp->cmdFrame.SignatureType = gZclSE_SignatureECDSA_c;

	/* Add ECDSA Signature*/
	ZCL_ApplyECDSASign((uint8_t*)&pRsp->cmdFrame, (sizeof(zclCmdDevMgmt_ChangeSupplyRsp_t) - sizeof(Signature_t)), (uint8_t*)&signature);
	FLib_MemCpy(pRsp->cmdFrame.Signature, &signature, sizeof(Signature_t));
    }
#else
  pRsp->cmdFrame.SignatureType = gZclSE_SignatureNotUsed_c;
  len -= sizeof(Signature_t);
#endif

  return ZCL_SendServerRspSeqPassed(gZclCmdDevMgmt_ChangeSupplyRsp_c, len, (zclGenericReq_t *)pRsp);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_RequestNewPasswordRsp(zclDevMgmt_ReqNewPasswordRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Request New Password Response frame from the Device Management server.
 *
 */
zbStatus_t zclDevMgmt_RequestNewPasswordRsp
  (
  zclDevMgmt_ReqNewPasswordRsp_t *pRsp
  )
{
#if gZclSE_EnableCommandSignature_d
  {
	Signature_t signature;
	pRsp->cmdFrame.SignatureType = gZclSE_SignatureECDSA_c;

	/* Add ECDSA Signature*/
	ZCL_ApplyECDSASign((uint8_t*)&pRsp->cmdFrame, (sizeof(zclCmdDevMgmt_ReqNewPasswordRsp_t) - sizeof(Signature_t)), (uint8_t*)&signature);
	FLib_MemCpy(pRsp->cmdFrame.Signature, &signature, sizeof(Signature_t));
    }
#else
  pRsp->cmdFrame.SignatureType = gZclSE_SignatureNotUsed_c;
#endif

  return ZCL_SendServerRspSeqPassed(gZclCmdDevMgmt_RequestNewPasswordRsp_c, sizeof(zclCmdDevMgmt_ReqNewPasswordRsp_t), (zclGenericReq_t *)pRsp);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_LocalChangeSupplyRsp(zclDevMgmt_LocalChangeSupplyRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Local Change Supply Response frame from the Device Management server.
 *
 */
zbStatus_t zclDevMgmt_LocalChangeSupplyRsp
  (
  zclDevMgmt_LocalChangeSupplyRsp_t *pReq
  )
{
  return ZCL_SendServerReqSeqPassed(gZclCmdDevMgmt_LocalChangeSupplyRsp_c, sizeof(zclCmdDevMgmt_LocalChangeSupplyRsp_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_UpdateSiteIDRsp(zclDevMgmt_UpdateSiteIDRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an Update Site ID Response frame from the Device Management server.
 *
 */
zbStatus_t zclDevMgmt_UpdateSiteIDRsp
  (
  zclDevMgmt_UpdateSiteIDRsp_t *pReq
  )
{
  uint8_t len = sizeof(zclCmdDevMgmt_UpdateSiteIDRsp_t);
#if !gZclSE_EnableCommandSignature_d
  len -= sizeof(Signature_t);
#endif
  return ZCL_SendServerReqSeqPassed(gZclCmdDevMgmt_UpdateSiteIDRsp_c, len, (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_SetSupplyStatusRsp(zclDevMgmt_SetSupplyStatusRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Set Supply Status Response frame from the Device Management server.
 *
 */
zbStatus_t zclDevMgmt_SetSupplyStatusRsp
  (
  zclDevMgmt_SetSupplyStatusRsp_t *pReq
  )
{
  return ZCL_SendServerRspSeqPassed(gZclCmdDevMgmt_SetSupplyStatusRsp_c, sizeof(zclCmdDevMgmt_SetSupplyStatusRsp_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_SetEvtCfgRsp(zclDevMgmt_SetEvtCfgRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Set Event Configuration Response frame from the Device Management server.
 *
 */
zbStatus_t zclDevMgmt_SetEvtCfgRsp
  (
  zclDevMgmt_SetEvtCfgRsp_t *pReq
  )
{
  return ZCL_SendServerReqSeqPassed(gZclCmdDevMgmt_SetEvtConfigRsp_c, sizeof(zclCmdDevMgmt_SetEvtCfgRsp_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_GetEvtCfgRsp(zclDevMgmt_GetEvtCfgRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Get Event Configuration Response frame from the Device Management server.
 *
 */
zbStatus_t zclDevMgmt_GetEvtCfgRsp
  (
  zclDevMgmt_GetEvtCfgRsp_t *pReq
  )
{
  return ZCL_SendServerReqSeqPassed(gZclCmdDevMgmt_GetEvtConfigRsp_c, sizeof(zclCmdDevMgmt_GetEvtCfgRsp_t), (zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_ChangeTenancy(DevMgmt_ChangeOfTenancy_t *pReq) 
 *
 * @brief	Stores new Tenancy Information.
 *
 */
zbStatus_t zclDevMgmt_ChangeTenancy
  (
	DevMgmt_ChangeOfTenancy_t *pReq
  )
{
	FLib_MemCpy(&mZclDevMgmt_ChangeOfTenancy, pReq, sizeof(DevMgmt_ChangeOfTenancy_t));
	return gZclSuccess_c;
}

/*!
 * @fn 		zbStatus_t zclDevMgmt_ChangeSupplier(DevMgmt_ChangeOfSupplier_t *pReq) 
 *
 * @brief	Stores new Supplier Information.
 *
 */
zbStatus_t zclDevMgmt_ChangeSupplier
  (
	DevMgmt_ChangeOfSupplier_t *pReq
  )
{
	FLib_MemCpy(&mZclDevMgmt_ChangeOfSupplier, pReq, sizeof(DevMgmt_ChangeOfSupplier_t));
	return gZclSuccess_c;
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* zclDevMgmt_ProcessGetChangeOfTenancyReq
* Processes the GetChangeOfTenancy Request.
*
* Returns Success if response is sent.
*****************************************************************************/
static zbStatus_t zclDevMgmt_ProcessGetChangeOfTenancyReq
  (
  afAddrInfo_t *pAddrInfo,
  uint8_t zclTransactionId
  )
{
  zclDevMgmt_PublishChangeOfTenancyRsp_t *pRsp;
  zbStatus_t status;
 
  pRsp = MSG_Alloc(sizeof(zclDevMgmt_PublishChangeOfTenancyRsp_t));

  if (!pRsp)
    return gZclFailure_c;

  FLib_MemCpy((uint8_t*)&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  pRsp->zclTransactionId = zclTransactionId;

  /* Fill the response payload */
  FLib_MemCpy(&pRsp->cmdFrame, &mZclDevMgmt_ChangeOfTenancy, sizeof(DevMgmt_ChangeOfTenancy_t));

  status =  zclDevMgmt_PublishChangeOfTenancyRsp(pRsp);
  MSG_Free(pRsp);
  return status;
}

/*****************************************************************************
* zclDevMgmt_ProcessGetChangeOfSupplierReq
* Processes the GetChangeOfSupplier Request.
*
* Returns Success if response is sent.
*****************************************************************************/
static zbStatus_t zclDevMgmt_ProcessGetChangeOfSupplierReq
  (
  afAddrInfo_t *pAddrInfo,
  uint8_t zclTransactionId
  )
{
  zclDevMgmt_PublishChangeOfSupplierRsp_t *pRsp;
  zbStatus_t status;

  pRsp = MSG_Alloc(sizeof(zclDevMgmt_PublishChangeOfSupplierRsp_t));

  if (!pRsp)
    return gZclFailure_c;

  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  pRsp->zclTransactionId = zclTransactionId;

  /* Fill the response payload */
  ZCL_GetAttribute(BeeAppDataInit(appEndPoint), (uint8_t*)maClusterId, gZclAttrMetRISCurrSummDlvrd_c, gZclServerAttr_c, pRsp->cmdFrame.CurrentProviderID, NULL);
  FLib_MemCpy(pRsp->cmdFrame.IssuerEvtID, &mZclDevMgmt_ChangeOfSupplier, sizeof(DevMgmt_ChangeOfSupplier_t));

  status = zclDevMgmt_PublishChangeOfSupplierRsp(pRsp);
  MSG_Free(pRsp);
  return status;
}

/*****************************************************************************
* zclDevMgmt_ProcessGetChangeSupplyReq
* Processes the GetChangeSupply Request.
*
* Returns Success if response is sent.
*****************************************************************************/
static zbStatus_t zclDevMgmt_ProcessGetChangeSupplyReq
  (
  afAddrInfo_t *pAddrInfo,
  uint8_t zclTransactionId
  )
{
  zclDevMgmt_ChangeSupplyRsp_t *pRsp;
  zbStatus_t status;

  pRsp = MSG_Alloc(sizeof(zclDevMgmt_ChangeSupplyRsp_t));

  if (!pRsp)
    return gZclFailure_c;

  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  pRsp->zclTransactionId = zclTransactionId;

  /* Fill the response payload */
  FLib_MemCpy(&pRsp->cmdFrame, &mZclDevMgmt_ChangeSupply, sizeof(DevMgmt_ChangeSupply_t));

  status =  zclDevMgmt_ChangeSupplyRsp(pRsp);
  MSG_Free(pRsp);
  return status;
}

/*****************************************************************************
* zclDevMgmt_ProcessReqNewPasswordReq
* Processes the ReqPassword Request.
*
* Returns Success if response is sent.
*****************************************************************************/
static zbStatus_t zclDevMgmt_ProcessReqNewPasswordReq
  (
  afAddrInfo_t *pAddrInfo,
  uint8_t zclTransactionId,
  zclCmdDevMgmt_ReqNewPasswordReq_t *pReq
  )
{
  zclDevMgmt_ReqNewPasswordRsp_t *pRsp;
  zbStatus_t status;

  if (mZclDevMgmt_Password.PasswordType != pReq->PasswordType)
  	  return gZclFailure_c;

  pRsp = MSG_Alloc(sizeof(zclDevMgmt_ChangeSupplyRsp_t));

  if (!pRsp)
    return gZclFailure_c;

  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  pRsp->zclTransactionId = zclTransactionId;

  /* Fill the response payload */
  FLib_MemCpy(&pRsp->cmdFrame, &mZclDevMgmt_Password, sizeof(DevMgmt_Password_t));

  status =  zclDevMgmt_RequestNewPasswordRsp(pRsp);
  MSG_Free(pRsp);
  return status;
}

/*****************************************************************************
* zclDevMgmt_ProcessChangeSupply
* Processes the ChangeSupply Response.
*
* Returns Success if response is sent.
*****************************************************************************/
static zbStatus_t zclDevMgmt_ProcessChangeSupplyRsp
  (
  afAddrInfo_t *pAddrInfo,
  uint8_t zclTransactionId,
  zclCmdDevMgmt_ChangeSupplyRsp_t *pReq
  )
{
  zclDevMgmt_ChangeSupplyRsp_t *pRsp;
  zbStatus_t status;

  pRsp = MSG_Alloc(sizeof(zclDevMgmt_ChangeSupplyRsp_t));

  if (!pRsp)
    return gZclFailure_c;

  FLib_MemCpy(&pRsp->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  pRsp->zclTransactionId = zclTransactionId;

  /* Fill the response payload */
  FLib_MemCpy(&pRsp->cmdFrame, &mZclDevMgmt_Password, sizeof(DevMgmt_Password_t));

  status =  zclDevMgmt_ChangeSupplyRsp(pRsp);
  MSG_Free(pRsp);
  return status;
}
