/**
  ******************************************************************************
  * @file    CAN/CAN_DualFIFO/stm32f30x_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
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
#include "stm32f30x_it.h"
#include "main.h"

/** @addtogroup STM32F30x_StdPeriph_Examples
  * @{
  */

/** @addtogroup CAN_DualFIFO
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MESSAGE1   "FIFO0 : Msg received" 
#define MESSAGE2   "FIFO1 : Msg received" 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CanRxMsg RxMessage, RxMessage1;
extern uint8_t KeyNumber;
extern CanTxMsg TxMessage, TxMessage1;

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
  * @brief  This function handles CAN1 RX0 request.
* @param  None
* @retval None
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  /* Set the LCD Back Color */
  LCD_SetBackColor(Green);
  /* Set the LCD Text Color */
  LCD_SetTextColor(Black);
  /* Displays MESSAGE1 on line 1 */
  LCD_DisplayStringLine(LINE(6), (uint8_t *)MESSAGE1);
  
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
  LED_Display(RxMessage.Data[0]);
  KeyNumber = RxMessage.Data[0];
  
}

/**
* @brief  This function handles CAN1 RX1 request.
* @param  None
* @retval None
*/
void CAN1_RX1_IRQHandler(void)
{
  /* Set the LCD Back Color */
  LCD_SetBackColor(Cyan);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(Black);
  
  /* Displays MESSAGE1 on line 1 */
  LCD_DisplayStringLine(LINE(6), (uint8_t *)MESSAGE2);
  
  CAN_Receive(CAN1, CAN_FIFO1, &RxMessage1);
  LED_Display(RxMessage1.Data[0]);
  KeyNumber = RxMessage1.Data[0];
}

/**
  * @brief  This function handles EXTI 6 request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{ 
 if(KeyNumber < 0x2) 

  {
    KeyNumber = 0x05;
  }
  LED_Display(--KeyNumber);

  TxMessage.Data[0] = KeyNumber;

  CAN_Transmit(CANx, &TxMessage);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Green);
  /* Set the LCD Text Color */
  LCD_SetTextColor(Black);
  /* Displays MESSAGE1 on line 6 */
  LCD_DisplayStringLine(LINE(6), (uint8_t *)MESSAGE1);
   
  /* Clear the EXTI line 6 pending bit */
  EXTI_ClearITPendingBit(EXTI_Line6);
}

/**
  * @brief  This function handles EXTI 10 to 15 request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  if(KeyNumber == 0x4) 
  {
    KeyNumber = 0x00;
  }
  LED_Display(++KeyNumber);
  TxMessage1.Data[0] = KeyNumber;
  CAN_Transmit(CANx, &TxMessage1);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Cyan);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(Black);
  
  /* Displays MESSAGE1 on line 6 */
  LCD_DisplayStringLine(LINE(6), (uint8_t *)MESSAGE2);
   
  /* Clear the EXTI line 13 pending bit */
  EXTI_ClearITPendingBit(EXTI_Line13);
  
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
