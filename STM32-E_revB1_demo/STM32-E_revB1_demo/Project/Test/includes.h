/***************************************************************************
 **
 **
 **    Master include file
 **
 **    Used with ARM IAR C/C++ Compiler
 **
 **    (c) Copyright IAR Systems 2007
 **
 **    $Revision: #3 $
 **
 ***************************************************************************/

#ifndef __INCLUDES_H
#define __INCLUDES_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <intrinsics.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>

#include "arm_comm.h"
#include "iar_stm32f217ze_sk.h"

#include "stm32f2xx.h"
#include "stm32f2xx_conf.h"


//#include "drv_glcd_cnfg.h"
//#include "drv_glcd.h"
//#include "glcd_ll.h"
//#include "accl_drv.h"
#include "i2c1_drv.h"
//#include "iar_logo.h"

#include    "clock-arch.h"

#include    "timer.h"
#include    "uip-conf.h"
#include    "uipopt.h"
#include    "uip_arp.h"
#include    "uip.h"
#include    "tapdev.h"
#include    "httpd.h"

#include "usbd_core.h"
#include "usbd_req.h"
#include "usbd_ioreq.h"
#include "usb_dcd_int.h"
#include "usbd_hid_core.h"
#include "usbd_desc.h"
#include "usb_core.h"

//#include "temp_sensor_drv.h"

#include "disk.h"
#include "sd_card_mode.h"

#include "Sin_Table.h"
/*#include "buttons.h"

#include    "usbhost_inc.h"
#include    "io_cache.h"

#include "disk.h"
#include "sd_spi_mode.h"
#include "sd_ll_spi1.h"

#include "ntc.h"
#include "temperature.h"

*/
#endif  // __INCLUDES_H
