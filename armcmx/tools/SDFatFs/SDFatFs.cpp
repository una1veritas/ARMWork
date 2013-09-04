 /*
  * Wrapper for FatFs by ChaN
  */
  
#include "SDFatFs.h"

SDFatFs SD(SPI0, PIO0_2, PIO1_16, PIO1_19);

uint32 SDFatFs::time;
uint32 SDFatFs::cal;

void SDFatFs::begin(void) {
  pinMode(pin_cs, OUTPUT);
  if ( pin_detect != NOT_A_PIN || pin_detect != PIN_NOT_DEFINED )
    pinMode(pin_detect, INPUT);
  if ( pin_busyled != NOT_A_PIN || pin_busyled != PIN_NOT_DEFINED )
    pinMode(pin_busyled, OUTPUT);
  f_mount(0, &fatfs);		/* Register volume work area (never fails) */
}
  
// for get_fattime
uint32_t SDFatFs::fattime(void) {
  uint8_t y,m,d, hh, mm, ss;
  y = 20 + (cal>>16&0x0f) + (cal>>20&0x0f)*10;
  m = (cal>>8&0x0f) + (cal>>12&0x0f)*10;
  d = (cal&0x0f) + (cal>>4&0x0f)*10;
  hh = (time>>16&0x0f) + (time>>20&0x0f)*10;
  mm = (time>>8&0x0f) + (time>>12&0x0f)*10;
  ss = (time&0x0f) + (time>>4&0x0f)*10;
  
  return ((uint32_t)y<<25) | m<<21 | d<<16 | hh << 11 | mm<<5 | ss>>1;
}


void SDFatFile::open(const char * fname, const uint8_t mode) {
  digitalWrite(PIO1_19, LOW);
  peeked = false;
  sdfs.rescode = f_open(&file, fname, mode);
  if (mode == FILE_WRITE && !sdfs.rescode ) {
    sdfs.rescode = f_lseek(&file, f_size(&file));
  }
}

void SDFatFile::close(void) {
  sdfs.rescode = f_close(&file);
  digitalWrite(PIO1_19, LOW);
}

size_t SDFatFile::write(const uint8_t * buf, size_t num) {
  UINT count;
  sdfs.rescode = f_write(&file, buf, num, &count);
  return count;
}

int SDFatFile::read(void) {
  UINT n = 0;
  if ( !peeked )
    sdfs.rescode = f_read(&file, &rbuf, 1, &n);
  peeked = false;
  return rbuf;
}

int SDFatFile::peek(void) {
  UINT n = 0;
  if ( !peeked ) 
    sdfs.rescode = f_read(&file, &rbuf, 1, &n);
  peeked = true;
  return rbuf;
}

char * SDFatFile::gets(char * buff, size_t sz) {
  if ( peeked ) {
    peeked = false;
    *buff = rbuf;
    return f_gets((TCHAR*)buff+1, sz-1, &file);      
  } else {
    return f_gets((TCHAR*)buff, sz, &file);
  }
}



/*--------------------------------------------------------------------------

   FatFs Lower Layer functions

---------------------------------------------------------------------------*/

#include "diskio.h"
#include "mmc_sd.h"

//#define xchg_spi(x)  SPI_transfer(&SPI0Def, (x))

static volatile
DSTATUS Stat = STA_NOINIT;	/* Physical drive status */

static volatile
UINT Timer1, Timer2;	/* 1kHz decrement timer stopped at zero (disk_timerproc()) */

static
BYTE CardType;			/* Card type flags */

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Send/Receive data to the MMC  (Platform dependent)                    */
/*-----------------------------------------------------------------------*/

/* Receive multiple byte */

