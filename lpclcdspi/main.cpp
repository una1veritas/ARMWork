/****************************************************************************
 *   $Id:: ssptest.c 4103 2010-08-02 20:56:20Z usb00423                     $
 *   Project: NXP LPC11Uxx SSP example
 *
 *   Description:
 *     This file contains SSP test modules, main entry, to test SSP APIs.
*
****************************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.

* Permission to use, copy, modify, and distribute this software and its 
* documentation is hereby granted, under NXP Semiconductors' 
* relevant copyright in the software, without fee, provided that it 
* is used in conjunction with NXP Semiconductors microcontrollers.  This 
* copyright, permission, and disclaimer notice must appear in all copies of 
* this code.

****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "LPC11Uxx.h"
#include "type.h"

#include "armcmx.h"
#include "SPIBus.h"
#include "SPISRAM.h"
#include "USARTSerial.h"


//#define SSP_NUM			0
#define SSP_CS1       PIO1_23
#define LED_SD_BUSY   PIO1_19

SPIBus SPI1(&SPI1Def, SSP_CS1, SSP_CS1, SSP_CS1, SSP_CS1);
SPISRAM sram(SPI1, SSP_CS1, SPISRAM::BUS_MBITS);

/******************************************************************************
**   Main Function  main()
******************************************************************************/

char text[] = "Happy are those who knwon they are spiritually poor;";
int n = strlen(text);

int main (void) {
  uint32_t t = 0;
  uint16_t i;
  char strbuf[64];
  char c;
   
  SystemCoreClockUpdate();
  start_delay();

  Serial.begin(115200);
  Serial.println("Hello.");
  
  SPI1.begin();
  sram.begin();

  srand((uint16_t)micros());
  
  while ( 1 ) {
    c = text[t];
    Serial.print(t, HEX);
    Serial.print(" ");
    Serial.print(c);
    sram.write(t, c);
    c = sram.read(t);
    Serial.print(" ");
    Serial.print(c);
    Serial.print(" (");
    Serial.print(c, HEX);
    Serial.println(")");

    t++;
    t %= n;
    delay(1000);
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
