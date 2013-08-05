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
#define DELAY_SYSTICK
#include "delay.h"
#include "usart.h"
//#include "i2c.h"
//#include "i2clcd.h"
#include "ff.h"
#include "cappuccino.h"

#ifdef _LPCXPRESSO_CRP_
#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif


void sd_test(void);

int main(void) {
	long cn;

	SystemInit();
	GPIOInit();
  start_systick_delay();
  
	USART_init(&usart, PIO1_17, PIO1_18);
	USART_begin(&usart, 115200);	
  USART_puts(&usart, "Hello, its nice to see you!");

	  // PIO1_6 USR LED
	pinMode(USERLED, OUTPUT);
	digitalWrite(USERLED, HIGH);
/*
	  i2c_puts("LPCcappuccino!");
	  i2c_cmd(0xC0);	// move to 2nd line
	  i2c_puts("StrawberryLinux");
*/

//	 下記は不要な部分はコメントアウトしてお試しください。

/*
 *	SDカードのデモ（エンドレス）
 */
	  sd_test();
/*
 * i2C液晶のテスト（エンドレス）
 */
	  cn = 0;
	  while (1){                                /* Loop forever */
		  digitalWrite(PIO1_19, LOW);
		  delay(500);
		  digitalWrite(PIO1_19, HIGH);
		  delay(500);

		  //i2clcd_backlight(cn &1);
		  //i2c_cmd(0x80+7);
	  }

	return 0 ;
}


