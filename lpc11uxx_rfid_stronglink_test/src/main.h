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


#endif
