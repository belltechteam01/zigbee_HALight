/************************************************************************************
* This file defines how the MAC/PHY is configured by the Application. This includes
* setting up the type of device (RFD, FFD, etc) .
*
*
* (c) Copyright 2012, Freescale, Inc.  All rights reserved.
*
*
* No part of this document may be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#ifndef _APPTOMACPHYCONFIG_H_
#define _APPTOMACPHYCONFIG_H_

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#ifndef gFSCI_IncludeMacCommands_c
  #define gFSCI_IncludeMacCommands_c 0
#endif

#ifndef gMpmMaxPANs_c
    #define gMpmMaxPANs_c       1
#endif    
#ifndef gMacInstancesCnt_c
    #define gMacInstancesCnt_c      1
#endif
#endif //_APPTOMACPHYCONFIG_H_
