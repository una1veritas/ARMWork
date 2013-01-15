/*
 * main.c
 *
 *  Created on: 2012/10/08
 *      Author: sin
 */

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include <stm32f4xx.h>
#include <stm32f4xx_usart.h>

#include "stm32f4xx_it.h"

#include "armcore.h"
#include "gpio.h"
#include "delay.h"

#include "USARTSerial.h"
#include "SPIBus.h"

#include "Boards/stm32f4_discovery.h"
#include "GLCD/Nokia5110.h"
#include "RFID/PN532_I2C.h"
#include "RFID/ISO14443.h"

const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

USARTSerial Serial6(USART6, PC7, PC6);
SPIBus SPI1Bus;
Nokia5110 glcd(&SPI1Bus, PA4, PA3, PA2);

#define IRQ   PB11
#define RESET PB12
// Not connected by default on the NFC Shield

I2CWire Wire1(I2C1, PB8, PB9);
PN532 nfc(Wire1, PN532::I2C_ADDRESS, IRQ, RESET);
void PN532_init();
ISO14443 card;
byte buf[48];
byte polling[] = {
  2,
  TypeA,
  TypeF
};

int main(void) {
	char tmp[256];

	TIM2_timer_start();
	Serial6.begin(57600);
	spi_init(&SPI1Bus, SPI1, PA5, PA6, PA7, PA4); 
	//  sck = PA5 / PB3, miso = PA6/ PB4, mosi = PA7 / PB5, nSS = PA4 / PA15
	Wire1.begin(100000);

	GPIOMode(PinPort(PD12), PinBit(PD12), OUTPUT, MEDSPEED, PUSHPULL, NOPULL);
	for(int i = 0; i < 3; i++) {
		digitalWrite(PD12, HIGH);
		delay(500);
		digitalWrite(PD12, LOW);
		delay(500);
	}
	
	Serial6.print("Basic initialization has been finished.\n");
	
	glcd.begin();
	nfc.begin();
	
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	

	sprintf(tmp, "Clock frequencies: SYSCLK = %dl, HCLK = %dl, PCLK1 = %dl\r\n", 
		RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency);
	Serial6.print(tmp); 

					
	digitalWrite(PD12, HIGH);
	glcd.clear();
//	glcd.selectFont(Nokia5110::CHICAGO10);
	glcd.drawString("Hello, there.");
	delay(1000);
	digitalWrite(PD12, LOW);
		
	PN532_init();
  card.clear();

	uint16 shift = 0;
		
	while (1) {
		uint8 c;
		
		if ( (c = nfc.InAutoPoll(1, 1, polling+1, polling[0])) && (c = nfc.getAutoPollResponse((byte*) buf)) ) {
      //mon << mon.printArray(tmp, 8) << mon.endl;
      // NbTg, type1, length1, [Tg, ...]
      card.set(buf[1], buf+3);
			if ( card.type == 0x11 ) {
				glcd.clear();
				glcd.drawString("Felica  IDm: ");
				Serial6.print("FeliCa IDm: ");
				for(int i = 0; i < 8; i++) {
          Serial6.print((uint8)card.IDm[i], HEX);
          Serial6.print(' ');
					sprintf(tmp, " %2x", card.IDm[i]);
					glcd.drawString(tmp);
				}
        Serial6.println(" detected. ");
			} else if ( card.type == 0x10 ) {
//				digitalWrite(LED1, LOW);
				glcd.clear();
				glcd.drawString("Mifare  ID: ");
        Serial6.print("Mifare  ID: ");
				for(int i = 0; i < card.IDLength; i++) {
          Serial6.print(card.UID[i], HEX);
          Serial6.print(' ');
					sprintf(tmp, " %2x", card.UID[i]);
					glcd.drawString(tmp);
				}
				Serial6.println();
			}
			delay(500);
//			digitalWrite(LED1, HIGH);
		}
		
		/*
		if ( millis() / 125 != shift ) {
			shift = millis()/ 125;
			nokiaLCD.clear();
			nokiaLCD.gotoXY(7- shift%7,0);
			strncpy(tmp, message+((shift/7) % messlen), 48);
			tmp[48] = 0;
			nokiaLCD.drawString(tmp);
//		usart_print(&Serial6, tmp);
//		usart_print(&Serial6, "\r\n");
		}
		*/
	}
}


void PN532_init() {
	uint8 respobuf[8];
	char tmp[64];
  int i;

	glcd.clear();
	glcd.drawString("Initializing PN53x...");
  for (i = 0; i < 3 ; i++) {
    if ( nfc.GetFirmwareVersion() && nfc.getCommandResponse(respobuf) )
      break;
    delay(250);
  }
  if ( !(i < 3) ) {
    Serial6.println("Couldn't find PN53x on Wire.");
    Serial6.println("Halt.");
    while (1); // halt
  } 
	
	glcd.clear();
	sprintf(tmp, "IC ver. %c, firm %d rev %d; ", respobuf[0], respobuf[1], respobuf[2]); 
	glcd.drawString(tmp);
	Serial6.println(tmp);
	
	Serial6.print("Support "); 
	if (respobuf[3] & 1 )
		Serial6.print("ISO/IEC 14443 Type A, ");
	if (respobuf[3] & 2 )
		Serial6.print("ISO/IEC 14443 Type B, ");
	if (respobuf[3] & 4)
		Serial6.print("ISO 18092 ");
	Serial6.println();

  Serial6.print("SAMConfiguration ");
  if ( !nfc.SAMConfiguration() ) {
		Serial6.println("failed, halt.");
		while (1);
	}
  Serial6.print("finished.");
	//glcd.clear();
	glcd.drawString("SAM Config finished.");	
}

