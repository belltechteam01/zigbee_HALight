/******************************************************************************
* This is the Private header file for the SecurityManagementService module.
*
* (c) Copyright 2005, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*******************************************************************************/
#ifndef _ZB_APP_INTERFACE_H_
#define _ZB_APP_INTERFACE_H_

#ifdef __cplusplus
    extern "C" {
#endif
/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/
/* Application Device Version and flag values */
#if gUser_Desc_rsp_d && gComplex_Desc_rsp_d
  /* user and complex descriptor both supported */
  #define gAppDeviceVersionAndFlag_c 0x30 
#elif gUser_Desc_rsp_d
  /* user descriptor supported */
  #define gAppDeviceVersionAndFlag_c 0x20
#elif gComplex_Desc_rsp_d
  /* complex descriptor supported */
  #define gAppDeviceVersionAndFlag_c 0x10
#else
  /* user and complex descriptor both not supported */
  #define gAppDeviceVersionAndFlag_c 0x00
#endif


extern endPointDesc_t endPoint0Desc;

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/
/* None */
/******************************************************************************
*******************************************************************************
* Public type Definations
*******************************************************************************
******************************************************************************/
/* None */
/******************************************************************************
*******************************************************************************
* Public Memory Decleration
*******************************************************************************
******************************************************************************/
/* None */
/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/
/* None */
#ifdef __cplusplus
}
#endif
#endif /* _ZB_APP_INTERFACE_H_ */



