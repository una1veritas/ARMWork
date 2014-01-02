/**
  ******************************************************************************
  * @file    stm32373c_eval_i2c_tsensor_cpal.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
  * @brief   This file provides a set of functions needed to manage the I2C LM75 
  *          temperature sensor mounted on STM32373C-EVAL board . 
  *          It implements a high level communication layer for read and write 
  *          from/to this sensor. 
  *            
  *          @note This file is a modified version of stm32373c_eval_i2c_tsensor.c driver;
  *                I2C CPAL library drivers are used instead of the Standard Peripherals
  *                I2C driver.
  *                 
  *
  *     +-----------------------------------------------------------------+
  *     |                        Pin assignment                           |                 
  *     +---------------------------------------+-----------+-------------+
  *     |  STM32 I2C Pins                       |   STLM75  |   Pin       |
  *     +---------------------------------------+-----------+-------------+
  *     | LM75_I2C_SDA_PIN/ SDA                 |   SDA     |    1        |
  *     | LM75_I2C_SCL_PIN/ SCL                 |   SCL     |    2        |
  *     |                                       |   OS/INT  |    3        |
  *     | .                                     |   GND     |    4  (0V)  |
  *     | .                                     |   GND     |    5  (0V)  |
  *     | .                                     |   GND     |    6  (0V)  |
  *     | .                                     |   GND     |    7  (0V)  |
  *     | .                                     |   VDD     |    8  (3.3V)|
  *     +---------------------------------------+-----------+-------------+
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
#include "i2c_cpal.h"
#if defined(STM32F10X_MD )
#include "cpal_i2c.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#ifdef ORIGINAL

CPAL_TransferTypeDef  I2C_CPAL_RXTransfer = { 
                        /* Initialize RX Transfer structure */
                        pNULL,
                        0,
                        0,
                        0};

CPAL_TransferTypeDef  I2C_CPAL_TXTransfer = {
                        /* Initialize TX Transfer structure */
                        pNULL,
                        0,
                        0,
                        0};

uint8_t I2C_CPAL_Buffer[I2C_BUFFER_SIZE] = { 0x00 }; 

//extern CPAL_InitTypeDef Wire_Structure;

__IO uint32_t  I2C_CPAL_Timeout = I2C_CPAL_TIMEOUT; 

#endif

i2c i2c1 = { I2C1 };

/* Private function prototypes -----------------------------------------------*/

static void I2C_CPAL_StructInit(void);
static uint32_t I2C_CPAL_Status (i2c * i2cBus, uint8_t addr);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure the LM75_I2C.
  * @param  None
  * @retval None
  */
void i2c_begin(void)
{
	i2c1.RXTransfer.pbBuffer = pNULL;
	i2c1.RXTransfer.wAddr1 = 0;
	i2c1.RXTransfer.wAddr2 = 0;
	i2c1.RXTransfer.wNumData = 0;
	i2c1.TXTransfer.pbBuffer = pNULL;
	i2c1.TXTransfer.wAddr1 = 0;
	i2c1.TXTransfer.wAddr2 = 0;
	i2c1.TXTransfer.wNumData = 0;
	i2c1.Timeout = I2C_CPAL_TIMEOUT;
	i2c1.pDevStructure = & I2C1_DevStructure;
	
  I2C_CPAL_StructInit ();
  i2c_init();
}

/**
  * @brief  Deinitialize the LM75_I2C.
  * @param  None
  * @retval None
  */
void i2c_deinit(void)
{
    /* Initialize CPAL peripheral */
    CPAL_I2C_DeInit(i2c1.pDevStructure); //CPAL_I2C_DeInit(&I2C_CPAL_DevStructure);
}

/**
  * @brief  Initializes the LM75_I2C.
  * @param  None
  * @retval None
  */
void i2c_init(void)
{
  /* Initialize CPAL peripheral */
  CPAL_I2C_Init(i2c1.pDevStructure); //CPAL_I2C_Init(&I2C_CPAL_DevStructure);
}

