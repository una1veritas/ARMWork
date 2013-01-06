/**
  ******************************************************************************
  * @file    stm324xg_clk_synthesizer_cs2200.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file includes the external clock synthesizer controller.
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
#include "stm324xg_clk_synthesizer_cs2200.h"
#include "stm32_audio_correction.h"

#if (AUDIO_SYNCHRO_CLKSYNTH_ENABLED == 1)
/** @addtogroup Audio_Utilities
* @{
*/

/** @defgroup CLK_SYNTHESIZER_CS2200
* @brief    This file includes the audio correction functional layer
* @{
*/ 

/** @defgroup CLK_SYNTHESIZER_CS2200_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup CLK_SYNTHESIZER_CS2200_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup CLK_SYNTHESIZER_CS2200_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup CLK_SYNTHESIZER_CS2200_Private_Variables
* @{
*/

uint32_t InputFreq     = 12000000; 
uint32_t OutputFreq    = 48000;
uint32_t Ratio         = 1;
uint32_t InitialRatio  = 1; 
static uint32_t ExtClkState  = STATE_NOTINITED;

DMA_InitTypeDef DMA_InitStructure;

extern SynchStruct_TypeDef SYNCH_EXTCLK_SynchStruct;

/* I2C Send/Receive Buffers */
uint32_t TxBuffer[CLK_SYNTH_I2C_MAX_SIZE];
uint32_t RxBuffer[CLK_SYNTH_I2C_MAX_SIZE];

/**
* @}
*/

/** @defgroup CLK_SYNTHESIZER_CS2200_Private_Constants
* @{
*/ 

/**
* @}
*/

/** @defgroup CLK_SYNTHESIZER_CS2200_Private_FunctionPrototypes
* @{
*/

/* Configuration functions */
static void    CLK_SYNTH_GPIO_Init            (void);
static void    CLK_SYNTH_GPIO_DeInit          (void);
static void    CLK_SYNTH_CtrlInterface_Init   (void);
static void    CLK_SYNTH_CtrlInterface_DeInit (void);
static void    CLK_SYNTH_RefClk_Init          (uint32_t Freq);
static void    CLK_SYNTH_RefClk_DeInit        (void);

/* Control port access functions */
static uint8_t CLK_SYNTH_WriteReg  (uint8_t Reg, uint8_t Value);
static uint8_t CLK_SYNTH_WriteBuff (uint8_t* pBuf, uint32_t Size);
static void    CLK_SYNTH_DMAConfig (void);

#ifdef VERIFY_WRITTENDATA
static uint8_t CLK_SYNTH_ReadReg   (uint8_t Reg);
#endif /* VERIFY_WRITTENDATA */

/**
* @}
*/ 


/** @defgroup CLK_SYNTHESIZER_CS2200_Private_Functions
* @{
*/ 

/**
  * @brief  CLK_SYNTH_Init 
  *         Initialize the external clock interfaces.
  * @param  OutFreq: Target output frequency in Hz
  * @param  InFreq: Input frequency in Hz (In this driver input frequency is fixed to 
  *         the value defined by I2S_EXT_REF_CLK in stm324xg_clk_synthesizer_cs2200.h)
  * @param  InitCtrlPort: State of the control port. Can be 0: control port
  *         not initialized, or !0 meaning control port already initialized.
  * @param  Opt: optional parameter. Not used for this driver.
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint32_t CLK_SYNTH_Init(uint32_t OutFreq, uint32_t InFreq, uint32_t InitCtrlPort, uint32_t Opt)
{ 
  uint32_t result = 0;

  /* Check if the component is already intialized */
  if (ExtClkState == STATE_INITED)
  {
    /* No initialization needed */
    return 0;
  }

  /* Configure the reference clock (from STM32 MCO output clock) */
  CLK_SYNTH_RefClk_Init(I2S_EXT_REF_CLK/*InFreq*/);
  
  /* Check if the control port is already initialized */
  if (InitCtrlPort == 0)
  {
    /* Initialize the control lines */ 
    CLK_SYNTH_GPIO_Init();
    
    /* Initialize the control port */
    CLK_SYNTH_CtrlInterface_Init();
  }
  
  /* Configure the DMA for the control port operations (I2C) */
  CLK_SYNTH_DMAConfig();
  
  /* Get the ratio to be programmed */
  InputFreq = InFreq;
  OutputFreq = OutFreq; 
  
  /* Check which frequency group should be configured (48KHz group and 44.1 group are not compatible) */
  if (OutFreq == (12288000 * 4))
  {
    /* Ratio calculated for Reference clock input = 40MHz  and Output frequency = 12.288MHz */
    Ratio = DEFAULT_RATIO_12_288 + SYNCH_EXTCLK_SynchStruct.pSynchOpt[0]; 
  }
  else if (OutFreq == (11289600 * 4))
  {
    /* Ratio calculated for Reference clock input = 40MHz  and Output frequency = 11.2896MHz */
    Ratio = DEFAULT_RATIO_11_2896 + SYNCH_EXTCLK_SynchStruct.pSynchOpt[0]; 
  }
  
  /* Keep the initial value of the Ratio for future modifications */
  InitialRatio = Ratio;
  
  /* Freeze the configuration */
  result += CLK_SYNTH_WriteReg(CS_GLOBAL_CFG, 0x08); 
  
  /* Auxilary output disabled */
  result += CLK_SYNTH_WriteReg(CS_DEVICE_CTRL, 0x02);
  
  /* No division on Ratio */
  result += CLK_SYNTH_WriteReg(CS_DEVICE_CFG1, 0x00);
  
