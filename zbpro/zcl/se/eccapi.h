/******************************************************************************
* Eccapi.h
*
* This module contains code that handles EccApi.
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*
*******************************************************************************/
#ifndef _ECCAPI_H_
#define _ECCAPI_H_

#ifdef __cplusplus
    extern "C" {
#endif

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/



/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
#define AES_MMO_HASH_SIZE (16)

typedef int GetRandomDataFunc(unsigned char *buffer, unsigned long sz);
typedef int HashFunc(unsigned char *digest, unsigned long sz, unsigned char *data);
typedef int YieldFunc(void);
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

int ZSE_ECDSASign(unsigned char *privateKey, unsigned char *msgDigest,
              GetRandomDataFunc *GetRandomData,
              unsigned char *r, unsigned char *s,
              YieldFunc *yield, unsigned long yieldLevel);
int ZSE_ECCGenerateKey(unsigned char *privateKey, unsigned char *publicKey,
                   GetRandomDataFunc *GetRandomData,
                   YieldFunc *yield, unsigned long yieldLevel);
 
int ZSE_ECCKeyBitGenerate(unsigned char *privateKey, 
                          unsigned char *ephemeralPrivateKey, 
                          unsigned char *ephemeralPublicKey, 
                          unsigned char *remoteCertificate, 
                          unsigned char *remoteEphemeralPublicKey, 
                          unsigned char *caPublicKey, 
                          unsigned char *keyBits, HashFunc *Hash,
                          YieldFunc *yield, unsigned long yieldLevel);

int aesMmoHash(unsigned char *digest, unsigned long int sz, unsigned char *dat);

int ZSE_ECDSAVerify(unsigned char *publicKey, 
                    unsigned char *msgDigest, 
                    unsigned char *r, 
                    unsigned char *s, 
                    YieldFunc *yield, 
                    unsigned long yieldLevel);

int ZSE_ECQVReconstructPublicKey(unsigned char* certificate,
                                 unsigned char* caPublicKey,
                                 unsigned char* publicKey,
                                 HashFunc *Hash, 
                                 YieldFunc *yield, 
                                 unsigned long yieldLevel);
 
#ifdef __cplusplus
}
#endif                                 

#endif

