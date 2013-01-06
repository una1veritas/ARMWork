/**
  ******************************************************************************
  * @file    stm32_audio_decoders.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file includes Decoders layer routines
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
#include "stm32_audio_decoders.h"

/** @addtogroup STM32_audio_codecs
  * @{
  */

/** @defgroup Decoders
  * @brief Interface between the player and decoders modules
  * @{
  */

/** @defgroup Decoders_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup Decoders_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup Decoders_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup Decoders_Private_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup Decoders_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */

/** @defgroup Decoders_Private_Functions
  * @{
  */

/**
  * @brief  This function initialize decoder struct depending on file extension.
  * @param  last char of file extension
  * @retval 0 if success, 1 if unhandled format.
  */
int8_t Decoders_SelectDecoder(Decoder_TypeDef* pDecoderStruct, int8_t ch)
{
  switch (ch)
  {
#ifdef __MP3_DECODER__
      /* .mp3 files (.mp'3') */
    case ('3'):
      pDecoderStruct->DecoderInit = Mp3Process_DecoderInit;
      pDecoderStruct->DecoderDeInit = Mp3Process_DecoderDeInit;
      pDecoderStruct->DecoderStopMode = Mp3Process_StopMode;
      pDecoderStruct->Decoder_StreamForward = Mp3Process_StreamForward;
      pDecoderStruct->Decoder_StreamBackward = Mp3Process_StreamBackward;
      pDecoderStruct->Decoder_DecodeData = Mp3Process_DecodeData;
      pDecoderStruct->Decoder_ReadTags = Mp3Process_RetrieveIDTAGS;
      pDecoderStruct->Decoder_GetSamplingRate = Mp3Process_GetSamplingRate;
      pDecoderStruct->Decoder_GetStreamLength = Mp3Process_GetStreamLenght;
      pDecoderStruct->Decoder_GetElapsedTime = Mp3Process_GetElapsedTime;
      pDecoderStruct->Decoder_VBRDetected = Mp3Process_VBRDetected;
      pDecoderStruct->PacketSize = MP3_PACKET_SZE;
      return 0;
#endif /* __MP3_DECODER__ */
      
#ifdef __WMA_DECODER__
      /* .wma files (.wm'{a,A}') */
    case ('a'):
    case ('A'):
      pDecoderStruct->DecoderInit = WmaProcess_DecoderInit;
      pDecoderStruct->DecoderDeInit = WmaProcess_DecoderDeInit;
      pDecoderStruct->DecoderStopMode = WmaProcess_StopMode;
      pDecoderStruct->Decoder_StreamForward = WmaProcess_Forward;
      pDecoderStruct->Decoder_StreamBackward = WmaProcess_Backward;
      pDecoderStruct->Decoder_DecodeData = WmaProcess_DecodeData;
      pDecoderStruct->Decoder_ReadTags = NULL; /* WmaProcess_RetrieveIDTAGS; */
      pDecoderStruct->Decoder_GetSamplingRate = WmaProcess_GetSamplingRate;
      pDecoderStruct->Decoder_GetStreamLength = WmaProcess_GetStreamLenght;
      pDecoderStruct->Decoder_GetElapsedTime = WmaProcess_GetElapsedTime;
      pDecoderStruct->Decoder_VBRDetected = NULL;
      pDecoderStruct->PacketSize = WMA_PACKET_SZE;
      return 0;
#endif /* __WMA_DECODER__ */
      
#ifdef __WAV_DECODER__
      /* .wav files (.wa'{v,V}') */
    case ('v'):
    case ('V'):
      pDecoderStruct->DecoderInit = WavProcess_DecoderInit;
      pDecoderStruct->DecoderDeInit = NULL;
      pDecoderStruct->DecoderStopMode = WavProcess_StopMode;
      pDecoderStruct->Decoder_StreamForward = WavProcess_StreamForward;
      pDecoderStruct->Decoder_StreamBackward = WavProcess_StreamBackward;
      pDecoderStruct->Decoder_DecodeData = WavProcess_DecodeData;
      pDecoderStruct->Decoder_ReadTags = NULL;
      pDecoderStruct->Decoder_GetSamplingRate = WavProcess_GetSamplingRate;
      pDecoderStruct->Decoder_GetStreamLength = WavProcess_GetStreamLenght;
      pDecoderStruct->Decoder_GetElapsedTime = WavProcess_GetElapsedTime;
      pDecoderStruct->Decoder_VBRDetected = NULL;
      pDecoderStruct->PacketSize = WAV_PACKET_SZE;
      return 0;
#endif /* __WAV_DECODER__ */
      
      /* Unsupported file format */
    default:
      pDecoderStruct->DecoderInit = NULL;
      pDecoderStruct->DecoderDeInit = NULL;
      pDecoderStruct->DecoderStopMode = NULL;
      pDecoderStruct->Decoder_StreamForward = NULL;
      pDecoderStruct->Decoder_StreamBackward = NULL;
      pDecoderStruct->Decoder_GetStreamLength = NULL;
      pDecoderStruct->Decoder_GetElapsedTime = NULL;
      pDecoderStruct->Decoder_ReadTags = NULL;
      pDecoderStruct->Decoder_DecodeData = NULL;
      pDecoderStruct->Decoder_GetSamplingRate = NULL;
      pDecoderStruct->Decoder_VBRDetected = NULL;
      pDecoderStruct->PacketSize = DEFAULT_PACKET_SZE;
      return 1;
  }
}

