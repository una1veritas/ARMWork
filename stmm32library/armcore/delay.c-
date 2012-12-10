/**
  ******************************************************************************
  * @file    lib_std/UTIL/src/wait.c
  * @author  Yasuo Kawachi
  * @version V1.0.0
  * @date    04/15/2009
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * Copyright 2008-2009 Yasuo Kawachi All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *  1. Redistributions of source code must retain the above copyright notice,
  *  this list of conditions and the following disclaimer.
  *  2. Redistributions in binary form must reproduce the above copyright notice,
  *  this list of conditions and the following disclaimer in the documentation
  *  and/or other materials provided with the distribution.
  *
  * THIS SOFTWARE IS PROVIDED BY YASUO KAWACHI "AS IS" AND ANY EXPRESS OR IMPLIE  D
  * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  * EVENT SHALL YASUO KAWACHI OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "delay.h"
//#include "platform_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM_NUM  TIM2
#define TIM_RCC  RCC_APB1Periph_TIM2 //TIM2_RCC
/* Private variables ---------------------------------------------------------*/
// TIM_TimeBaseInitTypeDef's order is {uint16_t TIM_Prescaler, uint16_t TIM_CounterMode, ,uint16_t TIM_Period, uint16_t TIM_ClockDivision, uint8_t TIM_RepetitionCounter}
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructureMicro = {42,TIM_CounterMode_Up,0,0,0};
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructureMili =  {(uint16_t)42000,TIM_CounterMode_Up,0,0,0};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  wait micro second
  * @param  wait_time : time to wait in micro second
  * @retval None
  */
void delay_us(uint16_t waittime)
{
  //Supply APB1 Clock
  RCC_APB1PeriphClockCmd(TIM_RCC , ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructureMicro.TIM_Period = ((waittime+1) * 1)-1;
  TIM_TimeBaseInit(TIM_NUM, &TIM_TimeBaseStructureMicro);

  TIM_SelectOnePulseMode(TIM_NUM, TIM_OPMode_Single);

  TIM_SetCounter(TIM_NUM,2);

  /* TIM enable counter */
  TIM_Cmd(TIM_NUM, ENABLE);

  while (TIM_GetCounter(TIM_NUM)){};

  /* TIM enable counter */
  TIM_Cmd(TIM_NUM, DISABLE);

}

/**
  * @brief  wait millisecond
  * @param  wait_time : time to wait in millisecond
  * @retval None
  */
void delay_ms(uint16_t waittime)
{
  //Supply APB1 Clock
  RCC_APB1PeriphClockCmd(TIM_RCC , ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructureMili.TIM_Period = ((waittime+1) * 1)-1;
  TIM_TimeBaseInit(TIM_NUM, &TIM_TimeBaseStructureMili);

  TIM_SelectOnePulseMode(TIM_NUM, TIM_OPMode_Single);

  TIM_SetCounter(TIM_NUM,2);

  /* TIM enable counter */
  TIM_Cmd(TIM_NUM, ENABLE);

  while (TIM_GetCounter(TIM_NUM)){};

  /* TIM enable counter */
  TIM_Cmd(TIM_NUM, DISABLE);
}
