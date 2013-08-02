#ifndef _DELAY_H_
#define _DELAY_H_

#ifdef DELAY_SYSTICK
#include "delay_systick.h"
#else
#include "delay_timer16_1.h"
#endif

#endif
