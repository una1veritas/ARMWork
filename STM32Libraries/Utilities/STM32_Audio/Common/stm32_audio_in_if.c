/**
  ******************************************************************************
  * @file    stm32_audio_in_if.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file provides the Audio In (Microphone) interface API.
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
#include "stm32_audio.h"
#include "stm32_audio_in_if.h"

#ifdef USE_STM3210C_EVAL
 #include "stm3210c_audio_recorder.h"
#elif defined(USE_STM322xG_EVAL)
 #include "stm322xg_audio_recorder.h"
#elif defined(USE_STM324xG_EVAL)
 #include "stm324xg_audio_recorder.h"
#elif defined(USE_STM32F4XX_DISCOVERY)
 #include "stm32f4_discovery_audio_recorder.h"
#endif /* USE_STM3210C_EVAL */

/** @addtogroup STM32_Audio_Utilities
  * @{
  */


/** @defgroup stm32_audio_in_if 
  * @brief Audio IN interface module
  * @{
  */ 

/** @defgroup stm32_audio_in_if_Private_TypesDefinitions
  * @{
  */ 
#ifndef DEFAULT_IN_AUDIO_FREQ
 #define DEFAULT_IN_AUDIO_FREQ                    ( 8000 ) /* 8 KHz  */
#endif /* DEFAULT_IN_AUDIO_FREQ */

/**
  * @}
  */ 


/** @defgroup stm32_audio_in_if_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup stm32_audio_in_if_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup stm32_audio_in_if_Private_FunctionPrototypes
  * @{
  */
static uint8_t  Init         (uint32_t  AudioFreq, uint32_t Volume, uint32_t options);
static uint8_t  DeInit       (uint32_t options);
static uint8_t  AudioCmd     (uint8_t* pbuf, uint32_t size, uint8_t cmd);
static uint8_t  VolumeCtl    (uint8_t vol);
static uint8_t  MuteCtl      (uint8_t cmd);
static uint32_t OptionCtrl   (uint8_t* cmd, uint32_t size);
static uint8_t  GetState     (void);
static void     SetXferCpltCallback(fnXFerCpltCallback_TypeDef* Clbck);
__IO   uint32_t InOptions = 0;

/**
  * @}
  */ 

/** @defgroup stm32_audio_in_if_Private_Variables
  * @{
  */ 
AUDIO_FOPS_TypeDef  AUDIO_IN_fops = 
{
  Init,
  DeInit,
  AudioCmd,
  VolumeCtl,
  MuteCtl,
  OptionCtrl,
  GetState,
  SetXferCpltCallback,
  &InOptions
};

static uint8_t AudioState = AUDIO_IN_STATE_INACTIVE;
static uint32_t Initialized = 0;
fnXFerCpltCallback_TypeDef* InXferCpltCallback = NULL;

/**
  * @}
  */ 

/** @defgroup stm32_audio_in_if_Private_Functions
  * @{
  */ 

/**
  * @brief  Initialize and configures all required resources for audio record function.
  * @param  AudioFreq: Statrtup audio frequency. 
  * @param  Volume: Startup volume to be set.
  * @param  options: specific options passed to low layer function.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  Init         (uint32_t AudioFreq, 
                              uint32_t Volume, 
                              uint32_t options)
{  
  /* Check if the low layer has already been initialized */
  if (Initialized == 0)
  {
    /* Call low layer function */
    STM32_AudioRec_Init(AudioFreq, DEFAULT_IN_BIT_RESOLUTION, DEFAULT_IN_CHANNEL_NBR);
    
    /* Set the Initialization flag to prevent reinitializing the interface again */
    Initialized = 1;
  }
  
  /* Update the Audio state machine */
  AudioState = AUDIO_IN_STATE_ACTIVE;
    
  return AUDIO_OK;
}

/**
  * @brief  Free all resources used by low layer and stops audio recoed function.
  * @param  options: options passed to low layer function.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  DeInit       (uint32_t options)
{
  /* Call low layer function */
  /* STM32_AudioRec_DeInit(); */
    
  /* Update the Audio state machine */
  AudioState = AUDIO_IN_STATE_INACTIVE;
  Initialized = 0;
  
  return AUDIO_OK;
}

