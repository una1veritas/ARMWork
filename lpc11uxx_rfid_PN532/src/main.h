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

void parse_do_command(StringBuffer & sbuf);

// Translation template data structure

union IDCardFormat {
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
  uint8 raw[64];
};

#endif
