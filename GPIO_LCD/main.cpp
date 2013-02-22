
#include "armcmx.h"

#include "GLCD/GLCD_KS0108.h"

KS0108 lcd;  // w/ init()

int main(void) {
	armcmx_init();
	
	printf("Hello!\n");
	
	lcd.start();
	
	printf("done 1.\n");
	uint8 x = 0, y = 0;
	for(x = 0; x < 128; x++) {
		for(y = 0; y < 64; y+=8) {
			lcd.SetAddress(x/CHIP_WIDTH, y>>3, x%CHIP_WIDTH);
			lcd.WriteData(0x00, x/CHIP_WIDTH);
		}
	}
	delay(10);
	x = 0;
	y = 0;
	uint8 color = 0x01;
	countdown_start(500);
	while (1) {
		if ( countdown_expired() ) {
			digitalToggle(PD12);
			countdown_start(500);
		}

		lcd.SetAddress(x/CHIP_WIDTH, y>>3, x%CHIP_WIDTH);
		lcd.WriteData(color<<(y&0x07), x/CHIP_WIDTH);
		x++;
		if ( x > DISPLAY_WIDTH ) {
			x = 0;
			y++;
			if ( y > DISPLAY_HEIGHT ) {
				y = 0;
				color = ~color;
			}
		}
		delay(10);
	}
}


