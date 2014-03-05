/**
  ******************************************************************************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Two_Boards/main..h 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    21-December-2012
  * @brief   Header file for main.c.
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "cpal_i2c.h"

#ifdef USE_STM322xG_EVAL 
 #include "stm32f2xx.h"
 #include "stm322xg_eval.h"
 #include "stm322xg_eval_lcd.h"
#elif defined USE_STM324xG_EVAL 
 #include "stm32f4xx.h"
 #include "stm324xg_eval.h"
 #include "stm324xg_eval_lcd.h"
#elif defined USE_STM3210C_EVAL 
 #include "stm32f10x.h"
 #include "stm3210c_eval.h"
 #include "stm3210c_eval_lcd.h"
#elif defined USE_STM32100E_EVAL
 #include "stm32f10x.h"
 #include "stm32100e_eval.h"
 #include "stm32100e_eval_lcd.h"
#elif defined USE_STM32L152_EVAL
 #include "stm32l1xx.h"
 #include "stm32l152_eval.h"
 #include "stm32l152_eval_lcd.h"
#elif defined USE_STM32L152D_EVAL
 #include "stm32l1xx.h"
 #include "stm32l152d_eval.h"
 #include "stm32l152d_eval_lcd.h"
#endif

#include <stdio.h>

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* This define select used I2Cx device for communication */

#if defined (USE_STM32L152_EVAL) || defined (USE_STM32L152D_EVAL) || defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL) || defined (USE_STM324xG_EVAL)
 #define I2C_DevStructure        I2C1_DevStructure
#endif

#ifdef USE_STM32100E_EVAL
 #define I2C_DevStructure        I2C2_DevStructure
#endif

#if defined (USE_STM32L152_EVAL) || defined (USE_STM32L152D_EVAL)
 #define BUTTON_TAMPER 6
#endif

#define countof(a) (sizeof(a) / sizeof(*(a)))

#define MESSAGE1                (uint8_t*)"---STM32 CPAL Lib---"
#define MESSAGE2                (uint8_t*)"I2C 2xBoards Example"
#define MESSAGE3                (uint8_t*)"-------READY--------"

#define MEASSAGE_EMPTY          (uint8_t*)"                    "

#define MAX_BUFF_SIZE           200
#define BUFFER_SIZE             (countof(tStateSignal)-1)

#define OWN_ADDRESS             0x74

/* To use the I2C at 400 KHz (in fast mode), the PCLK1 frequency (I2C peripheral
   input clock) must be a multiple of 10 MHz */
#define I2C_SPEED               300000  /* Speed in Hz */

#define ACTION_NONE             0xFF
#define ACTION_DISABLED         0xFD
#define ACTION_PENDING          0xFE
#define ACTION_PERIODIC         0xFC

#define STATE_OFF               0
#define STATE_ON                1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
