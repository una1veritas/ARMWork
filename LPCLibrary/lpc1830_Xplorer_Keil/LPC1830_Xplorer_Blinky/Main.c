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
#include "lpc18xx_libcfg.h"
#include "lpc18xx_cgu.h"
#include "lpc18xx_utils.h"


/* Example group ----------------------------------------------------------- */
/** @defgroup Gpio_LedBlinky	Gpio_LedBlinky
 * @ingroup GPIO_Examples
 * @{
 */


/************************** PRIVATE DEFINITIONS *************************/
#define LED1_BIT			11 
#define LED1_PORT			1
#define LED2_BIT			12 
#define LED2_PORT			1
#define LED_DELAY 		100
/************************** PRIVATE VARIABLES *************************/





/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
uint32_t tempx, tempy;

int c_entry (void) {                       /* Main Program                       */
	SystemInit();
	CGU_Init();

  scu_pinmux(0x2 ,11 , MD_PUP, FUNC0); 	// P2.11 : GPIO1_11: Xplorer BOARD (LED3)
	GPIO_SetDir(LED1_PORT,(1<<LED1_BIT), 1);
	GPIO_ClearValue(LED1_PORT,(1<<LED1_BIT));
	scu_pinmux(0x2 ,12 , MD_PUP, FUNC0); 	// P2.12 : GPIO1_12: Xplorer BOARD (LED4)
	GPIO_SetDir(LED2_PORT,(1<<LED2_BIT), 1);
	GPIO_ClearValue(LED2_PORT,(1<<LED2_BIT));

	while (1)
	{                           					// Loop forever
		timer_delay_ms(LED_DELAY);
		GPIO_ClearValue(LED1_PORT,(1<<LED1_BIT));
		timer_delay_ms(LED_DELAY);
		GPIO_ClearValue(LED2_PORT,(1<<LED2_BIT));
		GPIO_SetValue(LED1_PORT,(1<<LED1_BIT));
		timer_delay_ms(LED_DELAY);
		GPIO_SetValue(LED2_PORT,(1<<LED2_BIT));

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
