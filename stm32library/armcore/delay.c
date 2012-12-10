/*
 * delay5.c
 *
 *  Created on: 2012/11/15
 *      Author: sin
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "delay.h"
//#include "platform_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

volatile uint32_t __counter_micros;
volatile uint32_t __counter_millis;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  millisecond
 * @param  none
 * @retval None
 */
void TIM2_timer_start(void) {
	// TIM_TimeBaseInitTypeDef's order is {uint16_t TIM_Prescaler, uint16_t TIM_CounterMode, uint16_t TIM_Period, uint16_t TIM_ClockDivision, uint8_t TIM_RepetitionCounter}
	TIM_TimeBaseInitTypeDef TimeBaseStructure;
//			= { 84, TIM_CounterMode_Up, 999, TIM_CKD_DIV1, 0 };

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.SYSCLK_Frequency>>1)/1000000L ;
	TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TimeBaseStructure.TIM_Period = 1000 - 1;
	TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseStructure.TIM_RepetitionCounter = 0;

	//Supply APB1 Clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseInit(TIM2, &TimeBaseStructure);
//  TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Repetitive);
	TIM_SetCounter(TIM2, 0);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM enable counter */
	TIM_Cmd(TIM2, ENABLE);

	__counter_micros = 0;
	__counter_millis = 0;
}

uint32_t micros(void) {
	return __counter_micros + TIM_GetCounter(TIM2 );
}

uint32_t millis(void) {
	return __counter_millis;
}

void delay_ms(uint32_t w) {
	uint32_t wtill = millis() + w;
	if ( wtill < millis() ) {
		while ( millis() >= wtill );
	}
	while (millis() < wtill)
		;
}

void delay_us(uint32_t w) {
	uint32_t wtill = micros() + w;
	if ( wtill < micros() ) {
		// overflow
		while ( micros() >= wtill );
	}
	while (micros() < wtill)
		;
}

void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update ) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update );
		__counter_micros += 1000;
		__counter_millis += 1;
	}
}
