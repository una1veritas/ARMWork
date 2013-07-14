/*****************************************************************************
 *   extint.c:  External interrupt API C file for NXP LPC17xx 
 *   Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "lpc17xx.h"
#include "lpc_types.h"
#include "extint.h"
#include "delay.h"
#include "stdint.h"
#include "lpc17xx_gpio.h"
#include "debug_frmwrk.h"

#define TEST_LED (1 << 10)
uint32_t int_flag=0;
// TODO: insert other definitions and declarations here

const char menu[] =
"********************************************************************************\n\r"
"NGX Technologies \n\r"
"extint demo \n\r"
"\t - MCU: LPC1768 \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: USB VCOM \n\r"

"This example used to configure the\n\r "
"external interrupt \n\r"
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

int main (void) 
{
    SystemInit();                         /* initialize clocks */
	debug_frmwrk_init();
	do{
	user_switch_init();
	}while(int_flag != 1);
	print_menu();
    while(1)
	{
    	user_switch_init();
	  if(int_flag == 1)
		{
			_DBG("User input detected\r\n");
		     int_flag = 0;
		}
	}
}

/******************************************************************************
**                            End Of File
******************************************************************************/

