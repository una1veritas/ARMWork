/**
  ******************************************************************************
  * @file    RTC/RTC_Timer/stm32f30x_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
  * @brief   Main Interrupt Service Routines.
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

/** @addtogroup RTC_Timer
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MESSAGE4   " RESET | START |   NA     " 
#define MESSAGE5   " RESET | PAUSE |   NA     " 
#define MESSAGE6   " RESET | START | ADJUST   " 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t RTCAlarmCount = 0;
uint32_t SecondNumb = 60;
__IO uint8_t StartEvent;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/******************************************************************************/
/**
  * @brief  This function handles RTC Alarm interrupt (A and B) request.
  * @param  None
  * @retval None
  */
void RTC_Alarm_IRQHandler(void)
{
  uint32_t tmp =0;
  
  /* Check on the AlarmA falg and on the number of interrupts per Second (60*8) */
  if(RTC_GetITStatus(RTC_IT_ALRA) != RESET) 
  { 
    /* Clear RTC AlarmA Flags */
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    
    /* Generate alarms 480 = 60 * 8 */   
    if(RTCAlarmCount != (8 * SecondNumb))
    {
      /* Increament the counter of Alarma interrupts*/
      RTCAlarmCount++;
      
      
      /* Define the rate of Progress bar */
      tmp = (uint32_t) ((RTCAlarmCount * 100)/ (8 * SecondNumb)); 
      
      /* Set the LCD text color */
      LCD_SetTextColor(Blue);
      
      /* Set the LCD Font */
      LCD_SetFont(&Font16x24);
      
      /* Display Char on the LCD : XXX% */
      LCD_DisplayChar(40,110, (tmp / 100) +0x30);
      LCD_DisplayChar(40,88, ((tmp  % 100 ) / 10) +0x30);
      LCD_DisplayChar(40,66, (tmp % 10) +0x30);
      LCD_DisplayChar(40,44, 0x25);   
      
      if(SecondNumb > 13)
      {
        if((tmp % 2) == 0 )
        {  
          /* Set the LCD text color */
          LCD_SetTextColor(White);
          LCD_DrawLine(70 + (tmp/2) , 120 - (tmp/2)  , 100 - tmp  ,Horizontal);
          LCD_DrawLine(170 - (tmp/2) , 120 - (tmp/2)  , 100 - tmp ,Horizontal);
          
          /* Set the LCD Text Color */
          LCD_SetTextColor(Blue);
          LCD_DrawLine(170 - (tmp/2) , 120 - (tmp/2)  , 100 - tmp ,Horizontal);
          
        }
      }      
    }
    else
    {
      StartEvent = 9;
      /* Disable the alarmA */
      RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
    }
  }
  /* Clear the EXTIL line 17 */
  EXTI_ClearITPendingBit(EXTI_Line17);
}

