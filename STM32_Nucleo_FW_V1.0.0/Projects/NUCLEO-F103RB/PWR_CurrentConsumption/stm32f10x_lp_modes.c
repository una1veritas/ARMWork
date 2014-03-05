/**
  ******************************************************************************
  * @file    stm32f10x_lp_modes.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the STM32F10x Low Power Modes:
  *           - Sleep Mode
  *           - STOP mode with RTC
  *           - STANDBY mode with RTC
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#include "stm32f10x_lp_modes.h"
    
/** @addtogroup PWR_CurrentConsumption
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function configures the system to enter Sleep mode for
  *         current consumption measurement purpose.
  *         Sleep Mode
  *         ==========  
  *            - System Running at PLL (72MHz)
  *            - Flash 1 wait states
  *            - Prefetch and Cache enabled
  *            - Code running from Internal FLASH
  *            - All peripherals disabled except PC13 (used for User button)
  *            - Wakeup using EXTI Line (KEY Button PC.13)
  * @param  None
  * @retval None
  */
void SleepMode_Measure(void)
{
  __IO uint32_t index = 0;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                        RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
                        RCC_APB2Periph_GPIOG, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_Init(GPIOF, &GPIO_InitStructure); 
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* Disable GPIOs clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
                         RCC_APB2Periph_GPIOG, DISABLE);

  /* Configure User button, will be used to exit system from Sleep mode */
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Request to enter SLEEP mode */
  __WFI();

  /* Initialize LED2 on STM32NUCLEO board */
  STM_EVAL_LEDInit(LED2);

  /* Infinite loop */
  while (1)
  {
    /* Toggle The LED2 */
    STM_EVAL_LEDToggle(LED2);

    /* Inserted Delay */
    for(index = 0; index < 0x7FFFF; index++);
  }
}

/**
  * @brief  This function configures the system to enter Stop mode with RTC 
  *         clocked by LSE for current consumption measurement purpose.
  *         STOP Mode with RTC clocked by LSI
  *         =====================================   
  *           - RTC Clocked by LSI
  *           - Regulator in LP mode
  *           - HSI, HSE OFF and LSI OFF if not used as RTC Clock source
  *           - No IWDG
  *           - FLASH in deep power down mode
  *           - Automatic Wakeup using RTC clocked by LSI (~15s)
  * @param  None
  * @retval None
  */
void StopMode_Measure(void)
{
  __IO uint32_t index = 0;
  
  /* Alarm in 5 second */
  RTC_SetAlarm(RTC_GetCounter()+ 5);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  
  /* Enter Stop Mode */
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
  
  /* Initialize LED2 on STM32NUCLEO board */
  STM_EVAL_LEDInit(LED2);
  
  /* Infinite loop */
  while (1)
  {
    /* Toggle The LED2 */
    STM_EVAL_LEDToggle(LED2);
    
    /* Inserted Delay */
    for(index = 0; index < 0x5FFFF; index++);
  }
}

/**
  * @brief  This function configures the system to enter Standby mode with RTC 
  *         clocked by LSI for current consumption measurement purpose.
  *         STANDBY Mode with RTC clocked by LSI
  *         ========================================
  *           - RTC Clocked by LSI
  *           - IWDG OFF
  *           - Automatic Wakeup using RTC 
  * @param  None
  * @retval None
  */
void StandbyRTCMode_Measure(void)
{  
    /* Alarm in 5 second */
    RTC_SetAlarm(RTC_GetCounter()+ 5);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    
  /* Clear Wakeup flag */
  PWR_ClearFlag(PWR_FLAG_WU);      
    
  /* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
  PWR_EnterSTANDBYMode();
  
  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
