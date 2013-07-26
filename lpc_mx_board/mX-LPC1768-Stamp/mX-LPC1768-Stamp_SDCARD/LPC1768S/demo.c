/*----------------------------------------------------------------------------
 *      Name:    DEMO.C
 *      Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC family microcontroller devices only. Nothing 
 *      else gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "LPC17xx.H"                        /* LPC17xx definitions */
#include "type.h"
#include "demo.h"
#include "lcd.h"
#include "spi.h"
#include "lpc2000_spi.h"





/**
*******************************************************************
	Funtion Name : init_sdcard()

	Description  : 

	Input        : 

	Output       : None

	Note         :
*****************************************************************
*/
U8 init_sdcard(void);
U8 init_sdcard(void)
{
 if(if_initInterface(0)==0)
 return 1;
 else
 return 0;
}
/**
*******************************************************************
	Funtion Name : test_sd_card()

	Description  : 

	Input        : 

	Output       : None

	Note         :
*****************************************************************
*/
void test_sd_card(void);
void test_sd_card(void)
{
  if(init_sdcard())
  {
    //lcd_clear();
    lcd_putstring(LINE2,"SD card - PASS");
  }
  else
  {
    //lcd_clear();
    lcd_putstring(LINE2,"SD card - FAIL");
  }
}

/* Main Program */

int main (void) {

	volatile int i=0,j=0;

	SystemInit();                         /* initialize clocks */
	init_lcd();
	lcd_backlight_on();
	delay(1000);	
	lcd_putstring(0,"NGX TECHNOLOGIES");
  	lcd_putstring(1,"mX-LPC1768 Stamp");
	delay(1000000);
	lcd_clear();

	test_sd_card();
	
	
 	while (1) 
	{ 	  
	}

}

