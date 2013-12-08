/**
  ******************************************************************************
  * @file    RTC/RTC_Timer/main.c 
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

/** @addtogroup RTC_Timer
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MESSAGE1   "        Timer       "
#define MESSAGE2   " LEFT   | RIGHT  | UP/DOWN  " 
#define MESSAGE3   " RESET  | START  | ADJUST   " 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t StartEvent = 0; 
extern __IO uint32_t RTCAlarmCount;
extern uint32_t SecondNumb;

/* Private function prototypes -----------------------------------------------*/
static void RTC_Config(void);
static void RTC_AlarmConfig(void);

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

  uint32_t index = 0;
  
  /* Initialize the LCD */
  STM32373C_LCD_Init(); 
  
  /* Clear the LCD */ 
  LCD_Clear(White);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(White);
  
  /* Displays MESSAGE1 on line 1 */
  LCD_DisplayStringLine(LINE(0), (uint8_t *)MESSAGE1);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Cyan);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(Black);
  
  /* Draw 2 lines */
  LCD_DrawUniLine(69, 121, 69,20);
  LCD_DrawUniLine(171, 121, 171, 20);
  
  /* Draw 4 lines  */
  LCD_DrawUniLine(70, 121, 120, 71);
  LCD_DrawUniLine(120, 71, 170,121);
  LCD_DrawUniLine(70, 20, 120, 70);
  LCD_DrawUniLine(120, 70, 170,20);
  
  /* Initialize the Display Picture */  
  for (index = 0; index < 100 ; index++)
  {
    if ((index % 2) == 0)
    {
      /* Set the LCD Text Color */
      LCD_SetTextColor(Blue);
      /* Draw Horizontal line */
      LCD_DrawLine(70 + (index/2) , 120 - (index/2)  , 101 - (index + 1) ,Horizontal);
      /* Set the LCD Text Color */
      LCD_SetTextColor(White);
      /* Draw Horizontal line */
      LCD_DrawLine(170 - (index/2) , 120 - (index/2)  , 101 - (index + 1) ,Horizontal);
    }
  }
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(Blue); 
  /* Set the LCD Back Color */
  LCD_SetBackColor(White);
  
  /* Display String on the LCD  */
  LCD_DisplayStringLine(LINE(3), (uint8_t*)" Set Timer:");
  LCD_DisplayStringLine(95, (uint8_t*)"  01:00    ");
  
  /* Initialize Timer to 60 seconds */
  SecondNumb = 60;
  
  /* Configure the external interrupt "LEFT", "RIGHT" , "DOWN" and "UP" buttons */
  STM_EVAL_PBInit(BUTTON_RIGHT, BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(BUTTON_LEFT, BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(BUTTON_UP, BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(BUTTON_DOWN, BUTTON_MODE_EXTI);
  
  /* Configure DOWN button in GPIO mode */  
   STM_EVAL_PBInit(BUTTON_DOWN, BUTTON_MODE_GPIO);  
  
  /* Configure the RTC peripheral by selecting the clock source.*/
  RTC_Config();
 
  /* Configure RTC AlarmA register to generate 8 interrupts per 1 Second */
  RTC_AlarmConfig();
  
  /* Set Text and Back color and Text size */
  LCD_SetFont(&Font12x12);
  LCD_SetBackColor(Cyan);
  LCD_SetTextColor(Black);
  LCD_DisplayStringLine(LINE(18), (uint8_t *)MESSAGE2);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);
  LCD_DisplayStringLine(LINE(19), (uint8_t *)MESSAGE3);
  LCD_SetFont(&Font16x24);

  /* Infinite loop */
  while (1)
  {}
}

/**
  * @brief  Configures the RTC peripheral and select the clock source.
  * @param  None
  * @retval None
  */
static void RTC_Config(void)
{
  RTC_InitTypeDef RTC_InitStructure;
  RTC_TimeTypeDef  RTC_TimeStruct;
  
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
  
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Configure the RTC data register and RTC prescaler */
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
  RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);
  
  /* Set the time to 00h 00mn 00s AM */
  RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
  RTC_TimeStruct.RTC_Hours   = 0x00;
  RTC_TimeStruct.RTC_Minutes = 0x00;
  RTC_TimeStruct.RTC_Seconds = 0x00;  
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
    /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
}

/**
  * @brief  Configures the RTC Alarm.
  * @param  None
  * @retval None
  */
static void RTC_AlarmConfig(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  RTC_AlarmTypeDef RTC_AlarmStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
   
  RTC_AlarmStructInit(&RTC_AlarmStructure);
  
  /* EXTI configuration */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  /* Set the alarmA Masks */
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All;
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
  
  /* Set AlarmA subseconds and enable SubSec Alarm : generate 8 interrupts per Second */
  RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xFF, RTC_AlarmSubSecondMask_SS14_5);

  /* Enable AlarmA interrupt */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
  
  /* Enable the alarmA */
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
  
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
