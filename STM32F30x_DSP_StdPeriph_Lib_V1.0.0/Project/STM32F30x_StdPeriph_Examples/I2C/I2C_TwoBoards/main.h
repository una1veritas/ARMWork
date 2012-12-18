/**
  ******************************************************************************
  * @file    I2C/I2C_TwoBoards/main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
  * @brief   Header for main.c module
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
#include "stm32f30x_i2c_cpal.h"
#include "stm32f30x.h"
#include "stm32303c_eval.h"
#include "stm32303c_eval_lcd.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* This define select used I2Cx device for communication */

#define I2C_DevStructure        I2C2_DevStructure


#define countof(a) (sizeof(a) / sizeof(*(a)))

#define MESSAGE1                (uint8_t*)"---STM32 CPAL Lib---"
#define MESSAGE2                (uint8_t*)"I2C 2xBoards Example"
#define MESSAGE3                (uint8_t*)"-------READY--------"

#define MEASSAGE_EMPTY          (uint8_t*)"                    "

#define MAX_BUFF_SIZE           200
#define BUFFER_SIZE             (countof(tStateSignal)-1)

#define OWN_ADDRESS             0x74

#define I2C_TIMING              0x40110405 

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
