#include "LPC17xx.H"/* LPC17xx definitions */
#include "lpc_types.h"
#include "i2c.h"
#include <stdio.h>
#include "tcpip.h"
#include "delay.h"
#include "rtc.h"
#include "lpc17xx_gpio.h"
#include "debug_frmwrk.h"
#include "sd.h"
#include "spi.h"
#include "sd.h"
#include "lpc1768_spi.h"
#include "timer_utilities.h"
#define TEST_LED_BIT  10
#define TEST_LED_PORT 0
#define FILENAME_W 	"MSWRITE.txt"
#define FILENAME_R 	"MSREAD.txt"
#define FILENAME_WR "MSWR.txt"
char buf_time[16] = {0};
char buf_date[16] = {0};
RTCTime local_time, alarm_time, current_time;
void ethernet_test(void);
void ethernet_init(void);
uint32_t int_flag=0;
#define SECTOR_SIZE 512
uint8_t buf[SECTOR_SIZE]={0};

// TODO: insert other definitions and declarations here

const uint8_t menu[] =
"********************************************************************************\n\r"
"NGX Technologies \n\r"
"All peripheral demo \n\r"
"\t - MCU: LPC1768 \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: USB VCOM \n\r"

"This example used to configure the\n\r "
" All peripheral \n\r"
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

/**
*******************************************************************
	Funtion Name : init_sdcard()

	Description  : 

	Input        : 

	Output       : None

	Note         :
*****************************************************************
*/
U8 init_sdcard(void)
{
 if(if_initInterface(0)==0)
 return 1;
 else
 return 0;
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
********************************************************************************************
	Function Name :	sd_card_test()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
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
/**
********************************************************************************************
	Function Name :	test_i2c_at24c256_flash()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
int8_t test_i2c_at24c256_flash(void)
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
     return I2C_NO_ERR;
  
	 
  }
  else
  {
     return  I2C_ERR_NO_RESPONSE;
	   
  }
}
/**
********************************************************************************************
	Function Name :	test_led_init()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
void test_led_init (void)
 {
	
	 LPC_GPIO0->FIODIR |= (1<<TEST_LED_BIT);
	  
 }

/**
********************************************************************************************
	Function Name :	test_led()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
void test_led (void)
 {
		int i=0;
for(i=0;i<10;i++)
{	 
	GPIO_ClearValue(TEST_LED_PORT,(1<<TEST_LED_BIT));
	delay(80000);
//	Timer_delay_ms(85);
    GPIO_SetValue(TEST_LED_PORT,(1<<TEST_LED_BIT));
	delay(80000);
//	Timer_delay_ms(85);
 }
}
 /**
********************************************************************************************
	Function Name :	rtc_value_set()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
void rtc_value_set(void)
{
  local_time.RTC_Sec = 20;
  local_time.RTC_Min = 25;
  local_time.RTC_Hour = 12;
  local_time.RTC_Mday = 31;
  local_time.RTC_Wday = 3;
  local_time.RTC_Yday = 1;		/* current date 07/12/2006 */
  local_time.RTC_Mon = 1;
  local_time.RTC_Year = 2012;
  RTCSetTime( local_time );		/* Set local time */
}

void rtc_test(void)
{
	static uint32_t previous_second=0;
	uint32_t second=0;
    current_time = RTCGetTime();
    second=current_time.RTC_Sec;
    if((second - previous_second)>=1)
    {
	lpc_printf("DATE:%.2d:%.2d:%d\t", current_time.RTC_Mday,current_time.RTC_Mon,current_time.RTC_Year);
	lpc_printf("TIME:%.2d:%.2d:%.2d\r", current_time.RTC_Hour,current_time.RTC_Min,current_time.RTC_Sec);
    }
    previous_second=second;
}

/**
********************************************************************************************
	Function Name :	hardware_init()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
void hardware_init(void)
{

	i2c_lpc_init(I2C_SPEED_100);
    RTCInit();
	test_led_init();                        	
}

/**
********************************************************************************************
	Function Name :	main()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
int main (void) 
{
	int8_t sdcard_test,i2c_test;
	SystemInit();                         /* initialize clocks */
	hardware_init();

	debug_frmwrk_init();
	do{
	  user_switch_init();
	  }while(int_flag != 1);
	print_menu();
	_DBG("User input detected\r\n\n");
	_DBG("Initializing LPC1768 Xplorer peripherals. Please wait..!\r\n\n");
	int_flag = 0;
	ethernet_init();
	_DBG("Ethernet PHY detected and initialized..!\r\n");
	_DBG("IP address of LPC1768 Xplorer is 192.168.1.123\r\n");
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
   i2c_test=test_i2c_at24c256_flash();
    if(i2c_test==	I2C_NO_ERR)
      {
	   _DBG("I2C EEPROM PASS\r\n\n");
	  }
    else if(i2c_test== I2C_ERR_NO_RESPONSE)
	  {
	  _DBG("I2C EEPROM FAIL\r\n\n");
	  }
   rtc_value_set();
   RTCStart();
   test_led();

	while (1) 
	 {				/* Loop forever */
		
		rtc_test();
		ethernet_test();
		user_switch_init();
		if(int_flag == 1)
		{
			_DBG("\nUser input detected\r\n");
			int_flag = 0;
		}	
    }
}



