/***********************************************************************
 * $Id::                                                               $
 *
 * File:	small_systick.h: Small systick for LPC111x/LPC134x/LPC17xx
 * Description:
 * 			Generates 10ms systick events
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

#ifndef SMALLSYSTICK_H_INCLUDED
#define SMALLSYSTICK_H_INCLUDED

#include "LPC17xx.h"

// InitSysTick() sets systick timer to 10 mS
#define InitSysTick(MHz) SysTick_Config(MHz * (1000000/100))

// #ifndef SMALLSYSTICK_H_INCLUDED
#endif
