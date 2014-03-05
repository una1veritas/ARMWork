/**
  ******************************************************************************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Advanced_Example/main.h
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
 #include "stm3210c_eval_lcd.h"
 #include "stm3210c_eval_ioe_cpal.h"
#elif defined USE_STM32100E_EVAL
 #include "stm32100e_eval.h"
 #include "stm32100e_eval_lcd.h"
 #include "stm32100e_eval_ioe_cpal.h"
 #include "stm32_eval_i2c_tsensor_cpal.h"
#elif defined USE_STM322xG_EVAL
 #include "stm322xg_eval.h"
 #include "stm322xg_eval_lcd.h"
 #include "stm322xg_eval_ioe_cpal.h"
#elif defined USE_STM324xG_EVAL
 #include "stm324xg_eval.h"
 #include "stm324xg_eval_lcd.h"
 #include "stm324xg_eval_ioe_cpal.h" 
#endif

#include "stm32_eval_i2c_ee_cpal.h"
#include "demo.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
typedef enum   
{
  NO_OP               =   0x00,	
  ERASE_EEPROM_OP     =   0x01,	    
  GET_REPORT_OP       =   0x02,     
  SAVE_REPORT_OP      =   0x03,
  SHOW_TIME_OP        =   0x04,
  SHOW_TEMPERATURE_OP =   0x05  
}Operation_Typedef;

/* Exported constants --------------------------------------------------------*/


/* Define EEPROM address, size and page size */
#define EEPROM_ADDRESS        0xA0  /* M24C64 address is 0xA0: E0, E1 and E2 tied to ground */
#define EEPROM_SIZE           8192 /* bytes */
#define EEPROM_PAGE_SIZE      32   /* bytes */ 


#ifdef USE_STM32100E_EVAL
 #define sEE_DevStructure                sEE2_DevStructure 
#else
 #define sEE_DevStructure                sEE1_DevStructure 
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
