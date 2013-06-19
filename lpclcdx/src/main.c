/*
 * LPCLDC ported to Keil uV
 *
 */

#include <stdio.h>
#include <stdlib.h>
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

#define RTC_ADDR 0xd0

char day[7][4] = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat"
  };

int main (void) {
	long cn;
  long i;
  char c = 0;
  char str[32];
  char message[32];
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
  pinMode(PIO1_3, OUTPUT);
  digitalWrite(PIO1_3, LOW);

  pinMode(PIO0_1, INPUT);
  
  /* NVIC is installed inside UARTInit file. */
  USART_init(&uart, PIO0_18, PIO0_19);
  USART_begin(&uart, 115200);

  if ( I2C_init(&i2c, (uint32_t)I2CMASTER ) == FALSE ){	/* initialize I2c */
  	while ( 1 );				/* Fatal error */
  }

  tmp[0] = 0;
  I2C_read(&i2c, RTC_ADDR, (uint8*)tmp, 1, 8);
  for(i = 0; i < 8; i++) {
    sprintf(str, "%02x", tmp[8-i-1]);
    USART_print(&uart, str);
  }
  USART_print(&uart, ";\n");

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
    
    if ( millis() - cn >= 100 ) {
      digitalToggle(PIO1_6);
      cn = millis();
      
      i2clcd_cursor(1,0);
      tmp[0] = 0;
      I2C_read(&i2c, RTC_ADDR, (uint8*)tmp, 1, 8);
      sprintf(str, "%02x:%02x:%02x", tmp[3]&0x3f, tmp[2]&0x7f, tmp[1]&0x7f);
      i2clcd_puts((uint8_t*)str);
      sprintf(str, " %02x/%02x %02x", tmp[6]&0x1f, tmp[5]&0x3f, tmp[7]);
      i2clcd_puts((uint8_t*)str);
    }
    
    if ( USART_available(&uart) > 0 ) {
      i = strlen(message);
      while ( USART_available(&uart) > 0 ) {
        c = USART_read(&uart);
        message[i] = c;
        if ( c == '\n' || c == '\r' )
          break;
        i++;
      }
      message[i] = 0;
      i2clcd_cursor(0,0);
      i2clcd_puts(message);
      if ( c == '\n' || c == '\r' ) {
        if ( message[0] == 't' ) {
          
 //         USART_prints(Revert);
        }
        message[0] = 0;
      }
    }

    // cn++;
    
  }
  

}

/******************************************************************************
**                            End Of File
******************************************************************************/
