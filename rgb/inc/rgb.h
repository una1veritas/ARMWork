/***********************************************************************
 * $Id::                                                               $
 *
 * Project:	rgb : HSL<=>RGB conversion using LPCXpresso Baseboard
 * File:	rgb.h
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

#ifndef RGB_H_INCLUDED
#define RGB_H_INCLUDED

#include "LPC17xx.h"

#define MAX2(a,b) ((a)>(b) ? (a) : (b))
#define MAX3(a,b,c) (MAX2((a),(b)) > (c) ? MAX2((a),(b)) : (c))
#define MAX4(a,b,c,d) (MAX3((a),(b),(c)) > (d) ? MAX3((a),(b),(c)) : (d))

#define MIN2(a,b) ((a)<(b) ? (a) : (b))
#define MIN3(a,b,c) (MIN2((a),(b)) < (c) ? MIN2((a),(b)) : (c))
#define MIN4(a,b,c,d) (MIN3((a),(b),(c)) < (d) ? MIN3((a),(b),(c)) : (d))

#define ABS(a) ((a)>=0 ? (a) : -(a))
#define BOUND(l, n, h) ((n)<(l) ? (l) : ((n)>(h) ? (h) : (n)))

#define PWM_PRESCALER 0
#define PWM_BITS      14
#define PWM_SATURATION ((1<<PWM_BITS)-1)
#define LER0_EN			1 << 0
#define LER1_EN			1 << 1
#define LER2_EN			1 << 2
#define LER3_EN			1 << 3
#define LER4_EN			1 << 4
#define LER5_EN			1 << 5
#define LER6_EN			1 << 6
#define PWMENA1			1 << 9
#define PWMENA2			1 << 10
#define PWMENA3			1 << 11
#define PWMENA4			1 << 12
#define PWMENA5			1 << 13
#define PWMENA6			1 << 14
#define TCR_CNT_EN		0x00000001
#define TCR_RESET		0x00000002
#define TCR_PWM_EN		0x00000008

extern int  newred, newgreen, newblue;

void InitLEDs(void);
void HSLtoRGB(double hue, double saturation, double luma, double *red, double *green, double *blue);
void SetRGB(double red, double green, double blue);
void StartFade(double red, double green, double blue, int steps);
int FadeStep();
void RGBdoubleToInt(double f_red, double f_green, double f_blue, int *i_red, int *i_green, int *i_blue);

// #ifndef RGB_H_INCLUDED
#endif
