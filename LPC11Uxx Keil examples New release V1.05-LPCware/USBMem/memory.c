/*----------------------------------------------------------------------------
 * Name:    memory.c
 * Purpose: USB Memory Storage Demo
 * Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else 
 *      gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "LPC11Uxx.h"                        /* LPC11xx definitions */
#include "type.h"
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "mscuser.h"
#include "memory.h"


extern uint8_t Memory[MSC_PhysicalMemorySize];           /* MSC Memory in RAM */

/* For WIN7 environment, the minimum FAT12 file system is 8K. On LPC11Uxx,
the current IRAM is only 4K, so, the maximum RAM allocated for RAM disk drive is 3K. 
Once the RAM disk drive pops out with MSC_MemorySize capacity, it doesn't mean
that you can access the maximum RAM capacity, but the MSC_PhysicalMemorySize minus
the minimum memory allocated for FAT12 Boot sector(approx. 2.5K), etc.  */

/* Main Program */

int main (void) 
{
  uint32_t n;

  for (n = 0; n < MSC_PhysicalMemorySize; n++) {     /* Copy Initial Disk Image */
    Memory[n] = DiskImage[n];               /*   from Flash to RAM     */
  }

  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */

  while (1);                                /* Loop forever */
}
