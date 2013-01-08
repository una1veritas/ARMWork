/*-----------------------------------------------------------------------*/
/* Glue functions for FatFs 				            (C)ChaN, 2010  	 */
/*-----------------------------------------------------------------------*/
/* 20120606 Nemui Trinomius												 */
/*   Adopted STM32F4/F2xx_StdPeriph_Driver from ChaN's MCI/NAND Sample!	 */
/*-----------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "diskio.h"
#include "sdio_stm32f4.h"

/* Defines -------------------------------------------------------------------*/
#define SECTOR_SIZE 	512		/* Must be Set "512" in use of SDCARD! */
#define SDIO_DRIVE		0		/* Physical Drive Number set to 0. */
#define SOCKWP			0		/* Write Protect Switch is not Supported... */

/* Variables -----------------------------------------------------------------*/
volatile SD_Error Status = SD_OK;
SD_CardStatus SDCardStatus;
static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */
static volatile uint32_t Timer1, Timer2;	/* 100Hz decrement timers */
 
/* Constants -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**************************************************************************/
/*! 
    @brief Initialize a Drive.
	@param  drv     : Physical drive number (0..).
    @retval DSTATUS :
*/
/**************************************************************************/
DSTATUS disk_initialize(uint8_t drv)
{ 
	switch (drv) 
	{
		case SDIO_DRIVE:
		{     
			/* Initialize SD Card */
			Status = SD_Init(); 

			if (Status != SD_OK)
				return STA_NOINIT;
			else
				return 0x00;
		}
  }

  return STA_NOINIT;

}


/**************************************************************************/
/*! 
    @brief Return Disk Status.
	@param  drv     : Physical drive number (0..).
    @retval DSTATUS :
*/
/**************************************************************************/
DSTATUS disk_status(uint8_t drv)
{
	switch (drv) 
	{
		case SDIO_DRIVE:
		{
			Status = SD_GetCardInfo(&SDCardInfo);

			if (Status != SD_OK)
				return STA_NOINIT;
			else
				return 0x00;
		}
	}
  
	return STA_NOINIT;
}


/**************************************************************************/
/*! 
    @brief Read Sector(s).
	@param  drv     : Physical drive number (0..).
	@param  *buff   : Data buffer to store read data.
	@param  sector  : Sector address (LBA).
	@param  count   : Number of sectors to read (1..255).
    @retval DSTATUS :
*/
/**************************************************************************/
DRESULT disk_read(uint8_t drv,uint8_t *buff,uint32_t sector,uint8_t count)
{
	switch (drv) 
	{
		case SDIO_DRIVE:
		{     
			Status = SD_OK;
	  
#if defined(SD_DMA_MODE) 	/* DMA Transfer */
			/* Read Multiple Blocks */
			Status = SD_ReadMultiBlocks((uint8_t*)(buff),(sector)*SECTOR_SIZE,SECTOR_SIZE,count);

			/* Check if the Transfer is finished */
			Status = SD_WaitReadOperation();

			/* Wait until end of DMA transfer */
			while(SD_GetStatus() != SD_TRANSFER_OK);

#else  /* Polling Transfer */
			for (int secNum = 0; secNum < count && Status == SD_OK; secNum++){
				Status = SD_ReadBlock((uint8_t*)(buff+SECTOR_SIZE*secNum),
								  (sector+secNum)*SECTOR_SIZE, 
								  (uint8_t)SECTOR_SIZE);
			}
#endif
		if (Status == SD_OK)	return RES_OK;
		else					return RES_ERROR;
    }
  }
  return RES_PARERR;
}


