// F4D_MP3 MP3 player for the STM32F4 Discovery board
// hacked together in 03/2012 by watz@gmx.net
//
// SW used
// ChibiOS 2.4.0 with PLLI2S fix
// Helix fixpt decoder from CVS (all C, no asm optims)
// I2S + CS43L22 code ripped from ST FW Libs "Audio_playback_and_record" demo

// connect your SD card to the STM32F4 Discovery board as follows:
//
// GND to SD card pin 3 (GND)
// 3V to SD card pin 4 (VCC)
// PC4 (NSS) to SD card pin 1 (CS)
// PA5 (SCK) to SD card pin 5 (SCLK)
// PA6 (MISO) to SD card pin 7 (DO)
// PA7 (MOSI) to SD card pin 2 (DI)
//
// more info: http://elm-chan.org/docs/mmc/mmc_e.html
// (you may also add the two recommended pull up resistors and blocking cap described there)

// you can optionally connect an RS232 Transceiver + Terminal Emu:
//
// PA2 (TX)
// PA3 (RX)

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include <ff.h>
#include <string.h>
#include "mp3dec.h"
#include "codec.h"

FATFS MMC_FS;
MMCDriver MMCD1;
static bool_t fs_ready = FALSE;
static SPIConfig hs_spicfg = { NULL, GPIOC, 4, 0 };
static SPIConfig ls_spicfg = { NULL, GPIOC, 4, SPI_CR1_BR_2 | SPI_CR1_BR_1 };
#define MP3_INBUF_SIZE 4096
static uint8_t g_Mp3InBuffer[MP3_INBUF_SIZE];
static uint16_t g_pMp3OutBuffer[MAX_NCHAN * MAX_NGRAN * MAX_NSAMP];
static uint16_t* g_pMp3OutBufferPtr = NULL;
#define MP3_DMA_BUFFER_SIZE (4 * MAX_NCHAN * MAX_NGRAN * MAX_NSAMP)
static uint16_t g_pMp3DmaBuffer[MP3_DMA_BUFFER_SIZE];
static uint16_t* g_pMp3DmaBufferPtr = NULL;
static Thread* g_pMp3DecoderThread = NULL;

static uint32_t Mp3ReadId3V2Text(FIL* pInFile, uint32_t unDataLen, char* pszBuffer, uint32_t unBufferSize)
{
  UINT unRead = 0;
  BYTE byEncoding = 0;
  if((f_read(pInFile, &byEncoding, 1, &unRead) == FR_OK) && (unRead == 1))
  {
    unDataLen--;
    if(unDataLen <= (unBufferSize - 1))
    {
      if((f_read(pInFile, pszBuffer, unDataLen, &unRead) == FR_OK) ||
          (unRead == unDataLen))
      {
        if(byEncoding == 0)
        {
          // ISO-8859-1 multibyte
          // just add a terminating zero
          pszBuffer[unDataLen] = 0;
        }
        else if(byEncoding == 1)
        {
          // UTF16LE unicode
          uint32_t r = 0;
          uint32_t w = 0;
          if((unDataLen > 2) && (pszBuffer[0] == 0xFF) && (pszBuffer[1] == 0xFE))
          {
            // ignore BOM, assume LE
            r = 2;
          }
          for(; r < unDataLen; r += 2, w += 1)
          {
            // should be acceptable for 7 bit ascii
            pszBuffer[w] = pszBuffer[r];
          }
          pszBuffer[w] = 0;
        }
      }
      else
      {
        return 1;
      }
    }
    else
    {
      // we won't read a partial text
      if(f_lseek(pInFile, f_tell(pInFile) + unDataLen) != FR_OK)
      {
        return 1;
      }
    }
  }
  else
  {
    return 1;
  }
  return 0;
}

