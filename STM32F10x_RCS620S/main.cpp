#include <ctype.h>

#include <stm32f10x.h>

#include "armcmx.h"
#include "olimexino.h"
#include "USARTSerial.h"
#include "RCS620S.h"

#include "FCF.h"

void print_FCF(Print & prn, FCF_IDCard & idcard);

#define CHARNUM(x)  ((x)? ((x) - '0') & 0x0f : 0)

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
  char tmpstr[16];
	static uint8_t payload[] = { 0x00, 0xff, 0xff, 0x01, 0x00 };
	FCF_IDCard idcard;
  
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
		 
			if( (reslen = nfcreader.InListPassiveTarget(1, NFC::BAUDTYPE_212K_F, payload, tmp)) != 0 ) {
				digitalWrite(USERLED2, HIGH);
				
        Serial.println();
				Serial.print("IDm: ");
				Serial.printBytes(nfcreader.idm, 8);
				Serial.println(".");
				Serial.print("PMm: ");
				Serial.printBytes(nfcreader.pmm, 8);
				Serial.println(".");
				//
				reslen = nfcreader.FeliCa_RequestService(0x1a8b);
				Serial.print("Service ver.: ");
				Serial.println(reslen, HEX);
				if ( reslen != 0xffff ) {
					Serial.print("FCF ID Card: ");
          if ( nfcreader.FeliCa_ReadWithoutEncryption((word)0x1a8b, 0, tmp ) ) {
            idcard.division = CHARNUM(tmp[0])<<4 | CHARNUM(tmp[1]);
            memcpy(idcard.pid, tmp+2, 12);
            idcard.issue = tmp[14] - '0';
            idcard.gend = tmp[15] - '0';
          }
          if ( nfcreader.FeliCa_ReadWithoutEncryption((word)0x1a8b, 2, tmp ) ) {
            memcpy(tmpstr, tmp, 8);
            tmpstr[8] = 0;
            idcard.orgid = atol(tmpstr);
            memcpy(tmpstr, tmp+8, 8);
            tmpstr[8] = 0;
            idcard.doi = strtol(tmpstr, 0, 16);            
          }
          if ( nfcreader.FeliCa_ReadWithoutEncryption((word)0x1a8b, 3, tmp ) ) {
            memcpy(tmpstr, tmp, 8);
            tmpstr[8] = 0;
            idcard.gdthru = strtol(tmpstr, 0, 16);
          }
          print_FCF(Serial, idcard);
          Serial.println();
				}
				digitalWrite(USERLED2, LOW);
        Serial.println(".");
			} else if ( (reslen = nfcreader.InListPassiveTarget(1, NFC::BAUDTYPE_106K_A, payload, tmp)) != 0 ) {
        Serial.println("Mifare?");
        Serial.printBytes(nfcreader.idm, nfcreader.idm[0]+1);
        Serial.println();
        Serial.printBytes(tmp, reslen);
        Serial.println();
        nfcreader.Mifare_Authenticate((const uint8_t *)"\xff\xff\xff\xff\xff\xff");
      } else {
        Serial.print(".");
      }
			nfcreader.RFOff();
			digitalWrite(USERLED1, LOW);
			since = millis();
		}
	}
}

void print_FCF(Print & prn, FCF_IDCard & idcard) {
  char tmpstr[16];
  prn.print("DIV ");
  prn.print(idcard.division, HEX);
  memcpy(tmpstr, idcard.pid, 12);
  tmpstr[12] = 0;
  prn.print(", ID ");
  prn.print(idcard.pid);
  prn.print("-");
  prn.print(idcard.issue);
  prn.print(", GENDER ");
  prn.print( idcard.gender() );
  prn.print(", ORG ID ");
  prn.print( idcard.orgid );
  prn.print(", Day of Issue ");
  prn.print( idcard.doi, HEX );
  prn.print(", Good Thru ");
  prn.print( idcard.gdthru, HEX );
  prn.println();
}

