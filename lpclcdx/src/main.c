/*
 * LPCLDC ported to Keil uV
 *
 */

#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"
#include "gpio.h"
#include "xprintf.h"
#include "systick.h"
#include "uart.h"
#include "i2c.h"
#include "i2clcd.h"

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
  
  SystemInit();
  GPIOInit();
  
  // systick initialize
  SysTick_Config( SystemCoreClock / 1000 );
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

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
  UARTInit(115200);

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
    
  UARTputs((uint8_t*)"Hello!\n");

  i2clcd_puts((uint8_t*)"lpclcd");
  i2clcd_cursor(1, 0);	// move to 2nd line
  i2clcd_puts((uint8_t*)"LPCLCD Module");

  cn = 0;

  while (1){    /* Loop forever */
    
    //	GPIOSetBitValue( 1, 6, 0 );
    digitalWrite(PIO1_6, LOW);
    wait_ms(500);

    //	GPIOSetBitValue( 1, 6, 1 );
    digitalWrite(PIO1_6, HIGH);
    wait_ms(500);

    if ( UARTavailable() > 0 ) {
      i = strlen(message);
      while ( UARTavailable() > 0 ) {
        c = UARTread();
        UARTSend((uint8_t*)&c, 1);
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

    i2clcd_cursor(0,7);
    xprintf("%9d", cn++);
    
  }
  

}

/******************************************************************************
**                            End Of File
******************************************************************************/
