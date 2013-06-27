/****************************************************************************
 *   $Id:: ssp.h 4102 2010-08-02 20:53:05Z usb00423                         $
 *   Project: NXP LPC11xx SSP example
 *
 *   Description:
 *     This file contains SSP code header definition.
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
#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include "ssp.h"

typedef struct {
  uint8_t portno;
  /* statistics of all the interrupts */
  volatile uint32_t interruptRxStat; //0 = 0;
  volatile uint32_t interruptOverRunStat; //0 = 0;
  volatile uint32_t interruptRxTimeoutStat; //0 = 0;
  
  /* There are there modes in SSP: loopback, master or slave. */
/* Here are the combination of all the tests. 
(1) LOOPBACK test:		LOOPBACK_MODE=1, TX_RX_ONLY=0, USE_CS=1;
(2) Serial EEPROM test:	LOOPBACK_MODE=0, TX_RX_ONLY=0, USE_CS=0; (default)
(3) TX(Master) Only:	LOOPBACK_MODE=0, SSP_SLAVE=0, TX_RX_ONLY=1, USE_CS=1;
(4) RX(Slave) Only:		LOOPBACK_MODE=0, SSP_SLAVE=1, TX_RX_ONLY=1, USE_CS=1 */

  uint8_t LOOPBACK_MODE; //   0
  /* 1 is loopback, 0 is normal operation. */
  uint8_t SSP_SLAVE; //       0
  /* 1 is SLAVE mode, 0 is master mode */
  uint8_t TX_RX_ONLY; //      0		
  /* 1 is TX or RX only depending on SSP_SLAVE
								flag, 0 is either loopback mode or communicate
								with a serial EEPROM. */

/* if USE_CS is zero, set SSEL as GPIO that you have total control of the sequence */
/* When test serial SEEPROM(LOOPBACK_MODE=0, TX_RX_ONLY=0), set USE_CS to 0. */
/* When LOOPBACK_MODE=1 or TX_RX_ONLY=1, set USE_CS to 1. */

  bool USE_CS; //          0

} SPIDef;

uint8_t SPI_transfer(SPIDef * port, uint8_t data);

#endif  /* __SPI_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/

