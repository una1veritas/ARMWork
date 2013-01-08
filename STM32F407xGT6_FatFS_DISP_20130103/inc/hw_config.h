/********************************************************************************/
/*!
	@file			hw_config.h
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
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H 0x0100

#ifdef __cplusplus
 extern "C" {
#endif

/* General Inclusion */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_conf.h"
#include "platform_config.h"

/* Function Inclusion */
#include "systick.h"
#include "uart_support.h"
#include "rtc_support.h"

/* High Level Function */
#include "diskio.h"
#include "ff.h"
#include "ff_rtc_if.h"
#include "ff_support.h"
#include "term_io.h"
#include "display_if.h"
#include "display_if_support.h"
#include "ts_fileloads.h"

/* Macros */
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Externals */
extern __IO uint16_t CmdKey;
extern void Set_System(void);
extern void NVIC_Configuration(void);
extern void LED_Configuration(void);
extern void KEY_Configuration(void);
extern void disk_timerproc(void);
extern void JoyInp_Chk(void);
extern void Ext_SramInit(void);

#ifdef __cplusplus
}
#endif

#endif  /* __HW_CONFIG_H */
