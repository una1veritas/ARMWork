/**
  ******************************************************************************
  * @file    stm32_audio_player.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file provides all the Application firmware functions.
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

#include  "stm32_audio_player.h"

/** @addtogroup STM32_Audio_Utilities
  * @{
  */


/** @defgroup STM32_AUDIO_PLAYER 
  * @brief STM32 Audio Player module
  * @{
  */ 

/** @defgroup STM32_AUDIO_PLAYER_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32_AUDIO_PLAYER_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32_AUDIO_PLAYER_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32_AUDIO_PLAYER_Private_Variables
  * @{
  */ 
/* Main Buffer for Audio Data Out transfers and its relative pointers */
typedef struct
{
  uint16_t* pBuf;
  uint32_t  Size;
  uint32_t  isReady;
}AUDIO_Buffer_TypeDef;

AUDIO_Buffer_TypeDef AudioBuffers[3];

uint32_t OutPacketSize = MAX_OUT_PACKET_SZE;
uint16_t *OutBuff1 = NULL, *OutBuff2 = NULL, *OutBuff3 = NULL;
uint32_t WrBuffIdx = 0, RdBuffIdx = 0;


/* Current decoded buffer size */
uint32_t      tsize = 0;

FIL AudioFile;
__IO uint32_t isFileInfoAvailable = 0;

 /* Buffer used to store the audio file header */
uint8_t tHeaderTmp[MAX_AUDIO_HEADER_SIZE]; 
__IO uint32_t NumberOfData = 0;

extern uint32_t DataStartOffset;

static AudioPlayerState_TypeDef AudioPlayerState = PLAYER_IDLE;

static uint32_t      SamplingRate = 0;

/* Decoder struncture instance */
Decoder_TypeDef sDecoderStruct;

/**
  * @}
  */ 


/** @defgroup STM32_AUDIO_PLAYER_Private_FunctionPrototypes
  * @{
  */ 
void  AudioPlayer_XferCplt (uint8_t Direction, uint8_t** pbuf, uint32_t* pSize);
static uint32_t Player_FRead (void* pfile, uint8_t* pbuf, uint32_t size);
/* static uint32_t AudioPlayer_MonoToStereo(int16_t* BIn, int16_t* BOut, uint32_t Size); */
/**
  * @}
  */ 

/** @defgroup STM32_AUDIO_PLAYER_Private_Functions
  * @{
  */ 

/**
  * @brief  Initialize all resources used by the Audio Player
  * @param  None
  * @retval None
  */
uint32_t AudioPlayer_Init(void)
{    
  /* Set the default state of the player to idle */
  AudioPlayerState = PLAYER_IDLE;
  
  /* Inititalize the decoder instance with Null pointers */
  Decoders_SelectDecoder(&sDecoderStruct, 'v');

  /* Initialize the main audio player task (handling the buffers read and decode) */
  AudioPlayer_TaskInit();
  
  /* Set packet size to the default value */
  OutPacketSize = MAX_OUT_PACKET_SZE;

  /* Allocate max packet size to avoid fragmentation */
  OutBuff1 =(uint16_t*) malloc(MAX_OUT_PACKET_SZE);
  OutBuff2 =(uint16_t*) malloc(MAX_OUT_PACKET_SZE);
  OutBuff3 =(uint16_t*) malloc(MAX_OUT_PACKET_SZE);
  if ((OutBuff1 == NULL) || (OutBuff2 == NULL) || (OutBuff3 == NULL))
  {
    free (OutBuff1);
    free (OutBuff2);
    free (OutBuff3);
    return 1;
  }
    
  return 0;
}

/**
  * @brief  Free all resources used by the Audio Player
  * @param  None
  * @retval None
  */
