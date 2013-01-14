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
Nokia5110 nokiaLCD(&SPI1Bus, PA4, PA3, PA2);

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

	const char message[] = 
			"This royal throne of kings, this scepter'd isle, \n"
			"This earth of majesty, this seat of Mars, \n"
			"This other Eden, demi-paradise, \n"
			"This fortress built by Nature for herself\n"
			"Against infection and the hand of war, \n"
			"This happy breed of men, this little world,\n" 
			"This precious stone set in the silver sea, \n"
			"Which serves it in the office of a wall, \n"
			"Or as a moat defensive to a house, \n"
			"Against the envy of less happier lands, \n"
			"This blessed plot, this earth, this realm, this England,";
	const uint16 messlen = strlen(message);
	
	TIM2_timer_start();
	Serial6.begin(57600);
	spi_init(&SPI1Bus, SPI1, PA5, PA6, PA7, PA4); 
	//  sck = PA5 / PB3, miso = PA6/ PB4, mosi = PA7 / PB5, nSS = PA4 / PA15
	Wire1.begin(100000);
	
	Serial6.print("Basic initialization has been finished.\n");
	
	nokiaLCD.init();
	nfc.begin();
	
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	Serial6.print(message);
	Serial6.print("\r\n\r\n");
	Serial6.flush();

	sprintf(tmp, "Clock frequencies: SYSCLK = %dl, HCLK = %dl, PCLK1 = %dl\r\n", 
		RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency);
	Serial6.print(tmp); 

	GPIOMode(PinPort(LED1), (PinBit(LED1) | PinBit(LED2) | PinBit(LED3) | PinBit(LED4)), 
					OUTPUT, FASTSPEED, PUSHPULL, NOPULL);
					
	nokiaLCD.clear();
	nokiaLCD.selectFont(Nokia5110::CHICAGO10);
	nokiaLCD.drawString("Hello, there.");
	delay(1000);
		
		
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
				nokiaLCD.clear();
				nokiaLCD.drawString("Felica  IDm: ");
				Serial6.print("FeliCa IDm: ");
				for(int i = 0; i < 8; i++) {
          Serial6.print((uint8)card.IDm[i], HEX);
          Serial6.print(' ');
					sprintf(tmp, " %2x", card.IDm[i]);
					nokiaLCD.drawString(tmp);
				}
        Serial6.println(" detected. ");
			} else if ( card.type == 0x10 ) {
				nokiaLCD.clear();
				nokiaLCD.drawString("Mifare  ID: ");
        Serial6.print("Mifare  ID: ");
				for(int i = 0; i < card.IDLength; i++) {
          Serial6.print(card.UID[i], HEX);
          Serial6.print(' ');
					sprintf(tmp, " %2x", card.UID[i]);
					nokiaLCD.drawString(tmp);
				}
				Serial6.println();
			}
			delay(500);
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
  int i;
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

	Serial6.print("PN53x IC version '"); 
	Serial6.print((char) respobuf[0]); 
	Serial6.print("', firmware ver. "); 
	Serial6.print(respobuf[1]); 
	Serial6.print(", rev. "); 
	Serial6.println(respobuf[2]);
	Serial6.print("Support "); 
	if (respobuf[3] & 1 )
		Serial6.print("ISO/IEC 14443 Type A, ");
	if (respobuf[3] & 2 )
		Serial6.print("ISO/IEC 14443 Type B, ");
	if (respobuf[3] & 4)
		Serial6.print("ISO 18092 ");
	Serial6.println();

  Serial6.print("SAMConfiguration ");
  if ( nfc.SAMConfiguration() )
		Serial6.println("finished.");
}

