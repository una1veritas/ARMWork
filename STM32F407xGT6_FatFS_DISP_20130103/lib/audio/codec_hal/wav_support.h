/********************************************************************************/
/*!
	@file			wav_support.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2012.11.30
	@brief          ChaN's Riff-Wave Player + I2C AudioCodec Upper Layer.
					Based On ChaN's FM3 uC Examples Thanks!
					
    @section HISTORY
		2012.11.30	V1.00	Start Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __WAV_SUPPORT_H
#define __WAV_SUPPORT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* General Inclusion */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

/* uC Specific Inclusion */
#include "stm32f4xx.h"
#include "systick.h"
#include "codec_if.h"
#include "ff.h"

/* General Definition */
#define FMT_LPCM 0x0001

/* Funcion Prototypes */
extern int load_wav(FIL *fp, const char *title,void *work , uint32_t sz_work);
/* Externs */


#ifdef __cplusplus
}
#endif

#endif	/* __WAV_SUPPORT_H */
