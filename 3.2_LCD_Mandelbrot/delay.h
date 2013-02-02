#ifndef DELAY_H_
#define DELAY_H_
#include "stm32f4xx.h"

void Delay(__IO uint32_t nTime);

void TimingDelay_Decrement(void);
static __IO uint32_t TimingDelay;

#endif