#ifdef USE_I2C_DMA_TC_IT
  /* Write the Ratio bytes */
  result += CLK_SYNTH_WriteReg(CS_RATIO_3, (uint8_t)(((Ratio & 0xFF000000) >> 24)));
  result += CLK_SYNTH_WriteReg(CS_RATIO_2, (uint8_t)(((Ratio & 0xFF0000) >> 16)));
  result += CLK_SYNTH_WriteReg(CS_RATIO_1, (uint8_t)(((Ratio & 0xFF00)>> 8)));
  result += CLK_SYNTH_WriteReg(CS_RATIO_0, (uint8_t)(Ratio & 0xFF));
#else   
  *(uint8_t*)((uint8_t*)TxBuffer) = (CS_RATIO_3 | 0x80);
  *(uint8_t*)((uint8_t*)TxBuffer + 1) = (uint8_t)(((Ratio & 0xFF000000) >> 24));
  *(uint8_t*)((uint8_t*)TxBuffer + 2) = (uint8_t)(((Ratio & 0xFF0000) >> 16));
  *(uint8_t*)((uint8_t*)TxBuffer + 3) = (uint8_t)(((Ratio & 0xFF00)>> 8));
  *(uint8_t*)((uint8_t*)TxBuffer + 4) = (uint8_t)(Ratio & 0xFF);
  CLK_SYNTH_WriteBuff((uint8_t *)TxBuffer, 5);
#endif /* USE_I2C_DMA_TC_IT */
  
  /* Reference clock input divider set to 2 (input = 24 MHz) */
  result += CLK_SYNTH_WriteReg(CS_FUNCT_CFG1, 0x08);
  
  /* Unfreeze: start clocking */
  result += CLK_SYNTH_WriteReg(CS_GLOBAL_CFG, 0x01);
  
  /* Check if the injected error is not 0 */
  if (SYNCH_EXTCLK_SynchStruct.pSynchOpt[0])
  {
    /* Update the correction period and step (more than 100ppm error is injected) */
    SYNCH_EXTCLK_SynchStruct.SynchPeriod = SYNCH_EXTCLK_SynchStruct.pSynchOpt[1];
    SYNCH_EXTCLK_SynchStruct.SynchStep = SYNCH_EXTCLK_SynchStruct.pSynchOpt[2];
  }
  
  return result;
}

/**
  * @brief  CLK_SYNTH_DeInit 
  *         Free all resources used by audio correction mechanism.
  * @param  speed : device speed
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint32_t CLK_SYNTH_DeInit(void)
{
  /* Check if the component is already deinitialized */
  if (ExtClkState == STATE_NOTINITED)
  {/* No need for deinitialization */
    return 0;
  }

  /* Free used IOs */
  CLK_SYNTH_GPIO_DeInit();

  /* Free the control port */
  CLK_SYNTH_CtrlInterface_DeInit();
  
  /* Free the reference clock source */
  CLK_SYNTH_RefClk_DeInit();
  
  /* Reset state to not initialized */
  ExtClkState = STATE_NOTINITED;  
  
  return 0;
}

