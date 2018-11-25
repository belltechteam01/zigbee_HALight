/*! @file 	  ZclSensing.c
 *
 * @brief	  This source file describes specific functionality implemented
 *			  for ZCL measurement and sensing domain.
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
#include "ZCLSensing.h"

#include "ZclOptions.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************
  Illuminance Measurement Cluster
  See ZCL Specification Section 4.2
*******************************/

/******************************
  Illuminance Level Sensing Cluster
  See ZCL Specification Section 4.3
*******************************/

/******************************
  Temperature Measurement Cluster
  See ZCL Specification Section 4.4
*******************************/
const zclAttrDef_t gaZclTemperatureMeasurementClusterAttrDef[] = {  
  {gZclAttrIdTemperatureMeasurement_MeasuredValueId_c,     gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsReportable_c,sizeof(int16_t),  (void *)MbrOfs(zclTemperatureMeasurementAttrs_t,MeasuredValue)},
  {gZclAttrIdTemperatureMeasurement_MinMeasuredValuedId_c, gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c,sizeof(int16_t),  (void *)MbrOfs(zclTemperatureMeasurementAttrs_t,MinMeasuredValue)},
  {gZclAttrIdTemperatureMeasurement_MaxMeasuredValuedId_c, gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c,sizeof(int16_t),  (void *)MbrOfs(zclTemperatureMeasurementAttrs_t,MaxMeasuredValue)}
#if gZclClusterOptionals_d
  , {gZclAttrIdTemperatureMeasurement_ToleranceId_c, gZclDataTypeUint16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsReportable_c,sizeof(uint16_t),  (void *)MbrOfs(zclTemperatureMeasurementAttrs_t,Tolerance)}
#endif
};

const zclAttrSet_t gaZclTemperatureMeasurementClusterAttrSet[] = {
  {gZclAttrSetTemperatureMeasurement_c, (void *)&gaZclTemperatureMeasurementClusterAttrDef, NumberOfElements(gaZclTemperatureMeasurementClusterAttrDef)}
};

const zclAttrSetList_t gZclTemperatureMeasurementClusterAttrSetList = {
  NumberOfElements(gaZclTemperatureMeasurementClusterAttrSet),
  gaZclTemperatureMeasurementClusterAttrSet
};

/******************************
  Pressure Measurement Cluster
  See ZCL Specification Section 4.5
*******************************/

/******************************
  Flow Measurement Cluster
  See ZCL Specification Section 4.6
*******************************/

/******************************
  Relative Humidity Measurement Cluster
  See ZCL Specification Section 4.7
*******************************/

/******************************
  Occupancy Sensing Cluster
  See ZCL Specification Section 4.8
*******************************/
const zclAttrDef_t gaZclOccupancySensingClusterAttrDef[] = {  
  //occupancy sensor information set
  {gZclAttrIdOccupancySensing_OccupancyId_c , gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsReportable_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, Occupancy)},
  {gZclAttrIdOccupancySensing_OccupancySensorTypeId_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, OccupancySensorType)}
  //PIR configuration set
#if gZclClusterOptionals_d
  , {gZclAttrIdOccupancySensing_PIROccupiedToUnoccupiedDelayId_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c ,sizeof(int16_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, PIROccupiedToUnoccupiedDelay)},
  {gZclAttrIdOccupancySensing_PIRUnoccupiedToOccupiedDelayId_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c ,sizeof(uint16_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, PIRUnoccupiedToOccupiedDelay)},
  {gZclAttrIdOccupancySensing_PIRUnoccupiedToOccupiedThresholdId_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c ,sizeof(uint8_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, PIRUnoccupiedToOccupiedThreshold)}
#endif
  //ultrasonic configuration set  
#if gZclClusterOptionals_d
  , {gZclAttrIdOccupancySensing_UltrasonicOccupiedToUnoccupiedDelayId_c, gZclDataTypeUint16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, UltrasonicOccupiedToUnoccupiedDelay)},
  {gZclAttrIdOccupancySensing_UltrasonicUnoccupiedToOccupiedDelayId_c, gZclDataTypeUint16_c,gZclAttrFlagsInRAM_c, sizeof(uint16_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, UltrasonicUnoccupiedToOccupiedDelay)},
  {gZclAttrIdOccupancySensing_UltrasonicUnoccupiedToOccupiedThresholdId_c, gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c, sizeof(uint8_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, UltrasonicUnoccupiedToOccupiedThreshold)}
#endif
};

const zclAttrSet_t gaZclOccupancySensingClusterAttrSet[] = {
  {gZclAttrSetOccupancySensing_c, (void *)&gaZclOccupancySensingClusterAttrDef, NumberOfElements(gaZclOccupancySensingClusterAttrDef)}
};

const zclAttrSetList_t gZclOccupancySensingClusterAttrSetList = {
  NumberOfElements(gaZclOccupancySensingClusterAttrSet),
  gaZclOccupancySensingClusterAttrSet
};


/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/
/* for read/write/report attribute commands */
/******************************
  Temperature Measurement Cluster
  See ZCL Specification Section 4.4
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_TemperatureMeasureClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Temperature Measurement Cluster Server. 
 *
 */
zbStatus_t ZCL_TemperatureMeasureClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    (void) pIndication;
    (void) pDevice;
    return gZbSuccess_c;
}

/******************************
  Occupancy Sensing Cluster
  See ZCL Specification Section 4.8
*******************************/

/*!
 * @fn 		zbStatus_t ZCL_OccupancySensingClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Occupancy Sensing Cluster Server. 
 *
 */
zbStatus_t ZCL_OccupancySensingClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    (void) pIndication;
    (void) pDevice;
    return gZbSuccess_c;
}


