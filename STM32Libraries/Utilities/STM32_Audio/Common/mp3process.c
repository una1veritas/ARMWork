/**
  ******************************************************************************
  * @file    mp3process.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file includes MP3 processing layer routines
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
#include "mp3process.h"


/** @addtogroup STM32_Audio_Utilities
  * @{
  */

/** @defgroup MP3PROCESS_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup MP3PROCESS_Private_Defines
  * @{
  */
#define MP3_HEADER_SIZE_POSITION  0x06
/**
  * @}
  */

/** @defgroup MP3PROCESS_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup MP3PROCESS_Private_Variables
  * @{
  */
/* Decoder object pointer */
TSpiritMP3Decoder  *MP3Decoder_Instance;

/* Current Stream Info */
TSpiritMP3Info     mp3_Info;

/* Data offset */
uint32_t DataStartOffset = 0x00;
/* BitRateSave*/
__IO uint16_t LastnBitrateKbps = 0x00, VBR_Detect = 0x00;


int32_t *pcm_buffer1;
uint8_t pbufferID[256];

/**
  * @}
  */

/** @defgroup MP3PROCESS_Private_FunctionPrototypes
  * @{
  */
fnSetPositionCallback_TypeDef* pMP3SetPosCallback;

/**
  * @}
  */

/** @defgroup MP3PROCESS_Private_Functions
  * @{
  */

/**
  * @brief  retrieve mp3 file parameters.
  * @param  None.
  * @retval pointer to 'TSpiritMP3Info' struct.
  */
TSpiritMP3Info * MP3Process_GetFileInfoStruct(void)
{
  return ( TSpiritMP3Info *)&mp3_Info;
}

/**
  * @brief  Decoder structure initialize.
  * @param  None.
  * @retval None.
  */
uint32_t Mp3Process_DecoderInit(uint8_t* pHeader, 
                                fnReadCallback_TypeDef* pReadCallback, 
                                fnSetPositionCallback_TypeDef* pSetPosCallback)
{
  __IO uint16_t time_out = 1153;
  uint8_t tmpBuffer[10];
  uint8_t nb_frame = 0;

  uint32_t i;

  /* Enable the CRC */
  RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
  
  /* Allocate space for the MP3 decoder instance */
  MP3Decoder_Instance = (TSpiritMP3Decoder*)malloc(sizeof(TSpiritMP3Decoder)); /* Might use p_malloc if SDcard is not used */
  
  if (MP3Decoder_Instance == NULL)
  {
    return 0xFF;
  }
  
  /* Temporary buffer */
  pcm_buffer1 = malloc((1152<<1) * sizeof(int16_t));
  if (pcm_buffer1 == NULL)
  {
    free(MP3Decoder_Instance);
    MP3Decoder_Instance = NULL;
    return 0xFE;
  }
  
  /* Initialize the MP3 info structure */
  for (i=0; i < sizeof(TSpiritMP3Info); i++)
  {
    *(uint8_t*)(((uint32_t)&mp3_Info) + i) = 0;
  }

  /* check for headers */
  for (i=0; i < 10; i++)
  {
     tmpBuffer[i] = *(uint8_t*)(((uint32_t)(&pHeader[0])) + i);
  }

  if (tmpBuffer[0] == 'I' && tmpBuffer[1] == 'D' && tmpBuffer[2] == '3')
  {
    /* check for data start offset. */
    DataStartOffset =  ((tmpBuffer[MP3_HEADER_SIZE_POSITION] & 0x7f) << 21)
                       | ((tmpBuffer[MP3_HEADER_SIZE_POSITION + 1] & 0x7f) << 14)
                       | ((tmpBuffer[MP3_HEADER_SIZE_POSITION + 2] & 0x7f) << 7)
                       | (tmpBuffer[MP3_HEADER_SIZE_POSITION + 3] & 0x7f);
  }
  else
  {
    DataStartOffset = 0;
  }

  if (&mp3_Info != NULL)
  {
    /* Initialize the decoder */
    SpiritMP3DecoderInit(MP3Decoder_Instance, 
                         pReadCallback , 
                         NULL, 
                         NULL);

    pMP3SetPosCallback = pSetPosCallback;
    
    /* Set cursor to the actual data position */
    pMP3SetPosCallback(DataStartOffset);

    /* MP3 decoder needs to process more than one frame to be sure that headers are correctly decoded */
    do
    {
      SpiritMP3Decode(
        (TSpiritMP3Decoder*)(MP3Decoder_Instance), /* Decoder structure */
        (int16_t*)pcm_buffer1,                      /* [OUT] Output PCM buffer */
        1152,                                       /* Output PCM buffer size in samples */
        (TSpiritMP3Info*)(&mp3_Info)                /* [OUT] Audio Information structure */
      );

      /* check for two good frames */
      if (!(!((&mp3_Info)->IsGoodStream) || ((&mp3_Info)->nBitrateKbps == 0) || ((&mp3_Info)->nSampleRateHz == 0)))
      {
        nb_frame++;
      }
    }
    while ( (nb_frame < 1) && (time_out-- != 0));
    
    /* Set cursor to the actual data position */
    pMP3SetPosCallback(DataStartOffset);
     
    VBR_Detect = LastnBitrateKbps = 0x00;
  }
  else
  {
    free(MP3Decoder_Instance);
    MP3Decoder_Instance = NULL;
  }
  
  /* Free the pcm_buffer1 before exit */
  if (pcm_buffer1 != NULL)
  {
    free(pcm_buffer1);
    pcm_buffer1 = NULL;
  }
  
  /* Check if the current stream is correct */
  return (mp3_Info.IsGoodStream == 0)? 1:0;
}