static uint32_t Mp3ReadId3V2Tag(FIL* pInFile, char* pszArtist, uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize)
{
  pszArtist[0] = 0;
  pszTitle[0] = 0;

  BYTE id3hd[10];
  UINT unRead = 0;
  if((f_read(pInFile, id3hd, 10, &unRead) != FR_OK) || (unRead != 10))
  {
    return 1;
  }
  else
  {
    uint32_t unSkip = 0;
    if((unRead == 10) &&
        (id3hd[0] == 'I') &&
        (id3hd[1] == 'D') &&
        (id3hd[2] == '3'))
    {
      unSkip += 10;
      unSkip = ((id3hd[6] & 0x7f) << 21) | ((id3hd[7] & 0x7f) << 14) | ((id3hd[8] & 0x7f) << 7) | (id3hd[9] & 0x7f);

      // try to get some information from the tag
      // skip the extended header, if present
      uint8_t unVersion = id3hd[3];
      if(id3hd[5] & 0x40)
      {
        BYTE exhd[4];
        f_read(pInFile, exhd, 4, &unRead);
        size_t unExHdrSkip = ((exhd[0] & 0x7f) << 21) | ((exhd[1] & 0x7f) << 14) | ((exhd[2] & 0x7f) << 7) | (exhd[3] & 0x7f);
        unExHdrSkip -= 4;
        if(f_lseek(pInFile, f_tell(pInFile) + unExHdrSkip) != FR_OK)
        {
          return 1;
        }
      }
      uint32_t nFramesToRead = 2;
      while(nFramesToRead > 0)
      {
        char frhd[10];
        if((f_read(pInFile, frhd, 10, &unRead) != FR_OK) || (unRead != 10))
        {
          return 1;
        }
        if((frhd[0] == 0) || (strncmp(frhd, "3DI", 3) == 0))
        {
          break;
        }
        char szFrameId[5] = {0, 0, 0, 0, 0};
        memcpy(szFrameId, frhd, 4);
        uint32_t unFrameSize = 0;
        uint32_t i = 0;
        for(; i < 4; i++)
        {
          if(unVersion == 3)
          {
            // ID3v2.3
            unFrameSize <<= 8;
            unFrameSize += frhd[i + 4];
          }
          if(unVersion == 4)
          {
            // ID3v2.4
            unFrameSize <<= 7;
            unFrameSize += frhd[i + 4] & 0x7F;
          }
        }

        if(strcmp(szFrameId, "TPE1") == 0)
        {
          // artist
          if(Mp3ReadId3V2Text(pInFile, unFrameSize, pszArtist, unArtistSize) != 0)
          {
            break;
          }
          nFramesToRead--;
        }
        else if(strcmp(szFrameId, "TIT2") == 0)
        {
          // title
          if(Mp3ReadId3V2Text(pInFile, unFrameSize, pszTitle, unTitleSize) != 0)
          {
            break;
          }
          nFramesToRead--;
        }
        else
        {
          if(f_lseek(pInFile, f_tell(pInFile) + unFrameSize) != FR_OK)
          {
            return 1;
          }
        }
      }
      chprintf((BaseChannel*)&SD2, "Mp3Decode: Skipping %u bytes of ID3v2 tag\r\n", unSkip + 1);
    }
    if(f_lseek(pInFile, unSkip) != FR_OK)
    {
      return 1;
    }
  }

  return 0;
}

static UINT Mp3FillReadBuffer(BYTE* pInData, UINT unInDataLeft, FIL* pInFile)
{

  // move last, small chunk from end of buffer to start, then fill with new data
  memmove(g_Mp3InBuffer, pInData, unInDataLeft);

  UINT unSpaceLeft = MP3_INBUF_SIZE - unInDataLeft;
  UINT unRead = 0;
  palSetPad(GPIOD, 12); // green LED
  FRESULT fr = f_read(pInFile, g_Mp3InBuffer + unInDataLeft, unSpaceLeft, &unRead);
  palClearPad(GPIOD, 12); // green LED
  if(fr != FR_OK)
  {
    unRead = 0;
  }
  if(unRead < unSpaceLeft)
  {
    // zero-pad to avoid finding false sync word after last frame (from old data in readBuf)
    memset(g_Mp3InBuffer + unInDataLeft + unRead, unSpaceLeft - unRead, 0);
  }

  return unRead;
}

