//******************************************************************************
// STM32F4 Discovery SDCard + FatFs Test - CLIVE - SOURCER32@GMAIL.COM
//******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
//#include "stm32f4_discovery_sdio_sd.h"

#include "armcore.h"
#include "usart.h"

#include "stdiodef.h"

#include "rtc.h"

/* Exported functions ------------------------------------------------------- */
void RTC_Config(void);
void RTC_TimeRegulate(void);

//******************************************************************************

char *dec32(unsigned long i)
{
  static char str[16];
  char *s = str + sizeof(str);

  *--s = 0;

  do
  {
    *--s = '0' + (char)(i % 10);
    i /= 10;
  }
  while(i);

  return(s);
}

//******************************************************************************

RTC_InitTypeDef RTC_InitStructure;
#define RTC_CLOCK_SOURCE_LSE   /* LSE used as RTC source clock */
/* #define RTC_CLOCK_SOURCE_LSI */ /* LSI used as RTC source clock. The RTC Clock
                                      may varies due to LSI frequency dispersion. */ 
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
	RCC_ClocksTypeDef RCC_Clocks;

	TIM2_timer_start();
	usart_init(&STDSERIAL, USART6, PC7, PC6, 115200);


	puts("RTC Test.\n");
	
	RCC_GetClocksFreq(&RCC_Clocks);

	printf( "SYSCLK = %ld, HCLK = %ld\n", RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency);

	  /* Output a message on Hyperterminal using printf function */
  printf("\n\r  *********************** RTC Hardware Calendar Example ***********************\n\r");
  
  if ( RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET || (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2))
  {  
    // RTC configuration  //
    RTC_Config();

    // Configure the RTC data register and RTC prescaler //
    RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   
    // Check on RTC init //
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
      printf("\n\r        /!\\***** RTC Prescaler Config failed *******!\\ \n\r");
    }

    // Configure the time register //
    RTC_TimeRegulate(); 
  }
  else
  {
    // Check if the Power On Reset flag is set //
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("\r\n Power On Reset occurred....\n\r");
    }
    // Check if the Pin Reset flag is set //
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("\r\n External Reset occurred....\n\r");
    }

    printf("\n\r No need to configure RTC....\n\r");
    
    // Enable the PWR clock //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Allow access to RTC //
    PWR_BackupAccessCmd(ENABLE);

    // Wait for RTC APB registers synchronisation //
    RTC_WaitForSynchro();

    // Clear the RTC Alarm Flag //
    RTC_ClearFlag(RTC_FLAG_ALRAF);

    // Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) //
    EXTI_ClearITPendingBit(EXTI_Line17);

    // Display the RTC Time and Alarm //
    printf("Current time: %06ux\n", RTC_getTime());		
    printf("Alarm setting: %06ux\n", RTC_getAlarm());		
  }
	RCC_ClearFlag();

  while(1) /* Infinite loop */
	{
		delay(1000);
    printf("%06x %08x\n", RTC_getTime(), RTC_getCalDate());		
	}
	
}

//******************************************************************************

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
    
#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
/* The RTC Clock may varies due to LSI frequency dispersion. */   
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  
  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

