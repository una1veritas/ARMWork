/****************************************************************************
 *   $Id:: io_event.h 6172 2011-01-13 18:22:51Z usb00423                    $
 *   Project: NXP LPC11xx software example
 *
 *   Description:
 *     This file contains definition and prototype for GPIO.
 *
 ****************************************************************************
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
****************************************************************************/
#ifndef __IO_EVENT_H 
#define __IO_EVENT_H

#define PORT0		0
#define PORT1		1

void FLEX_INT0_IRQHandler(void);
void SetRemoteWakeupSource (void);

void GPIOInit( void );
void GPIOSetInterrupt( uint32_t ch_num, uint32_t sense, uint32_t event );
void GPIOIntEnable( uint32_t ch_num, uint32_t event );
void GPIOSetDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir );

#endif /* end __IO_EVENT_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