int Mp3Decode(const char* pszFile)
{
  int nResult = 0;
  BYTE* pInData = g_Mp3InBuffer;
  UINT unInDataLeft = 0;
  FIL fIn;
  UINT bEof = FALSE;
  UINT bOutOfData = FALSE;
  MP3FrameInfo mp3FrameInfo;
  uint32_t unDmaBufMode = 0;
  g_pMp3DmaBufferPtr = g_pMp3DmaBuffer;
  g_pMp3DecoderThread = chThdSelf();

  FRESULT errFS = f_open(&fIn, pszFile, FA_READ);
  if(errFS != FR_OK)
  {
    chprintf((BaseChannel*)&SD2, "Mp3Decode: Failed to open file \"%s\" for reading, err=%d\r\n", pszFile, errFS);
    return -1;
  }

  HMP3Decoder hMP3Decoder = MP3InitDecoder();
  if(hMP3Decoder == NULL)
  {
    chprintf((BaseChannel*)&SD2, "Mp3Decode: Failed to initialize mp3 decoder engine\r\n");
    return -2;
  }

  chprintf((BaseChannel*)&SD2, "Mp3Decode: Start decoding \"%s\"\r\n", pszFile);

  char szArtist[80];
  char szTitle[80];
  palSetPad(GPIOD, 12); // green LED
  Mp3ReadId3V2Tag(&fIn, szArtist, sizeof(szArtist), szTitle, sizeof(szTitle));
  palClearPad(GPIOD, 12); // green LED
  if(szArtist[0] != 0 || szTitle[0] != 0)
  {
    chprintf((BaseChannel*)&SD2, "Mp3Decode: Now playing (ID3v2): %s - %s\r\n", szArtist, szTitle);
  }
  int nDecodeRes = ERR_MP3_NONE;
  UINT unFramesDecoded = 0;
  do
  {
    if(unInDataLeft < (2 * MAINBUF_SIZE) && (!bEof))
    {
      UINT unRead = Mp3FillReadBuffer(pInData, unInDataLeft, &fIn);
      unInDataLeft += unRead;
      pInData = g_Mp3InBuffer;
      if(unRead == 0)
      {
        bEof = 1;
      }
    }

    // find start of next MP3 frame - assume EOF if no sync found
    int nOffset = MP3FindSyncWord(pInData, unInDataLeft);
    if(nOffset < 0)
    {
      bOutOfData = TRUE;
      break;
    }
    pInData += nOffset;
    unInDataLeft -= nOffset;

    // decode one MP3 frame - if offset < 0 then bytesLeft was less than a full frame
    nDecodeRes = MP3Decode(hMP3Decoder, &pInData, (int*)&unInDataLeft, (short*)g_pMp3OutBuffer, 0);
    switch(nDecodeRes)
    {
      case ERR_MP3_NONE:
      {
        MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
        if(unFramesDecoded == 0)
        {
          chprintf((BaseChannel*)&SD2, "Mp3Decode: %d Hz %d Bit %d Channels\r\n",
                   mp3FrameInfo.samprate, mp3FrameInfo.bitsPerSample, mp3FrameInfo.nChans);
          if((mp3FrameInfo.samprate > 48000) || (mp3FrameInfo.bitsPerSample != 16) || (mp3FrameInfo.nChans != 2))
          {
            chprintf((BaseChannel*)&SD2, "Mp3Decode: incompatible MP3 file.\r\n");
            nResult = -5;
            break;
          }
        }
        if((unFramesDecoded) % 100 == 0)
        {
          chprintf((BaseChannel*)&SD2, "Mp3Decode: frame %u, bitrate=%d\r\n", unFramesDecoded, mp3FrameInfo.bitrate);
        }
        unFramesDecoded++;
        g_pMp3OutBufferPtr = g_pMp3OutBuffer;

        uint32_t unOutBufferAvail= mp3FrameInfo.outputSamps;
        while(unOutBufferAvail > 0)
        {
          // fill up the whole dma buffer
          uint32_t unDmaBufferSpace = 0;
          if(unDmaBufMode == 0)
          {
            // fill the whole buffer
            // dma buf ptr was reset to beginning of the buffer
            unDmaBufferSpace = g_pMp3DmaBuffer + MP3_DMA_BUFFER_SIZE - g_pMp3DmaBufferPtr;
          }
          else if(unDmaBufMode == 1)
          {
            // fill the first half of the buffer
            // dma buf ptr was reset to beginning of the buffer
            unDmaBufferSpace = g_pMp3DmaBuffer + (MP3_DMA_BUFFER_SIZE / 2) - g_pMp3DmaBufferPtr;
          }
          else
          {
            // fill the last half of the buffer
            // dma buf ptr was reset to middle of the buffer
            unDmaBufferSpace = g_pMp3DmaBuffer + MP3_DMA_BUFFER_SIZE - g_pMp3DmaBufferPtr;
          }
          uint32_t unCopy = unDmaBufferSpace > unOutBufferAvail ? unOutBufferAvail : unDmaBufferSpace;
          if(unCopy > 0)
          {
            memcpy(g_pMp3DmaBufferPtr, g_pMp3OutBufferPtr, unCopy * sizeof(uint16_t));
            unOutBufferAvail -= unCopy;
            g_pMp3OutBufferPtr += unCopy;
            unDmaBufferSpace -= unCopy;
            g_pMp3DmaBufferPtr += unCopy;
          }
          if(unDmaBufferSpace == 0)
          {
            // dma buffer full
            // see if this was the first run
            if(unDmaBufMode == 0)
            {
              // on the first buffer fill up,
              // start the dma transfer
              if(EVAL_AUDIO_Init(OUTPUT_DEVICE_HEADPHONE, 80, (uint32_t)mp3FrameInfo.samprate))
              {
                chprintf((BaseChannel *) &SD2, "Mp3Decode: audio init failed\r\n");
                nResult = -4;
                break;
              }
              EVAL_AUDIO_Play(g_pMp3DmaBuffer, MP3_DMA_BUFFER_SIZE * sizeof(uint16_t));
            }
            // we must wait for the dma stream tx interrupt here
            eventmask_t em = chEvtWaitAny((eventmask_t)2 | 4 | 8);
            if(em & 8)
            {
              // stop requested
              chprintf((BaseChannel*)&SD2, "Mp3Decode: Stop requested\r\n");
              nResult = 1;
              break;
            }
            if((em & 2) && (em & 4))
            {
              chprintf((BaseChannel*)&SD2, "Mp3Decode: DMA out of sync (HT and TC both set)\r\n");
              nResult = -3;
              break;
            }
            if(unDmaBufMode == 0 || unDmaBufMode == 2)
            {
              // the dma event we expect is "half transfer" (=2)
              if(em & 2)
              {
                // set up first half mode
                unDmaBufMode = 1;
                g_pMp3DmaBufferPtr = g_pMp3DmaBuffer;
              }
              else
              {
                chprintf((BaseChannel*)&SD2, "Mp3Decode: DMA out of sync (expected HT, got TC)\r\n");
                nResult = -3;
                break;
              }
            }
            else
            {
              // the dma event we expect is "transfer complete" (=4)
              if(em & 4)
              {
                // set up last half mode
                unDmaBufMode = 2;
                g_pMp3DmaBufferPtr = g_pMp3DmaBuffer + (MP3_DMA_BUFFER_SIZE / 2);
              }
              else
              {
                chprintf((BaseChannel*)&SD2, "Mp3Decode: DMA out of sync (expected TC, got HT)\r\n");
                nResult = -3;
              }
            }
          }
        }
        break;
      }
      case ERR_MP3_MAINDATA_UNDERFLOW:
      {
        // do nothing - next call to decode will provide more mainData
        break;
      }
      case ERR_MP3_FREE_BITRATE_SYNC:
      {
        break;
      }
      case ERR_MP3_INDATA_UNDERFLOW:
      {
        chprintf((BaseChannel*)&SD2, "Mp3Decode: Decoding error ERR_MP3_INDATA_UNDERFLOW\r\n");
        bOutOfData = TRUE;
        break;
      }
      default:
      {
        chprintf((BaseChannel*)&SD2, "Mp3Decode: Decoding error %d\r\n", nDecodeRes);
        bOutOfData = TRUE;
        break;
      }
    }
  }
  while((!bOutOfData) && (nResult == 0));

  chprintf((BaseChannel*)&SD2, "Mp3Decode: Finished decoding\r\n");

  MP3FreeDecoder(hMP3Decoder);
  if(EVAL_AUDIO_Stop(CODEC_PDWN_HW))
  {
    chprintf((BaseChannel*)&SD2, "Mp3Decode: Failed to stop audio\r\n");
  }
  EVAL_AUDIO_DeInit();
  f_close(&fIn);
  // this is the only legit way I know
  // to remvove still pending event flags
  // from the thread
  chEvtWaitOneTimeout(2, 50);
  chEvtWaitOneTimeout(4, 50);

  return nResult;
}

