/******************************************************************************
* This public Source file is for the functions that are common in the stack subsystems. 
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
* Freescale Semiconductor Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
******************************************************************************/
#include "EmbeddedTypes.h"
#include "NV_Data.h"
#include "MsgSystem.h"
#include "NVM_Interface.h"
#include "FunctionLib.h"
//#include "PublicConst.h"
#ifdef PROCESSOR_KINETIS
#include "RNG_Interface.h"
#endif

#include "BeeStackUtil.h"
#include "BeeStack_Globals.h"

#include "ZdoApsInterface.h"
#include "AppZdoInterface.h"
//#include "AppAspInterface.h"
#include "AfApsInterface.h"

#include "ZdoNwkInterface.h"
 
/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

/*None*/

/******************************************************************************
*******************************************************************************
* Private Protypes
*******************************************************************************
******************************************************************************/

/*None*/

/******************************************************************************
*******************************************************************************
* Private type Definations
*******************************************************************************
******************************************************************************/

/*None*/

/******************************************************************************
*******************************************************************************
* Public memory Declarations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Memory Declerations
*******************************************************************************
******************************************************************************/


uint16_t  gRandom;

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/
/*
  Frees the buffer when it has a message Id  just before the address of the give pointer.
*/
void FreeRootBuffer(void *pMsg)
{
  void * ptr;
  ptr = (void*)((uint8_t *)pMsg - sizeof(zbMsgId_t));
  MSG_Free(ptr);
}

/*
  Return a native 16-bit value from an OTA 2-byte array.
*/
uint16_t GetNative16BitInt(TwoByteArray_t aArray)
{
#if  ( gBigEndian_c == TRUE)
  return OTA2Native16(*(uint16_t *)aArray);  
#else  
  uint16_t localVal;
  localVal = aArray[1];
  localVal = (localVal << 0x08) | aArray[0];
  return  localVal;
#endif    
}

/*
  Return a native 32-bit value from an OTA 4-byte array.
*/
uint32_t GetNative32BitInt(FourByteArray_t aArray)
{
#if  ( gBigEndian_c == TRUE)
  return OTA2Native32(*(uint32_t *)aArray);  
#else  
  uint32_t localVal;
  localVal = aArray[3];
  localVal = (localVal << 0x08) | aArray[2];
  localVal = (localVal << 0x08) | aArray[1];
  localVal = (localVal << 0x08) | aArray[0];
  return  localVal;
#endif    
}

/*
  Set an OTA 2-byte array from a native 16-bit value.
*/
void SetNative16BitInt(TwoByteArray_t aArray, uint16_t iInt16)
{
  iInt16 = Native2OTA16(iInt16);
  Copy2Bytes(aArray, &iInt16);
}

/*
  Set an OTA 4-byte array from a native 32-bit value.
*/
void SetNative32BitInt(FourByteArray_t aArray, uint32_t iInt32)
{
  iInt32 = Native2OTA32(iInt32);
  FLib_MemCpy(aArray, &iInt32, sizeof(iInt32));
}

/*
  Return a native 32-bit value from an OTA 3-byte array.
*/
uint32_t GetNative32BitIntFrom3ByteArray(ThreeByteArray_t aArray)
{
#if  ( gBigEndian_c == TRUE)
  FourByteArray_t localArray;
  FLib_MemCpy(localArray, aArray, sizeof(ThreeByteArray_t));
  localArray[3] = 0;
  return OTA2Native32(*(uint32_t *)localArray);  
#else  
  uint32_t localVal = 0;
  localVal = (localVal << 0x08) | aArray[2];
  localVal = (localVal << 0x08) | aArray[1];
  localVal = (localVal << 0x08) | aArray[0];
  return  localVal;
#endif    
}

/* is this ieee address ourselves? */
bool_t IsSelfIeeeAddress(zbIeeeAddr_t aIeeeAddr)
{
  return (IsEqual8Bytes(aIeeeAddr, NlmeGetRequest(gNwkIeeeAddress_c)));
}

