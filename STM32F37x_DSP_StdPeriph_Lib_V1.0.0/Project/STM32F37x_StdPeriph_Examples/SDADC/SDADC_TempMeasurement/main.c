/**
  ******************************************************************************
  * @file    SDADC/SDADC_TempMeasurement/main.c 
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

/** @addtogroup SDADC_TempMeasurement
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO int16_t AvrgRegularConvData = 0;
__IO uint32_t TimingDelay = 0;

/* Private function prototypes -----------------------------------------------*/
static uint32_t SDADC_Config(void);
static float SDADC_Calibration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  NVIC_InitTypeDef NVIC_InitStructure;
  float CoeffCorrection = 1.0f;
  float TemperaturePT100 = 0;
  char LCDstr[22] = {0};
  
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f37x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f37x.c file
     */  
  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  
  /* Configure the SEL button in GPIO mode */
  STM_EVAL_PBInit(BUTTON_SEL, BUTTON_MODE_GPIO);
  
  /* Initialize the STM32373C-EVAL LCD */
  STM32373C_LCD_Init();
  /* Clear the LCD */ 
  LCD_Clear(LCD_COLOR_WHITE);
  /* Set the LCD Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_0, (uint8_t *)MESSAGE1);
  LCD_DisplayStringLine(LCD_LINE_1, (uint8_t *)MESSAGE2);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(LCD_COLOR_WHITE);
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_RED);
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_RED);
  LCD_DisplayStringLine(LCD_LINE_4, (uint8_t *)"Calibration phase:  ");
  LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"1. Set JP18 in 2-3  ");
  LCD_DisplayStringLine(LCD_LINE_6, (uint8_t *)"2. Press SEL button ");
  
  /* Wait for SEL button to be pressed */
  while(STM_EVAL_PBGetState(BUTTON_SEL) != SET);
  
  /* Wait for SEL button to be released */
  while(STM_EVAL_PBGetState(BUTTON_SEL) == SET);
  
  /* Configure the SDADC in single mode */
  if(SDADC_Config() != 0)
  {
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_BLUE);
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_DisplayStringLine(LCD_LINE_4, (uint8_t*)"Fail to configure   ");
    LCD_DisplayStringLine(LCD_LINE_5, (uint8_t*)"SigmaDelta Converter");
    /* Forever loop */
    while(1);
  }
  else
  {
    /* PT100_SDADC calibration for temperature measurement */
    CoeffCorrection = SDADC_Calibration();
  }
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_MAGENTA);
  LCD_DisplayStringLine(LCD_LINE_4, (uint8_t *)"Measurement phase:  ");
  LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"1. Set JP18 in 1-2  ");  
  LCD_DisplayStringLine(LCD_LINE_6, (uint8_t *)"2. Press SEL button ");
  /* Wait for SEL button to be pressed  */
  while(STM_EVAL_PBGetState(BUTTON_SEL) != SET);
    
  /* Enable continuous mode */
  SDADC_ContinuousModeCmd(PT100_SDADC, ENABLE);
  /* Enable regular end of conversion interrupt */
  SDADC_ITConfig(PT100_SDADC, SDADC_IT_REOC, ENABLE);
  
  /* NVIC Configuration */
  NVIC_InitStructure.NVIC_IRQChannel = SDADC1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable software start conversion of the selected regular channel */
  SDADC_SoftwareStartConv(PT100_SDADC);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(LCD_COLOR_WHITE);
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_BLUE);
  LCD_ClearLine(LCD_LINE_4);
  LCD_ClearLine(LCD_LINE_5);
  LCD_ClearLine(LCD_LINE_6);
  
  /* Infinite loop */
  while (1)
  {
    /* Compute the temperature value */
    TemperaturePT100 = (float)((((CoeffCorrection * (AvrgRegularConvData/SDADC_GAIN) * REFERENCE_RESISTOR * 2) / SDADCRESOL) - RESISTANCE_ZERODEGRE) / RESISTANCE_COEFFICIENT);
    sprintf(LCDstr, "Temperature = %2.1f C ", TemperaturePT100);
    LCD_DisplayStringLine(LCD_LINE_6, (uint8_t*)LCDstr);
  }
}

/**
  * @brief  Configure PT100_SDADC channel 3 in Single Ended Offset mode.
  * @param  None
  * @retval 0: SDADC Configured successfully
  *         1: INITRDY flag is not set, check the SDVDDA and SDVREF
  *         2: EOCAL flag is not set
  */
