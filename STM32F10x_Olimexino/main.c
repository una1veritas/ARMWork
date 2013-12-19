#include <stm32f10x.h>

#include "armcmx.h"

int main(void) {
	armcmx_init();
	
	pinMode(PA5, OUTPUT | MEDSPEED);
	while(1) {
		digitalWrite(PA5, HIGH);
		delay(1000);
		digitalWrite(PA5, LOW);
		delay(1000);
	}
}
