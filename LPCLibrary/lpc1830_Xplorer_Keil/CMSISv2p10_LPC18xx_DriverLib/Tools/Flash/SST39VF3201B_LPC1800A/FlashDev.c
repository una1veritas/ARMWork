/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright KEIL ELEKTRONIK GmbH 2003 - 2006                         */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Device Description for SST39VF3201B (16-bit Bus)      */
/*               connected to CS0 of LPC1800 on the Hitex board v2     */
/*                                                                     */
/***********************************************************************/

#include "..\FlashOS.H"        // FlashOS Structures


struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "SST39VF3201B @ LPC1800A-Hitex Board v2",         // Device Name
   EXT16BIT,                   // Device Type
   0x1C000000,                 // Device Start Address
   0x00400000,                 // Device Size in Bytes (4MB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   50,                        // Program Page Timeout 5 mSec
   50,                       // Erase Sector Timeout 100 mSec

// Specify the sector sizes and the relative start address of the first sector
   0x001000, 0x000000,         // Sector Size 4kB (1024 Sectors)
   SECTOR_END
};
