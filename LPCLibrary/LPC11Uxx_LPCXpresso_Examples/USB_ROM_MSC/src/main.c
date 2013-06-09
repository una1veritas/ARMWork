/***********************************************************************
 * $Id:: main.c 211 2012-01-13 21:34:04Z usb06052                      $
 *
 * Project: USB Mass storage class ROM driver
 *
 * Description:
 *     USB Mass storage class ROM driver example.
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

extern ErrorCode_t usb_msc_mem_init(USBD_HANDLE_T hUsb, USB_INTERFACE_DESCRIPTOR* pIntfDesc, uint32_t* mem_base, uint32_t* mem_size);
extern uint8_t USB_DeviceDescriptor[];
extern uint8_t USB_StringDescriptor[];
extern uint8_t USB_FsConfigDescriptor[];

/**********************************************************************
 ** Global data 
**********************************************************************/
volatile uint32_t u32Milliseconds = 0;
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
  address the wrong status in VBUSDebouncing bit in CmdStatus register. It
  happens on the NXP Validation Board only that a wrong ESD protection chip is used. */
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
  USB_CORE_DESCS_T desc;
  ErrorCode_t ret; 
 
  SystemCoreClockUpdate();

  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  UARTInit(115200);	
  UARTSend((uint8_t *)"\r\nLPC11Uxx USB ROMAPI example>", 31);

  /* get USB API table pointer */
  pUsbApi = (USBD_API_T*)((*(ROM **)(0x1FFF1FF8))->pUSBD);

  /* enable clocks and pinmux for usb0 */
  USB_pin_clk_init();

  /* initilize call back structures */
  memset((void*)&usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
  usb_param.usb_reg_base = LPC_USB_BASE;
  usb_param.mem_base = MSC_PARAM_MEM;
  usb_param.mem_size = MSC_RARAM_SIZE;
  usb_param.max_num_ep = 2;

/* Initialize Descriptor pointers */
  memset((void*)&desc, 0, sizeof(USB_CORE_DESCS_T));
  desc.device_desc = (uint8_t *)&USB_DeviceDescriptor[0];
  desc.string_desc = (uint8_t *)&USB_StringDescriptor[0];
  desc.full_speed_desc = (uint8_t *)&USB_FsConfigDescriptor[0];
  desc.high_speed_desc = (uint8_t *)&USB_FsConfigDescriptor[0];

  /* USB Initialization */
  ret = pUsbApi->hw->Init(&hUsb, &desc, &usb_param);  
  if (ret == LPC_OK) {
	usb_param.mem_base = 0x10001500;
	usb_param.mem_size = 0x300;
    ret = usb_msc_mem_init(hUsb, 
	    (USB_INTERFACE_DESCRIPTOR *)&USB_FsConfigDescriptor[sizeof(USB_CONFIGURATION_DESCRIPTOR)], 
	    &usb_param.mem_base, &usb_param.mem_size);
    if (ret == LPC_OK) {
      NVIC_EnableIRQ(USB_IRQn); //  enable USB interrrupts 
      /* now connect */
      pUsbApi->hw->Connect(hUsb, 1);	
    }
	else {
      UARTSend((uint8_t *)"\r\n usb_msc_mem_init error!!!", 31);
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
