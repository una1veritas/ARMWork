/**
  ******************************************************************************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Basic_EEPROM/main.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    21-December-2012
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
#ifdef USE_STM3210C_EVAL
 #include "stm3210c_eval.h"
#elif defined USE_STM32100E_EVAL
 #include "stm32100e_eval.h"
#elif defined USE_STM322xG_EVAL
 #include "stm322xg_eval.h"
#elif defined USE_STM324xG_EVAL
 #include "stm324xg_eval.h"
#elif defined USE_STM32L152_EVAL
 #include "stm32l152_eval.h"
#elif defined USE_STM32L152D_EVAL
 #include "stm32l152d_eval.h"
#endif

#include "stm32_eval_i2c_ee_cpal.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Define EEPROM address, size and page size */
#ifdef USE_STM32L152D_EVAL
 #define EEPROM_ADDRESS        0xA0
 #define EEPROM_PAGE_SIZE      4
#else
 #define EEPROM_ADDRESS        0xA0
 #define EEPROM_PAGE_SIZE      32 
#endif /* USE_STM32L152D_EVAL */ 


#if defined (USE_STM32L152_EVAL) || defined (USE_STM32L152D_EVAL) || defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL) || defined (USE_STM324xG_EVAL)
 #define sEE_DevStructure sEE1_DevStructure 
#elif defined USE_STM32100E_EVAL
 #define sEE_DevStructure sEE2_DevStructure 
#endif /* USE_STM32L152_EVAL || USE_STM3210C_EVAL || USE_STM322xG_EVAL || USE_STM324xG_EVAL */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
