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
#include "app_usbd_cfg.h"
#include "sbl_iap.h"


extern USBD_API_T* pUsbApi;

/* DFU boot definitions */
#define DFU_DEST_BASE         0x4000
#define DFU_MAX_IMAGE_LEN     (8 * 1024)
#define DFU_MAX_BLOCKS        (DFU_MAX_IMAGE_LEN/USB_DFU_XFER_SIZE)

extern void dfu_detach(USBD_HANDLE_T hUsb);


/* return 
* DFU_STATUS_ : values defined in mw_usbd_dfu.h in case of errors
* 0 : If end of memory reached
* length : Amount of data copied to destination buffer
*/
uint32_t dfu_rd( uint32_t block_num, uint8_t** pBuff, uint32_t length)
{
  uint32_t src_addr = DFU_DEST_BASE;

  if ( length != 0 )
  {
    if (block_num == DFU_MAX_BLOCKS)
      return 0;
  
    if (block_num > DFU_MAX_BLOCKS)
      return DFU_STATUS_errADDRESS;
    
    src_addr += (block_num * USB_DFU_XFER_SIZE);
    //*pBuff = (uint8_t*)src_addr;
    
    memcpy((void*)(*pBuff), (void*)src_addr, length);
  }
  return length;
}

/* return DFU_STATUS_ values defined in mw_usbd_dfu.h */
uint8_t dfu_wr( uint32_t block_num, uint8_t** pBuff, uint32_t length,
  uint8_t* bwPollTimeout)
{
  bwPollTimeout[0] = 255;
//  bwPollTimeout[1] = 255;
//  bwPollTimeout[2] = 255;
  if ( length != 0 )
  {
    uint32_t dest_addr = DFU_DEST_BASE;
  
    if (block_num >= DFU_MAX_BLOCKS)
      return DFU_STATUS_errADDRESS;
    
    dest_addr += (block_num * USB_DFU_XFER_SIZE);
    write_flash((unsigned *)dest_addr,(char*)&((*pBuff)[0]),length);
  }
  return DFU_STATUS_OK;
}

void dfu_done(void)
{
  return;  
}

/* Main Program */
ErrorCode_t usb_dfu_init(USBD_HANDLE_T hUsb, USB_INTERFACE_DESCRIPTOR* pIntfDesc, uint32_t* mem_base, uint32_t* mem_size)
{
  USBD_DFU_INIT_PARAM_T dfu_param;
  ErrorCode_t ret = LPC_OK;

  memset((void*)&dfu_param, 0, sizeof(USBD_DFU_INIT_PARAM_T));
  dfu_param.mem_base = *mem_base;
  dfu_param.mem_size = *mem_size;
  /* DFU paramas */
  dfu_param.wTransferSize = USB_DFU_XFER_SIZE;

  if ((pIntfDesc == 0) ||
    (pIntfDesc->bInterfaceClass != USB_DEVICE_CLASS_APP) ||
        (pIntfDesc->bInterfaceSubClass != USB_DFU_SUBCLASS) )
    return ERR_FAILED;

  dfu_param.intf_desc = (uint8_t*)pIntfDesc;
  /* user defined functions */
  dfu_param.DFU_Write = dfu_wr; 
  dfu_param.DFU_Read = dfu_rd;
  dfu_param.DFU_Done = dfu_done; 
  dfu_param.DFU_Detach = dfu_detach;

  ret = pUsbApi->dfu->init(hUsb, &dfu_param, 0);
  /* update memory variables */
  *mem_base = dfu_param.mem_base;
  *mem_size = dfu_param.mem_size;
  return ret;
}


