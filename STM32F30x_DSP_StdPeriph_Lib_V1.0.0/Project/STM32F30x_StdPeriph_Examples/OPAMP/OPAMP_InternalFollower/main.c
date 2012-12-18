/**
  ******************************************************************************
  * @file    OPAMP/OPAMP_InternalFollower/main.c 
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
#include "stm32f30x.h"

/** @addtogroup OPAMP_InternalFollower
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO float ADCVal = 0; /* Add "ADCVal" to live watch to display the measured voltage */
__IO uint32_t TimingDelay = 0;
/* Private function prototypes -----------------------------------------------*/
static void ADC_Config(void);
static void OPAMP_Config(void);
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

  /* OPAMP1 configuration: OPAMP1 in internal follower mode, OPAMP1 output is 
     internally connected to ADC1 Channel 3 (PA2) */
  OPAMP_Config();

  /* ADC1 configuration: Channel 3 (PA2) in continuous mode using software start conversion,
     End Of Conversion (EOC) interrupt is enabled */
  ADC_Config();

  while (1)
  {
  }
}

/**
  * @brief  ADC configuration.
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
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
  
  /* ADC Channel3 configuration */
   /* GPIO PA0 configuration is done within OPAMP_Config function
      as the ADC channel3 is the OPAMP1 output*/
  
  ADC_StructInit(&ADC_InitStructure);

  /* Calibration procedure */
  ADC_VoltageRegulatorCmd(ADC1, ENABLE);
  
  /* Insert delay equal to 10 µs */
  Delay(10);
  
  ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
  ADC_StartCalibration(ADC1);
  
  while(ADC_GetCalibrationStatus(ADC1) != RESET );
  calibration_value = ADC_GetCalibrationValue(ADC1);

  /* Configure the ADC1 in continuous mode */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                                                                    
  ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;             
  ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;                  
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;      
  ADC_CommonInit(ADC1, &ADC_CommonInitStructure);
  
  ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
  ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;         
  ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;
  ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
  ADC_InitStructure.ADC_NbrOfRegChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  /* ADC1 regular channel3 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_1Cycles5);
  
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
  * @brief  OPAMP configuration.
  * @param  None
  * @retval None
  */
static void OPAMP_Config(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  OPAMP_InitTypeDef OPAMP_InitStructure;

  /* GPIOA Peripheral clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* Configure OPAMP1 non inverting input IO1 (PA7)
           and OPAMP1 Out (PA2) in analog mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Select Vout as inverting input (internal follower) for OPAMP1 */
  OPAMP_InitStructure.OPAMP_InvertingInput = OPAMP_InvertingInput_Vout;
  /* Select IO1 (PA7) as non inverting input for OPAMP1 */
  OPAMP_InitStructure.OPAMP_NonInvertingInput = OPAMP_NonInvertingInput_IO1;
  OPAMP_Init(OPAMP_Selection_OPAMP1, &OPAMP_InitStructure);

  /* Enable OPAMP1 */
  OPAMP_Cmd(OPAMP_Selection_OPAMP1, ENABLE);
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
