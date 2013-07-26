#include "lcd.h"
int main(void)
{
	init_lcd();
	lcd_putstring(0, "     NGX     ");
	lcd_putstring(1, "  TECHNOLOGIES  ");
    while(1)
    {
    }
}