/**
  * @brief  CLK_SYNTH_AdjustPpm
  *         Update the current output frequency with ppm step.
  * @param  Ppm: Number of ppm to be increased or decreased.
  * @param  Direction: Direction of the update. EXTCLK_DIR_UP to 
  *         increase the frequency or EXTCLK_DIR_DOWN to decrease 
  *         the frequency.
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint32_t CLK_SYNTH_AdjustPpm(uint32_t Ppm, uint32_t Direction)
{ 
  uint32_t result = 0;
  
  /* Check the update direction */
  if (Direction == EXTCLK_DIR_UP)
  {
    /* Increase the output frequency */
    Ratio += (Ratio * Ppm) / 1000000;
  }
  else /* if (Direction == EXTCLK_DIR_DOWN) */
  {
    /* Decrease the output frequency */
    Ratio -= (Ratio * Ppm) / 1000000;
  } 
  
  *(uint8_t*)((uint8_t*)TxBuffer) = (CS_RATIO_3 | 0x80);
  *(uint8_t*)((uint8_t*)TxBuffer + 1) = (uint8_t)(((Ratio & 0xFF000000) >> 24));
  *(uint8_t*)((uint8_t*)TxBuffer + 2) = (uint8_t)(((Ratio & 0xFF0000) >> 16));
  *(uint8_t*)((uint8_t*)TxBuffer + 3) = (uint8_t)(((Ratio & 0xFF00)>> 8));
  *(uint8_t*)((uint8_t*)TxBuffer + 4) = (uint8_t)(Ratio & 0xFF);
  CLK_SYNTH_WriteBuff((uint8_t *)TxBuffer, 5);
  
  return result;
}

/** @brief  CLK_SYNTH_UpdateFreq
  *         Update the current output frequency with new frequency.
  * @param  Freq: New frequency to be configured.
  * @param  opt1: optional parameter. Not used for this driver.
  * @param  opt2: optional parameter. Not used for this driver.
  * @param  opt3: optional parameter. Not used for this driver.
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint32_t CLK_SYNTH_UpdateFreq(uint32_t Freq, uint32_t opt1, uint32_t opt2, uint32_t opt3)
{ 
  uint32_t result = 0;
  
  /* Get the ratio to be programmed */
  OutputFreq = Freq; 
  
  /* Check which frequency group should be configured (48KHz group and 44.1 group are not compatible) */
  if (OutputFreq == (12288000 * 4))
  {
    /* Ratio calculated for Reference clock input = 40MHz  and Output frequency = 12.288MHz */
    Ratio = DEFAULT_RATIO_12_288 + SYNCH_EXTCLK_SynchStruct.pSynchOpt[0]; 
  }
  else if (OutputFreq == (11289600 * 4))
  {
    /* Ratio calculated for Reference clock input = 40MHz  and Output frequency = 11.2896MHz */
    Ratio = DEFAULT_RATIO_11_2896 + SYNCH_EXTCLK_SynchStruct.pSynchOpt[0]; 
  }
  else if (OutputFreq == (11264000 * 4))
  {
    /* Ratio calculated for Reference clock input = 40MHz  and Output frequency = 11.2896MHz */
    Ratio = DEFAULT_RATIO_11_264 + SYNCH_EXTCLK_SynchStruct.pSynchOpt[0];     
  }
  
  /* Update the initial value of the Ratio for future modifications */
  InitialRatio = Ratio;

  /* Update the ratio value in sequence write */
  *(uint8_t*)((uint8_t*)TxBuffer) = (CS_RATIO_3 | 0x80);
  *(uint8_t*)((uint8_t*)TxBuffer + 1) = (uint8_t)(((Ratio & 0xFF000000) >> 24));
  *(uint8_t*)((uint8_t*)TxBuffer + 2) = (uint8_t)(((Ratio & 0xFF0000) >> 16));
  *(uint8_t*)((uint8_t*)TxBuffer + 3) = (uint8_t)(((Ratio & 0xFF00)>> 8));
  *(uint8_t*)((uint8_t*)TxBuffer + 4) = (uint8_t)(Ratio & 0xFF);
  CLK_SYNTH_WriteBuff((uint8_t *)TxBuffer, 5);
  
  return result;
}


