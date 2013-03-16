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
#include <stm32f4xx_it.h>

#include "cmcore.h"

#include "spi.h"
#include "utility/stm32f4_discovery.h"
#include "GLCD/PCD8544.h"
#include "GLCD/fonts.h"

#include "i2c.h"
#include "RTC/DS1307.h"

#include "RFID/PN532_I2C.h"
#include "RFID/ISO14443.h"

void PN532_init();
void setup_peripherals(void);
char * readcard(ISO14443 & card);

spi spi2bus = {SPI2, PB10, PC2, PC3, PB9};
PCD8544 nokiaLCD(spi2bus, PB9, PD13, PB0);

const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#define PN532_REQ   PB11
#define PN532_RST 	0xff
// Not connected by default on the NFC Shield
I2CWire Wire1(I2C1, PB8, PB7);
PN532 nfc(Wire1, PN532::I2C_ADDRESS, PN532_REQ, PN532_RST);

DS1307 ds3231(Wire1, DS1307::CHIP_M41T62);

ISO14443 card;
byte pollingOrder[] = {
  2,
  TypeA,
  TypeF
};

int main(void) {
	byte bmap[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	char mess[128], *res;
	byte readerbuf[128];
	ISO14443 cardtmp;
	uint32 lastread = 0;
	enum STATUS {
		S_IDLE,
		S_CARD_DETECT
	} status = S_IDLE;
	
	cmcore_init();
	setup_peripherals();
	
	nokiaLCD.clear();
	
	while (1) {
		if ( status == S_IDLE ) {
			if ( nfc.InAutoPoll(1, 1, pollingOrder+1, pollingOrder[0]) 
					&& nfc.getAutoPollResponse((byte*) readerbuf) ) {
				digitalWrite(LED1, HIGH);
				// NbTg, type1, length1, [Tg, ...]
				cardtmp.set(readerbuf[1], readerbuf+3);
				if ( cardtmp == card ) {
						cardtmp.clear();
				}
			}
			if ( !cardtmp.isEmpty() && lastread + 500 <= millis() ) {
				card = cardtmp;
				res = readcard(card);
				lastread = millis();
				nokiaLCD.cursor(84);
				nokiaLCD.drawString(res);
				printf(res);
				printf("\n");
			}
			if ( digitalRead(LED1) && millis() > lastread + 1500 ) {
				digitalWrite(LED1, LOW);
				nokiaLCD.gotoXY(0,1);
				for(int i = 0; i < 4*84/8; i++)
					nokiaLCD.drawBitmap(bmap, 8, 8);
			}
			if ( ds3231.updateTime() ) {
				ds3231.updateCalendar();
				sprintf(mess, "%02x:%02x:%02x  ", ds3231.time>>16, ds3231.time>>8&0x7f, ds3231.time&0x7f);
				nokiaLCD.cursor(0);
				nokiaLCD.drawString(mess);
			}
		} 
	}
	
}

void setup_peripherals(void) {
	char tmp[64];
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	printf("\r\nSTM32F4 Discovery handmade proto-sys started w/ clock freqs: SYSCLK = %dl, HCLK = %dl, PCLK1 = %dl.\r\n", 
				RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency);

	printf("Initializing SPI2 bus...");
	spi_init(&spi2bus, SPI2, PB10, PC2, PC3, PB9);
	spi_begin(&spi2bus);

	printf("Nokia LCD w/ PCD8544...");	
	nokiaLCD.init();	
	nokiaLCD.setContrast(0xaa);
	nokiaLCD.clear();
//	nokiaLCD.drawBitmap(PCD8544::SFEFlame);
//	delay(500);
	nokiaLCD.setFont(Fixed_8w8);
	printf("done.  ");
	printf("\r\n");

	printf("Initializing I2C1 bus...");
	//  sck = PA5 / PB3, miso = PA6/ PB4, mosi = PA7 / PB5, nSS = PA4 / PA15
	Wire1.begin(100000);
	printf("MAXIM DS3231 ");	
	ds3231.begin();
	ds3231.updateTime();
	ds3231.updateCalendar();
	if ( ds3231.time == 0 ) {
		printf("reset rtc...");
		ds3231.setTime(0x95100);
		ds3231.setCalendar(0x130211);
	}
	printf(" %06x, %s, %06x, Ok.  ", ds3231.time, ds3231.copyNameOfDay(tmp, ds3231.dayOfWeek()), ds3231.cal);
	
	printf("NXP PN532 RFID reader...");	
	nfc.begin();
	PN532_init();
	printf("Ok. ");	
	printf("\r\n");
	
	GPIOMode(PinPort(LED1), PinBit(LED1), OUTPUT, MEDSPEED, PUSHPULL, NOPULL);

	printf("\nBasic initialization has been finished.\n\n");
}

void PN532_init() {
	uint8 respobuf[4];
	char tmp[64];
  int i;

  for (i = 0; i < 3 ; i++) {
    if ( nfc.GetFirmwareVersion() && nfc.getCommandResponse(respobuf) )
      break;
    delay(250);
  }
  if ( !(i < 3) ) {
    printf("Couldn't find PN53x on Wire.\n");
    printf("Halt.\n");
    while (1); // halt
  } 
	
	sprintf(tmp, "IC ver. %c, firm %d rev %d; ", respobuf[0], respobuf[1], respobuf[2]); 
	printf(tmp);
	
	printf("Support "); 
	if (respobuf[3] & 1 )
		printf("ISO/IEC 14443 Type A, ");
	if (respobuf[3] & 2 )
		printf("ISO/IEC 14443 Type B, ");
	if (respobuf[3] & 4)
		printf("ISO 18092, ");
  if ( !nfc.SAMConfiguration() ) {
		printf("\nSAMConfiguration failed, halt.\n");
		while (1);
	}
  printf("SAM Configured...");
}

char * readcard(ISO14443 & card) {
static char msg[256];
	char tmp[128];
	int length = 0;
	msg[0] = 0;
	if ( card.type == 0x11 ) {
		length += sprintf(tmp, "FeliCa %02x", (uint8)card.IDm[0]);
		strcat(msg, tmp);
		for(int i = 1; i < 8; i++) {
			length += sprintf(tmp, "-%02x", (uint8)card.IDm[i]);
			strcat(msg, tmp);
		}
		//length += sprintf(tmp, "]");
		//strcat(msg, tmp);
	} else if ( card.type == 0x10 ) {
		length += sprintf(tmp, "Mifare %02x", card.UID[0]);
		strcat(msg, tmp);
		for(int i = 1; i < card.IDLength; i++) {
			length += sprintf(tmp, "-%02x", card.UID[i]);
			strcat(msg, tmp);
		}
		//length += sprintf(tmp, "]");
		//strcat(msg, tmp);
	}
	return msg;
}
