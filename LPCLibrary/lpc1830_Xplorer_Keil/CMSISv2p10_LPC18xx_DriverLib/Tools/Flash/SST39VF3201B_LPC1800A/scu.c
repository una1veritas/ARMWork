/***********************************************************************
 * $Id: scu.c 8313 2011-10-14 22:30:17Z nxp21346 $
 *
 * Project: LPC18xx Common
 *
 * Description:
 *     This file contains code to configure the PINMUX
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#if defined CORE_M3 || defined CORE_M4
#include "LPC18xx.h"                    /* LPC18xx definitions                */
#endif
#ifdef CORE_M0s
#include "LPC18xx_m0s.h"                /* LPC18xx definitions                */
#endif
#ifdef CORE_M0a
#include "LPC18xx_m0a.h"                /* LPC18xx definitions                */
#endif

#include "type.h"
#include "scu.h"

void scu_pinmux(unsigned port, unsigned pin, unsigned mode, unsigned func)
{
  volatile unsigned int * const scu_base=(unsigned int*)(LPC_SCU_BASE);
  scu_base[(PORT_OFFSET*port+PIN_OFFSET*pin)/4]=mode+func;
} /* scu_pinmux */
