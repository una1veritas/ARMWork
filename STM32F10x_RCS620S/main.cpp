#include <ctype.h>

#include <stm32f10x.h>

#include "armcmx.h"
#include "olimexino.h"
#include "USARTSerial.h"
#include "RCS620S.h"

usart usart1 = { USART1, PA10, PA9, 115200, WORDLENGTH_8BIT | FLOW_NONE | PARITY_NONE | STOPBITS_1 };
USARTSerial Serial1(&usart1);
RCS620S nfcreader(Serial1);

int main(void) {
	int16_t c;
	uint32_t since;
	
	armcmx_init();
	
	Serial1.begin(115200);

	usart_begin(&stdserial, 115200);
	usart_print(&stdserial, "Hello, welcome to the world of F1!\n");
	
	nfcreader.start();
	
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
