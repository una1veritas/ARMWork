/****************************************************************************
 *   $Id:: i2c.h 4058 2010-07-30 01:03:21Z usb00423                         $
 *   Project: NXP LPC11xx I2C example
 *
 *   Description:
 *     This file contains I2C code header definition.
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
#ifndef __I2C_H 
#define __I2C_H

/* If I2C SEEPROM is tested, make sure FAST_MODE_PLUS is 0.
For board to board test, this flag can be turned on. */

#include <stdio.h>
#include "armcmx.h"

#define FAST_MODE_PLUS      0

#define I2C_BUFSIZE             64
#define MAX_TIMEOUT         0x00FFFFFF

#define I2CMASTER           0x01
#define I2CSLAVE            0x02

//#define PCF8594_ADDR        0xA0
#define READ_WRITE          0x01

/* For more info, read NXP's SE95 datasheet */
#define SE95_ADDR			0x90
#define SE95_ID				0x05
#define SE95_CONFIG			0x01
#define SE95_TEMP			0x00

#define RD_BIT              0x01

// I2C State const
#define I2C_IDLE              0
#define I2C_STARTED           1
#define I2C_RESTARTED         2
#define I2C_REPEATED_START    3
#define DATA_ACK              4
#define DATA_NACK             5
#define I2C_BUSY              6
#define I2C_NO_DATA           7
#define I2C_NACK_ON_ADDRESS   8
#define I2C_NACK_ON_DATA      9
#define I2C_ARBITRATION_LOST  10
#define I2C_TIME_OUT          11
#define I2C_OK                12

#define I2CONSET_I2EN       (0x1<<6)  /* I2C Control Set Register */
#define I2CONSET_AA         (0x1<<2)
#define I2CONSET_SI         (0x1<<3)
#define I2CONSET_STO        (0x1<<4)
#define I2CONSET_STA        (0x1<<5)

#define I2CONCLR_AAC        (0x1<<2)  /* I2C Control clear Register */
#define I2CONCLR_SIC        (0x1<<3)
#define I2CONCLR_STAC       (0x1<<5)
#define I2CONCLR_I2ENC      (0x1<<6)

#define I2DAT_I2C           0x00000000  /* I2C Data Reg */
#define I2ADR_I2C           0x00000000  /* I2C Slave Address Reg */
#define I2SCLH_SCLH         0x00000180  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL         0x00000180  /* I2C SCL Duty Cycle Low Reg */
#define I2SCLH_HS_SCLH		0x00000015  /* Fast Plus I2C SCL Duty Cycle High Reg */
#define I2SCLL_HS_SCLL		0x00000015  /* Fast Plus I2C SCL Duty Cycle Low Reg */

#define I2C_MODE_WRITE    0
#define I2C_MODE_REQUEST  1
#define I2C_MODE_READ  2

typedef struct {
//  volatile uint32_t MasterState; // = I2C_IDLE;
//  volatile uint32_t SlaveState; // = I2C_IDLE;
  volatile uint32_t State;
  volatile uint32_t timeout; // = 0;
    
  volatile uint8_t DstAddr;
  volatile uint8_t Mode;

//  volatile uint8_t MasterBuffer[I2C_BUFSIZE];
  volatile uint8_t Buffer[I2C_BUFSIZE];
//  volatile uint8_t SlaveBuffer[I2C_BUFSIZE];
  volatile uint32_t Count; // = 0;
  volatile uint32_t ReadLength;
  volatile uint32_t WriteLength;
  
  volatile uint32_t RdIndex; // = 0;
  volatile uint32_t WrIndex; // = 0;
} I2CDef;

extern I2CDef i2c;

/*
void i2c_init(i2c * i2cxptr, I2C_TypeDef * I2Cx, GPIOPin scl, GPIOPin sda);
boolean i2c_begin(i2c * i2cxptr, uint32 clk);
//I2C_TypeDef * I2Cx, uint32_t clk);

void i2c_setup_comm(i2c * i2cxptr, I2C_Mode mode, uint8_t dstaddr, uint8_t * databuffer, uint16_t length);
boolean i2c_start_send(i2c * i2cxptr);
boolean i2c_start_receive(i2c * i2cxptr);

boolean i2c_transmit(i2c * i2cxptr, uint8_t addr, uint8_t * data, uint16_t length);
boolean i2c_request(i2c * I2Cbuf, uint8_t addr, uint8_t * data, uint16_t len);
boolean i2c_receive(i2c * I2Cbuf, uint8_t * data, uint16_t lim);
*/

void I2C_IRQHandler( void );
uint32 I2C_init(I2CDef * i2c, uint32 I2cMode );
uint32 I2C_start(I2CDef * i2c);
uint32 I2C_stop(I2CDef * i2c);
uint32 I2C_Engine(I2CDef * i2c);
//
uint8 I2C_write(I2CDef * i2c, uint8 addr, uint8 * data, size_t length);
uint8 I2C_read(I2CDef * i2c, uint8 addr, uint8 * data, size_t reqlen, size_t reclen);
uint8_t I2C_request(I2CDef * i2c, uint8_t addr, uint8_t * data, size_t reqlen);
uint8_t I2C_receive(I2CDef * i2c, uint8_t addr, uint8_t * data, size_t reclen);
//
uint8 I2C_write16(I2CDef * i2c, uint8 addr, uint16 data);

#endif /* end __I2C_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
