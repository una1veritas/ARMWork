/***********************************************************************
 * $Id::                                                               $
 *
 * Project:	rgb : HSL<=>RGB conversion using LPCXpresso Baseboard
 * File:	rgb.c
 * Description:
 * 			Routines to convert from HSL to RGB color space, and
 * 			to control the three color LED via LPC17xx's PWM1.
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

/**********************************************************************
 * NOTE! - 	Please be aware that this application requires a wire
 * 			jumper connecting PIO1_2 to PIO1_11 be installed on the
 * 			base board.
 **********************************************************************/

#include "LPC17xx.h"
#include "small_utils.h"
#include "rgb.h"

int newred=0;
int newgreen=0;
int newblue=0;

int oldred = 0;
int oldgreen = 0;
int oldblue = 0;

int stepred, stepgreen, stepblue;

void RGBdoubleToInt(double f_red, double f_green, double f_blue, int *i_red, int *i_green, int *i_blue)
{
    *i_red   = BOUND(0, ((int)(f_red  *((double)(1<<PWM_BITS)))) - 1, PWM_SATURATION);
    *i_green = BOUND(0, ((int)(f_green*((double)(1<<PWM_BITS)))) - 1, PWM_SATURATION);
    *i_blue  = BOUND(0, ((int)(f_blue *((double)(1<<PWM_BITS)))) - 1, PWM_SATURATION);
}

void SetRGB(double red, double green, double blue)
{
    RGBdoubleToInt(red, green, blue, &oldred, &oldgreen, &oldblue);
    LPC_PWM1->MR1 = oldred;
    LPC_PWM1->MR2 = oldgreen;
    LPC_PWM1->MR3 = oldblue;
    LPC_PWM1->LER = LER1_EN | LER2_EN | LER3_EN ; //Load Shadow register contents
}

void StartFade(double red, double green, double blue, int steps)
{
    RGBdoubleToInt(red, green, blue, &newred, &newgreen, &newblue);
    stepred   = (((newred   - oldred)*256)   / steps) / 256;
    stepgreen = (((newgreen - oldgreen)*256) / steps) / 256;
    stepblue  = (((newblue  - oldblue)*256)  / steps) / 256;
}

int FadeStep()
{
    int n = 1;

    if((stepred > 0 && oldred+stepred < newred) || (stepred < 0 && oldred+stepred > newred))
    {
    	oldred += stepred;
    	LPC_PWM1->MR1 = oldred;
    	n=0;
    }
    else
    {
    	LPC_PWM1->MR1 = newred;
    }

    if((stepgreen > 0 && oldgreen+stepgreen < newgreen) || (stepgreen < 0 && oldgreen+stepgreen > newgreen))
    {
    	oldgreen += stepgreen;
    	LPC_PWM1->MR2 = oldgreen;
    	n=0;
    }
    else
    {
    	LPC_PWM1->MR2 = newgreen;
    }

    if((stepblue > 0 && oldblue+stepblue < newblue) || (stepblue < 0 && oldblue+stepblue > newblue))
    {
    	oldblue += stepblue;
    	LPC_PWM1->MR3 = oldblue;
    	n=0;
    }
    else
    {
    	LPC_PWM1->MR3 = newblue;
    }

    LPC_PWM1->LER = LER0_EN | LER1_EN | LER2_EN | LER3_EN ; //Load Shadow register contents
    return n;
}

// Wikipedia: "HSL and HSV"
void HSLtoRGB(double hue, double saturation, double lightness, double *red, double *green, double *blue)
{
    double chroma, hueprime, x, m;

    if(lightness <= 0.5)
        chroma = 2*lightness*saturation;
    else
        chroma = (2.0-2*lightness)*saturation;

    hueprime = hue/60;
    x = chroma*(1.0-ABS(small_fmodf(hueprime,2)-1));
    if(hue == -1)
        *red = *green = *blue = 0;
    else if(hueprime < 1)
        *red = chroma, *green = x,      *blue = 0;
    else if(hueprime < 2)
        *red = x,      *green = chroma, *blue = 0;
    else if(hueprime < 3)
        *red = 0,      *green = chroma, *blue = x;
    else if(hueprime < 4)
        *red = 0,      *green = x,      *blue = chroma;
    else if(hueprime < 5)
        *red = x,      *green = 0,      *blue = chroma;
    else
        *red = chroma, *green = 0,      *blue = x;

    m = lightness - 0.5*chroma;
    *red += m; *green += m; *blue += m;
}

void InitLEDs(void)
{
	/**********************************************************************
	 * NOTE! - 	Please be aware that this application requires a wire
	 * 			jumper connecting PIO1_2 to PIO1_11 be installed on the
	 * 			base board.
	 **********************************************************************/

    LPC_SC->PCONP |= 1<<6; //enable PWM1 Power
    LPC_SC->PCLKSEL0 |= 0x1<<12; //PWM peripheral clk = PCLK

    //Put P0.26 in Hi-Z so it can be shorted to PWM1.2 (P2.2)
    LPC_PINCON->PINMODE1 &= ~(0x3<<20);
    LPC_PINCON->PINMODE1 |= 0x2<<20;
    LPC_GPIO0->FIODIR &= ~(1<<26);

    LPC_PINCON->PINSEL4 = (0x1<<0) | (0x1<<2) | (0x1<<4);	/* set GPIOs for PWM pins on PWM1.1,PWM1.2,PWM1.3 */
    LPC_PWM1->PR = 0x00;									/* count frequency:Fpclk */

    LPC_PWM1->MCR = 1 << 1;									/*	reset on MR0 */
    LPC_PWM1->MR0 = (1<<PWM_BITS) - 2;						/* set PWM cycle */

    //LEDs default to OFF
    LPC_PWM1->MR1 = 0;
    LPC_PWM1->MR2 = 0;
    LPC_PWM1->MR3 = 0;

    LPC_PWM1->LER = LER0_EN | LER1_EN | LER2_EN | LER3_EN ; //Load Shadow register contents
    LPC_PWM1->PCR = PWMENA1 | PWMENA2 | PWMENA3 ; 			//Enable PWM outputs
    LPC_PWM1->TCR = TCR_CNT_EN | TCR_PWM_EN;				//Enable PWM Timer
}

