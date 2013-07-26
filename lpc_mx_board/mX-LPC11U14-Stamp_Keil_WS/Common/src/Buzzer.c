#include "lpc11xx_syscon.h"
#include "lpc11Uxx.h"

void Buzzer()
{
	int i, j;

	/* Enable GPIO block clock */
	SYSCON_AHBPeriphClockCmd(SYSCON_AHBPeriph_GPIO, ENABLE);

	/* If PIO2_0 had been set to other function, set it to PIO function. 
	 * NOTE: Component IOCON should be checked */
	//IOCON_SetPinFunc(IOCON_PIO2_0, PIO2_0_FUN_PIO);

	LPC_GPIO->DIR[0] |= (1<<23);

	while(1) {

		/* Delay some time */
		for(i=0; i<20; i++) {
			for(j=0; j<10000; j++) {
			}
		}

		LPC_GPIO->NOT[0] = (1<<23);

	}
}