#else
  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeRegulate(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_AlarmTypeDef RTC_AlarmStructure;
	RTC_DateTypeDef RTC_DateStructure;

  uint32 ttime = 0;
  char tmp[32];

  printf("\n\r==============Time Settings=====================================\n\r");
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  while (!ttime)
  {
		printf("  Please Set Time in HHMMSS form:\n\r");
	  fgets(tmp, 32, stdin);
    ttime = strtol(tmp,NULL,16);
		printf("%x\n", ttime);
		if ( IS_RTC_HOUR24(ttime>>16 & 0xff) && IS_RTC_MINUTES(ttime>>8 & 0xff) && IS_RTC_SECONDS(ttime & 0xff) ) 
			break;
  }
	RTC_TimeStructure.RTC_Hours = ttime>>16 & 0x3f;
	RTC_TimeStructure.RTC_Minutes = ttime>>8 & 0x7f;
	RTC_TimeStructure.RTC_Seconds = ttime & 0x7f;

	/* Configure the RTC time register */
	if(RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure) == ERROR)
	{
		printf("\n\r>> !! RTC Set Time failed. !! <<\n\r");
	} 
	else
	{
		printf("\n\r>> !! RTC Set Time success. !! <<\n\r");
		printf("Current time: %06x\n", RTC_getTime());		
		/* Indicator for the RTC configuration */
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
	}
	
	printf("\n\r==============Date Settings=====================================\n\r");
	ttime = 0;
  while (!ttime)
  {
		printf("  Please Set Calendar Date in YYMMDD form:\n\r");
	  fgets(tmp, 32, stdin);
    ttime = strtol(tmp,NULL,16);
		printf("%x\n", ttime);
		if ( IS_RTC_YEAR(ttime>>16 & 0xff) && IS_RTC_MONTH(ttime>>8 & 0xff) && IS_RTC_DATE(ttime & 0xff) ) 
			break;
		ttime = 0;
  }
  RTC_DateStructure.RTC_WeekDay = 1;
  RTC_DateStructure.RTC_Date = ttime&0xff;
  RTC_DateStructure.RTC_Month = ttime>>8&0xff;
  RTC_DateStructure.RTC_Year = ttime>>16&0xff;

  /* Configure the RTC date register */
  if(RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure) == ERROR)
  {
    printf("\n\r>> !! RTC Set Date failed. !! <<\n\r");
  } 
  else
  {
    printf("\n\r>> !! RTC Set Date success. !! <<\n\r");
		printf("Calendar Date: %06x\n", RTC_getCalDate());		
    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
  }

	
  /* Disable the Alarm A */
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

  printf("\n\r==============Alarm A Settings=====================================\n\r");
  RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
  ttime = 0;
  while (!ttime)
  {
		printf("  Please Set Alarm in HHMMSS form:\n\r");
	  fgets(tmp, 32, stdin);
    ttime = strtol(tmp,NULL,16);
		if ( IS_RTC_HOUR24(ttime>>16 & 0xff) && IS_RTC_MINUTES(ttime>>8 & 0xff) && IS_RTC_SECONDS(ttime & 0xff) ) 
			break;
		else {
			ttime = 0;
			break;
		}
  }
	if ( ttime ) {
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = ttime>>16 & 0x3f;
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = ttime>>8 & 0x7f;
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = ttime & 0x7f;

		/* Set the Alarm A */
		RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
		RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
		RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

		/* Configure the RTC Alarm A register */
		RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
		printf("\n\r>> !! RTC Set Alarm success. !! <<\n\r");
		printf("Alarm setting: %06x\n", RTC_getAlarm());		

		/* Enable the RTC Alarm A Interrupt */
		RTC_ITConfig(RTC_IT_ALRA, ENABLE);
		 
		/* Enable the alarm  A */
		RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	}
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
uint32 RTC_getTime(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
  /* Get the current Time */
  RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
  return RTC_TimeStructure.RTC_Hours<<16 | RTC_TimeStructure.RTC_Minutes<<8 | RTC_TimeStructure.RTC_Seconds;
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
uint32 RTC_getAlarm(void)
{
	RTC_AlarmTypeDef RTC_AlarmStructure;
  /* Get the current Alarm */
  RTC_GetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
  return RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours<<16 |
			RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes<<8 | RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds;
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
uint32 RTC_getCalDate(void)
{
	RTC_DateTypeDef RTC_DateStructure;
  /* Get the current Time */
  RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);
  return RTC_DateStructure.RTC_WeekDay<<24 | RTC_DateStructure.RTC_Year<<16 | RTC_DateStructure.RTC_Month<<8 | RTC_DateStructure.RTC_Date;
}

//******************************************************************************

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while(1); /* Infinite loop */
}
#endif

//******************************************************************************