static void rcvr_spi_multi(BYTE *buff,	UINT btr /* Number of bytes to receive (16, 64 or 512) */ ) {
  int i;

  SD.setDataSize(SPI_DSS_8BIT);

  for ( i = 0; i < btr; i++) {
    /*
    SPI0Def.SSPx->DR = 0xff; //buff[i];
    while ( !(SPI0Def.SSPx->SR & SSPSR_RNE) || (SPI0Def.SSPx->SR & SSPSR_BSY) );
    buff[i] = SPI0Def.SSPx->DR;
    */ 
    buff[i] = SPI_transfer(&SPI0Def, 0xff);
  }
}


/* Send multiple byte */
static
void xmit_spi_multi (
	const BYTE *buff,	/* Pointer to the data */
	UINT btx			/* Number of bytes to send (512) */
)
{
	UINT n; // = 512;
	WORD d;

	//SPI0Def.SSPx->CR0 = 0x000F;			/* Select 16-bit mode */
  SD.setDataSize(SPI_DSS_16BIT);

  /*
	for (n = 0; n < 8; n++) {	// Push 8 frames into pipeline
		d = *buff++;
		d = (d << 8) | *buff++;
		SPI0Def.SSPx->DR = d;
	}
	btx -= 16;
	do {						// Transmit data block 
		d = *buff++;
		d = (d << 8) | *buff++;
		while (!(SPI0Def.SSPx->SR & SSPSR_RNE )); // _BV(2) )) ;
		SPI0Def.SSPx->DR; SPI0Def.SSPx->DR = d;
	} while (btx -= 2);
	for (n = 0; n < 8; n++) {
		while (!(SPI0Def.SSPx->SR & SSPSR_RNE )); //_BV(2) )) ;
		SPI0Def.SSPx->DR;
	}
  */
  for(n = 0; n < btx; n++) {
    d = *buff++;
    d <<= 8;
    d |= *buff++;
    d = SD.transfer(0xff);
  }
  
	//SPI0Def.SSPx->CR0 = 0x0007;			// Select 8-bit mode 
  SD.setDataSize(SPI_DSS_8BIT);
}


