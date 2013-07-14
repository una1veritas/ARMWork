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

#include "lpc17xx_gpio.h"
#include "spi.h"
#include "sd.h"
#include "lpc1768_spi.h"
#include "debug_frmwrk.h"
#define TEST_LED (1 << 10)
#define SECTOR_SIZE 512
uint8_t buf[SECTOR_SIZE]={0};
uint32_t int_flag=0;
 const char menu[] =
"********************************************************************************\n\r"
"NGX Technologies \n\r"
"sdcard demo \n\r"
"\t - MCU: LPC1768 \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: USB VCOM \n\r"

"This example used to configure the\n\r "
"sdcard \n\r"
"********************************************************************************\n\r";

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



/**
*******************************************************************
	Funtion Name : test_sd_card()

	Description  : 

	Input        : 

	Output       : None

	Note         :
*****************************************************************
*/

int8_t sd_card_test(void)
{
 uint16_t byte_number=0;
	  uint8_t status=1;
	  uint8_t sector_address=0;
      
  if(init_sdcard())
    {
   for(sector_address=0;sector_address<10;sector_address++)
      {	  
	     for(byte_number=0;byte_number<sizeof(buf);byte_number++)
		  {
			buf[byte_number] = 0;
		  }
	     sd_writeSector(sector_address,buf);

	     for(byte_number=0;byte_number<sizeof(buf);byte_number++)
	      {
			if(byte_number%2)
			buf[byte_number]=0xAA;
			else
			buf[byte_number]=0x55;
	      }
         sd_writeSector(sector_address,buf);

	     for(byte_number=0;byte_number<sizeof(buf);byte_number++)
	      {
		   buf[byte_number]=0;
	      }

	     sd_readSector( sector_address, buf, sizeof(buf));
		
	     for(byte_number=0;byte_number<sizeof(buf);byte_number++)
           {
			if(byte_number%2)
			 {
			 	if ( buf[byte_number]!=0xAA )
			 	{
			 	status  = 0;
			 	break ;
			 	}
			 }
			else
			{
			 	if (buf[byte_number]!= 0x55)
			 	{
			 		status = 0 ;
			 		break ;
				}
			}
          }

         if(status == 1)
          {

		   return SDCARD_FOUND;
         
          }
         else
          {

		   return SDCARD_FAIL;
        
          }
      }
  }
  else
       {
	    return SDCARD_NOT_DETECTED;
        
       }
   	 return 0;
}

/* Main Program */

int main (void) {


	int8_t sdcard_test;
	SystemInit();                         /* initialize clocks */
	SystemInit();                         /* initialize clocks */
	debug_frmwrk_init();
	do{
	  user_switch_init();
	  }while(int_flag != 1);
	print_menu();
    sdcard_test=sd_card_test();

	if(sdcard_test== SDCARD_FOUND)
	 {
	    
	    _DBG("SDcard PASS\r\n");
	 }
	else if(sdcard_test==SDCARD_FAIL)
	 {
	    
	    _DBG("SDcard FAIL\r\n");
	 }
	else if(sdcard_test==SDCARD_NOT_DETECTED)
	 {
	    
	    _DBG("SDcard misssing\r\n");
	 }

 	while (1) 
	{ 	  
	}

}

