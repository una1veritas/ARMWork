/*
 * delay.c
 *
 *  Created on: 2012/09/29
 *      Author: sin
 */

#include "delay.h"

/**
 * @brief  Inserts a delay time.
 * @param  nTime: specifies the delay time length, in milliseconds.
 * @retval None
 */
void SysTick_delay(__IO uint32_t nTime) {
	TimingDelay = nTime;

	while (TimingDelay != 0)
		;
}

/**
 * @brief  Decrements the TimingDelay variable.
 * @param  None
 * @retval None
 */
void SysTick_decrement(void) {
	if (TimingDelay != 0x00) {
		TimingDelay--;
	}
}

uint8_t SysTick_init(uint32_t coreClockPerTick) {
	return (SysTick_Config(SystemCoreClock / coreClockPerTick));
}

void SysTick_start(uint32_t Tick) {
	RCC_ClocksTypeDef Clocks;
	volatile uint32_t dummy;

	RCC_GetClocksFreq(&Clocks);

	dummy = SysTick ->CTRL;
	SysTick ->LOAD = (Clocks.HCLK_Frequency / 8) / Tick;

	SysTick ->CTRL = 1;
}

void SysTick_stop(void) {
	SysTick ->CTRL = 0;
}
/*
void SysTick_Handler(void) {
	SysTick_decrement();
}
*/
