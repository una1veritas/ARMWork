//*****************************************************************************
//   +--+       
//   | ++----+   
//   +-++    |  
//     |     |  
//   +-+--+  |   
//   | +--+--+  
//   +----+    Copyright (c) 2009 Code Red Technologies Ltd. 
//
// main_ledflash.c demonstrates the use of the "user LEDs" on the
// RDB1768 development board (LEDs 2-5).
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

#include "leds.h"

// Function to provide short delay
void short_delay (int n) __attribute__((noinline));
void short_delay(int n)
{
   volatile int d;
   for (d=0; d<n*3000; d++){}
}

int main(void) {

	volatile static int i = 0 ;
	int loop;

	// Initialise GPIO to access user LEDs
	leds_init();
	
	// Enter an infinite loop, cycling through led flash sequence
	while(1) {

		// Turn all leds on, then off, 5 times
		for (loop=1; loop < 5; loop++) {
			leds_all_on();
			short_delay(2000);
			leds_all_off();		
			short_delay(2000);
		}
		short_delay(3000);
		
	
		// Turn each led on (and then off) in turn
		// Pattern is 3,2,5,4 because of order leds are
		// laid out on board
		led_on (LED_3);
		short_delay(2000);		
		led_off (LED_3);
		led_on (LED_2);
		short_delay(2000);
		led_off (LED_2);
		led_on (LED_5);
		short_delay(2000);
		led_off (LED_5);
		led_on (LED_4);
		short_delay(2000);
		led_off (LED_4);
		short_delay(5000);
		
		// Turn on leds 2 and 3, ready for inverting
		led_on (LED_2);	
		led_on (LED_3);
		short_delay(2000);
		
		// Invert the current state of the leds, 5 times
		for (loop=1; loop < 5; loop++) {
			leds_invert();
			short_delay (2000);
		}
		// Turn leds off, ready for next iteration of led flashes
		leds_all_off();	
		short_delay(5000);
		i++ ;
	}
	return 0 ;
}
