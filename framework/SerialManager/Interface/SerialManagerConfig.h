/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file SerialManagerConfig.h
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


#ifndef _SERIALMANAGER_CONFIG_H_
#define _SERIALMANAGER_CONFIG_H_

#ifndef gSerialMgr_ParamValidation_d
#define gSerialMgr_ParamValidation_d 1
#endif
#ifndef gSerialMgr_BlockSenderOnQueueFull_c
#define gSerialMgr_BlockSenderOnQueueFull_c 1
#endif
#ifndef gSerialManagerMaxInterfaces_c
#define gSerialManagerMaxInterfaces_c 1
#endif
#ifndef gSerialMgrUseUart_c
#define gSerialMgrUseUart_c 1
#endif
#ifndef gSerialMgrUseUSB_c
#define gSerialMgrUseUSB_c 0
#endif
#ifndef gSerialMgrUseIIC_c
#define gSerialMgrUseIIC_c 0
#endif
#ifndef gSerialMgrUseSPI_c
#define gSerialMgrUseSPI_c 0
#endif
#ifndef gSerialMgrIICAddress_c
#define gSerialMgrIICAddress_c 0x76
#endif
#ifndef gSerialMgrRxBufSize_c
#define gSerialMgrRxBufSize_c 32
#endif
#ifndef gSerialMgrTxQueueSize_c
#define gSerialMgrTxQueueSize_c 5
#endif
#ifndef gSerialTaskStackSize_c
#define gSerialTaskStackSize_c 1024
#endif
#ifndef gSerialTaskPriority_c
#define gSerialTaskPriority_c 3
#endif

#endif //_SERIALMANAGER_CONFIG_H_