static void I2SDmaTxInterrupt(SPIDriver* spip, uint32_t flags)
{
  if(spip == &SPID3)
  {
    if(flags & STM32_DMA_ISR_HTIF)
    {
      // let the decoder thread play the next samples
      chSysLockFromIsr();
      chEvtSignalFlagsI(g_pMp3DecoderThread, (eventmask_t)2);
      chSysUnlockFromIsr();
    }
    if(flags & STM32_DMA_ISR_TCIF)
    {
      // let the decoder thread play the next samples
      chSysLockFromIsr();
      chEvtSignalFlagsI(g_pMp3DecoderThread, (eventmask_t)4);
      chSysUnlockFromIsr();
    }
  }
}

/* Card insertion verification.*/
static bool_t mmc_is_inserted(void)
{
  // not wired yet, assume card is always in
  //return palReadPad(IOPORT3, GPIOC_MMCCP);
  return 1;
}

/* Card protection verification.*/
static bool_t mmc_is_protected(void)
{
  // not wired yet
  //return !palReadPad(IOPORT3, GPIOC_MMCWP);
  return 1;
}

static WORKING_AREA(waBlinkThread, 256);
static msg_t BlinkThread(void *arg)
{
  (void) arg;
  chRegSetThreadName("BlinkThread");
  while(TRUE)
  {
    if(palReadPad(GPIOA, GPIOA_BUTTON))
    {
      if(g_pMp3DecoderThread != NULL)
      {
        chEvtSignalFlags(g_pMp3DecoderThread, (eventmask_t)8);
      }
    }
    palSetPad(GPIOD, GPIOD_LED3);
    chThdSleepMilliseconds(80);
    palClearPad(GPIOD, GPIOD_LED3);
    chThdSleepMilliseconds(80);
  }

  return (msg_t)0;
}