/*-----------------------------------------------------------------------*/
/* Initialize disk drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive number (0) */
)
{
	BYTE n, cmd, ty, ocr[4];


	if (drv) return STA_NOINIT;			/* Supports only drive 0 */
	// SD.begin(); //power_on();							/* Initialize SPI */

	if (Stat & STA_NODISK) return Stat;	/* Is card existing in the soket? */

	SD.setClockDivider(SPI_CLOCK_DIV64); //FCLK_SLOW();
	for (n = 10; n; n--) 
    SD.transfer(0xFF);	/* Send 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Put the card SPI/Idle state */
		Timer1 = 1000;						/* Initialization timeout = 1 sec */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
			for (n = 0; n < 4; n++) ocr[n] = SD.transfer(0xFF);	/* Get 32 bit return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* Is the card supports vcc of 2.7-3.6V? */
				while (Timer1 && send_cmd(ACMD41, 1UL << 30)) ;	/* Wait for end of initialization with ACMD41(HCS) */
				if (Timer1 && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = SD.transfer(0xFF);
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* Card id SDv2 */
				}
			}
		} else {	/* Not SDv2 card */
			if (send_cmd(ACMD41, 0) <= 1) 	{	/* SDv1 or MMC? */
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 (ACMD41(0)) */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 (CMD1(0)) */
			}
			while (Timer1 && send_cmd(cmd, 0)) ;		/* Wait for end of initialization */
			if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set block length: 512 */
				ty = 0;
		}
	}
	CardType = ty;	/* Card type */
	SD.deselect();

	if (ty) {			/* OK */
		SD.setClockDivider(SPI_CLOCK_DIV4); //FCLK_FAST();			/* Set fast clock */
		Stat &= ~STA_NOINIT;	/* Clear STA_NOINIT flag */
	} else {			/* Failed */
		SD.end(); //power_off();
		Stat = STA_NOINIT;
	}

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Send a command packet to the MMC                                      */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (		/* Return value: R1 resp (bit7==1:Failed to send) */
	BYTE cmd,		/* Command index */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* Send a CMD55 prior to ACMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select card */
	SD.deselect();
	if (!SD.select()) return 0xFF;

	/* Send command packet */
	SD.transfer(0x40 | cmd);				/* Start + command index */
	SD.transfer((BYTE)(arg >> 24));		/* Argument[31..24] */
	SD.transfer((BYTE)(arg >> 16));		/* Argument[23..16] */
	SD.transfer((BYTE)(arg >> 8));			/* Argument[15..8] */
	SD.transfer((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	SD.transfer(n);

	/* Receive command resp */
	if (cmd == CMD12) SD.transfer(0xFF);	/* Diacard following one byte when CMD12 */
	n = 10;								/* Wait for response (10 bytes max) */
	do
		res = SD.transfer(0xFF);
	while ((res & 0x80) && --n);

	return res;							/* Return received response */
}




/*-----------------------------------------------------------------------*/
/* Receive a data packet from the MMC                                    */
/*-----------------------------------------------------------------------*/

static
int rcvr_datablock (	/* 1:OK, 0:Error */
	BYTE *buff,			/* Data buffer */
	UINT btr			/* Data block length (byte) */
)
{
	BYTE token;


	Timer1 = 200;
	do {							/* Wait for DataStart token in timeout of 200ms */
		token = SD.transfer(0xFF);
		/* This loop will take a time. Insert rot_rdq() here for multitask envilonment. */
	} while ((token == 0xFF) && Timer1);
	if(token != 0xFE) return 0;		/* Function fails if invalid DataStart token or timeout */

	rcvr_spi_multi(buff, btr);		/* Store trailing data to the buffer */
	SD.transfer(0xFF); SD.transfer(0xFF);			/* Discard CRC */

	return 1;						/* Function succeeded */
}


/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
int wait_ready (	/* 1:Ready, 0:Timeout */
	UINT wt			/* Timeout [ms] */
)
{
	BYTE d;
  
  wt += millis();

	Timer2 = wt;
	do {
		d = SD.transfer(0xFF);
		/* This loop takes a time. Insert rot_rdq() here for multitask envilonment. */
	} while (d != 0xFF && wt > millis() ); 
  //Timer2);	/* Wait for card goes ready or timeout */

	return (d == 0xFF) ? 1 : 0;
}


/*-----------------------------------------------------------------------*/
/* Get disk status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only drive 0 */

	return Stat;	/* Return disk status */
}



/*-----------------------------------------------------------------------*/
/* Read sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive number (0) */
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	BYTE count		/* Number of sectors to read (1..128) */
)
{
	if (drv || !count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* LBA ot BA conversion (byte addressing cards) */

	if (count == 1) {	/* Single sector read */
		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& rcvr_datablock(buff, 512))
			count = 0;
	}
	else {				/* Multiple sector read */
		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	SD.deselect();

	return count ? RES_ERROR : RES_OK;	/* Return result */
}



/*-----------------------------------------------------------------------*/
/* Write sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Ponter to the data to write */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Number of sectors to write (1..128) */
)
{
	if (drv || !count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check drive status */
	if (Stat & STA_PROTECT) return RES_WRPRT;	/* Check write protect */

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* LBA ==> BA conversion (byte addressing cards) */

	if (count == 1) {	/* Single sector write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple sector write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);	/* Predefine number of sectors */
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	SD.deselect();

	return count ? RES_ERROR : RES_OK;	/* Return result */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to the MMC                                         */
/*-----------------------------------------------------------------------*/

static
int xmit_datablock (	/* 1:OK, 0:Failed */
	const BYTE *buff,	/* Ponter to 512 byte data to be sent */
	BYTE token			/* Token */
)
{
	BYTE resp;


	if (!wait_ready(500)) return 0;		/* Wait for card ready */

	SD.transfer(token);					/* Send token */
	if (token != 0xFD) {				/* Send data if token is other than StopTran */
		xmit_spi_multi(buff, 512);		/* Data */
		SD.transfer(0xFF); SD.transfer(0xFF);	/* Dummy CRC */

		resp = SD.transfer(0xFF);				/* Receive data resp */
		if ((resp & 0x1F) != 0x05)		/* Function fails if the data packet was not accepted */
			return 0;
	}
	return 1;
}


/*-----------------------------------------------------------------------*/
/* Miscellaneous drive controls other than data read/write               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE cmd,		/* Control command code */
	void *buff		/* Pointer to the conrtol data */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = (BYTE*) buff;
	DWORD *dp, st, ed, csize;


	if (drv) return RES_PARERR;					/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

	res = RES_ERROR;

	switch (cmd) {
	case CTRL_SYNC :		/* Wait for end of internal write process of the drive */
		if (SD.select()) res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get drive capacity in unit of sector (DWORD) */
		if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
			if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
				csize = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
				*(DWORD*)buff = csize << 10;
			} else {					/* SDC ver 1.XX or MMC ver 3 */
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
				*(DWORD*)buff = csize << (n - 9);
			}
			res = RES_OK;
		}
		break;

	case GET_SECTOR_SIZE :	/* Get sector size in unit of byte (WORD) */
		*(WORD*)buff = 512;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		if (CardType & CT_SD2) {	/* SDC ver 2.00 */
			if (send_cmd(ACMD13, 0) == 0) {	/* Read SD status */
				SD.transfer(0xFF);
				if (rcvr_datablock(csd, 16)) {				/* Read partial block */
					for (n = 64 - 16; n; n--) SD.transfer(0xFF);	/* Purge trailing data */
					*(DWORD*)buff = 16UL << (csd[10] >> 4);
					res = RES_OK;
				}
			}
		} else {					/* SDC ver 1.XX or MMC */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
				if (CardType & CT_SD1) {	/* SDC ver 1.XX */
					*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else {					/* MMC */
					*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = RES_OK;
			}
		}
		break;

	case CTRL_ERASE_SECTOR :	/* Erase a block of sectors (used when _USE_ERASE == 1) */
		if (!(CardType & CT_SDC)) break;				/* Check if the card is SDC */
		if (disk_ioctl(drv, MMC_GET_CSD, csd)) break;	/* Get CSD */
		if (!(csd[0] >> 6) && !(csd[10] & 0x40)) break;	/* Check if sector erase can be applied to the card */
		dp = (DWORD*) buff; st = dp[0]; ed = dp[1];				/* Load sector block */
		if (!(CardType & CT_BLOCK)) {
			st *= 512; ed *= 512;
		}
		if (send_cmd(CMD32, st) == 0 && send_cmd(CMD33, ed) == 0 && send_cmd(CMD38, 0) == 0 && wait_ready(30000))	/* Erase sector block */
			res = RES_OK;	/* FatFs does not check result of this command */
		break;

	/* Following commands are never used by FatFs module */

	case MMC_GET_TYPE :		/* Get MMC/SDC type (BYTE) */
		*ptr = CardType;
		res = RES_OK;
		break;

	case MMC_GET_CSD :		/* Read CSD (16 bytes) */
		if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
			&& rcvr_datablock(ptr, 16))
			res = RES_OK;
		break;

	case MMC_GET_CID :		/* Read CID (16 bytes) */
		if (send_cmd(CMD10, 0) == 0		/* READ_CID */
			&& rcvr_datablock(ptr, 16))
			res = RES_OK;
		break;

	case MMC_GET_OCR :		/* Read OCR (4 bytes) */
		if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
			for (n = 4; n; n--) *ptr++ = SD.transfer(0xFF);
			res = RES_OK;
		}
		break;

	case MMC_GET_SDSTAT :	/* Read SD status (64 bytes) */
		if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
			SD.transfer(0xFF);
			if (rcvr_datablock(ptr, 64))
				res = RES_OK;
		}
		break;

	default:
		res = RES_PARERR;
	}

	SD.deselect();

	return res;
}


DWORD get_fattime(void) {
  return SDFatFs::fattime();
}

