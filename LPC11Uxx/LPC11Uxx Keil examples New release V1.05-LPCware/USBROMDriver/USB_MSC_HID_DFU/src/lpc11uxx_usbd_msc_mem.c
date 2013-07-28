/*----------------------------------------------------------------------------
 *      Name:    MEMORY.C
 *      Purpose: USB Mass Storage Demo
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include <string.h>
#include "mw_usbd_rom_api.h"
#include "sbl_iap.h"

/* MSC Disk Image Definitions */
/* Mass Storage Memory Layout */
#define MSC_BlockSize       512
#define MSC_MemorySize      ((uint32_t)(8 * 1024)) 
#define MSC_BlockCount      (MSC_MemorySize / MSC_BlockSize)

extern USBD_API_T* pUsbApi;


const uint8_t InquiryStr[] = {'N','X','P',' ',' ',' ',' ',' ',     \
                           'L','P','C',' ','M','e','m',' ',     \
                           'D','i','s','k',' ',' ',' ',' ',     \
                           '1','.','0',' ',};


uint8_t* Memory = (uint8_t*)0x6000;

void translate_rd( uint32_t offset, uint8_t** buff_adr, uint32_t length)
{
  uint32_t i;
  for ( i = 0; i<length; i++)
  {
    (*buff_adr)[i] = Memory[offset+i];
  }
}

void translate_wr( uint32_t offset, uint8_t** buff_adr, uint32_t length)
{
  write_flash((unsigned *)&Memory[offset],(char*)&((*buff_adr)[0]),length);
}

ErrorCode_t translate_verify( uint32_t offset, uint8_t* src, uint32_t length)
{
  if (memcmp((void*)&Memory[offset], src, length))
    return ERR_FAILED;

  return LPC_OK;
}

/* Main Program */

ErrorCode_t usb_msc_mem_init(USBD_HANDLE_T hUsb, USB_INTERFACE_DESCRIPTOR* pIntfDesc, uint32_t* mem_base, uint32_t* mem_size)
{
  USBD_MSC_INIT_PARAM_T msc_param;
  ErrorCode_t ret = LPC_OK;

  memset((void*)&msc_param, 0, sizeof(USBD_MSC_INIT_PARAM_T));
  msc_param.mem_base = *mem_base;
  msc_param.mem_size = *mem_size;
  /* mass storage paramas */
  msc_param.InquiryStr = (uint8_t*)InquiryStr; 
  msc_param.BlockCount = MSC_MemorySize / MSC_BlockSize;
  msc_param.BlockSize = MSC_BlockSize;
  msc_param.MemorySize = MSC_MemorySize;

  if ((pIntfDesc == 0) ||
    (pIntfDesc->bInterfaceClass != USB_DEVICE_CLASS_STORAGE) ||
        (pIntfDesc->bInterfaceSubClass != MSC_SUBCLASS_SCSI) )
    return ERR_FAILED;

  msc_param.intf_desc = (uint8_t*)pIntfDesc;
  /* user defined functions */
  msc_param.MSC_Write = translate_wr; 
  msc_param.MSC_Read = translate_rd;
  msc_param.MSC_Verify = translate_verify;   

  ret = pUsbApi->msc->init(hUsb, &msc_param);
  /* update memory variables */
  *mem_base = msc_param.mem_base;
  *mem_size = msc_param.mem_size;

  init_usb_iap();

  return ret;
}

