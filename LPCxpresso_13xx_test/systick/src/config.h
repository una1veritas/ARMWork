/*****************************************************************************
  *   config.h:  config file for extint example for NXP LPC13xx Family
 *   Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.07.19  ver 1.00    Preliminary version, first Release
 *
******************************************************************************/

/*
Overview:
   This example shows how to set up I/O pins to generate interrupts. It uses
   libLPCxxx to set up an interrupt on P2.1 that toggles an LED.

User #defines (in lpcxpresso_extint.c):
   LED_PORT	I/O port driving an LED
   LED_BIT  I/O port bit driving an LED
   LED_ON   Value to set the bit to turn on the LED
   LED_OFF  Value to set the bit to turn off the LED

How to use:
   Click the debug toolbar button.

   * Use a jumper wire to pull P2.1 low.
   * You should see the LED toggle.
*/

#define LED_PORT 0		// Port for led
#define LED_BIT 7		// Bit on port for led
#define LED_ON 1		// Level to set port to turn on led
#define LED_OFF 0		// Level to set port to turn off led

/*********************************************************************************
**                            End Of File
*********************************************************************************/
