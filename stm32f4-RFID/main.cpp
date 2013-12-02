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
#include "i2c.h"
#include "utility/stm32f4_discovery.h"

#include "LCD/LiquidCrystal.h"
#include "RTC/DS1307.h"
#include "RFID/PN532_I2C.h"
#include "RFID/ISO14443.h"

#define dtoi(c) ( isdigit(c) ? (c) - '0' : (c) )
  static uint32 asctoBCD(char asc[8], uint8 maxlen) { // returns bcd
    uint32 res = 0;
    for(int i = 0; i < maxlen; i++) {
      if ( asc[i] == ' ' ) break;
      res <<= 4;
      res |= dtoi(asc[i]) & 0x0f;
    }
    return res;
  }
  
  uint32 BCDtoi(uint32 bcd) {
    uint32 result = 0;
    for(int i = 0; i < 4; i++) {
      result *= 10;
      result += bcd >> (8*(3-i) +4) & 0x0f;
      result *= 10;
      result += bcd >> (8*(3-i)) & 0x0f;
    }
    return result;
  }
  
  uint32 itoBCD(uint32 val) {
    uint32 result = 0;
    for(int i = 0; i < 8 && val > 0; i++) {
      result += (val%10)<<(i*4);
      val /= 10;
    }
    return result;
  }

void PN532_init();
void setup(void);
void readcard(ISO14443 & card);
uint8 get_FCFBlock(ISO14443 & card);
uint8 get_MifareBlock(ISO14443 & card);

ISO14443 idcard;

const static byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const static byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#define PN532_REQ   PB9
#define PN532_RST 	0xff  // Not connected by default on Adafruit's shield
I2CWire Wire1(I2C1, PB8, PB7);
PN532 nfc(Wire1, PN532::I2C_ADDRESS, PN532_REQ, PN532_RST);
byte pollingOrder[] = {
  2,
  TypeA,
  TypeF
};

DS1307 ds3231(Wire1, DS1307::CHIP_M41T62);
LiquidCrystal lcd(PD0, PD1, PE8, PE9, PE10, PE11);
char rbuf[32];

