/**
  ******************************************************************************
  * @file    ADC/ADC_DMA/main.c 
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
#include "main.h"

/** @addtogroup STM32F37x_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_DMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address             0x4001244C

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t RegularConvData_Tab[4];

/* Private function prototypes -----------------------------------------------*/
static void ADC_Config(void);
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

  /* LCD Display init  */
  Display_Init();
  
  /* ADC1 channel9, TempSenso, Vrefint and Vbat with DMA configuration */
  ADC_Config();
  
  /* Infinite loop */
  while (1)
  {
    /* Test DMA1 TC flag */
    while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET ); 
    
    /* Clear DMA TC flag */
    DMA_ClearFlag(DMA1_FLAG_TC1);
    
    /* Display converted data on the LCD */
    Display();
  }
}

/**
  * @brief  ADC1 channel with DMA configuration
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
  ADC_InitTypeDef     ADC_InitStructure;
  GPIO_InitTypeDef    GPIO_InitStructure;
  DMA_InitTypeDef     DMA_InitStructure;
  
  /* ADCCLK = PCLK2/4 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div4);
    
  /* GPIOB Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
  /* Configure ADC Channel9 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
  
  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RegularConvData_Tab;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 4;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* ADC1 DeInit */  
  ADC_DeInit(ADC1);
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continuous mode */
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 4;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  /* Convert the ADC1 Channel 9 with 55.5 Cycles as sampling time */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_55Cycles5);
 
  /* Convert the ADC1 temperature sensor  with 55.5 Cycles as sampling time */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor ,2, ADC_SampleTime_55Cycles5);  
  
  /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint , 3, ADC_SampleTime_55Cycles5); 
    
  /* Convert the ADC1 Vbat with 239.5 Cycles as sampling time */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_Vbat ,4, ADC_SampleTime_239Cycles5);  
  
  ADC_TempSensorVrefintCmd(ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);  
  
  /* ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  
  while(ADC_GetResetCalibrationStatus(ADC1));
  
  /* ADC1 calibration start */
  ADC_StartCalibration(ADC1);
  
  while(ADC_GetCalibrationStatus(ADC1)); 
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE); 
}

/**
  * @brief  Display ADC converted value on LCD
  * @param  None
  * @retval None
  */
void Display(void)
{
  uint32_t v = 0, mv = 0;
  uint8_t text[50] = {0}, index = 0;
  
  /* Set the LCD Back Color and Text Color */
  LCD_SetBackColor(White);
  LCD_SetTextColor(Red);
  for(index=0;index<4;index++)
  {
    if (index == 3)
    {
      v=((2* RegularConvData_Tab[index]* 3300) / 0xFFF) / 1000;
      mv = (((2* RegularConvData_Tab[index]* 3300) / 0xFFF)%1000)/100;
    }
    else
    {
      v=((RegularConvData_Tab[index]* 3300) / 0xFFF) / 1000;
      mv = (((RegularConvData_Tab[index]* 3300) / 0xFFF)%1000)/100;
    } 
    
    if (index == 0)
    {
      sprintf((char*)text," Pot (RV3)   = %lu,%lu V   ",v,mv);
    }
    else if (index == 1)
    {
      sprintf((char*)text," V(sense)    = %lu,%lu V   ",v,mv);
    }
     else if (index == 2)
    {
      sprintf((char*)text," V(ref int)  = %lu,%lu V   ",v,mv);
    }
     else
    {
      sprintf((char*)text," V(Battery)  = %lu,%lu V",v,mv);
    }
    
    LCD_DisplayStringLine(LINE(5+index),text);
  } 
}

/**
  * @brief  Display Init (LCD)
  * @param  None
  * @retval None
  */
void Display_Init(void)
{
  /* Initialize the LCD */
  STM32373C_LCD_Init();

  /* Clear the LCD */ 
  LCD_Clear(White);

  /* Set the LCD Text size */
  LCD_SetFont(&Font8x12);

  /* Set the LCD Back Color and Text Color */
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);

  /* Display */
  LCD_DisplayStringLine(LINE(0x13), (uint8_t*)"         ADC DMA conversion example         ");

  /* Set the LCD Text size */
  LCD_SetFont(&Font16x24);

  LCD_DisplayStringLine(LINE(0), (uint8_t*)" STM32F37x CortexM4 ");
  LCD_DisplayStringLine(LINE(1), (uint8_t*)"   STM32373C-EVAL   ");
  
  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue);

  /* Display */
  LCD_DisplayStringLine(LINE(3), (uint8_t*)"  Turn RV3(PB.01)    ");
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
