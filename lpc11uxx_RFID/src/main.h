#ifndef _MAIN_H_
#define _MAIN_H_

#include "armcmx.h"

// Strawberry Linux original lpclcd port maps

#if defined(LPCLCD)
#include "lpclcd.h"

#define SD_SPI  SPI0
#define SD_SCK  PIO1_29
#define SD_MISO PIO0_8
#define SD_MOSI PIO0_9
#define SD_CS   PIO0_2
#define SD_DETECT   PIO1_4 

#define NFC_IRQ     PIO1_5
#define NFC_RSTPD   PIN_NOT_DEFINED

#define SRAM_CS PIO1_23

#elif defined (CAPPUCCINO)
#include "cappuccino.h"

#define SD_SPI  SPI0
#define SD_SCK  PIO1_29
#define SD_MISO PIO0_8
#define SD_MOSI PIO0_9
#define SD_CS   PIO0_2
#define SD_DETECT   PIO1_16 

#define NFC_IRQ     PIO0_17
#define NFC_RSTPD   PIN_NOT_DEFINED
#define LED_USER    LED_SDBUSY

#define SRAM_CS PIO1_23
#endif

#include "StringBuffer.h"
#include "ISO14443.h"

#if defined(USE_PN532)
#include "PN532_I2C.h"
#elif defined(USE_SL030)
#include "StrongLink_I2C.h"
#endif

void parse_do_command(StringBuffer & sbuf);


union IDFormat {
  struct {
    uint8 division[2];
    uint8 pid[12];
    uint8 issue;
    uint8 gender;
    uint8 namekana[16];
    uint8 orgid[8];
    uint8 dofissue[8];
    uint8 goodthru[8];
    uint8 issuerdata[8];
  } fcf;
  struct {
    uint8 division[2];
    uint8 pid[8];
    uint8 issue;
    uint8 reserved1[5];
    uint8 namesjis[16];
    uint8 dofbirth[7];
    uint8 gender;
    uint8 dofissue[7];
    uint8 reserved2;
  } iizuka;
};


// Translation template data structure

#if defined(USE_PN532)
extern PN532 nfcreader;
extern const byte NFCPolling[];
#elif defined(USE_SL030)
extern StrongLink_I2C nfcreader;
#endif

uint8 getIDInfo(ISO14443Card & card, const byte authkey[8], char data[64]);
uint8 putIDInfo(ISO14443Card & card, const byte authkey[8], char data[64]);
//void IDDataString(char *, const uint8 cardtype, const IDData &);

uint8 get_MifareBlock(ISO14443Card & card, const uint8_t * key, char data[64]);
uint8 get_FCFBlock(ISO14443Card & card, char data[64]);

extern const byte IizukaKey_b[7];
extern const byte factory_a[7];

#define ACCESSBITS(x)     ( ((x)[8])<<4 & 0xff0L | ((x)[7])>>4 & 0x000fL )
#define TRAILERBITS(x)    ((((x)&1L)<<3)<<8 | (((x)>>1&1L)<<3)<<4 | (((x)>>2&1L)<<3))
#define DATABLOCKBITS(x, b)    ((((x)&1L)<<(b&3))<<8 | (((x)>>1&1L)<<(b&3))<<4 | (((x)>>2&1L)<<(b&3)))

#endif