/**
  * @brief  Initializes the LM75_I2C.
  * @param  None
  * @retval None
  */
static void I2C_CPAL_StructInit(void)
{
  /* Set CPAL structure parameters to their default values */  
  CPAL_I2C_StructInit(i2c1.pDevStructure); //CPAL_I2C_StructInit(&I2C_CPAL_DevStructure);
  
#if defined(STM32F30X)
  i2c1.pDevStructure->pCPAL_I2C_Struct->I2C_Timing = I2C_CPAL_TIMING;   /* Set I2C clock speed */
#endif
  			 
#ifdef I2C_IT
  i2c1.pDevStructure->CPAL_ProgModel = CPAL_PROGMODEL_INTERRUPT; /* Select Interrupt programming model */
#else
  i2c1.pDevStructure->CPAL_ProgModel = CPAL_PROGMODEL_DMA;   /* Select DMA programming model */
#endif /* LM75_IT */    
  i2c1.pDevStructure->wCPAL_Options  = 0;  /* Disable all options */
  /* point to CPAL_TransferTypeDef structure */
  i2c1.pDevStructure->pCPAL_TransferTx = &i2c1.TXTransfer;
  i2c1.pDevStructure->pCPAL_TransferRx = &i2c1.RXTransfer;
}

static uint32_t I2C_CPAL_Status (i2c * i2cBus, uint8_t addr)
{
  i2cBus->pDevStructure->pCPAL_TransferTx = &i2c1.TXTransfer;     
  i2cBus->pDevStructure->pCPAL_TransferTx->wAddr1 = (uint32_t) addr;
  
  return CPAL_I2C_IsDeviceReady(i2cBus->pDevStructure);
}

/**
  * @brief  Checks the LM75 status.
  * @param  None
  * @retval ErrorStatus: LM75 Status (ERROR or SUCCESS).
  */
ErrorStatus i2c_getstatus(i2c * bus, uint8_t addr)
{  
  /* Test if LM75 is ready */
  while ((I2C_CPAL_Status(bus, addr) == CPAL_FAIL) && i2c1.Timeout)  
  {
    bus->Timeout--;
  }
  
  /* If LM75 is not responding return ERROR */
  if (bus->Timeout == 0)
  {
    return ERROR;
  }
  
  /* In other case return SUCCESS */
  return SUCCESS;  
}
/**
  * @brief  Read the specified register from the LM75.
  * @param  RegName: specifies the LM75 register to be read.
  *              This member can be one of the following values:  
  *                  - LM75_REG_TEMP: temperature register
  *                  - LM75_REG_TOS: Over-limit temperature register
  *                  - LM75_REG_THYS: Hysteresis temperature register
  * @retval LM75 register value.
  */
uint8_t i2c_read8(i2c * bus, uint8_t addr, uint8_t reg)
{ 
	uint8_t resp;
  bus->Buffer[0] = 0;
  bus->Buffer[1] = 0;
  
  /* Disable all options */
  bus->pDevStructure->wCPAL_Options = 0;

  /* point to CPAL_TransferTypeDef structure */
  bus->pDevStructure->pCPAL_TransferRx = &bus->RXTransfer;
  
  /* Configure transfer parameters */  
  bus->pDevStructure->pCPAL_TransferRx->wNumData = 2;
  bus->pDevStructure->pCPAL_TransferRx->pbBuffer = bus->Buffer ;
  bus->pDevStructure->pCPAL_TransferRx->wAddr1   = (uint32_t)addr;
  bus->pDevStructure->pCPAL_TransferRx->wAddr2   = (uint32_t)reg;
  
  /* Read Operation */
  if(CPAL_I2C_Read(bus->pDevStructure) == CPAL_PASS)
  {
    while ((bus->pDevStructure->CPAL_State != CPAL_STATE_READY) && (bus->pDevStructure->CPAL_State != CPAL_STATE_ERROR) )
    { }
  }

	resp = bus->Buffer[0];
  /* return a Reg value */
  return resp;  
}

