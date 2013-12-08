/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : clock-arch.c
 *    Description : Implementation of architecture-specific clock functionality
 *
 *    History :
 *    1. Date        : October 4, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: #1 $
**************************************************************************/
#include "clock-arch.h"
#include "iar_stm32f217ze_sk.h"
volatile clock_time_t Ticks;
unsigned int Tim0Per;

/*************************************************************************
 * Function Name: Tim2Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: TIM2 interrupt handler
 *
 *************************************************************************/
void TIM2_IRQHandler (void)
{
  // Clear TIM1 counter
  TIM_ClearFlag(TIM2,TIM_IT_Update);
  STM_EVAL_LEDToggle(LED1);
  ++Ticks;
}

/*************************************************************************
 * Function Name: clock_init
 * Parameters: uint32_t IntrPriority
 *
 * Return: none
 *
 * Description: Timer init
 *
 *************************************************************************/
void clock_init(uint32_t IntrPriority)
{
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
RCC_ClocksTypeDef RCC_ClocksStatus;

  Ticks = 0;

  // Enable Timer1 clock and release reset
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2,DISABLE);

  RCC_GetClocksFreq(&RCC_ClocksStatus);

  TIM_InternalClockConfig(TIM2);

  // Time base configuration
  TIM_TimeBaseStructure.TIM_Prescaler = 300-1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 2*1000;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

  // TIM1 counter enable
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_ClearFlag(TIM2,TIM_IT_Update);

  /* Enable the TIM2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIM2,ENABLE);
}

/*************************************************************************
 * Function Name: clock_init
 * Parameters: none
 *
 * Return: none
 *
 * Description: The current clock time, measured in system ticks
 *
 *************************************************************************/
clock_time_t clock_time(void)
{
  return(Ticks);
}