/* is this nwk address ourselves? */
bool_t IsSelfNwkAddress(zbNwkAddr_t aNwkAddr)
{
  return IsEqual2Bytes(aNwkAddr, NlmeGetRequest(gNwkShortAddress_c));
}

/* is this a valid */
bool_t IsValidPanId(zbPanId_t aPanId)
{
  return (aPanId[1] <= gaPIDUpperLimit[0]);
}

#if !defined(__IAR_SYSTEMS_ICC__) || defined(PROCESSOR_KINETIS)
/* Is this one of the broadcast addresses? */
bool_t IsBroadcastAddress(zbNwkAddr_t aNwkAddr)
{
  return (aNwkAddr[1] == 0xff && (aNwkAddr[0] == 0xff || aNwkAddr[0] == 0xfd || aNwkAddr[0] == 0xfc));
}

/* is this a valid nwk addr? */
bool_t IsValidNwkUnicastAddr(zbNwkAddr_t aNwkAddr)
{
  /* allow addresses 0x0000 - 0xfff0 (little endian) */
  return ((aNwkAddr[1] != 0xff) || (aNwkAddr[0] <= 0xf0));
}

/* is this a valid nwk addr? */
bool_t IsIncompleteBindNwkAddr(zbNwkAddr_t aNwkAddr)
{
  /* allow addresses 0x0000 - 0xfff0 (little endian) */
  return ((aNwkAddr[1] == 0xff) && (aNwkAddr[0] == 0xfe));
}

/* Is this a valid address to send to? */
bool_t IsValidNwkAddr(zbNwkAddr_t aNwkAddr)
{
  return (IsValidNwkUnicastAddr(aNwkAddr) || IsBroadcastAddress(aNwkAddr));
}
#endif

#if !defined(__IAR_SYSTEMS_ICC__) || defined(PROCESSOR_KINETIS)
/* is this a valid extended PAN ID? */
bool_t IsValidExtendedPanId(zbIeeeAddr_t aIeeeAddr)
{
  return !(Cmp8BytesToZero(aIeeeAddr) || Cmp8BytesToFs(aIeeeAddr));
}
#endif 
/* Validates if the current logical channel is between the right range. */

bool_t IsValidLogicalChannel(uint8_t iChannel)
{
  /* The range for valid channle goes from 11 up to 26 */
  //return (11 <= iChannel <= 26);
  return ((11 <= iChannel) && (iChannel <= 26));
}

/*
  BeeUtilLargeMemSet

  This can set a large buffer (bigger than FLib_MemSet)
*/
#if !defined(__IAR_SYSTEMS_ICC__) || defined(PROCESSOR_KINETIS)
void BeeUtilLargeMemSet(void *pBuffer, uint8_t value, uint16_t iCount)
{
  /* Use the byte pointer instead of casting. */
  uint8_t *pBuffer_uint8 = pBuffer;
  while(iCount)
  {
   *pBuffer_uint8 = value;
    pBuffer_uint8++;
    --iCount;
  }
}

/*
  Set a large buffer to 0
*/
void BeeUtilLargeZeroMemory(void *pBuffer, uint16_t iCount)
{
  BeeUtilLargeMemSet(pBuffer, 0, iCount);
}

/*
  Clears a buffer to zero
*/
void BeeUtilZeroMemory
(
	void  *pPtr,          /* IN: The poointer to the buffer to be cleaned. */
	zbSize_t  bufferSize  /* IN: The amount of byte to clean. */
)
{
	if (pPtr == NULL)
		return;

	FLib_MemSet(pPtr, 0x00, bufferSize);
}
#endif
void BeeUtilSetToF
(
  void  *pPtr,          /* IN: The poointer to the buffer to be cleaned. */
  zbSize_t  bufferSize  /* IN: The amount of byte to clean. */
)
{
  if (pPtr == NULL)
		return;

	FLib_MemSet(pPtr, 0xFF, bufferSize);
}


