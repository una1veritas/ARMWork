/*
 * LPCLDC ported to Keil uV
 *
 */

#include "LPC11Uxx.h"
#include "type.h"
#include "gpio.h"
#include "xprintf.h"
#include "systick.h"
#include "uart.h"
#include "i2c.h"
#include "i2clcd.h"

//#include <cr_section_macros.h>
//#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
//__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;



/*******************************************************************************
**   Main Function  main()
*******************************************************************************/
int main (void)
{
	long cn;

  SystemInit();
  GPIOInit();
  
  // systick initialize
  SysTick_Config( SystemCoreClock / 1000 );
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

  // initialize xprintf
  xfunc_out = (void(*)(unsigned char))i2c_data;
  
  // I2C LCD Backlight controll pin
  GPIOSetDir(1, 3, 1 );
  GPIOSetBitValue( 1, 3, 0);

  /* NVIC is installed inside UARTInit file. */
  UARTInit(115200);

  if ( I2CInit( (uint32_t)I2CMASTER ) == FALSE ){	/* initialize I2c */
  	while ( 1 );				/* Fatal error */
  }

  // I2C液晶を初期化します
  while(1){
        if(!i2clcd_init(0x27)) break;   // 初期化完了ならwhileを抜ける
        // 失敗したら初期化を永遠に繰り返す
  }

  // PIO1_6 USR LED
  GPIOSetDir(1, 6, 1 );
  GPIOSetBitValue( 1, 6, 1);

  i2c_puts((uint8_t*)"lpclcd");
  i2c_cmd(0xC0);	// move to 2nd line
  i2c_puts((uint8_t*)"Smart LCD Module");

  cn = 0;

  while (1){                                /* Loop forever */

	GPIOSetBitValue( 1, 6, 0 );
        wait_ms(250);
	GPIOSetBitValue( 1, 6, 1 );
        wait_ms(250);


        i2c_cmd(0x80+7);
        xprintf("%9d", cn++);
  }
  

}

/******************************************************************************
**                            End Of File
******************************************************************************/
