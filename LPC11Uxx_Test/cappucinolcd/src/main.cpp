/*
 * LPCLDC ported to Keil uV
 *
 */

#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

#include "LPC11Uxx.h"
//#include "type.h"

//#define DELAY_TIMER16_1
//DELAY_SYSTICK

#include "armcmx.h"
#include "delay.h"

#include "USARTSerial.h"
#include "I2CBus.h"
#include "ST7032i.h"

#include "StringStream.h"

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
#define LPCLCDBKLT PIO1_3
#define CAPPUCINOBKLT PIO0_3
#define LCDRST  PIO1_25
#define USERLED PIO1_6
#define USERBTN PIO0_1
#define RXD2    PIO0_18
#define TXD2    PIO0_19

#define CAPPUCINO_LED_SDBUSY   PIO1_19

ST7032i i2clcd(Wire, LPCLCDBKLT, LCDRST);

int main (void) {
	long sw, mu;
  char str[32];
  int i;
  
  int c;
  uint32 lastread;
  char buf[64];
  StringStream strm(buf, 64);
  
  SystemInit();
  GPIOInit();
  start_delay();
    
  delay(200);
  // I2C LCD Backlight controll pin
  pinMode(LPCLCDBKLT, OUTPUT);
  digitalWrite(LPCLCDBKLT, LOW);
  
  pinMode(USERBTN, INPUT);
  
  Serial.begin(115200);
  Serial.println("Testing serial out.");
  
  Wire.begin();
  if ( Wire.status == FALSE ) 
  	while ( 1 );				/* Fatal error */

  // I2C液晶を初期化します
  if ( ! i2clcd.begin() ) 
    while (1); 

  pinMode(USERLED, OUTPUT);
  digitalWrite(USERLED, HIGH);
    
  i2clcd.print("I was an lpclcd.");
  i2clcd.setCursor(0, 1);	// move to 2nd line
  i2clcd.print("Hi, everybody!");
  for(i = 0; i < 3; i++) {
    delay(500);
    i2clcd.noDisplay();
    delay(500);
    i2clcd.display();
  }
  delay(500);
  i2clcd.clear();
  
  sw = millis();
  
  while (1){    /* Loop forever */
    
    if ( millis() != sw ) {
      mu = micros();
      sw = millis();

      i2clcd.setCursor(0, 0);
      sprintf(str, " %08ld", mu);
      i2clcd.print(str);
      i2clcd.setCursor(0, 1);
      sprintf(str, " %08ld", sw);
      i2clcd.print(str);
      
      while ( digitalRead(USERBTN) == LOW);
    }
    
    
    if ( Serial.available() ) {
      strm.flush();
      lastread = millis();
      do {
        while ( !Serial.available() ) {
          if ( millis() > lastread + 10000 ) break;
        }
        c = Serial.read();
        if ( c == -1 ) 
          break;
        if ( c == '\n' || c == '\r' || c == 0 )
          break;
        strm.write(c);
        lastread = millis();
      } while ( millis() < lastread + 10000 );
      Serial << "length = " << strm.length() << nl;
      if ( strm.length() > 0 ) {
        Serial.print("Request: ");
        Serial.println(strm);
        //
        Serial << "strm = " << '"' << strm << '"' << nl;
        Serial << "buf = " << '"' << buf << '"' << nl;
      }
    }

  }
  
}

/******************************************************************************
**                            End Of File
******************************************************************************/