#if !(gBigEndian_c)
/* This pair of function handle the needs for the Kaibab platform (ARM 7) */
bool_t IsEqual2BytesInt(void *ptr, uint16_t val)
{
  return FLib_MemCmp(ptr,&val,2);
}

/*
  Set2Bytes
  copies a 16 bit variable to a location with the bytes swapped
  Note needed for big endian
*/
void Set2Bytes(void *ptr, uint16_t val)
{
  /* copy bytes to pointer*/  
  Copy2Bytes(ptr,&val);
}  

void Inc4Bytes(uint8_t* aVal1, uint32_t Val2)
{
uint32_t temp;
temp = (uint32_t)TwoBytesToUint16(aVal1);
temp |= (((uint32_t)TwoBytesToUint16(((uint8_t*)aVal1)+2))<<16);
temp += Val2;
FLib_MemCpy(aVal1,(void*)&temp,4);
}
#endif /* #if (gBigEndian_c)*/
/*
  Copy8Bytes

  Copies 8 bytes from one location to another. Assumes they don't overlap. Used 
  throughout the code to code size.
*/
void Copy8Bytes(zbIeeeAddr_t aIeeeAddr1,zbIeeeAddr_t aIeeeAddr2)
{
  FLib_MemCpy(aIeeeAddr1, aIeeeAddr2, sizeof(zbIeeeAddr_t));
}

/*
  Colpies 16 bytes from one location to another. Assumes they do not overlap
  and that the poiners are not null pointers.
*/
void Copy16Bytes(void *pDst, void *pSrc)
{
  FLib_MemCpy(pDst, pSrc, sizeof(zbAESKey_t));
}

/*
  IsEqual8Bytes

  Compares two Ieee addresses. Used throughout the code to reduce space.
*/
bool_t IsEqual8Bytes(zbIeeeAddr_t aIeeeAddr1, zbIeeeAddr_t aIeeeAddr2)
{
  /* note: FLib_MemCmp returns 1 if equal, 0 if not */
  return FLib_MemCmp(aIeeeAddr1, aIeeeAddr2, sizeof(zbIeeeAddr_t));
}

/*
  Cmp8BytesToZero

  Are all the bytes 0 in this array?
*/
bool_t Cmp8BytesToZero(zbIeeeAddr_t aIeeeAddr1)
{
  return BeeUtilArrayIsFilledWith(aIeeeAddr1, 0, sizeof(zbIeeeAddr_t));
}

/*
  Cmp16BytesToZero

  Are all the bytes equal zero in this array?
*/
bool_t Cmp16BytesToZero(void *pArray)
{
  return BeeUtilArrayIsFilledWith(pArray, 0, sizeof(zbAESKey_t));
}

/*
  Cmp8BytesToFs

  Are all the bytes 0xFF in this array?
*/
bool_t Cmp8BytesToFs(zbIeeeAddr_t aIeeeAddr1)
{
  return BeeUtilArrayIsFilledWith(aIeeeAddr1, 0xFF, sizeof(zbIeeeAddr_t));
}

/* Are all the bytes equal in this two arrays? */
bool_t Cmp8Bytes(zbIeeeAddr_t  aIeeeAddr1, zbIeeeAddr_t aIeeeAddr2)
{
  return FLib_MemCmp(aIeeeAddr1, aIeeeAddr2, sizeof(zbIeeeAddr_t));
}

/* Are all the bytes equal in this two arrays? */
bool_t Cmp16Bytes(zbAESKey_t aAESKey1, zbAESKey_t aAESKey2)
{
  return FLib_MemCmp(aAESKey1, aAESKey2, sizeof(zbAESKey_t));
}

/* Is the given buffer filled with zeros? */
bool_t CmpToZero(void *pPtr, zbSize_t size)
{
  return BeeUtilArrayIsFilledWith(pPtr, 0x00, size);
}

/* Is the given buffer filled with fs */
bool_t CmpToFs(void *pPtr, zbSize_t size)
{
  return BeeUtilArrayIsFilledWith(pPtr, 0xFF, size);
}

