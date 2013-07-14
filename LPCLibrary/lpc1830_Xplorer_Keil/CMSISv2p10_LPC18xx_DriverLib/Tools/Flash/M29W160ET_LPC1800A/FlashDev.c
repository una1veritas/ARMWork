/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright KEIL ELEKTRONIK GmbH 2003 - 2007                         */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Device Description for ST M29W160ET (16-bit Bus)      */
/*               for NXP LPC1800 Eval board rev 4.1                    */
/*                                                                     */
/***********************************************************************/

#include "..\FlashOS.H"        // FlashOS Structures


struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "M29W160ET @ LPC1800A-NXP Evalboard v4.1",      // Device Name: this will be displayed when the user selects the flash algorithm.
                               // Please mention the processor type here, for the user it is important to see for which
                               // processor this flash driver has been compiled. In this case it's code for a Cortex-M4. 
   EXT16BIT,                   // Flash Device Type
   0x1C000000,                 // Device Start Address of the flash connected to CS0 of LPC4300
   0x00200000,                 // Device Size in Bytes (16MBit = 2MByte)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   3000,                       // Erase Sector Timeout 3000 mSec

// Specify Size and Address of Sectors
   0x10000, 0x000000,          // Sector Size 64kB (31 Sectors)
   0x08000, 0x1F0000,          // Sector Size 32kB (1 Sector)
   0x02000, 0x1F8000,          // Sector Size  8kB (2 Sectors)
   0x04000, 0x1FC000,          // Sector Size 16kB (1 Sector)
   SECTOR_END
};
