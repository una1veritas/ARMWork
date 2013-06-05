/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    mscuser.h
 * Purpose: Mass Storage Class Custom User Definitions
 * Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC family microcontroller devices only. Nothing 
 *      else gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#ifndef __MSCUSER_H__
#define __MSCUSER_H__

/* For WIN7 environment, the minimum FAT12 file system is 8K. For LPC11Uxx,
the current IRAM is only 4K, so, the maximum RAM allocated for RAM disk drive is 3K. 
Once the RAM disk drive pops out with MSC_MemorySize capacity, it doesn't mean
that you can access the maximum RAM capacity, but the MSC_PhysicalMemorySize minus
the minimum memory allocated for FAT12 Boot sector(approx. 2.5K), etc.  */

/* Mass Storage Memory Layout */
#define MSC_PhysicalMemorySize  (1024 * 3)    /* = 0x0C00 (3kB)*/
/* For compliance test, to fake the disk size will cause failure in compliance test,
so, set MemorySize the same as PhysicalMemorySize, 3K. Otherwise, fake it, make 
MSC_MemorySize at least 8K. Further investigation is needed on this. */
#if USB_COMPLIANCE_ENABLED 
#define MSC_MemorySize  MSC_PhysicalMemorySize
#else
#define MSC_MemorySize  (1024 * 8)            /* = 0x2000 (8kB)*/
#endif
#define MSC_BlockSize   512
#define MSC_BlockCount  (MSC_MemorySize / MSC_BlockSize)


/* Max In/Out Packet Size */
#define MSC_MAX_PACKET  64

/* MSC In/Out Endpoint Address */
#define MSC_EP_IN       0x82
#define MSC_EP_OUT      0x02

/* MSC Requests Callback Functions */
extern uint32_t MSC_Reset     (void);
extern uint32_t MSC_GetMaxLUN (void);

/* MSC Bulk Callback Functions */
extern void MSC_GetCBW (void);
extern void MSC_SetCSW (void);
extern void MSC_BulkIn (void);
extern void MSC_BulkOut(void);


#endif  /* __MSCUSER_H__ */
