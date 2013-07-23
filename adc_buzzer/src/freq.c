/***********************************************************************
 * $Id::                                                               $
 *
 * Project:	buzzer_adc : PWM audio with freqency controlled via ADC
 * File:	freq.c
 * Description:
 * 			Read ADC and adjust PWM frequency accordingly. Uses
 * 			SystemTick driven timer to modulate PWM duty cycle.
 * 			When used with LPCXpresso Baseboard PWM can drive
 * 			the speaker when connected with a jumper wire.
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

#include "LPC17xx.h"
#include "small_utils.h"
#include "freq.h"

/*
 * updateFreq:	Update the PWM output's frequency and duty cycle
 *
 * Arguments:	hertz - the requested PWM output frequency in Hz units
 * 				duty  - the requested PWM output duty cycle in % units
 *
 */
void updateFreq(unsigned int hertz,unsigned int duty)
{
    unsigned int freqValue = BOUND(MIN_PERIOD,1000000/hertz,MAX_PERIOD);
    unsigned int dutyCycle = BOUND(10,duty,90);
	LPC_PWM1->MR0 = freqValue;
    LPC_PWM1->MR3 = (freqValue * dutyCycle) / 100;
    LPC_PWM1->LER = LER0_EN | LER3_EN ; //Load Shadow register contents
}

/*
 * initFreq:	Initialize PWM1 to generate output
 *
 */
void initFreq(void)
{
    LPC_SC->PCONP |= 1<<6; //enable PWM1 Power
    LPC_SC->PCLKSEL0 |= 0x1<<12; //PWM peripheral clk = PCLK

    //Put P0.26 in Hi-Z so it can be shorted to PWM1.2 (P2.2)
    //On Baseboard these are labeled PIO1_2, PIO1_11
    LPC_PINCON->PINMODE1 &= ~(0x3<<20);
    LPC_PINCON->PINMODE1 |= 0x2<<20;
    LPC_GPIO0->FIODIR &= ~(1<<26);

    LPC_PINCON->PINSEL4 = (0x1<<4);	/* set GPIOs for PWM pins on PWM1.1,PWM1.2,PWM1.3 */

    LPC_PWM1->PR = 100;			/*	count frequency:Fpclk/100 */
    LPC_PWM1->MCR = 1 << 1;		/*	reset on MR0 */
    LPC_PWM1->MR0 = 46;			/*	set PWM cycle */
    LPC_PWM1->MR3 = 23;

    LPC_PWM1->LER = LER0_EN | LER3_EN ; 					//Load Shadow register contents
    LPC_PWM1->PCR = PWMENA3 ; 								//Enable PWM outputs
    LPC_PWM1->TCR = TCR_CNT_EN | TCR_PWM_EN;				//Enable PWM Timer
}

