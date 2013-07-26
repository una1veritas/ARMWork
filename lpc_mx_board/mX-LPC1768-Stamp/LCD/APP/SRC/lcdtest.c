#include "LPC17xx.H"                        /* LPC17xx definitions */
#include "lcd.h"

int main (void) 
{
	SystemInit();                         /* initialize clocks */

	init_lcd();
  	lcd_backlight_on();	
  	lcd_putstring(0,"      NGX       ");
  	lcd_putstring(1,"mX-LPC1768 Stamp");

	while(1);
}
