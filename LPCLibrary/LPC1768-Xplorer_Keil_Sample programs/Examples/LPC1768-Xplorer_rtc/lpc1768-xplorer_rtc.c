/****************************************************************************
 *   $Id:: rtctest.c 6097 2011-01-07 04:31:25Z nxp12832                     $
 *   Project: NXP LPC17xx RTC example
 *
 *   Description:
 *     This file contains RTC test modules, main entry, to test RTC APIs.
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
****************************************************************************/
#include "lpc17xx.h"
#include "lpc_types.h"
#include "rtc.h"
#include "stdio.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_gpio.h"
//#include "extint.h"
#include "delay.h"
#include "debug_frmwrk.h"
char buf_time[16] = {0};
char buf_date[16] = {0};
extern volatile uint32_t alarm_on;
RTCTime local_time, alarm_time, current_time;
uint32_t int_flag=0;
const char menu[] =
"********************************************************************************\n\r"
"NGX Technologies \n\r"
"RTC demo \n\r"
"\t - MCU: LPC1768 \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: USB VCOM \n\r"

"This example used to configure the\n\r "
" RTC \n\r"
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
			int_flag = 1;

		}

		else
		{
			int_flag = 0;
		}
 }

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{ 
  /* SystemClockUpdate() updates the SystemFrequency variable */
  
//  SystemClockUpdate();
    SystemInit();                         /* initialize clocks */
    RTCInit();
    debug_frmwrk_init();
    do{
     user_switch_init();
      }while(int_flag != 1);
	print_menu();
  	
  /* Initialize RTC module */
  
#if 1
  local_time.RTC_Sec = 20;
  local_time.RTC_Min = 25;
  local_time.RTC_Hour = 5;
  local_time.RTC_Mday = 21;
  local_time.RTC_Wday = 4;
  local_time.RTC_Yday = 1;		/* current date 07/12/2006 */
  local_time.RTC_Mon = 1;
  local_time.RTC_Year = 2012;
  RTCSetTime( local_time );		/* Set local time */
#endif
#if 0
  alarm_time.RTC_Sec = 0;
  alarm_time.RTC_Min = 0;
  alarm_time.RTC_Hour = 0;
  alarm_time.RTC_Mday = 1;
  alarm_time.RTC_Wday = 0;
  alarm_time.RTC_Yday = 1;		/* alarm date 01/01/2007 */
  alarm_time.RTC_Mon = 1;
  alarm_time.RTC_Year = 2007;
  RTCSetAlarm( alarm_time );		/* set alarm time */
#endif
#if 0
  NVIC_EnableIRQ(RTC_IRQn);

  /* mask off alarm mask, turn on IMYEAR in the counter increment interrupt
  register */
  RTCSetAlarmMask(AMRSEC|AMRMIN|AMRHOUR|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR);
  LPC_RTC->CIIR = IMMIN | IMYEAR;
  /* 2007/01/01/00:00:00 is the alarm on */
#endif    
  RTCStart();

  while (1)
  {					/* Loop forever */
	current_time = RTCGetTime();

	lpc_printf("DATE:%.2d:%.2d:%d\t", current_time.RTC_Mday,current_time.RTC_Mon,current_time.RTC_Year);
	lpc_printf("TIME:%.2d:%.2d:%.2d\r", current_time.RTC_Hour,current_time.RTC_Min,current_time.RTC_Sec);
	
	
//	lpc_printf(buf_time);
//	lpc_printf(buf_date);	
  }
//	while(1);
	
#if 0
	if ( alarm_on != 0 )								  
	{
	  alarm_on = 0;
	  /* Get current time when alarm is on */
	  current_time = RTCGetTime();
	  	
	}
#endif
  }

//}

/******************************************************************************
**                            End Of File
******************************************************************************/
