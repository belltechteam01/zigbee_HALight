/************************************************************************************
* This file is owned by the platform/application specific software and basically 
* defines how the 802.15.4 Freescale MAC is configured. The file is included by the
* relevant MAC modules and is necessary for the MAC to compile.
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

#ifndef _APPTOPLATFORMCONFIG_H_
#define _APPTOPLATFORMCONFIG_H_

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef PoolsDetails_c
#define PoolsDetails_c \
         _block_size_  60  _number_of_blocks_    12 _eol_  \
         _block_size_ 192  _number_of_blocks_    16 _eol_  \
         _block_size_ 256  _number_of_blocks_     2 _eol_
#endif
/*!=================================================================================================
  NVM
==================================================================================================*/
#ifndef gNvStorageIncluded_d
    #define gNvStorageIncluded_d        1
#endif
#ifndef gNvFragmentation_Enabled_d
    #define gNvFragmentation_Enabled_d  1
#endif
#ifndef gNvUseExtendedFeatureSet_d
    #define gNvUseExtendedFeatureSet_d  1
#endif
#ifndef gUnmirroredFeatureSet_d
    #define gUnmirroredFeatureSet_d     0
#endif
#ifndef gNvDisableIntCmdSeq_c
    #define gNvDisableIntCmdSeq_c       TRUE
#endif
#ifndef gNvmEnableFSCIMonitoring_c
  #define gNvmEnableFSCIMonitoring_c      (0)
#endif
/*!=================================================================================================
  KEYBOARD
==================================================================================================*/
#ifndef gKBD_KeysCount_c
    #define gKBD_KeysCount_c        4
#endif
/*!=================================================================================================
  SERIAL MANAGER
==================================================================================================*/
#ifndef gSerialMgrUseUSB_c
  #define gSerialMgrUseUSB_c     0
#endif
#ifndef gSerialMgrUseUart_c
    #define gSerialMgrUseUart_c    1
#endif
#ifndef gSerialMgrRxBufSize_c
    #define gSerialMgrRxBufSize_c       256
#endif
#ifndef gSerialMgrTxQueueSize_c
    #define gSerialMgrTxQueueSize_c     20
#endif
/*!=================================================================================================
  FSCI
==================================================================================================*/
#ifndef gFsciIncluded_c
    #define gFsciIncluded_c         1
#endif
#ifndef gFsciMaxOpGroups_c
    #define gFsciMaxOpGroups_c      70
#endif
/*!=================================================================================================
  TMR
==================================================================================================*/
#ifndef gTmrTaskStackSize_c
    #define gTmrTaskStackSize_c 500
#endif
#ifndef gTmrStackTimers_c
    #define gTmrStackTimers_c   10
#endif
/*!=================================================================================================
* OSA EXT
==================================================================================================*/
#ifndef osNumberOfSemaphores
    #define osNumberOfSemaphores 5
#endif
#ifndef osNumberOfMutexes
    #define osNumberOfMutexes    4
#endif
#ifndef osNumberOfMessageQs
    #define osNumberOfMessageQs  1
#endif
#ifndef osNumberOfMessages
    #define osNumberOfMessages   20
#endif
#ifndef osNumberOfEvents
    #define osNumberOfEvents     10
#endif
#ifndef gMainThreadPriority_c
    #define gMainThreadPriority_c  (OSA_PRIORITY_IDLE + 2)
#endif
#ifndef gMainThreadStackSize_c
    #define gMainThreadStackSize_c 1280
#endif
//**********************************************************************************
#ifdef __cplusplus
}
#endif
#endif /* _APPTOPLATFORMCONFIG_H_ */
