/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/
#include "lcd.h"
#include "type.h"
#include "types.h"
#include "sd.h"
#include "utils.h"


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
void test_sd_card(void)
{

  if(init_sdcard())
  {
  //  lcd_clear();
    lcd_putstring(LINE2,"SD card - PASS");

  }
  else
  {
   // lcd_clear();
    lcd_putstring(LINE2,"SD card - FAIL");

  }
}

int main (void)
{
  init_lcd();
  lcd_clear();
  lcd_putstring(LINE1,"NGX TECHNOLOGIES");
  delay(10000);
  test_sd_card();
  while (1)
  {
  }
}

