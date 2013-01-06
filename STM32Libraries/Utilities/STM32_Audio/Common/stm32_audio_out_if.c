/**
  ******************************************************************************
  * @file    stm32_audio_out_if.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file provides the Audio Out (palyback) interface API.
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
#include "stm32_audio_out_if.h"
#include "stm32_audio_correction.h"

/** @addtogroup STM32_Audio_Utilities
  * @{
  */


/** @defgroup stm32_audio_out_if 
  * @brief Audio out interface module
  * @{
  */ 

/** @defgroup stm32_audio_out_if_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup stm32_audio_out_if_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup stm32_audio_out_if_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup stm32_audio_out_if_Private_FunctionPrototypes
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
__IO   uint32_t OutOptions = 0;

fnXFerCpltCallback_TypeDef* OutXferCpltCallback = NULL;
/**
  * @}
  */ 

/** @defgroup stm32_audio_out_if_Private_Variables
  * @{
  */ 
AUDIO_FOPS_TypeDef  AUDIO_OUT_fops = 
{
  Init,
  DeInit,
  AudioCmd,
  VolumeCtl,
  MuteCtl,
  OptionCtrl,
  GetState,
  SetXferCpltCallback,
  &OutOptions
};

static uint8_t AudioState = AUDIO_STATE_INACTIVE;
static uint32_t Initialized = 0;
/**
  * @}
  */ 

/** @defgroup stm32_audio_out_if_Private_Functions
  * @{
  */ 

/**
  * @brief  Initialize and configures all required resources for audio play function.
  * @param  AudioFreq: Statrtup audio frequency. 
  * @param  Volume: Startup volume to be set.
  * @param  options: specific options passed to low layer function.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  Init         (uint32_t AudioFreq, 
                              uint32_t Volume, 
                              uint32_t options)
{
  uint32_t extclk = 0;
  
  /* Check if the low layer has already been initialized */
  if (Initialized == 0)
  {    
    /* Get the currently active synchronization method */
#ifdef USE_SYNCHRONIZATION    
 #if (AUDIO_SYNCHRO_CLKSYNTH_ENABLED == 1) 
    /* If the current method is using external clock, get its value */
    if (AUDIO_CORR_GetActiveMethod(DIRECTION_OUT) == SYNCH_EXTCLK)
    {
      extclk = AudioFreq * 256 * 4;
    }
    else
    {
      extclk = 0;
    }
#endif /* AUDIO_SYNCHRO_CLKSYNTH_ENABLED */

    /* Set the current synchro method */
    OutOptions = AUDIO_CORR_GetActiveMethod(DIRECTION_OUT);
#endif /* USE_SYNCHRONIZATION */
    
    /* Call low layer function */
    if (EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, Volume, AudioFreq, extclk) != 0)
    {
      AudioState = AUDIO_STATE_ERROR;
      return AUDIO_FAIL;
    }
    
    /* Set the Initialization flag to prevent reinitializing the interface again */
    Initialized = 1;
  }
  
  /* Update the Audio state machine */
  AudioState = AUDIO_STATE_ACTIVE;
    
  return AUDIO_OK;
}

/**
  * @brief  Free all resources used by low layer and stops audio-play function.
  * @param  options: options passed to low layer function.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  DeInit       (uint32_t options)
{
  /* Update the Audio state machine */
  AudioState = AUDIO_STATE_INACTIVE;
  Initialized = 0;
  
  return AUDIO_OK;
}

