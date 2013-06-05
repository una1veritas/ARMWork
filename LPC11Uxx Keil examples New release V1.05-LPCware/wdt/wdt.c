/****************************************************************************
 *   $Id:: wdt.c 5316 2010-10-20 22:29:46Z usb01267                         $
 *   Project: NXP LPC11Uxx WDT(Watchdog timer) example
 *
 *   Description:
 *     This file contains WDT code example which include WDT 
 *     initialization, WDT interrupt handler, and APIs for WDT
 *     reading.
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
#include "wdt.h"

volatile uint32_t wdt_counter;

/*****************************************************************************
** Function name:		WDTHandler
**
** Descriptions:		Watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDT_IRQHandler(void)
{
  uint32_t i;

#if PROTECT_MODE
  /* For WDPROTECT test */
  /* Make sure counter is below WDWARNINT */
  wdt_counter = LPC_WWDT->TV;
  while (wdt_counter >= 0x3FF)
  {
    wdt_counter = LPC_WWDT->TV;
  }

  LPC_WWDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WWDT->FEED = 0x55;    
  /* Make sure feed sequence executed properly */
  for (i = 0; i < 0x80; i++);

  /* Make sure counter is reloaded with WDTC */
  wdt_counter = LPC_WWDT->TV;
  while (wdt_counter < 0x400)
  {
    wdt_counter = LPC_WWDT->TV;
  }
#endif

  LPC_WWDT->MOD &= ~WDTOF;		/* clear the time-out flag and interrupt flag */
  LPC_WWDT->MOD &= ~WDINT;		/* clear the time-out flag and interrupt flag */
}

/*****************************************************************************
** Function name:		WDTInit
**
** Descriptions:		Initialize watchdog timer, install the
**						watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTInit( void )
{
  uint32_t i, regVal;

#if	WDLOCK_MODE
  LPC_WWDT->CLKSEL |= 0x80000000;
#endif

  wdt_counter = 0;

  /* Enable the WDT Interrupt */
  NVIC_EnableIRQ(WDT_IRQn);

  LPC_WWDT->TC = WDT_FEED_VALUE;	/* once WDEN is set, the WDT will start after feeding */
#if PROTECT_MODE
  LPC_WWDT->TC = 0x3FFF;
#endif
#if WINDOW_MODE
  LPC_WWDT->TC = 0x3FFF;
#endif

#if WATCHDOG_RESET
  regVal = WDEN | WDRESET;
  LPC_WWDT->MOD = regVal;
#else
  regVal = WDEN;
  LPC_WWDT->MOD = regVal;
#endif

#if LOCKCLK_MODE
  regVal |= WDLOCKCLK;
  LPC_WWDT->MOD = regVal;
#endif

  LPC_WWDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WWDT->FEED = 0x55;    
  /* Make sure feed sequence executed properly */
  for (i = 0; i < 0x80; i++);

  /* For WDWARNINT test */
  LPC_WWDT->WARNINT = 0x3FF;

#if PROTECT_MODE
  /* For WDPROTECT test */
  regVal |= WDPROTECT;
  LPC_WWDT->MOD = regVal;

  LPC_WWDT->WINDOW = 0x2FFF;
#endif

#if WINDOW_MODE
  /* For WDWINDOW test */
  LPC_WWDT->WINDOW = 0x2FFF;
#endif

#if WINDOW_MODE
  /* For WDWINDOW test */
  while (1)
  {
    wdt_counter = LPC_WWDT->TV;
    while (wdt_counter >= 0x000001FFF)
    {
      wdt_counter = LPC_WWDT->TV;
    }

    LPC_WWDT->FEED = 0xAA;		/* Feeding sequence */
    LPC_WWDT->FEED = 0x55;    
    /* Make sure feed sequence executed properly */
    for (i = 0; i < 0x80; i++);
  }
#endif
  return;
}

/*****************************************************************************
** Function name:		WDTFeed
**
** Descriptions:		Feed watchdog timer to prevent it from timeout
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTFeed( void )
{
  LPC_WWDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WWDT->FEED = 0x55;
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
