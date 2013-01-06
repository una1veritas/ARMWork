/**
  ******************************************************************************
  * @file    wavprocess.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file includes wav processing layer routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#include "wavprocess.h"

/** @addtogroup STM32_audio_codecs
  * @{
  */

/** @defgroup Wavprocess
  * @brief WavProcess driver modules
  * @{
  */

/* .WAV file format : ----------------------------------------------------------

 Endian      Offset      Length      Contents
  big         0           4 bytes     'RIFF'             -- 0x52494646
  little      4           4 bytes     <file length - 8>
  big         8           4 bytes     'WAVE'             -- 0x57415645

Next, the fmt chunk describes the sample format:

  big         12          4 bytes     'fmt '             -- 0x666D7420
  little      16          4 bytes     0x00000010         -- Length of the fmt data (16 bytes)
  little      20          2 bytes     0x0001             -- Format tag: 1 = PCM
  little      22          2 bytes     <channels>         -- Channels: 1 = mono, 2 = stereo
  little      24          4 bytes     <sample rate>      -- Samples per second: e.g., 22050
  little      28          4 bytes     <bytes/second>     -- sample rate * block align
  little      32          2 bytes     <block align>      -- channels * bits/sample / 8
  little      34          2 bytes     <bits/sample>      -- 8 or 16

Finally, the data chunk contains the sample data:

  big         36          4 bytes     'data'             -- 0x64617461
  little      40          4 bytes     <length of the data block>
  little      44          *           <sample data>
--------------------------------------------------------------------------------
*/

/** @defgroup WavProcess_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup WavProcess_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup WavProcess_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup WavProcess_Private_Variables
  * @{
  */

WAVE_FormatTypeDef WaveFormatStruct;
WAVE_FormatTypeDef EncWavFmtStruct;

uint32_t WavEncInited = 0;

/**
  * @}
  */

/** @defgroup WavProcess_Private_FunctionPrototypes
  * @{
  */
static uint32_t WavProcess_ParseHeader(uint8_t* pHeader, WAVE_FormatTypeDef* pWaveFormatStruct);
static uint32_t WavProcess_HeaderInit(uint8_t* pHeader, WAVE_FormatTypeDef* pWaveFormatStruct);

static uint32_t ReadUnit(uint8_t NbrOfBytes, uint32_t BytesFormat, uint8_t* pBuff, uint32_t BuffIdx);

fnReadCallback_TypeDef* pWavReadCallback;
fnSetPositionCallback_TypeDef* pWAVSetPosCallback;
/**
  * @}
  */

/** @defgroup WavProcess_Private_Functions
  * @{
  */

/**
  * @brief  Get the sampling rate of the current wav stream.
  * @param  None.
  * @retval Return the sampling rate.
  */
uint32_t WavProcess_GetSamplingRate(void)
{
  return (WaveFormatStruct.SampleRate);
}

/**
  * @brief  Decoder structure initialize.
  * @param  None.
  * @retval None.
  */
uint32_t WavProcess_DecoderInit(uint8_t* pHeader, 
                                fnReadCallback_TypeDef* pReadCallback, 
                                fnSetPositionCallback_TypeDef* pSetPosCallback)
{
  /* Parse the wav file header and extract required information */
  if (WavProcess_ParseHeader(pHeader, &WaveFormatStruct))
  {
    return 1;
  }
  
  /* Get the upper layer callbacks for reading data and setting file position offset */
  pWavReadCallback = pReadCallback;
  pWAVSetPosCallback = pSetPosCallback;
    
  /* Go the start of audio payload */
  pWAVSetPosCallback(WaveFormatStruct.AudioStartAddr);

  return 0;
}

/**
  * @brief  Provide output pcm stream.
  * @param  OutPutBuffer: output PCM.
  * @param  NbSamples: number of pcm to be decoded.
  * @param  none: NULL this parametter is kept in order to be on line with wma
  *         and mp3 decoder structures.
  * @retval Number of decoded data (in bytes).
  */
uint32_t WavProcess_DecodeData(__IO int16_t* OutPutBuffer, uint32_t NbSamples, void * none)
{
  uint32_t size = 0x00;

  if (pWavReadCallback != NULL)
  {
    size = pWavReadCallback((int8_t*)OutPutBuffer, (NbSamples * \
                                                    (WaveFormatStruct.BitsPerSample)/8 * \
                                                     WaveFormatStruct.NumChannels), \
                                                     NULL);
  }

  
  return size;
}

