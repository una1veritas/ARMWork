/********************************************************************************/
/*!
	@file			rtc_support.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2011.06.12
	@brief          Based on ST Microelectronics's Sample Thanks!

    @section HISTORY
		2011.06.12	V1.00	Start Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __RTC_SUPPORT_H
#define __RTC_SUPPORT_H 0x0100

#ifdef __cplusplus
 extern "C" {
#endif

/* General Inclusion */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"

/* Defines */
#define 	RTC_CLOCK_SOURCE_LSE

/* Function Prototypes */
extern void RTC_Configuration(void);

/* External Glovals */
extern __IO uint32_t TimeDisplay;
extern RTC_TimeTypeDef   RTC_TimeStructure;
extern RTC_DateTypeDef   RTC_DateStructure;

#define ts_hour	RTC_TimeStructure.RTC_Hours
#define ts_min  RTC_TimeStructure.RTC_Minutes
#define ts_sec  RTC_TimeStructure.RTC_Seconds
#define ts_mon  RTC_DateStructure.RTC_Month
#define ts_mday RTC_DateStructure.RTC_Date 
#define ts_year RTC_DateStructure.RTC_Year

#ifdef __cplusplus
}
#endif

#endif /* __RTC_SUPPORT_H */
