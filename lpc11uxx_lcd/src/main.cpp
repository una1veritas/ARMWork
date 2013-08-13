/*
 * LPCLDC ported to Keil uV
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"

#include "armcmx.h"
#include "USARTSerial.h"
#include "I2CBus.h"
#include "i2clcd.h"
#include "ST7032i.h"
#include "I2CRTC.h"

#include "cappuccino.h"

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

ST7032i i2clcd(Wire, LED_LCDBKLT, LCDRST);
I2CRTC rtc(Wire, I2CRTC::CHIP_M41T62);

int task_serial(void);
char rxbuff[64];
int rxindex = 0;

int main (void) {
  long ontime = 0; //, offtime;
  uint32_t val;
  char str[32];
  char buf[4];
  char * ptr;
//  int len;
  boolean laststate;
  
  struct {
    uint32_t master;
    uint32_t rtc;
    uint32_t serial;
    uint32_t button;
  } task = { 0, 0, 0, 0 };
  
  SystemInit();
  GPIOInit();
  
  // systick initialize
  start_delay();
  
  // I2C LCD Backlight controll pin
  pinMode(LED_LCDBKLT, OUTPUT);
  digitalWrite(LED_LCDBKLT, LOW);

  pinMode(SW_USERBTN, INPUT);
  
  /* NVIC is installed inside UARTInit file. */
  //USART_init(&usart, PIO0_18, PIO0_19);
  Serial.begin(115200);
  rxindex = 0;
  
  Wire.begin();
  if ( Wire.status == FALSE ){	/* initialize I2c */
  	while ( 1 );				/* Fatal error */
  }

  // I2C液晶を初期化します
  while(1){
    //if(!i2clcd_init(0x27)) break;   // 初期化完了ならwhileを抜ける
    if ( i2clcd.begin() ) break;
    // 失敗したら初期化を永遠に繰り返す
  }

  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(LED_SDBUSY, OUTPUT);
  digitalWrite(LED_SDBUSY, HIGH);
  pinMode(SW_SDDETECT, INPUT);
  laststate = digitalRead(SW_SDDETECT);
    
  Serial.print("Hello!\n");
  
  rtc.begin();
  rtc.update();
//  sprintf(str, "%02x:%02x:%02x\n%s\n%02x/%02x/'%02x\n", rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f, 
//      day[rtc.cal&0x07], rtc.cal>>16&0x1f, rtc.cal>>8&0x3f, rtc.cal>>16);
  Serial.println(rtc.time, HEX);
  Serial.println(rtc.cal, HEX);
  //Serial.print(str);

  i2clcd.print("I was an lpclcd.");
  i2clcd.setCursor(0, 1);	// move to 2nd line
  i2clcd.print("Hi, everybody!");
  
  task.master = millis();
  //
  while (1){    /* Loop forever */
    
    if ( task.master != millis() ) {
      if ( task.rtc ) 
        task.rtc--;
      if ( task.button )
        task.button--;
      if ( task.serial )
        task.serial--;
    }
    
    if ( !task.rtc ) {
      rtc.update();
      i2clcd.setCursor(0, 0);
      sprintf(str, "%s %02x/%02x       ", 
          rtc.copyNameOfDay(buf, rtc.dayOfWeek()), rtc.cal>>8&0x1f, rtc.cal&0x3f );
      i2clcd.print(str);
      i2clcd.setCursor(0, 1);
      sprintf(str, "%02x:%02x:%02x        ", rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f);
      i2clcd.print(str);
      //
      task.rtc = 33;
    }
    
    if ( !task.button ) {
      if ( laststate != digitalRead(SW_SDDETECT) ) {
        laststate = digitalRead(SW_SDDETECT);
        if ( laststate ) {
          Serial.println("SD DETECT is HIGH");
          digitalWrite(LED_SDBUSY, HIGH);
        } else {
          Serial.println("SD DETECT is LOW");
          digitalWrite(LED_SDBUSY, LOW);
        }
      }
      if ( digitalRead(SW_USERBTN) == LOW ) {
        if ( ontime == 0 ) {
          ontime = millis();
          Serial.println(micros());
        }
      } else /* if ( digitalRead(USERBTN) == HIGH ) */ {
        if ( ontime > 0 && (millis() - ontime >= 1000) ) {
          Serial.println(millis() - ontime);
          digitalToggle(LED_LCDBKLT);
        } 
        ontime = 0;
      }
      //
      task.button = 25;
    }
    
    if ( !task.serial ) {
      if ( task_serial() ) {
        Serial.print("'");
        Serial.print(rxbuff);
        Serial.print("' ");
        //
        rxindex = 0;
        //
        ptr = rxbuff;
        if ( strncmp(ptr, "T", 1) == 0 || strncmp(ptr, "t", 1) == 0 ) {
          strncpy(str,ptr,1);
          str[1] = 0;
          Serial.println(str);
          //
          val = strtol(ptr+1, 0, 16);
          if ( val > 0 ) {
            rtc.setTime(val);
          }
          Serial.print("val = ");
          Serial.println(val, HEX);
        } else if ( strncmp(ptr, "C", 1) == 0 || strncmp(ptr, "c", 1) == 0 ) {
          strncpy(str,ptr,1);
          str[1] = 0;
          Serial.println(str);
          //
          val = strtol(ptr+1, 0, 16);
          if ( val > 0 ) {
            rtc.setCalendar(val);
          }
          Serial.print("val = ");
          Serial.println(val, HEX);
        }
        rtc.update();
        Serial.print("time = ");
        Serial.print(rtc.time, HEX);
        Serial.print(", calendar = ");
        Serial.print(rtc.cal, HEX);
        Serial.println();
      }
      //
      task.serial = 3;
    }
  }
  
}

int task_serial(void) {
  int c = 0;

  if ( !Serial.available() ) 
    return 0;

  while ( Serial.available() > 0 ) {
    c = Serial.read();
    rxbuff[rxindex] = c;
    if ( c == '\n' || c == '\r' || c == 0 )
      break;
    rxindex++;
  }
  rxbuff[rxindex] = 0;
  
  if ( (c == '\n' || c == '\r') && rxindex > 0 ) 
    return rxindex;
  
  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
