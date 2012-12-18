/**
  ******************************************************************************
  * @file    ADC/ADC_AnalogWatchdog/main.c 
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

/** @addtogroup AWDG_Mode
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MESSAGE1   " STM32F30x  CortexM4" 
#define MESSAGE2   "   STM32303C-EVAL   " 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;
__IO uint32_t TimingDelay = 0, calibration_value = 0;
ADC_InitTypeDef       ADC_InitStructure;
ADC_CommonInitTypeDef ADC_CommonInitStructure;
GPIO_InitTypeDef    GPIO_InitStructure;
NVIC_InitTypeDef    NVIC_InitStructure;

/* Private function prototypes -----------------------------------------------*/
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
  
  /* Configure the ADC clock */
  RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div128);
  
  /* Enable ADC1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
  
  /* LCD Display init  */
  Display_Init();

  /* Initialize LED4 */
  STM_EVAL_LEDInit(LED4);
      
  /* Setup SysTick Timer for 1 µsec interrupts  */
  if (SysTick_Config(SystemCoreClock / 1000000))
  { 
    /* Capture error */ 
    while (1)
    {}
  }
  
  /* ADC Channel configuration */
  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /* Configure ADC Channel7 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  ADC_StructInit(&ADC_InitStructure);

  /* Calibration procedure */ 
  ADC_VoltageRegulatorCmd(ADC1, ENABLE);
  
  /* Insert delay equal to 10 µs */
  Delay(10);
  
  ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
  ADC_StartCalibration(ADC1);
  
  while(ADC_GetCalibrationStatus(ADC1) != RESET );
  calibration_value = ADC_GetCalibrationValue(ADC1);
     
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
  ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Enable;   
  ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
  ADC_InitStructure.ADC_NbrOfRegChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  /* ADC1 regular channel7 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_7Cycles5);

  /* Analog watchdog 2 configuration */  
  /* Configure channel7 as the single analog watchdog guarded channel */
  ADC_AnalogWatchdog2SingleChannelConfig(ADC1, ADC_Channel_7);

  /* Configure the ADC Thresholds between 1.5V and 0.8V */
  ADC_AnalogWatchdog2ThresholdsConfig(ADC1, 0x7F, 0x3F);
  
  /* Configure and enable ADC1 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;   	 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable AWD interrupt */
  ADC_ITConfig(ADC1, ADC_IT_AWD2, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  /* wait for ADRDY */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY));
  
  /* Start ADC1 Software Conversion */ 
  ADC_StartConversion(ADC1);   
  
  /* Infinite loop */
  while (1)
  {
     /* Get ADC1 converted data */
     ADC1ConvertedValue = ADC_GetConversionValue(ADC1);
    
     /* Compute the voltage */
     ADC1ConvertedVoltage = (ADC1ConvertedValue *3300)/0xFFF;  
     /* Display converted data on the LCD */
     Display();
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
  * @brief  Display ADC converted value on LCD
  * @param  None
  * @retval None
  */
void Display(void)
{
  uint32_t v=0,mv=0;
  uint8_t text[50];

  v=(ADC1ConvertedVoltage)/1000;
  mv = (ADC1ConvertedVoltage%1000)/100;
  sprintf((char*)text,"   ADC = %d,%d V   ",v,mv);
  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue);
  
  LCD_DisplayStringLine(LINE(6),text);
}

/**
  * @brief  Display Init (LCD)
  * @param  None
  * @retval None
  */
void Display_Init(void)
{
  /* Initialize the LCD */
  STM32303C_LCD_Init();

  /* Clear the LCD */ 
  LCD_Clear(White);

  /* Set the LCD Text size */
  LCD_SetFont(&Font8x12);

  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);

  /* Display */
  LCD_DisplayStringLine(LINE(0x13), (uint8_t*)"  ADC conversion example (AWDG example)");

  /* Set the LCD Text size */
  LCD_SetFont(&Font16x24);

  LCD_DisplayStringLine(LINE(0), (uint8_t*)"STM32F303x CortexM4 ");
  LCD_DisplayStringLine(LINE(1), (uint8_t*)"   STM32303C-EVAL   ");
  
  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue);

  /* Display */
  LCD_DisplayStringLine(LINE(3),(uint8_t*)"  Turn RV2(PC.01)    ");
  LCD_DisplayStringLine(LINE(4),(uint8_t*)"   Potentiometer     ");

   /* Set the LCD Text size */
  LCD_SetFont(&Font12x12);
  LCD_SetTextColor(Red);
  
  LCD_DisplayStringLine(LINE(16), (uint8_t*)"AWD High threshold = 1.5 V");
  LCD_DisplayStringLine(LINE(17), (uint8_t*)"AWD Low threshold  = 0.8 V");
  
  /* Set the LCD Text size */
  LCD_SetFont(&Font16x24); 
  LCD_SetTextColor(Blue);
     
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
