/********************************************************************************/
/*!
	@file			ff_support.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2012.08.27
	@brief          Interface of FatFs API Control.						@n
					Based on Chan's FatFs Test Terminal Thanks!

    @section HISTORY
		2012.08.27	V1.00	ReReStart Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __FF_SUPPORT_H
#define __FF_SUPPORT_H	0x0100

#ifdef __cplusplus
 extern "C" {
#endif

/* Basics */
#include <string.h> /* memset et al*/

/* Miscs */
#include "hw_config.h"
#include "diskio.h"
#include "ff.h"
#include "term_io.h"
#include "rtc_support.h"
#include "display_if.h"

/* Defines */
/* Determine FatFs Filer Fonts */
#define USE_FILER_FONTX		1	/* 0:use internal filer font*/
                                /* 1:use fontx user fonts */
/* Determine FatFs Buffer memories */
#if defined(USE_IJG_LIB) || defined(USE_LIBPNG) || defined(USE_GIFLIB)
 #define BUFSIZE	19*1024
#else
 #define BUFSIZE	32*1024
#endif

/* Function Prototypes */
extern void ff_support_timerproc(void);
extern int ff_test_term(void);

#ifdef __cplusplus
}
#endif

#endif /*__FF_SUPPORT_H */