/**
  * @brief  CLK_SYNTH_WriteReg
  *         Writes a value to a register.
  * @param  Reg: register address to be written.
  * @param  Value: value to be written to the reigster.
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint8_t CLK_SYNTH_WriteReg(uint8_t Reg, uint8_t Value)
{
  uint32_t result = 0;
  uint32_t Timeout = 0;
  
  /* Disable the interrupts mechanism to prevent the I2C communication from corruption */
  __disable_irq();

  /*!< While the bus is busy */
  Timeout = CLK_SYNTH_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(CLK_SYNTH_I2C, I2C_FLAG_BUSY))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /* Start the config sequence */
  I2C_GenerateSTART(CLK_SYNTH_I2C, ENABLE);

  /* Test on EV5 and clear it */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CLK_SYNTH_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(CLK_SYNTH_I2C, CLK_SYNTH_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CLK_SYNTH_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }

  /* Transmit the first address for write operation */
  I2C_SendData(CLK_SYNTH_I2C, Reg);

  /* Test on EV8 and clear it */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CLK_SYNTH_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /* Prepare the register value to be sent */
  I2C_SendData(CLK_SYNTH_I2C, Value);
  
  /*!< Wait till all data have been physically transferred on the bus */
  Timeout = CLK_SYNTH_LONG_TIMEOUT;
  while(!I2C_GetFlagStatus(CLK_SYNTH_I2C, I2C_FLAG_BTF))
  {
    if((Timeout--) == 0) CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /* End the configuration sequence */
  I2C_GenerateSTOP(CLK_SYNTH_I2C, ENABLE);  
  
#ifdef VERIFY_WRITTENDATA
  /* Verify that the data has been correctly written */  
  result = (ReadRegister(RegisterAddr) == RegisterValue)? 0:1;
#endif /* VERIFY_WRITTENDATA */
  
  /* Re-enable the interrupt mechanism */
  __enable_irq();
  
  /* Return the verifying value: 0 (Passed) or 1 (Failed) */
  return result;
}

#ifdef VERIFY_WRITTENDATA
/**
  * @brief  CLK_SYNTH_ReadReg
  *         Reads the value of a register.
  * @param  Reg: register address to be written.
  * @retval Value read from the register (or 0xFF if opearation failed).
  */
uint8_t CLK_SYNTH_ReadReg(uint8_t Reg)
{
  uint32_t result = 0;
  uint32_t Timeout = 0;
  
  __disable_irq();

  /*!< While the bus is busy */
  Timeout = CLK_SYNTH_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(CLK_SYNTH_I2C, I2C_FLAG_BUSY))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /* Start the config sequence */
  I2C_GenerateSTART(CLK_SYNTH_I2C, ENABLE);

  /* Test on EV5 and clear it */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CLK_SYNTH_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(CLK_SYNTH_I2C, CLK_SYNTH_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  Timeout = CLK_SYNTH_LONG_TIMEOUT;
  while (!I2C_CheckEvent(CLK_SYNTH_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }

  /* Transmit the register address to be read */
  I2C_SendData(CLK_SYNTH_I2C, Reg);

  /* Test on EV8 and clear it */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(CLK_SYNTH_I2C, I2C_FLAG_BTF) == RESET)
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /*!< Send STRAT condition a second time */  
  I2C_GenerateSTART(CLK_SYNTH_I2C, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(CLK_SYNTH_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  } 
  
  /*!< Send Codec address for read */
  I2C_Send7bitAddress(CLK_SYNTH_I2C, CLK_SYNTH_ADDRESS, I2C_Direction_Receiver);  
  
  /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(CLK_SYNTH_I2C, I2C_FLAG_ADDR) == RESET)
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }     
  
  /*!< Disable Acknowledgement */
  I2C_AcknowledgeConfig(CLK_SYNTH_I2C, DISABLE);   
  
  /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
  (void)CLK_SYNTH_I2C->SR2;
  
  /*!< Send STOP Condition */
  I2C_GenerateSTOP(CLK_SYNTH_I2C, ENABLE);
  
  /* Wait for the byte to be received */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(CLK_SYNTH_I2C, I2C_FLAG_RXNE) == RESET)
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /*!< Read the byte received from the Codec */
  result = I2C_ReceiveData(CLK_SYNTH_I2C);
  
  /* Wait to make sure that STOP flag has been cleared */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while(CLK_SYNTH_I2C->CR1 & I2C_CR1_STOP)
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }  
  
  /*!< Re-Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(CLK_SYNTH_I2C, ENABLE);  
  
  /* Clear AF flag for next communication */
  I2C_ClearFlag(CLK_SYNTH_I2C, I2C_FLAG_AF); 

  /* Re-enable the interrupt mechanism */
  __enable_irq();
  
  /* Return the byte read from Codec */
  return result;
}
#endif /* VERIFY_WRITTENDATA */

