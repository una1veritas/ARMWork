//*****************************************************************************
//   +--+
//   | ++----+
//   +-++    |
//     |     |
//   +-+--+  |
//   | +--+--+
//   +----+    Copyright (c) 2009 Code Red Technologies Ltd.
//
// LED flashing SysTick application for LPCXPresso1343 board
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
//
//*****************************************************************************
#include "LPC13xx.h"                        /* LPC13xx definitions */
#include "gpio.h"

// Time period to wait between toggling LED. 2000 msecs = 2 secs.
#define DELAY_LEN 2000

volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
__INLINE static void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/* Main Program */

int main (void) {
	  /* Basic chip initialization is taken care of in SystemInit() called
	   * from the startup code. SystemInit() and chip settings are defined
	   * in the CMSIS system_<part family>.c file.
	   */

  GPIOInit();

  /* Set port 0_7 - LED2 on LPCXpresso board to output */
  GPIOSetDir( 0, 7, 1 );

  if (SysTick_Config(SystemCoreClock / 1000)) { /* Setup SysTick Timer for 1 msec interrupts  */
    while (1);                                  /* Capture error */
  }

  if ( !(SysTick->CTRL & (1<<SysTick_CTRL_CLKSOURCE_Msk)) )
  {
	/* When external reference clock is used(CLKSOURCE in
	Systick Control and register bit 2 is set to 0), the 
	SYSTICKCLKDIV must be a non-zero value and 2.5 times 
	faster than the reference clock. 
	When core clock, or system AHB clock, is used(CLKSOURCE 
	in Systick Control and register bit 2 is set to 1), the 
	SYSTICKCLKDIV has no effect to the SYSTICK frequency. See 
	more on Systick clock and status register in Cortex-M3 
	technical Reference Manual. */
	LPC_SYSCON->SYSTICKCLKDIV = 0x08;
  }


  while (1)                                /* Loop forever */
  {
	  // Turn LED on, then wait
	  GPIOSetValue( 0, 7, 1 );
	  Delay (DELAY_LEN);

	  // Turn LED off, then wait
	  GPIOSetValue( 0, 7, 0 );
	  Delay (DELAY_LEN);
  }
}
