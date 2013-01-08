/********************************************************************************/
/*!
	@file			mp3_support.c
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2012.11.30
	@brief          Helix-MP3 Decorder + I2C AudioCodec Upper Layer.
					Based On UnderURL Thanks!
					http://www.mikrocontroller.net/topic/252319

    @section HISTORY
		2012.11.30	V1.00	Start Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "mp3_support.h"
#include "ts.h"
#include "xprintf.h"

/* Defines -------------------------------------------------------------------*/
#define MP3_INBUF_SIZE 		(4096)
#define MP3_OUTBUF_SIZE		(MAX_NCHAN * MAX_NGRAN * MAX_NSAMP)
#define MP3_DMA_BUFFER_SIZE (4 * MP3_OUTBUF_SIZE)
#define MIN(a,b)	 		(a < b ? b : a)
#define MP3ID1_OFS			128

#ifdef  MP3_SUPPORT_MESSAGES
 #define MSG_PRINTF(...)	printf(...)
#else
 #define MSG_PRINTF(...)	
#endif

/* Variables -----------------------------------------------------------------*/
uint8_t* g_Mp3InBuffer;
uint16_t* g_pMp3OutBufferPtr = NULL;
uint16_t* g_pMp3DmaBuffer;
uint16_t* g_pMp3DmaBufferPtr = NULL;
/* Array Buff[] must be declared FatFs Basic Routine! */
extern uint8_t Buff[];

/* Constants -----------------------------------------------------------------*/
/* MPEG1Layer-3 Frame Duration Table */
const uint8_t FrmDur[8] ={
      12,	/* 96000kHz Frame Duration */
      24,   /* 48000kHz Frame Duration */
      26,   /* 44100kHz Frame Duration */
      36,   /* 32000kHz Frame Duration */
      52,   /* 22050kHz Frame Duration */
      72,   /* 16000kHz Frame Duration */
      104,  /* 11025kHz Frame Duration */
      144,  /*  8000kHz Frame Duration */
  };

/* Function prototypes -------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
/**************************************************************************/
/*! 
	For Xing Header Detection.
*/
/**************************************************************************/
static char* extstr(char *ptr1,int len1,const char *ptr2,int len2)
{
    int len,start,end;
    char *ret=NULL;

	/* Must be shorter than Target Strings */
    if(len2 <= len1)
    {
		/* Store End Position */
		end = len1-len2;
		
		/* String Detection Routine */
        for(start=0,len=0; (start<=end)&&(len!=len2); start++)  
        {
			/* Store Strings Header */
            ret = &ptr1[start];
			/* Detect Valid Strings*/
            for (len = 0; (len<len2)&&(ret[len]==ptr2[len]); len++);
        }
		/* Detect Overload */
        if (len!=len2) ret = NULL;
    }
	
    return ret;
}

/**************************************************************************/
/*! 
	Get Xing Tag If present.
	Returns TotalFrames.
	Return "0"indicates CBR or Invalid VBR Header.
	http://multimedia.cx/mp3extensions.txt thanks!
*/
/**************************************************************************/
static uint32_t MP3_XingHeaderCheck(FIL* pInFile, void *work)
{	
	uint32_t TotalFrameCount=0;
	uint32_t XingFlag;
	UINT  br;
	UINT  tfp;
	char  xing[] = "Xing";
	char* chr;
	char  unk[256];

	tfp = f_tell(pInFile);

	if((f_read(pInFile, unk, 256, &br) != FR_OK)) return 0;

	/* Detect Xing Header */
	chr = extstr(unk,256,xing,sizeof(xing));

	/* Restore file Pointer */
	if(f_lseek(pInFile,tfp) != FR_OK) return 0;

	/* Get Total Bitstream in Bytes */
	if(chr == NULL)	return 0;
	else{
		/* Get Xing Flags */
		chr +=4; /* Pointer to Flags */
		XingFlag  = (uint32_t)(0xFF & chr[3]);
		XingFlag |= (uint32_t)(0xFF & chr[2])<<8;
		XingFlag |= (uint32_t)(0xFF & chr[1])<<16;
		XingFlag |= (uint32_t)(0xFF & chr[0])<<24;
		
		if(XingFlag & 0x0001){ /* Frames is present */
			chr +=4;	/* 4("Xing"Header ID) + 4(Flags) */
			/* Size of Frame is stored in BIG Endian */
			TotalFrameCount  = (uint32_t)(0xFF & chr[3]);
			TotalFrameCount |= (uint32_t)(0xFF & chr[2])<<8;
			TotalFrameCount |= (uint32_t)(0xFF & chr[1])<<16;
			TotalFrameCount |= (uint32_t)(0xFF & chr[0])<<24;
		}
		else return 0;
	}
	return TotalFrameCount;
}

