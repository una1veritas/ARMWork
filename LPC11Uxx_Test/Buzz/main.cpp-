/*
 * LPCLDC ported to Keil uV
 *
 */

#include <stdio.h>
#include <string.h>

#include "LPC11Uxx.h"
//#include "type.h"

#include "armcmx.h"
#include "USARTSerial.h"
#include "I2CBus.h"
#include "ST7032i.h"

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

ST7032i i2clcd(Wire, LCDBKLT, LCDRST);
/*
uint8_t i2clcd_data(uint8_t d) {
  return (uint8_t) i2clcd.write(d);
}
*/

int main (void) {
  
  SystemInit();
  GPIOInit();
  
  init_timer16_1();
  enable_timer16_1();

  pinMode(USERBTN, INPUT);
  
  /* NVIC is installed inside UARTInit file. */
  //USART_init(&usart, PIO0_18, PIO0_19);
  Serial.begin(115200);

  Wire.begin();
  if ( Wire.status == FALSE ){	/* initialize I2c */
  	while ( 1 );				/* Fatal error */
  }

  // I2C液晶を初期化します
  while(1){
    //if(!i2clcd_init(0x27)) break;   // 初期化完了ならwhileを抜ける
    if ( i2clcd.begin() ) break;
    // 失敗したら初期化を永遠に繰り返す
  }

  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(USERLED, OUTPUT);
  digitalWrite(USERLED, HIGH);
    
  i2clcd.setCursor(0, 1);	// move to 2nd line
  i2clcd.print("Hi, everybody!");
  
  while (1){    /* Loop forever */
    

  }
  
}

/******************************************************************************
**                            End Of File
******************************************************************************/
