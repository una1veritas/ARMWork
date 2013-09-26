#ifndef _DELAY_H_
#define _DELAY_H_

#if ! defined (DELAY_SYSTICK) && ! defined (DELAY_TIMER16_1) 
#define DELAY_SYSTICK
#endif

#if defined (DELAY_SYSTICK)
#include "delay_systick.h"
#elif defined (DELAY_TIMER16_1)
#include "delay_timer16_1.h"
#endif

#endif
