/****************************************************************************
 *   $Id:: EEPROMtest.c 6194 2011-01-17 22:20:56Z nxp28548                  $
 *   Project: NXP LPC11xx EEPROM example
 *
 *   Description:
 *     This file contains EEPROM test modules, main entry, to test EEPROM APIs.
 *
 ****************************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.

* Permission to use, copy, modify, and distribute this software and its 
* documentation is hereby granted, under NXP Semiconductors� 
* relevant copyright in the software, without fee, provided that it 
* is used in conjunction with NXP Semiconductors microcontrollers.  This 
* copyright, permission, and disclaimer notice must appear in all copies of 
* this code.

****************************************************************************/
#include "LPC11Uxx.h"
#include "uart.h"
#include "eepromAPI.h"
#include <stdio.h>
#include <string.h>

#ifndef EE_SIZE
#define EE_SIZE 4032
#endif

static void menu(void);
static void eefill(uint8_t fillPattern);
static void eeDump(void);

#define MENU_SIZE 6
const uint8_t* menu_array[MENU_SIZE] = {
(uint8_t *)"\r\nLPC11Axx EEPROM Test\r\n",
(uint8_t *)"'d' - Display Contents of EEPROM (Little Endian)\r\n",
(uint8_t *)"'f' - Fill EEPROM with 0xFF\r\n",
(uint8_t *)"'0' - Fill EEPROM with 0x00\r\n",
(uint8_t *)"'c' - Fill EEPROM with incrementing bytes\r\n",
(uint8_t *)"'t' - Fill EEPROM with timestamps\r\n\r\n>"};

typedef enum{
	CMD_DUMP	= 'd',
	CMD_FF		= 'f',
	CMD_00		= '0',
	CMD_INC		= 'c',
	CMD_TIME	= 't'
}commandEnum;

int main (void)
{	    		
	uint8_t c;
	uint32_t i, hold;
	char cmd;

	SystemCoreClockUpdate();
    
	UARTInit(9600);
	NVIC_DisableIRQ(UART_IRQn);

	/* Setup Timer to performance metrics */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);

	menu();
	/* Loop forever */
	while (1) 
	{				
		cmd = get_key();		
		print_string((uint8_t *)"\r\n");
		LPC_CT32B0->TCR = 2;
		switch(cmd)
		{
			case CMD_FF:
				eefill(0xFF);
				eeDump();
				break;
	
			case CMD_00:
				eefill(0x00);
				eeDump();				
				break;
	
			case CMD_INC:
				for (i=0,c=0x0; i<EE_SIZE; i+=4)
				{
					hold = c++;
					hold |= c++<<8;
					hold |= c++<<16;
					hold |= c++<<24;
					writeEEPROM( (uint8_t*) i, (uint8_t*) &hold, 4 );
				}
				eeDump();
				break;

			case CMD_TIME:				
				for (i=0; i<EE_SIZE; i+=4)
				{
					LPC_CT32B0->TCR = 0;
					hold = LPC_CT32B0->TC;
					LPC_CT32B0->TCR = 1;
					writeEEPROM( (uint8_t*) i, (uint8_t*) &hold, 4 );					
				}
				LPC_CT32B0->TCR = 2;
				eeDump();
				break;
						 	
			case CMD_DUMP:	
				eeDump();		
				break;
	
			/* Default case whenever an incorrect cmd has been entered */
			default:
				print_string((uint8_t *)"Invalid command!\r\n\r\n");
				break;
	
		}
		menu();
	}
}

void menu(void)
{
	uint32_t i;
	for (i=0; i<MENU_SIZE; i++)
	{
		print_string( (uint8_t*) menu_array[i] );
	}
}

void eefill(uint8_t fillPattern)
{
	uint32_t i,j,z;
	z 	= (	fillPattern<<24 | 
			fillPattern<<16 |
			fillPattern<<8	|
			fillPattern<<0 );

	for (i=0; i<EE_SIZE; i+=16)
	{
		for (j=0; j<16; j+=4)
		{
			writeEEPROM( (uint8_t*) i+j, 
						 (uint8_t*) &z, sizeof(z));
		}
	}		
}

void eeDump(void)
{
	uint32_t i,j,z;

	char valBuff[12];
	char lineBuff[80];

	for (i=0; i<EE_SIZE; i+=16)
	{
		sprintf(lineBuff,"0x%03X ", i);
		for (j=0; j<16; j+=4)
		{
			readEEPROM( (uint8_t*) i+j, 
						(uint8_t*) &z, sizeof(z));									
			sprintf(valBuff,"%08X ", z);
			strcat(lineBuff,valBuff);
		}
		strcat(lineBuff,"\r\n");
		print_string((uint8_t*) lineBuff);
	}		
}
