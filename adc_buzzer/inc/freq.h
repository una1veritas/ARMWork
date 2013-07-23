/*
===============================================================================
 File rgb.c: RGB LED functions for LPC111x and LPC134x
 Project DebugTips_1343: Demonstration of debugging techniques for LPCXpresso
 Copyright (C) 2010 NXP Semiconductors
===============================================================================
*/
#ifndef BUZZER_H_INCLUDED
#define BUZZER_H_INCLUDED

#define MAX_FREQ 5000
#define MIN_FREQ 25

#define MAX_PERIOD	(1000000/MIN_FREQ)
#define MIN_PERIOD	(1000000/MAX_FREQ)

#define ABS(a) ((a)>=0 ? (a) : -(a))
#define BOUND(l, n, h) ((n)<(l) ? (l) : ((n)>(h) ? (h) : (n)))

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

void initFreq(void);
void updateFreq(unsigned int hertz,unsigned int duty);

// #ifndef RGB_H_INCLUDED
#endif
