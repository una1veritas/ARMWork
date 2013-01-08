/********************************************************************************/
/*!
	@file			wav_support.c
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2012.11.30
	@brief          ChaN's Riff-Wave Player + I2C AudioCodec Upper Layer.
					Based On ChaN's FM3 uC Examples Thanks!
					
    @section HISTORY
		2012.11.30	V1.00	Start Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "wav_support.h"
#include "ts.h"
#include "term_io.h"

/* Defines -------------------------------------------------------------------*/
/* WaveFile Streaming Buffer */
#define WAV_OUTBUF_SIZE			512
#define WAV_DMA_BUFFER_SIZE 	(2 * WAV_OUTBUF_SIZE)

/* Size of name buffer */
#define NBSIZE 					64				
/* Create FourCC */
#define FCC(c1,c2,c3,c4)		((c4<<24)+(c3<<16)+(c2<<8)+c1)	

/* Variables -----------------------------------------------------------------*/
uint16_t* g_pWAVDmaBuffer;
uint16_t* g_pWAVDmaBufferPtr = NULL;
volatile uint32_t bytesToPlay,TimeCount;

/* Constants -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**************************************************************************/
/*! 
	Simple WAV file player.
*/
/**************************************************************************/


int load_wav(
	FIL *fp,			/* Pointer to the open file object to play */
	const char *title,	/* Title (file name, etc...) */
	void *work,			/* Pointer to working buffer (must be-4 byte aligned) */
	uint32_t sz_work	/* Size of working buffer (must be power of 2) */
)
{
	UINT br, tc, t;
	uint16_t resolution;
	uint16_t channels;
	uint32_t samplerate;
	uint32_t chankbytes;
	uint32_t subchankbytes;
	uint32_t totalsize;
	
	DWORD offw, wsmp, eof;
	int nResult =0;
	BYTE *buff = work;
	char *p, nam[NBSIZE], art[NBSIZE];
	TimeCount =0;
	
	/* DMA Buffer Size Check */
	if(sz_work <= WAV_DMA_BUFFER_SIZE*2 ) return -1;
	
	/* Init Ring Buffer */
	unDmaBufMode    = INIT_RINGBUF;
	EV_I2SDMA       = IDLING;
	g_pWAVDmaBuffer = (uint16_t*)work;
	g_pWAVDmaBufferPtr = g_pWAVDmaBuffer;


	Display_clear_if();
	ts_locate(0, 0 ,0);
	xprintf("\33\x87\f");
	xprintf("Playing WAVE File\n");
	xprintf("->%s\n", title);

	/* Is this a WAV file? */
	if (f_read(fp, buff, 12, &br) || br != 12) return -1;
	if (LD_DWORD(&buff[0]) != FCC('R','I','F','F')) return -1;
	if (LD_DWORD(&buff[8]) != FCC('W','A','V','E')) return -1;
	eof = LD_DWORD(&buff[4]) + 8;

	/* Analyze the RIFF-WAVE header and get properties */
	nam[0] = art[0] = 0;
	channels = samplerate = wsmp = offw = totalsize = 0;

	while (f_tell(fp) < eof) {
		if (f_read(fp, buff, 8, &br) || br != 8) return -1;
		chankbytes = (LD_DWORD(&buff[4]) + 1) & ~1;
		switch (LD_DWORD(&buff[0])) {
			case FCC('f','m','t',' ') :	/* fmt chunk */
				if (chankbytes > 1000 || chankbytes < 16 || f_read(fp, buff, chankbytes, &br) || chankbytes != br) return -1;
				if (LD_WORD(&buff[0]) != FMT_LPCM) return -1;	/* Check if LPCM or not */
				
				channels   = LD_WORD(&buff[2]);		/* Check channels (1 or 2) */
				resolution = LD_WORD(&buff[14]);	/* Resolution (8 or 16) */
				samplerate = LD_DWORD(&buff[4]);	/* Sampling rate */
				wsmp = resolution * channels / 8;   /* DATA-Word Length */
				break;

			case FCC('f','a','c','t') :	/* fact chunk */
				f_lseek(fp, f_tell(fp) + chankbytes);
				break;

			case FCC('d','a','t','a') :	/* data chunk */
				offw = f_tell(fp);	/* Wave data start offset */
				totalsize = chankbytes;			/* Size of wave data [byte] */
				f_lseek(fp, f_tell(fp) + chankbytes);
				break;

			case FCC('L','I','S','T'):	/* LIST chunk */
				chankbytes += f_tell(fp);
				if (f_read(fp, buff, 4, &br) || br != 4) return -1;
				if (LD_DWORD(buff) == FCC('I','N','F','O')) {	/* LIST/INFO sub-chunk */
					while (f_tell(fp) < chankbytes) {
						if (f_read(fp, buff, 8, &br) || br != 8) return -1;
						subchankbytes = (LD_DWORD(&buff[4]) + 1) & ~1;
						p = 0;
						switch (LD_DWORD(buff)) {
						case FCC('I','N','A','M'):		/* INAM (name) field */
							p = nam; break;
						case FCC('I','A','R','T'):		/* IART (artist) field */
							p = art; break;
						}
						if (p && subchankbytes <= NBSIZE) {
							if (f_read(fp, p, subchankbytes, &br) || br != subchankbytes) return -1;
						} else {
							if (f_lseek(fp, f_tell(fp) + subchankbytes)) return -1;
						}
					}
				} else {
					if (f_lseek(fp, chankbytes)) return -1;	/* Skip unknown sub-chunk */
				}
				break;

		default :	/* Unknown chunk */
			return -1;
		}
	}
	
	/* Check if valid WAV file */
	if (!totalsize || !samplerate) return -1;
	/* Seek to top of wav data */
	if (f_lseek(fp, offw)) return -1;
	/* Time Length (sec) */
	tc = totalsize / samplerate / wsmp;
	
	/* Display Informations */
	ts_locate(3, 0 ,0);
	xprintf("IART=%s\nINAM=%s\n", art, nam);
	xprintf("Sample=%u.%ukHz/%ubit/%s\nLength=%u:%02u\n",
			 samplerate / 1000,
			(samplerate / 100) % 10,
			resolution, 
			(channels == 2) ? "ST" : "MONO",
			 tc / 60, tc % 60);
	
	ts_locate(7, 0 ,0);
	xprintf("Sample Length\n");
	xprintf(" ->%ld bytes", totalsize);

	/* Set I2S Clock from Samplingrate */
	Codec_SetI2SCLK(samplerate);
	
	/* Initialize wave player (Codec, DMA, I2C) */
  	if(EVAL_AUDIO_Init(OUTPUT_DEVICE_HEADPHONE,65, (uint32_t)samplerate))
	{
		printf( "Audio Device Init Failed...\r\n");
		nResult = -4;
		return -1;
	}

	/* Init AudioRingBuffer DATA */
	bytesToPlay  = totalsize;
	unDmaBufMode = INIT_RINGBUF;
	
	/* CirculerDMA's First Kick */
	f_read(fp, g_pWAVDmaBuffer, WAV_DMA_BUFFER_SIZE, &br);
	bytesToPlay-=br;
	EVAL_AUDIO_Play(g_pWAVDmaBuffer, WAV_DMA_BUFFER_SIZE*sizeof(uint16_t));
	TimeCount += br;

	/* CirculerDMA Loop */
	while(bytesToPlay)
	{

		/* Wait DMA Transfer or Key Input */
		uint8_t em = AUDIO_I2S_DMA_EventWait();

		if(em & KEY_STOP)
		{
			/* stop requested */
			printf( "Stop requested\r\n");
			nResult = 1;
			break;
		}
		if((em & TRANSFER_FIRST_HALF) && (em & TRANSFER_LAST_HALF))
		{
			printf( "DMA out of sync (HT and TC both set)\r\n");
			nResult = -3;
			break;
		}
		if(unDmaBufMode == INIT_RINGBUF || unDmaBufMode == FULL_RINGBUF)
		{
			/* Check FirstHalf Transfer in LastHalf and BufferInit Mode */
			if(em & TRANSFER_FIRST_HALF)
			{
				printf( "DMA out of sync (expected TC, got HT)\r\n");
				nResult = -3;
				break;
			}
			else{
				g_pWAVDmaBufferPtr = g_pWAVDmaBuffer + (WAV_DMA_BUFFER_SIZE / 2);
				f_read(fp, g_pWAVDmaBufferPtr, WAV_DMA_BUFFER_SIZE, &br);
				bytesToPlay -= br;
			}
		}
		else /* unDmaBufMode == HALF_RINGBUF */
		{
			/* Check LastHalf Transfer in FirstHalf Mode */
			if(em & TRANSFER_LAST_HALF)
			{
				printf( "DMA out of sync (expected HT, got TC)\r\n");
				nResult = -3;
				break;
			}
			else{
				g_pWAVDmaBufferPtr = g_pWAVDmaBuffer;
				f_read(fp, g_pWAVDmaBufferPtr, WAV_DMA_BUFFER_SIZE, &br);
				bytesToPlay -= br;
			}
		}

		/* Refresh time display every 1 sec */
		TimeCount += br;
		t = TimeCount / samplerate / wsmp;
		if (t != tc) {
			tc = t;
			ts_locate(10, 0 ,0);
			xprintf("\rTime=%u:%02u", tc / 60, tc % 60);
		}

	}

	/* Stop Streaming and Disable Audio Device */
	if(EVAL_AUDIO_Stop(CODEC_PDWN_HW))
	{
		printf("Failed to stop audio\r\n");
	}
	EVAL_AUDIO_DeInit();

	return nResult;

}

/* End Of File ---------------------------------------------------------------*/