/*
  Fill8BytesToZero

  Fill with 0s
*/
void Fill8BytesToZero(zbIeeeAddr_t aIeeeAddr1)
{
  FLib_MemSet(aIeeeAddr1, 0, sizeof(zbIeeeAddr_t));
}

/*
  FillWithZero

  Fill with 0s: DEPRECATED
*/
//void FillWithZero(void *pBuffer, uint8_t size)
//{
//  FLib_MemSet(pBuffer, 0, size);
//}

/*
  Swap2Bytes

  Used to convert between over-the-air (OTA) format (little endian) and native 
  format (big endian on HCS08).
*/
#if !defined(__IAR_SYSTEMS_ICC__) || defined(PROCESSOR_KINETIS)
#if gBigEndian_c
// HCS08 version
uint16_t Swap2Bytes(uint16_t iOldValue)
{
  uint16_t iValue;

  iValue = ((uint8_t *)&iOldValue)[1] << 8;
  iValue += ((uint8_t *)&iOldValue)[0];

  return iValue;
}
#endif
#endif
/*
  Swap2BytesArray

  Used to convert between over-the-air (OTA) format (little endian) and native 
  format (big endian on HCS08).
*/
#if !defined(__IAR_SYSTEMS_ICC__) || defined(PROCESSOR_KINETIS)
void Swap2BytesArray(uint8_t *pArray)
{
	uint8_t aTmp[2];

	aTmp[0] = pArray[1];
	aTmp[1] = pArray[0];

	Copy2Bytes(pArray, aTmp);
}

/*
	Swap4Bytes

	Used in the securityto convert over-the-air (OTA) frame counters and native format (big endian on HCS08).
*/
uint32_t Swap4Bytes(uint32_t iOldValue)
{
	uint32_t iNewValue;
	uint8_t *pBytes;
	uint8_t byte;

	iNewValue = iOldValue;

	/* swap the bytes */
	pBytes = (uint8_t *)&iNewValue;
	byte = pBytes[3];
	pBytes[3] = pBytes[0];
	pBytes[0] = byte;
	byte = pBytes[2];
	pBytes[2] = pBytes[1];
	pBytes[1] = byte;

	return iNewValue;
}

void Swap8Bytes(uint8_t  *pInput)
{
  uint8_t i;
  uint8_t j;
  uint8_t size = sizeof(zbIeeeAddr_t);
  zbIeeeAddr_t  Output;

  for (i = 0, j = (size - 1); i < size; i++, j--)
  {
    Output[i] = pInput[j];
  }

  Copy8Bytes(pInput, Output);
}

/*
  FLib_MemChr

  Look through array for this byte.
*/

uint8_t *FLib_MemChr(uint8_t *pArray, uint8_t iValue, uint8_t iLen)
{
  index_t i;
  for(i=0; i<iLen; ++i) {
    if(pArray[i] == iValue)
      return &(pArray[i]);
  }
  return NULL;
}
#endif
/*
  BeeUtilSetIndexedBit

  Sets a single bit in the array, as indexed by iBit.
*/
#if !defined(__IAR_SYSTEMS_ICC__) || defined(PROCESSOR_KINETIS)
void BeeUtilSetIndexedBit(uint8_t *pBitArray, index_t iBit)
{
  pBitArray += (iBit >> 3);
  *pBitArray |= 1 << (iBit & 0x07);
}

/*
  BeeUtilGetIndexedBit

  Returns TRUE if bit is set, FALSE if not.
*/
bool_t BeeUtilGetIndexedBit(uint8_t *pBitArray, index_t iBit)
{
  pBitArray += (iBit >> 3);
  return (*pBitArray & (1 << (iBit & 0x07)));
}

/*
  BeeUtilClearIndexedBit

  Clears a single bit in the array, as indexed by iBit.

  Returns 0x00 if old bit was not set. Returns 0x01 if old bit was set.
*/
uint8_t BeeUtilClearIndexedBit(uint8_t *pBitArray, index_t iBit)
{
  uint8_t oldBit;
  uint8_t iBitMask;

  /* determine if old bit was set */
  pBitArray += (iBit >> 3);
  iBitMask = (1 << (iBit & 0x07));
  oldBit = 0;
  if(*pBitArray & iBitMask)
    ++oldBit;

  /* clear that bit */
  *pBitArray &= ~iBitMask;

  return oldBit;
}

