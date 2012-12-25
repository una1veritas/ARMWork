/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : clock-arch.h
 *    Description : Implementation of architecture-specific clock
 *                  functionality include file
 *
 *    History :
 *    1. Date        : October 4, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: #1 $
**************************************************************************/
//#include "my_includes.h"
#include <stdint.h>
#include "stm32f2xx_conf.h"

#ifndef __CLOCK_ARCH_H__
#define __CLOCK_ARCH_H__

#define TICK_PER_SEC        100

typedef uint32_t clock_time_t;

#define CLOCK_CONF_SECOND TICK_PER_SEC

/*************************************************************************
 * Function Name: Tim1Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: TIM1 interrupt handler
 *
 *************************************************************************/
void Tim2Handler (void);

/*************************************************************************
 * Function Name: clock_init
 * Parameters: uint32_t IntrPriority
 *
 * Return: none
 *
 * Description: Timer init
 *
 *************************************************************************/
void clock_init(uint32_t IntrPriority);

/*************************************************************************
 * Function Name: clock_init
 * Parameters: none
 *
 * Return: none
 *
 * Description: The current clock time, measured in system ticks
 *
 *************************************************************************/
clock_time_t clock_time(void);

#endif /* __CLOCK_ARCH_H__ */
