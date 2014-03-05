/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System:Drive
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config_MC_0.h
 * Purpose: File System Configuration for Memory Card Drive
 * Rev.:    V5.01
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>Memory Card Drive 0
#define MC0_ENABLE              1

//   <o>Connect to hardware via Driver_MCI# <0-255>
//   <i>Select driver control block for hardware interface
#define MC0_MCI_DRIVER          0

//   <o>Connect to hardware via Driver_SPI# <0-255>
//   <i>Select driver control block for hardware interface when in SPI mode
#define MC0_SPI_DRIVER          1

//   <o>Bus Mode <0=>Native <1=>SPI
//   <i>Define Memory Card bus interface mode.
#define MC0_SPI                 1
          
//   <o>File System Cache <0=>OFF <1=>1 KB <2=>2 KB <4=>4 KB
//                        <8=>8 KB <16=>16 KB <32=>32 KB
//   <i>Define System Cache buffer size for file IO. 
//   <i>Increase this number for faster r/w access.
//   <i>Default: 4 kB
#define MC0_CACHE_SIZE          4

//   <e>Relocate Cache Buffer
//   <i>Locate Cache Buffer at a specific address.
//   <i>Some devices require a Cache buffer for DMA transfer
//   <i>located at specific address.
#define MC0_CACHE_RELOC         0

//     <o>Base address <0x0000-0xFFFFFE00:0x200>
//     <i>Define the Cache buffer base address.
//     <i>For LPC23xx/24xx devices this is USB RAM
//     <i>starting at 0x7FD00000.
#define MC0_CACHE_ADDR          0x7FD00000

//   </e>

//   <q>FAT Journal
//   <i>Enable FAT Journal in order to guarantee
//   <i>fail-safe FAT file system operation.
#define MC0_FAT_JOURNAL         0

//   <q>Default Drive [M0:]
//   <i>Used when Drive letter not specified
#define MC0_DEFAULT_DRIVE       1

// </h>
