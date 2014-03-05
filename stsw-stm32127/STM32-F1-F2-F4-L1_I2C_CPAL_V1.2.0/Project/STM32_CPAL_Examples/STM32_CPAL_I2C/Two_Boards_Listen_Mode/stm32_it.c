/**
  ******************************************************************************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Two_Boards_Listen_Mode/stm32_it.c 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    21-December-2012
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
#include "stm32_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* CPAL local transfer structures */
extern __IO uint32_t ActionState, DeviceMode, LastMode;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-MX Processor Exceptions Handlers (X can be 3 or 4)       */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
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
  * @brief  This function handles EXTI15_10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
#if defined (USE_STM32100E_EVAL) || defined(USE_STM3210C_EVAL)
  /* Check if the Tamper push button has been pushed */
  if (EXTI_GetFlagStatus(TAMPER_BUTTON_EXTI_LINE) != RESET)  
  {
    if (((ActionState == ACTION_NONE) && (DeviceMode == MASTER)) || ((LastMode == SLAVE) && (DeviceMode == SLAVE)))
    {
      ActionState = BUTTON_TAMPER;
    }
    
    /* Clear the interrupt pending bit */
    EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);
  }
#endif /* USE_STM32100E_EVAL || USE_STM3210C_EVAL */
  
#if defined (USE_STM322xG_EVAL) || defined (USE_STM324xG_EVAL)
  /* Check if the Key push button has been pushed */
  if (EXTI_GetFlagStatus(KEY_BUTTON_EXTI_LINE) != RESET)  
  {
    if (((ActionState == ACTION_NONE) && (DeviceMode == MASTER)) || ((LastMode == SLAVE) && (DeviceMode == SLAVE)))
    {
      ActionState = BUTTON_KEY;
    }
    
    /* Clear the interrupt pending bit */
    EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);
  }
  
  /* Check if the Tamper push button has been pushed */
  if (EXTI_GetFlagStatus(TAMPER_BUTTON_EXTI_LINE) != RESET)  
  {
    if (((ActionState == ACTION_NONE) && (DeviceMode == MASTER)) || ((LastMode == SLAVE) && (DeviceMode == SLAVE)))
    {
      ActionState = BUTTON_TAMPER;
    }
    
    /* Clear the interrupt pending bit */
    EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);
  }
#endif /* USE_STM322xG_EVAL || USE_STM324xG_EVAL*/
  
#ifdef USE_STM32L152_EVAL  
  /* Check if the Left push button has been pushed */
  if (EXTI_GetFlagStatus(LEFT_BUTTON_EXTI_LINE) != RESET)  
  {
    if (((ActionState == ACTION_NONE) && (DeviceMode == MASTER)) || ((LastMode == SLAVE) && (DeviceMode == SLAVE)))
    {
      ActionState = BUTTON_TAMPER;
    }
    
    /* Clear the interrupt pending bit */
    EXTI_ClearITPendingBit(LEFT_BUTTON_EXTI_LINE);
  }
  
  /* Check if the Right push button has been pushed */
  if (EXTI_GetFlagStatus(RIGHT_BUTTON_EXTI_LINE) != RESET)  
  {
    if (((ActionState == ACTION_NONE) && (DeviceMode == MASTER)) || ((LastMode == SLAVE) && (DeviceMode == SLAVE)))
    {
      ActionState = BUTTON_KEY;
    }
    
    /* Clear the interrupt pending bit */
    EXTI_ClearITPendingBit(RIGHT_BUTTON_EXTI_LINE);
  }
#endif /* USE_STM32L152_EVAL */
}

/**
  * @brief  This function handles EXTI9_5 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
#if defined (USE_STM32100E_EVAL) || defined(USE_STM3210C_EVAL)  
  /* Check if the Key push button has been pushed */
  if (EXTI_GetFlagStatus(KEY_BUTTON_EXTI_LINE) != RESET)  
  {
    if (((ActionState == ACTION_NONE) && (DeviceMode == MASTER)) || ((LastMode == SLAVE) && (DeviceMode == SLAVE)))
    {
      ActionState = BUTTON_KEY;
    }
    
    /* Clear the interrupt pending bit */
    EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);
  }
#endif /* USE_STM32100E_EVAL || USE_STM3210C_EVAL */
  
#ifdef USE_STM32L152D_EVAL
  /* Check if the Left push button has been pushed */
  if (EXTI_GetFlagStatus(LEFT_BUTTON_EXTI_LINE) != RESET)  
  {
    if (((ActionState == ACTION_NONE) && (DeviceMode == MASTER)) || ((LastMode == SLAVE) && (DeviceMode == SLAVE)))
    {
      ActionState = BUTTON_TAMPER;
    }
    
    /* Clear the interrupt pending bit */
    EXTI_ClearITPendingBit(LEFT_BUTTON_EXTI_LINE);
  }
  
  /* Check if the Right push button has been pushed */
  if (EXTI_GetFlagStatus(RIGHT_BUTTON_EXTI_LINE) != RESET)  
  {
    if (((ActionState == ACTION_NONE) && (DeviceMode == MASTER)) || ((LastMode == SLAVE) && (DeviceMode == SLAVE)))
    {
      ActionState = BUTTON_KEY;
    }
    
    /* Clear the interrupt pending bit */
    EXTI_ClearITPendingBit(RIGHT_BUTTON_EXTI_LINE);
  }
#endif /* USE_STM32L152D_EVAL */   
}

/**
  * @brief  This function handles TIM6 global interrupt request.
  * @param  None
  * @retval None
  */

#if defined (USE_STM32100E_EVAL) || defined (USE_STM322xG_EVAL) || defined (USE_STM324xG_EVAL)
void TIM6_DAC_IRQHandler(void) 
#elif defined (USE_STM32L152_EVAL) || defined (USE_STM32L152D_EVAL) || defined(USE_STM3210C_EVAL)   
void TIM6_IRQHandler(void)  
#endif /* USE_STM32100E_EVAL || USE_STM322xG_EVAL || USE_STM324xG_EVAL */
{
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {    
    STM_EVAL_LEDToggle(LED1);
    STM_EVAL_LEDToggle(LED4);
    
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  } 
}

/**
  * @brief  This function handles TIM7 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM7_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
  {    
    if (((ActionState == ACTION_NONE) && (DeviceMode == MASTER)) || ((LastMode == SLAVE) && (DeviceMode == SLAVE)))
    {
      ActionState =  ACTION_PERIODIC ;
    }
    
    /* Clear Timer interrupt pending bit */
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    
    /* Clear Key Button Interrupt pending bit */
    EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);
    EXTI_ClearFlag(KEY_BUTTON_EXTI_LINE); 
  } 
}

/******************************************************************************/
/*                 STM32xxxx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handlers name please refer to the startup */
/*  file (startup_stm32xxxx_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
