#include "lpc17xx.h"
#include "extint.h"

int main (void) 
{
	SystemInit();                         /* initialize clocks */
	EINTInit();

	while(1)
	{
	}
}
