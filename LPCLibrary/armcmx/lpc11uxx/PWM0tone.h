/*
 */
#ifndef _PWM0TONE_H_
#define _PWM0TONE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "armcmx.h"
  
void TIMER16_0_IRQHandler(void);
void PWM0_toneOff(void);
void PWM0_tone(GPIOPin outpin, uint32_t freq, uint32_t duration);

#ifdef __cplusplus
}
#endif

#endif