/**
  * @brief  Deinitializes the MP3 Decoder structure.
  * @param  None.
  * @retval None.
  */
uint32_t Mp3Process_DecoderDeInit(void)
{
  /* Free all used resources */
  if (MP3Decoder_Instance != NULL)
  {    
    free(MP3Decoder_Instance);
    MP3Decoder_Instance = NULL;
  }
  
  if (pcm_buffer1 != NULL)
  {
    free(pcm_buffer1);
    pcm_buffer1 = NULL;
  }
  
  return 0;
}

/**
  * @brief  provide output PCM stream.
  * @param  OutPutBuffer: output PCM.
  * @param  NbSamples: number of pcm to be decoded.
  * @param  Usr: Not used. Value ignored.
  * @retval Number of decoded data in bytes.
  */
uint32_t Mp3Process_DecodeData(__IO int16_t* OutPutBuffer, uint32_t NbSamples, void * Usr)
{
  uint32_t tmp = 0x00;

  if (MP3Decoder_Instance == NULL)
  {
    return 0;
  }
  
  /* Start MP3 decoding */
  tmp = SpiritMP3Decode(
          (TSpiritMP3Decoder*)MP3Decoder_Instance, /* Decoder structure */
          (int16_t*)OutPutBuffer,                  /*[OUT] Output PCM buffer */
          NbSamples,                               /* Output PCM buffer size in samples */
          &mp3_Info                                /* [OUT] Audio Information structure */
        );
  
  /* Check the Current bitrate */
  if ((mp3_Info.IsGoodStream == 0x01) && (VBR_Detect == 0x00))
  {
    if (LastnBitrateKbps == 0x00)
    {
      LastnBitrateKbps = mp3_Info.nBitrateKbps;
    }
    else
    {
      if ((LastnBitrateKbps != mp3_Info.nBitrateKbps))
      {
        VBR_Detect = 0x01;
      }
    }
  }

  return tmp*4; //NbSamples*4; //tmp;
}

/**
  * @brief  Provide the tag of the current stream.
  * @param  pIDTAG: pointer to the Tag structure.
  * @retval None.
  */
uint32_t Mp3Process_RetrieveIDTAGS(TAGS_TypeDef * pIDTAG, FIL *pFile)
{
  uint32_t i;

  /* pbuffer initialization */
  for (i= 0; i< 256; i++)
  {
    pbufferID[i] = 0;
  }
  
  if (pFile != NULL)
  {
    f_lseek(pFile, 0);
    SongUtilities_MP3TagParser(pbufferID, pIDTAG, pFile);
    f_lseek(pFile, 0);
  }
  
  return 0;
}

/**
  * @brief  Get the lenght of the current stream.
  * @param  None.
  * @retval return the stream lenght.
  */
uint32_t Mp3Process_GetStreamLenght(uint32_t fLength)
{
  if (mp3_Info.nBitrateKbps != 0)
    return ((((fLength - DataStartOffset) * 8 ) / 1024) / (mp3_Info.nBitrateKbps ));
  else
    return 0;
}

/**
  * @brief  Get the elapsed time of the current stream.
  * @param  CurPos: Current position of pointer in the audio stream.
  * @retval return the elapsed stream time in seconds.
  */
uint32_t Mp3Process_GetElapsedTime(uint32_t CurPos)
{
  if ((&mp3_Info)->nBitrateKbps != 0)
  {
    if (CurPos >= DataStartOffset)
      return ((((CurPos - DataStartOffset) * 8  ) / 1024) / ((&mp3_Info)->nBitrateKbps ));
    else
    {
      return 0;
    }
  }
  else
    return 0;
}

/**
  * @brief  Variable bit rate detect.
  * @param  None.
  * @retval return 0 if no VBR is detected else !0.
  */
uint32_t Mp3Process_VBRDetected(void)
{
  return VBR_Detect;
}
/**
  * @brief  Get the sampling rate of the current stream.
  * @param  None.
  * @retval Return the sampling rate.
  */
uint32_t Mp3Process_GetSamplingRate(void)
{
  return (&mp3_Info)->nSampleRateHz;
}

