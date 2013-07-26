#include "type.h"
#include "LPC17xx.H"                        /* LPC17xx definitions */
#include "ps2kbd.h"
#include "uart.h"

int main (void) 
{
	int8_t key = 0;

	SystemInit();                         /* initialize clocks */

	init_keyboard();
  	UARTInit(3, 38400);	/* baud rate setting */
  	UARTInit(1, 38400);	/* baud rate setting */

 	while (1) 
  	{
		if(keyhit())
		{
			key = xgetkey();
		    UARTSend( 3, (uint8_t *)&key, sizeof(key));
		    UARTSend( 0, (uint8_t *)&key, sizeof(key));
		}
	}
}

