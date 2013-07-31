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

#define LCDBKLT PIO0_3
#define LCDRST  PIO1_25
#define USERLED PIO1_6
#define USERBTN PIO0_1
#define RXD2    PIO0_18
#define TXD2    PIO0_19

volatile uint32_t period;
void beep_up(void);
void beep_down(void);

/* Main Program */

int main (void) 
{ 
  long swatch;
  
  SystemInit();
  SystemCoreClockUpdate();  

  start_delay();
  delay(5);
  GPIOInit();
  
  pinMode(PIO0_1, INPUT);
  pinMode(PIO1_6, OUTPUT);
  
  while (1)                                /* Loop forever */
  {
    if ( digitalRead(PIO0_1) == LOW ) {
      swatch = millis();
      while ( digitalRead(PIO0_1) == LOW) {
        if ( millis() >= swatch + 500 )
          break;
      }
      if ( millis() >= swatch + 500 )
        beep_up();
      else 
        beep_down();
    }
    digitalToggle(PIO1_6);
    delay(1000);
  }
}

void beep_up() {
    /* Initialize the PWM in timer16_0 enabling match1 output */
	period = SystemCoreClock/1320;  // PWM frequency  
  init_timer16PWM(0, period, MATCH1, 0);
//  setMatch_timer16PWM (0, 1, period/2);
  enable_timer16(0);
  delay(100);
	period = SystemCoreClock/1540;  // PWM frequency  
  init_timer16PWM(0, period, MATCH1, 0);
  enable_timer16(0);
  delay(100);
  disable_timer16(0);
}

void beep_down() {
    /* Initialize the PWM in timer16_0 enabling match1 output */
	period = SystemCoreClock/1100;  // PWM frequency  
  init_timer16PWM(0, period, MATCH1, 0);
//  setMatch_timer16PWM (0, 1, period/2);
  enable_timer16(0);
  delay(100);
	period = SystemCoreClock/880;  // PWM frequency  
  init_timer16PWM(0, period, MATCH1, 0);
  enable_timer16(0);
  delay(100);
  disable_timer16(0);
}

