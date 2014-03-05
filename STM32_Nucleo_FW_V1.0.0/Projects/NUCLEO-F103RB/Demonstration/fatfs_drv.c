/**
  ******************************************************************************
  * @file    fatfs_drv.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   diskio interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ff.h"
#include "diskio.h"
#include "stm32_adafruit_spi_usd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLOCK_SIZE 512 /* Block Size in Bytes */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile DSTATUS Stat = STA_NOINIT; /* Disk status */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Disk
  * @param  pdrv: Physical drive number
  * @retval DSTATUS: Operation status
  */
DSTATUS disk_initialize(BYTE pdrv)
{
  Stat = STA_NOINIT;
  
  if (pdrv == 0)
  {
    if (SD_ADAFRUIT_Init() == SD_RESPONSE_NO_ERROR)
    {
      Stat &= ~STA_NOINIT;
    }
  }
  
  return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number
  * @retval DSTATUS: Operation status
  */
DSTATUS disk_status (BYTE pdrv)
{
  if (pdrv == 0) /* Supports only single drive */
  {
    Stat = STA_NOINIT;
    
    if(SD_GetStatus() == 0xFF)
    {
      Stat &= ~STA_NOINIT;
    }
  }
  
  return Stat;
}

/**
  * @brief  Reads Sector
  * @param  pdrv: Physical drive number
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read
  * @retval DRESULT: Operation result
  */
DRESULT disk_read (BYTE pdrv, BYTE*buff, DWORD sector, UINT count)
{
  if (pdrv == 0)
  {
    if(SD_GetStatus() != 0xFF)
    {
      return(RES_NOTRDY);
    }
    
    SD_ReadBlock(buff, sector << 9, BLOCK_SIZE);
  }
  return RES_OK;
}

/**
  * @brief  Writes Sector
  * @param  pdrv: Physical drive number
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write
  * @retval DRESULT: Operation result
  * @note   The FatFs module will issue multiple sector transfer request
  *         (count > 1) to the disk I/O layer. The disk function should process
  *         the multiple sector transfer properly Do. not translate it into
  *         multiple single sector transfers to the media, or the data read/write
  *         performance may be drasticaly decreased.
  */
#if _USE_WRITE == 1
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
  if (pdrv == 0)
  {
    if(SD_GetStatus() != 0xFF)
    {
      return(RES_NOTRDY);
    }
    
    SD_WriteBlock((BYTE *)buff, sector << 9, BLOCK_SIZE); 
  }
  return RES_OK;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  Get current time 
  * @param  none
  * @retval DWORD: Current time
  */
DWORD get_fattime ()
{
  return(0);
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL != 0
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
  DRESULT res = RES_OK;
  SD_CardInfo cardinfo; 
  
  if (pdrv) return RES_PARERR;
  
  res = RES_ERROR;
  
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (cmd) {
    /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;
    
    /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    if(pdrv == 0)
    {
      SD_GetCardInfo(&cardinfo);  
      *(DWORD*)buff = cardinfo.CardCapacity / 512; 
    }

    res = RES_OK;
    break;
    
    /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :	 
    *(WORD*)buff = BLOCK_SIZE;
    res = RES_OK;
    break;
    
    /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    if(pdrv == 0)
      *(DWORD*)buff = cardinfo.CardBlockSize;
    else
      *(DWORD*)buff = 32;
    
  default:
    res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL == 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
