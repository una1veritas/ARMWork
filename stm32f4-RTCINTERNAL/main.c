//******************************************************************************
// STM32F4 Discovery SDCard + FatFs Test - CLIVE - SOURCER32@GMAIL.COM
//******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
//#include "stm32f4_discovery_sdio_sd.h"

#include "cmcore.h"
//#include "usart.h"

//#include "stdiodef.h"

#include "stm32f4xx_rtc.h"
#include "rtc.h"

/* Exported functions ------------------------------------------------------- */

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

//#define RTC_CLOCK_SOURCE_LSE   /* LSE used as RTC source clock */
/* #define RTC_CLOCK_SOURCE_LSI */ /* LSI used as RTC source clock. The RTC Clock
                                      may varies due to LSI frequency dispersion. */ 
//__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
	RCC_ClocksTypeDef RCC_Clocks;

	cmcore_init();

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
