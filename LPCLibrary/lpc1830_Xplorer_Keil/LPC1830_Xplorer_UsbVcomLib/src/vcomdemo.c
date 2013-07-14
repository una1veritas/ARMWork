/*----------------------------------------------------------------------------
 *      Name:    vcomdemo.c
 *      Purpose: USB virtual COM port Demo
 *      Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else
 *      gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "lpc18xx.H"                        /* LPC23xx definitions */
#include "lpc_types.h"
#include "lpc18xx_scu.h"
#include "lpc18xx_uart.h"
#include <stdio.h>

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"
#include "usbuser.h"

#include "lpc18xx_cgu.h"
#include "lpc18xx_libcfg_default.h"
#include "debug_frmwrk.h"
#include "serial.h"
#include "vcomdemo.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup USBDEV_USBCDC	USBCDC
 * @ingroup USBDEV_Examples
 * @{
 */
volatile uint32_t systicks=0;

extern DQH_T ep_QH[EP_NUM_MAX];
extern DTD_T ep_TD[EP_NUM_MAX];
extern uint32_t DevStatusFS2HS;

extern void USB_EndPoint0 (uint32_t event);

//void IOInit(void);
/*----------------------------------------------------------------------------
 Initialises the VCOM port.
 Call this function before using VCOM_putchar or VCOM_getchar
 *---------------------------------------------------------------------------*/
void VCOM_Init(void) {
#if PORT_NUM
  CDC_Init (1);
#else
  CDC_Init (0);
#endif
}


/*----------------------------------------------------------------------------
  Reads character from serial port buffer and writes to USB buffer
 *---------------------------------------------------------------------------*/
void VCOM_Serial2Usb(void) {
#ifdef __ICCARM__
#pragma data_alignment=4
  static char serBuf [USB_CDC_BUFSIZE];
#else
#ifdef __GNUC__
#define __align(x) __attribute__((aligned(x)))
#endif
  __align(4) static char serBuf [USB_CDC_BUFSIZE];
#endif
         int  numBytesRead, numAvailByte;

  ser_AvailChar (&numAvailByte);
  if (numAvailByte > 0) {
    if (CDC_DepInEmpty) {
      numBytesRead = ser_Read (&serBuf[0], &numAvailByte);

      CDC_DepInEmpty = 0;
	  USB_WriteEP (CDC_DEP_IN, (unsigned char *)&serBuf[0], numBytesRead);
    }
  }

}

/*----------------------------------------------------------------------------
  Reads character from USB buffer and writes to serial port buffer
 *---------------------------------------------------------------------------*/
static volatile uint32_t i=0; 
void Vcom(void) {
#ifdef __ICCARM__
#pragma data_alignment=4
    static char serBuf [32];
#else
#ifdef __GNUC__
#define __align(x) __attribute__((aligned(x)))
#endif
  __align(4) static char serBuf [20];
#endif
	  sprintf(serBuf," Test iteration:%d\r",i++);
	  USB_WriteEP (CDC_DEP_IN, (unsigned char *)&serBuf,strlen(serBuf));

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
  Vcom_init
 *---------------------------------------------------------------------------*/
void Vcom_init (void) {

	LPC_USBDRV_INIT_T usb_cb;

	/* initilize call back structures */
	memset((void*)&usb_cb, 0, sizeof(LPC_USBDRV_INIT_T));
	usb_cb.USB_Reset_Event = USB_Reset_Event;
	usb_cb.USB_P_EP[0] = USB_EndPoint0;
	usb_cb.USB_P_EP[1] = USB_EndPoint1;
	usb_cb.USB_P_EP[2] = USB_EndPoint2;
	usb_cb.ep0_maxp = USB_MAX_PACKET0;

  USB_Init(&usb_cb);                               // USB Initialization
  USB_Connect(TRUE);                        // USB Connect

  while (!USB_Configuration) ;              // wait until USB is configured
} 


/**
 * @}
 */
