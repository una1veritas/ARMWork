/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

#include "ch.h"
#include "hal.h"

/*
 * Early initialization code.
 * This initialization must be performed just after stack setup and before
 * any other initialization.
 */
void __early_init(void) {

  spc563_clock_init();
}

/*
 * Board-specific initialization code.
 */
void boardInit(void) {

  /*
   * Various initialization (temporary code).
   */
  SIU.PCR[GPIO_LED1].R     = 0x0300;                /* OBE | IBE.           */
  SIU.PCR[GPIO_LED2].R     = 0x0300;                /* OBE | IBE.           */
  SIU.PCR[GPIO_LED3].R     = 0x0300;                /* OBE | IBE.           */
  SIU.PCR[GPIO_LED4].R     = 0x0300;                /* OBE | IBE.           */
  SIU.PCR[GPIO_BUTTON1].R  = 0x0100;                /* IBE.                 */
  SIU.PCR[GPIO_BUTTON2].R  = 0x0100;                /* IBE.                 */
  SIU.PCR[GPIO_BUTTON3].R  = 0x0100;                /* IBE.                 */
  SIU.PCR[GPIO_BUTTON4].R  = 0x0100;                /* IBE.                 */
  SIU.PCR[GPIO_SCI_A_TX].R = 0x0500;                /* Primary | IBE.       */
  SIU.PCR[GPIO_SCI_A_RX].R = 0x0500;                /* Primary | IBE.       */
}