uint32_t AudioPlayer_DeInit(void)
{  
  /* Set the default state of the player to idle */
  AudioPlayerState = PLAYER_IDLE;
  
  /* Stop and free resources used by main audio player task */
  AudioPlayer_TaskInit();  
  
  /* Free the audio buffer */
  if (OutBuff1 != NULL)
  {
    free(OutBuff1);
    OutBuff1 = NULL;
  }
  if (OutBuff2 != NULL)
  {
    free(OutBuff2);
    OutBuff2 = NULL;
  }
  if (OutBuff3 != NULL)
  {
    free(OutBuff3);
    OutBuff3 = NULL;
  }
  
  WrBuffIdx = RdBuffIdx = 0;
  
  return 0;
}

/**
  * @brief  Starts a new session of audio playing (variables initializations...)
  * @param  None
  * @retval None
  */
uint32_t AudioPlayer_Open(uint8_t  *FilePath)
{
  uint32_t volume = DEFAULT_VOLUME;  
  
  /* Check player state */
  if (AudioPlayerState != PLAYER_IDLE)
  {
    return 1;
  }
  
  /* Open the audio file */
  if(f_open(&AudioFile, (char *)FilePath, FA_OPEN_EXISTING | FA_READ) != FR_OK)
  {
    return 1;
  }
  
  /* Get the header buffer from the audio File */
  f_read(&AudioFile, tHeaderTmp, MAX_AUDIO_HEADER_SIZE, (uint32_t*)(&NumberOfData));
  
  /* Select the appropriate decoder */
  Decoders_SelectDecoder(&sDecoderStruct, FilePath[strlen((char *)FilePath) - 1]);

  /* Use the most suitable packet size */
  OutPacketSize = sDecoderStruct.PacketSize;
  
  /* Initialization of the decoder */
  if (sDecoderStruct.DecoderInit != NULL)
  {
    if (sDecoderStruct.DecoderInit((uint8_t*)tHeaderTmp, 
                                   Dec_ReadDataCallback, 
                                   AudioPlayer_SetPosition) != 0)
    {
      return 2;
    }
  }  

  /* Extract the current sampling rate */
  if (sDecoderStruct.Decoder_GetSamplingRate != NULL)
  {
    SamplingRate = sDecoderStruct.Decoder_GetSamplingRate();
  }
  
  /* Initialize the Audio output Hardware layer */
  if (AUDIO_OUT_fops.Init(SamplingRate, volume, 0) != 0)
  {
    return 3;
  }
  
  /* Set the callback to be called when a buffer transfer complete occurs */
  AUDIO_OUT_fops.SetXferCpltCallback(AudioPlayer_XferCplt);

  return 0;
}

/**
  * @brief  Closes the current session of audio playing (free resources).
  * @param  None
  * @retval None
  */
uint32_t AudioPlayer_Close(void)
{  
  /* If current state is different from IDLE then it is not possible to close */
  if (AudioPlayerState != PLAYER_IDLE)
  {
    return 1;
  }
  
  /* DeInitialize the Audio output Hardware layer */
  if (AUDIO_OUT_fops.DeInit(0) != 0)
  {
    return 2;
  }

  /* Close the decoder instance */
  if (sDecoderStruct.DecoderDeInit != NULL)
  {
    sDecoderStruct.DecoderDeInit(); 
  }
  
  /* Empty the decoder structure */
  Decoders_SelectDecoder(&sDecoderStruct, ' ');
  
  /* Close the current file */
  f_close(&AudioFile);
  
  return 0;
}

/**
  * @brief  Update state and pointers. This function is called when previous buffer
  *         has been consumed and the next buffer should be prepared (read from
  *         storage).  
  * @param  None
  * @retval None
  */

uint32_t AudioPlayer_PlayUpdate(void)
{      
  /* Check if the buffer has already been used */
  if ((AudioBuffers[WrBuffIdx].isReady == 0))
  {
    /* Read and decode next buffer */
    AudioBuffers[WrBuffIdx].Size = Player_FRead (NULL, 
                                                 (uint8_t*)AudioBuffers[WrBuffIdx].pBuf, 
                                                 OutPacketSize);
  
    AudioBuffers[WrBuffIdx].isReady = 1;
    
    /* Update index */
    WrBuffIdx = (WrBuffIdx >= 2)? 0:(WrBuffIdx+1);
  }
  
  return 0;
}

