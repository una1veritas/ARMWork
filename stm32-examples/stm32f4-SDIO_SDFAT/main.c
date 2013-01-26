//******************************************************************************
// STM32F4 Discovery SDCard + FatFs Test - CLIVE - SOURCER32@GMAIL.COM
//******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4_discovery_sdio_sd.h"

#include "cmcore.h"

#define DBG
#define DBGIO


//******************************************************************************

void NVIC_Configuration(void);
//void RCC_Configuration(void);
//void GPIO_Configuration(void);
//void USART6_Configuration(void);

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
usart ser6;

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
		RCC_ClocksTypeDef RCC_Clocks;
	char tmp[64];

	char fname_read[] = "MESSAGE.TXT";

	UINT Total;
	
  NVIC_Configuration(); /* Interrupt Config */
//	RCC_Configuration();
//	GPIO_Configuration();
//  USART6_Configuration();

	cmcore_init();
	
	usart_init(&ser6, USART6, PC7, PC6);
	usart_begin(&ser6, 57600);
		
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */  

  RCC_GetClocksFreq(&RCC_Clocks);
//  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	
//	usart_init(&stdserial, USART3, PB11, PB10, 57600);
	sprintf(tmp, "SYSCLK = %ld\n", RCC_Clocks.SYSCLK_Frequency);
	usart_print(&ser6, tmp);
	sprintf(tmp, "HCLK = %ld\n", RCC_Clocks.HCLK_Frequency);
	usart_print(&ser6, tmp);
	sprintf(tmp, "PCLK1 = %ld\n", RCC_Clocks.PCLK1_Frequency);
	usart_print(&ser6, tmp);
	sprintf(tmp, "PCLK2 = %ld\n", RCC_Clocks.PCLK2_Frequency);
	usart_print(&ser6, tmp);
//	usart_print(&stdserial, tmp);
	

	usart_print(&ser6, "FatFs Testing\n");

	memset(&fs32, 0, sizeof(FATFS));
	memset(&fil, 0, sizeof(FIL));

	res = f_mount(0, &fs32);
	if (res != FR_OK)
		printf("error: res = %d f_mount\n", res);	

	res = f_open(&fil, fname_read, FA_READ);
	if (res != FR_OK)
		printf("error: res = %d f_open %s\n", res, fname_read);
	
	if (res == FR_OK) {
		printf("\n>>> Mount, Open %s succeeded. <<<\n\n", fname_read);
		
		Total = 0;
		while(1)
		{
			BYTE Buffer[512];
			UINT BytesRead;
			UINT i;

			res = f_read(&fil, Buffer, sizeof(Buffer), &BytesRead);
			if (res != FR_OK) {
				printf("error: res = %d f_read %s\n", res, fname_read);
				goto _error;
			}
			printf("Buffer block %d bytes read.\n", BytesRead);

			Total += BytesRead;

			for(i=0; i<BytesRead; i++)
				putchar(Buffer[i]);

			if (BytesRead < sizeof(Buffer))
				break;
		}

		res = f_close(&fil); // MESSAGE.TXT		
		if (res != FR_OK) {
			printf("error: res = %d f_close%s\n", res, fname_read);
			goto _error;
		}
		
		printf("\n>>> Read operations for %dn bytes and file close %s succeeded. <<<\n\n", Total, fname_read);


    res = f_open(&fil, "LENGTH.TXT", FA_CREATE_ALWAYS | FA_WRITE);
		if (res != FR_OK)
			printf("res = %d f_open LENGTH.TXT\n", res);
	
    if (res == FR_OK)
    {
      UINT BytesWritten;
      char crlf[] = "\r\n";
      char *s = dec32(Total);

      res = f_write(&fil, s, strlen(s), &BytesWritten);

      res = f_write(&fil, crlf, strlen(crlf), &BytesWritten);

  		res = f_close(&fil); // LENGTH.TXT

  		if (res != FR_OK) {
	  		printf("res = %d f_close LENGTH.TXT\n", res);
				goto _error;
			}		
			printf("\n>>> Write operations file close LENGTH.TXT succeeded. <<<\n\n");
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
	
  printf("Good night.\n");
	
  while(1); /* Infinite loop */
	
_error:
	printf("error occurred.\n");
	while(1);
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
	usart_write(&ser6, ch);
  return(ch);
}

int fgetc(FILE *f)
{
  return((int)usart_read(&ser6));
}

int ferror(FILE *f)
{
  /* Your implementation of ferror */
  return EOF;
}

void _ttywrch(int ch)
{
	usart_write(&ser6, ch);
}

void _sys_exit(int return_code)
{
label:  goto label;  /* endless loop */
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
