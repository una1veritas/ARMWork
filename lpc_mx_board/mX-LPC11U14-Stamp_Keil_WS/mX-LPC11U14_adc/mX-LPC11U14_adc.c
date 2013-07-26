#include "lcd.h"


void ADCExp(void);

int main(void)
{

	init_lcd();						// Initialize LCD
	lcd_putstring(LINE1,"NGX TECHNOLOGIES");
   	ADCExp();
	while(1)
    {
    }
}

