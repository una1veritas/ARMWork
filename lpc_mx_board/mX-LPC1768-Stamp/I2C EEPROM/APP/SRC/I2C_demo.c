/*----------------------------------------------------------------------------
 *      Name:    DEMO.C
 *      Purpose: USB HID Demo
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

#include "i2c.h"

#define TEST_LED (1 << 2)

void test_i2c_at24c256_flash(void)
{
  char  i=0;
  
  char status=1;
  unsigned char buf[128];

  for(i=0;i<sizeof(buf);i++)
  
    buf[i] = i;
  
  m24xx_write(EEPROM_24XX256, 0 , 0 , (char *)buf , sizeof(buf));

  for(i=0;i<sizeof(buf);i++)
    buf[i] = 0;

  m24xx_read(EEPROM_24XX256, 0 , 0 , (char *)buf , sizeof(buf));

  for(i=0;i<sizeof(buf);i++)
  {
    if(buf[i] != i)
    {
      status = 0;
      break;
    }    
  }

  if(status == 1)
  {
    LPC_GPIO0->FIOSET = TEST_LED;
	
  }
  else
  {
    LPC_GPIO0->FIOCLR = TEST_LED;
  }
}

/* Main Program */

int main (void) 
{
  int i=0,j=0,k=0;

  SystemInit();                         /* initialize clocks */

  LPC_GPIO0->FIODIR |= TEST_LED;
  
  for(i=0; i < 5 ;i++ )
  {
    LPC_GPIO0->FIOSET = TEST_LED;
    for(j=0;j<1000000;j++);
    LPC_GPIO0->FIOCLR  = TEST_LED;
    for(k=0;k<1000000;k++);
  }

  i2c_lpc_init(I2C_SPEED_100);
  test_i2c_at24c256_flash();
  while(1);
}
