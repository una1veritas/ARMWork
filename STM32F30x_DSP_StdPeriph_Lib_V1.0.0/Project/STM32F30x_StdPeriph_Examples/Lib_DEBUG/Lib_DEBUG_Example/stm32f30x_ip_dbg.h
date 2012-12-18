/**
  ******************************************************************************
  * @file    Lib_DEBUG/Lib_DEBUG_Example/stm32f30x_ip_dbg.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
  * @brief   This file provides all STM32F30x peripherals pointers initialization.
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
#ifndef __STM32F30X_IP_DBG_H
#define __STM32F30X_IP_DBG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define PERIPH_DEBUG

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void IP_Debug(void);
 
#endif /* __STM32F30X_IP_DBG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
