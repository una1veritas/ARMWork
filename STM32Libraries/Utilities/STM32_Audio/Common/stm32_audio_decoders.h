/**
  ******************************************************************************
  * @file    stm32_audio_decoders.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file contains all the functions prototypes for Decoders layer
  *          routines
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_ADUDIO_DECODERS_H_
#define __STM32_ADUDIO_DECODERS_H_
/* Includes ------------------------------------------------------------------*/

#include "audio_app_conf.h"
#include "songutilities.h"

#define DEFAULT_PACKET_SZE                             (uint32_t)(10*2*2)

#if defined(__MP3_DECODER__) || defined(__MP3_ENCODER__)
 /* mp3 processing routines */
 #include "mp3process.h"
 #ifndef MP3_PACKET_SZE
  #define MP3_PACKET_SZE                                (uint32_t)(1152*2*2) /* 1152 stereo samples of 16-bits */
 #endif
#endif /* __MP3_DECODER__ */

#if defined(__WAV_DECODER__) || defined(__WAV_ENCODER__)
 /* wav processing routines */
 #include "wavprocess.h"
 #ifndef WAV_PACKET_SZE
  #define WAV_PACKET_SZE                                (uint32_t)(2048*2*2) /* 2048 stereo samples of 16-bits */
 #endif
#endif /* __WAV_DECODER__ */

#ifdef __WMA_DECODER__
 /* wma processing routines */
 #include "wmaprocess.h"
 #ifndef WMA_PACKET_SZE
  #define WMA_PACKET_SZE                                (uint32_t)(3072*2*2) /* Max decoded frame size = 2048 samples x 1.5 */
 #endif
#endif /* __WMA_DECODER__ */

/**
 * Extend the list ;)
 * ......
 */

/** @addtogroup STM32_audio_codecs
  * @{
  */

/** @addtogroup Decoders
  * @{
  */

/** @defgroup Decoders_Exported_Types
  * @{
  */
/* Decoders structure */
typedef struct {
  uint32_t      (*DecoderInit)            (uint8_t* pHeader, 
                                           fnReadCallback_TypeDef* pReadCallback, 
                                           fnSetPositionCallback_TypeDef* pSetPosCallback);
  uint32_t      (*DecoderDeInit)          (void);
  uint32_t      (*DecoderStopMode)        (void);
  uint32_t      (*Decoder_StreamForward)  (void);
  uint32_t      (*Decoder_StreamBackward) (void);
  uint32_t      (*Decoder_DecodeData)     (__IO int16_t *,uint32_t,void*); /* Data Processing routine to provide output PCM stream */
  uint32_t      (*Decoder_ReadTags)       (TAGS_TypeDef*, FIL *file);
  uint32_t      (*Decoder_GetSamplingRate)(void);
  uint32_t      (*Decoder_GetStreamLength)(uint32_t fLength);
  uint32_t      (*Decoder_GetElapsedTime) (uint32_t CurrPos);
  uint32_t      (*Decoder_VBRDetected)    (void);
  uint32_t      PacketSize;
} Decoder_TypeDef;

/* Encoders structure */
typedef struct {
  uint32_t      (*EncoderInit)            (uint32_t Freq, uint8_t* Param);
  uint32_t      (*EncoderDeInit)          (void);
  uint32_t      (*EncoderStopMode)        (void);
  uint32_t      (*Encoder_EncodeData)     (int8_t *pBufIn, int8_t *pBufOut, uint32_t* nEnc, void* pOpt);
  uint32_t      (*Encoder_SetTags)        (TAGS_TypeDef*, void *file);
  uint32_t      (*Encoder_GetSampleRate)  (void);
  uint32_t      (*Encoder_GetStreamLength)(void);
  uint32_t      (*Encoder_GetElapsedTime) (void);
  uint32_t      PacketSize;
} Encoder_TypeDef;

/**
  * @}
  */

/** @defgroup Decoders_Exported_Constants
  * @{
  */

/**
  * @}
  */

/** @defgroup Decoders_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup Decoders_Exported_Functions
  * @{
  */

int8_t Decoders_SelectDecoder(Decoder_TypeDef* pDecoderStruct, int8_t ch);
int8_t Encoders_SelectEncoder(Encoder_TypeDef* pEncoderStruct, int8_t ch);

#endif /*__STM32_ADUDIO_DECODERS_H_*/

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
