/****************************************************************************
 *   $Id:: systicktest.c 4079 2010-07-30 17:55:20Z usb00423                 $
 *   Project: NXP LPC11Uxx Systick example
 *
 *   Description:
 *     This file contains System tick test modules, main entry, to test 
 *     systick APIs.
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
#include "gpio.h"

#include "armcmx.h"
#include "delay.h"

#define SYSTICK_DELAY		(SystemCoreClock/100)

volatile uint32_t TimeTick = 0;

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler(void)
{
  TimeTick++;
}

void delaySysTick(uint32_t tick)
{
  uint32_t timetick;

  /* Clear SysTick Counter */
  SysTick->VAL = 0;
  /* Enable the SysTick Counter */
  SysTick->CTRL |= (0x1<<0);

  timetick = TimeTick;
  while ((TimeTick - timetick) < tick);
  
  /* Disable SysTick Counter */
  SysTick->CTRL &= ~(0x1<<0);
  /* Clear SysTick Counter */
  SysTick->VAL = 0;
  return;
}

/* Main Program */

int main (void) 
{
  SystemCoreClockUpdate();

  /* Called for system library in core_cmx.h(x=0 or 3). */
  SysTick_Config( SYSTICK_DELAY );
  
  /* Enable AHB clock to the GPIO domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

  start_delay();
  
  /* Set port 1_20 to output */
  GPIOSetDir( 1, 6, 1 );

  while (1)                                /* Loop forever */
  {
	delaySysTick(20);
	GPIOSetBitValue( 1, 6, 0 );
	delaySysTick(20);
	GPIOSetBitValue( 1, 6, 1 );
	delay(100);
	GPIOSetBitValue( 1, 6, 0 );
	delay(100);
	GPIOSetBitValue( 1, 6, 1 );
  }
}
