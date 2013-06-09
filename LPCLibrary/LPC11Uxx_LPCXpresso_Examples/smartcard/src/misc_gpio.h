/****************************************************************************
 *   $Id:: scuart.h 5143 2010-10-07 23:06:18Z usb00423                       $
 *   Project: NXP LPC software example
 *
 *   Description:
 *     This file contains definition and prototype for a software
 *     implemented Smart Card UART.
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
#ifndef __MISC_GPIO_H__ 
#define __MISC_GPIO_H__

#include "config.h"

#ifndef _BV
#define _BV(_x_) (1UL << (_x_))
#endif

/***************************************************************************/
/* Prototypes and global values */

void MiscGpioInit(void);

static __INLINE void CardOff(void)
{
  LPC_GPIO->CLR[1] = CARD_RESET | POWER_ENABLE | CARD_ACTIVITY;
}

static __INLINE void CardPowerOn(void)
{
  LPC_GPIO->SET[1] = POWER_ENABLE;
}

static __INLINE void CardAssertReset(void)
{
  LPC_GPIO->CLR[1] = CARD_RESET;
}

static __INLINE void CardDeassertReset(void)
{
  LPC_GPIO->SET[1] = CARD_RESET;
}

static __INLINE int isCardInserted(void)
{
  return ((LPC_GPIO->PIN[1] & CARD_DETECT) != 0);
}

static __INLINE void ShowActivity(void)
{
  if (LPC_GPIO->SET[1] & CARD_ACTIVITY)
    LPC_GPIO->CLR[1] = CARD_ACTIVITY;
  else
    LPC_GPIO->SET[1] = CARD_ACTIVITY;
}

static __INLINE void ShowInactivity(void)
{
  LPC_GPIO->CLR[1] = CARD_ACTIVITY;
}

static __INLINE void CardClockOn(void)
{
  LPC_IOCON->PIO0_17 |= 0x03;     /* UART SCLK */
}

static __INLINE void CardClockOff(void)
{
  LPC_IOCON->PIO0_17 &= ~0x07;	
}

#endif /* end __MISC_GPIO_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/
