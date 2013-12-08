/**
  ******************************************************************************
  * @file    I2C/I2C_WakeUpFromStop/main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
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
#include "stm32f37x.h"
#include "stm32373c_eval.h"
#include "stm32f37x_i2c_cpal.h"
#include "stm32373c_eval_lcd.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {FAILED = 1, PASSED = !FAILED} TestStatus;

/* Exported constants --------------------------------------------------------*/

/* Uncomment the line below if you will use the I2C peripheral as a Master*/
//#define I2C_MASTER  
/* Uncomment the line below if you will use the I2C peripheral as a Slave*/
#define I2C_SLAVE

/* Configure I2C TIMING by defining the value of TIMINGR Register */
#define MASTER_I2C_TIMING                      0x10420F13
#define SLAVE_I2C_TIMING                       0x10420F13

/* Define the used Devices */
#define MASTERSTRUCTURE           I2C2_DevStructure  
#define SLAVESTRUCTURE            I2C2_DevStructure

/* Configure the OA1 address of the slave device */
#define OWNADDRESS                      ((uint8_t)0xCD)


#define BUFFSIZE                       (countof(BufferTX)-1)

#define MEASSAGE_EMPTY                  (uint8_t*)"                    "
#define MESSAGE1                        (uint8_t*)"---STM32 CPAL Lib---"
#define MESSAGE2                        (uint8_t*)"I2C WakeUp From Stop"
#define MESSAGE3                        (uint8_t*)"-------READY--------"
#define MESSAGE4                        (uint8_t*)" Press Key to start "
#define MESSAGE5                        (uint8_t*)"       Tansfer      "
#define MESSAGE6                        (uint8_t*)" Transfer Complete  "
#define MESSAGE7                        (uint8_t*)" Entering Stop Mode "
#define MESSAGE8                        (uint8_t*)" Device in Stop Mode"  
#define MESSAGE9                        (uint8_t*)"  WakeUp From Stop  "
#define MESSAGE10                       (uint8_t*)" Slave Not yet Ready"
#define MESSAGE11                       (uint8_t*)"   Error occurred   "
#define MESSAGE12                       (uint8_t*)"  Timeout occurred  "

/* Exported macro ------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
