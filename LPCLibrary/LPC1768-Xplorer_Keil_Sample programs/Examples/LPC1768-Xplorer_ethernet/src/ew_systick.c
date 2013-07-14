//*****************************************************************************
//   +--+       
//   | ++----+   
//   +-++    |  
//     |     |  
//   +-+--+  |   
//   | +--+--+  
//   +----+    Copyright (c) 2009 Code Red Technologies Ltd. 
//
// ew_systick.c provided SysTick functions for use by EasyWeb port to RDB1768
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

#include "LPC17xx.h"

extern void TCPClockHandler(void);

volatile uint32_t TimeTick = 0;
volatile uint32_t TimeTick2 = 0;


// ****************
//  SysTick_Handler
void SysTick_Handler(void)
{
	TimeTick++;		// Increment first SysTick counter
	TimeTick2++;	// Increment second SysTick counter
	
	// After 100 ticks (100 x 10ms = 1sec)
	if (TimeTick >= 100) {
	  TimeTick = 0;	// Reset counter
	  LPC_GPIO1->FIOPIN ^= 1 << 25;	// Toggle user LED
	}
	// After 20 ticks (20 x 10ms = 1/5sec)
	if (TimeTick2 >= 1) {
	  TimeTick2 = 0; // Reset counter
	  TCPClockHandler();  // Call TCP handler
	}  
}

// ****************
// Setup SysTick Timer to interrupt at 10 msec intervals
void Start_SysTick10ms(void)
{
	if (SysTick_Config(SystemCoreClock / 100)) { 
		while (1);  // Capture error
	}
}