/**
  * @brief  Play, Stop, Pause or Resume current file.
  * @param  pbuf: address from which file shoud be played.
  * @param  size: size of the current buffer/file.
  * @param  cmd: command to be executed, can be AUDIO_CMD_PLAY , AUDIO_CMD_PAUSE, 
  *              AUDIO_CMD_RESUME or AUDIO_CMD_STOP.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  AudioCmd(uint8_t* pbuf, 
                         uint32_t size,
                         uint8_t cmd)
{
  /* Check the current state */
  if ((AudioState == AUDIO_STATE_INACTIVE) || (AudioState == AUDIO_STATE_ERROR))
  {
    AudioState = AUDIO_STATE_ERROR;
    return AUDIO_FAIL;
  }
  
  switch (cmd)
  {
    /* Process the PLAY command ----------------------------*/
  case AUDIO_CMD_PLAY:
    /* If current state is Active or Stopped */
    if ((AudioState == AUDIO_STATE_ACTIVE) || \
       (AudioState == AUDIO_STATE_STOPPED) || \
       (AudioState == AUDIO_STATE_PLAYING))
    {
      if (size == 0)
      {
        return AUDIO_OK;
      }
      Audio_MAL_Play((uint32_t)pbuf, (size/2));
      AudioState = AUDIO_STATE_PLAYING;
      return AUDIO_OK;
    }
    /* If current state is Paused */
    else if (AudioState == AUDIO_STATE_PAUSED)
    {
      if (size == 0)
      {
        return AUDIO_OK;
      }
      if (EVAL_AUDIO_PauseResume(AUDIO_RESUME, (uint32_t)pbuf, (size/2)) != 0)
      {
        AudioState = AUDIO_STATE_ERROR;
        return AUDIO_FAIL;
      }
      else
      {
        AudioState = AUDIO_STATE_PLAYING;
        return AUDIO_OK;
      } 
    } 
    else /* Not allowed command */
    {
      return AUDIO_FAIL;
    }
    
    /* Process the STOP command ----------------------------*/
  case AUDIO_CMD_STOP:
    if (AudioState != AUDIO_STATE_PLAYING)
    {
      /* Unsupported command */
      return AUDIO_FAIL;
    }
    else if (EVAL_AUDIO_Stop(CODEC_PDWN_SW) != 0)
    {
      AudioState = AUDIO_STATE_ERROR;
      return AUDIO_FAIL;
    }
    else
    {
      AudioState = AUDIO_STATE_STOPPED;
      return AUDIO_OK;
    }
  
    /* Process the PAUSE command ---------------------------*/
  case AUDIO_CMD_PAUSE:
    if (AudioState != AUDIO_STATE_PLAYING)
    {
      /* Unsupported command */
      return AUDIO_FAIL;
    }
    else if (EVAL_AUDIO_PauseResume(AUDIO_PAUSE, (uint32_t)pbuf, (size/2)) != 0)
    {
      AudioState = AUDIO_STATE_ERROR;
      return AUDIO_FAIL;
    }
    else
    {
      AudioState = AUDIO_STATE_PAUSED;
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
  if (EVAL_AUDIO_VolumeCtl(vol) != 0)
  {
    AudioState = AUDIO_STATE_ERROR;
    return AUDIO_FAIL;
  }
  
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
  if (EVAL_AUDIO_Mute(cmd) != 0)
  {
    AudioState = AUDIO_STATE_ERROR;
    return AUDIO_FAIL;
  }
  
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
  uint32_t freq = 48000;
  
  /* Decode the audio frequency */
  freq = (uint32_t)(*(uint8_t*)(cmd) | (*(uint8_t*)(cmd + 1) << 8) | (*(uint8_t*)(cmd + 2) << 16));

  /* Check ff the requested audio frequency is not supported */
  if ((freq > MAX_AUDIO_FREQ) || (freq < MIN_AUDIO_FREQ))
  {
    /* Set the default audio frequency */
    freq = DEFAULT_OUT_AUDIO_FREQ;
  }

#ifdef USE_SYNCHRONIZATION  
  /* If the external clock is used, reconfigure the clock synthesizer sub-system */
  AUDIO_CORR_Out_Update(freq, 0, 0, 0);
#endif /* USE_SYNCHRONIZATION */
  
  /* If the I2S peripheral is still not enabled, enable it */
  I2S_Cmd(CODEC_I2S, ENABLE);
    
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
  OutXferCpltCallback = Clbck;
}


/**
  * @brief  Manage the buffer transfer complete event.
  * @param  None
  * @retval None.
  */
void XferCplt(void)
{  
#if defined(USE_STM322xG_EVAL) || defined(USE_STM324xG_EVAL) || defined(USE_STM32F4XX_DISCOVERY)
  if (DMA_GetFlagStatus(AUDIO_MAL_DMA_STREAM, AUDIO_MAL_DMA_FLAG_TC) != RESET)
  {         
    /* Clear the Interrupt flag */
    DMA_ClearFlag(AUDIO_MAL_DMA_STREAM, AUDIO_MAL_DMA_FLAG_ALL);

#elif defined USE_STM3210C_EVAL
  if (DMA_GetFlagStatus(AUDIO_MAL_DMA_FLAG_TC) != RESET)
  {         
    /* Clear the Interrupt flag */
    DMA_ClearFlag(AUDIO_MAL_DMA_FLAG_ALL);
    
#endif /* USE_STM322xG_EVAL */  
    
    /* Call the higher layer function */
    if (OutXferCpltCallback != NULL)
    {
      OutXferCpltCallback(DIRECTION_OUT, 0, 0);
    }
  }
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
