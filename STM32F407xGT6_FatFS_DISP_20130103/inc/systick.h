/********************************************************************************/
/*!
	@file			systick.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        3.00
    @date           2012.04.25
	@brief          Based on ST Microelectronics's Sample Thanks!
					For STM32F2/F4 Only!

    @section HISTORY
		2011.06.12	V1.00	Start Here.
		2012.03.20  V2.00	Fixed Timer Settings in _delay_us();
		2012.04.25  V3.00	ReFixed Timer Settings for _delay_us();

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __SYSTICK_H
#define __SYSTICK_H 0x0300

#ifdef __cplusplus
 extern "C" {
#endif

/* General Inclusion */
#include "platform_config.h"

/* Systick Interval MilliSecond order (in Hz) */
#define INTERVAL		1000UL
/* Timer Interval MicroSecond order (in Hz) */
#define USEC_INTERVAL 	1000000UL

/* Inline Static Function */
extern __IO uint32_t TimingDelay;
// inline 
	 static void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/* Externals */
extern void _delay_ms(__IO uint32_t mSec);
extern void _delay_us(__IO uint32_t uSec);
extern void Delay(__IO uint32_t uCount);
extern void SysTickInit(__IO uint32_t interval);

#ifdef __cplusplus
}
#endif

#endif	/* __SYSTICK_H */
