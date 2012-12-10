/*
 * systick.h
 *
 *  Created on: 2012/10/08
 *      Author: sin
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "misc.h"
//#include "stm32f4xx_it.h"

extern volatile uint32_t _systick_counter;

void SysTick_Handler(void);
void SysTick_delay(const uint32_t dlyTicks);
void SysTick_Start(void);
uint32_t SysTick_count(void);

#ifdef __cplusplus
	}
#endif

#endif /* SYSTICK_H_ */
