/**
  ******************************************************************************
  * @file    ADC/ADC_DualMode/main.c 
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

/** @addtogroup Dual_Mode
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC_CDR_ADDRESS    ((uint32_t)0x5000030C)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t TimingDelay = 0;
__IO uint32_t ADCDualConvertedValue;
__IO uint16_t calibration_value_1 = 0, calibration_value_2 = 0;

ADC_InitTypeDef        ADC_InitStructure;
ADC_CommonInitTypeDef  ADC_CommonInitStructure;
GPIO_InitTypeDef       GPIO_InitStructure;
DMA_InitTypeDef        DMA_InitStructure;

/* Private function prototypes -----------------------------------------------*/
void ADC_GPIO_Configuration(void);
void ADC_DMA_Configuration(void);

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
  RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);  
  /* Enable ADC1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
  
  /* LCD Display init  */
  Display_Init();

  /* ADC GPIO configuration */
  ADC_GPIO_Configuration();

  /* ADC DMA Channel configuration */
  ADC_DMA_Configuration();
      
  /* Setup SysTick Timer for 1 µsec interrupts  */
  if (SysTick_Config(SystemCoreClock / 1000000))
  { 
    /* Capture error */ 
    while (1)
    {}
  }
  
  ADC_StructInit(&ADC_InitStructure);

  /* ADC Calibration procedure */
  ADC_VoltageRegulatorCmd(ADC1, ENABLE);
  ADC_VoltageRegulatorCmd(ADC2, ENABLE);
  
  /* Insert delay equal to 10 µs */
  Delay(10);
  
  ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
  ADC_StartCalibration(ADC1);

  ADC_SelectCalibrationMode(ADC2, ADC_CalibrationMode_Single);
  ADC_StartCalibration(ADC2);
  
  while(ADC_GetCalibrationStatus(ADC1) != RESET );
  calibration_value_1 = ADC_GetCalibrationValue(ADC1);

  while(ADC_GetCalibrationStatus(ADC2) != RESET );
  calibration_value_2 = ADC_GetCalibrationValue(ADC2);

  /* ADC Dual mode configuration */     
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Interleave;                                                                    
  ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;            
  ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular;                  
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = 10;          
  
  ADC_CommonInit(ADC1, &ADC_CommonInitStructure);

  /* */  
  ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
  ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;         
  ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;   
  ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
  ADC_InitStructure.ADC_NbrOfRegChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_Init(ADC2, &ADC_InitStructure);
  
  /* ADC1 regular channel7 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_7Cycles5);
  /* ADC2 regular channel1 configuration */ 
  ADC_RegularChannelConfig(ADC2, ADC_Channel_7, 1, ADC_SampleTime_7Cycles5);

  /* Configures the ADC DMA */
  ADC_DMAConfig(ADC1, ADC_DMAMode_Circular);
  /* Enable the ADC DMA */
  ADC_DMACmd(ADC1, ENABLE);
   
  /* Enable ADC1 and ADC2 */
  ADC_Cmd(ADC1, ENABLE);
  ADC_Cmd(ADC2, ENABLE);
  
  /* wait for ADC1 ADRDY */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY));

  /* wait for ADC2 ADRDY */
  while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_RDY));
  
  /* Enable the DMA channel */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  /* Start ADC1 Software Conversion */ 
  ADC_StartConversion(ADC1); 
  
  /* Infinite loop */
  while (1)
  {
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
  uint32_t Voltage =0, mVoltage=0;
  uint8_t text[50];
  __IO uint32_t ADC1ConvertedVoltage;
  __IO uint32_t ADC2ConvertedVoltage;
  __IO uint16_t ADC1ConvertedValue;
  __IO uint16_t ADC2ConvertedValue;

  ADC1ConvertedValue = (ADCDualConvertedValue & 0xFFFF);
  ADC1ConvertedVoltage = ADC1ConvertedValue *3300/0xFFF;

  Voltage = (ADC1ConvertedVoltage)/1000;
  mVoltage = (ADC1ConvertedVoltage%1000)/100;
  sprintf((char*)text,"   ADC1 = %lu,%lu V   ",Voltage,mVoltage);
  LCD_DisplayStringLine(LINE(6),text);


  ADC2ConvertedValue = ((ADCDualConvertedValue & 0xFFFF0000) >>16);
  ADC2ConvertedVoltage = ADC2ConvertedValue *3300/0xFFF;

  Voltage=ADC2ConvertedVoltage/1000;
  mVoltage = (ADC2ConvertedVoltage%1000)/100;
  sprintf((char*)text,"   ADC2 = %lu,%lu V   ",Voltage,mVoltage);
  LCD_DisplayStringLine(LINE(7),text);
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
  LCD_DisplayStringLine(LINE(0x13), (uint8_t*)"  ADC conversion example (Dual mode)");

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
     
}

/**
  * @brief  Configures the ADCs GPIO pins.
  * @param  None
  * @retval None
  */
void ADC_GPIO_Configuration(void)
{
  /* Enable GPIOA and GPIOC Periph clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /* ADC Channels configuration */
  /* Configure ADC1/2 Channel7 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

}

/**
  * @brief  Configures the ADCs DMA channel.
  * @param  None
  * @retval None
  */
void ADC_DMA_Configuration(void)
{
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* DMA configuration */
  /* DMA1 Channel1 Init Test */
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_CDR_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCDualConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
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
