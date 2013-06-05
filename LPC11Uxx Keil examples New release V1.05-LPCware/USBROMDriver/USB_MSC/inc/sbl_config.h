/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    SBL_CONFIG.H
 *      Purpose: USB Flash updater
 *      Version: V1.0
 *----------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------*/

#ifndef  _SBL_CONFIG_H
#define  _SBL_CONFIG_H

//*** <<< Use Configuration Wizard in Context Menu >>> ***

/*
// <h> Flash Configuration
//   <o0> User Start Sector <0-36>
//   <o1> Device Type
//        <36=> LPC1766
//   <o2> Code Read Protection
//        <0x11223344=> NO CRP <0x12345678=> CRP1 <0x87654321=> CRP2 <0x43218765=> CRP3
// </h>
*/


/*
// <h> Update Entry Pin
//   <o0> Port
//        <0x50014024=> Port 0 <0x50014064=> Port 1 <0x500140A4=> Port 2 <0x500140E4=> Port 3 <0x50014124=> Port 4
//   <o1> Pin <0-31>
// </h>
*/
//#define ISP_ENTRY_GPIO_REG 0x500140A4  /* Port */
//#define ISP_ENTRY_PIN 	   12          /* Pin  */

#define CCLK 48000 					/* 48,000 KHz for IAP call */

#define FLASH_BUF_SIZE 256
#define SECTOR_0_START_ADDR 0
#define SECTOR_SIZE 4096
#define MAX_USER_SECTOR 8
//#define USER_FLASH_START (sector_start_map[USER_START_SECTOR])
//#define USER_FLASH_END	 (sector_end_map[MAX_USER_SECTOR])
//#define USER_FLASH_SIZE  ((USER_FLASH_END - USER_FLASH_START) + 1)

                          
#endif  /* __SBL_CONFIG_H__ */