/**
  * @brief  This function initialize Encoder struct depending on file extension.
  * @param  last char of file extension
  * @retval 0 if success, 1 if unhandled format.
  */
int8_t Encoders_SelectEncoder(Encoder_TypeDef* pEncoderStruct, int8_t ch)
{
  switch (ch)
  {
#ifdef __MP3_ENCODER__
      /* .mp3 files (.mp'3') */
    case ('3'):
      pEncoderStruct->EncoderInit = Mp3Process_EncoderInit;
      pEncoderStruct->EncoderDeInit = Mp3Process_EncoderDeInit;
      pEncoderStruct->EncoderStopMode = Mp3Process_EncoderStopMode;
      pEncoderStruct->Encoder_EncodeData = Mp3Process_EncodeData;
      pEncoderStruct->Encoder_SetTags = Mp3Process_SetTags;
      pEncoderStruct->Encoder_GetSampleRate = Mp3Process_EncGetSampleRate;
      pEncoderStruct->Encoder_GetStreamLength = Mp3Process_EncGetStreamLength;
      pEncoderStruct->Encoder_GetElapsedTime = Mp3Process_EncGetElapsedTime;
      pEncoderStruct->PacketSize = MP3_PACKET_SZE;
      return 0;
#endif /* __MP3_ENCODER__ */

#ifdef __SPEEX_ENCODER__
      /* .spx files (.sp'{x,X}') */
    case ('x'):
    case ('X'):
      pEncoderStruct->EncoderInit = SpeexProcess_EncoderInit;
      pEncoderStruct->EncoderDeInit = SpeexProcess_EncoderDeInit;
      pEncoderStruct->EncoderStopMode = NULL;
      pEncoderStruct->Encoder_EncodeData = SpeexProcess_EncodeData;
      pEncoderStruct->Encoder_SetTags = NULL;
      pEncoderStruct->Encoder_GetSampleRate = NULL;
      pEncoderStruct->Encoder_GetStreamLength = NULL;
      pEncoderStruct->Encoder_GetElapsedTime = NULL;
      pEncoderStruct->PacketSize = SPEEX_PACKET_SZE;
      return 0;
#endif /* __SPEEX_ENCODER__ */
      
#ifdef __WAV_ENCODER__
      /* .wav files (.wa'{v,V}') */
    case ('v'):
    case ('V'):
      pEncoderStruct->EncoderInit = WavProcess_EncoderInit;
      pEncoderStruct->EncoderDeInit = WavProcess_EncoderDeInit;
      pEncoderStruct->EncoderStopMode = NULL;
      pEncoderStruct->Encoder_EncodeData = WavProcess_EncodeData;
      pEncoderStruct->Encoder_SetTags = NULL;
      pEncoderStruct->Encoder_GetSampleRate = WavProcess_EncGetSampleRate;
      pEncoderStruct->Encoder_GetStreamLength = WavProcess_EncGetStreamLength;
      pEncoderStruct->Encoder_GetElapsedTime = WavProcess_EncGetElapsedTime;
      pEncoderStruct->PacketSize = WAV_PACKET_SZE;
      return 0;
#endif /* __WAV_ENCODER__ */
      
      /* Unsupported file format */
    default:
      pEncoderStruct->EncoderInit = NULL;
      pEncoderStruct->EncoderDeInit = NULL;
      pEncoderStruct->EncoderStopMode = NULL;
      pEncoderStruct->Encoder_EncodeData = NULL;
      pEncoderStruct->Encoder_SetTags = NULL;
      pEncoderStruct->Encoder_GetSampleRate = NULL;
      pEncoderStruct->Encoder_GetStreamLength = NULL;
      pEncoderStruct->Encoder_GetElapsedTime = NULL;
      pEncoderStruct->PacketSize = DEFAULT_PACKET_SZE;
      return 1;
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
