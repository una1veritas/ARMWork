//******************************************************************************
// STM32F4 Discovery SDCard + FatFs Test - CLIVE - SOURCER32@GMAIL.COM
//******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4_discovery_sdio_sd.h"

/* Exported functions ------------------------------------------------------- */
void RTC_Config(void);
void RTC_TimeRegulate(void);
void RTC_TimeShow(void);
void RTC_AlarmShow(void);
uint8_t USART_Scanf(uint32_t value);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


#define DBG

//******************************************************************************

void NVIC_Configuration(void);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART6_Configuration(void);

//******************************************************************************

#include "ff.h"
#include "diskio.h"

FRESULT res;
FILINFO fno;
FIL fil;
DIR dir;
FATFS fs32;
char* path;

#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif

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

RTC_TimeTypeDef RTC_TimeStructure;
RTC_InitTypeDef RTC_InitStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;
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

  NVIC_Configuration(); /* Interrupt Config */


	
#ifdef DBG
	RCC_Configuration();

	GPIO_Configuration();

  USART6_Configuration();

	puts("FatFs Testing\n");
	
	RCC_GetClocksFreq(&RCC_Clocks);

	printf( "SYSCLK = %dl\n", RCC_Clocks.SYSCLK_Frequency);
	printf( ", HCLK = %dl\n", RCC_Clocks.HCLK_Frequency);

	  /* Output a message on Hyperterminal using printf function */
  printf("\n\r  *********************** RTC Hardware Calendar Example ***********************\n\r");
  
	/*
  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
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
//      printf("\n\r        /!\\***** RTC Prescaler Config failed *******!\\ \n\r");
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
    RTC_TimeShow();
    RTC_AlarmShow();
  }
	*/
#endif

	memset(&fs32, 0, sizeof(FATFS));

	res = f_mount(0, &fs32);

#ifdef DBG
	if (res != FR_OK)
		printf("res = %d f_mount\n", res);
#endif
	
	memset(&fil, 0, sizeof(FIL));
	
	res = f_open(&fil, "MESSAGE.TXT", FA_READ);

#ifdef DBG
	if (res != FR_OK)
		printf("res = %d f_open MESSAGE.TXT\n", res);
#endif
	
	if (res == FR_OK)
	{
		UINT Total = 0;

		while(1)
		{
			BYTE Buffer[512];
			UINT BytesRead;
			UINT i;

			res = f_read(&fil, Buffer, sizeof(Buffer), &BytesRead);

#ifdef DBG
			if (res != FR_OK)
				printf("res = %d f_read MESSAGE.TXT\n", res);
#endif
			
			if (res != FR_OK)
				break;

			Total += BytesRead;

#ifdef DBG
			for(i=0; i<BytesRead; i++)
				putchar(Buffer[i]);
#endif
			
			if (BytesRead < sizeof(Buffer))
				break;
		}

		res = f_close(&fil); // MESSAGE.TXT

#ifdef DBG
		if (res != FR_OK)
			printf("res = %d f_close MESSAGE.TXT\n", res);

		printf("Total = %d\n", Total);
#endif

    res = f_open(&fil, "LENGTH.TXT", FA_CREATE_ALWAYS | FA_WRITE);

#ifdef DBG
		if (res != FR_OK)
			printf("res = %d f_open LENGTH.TXT\n", res);
#endif
	
    if (res == FR_OK)
    {
      UINT BytesWritten;
      char crlf[] = "\r\n";
      char *s = dec32(Total);

      res = f_write(&fil, s, strlen(s), &BytesWritten);

      res = f_write(&fil, crlf, strlen(crlf), &BytesWritten);

  		res = f_close(&fil); // LENGTH.TXT

#ifdef DBG			
  		if (res != FR_OK)
	  		printf("res = %d f_close LENGTH.TXT\n", res);
#endif			
    }
	}

  res = f_open(&fil, "DIR.TXT", FA_CREATE_ALWAYS | FA_WRITE);

#ifdef DBG
	if (res != FR_OK)
		printf("res = %d f_open DIR.TXT\n", res);
#endif
	
  if (res == FR_OK)
  {
    UINT BytesWritten;

		path = "";

		res = f_opendir(&dir, path);

#ifdef DBG
		if (res != FR_OK)
			printf("res = %d f_opendir\n", res);
#endif
	
		if (res == FR_OK)
		{
			while(1)
			{
        char str[256];
        char *s = str;
				char *fn;

				res = f_readdir(&dir, &fno);

#ifdef DBG
				if (res != FR_OK)
					printf("res = %d f_readdir\n", res);
#endif
			
				if ((res != FR_OK) || (fno.fname[0] == 0))
					break;

#if _USE_LFN
				fn = *fno.lfname ? fno.lfname : fno.fname;
#else
				fn = fno.fname;
#endif

#ifdef DBG
				printf("%c%c%c%c ",
					((fno.fattrib & AM_DIR) ? 'D' : '-'),
					((fno.fattrib & AM_RDO) ? 'R' : '-'),
					((fno.fattrib & AM_SYS) ? 'S' : '-'),
					((fno.fattrib & AM_HID) ? 'H' : '-') );

				printf("%10d ", fno.fsize);

				printf("%s/%s\n", path, fn);
#endif
				
		  	*s++ = ((fno.fattrib & AM_DIR) ? 'D' : '-');
				*s++ = ((fno.fattrib & AM_RDO) ? 'R' : '-');
  			*s++ = ((fno.fattrib & AM_SYS) ? 'S' : '-');
	  		*s++ = ((fno.fattrib & AM_HID) ? 'H' : '-');

        *s++ = ' ';

        strcpy(s, dec32(fno.fsize));
        s += strlen(s);

        *s++ = ' ';

        strcpy(s, path);
        s += strlen(s);

        *s++ = '/';

        strcpy(s, fn);
        s += strlen(s);

        *s++ = 0x0D;
        *s++ = 0x0A;
        *s++ = 0;

        res = f_write(&fil, str, strlen(str), &BytesWritten);
			}
		}

  	res = f_close(&fil); // DIR.TXT

#ifdef DBG		
 		if (res != FR_OK)
  		printf("res = %d f_close DIR.TXT\n", res);
#endif		
  }

  while(1); /* Infinite loop */
}