/**
  * @brief  Main audio player task initialization.
  * @param  None
  * @retval None
  */
void AudioPlayer_TaskInit()
{   
  NVIC_InitTypeDef  NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
 
  /* Initialize the EXTI line used to trigger the software interrupt */
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
 
  /* Set the IRQ channel for the software triggered interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
}

/**
  * @brief  Free resources used by the audio player task.
  * @param  None
  * @retval None
  */
void AudioPlayer_TaskDeInit()
{   
  NVIC_InitTypeDef  NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
 
  /* Initialize the EXTI line used to trigger the software interrupt */
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);
 
  /* Set the IRQ channel for the software triggered interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure); 
}
/**
  * @brief  Main audio player task.
  * @param  Param: not used.
  * @retval None
  */
void AudioPlayer_Task(void * Param)
{   
  /* Check if a buffer need to be prepared */    
  if (AudioPlayerState == PLAYER_PLAYING)
  {
    /* Get next buffer from mass storage device and decode it */
    AudioPlayer_PlayUpdate();
  }
}

/**
  * @brief  This function handles EXTI4 global interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void)
{
  EXTI_ClearITPendingBit(EXTI_Line4);

  /* Call the main player task */
  AudioPlayer_Task(NULL);
}

/**
  * @brief  Start playing the audio file.
  * @param  None
  * @retval None
  */
uint32_t AudioPlayer_Play(uint8_t *FilePath)
{ 
  switch (AudioPlayerState)
  {
  case PLAYER_IDLE:
    /* Open the audio file for read */
    if (AudioPlayer_Open(FilePath) != 0)
    {      
      return 1;
    }
    
    AudioBuffers[0].pBuf = OutBuff1;
    AudioBuffers[1].pBuf = OutBuff2;
    AudioBuffers[2].pBuf = OutBuff3;
    for (int i = 0; i<3; i++)
    {
      AudioBuffers[i].Size = MAX_OUT_PACKET_SZE;
      AudioBuffers[i].isReady = 0;
    }
    WrBuffIdx = RdBuffIdx = 0;
    
    /* Fill all buffers with data */
    for (int i = 0; i<3; i++)
    {
      AudioPlayer_PlayUpdate();
    }

    /* Set the current state */
    AudioPlayerState = PLAYER_PLAYING;
    
    /* Start the audio streaming */
    AUDIO_OUT_fops.AudioCmd((uint8_t*)AudioBuffers[0].pBuf,  /* Samples buffer pointer */
                            AudioBuffers[0].Size,  /* Number of samples in Bytes */
                            AUDIO_CMD_PLAY);       /* Command to be processed */ 

    break;
    
  case PLAYER_PLAYING:
    break;
    
  case PLAYER_PAUSED:
    /* Set the current state */
    AudioPlayerState = PLAYER_PLAYING;
    
    /* Resume the audio stream */
    AUDIO_OUT_fops.AudioCmd((uint8_t*)AudioBuffers[RdBuffIdx].pBuf,  /* Samples buffer pointer */
                            AudioBuffers[RdBuffIdx].Size,  /* Number of samples in Bytes */
                            AUDIO_CMD_PLAY);               /* Command to be processed */ 
    break;
    
  case PLAYER_ERROR:
    break;    
  }

  return 0;
}

/**
  * @brief  Pause the audio file streaming.
  * @param  None
  * @retval None
  */
uint32_t AudioPlayer_Pause(void)
{
  switch (AudioPlayerState)
  {    
  case PLAYER_IDLE:
    break;
    
  case PLAYER_PLAYING:
    /* Set the current state to paused */
    AudioPlayerState = PLAYER_PAUSED;

    /* Pause the audio stream */
    AUDIO_OUT_fops.AudioCmd(0,                 /* Samples buffer pointer */
                            0,                 /* Number of samples in Bytes */
                            AUDIO_CMD_PAUSE);  /* Command to be processed */
    break;
    
  case PLAYER_PAUSED:
    break;
    
  case PLAYER_ERROR:
    break;    
  } 

  return 0;
}

