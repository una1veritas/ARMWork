/**
  ******************************************************************************
  * @file    RTC/RTC_StopWatch/main.c 
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

/** @addtogroup RTC_StopWatch
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MESSAGE1   " **** STOPWATCH ****" 
#define MESSAGE2   " LEFT         |      RIGHT " 
#define MESSAGE3   " START        |       GET  " 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t SecondFraction = 0;
__IO uint8_t StartEvent = 0; 
RTC_InitTypeDef  RTC_InitStructure;
RTC_TimeTypeDef  RTC_TimeStruct;
extern __IO uint32_t CurrentTimeSec;

/* Private function prototypes -----------------------------------------------*/
static void RTC_Config(void);
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

  /* Initialize the LCD */
  STM32373C_LCD_Init();      
    
  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(White);
  
  /* Displays MESSAGE1 on line 1 */
  LCD_DisplayStringLine(LINE(0), (uint8_t *)MESSAGE1);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(Red);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Red);
  LCD_DrawFullRect(31, 292,264,34);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(White);
  LCD_DrawFullRect(33, 290 ,260,30);
  
  /* Configure the external interrupt "RIGHT" and "LEFT" buttons */
  STM_EVAL_PBInit(BUTTON_RIGHT,BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(BUTTON_LEFT,BUTTON_MODE_EXTI);
  
  /* Configure the RTC peripheral by selecting the clock source.*/
  RTC_Config();
  
  /* Set the LCD Text, Back Colors and Text size */
  LCD_SetTextColor(Black); 
  LCD_SetBackColor(Cyan);
  LCD_SetFont(&Font12x12);
  
  LCD_DisplayStringLine(LINE(18), (uint8_t *)MESSAGE2);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);
  
  LCD_DisplayStringLine(LINE(19), (uint8_t *)MESSAGE3);
  
  /* Set text size */
  LCD_SetFont(&Font16x24);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(Blue);
  
  /* Initialize time Stucture */
  RTC_TimeStructInit(&RTC_TimeStruct); 

  /* Infinite loop */
  while (1)
  {
    /* Set the LCD Back Color and text size */
    LCD_SetFont(&Font16x24);
    LCD_SetBackColor(White);
    
    /* Check on the event 'start' */
    if(StartEvent != 0x0)
    {  
      /* Get the RTC sub second fraction */
      SecondFraction = (((256 - (uint32_t)RTC_GetSubSecond()) * 1000) / 256);
      
      /* Get the Curent time */
      RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
      
      /* Refresh : Display the curent time and the sub second on the LCD  */
      RTC_Time_Display(37, Black , RTC_Get_Time(SecondFraction , &RTC_TimeStruct));
    }
    else
    {
      /* Re-initialize the Display time on the LCD */
      RTC_Time_InitDisplay();
    }  
  }
}

/**
* @brief  Configure the RTC peripheral by selecting the clock source and configure 
*         RTC tamper register to clear all the Backup data register
* @param  None
* @retval None
*/
static void RTC_Config(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
    
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
  
  RTC_DeInit();
  
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
  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStruct);
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
  
  /* EXTI configuration for Tamper Pin ****************************************/
  EXTI_ClearITPendingBit(EXTI_Line19);
  EXTI_InitStructure.EXTI_Line = EXTI_Line19;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable RTC_IRQn */
  NVIC_InitStructure.NVIC_IRQChannel = TAMPER_STAMP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* determines the number of active pulse for the specific level */
  RTC_TamperFilterConfig(RTC_TamperFilter_2Sample);
  
  /* Determines the frequency at which each of the tamper inputs are sampled */
  RTC_TamperSamplingFreqConfig(RTC_TamperSamplingFreq_RTCCLK_Div32768);
  
  RTC_TamperPullUpCmd(DISABLE);
  
  /* Select the tamper 2 with High level */
  RTC_TamperTriggerConfig(RTC_Tamper_2, RTC_TamperTrigger_HighLevel);
  
  /* Clear tamper 2 flag */
  RTC_ClearFlag(RTC_FLAG_TAMP2F);
}

/**
* @brief  load the current time and sub second on a table form.
* @param  SecondFraction: the sub second fraction .
* @param  RTC_TimeStructure : pointer to a RTC_TimeTypeDef structure that 
*                             contains the current time values. 
* @retval table : return current time and sub second in a table form
*/
Table_TypeDef RTC_Get_Time(uint32_t SecondFrac , RTC_TimeTypeDef* RTC_TimeStructure )
{
  Table_TypeDef timetab;
  uint32_t currenttimesec = 0;
  
  /* Get the Current time in second */ 
  currenttimesec = ((RTC_TimeStructure->RTC_Hours * 3600) +(RTC_TimeStructure->RTC_Minutes * 60) +
                    RTC_TimeStructure->RTC_Seconds) - CurrentTimeSec; 
  
  /* Fill the table2 fields with the current Time*/
  timetab.tab[0]  = ((uint8_t)((currenttimesec/3600)/10) + 0x30);
  timetab.tab[1]  = ((uint8_t)((currenttimesec/3600) %10)+ 0x30); 
  timetab.tab[2]  = 0x3A;
  timetab.tab[3]  = (((uint8_t)(((currenttimesec % 3600)/60) /10)) + 0x30);
  timetab.tab[4]  = (((uint8_t)(((currenttimesec % 3600)/60) %10)) + 0x30);
  timetab.tab[5]  = 0x3A;
  timetab.tab[6]   = (((uint8_t)((currenttimesec% 60) /10))+ 0x30);
  timetab.tab[7]   = (((uint8_t)((currenttimesec% 60) %10)) + 0x30);
  timetab.tab[8]   = 0x2E;
  timetab.tab[9]   = (uint8_t)((SecondFrac / 100) + 0x30);
  timetab.tab[10]  = (uint8_t)(((SecondFrac % 100 ) / 10) +0x30);
  timetab.tab[11]  =  (uint8_t)((SecondFrac % 10) +0x30);
  
  /* return timetab */
  return timetab;
}

/**
* @brief  Initialize the time displays  on the LCD.
* @param  None.
* @retval None
*/
void RTC_Time_InitDisplay(void)
{
  uint8_t i = 0;
  for (i=0;i<12;i++)
  {
    if ((i==2) || (i==5))
    {
      /* Display char on the LCD */
      LCD_DisplayChar(37, (280 - (20 *i)), ':');
    }
    else if ((i==8) )
    {
      /* Display char on the LCD */
      LCD_DisplayChar(37, (280 - (20 *i)), '.');
    }
    else
    {
      /* Display char on the LCD */
      LCD_DisplayChar(37, (280 - (20 *i)), '0');
    }
  }
}

/**
* @brief  Displays the current Time on the LCD.
* @param  Line:  the Line where to display the Current time .
*         This parameter can be one of the following values:
* @arg Linex: where x can be 0..9
* @param  Color_x : specifies the Background Color.
* @param  table   : the Current time and sub second.
* @retval None
*/
void RTC_Time_Display(uint8_t Line, __IO uint16_t Colorx, Table_TypeDef timetable)
{   
  uint8_t i = 0;
  
  /* Initialize table */
  LCD_SetTextColor(Colorx);
  
  for (i=0;i<12;i++)
  {
    /* Display char on the LCD */
    LCD_DisplayChar(Line, (280 - (20 *i)), timetable.tab[i]);
    
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
