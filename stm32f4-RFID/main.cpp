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

#include "armcmx.h"

//#include "spi.h"
#include "utility/stm32f4_discovery.h"
#include "KS0108/glcd.h"
#include "fonts/allFonts.h"
//#include "GLCD/fonts.h"

#include "i2c.h"
#include "RTC/DS1307.h"

#include "RFID/PN532_I2C.h"
#include "RFID/ISO14443.h"

#include "fcf.h"

FCFStruct idd;

void PN532_init();
void setup_peripherals(void);
void readcard(ISO14443 & card, FCFStruct & fcf);
uint8 read_FCFBlock(FCFStruct & idd);

//spi spi2bus = {SPI2, PB10, PC2, PC3, PB9};

const static byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const static byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#define PN532_REQ   PB9
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

char rbuf[32];

int main(void) {
	char mess[64], c;
//  uint8 *resptr;
  int strpos = 0;
	byte readerbuf[64];
	ISO14443 cardtmp;
	uint32 lastread = 0;
	enum STATUS {
		S_IDLE,
		S_CARD_DETECT
	} status = S_IDLE;
	
	armcmx_init();
	setup_peripherals();
	
	while (1) {
		if ( status == S_IDLE ) {
      cardtmp.clear();
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
				readcard(card, idd);
				lastread = millis();
				GLCD.CursorTo(0, 2);
        GLCD.print(asctoBCD((char*)idd.pid, 12), HEX);
        GLCD.print('-');
        GLCD.print(dtoi(idd.reissue));
        //for(int i = 0; i < 12; i++) 
        //  printf("%c",idd.id[i]);
				//printf("\n");
			}
			if ( digitalRead(LED1) && millis() > lastread + 1500 ) {
				digitalWrite(LED1, LOW);
        card.clear();
			}
			if ( ds3231.updateTime() ) {
				ds3231.updateCalendar();
				sprintf(mess, "%02x:%02x:%02x %02x/%02x", ds3231.time>>16, ds3231.time>>8&0x7f, ds3231.time&0x7f, 
                ds3231.cal>>8&0x1f, ds3231.cal&0x3f);
        GLCD.ClearRect(0,0,127,8);
				GLCD.CursorTo(0, 0);
				GLCD.print(mess);
			}
      
      if ( usart_available(&stdserial) > 0 ) {
        c = 0;
        while ( usart_available(&stdserial) > 0 ) {
          c = usart_read(&stdserial);
          if ( c == '\n' || strpos > 61 ) {
            rbuf[strpos] = 0;
            strpos = 0;
            break;
          }
          if ( c == '\r' )
            continue;
          rbuf[strpos++] = c;
        }
        //
        if ( c == '\n' ) {
          ds3231.updateTime();
          printf("mess = %s, time = %06x\n", rbuf, ds3231.time);
          if ( rbuf[0] == 't' ) {
            ds3231.time = strtol(rbuf+1,NULL, 16);
            ds3231.setTime(ds3231.time);
            printf("time %06x\n", ds3231.time);
          } else
          if ( rbuf[0] == 'c' ) {
            ds3231.cal = strtol(rbuf+1,NULL, 16);
            ds3231.setCalendar(ds3231.cal);
            printf("cal %06x, %d\n", ds3231.cal, ds3231.dayOfWeek());
          }
        }
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

  printf("GPIO Bus LCD controller...");	
	GLCD.begin(); //NON_INVERTED);	
//	nokiaLCD.setContrast(0xaa);
	GLCD.ClearScreen();
//	nokiaLCD.drawBitmap(PCD8544::SFEFlame);
//	delay(500);
	GLCD.SelectFont(Fixed7x8);
	printf("done.  ");
	printf("\r\n");

//	printf("Initializing SPI2 bus...");
//	spi_init(&spi2bus, SPI2, PB10, PC2, PC3, PB9);
//	spi_begin(&spi2bus);

//	nokiaLCD.setContrast(0xaa);
//	nokiaLCD.drawBitmap(PCD8544::SFEFlame);
//	delay(500);
//	printf("done.  ");
//	printf("\r\n");

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

void readcard(ISO14443 & card, FCFStruct & fcf) {
  static char msg[128];
  char tmp[64];
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
    if ( read_FCFBlock(fcf) == 0 ) {
      printf("\n");
//      tone(4, 1200, 100);
      card.clear();
    }

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
	return;
}
  
byte read_FCFBlock(FCFStruct & idd) {
  word syscode = 0x00FE;
  int len;
  byte c;
  uint8 buf[128];

  // Polling command, with system code request.
  len = nfc.felica_Polling(buf, syscode);
  if ( len == 0 )
    return 0;
  printf("Sys. code %02x, ", syscode);
  printf(" IDm ");
  for(int i = 0; i < 8; i++) {
    printf("%02x", buf[i+1]);
    if ( i+1 < 8 )
      printf("-");
  }
  printf("\n");
  // low-byte first service code.
  // Suica, Nimoca, etc. 0x090f system 0x0300
  // Edy service 0x170f (0x1317), system 0x00FE // 8280
  // FCF 1a8b
  printf("Request Service code: ");
  word servcode = 0x1a8b;
  word scver = nfc.felica_RequestService(servcode);
  printf("%04x ver %04x.\n", servcode, scver);

  if ( scver == 0xffff ) 
    return 0;
  
  word blist[] = { 0, 1, 2, 3};
  c = nfc.felica_ReadBlocksWithoutEncryption(buf, servcode, (byte) 4, blist);
  if ( c == 0 ) {
    printf("\nfailed. \n");
    return 0;
  }
  for(int blk = 0; blk < 4; blk++) {
    printf("%02d: ", blk);
    for(int i = 0; i < 16; i++) {
      printf("%02x ",buf[i+blk*16]);
    }
    printf("\n");
    printf("  : ");
    for(int i = 0; i < 16; i++) {
      if ( isprint(buf[i]) ) {
        printf(" %c ", (char) buf[i+blk*16]);
      } else {
        printf("   ");
      }
    }
    printf("\n");
  }
  memcpy(&idd, buf, 64);
//  printf("\n--- End of FCF reading ---\n\n");
  return 1;
}
