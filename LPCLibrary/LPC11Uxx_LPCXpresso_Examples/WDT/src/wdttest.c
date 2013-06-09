/****************************************************************************
 *   $Id:: wdttest.c 4308 2010-08-18 00:47:51Z usb01267                     $
 *   Project: NXP LPC11Uxx WDT example
 *
 *   Description:
 *     This file contains WDT test modules, main entry, to test WDT APIs.
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

#include "LPC11Uxx.h"
#include "timer32.h"
#include "clkconfig.h"
#include "wdt.h"
#include "gpio.h"

extern volatile uint32_t timer32_0_counter;

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  SystemCoreClockUpdate();
  
  /* Config WDT clock */
#if 1
  WDT_CLK_Setup(WDTCLK_SRC_WDT_OSC);
#else
  WDT_CLK_Setup(WDTCLK_SRC_IRC_OSC);
#endif
   
  GPIOInit();

  /*** The main Function is an endless loop ****/
  init_timer32( 0, TIME_INTERVAL );
    
  WDTInit();
  enable_timer32( 0 );

  /* Set PIO0_8 to output */
  GPIOSetDir( 0, 8, 1 );

  /* Set PIO0_7 to output */
  GPIOSetDir( 0, 7, 1 );

  if ((LPC_SYSCON->SYSRSTSTAT & 0x4) == 0x4) /*WDT reset detected */
  {
    LPC_SYSCON->SYSRSTSTAT |= 0x4; /*Write a 1 to clear the WDT Reset */ 
    while( 1 ) 
    {
	  /* I/O configuration and LED setting pending. */
	  if ( (timer32_0_counter > 0) && (timer32_0_counter <= 200) )
	  {
	    GPIOSetBitValue( 0, 8, 0 );
	  }
	  if ( (timer32_0_counter > 200) && (timer32_0_counter <= 400) )
	  {
	    GPIOSetBitValue( 0, 8, 1 );
	  }
	  else if ( timer32_0_counter > 400 )
	  {
	    timer32_0_counter = 0;
	  }
    }
  }
  else  /* Else no WDT reset detected */
  {
    while( 1 ) 
    {
	  /* I/O configuration and LED setting pending. */
	  if ( (timer32_0_counter > 0) && (timer32_0_counter <= 200) )
	  {
	    GPIOSetBitValue( 0, 7, 0 );
	  }
	  if ( (timer32_0_counter > 200) && (timer32_0_counter <= 400) )
	  {
	    GPIOSetBitValue( 0, 7, 1 );
	  }
	  else if ( timer32_0_counter > 400 )
	  {
	    timer32_0_counter = 0;
	  }
    }
  }
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