/**
  * @brief  Stops the audio file streaming.
  * @param  None
  * @retval None
  */
uint32_t AudioPlayer_Stop(void)
{
  /* Check the current status of the player */
  if ((AudioPlayerState == PLAYER_PLAYING) || \
      (AudioPlayerState == PLAYER_PAUSED))
  {
    /* Set the current state */
    AudioPlayerState = PLAYER_IDLE;
 
    /* Pause the audio stream */
    AUDIO_OUT_fops.AudioCmd(0,                /* Samples buffer pointer */
                            0,                /* Number of samples in Bytes */
                            AUDIO_CMD_PAUSE); /* Command to be processed */
  }

  return 0;
}

/**
  * @brief  Forward the current position of audio file pointer.  
  * @param  None
  * @retval None
  */
uint32_t AudioPlayer_Forward(void)
{
  /*
     Not yet implemented...  
   */
  return 0;
}

/**
  * @brief  Rewind the current position of audio file pointer.
  * @param  None
  * @retval None
  */
uint32_t AudioPlayer_Rewind(void)
{
  /*
     Not yet implemented...  
   */
  return 0;
}

/**
  * @brief  Manages the volume of the current played file.
  * @param  None
  * @retval None
  */
uint32_t AudioPlayer_VolumeCtrl(uint8_t Volume)
{
   /* Update the volume level */
   return AUDIO_OUT_fops.VolumeCtl(Volume);
}

/**
  * @brief  Updates the current track information.
  * @param  None.
  * @retval pointer to a track information structure to be updated
  */
TAGS_TypeDef* AudioPlayer_GetFileInfo(void)
{  
  return (NULL);
}


/**
  * @brief  Updates the current time information (expressed in seconds).
  * @param  Length: pointer to the variable containing the total audio track length
  * @param  Total: pointer to the variable containing the elapsed time
  * @retval 0 if Passed, !0 else.
  */
uint32_t AudioPlayer_GetTimeInfo(uint32_t* Length, uint32_t* Elapsed)
{ 
  if (sDecoderStruct.Decoder_GetStreamLength != NULL)
  {
    *Length = sDecoderStruct.Decoder_GetStreamLength(AudioPlayer_GetFileLength());
  }
  if (sDecoderStruct.Decoder_GetElapsedTime != NULL)
  {
    *Elapsed = sDecoderStruct.Decoder_GetElapsedTime(AudioFile.fptr);
  }
  
  return 0;
}



/**
  * @brief  Reads a number of data from the audio file (if the fils is compressed, 
  *         then the uncompression algorithm function is called).
  * @param  pfile: pointer to the file.
  * @param  pbuf: Pointer to the address of the ram buffer to be filled
  * @param  size: number of data to be read in bytes
  * @retval number of data actually read (in bytes).
  */
static uint32_t  Player_FRead (void* pfile, uint8_t* pbuf, uint32_t size)
{  
  /* Initialize the total size variable */
  tsize = 0;
  
  /* Call the low layer function to get raw data */ 
  if (sDecoderStruct.Decoder_DecodeData != NULL)
  {
    tsize = sDecoderStruct.Decoder_DecodeData((__IO int16_t*)pbuf, (size/4), NULL);
  }

  /* If there is no data to read, exit player */
  if (tsize == 0)
  {
    /* Check on the end of file */
    AudioPlayer_Stop();
    return 0;
  }
  
  /* Workarround for WMA decoder (can be used for some specific cases):
    If the PCM buffer is too small, get more samples */  
  /* 
  if (tsize < 256*4)
  {
    SmallPacketCount++;
    tsize += sDecoderStruct.Decoder_DecodeData((__IO int16_t*)(pbuf + tsize/2), (size/4), NULL);
  }  
  */ 
  
  return tsize;
}

/**
  * @brief  Manage end of buffer transfer for each device.
  * @param  Direction: could be DIRECTION_IN or DIRECTION_OUT
  * @param  pbuf: Pointer to the address of the current buffer
  * @param  pSize: pointer to the variable which holds current buffer size
  * @retval status
  */