/**
  * @brief  Record, Stop current stream.
  * @param  pbuf: address to which file shoud be stored.
  * @param  size: size of the current buffer/file.
  * @param  cmd: command to be executed, can be AUDIO_IN_CMD_RECORD or
  *         AUDIO_IN_CMD_STOP.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  AudioCmd(uint8_t* pbuf, 
                         uint32_t size,
                         uint8_t cmd)
{
  /* Check the current state */
  if ((AudioState == AUDIO_IN_STATE_INACTIVE) || (AudioState == AUDIO_IN_STATE_ERROR))
  {
    AudioState = AUDIO_IN_STATE_ERROR;
    return AUDIO_FAIL;
  }
  
  switch (cmd)
  {
    /* Process the RECORD command ----------------------------*/
  case AUDIO_IN_CMD_START:
    /* If current state is Active or Stopped */
    if ((AudioState == AUDIO_IN_STATE_ACTIVE) || \
       (AudioState == AUDIO_IN_STATE_STOPPED))
    {
      /* Start recording */
      STM32_AudioRec_Start(pbuf, size);
      
      /* Update state machine */
      AudioState = AUDIO_IN_STATE_RECORDING;
      return AUDIO_OK;
    } 
    else /* Not allowed command */
    {
      return AUDIO_FAIL;
    }
    
    /* Process the STOP command ----------------------------*/
  case AUDIO_IN_CMD_STOP:
    if (AudioState != AUDIO_IN_STATE_RECORDING)
    {
      /* Unsupported command */
      return AUDIO_FAIL;
    }
    else
    {
      /* Call low layer function */
      STM32_AudioRec_Stop();
      
      /* Update state */
      AudioState = AUDIO_IN_STATE_STOPPED;
      return AUDIO_OK;
    }
  
    /* Unsupported command ---------------------------------*/
  default:
    return AUDIO_FAIL;
  }  
}

/**
  * @brief  Set the volume level in %
  * @param  vol: volume level to be set in % (from 0% to 100%)
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  VolumeCtl    (uint8_t vol)
{
  /* Call low layer volume setting function */
  
  return AUDIO_OK;
}

/**
  * @brief  Mute or Unmute the audio current output
  * @param  cmd: can be 0 to unmute, or 1 to mute.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  MuteCtl      (uint8_t cmd)
{
  /* Call low layer mute setting function */
  
  return AUDIO_OK;
}

/**
  * @brief  Options control function.
  * @param  cmd: pointer to the command/option buffer.
  * @param  size: Size of the command/option buffer.
  * @retval Value of the current audio sampling rate.
  */
static uint32_t  OptionCtrl   (uint8_t* cmd, uint32_t size)
{
  uint32_t freq = 8000;
  
  /* Decode the audio frequency */
  freq = (uint32_t)(*(uint8_t*)(cmd) | (*(uint8_t*)(cmd + 1) << 8) | (*(uint8_t*)(cmd + 2) << 16));

  /* Check if the requested audio frequency is not supported */
  if ((freq > MAX_AUDIO_IN_FREQ) || (freq < MIN_AUDIO_IN_FREQ))
  {
    /* Set the default audio frequency */
    freq = DEFAULT_IN_AUDIO_FREQ;
  }
  
  /* Update the hardware with the new configuration */
  //Record_AudioInterface_Init(freq);
    
  return freq;
}


/**
  * @brief  Return the current state of the audio machine
  * @param  None
  * @retval Current State.
  */
static uint8_t  GetState   (void)
{
  return AudioState;
}

/**
  * @brief  Set the address of the callback function to be called when a buffer transfer is complete.
  * @param  Clbck: pointer to the function callback to be used.
  * @retval None.
  */
static void SetXferCpltCallback(fnXFerCpltCallback_TypeDef* Clbck)
{
  /* Set the pointer to the callback */
  InXferCpltCallback = Clbck;
}

/**
  * @brief  Manage the transfer complete callback function
  * @param  None
  * @retval Current State.
  */
uint32_t stm32_audio_in_XferCplt(uint8_t** pbuf, uint32_t* pSize)
{
  /* Call the higher layer function */
  if (InXferCpltCallback != NULL)
  {
    InXferCpltCallback(DIRECTION_IN, pbuf, pSize);
  }
  
  return 0;
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
