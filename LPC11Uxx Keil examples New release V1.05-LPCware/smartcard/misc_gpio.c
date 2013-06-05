/****************************************************************************
 *   $Id:: misc_gpio.c 5143 2010-10-07 23:06:18Z usb00423                       $
 *   Project: NXP LPC software example
 *
 *   Description:
 *     This file contains routines to control and monitor
 *     Smart Card interface signals.
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
#include "type.h"
#include "misc_gpio.h"

void MiscGpioInit(void)
{
  /* Smart Card GPIO interface */
  /*
   P1.9; L=off, H=on; Pin used for the activity LED, it will toggle whenever bytes
   are recevied or transmitted and remain off when there is no activity
   P1.10; L=extracted, H=inserted; Pin is used to detect whether a Smart Card
   is inserted into its socket
   P1.11; L=off, H=on; Pin is used to enable power to the Smart Card
   P1.12; L=assert reset, H=out of reset; Pin is used to assert the reset condition
   to the Smart Card
  */
  LPC_IOCON->CARD_ACTIVITY_IOCON &= ~0x7;
  LPC_IOCON->CARD_ACTIVITY_IOCON |= 0x0;
  LPC_IOCON->CARD_RESET_IOCON &= ~0x7;
  LPC_IOCON->CARD_RESET_IOCON |= 0x0 | _BV(10);
  LPC_IOCON->POWER_ENABLE_IOCON &= ~0x7;
  LPC_IOCON->POWER_ENABLE_IOCON |= 0;
  LPC_IOCON->CARD_DETECT_IOCON &= ~0x7;
  LPC_IOCON->CARD_DETECT_IOCON |= 0; /* DETECT @ P1.10 */
  /* RST active, power off, activity inactive */
  /* Only RST, POWER_EN, activity LED are outputs */ 
  LPC_GPIO->CLR[1] = CARD_RESET | POWER_ENABLE | CARD_ACTIVITY;
  LPC_GPIO->DIR[1] |= CARD_RESET | POWER_ENABLE | CARD_ACTIVITY; 
  return;
}


/*****************************************************************************
**                            End Of File
******************************************************************************/