/**
  * @brief  Get the lenght of the current pcm stream in seconds.
  * @param  None.
  * @retval return the stream lenght.
  */
uint32_t WavProcess_GetStreamLenght(uint32_t fLength)
{
  return (uint32_t)(((fLength - WaveFormatStruct.AudioStartAddr) / 
                    ((WaveFormatStruct.BitsPerSample/8) * WaveFormatStruct.NumChannels)) /
                    WaveFormatStruct.SampleRate);
}

/**
  * @brief  Get the elapsed time of the current pcm stream.
  * @param  CurPos: current position of the audio file pointer.
  * @retval return the elapsed stream time.
  */
uint32_t WavProcess_GetElapsedTime(uint32_t CurPos)
{
  return (CurPos / (WaveFormatStruct.SampleRate * 
                    WaveFormatStruct.NumChannels * 
                    (WaveFormatStruct.BitsPerSample/8)));
}


/**
  * @brief  Forward the current pcm stream
  * @param  None.
  * @retval None.
  */
uint32_t WavProcess_StreamForward(void)
{
  //f_seek(current_file, ((uint32_t)Player_GetFileLength() / 40)*sizeof(short), F_SEEK_CUR );
  //pWAVSetPosCallback(34);

  return 0;
}

/**
  * @brief  Backward the current pcm stream
  * @param  None.
  * @retval None.
  */
uint32_t WavProcess_StreamBackward(void)
{
/*
  Not yet implemented
  
  int32_t seek_size = 0;
  seek_size = (((uint32_t)Player_GetFileLength() / 40) * sizeof(short));
  f_lseek(current_file, -1 * seek_size;
  
  pWAVSetPosCallback(34);
  
  */
  
  return 0;
}

/**
  * @brief  Rewind the file.
  * @param  None.
  * @retval None.
  */
uint32_t WavProcess_StopMode(void)
{
/*
  Not yet implemented
  
  f_lseek(current_file, 0;
  WAVSetPosCallback(0);
  
  */
  return 0;
}




/* WAV Encoder Functions *********************************************************/

/**
  * @brief  Encoder initialization.
  * @param  Pointer to the WAV file header to be written.
  * @retval None.
  */
uint32_t WavProcess_EncoderInit(uint32_t Freq, uint8_t* pHeader)
{  
  if (WavEncInited)
  {
    return 1;
  }
  
  /* Initialize the encoder structure */
  EncWavFmtStruct.SampleRate = Freq;        /* Audio sampling frequency */
  EncWavFmtStruct.NumChannels = 2;          /* Number of channels: 1:Mono or 2:Stereo */
  EncWavFmtStruct.BitsPerSample = 16;       /* Number of bits per sample (16, 24 or 32) */
  EncWavFmtStruct.AudioLength = 0;          /* Total length of useful audio data (payload) */
  EncWavFmtStruct.AudioStartAddr = 44;      /* Relative start address of audio payload */
  EncWavFmtStruct.RIFFchunksize = 44;       /* The file header chunk size */
  EncWavFmtStruct.FormatTag = 1;            /* Audio file format: PCM = 1 */
  EncWavFmtStruct.ByteRate = (EncWavFmtStruct.SampleRate * \
                           (EncWavFmtStruct.BitsPerSample/8) * \
                           EncWavFmtStruct.NumChannels);            /* Number of bytes per second  (sample rate * block align)  */
  EncWavFmtStruct.BlockAlign = EncWavFmtStruct.NumChannels * \
                            (EncWavFmtStruct.BitsPerSample/8);      /* channels * bits/sample / 8 */
  EncWavFmtStruct.DataSize = 0;             /* Total length of useful audio data (payload) */
    
  /* Parse the wav file header and extract required information */
  if (WavProcess_HeaderInit(pHeader, &EncWavFmtStruct))
  {
    return 1;
  }

  WavEncInited = 1;
  
  return 0;
}

/**
  * @brief  Encoder initialization.
  * @param  Pointer to the Wav file header to be written.
  * @retval None.
  */
uint32_t WavProcess_EncoderDeInit(void)
{
  if (WavEncInited)
  {
    WavEncInited = 0;
    
    return EncWavFmtStruct.AudioLength;
  }
  else
  {
    return 1;
  }
}

