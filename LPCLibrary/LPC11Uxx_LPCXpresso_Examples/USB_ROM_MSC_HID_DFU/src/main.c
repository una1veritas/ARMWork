/***********************************************************************
 * $Id:: main.c 211 2012-01-17 21:34:04Z usb06052                      $
 *
 * Project: USB_MSC_HID_DFU
 *
 * Description:
 *     A USB_MSC_HID_DFU example.
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
extern ErrorCode_t usb_dfu_init(USBD_HANDLE_T hUsb, USB_INTERFACE_DESCRIPTOR* pIntfDesc, uint32_t* mem_base, uint32_t* mem_size);
extern ErrorCode_t usb_hid_init(USBD_HANDLE_T hUsb, USB_INTERFACE_DESCRIPTOR* pIntfDesc, uint32_t* mem_base, uint32_t* mem_size);
extern ErrorCode_t USB_Configure_Event (USBD_HANDLE_T hUsb);
extern uint8_t USB_DeviceDescriptor[];
extern uint8_t USB_StringDescriptor[];
extern uint8_t USB_FsConfigDescriptor[];
extern uint8_t USB_dfuConfigDescriptor[];

/**********************************************************************
 ** Global data 
**********************************************************************/
volatile uint32_t u32Milliseconds = 0;
volatile uint32_t dfu_detach_sig = 0;
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
  address the wrong status in VBUSDebouncing bit in CmdStatus register (applies to Rev A). */
  LPC_IOCON->PIO0_3   &= ~0x1F; 
//  LPC_IOCON->PIO0_3   |= ((0x1<<3)|(0x01<<0));	/* Secondary function VBUS */
  LPC_IOCON->PIO0_3   |= (0x01<<0);			/* Secondary function VBUS */
  LPC_IOCON->PIO0_6   &= ~0x07;
  LPC_IOCON->PIO0_6   |= (0x01<<0);			/* Secondary function SoftConn */

  return;
}

void dfu_detach(USBD_HANDLE_T hUsb)
{
#if (USE_DFU_DESC || USE_COMP_DESC)
  USB_CORE_CTRL_T* pCtrl = (USB_CORE_CTRL_T*)hUsb;

  pCtrl->full_speed_desc = USB_dfuConfigDescriptor;
  pCtrl->high_speed_desc = USB_dfuConfigDescriptor;
#endif

  dfu_detach_sig = 1;
}

/*****************************************************************************
**   Main Function  main()
*****************************************************************************/
int main (void)
{
  USBD_API_INIT_PARAM_T usb_param;
  USB_CORE_DESCS_T desc;
  ErrorCode_t ret;
  USB_INTERFACE_DESCRIPTOR* pIntfDesc;
  USB_COMMON_DESCRIPTOR *pD;
  uint32_t next_desc_adr, total_len = 0; 

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
  usb_param.mem_base = 0x10000800;
  usb_param.mem_size = 0x0800;
  usb_param.max_num_ep = 4;
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

    pD = (USB_COMMON_DESCRIPTOR *)desc.high_speed_desc;
    next_desc_adr = (uint32_t)desc.high_speed_desc;

    while (pD->bLength) {
      if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {
        
        pIntfDesc = (USB_INTERFACE_DESCRIPTOR*)pD;
        
        switch(pIntfDesc->bInterfaceClass) {
          case USB_DEVICE_CLASS_STORAGE:
            ret = usb_msc_mem_init(hUsb, pIntfDesc, &usb_param.mem_base, &usb_param.mem_size);
            if (ret != LPC_OK)
              UARTSend((uint8_t *)"\r\n usb_msc_mem_init error!!!", 31);
            break;
          case USB_DEVICE_CLASS_APP:
            ret = usb_dfu_init(hUsb, pIntfDesc, &usb_param.mem_base, &usb_param.mem_size);
            if (ret != LPC_OK)
              UARTSend((uint8_t *)"\r\n usb_dfu_init error!!!", 27);
            break;
          case USB_DEVICE_CLASS_HUMAN_INTERFACE:
            ret = usb_hid_init(hUsb, pIntfDesc, &usb_param.mem_base, &usb_param.mem_size);
            if (ret != LPC_OK)
              UARTSend((uint8_t *)"\r\n usb_hid_init error!!!", 27);
            break;
        }
        if (ret != LPC_OK)
          break; /* break while loop no point proceeding further */
      }
      pIntfDesc = 0;
      total_len += pD->bLength;
      next_desc_adr = (uint32_t)pD + pD->bLength;
      pD = (USB_COMMON_DESCRIPTOR*)next_desc_adr;
    }

    if (total_len != ((USB_CONFIGURATION_DESCRIPTOR *)desc.high_speed_desc)->wTotalLength)
      UARTSend((uint8_t *)"\r\nBadly formed config descriptor!!!", 38);  

    if (ret == LPC_OK) {
      NVIC_EnableIRQ(USB_IRQn); //  enable USB interrrupts 
      /* now connect */
      pUsbApi->hw->Connect(hUsb, 1);	
    }

  }                       
  else {
    UARTSend((uint8_t *)"\r\nhwUSB_Init error!!!", 21);
  }

	while (1) 
	{                           				
	  u32Milliseconds = 100;
      if (dfu_detach_sig) {
        /* disconnect */
        pUsbApi->hw->Connect(hUsb, 0);
        /* for current test wrap-around condition is overlooked */
        while (u32Milliseconds < 98);
        /* connect the device back */
        pUsbApi->hw->Connect(hUsb, 1);
        dfu_detach_sig = 0;
      }		
	  /* Wait... */
	  while(u32Milliseconds);
	}
}

/**********************************************************************
 ** Function name:		
 **
 ** Description:		
 **						
 ** Parameters:			
 **
 ** Returned value:		
 **********************************************************************/
void SysTick_Handler (void) 					
{           
	if(u32Milliseconds > 0)
	{
		u32Milliseconds--; 
	}
}

/**********************************************************************
 **                            End Of File
 **********************************************************************/
