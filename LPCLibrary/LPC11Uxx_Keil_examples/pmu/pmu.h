/****************************************************************************
 *   $Id:: pmu.h 4043 2010-07-29 19:02:40Z usb01267                         $
 *   Project: NXP LPC11xx PMU example
 *
 *   Description:
 *     This file contains PMU code header definition.
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
#ifndef __PMU_H 
#define __PMU_H

#define Gint_Enable					0  /* Enable this to configure Group interrupts as wake-up source. Default configured to PIO0_17 */
/*------Power Modes -------*/
/* To enable set Flag to 1, to disable set Flag to 0*/
/* Only One of the four modes can be enabled */
#define MCU_SLEEP           0
#define MCU_DEEP_SLEEP      1
#define MCU_POWER_DOWN      0
#define TEST_DEEPPOWERDOWN  0
/*------Power Modes -------*/

/* System Low Power ----------------------------------------------------------*/
#define NVIC_LP_SEVONPEND   (0x10)
#define NVIC_LP_SLEEPDEEP   (0x04)
#define NVIC_LP_SLEEPONEXIT (0x02)

#define IRC_OUT_PD          (0x1<<0)
#define IRC_PD              (0x1<<1)
#define FLASH_PD            (0x1<<2)
#define BOD_PD              (0x1<<3)
#define ADC_PD              (0x1<<4)
#define SYS_OSC_PD          (0x1<<5)
#define WDT_OSC_PD          (0x1<<6)
#define SYS_PLL_PD          (0x1<<7)

void PMU_Init( void );
void PMU_Sleep( uint32_t SleepMode, uint32_t SleepCtrl );
void PMU_DeepPowerDown( void );

#endif /* end __PMU_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
