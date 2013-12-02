/****************************************************************************
 *   Project: Strawberry Linux LPCcappuccino(TM)
 *
 *   Description:
 *     This file contains definition and prototype for cappuccino.
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
#ifndef __LPCLCD_H
#define __LPCLCD_H

// Strawberry Linux original lpclcd port maps

#ifndef LPCLCD
#define LPCLCD
#endif

#define LCD_RST     PIO1_25
#define LED_USER    PIO1_6
#define LED_LCDBKLT PIO1_3
#define SW_USERBTN  PIO0_1

#define I2C_PULLUP      PIO0_7

#define RXD2        PIO0_18
#define TXD2        PIO0_19

#define RXD RXD2
#define TXD TXD2


#endif /* end __LPCLCD_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