/**
  * @brief  CLK_SYNTH_WriteBuff
  *         Writes a value to a register.
  * @param  pBuf: Buffer from which data should be sent.
  * @param  Size: Number of bytes to be sent.
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint8_t CLK_SYNTH_WriteBuff(uint8_t* pBuf, uint32_t Size)
{
  uint32_t result = 0;
  uint32_t Timeout = 0;
  
  /* Init DMA for Transmission */
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pBuf;
  DMA_InitStructure.DMA_BufferSize = Size;
  DMA_Init(CLK_SYNTH_I2C_DMA_STREAM_TX, &DMA_InitStructure);   
  
  /*!< While the bus is busy */
  Timeout = CLK_SYNTH_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(CLK_SYNTH_I2C, I2C_FLAG_BUSY))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /* Start the config sequence */
  I2C_GenerateSTART(CLK_SYNTH_I2C, ENABLE);

  /* Test on EV5 and clear it */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CLK_SYNTH_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(CLK_SYNTH_I2C, CLK_SYNTH_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CLK_SYNTH_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();
  }
  
  /* Enable DMA TX Channel */
  DMA_Cmd(CLK_SYNTH_I2C_DMA_STREAM_TX, ENABLE);  
  
  /* I2Cx DMA Enable */
  I2C_DMACmd(CLK_SYNTH_I2C, ENABLE);

#ifndef USE_I2C_DMA_TC_IT  
  /* Wait until DMA Transfer Complete or time out */
  Timeout = CLK_SYNTH_LONG_TIMEOUT;
  while (DMA_GetFlagStatus(CLK_SYNTH_I2C_DMA_STREAM_TX, CLK_SYNTH_I2C_TX_DMA_TCFLAG) == RESET)
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();   
  }
  
  /* I2C DMA Disable */
  I2C_DMACmd(CLK_SYNTH_I2C, DISABLE);
  
  ///  /* Wait until BTF Flag is set before generating STOP or time out */
  //  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  //  while (!I2C_GetFlagStatus(CLK_SYNTH_I2C, I2C_FLAG_BTF)) 
  //  {
  //    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();   
  //  }
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(CLK_SYNTH_I2C, ENABLE);

  /* Disable DMA TX Channel */
  DMA_Cmd(CLK_SYNTH_I2C_DMA_STREAM_TX, DISABLE);
  
  /* Wait until CLK_SYNTH_I2C_DMA_STREAM_TX disabled or time out */
  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
  while (DMA_GetCmdStatus(CLK_SYNTH_I2C_DMA_STREAM_TX)!= DISABLE)
  {
    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();   
  }
  
  /* Clear any pending flag on Tx Stream  */
  DMA_ClearFlag(CLK_SYNTH_I2C_DMA_STREAM_TX, CLK_SYNTH_I2C_TX_DMA_TCFLAG | CLK_SYNTH_I2C_TX_DMA_FEIFLAG | CLK_SYNTH_I2C_TX_DMA_DMEIFLAG | \
                                       CLK_SYNTH_I2C_TX_DMA_TEIFLAG | CLK_SYNTH_I2C_TX_DMA_HTIFLAG);
#endif /* USE_I2C_DMA_TC_IT */  
  
  /* Return the verifying value: 0 (Passed) or 1 (Failed) */
  return result;
}

#ifdef USE_I2C_DMA_TC_IT
/**
  * @brief  This function handles CLK_SYNTH_I2C_DMA_TX_IRQHandler global interrupt request.
  * @param  None
  * @retval None
  */
