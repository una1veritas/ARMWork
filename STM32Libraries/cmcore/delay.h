/*
 * delay5.h
 *
 *  Created on: 2012/11/15
 *      Author: sin
 */

#ifndef DELAY2_H_
#define DELAY2_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f4xx.h"
#include "cmcore.h"

extern volatile uint32_t __counter_micros;
extern volatile uint32_t __counter_millis;

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

uint32_t micros(void);
uint32_t millis(void);
	
void TIM2_delaytimer_start(void);
void TIM2_delaytimer_up(void);
void TIM2_delaytimer_reset(void);
void TIM2_IRQHandler(void);

void delay_ms(uint32_t);
void delay_us(uint32_t);
void countdown_start(uint32_t t);
uint8_t countdown(void);

#define delay(x)  delay_ms(x)
#define delayMicroseconds(x)  delay_us(x)


#ifdef __cplusplus
}
#endif


#endif /* DELAY5_H_ */
