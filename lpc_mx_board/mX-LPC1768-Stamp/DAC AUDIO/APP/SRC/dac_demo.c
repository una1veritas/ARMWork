#include "LPC17xx.H"                        /* LPC17xx definitions */
#include "type.h"
#include "dac.h"

void DAC_test(void)
{
	uint32_t i=0,j=0;
	uint32_t k=0;

  for(k=0;k<10000;k++)
  {
	LPC_DAC->DACR = (i << 6) | DAC_BIAS;
	i++;
	for(j = 1000; j > 1; j--);
	if ( i == 1024 )
	{
	  i = 0;
	}
  }
}

int main(void) 
{
	SystemInit();                         /* initialize clocks */

	DACInit();
	DAC_test();

    while(1);
}
