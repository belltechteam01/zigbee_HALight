/*! @file 	ZclSensing.h
 *
 * @brief	Types, definitions and prototypes for the ZCL ZCL measurement and sensing domain.
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
#ifndef _ZCL_SENSING_H
#define _ZCL_SENSING_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "ZclOptions.h"
#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "ZCL.h"


/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/

/******************************************
	Temperature Measurement Cluster
*******************************************/

#define gZclAttrSetTemperatureMeasurement_c                     0x00
#define gZclAttrIdTemperatureMeasurement_MeasuredValueId_c        0x00  /* M - Measured Value */
#define gZclAttrIdTemperatureMeasurement_MinMeasuredValuedId_c    0x01  /* M - MinMeasuredValue*/
#define gZclAttrIdTemperatureMeasurement_MaxMeasuredValuedId_c    0x02  /* M - MaxMeasuredValued */
#define gZclAttrIdTemperatureMeasurement_ToleranceId_c            0x03 /* M - Tolerance */

/* 4.4.2.2 Temperature Measurement Information cluster attributes */
#if !(gBigEndian_c)
#define gZclAttrTemperatureMeasurement_MeasuredValueId_c        0x0000  /* M - Measured Value */
#define gZclAttrTemperatureMeasurement_MinMeasuredValuedId_c    0x0001  /* M - MinMeasuredValue*/
#define gZclAttrTemperatureMeasurement_MaxMeasuredValuedId_c    0x0002  /* M - MaxMeasuredValued */
#define gZclAttrTemperatureMeasurement_ToleranceId_c            0x0003 /* M - Tolerance */
#else
#define gZclAttrTemperatureMeasurement_MeasuredValueId_c        0x0000  /* M - Measured Value */
#define gZclAttrTemperatureMeasurement_MinMeasuredValuedId_c    0x0100  /* M - MinMeasuredValue*/
#define gZclAttrTemperatureMeasurement_MaxMeasuredValuedId_c    0x0200  /* M - MaxMeasuredValued */
#define gZclAttrTemperatureMeasurement_ToleranceId_c            0x0300 /* M - Tolerance */
#endif

/* The comand use for this cluster are the read/write attributes, and reporting attributes */
typedef PACKED_STRUCT zclTemperatureMeasurementAttrs_tag
{
  int16_t      MeasuredValue[zclReportableCopies_c] ;      /* M - Measured Value */
  int16_t      MinMeasuredValue;   /* M - Minimal Measured Value */
  int16_t      MaxMeasuredValue;   /* M - Maximal Measured Value */
#if gZclClusterOptionals_d
  uint16_t  Tolerance[zclReportableCopies_c];                    /* O - Tolerance */
#endif
} zclTemperatureMeasurementAttrs_t;

/******************************************
	Occupancy Sensing Cluster
*******************************************/
#define gZclAttrSetOccupancySensing_c 0x00
#define gZclAttrIdOccupancySensing_OccupancyId_c                             0x00 /* M - Occupancy */
#define gZclAttrIdOccupancySensing_OccupancySensorTypeId_c                   0x01 /* M - Occupancy Sensor Type */
#define gZclAttrIdOccupancySensing_PIROccupiedToUnoccupiedDelayId_c          0x10 /* O - PIR Sensor configuration Occupied to Unoccupied Delay */
#define gZclAttrIdOccupancySensing_PIRUnoccupiedToOccupiedDelayId_c          0x11 /* O - PIR Sensor configuration Unoccupied to Occupied Delay */
#define gZclAttrIdOccupancySensing_PIRUnoccupiedToOccupiedThresholdId_c      0x12 /* O - PIR Sensor configuration Unoccupied to Occupied Threshold */
#define gZclAttrIdOccupancySensing_UltrasonicOccupiedToUnoccupiedDelayId_c   0x20 /* O - Ultrasonic Sensor configuration Occupied to Unoccupied Delay */
#define gZclAttrIdOccupancySensing_UltrasonicUnoccupiedToOccupiedDelayId_c   0x21 /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Delay */
#define gZclAttrIdOccupancySensing_UltrasonicUnoccupiedToOccupiedThresholdId_c 0x22 /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Threshold */