uint16_t i2c_read16(i2c * bus, uint8_t addr, uint8_t reg)
{     
  bus->Buffer[0] = 0;
  bus->Buffer[1] = 0;
  
  /* Disable all options */
  bus->pDevStructure->wCPAL_Options = 0;

  /* point to CPAL_TransferTypeDef structure */
  bus->pDevStructure->pCPAL_TransferRx = &bus->RXTransfer;
  
  /* Configure transfer parameters */  
  bus->pDevStructure->pCPAL_TransferRx->wNumData = 2;
  bus->pDevStructure->pCPAL_TransferRx->pbBuffer = bus->Buffer ;
  bus->pDevStructure->pCPAL_TransferRx->wAddr1   = (uint32_t)addr;
  bus->pDevStructure->pCPAL_TransferRx->wAddr2   = (uint32_t)reg;
  
  /* Read Operation */
  if(CPAL_I2C_Read(bus->pDevStructure) == CPAL_PASS)
  {
    while ((bus->pDevStructure->CPAL_State != CPAL_STATE_READY) && (bus->pDevStructure->CPAL_State != CPAL_STATE_ERROR) )
    { }
  }
  
  /* return a Reg value */
  return ((uint16_t)bus->Buffer[0]) << 8 | bus->Buffer[1];  
}

/**
  * @brief  Write to the specified register of the LM75.
  * @param  RegName: specifies the LM75 register to be written.
  *              This member can be one of the following values:    
  *                  - LM75_REG_TOS: Over-limit temperature register
  *                  - LM75_REG_THYS: Hysteresis temperature register
  * @param  RegValue: value to be written to LM75 register.  
  * @retval None
  */
uint8_t i2c_write8(i2c * bus, uint8_t addr, uint8_t reg, uint8_t val)
{   
  bus->Buffer[0] = (uint8_t)val;
  bus->Buffer[1] = 0;
     
  /* Disable all options */
  bus->pDevStructure->wCPAL_Options = 0;
  
  /* point to CPAL_TransferTypeDef structure */
  bus->pDevStructure->pCPAL_TransferTx = &bus->TXTransfer;
  
  /* Configure transfer parameters */  
  bus->pDevStructure->pCPAL_TransferTx->wNumData = 1;
  bus->pDevStructure->pCPAL_TransferTx->pbBuffer = bus->Buffer ;
  bus->pDevStructure->pCPAL_TransferTx->wAddr1   = (uint32_t) addr;
  bus->pDevStructure->pCPAL_TransferTx->wAddr2   = (uint32_t) reg;
  
  /* Write Operation */
  if(CPAL_I2C_Write(bus->pDevStructure) == CPAL_PASS)
  {
    while ((bus->pDevStructure->CPAL_State != CPAL_STATE_READY) && (bus->pDevStructure->CPAL_State != CPAL_STATE_ERROR) )
    { }
    
    if (bus->pDevStructure->CPAL_State == CPAL_STATE_ERROR)
    {
      return I2C_CPAL_FAIL;
    }
  }
  else
  {
    return I2C_CPAL_FAIL;
  }
  
  return I2C_CPAL_OK;
}

uint8_t i2c_readfrom(i2c * bus, uint8_t addr, uint8_t * data, uint16_t len)
{    
  uint16_t tmp = 0;
  
  memset(bus->Buffer, 0, len);
  
  /* Disable all options */
  bus->pDevStructure->wCPAL_Options = 0;

  /* point to CPAL_TransferTypeDef structure */
  bus->pDevStructure->pCPAL_TransferRx = &bus->RXTransfer;
  
  /* Configure transfer parameters */  
  bus->pDevStructure->pCPAL_TransferRx->wNumData = 2;
  bus->pDevStructure->pCPAL_TransferRx->pbBuffer = bus->Buffer ;
  bus->pDevStructure->pCPAL_TransferRx->wAddr1   = (uint32_t) addr;
  bus->pDevStructure->pCPAL_TransferRx->wAddr2   = (uint32_t) 0;
  
  /* Read Operation */
  if(CPAL_I2C_Read(bus->pDevStructure) == CPAL_PASS)
  {
    while ((bus->pDevStructure->CPAL_State != CPAL_STATE_READY) && (bus->pDevStructure->CPAL_State != CPAL_STATE_ERROR) )
    { }
  }
  
  /* Store received data */
	
	memcpy(data, bus->Buffer, len);
  /* Return Temperature value */
  return tmp;
}

