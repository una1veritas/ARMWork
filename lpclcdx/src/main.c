/*
 * LPCLDC ported to Keil uV
 *
 */

#include <stdio.h>
#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"
#include "gpio.h"
#include "xprintf.h"
#include "systick.h"
#include "usart.h"
#include "i2c.h"
#include "i2clcd.h"
#include "timer32.h"

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


int main (void) {
	long cn;
  int i;
  char c = 0;
  char message[16] = "";
  char tmp[32];
  
  SystemInit();
  GPIOInit();
  
  // systick initialize
  SysTick_Config( SystemCoreClock / 1000 );
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

  init_timer32(Timer0.num, TIME_INTERVAL);
  enable_timer32(Timer0.num);

  // initialize xprintf
  xfunc_out = (void(*)(unsigned char))i2clcd_data;
  
  // I2C LCD Backlight controll pin
  /*
  GPIOSetDir(1, 3, 1 );
  GPIOSetBitValue( 1, 3, 0);
  */
  pinMode(PIO1_3, OUTPUT);
  digitalWrite(PIO1_3, LOW);

  pinMode(PIO0_1, INPUT);
  
  /* NVIC is installed inside UARTInit file. */
  USART_init(&uart, PIO0_18, PIO0_19);
  USART_begin(&uart, 115200);

  if ( I2CInit( (uint32_t)I2CMASTER ) == FALSE ){	/* initialize I2c */
  	while ( 1 );				/* Fatal error */
  }

  // I2C液晶を初期化します
  while(1){
    if(!i2clcd_init(0x27)) break;   // 初期化完了ならwhileを抜ける
    // 失敗したら初期化を永遠に繰り返す
  }

  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(PIO1_6, OUTPUT);
  digitalWrite(PIO1_6, HIGH);
    
  USART_print(&uart, "Hello!\n");

  i2clcd_puts((uint8_t*)"lpclcd");
  i2clcd_cursor(1, 0);	// move to 2nd line
  i2clcd_puts((uint8_t*)"LPCLCD Module");

  cn = 0;

  while (1){    /* Loop forever */
    
    if ( millis() - cn >= 500 ) {
      digitalToggle(PIO1_6);
      cn = millis();
      i2clcd_cursor(0,0);
      sprintf(tmp, "%8d", cn );
      i2clcd_puts((uint8_t *)tmp);
    }
    
    if ( USART_available(&uart) > 0 ) {
      i = strlen(message);
      while ( USART_available(&uart) > 0 ) {
        c = USART_read(&uart);
        USART_write(&uart, c);
        if ( c == '\n' || c == '\r' )
          break;
        message[i++] = c;
      }
      message[i] = 0;
      i2clcd_cursor(1,0);
      for(i = 0; message[i] && i < 16; i++) {
        i2clcd_data(message[i]);
      }
      for( ; i < 16; i++) {
        i2clcd_data(' ');
      }
      if ( c == '\n' || c == '\r' ) {
        message[0] = 0;
      }
    }

    // cn++;
    
  }
  

}

/******************************************************************************
**                            End Of File
******************************************************************************/
