/*****************************************************************************
 *   dac.h:  Header file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.25  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __DAC_H 
#define __DAC_H

#define DAC_BIAS	0x00010000

extern void DACInit( void );
void DAC_test(void);

#endif /* end __DAC_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