/**
  * @brief  Encoder initialization.
  * @param  Pointer to the Wav file header to be written.
  * @retval None.
  */
uint32_t WavProcess_EncodeData(int8_t *pBufIn, int8_t *pBufOut, uint32_t* nEnc, void* pOpt)
{
  if (WavEncInited)
  {    
    /* Retrun the same buffer without processing */
    *(uint32_t*)pBufOut = (uint32_t)pBufIn;
    
    /* Update the total audio data size */
    EncWavFmtStruct.AudioLength += *nEnc;
  }
  
  return 0;
}

/**
  * @brief  Get the sample rate.
  * @param  None.
  * @retval Current Sample Rate.
  */
uint32_t WavProcess_EncGetSampleRate (void)
{
  if (WavEncInited)
  {
    return EncWavFmtStruct.SampleRate;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  Get the sample rate.
  * @param  None.
  * @retval Current stream length.
  */
uint32_t WavProcess_EncGetStreamLength (void)
{
  if (WavEncInited)
  {
    return EncWavFmtStruct.AudioLength;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  Get the sample rate.
  * @param  None.
  * @retval Elapsed time in seconds.
  */
uint32_t WavProcess_EncGetElapsedTime (void)
{
  if (WavEncInited)
  {
    return ((EncWavFmtStruct.AudioLength) / 
                    (EncWavFmtStruct.SampleRate * 
                    EncWavFmtStruct.NumChannels * 
                    (EncWavFmtStruct.BitsPerSample/8))); 
  }
  else
  {
    return 0;
  }
}





/**
  * @brief  Parse the wave file header and checks if it is valid
  * @param  pHeader: pointer to the wave file header table.
  * @param  pWaveFormatStruct: Pointer to the wave structure to be filled.
  * @retval 0 if wave file is valid, !0 else.
  */
static uint32_t WavProcess_ParseHeader(uint8_t* pHeader, WAVE_FormatTypeDef* pWaveFormatStruct)
{
  uint32_t Temp = 0x00;
  uint32_t ExtraFormatBytes = 0;
  uint32_t HeadTabIdx = 0;
  
  /* Read chunkID, must be 'RIFF' ----------------------------------------------*/
  Temp = ReadUnit(4, BIG_ENDIAN, pHeader, HeadTabIdx);
  if (Temp != CHUNK_ID)
  {
    return(INVALID_RIFF_ID);
  }
  HeadTabIdx += 4;
  
  /* Read the file length ----------------------------------------------------*/
  pWaveFormatStruct->RIFFchunksize = ReadUnit(4, LITTLE_ENDIAN, pHeader, HeadTabIdx);
  HeadTabIdx += 4;
  
  /* Read the file format, must be 'WAVE' ------------------------------------*/
  Temp = ReadUnit(4, BIG_ENDIAN, pHeader, HeadTabIdx);
  if (Temp != FILE_FORMAT)
  {
    return(INVALID_WAVE_FORMAT);
  }
  HeadTabIdx += 4;
  
  /* Read the format chunk, must be 'fmt ' -----------------------------------*/
  Temp = ReadUnit(4, BIG_ENDIAN, pHeader, HeadTabIdx);
  if (Temp != FORMAT_ID)
  {
    return(INVALID_FORMAT_CHUNK_ID);
  }
  HeadTabIdx += 4;
  
  /* Read the length of the 'fmt' data, must be 0x10 -------------------------*/
  Temp = ReadUnit(4, LITTLE_ENDIAN, pHeader, HeadTabIdx);
  if (Temp != 0x10)
  {
    ExtraFormatBytes = 1;
  }
  HeadTabIdx += 4;
  
  /* Read the audio format, must be 0x01 (PCM) -------------------------------*/
  pWaveFormatStruct->FormatTag = ReadUnit(2, LITTLE_ENDIAN, pHeader, HeadTabIdx);
  if (pWaveFormatStruct->FormatTag != WAVE_FORMAT_PCM)
  {
    return(UNSUPPORTED_FORMAT_TAG);
  }
  HeadTabIdx += 2;
  
  /* Read the number of channels: 0x02->Stereo 0x01->Mono --------------------*/
  pWaveFormatStruct->NumChannels = ReadUnit(2, LITTLE_ENDIAN, pHeader, HeadTabIdx);
  HeadTabIdx += 2;
   
  /* Read the Sample Rate ----------------------------------------------------*/
  pWaveFormatStruct->SampleRate = ReadUnit(4, LITTLE_ENDIAN, pHeader, HeadTabIdx);
  HeadTabIdx += 4;
  
  /* Read the Byte Rate ------------------------------------------------------*/
  pWaveFormatStruct->ByteRate = ReadUnit(4, LITTLE_ENDIAN, pHeader, HeadTabIdx);
  HeadTabIdx += 4;

  /* Read the block alignment ------------------------------------------------*/
  pWaveFormatStruct->BlockAlign = ReadUnit(2, LITTLE_ENDIAN, pHeader, HeadTabIdx);
  HeadTabIdx += 2;

  /* Read the number of bits per sample --------------------------------------*/
  pWaveFormatStruct->BitsPerSample = ReadUnit(2, LITTLE_ENDIAN, pHeader, HeadTabIdx);
  HeadTabIdx += 2;
  
  if (pWaveFormatStruct->BitsPerSample != BITS_PER_SAMPLE_16)
  {
    return(UNSUPPORTED_BITS_PER_SAMPLE);
  }
  /* If there are Extra format bytes, these bytes will be defined in "Fact Chunk" */
  if (ExtraFormatBytes == 1)
  {
    /* Read the Extra format bytes, must be 0x00 ------------------------------*/
    Temp = ReadUnit(2, LITTLE_ENDIAN, pHeader, HeadTabIdx);
    if (Temp != 0x00)
    {
      return(UNSUPPORTED_EXTRA_FORMAT_BYTES);
    }
    HeadTabIdx += 2;
    
    /* Read the Fact chunk, must be 'fact' -----------------------------------*/
    while (ReadUnit(4, BIG_ENDIAN, pHeader, HeadTabIdx) != FACT_ID)
    {
      if (HeadTabIdx >= MAX_AUDIO_HEADER_SIZE)
      {
        return (INVALID_FACT_CHUNK_ID);
      }
      else
      {
        HeadTabIdx++;
      }
    }
    HeadTabIdx += 4;
    
    /* Read Fact chunk data Size ---------------------------------------------*/
    Temp = ReadUnit(4, LITTLE_ENDIAN, pHeader, HeadTabIdx);
    HeadTabIdx += 4;
    
    /* Set the index to start reading just after the header end */
    HeadTabIdx += Temp;
  }
  /* Read the Data chunk, must be 'data' -------------------------------------*/
  
  
  /* Read the Fact chunk, must be 'fact' -----------------------------------*/
  while (ReadUnit(4, BIG_ENDIAN, pHeader, HeadTabIdx) != DATA_ID)
  {
    if (HeadTabIdx >= MAX_AUDIO_HEADER_SIZE)
    {
      return (INVALID_DATA_CHUNK_ID);
    }
    else
    {
      HeadTabIdx++;
    }
  }
  HeadTabIdx += 4;
  
  /* Read the number of sample data ------------------------------------------*/
  pWaveFormatStruct->AudioLength = ReadUnit(4, LITTLE_ENDIAN, pHeader, HeadTabIdx);
  HeadTabIdx += 4;
  
  /* Set the data pointer at the beginning of the effective audio data */
  pWaveFormatStruct->AudioStartAddr += HeadTabIdx;
  
  return(VALID_WAVE_FILE);
}

/**
  * @brief  Initialize the wave header file
  * @param  pHeader: Header Buffer to be filled
  * @param  pWaveFormatStruct: Pointer to the wave structure to be filled.
  * @retval 0 if passed, !0 if failed.
  */
static uint32_t WavProcess_HeaderInit(uint8_t* pHeader, WAVE_FormatTypeDef* pWaveFormatStruct)
{
  /* write chunkID, must be 'RIFF'  ----------------------------------------------*/
  pHeader[0] = 'R';
  pHeader[1] = 'I';
  pHeader[2] = 'F';
  pHeader[3] = 'F';

  /* Write the file length ----------------------------------------------------*/
  /* The sampling time: this value will be be written back at the end of the 
   recording opearation.  Example: 661500 Btyes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
  pHeader[4] = 0xFC; //pWaveFormatStruct->AudioLength
  pHeader[5] = 0x17;
  pHeader[6] = 0x0A;
  pHeader[7] = 0x00;

  /* Write the file format, must be 'WAVE' ------------------------------------*/
  pHeader[8]  = 'W';
  pHeader[9]  = 'A';
  pHeader[10] = 'V';
  pHeader[11] = 'E';

  /* Write the format chunk, must be'fmt ' --------------------------------------*/
  pHeader[12]  = 'f';
  pHeader[13]  = 'm';
  pHeader[14]  = 't';
  pHeader[15]  = ' ';

  /* Write the length of the 'fmt' data, must be 0x10 -------------------------*/
  pHeader[16]  = 0x10;
  pHeader[17]  = 0x00;
  pHeader[18]  = 0x00;
  pHeader[19]  = 0x00;

  /* Write the audio format, must be 0x01 (PCM) -------------------------------*/
  pHeader[20]  = 0x01;
  pHeader[21]  = 0x00;

  /* Write the number of channels, ie. 0x01 (Mono) ----------------------------*/
  pHeader[22]  = pWaveFormatStruct->NumChannels;
  pHeader[23]  = 0x00;

  /* Write the Sample Rate in Hz ----------------------------------------------*/
  /* Write Little Endian ie. 8000 = 0x00001F40 => byte[24]=0x40, byte[27]=0x00*/
  pHeader[24]  = (uint8_t)((pWaveFormatStruct->SampleRate & 0xFF));//0x40;
  pHeader[25]  = (uint8_t)((pWaveFormatStruct->SampleRate >> 8) & 0xFF);//0x1F;
  pHeader[26]  = (uint8_t)((pWaveFormatStruct->SampleRate >> 16) & 0xFF);//0x00;
  pHeader[27]  = (uint8_t)((pWaveFormatStruct->SampleRate >> 24) & 0xFF);//0x00;

  /* Write the Byte Rate ------------------------------------------------------*/
  pHeader[28]  = (uint8_t)((pWaveFormatStruct->ByteRate & 0xFF));
  pHeader[29]  = (uint8_t)((pWaveFormatStruct->ByteRate >> 8) & 0xFF);
  pHeader[30]  = (uint8_t)((pWaveFormatStruct->ByteRate >> 16) & 0xFF);
  pHeader[31]  = (uint8_t)((pWaveFormatStruct->ByteRate >> 24) & 0xFF);

  /* Write the block alignment ------------------------------------------------*/
  pHeader[32]  = pWaveFormatStruct->BlockAlign; 
  pHeader[33]  = 0x00;

  /* Write the number of bits per sample --------------------------------------*/
  pHeader[34]  = pWaveFormatStruct->BitsPerSample;
  pHeader[35]  = 0x00;

  /* Write the Data chunk, must be 'data' ---------------------------------------*/
  pHeader[36]  = 'd';
  pHeader[37]  = 'a';
  pHeader[38]  = 't';
  pHeader[39]  = 'a';

  /* Write the number of sample data ------------------------------------------*/
  /* This variable will be written back at the end of the recording operation */
  pHeader[40]  = 0xFC;
  pHeader[41]  = 0x17;
  pHeader[42]  = 0x0A;
  pHeader[43]  = 0x00;
  
  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Reads a number of bytes from a buffer and return them
  * @param  NbrOfBytes : number of bytes to read. This parameter must be a 
  *         number between 1 and 4.
  * @param  ReadAddr : memory address to read from.
  * @param  Endians : specifies the bytes endianness.
  *         This parameter can be one of the following values: LITTLE_ENDIAN or
  *         BIG_ENDIAN 
  * @retval Bytes read from the buffer..
  */
static uint32_t ReadUnit(uint8_t NbrOfBytes, uint32_t BytesFormat, uint8_t* pBuff, uint32_t BuffIdx)
{
  uint32_t index = 0;
  uint32_t tmp = 0;
  uint32_t Idx = BuffIdx;

  if (BytesFormat == LITTLE_ENDIAN)
  {
    for (index = 0; index < NbrOfBytes; index++)
    {
      tmp |= pBuff[Idx++] << (index * 8);
    }
  }
  else
  {
    for (index = NbrOfBytes; index != 0; index--)
    {
      tmp |= pBuff[Idx++] << ((index - 1) * 8);
    }
  }

  return tmp;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
