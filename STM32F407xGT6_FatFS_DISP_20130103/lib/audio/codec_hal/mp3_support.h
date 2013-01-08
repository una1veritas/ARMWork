/********************************************************************************/
/*!
	@file			mp3_support.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2012.11.30
	@brief          Helix-MP3 Decorder + I2C AudioCodec Upper Layer.
					Based On UnderURL Thanks!
					http://www.mikrocontroller.net/topic/252319

    @section HISTORY
		2012.11.30	V1.00	Start Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __MP3_SUPPORT_H
#define __MP3_SUPPORT_H

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
#include "mp3dec.h"
#include "codec_if.h"
#include "ff.h"

/* General Definition */
/* If u want to get advertize messages on decoding,
   uncomment this */
/*#define  MP3_SUPPORT_MESSAGES*/

/* Funcion Prototypes */
extern int load_mp3(FIL *fil, const char *filenamee, void *work, unsigned int sz_work);

/* Externs */


#ifdef __cplusplus
}
#endif

#endif	/* __MP3_SUPPORT_H */
