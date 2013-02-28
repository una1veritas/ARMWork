
#include "armcmx.h"
#include <stdlib.h>
#include "GLCD/GLCD.h"

GLCD lcd(GLCD::KS0108);  // w/ init()

int main(void) {
	armcmx_init();
	printf("Hello!\n");
	
	lcd.begin();
	lcd.ClearScreen();
	delay(500);
	lcd.ClearScreen();
	delay(500);
	lcd.ClearScreen();
	delay(500);

	srand(23);
	int x = 0, y = 0;
	uint8 d;
//	uint16 color = 0xff;
	while (1) {
		delay_ms(20);
		lcd.GotoXY(x, y);
		lcd.SetDot(x, y, 1);
		x = ++x % 128;
		if (x == 0) 
			y = ++y % 64;
	}
}

