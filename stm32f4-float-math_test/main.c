/**
  * Modified by Wolfgang Wieser to enable the FPU and compute the Mandelbrot 
  * set on the FPU as well as a sqrtf() benchmark. 
  * 
  ******************************************************************************
  * @file    IO_Toggle/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "mandelbrot.h"

#include "gpio_digital.h"
#include "systick.h"
#include "usart.h"

//GPIO_InitTypeDef  GPIO_InitStructure;


#define nop()       __asm__ __volatile__("nop")


int main(void) {

	SysTick_Start();

	GPIOMode(GPIOD, GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15,
			GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);

	usart_begin(19200);
	usart_print("Hello!\n");

  // NOTE: Important: Enable full access to FPU: 
  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

  while (1)
  {

	int bits = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIOD->BSRRL = bits;  // Set bits
    nop();nop();nop();nop();nop();nop();nop();nop();nop();
	GPIOD->BSRRH = bits;  // Reset bits
    nop();nop();nop();nop();nop();nop();nop();nop();nop();
	nop();
	GPIOD->BSRRL = bits;  // Set bits
    nop();nop();nop();nop();nop();nop();nop();nop();nop();
	GPIOD->BSRRH = bits;  // Reset bits

	delay(100);
	
#if 1
    /* PD12 to be toggled */
    //GPIO_SetBits(GPIOD, GPIO_Pin_12);
    digitalWrite(PD12, HIGH);
    
    /* Insert delay */
    delay(20); //(0x1FFFFF);
    
    /* PD13 to be toggled */
//    GPIO_SetBits(GPIOD, GPIO_Pin_13);
    digitalWrite(PD13, HIGH);

    /* Insert delay */
    delay(20); //(0x1FFFFF);
  
    /* PD14 to be toggled */
//    GPIO_SetBits(GPIOD, GPIO_Pin_14);
    digitalWrite(PD14, HIGH);
    
    /* Insert delay */
    delay(20); //(0x1FFFFF);
    
    /* PD15 to be toggled */
//    GPIO_SetBits(GPIOD, GPIO_Pin_15);
    digitalWrite(PD15, HIGH);

    /* Insert delay */
    delay(40); //(0x3FFFFF);
    
   // GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
    digitalWrite(PD12, LOW);
    digitalWrite(PD13, LOW);
    digitalWrite(PD14, LOW);
    digitalWrite(PD15, LOW);
    
    /* Insert delay */
    delay(200); //(0xFFFFFF);
#endif
	
	ComputeMandelbrot(4000,50);
	usart_printFloat(systicks()/1000.0f, 3);
	usart_print(".\n");

	SqrtBenchmark();
  }
}


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

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
