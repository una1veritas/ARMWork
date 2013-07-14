/**********************************************************************
* $Id$		Gpio_LedBlinky.c	2011-06-02
*//**
* @file		Gpio_LedBlinky.c
* @brief	This example describes how to use GPIO to drive LEDs
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
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
**********************************************************************/
#include "lpc18xx_gpio.h"
#include "lpc18xx_scu.h"
#include "lpc18xx_libcfg_default.h"
#include "lpc18xx_cgu.h"
#include "debug_frmwrk.h"

#include "tcpip.h"
#include "error.h"
#include "lpc18xx_utils.h"
#include "main.h"
#include "tcpip.h"

#define LED1_BIT			11 //LEDUSB
#define LED1_PORT			1
#define LED2_BIT			12 //LEDUSB
#define LED2_PORT			1


#define PIN_INT_NUM				0
#define INT_ID					PIN_INT0_IRQn

#define PIN_INT_PORT			0   //G_INT
#define PIN_INT_BIT				7

#define PIN_INT_NUM				0
#define INT_ID					PIN_INT0_IRQn

/* Example group ----------------------------------------------------------- */
/** @defgroup Gpio_LedBlinky	Gpio_LedBlinky
 * @ingroup GPIO_Examples
 * @{
 */

/************************** PRIVATE VARIABLES *************************/
extern int led_d2;
extern int led_d1;
uint32_t msec;
uint32_t int_flag=0;
uint8_t intMode;
uint8_t menu[] =
"********************************************************************************\n\r"
"\tNGX Technologies \n\r"
"\t - LPC1830-Xplorer: Peripheral test \n\r"
"\t - MCU: LPC18xx \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: USB VCOM \n\r"
"********************************************************************************\n\r";


 volatile uint32_t TimeTick  = 0;
/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		SysTick handler sub-routine (1ms)
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void SysTick_Handler (void) {
//if(msec)msec--;
	u32Milliseconds += 10;
 TCPClockHandler();
}

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		GPIO interrupt 0 handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/


void GPIO0_IRQHandler(void)
{
  if(GPIO_GetIntStatus(PIN_INT_NUM))
	 {
		GPIO_ClearInt(intMode,1<<PIN_INT_NUM);
	  	if (!(GPIO_ReadValue(0) & (1<<7)))
		{
			int_flag = 1;
		}
	 }
}

/************************** PRIVATE DEFINITIONS *************************/

/*********************************************************************//**
 * @brief		Print menu
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	lpc_printf((const  char *)menu);
}
/*----------------- User_switch_init  --------------------------*/
/*********************************************************************//**
 * @brief		User_switch_init sub-routine 
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void User_switch_init(void)
{
		/* Configure pin function */
	scu_pinmux(0x2 ,11 , MD_PDN, FUNC0); 					//LED1 P2_11
	scu_pinmux(0x2 ,7 ,(MD_PLN|MD_EZI|MD_ZI), FUNC0);		//P2.7 : G_INT

	/* Set direction for GPIO port */
	GPIO_SetDir(LED1_PORT,(1<<LED1_BIT), 1); 			//USB0_IND1 LED (INT_LED)
	GPIO_SetDir(PIN_INT_PORT,(1<<PIN_INT_BIT), 0);			//G_INT farnell design

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(INT_ID, ((0x01<<3)|0x01));
	intMode = 1;
	/* Configure GPIO interrupt */
	GPIO_IntCmd(PIN_INT_NUM,PIN_INT_PORT,PIN_INT_BIT,intMode); //GPIO0[7] : P2_7
	
	/* Enable interrupt for Pin Interrupt */
	NVIC_EnableIRQ(INT_ID);
}

/*----------------- Blinky  --------------------------*/
/*********************************************************************//**
 * @brief		Test_led sub-routine 
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void Test_led()
{
	if (led_d1==1)
		GPIO_ClearValue(LED1_PORT,(1<<LED1_BIT));
	else if (led_d2 == 0)
		GPIO_SetValue(LED1_PORT,(1<<LED1_BIT));
	timer_delay_ms(30);
	GPIO_ClearValue(LED2_PORT,(1<<LED2_BIT));
	timer_delay_ms(30);
	GPIO_SetValue(LED2_PORT,(1<<LED2_BIT));	
}





/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
uint32_t tempx, tempy;

int c_entry (void) {                       /* Main Program                       */
	static int i=0;
#ifndef __CODE_RED
	SystemInit();
	CGU_Init();
#endif
 	
	scu_pinmux(0x2 ,11 , MD_PUP, FUNC0); 	// P2.11 : GPIO1_11: Xplorer BOARD (LED3)
	scu_pinmux(0x2 ,12 , MD_PUP, FUNC0); 	// P2.12 : GPIO1_12: Xplorer BOARD (LED4)
	
	GPIO_SetDir(LED1_PORT,(1<<LED1_BIT), 1);
	GPIO_ClearValue(LED1_PORT,(1<<LED1_BIT));
	
	GPIO_SetDir(LED2_PORT,(1<<LED2_BIT), 1);
	GPIO_ClearValue(LED2_PORT,(1<<LED2_BIT));
    	
	// M3Frequency is automatically set when SetClock(BASE_M3_CLK... was called.
	SysTick_Config(CGU_GetPCLKFrequency(CGU_PERIPHERAL_M3CORE)/100);  				// Generate interrupt @ 1000 Hz
	debug_frmwrk_init();
	User_switch_init();
	while(int_flag != 1);
	print_menu();
	lpc_printf("User input detected\r\n");
	Hid_init();
	lpc_printf("Initializing LPC1830 Xplorer peripherals. Please wait..!\r\n");
	int_flag = 0;
	Emac_init();
	lpc_printf("Ethernet PHY detected and initialized..!\r\n");
	lpc_printf("IP address of LPC1830 Xplorer is 192.168.1.123\r\n");

	Sdio_init();
	Audio_init();
	lpc_printf("Audio codec detected and initialized..!\r\n");
	lpc_printf("\r\n");
	while (1)
	{// Loop forever
		Test_led();				//Blinky
		Test_ethernet();
		Test_audio(); 
//	Test_sdcard();
		if(int_flag == 1)
		{
			lpc_printf("User input detected\r\n");
			LPC_GPIO_PORT->NOT[LED1_PORT] =(1<<LED1_BIT);
			int_flag = 0;
		}
		lpc_printf("Test iteration:%d\r",i++);	
	}
}


/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int main(void)
{
    return c_entry();
}

#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif

/**
 * @}
 */