FRESULT Mp3PlayAllFiles(char* path)
{
  FILINFO fno;
  DIR dir;
  char fpath[50];

  chprintf((BaseChannel*)&SD2, "Mp3PlayAllFiles: Playing all files in \"%s\"\r\n", path);
  FRESULT res = f_opendir(&dir, path);
  if(res == FR_OK)
  {
    res = f_readdir(&dir, &fno);
    while((res == FR_OK) && (fno.fname[0] != 0))
    {
      strcpy(fpath, path);
      strcat(fpath, fno.fname);
      if(fno.fattrib & AM_DIR)
      {
        chprintf((BaseChannel*)&SD2, "Mp3PlayAllFiles: DIR \"%s\"\r\n", fpath);
      }
      else
      {
        chprintf((BaseChannel*)&SD2, "Mp3PlayAllFiles: FILE \"%s\"\r\n", fpath);
        size_t unLen = strlen(fpath);
        if(unLen > 3)
        {
          if(stricmp(fpath + unLen - 4, ".mp3") == 0)
          {
            Mp3Decode(fpath);
          }
        }
      }
      res = f_readdir(&dir, &fno);
    }
  }
  else
  {
    chprintf((BaseChannel*)&SD2, "Mp3PlayAllFiles: f_opendir failed %d\r\n", res);
  }
  chprintf((BaseChannel*)&SD2, "Mp3PlayAllFiles: Finished playing all files\r\n", path);

  return res;
}

