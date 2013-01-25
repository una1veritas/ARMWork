/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MESSAGE1   "     STM32F4xx      " 
#define MESSAGE2   " Device running on  " 
#define MESSAGE3   " STM3240_41_G-EVAL  " 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

usart stdserial;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
	char tmp[64];
	
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */  

	armcore_init();
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
//  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	
	usart_init(&stdserial, USART3, PB11, PB10, 57600);
	sprintf(tmp, "Hello, %ld\n", RCC_Clocks.HCLK_Frequency);
	usart_print(&stdserial, tmp);
	
  /* Initialize LEDs and LCD available on STM324xG-EVAL board *****************/
//  STM_EVAL_LEDInit(LED1);
//  STM_EVAL_LEDInit(LED2);
//  STM_EVAL_LEDInit(LED3);
//  STM_EVAL_LEDInit(LED4);

  /* Initialize the LCD */
//  STM324xG_LCD_Init();

  /* Display message on STM324xG-EVAL LCD *************************************/
  /* Clear the LCD */ 
//  LCD_Clear(White);

  /* Set the LCD Back Color */
//  LCD_SetBackColor(Blue);
  /* Set the LCD Text Color */
//  LCD_SetTextColor(White);
//  LCD_DisplayStringLine(LINE(0), (uint8_t *)MESSAGE1);
//  LCD_DisplayStringLine(LINE(1), (uint8_t *)MESSAGE2);
//  LCD_DisplayStringLine(LINE(2), (uint8_t *)MESSAGE3);

  /* Turn on LEDs available on STM324xG-EVAL **********************************/
//  STM_EVAL_LEDOn(LED1);
//  STM_EVAL_LEDOn(LED2);
//  STM_EVAL_LEDOn(LED3);
//  STM_EVAL_LEDOn(LED4);

  /* Add your application code here
     */

  /* Infinite loop */
  while (1)
  {
    /* Toggle LD4 */
//    STM_EVAL_LEDToggle(LED4);

    /* Insert 50 ms delay */
    delay(50);

    /* Toggle LD2 */
//    STM_EVAL_LEDToggle(LED2);

    /* Insert 50 ms delay */
    delay(50);
  }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
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

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