/**************************************************************************/
/*! 
	Get FrameDurations in VBR(but NOT accuracy).
	in mSec.
*/
/**************************************************************************/
uint8_t MP3_FrameDurationVBR(uint32_t samplerate)
{
	/* Nemuisan said......
	   MPEG1 Layer-3 has 1152 samples per frame.
       In 44.1kHz,FrameLength worth about 26mSec.
       Dulation(mSec) = ((Fs)^(-1))(sec)*1152(sample)*1000(msec) */

	return ((uint8_t) ((uint32_t)(1152*1000UL)/samplerate));
}


/**************************************************************************/
/*! 
	Get MP3 IDTag.
*/
/**************************************************************************/
static uint32_t Mp3ReadId3V1(FIL* pInFile, char* pszArtist, uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize, void *work)
{
	UINT br;
	UINT tfp;

	char* wrk = (char*)work;
	tfp = f_tell(pInFile);

	if(f_lseek(pInFile,pInFile->fsize - 128 ) != FR_OK) return 1;
	
	if(f_read(pInFile, wrk, 128 , &br) == FR_OK)
	{
		if (strncmp("TAG", wrk, 3) == 0)	/* ID3V1 */
		{
			strncpy(pszTitle, wrk + 3, MIN(30, sizeof(pszTitle) - 1)); 
			strncpy(pszArtist,wrk + 3 + 30, MIN(30, sizeof(pszArtist) - 1));
		}
	}
	else{
		return 1;
	}

	if(f_lseek(pInFile,tfp) != FR_OK) return 1;

	return 0;
}

/**************************************************************************/
/*! 
	Get MP3 IDTag.
*/
/**************************************************************************/
static uint32_t Mp3ReadId3V2Text(FIL* pInFile, uint32_t unDataLen, char* pszBuffer, uint32_t unBufferSize)
{
	UINT unRead = 0;
	BYTE byEncoding = 0;

	if((f_read(pInFile, &byEncoding, 1, &unRead) == FR_OK) && (unRead == 1))
	{
		unDataLen--;

		if(unDataLen <= (unBufferSize - 1))
		{
			if((f_read(pInFile, pszBuffer, unDataLen, &unRead) == FR_OK) ||(unRead == unDataLen))
			{
				if(byEncoding == 0){
					/* ISO-8859-1 multibyte
					 just add a terminating zero */
					pszBuffer[unDataLen] = 0;
				}
				else if(byEncoding == 1){
					/* UTF16LE unicode */
					uint32_t r = 0;
					uint32_t w = 0;
					if((unDataLen > 2) && (pszBuffer[0] == 0xFF) && (pszBuffer[1] == 0xFE))
					{
						/* ignore BOM, assume LE */
						r = 2;
						}
					
					for(; r < unDataLen; r += 2, w += 1)
					{
						/* should be acceptable for 7 bit ascii */
						pszBuffer[w] = pszBuffer[r];
					}
					pszBuffer[w] = 0;
				}
			}
			else{
				return 1;
			}
		}
		else
		{
			/* we won't read a partial text */
			if(f_lseek(pInFile, f_tell(pInFile) + unDataLen) != FR_OK) return 1;
		}
	}
	else{
		return 1;
	}

	return 0;
}

