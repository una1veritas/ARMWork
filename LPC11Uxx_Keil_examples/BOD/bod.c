/****************************************************************************
 *   $Id:: bod.c 4238 2010-08-06 22:54:40Z usb00423                         $
 *   Project: NXP LPC11Uxx BOD(Brown-Out Detect) example
 *
 *   Description:
 *     This file contains BOD code example which include BOD 
 *     initialization, BOD interrupt handler, and APIs.
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
#include "bod.h"

volatile uint32_t bod_counter;

/*****************************************************************************
** Function name:		BOD_IRQHandler
**
** Descriptions:		BOD interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void BOD_IRQHandler(void)
{
  if ( LPC_SYSCON->SYSRSTSTAT & BOD_RESET )
  {
	/* If the BOD RESET has occured, disable the BOD interrupt,
	or we will stuck here serving BOD interrupt instead of
	LED blinking showing BOD RESET. */ 
	NVIC_DisableIRQ(BOD_IRQn);
	return;
  }

  bod_counter++;

  /* Turn on BOD RST LED only, turn off the rest. */
  GPIOSetBitValue( 1, 14, 1 );
  GPIOSetBitValue( 1, 5, 0 );
  GPIOSetBitValue( 1, 13, 0 );
  return;
}

/*****************************************************************************
** Function name:		BOD_Init
**
** Descriptions:		Initialize BOD control register
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void BOD_Init( void )
{
  /* Set BOD control register. interrupt level assertion is 1.65V, 
  de-assertion is 1.80V. */
  LPC_SYSCON->BODCTRL = BOD_INT_LVL3|BOD_RST_LVL2|BOD_RST_ENABLE;
  /* Enable the BOD Interrupt */
  NVIC_EnableIRQ(BOD_IRQn);
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
