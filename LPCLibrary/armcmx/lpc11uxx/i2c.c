/****************************************************************************
 *   $Id:: i2c.c 4058 2010-07-30 01:03:21Z usb00423                         $
 *   Project: NXP LPC11Uxx I2C example
 *
 *   Description:
 *     This file contains I2C code example which include I2C initialization, 
 *     I2C interrupt handler, and APIs for I2C access.
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

#include <string.h>

#include "LPC11Uxx.h"			/* LPC11xx Peripheral Registers */
#include "armcmx.h"
#include "i2c.h"

I2CDef i2c;

/*
 volatile uint32_t I2CMasterState = I2C_IDLE;
 volatile uint32_t I2CSlaveState = I2C_IDLE;
 volatile uint32_t timeout = 0;

 volatile uint32_t I2CMode;

 volatile uint8_t I2CMasterBuffer[I2C_BUFSIZE];
 volatile uint8_t I2CSlaveBuffer[I2C_BUFSIZE];
 volatile uint32_t I2CCount = 0;
 volatile uint32_t I2CReadLength;
 volatile uint32_t I2CWriteLength;

 volatile uint32_t RdIndex = 0;
 volatile uint32_t WrIndex = 0;
 */

/* 
 From device to device, the I2C communication protocol may vary, 
 in the example below, the protocol uses repeated start to read data from or 
 write to the device:
 For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO 
 for master write: the sequence is: STA,Addr(W),offset,RE-STA,Addr(w),data...STO
 Thus, in state 8, the address is always WRITE. in state 10, the address could 
 be READ or WRITE depending on the I2C command.
 */

/*****************************************************************************
 ** Function name:		I2C_IRQHandler
 **
 ** Descriptions:		I2C interrupt handler, deal with master mode only.
 **
 ** parameters:			None
 ** Returned value:		None
 ** 
 *****************************************************************************/
