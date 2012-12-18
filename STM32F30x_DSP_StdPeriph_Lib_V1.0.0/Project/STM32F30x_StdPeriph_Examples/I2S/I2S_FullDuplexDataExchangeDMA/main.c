/**
  ******************************************************************************
  * @file    I2S/I2S_FullDuplexDataExchangeDMA/main.c  
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
  * @brief   Main program body
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
#include "main.h"

/** @addtogroup STM32F30x_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2S_FullDuplexDataExchangeDMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/
#define SPI_DR_Address     0x40003C0C /* SPI3 DR address*/
#define I2SEXT_DR_Address  0x4000400C /* I2S3ext DR address */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint16_t I2S_Buffer_Tx[32] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
                                    0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
                                    0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
                                    0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                                    0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
                                    0x3D3E, 0x3F40};
__IO uint8_t TxStatus= 0, RxStatus=0;
uint16_t I2S_Buffer_Rx[32]={0}, I2Sext_Buffer_Rx[32]={0};
TestStatus TransferStatus = FAILED;
/* Private function prototypes -----------------------------------------------*/
static TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);
static void I2S_Config(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f30x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f30x.c file
     */
     
  /* I2S peripheral Configuration */
  I2S_Config();
  
  /* Initialize the LEDs */
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED1);

#ifdef I2S_MASTER_FULL_DUPLEX 
  /* Configure the Key button */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /* Wait to Key button pressed */
  while (STM_EVAL_PBGetState(BUTTON_KEY) != RESET)
  {
  } 
  
  /* Enable the DMA_CHANNEL_RX transfer complete */   
  DMA_ITConfig(DMA_CHANNEL_RX, DMA_IT_TC, ENABLE);
  
  /* Enable the DMA_CHANNEL_TX transfer complete */  
  DMA_ITConfig(DMA_CHANNEL_TX, DMA_IT_TC, ENABLE);
  
  /* Enable the DMA channel Rx */
  DMA_Cmd(DMA_CHANNEL_RX, ENABLE);
  
  /* Enable the DMA channel Tx */
  DMA_Cmd(DMA_CHANNEL_TX, ENABLE); 
  
  /* Enable the I2Sext RX DMA request */
  SPI_I2S_DMACmd(I2Sext, SPI_I2S_DMAReq_Rx, ENABLE);  
  
  /* Enable the I2S TX DMA request */
  SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Tx, ENABLE);
  
  /* Enable the SPI Master peripheral */
  I2S_Cmd(SPI, ENABLE);
  
  /* Enable the I2Sext peripheral */
  I2S_Cmd(I2Sext, ENABLE);

  /* Wait for successful transfer complete */
  while((TxStatus == 0)||(RxStatus == 0))
  {
  }
  
  /* Disable the DMA_CHANNEL_TX transfer complete interrupt */  
  DMA_ITConfig(DMA_CHANNEL_TX, DMA_IT_TC, DISABLE);
  
  /* Disable the DMA_CHANNEL_RX transfer complete interrupt */  
  DMA_ITConfig(DMA_CHANNEL_RX, DMA_IT_TC, DISABLE);

  /* Disable the I2S Master peripheral */
  I2S_Cmd(SPI, DISABLE);
  
   /* Disable the I2Sext Master peripheral */
  I2S_Cmd(I2Sext, DISABLE);
  
  /* Disable DMA Channel 1*/
  DMA_Cmd(DMA_CHANNEL_RX, DISABLE);
  
   /* Disable DMA Channel TX */
  DMA_Cmd(DMA_CHANNEL_TX, DISABLE);
  
  /* Disables the I2S DMA interface */
  SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Tx,DISABLE);
  
  /* Disables the I2Sext DMA interface */
  SPI_I2S_DMACmd(I2Sext, SPI_I2S_DMAReq_Rx,DISABLE);
      
  /* Check if the data transmitted from Master Board and received by
  Slave Board are the same */
  TransferStatus = Buffercmp(I2Sext_Buffer_Rx, (uint16_t*)I2S_Buffer_Tx, 32);
  
  if (TransferStatus == PASSED) /* Successful transfer */ 
  {
    /* Green Led On */
    STM_EVAL_LEDOn(LED1);
    STM_EVAL_LEDOff(LED3);
  }
  else /* unsuccessful transfer */ 
  {
    /* Red Led On */
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOff(LED1);
  }
#endif /* I2S_MASTER_FULL_DUPLEX */
  
