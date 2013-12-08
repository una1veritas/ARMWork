/**
  ******************************************************************************
  * @file    RTC/RTC_StopWatch/stm32f37x_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "stm32f37x_it.h"

/** @addtogroup STM32F37x_StdPeriph_Examples
  * @{
  */

/** @addtogroup RTC_StopWatch
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MESSAGE4   "  Press and hold TAMPER   " 
#define MESSAGE5   " to reset Backup registers" 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_TimeTypeDef RTC_StampTimeStruct;
extern __IO uint8_t StartEvent;
uint32_t BackupIndex = 0;
__IO uint32_t SubSecFrac = 0;
/* Define the backup register */
uint32_t BKPDataReg[10] = { RTC_BKP_DR0, RTC_BKP_DR1, RTC_BKP_DR2, RTC_BKP_DR3, RTC_BKP_DR4,
                             RTC_BKP_DR5, RTC_BKP_DR6, RTC_BKP_DR7, RTC_BKP_DR8, RTC_BKP_DR9
                          };
__IO uint32_t CurrentTimeSec = 0;

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
/*                 STM32F37x Peripherals Interrupt Handlers                   */
/******************************************************************************/
/**
* @brief  This function handles External line 4 interrupt request.
* @param  None
* @retval None
*/
void EXTI4_IRQHandler(void)
{
  RTC_TimeTypeDef  RTC_TimeStructureInit;
  
  if((EXTI_GetITStatus(LEFT_BUTTON_EXTI_LINE) != RESET) && (StartEvent == 0x0))
  {
    /* Enable Tamper interrupt */
    RTC_ITConfig(RTC_IT_TAMP, ENABLE);
    
    /* Enabale the tamper 2 */
    RTC_TamperCmd(RTC_Tamper_2, ENABLE);
    
    /* Set the LCD Back Color */
    LCD_SetBackColor(White);
    
    /* Clear the LCD line 5 */
    LCD_ClearLine(Line5);
    
    /* Get the current time */
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructureInit);
    CurrentTimeSec = (RTC_TimeStructureInit.RTC_Hours * 3600) + (RTC_TimeStructureInit.RTC_Minutes * 60) + RTC_TimeStructureInit.RTC_Seconds;     
    
    /* start count */
    StartEvent = 0x1;
  }
  /* Clear the LEFT EXTI  pending bit */
  EXTI_ClearITPendingBit(LEFT_BUTTON_EXTI_LINE); 
}
/**
* @brief  This function handles External lines 9 to 5 interrupt request.
* @param  None
* @retval None
*/
void EXTI9_5_IRQHandler(void)
{
  uint16_t Colorx;
  
  if((EXTI_GetITStatus(RIGHT_BUTTON_EXTI_LINE) != RESET) && (StartEvent !=0x0) && (BackupIndex < 11))
  {  
    if (BackupIndex < 5)
    {
      if((uint8_t)(BackupIndex% 2) != 0x0)
      { 
        /* Set LCD backcolor*/
        LCD_SetBackColor(Blue2);
        Colorx = White;
      }
      else
      {
        /* Set LCD backcolor*/
        LCD_SetBackColor(Cyan);
        Colorx = Black;
      }
      
      /* Get the Current sub second and time */
      SubSecFrac = (((256 - (uint32_t)RTC_GetSubSecond()) * 1000) / 256);
      
      RTC_GetTime(RTC_Format_BIN, &RTC_StampTimeStruct);
      
      LCD_SetFont(&Font16x24);
      /* Display result on the LCD */
      RTC_Time_Display( LINE(3 + BackupIndex), Colorx, RTC_Get_Time(SubSecFrac , &RTC_StampTimeStruct) ); 
      
      if (BackupIndex < 2)
      {
        /* Save time register  to Backup register (the first 5 register is reserved for time) */
        RTC_WriteBackupRegister(BKPDataReg[BackupIndex],(uint32_t)RTC->TR);
        
        /* Save sub second time stamp register (the latest 6 register is reserved for time) */
        RTC_WriteBackupRegister(BKPDataReg[BackupIndex + 2], SubSecFrac);
      }
      
    }
    else
    {
      /* the backup register is full with 10trials */
      /* Set the LCD Back Color */
      LCD_SetBackColor(White);
      LCD_SetFont(&Font12x12);
      /* Set the LCD Text Color */
      LCD_SetTextColor(Red); 
      LCD_DisplayStringLine(LINE(16), (uint8_t *)MESSAGE4);
      LCD_DisplayStringLine(LINE(17), (uint8_t *)MESSAGE5);
      
    }  
    BackupIndex++;
    /* Set the LCD Back Color */
    LCD_SetBackColor(White);  
  }

  /* Clear the RIGHT EXTI line */
  EXTI_ClearITPendingBit(RIGHT_BUTTON_EXTI_LINE);  
}

/**
* @brief  This function handles Tamper pin interrupt request.
* @param  None
* @retval None
*/
void TAMPER_STAMP_IRQHandler(void)
{
  uint8_t i =0;
  if (RTC_GetITStatus(RTC_IT_TAMP2) != RESET)
  {
    /* Set the LCD Back Color */
    LCD_SetBackColor(White);
    LCD_SetFont(&Font16x24);
    /* Clear LCD line 5 to 9 */
    for (i=0; i < 5; i++)
    {  
      /* Clear all the LCD lignes from 3 to 7 */
      LCD_ClearLine(LINE(3+i));
    }
    
    /* reset Counter */
    BackupIndex = 0 ;
    
    /* Enetr to idle */
    StartEvent =0x0;
    
    LCD_SetFont(&Font12x12);
    RTC_Time_InitDisplay();
    
    LCD_SetFont(&Font12x12);
    LCD_ClearLine(LINE(16));
    LCD_ClearLine(LINE(17));
    
    /* Enable Tamper interrupt */
    RTC_ITConfig(RTC_IT_TAMP, DISABLE);
    /* Enabale the tamper 2 */
    RTC_TamperCmd(RTC_Tamper_2 , DISABLE);
    
  }
  /* Clear EXTI line 19 */
  EXTI_ClearITPendingBit(EXTI_Line19);
  /* Clear Tamper pin interrupt pending bit */
  RTC_ClearITPendingBit(RTC_IT_TAMP2);
}

/******************************************************************************/
/*                 STM32F37x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f37x.s).                                               */
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