void I2C_IRQHandler(void) {
	uint8_t StatValue;

	i2c.timeout = 0;
	/* this handler deals with master read and master write only */
	StatValue = LPC_I2C->STAT;
	switch (StatValue) {
	case 0x08: /* A Start condition is issued. */
		i2c.WrIndex = 0;
		i2c.RdIndex = 0;
		LPC_I2C->DAT = i2c.MasterBuffer[i2c.WrIndex++];
		LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
		break;

	case 0x10: /* A repeated started is issued */
		i2c.RdIndex = 0;
		/* Send SLA with R bit set, */
		LPC_I2C->DAT = i2c.MasterBuffer[i2c.WrIndex++];
		LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
		break;

	case 0x18: /* Regardless, it's a ACK */
		if (i2c.WriteLength == 1) {
			LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
			i2c.State = I2C_NO_DATA;
		} else {
			LPC_I2C->DAT = i2c.MasterBuffer[i2c.WrIndex++];
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x28: /* Data byte has been transmitted, regardless ACK or NACK */
		if (i2c.WrIndex < i2c.WriteLength) {
			LPC_I2C->DAT = i2c.MasterBuffer[i2c.WrIndex++]; /* this should be the last one */
		} else {
			if (i2c.ReadLength != 0) {
				LPC_I2C->CONSET = I2CONSET_STA; /* Set Repeated-start flag */
			} else {
				LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
				i2c.State = I2C_OK;
			}
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x30:
		LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
		i2c.State = I2C_NACK_ON_DATA;
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x40: /* Master Receive, SLA_R has been sent */
		if ((i2c.RdIndex + 1) < i2c.ReadLength) {
			/* Will go to State 0x50 */
			LPC_I2C->CONSET = I2CONSET_AA; /* assert ACK after data is received */
		} else {
			/* Will go to State 0x58 */
			LPC_I2C->CONCLR = I2CONCLR_AAC; /* assert NACK after data is received */
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x50: /* Data byte has been received, regardless following ACK or NACK */
		i2c.SlaveBuffer[i2c.RdIndex++] = LPC_I2C->DAT;
		if ((i2c.RdIndex + 1) < i2c.ReadLength) {
			LPC_I2C->CONSET = I2CONSET_AA; /* assert ACK after data is received */
		} else {
			LPC_I2C->CONCLR = I2CONCLR_AAC; /* assert NACK on last byte */
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x58:
		i2c.SlaveBuffer[i2c.RdIndex++] = LPC_I2C->DAT;
		i2c.State = I2C_OK;
		LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
		LPC_I2C->CONCLR = I2CONCLR_SIC; /* Clear SI flag */
		break;

	case 0x20: /* regardless, it's a NACK */
	case 0x48:
		LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
		i2c.State = I2C_NACK_ON_ADDRESS;
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x38: /* Arbitration lost, in this example, we don't
	 deal with multiple master situation */
	default:
		i2c.State = I2C_ARBITRATION_LOST;
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;
	}
	return;
}

/*****************************************************************************
 ** Function name:		I2CStart
 **
 ** Descriptions:		Create I2C start condition, a timeout
 **				value is set if the I2C never gets started,
 **				and timed out. It's a fatal error. 
 **
 ** parameters:			None
 ** Returned value:		true or false, return false if timed out
 ** 
 *****************************************************************************/
uint32_t I2C_start(I2CDef * i2c) {
	uint32_t timeout = 0;
	uint32_t retVal = FALSE;

	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2CONSET_STA; /* Set Start flag */

	/*--- Wait until START transmitted ---*/
	while (1) {
		if (i2c->State == I2C_STARTED) {
			retVal = TRUE;
			break;
		}
		if (timeout >= MAX_TIMEOUT) {
			retVal = FALSE;
			break;
		}
		timeout++;
	}
	return (retVal);
}

/*****************************************************************************
 ** Function name:		I2CStop
 **
 ** Descriptions:		Set the I2C stop condition, if the routine
 **				never exit, it's a fatal bus error.
 **
 ** parameters:			None
 ** Returned value:		true or never return
 ** 
 *****************************************************************************/
uint32_t I2C_stop(I2CDef * i2c) {
	LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
	LPC_I2C->CONCLR = I2CONCLR_SIC; /* Clear SI flag */

	/*--- Wait for STOP detected ---*/
	while (LPC_I2C->CONSET & I2CONSET_STO)
		;
	return TRUE;
}

/*****************************************************************************
 ** Function name:		I2CInit
 **
 ** Descriptions:		Initialize I2C controller
 **
 ** parameters:			I2c mode is either MASTER or SLAVE
 ** Returned value:		true or false, return false if the I2C
 **				interrupt handler was not installed correctly
 ** 
 *****************************************************************************/
uint32_t I2C_init(I2CDef * i2c, uint32_t I2cMode) {
	LPC_SYSCON->PRESETCTRL |= (0x1 << 1);

	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 5);
	LPC_IOCON->PIO0_4 &= ~0x3F; /*  I2C I/O config */
	LPC_IOCON->PIO0_4 |= 0x01; /* I2C SCL */
	LPC_IOCON->PIO0_5 &= ~0x3F;
	LPC_IOCON->PIO0_5 |= 0x01; /* I2C SDA */
	/* IOCON may change in the next release, save change for future references. */
//  LPC_IOCON->PIO0_4 |= (0x1<<10);	/* open drain pins */
//  LPC_IOCON->PIO0_5 |= (0x1<<10);	/* open drain pins */
	/*--- Clear flags ---*/
	LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC
			| I2CONCLR_I2ENC;

	/*--- Reset registers ---*/
#if FAST_MODE_PLUS
	LPC_IOCON->PIO0_4 |= (0x1<<9);
	LPC_IOCON->PIO0_5 |= (0x1<<9);
	LPC_I2C->SCLL = I2SCLL_HS_SCLL;
	LPC_I2C->SCLH = I2SCLH_HS_SCLH;
#else
	LPC_I2C->SCLL = I2SCLL_SCLL;
	LPC_I2C->SCLH = I2SCLH_SCLH;
#endif

// set initial value idle
	i2c->State = I2C_IDLE;
//	i2c->SlaveState = I2C_IDLE;

	if (I2cMode == I2CSLAVE) {
//		LPC_I2C->ADR0 = PCF8594_ADDR;
	}

	/* Enable the I2C Interrupt */
	NVIC_EnableIRQ (I2C_IRQn);

	LPC_I2C->CONSET = I2CONSET_I2EN;
	return (TRUE);
}

/*****************************************************************************
 ** Function name:		I2CEngine
 **
 ** Descriptions:		The routine to complete a I2C transaction
 **				from start to stop. All the intermitten
 **				steps are handled in the interrupt handler.
 **				Before this routine is called, the read
 **				length, write length, I2C master buffer,
 **				and I2C command fields need to be filled.
 **				see i2cmst.c for more details. 
 **
 ** parameters:			None
 ** Returned value:		true or false, return false only if the
 **				start condition can never be generated and
 **				timed out. 
 ** 
 *****************************************************************************/
uint32_t I2C_Engine(I2CDef * i2c) {
	i2c->RdIndex = 0;
	i2c->WrIndex = 0;

	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2CONSET_STA; /* Set Start flag */

	i2c->State = I2C_BUSY;

	while (i2c->State == I2C_BUSY) {
		if (i2c->timeout >= MAX_TIMEOUT) {
			i2c->State = I2C_TIME_OUT;
			break;
		}
		i2c->timeout++;
	}
	LPC_I2C->CONCLR = I2CONCLR_STAC;

	return (i2c->State);
}

uint8_t I2C_transmit(I2CDef * i2c, uint8_t addr, uint8_t * data, size_t length) {
	i2c->WriteLength = length + 1;
	i2c->ReadLength = 0;
	i2c->MasterBuffer[0] = addr;
	memcpy((void*) (i2c->MasterBuffer + 1), data, length);
	return !I2C_Engine(i2c);
}

uint8_t I2C_write16(I2CDef * i2c, uint8_t addr, uint16_t data) {
	uint8_t buf[2];
	buf[0] = data >> 8;
	buf[1] = data & 0xff;
	return I2C_transmit(i2c, addr, buf, 2);
}

uint8_t I2C_read(I2CDef * i2c, uint8_t addr, uint8_t * data, size_t reqlen,
		size_t rcvlen) {
	int i;
	/* Write SLA(W), address, SLA(R), and read one byte back. */
	i2c->WriteLength = reqlen+1;
	i2c->ReadLength = rcvlen+1;
	i2c->MasterBuffer[0] = addr & 0xFE;
	memcpy((void*) (i2c->MasterBuffer + 1), data, reqlen);
  i2c->MasterBuffer[reqlen+1] = addr | RD_BIT;
	I2C_Engine(i2c);
	//if(!I2CEngine()) return -1;

	i = 0;
	while (rcvlen--) {
		*data++ = i2c->SlaveBuffer[i++];
	}
	return 0;
}

uint8_t I2C_request(I2CDef * i2c, uint8_t addr, uint8_t * data, size_t reqlen) {
	/* Write SLA(W), address, SLA(R), and read one byte back. */
	i2c->WriteLength = reqlen;
	i2c->ReadLength = 0;
	i2c->MasterBuffer[0] = addr & 0xFE;
	memcpy((void*) (i2c->MasterBuffer + 1), data, reqlen);
  //  I2CMasterBuffer[2] = addr | RD_BIT;
  return 1;
}

uint8_t I2C_receive(I2CDef * i2c, uint8_t addr, uint8_t * data, size_t rcvlen) {
	int i;
	i2c->WriteLength = 1;
	/* Write SLA(W), address, SLA(R), and read one byte back. */
	i2c->ReadLength = rcvlen;

  i2c->MasterBuffer[0] = addr | RD_BIT;

	I2C_Engine(i2c);
	//if(!I2CEngine()) return -1;

	i = 0;
	while (rcvlen--) {
		*data++ = i2c->SlaveBuffer[i++];
	}
	return 0;
}


/******************************************************************************
 **                            End Of File
 ******************************************************************************/
