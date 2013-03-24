/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------------------------------------------           *
*                                                                             *
* Filename : interface.h                                                      *
* Description : This headerfile includes the right interface headerfile       *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; version 2                     *
* of the License.                                                             *
                                                                              *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* As a special exception, if other files instantiate templates or             *
* use macros or inline functions from this file, or you compile this          *
* file and link it with other works to produce a work based on this file,     *
* this file does not by itself cause the resulting work to be covered         *
* by the GNU General Public License. However the source code for this         *
* file must still be made available in accordance with section (3) of         *
* the GNU General Public License.                                             *
*                                                                             *
* This exception does not invalidate any other reasons why a work based       *
* on this file might be covered by the GNU General Public License.            *
*                                                                             *
*                                                    (c)2006 Lennart Yseboodt *
*                                                    (c)2006 Michael De Nil   *
\*****************************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

/*****************************************************************************/
#include "types.h"
#include "config.h"
/*****************************************************************************/

#if defined(HW_ENDPOINT_STM32F10X_SD)
	#include "sd_stm32.h"

#else
	#error "NO INTERFACE DEFINED - see interface.h"
#endif

#endif

