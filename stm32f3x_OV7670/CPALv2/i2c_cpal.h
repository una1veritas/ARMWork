/**
  ******************************************************************************
  * @file    stm32373c_eval_i2c_tsensor_cpal.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
  * @brief   This file contains all the functions prototypes for the 
  *          stm32373c_eval_i2c_tsensor_cpal.c firmware driver.
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
#ifndef __I2C_CPAL_H
#define __I2C_CPAL_H

#ifdef __cplusplus
 extern "C" {
#endif
   
   
/* Includes ------------------------------------------------------------------*/
#include "stm32f30x_i2c_cpal.h"

#include "armcmx.h"

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  TSENSOR Status  
  */ 
typedef enum
{
  I2C_CPAL_OK = 0,
  I2C_CPAL_FAIL
} I2C_CPAL_Status_TypDef;

#define I2C_BUFFER_SIZE 256
	/*
typedef struct {
	uint8 rx[I2C_BUFFER_SIZE];
	uint8 tx[I2C_BUFFER_SIZE];
	uint16 rxcount, txcount;
//	boolean waitfinish;
//	__IO I2C_Mode mode;
//	__IO uint16_t position;
	__IO uint16_t count;
	__IO uint32 flagstatus;
	__IO uint32 watch;
} I2CBuffer;
	*/

typedef struct {
	I2C_TypeDef * I2Cx;
	GPIOPin scl, sda;
	
	CPAL_TransferTypeDef RXTransfer;
	CPAL_TransferTypeDef TXTransfer;
	uint8_t Buffer[I2C_BUFFER_SIZE];
	
	CPAL_InitTypeDef * pDevStructure;
	
	__IO uint32_t  Timeout; 
} i2c;


/* Exported constants --------------------------------------------------------*/
    
/*====================== CPAL Structure configuration ========================*/ 
/* Select I2C device (uncomment relative define) */

//#define I2C_CPAL_DevStructure I2C1_DevStructure
   
/*============== TIMING Configuration ==========================*/
/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated in case of the I2C Clock source is the SYSCLK = 72 MHz */
/* set TIMING to 0xC062121F to reach 100 KHz speed (Rise time = 640ns, Fall time = 20ns) */

#define I2C_CPAL_TIMING              0xC062121F


/*=================== Programming model Configuration ========================*/
/* Select interrupt programming model : By default DMA programming model is selected.
 To select interrupt programming model uncomment this define */
//#define LM75_IT


/* Maximum Timeout values for waiting until device is ready for communication.*/
   
#define I2C_CPAL_TIMEOUT        ((uint32_t)0x3FFFF)

/**
  * @brief  Internal register Memory
  */   

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

extern i2c i2c1;

void i2c_deinit(void);
void i2c_begin(void);
void i2c_init(void);

ErrorStatus i2c_getstatus(i2c * bus, uint8_t addr);

uint8_t i2c_read8(i2c * bus, uint8_t addr, uint8_t reg);
uint8_t i2c_write8(i2c * bus, uint8_t addr, uint8_t reg, uint8_t val);
uint16_t i2c_read16(i2c * bus, uint8_t addr, uint8_t reg);
uint8_t i2c_write16(i2c * bus, uint8_t addr, uint8_t reg, uint16_t val);

uint8_t i2c_transmit(i2c * bus, uint8_t addr, uint8_t * data, uint32_t numbyte);
boolean i2c_readfrom(i2c * bus, uint8_t addr, uint8_t * data, uint16_t len);
//boolean i2c_receive(i2c * I2Cbuf, uint8_t * data, uint16_t lim);
uint8_t i2c_ShutDown(FunctionalState NewState);


 
#ifdef __cplusplus
}
#endif

#endif /* __I2C_CPAL_H */
/**
  * @}
  */

/**
  * @}
  */

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
