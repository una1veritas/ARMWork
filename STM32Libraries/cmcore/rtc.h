#ifndef _RTC_H_
#define _RTC_H_

#include "cmcore.h"

#ifdef __cplusplus
extern "C" {
#endif

extern __IO uint32_t AsynchPrediv, SynchPrediv;

//#define RTC_CLOCK_SOURCE_LSI  0x00
	/* LSI used as RTC source clock. The RTC Clock */
#define RTC_CLOCK_SOURCE_LSE  0x01
	/* LSE used as RTC source clock */

void RTC_Config(void);
	
uint32 RTC_getTime(void);
uint32 RTC_getCalDate(void);
uint32 RTC_getAlarm(void);

#ifdef __cplusplus
}
#endif

#endif //_RTC_H_
