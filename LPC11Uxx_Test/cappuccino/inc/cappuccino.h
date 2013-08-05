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


#define	HIGH(portNum, bit) { \
	LPC_GPIO->SET[portNum] = (1<<bit); \
}

#define	LOW(portNum, bit) { \
	LPC_GPIO->CLR[portNum] = (1<<bit); \
}

#define	INPUT(portNum, bit) { \
	LPC_GPIO->DIR[portNum] &= ~(1<<bit); \
}

#define	OUTPUT(portNum, bit) { \
	LPC_GPIO->DIR[portNum] |= (1<<bit); \
}

#define IOSET(portNum, bit, val) { \
	if(val){ \
		LPC_GPIO->SET[portNum] = (1<<bit); \
	} else { \
		LPC_GPIO->CLR[portNum] = (1<<bit); \
	} \
}

#endif /* end __CAPPUCCINO_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