void CLK_SYNTH_I2C_DMA_TX_IRQHandler(void)
{
  /* check if DMA Transfer Complete occurred */
  if (DMA_GetFlagStatus(CLK_SYNTH_I2C_DMA_STREAM_TX, CLK_SYNTH_I2C_TX_DMA_TCFLAG) == SET)
  {  
    /* I2C DMA Disable */
    I2C_DMACmd(CLK_SYNTH_I2C, DISABLE);
    
    ///  /* Wait until BTF Flag is set before generating STOP or time out */
    //  Timeout = CLK_SYNTH_FLAG_TIMEOUT;
    //  while (!I2C_GetFlagStatus(CLK_SYNTH_I2C, I2C_FLAG_BTF)) 
    //  {
    //    if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();   
    //  }
    
    /* Send STOP Condition */
    I2C_GenerateSTOP(CLK_SYNTH_I2C, ENABLE);
    
    /* Disable DMA TX Channel */
    DMA_Cmd(CLK_SYNTH_I2C_DMA_STREAM_TX, DISABLE);
    
    ///    /* Wait until CLK_SYNTH_I2C_DMA_STREAM_TX disabled or time out */
    //    Timeout = CLK_SYNTH_FLAG_TIMEOUT;
    //    while (DMA_GetCmdStatus(CLK_SYNTH_I2C_DMA_STREAM_TX)!= DISABLE)
    //    {
    //      if((Timeout--) == 0) return CLK_SYNTH_TIMEOUT_UserCallback();   
    //    }
    
    /* Clear any pending flag on Tx Stream  */
    DMA_ClearFlag(CLK_SYNTH_I2C_DMA_STREAM_TX, CLK_SYNTH_I2C_TX_DMA_TCFLAG | CLK_SYNTH_I2C_TX_DMA_FEIFLAG | CLK_SYNTH_I2C_TX_DMA_DMEIFLAG | \
      CLK_SYNTH_I2C_TX_DMA_TEIFLAG | CLK_SYNTH_I2C_TX_DMA_HTIFLAG);
  }
}
#endif /* USE_I2C_DMA_TC_IT */

/**
  * @brief  CLK_SYNTH_DMAConfig
  *         Initialize the I2C DMA channels.
  * @param  None.
  * @retval None.
  */
static void CLK_SYNTH_DMAConfig(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Clear any pending flag on Tx Stream  */
  DMA_ClearFlag(CLK_SYNTH_I2C_DMA_STREAM_TX, CLK_SYNTH_I2C_TX_DMA_TCFLAG | CLK_SYNTH_I2C_TX_DMA_FEIFLAG | CLK_SYNTH_I2C_TX_DMA_DMEIFLAG | \
    CLK_SYNTH_I2C_TX_DMA_TEIFLAG | CLK_SYNTH_I2C_TX_DMA_HTIFLAG);
  
  /* Clear any pending flag on Rx Stream  */
  DMA_ClearFlag(CLK_SYNTH_I2C_DMA_STREAM_RX, CLK_SYNTH_I2C_RX_DMA_TCFLAG | CLK_SYNTH_I2C_RX_DMA_FEIFLAG | CLK_SYNTH_I2C_RX_DMA_DMEIFLAG | \
    CLK_SYNTH_I2C_RX_DMA_TEIFLAG | CLK_SYNTH_I2C_RX_DMA_HTIFLAG);
  
  /* Disable the I2C Tx DMA stream */
  DMA_Cmd(CLK_SYNTH_I2C_DMA_STREAM_TX, DISABLE);
  /* Configure the DMA stream for the I2C peripheral TX direction */
  DMA_DeInit(CLK_SYNTH_I2C_DMA_STREAM_TX);
  
  /* Disable the I2C Rx DMA stream */
  DMA_Cmd(CLK_SYNTH_I2C_DMA_STREAM_RX, DISABLE);
  /* Configure the DMA stream for the I2C peripheral RX direction */
  DMA_DeInit(CLK_SYNTH_I2C_DMA_STREAM_RX);
  
  /* Initialize the DMA Structure common fields */
  DMA_InitStructure.DMA_Channel = CLK_SYNTH_I2C_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = CLK_SYNTH_I2C_DR_ADDRESS;       
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  
  /* Init DMA for Transmission */
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TxBuffer;
  DMA_InitStructure.DMA_BufferSize = 0xFFFF;
  DMA_Init(CLK_SYNTH_I2C_DMA_STREAM_TX, &DMA_InitStructure);   
  
  /* Init DMA for Reception */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)RxBuffer;
  DMA_InitStructure.DMA_BufferSize = 0xFFFF;
  DMA_Init(CLK_SYNTH_I2C_DMA_STREAM_RX, &DMA_InitStructure); 

#ifdef USE_I2C_DMA_TC_IT  
  /* Enable the interrupt of I2C DMA TX Stream */
  DMA_ITConfig(CLK_SYNTH_I2C_DMA_STREAM_TX, DMA_IT_TC, ENABLE);
  
  /* Enable the I2C DMA TX Stream Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = CLK_SYNTH_I2C_DMA_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = STM32_CLK_SYNTH_PRE_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = STM32_CLK_SYNTH_SUB_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
#endif /* USE_I2C_DMA_TC_IT */
}

