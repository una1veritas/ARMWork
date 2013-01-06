//******************************************************************************
// STM32F4 Discovery SDCard + FatFs Test - CLIVE - SOURCER32@GMAIL.COM
//******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4_discovery_sdio_sd.h"

#include "armcore.h"
#include "gpio.h"
#include "usart.h"

#define DBG

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

char *dec32(unsigned long i) {
	static char str[16];
	char *s = str + sizeof(str);

	*--s = 0;

	do {
		*--s = '0' + (char) (i % 10);
		i /= 10;
	} while (i);

	return (s);
}

//******************************************************************************

int main(void) {
	/*!< At this stage the microcontroller clock setting is already configured,
	 this is done through SystemInit() function which is called from startup
	 file (startup_stm32f4xx.s) before to branch to application main.
	 To reconfigure the default setting of SystemInit() function, refer to
	 system_stm32f4xx.c file
	 */
	USART usart6;
	char tmp[64];

	NVIC_Configuration(); /* Interrupt Config */

#ifdef DBG
	usart_begin(&usart6, USART6, PC7, PC6, 115200);
	usart_print(&usart6, "FatFs Testing.\r\n");
	usart_flush(&usart6);
	
//	RCC_Configuration();

//	GPIO_Configuration();

// 	USART6_Configuration();

//	puts("FatFs Testing");
#endif

	memset(&fs32, 0, sizeof(FATFS));

	res = f_mount(0, &fs32);

#ifdef DBG
	if (res != FR_OK)
		//printf("res = %d f_mount\n", res);
	{
		sprintf(tmp, "res = %d f_mount\n", res);
		usart_print(&usart6, tmp);
		usart_flush(&usart6);
	}
#endif

	memset(&fil, 0, sizeof(FIL));

	res = f_open(&fil, "MESSAGE.TXT", FA_READ);

#ifdef DBG
	if (res != FR_OK)
		//printf("res = %d f_open MESSAGE.TXT\n", res);
	{
		sprintf(tmp, "res = %d f_open MESSAGE.TXT\n", res);
		usart_print(&usart6, tmp);
		usart_flush(&usart6);
	}
#endif

	if (res == FR_OK) {
		UINT Total = 0;

		while (1) {
			BYTE Buffer[512];
			UINT BytesRead;
			UINT i;

			res = f_read(&fil, Buffer, sizeof(Buffer), &BytesRead);

#ifdef DBG
			if (res != FR_OK)
			//	printf("res = %d f_read MESSAGE.TXT\n", res);
			{
				sprintf(tmp, "res = %d f_read MESSAGE.TXT\n", res);
				usart_print(&usart6, tmp);
			usart_flush(&usart6);
			}
#endif

			if (res != FR_OK)
				break;

			Total += BytesRead;

#ifdef DBG
			for (i = 0; i < BytesRead; i++)
				//putchar(Buffer[i]);
				usart_write(&usart6, Buffer[i]);
			usart_flush(&usart6);
#endif

			if (BytesRead < sizeof(Buffer))
				break;
		}

		res = f_close(&fil); // MESSAGE.TXT

#ifdef DBG
		if (res != FR_OK)
		//	printf("res = %d f_close MESSAGE.TXT\n", res);
		{
			sprintf(tmp, "res = %d f_close MESSAGE.TXT\n", res);
			usart_print(&usart6, tmp);
			usart_flush(&usart6);
		}

//		printf("Total = %d\n", Total);
		sprintf(tmp, "Total = %d\n", Total);
		usart_print(&usart6, tmp);
		usart_flush(&usart6);
#endif

		res = f_open(&fil, "LENGTH.TXT", FA_CREATE_ALWAYS | FA_WRITE);

#ifdef DBG
		if (res != FR_OK)
//			printf("res = %d f_open LENGTH.TXT\n", res);
		{
			sprintf(tmp, "res = %d f_open LENGTH.TXT\n", res);
			usart_print(&usart6, tmp);
		}
#endif

		if (res == FR_OK) {
			UINT BytesWritten;
			char crlf[] = "\r\n";
			char *s = dec32(Total);

			res = f_write(&fil, s, strlen(s), &BytesWritten);

			res = f_write(&fil, crlf, strlen(crlf), &BytesWritten);

			res = f_close(&fil); // LENGTH.TXT

#ifdef DBG			
			if (res != FR_OK)
			//	printf("res = %d f_close LENGTH.TXT\n", res);
			{
				sprintf(tmp, "res = %d f_close LENGTH.TXT\n", res);
				usart_print(&usart6, tmp);
			}
#endif			
		}
	}

	res = f_open(&fil, "DIR.TXT", FA_CREATE_ALWAYS | FA_WRITE);

#ifdef DBG
	if (res != FR_OK)
//		printf("res = %d f_open DIR.TXT\n", res);
	{
		sprintf(tmp, "res = %d f_open DIR.TXT\n", res);
		usart_print(&usart6, tmp);
	}
#endif

	if (res == FR_OK) {
		UINT BytesWritten;

		path = "";

		res = f_opendir(&dir, path);

#ifdef DBG
		if (res != FR_OK)
		//	printf("res = %d f_opendir\n", res);
		{
			sprintf(tmp, "res = %d f_opendir\n", res);
			usart_print(&usart6, tmp);
		}
#endif

		if (res == FR_OK) {
			while (1) {
				char str[256];
				char *s = str;
				char *fn;

				res = f_readdir(&dir, &fno);

#ifdef DBG
				if (res != FR_OK)
				//	printf("res = %d f_readdir\n", res);
				{
					sprintf(tmp, "res = %d f_readdir\n", res);
					usart_print(&usart6, tmp);
				}
#endif

				if ((res != FR_OK) || (fno.fname[0] == 0))
					break;

#if _USE_LFN
				fn = *fno.lfname ? fno.lfname : fno.fname;
#else
				fn = fno.fname;
#endif

#ifdef DBG
				sprintf(tmp, "%c%c%c%c ", ((fno.fattrib & AM_DIR) ? 'D' : '-'),
						((fno.fattrib & AM_RDO) ? 'R' : '-'),
						((fno.fattrib & AM_SYS) ? 'S' : '-'),
						((fno.fattrib & AM_HID) ? 'H' : '-'));
				usart_print(&usart6, tmp);

				sprintf(tmp, "%10d ", fno.fsize);
				usart_print(&usart6, tmp);

				sprintf(tmp, "%s/%s\n", path, fn);
				usart_print(&usart6, tmp);
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
		if (res != FR_OK) {
			sprintf(tmp, "res = %d f_close DIR.TXT\n", res);
			usart_print(&usart6, tmp);
		}
#endif		
	}

	while (1)
		; /* Infinite loop */
}

//******************************************************************************

void NVIC_Configuration(void) {
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1 );

	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************************************************/

void _sys_exit(int return_code) {
	label: goto label;
	/* endless loop */
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
