/****************************************************************************
 *   $Id:: i2cmonitor.c 4243 2010-08-09 20:32:05Z usb00423                  $
 *   Project: NXP LPC11Uxx I2C Monitor example
 *
 *   Description:
 *     This file contains I2C monitor code example which include I2C monitor 
 *     initialization, I2C interrupt handler, and APIs for I2C monitoring.
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

volatile uint32_t I2CMonitorState = I2C_IDLE;

volatile uint8_t I2CRdBuffer[BUFSIZE];
volatile uint8_t I2CWrBuffer[BUFSIZE];
volatile uint8_t I2CGeneralCall[BUFSIZE];
volatile uint32_t RdIndex = 0;
volatile uint32_t WrIndex = 0;
volatile uint32_t GCIndex = 0;

volatile uint8_t I2CGeneralCall[BUFSIZE];

/*****************************************************************************
** Function name:		I2C_IRQHandler
**
** Descriptions:		I2C interrupt handler, deal with master mode only.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void I2C_IRQHandler(void) 
{
  uint8_t StatValue;

  /* this handler deals with master read and master write only */
  StatValue = LPC_I2C->STAT;
  switch ( StatValue )
  {
	case 0x60:							/* An own SLA_W has been received. */
	case 0x68:
	RdIndex = 0;
	I2CRdBuffer[RdIndex++] = LPC_I2C->DAT;
	LPC_I2C->CONSET = I2CONSET_AA;		/* assert ACK after SLV_W is received */
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	I2CMonitorState = I2C_WR_STARTED;
	break;
	
	case 0x80:							/*  data receive */
	case 0x90:
	if ( I2CMonitorState == I2C_WR_STARTED )
	{
	  I2CRdBuffer[RdIndex++] = LPC_I2C->DAT;
	  LPC_I2C->CONSET = I2CONSET_AA;	/* assert ACK after data is received */
	}
	else
	{
	  LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK */
	}
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;
		
	case 0xA8:							/* An own SLA_R has been received. */
	case 0xB0:
	WrIndex = 0;
//	LPC_I2C->CONSET = I2CONSET_AA;		/* assert ACK after SLV_R is received */
	I2CWrBuffer[WrIndex++] = LPC_I2C->DAT;
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	I2CMonitorState = I2C_RD_STARTED;
	break;
	
	case 0xB8:							/* Data byte has been transmitted */
	case 0xC8:
	if ( I2CMonitorState == I2C_RD_STARTED )
	{
	  I2CWrBuffer[WrIndex++] = LPC_I2C->DAT; 
	  LPC_I2C->CONSET = I2CONSET_AA;	/* assert ACK  */
	}
	else
	{
	  LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK  */
	}	
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0xC0:							/* Data byte has been transmitted, NACK */
	if ( I2CMonitorState == I2C_RD_STARTED )
	{
	  I2CWrBuffer[WrIndex++] = LPC_I2C->DAT;
	}					
	LPC_I2C->CONCLR = I2CONCLR_AAC;		/* assert NACK  */
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	I2CMonitorState = DATA_NACK;
	break;

	case 0xA0:							/* Stop condition or repeated start has */
	LPC_I2C->CONSET = I2CONSET_AA;		/* been received, assert ACK.  */
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	I2CMonitorState = I2C_IDLE;
	break;

	case 0x70:							/* General call address received. */
	LPC_I2C->CONSET = I2CONSET_AA;		/* been received, assert ACK.  */
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	I2CGeneralCall[GCIndex++] = LPC_I2C->DAT;
	I2CMonitorState = I2C_IDLE;

	default:
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	LPC_I2C->CONSET = I2CONSET_I2EN | I2CONSET_SI;	
	break;
  }
  return;
}

/*****************************************************************************
** Function name:		I2CMonitorInit
**
** Descriptions:		Initialize I2C controller
**
** parameters:			I2c mode is either MASTER or SLAVE
** Returned value:		true or false, return false if the I2C
**						interrupt handler was not installed correctly
** 
*****************************************************************************/
void I2CMonitorInit( void ) 
{
  LPC_SYSCON->PRESETCTRL |= (0x1<<1);
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
  LPC_IOCON->PIO0_4 &= ~0x3F;	    /*  I2C I/O config */
  LPC_IOCON->PIO0_4 |= 0x01;		/* I2C SCL */
  LPC_IOCON->PIO0_5 &= ~0x3F;	
  LPC_IOCON->PIO0_5 |= 0x01;		/* I2C SDA */
//  LPC_IOCON->PIO0_4 |= (0x1<<10);	/* open drain pins */
//  LPC_IOCON->PIO0_5 |= (0x1<<10);	/* open drain pins */

  /*--- Clear flags ---*/
  LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;    

  /*--- Reset registers ---*/
  LPC_I2C->SCLL = I2SCLL_SCLL;
  LPC_I2C->SCLH = I2SCLH_SCLH;

  LPC_I2C->ADR0 = I2C_ADDR0;
  LPC_I2C->ADR1 = I2C_ADDR1;
  LPC_I2C->ADR2 = I2C_ADDR2;
  LPC_I2C->ADR3 = I2C_ADDR3;     
  
  I2CMonitorState = I2C_IDLE;

  LPC_I2C->MMCTRL = MM_ENA | MATCH_ALL;
//  LPC_I2C->MMCTRL = MM_ENA;

  /* Enable the I2C Interrupt */
  NVIC_EnableIRQ(I2C_IRQn);

  LPC_I2C->CONSET = I2CONSET_I2EN | I2CONSET_SI;
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

