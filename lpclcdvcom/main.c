/****************************************************************************
 *   $Id:: ssptest.c 4103 2010-08-02 20:56:20Z usb00423                     $
 *   Project: NXP LPC11Uxx SSP example
 *
 *   Description:
 *     This file contains SSP test modules, main entry, to test SSP APIs.
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

#include "LPC11Uxx.h"                        /* LPC11xx definitions */
#include "gpio.h"
#include "ssp.h"
//#if SSP_DEBUG
#include "usart.h"
//#endif

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"
//#include "serial.h"
//#include "vcomdemo.h"

//#define SSP_NUM			0

uint8_t src_addr[SSP_BUFSIZE]; 
uint8_t dest_addr[SSP_BUFSIZE];

/*****************************************************************************
** Function name:		LoopbackTest
**
** Descriptions:		Loopback test
**				
** parameters:			port #
** Returned value:		None
** 
*****************************************************************************/
void SSP_LoopbackTest(SPIDef * port)
{
  uint32_t i;

	/* Set SSEL pin to output low. */
	digitalWrite( port->SSel, LOW );
	i = 0;
	while ( i <= SSP_BUFSIZE )
	{
	  /* to check the RXIM and TXIM interrupt, I send a block data at one time 
	  based on the FIFOSIZE(8). */
	  SSP_Send( port, (uint8_t *)&src_addr[i], FIFOSIZE );
	  /* If RX interrupt is enabled, below receive routine can be
	  also handled inside the ISR. */
	  SSP_Receive( port, (uint8_t *)&dest_addr[i], FIFOSIZE );
	  i += FIFOSIZE;
	}
	/* Set SSEL pin to output high. */
	digitalWrite(port->SSel, HIGH );
  return;
}


/*----------------------------------------------------------------------------
 Initialises the VCOM port.
 Call this function before using VCOM_putchar or VCOM_getchar
 *---------------------------------------------------------------------------*/
void VCOM_Init(void) {

  CDC_Init ();
}


/*----------------------------------------------------------------------------
  Reads character from serial port buffer and writes to USB buffer
 *---------------------------------------------------------------------------*/

void VCOM_Serial2Usb(void) {
  static char serBuf [USB_CDC_BUFSIZE];
  int  numBytesRead, numAvailByte;
	
  //ser_AvailChar (&numAvailByte);
  numAvailByte = USART_available(&usart);
  if (numAvailByte > 0) {
    if (CDC_DepInEmpty) {
      for ( numBytesRead = 0; numBytesRead < numAvailByte && numBytesRead < USB_CDC_BUFSIZE; numBytesRead++) 
        serBuf[numBytesRead] = USART_read(&usart);  //ser_Read (&serBuf[0], &numAvailByte);
      
      CDC_DepInEmpty = 0;
      USB_WriteEP (CDC_DEP_IN, (unsigned char *)&serBuf[0], numBytesRead);
    }
  }

}

/*----------------------------------------------------------------------------
  Reads character from USB buffer and writes to serial port buffer
 *---------------------------------------------------------------------------*/
void VCOM_Usb2Serial(void) {
  static char serBuf [32];
  int  numBytesToRead, numBytesRead, numAvailByte;

  CDC_OutBufAvailChar (&numAvailByte);
  if (numAvailByte > 0) {
      numBytesToRead = numAvailByte > 32 ? 32 : numAvailByte; 
      numBytesRead = CDC_RdOutBuf (&serBuf[0], &numBytesToRead);
      //ser_Write (&serBuf[0], &numBytesRead);
      numBytesToRead = numBytesRead;
      for (numBytesRead = 0; numBytesRead < numBytesToRead; numBytesRead++)
        USART_write(&usart, serBuf[numBytesRead]);
  }

}


/*----------------------------------------------------------------------------
  checks the serial state and initiates notification
 *---------------------------------------------------------------------------*/
void VCOM_CheckSerialState (void) {
         unsigned short temp;
  static unsigned short serialState;

  temp = CDC_GetSerialState();
  if (serialState != temp) {
     serialState = temp;
     CDC_NotificationIn();                  // send SERIAL_STATE notification
  }
}

/*----------------------------------------------------------------------------
  Main Program
 *---------------------------------------------------------------------------*/

int main (void) {
  
  SystemCoreClockUpdate();

  USART_init(&usart, PIO0_18, PIO0_19);
  USART_begin(&usart, 9600);
  USART_puts(&usart, "Hello.\n");

  VCOM_Init();                              // VCOM Initialization

  USB_Init();                               // USB Initialization
  USB_Connect(TRUE);                        // USB Connect

  while (!USB_Configuration) ;              // wait until USB is configured

  while (1) {                               // Loop forever
    VCOM_Serial2Usb();                      // read serial port and initiate USB event
    VCOM_CheckSerialState();
	VCOM_Usb2Serial();
  } // end while											   
  
  SPI_init(&SPI0, PIO0_1, PIO0_1, PIO0_1, PIO0_2);			
	
  while ( 1 );
}

/******************************************************************************
**                            End Of File
******************************************************************************/

