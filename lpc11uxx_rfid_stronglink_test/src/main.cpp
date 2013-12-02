/*
 * LPCLDC ported to Keil uV
 *
 */

#include <string.h>
#include "LPC11Uxx.h"
//#include "type.h"

#include "armcmx.h"
#include "USARTSerial.h"
//#include "SPI.h"
#include "I2CBus.h"
#include "ST7032i.h"
#include "RTC.h"

#include "ISO14443.h"
#include "StrongLink_I2C.h"

#include "PWM0Tone.h"

#include "StringBuffer.h"
//#include "SDFatFs.h"
//#include "SPISRAM.h"

#include "main.h"
//#include "nfcreader.h"
//#include "Task.h"


#define match(x,y)  (strcasecmp((char*)(x), (char*)(y)) == 0)
#define matchn(x,y,n)  (strcasencmp((char*)(x), (char*)(y), (n)) == 0)

#ifdef _LPCXPRESSO_CRP_
#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif


/*******************************************************************************
**   Main Function  main()
*******************************************************************************/


union IDCardFormat {
  struct {
    uint8 division[2];
    uint8 pid[8];
    uint8 issue;
    uint8 reserved1[5];
    uint8 namesjis[16];
    uint8 dofbirth[7];
    uint8 gender;
    uint8 dofissue[7];
    uint8 reserved2;
  } Iizuka;
  uint8 raw[48];
  
  void printOn(Print & p) {
    int i;
    p.print((char)Iizuka.division[1]);
    p.print('-');
    for (i = 0; i < 8; i++) {
      p.print((char)Iizuka.pid[i]);
    }
    p.print('-');
    p.print((char)Iizuka.issue);
  }
  
} idcard;

//char str64[64];
//StringBuffer stream(str64, 64);

uint8 key[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
uint8 factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#define SL030_TAGDETECT PIO0_17
#define SL030_WAKEUP PIO0_16

StrongLink_I2C reader(0x50, SL030_TAGDETECT, SL030_WAKEUP);
ISO14443Card lastcard;
uint32 lastread;
uint8 data[64];

ST7032i i2clcd(Wire, LED_LCDBKLT, LCD_RST);
RTC rtc(Wire, RTC::ST_M41T62);

uint32 dectobcd(uint32 val) {
  uint32 t = 0;
  int i;
  for(i = 0; i < 8 && val > 0; i++) {
    t |= (val % 10)<<(i*4);
    val /= 10;
  }
  return t;
}


void init() {
  SystemInit();
  GPIOInit();
  start_delay(); // for delay
}

/**
 *  @title:  StrongLink SL018/SL030 RFID reader demo
 *  @author: marc@marcboon.com
 *  @see:    http://www.stronglink.cn/english/sl018.htm
 *  @see:    http://www.stronglink.cn/english/sl030.htm
 *
 *  Arduino to SL018/SL030 wiring:
 *  A4/SDA     2     3
 *  A5/SCL     3     4
 *  5V         4     -
 *  GND        5     6
 *  3V3        -     1
 */


void loop() {
  int i;
  bool result;
  
  // start seek mode
  if ( (millis() > lastread + 1000) ) {
    if ( reader.detect() && reader.select() && lastcard != reader.card ) {
      lastcard = reader.card;
      lastread = millis();
      Serial.print(lastcard);
      Serial.println();
      if ( lastcard.type != NFC::CARDTYPE_EMPTY ) {
        if ( lastcard.type == NFC::CARDTYPE_MIFARE ) {
          Serial.print("Trying to auth...");
          if ( reader.auth_sector(4>>2, key) ) {
            Serial.print(" auth succeeded, ");
            delay(20);
            i = 0;
            do {
              result = true;
              result = reader.read_block(4, data);
              //delay(20);
              result = result && reader.read_block(5, data+16);
              //delay(20);
              result = result && reader.read_block(6, data+32);
              //delay(20);
              if ( !result ) {
                Serial.println("Read data blocks failed. ");
              }
              delay(50);
              i++;
            } while (i < 10 && !result);
            Serial.println("blocks red.");
            memcpy(idcard.raw, data, 48);
            idcard.printOn(Serial);
            Serial.println();
          }
        }
      }
    } else if ( millis() > lastread + 5000 ) {
      if ( !lastcard.is_empty() ) {
        lastcard.clear();
        Serial.println("Idle.");
      }
      delay(100);
    }
  }    

}


void setup() {
  char tmp32[32];
  pinMode(SW_USERBTN, INPUT);
  
  Serial.begin(115200, RXD2, TXD2);
  Serial.println("\n\nUSART Serial started. \nHello.");

  Serial.print("Starting I2C...");
  Wire.begin(); 	/* initialize I2c */
  if ( Wire.status == FALSE )
  	while ( 1 );				/* Fatal error */
  pinMode(I2C_PULLUP, OUTPUT); // pull up by 4k7
  digitalWrite(I2C_PULLUP, LOW);
  
  // I2C lcd
  Serial.print(" LCD");
  while(1)
    if ( i2clcd.begin() ) break;
  i2clcd.clear();
  i2clcd.print("Hello.");
  
  Serial.print(", RTC");
  while(1)
    if ( rtc.begin() ) break;
  rtc.update();
  sprintf((char*)tmp32, "20%02x/%02x/%02x-%02x:%02x:%02x", rtc.cal>>16&0xff, rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f );
  Serial.print(" [");
  Serial.print((char*)tmp32);
  Serial.print("] ");
  
  Serial.print(" StrongLink 030");
  reader.begin();
  Serial.println(".\n");
  
  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(LED_USER, OUTPUT);
  digitalWrite(LED_USER, HIGH);

  lastread = millis();
  
  Serial.println("\nSetup finished.\n");
}


int main (void) {  
  init();
  PWM0_tone(PIO1_13, 880, 250);
  setup();
  // ---------------------------------------
  for(;;)
    loop();
  
}

/******************************************************************************
**                            End Of File
******************************************************************************/
