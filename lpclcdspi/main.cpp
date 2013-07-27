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

#include "LPC11Uxx.h"
#include "type.h"

#include "armcmx.h"
#include "SPIBus.h"
#include "SPISRAM.h"



//#define SSP_NUM			0
#define SSP_CS1       PIO1_23
#define LED_SD_BUSY   PIO1_19

SPIBus SPI1(&SPI1Def, SSP_CS1, SSP_CS1, SSP_CS1, SSP_CS1);
SPISRAM sram(SPI1, SSP_CS1, SPISRAM::BUS_MBits);

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void) {
  int i;
   
  SystemCoreClockUpdate();
  start_delay();

  USART_init(&usart, PIO0_18, PIO0_19);
  USART_begin(&usart, 115200);
  USART_puts(&usart, "Hello.\n");
  
  SPI1.begin();
	
  while ( 1 ) {
    uint8_t t = millis() & 0xff;
    sram.read(0x55);
    delay(10);
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/

