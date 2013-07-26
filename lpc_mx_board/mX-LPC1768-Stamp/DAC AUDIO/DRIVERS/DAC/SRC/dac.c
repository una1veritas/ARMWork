/*****************************************************************************
 *   dac.c:  DAC module file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.25  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "lpc17xx.h"
#include "type.h"
#include "dac.h"

/*****************************************************************************
** Function name:		DACInit
**
** Descriptions:		initialize DAC channel
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void DACInit( void )
{
  /* setup the related pin to DAC output */
  LPC_PINCON->PINSEL1 &= ~0x00300000;
  LPC_PINCON->PINSEL1 |= 0x00200000;	/* set p0.26 to DAC output */   
  return;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