/**
  * @brief  CLK_SYNTH_GPIO_Init
  *         Initialize the I2C IOs.
  * @param  None.
  * @retval None.
  */
static void CLK_SYNTH_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable I2S and I2C GPIO clocks */
  RCC_AHB1PeriphClockCmd(CLK_SYNTH_I2C_GPIO_CLOCK, ENABLE);

  /* CLK_SYNTH_I2C SCL and SDA pins configuration -------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = CLK_SYNTH_I2C_SCL_PIN | CLK_SYNTH_I2C_SDA_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CLK_SYNTH_I2C_GPIO, &GPIO_InitStructure);     
  /* Connect pins to Periph */
  GPIO_PinAFConfig(CLK_SYNTH_I2C_GPIO, CLK_SYNTH_I2C_SCL_PINSRC, CLK_SYNTH_I2C_GPIO_AF);  
  GPIO_PinAFConfig(CLK_SYNTH_I2C_GPIO, CLK_SYNTH_I2C_SDA_PINSRC, CLK_SYNTH_I2C_GPIO_AF);
}

/**
  * @brief Restores the IOs used by the Audio Codec interface to their default 
  *        state
  * @param  None.
  * @retval None.
  */
static void CLK_SYNTH_GPIO_DeInit(void)
{
  /* I2C IOs are not deinitialized because it may be used by other modules */    
}

/**
  * @brief Initializes the control interface (I2C).
  * @param  None.
  * @retval None.
  */
static void CLK_SYNTH_CtrlInterface_Init(void)
{
  I2C_InitTypeDef I2C_InitStructure;
  
  /* Enable the CLK_SYNTH_I2C peripheral clock */
  RCC_APB1PeriphClockCmd(CLK_SYNTH_I2C_CLK, ENABLE);
  
  /* If the I2C peripheral is already enabled, don't reconfigure it */
  if (CLK_SYNTH_I2C->CR1 & I2C_CR1_PE)
  {   
    /* CLK_SYNTH_I2C peripheral configuration */
    I2C_DeInit(CLK_SYNTH_I2C);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x33;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
    
    /* Enable the I2C peripheral */
    I2C_Cmd(CLK_SYNTH_I2C, ENABLE);  
    I2C_Init(CLK_SYNTH_I2C, &I2C_InitStructure);
  }
}

/**
  * @brief Restore the control interface to its default state.
  *        This function doesn't de-initialize the I2C because the I2C peripheral
  *        may be used by other modules.
  * @param  None.
  * @retval None.
  */
static void CLK_SYNTH_CtrlInterface_DeInit(void)
{
  /* Disable the I2C peripheral */ /* This step is not done here because 
     the I2C interface can be used by other modules */
  /* I2C_DeInit(CLK_SYNTH_I2C); */
}

/**
  * @brief CLK_SYNTH_RefClk_Init
  *        Initialize the reference clock source.
  * @param  Freq: Reference clock source to be programmed in Hz.
  * @retval None.
  */
static void CLK_SYNTH_RefClk_Init(uint32_t Freq)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable the SPI clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  /* Configure the I2S_CKIN as Input */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  /* Set the configuration of PC9 pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOC, &GPIO_InitStructure);   
  
  /* Connect the AF pin to the I2S IPs */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_SPI2);
  
  /* Set the External clock as source for the I2S IP */
  RCC->CFGR |= (uint32_t)RCC_CFGR_I2SSRC;  
  
  /* Enable the GPIOA peripheral */ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* Configure MCO1 pin(PA8) in alternate function */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* Connect the AF pin to the MCO function */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO); 
  
  /* PLL clock selected to output on MCO1 pin(PA8)*/
  RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_3); /* Reference Clk= 120MHz/3 = 40MHz */
}

/**
  * @brief  CLK_SYNTH_RefClk_DeInit
  *         Restore the reference clock source state to default.
  * @param  None.
  * @retval None.
  */
static void CLK_SYNTH_RefClk_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Restore MCO1 pin(PA8) to default state */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* HSI clock selected to output on MCO1 pin(PA8)*/
  RCC_MCO1Config(RCC_MCO1Source_HSI, RCC_MCO1Div_1);
}

#endif /* AUDIO_SYNCHRO_CLKSYNTH_ENABLED */
/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
