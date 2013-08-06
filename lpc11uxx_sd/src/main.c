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
#include "armcmx.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
//#include "i2c.h"
//#include "i2clcd.h"
#include "spi.h"
#include "ff.h"

#include "PWM0Tone.h"

#include "cappuccino.h"

#ifdef _LPCXPRESSO_CRP_
#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif

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
  
	USART_init(&usart, RXD_A, TXD_A);
//  USART_init(&usart, RXD_C, TXD_C);
	USART_begin(&usart, 115200);
  USART_puts(&usart, "\nHello, \nits nice to see you!\n");

/*
	  i2c_puts("LPCcappuccino!");
	  i2c_cmd(0xC0);	// move to 2nd line
	  i2c_puts("StrawberryLinux");
*/

//	 下記は不要な部分はコメントアウトしてお試しください。

/*
 *	SDカードのデモ（エンドレス）
 */
  SPI_init(&SPI0Def, PIO1_29, PIO0_8, PIO0_9, SSP_CS0);

	sd_test();
/*
 * i2C液晶のテスト（エンドレス）
 */
	  while (1){                                /* Loop forever */
		  digitalWrite(LED_SDBUSY, LOW);
		  delay(500);
		  digitalWrite(LED_SDBUSY, HIGH);
		  delay(500);

		  //i2clcd_backlight(cn &1);
		  //i2c_cmd(0x80+7);
	  }

//	return 0 ;
}

