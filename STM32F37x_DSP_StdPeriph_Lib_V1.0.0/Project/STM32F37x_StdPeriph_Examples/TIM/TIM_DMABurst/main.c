/**
  ******************************************************************************
  * @file    TIM/TIM_DMABurst/main.c  
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
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
#include "stm32f37x.h"

/** @addtogroup STM32F37x_StdPeriph_Examples
  * @{
  */

/** @addtogroup TIM_DMABurst
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
#define TIM2_DMAR_ADDRESS ((uint32_t)0x4000004C) /* TIM DMAR address */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void TIM_Config(void);

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
       file (startup_stm32f37x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f37x.c file
     */ 

  /* TIM Configuration */
  TIM_Config();       
  
  /* Infinite loop */ 
  while(1)
  {
  }
}

/**
  * @brief  Configure the TIM2 to generate 1 PWM signal using the DMA burst mode.
  * @param  None
  * @retval None
  */
static void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef        TIM_OCInitStructure;
  uint16_t SRC_Buffer[3] = {0x00000FFF, 0x0000, 0x00000555};
  
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* DMA2 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* GPIOA Configuration: PA0(TIM2 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect TIM pins to AF1 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_1);
  
  /* DeInitialize the DMA2 Channel2 */
  DMA_DeInit(DMA1_Channel2);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM2_DMAR_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SRC_Buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
  DMA_InitStructure.DMA_BufferSize = 3;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  
    /* -----------------------------------------------------------------------
    TIM2 Configuration: generate 1 PWM signal using the DMA burst mode:
   
    TIM2 input clock (TIM2CLK) is set to 2*APB1 clock (PCLK1) 
      TIM2CLK = 2*PCLK1  
      PCLK1 = HCLK/2  
      => TIM2CLK = HCLK = SystemCoreClock
    
    To get TIM2 counter clock at 24 MHz, the prescaler is computed as follows:
      Prescaler = (TIM2CLK / TIM2 counter clock) - 1
      Prescaler = (SystemCoreClock /24 MHz) - 1
  
    TIM2 Frequency = TIM2 counter clock/(ARR + 1)
                                               = 24 MHz / 4096 = 5.85 KHz
    TIM2 Channel1 duty cycle = (TIM2_CCR1/ TIM2_ARR)* 100 = 33.33%
  
    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f37x.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.  
  ----------------------------------------------------------------------- */  
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;          
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 24000000) - 1;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Init OC structure */
  TIM_OCStructInit(&TIM_OCInitStructure);
  
  /* TIM Configuration in PWM Mode */
  TIM_OCInitStructure.TIM_OCMode =  TIM_OCMode_PWM1;    
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;          
  TIM_OCInitStructure.TIM_Pulse = 0xFFF;  
  TIM_OC1Init(TIM2, &TIM_OCInitStructure); 

  /* TIM2 DMAR Base register and DMA Burst Length Config */
  TIM_DMAConfig(TIM2, TIM_DMABase_ARR, TIM_DMABurstLength_3Transfers);

  /* TIM2 DMA Update enable */
  TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);

  /* TIM2 enable */
  TIM_Cmd(TIM2, ENABLE);

  /* Enable DMA2 Channel2  */
  DMA_Cmd(DMA1_Channel2, ENABLE);

  /* Wait until DMA2 Channel2 end of Transfer */
  while (!DMA_GetFlagStatus(DMA1_FLAG_TC2))
  {
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

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
