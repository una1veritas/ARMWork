/****************************************************************************
 *   $Id:: tmr32test.c 4101 2010-08-02 16:52:26Z usb00423                   $
 *   Project: NXP LPC11Uxx 32-bit timer example
 *
 *   Description:
 *     This file contains 32-bit timer test modules, main entry, to test 
 *     timer APIs.
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
#include "timer32.h"
#include "gpio.h"

#define TEST_TIMER_NUM		0		/* 0 or 1 for 32-bit timers only */

extern volatile uint32_t timer32_0_counter[4];
extern volatile uint32_t timer32_1_counter[4];

/* Main Program */

int main (void) 
{

  SystemCoreClockUpdate();

  /* TEST_TIMER_NUM is either 0 or 1 for 32-bit timer 0 or 1. */
  init_timer32(TEST_TIMER_NUM, TIME_INTERVAL);
  enable_timer32(TEST_TIMER_NUM);

  /* Enable AHB clock to the GPIO domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);
    
  /* Set port 1_20 to output */
  GPIOSetDir( 0, 6, 1 );

  while (1)                                /* Loop forever */
  {
#if TEST_TIMER_NUM
	/* I/O configuration and LED setting pending. */
	if ( (timer32_1_counter[0] > 0) && (timer32_1_counter[0] <= 50) )
	{
	  GPIOSetBitValue( 0, 6, 0 );
	}
	if ( (timer32_1_counter[0] > 50) && (timer32_1_counter[0] <= 100) )
	{
	  GPIOSetBitValue( 0, 6, 1 );
	}
	else if ( timer32_1_counter[0] > 100 )
	{
	  timer32_1_counter[0] = 0;
	}
#else
	/* I/O configuration and LED setting pending. */
	if ( (timer32_0_counter[0] > 0) && (timer32_0_counter[0] <= 50) )
	{
	  GPIOSetBitValue( 0, 6, 0 );
	}
	if ( (timer32_0_counter[0] > 50) && (timer32_0_counter[0] <= 100) )
	{
	  GPIOSetBitValue( 0, 6, 1 );
	}
	else if ( timer32_0_counter[0] > 100 )
	{
	  timer32_0_counter[0] = 0;
	}
#endif
  }
}