// MMC card insertion event
static void InsertHandler(eventid_t id)
{
  FRESULT err;
  (void) id;
  if(mmcConnect(&MMCD1))
  {
    chprintf((BaseChannel *) &SD2, "SD: Failed to connect to card\r\n");
    return;
  }
  else
  {
    chprintf((BaseChannel *) &SD2, "SD: Connected to card\r\n");
  }

  err = f_mount(0, &MMC_FS);
  if(err != FR_OK)
  {
    chprintf((BaseChannel *) &SD2, "SD: f_mount() failed %d\r\n", err);
    mmcDisconnect(&MMCD1);
    return;
  }
  else
  {
    chprintf((BaseChannel *) &SD2, "SD: File system mounted\r\n");
  }
  fs_ready = TRUE;

  // quick and dirty
  // directly decode all mp3's in the
  // cards root in a loop (assuming the card is never pulled)
  while(1)
  {
    Mp3PlayAllFiles("/");
  }
}

// MMC card removal event
static void RemoveHandler(eventid_t id)
{
  (void) id;
  fs_ready = FALSE;
}

// called on kernel panic
void port_halt(void)
{
  port_disable();
  palSetPad(GPIOD, 15); // turn on blue LED
  while(TRUE)
  {
  }
}

// called on I2C communication failures with the DAC
uint32_t Codec_TIMEOUT_UserCallback(void)
{
  port_halt();
  return 0;
}

int main(void)
{
  static const evhandler_t evhndl[] = {InsertHandler, RemoveHandler};
  struct EventListener el0, el1;

  // os init
  halInit();
  chSysInit();

  // setup LED pads
  palSetPadMode(GPIOD, 12, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
  palClearPad(GPIOD, 12); // green LED
  palSetPadMode(GPIOD, 15, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
  palClearPad(GPIOD, 15); // blue LED

  // setup pads to USART2 function (connect these pads through RS232 transceiver with PC, terminal emu needs 38400 baud)
  sdStart(&SD2, NULL);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7) | PAL_STM32_OSPEED_HIGHEST); // TX
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7)); // RX

  // setup pads to SPI1 function (connect these pads to your SD card accordingly)
  palSetPadMode(GPIOC, 4, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); // NSS
  palSetPadMode(GPIOA, 5, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST); // SCK
  palSetPadMode(GPIOA, 6, PAL_MODE_ALTERNATE(5)); // MISO
  palSetPadMode(GPIOA, 7, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST); // MOSI
  palSetPad(GPIOC, 4); // set NSS high

  // initialize MMC driver
  mmcObjectInit(&MMCD1, &SPID1, &ls_spicfg, &hs_spicfg, mmc_is_protected, mmc_is_inserted);
  mmcStart(&MMCD1, NULL);

  // ChibiOS has no I2S support yet;
  // codec.c initializes everything necessary
  // except the I2S TX DMA interrupt vector (because it would
  // conflict with the ChibiOS kernel)
  // we can make ChibiOS call our own handler by letting
  // it create the DMA stream for SPI3
  dmaStreamAllocate(SPID3.dmatx,
                    STM32_SPI_SPI3_IRQ_PRIORITY,
                    (stm32_dmaisr_t)I2SDmaTxInterrupt,
                    &SPID3);

  // blink thread; also checks the user button
  chThdCreateStatic(waBlinkThread, sizeof(waBlinkThread), NORMALPRIO, BlinkThread, NULL);

  chEvtRegister(&MMCD1.inserted_event, &el0, 0);
  chEvtRegister(&MMCD1.removed_event, &el1, 1);
  while(TRUE)
  {
    chEvtDispatch(evhndl, chEvtWaitOne(ALL_EVENTS));
  }
}