//******************************************************************************

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**************************************************************************************/

void RCC_Configuration(void)
{
  /* --------------------------- System Clocks Configuration -----------------*/
  /* USART6 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

  /* GPIOA clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
}

/**************************************************************************************/

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*-------------------------- GPIO Configuration ----------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Connect USART pins to AF */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);  // USART6_TX
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);  // USART6_RX
}

/**************************************************************************************/

void USART6_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;

  /* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART6, &USART_InitStructure);

  USART_Cmd(USART6, ENABLE);
}

//******************************************************************************
// Hosting of stdio functionality through USART6
//******************************************************************************

#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f)
{
	static int last;

	if ((ch == (int)'\n') && (last != (int)'\r'))
	{
		last = (int)'\r';

  	while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);

 	  USART_SendData(USART6, last);
	}
	else
		last = ch;

	while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);

  USART_SendData(USART6, ch);

  return(ch);
}

int fgetc(FILE *f)
{
	char ch;

	while(USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == RESET);

	ch = USART_ReceiveData(USART6);

  return((int)ch);
}

int ferror(FILE *f)
{
  /* Your implementation of ferror */
  return EOF;
}

void _ttywrch(int ch)
{
	static int last;

	if ((ch == (int)'\n') && (last != (int)'\r'))
	{
		last = (int)'\r';

  	while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);

 	  USART_SendData(USART6, last);
	}
	else
		last = ch;

	while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);

  USART_SendData(USART6, ch);
}

/**
  * @brief  Gets numeric values from the hyperterminal.
  * @param  None
  * @retval None
  */
uint8_t USART_Scanf(uint32_t value)
{
  uint32_t index = 0;
  uint32_t tmp[2] = {0, 0};

  while (index < 2)
  {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == RESET)
    {}
    tmp[index++] = (USART_ReceiveData(USART6));
    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    {
      printf("\n\r Please enter valid number between 0 and 9 \n\r");
      index--;
    }
  }
  /* Calculate the Corresponding value */
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  /* Checks */
  if (index > value)
  {
    printf("\n\r Please enter valid number between 0 and %d \n\r", value);
    return 0xFF;
  }
  return index;
}

void _sys_exit(int return_code)
{
label:  goto label;  /* endless loop */
}

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
  uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

  printf("\n\r==============Time Settings=====================================\n\r");
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  printf("  Please Set Hours:\n\r");
  while (tmp_hh == 0xFF)
  {
    tmp_hh = USART_Scanf(23);
    RTC_TimeStructure.RTC_Hours = tmp_hh;
  }
  printf("  %0.2d\n\r", tmp_hh);
  
  printf("  Please Set Minutes:\n\r");
  while (tmp_mm == 0xFF)
  {
    tmp_mm = USART_Scanf(59);
    RTC_TimeStructure.RTC_Minutes = tmp_mm;
  }
  printf("  %0.2d\n\r", tmp_mm);
  
  printf("  Please Set Seconds:\n\r");
  while (tmp_ss == 0xFF)
  {
    tmp_ss = USART_Scanf(59);
    RTC_TimeStructure.RTC_Seconds = tmp_ss;
  }
  printf("  %0.2d\n\r", tmp_ss);

  /* Configure the RTC time register */
  if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
  {
    printf("\n\r>> !! RTC Set Time failed. !! <<\n\r");
  } 
  else
  {
    printf("\n\r>> !! RTC Set Time success. !! <<\n\r");
    RTC_TimeShow();
    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
  }

  tmp_hh = 0xFF;
  tmp_mm = 0xFF;
  tmp_ss = 0xFF;

  /* Disable the Alarm A */
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

  printf("\n\r==============Alarm A Settings=====================================\n\r");
  RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
  printf("  Please Set Alarm Hours:\n\r");
  while (tmp_hh == 0xFF)
  {
    tmp_hh = USART_Scanf(23);
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = tmp_hh;
  }
  printf("  %0.2d\n\r", tmp_hh);
  
  printf("  Please Set Alarm Minutes:\n\r");
  while (tmp_mm == 0xFF)
  {
    tmp_mm = USART_Scanf(59);
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = tmp_mm;
  }
  printf("  %0.2d\n\r", tmp_mm);
  
  printf("  Please Set Alarm Seconds:\n\r");
  while (tmp_ss == 0xFF)
  {
    tmp_ss = USART_Scanf(59);
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = tmp_ss;
  }
  printf("  %0.2d", tmp_ss);

  /* Set the Alarm A */
  RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

  /* Configure the RTC Alarm A register */
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
  printf("\n\r>> !! RTC Set Alarm success. !! <<\n\r");
  RTC_AlarmShow();

  /* Enable the RTC Alarm A Interrupt */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
   
  /* Enable the alarm  A */
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeShow(void)
{
  /* Get the current Time */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  printf("\n\r  The current time is :  %0.2d:%0.2d:%0.2d \n\r", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_AlarmShow(void)
{
  /* Get the current Alarm */
  RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
  printf("\n\r  The current alarm is :  %0.2d:%0.2d:%0.2d \n\r", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
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
