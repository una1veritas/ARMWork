/****************************************************************************
 *   $Id:: i2cmontst.c 4242 2010-08-09 19:33:04Z usb00423                   $
 *   Project: NXP LPC11Uxx I2C example
 *
 *   Description:
 *     This file contains I2C slave test modules, main entry, to test I2C 
 *     slave APIs.
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

* Permission to use, copy, modify, and distribute this software and its 
* documentation is hereby granted, under NXP Semiconductors' 
* relevant copyright in the software, without fee, provided that it 
* is used in conjunction with NXP Semiconductors microcontrollers.  This 
* copyright, permission, and disclaimer notice must appear in all copies of 
* this code.

****************************************************************************/

#include "LPC11Uxx.h"			/* LPC11xx Peripheral Registers */
#include "type.h"
#include "i2cmonitor.h"

extern volatile uint8_t I2CRdBuffer[BUFSIZE];
extern volatile uint32_t I2CSlaveState;
extern volatile uint32_t I2CReadLength;

/*******************************************************************************
**   Main Function  main()
*******************************************************************************/
int main (void)
{
  uint32_t i;
  SystemCoreClockUpdate();
  
  for ( i = 0; i < BUFSIZE; i++ )
  {
	I2CRdBuffer[i] = 0x00;
  }
  
  I2CMonitorInit();			/* initialize I2c Monitor Mode */
 
  while ( 1 );
}

/******************************************************************************
**                            End Of File
******************************************************************************/
