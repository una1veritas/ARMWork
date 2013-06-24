/*
 * LPCLDC ported to Keil uV
 *
 */

#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"
//#include "xprintf.h"
//#include "systick.h"

#include "armcmx.h"
#include "USARTSerial.h"
#include "I2CBus.h"
#include "i2clcd.h"
#include "ST7032i.h"
#include "DS1307.h"
#include "PN532_I2C.h"

#include "ISO14443.h"

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

// Strawberry Linux original lpclcd port maps
#define LCDBKLT PIO1_3
#define LCDRST  PIO1_25
#define USERLED PIO1_6
#define USERBTN PIO0_1
#define RXD2    PIO0_18
#define TXD2    PIO0_19
#define NFC_IRQ PIO1_5

#define RTC_ADDR 0x68


ST7032i i2clcd(Wire, LCDBKLT, LCDRST);
DS1307 rtc(Wire, DS1307::CHIP_M41T62);
PN532  nfcreader(Wire, PN532::I2C_ADDRESS, NFC_IRQ, PIO1_25);
const byte NFCpollingOrder[] = {
  2,
  TypeA,
  TypeF
};

void init() {
  SystemInit();
  GPIOInit();
  // systick initialize
//  SysTick_Config( SystemCoreClock / 1000 );
//  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

  init_timer16_1();
  // initialize xprintf
  //xfunc_out = (void(*)(unsigned char))i2clcd_data;
}

char msg[32];
uint8_t tmp[32];

void setup() {
  
  enable_timer16_1(); // for delay
  
  pinMode(USERBTN, INPUT);
  
  /* NVIC is installed inside UARTInit file. */
  //USART_init(&usart, PIO0_18, PIO0_19);
  Serial.begin(115200);
  Serial.println("\nUSART Serial started. \nHello.");

  Wire.begin();
  if ( Wire.status == FALSE ){	/* initialize I2c */
  	while ( 1 );				/* Fatal error */
  }
  Serial.println("I2C Bus started.");
  
  // I2C液晶を初期化します
  while(1){
    //if(!i2clcd_init(0x27)) break;   // 初期化完了ならwhileを抜ける
    if ( i2clcd.begin() ) break;
    // 失敗したら初期化を永遠に繰り返す
  }
  Serial.println("I2CLCD started.");
  
  while(1){
    if ( rtc.begin() ) break;
    // 失敗したら初期化を永遠に繰り返す
  }
  Serial.println("I2C RTC started.");

  nfcreader.begin();
  while (1) {
    if ( nfcreader.GetFirmwareVersion() && nfcreader.getCommandResponse((uint8_t*)tmp) ) {
      break;
    }
    delay(250);
  }
  Serial.print("I2C NFC reader ver. ");
  Serial.print(tmp[0], HEX); 
  Serial.print(" firm. ");
  Serial.print(tmp[1], HEX); 
  Serial.print(" rev. ");
  Serial.print(tmp[2], HEX);
  Serial.print(" supported ");
  Serial.print(tmp[3], BIN);
  if ( !nfcreader.SAMConfiguration() ) {
		Serial.println("....SAMConfiguration failed. Halt.\n");
		while (1);
	}
  Serial.println(", SAM Configured.");
  
  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(USERLED, OUTPUT);
  digitalWrite(USERLED, HIGH);
}


int main (void) {
	long sw, ontime = 0; //, offtime;
  long i;
  char c = 0;
  ISO14443 card;
  
  
  init();

  setup();
  // ---------------------------------------
  
  rtc.updateCalendar();
  rtc.updateTime();
  sprintf(msg, "%02x:%02x:%02x\n%02x/%02x/'%02x\n", 
                rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f, 
                rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.cal>>16&0xff);
  Serial.print(msg);


  i2clcd.print("I was an lpclcd.");
  i2clcd.setCursor(0, 1);	// move to 2nd line
  i2clcd.print("Hi, everybody!");

  sw = millis();
  
  while (1){    /* Loop forever */
    
		if ( nfcreader.InAutoPoll(1, 1, NFCpollingOrder+1, NFCpollingOrder[0]) 
        && nfcreader.getAutoPollResponse(tmp) ) {
				// NbTg, type1, length1, [Tg, ...]
          Serial.print("InAutoPoll: ");
        card.set(tmp[1], tmp+3);
        card.printOn(Serial);
          Serial.println();
    }

    if ( millis() - sw >= 100 ) {
      sw = millis();

      i2clcd.setCursor(0, 1);
      rtc.updateTime();
      sprintf((char*)tmp, "%02x:%02x:%02x", rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f);
      i2clcd.print((char*)tmp);
      sprintf((char*)tmp, " %06d", millis());
      i2clcd.print((char*)tmp);
    }
    
    if ( digitalRead(USERBTN) == LOW ) {
      if ( ontime == 0 ) {
        ontime = millis();
        Serial.println(micros());
      }
    } else /* if ( digitalRead(USERBTN) == HIGH ) */ {
      if (  ontime > 0 && (millis() - ontime >= 5000) ) {
        Serial.println(millis() - ontime);
        rtc.setTime(0x235000);
        rtc.setCalendar(0x235000);
      } else if ( ontime > 0 && (millis() - ontime >= 1000) ) {
        Serial.println(millis() - ontime);
        digitalToggle(LCDBKLT);
      } 
      ontime = 0;
    }
    
    if ( Serial.available() > 0 ) {
      i = strlen(msg);
      while ( Serial.available() > 0 ) {
        c = Serial.read();
        if ( c == '\n' || c == '\r' )
          break;
        msg[i] = c;
        i++;
      }
      msg[i] = 0;
      i2clcd.home();
      i2clcd.print(msg);
      if ( c == '\n' || c == '\r' ) {
        Serial.println(msg);
        for( ; i < 16; i++) 
          i2clcd.print(' ');
        msg[0] = 0;
      }
    }

  }
  
}

/******************************************************************************
**                            End Of File
******************************************************************************/