/**************************************************************************/
/*! 
	Get MP3 IDTag.
*/
/**************************************************************************/
static uint32_t Mp3ReadId3V2Tag(FIL* pInFile, char* pszArtist, uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize)
{
	pszArtist[0] = 0;
	pszTitle[0] = 0;

	BYTE id3hd[10];
	UINT unRead = 0;
	BYTE exhd[4];
	uint8_t unVersion = id3hd[3];
	uint32_t nFramesToRead = 2;
	char frhd[10];
	char szFrameId[5] = {0, 0, 0, 0, 0};
	uint32_t unFrameSize = 0;
	
	if((f_read(pInFile, id3hd, 10, &unRead) != FR_OK) || (unRead != 10))
	{
		return 1;
	}
	else
	{
		uint32_t unSkip = 0;
		if(	(unRead == 10) 	  &&
			(id3hd[0] == 'I') &&
			(id3hd[1] == 'D') &&
			(id3hd[2] == '3'))
		{
			unSkip += 10;
			unSkip = ((id3hd[6] & 0x7f) << 21) | ((id3hd[7] & 0x7f) << 14) | ((id3hd[8] & 0x7f) << 7) | (id3hd[9] & 0x7f);

			/* try to get some information from the tag
			skip the extended header, if present */
			if(id3hd[5] & 0x40)
			{
				f_read(pInFile, exhd, 4, &unRead);
				size_t unExHdrSkip = ((exhd[0] & 0x7f) << 21) | ((exhd[1] & 0x7f) << 14) | ((exhd[2] & 0x7f) << 7) | (exhd[3] & 0x7f);
				unExHdrSkip -= 4;
				if(f_lseek(pInFile, f_tell(pInFile) + unExHdrSkip) != FR_OK) return 1;
			}

			while(nFramesToRead > 0)
			{

				if((f_read(pInFile, frhd, 10, &unRead) != FR_OK) || (unRead != 10)) return 1;
				if((frhd[0] == 0) || (strncmp(frhd, "3DI", 3) == 0)) break;

				memcpy(szFrameId, frhd, 4);
				
				for(uint32_t i=0; i < 4; i++)
				{
					if(unVersion == 3)
					{
						/* ID3v2.3 */
						unFrameSize <<= 8;
						unFrameSize += frhd[i + 4];
					}
					if(unVersion == 4)
					{
						/* ID3v2.4 */
						unFrameSize <<= 7;
						unFrameSize += frhd[i + 4] & 0x7F;
					}
				}

				if(strcmp(szFrameId, "TPE1") == 0)
				{
					/* artist */
					if(Mp3ReadId3V2Text(pInFile, unFrameSize, pszArtist, unArtistSize) != 0) break;
					nFramesToRead--;
				}
				else if(strcmp(szFrameId, "TIT2") == 0)
				{
					/* title */
					if(Mp3ReadId3V2Text(pInFile, unFrameSize, pszTitle, unTitleSize) != 0) break;
					nFramesToRead--;
				}
				else
				{
					if(f_lseek(pInFile, f_tell(pInFile) + unFrameSize) != FR_OK) return 1;
				}
			}
			MSG_PRINTF("Mp3Decode: Skipping %lu bytes of ID3v2 tag\r\n", unSkip + 1);
		}

		if(f_lseek(pInFile, unSkip) != FR_OK) return 1;
	}

	return 0;
}

/**************************************************************************/
/*! 
	Read MP3 Data From MMC.
*/
/**************************************************************************/
static UINT Mp3FillReadBuffer(BYTE* pInData, UINT unInDataLeft, FIL* pInFile)
{
	UINT unRead = 0;
	UINT unSpaceLeft = MP3_INBUF_SIZE - unInDataLeft;

	/* move last, small chunk from end of buffer to start, then fill with new data */
	memmove(g_Mp3InBuffer, pInData, unInDataLeft);

	FRESULT fr = f_read(pInFile, g_Mp3InBuffer + unInDataLeft, unSpaceLeft, &unRead);

	if(fr != FR_OK) unRead = 0;

	if(unRead < unSpaceLeft)
	{
		/* zero-pad to avoid finding false sync word after last frame (from old data in readBuf) */
		memset(g_Mp3InBuffer + unInDataLeft + unRead, unSpaceLeft - unRead, 0);
	}

	return unRead;
}