#ifdef I2S_SLAVE_FULL_DUPLEX  
  /* Enable the DMA_CHANNEL_RX transfer complete */      
  DMA_ITConfig(DMA_CHANNEL_RX, DMA_IT_TC, ENABLE);
  
  /* Enable the DMA_CHANNEL_TX transfer complete */  
  DMA_ITConfig(DMA_CHANNEL_TX, DMA_IT_TC, ENABLE);
  
  /* Enable the DMA channel1 Rx */
  DMA_Cmd(DMA_CHANNEL_RX, ENABLE);
  /* Enable the DMA channel2  Tx */
  DMA_Cmd(DMA_CHANNEL_TX, ENABLE);
  
  /* Enable the I2S RX DMA request */
  SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Rx, ENABLE);  
  
  /* Enable the I2S TX DMA request */
  SPI_I2S_DMACmd(I2Sext, SPI_I2S_DMAReq_Tx, ENABLE);   
  
  /* Enable the I2S Slave peripheral */
  I2S_Cmd(SPI, ENABLE);
    
  /* Enable the I2Sext Slave peripheral */
  I2S_Cmd(I2Sext, ENABLE);
 
  /* Wait for successful transfer complete*/
  while((TxStatus == 0)||(RxStatus == 0));
    
  /* Disable the I2S Slave peripheral */
  I2S_Cmd(SPI, DISABLE);
  
  /* Disable the I2Sext Slave peripheral */
  I2S_Cmd(I2Sext, DISABLE);
  
  /* Disable the DMA_CHANNEL_RX transfer complete */  
  DMA_ITConfig(DMA_CHANNEL_RX, DMA_IT_TC, DISABLE);
  
  /* Disable the DMA_CHANNEL_TX transfer complete */  
  DMA_ITConfig(DMA_CHANNEL_TX, DMA_IT_TC, DISABLE);
  
  /* Disable DMA channel 1 Rx */
  DMA_Cmd(DMA_CHANNEL_RX, DISABLE);
  
  /* Disable DMA channel 2 Tx */
  DMA_Cmd(DMA_CHANNEL_TX, DISABLE);
  
  /* Disable the I2S RX DMA request */
  SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Rx, DISABLE);
  
  /* Disable the I2S TX DMA request */
  SPI_I2S_DMACmd(I2Sext, SPI_I2S_DMAReq_Tx, DISABLE);

  /* Check if the data transmitted from Master Board and received by
  Slave Board are the same */
  TransferStatus = Buffercmp(I2S_Buffer_Rx, (uint16_t*)I2S_Buffer_Tx, 32);
  
  if (TransferStatus == PASSED) /* Successful transfer */ 
  {
    /* Green Led On */
    STM_EVAL_LEDOn(LED1);
    STM_EVAL_LEDOff(LED3);
  }
  else /* unsuccessful transfer */ 
  {
    /* Red Led On */
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOff(LED1);
  }
#endif /* I2S_SLAVE_FULL_DUPLEX */

  /* Infinite loop */
  while (1)
  {}
}

/**
  * @brief  Configures I2S peripheral.
  * @param  None
  * @retval None
  */
static void I2S_Config(void)
{
  I2S_InitTypeDef I2S_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure; 
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  /* Enable SPI APB clocks */
  RCC_APB1PeriphClockCmd(RCC_APBPeriph_SPI, ENABLE);
  
  /* Enable the DMA AHB clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);
    
  /* Enable GPIOs clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO, ENABLE);     
  
  /* SPI Pins configuration */
  GPIO_InitStructure.GPIO_Pin = I2S_CK_PIN|I2S_DIN_PIN|I2S_DOUT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(I2S_GPIO_PORT, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pin = I2S_WS_PIN;
  GPIO_Init(I2S_WS_GPIO_PORT, &GPIO_InitStructure);   
  
  /* Connect pin to Periph */
  GPIO_PinAFConfig(I2S_GPIO_PORT, I2S_CK_SOURCE, I2S_GPIO_AF); 
  GPIO_PinAFConfig(I2S_GPIO_PORT, I2S_DIN_SOURCE, I2S_GPIO_AF); 
  GPIO_PinAFConfig(I2S_GPIO_PORT, I2S_DOUT_SOURCE, I2S_GPIO_AF);    
  GPIO_PinAFConfig(I2S_WS_GPIO_PORT, I2S_WS_SOURCE, I2S_GPIO_AF); 
    
  /* I2S peripheral configuration */
  I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
  I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
  
   /* Common DMA configuration */
  DMA_InitStructure.DMA_BufferSize = 32;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

#ifdef I2S_MASTER_FULL_DUPLEX   
  /* DMA channel (SPI_Tx) configuration */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2S_Buffer_Tx;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_Init(DMA_CHANNEL_TX, &DMA_InitStructure);

  /* SPI IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = DMA_ChannelTX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* DMA2 channel 1 (SPI_Rx) Configuration */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2SEXT_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2Sext_Buffer_Rx;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_Init(DMA_CHANNEL_RX, &DMA_InitStructure);
      
  /* I2Sext IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = DMA_ChannelRX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* I2S Master Transmitter configuration */
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;    
#endif /* I2S_MASTER_FULL_DUPLEX */
  
#ifdef I2S_SLAVE_FULL_DUPLEX
  
  /* DMA channel (SPI_Rx) Configuration */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2S_Buffer_Rx;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_Init(DMA_CHANNEL_RX, &DMA_InitStructure);
  
  /* SPI IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = DMA_ChannelRX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* DMA channel (I2Sext_Tx) configuration */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2SEXT_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2S_Buffer_Tx;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_Init(DMA_CHANNEL_TX, &DMA_InitStructure);
  
  /* SPI IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = DMA_ChannelTX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* I2S Slave Receiver configuration */
  I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveRx;  
#endif /* I2S_SLAVE_FULL_DUPLEX */

  I2S_Init(SPI, &I2S_InitStructure);
  I2S_FullDuplexConfig(I2Sext,&I2S_InitStructure); 
}
    
/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
static TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
