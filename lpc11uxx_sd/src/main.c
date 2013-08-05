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
	long cn;

	SystemInit();
	GPIOInit();
  start_delay();

  PWM0_tone(PIO1_13, 1320, 100);
	PWM0_tone(PIO1_13, 1540, 100);

	  // PIO1_6 USR LED
	pinMode(USERLED, OUTPUT);
	digitalWrite(USERLED, HIGH);
  
	USART_init(&usart, PIO1_26, PIO1_27);
//  USART_init(&usart, PIO0_18, PIO0_19);
	USART_begin(&usart, 115200);	
  USART_puts(&usart, "Hello, its nice to see you!");

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
	  cn = 0;
	  while (1){                                /* Loop forever */
		  digitalWrite(LED_SD_BUSY, LOW);
		  delay(500);
		  digitalWrite(LED_SD_BUSY, HIGH);
		  delay(500);

		  //i2clcd_backlight(cn &1);
		  //i2c_cmd(0x80+7);
	  }

//	return 0 ;
}