/*
  BeeUtilArrayIsFilledWith

  Check each byte in the array. Length is in bytes.
*/
bool_t BeeUtilArrayIsFilledWith(uint8_t *pArray, uint8_t value, index_t iLen)
{
  index_t i;
  
  for(i=0; i<iLen; ++i) {
    if(pArray[i] != value)
      return FALSE;
  }
  return TRUE;
}

/*
  BeeUtilBitToIndex

  Determine first bit set in a bit array. length is in bytes.

  Returns 0 if not found (so assumes bit 0 set).
*/
uint8_t BeeUtilBitToIndex(uint8_t *pBitArray, index_t iLen)
{
  index_t i;
  uint8_t iBitThisByte;

  for(i=0; i<iLen; ++i) {
    for(iBitThisByte=0; iBitThisByte < 8; ++iBitThisByte ) {
      if((pBitArray[i]) & (1 << iBitThisByte))
        return ((i << 3) + iBitThisByte);
    }
  }
  return 0;
}
#endif 
/*
  SeedRandomNumber

  Seeds the random number generator.

  This function handles both platforms QE (HSC08) and Kaibab (ARM 7)
*/
#ifndef PROCESSOR_KINETIS
extern uint16_t CM_RandomBackoff;
#endif

void SeedRandomNumber(uint16_t  seed)
{
#ifdef PROCESSOR_KINETIS
  (void)seed;
#else
  (void)seed;
  gRandom = CM_RandomBackoff;
#endif  
	/*Seed random no with value from Radio*/
  /* not needed 
	mac_randomw = seed;
  */
}

/*
  GetRandomNumber

  Returns a random # between 0-65535. Make sure to seed initially (or often) 
  with some external random number such as something time based. See 
  SeedRandomNumber().

  This function handles both platforms QE (HSC08) and Kaibab (ARM 7)
*/
#if !defined(__IAR_SYSTEMS_ICC__) || defined(PROCESSOR_KINETIS)

uint32_t GetRandomNumber(void)
{
#ifdef PROCESSOR_KINETIS
  uint32_t rnd;
  (void)RNG_GetRandomNo(&rnd);
  return (rnd);
#else  
  return maca_random;
#endif /* PROCESSOR_KINETIS */
}
#if 0
uint16_t GetRandomNumber(void)
{
  gRandom = (gRandom * 6075) + 1283;
  return gRandom;
}
#endif
  


/*
  GetRandomRange

  For example, if you want a number between 0-15, pass 0,15
*/
uint8_t GetRandomRange(uint8_t low, uint8_t high)
{
	/* The new random fucntion return 8 bits numbers */
  uint16_t random;

	/* The next two lines were comented out to use the new random number generator */

#ifdef PROCESSOR_KINETIS
  uint32_t rnd;
(void)RNG_GetRandomNo(&rnd);
  random = (uint16_t)rnd;
#else  
  random = maca_random;
#endif /* PROCESSOR_KINETIS */ 

#if 0
 // SeedRandomNumber(CM_GetRandomBackoff( 22 ));
  random = GetRandomNumber();
#endif  
	

  if(high <= low)
    return low;
  return low + (uint8_t)(random % (high - low + 1));
}
#endif /*not defined iar systems || defined(PROCESSOR_KINETIS)*/

/* Free all the meesages from a given anchor */
#if !defined(__IAR_SYSTEMS_ICC__) || defined(PROCESSOR_KINETIS)
void DeQueueAllMessages ( anchor_t *pAnchor )
{
  void *pMsg;
  /* dequeue all pending messages */
  while( MSG_Pending( pAnchor ) ) {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( pAnchor );
    /* Free the message */
    MSG_Free( pMsg );
  }
}
#endif

