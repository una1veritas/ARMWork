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
#ifndef __CAPPUCCINO_H
#define __CAPPUCCINO_H

#define SSP_CS0       PIO0_2
#define SSP_CS1       PIO1_23

// Strawberry Linux original lpclcd port maps

#define CAPPUCCINO

#define LED_LCDBKLT PIO0_3
#define LED_SDBUSY   PIO1_19

#define LCDRST  PIO1_25
#define USERBTN PIO0_1
#define RXD_C    PIO0_18
#define TXD_C    PIO0_19
#define RXD_A    PIO1_26
#define TXD_A    PIO1_27
#define RXD_E    PIO1_17
#define TXD_E    PIO1_18

#endif /* end __CAPPUCCINO_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
