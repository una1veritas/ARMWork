/**
  ******************************************************************************
  * @file    SDADC/SDADC_PressureMeasurement/main.c 
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

/** @addtogroup SDADC_PressureMeasurement
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int16_t InjectedConvData = 0;
__IO uint32_t TimingDelay = 0;

/* Private function prototypes -----------------------------------------------*/
static void TIM_Config(void);
static uint32_t SDADC1_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  float PressuremB = 0;
  char LCDstr[40] = {0};  
  float inputvoltage = 0.0f;
  
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f37x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f37x.c file
     */
     
  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  
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

  /* MPX2102_SDADC channel8P channel8N (Differential mode) configuration using injected
     conversion and TIM19 OC2 as trigger source */
  if(SDADC1_Config() != 0)
  {
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_BLUE);
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_DisplayStringLine(LCD_LINE_4, (uint8_t*)"Fail to configure   ");
    LCD_DisplayStringLine(LCD_LINE_5, (uint8_t*)"SigmaDelta Converter");
  }
  else
  {
    /* Set the LCD Back Color */
    LCD_SetBackColor(LCD_COLOR_WHITE);
    /* Set the LCD Text Color */
    LCD_SetTextColor(LCD_COLOR_BLUE);
    LCD_ClearLine(LCD_LINE_4);
    LCD_ClearLine(LCD_LINE_5);
    LCD_ClearLine(LCD_LINE_6);
    
    /* Configure and enable TIM19 to trigger the SDADC conversions */
    TIM_Config();
    
    /* Infinite loop */
    while (1)
    {
      /* Compute the voltage on PE8 and PE9 */
      inputvoltage = (float)(InjectedConvData * (SDADC_VREF / (SDADC_RESOL * SDADC_GAIN)));
      inputvoltage += OFFSET_VOLTAGE;
      /* Compute the pressure in mB */
      PressuremB = ((1000000 * inputvoltage) / (MPX2102_SENSITIVITY * EXTERNGAIN));

      /* write result to LCD */
      sprintf(LCDstr, "Pressure = %2.0f mB  ", PressuremB);
      LCD_DisplayStringLine(LCD_LINE_5, (uint8_t*)LCDstr);
    }
  }
}

/**
  * @brief  Configure TIM19 channel 2 to trigger the SDADC conversion 
  * @param  None
  * @retval None
  */
static void TIM_Config(void)
{
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  /* Enable TIM19 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM19, ENABLE);

  /* TIM19 Configuration */
  TIM_DeInit(TIM19);

  /* Fills each TIM_TimeBaseInitStruct member with its default value */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

  /* Time base configuration: MPX2102_SDADC will be triggered each sysclk/Period
    = 72MHz/10000 = 7.2 KHz */
  TIM_TimeBaseStructure.TIM_Period = 10000;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM19, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel2 (OC2) */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 10000/2;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC2Init(TIM19, &TIM_OCInitStructure);

  /* Enable TIM19 counter */
  TIM_Cmd(TIM19, ENABLE);    
}

/**
  * @brief  Configure MPX2102_SDADC channel 8P and 8N in Differential mode using injected
  *         conversion. The conversion is triggered by TIM19 channel 2
  * @param  None
  * @retval 0: SDADC Configured successfully
  *         1: INITRDY flag is not set, check the SDVDDA and SDVREF
  *         2: EOCAL flag is not set
  */
