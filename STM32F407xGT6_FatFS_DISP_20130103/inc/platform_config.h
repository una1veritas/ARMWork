/********************************************************************************/
/*!
	@file			platform_config.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2011.06.12
	@brief          Based on ST Microelectronics's Sample Thanks!

    @section HISTORY
		2011.06.12	V1.00	Start Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H 0x0100

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#ifndef BSRR
#define BSRR BSRRL			/* Set   */
#define BRR  BSRRH			/* Reset */
#endif


/* Exported types ------------------------------------------------------------*/
#if defined(USE_REDBULL)
/* RED BULL Key Inputs */
#define KEY_PORT_USER1				(GPIOA)
#define KEY_PORT_USER2				(GPIOD)
#define KEY_PORT_TAMPER				(GPIOC)
#define KEY_PORT_WAKEUP				(GPIOA)

#define KEY_CLK_USER1				(RCC_AHB1Periph_GPIOA)
#define KEY_CLK_USER2				(RCC_AHB1Periph_GPIOD)
#define KEY_CLK_TAMPER				(RCC_AHB1Periph_GPIOC)
#define KEY_CLK_WAKEUP				(RCC_AHB1Periph_GPIOA)

#define K_USER1		       			(GPIO_Pin_8)
#define K_USER2       				(GPIO_Pin_3)
#define K_TAMPER		       		(GPIO_Pin_13)
#define K_WAKEUP       				(GPIO_Pin_0)

#define K_USER1_SFT					7
#define K_USER2_SFT					1
#define K_TAMPER_SFT				10
#define K_WAKEUP_SFT				0

#define KEY_U		       			(1<<2)	/* K_USER2 */
#define KEY_D       				(1<<1)	/* K_USER1 */		
#define KEY_R		       			(1<<3)	/* K_TAMPER */
#define KEY_L       				(1<<0)	/* K_WAKEUP */

#define LED_PORT_LED1				(GPIOF)
#define LED_PORT_LED2				(GPIOF)
#define LED_PORT_LED3				(GPIOF)
#define LED_PORT_LED4				(GPIOF)
#define LED_PORT_LED5				(GPIOF)

#define LED_CLK_LED1				(RCC_AHB1Periph_GPIOF)
#define LED_CLK_LED2				(RCC_AHB1Periph_GPIOF)
#define LED_CLK_LED3				(RCC_AHB1Periph_GPIOF)
#define LED_CLK_LED4				(RCC_AHB1Periph_GPIOF)
#define LED_CLK_LED5				(RCC_AHB1Periph_GPIOF)

#define LED1						(GPIO_Pin_6)					
#define LED2						(GPIO_Pin_7)			
#define LED3						(GPIO_Pin_8)
#define LED4						(GPIO_Pin_9)
#define LED5						(GPIO_Pin_10)

#define LED1_On()					(LED_PORT_LED1->BRR = LED1)
#define LED2_On()					(LED_PORT_LED2->BRR = LED2)
#define LED3_On()					(LED_PORT_LED3->BRR = LED3)
#define LED4_On()					(LED_PORT_LED4->BRR = LED4)
#define LED5_On()					(LED_PORT_LED5->BRR = LED5)

#define LED1_Off()					(LED_PORT_LED1->BSRR = LED1)
#define LED2_Off()					(LED_PORT_LED2->BSRR = LED2)
#define LED3_Off()					(LED_PORT_LED3->BSRR = LED3)
#define LED4_Off()					(LED_PORT_LED4->BSRR = LED4)
#define LED5_Off()					(LED_PORT_LED5->BSRR = LED5)

#elif defined(USE_STM32F4DISCOVERY)

#define LED_PORT_LED1				(GPIOD)
#define LED_PORT_LED2				(GPIOD)
#define LED_PORT_LED3				(GPIOD)
#define LED_PORT_LED4				(GPIOD)

#define LED_CLK_LED1				(RCC_AHB1Periph_GPIOD)
#define LED_CLK_LED2				(RCC_AHB1Periph_GPIOD)
#define LED_CLK_LED3				(RCC_AHB1Periph_GPIOD)
#define LED_CLK_LED4				(RCC_AHB1Periph_GPIOD)

#define LED1						(GPIO_Pin_12)						
#define LED2						(GPIO_Pin_13)				
#define LED3						(GPIO_Pin_14)
#define LED4						(GPIO_Pin_15)

#define LED1_On()					(LED_PORT_LED1->BSRR = LED1)
#define LED2_On()					(LED_PORT_LED2->BSRR = LED2)
#define LED3_On()					(LED_PORT_LED3->BSRR = LED3)
#define LED4_On()					(LED_PORT_LED4->BSRR = LED4)

#define LED1_Off()					(LED_PORT_LED1->BRR = LED1)
#define LED2_Off()					(LED_PORT_LED2->BRR = LED2)
#define LED3_Off()					(LED_PORT_LED3->BRR = LED3)
#define LED4_Off()					(LED_PORT_LED4->BRR = LED4)

#endif

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif	/* __PLATFORM_CONFIG_H */
