
#include "armcmx.h"

#include "GLCD/GLCD.h"

GLCD lcd(GLCD::KS0108);  // w/ init()

int main(void) {
	armcmx_init();
	printf("Hello!\n");
	
	lcd.begin();
	lcd.ClearScreen(0xff);
	while(1);
	int x = 0, y = 0;
	while (1) {
		delay(100);
		lcd.Address(x/64,y>>3,x%64);
		lcd.WriteData(0xff, x/64);
		x++;
		y++;
		x %= 128;
		y %= 64;
	}
}


