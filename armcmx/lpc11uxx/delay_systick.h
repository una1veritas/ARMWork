#ifndef _DELAY_SYSTICK_H_
#define _DELAY_SYSTICK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//#include "armcmx.h"

//#define SYSTICK_DELAY		(SystemCoreClock/100)

//extern volatile uint32_t __counter_systick;
extern volatile uint32_t __counter_micros;
extern volatile uint32_t __counter_millis;
  
void init_systick_delay(void);
//void enable_systick_delay(void);
void start_systick_delay(void);
//void disable_systick_delay(void);
void stop_systick_delay(void);
void reset_systick(void);

void SysTick_Handler(void);

uint32_t micros(void);
uint32_t millis(void);
	
void delay_ms(uint32_t);
void delay_us(uint32_t);

#define start_delay(x) init_systick_delay(x)
#define delay(x)  delay_ms(x)
#define delayMicroseconds(x)  delay_us(x)

#ifdef __cplusplus
}
#endif

#endif
