/**
  ******************************************************************************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Advanced_Example/main.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    17-June-2011
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32_eval.h"

#ifdef USE_STM3210C_EVAL
 #include "stm3210c_eval_lcd.h"
 #include "stm3210c_eval_ioe_cpal.h"
#elif defined USE_STM32100E_EVAL
 #include "stm32100e_eval_lcd.h"
 #include "stm32100e_eval_ioe_cpal.h"
 #include "stm32_eval_i2c_tsensor_cpal.h"
#elif defined USE_STM322xG_EVAL
 #include "stm322xg_eval_lcd.h"
 #include "stm322xg_eval_ioe_cpal.h"
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

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
