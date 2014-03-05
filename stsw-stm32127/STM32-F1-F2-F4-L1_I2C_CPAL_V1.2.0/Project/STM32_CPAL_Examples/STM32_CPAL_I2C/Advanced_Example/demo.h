/**
  ******************************************************************************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Advanced_Example/demo.h 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    21-December-2012
  * @brief   This file contains all the functions prototypes for the
  *          menu navigation firmware driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEMO_H
#define __DEMO_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define TIMEOUT_DEFAULT               ((uint32_t)0xFFFFFFFF)
#define TIMEOUT_DISPLAY               ((uint32_t)0x00000020)

#define  NOKEY    0
#define  SEL      1
#define  RIGHT    2
#define  LEFT     3
#define  UP       4
#define  DOWN     5
#define  KEY      6

/* private variables --------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void Demo_Init(void);
void Display_Menu(void);
void Erase_EEPROM(void);
void Get_Report(void);
void Save_Report(void);
void Show_Temperature(void);
void Show_Time(void);
void Clear_IOEPendingIT(void);

#endif /* __DEMO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