/**************************************************************************/
/*! 
	Load and Play MP3.
*/
/**************************************************************************/
int load_mp3(FIL *fil, const char *filename, void *work, unsigned int sz_work)
{
	/* Function Information */
	int nResult = 0;

	/* MP3 DATA Informations */
	MP3FrameInfo mp3FrameInfo;
	char szArtist[80];
	char szTitle[80];
   	int nDecodeRes = ERR_MP3_NONE;
	unsigned int unFramesDecoded = 0;
	uint32_t MP3ID2_OFS,MP3DAT_OFS =0;
	
  	/* I2S Transfer Relations */
	uint16_t g_pMp3OutBuffer[MP3_OUTBUF_SIZE];
	BYTE* pInData;
	UINT unInDataLeft = 0;
	UINT bEof = false;
	UINT bOutOfData = false;
	
	/* Display Time Relations */
	uint32_t TimeCount =0;
	uint32_t TimeCountVBR =0;
	uint32_t TimeDiff =0;
	uint32_t tc = 0;



	/* DMA Buffer Size Check */
	if(sz_work <= MP3_DMA_BUFFER_SIZE*2 ) return -2;

	/* Anyway Clear the screen */
	Display_clear_if();
	ts_locate(0, 0 ,0);
	xprintf("\33\x87\f");
	xprintf("Playing MP3 File\n");
	xprintf("->%s\n", filename);

	/* MP3Decorder & Buffer Check */
	HMP3Decoder hMP3Decoder = MP3InitDecoder();
	if(hMP3Decoder == NULL)
	{
		MSG_PRINTF("Mp3Decode: Failed to initialize mp3 decoder engine\r\n");
		return -2;
	}
	MSG_PRINTF("Mp3Decode: Start decoding \"%s\"\r\n", filename);
  
	g_Mp3InBuffer = malloc(MP3_INBUF_SIZE);
  	if(g_Mp3InBuffer == NULL)
	{	
		MSG_PRINTF("Not Enough MP3Inbuffer data!\r\n");
		return -2;
	}

	/* Init Ring Buffer */
	pInData 		= g_Mp3InBuffer;
	unDmaBufMode 	= INIT_RINGBUF;
	EV_I2SDMA 		= IDLING;
	g_pMp3DmaBuffer = (uint16_t*)work;
	g_pMp3DmaBufferPtr = g_pMp3DmaBuffer;

	/* Store Current FilePoiner Offset */
	MP3DAT_OFS = f_tell(fil);
	
 	/* Get MP3-ID Tag */
	Mp3ReadId3V2Tag(fil, szArtist, sizeof(szArtist), szTitle, sizeof(szTitle));
	if(szArtist[0] != 0 || szTitle[0] != 0)
	{
		MSG_PRINTF("Mp3Decode: Now playing (ID3v2): %s - %s\r\n", szArtist, szTitle);
	}
	else{
		Mp3ReadId3V1(fil, szArtist, sizeof(szArtist), szTitle, sizeof(szTitle),work);
		if(szArtist[0] != 0 || szTitle[0] != 0)
		{
			MSG_PRINTF("Mp3Decode: Now playing (ID3v1): %s - %s\r\n", szArtist, szTitle);
		}
	}
	
	/* Store the MP3 DataStream Offset */
	MP3ID2_OFS = (uint32_t)(f_tell(fil) - MP3DAT_OFS);
	/* Xing Header(including VBR Frame) Check */
	uint32_t tfc = MP3_XingHeaderCheck(fil,work);

	/* Decode and Transfer I2S */
	do
	{
		if(unInDataLeft < (2 * MAINBUF_SIZE) && (!bEof))
		{
		  UINT unRead = Mp3FillReadBuffer(pInData, unInDataLeft, fil);
		  unInDataLeft += unRead;
		  pInData = g_Mp3InBuffer;
		  if(unRead == 0) bEof = 1;
		}

		/* find start of next MP3 frame - assume EOF if no sync found */
		int nOffset = MP3FindSyncWord(pInData, unInDataLeft);
		if(nOffset < 0)
		{
		  bOutOfData = true;
		  break;
		}
		pInData += nOffset;
		unInDataLeft -= nOffset;

		/* decode one MP3 frame - if offset < 0 then bytesLeft was less than a full frame */
		nDecodeRes = MP3Decode(hMP3Decoder, &pInData, (int*)&unInDataLeft, (short*)g_pMp3OutBuffer, 0);
		switch(nDecodeRes)
		{
			case ERR_MP3_NONE:
			{
				MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
		
				if(unFramesDecoded == 0) /* For the First Time */
				{
					MSG_PRINTF("Mp3Decode: %d Hz %d Bit %d Channels\r\n",
							mp3FrameInfo.samprate, mp3FrameInfo.bitsPerSample, mp3FrameInfo.nChans);
					if((mp3FrameInfo.samprate > 48000) || (mp3FrameInfo.bitsPerSample != 16) || (mp3FrameInfo.nChans != 2))
					{
						MSG_PRINTF("Mp3Decode: incompatible MP3 file.\r\n");
						nResult = -5;
						break;
					}
					
					/* Calculate Playing Time(VBR is may not accurate) */
					if(tfc!=0){
						/* VBR */
						tc = (tfc * MP3_FrameDurationVBR(mp3FrameInfo.samprate)) / 1000;
					}
					else{
						/* CBR  or Invalid VBR Header */
						uint32_t rawbytes = (uint32_t)fil->fsize - MP3ID1_OFS - MP3ID2_OFS;
						tc =  rawbytes * 8 / mp3FrameInfo.bitrate;
					}
					
					/* Get Current MP3 DataStream Pointer */
					MP3DAT_OFS = f_tell(fil);
					
					/* Display MP3 Informations */
					ts_locate(3, 0 ,0);
					xprintf("IART=%s\nINAM=%s\n", szArtist, szTitle);
					xprintf("Sample=%u.%ukHz/%ubit/%s\n",
							 mp3FrameInfo.samprate/ 1000,
							(mp3FrameInfo.samprate / 100) % 10,
							mp3FrameInfo.bitsPerSample, 
							(mp3FrameInfo.nChans == 2) ? "ST" : "MONO");

					xprintf("Length=%u:%02u %s\n",
							 tc / 60, tc % 60,
							 (tfc!=0) ? "VBR" : "CBR");

					/* Set I2S Clock from Samplingrate */
					Codec_SetI2SCLK(mp3FrameInfo.samprate);
				}

				if((unFramesDecoded) % 100 == 0)
				{
					MSG_PRINTF("Mp3Decode: frame %u, bitrate=%d\r\n", unFramesDecoded, mp3FrameInfo.bitrate);
				}

				unFramesDecoded++;
				g_pMp3OutBufferPtr = g_pMp3OutBuffer;

				uint32_t unOutBufferAvail= mp3FrameInfo.outputSamps;
				while(unOutBufferAvail > 0)
				{
					/* fill up the whole dma buffer */
					uint32_t unDmaBufferSpace = 0;
					if(unDmaBufMode == INIT_RINGBUF)
					{
						/* fill the whole buffer
						   dma buf ptr was reset to beginning of the buffer */
						unDmaBufferSpace = g_pMp3DmaBuffer + MP3_DMA_BUFFER_SIZE - g_pMp3DmaBufferPtr;
					}
					else if(unDmaBufMode == HALF_RINGBUF)
					{
						/* fill the first half of the buffer
						   dma buf ptr was reset to beginning of the buffer */
						unDmaBufferSpace = g_pMp3DmaBuffer + (MP3_DMA_BUFFER_SIZE / 2) - g_pMp3DmaBufferPtr;
					}
					else  /* unDmaBufMode == FULL_RINGBUF */
					{
						/* fill the last half of the buffer
						   dma buf ptr was reset to middle of the buffer */
						unDmaBufferSpace = g_pMp3DmaBuffer + MP3_DMA_BUFFER_SIZE - g_pMp3DmaBufferPtr;
					}

					uint32_t unCopy = unDmaBufferSpace > unOutBufferAvail ? unOutBufferAvail : unDmaBufferSpace;
					if(unCopy > 0)
					{
						memcpy(g_pMp3DmaBufferPtr, g_pMp3OutBufferPtr, unCopy * sizeof(uint16_t));
						unOutBufferAvail   -= unCopy;
						g_pMp3OutBufferPtr += unCopy;
						unDmaBufferSpace   -= unCopy;
						g_pMp3DmaBufferPtr += unCopy;
					}
					if(unDmaBufferSpace == 0)
					{
						/* dma buffer full, see if this was the first run */
						if(unDmaBufMode == INIT_RINGBUF)
						{
							/* on the first buffer fill up,start the dma transfer */
							if(EVAL_AUDIO_Init(OUTPUT_DEVICE_HEADPHONE,65, (uint32_t)mp3FrameInfo.samprate))
							{
								MSG_PRINTF("Mp3Decode: audio init failed\r\n");
								nResult = -4;
								break;
							}
							/* I2S DMA Transfer First Trigger */
							EVAL_AUDIO_Play(g_pMp3DmaBuffer, MP3_DMA_BUFFER_SIZE * sizeof(uint16_t));
						}

						/* Wait DMA Transfer Interrupt or Key Input */
						uint8_t em = AUDIO_I2S_DMA_EventWait();
						if(em & KEY_STOP)
						{
							/* stop requested */
							MSG_PRINTF("Mp3Decode: Stop requested\r\n");
							nResult = 1;
							break;
						}
						if((em & TRANSFER_FIRST_HALF) && (em & TRANSFER_LAST_HALF))
						{
							MSG_PRINTF("Mp3Decode: DMA out of sync (HT and TC both set)\r\n");
							nResult = -3;
							break;
						}
						if(unDmaBufMode == INIT_RINGBUF || unDmaBufMode == FULL_RINGBUF)
						{
							/* Check FirstHalf Transfer in LastHalf and BufferInit Mode */
							if(em & TRANSFER_FIRST_HALF)
							{
								MSG_PRINTF("Mp3Decode: DMA out of sync (expected TC, got HT)\r\n");
								nResult = -3;
								break;
							}
							else{
								g_pMp3DmaBufferPtr = g_pMp3DmaBuffer + (MP3_DMA_BUFFER_SIZE / 2);
							}
						}
						else /* unDmaBufMode == HALF_RINGBUF */
						{
							/* Check LastHalf Transfer in FirstHalf Mode */
							if(em & TRANSFER_LAST_HALF)
							{
								MSG_PRINTF("Mp3Decode: DMA out of sync (expected HT, got TC)\r\n");
								nResult = -3;
								break;
							}
							else{
								g_pMp3DmaBufferPtr = g_pMp3DmaBuffer;
							}
						}
					}
				}
				
				/* Refresh time display every 1 sec */
				if(!tfc){ /* CBR and Invalid VBRHeader Time Display */
					TimeDiff = f_tell(fil) - MP3DAT_OFS;
					TimeCount = (TimeDiff * 8) / mp3FrameInfo.bitrate;
				}
				else{     /* VBR Time Display */
					TimeDiff = f_tell(fil) - MP3DAT_OFS;
					MP3DAT_OFS = f_tell(fil);
					TimeCountVBR += (TimeDiff * 8 * 1000) / mp3FrameInfo.bitrate;
					TimeCount = TimeCountVBR /1000;
				}
				if (TimeCount != tc) {
					tc = TimeCount;
					ts_locate(10, 0 ,0);
					xprintf("\rTime=%u:%02u", tc/ 60, tc % 60);
				}

				break;
			}
			case ERR_MP3_MAINDATA_UNDERFLOW:
			{
				/* do nothing - next call to decode will provide more mainData */
				break;
			}
			case ERR_MP3_FREE_BITRATE_SYNC:
			{
				MSG_PRINTF("Mp3Decode: Decoding error ERR_MP3_FREE_BITRATE_SYNC\r\n");
				break;
			}
			case ERR_MP3_INDATA_UNDERFLOW:
			{
				MSG_PRINTF("Mp3Decode: Decoding error ERR_MP3_INDATA_UNDERFLOW\r\n");
				bOutOfData = true;
				break;
			}
			default:
			{
				MSG_PRINTF("Mp3Decode: Decoding error %d\r\n", nDecodeRes);
				bOutOfData = true;
				break;
			}
		}

	}while((!bOutOfData) && (nResult == 0));

	MSG_PRINTF("Mp3Decode: Finished decoding\r\n");

	/* Free Decode Buffer */
	MP3FreeDecoder(hMP3Decoder);
	free(g_Mp3InBuffer);

	/* Stop Streaming and Disable Audio Device */
	if(EVAL_AUDIO_Stop(CODEC_PDWN_HW))
	{
		MSG_PRINTF("Mp3Decode: Failed to stop audio\r\n");
	}
	EVAL_AUDIO_DeInit();

	return nResult;
}


/* End Of File ---------------------------------------------------------------*/
