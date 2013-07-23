/***********************************************************************
 * $Id::                                                               $
 *
 * Project:	dac	: Simple demo of LPC17xx DAC
 * File:	dactest.c
 * Description:
 * 			Generate saw-tooth pattern using DAC
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
#include "type.h"
#include "dac.h"

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
	uint32_t i = 0;
	volatile uint32_t delay_counter;

	//SystemInit(); //Called by startup code

	/* Initialize DAC  */
	DACInit();

	while ( 1 )
	{
		for(delay_counter = 1000; delay_counter > 1; delay_counter--);
		LPC_DAC->DACR = ((i&0x3FF) << 6) | DAC_BIAS;
		if ( i++ > 1023 )
		{
			i = 0;
		}
	}
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/

