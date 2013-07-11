/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

//#include <cr_section_macros.h>
//#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
//__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

// TODO: insert other include files here
#include "lpc_types.h"
#include "i2c.h"
//#include "utils.h"
#include "lpc17xx_gpio.h"
#include "delay.h"
#include "debug_frmwrk.h"

uint32_t int_flag=0;
// TODO: insert other definitions and declarations here

const uint8_t menu[] =
"********************************************************************************\n\r"
"NGX Technologies \n\r"
"I2C EEPROM demo \n\r"
"\t - MCU: LPC1768 \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: USB VCOM \n\r"

"This example used to configure the\n\r "
" I2C EEPROM \n\r"
"********************************************************************************\n\r";

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Print menu
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	_DBG(menu);
}

/*****************************************************************************
** Function name:		user_switch_init
**
** Descriptions:		Initialize external interrupt pin and
**						install interrupt handler
**
** parameters:			None
** Returned value:		true or false, return false if the interrupt
**						handler can't be installed to the VIC table.
**
*****************************************************************************/
void user_switch_init( void )
{

	GPIO_SetDir(1, 1<<25,0);
		if(!(GPIO_ReadValue(1)&1<<25))
		{
			int_flag=1;

		}

		else
		{
			int_flag = 0;
		}
}


void test_i2c_at24c256_flash(void)
{
  int i=0;

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

		 
	 	 	_DBG("I2C EEPROM PASS \n\t");
    }
  else
   {
	    	_DBG("I2C EEPROM FAIL\n\t");
   }
 }

/* Main Program */

int main (void)
{

    SystemInit();                         /* initialize clocks */
    debug_frmwrk_init();
    do{
      user_switch_init();
      }while(int_flag != 1);

  	print_menu();

    i2c_lpc_init(I2C_SPEED_100);
    test_i2c_at24c256_flash();
    while(1);
}
