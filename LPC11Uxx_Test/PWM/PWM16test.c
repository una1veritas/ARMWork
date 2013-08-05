/****************************************************************************
 *   $Id:: PWM16_32test.c 4246 2010-08-10 00:43:28Z usb00423                $
 *   Project: NXP LPC11xx 16-bit/32-bit PWM example
 *
 *   Description:
 *     This file contains PWM test modules, main entry, to test PWM APIs.
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

#include "LPC11Uxx.h"                        /* LPC11xx definitions */
#include "armcmx.h"
#include "gpio.h"
#include "timer16.h"
#include "delay.h"

#include "PWM0tone.h"

#ifdef CAPPUCINO
#define LCDBKLT PIO0_3
#else
#define LCDBKLT PIO1_3
#endif
#define LCDRST  PIO1_25
#define USERLED PIO1_6
#define USERBTN PIO0_1
#define RXD2    PIO0_18
#define TXD2    PIO0_19

/* Main Program */

int main (void) 
{ 
  long toggled;
  long swatch, btndur;
  uint8_t btnstate;
  
  SystemInit();
  SystemCoreClockUpdate();  

  start_delay();
  GPIOInit();
  
  pinMode(USERBTN, INPUT);
  pinMode(USERLED, OUTPUT);
  pinMode(LCDBKLT, OUTPUT);
  
  PWM0_tone(PIO1_13, 1320, 100);
	PWM0_tone(PIO1_13, 1540, 100);

  toggled = millis();
  btnstate = HIGH;

  while (1)                                /* Loop forever */
  {
    if ( btnstate == HIGH && digitalRead(PIO0_1) == LOW ) {
      btnstate = LOW;
      swatch = millis();
    } else
    if ( btnstate == LOW && digitalRead(PIO0_1) == HIGH ) {
      btnstate = HIGH;
      if ( millis() >= swatch + 10 ) {
        btndur = millis() - swatch;
      } else {
        btndur = 0;
      }
      if ( btndur > 667 ) {
        digitalToggle(LCDBKLT);
        PWM0_tone(PIO0_8, 1320, 100);
        PWM0_tone(PIO0_8, 1540, 100);
      } else {
        PWM0_tone(PIO0_8, 1100, 100);
        PWM0_tone(PIO0_8, 880, 100);
      }
    }
    if ( millis() >= toggled + 1000 ) {
      digitalToggle(USERLED);
      toggled = millis();
    }
  }
}

