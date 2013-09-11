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
#include "gpio.h"
#include "xprintf.h"
#include "systick.h"
#include "uart.h"
#include "i2c.h"
#include "i2clcd.h"
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

	  // systick initialize
	  SysTick_Config( SystemCoreClock / 1000 );
	  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

	  // initialize xprintf
	  // xprintf関数の出力先をUARTにします。
//	  xfunc_out = (void(*)(unsigned char))i2c_data;
	  xfunc_out = (void(*)(unsigned char))uart_putc;

	  /*
	   * UARTを初期化します。ポートはPIO1_13を指定します。全部で４種類指定が可能
	   */
	  UARTInit(115200, 0x127); //0x118);	// PIO1_13

	  if ( I2CInit( (uint32_t)I2CMASTER ) == FALSE ){	/* initialize I2c */
	  	while ( 1 );				/* Fatal error */
	  }
	  Systick_Start();
	  while(1){
	        if(!i2clcd_init(0x27)) break;
	        if(Systick_Gettime()>3000) break;
	  }
	  Systick_Stop();

	  // PIO1_6 USR LED
	  OUTPUT(1, 19);
	  HIGH(1, 19);

	  i2c_puts("LPCcappuccino!");
	  i2c_cmd(0xC0);	// move to 2nd line
	  i2c_puts("StrawberryLinux");


//	 下記は不要な部分はコメントアウトしてお試しください。

/*
 * L6470ステッピングドライバを回すデモ（エンドレス）
 */
//	  l6470_init();
//	  l6470_demo();

/*
 *	SDカードのデモ（エンドレス）
 */
	  sd_test();
/*
 * i2C液晶のテスト（エンドレス）
 */
	  cn = 0;
	  while (1){                                /* Loop forever */
		  LOW(1, 19);
		  wait_ms(500);
		  HIGH(1, 19);
		  wait_ms(500);

		  i2clcd_backlight(cn &1);
		  i2c_cmd(0x80+7);
		  xprintf("%9d", cn++);
	  }

	return 0 ;
}


