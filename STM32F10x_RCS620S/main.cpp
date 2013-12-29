#include <ctype.h>

#include <stm32f10x.h>

#include "armcmx.h"
#include "olimexino.h"
#include "USARTSerial.h"
#include "RCS620S.h"

usart usart1 = { USART1, PA10, PA9, 57600, WORDLENGTH_8BIT | FLOW_NONE | PARITY_NONE | STOPBITS_1 };
USARTSerial Serial1(&usart1);
RCS620S nfcreader(Serial1);

#define COMMAND_TIMEOUT 400
#define POLLING_INTERVAL 1000
 
int main(void) {
	int16_t c;
	uint32_t since;
	uint16_t reslen;
	uint8_t tmp[64];
	
	int i;
	
	armcmx_init();
	
	Serial1.begin(115200);

	Serial.begin(57600);
	Serial << "Hello, welcome to the world of F1!\n";
	
	if ( !nfcreader.start() ) {
		Serial << "RCS620S start failed.\n";
		while (1);
	}
	
	pinMode(USERLED1, OUTPUT | MEDSPEED);
	pinMode(USERLED2, OUTPUT | MEDSPEED);
	
	since = millis();
	while(1) {
		if ( usart_available(&stdserial) > 0 ) {
			while ( (c = usart_read(&stdserial)) > 0 ) 
				usart_write(&stdserial, toupper(c&0xff));
		}
		
		if ( since + 3000 < millis() ) {
			since = millis();
			digitalWrite(USERLED1, HIGH);
				nfcreader.timeout = COMMAND_TIMEOUT;
				reslen = nfcreader.InListPassiveTarget(tmp);
		 
			if(reslen) {
				Serial.print("IDm: ");
				for(i = 0; i < 8; i++) {
					Serial.print(nfcreader.idm[i]>>8, HEX);
					Serial.print(nfcreader.idm[i]&0x0f, HEX);
					Serial.print(' ');
				}
				Serial.println(".\n");
			}
		 
			nfcreader.RFOff();
			digitalWrite(USERLED1, LOW);
		}
	}
}
