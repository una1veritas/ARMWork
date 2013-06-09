/***********************************************************************
 * $Id:: main.c 211 2012-01-11 21:34:04Z usb06052                      $
 *
 * Project: USB HID
 *
 * Description:
 *     A USB HID (using ROM Drivers) example.
 *
 ***********************************************************************
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
 **********************************************************************/
#include <string.h>
#include "LPC11Uxx.h"          
#include "mw_usbd_rom_api.h"          
#include "power_api.h"
#include "uart.h"

extern ErrorCode_t usb_hid_init(USBD_HANDLE_T hUsb, USB_INTERFACE_DESCRIPTOR* pIntfDesc, uint32_t* mem_base, uint32_t* mem_size);
extern ErrorCode_t USB_Configure_Event (USBD_HANDLE_T hUsb);
extern uint8_t USB_DeviceDescriptor[];
extern uint8_t USB_StringDescriptor[];
extern uint8_t USB_FsConfigDescriptor[];

/**********************************************************************
 ** Global data 
**********************************************************************/
USBD_API_T* pUsbApi;

/* local data */
static USBD_HANDLE_T hUsb;


void USB_IRQHandler(void)
{
  pUsbApi->hw->ISR(hUsb);
}

void USB_pin_clk_init(void)
{
  /* Enable AHB clock to the GPIO domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

  /* Enable AHB clock to the USB block and USB RAM. */
  LPC_SYSCON->SYSAHBCLKCTRL |= ((0x1<<14)|(0x1<<27));

  /* Pull-down is needed, or internally, VBUS will be floating. This is to
  address the wrong status in VBUSDebouncing bit in CmdStatus register (applies to rev A) */
  LPC_IOCON->PIO0_3   &= ~0x1F; 
//  LPC_IOCON->PIO0_3   |= ((0x1<<3)|(0x01<<0));	/* Secondary function VBUS */
  LPC_IOCON->PIO0_3   |= (0x01<<0);			/* Secondary function VBUS */
  LPC_IOCON->PIO0_6   &= ~0x07;
  LPC_IOCON->PIO0_6   |= (0x01<<0);			/* Secondary function SoftConn */

  return;
}

/*****************************************************************************
**   Main Function  main()
*****************************************************************************/
int main (void)
{
  USBD_API_INIT_PARAM_T usb_param;
	ErrorCode_t ret;
  USB_CORE_DESCS_T desc;

  SystemCoreClockUpdate();

  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  UARTInit(115200);	
  UARTSend((uint8_t *)"\r\nLPC11Uxx USB ROMAPI example>", 31);

  /* get USB API table pointer */
  pUsbApi = (USBD_API_T*)((*(ROM **)(0x1FFF1FF8))->pUSBD);  

  /* enable clocks and pinmux for usb */
  USB_pin_clk_init();

  /* initilize call back structures */
  memset((void*)&usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
  usb_param.usb_reg_base = LPC_USB_BASE;
  usb_param.mem_base = 0x10001000;
  usb_param.mem_size = 0x1000;
  usb_param.max_num_ep = 2;
  usb_param.USB_Configure_Event = USB_Configure_Event;

/* Initialize Descriptor pointers */
  memset((void*)&desc, 0, sizeof(USB_CORE_DESCS_T));
  desc.device_desc = (uint8_t *)&USB_DeviceDescriptor[0];
  desc.string_desc = (uint8_t *)&USB_StringDescriptor[0];
  desc.full_speed_desc = (uint8_t *)&USB_FsConfigDescriptor[0];
  desc.high_speed_desc = (uint8_t *)&USB_FsConfigDescriptor[0];
  
  /* USB Initialization */
  ret = pUsbApi->hw->Init(&hUsb, &desc, &usb_param);  
  if (ret == LPC_OK) 
  {
    ret = usb_hid_init(hUsb, (USB_INTERFACE_DESCRIPTOR *)&USB_FsConfigDescriptor[sizeof(USB_CONFIGURATION_DESCRIPTOR)], 
			&usb_param.mem_base, &usb_param.mem_size);
    if (ret == LPC_OK) {
      NVIC_EnableIRQ(USB_IRQn); //  enable USB interrrupts 
      /* now connect */
      pUsbApi->hw->Connect(hUsb, 1);	
    }
  }                       
  else {    
    UARTSend((uint8_t *)"\r\nhwUSB_Init error!!!", 21);
  }

	while (1);
}

/**********************************************************************
 **                            End Of File
 **********************************************************************/
