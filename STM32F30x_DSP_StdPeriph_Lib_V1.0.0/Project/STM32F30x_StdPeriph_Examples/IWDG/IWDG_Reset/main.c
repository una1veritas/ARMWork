/**
  ******************************************************************************
  * @file    IWDG/IWDG_Reset/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
  * @brief   Main program body.
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

/** @addtogroup STM32F30x_StdPeriph_Examples
  * @{
  */

/** @addtogroup IWDG_Reset
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t TimingDelay = 0;
__IO uint32_t LsiFreq = 42000;
extern __IO uint16_t CaptureNumber;

/* Private function prototypes -----------------------------------------------*/
static void TIM16_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f30x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f30x.c file
     */     
       
  /* Initialize LED1 and Key Button mounted on STM32303C-EVAL board */       
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_EXTI);

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }


  /* Check if the system has resumed from IWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    /* IWDGRST flag set */
    /* Turn on LED1 */
    STM_EVAL_LEDOn(LED1);

    /* Clear reset flags */
    RCC_ClearFlag();
  }
  else
  {
    /* IWDGRST flag is not set */
    /* Turn off LED1 */
    STM_EVAL_LEDOff(LED1);
  }

#ifdef LSI_TIM_MEASURE
  /* TIM Configuration -------------------------------------------------------*/
  TIM16_Config();
  
  /* Wait until the TIM16 get 2 LSI edges */
  while(CaptureNumber != 2)
  {
  }

  /* Disable TIM16 CC1 Interrupt Request */
  TIM_ITConfig(TIM16, TIM_IT_CC1, DISABLE);
#endif /* LSI_TIM_MEASURE */
  
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_32);

  /* Set counter reload value to obtain 250ms IWDG TimeOut.
     Counter Reload Value = 250ms * IWDG counter clock period
                          = 250ms * (LSI/32)
                          = 0.25s * (LsiFreq/32)
                          = LsiFreq / (32 * 4)
                          = LsiFreq / 128
   */
  IWDG_SetReload(LsiFreq/128);

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();

  while (1)
  {
    /* Toggle LED2 */
    STM_EVAL_LEDToggle(LED2);

    /* Insert 240 ms delay */
    Delay(240);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();  
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

#ifdef LSI_TIM_MEASURE
/**
  * @brief  Configures TIM16 to measure the LSI oscillator frequency.
  * @param  None
  * @retval None
  */
static void TIM16_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  
  /* Enable peripheral clocks ------------------------------------------------*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to the RTC */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset RTC Domain */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
  
  /*!< LSI Enable */
  RCC_LSICmd(ENABLE);
  
  /*!< Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
  
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
   
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
  
  /* Enable TIM16 clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);  
  
  /* Enable the TIM16 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Configure TIM16 prescaler */
  TIM_PrescalerConfig(TIM16, 0, TIM_PSCReloadMode_Immediate);
  
  /* Connect internally the TM16_CH1 Input Capture to the LSI clock output */
//  TIM_RemapConfig(TIM16, TIM16_RTC_CLK);
  TIM16->OR = 0x3;
  RCC_MCOConfig(RCC_MCOSource_LSI);
  
  /* TIM16 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM16 CH1
     The Rising edge is used as active edge,
     The TIM16 CCR1 is used to compute the frequency value 
  ------------------------------------------------------------ */
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM16, &TIM_ICInitStructure);
  
  /* TIM16 Counter Enable */
  TIM_Cmd(TIM16, ENABLE);

  /* Reset the flags */
  TIM16->SR = 0;
    
  /* Enable the CC1 Interrupt Request */  
  TIM_ITConfig(TIM16, TIM_IT_CC1, ENABLE);  
}
#endif

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
  {}
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
