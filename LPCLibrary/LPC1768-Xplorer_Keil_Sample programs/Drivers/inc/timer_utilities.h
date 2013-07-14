#ifndef __TIMER_UTILITIES_H
#define __TIMER_UTILITIES_H

#include "lpc_types.h"
extern uint32_t msec;

void SysTick_Handler (void);
int timer_delay_us( int count);
int Timer_delay_ms(int count);
#endif