/**
  * @brief  This function handles External line 5 to 9 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
  uint8_t index = 0;
	uint32_t tmp = 0, tmp1 = 0;
  if((EXTI_GetITStatus(RIGHT_BUTTON_EXTI_LINE) != RESET) )
  {
    /* Set the LCD Back Color */
    LCD_SetBackColor(White);
    StartEvent = 0;
    /* Reset Counter*/
    RTCAlarmCount = 0;
    
    /* Disable the alarm */
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
    
    /* Display Char on the LCD : XXX% */
    LCD_DisplayChar(40,110,0x30);
    LCD_DisplayChar(40,88, 0x30);
    LCD_DisplayChar(40,66, 0x30);
    LCD_DisplayChar(40,44, 0x25);
    
    for (index = 0; index < 100 ; index++)
    {
      if ((index % 2) ==0)
      {
        /* Set the LCD Text Color */
        LCD_SetTextColor(Blue);
        LCD_DrawLine(70 + (index/2) , 120 - (index/2)  , 101 - (index + 1) ,Horizontal);
        /* Set the LCD Text Color */
        LCD_SetTextColor(White);
        LCD_DrawLine(170 - (index/2) , 120 - (index/2)  , 101 - (index + 1) ,Horizontal);
        
      }
    } 
    /* Displays MESSAGE6 on line 5 */
    LCD_SetFont(&Font12x12);
    /* Set the LCD Back Color */
    LCD_SetBackColor(Blue);
    LCD_SetTextColor(White);
    LCD_DisplayStringLine(LINE(19), (uint8_t *)MESSAGE6);
    LCD_SetFont(&Font16x24);
    /* Set the LCD Text Color */
    LCD_SetTextColor(Black); 
    
    /* Clear the RIGHT EXTI  pending bit */
    EXTI_ClearITPendingBit(RIGHT_BUTTON_EXTI_LINE);  
  }
    
    
    if(EXTI_GetITStatus(DOWN_BUTTON_EXTI_LINE) != RESET)
  {
    
    if(RTCAlarmCount == 0)
    {
      SecondNumb--;
      if(SecondNumb < 15) SecondNumb = 15; 
      
      tmp = (uint32_t) (SecondNumb/60);
      tmp1 =   SecondNumb -(tmp*60);
      LCD_SetFont(&Font16x24); 
      /* Set the LCD text color */
      LCD_SetTextColor(Blue);
      /* Set the LCD Back Color */
      LCD_SetBackColor(White);
      LCD_DisplayStringLine(95,     "         ");  
      /* Display Char on the LCD : XXX% */       
      LCD_DisplayChar(95,294, (tmp / 10) +0x30);
      LCD_DisplayChar(95,278, (tmp  % 10 ) +0x30);
      LCD_DisplayChar(95,262, ':');
      LCD_DisplayChar(95,246, (tmp1 / 10) +0x30);
      LCD_DisplayChar(95,230, (tmp1  % 10 ) +0x30);
    }
    
    /* Clear the RIGHT EXTI  pending bit */
    EXTI_ClearITPendingBit(DOWN_BUTTON_EXTI_LINE); 
  }
  
  if (EXTI_GetITStatus(UP_BUTTON_EXTI_LINE) != RESET)
  {
    if(RTCAlarmCount == 0)
    {
      SecondNumb++;
      
      tmp = (uint32_t) (SecondNumb/60);
      tmp1 =   SecondNumb -(tmp*60);
      
      LCD_SetFont(&Font16x24);
      /* Set the LCD text color */
      LCD_SetTextColor(Blue);
      /* Set the LCD Back Color */
      LCD_SetBackColor(White);
      LCD_DisplayStringLine(95,     "         "); 
      /* Display Char on the LCD : XXX% */       
      LCD_DisplayChar(95,294, (tmp / 10) +0x30);
      LCD_DisplayChar(95,278, (tmp  % 10 ) +0x30);
      LCD_DisplayChar(95,262, ':');
      LCD_DisplayChar(95,246, (tmp1 / 10) +0x30);
      LCD_DisplayChar(95,230, (tmp1  % 10 ) +0x30);
      
    }
    /* Clear the EXTI Line 11 */
    EXTI_ClearITPendingBit(UP_BUTTON_EXTI_LINE);
  }
}

/**
  * @brief  This function handles External line 10 to 15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  if (EXTI_GetITStatus(SEL_BUTTON_EXTI_LINE) != RESET)
  {
    
    if(StartEvent == 8)
    {
      StartEvent = 0;
   
      /* Enable the alarmA */
      RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
      
      /* Clear the TAMPER EXTI  pending bit */
      EXTI_ClearITPendingBit(SEL_BUTTON_EXTI_LINE); 
      
      /* Displays MESSAGE4 on line 5 */
      LCD_SetFont(&Font12x12);
      /* Set the LCD Back Color */
      LCD_SetBackColor(Blue);
      LCD_SetTextColor(White);
      LCD_DisplayStringLine(LINE(19), (uint8_t *)MESSAGE4);
      LCD_SetFont(&Font16x24);
      /* Set the LCD Text Color */
      LCD_SetTextColor(Black); 
    }
    else
    {
      
      /* Displays MESSAGE5 on line 5 */
      LCD_SetFont(&Font12x12);
      
      /* Set the LCD Back Color */
      LCD_SetBackColor(Blue);
      LCD_SetTextColor(White);
      LCD_DisplayStringLine(LINE(19), (uint8_t *)MESSAGE5);
      LCD_SetFont(&Font16x24);
      /* Set the LCD Back Color */
      LCD_SetBackColor(White);
      
      /* Enable the alarmA */
      RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
      StartEvent = 8;
      /* Clear the SEL EXTI  pending bit */
      EXTI_ClearITPendingBit(SEL_BUTTON_EXTI_LINE); 
    }  
  }
}
/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
