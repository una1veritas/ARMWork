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
	long sw, ontime = 0; //, offtime;
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
  pinMode(LCDBKLT, OUTPUT);
  digitalWrite(LCDBKLT, HIGH);

  pinMode(USERBTN, INPUT);
  
  /* NVIC is installed inside UARTInit file. */
  USART_init(&usart, PIO0_18, PIO0_19);
  USART_begin(&usart, 115200);

  if ( I2C_init(&i2c, (uint32_t)I2CMASTER ) == FALSE ){	/* initialize I2c */
  	while ( 1 );				/* Fatal error */
  }

  // I2C液晶を初期化します
  while(1){
    if(!i2clcd_init(0x27)) break;   // 初期化完了ならwhileを抜ける
    // 失敗したら初期化を永遠に繰り返す
  }

  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(USERLED, OUTPUT);
  digitalWrite(USERLED, HIGH);
    
  USART_print(&usart, "Hello!\n");
  
  tmp[0] = 0;
  I2C_request(&i2c, RTC_ADDR, (uint8*)tmp, 1);
  I2C_receive(&i2c, RTC_ADDR, (uint8*)tmp, 8);
  sprintf(str, "%02x:%02x:%02x\n%s\n%02x/%02x/'%02x\n", tmp[3]&0x3f, tmp[2]&0x7f, tmp[1]&0x7f, day[tmp[4]&0x07], tmp[6]&0x1f, tmp[5]&0x3f, tmp[7]);
  USART_print(&usart, (uint8_t*)str);

  
  i2clcd_puts((uint8_t*)"lpclcd");
  i2clcd_cursor(1, 0);	// move to 2nd line
  i2clcd_puts((uint8_t*)"LPCLCD Module");

  sw = millis();
  
  while (1){    /* Loop forever */
        
    if ( millis() - sw >= 100 ) {
      sw = millis();

      i2clcd_cursor(1,0);
      tmp[0] = 0;
      I2C_read(&i2c, RTC_ADDR, (uint8*)tmp, 1, 8);
      sprintf(str, "%02x:%02x:%02x", tmp[3]&0x3f, tmp[2]&0x7f, tmp[1]&0x7f);
      i2clcd_puts((uint8_t*)str);
      sprintf(str, " %02x/%02x %02x", tmp[6]&0x1f, tmp[5]&0x3f, tmp[7]);
      i2clcd_puts((uint8_t*)str);
    }
    
    if ( digitalRead(USERBTN) == LOW ) {
      if ( ontime == 0 ) 
        ontime = millis();
    } else /* if ( digitalRead(USERBTN) == HIGH ) */ {
      if (  ontime > 0 && (millis() - ontime >= 5000) ) {
        tmp[0] = 1;
        tmp[1] = 0x00;
        tmp[2] = 0x41;
        tmp[3] = 0x23;
        tmp[4] = 0x04;
        tmp[5] = 0x20;
        tmp[6] = 0x06;
        tmp[7] = 0x13;
        I2C_write(&i2c, RTC_ADDR, (uint8_t*)tmp, 8);
      } else if ( ontime > 0 && (millis() - ontime >= 1000) ) {
        digitalToggle(LCDBKLT);
      } 
      ontime = 0;
    }
    
    if ( USART_available(&usart) > 0 ) {
      i = strlen(message);
      while ( USART_available(&usart) > 0 ) {
        c = USART_read(&usart);
        message[i] = c;
        if ( c == '\n' || c == '\r' )
          break;
        i++;
      }
      message[i] = 0;
      i2clcd_cursor(0,0);
      i2clcd_puts((uint8_t *)message);
      if ( c == '\n' || c == '\r' ) {
        if ( message[0] == 't' ) {
          
 //         USART_prints(Revert);
        }
        message[0] = 0;
      }
    }

  }
  
}

/******************************************************************************
**                            End Of File
******************************************************************************/
