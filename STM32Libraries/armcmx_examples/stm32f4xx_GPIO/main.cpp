#include <stm32f4xx.h>

#include "armcmx.h"

GPIOPin LEDs[] = { PD12, PD13, PD14, PD15 };

int main(void) {

	armcmx_init();
	
	for(int i = 0; i < 4; i++) {
		pinMode(LEDs[i], OUTPUT);
	}
		
	int8 flg = 1;
	int bit = 1;	
	while (1) {

		for(int i = 0; i < 4; i++) {
			if ( bit>>i & 1 )
				digitalWrite(LEDs[i], HIGH);
			else
				digitalWrite(LEDs[i], LOW);
		}
		if ( bit & 1 )
			flg = 1;
		else if ( bit & 0x08 )
			flg = -1;
		if ( flg > 0 )
			bit <<= 1;
		else
			bit >>= 1;
		delay(200);		
	}
}
