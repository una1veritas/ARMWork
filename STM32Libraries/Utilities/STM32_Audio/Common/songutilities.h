/**
  ******************************************************************************
  * @file    songutilities.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file contains all the functions prototypes for memory management
  *          layer routines
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
#ifndef __SONG_UTILITIES_H
#define __SONG_UTILITIES_H

/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"
#include "ff.h"

/** @addtogroup STM32_audio_codecs
  * @{
  */

/** @addtogroup SongUtilities
  * @{
  */

/** @defgroup SongUtilities_Exported_Types
  * @{
  */
typedef struct {
  char * Title;
  char * Artist;
  char * Album;
  char * Year;
  char * Genre;
  uint32_t SamplingRate;
}TAGS_TypeDef;
/**
  * @}
  */

/** @defgroup SongUtilities_Exported_Constants
  * @{
  */
#define MAX_TAG_STRING_LENGTH          20

/**
  * @}
  */

/** @defgroup SongUtilities_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup SongUtilities_Exported_Functions
  * @{
  */
int8_t SongUtilities_MP3TagParser(uint8_t * buffer,  TAGS_TypeDef * pIDTAGS ,FIL * file);

#if 0 /* Not yet impelemented */
void SongUtilities_WMATagParser( TAGS_TypeDef *pIDTAG,FIL * file, uint8_t * pBuf );
#endif 

void StrCtrlLength(void* Dst, void* Src, uint32_t MaxLen);

#endif /* __SONG_UTILITIES_H */

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
