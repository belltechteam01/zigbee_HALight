/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file ltc_interface.h
* This is the header file for the security module that implements LTC
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

#ifndef __LTC_INTERFACE_H__
#define __LTC_INTERFACE_H__

#define gLtcBleDebugEnabled 0

#if gLtcBleDebugEnabled
#warning LTC BLE debug is enabled. The decrypted MIC will be written after the decrypted output data!!!
#endif

#define gLTC_CCM_Encrypt_c 0
#define gLTC_CCM_Decrypt_c 1

/*! *********************************************************************************
*************************************************************************************
* Public data types
*************************************************************************************
********************************************************************************** */

typedef enum bleAesCcmErrorStatus_tag
{
    bleAesCcmSuccess_c                              = 0x00,
    bleAesCcmErrNullOutputOrLengthPointer_c         = 0x01,
    bleAesCcmErrEncryptedMessageLengthIsTooSmall_c  = 0x02,
    bleAesCcmErrMacCheckFailed_c                    = 0x03,
} bleAesCcmErrorStatus_t;

/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */
/*! *********************************************************************************
* \brief  This function performs AES-128 encryption on a 16-byte block.
*
* \param[in]  pInput Pointer to the location of the 16-byte plain text block.
*
* \param[in]  pKey Pointer to the location of the 128-bit key.
*
* \param[out]  pOutput Pointer to the location to store the 16-byte ciphered output.
*
* \pre All Input/Output pointers must refer to a memory address alligned to 4 bytes!
*
********************************************************************************** */
void LTC_AES128_Encrypt(const uint8_t* pInput,
                        const uint8_t* pKey,
                        uint8_t* pOutput);

/*! *********************************************************************************
* \brief  This function performs AES-128 decryption on a 16-byte block.
*
* \param[in]  pInput Pointer to the location of the 16-byte plain text block.
*
* \param[in]  pKey Pointer to the location of the 128-bit key.
*
* \param[out]  pOutput Pointer to the location to store the 16-byte ciphered output.
*
* \pre All Input/Output pointers must refer to a memory address alligned to 4 bytes!
*
********************************************************************************** */
void LTC_AES128_Decrypt(const uint8_t* pInput,
                        const uint8_t* pKey,
                        uint8_t* pOutput);

/*! *********************************************************************************
* \brief  This function performs AES-128-ECB encryption on a message block.
*
* \param[in]  pInput Pointer to the location of the input message.
*
* \param[in]  inputLen Input message length in bytes.
*
* \param[in]  pKey Pointer to the location of the 128-bit key.
*
* \param[out]  pOutput Pointer to the location to store the ciphered output.
*
* \pre All Input/Output pointers must refer to a memory address alligned to 4 bytes!
*
********************************************************************************** */
void LTC_AES128_ECB_Encrypt(uint8_t* pInput,
                            uint32_t inputLen,
                            uint8_t* pKey,
                            uint8_t* pOutput);

/*! *********************************************************************************
* \brief  This function performs AES-128-CBC encryption on a message block.
*
* \param[in]  pInput Pointer to the location of the input message.
*
* \param[in]  inputLen Input message length in bytes.
*
* \param[in]  pInitVector Pointer to the location of the 128-bit initialization vector.
*
* \param[in]  pKey Pointer to the location of the 128-bit key.
*
* \param[out]  pOutput Pointer to the location to store the ciphered output.
*
********************************************************************************** */
void LTC_AES128_CBC_Encrypt(uint8_t* pInput,
                            uint32_t inputLen,
                            uint8_t* pInitVector,
                            uint8_t* pKey,
                            uint8_t* pOutput);
							  
/*! *********************************************************************************
* \brief  This function performs AES-128-CTR encryption on a message block.
*
* \param[in]  pInput Pointer to the location of the input message.
*
* \param[in]  inputLen Input message length in bytes.
*
* \param[in]  pCounter Pointer to the location of the 128-bit counter.
*
* \param[in]  pKey Pointer to the location of the 128-bit key.
*
* \param[out]  pOutput Pointer to the location to store the ciphered output.
*
********************************************************************************** */
void LTC_AES128_CTR(uint8_t* pInput,
                    uint32_t inputLen,
                    uint8_t* pCounter,
                    uint8_t* pKey,
                    uint8_t* pOutput);
							  
/*! *********************************************************************************
* \brief  This function performs AES-128-CMAC on a message block.
*
* \param[in]  pInput Pointer to the location of the input message.
*
* \param[in]  inputLen Length of the input message in bytes.
*
* \param[in]  pKey Pointer to the location of the 128-bit key.
*
* \param[out]  pOutput Pointer to the location to store the 16-byte authentication code.
*
* \remarks This is public open source code! Terms of use must be checked before use!
*
********************************************************************************** */
void LTC_AES128_CMAC(uint8_t* pInput,
                     uint32_t inputLen,
                     uint8_t* pKey,
                     uint8_t* pOutput);
							  
/*! *********************************************************************************
* \brief  This function performs AES-128-CCM on a message block.
*
* \param[in]  pInput Pointer to the location of the input message.
*
* \param[in]  inputLen Length of the input message in bytes.
*
* \param[in]  pInitVector Pointer to the location of the 128-bit initialization vector.
*
* \param[in]  pCounter Pointer to the location of the 128-bit counter.
*
* \param[in]  pKey Pointer to the location of the 128-bit key.
*
* \param[out]  pOutput Pointer to the location to store the 16-byte authentication code.
*
* \param[out]  pOutCbcMac Pointer to the location to store the authentication code.
*
* \param[in]  flags specifies if operation is encrypt/decrypt.
*
********************************************************************************** */
uint8_t LTC_AES128_CCM(uint8_t* pInput,
                       uint32_t inputLen,
                       uint8_t* pAuthData,
                       uint32_t authDataLen,
                       uint8_t* pInitVector,
                       uint8_t* pCounter,
                       uint8_t* pKey,
                       uint8_t* pOutput,
                       uint8_t* pCbcMac,
                       uint8_t flags);


/*! *********************************************************************************
*
********************************************************************************** */
uint8_t LtcBle_aesCcmEncrypt (uint8_t *pKey,
                              uint8_t *pNonce,
                              uint8_t aad,
                              uint8_t *pInput,
                              uint8_t *pLength,
                              uint8_t *pOutput);

/*! *********************************************************************************
*
********************************************************************************** */
uint8_t LtcBle_aesCcmDecrypt (uint8_t *pKey,
                              uint8_t *pNonce,
                              uint8_t aad,
                              uint8_t *pInput,
                              uint8_t *pLength,
                              uint8_t *pOutput);

#endif /* __LTC_INTERFACE_H__ */