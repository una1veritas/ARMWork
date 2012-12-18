/**
  ******************************************************************************
  * @file    OPAMP/OPAMP_TimerControlledMux/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
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

/** @addtogroup STM32F30x_StdPeriph_Examples
  * @{
  */

/** @addtogroup OPAMP_TimerControlledMux
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
float ADCVal[BUFFER_SIZE] = {0}; /* Add "ADCVal" to live watch to display the measured voltage */
__IO uint32_t TimingDelay = 0;

/* Private function prototypes -----------------------------------------------*/
static void OPAMP1_Config(void);
static void TIM1_Config(void);
static void ADC1_Config(void);
static void Delay(__IO uint32_t nTime);

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
       file (startup_stm32f30x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f30x.c file
     */
  
  /* OPAMP1 configuration */
  OPAMP1_Config();
  
  /* TIM1 configuration */
  TIM1_Config();
  
  /* ADC1 configuration: Channel 3 (PA2) using TIM1 update event as trigger source,
     End Of Conversion (EOC) interrupt is enabled */
  ADC1_Config();
  
  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);
  
  /* forever loop */
  while(1);
}

/**
  * @brief  Configures OPAMP1 in timer-controlled multiplexer
  * @param  None
  * @retval None
  */
static void OPAMP1_Config(void)
{
  OPAMP_InitTypeDef       OPAMP_InitStructure;
  GPIO_InitTypeDef        GPIO_InitStructure;
  
  /* GPIOA and GPIOC clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOC, ENABLE);
  
  /* Configure PA2, PA5 and PA7 in analog mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure PC5 in analog mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* OPAMP Peripheral clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Select inverting input connected to IO1 (PC5) and non inverting input IO1 (PA7) */
  OPAMP_InitStructure.OPAMP_InvertingInput = OPAMP_InvertingInput_IO1;
  OPAMP_InitStructure.OPAMP_NonInvertingInput = OPAMP_NonInvertingInput_IO1;
  OPAMP_Init(OPAMP_Selection_OPAMP1, &OPAMP_InitStructure);
  
  /* Configure the secondary non-inverting input IO2 (PA5) */
  OPAMP_InitStructure.OPAMP_InvertingInput = OPAMP_InvertingInput_IO1;
  OPAMP_InitStructure.OPAMP_NonInvertingInput = OPAMP_NonInvertingInput_IO2;
  OPAMP_TimerControlledMuxConfig(OPAMP_Selection_OPAMP1, &OPAMP_InitStructure);
  
  /* Enable the timer-controlled mux mode */
  OPAMP_TimerControlledMuxCmd(OPAMP_Selection_OPAMP1, ENABLE);
  
  /* Enable OPAMP1 */
  OPAMP_Cmd(OPAMP_Selection_OPAMP1, ENABLE);
}

/**
  * @brief  Configure TIM1 to trigger ADC conversion
  * @param  None
  * @retval None
  */
static void TIM1_Config(void)
{
  /* Init Structure definition */
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  
  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  /* Time Base configuration: Channel 6 frequency is APB2 clock / ((prscaler+1) * period)
     = 72000000 / (20 * 65535) = 54 Hz */
  TIM_TimeBaseStructure.TIM_Prescaler = 19;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned3;
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  
  /* TIM1 Channel 6 configuration in PWM mode: duty cycle is pulse / period
     = 100 * (32767 / 65535) =  50% */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 32767;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  TIM_OC6Init(TIM1, &TIM_OCInitStructure);
  
  /* Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
  
  /* Select update event as trigger source: This is used to trigger ADC conversion */
  TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
}

/**
  * @brief  ADC1 configuration.
  * @param  None
  * @retval None
  */
static void ADC1_Config(void)
{
  __IO uint16_t calibration_value = 0x00;
  
  ADC_InitTypeDef   ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;  
  NVIC_InitTypeDef  NVIC_InitStructure;

  /* Configure the ADC clock */
  RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);
  
  /* Enable ADC1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);

  /* Setup SysTick Timer for 1 µsec interrupts  */
  if (SysTick_Config(SystemCoreClock / 1000000))
  { 
    /* Capture error */ 
    while (1)
    {}
  }  
  
   /* GPIO PA2 configuration is done within OPAMP1_Config function
      as the ADC channel3 is the OPAMP1 output */  
  /* Init ADC_InitStructure */
  ADC_StructInit(&ADC_InitStructure);

  /* Calibration procedure */  
  ADC_VoltageRegulatorCmd(ADC1, ENABLE);
  
  /* Insert delay equal to 10 µs */
  Delay(10);
  
  /* Select single calibration mode */
  ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
  ADC_StartCalibration(ADC1);
  
  /* Wait for calibration flag to be reset */
  while(ADC_GetCalibrationStatus(ADC1) != RESET );
  calibration_value = ADC_GetCalibrationValue(ADC1);
  
  /* Configure the ADC1 in continuous mode */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                                                                    
  ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;             
  ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;                  
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
  ADC_CommonInit(ADC1, &ADC_CommonInitStructure);
  
  /* Select Event9 (TIM1 TRGO)s as trigger source */
  ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_9;
  ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;
  ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;
  ADC_InitStructure.ADC_NbrOfRegChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  /* ADC1 regular channel3 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_7Cycles5);
  
  /* Enable EOC interupt */
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  
  /* Enable ADC1 IRQ */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  /* wait for ADRDY */
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY) == RESET);
  
  /* ADC1 start Conversion */
  ADC_StartConversion(ADC1);
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
static void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
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
