/****************************************************************************
 *   $Id:: gpiotest.c 4216 2010-08-05 00:06:09Z usb00423                    $
 *   Project: NXP LPC11Uxx GPIO example
 *
 *   Description:
 *     This file contains I2C slave test modules, main entry, to test I2C 
 *     slave APIs.
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
#include "LPC11Uxx.h"			/* LPC11xx Peripheral Registers */
#include "gpio.h"

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  SystemCoreClockUpdate();
  GPIOInit();

  /* Route p0.1 to FLEX0, p1.0 to FLEX1, p0.16 to FLEX2,
  and P1.16 to FLEX3. */

  /* use port0_1 as input event, interrupt test. */
  GPIOSetDir( PORT0, 1, 0 );
  /* channel 0, port0_1, edge trigger, active high. */
  GPIOSetFlexInterrupt( 0, 0, 1, 0, 1 );
  GPIOFlexIntEnable( 0, 1 );

  /* use port1_0 as input event, interrupt test. */
  GPIOSetDir( PORT1, 0, 0 );
  /* channel 1, port1_0, edge trigger, active high. */
  GPIOSetFlexInterrupt( 1, 1, 0, 0, 1 );
  GPIOFlexIntEnable( 1, 1 );

  /* use port0_16 as input event, interrupt test. */
  GPIOSetDir( PORT0, 16, 0 );
  /* channel 2, port0_16, edge trigger, active high. */
  GPIOSetFlexInterrupt( 2, 0, 16, 0, 1 );
  GPIOFlexIntEnable( 2, 1 );

  /* use port1_16 as input event, interrupt test. */
  GPIOSetDir( PORT1, 16, 0 );
  /* channel 3, port1_16, edge trigger, active high. */
  GPIOSetFlexInterrupt( 3, 1, 16, 0, 1 );
  GPIOFlexIntEnable( 3, 1 );

  /* Route p0.16 to FLEX4, p1.15 to FLEX5, p0.23 to FLEX6,
  and P1.31 to FLEX7. */
    	 
  /* use port0_16 as input event, interrupt test. */
  GPIOSetDir( PORT0, 16, 0 );
  /* channel 4, port0_16, level trigger, active low. */
  GPIOSetFlexInterrupt( 4, 0, 16, 1, 0 );
  GPIOFlexIntEnable( 4, 0 );

  /* use port1_15 as input event, interrupt test. */
  GPIOSetDir( PORT1, 15, 0 );
  /* channel 5, port1_15, level trigger, active low. */
  GPIOSetFlexInterrupt( 5, 1, 15, 1, 0 );
  GPIOFlexIntEnable( 5, 0 );

  /* use port0_23 as input event, interrupt test. */
  GPIOSetDir( PORT0, 23, 0 );
  /* channel 6, port0_23, level trigger, active low. */
  GPIOSetFlexInterrupt( 6, 0, 23, 1, 0 );
  GPIOFlexIntEnable( 6, 0 );

  /* use port1_31 as input event, interrupt test. */
  GPIOSetDir( PORT1, 31, 0 );
  /* channel 7, port1_31, level trigger, active low. */
  GPIOSetFlexInterrupt( 7, 1, 31, 1, 0 );
  GPIOFlexIntEnable( 7, 1 );
  while( 1 );
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