static uint32_t SDADC1_Config(void)
{
  SDADC_AINStructTypeDef SDADC_AINStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  uint32_t SDADCTimeout = 0;
  
  /* MPX2102_SDADC APB2 interface clock enable */
  RCC_APB2PeriphClockCmd(MPX2102_SDADC_CLK, ENABLE);

  /* PWR APB1 interface clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* Enable MPX2102_SDADC analog interface */
  PWR_SDADCAnalogCmd(MPX2102_SDADC_PWR, ENABLE);

  /* Set the SDADC divider: The SDADC should run @6MHz */
  /* If Sysclk is 72MHz, SDADC divider should be 12 */
  RCC_SDADCCLKConfig(RCC_SDADCCLK_SYSCLK_Div12);

  /* GPIOE Peripheral clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);

  /* MPX2102_SDADC channel 8P (PE8)  and channel 8N (PE9) */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin   = MPX2102_GPIO_PIN_P | MPX2102_GPIO_PIN_N;
  GPIO_Init(MPX2102_SDADC_GPIO_PORT, &GPIO_InitStructure);

  /* Select External reference: The reference voltage selection is available
     only in SDADC1 and therefore to select the VREF for SDADC2/SDADC3, SDADC1
     clock must be already enabled */
  SDADC_VREFSelect(MPX2102_SDADC_VREF);
  /* Insert delay equal to ~5 ms */
  Delay(5);
  
  /* Enable MPX2102_SDADC */
  SDADC_Cmd(MPX2102_SDADC, ENABLE);

  /* Enter initialization mode */
  SDADC_InitModeCmd(MPX2102_SDADC, ENABLE);
  SDADCTimeout = SDADC_INIT_TIMEOUT;
  /* wait for INITRDY flag to be set */
  while((SDADC_GetFlagStatus(MPX2102_SDADC, SDADC_FLAG_INITRDY) == RESET) && (--SDADCTimeout != 0));

  if(SDADCTimeout == 0)
  {
    /* INITRDY flag is not set */
    return 1;
  }

  /* Analog Input configuration conf0: use differential mode and gain = 4 */
  SDADC_AINStructure.SDADC_InputMode = SDADC_InputMode_Diff;
  SDADC_AINStructure.SDADC_Gain = MPX2102_SDADC_GAIN;
  SDADC_AINStructure.SDADC_CommonMode = SDADC_CommonMode_VSSA;
  SDADC_AINStructure.SDADC_Offset = 0;
  SDADC_AINInit(MPX2102_SDADC, SDADC_Conf_0, &SDADC_AINStructure);

  /* select MPX2102_SDADC channel 8 to use conf0 */
  SDADC_ChannelConfig(MPX2102_SDADC, MPX2102_SDADC_CHANNEL, SDADC_Conf_0);

  /* select channel 8 */
  SDADC_InjectedChannelSelect(MPX2102_SDADC, MPX2102_SDADC_CHANNEL);
  /* Select an external trigger */
  SDADC_ExternalTrigInjectedConvConfig(MPX2102_SDADC, SDADC_ExternalTrigInjecConv_T19_CC2);
  /* Select rising edge */
  SDADC_ExternalTrigInjectedConvEdgeConfig(MPX2102_SDADC, SDADC_ExternalTrigInjecConvEdge_Rising);

  /* Exit initialization mode */
  SDADC_InitModeCmd(MPX2102_SDADC, DISABLE);

  /* Enable end of injected conversion interrupt */
  SDADC_ITConfig(MPX2102_SDADC, SDADC_IT_JEOC, ENABLE);

  /* NVIC Configuration */
  NVIC_InitStructure.NVIC_IRQChannel = SDADC1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* configure calibration to be performed on conf0 */
  SDADC_CalibrationSequenceConfig(MPX2102_SDADC, SDADC_CalibrationSequence_1);
  /* start MPX2102_SDADC Calibration */
  SDADC_StartCalibration(MPX2102_SDADC);
  /* Set calibration timeout: 5.12 ms at 6 MHz in a single calibration sequence */
  SDADCTimeout = SDADC_CAL_TIMEOUT;
  /* wait for MPX2102_SDADC Calibration process to end */
  while((SDADC_GetFlagStatus(MPX2102_SDADC, SDADC_FLAG_EOCAL) == RESET) && (--SDADCTimeout != 0));

  if(SDADCTimeout == 0)
  {
    /* EOCAL flag can not set */
    return 2;
  }

  return 0;
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
