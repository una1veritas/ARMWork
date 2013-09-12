/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/

#include "LPC11Uxx.h"
#include "type.h"

#include "integer.h"
#include "ff.h"

#include "armcmx.h"
#include "USARTSerial.h"
#include "I2Cbus.h"
#include "ST7032i.h"
#include "RTC.h"
#include "SPI.h"
#include "SPISRAM.h"

#include "PWM0Tone.h"

#define LPCLCD
#if defined LPCLCD
#include "lpclcd.h"
#define LED_SDBUSY LED_USER
#define SD_DETECT  PIO1_4
#elif defined CAPPUCCINO
#include "cappuccino.h"
#endif

#ifdef _LPCXPRESSO_CRP_
#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif

//SPIBus SPI0(&SPI0Def, PIO1_29, PIO0_8, PIO0_9, PIO0_2); // sck, miso, mosi, cs
//SPIBus SPI1(&SPI1Def, PIO1_20, PIO1_21, PIO1_22, PIO1_23); // sck, miso, mosi, cs

void sd_test(void);

ST7032i lcd(Wire, LED_LCDBKLT);
RTC rtc(RTC::ST_M41T62);
//SPIBus SPI1(&SPI1Def, PIO1_20, PIO1_21, PIO1_22, PIO1_23); // sck, miso, mosi, cs
//SPISRAM sram(SPI1, PIO1_23, SPISRAM::BUS_MBITS);


int main(void) {
	int i;

	SystemInit();
	GPIOInit();
  start_delay();

	// GPIO, delay, PWM0Tone test.
	pinMode(LED_SDBUSY, OUTPUT);
  for (i = 0; i < 3; i++) {
    digitalWrite(LED_SDBUSY, HIGH);
    delay(100);
    digitalWrite(LED_SDBUSY, LOW);
    delay(100);
  }
  digitalWrite(LED_SDBUSY, HIGH);
  PWM0_tone(PIO1_13, 1320, 100);
	PWM0_tone(PIO1_13, 1540, 100);
  delay(500);
  digitalWrite(LED_SDBUSY, LOW);
  //
  
	USART_init(&usart, RXD2, TXD2);
//  USART_init(&usart, RXD_C, TXD_C);
	USART_begin(&usart, 115200);
  USART_puts(&usart, "\nHello, nice to see you!\n");

  Wire.begin();
  lcd.begin();
  lcd.backlightHigh();
  lcd.print("Let's start LCD!");

  rtc.begin();
  rtc.update();
  lcd.setCursor(0,1);
  lcd.print(rtc.time, HEX);
  lcd.print(" ");
  lcd.print(rtc.cal, HEX);
  Serial.print("Current time printed on LCD: ");
  Serial.println(rtc.time, HEX);
//	 下記は不要な部分はコメントアウトしてお試しください。

//  SPI1.begin();
//  sram.begin();
 /*
  * SDカードのデモ（エンドレス）
  */
  Serial.print("result of get_fattime: ");
  Serial.println(get_fattime(), HEX);
  
  //SPI_init(&SPI0Def, PIO1_29, PIO0_8, PIO0_9, SSP_CS0);
  SPI0.begin();
  if ( digitalRead(SD_DETECT) == HIGH ) {
    Serial.println("SD slot is empty.");
  } else {
    Serial.println("Card is in SD slot.");
  }

	sd_test();
/*
 * i2C液晶のテスト（エンドレス）
 */
  
	  while (1){                                /* Loop forever */
		  digitalWrite(LED_SDBUSY, LOW);
		  delay(500);
		  digitalWrite(LED_SDBUSY, HIGH);
		  delay(500);

		  rtc.update();
      lcd.setCursor(0,1);
      lcd.print(rtc.time, HEX);
      lcd.print(" ");
      lcd.print(rtc.cal, HEX);
      lcd.print("   ");
	  }

//	return 0 ;
}