void  AudioPlayer_XferCplt (uint8_t Direction, uint8_t** pbuf, uint32_t* pSize)
{
  if (AudioPlayerState == PLAYER_PLAYING)
  {
    /* Release previous buffer for write operation */
    AudioBuffers[RdBuffIdx].isReady = 0;
    
    /* Increment the buffer index */
    RdBuffIdx = (RdBuffIdx >= 2)? 0:(RdBuffIdx+1);
    
    if (!(AudioBuffers[RdBuffIdx].isReady))
    {
      RdBuffIdx = (RdBuffIdx > 0)? (RdBuffIdx-1):2;
    }
    
    /* Resume the audio stream */
    AUDIO_OUT_fops.AudioCmd((uint8_t*)AudioBuffers[RdBuffIdx].pBuf,  /* Samples buffer pointer */
                            AudioBuffers[RdBuffIdx].Size,  /* Number of samples in Bytes */
                            AUDIO_CMD_PLAY);               /* Command to be processed */
    
    /* Trigger the Main audio player task to update the next buffers */
    EXTI_GenerateSWInterrupt(EXTI_Line4);
  }
}

/**
  * @brief  Sets the current position of the audio file pointer.
  * @param  Pos: pointer position to be set
  * @retval None
  */
uint32_t AudioPlayer_SetPosition(uint32_t Pos)
{
  /* Call the Fat FS seek function */
  return f_lseek(&AudioFile, Pos);
}

/**
  * @brief  Sets the current position of the audio file pointer.
  * @param  Pos: pointer position to be set
  * @retval None
  */
uint32_t AudioPlayer_GetFileLength(void)
{
  /* Call the Fat FS seek function */
  return AudioFile.fsize;
}

/**
  * @brief  Returns the current state of the audio player.
  * @param  None
  * @retval PLAYER_IDLE, PLAYER_PLAYING, PLAYER_PAUSED, PLAYER_STOPPED or PLAYER_ERROR.
  */
AudioPlayerState_TypeDef AudioPlayer_GetState(void)
{
  /* Return the current state of the audio player */
  return AudioPlayerState;
}

/**
  * @brief  Callback function to supply the decoder with input MP3 bitsteram.
  * @param  pMP3CompressedData: pointer to the target buffer to be filled.
  * @param  nMP3DataSizeInChars: number of data to be read in bytes
  * @param  pUserData: optional parameter (not used in this version)
  * @retval return the mp3 decode frame.
  */
unsigned int Dec_ReadDataCallback(void *    pCompressedData,        /* [OUT] Bitbuffer */
                                  uint32_t  nDataSizeInChars, /* sizeof(Bitbuffer) */
                                  void *    pUserData               /* Application-supplied parameter */
)
{
  uint32_t tmp = 0x00;
  
  tmp = f_read(&AudioFile, pCompressedData, nDataSizeInChars, (uint32_t*)(&NumberOfData));
  
  if (tmp != FR_OK)
  {
    AudioPlayer_Stop();
    
    /* 
      ...
      Error management 
      ...
     */

    return 0;    
  }

  /* Check on the end of file */
  if (AudioFile.fptr >= AudioFile.fsize)
  {
    /* AudioPlayer_SetPosition(DataStartOffset); *//* Fast replay without GUI update */
    /* return  NumberOfData; */
    AudioPlayer_Stop();
  }

  return  NumberOfData;
}

/**
* @brief  Transforms a 16-bit mono buffer to a 16-bit stereo buffer by duplicating all values
* @param  BIn    : pointer to the input frame
* @param  BOut   : pointer to the output encoded frame (can be the same input buffer)
* @param  Size   : Input buffer frame size
* @retval 0 if OK.
*/
/* static uint32_t AudioPlayer_MonoToStereo(int16_t* BIn, int16_t* BOut, uint32_t Size)
{
  int32_t i;
  
  for (i = (Size*2)-1; i >= 0; i -= 2)
  {
    BOut[i] = BIn[i/2];
    BOut[i - 1] = BIn[i/2];
  }
  
  return 0;
}
*/
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