/**************************************************************************/
/*! 
    @brief Write Sector(s).
	@param  drv     : Physical drive number (0..).
	@param  *buff   : Data to be written.
	@param  sector  : Sector address (LBA).
	@param  count   : Number of sectors to write (1..255).
    @retval DSTATUS :
*/
/**************************************************************************/
#if _READONLY == 0
DRESULT disk_write(uint8_t drv,const uint8_t *buff,uint32_t sector,uint8_t count)
{
	switch (drv) 
	{
		case SDIO_DRIVE:
		{     
			Status = SD_OK;
	
#if defined(SD_DMA_MODE)  /* DMA Transfer */
			/* Write Multiple Blocks */
			Status = SD_WriteMultiBlocks((uint8_t*)(buff),(sector)*SECTOR_SIZE,SECTOR_SIZE,count);

			/* Check if the Transfer is finished */
			Status = SD_WaitWriteOperation();

			/* Wait until end of DMA transfer */
			while(SD_GetStatus() != SD_TRANSFER_OK);
	
#else  /* Polling Transfer */
			for (int secNum = 0; secNum < count && Status == SD_OK; secNum++){
				Status = SD_WriteBlock((uint8_t*)(buff+SECTOR_SIZE*secNum),
							  (sector+secNum)*SECTOR_SIZE, 
                              (uint8_t)SECTOR_SIZE);
			}
#endif
			if (Status == SD_OK)	return RES_OK;
			else					return RES_ERROR;
		}
	}
	return RES_PARERR;
}
#endif /* _READONLY */


/**************************************************************************/
/*! 
    @brief Miscellaneous Functions.
	@param  drv     : Physical drive number (0..).
	@param  ctrl    : Control code.
	@param  *buff   : Buffer to send/receive control data.
    @retval DSTATUS :
*/
/**************************************************************************/
DRESULT disk_ioctl(uint8_t drv,uint8_t ctrl,void *buff)
{
	switch (drv) 
	{
		case SDIO_DRIVE:
		{
		  switch (ctrl)
		  {
			case CTRL_SYNC:
			  /* no synchronization to do since not buffering in this module */
			  return RES_OK;
			case GET_SECTOR_SIZE:
			  *(uint16_t*)buff = SECTOR_SIZE;
			  return RES_OK;
			case GET_SECTOR_COUNT:
			  *(uint32_t*)buff = SDCardInfo.CardCapacity / SECTOR_SIZE;
			  return RES_OK;
			case GET_BLOCK_SIZE:
			  *(uint32_t*)buff = SDCardInfo.CardBlockSize;
			  return RES_OK;
			 
			/* Following command are not used by FatFs module */
			case MMC_GET_TYPE :		/* Get MMC/SDC type (uint8_t) */
				*(uint8_t*)buff = SDCardInfo.CardType;
				return RES_OK;
			case MMC_GET_CSD :		/* Read CSD (16 bytes) */
				memcpy((void *)buff,&SDCardInfo.SD_csd,16);
				return RES_OK;
			case MMC_GET_CID :		/* Read CID (16 bytes) */
				memcpy((void *)buff,&SDCardInfo.SD_cid,16);
				return RES_OK;
			case MMC_GET_OCR :		/* Read OCR (4 bytes) */
				*(uint32_t*)buff = SDCardInfo.SD_csd.MaxRdCurrentVDDMin;
				return RES_OK;
			case MMC_GET_SDSTAT :	/* Read SD status (64 bytes) */
				SD_GetCardStatus(&SDCardStatus);
				memcpy((void *)buff,&SDCardStatus,64);
				return RES_OK;
			default :
				return RES_OK;

			}
		}
	}
	return RES_PARERR;
}


/**************************************************************************/
/*! 
    @brief Device Timer Interrupt Procedure.								@n
		   This function must be called in period of 10ms.
	@param  none
    @retval none
*/
/**************************************************************************/
void disk_timerproc(void)
{
	uint8_t n, s;

	n = Timer1;					/* 100Hz decrement timer */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;


	s = Stat;
	if (SOCKWP)					/* WP is H (write protected) */
		s |= STA_PROTECT;
	else						/* WP is L (write enabled) */
		s &= ~STA_PROTECT;

	if (!SD_Detect())			/* INS = H (Socket empty) */
		s |= (STA_NODISK | STA_NOINIT);
	else						/* INS = L (Card inserted) */
		s &= ~STA_NODISK;
	Stat = s;
}

/* End Of File ---------------------------------------------------------------*/
