#include <ctype.h>

#include <stm32f10x.h>

#include "armcmx.h"
#include "olimexino.h"

int main(void) {
	int16_t c;
	uint32_t since;
	
	armcmx_init();
	
	usart_begin(&stdserial, 115200);
	usart_print(&stdserial, "Hello, welcome to the world of F1!\n");
	pinMode(USERLED1, OUTPUT | MEDSPEED);
	pinMode(USERLED2, OUTPUT | MEDSPEED);
	since = millis();
	while(1) {
		if ( usart_available(&stdserial) > 0 ) {
			while ( (c = usart_read(&stdserial)) > 0 ) 
				usart_write(&stdserial, toupper(c&0xff));
		}
		if ( since + 500 < millis() ) {
			if ( digitalRead(USERLED1) ) {
				digitalWrite(USERLED1, LOW);
				digitalWrite(USERLED2, HIGH);
			} else {
				digitalWrite(USERLED1, HIGH);
				digitalWrite(USERLED2, LOW);
			}
			since = millis();
		}
	}
}
