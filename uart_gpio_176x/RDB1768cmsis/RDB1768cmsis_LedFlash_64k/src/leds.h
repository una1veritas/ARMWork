//*****************************************************************************
//   +--+       
//   | ++----+   
//   +-++    |  
//     |     |  
//   +-+--+  |   
//   | +--+--+  
//   +----+    Copyright (c) 2009 Code Red Technologies Ltd. 
//
// leds.h header declares functions and constansts for accessing the "user LEDs"
// on the RDB1768 development board (LEDs 2-5).
//
//
// Software License Agreement
// 
// The software is owned by Code Red Technologies and/or its suppliers, and is 
// protected under applicable copyright laws.  All rights are reserved.  Any 
// use in violation of the foregoing restrictions may subject the user to criminal 
// sanctions under applicable laws, as well as to civil liability for the breach 
// of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD. 

#ifndef LEDS_H_
#define LEDS_H_

// Set of defines for RDB1768 board user leds
// Led 2 is P1_24
#define LED_2 (1 << 24)
// Led 3 is P1_25
#define LED_3 (1 << 25)
// Led 4 is P1_28
#define LED_4 (1 << 28)
// Led 5 is P1_29
#define LED_5 (1 << 29)
// All Leds
#define LED_ALL (LED_2 | LED_3 | LED_4 | LED_5)

void leds_init (void);
void leds_all_on (void);
void leds_all_off (void);
void leds_invert (void);
void led_on (unsigned int ledstate);
void led_off (unsigned int ledstate);

#endif /*LEDS_H_*/