/**
  * @brief  Forward the current stream
  * @param  None.
  * @retval None.
  */
uint32_t Mp3Process_StreamForward(void)
{
  /* N/A */
  
  return 0;  
}

/**
  * @brief  Backward the current stream
  * @param  None.
  * @retval None.
  */
uint32_t Mp3Process_StreamBackward(void)
{
  /* N/A */
  
  return 0; 
}

/**
  * @brief  Stop file decoding.
  * @param  None.
  * @retval None.
  */
uint32_t Mp3Process_StopMode(void)
{
  /* N/A */
  
  return 0; 
}

/****** MP3 ENCODER ***********************************************************************/
#include "spiritMP3Enc.h"

TMP3E_persist* pMP3EncPersistent = NULL;
TMP3E_scratch* pMP3EncScratch = NULL;


uint32_t MP3EncAudioLength = 0;
uint32_t MP3EncSampleRate  = 8000;
uint32_t MP3EncNumChannels = 2;
uint32_t MP3EncBitRate     = 48000;


/**
* @brief  
* @param  
* @retval 
*/
uint32_t Mp3Process_EncoderInit        (uint32_t Freq, uint8_t* Param)
{
  /* Enable the CRC */
  RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
  
  /* Update sample freq */
  MP3EncSampleRate = Freq;
  
  pMP3EncPersistent = p_malloc(MP3E__SizeofPersistent()); /* 8300 */
  if(pMP3EncPersistent == NULL) 
  {
    return 0xFF;
  }
  
  pMP3EncScratch = p_malloc(MP3E__SizeofScratch()); /* 7400 */
  if(pMP3EncScratch == NULL) 
  {
    p_free(pMP3EncPersistent);
    pMP3EncPersistent = NULL;
    return 0xFF;
  }
  
  /*
    uiSampleRateHz:
        48000, 44100, 32000 (MPEG-1)
        24000, 22050, 16000 (MPEG-2)
        12000, 11025,  8000 (MPEG-2.5)
    iBitrateKbps:
        32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 for MPEG-1 sampling rates
        8,  16, 24, 32, 40, 48, 56, 64,  80,  96,  112, 128, 144, 160 for MPEG-2 sampling rates
        8,  16, 24, 32, 40, 48, 56, 64 for MPEG-2.5 sampling rates  
  */
  
  if (MP3E__Init(pMP3EncPersistent,
                 MP3EncSampleRate,       /* Sample rate */
                 MP3EncBitRate/1000,     /* Bitrate, Kbps */
                 MP3EncNumChannels,      /* Number of input channels */
                 0))                     /* Cut-off freq Hz */

  {
    return 0xFE;
  }
  
  
  MP3EncAudioLength = 0;
  
  return 0;
}

/**
* @brief  
* @param  
* @retval 
*/
uint32_t Mp3Process_EncoderDeInit      (void)
{
  /* Free allocated resources */
  if (pMP3EncPersistent != NULL)
  {
    p_free(pMP3EncPersistent);
    pMP3EncPersistent = NULL;
  }
  
  if (pMP3EncScratch != NULL)
  {
    p_free(pMP3EncScratch);
    pMP3EncScratch = NULL;
  }  
  
  MP3EncAudioLength = 0;
  
  return 0;
}

/**
* @brief  
* @param  
* @retval 
*/
uint32_t Mp3Process_EncoderStopMode    (void)
{  
  
  return 0;
}

/**
* @brief  
* @param  
* @retval 
*/
uint32_t Mp3Process_EncodeData        (int8_t *pBufIn, int8_t *pBufOut, uint32_t* nEnc, void* pOpt)
{
  
  /* Update the total audio data size */
  MP3EncAudioLength += *nEnc;

  /* Encode PCM data and get the MP3 frame */
  *(uint32_t*)pBufOut = (uint32_t) MP3E__Encode576( pMP3EncPersistent,               
                                                   pMP3EncScratch,                   
                                                   (const short   *) pBufIn,         
                                                   (const short   *) ((uint32_t)pBufIn + 2),
                                                   MP3EncNumChannels,     
                                                   nEnc);
  
    
  
  return 0;
}

/**
* @brief  
* @param  
* @retval 
*/
uint32_t Mp3Process_SetTags           (TAGS_TypeDef* IdTags, void *file)
{  
  
  return 0;
}

/**
* @brief  
* @param  
* @retval 
*/
uint32_t Mp3Process_EncGetSampleRate  (void)
{  
  
  return MP3EncSampleRate;
}

/**
* @brief  
* @param  
* @retval 
*/
uint32_t Mp3Process_EncGetStreamLength(void)
{  
  
  return MP3EncAudioLength;
}

/**
* @brief  
* @param  
* @retval 
*/
uint32_t Mp3Process_EncGetElapsedTime (void)
{
  return (MP3EncAudioLength / 
          (MP3EncSampleRate * 
          MP3EncNumChannels * 
          (16/8)));
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