/* 4.8.2.2 Attributes of the occupancy sensor information attribute set */
#if !(gBigEndian_c)
#define gZclAttrOccupancySensing_OccupancyId_c                             0x0000 /* M - Occupancy */
#define gZclAttrOccupancySensing_OccupancySensorTypeId_c                   0x0001 /* M - Occupancy Sensor Type */
#define gZclAttrOccupancySensing_PIROccupiedToUnoccupiedDelayId_c          0x0010 /* O - PIR Sensor configuration Occupied to Unoccupied Delay */
#define gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedDelayId_c          0x0011 /* O - PIR Sensor configuration Unoccupied to Occupied Delay */
#define gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedThresholdId_c      0x0012 /* O - PIR Sensor configuration Unoccupied to Occupied Threshold */
#define gZclAttrOccupancySensing_UltrasonicOccupiedToUnoccupiedDelayId_c   0x0020 /* O - Ultrasonic Sensor configuration Occupied to Unoccupied Delay */
#define gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedDelayId_c   0x0021 /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Delay */
#define gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedThresholdId_c 0x0022 /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Threshold */
#else
#define gZclAttrOccupancySensing_OccupancyId_c                             0x0000 /* M - Occupancy */
#define gZclAttrOccupancySensing_OccupancySensorTypeId_c                   0x0100 /* M - Occupancy Sensor Type */
#define gZclAttrOccupancySensing_PIROccupiedToUnoccupiedDelayId_c          0x1000 /* O - PIR Sensor configuration Occupied to Unoccupied Delay */
#define gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedDelayId_c          0x1100 /* O - PIR Sensor configuration Unoccupied to Occupied Delay */
#define gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedThresholdId_c      0x1200 /* O - PIR Sensor configuration Unoccupied to Occupied Threshold */
#define gZclAttrOccupancySensing_UltrasonicOccupiedToUnoccupiedDelayId_c   0x2000 /* O - Ultrasonic Sensor configuration Occupied to Unoccupied Delay */
#define gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedDelayId_c   0x2100 /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Delay */
#define gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedThresholdId_c 0x2200 /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Threshold */
#endif

/* 4.8.2.2.1.2 Values of the OccupancySensorType attribute */
enum{
  gZclTypeofSensor_PIR_c = 0x00,               /* PIR sensor type */
  gZclTypeofSensor_Ultrasonic_c = 0x01,        /* Ultrasonic sensor type */
  gZclTypeofSensor_PirAndUltrasonic_c = 0x02   /* PIR and Utrasonic sensor type */
};

/*Cluster Commands:
	This cluster shall support attribute reporting using the Report Attributes command and according to the
    minimum and maximum reporting interval settings described in the ZCL Specification (see
    [R2]). The following attribute shall be reported:
    Occupancy.
    No commands are generated by the server or received by the client cluster, except responses to read 
    and write attribute commands, and attribute reports.
	*/

typedef PACKED_STRUCT zclOccupancySensingAttrs_tag
{
  uint8_t    Occupancy;                            /* M - Occupancy is a 8b bitmap */
  uint8_t OccupancySensorType;  /* M - Occupancy Sensor type, is an enumeration */
#if gZclClusterOptionals_d
  uint16_t PIROccupiedToUnoccupiedDelay;            /* O - PIR Sensor configuration Occupied to Unoccupied Delay */
  uint16_t PIRUnoccupiedToOccupiedDelay;            /* O - PIR Sensor configuration Unoccupied to Occupied Delay */
  uint8_t  PIRUnoccupiedToOccupiedThreshold;         /* O - PIR Sensor configuration Unoccupied to Occupied Threshold */
  uint16_t UltrasonicOccupiedToUnoccupiedDelay;     /* O - Ultrasonic Sensor configuration Occupied to Unoccupied Delay */
  uint16_t UltrasonicUnoccupiedToOccupiedDelay;     /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Delay */
  uint8_t  UltrasonicUnoccupiedToOccupiedThreshold;     /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Threshold */
#endif
} zclOccupancySensingAttrs_t;

typedef PACKED_STRUCT zclOccupancySetAttrValue_tag
{
  uint8_t      ep;     /* endpoint*/
  uint8_t      valueAttr;
  uint16_t     attrID;
} zclOccupancySetAttrValue_t;

/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/
/*!
 * @fn 		zbStatus_t ZCL_TemperatureMeasureClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Temperature Measurement Cluster Server. 
 *
 */
zbStatus_t ZCL_TemperatureMeasureClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
/*!
 * @fn 		zbStatus_t ZCL_OccupancySensingClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
 *
 * @brief	Processes the requests received on the Occupancy Sensing Cluster Server. 
 *
 */
zbStatus_t ZCL_OccupancySensingClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
#ifdef __cplusplus
}
#endif
#endif  /* _ZCL_SENSING_H */