int main(void) {
	char mess[64], c;
//  uint8 *resptr;
  int strpos = 0;
	byte readerbuf[64];
	ISO14443 cardtmp;
	uint32 lastread = 0;
  uint32 tdate;
	enum STATUS {
		S_IDLE,
		S_CARD_DETECT
	} status = S_IDLE;
	
	armcmx_init();
	setup();
  lcd.clear();
//  GLCD.SelectFont(Chicago_12);
//  GLCD.CursorTo(0,0);
//  GLCD.println("!\"#0123456789 A Quick brown fox jumped over the lazy dog0|.");

	while (1) {
		if ( status == S_IDLE ) {
			if ( nfc.InAutoPoll(1, 1, pollingOrder+1, pollingOrder[0]) 
        && nfc.getAutoPollResponse((byte*) readerbuf) ) {
				digitalWrite(LED1, HIGH);
				// NbTg, type1, length1, [Tg, ...]
				cardtmp.set(readerbuf[1], readerbuf+3);
				if ( cardtmp != idcard && millis() > lastread + 500 ) {
          idcard = cardtmp;
          readcard(idcard);
          if ( !idcard.isEmpty() ) {
            lastread = millis();
            //
            //lcd.cursor();
            //lcd.blink();
            lcd.setCursor(0, 1);
            lcd.print("ID ");
            if ( idcard.type == FeliCa212kb )
              sprintf(mess,"%08x-%x",asctoBCD((char*)idcard.datablock.fcf.pid, 8), dtoi(idcard.datablock.fcf.reissue));
            else
              sprintf(mess,"%08x-%x",asctoBCD((char*)idcard.datablock.ktech.pid, 8), dtoi(idcard.datablock.ktech.reissue));
            lcd.print(mess);
            if ( idcard.type == FeliCa212kb )
              sprintf(mess, "  (%c%c)", 
                (isprint(idcard.datablock.fcf.division[0])? idcard.datablock.fcf.division[0] : ' '), 
                (isprint(idcard.datablock.fcf.division[1])? idcard.datablock.fcf.division[1] : ' '));
            else
              sprintf(mess, "  (%c%c)", 
                (isprint(idcard.datablock.ktech.division[0])? idcard.datablock.ktech.division[0] : ' '), 
                (isprint(idcard.datablock.ktech.division[1])? idcard.datablock.ktech.division[1] : ' ')); 
            lcd.print(mess);
            lcd.setCursor(0,2);
            lcd.print("Expires ");
            if ( idcard.type == FeliCa212kb ) 
              tdate = asctoBCD((char*)idcard.datablock.fcf.goodthru, 8);
            else {
              tdate = asctoBCD((char*) (idcard.datablock.ktech.dofissue + 1), 6);
              uint16 gyear;
              printf("%08x, %08x\n", gyear, tdate);
              if ( idcard.datablock.ktech.dofissue[0] == 'H' ) {
                gyear = BCDtoi(tdate>>16) + 1988;
                tdate = (tdate & 0xffff) | itoBCD(gyear)<<16;
              } else {
                // unknown, probably 'Showa'
                gyear = BCDtoi(tdate>>16) + 1925;
                tdate = (tdate & 0xffff) | itoBCD(gyear)<<16;
              }
             // printf("%08x, %08x\n", gyear, tdate);
              float jd = ds3231.JD2000(tdate+0x030000) - 1;
             // printf("expire %012d\n", (uint32)ds3231.CalendarDate(jd));
              tdate = itoBCD((uint32)ds3231.CalendarDate(jd));
            }
            sprintf(mess, "%04x/%02x/%02x", tdate>>16&0xffff, tdate>>8&0xff, tdate&0xff);
            lcd.print(mess);
            lcd.setCursor(0,3);
            for(int i = 0; i < 16; i++) {
              if ( idcard.type == FeliCa212kb && idcard.datablock.fcf.namekana[i] != 0 ) 
                lcd.print((char)idcard.datablock.fcf.namekana[i]);
              else
                lcd.print(' ');
            }
           // lcd.noCursor();
           // lcd.noBlink();
          }
        }
      }
			if ( digitalRead(LED1) && millis() > lastread + 2500 ) {
				digitalWrite(LED1, LOW);
        idcard.clear();
        lcd.setCursor(0,1);
        lcd.print("                    ");
        lcd.setCursor(0,2);
        lcd.print("                    ");
        lcd.setCursor(0,3);
        lcd.print("                    ");
			}
			if ( ds3231.updateTime() ) {
				ds3231.updateCalendar();
				sprintf(mess, "%02x:%02x:%02x %02x ", ds3231.time>>16, ds3231.time>>8&0x7f, ds3231.time&0x7f, ds3231.cal&0x3f);
        lcd.setCursor(0, 0);
        lcd.print(mess);
        lcd.print(ds3231.copyNameOfMonth(mess,ds3231.cal>>8)); 
        lcd.print(" ");
        sprintf(mess, "20%02x", ds3231.cal>>16&0xff);
				lcd.print(mess);
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


void setup(void) {
  
	char tmp[64];
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	printf("\r\nSTM32F4 Discovery handmade proto-sys started w/ clock freqs: SYSCLK = %dl, HCLK = %dl, PCLK1 = %dl.\r\n", 
				RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency);

  printf("GPIO Bus LCD controller...");	
	lcd.begin(20,4);
  lcd.print("Initializing periphs..");
	printf("done.  ");
	printf("\r\n");

	printf("Initializing I2C1 bus...");
	//  sck = PA5 / PB3, miso = PA6/ PB4, mosi = PA7 / PB5, nSS = PA4 / PA15
	Wire1.begin(100000);
	printf("RTC MAXIM DS3231/ST M41T62");	
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

void readcard(ISO14443 & card) {
	if ( card.type == 0x11 ) {
		printf("FeliCa %02x", (uint8)card.IDm[0]);
		for(int i = 1; i < 8; i++) {
			printf("-%02x", (uint8)card.IDm[i]);
		}
    printf("\n");
    if ( get_FCFBlock(card) == 0 ) {
      card.clear();
      return;
    }
	} else if ( card.type == 0x10 ) {
		printf("Mifare %02x", card.UID[0]);
		for(int i = 1; i < card.IDLength; i++) {
			printf("-%02x", card.UID[i]);
		}
    printf("\n");
    if ( get_MifareBlock(card) == 0 ) {
      card.clear();
      return;
    }
	}
	return;
}
  
uint8 get_FCFBlock(ISO14443 & card) {
  word syscode = 0x00FE;
  int len;
  byte c;

  // Polling command, with system code request.
  len = nfc.felica_Polling(card.datablock.raw, syscode);
  if ( len == 0 ) {
    printf("failed polling w/ system code 0x00fe.\n");
    return 0;
  }
  // low-byte first service code.
  // Suica, Nimoca, etc. 0x090f system 0x0300
  // Edy service 0x170f (0x1317), system 0x00FE // 8280
  // FCF 1a8b
  printf("Request Service code: ");
  word servcode = 0x1a8b;
  word scver = nfc.felica_RequestService(servcode);
  if ( scver == 0xffff ) 
    return 0;
  printf("%04x ver %04x.\n", servcode, scver);  
  word blist[] = { 0, 1, 2, 3};
  c = nfc.felica_ReadBlocksWithoutEncryption(card.datablock.raw, servcode, (byte) 4, blist);
  if ( c == 0 ) {
    printf("\nfailed reading FCF blocks. \n");
    return 0;
  }
  for(int blk = 0; blk < 4; blk++) {
    printf("%02d: ", blk);
    for(int i = 0; i < 16; i++) {
      printf("%02x ",card.datablock.raw[i+blk*16]);
    }
    printf("\n");
    printf("  : ");
    for(int i = 0; i < 16; i++) {
      if ( isprint(card.datablock.raw[i]) ) {
        printf(" %c ", (char) card.datablock.raw[i+blk*16]);
      } else {
        printf("   ");
      }
    }
    printf("\n");
  }
//  printf("\n--- End of FCF reading ---\n\n");
  return 1;
}


uint8 get_MifareBlock(ISO14443 & card) {
  uint8 res;
  nfc.targetSet(0x10, card.UID, card.IDLength);
  if ( nfc.mifare_AuthenticateBlock(4, IizukaKey_b) 
     && nfc.getCommandResponse(&res) && res == 0) {
    nfc.mifare_ReadDataBlock(4, card.datablock.raw);
    nfc.mifare_ReadDataBlock(5, card.datablock.raw+16);
    nfc.mifare_ReadDataBlock(6, card.datablock.raw+32);
    idcard.type = Mifare;
    for(int blk = 0; blk < 3; blk++) {
      printf("%02d: ", blk);
      for(int i = 0; i < 16; i++) {
        printf("%02x ",card.datablock.raw[i+blk*16]);
      }
      printf("\n");
      printf("  : ");
      for(int i = 0; i < 16; i++) {
        if ( isprint(card.datablock.raw[i+blk*16]) ) {
          printf(" %c ", (char) card.datablock.raw[i+blk*16]);
        } else {
          printf("   ");
        }
      }
      printf("\n");
    }

  } 
  else {
    printf("Mifare sector 1 Authentication failed.\n");
    return 0;
  }
  return 1;
}
