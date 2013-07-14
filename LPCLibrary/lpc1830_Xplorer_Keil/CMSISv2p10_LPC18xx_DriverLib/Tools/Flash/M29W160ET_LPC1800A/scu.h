/***********************************************************************
 * $Id: scu.h 8313 2011-10-14 22:30:17Z nxp21346 $
 *
 * Project: LPC18xx Common
 *
 * Description:
 *     Header file for PINMUX configuration
 *
 ***********************************************************************
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
 **********************************************************************/
#ifndef __SCU_H 
#define __SCU_H

#define PORT_OFFSET 0x80
#define PIN_OFFSET  0x04

/* Pin modes */
#define MD_PUP  (0x0<<3)
#define MD_BUK  (0x1<<3)
#define MD_PLN  (0x2<<3)
#define MD_PDN  (0x3<<3)
#define MD_EHS  (0x1<<5)
#define MD_EZI  (0x1<<6)
#define MD_ZI   (0x1<<7)
#define MD_EHD0 (0x1<<8)
#define MD_EHD1 (0x1<<8)
#define MD_PLN_FAST (MD_PLN | /*MD_EHS |*/ MD_EZI | MD_ZI)

/* Pin function */
#define FUNC0 0x0
#define FUNC1 0x1
#define FUNC2 0x2
#define FUNC3 0x3
#define FUNC4 0x4
#define FUNC5 0x5
#define FUNC6 0x6
#define FUNC7 0x7

extern void scu_pinmux(unsigned port, unsigned pin, unsigned mode, unsigned func);

#define LPC_SCU_PIN(po, pi)   (*(volatile int         *) (LPC_SCU_BASE + ((po) * 0x80) + ((pi) * 0x4))    )
#define LPC_SCU_CLK(c)        (*(volatile int         *) (LPC_SCU_BASE + 0xC00 + ((c) * 0x4))    )


#endif /* end __SCU_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
