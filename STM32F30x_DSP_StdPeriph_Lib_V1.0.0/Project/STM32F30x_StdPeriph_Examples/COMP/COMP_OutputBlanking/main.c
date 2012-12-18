/**
  ******************************************************************************
  * @file    COMP/COMP_OutputBlanking/main.c 
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

/** @addtogroup COMP_OutputBlanking
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void COMP1_Config(void);
static void TIM1_Config(void);

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
  
  /* COMP1 Configuration: output redirected to TIM1 BKIN and blanking source is TIM1 OC5 */
  COMP1_Config();
  
  /* TIM1 Configuration in PWM mode */
  TIM1_Config();
  
  /* Infinite loop */
  while(1)
  {
  }
}

/**
  * @brief  Configures COMP1: VREFINT used as inverting input, output redirected
  *                           to TIM1 BKIN and blanking source is TIM1 OC5 
  * @param  None
  * @retval None
  */
static void COMP1_Config(void)
{
  /* Init Structure definition */
  COMP_InitTypeDef COMP_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* GPIOA Peripheral clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* Configure PA1 in analog mode: PA1 is connected to COMP1 non inverting input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* COMP1 output config: PA0 */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Map COMP1 output to PA0 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_8);
  
  /* COMP Peripheral clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* COMP1 config: COMP1 Output is redirected to TIM1 BKIN 
                   and output blanking source is TIM1 OC5 */
  COMP_InitStructure.COMP_NonInvertingInput = COMP_NonInvertingInput_IO1;
  COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_VREFINT;
  COMP_InitStructure.COMP_Output = COMP_Output_TIM1BKIN;
  COMP_InitStructure.COMP_BlankingSrce = COMP_BlankingSrce_TIM1OC5;
  COMP_InitStructure.COMP_OutputPol = COMP_OutputPol_NonInverted;
  COMP_InitStructure.COMP_Hysteresis = COMP_Hysteresis_No;
  COMP_InitStructure.COMP_Mode = COMP_Mode_HighSpeed;
  COMP_Init(COMP_Selection_COMP1, &COMP_InitStructure);
  
  /* Enable COMP1 */
  COMP_Cmd(COMP_Selection_COMP1, ENABLE);
}

/**
  * @brief  Configures TIM1 channel 2 and channel 5 in PWM mode
  * @param  None
  * @retval None
  */
static void TIM1_Config(void)
{
  /* Init Structure definition */
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* GPIOA clock is already enabled in COMP1_Config() */
  
  /* TIM1 channels pin configuration:
       TIM1_CH2 -> PA9
  */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_6);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_6);
  
  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  /* Time Base configuration: Channel 2 and channel 5 frequency is 
     APB2 clock / period = 72000000 / 50000 = 1440 Hz */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 50000;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  
  /* Automatic Output enable, Break, dead time and lock configuration*/
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM_BDTRInitStructure.TIM_DeadTime = 0;
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
  
  /* TIM1 Channel 2 configuration in PWM mode: duty cycle is pulse/period = 100 * (37500 / 50000) =  75% */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 37500;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  /* Init channel 2 OC2 */
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
  
  /* TIM1 Channel 5 configuration in PWM mode: channel 5 is an internal channel 
  (not available on GPIO): TIM1 OC5 is high during 2000 / 72000000 = 27.7 micro second */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 2000;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  /* Init channel 5 OC5: This channel is internal (not available on GPIO) */
  TIM_OC5Init(TIM1, &TIM_OCInitStructure);
  
  /* Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
  
  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);
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
