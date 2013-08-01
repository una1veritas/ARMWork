/*
 * LPCLDC ported to Keil uV
 *
 */

#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

#include "LPC11Uxx.h"
//#include "systick.h"

#include "armcmx.h"
#include "USARTSerial.h"
#include "I2CBus.h"
#include "i2clcd.h"
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
//#define LCDBKLT PIO1_3
#define LCDBKLT PIO1_3
#define LCDRST  PIO1_25
#define USERLED PIO1_6
#define USERBTN PIO0_1
#define RXD2    PIO0_18
#define TXD2    PIO0_19

//#define LED_SD_BUSY   PIO1_19
#define SYSTICK_DELAY		(SystemCoreClock/100)

volatile uint32_t TimeTick = 0;

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler(void)
{
  TimeTick++;
}

void delaySysTick(uint32_t tick)
{
  uint32_t timetick;

  /* Clear SysTick Counter */
  SysTick->VAL = 0;
  /* Enable the SysTick Counter */
  SysTick->CTRL |= (0x1<<0);

  timetick = TimeTick;
  while ((TimeTick - timetick) < tick);
  
  /* Disable SysTick Counter */
  SysTick->CTRL &= ~(0x1<<0);
  /* Clear SysTick Counter */
  SysTick->VAL = 0;
  return;
}
ST7032i i2clcd(Wire, LCDBKLT, LCDRST);

int main (void) {
  char str[32];
  long sw;
  int i;
  
  SystemInit();
  SystemCoreClockUpdate();
  
  GPIOInit();
  
  // systick initialize
  SysTick_Config( SystemCoreClock / 1000 );
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

    /* Clear SysTick Counter */
  SysTick->VAL = 0;
  /* Enable the SysTick Counter */
  SysTick->CTRL |= (0x1<<0);

  init_timer16_1();
  enable_timer16_1();
  
  // I2C LCD Backlight controll pin
  pinMode(LCDBKLT, OUTPUT);
  digitalWrite(LCDBKLT, LOW);
  
  pinMode(USERBTN, INPUT);

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
    
  Serial.print("Hello!\n");


  i2clcd.print("I'm lpclcd.");
  i2clcd.setCursor(0, 1);	// move to 2nd line
  i2clcd.print("Hi, everybody!");
  for(i = 0; i < 3; i++) {
    delay(750);
    i2clcd.noDisplay();
    delay(250);
    i2clcd.display();
  }
  i2clcd.clear();
  sw = millis();
  
  while (1){    /* Loop forever */
    
    if ( millis() != sw ) {
      sw = millis();

      i2clcd.setCursor(0, 1);
      sprintf(str, " %06d", TimeTick);
      i2clcd.print(str);
    }
    

  }
  
}

/******************************************************************************
**                            End Of File
******************************************************************************/
