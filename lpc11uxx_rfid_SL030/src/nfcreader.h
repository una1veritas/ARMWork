#ifndef _NFCREADER_H_
#define _NFCREADER_H_

#define USE_SL030

#if defined (USE_PN532)

#include "PN532_I2C.h"
extern PN532 nfcreader;
extern const byte NFCPolling[];

#elif defined(USE_SL030)

#include "StrongLink_I2C.h"
#define SL030_TAGDETECT PIO0_17
#define SL030_WAKEUP PIO0_16

extern StrongLink_I2C nfcreader;

#endif

#include "ISO14443.h"


uint8 getIDInfo(ISO14443CardInfo & card, IDCardFormat & data, const byte authkey[8]);
uint8 putIDInfo(ISO14443CardInfo & card, IDCardFormat & data, const byte authkey[8]);
//void IDDataString(char *, const uint8 cardtype, const IDData &);

uint8 get_MifareBlock(ISO14443CardInfo & card, IDCardFormat & data, const uint8_t * key);
uint8 get_FCFBlock(ISO14443CardInfo & card, IDCardFormat & data);

extern const byte IizukaKey_b[7];
extern const byte factory_a[7];

#define ACCESSBITS(x)     ( ((x)[8])<<4 & 0xff0L | ((x)[7])>>4 & 0x000fL )
#define TRAILERBITS(x)    ((((x)&1L)<<3)<<8 | (((x)>>1&1L)<<3)<<4 | (((x)>>2&1L)<<3))
#define DATABLOCKBITS(x, b)    ((((x)&1L)<<(b&3))<<8 | (((x)>>1&1L)<<(b&3))<<4 | (((x)>>2&1L)<<(b&3)))

#endif
