/********************************************************************************/
/*!
	@file			main.c
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        W.I.P
    @date           2013.01.02
	@brief          BARE-METAL Project for STM32F407VGT6(STM32F4Discovery).	@n
									   and STM32F407ZGT6(REDBULL).

    @section HISTORY
		2013.01.02	Vx.xx	See Whatnew.txt

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**************************************************************************/
/*! 
    @brief  Main Program.
	@param  None.
    @retval None.
*/
/**************************************************************************/
int main(void)
{
	/* Set Basis System */
	Set_System();

	/* LED Indicator */
	LED_Configuration();
	
	/* Set SysTickCounter for _delay_ms() and _delay_us() */
	SysTickInit(INTERVAL);

#if 0 /* Simplest Program (For Test) */
	/* Infinite Loop */
	for(;;){
		_delay_ms(500);
		LED1_On();
		LED2_On();
		LED3_Off();
		LED4_Off();
		_delay_ms(500);
		LED1_Off();
		LED2_Off();
		LED3_On();
		LED4_On();
	}
#endif

	/* Display Configuration */
	Display_init_if();

#ifdef USE_REDBULL
	/* RTC Configuration */
	RTC_Configuration();
#endif

	/* Set UART and redirect to STDIO */
	conio_init(UART_DEFAULT_NUM,UART_BAUDLATE);
	/* STDIO function test */
	printf("\n");
	printf("Welcome to %s test program !!\n",MPU_SUBMODEL);
	printf("Version %s!!\n",APP_VERSION);
	printf("Build Date : %s\n",__DATE__);
	printf("\n");

	/* Detect STM32F4 Chip Revision */
	char mcuid = 'A';
    if((DBGMCU->IDCODE & DBGMCU_IDCODE_DEV_ID) == 0x0413) mcuid = 'Z';
	printf("STM32F4xxx ChipRevision : %c\n",mcuid);
	printf("\n");

#if 0	/* FPU function test */
	volatile float fnk1,fnk2,fnk3=0;
	fnk1 = 14.0;
	fnk2 = 9.0;
	fnk3 = fnk1/fnk2;
	printf("Floating point calculation: %f/%f=%f\n",fnk1,fnk2,fnk3);
	printf("\n");
#endif

	/* Loading FONTX2 Drivers */
#if defined(USE_ADS7843) || USE_FILER_FONTX
	/* Init Font Driver Ank*/
	InitFont_Ank(&ANKFONT,font_table_ank);
	/* USE Fonts */
	ChangeCurrentAnk(&ANKFONT);
#endif
#if USE_FILER_FONTX && defined(USE_KANJIFONT)
	/* Init Font Driver Kanji*/
	InitFont_Kanji(&KANJIFONT,font_table_kanji);
	/* USE Fonts */
	ChangeCurrentKanji(&KANJIFONT);
#endif

	/* Loading TouchSence Driver 			*/
	/* MUST be loaded FONTX2 Drivers before! */
#if	defined(USE_ADS7843)
	/* Touch Screen configuration */
	TC_IoInit_If();
	TC_ReadRaw();
	TC_CalibScreen_If();
#endif

	/* Main Loop */
	while (1)
	{
		/* FatFs Test Terminal */
		ff_test_term();
	}

}

/* End Of File ---------------------------------------------------------------*/
