/****************************************************************************
 *   $Id:: blinky.c 9190 2012-02-16 20:59:45Z nxp41306                      $
 *   Project: NXP LPC11xx Blinky example
 *
 *   Description:
 *     This file contains LED blink code example which include timer, 
 *     GPIO initialization, and clock monitoring.
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
****************************************************************************/
#include "LPC11Uxx.h"                        /* LPC11Uxx definitions */
#include "timer16.h"
#include "clkconfig.h"
#include "gpio.h"
#include "nmi.h"

#define TEST_TIMER_NUM		0		/* 0 or 1 for 16-bit timers only */

extern volatile uint32_t timer16_0_counter[2];
extern volatile uint32_t timer16_1_counter[2];

/* Main Program */

int main (void) {
  uint32_t interval;

  SystemCoreClockUpdate();

  /* Config CLKOUT, mostly used for debugging. */
  CLKOUT_Setup( CLKOUTCLK_SRC_MAIN_CLK );
  LPC_IOCON->PIO0_1 &= ~0x07;	
  LPC_IOCON->PIO0_1 |= 0x01;		/* CLK OUT */

  /* Enable AHB clock to the GPIO domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

  /* TEST_TIMER_NUM is either 0 or 1 for 16-bit timer 0 or 1. */
  interval = SystemCoreClock/1000 - 1;
  if ( interval > 0xFFFF )
  {
	interval = 0xFFFF;
  }
  init_timer16(TEST_TIMER_NUM, interval);
  enable_timer16(TEST_TIMER_NUM);

  /* Set port 1_19 to output */
  GPIOSetDir( 1, 19, 1 );

  while (1)                                /* Loop forever */
  {
#if TEST_TIMER_NUM
	/* I/O configuration and LED setting pending. */
	if ( (timer16_1_counter[0] > 0) && (timer16_1_counter[0] <= 200) )
	{
	  GPIOSetBitValue( 1, 19, 0 );
	}
	if ( (timer16_1_counter[0] > 200) && (timer16_1_counter[0] <= 400) )
	{
	  GPIOSetBitValue( 1, 19, 1 );
	}
	else if ( timer16_1_counter[0] > 400 )
	{
	  timer16_1_counter[0] = 0;
	}
#else
	/* I/O configuration and LED setting pending. */
	if ( (timer16_0_counter[0] > 0) && (timer16_0_counter[0] <= 200) )
	{
	  GPIOSetBitValue( 1, 19, 0 );
	}
	if ( (timer16_0_counter[0] > 200) && (timer16_0_counter[0] <= 400) )
	{
	  GPIOSetBitValue( 1, 19, 1 );
	}
	else if ( timer16_0_counter[0] > 400 )
	{
	  timer16_0_counter[0] = 0;
	}
#endif
  }
}