static uint32_t SDADC_Config(void)
{
  SDADC_InitTypeDef SDADC_InitStructure;
  SDADC_AINStructTypeDef SDADC_AINStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  uint32_t SDADCTimeout = 0;

  /* PT100_SDADC APB2 interface clock enable */
  RCC_APB2PeriphClockCmd(PT100_SDADC_CLK, ENABLE);
  
  /* DeInit PT100_SDADC */
  SDADC_DeInit(PT100_SDADC);
  
  /* PWR APB1 interface clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* Enable PT100_SDADC analog interface */
  PWR_SDADCAnalogCmd(PT100_SDADC_PWR, ENABLE);
  
  /* Set the SDADC divider: The SDADC should run @6MHz */
  /* If Sysclk is 72MHz, SDADC divider should be 12 */
  RCC_SDADCCLKConfig(RCC_SDADCCLK_SYSCLK_Div12);

  /* GPIO Peripheral clock enable */
  RCC_AHBPeriphClockCmd(PT100_GPIO_CLK, ENABLE);

  /* PT100_SDADC channel 3P pin configuration: PE7 */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin   = PT100_GPIO_PIN;
  GPIO_Init(PT100_GPIO_PORT, &GPIO_InitStructure);

  /* Select External reference: The reference voltage selection is available
     only in SDADC1 and therefore to select the VREF for SDADC2/SDADC3, SDADC1
     clock must be already enabled */
  SDADC_VREFSelect(PT100_SDADC_VREF);

  /* Insert delay equal to ~5 ms */
  Delay(5);
  
  /* Enable PT100_SDADC */
  SDADC_Cmd(PT100_SDADC, ENABLE);
  
  /* Enter initialization mode */
  SDADC_InitModeCmd(PT100_SDADC, ENABLE);

  SDADCTimeout = SDADC_INIT_TIMEOUT;
  /* wait for INITRDY flag to be set */
  while((SDADC_GetFlagStatus(PT100_SDADC, SDADC_FLAG_INITRDY) == RESET) && (--SDADCTimeout != 0));

  if(SDADCTimeout == 0)
  {
    /* INITRDY flag can not set */
    return 1;
  }
  
  /* Analog Input configuration conf0: use Single Ended Offset mode */
  SDADC_AINStructure.SDADC_InputMode = SDADC_InputMode_SEOffset;
  SDADC_AINStructure.SDADC_Gain = PT100_SDADC_GAIN;
  SDADC_AINStructure.SDADC_CommonMode = SDADC_CommonMode_VSSA;
  SDADC_AINStructure.SDADC_Offset = 0;
  SDADC_AINInit(PT100_SDADC, SDADC_Conf_0, &SDADC_AINStructure);

  /* select PT100_SDADC channel to use conf0 */
  SDADC_ChannelConfig(PT100_SDADC, PT100_SDADC_CHANNEL, SDADC_Conf_0);

  /* Channel3 configuration */
  SDADC_InitStructure.SDADC_Channel = PT100_SDADC_CHANNEL;
  SDADC_InitStructure.SDADC_ContinuousConvMode = DISABLE;
  SDADC_InitStructure.SDADC_FastConversionMode = DISABLE;
  SDADC_Init(PT100_SDADC, &SDADC_InitStructure);

  /* Exit initialization mode */
  SDADC_InitModeCmd(PT100_SDADC, DISABLE);

  /* configure calibration to be performed on conf0 */
  SDADC_CalibrationSequenceConfig(PT100_SDADC, SDADC_CalibrationSequence_1);
  /* start PT100_SDADC Calibration */
  SDADC_StartCalibration(PT100_SDADC);
 
  /* Set calibration timeout: 5.12 ms at 6 MHz in a single calibration sequence */
  SDADCTimeout = SDADC_CAL_TIMEOUT;
  /* wait for PT100_SDADC Calibration process to end */
  while((SDADC_GetFlagStatus(PT100_SDADC, SDADC_FLAG_EOCAL) == RESET) && (--SDADCTimeout != 0));

  if(SDADCTimeout == 0)
  {
    /* EOCAL flag can not set */
    return 2;
  }

  /* SDADC successfully configured */
  return 0;
}

/**
  * @brief  Convert 256 times the external reference voltage (using external
  *         reference resistor 100 Ohm) and compute the correction factor
  * @param  None
  * @retval the correction factor
  */
static float SDADC_Calibration(void)
{
  float averagevalue = 0.0f;
  uint16_t samplescounter = 0;
  
  /* In prior to the Calibration execution, the SDADC should be configured
    in single mode, a common SDADC configuration phase(RCC, GPIO, SDADC configuration
    steps, Channel configuration...) are managed by the above SDADC_Config() function */
  
  for (samplescounter = 0; samplescounter < 256; samplescounter++)
  {
    /* Enable a Software start trigger */
    SDADC_SoftwareStartConv(PT100_SDADC);
    /* Wait Regular end of conversion flag to be set */
    while(SDADC_GetFlagStatus(PT100_SDADC, SDADC_FLAG_REOC) == RESET);
    /* compute the average value */
    averagevalue += (SDADC_GetConversionValue(PT100_SDADC)/SDADC_GAIN);
  }
  /* Compute the average value */
  averagevalue = averagevalue/256.0f;
  /* compute the correction factor */
  return((float)(1820.0f / averagevalue));/* 1820 = 100 * 65535 / 3600 */
  /* T = ((sdadcsample * 3600 / 65535) - 100) / 0.385 = 0 °C
     sdadcsample * 3600 / 65535 = 100 ==> sdadcsample = 100 * 65535 / 3600 = 1820 */
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 1 ms.
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
