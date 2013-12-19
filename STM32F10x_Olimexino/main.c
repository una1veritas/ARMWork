#include <ctype.h>

#include <stm32f10x.h>

#include "armcmx.h"

void usart_init(usart * usx, USART_TypeDef * usartx, const GPIOPin rx, const GPIOPin tx);
void usart_begin(usart * usx, const uint32_t baud);

int main(void) {
	int16_t c;
	
	armcmx_init();
	
	usart_init(&stdserial, USART2, PA2, PA1);
	usart_begin(&stdserial, 115200);
	usart_print(&stdserial, "Hello, welcome to the world of F1!\n");
	pinMode(PA5, OUTPUT | MEDSPEED);
	while(1) {
		if ( usart_available(&stdserial) > 0 ) {
			while ( (c = usart_read(&stdserial)) > 0 ) 
				usart_write(&stdserial, toupper(c&0xff));
		}
		/*
		digitalWrite(PA5, HIGH);
		delay(1000);
		digitalWrite(PA5, LOW);
		delay(1000);
		*/
	}
}
