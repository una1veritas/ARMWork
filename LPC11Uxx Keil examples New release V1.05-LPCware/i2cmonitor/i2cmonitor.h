/****************************************************************************
 *   $Id:: i2cmonitor.h 4242 2010-08-09 19:33:04Z usb00423                    $
 *   Project: NXP LPC11xx I2C Monitor example
 *
 *   Description:
 *     This file contains I2C monitor code header definition.
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
#ifndef __I2CMONITOR_H 
#define __I2CMONITOR_H
 
#define BUFSIZE             6
#define MAX_TIMEOUT         0x00FFFFFF

#define I2C_ADDR0           0x80
#define I2C_ADDR1           0x90
#define I2C_ADDR2           0xA0
#define I2C_ADDR3           0xB0

#define I2C_MASK0           0x80
#define I2C_MASK1           0x90
#define I2C_MASK2           0xA0
#define I2C_MASK3           0xB0

#define RD_BIT              0x01

#define I2C_IDLE            0
#define I2C_STARTED         1
#define I2C_RESTARTED       2
#define I2C_REPEATED_START  3
#define DATA_ACK            4
#define DATA_NACK           5
#define I2C_WR_STARTED      6
#define I2C_RD_STARTED      7

#define I2CONSET_I2EN       (0x1<<6)  /* I2C Control Set Register */
#define I2CONSET_AA         (0x1<<2)
#define I2CONSET_SI         (0x1<<3)
#define I2CONSET_STO        (0x1<<4)
#define I2CONSET_STA        (0x1<<5)

#define I2CONCLR_AAC        (0x1<<2)  /* I2C Control clear Register */
#define I2CONCLR_SIC        (0x1<<3)
#define I2CONCLR_STAC       (0x1<<5)
#define I2CONCLR_I2ENC      (0x1<<6)

#define MM_ENA				(0x1<<0)
#define EN_SCL				(0x1<<1)
#define MATCH_ALL			(0x1<<2)

#define I2SCLH_SCLH         0x00000180  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL         0x00000180  /* I2C SCL Duty Cycle Low Reg */

extern void I2C_IRQHandler( void );
extern void I2CMonitorInit( void );

#endif /* end __I2CMONITOR_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
