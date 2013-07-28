/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbcfg.h
 * Purpose: USB Custom Configuration
 * Version: V1.20
 *----------------------------------------------------------------------------
 * This software is supplied "AS IS" without any warranties, express,
 * implied or statutory, including but not limited to the implied
 * warranties of fitness for purpose, satisfactory quality and
 * noninfringement. Keil extends you a royalty-free right to reproduce
 * and distribute executable files created using this software for use
 * on NXP ARM microcontroller devices only. Nothing else gives
 * you the right to use this software.
 *
 * Copyright (c) 2005-2009 Keil Software.
 * Adaption to LPCxxxx, Copyright (c) 2009 NXP.
 *----------------------------------------------------------------------------*/

#ifndef __USBCFG_H__
#define __USBCFG_H__

#define USB_MAX_IF_NUM  8
#define USB_MAX_EP_NUM  5
#define USB_MAX_PACKET0 64
/* Max In/Out Packet Size */
#define USB_FS_MAX_BULK_PACKET  64
/* Full speed device only */
#define USB_HS_MAX_BULK_PACKET  USB_FS_MAX_BULK_PACKET
/* IP3511 is full speed only */
#define FULL_SPEED_ONLY

#define CDC_MEM_BASE           0x20000000
#define CDC_MEM_SIZE           0x00001000
#define USB_CDC_CIF_NUM   0
#define USB_CDC_DIF_NUM   1
#define USB_CDC_EP_BULK_IN   USB_ENDPOINT_IN(2)
#define USB_CDC_EP_BULK_OUT  USB_ENDPOINT_OUT(2)
#define USB_CDC_EP_INT_IN    USB_ENDPOINT_IN(1)


#endif  /* __USBCFG_H__ */
