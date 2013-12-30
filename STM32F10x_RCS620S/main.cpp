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
#define POLLING_INTERVAL 2000
 		
int main(void) {
	int16_t c;
	uint32_t since;
	uint16_t reslen;
	uint8_t tmp[64];
	uint16_t blklist[] = { 0x00 };
	static uint8_t payload[] = { 0x00, 0xff, 0xff, 0x01, 0x00 };
	
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
		
		if ( since + POLLING_INTERVAL < millis() ) {
			digitalWrite(USERLED1, HIGH);
			nfcreader.timeout = COMMAND_TIMEOUT;  
			*((uint16_t*) &payload[1]) = (word) 0x00fe;
			reslen = nfcreader.InListPassiveTarget(1, NFC::BAUDTYPE_212K_F, payload, tmp);
		 
			if(reslen) {
				digitalWrite(USERLED2, HIGH);
				
				Serial.print("IDm:");
				Serial.printBytes(nfcreader.idm, 8);
				Serial.println(".");
				Serial.print("PMm:");
				Serial.printBytes(nfcreader.pmm, 8);
				Serial.println(".\n");
				//
				
				reslen = nfcreader.FeliCa_RequestService(0x1a8b);
				Serial.print("Req. Service resp.: ");
				Serial.println(reslen, HEX);
				if ( reslen != 0xffff ) {
					Serial.println("FCF ID Card: ");
					reslen = nfcreader.FeliCa_ReadWithoutEncryption((word)0x1a8b, 0, tmp );
					if ( reslen != 0 ) {
						Serial.printBytes(tmp, reslen*16);
						Serial.print(", \"");
						for(int i = 0; i < 16; i++) {
							if ( isprint(tmp[i]) )
								Serial.print((char)tmp[i]);
							else
								Serial.print(' ');
						}
						Serial.println('"');
					}
				}
				digitalWrite(USERLED2, LOW);
				Serial.flush();
			}
			nfcreader.RFOff();
			digitalWrite(USERLED1, LOW);
			since = millis();
		}
	}
}