/**
  * @brief  Write to the configuration register of the LM75.
  * @param  RegValue: specifies the value to be written to LM75 configuration 
  *         register.
  * @retval None
  */
uint8_t i2c_transmit(i2c * bus, uint8_t addr, uint8_t * data, uint32_t n)
{   
  memcpy(bus->Buffer, data+1, n-1);
    
  /* Disable all options */
  bus->pDevStructure->wCPAL_Options = 0;
  
  /* point to CPAL_TransferTypeDef structure */
  bus->pDevStructure->pCPAL_TransferTx = &i2c1.TXTransfer;
  
  /* Configure transfer parameters */  
  bus->pDevStructure->pCPAL_TransferTx->wNumData = n-1;
  bus->pDevStructure->pCPAL_TransferTx->pbBuffer = bus->Buffer ;
  bus->pDevStructure->pCPAL_TransferTx->wAddr1   = (uint32_t) addr;
  bus->pDevStructure->pCPAL_TransferTx->wAddr2   = (uint32_t) data[0];
  
  /* Write Operation */
  if(CPAL_I2C_Write(i2c1.pDevStructure) == CPAL_PASS)
  {
    while ((bus->pDevStructure->CPAL_State != CPAL_STATE_READY) && (bus->pDevStructure->CPAL_State != CPAL_STATE_ERROR) )
    { }
    
    if (bus->pDevStructure->CPAL_State == CPAL_STATE_ERROR)
    {
      return I2C_CPAL_FAIL;
    }
  }
  else
  {
    return I2C_CPAL_FAIL;
  }
  
  return I2C_CPAL_OK;
  
}

boolean i2c_request(i2c * I2Cbus, uint8_t addr, uint8_t * data, uint16_t len) {
	
  memset(I2Cbus->Buffer, 0, len);
  
  /* Disable all options */
  I2Cbus->pDevStructure->wCPAL_Options = 0;

  /* point to CPAL_TransferTypeDef structure */
  I2Cbus->pDevStructure->pCPAL_TransferRx = &I2Cbus->RXTransfer;
  
  /* Configure transfer parameters */  
  I2Cbus->pDevStructure->pCPAL_TransferRx->wNumData = len;
  I2Cbus->pDevStructure->pCPAL_TransferRx->pbBuffer = I2Cbus->Buffer ;
  I2Cbus->pDevStructure->pCPAL_TransferRx->wAddr1   = (uint32_t)addr;
  I2Cbus->pDevStructure->pCPAL_TransferRx->wAddr2   = (uint32_t)data[0];

	return true;  
}

boolean i2c_receive(i2c * I2Cbus, uint8_t * data, uint16_t len) {

	I2Cbus->pDevStructure->pCPAL_TransferRx->wNumData = len;
//  I2Cbus->pDevStructure->wCPAL_Options = CPAL_OPT_I2C_ERRIT_DISABLE;
	
  /* Read Operation */
  if(CPAL_I2C_Read(I2Cbus->pDevStructure) == CPAL_PASS)
  {
    while ((I2Cbus->pDevStructure->CPAL_State != CPAL_STATE_READY) && (I2Cbus->pDevStructure->CPAL_State != CPAL_STATE_ERROR) )
    { }
  }
  
	memcpy(data, I2Cbus->Buffer, len);
  /* return a Reg value */
  return len - I2Cbus->pDevStructure->pCPAL_TransferRx->wNumData;  
	
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
