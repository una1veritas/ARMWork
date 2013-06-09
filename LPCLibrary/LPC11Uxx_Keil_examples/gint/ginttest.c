/****************************************************************************
 *   $Id:: ginttest.c 4217 2010-08-05 00:19:31Z usb00423                    $
 *   Project: NXP LPC11Uxx GINT example
 *
 *   Description:
 *     This file contains GINT test modules, main entry, to test I2C APIs.
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
#include "type.h"
#include "gpio.h"

/*******************************************************************************
**   Main Function  main()
*******************************************************************************/
int main (void)
{
  uint32_t bitPattern[2];
  uint32_t eventPattern[2];
  SystemCoreClockUpdate();
  GPIOInit();

#if 1
  bitPattern[0] = 0x1<<5;		/* PIO0_5 enabled */
  bitPattern[1] = 0x1<<5;		/* PIO1_5 enabled */
  eventPattern[0] = 0x1<<5;		/* PIO0_5 rising edge */
  eventPattern[1] = 0x0<<5;		/* PIO1_5 falling edge */
  /* GINT0_INT sources OR together */
  GPIOSetGroupedInterrupt( GROUP0, &bitPattern[0], 0, 0, &eventPattern[0] );
#endif

#if 0
  bitPattern[0] = 0x1<<5;		/* PIO0_5 enabled */
  bitPattern[1] = 0x1<<5;		/* PIO1_5 enabled */
  eventPattern[0] = 0x1<<5;		/* PIO0_5 rising edge */
  eventPattern[1] = 0x0<<5;		/* PIO1_5 falling edge */
  /* GINT1_INT sources AND together */
  GPIOSetGroupedInterrupt( GROUP1, &bitPattern[0], 1, 0, &eventPattern[0] );
#endif

  while ( 1 );
}

/******************************************************************************
**                            End Of File
******************************************************************************/
