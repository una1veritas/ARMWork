#include "stlib.h"
#include "codec.h"

/* Audio Reset Pin definition */
#define AUDIO_RESET_GPIO_CLK           RCC_AHB1Periph_GPIOD
#define AUDIO_RESET_PIN                GPIO_Pin_4
#define AUDIO_RESET_GPIO               GPIOD

/* I2S peripheral configuration defines */
#define CODEC_I2S                      SPI3
#define CODEC_I2S_CLK                  RCC_APB1Periph_SPI3
#define CODEC_I2S_ADDRESS              0x40003C0C
#define CODEC_I2S_GPIO_AF              GPIO_AF_SPI3
#define CODEC_I2S_IRQ                  SPI3_IRQn
#define CODEC_I2S_GPIO_CLOCK           (RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA)
#define CODEC_I2S_WS_PIN               GPIO_Pin_4
#define CODEC_I2S_SCK_PIN              GPIO_Pin_10
#define CODEC_I2S_SD_PIN               GPIO_Pin_12
#define CODEC_I2S_MCK_PIN              GPIO_Pin_7
#define CODEC_I2S_WS_PINSRC            GPIO_PinSource4
#define CODEC_I2S_SCK_PINSRC           GPIO_PinSource10
#define CODEC_I2S_SD_PINSRC            GPIO_PinSource12
#define CODEC_I2S_MCK_PINSRC           GPIO_PinSource7
#define CODEC_I2S_GPIO                 GPIOC
#define CODEC_I2S_WS_GPIO              GPIOA
#define CODEC_I2S_MCK_GPIO             GPIOC
#define Audio_I2S_IRQHandler           SPI3_IRQHandler

/* I2S DMA Stream definitions */
#define AUDIO_I2S_DMA_CLOCK            RCC_AHB1Periph_DMA1
#define AUDIO_I2S_DMA_STREAM           DMA1_Stream7
#define AUDIO_I2S_DMA_DREG             CODEC_I2S_ADDRESS
#define AUDIO_I2S_DMA_CHANNEL          DMA_Channel_0
#define AUDIO_I2S_DMA_IRQ              DMA1_Stream7_IRQn
#define AUDIO_I2S_DMA_FLAG_TC          DMA_FLAG_TCIF7
#define AUDIO_I2S_DMA_FLAG_HT          DMA_FLAG_HTIF7
#define AUDIO_I2S_DMA_FLAG_FE          DMA_FLAG_FEIF7
#define AUDIO_I2S_DMA_FLAG_TE          DMA_FLAG_TEIF7
#define AUDIO_I2S_DMA_FLAG_DME         DMA_FLAG_DMEIF7

/* I2C peripheral configuration defines (control interface of the audio codec) */
#define CODEC_I2C                      I2C1
#define CODEC_I2C_CLK                  RCC_APB1Periph_I2C1
#define CODEC_I2C_GPIO_CLOCK           RCC_AHB1Periph_GPIOB
#define CODEC_I2C_GPIO_AF              GPIO_AF_I2C1
#define CODEC_I2C_GPIO                 GPIOB
#define CODEC_I2C_SCL_PIN              GPIO_Pin_6
#define CODEC_I2C_SDA_PIN              GPIO_Pin_9
#define CODEC_I2S_SCL_PINSRC           GPIO_PinSource6
#define CODEC_I2S_SDA_PINSRC           GPIO_PinSource9

/* Mask for the bit EN of the I2S CFGR register */
#define I2S_ENABLE_MASK                 0x0400

/* Delay for the Codec to be correctly reset */
#define CODEC_RESET_DELAY              0x4FFF
#define CODEC_FLAG_TIMEOUT             ((uint32_t)0x1000)
#define CODEC_LONG_TIMEOUT             ((uint32_t)(300 * CODEC_FLAG_TIMEOUT))
#define CODEC_I2C_SPEED                100000

/*
#define I2S_Standard_Phillips           ((uint16_t)0x0000)
#define I2S_Standard_MSB                ((uint16_t)0x0010)
#define I2S_Standard_LSB                ((uint16_t)0x0020)
#define I2S_Standard_PCMShort           ((uint16_t)0x0030)
#define I2S_Standard_PCMLong            ((uint16_t)0x00B0)
*/
#define I2S_STANDARD                   I2S_Standard_Phillips

/* The 7 bits Codec address (sent through I2C interface) */
#define CODEC_ADDRESS                   0x94  /* b00100111 */
volatile uint32_t CODECTimeout = CODEC_LONG_TIMEOUT;

#define VOLUME_CONVERT(x)    ((x > 100)? 100:((uint8_t)((x * 255) / 100)))

/*-----------------------------------
                           Audio Codec functions
                                    ------------------------------------------*/
/* High Layer codec functions */
static uint32_t Codec_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
static uint32_t Codec_DeInit(void);
static uint32_t Codec_PauseResume(uint32_t Cmd);
static uint32_t Codec_Stop(uint32_t Cmd);
static uint32_t Codec_VolumeCtrl(uint8_t Volume);
static uint32_t Codec_Mute(uint32_t Cmd);
/* Low layer codec functions */
static void     Codec_CtrlInterface_Init(void);
static void     Codec_CtrlInterface_DeInit(void);
static void     Codec_AudioInterface_Init(uint32_t AudioFreq);
static void     Codec_AudioInterface_DeInit(void);
static void     Codec_Reset(void);
static uint32_t Codec_WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue);
static uint32_t Codec_ReadRegister(uint8_t RegisterAddr);
static void     Codec_GPIO_Init(void);
static void     Codec_GPIO_DeInit(void);
static void     Delay(__IO uint32_t nCount);

void I2SEnable()
{
  if((CODEC_I2S->I2SCFGR & I2S_ENABLE_MASK) == 0)
  {
    /* Enable the selected SPI peripheral (in I2S mode) */
    CODEC_I2S->I2SCFGR |= SPI_I2SCFGR_I2SE;
  }
}

void I2SDisable()
{
  /* Disable the selected SPI peripheral in I2S mode */
  CODEC_I2S->I2SCFGR &= (uint16_t)~((uint16_t)SPI_I2SCFGR_I2SE);
}

void I2SEnableDmaStreamRequest()
{
  /* Enable the selected SPI DMA requests */
  CODEC_I2S->CR2 |= SPI_I2S_DMAReq_Tx;
}

void I2SDisableDmaStreamRequest()
{
  /* Disable the selected SPI DMA requests */
  CODEC_I2S->CR2 &= (uint16_t)~SPI_I2S_DMAReq_Tx;
}

void I2SEnableDmaStream()
{
  /* Enable the selected DMAy Streamx by setting EN bit */
  AUDIO_I2S_DMA_STREAM->CR |= (uint32_t)DMA_SxCR_EN;
}

void I2SDisableDmaStream(uint32_t unWait)
{
  /* Disable the selected DMAy Streamx by clearing EN bit */
  AUDIO_I2S_DMA_STREAM->CR &= ~(uint32_t)DMA_SxCR_EN;
  if(unWait)
  {
    /* Wait the DMA Stream to be effectively disabled */
    while((AUDIO_I2S_DMA_STREAM->CR & (uint32_t)DMA_SxCR_EN) != 0)
    {
    }
  }
}

void I2SEnableDmaStreamInterrupts()
{
  /* Enable the selected DMA transfer interrupts */
  AUDIO_I2S_DMA_STREAM->CR |= (uint32_t)(DMA_IT_TC | DMA_IT_HT);
}

void I2SClearDmaStreamInterrupt(uint32_t unFlag)
{
  DMA_ClearFlag(AUDIO_I2S_DMA_STREAM, unFlag);
}

/**
  * @brief  Configure the audio peripherals.
  * @param  OutputDevice: OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
  *                       OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO .
  * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq)
{
  /* Perform low layer Codec initialization */
  if (Codec_Init(OutputDevice, VOLUME_CONVERT(Volume), AudioFreq) != 0)
  {
    return 1;
  }
  else
  {
    /* Enable the DMA clock */
    RCC_AHB1PeriphClockCmd(AUDIO_I2S_DMA_CLOCK, ENABLE);

    // Configure the DMA Stream
    I2SDisableDmaStream(0);
    DMA_DeInit(AUDIO_I2S_DMA_STREAM);
    I2SEnableDmaStreamInterrupts();

    /* Enable the I2S DMA request */
    I2SEnableDmaStreamRequest();

    /* Return 0 when all operations are OK */
    return 0;
  }
}

/**
  * @brief  Deinitializes all the resources used by the codec (those initialized
  *         by EVAL_AUDIO_Init() function).
  * @param  None
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_DeInit(void)
{
  /* Disable the DMA stream before the deinit */
  I2SDisableDmaStream(0);

  /* Dinitialize the DMA Stream */
  DMA_DeInit(AUDIO_I2S_DMA_STREAM);

  /*
     The DMA clock is not disabled, since it can be used by other streams
                                                                          */

  /* DeInitialize Codec */
  Codec_DeInit();

  return 0;
}

uint32_t EVAL_AUDIO_Play(uint16_t* pBuffer, uint32_t unSize)
{
  /* Disable the I2S DMA Stream and wait for it to be disabled*/
  I2SDisableDmaStream(1);

  /* Clear the Interrupt flag */
  //DMA_ClearFlag(AUDIO_I2S_DMA_STREAM, AUDIO_I2S_DMA_FLAG_HT);
  //DMA_ClearFlag(AUDIO_I2S_DMA_STREAM, AUDIO_I2S_DMA_FLAG_TC);
  I2SClearDmaStreamInterrupt(AUDIO_I2S_DMA_FLAG_HT);
  I2SClearDmaStreamInterrupt(AUDIO_I2S_DMA_FLAG_TC);


  /* Configure the buffer address and size */
  DMA_InitTypeDef DMA_InitStructure;
  DMA_InitStructure.DMA_Channel = AUDIO_I2S_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = AUDIO_I2S_DMA_DREG;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pBuffer;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)(unSize / 2);

  /* Configure the DMA Stream with the new parameters */
  DMA_Init(AUDIO_I2S_DMA_STREAM, &DMA_InitStructure);

  /* Enable the I2S DMA Stream*/
  //DMA_Cmd(AUDIO_MAL_DMA_STREAM, ENABLE);
  I2SEnableDmaStream();

  /* If the I2S peripheral is still not enabled, enable it */
  I2SEnable();

  return 0;
}

/**
  * @brief  This function Pauses or Resumes the audio file stream. In case
  *         of using DMA, the DMA Pause feature is used. In all cases the I2S
  *         peripheral is disabled.
  *
  * @WARNING When calling EVAL_AUDIO_PauseResume() function for pause, only
  *          this function should be called for resume (use of EVAL_AUDIO_Play()
  *          function for resume could lead to unexpected behavior).
  *
  * @param  Cmd: AUDIO_PAUSE (or 0) to pause, AUDIO_RESUME (or any value different
  *         from 0) to resume.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_PauseResume(uint32_t Cmd)
{
  /* Call the Audio Codec Pause/Resume function */
  if (Codec_PauseResume(Cmd) != 0)
  {
    return 1;
  }
  else
  {
    /* Pause the audio file playing */
    if (Cmd == AUDIO_PAUSE)
    {
      /* Disable the I2S DMA request */
      I2SDisableDmaStreamRequest();

      /* Pause the I2S DMA Stream
          Note. For the STM32F40x devices, the DMA implements a pause feature,
                by disabling the stream, all configuration is preserved and data
                transfer is paused till the next enable of the stream.
                This feature is not available on STM32F40x devices. */
      I2SDisableDmaStream(0);
    }
    else /* AUDIO_RESUME */
    {
      /* Enable the I2S DMA request */
      I2SEnableDmaStreamRequest();

      /* Resume the I2S DMA Stream
          Note. For the STM32F40x devices, the DMA implements a pause feature,
                by disabling the stream, all configuration is preserved and data
                transfer is paused till the next enable of the stream.
                This feature is not available on STM32F40x devices. */
      I2SEnableDmaStream();

      /* If the I2S peripheral is still not enabled, enable it */
      I2SEnable();
    }

    /* Return 0 if all operations are OK */
    return 0;
  }
}

/**
  * @brief  Stops audio playing and Power down the Audio Codec.
  * @param  Option: could be one of the following parameters
  *           - CODEC_PDWN_SW: for software power off (by writing registers).
  *                            Then no need to reconfigure the Codec after power on.
  *           - CODEC_PDWN_HW: completely shut down the codec (physically).
  *                            Then need to reconfigure the Codec after power on.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_Stop(uint32_t Option)
{
  /* Call Audio Codec Stop function */
  if (Codec_Stop(Option) != 0)
  {
    return 1;
  }
  else
  {
    /* Stop the Transfer on the I2S side: Stop and disable the DMA stream */
    I2SDisableDmaStream(0);

    /* Clear all the DMA flags for the next transfer */
    //DMA_ClearFlag(AUDIO_I2S_DMA_STREAM, AUDIO_I2S_DMA_FLAG_TC |AUDIO_I2S_DMA_FLAG_HT |
    //                                AUDIO_I2S_DMA_FLAG_FE | AUDIO_I2S_DMA_FLAG_TE);
    I2SClearDmaStreamInterrupt(AUDIO_I2S_DMA_FLAG_TC);
    I2SClearDmaStreamInterrupt(AUDIO_I2S_DMA_FLAG_HT);
    I2SClearDmaStreamInterrupt(AUDIO_I2S_DMA_FLAG_FE);
    I2SClearDmaStreamInterrupt(AUDIO_I2S_DMA_FLAG_TE);

    /*
             The I2S DMA requests are not disabled here.
                                                              */

    /* In all modes, disable the I2S peripheral */
    I2SDisable();

    /* Return 0 when all operations are correctly done */
    return 0;
  }
}

/**
  * @brief  Controls the current audio volume level.
  * @param  Volume: Volume level to be set in percentage from 0% to 100% (0 for
  *         Mute and 100 for Max volume level).
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_VolumeCtl(uint8_t Volume)
{
  /* Call the codec volume control function with converted volume value */
  return (Codec_VolumeCtrl(VOLUME_CONVERT(Volume)));
}

/**
  * @brief  Enables or disables the MUTE mode by software
  * @param  Command: could be AUDIO_MUTE_ON to mute sound or AUDIO_MUTE_OFF to
  *         unmute the codec and restore previous volume level.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_Mute(uint32_t Cmd)
{
  /* Call the Codec Mute function */
  return (Codec_Mute(Cmd));
}


/*========================

                CS43L22 Audio Codec Control Functions
                                                ==============================*/
/**
  * @brief  Initializes the audio codec and all related interfaces (control
  *         interface: I2C and audio interface: I2S)
  * @param  OutputDevice: can be OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
  *                       OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO .
  * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @retval 0 if correct communication, else wrong communication
  */
static uint32_t Codec_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq)
{
  uint32_t counter = 0;

  /* Configure the Codec related IOs */
  Codec_GPIO_Init();

  /* Reset the Codec Registers */
  Codec_Reset();

  /* Initialize the Control interface of the Audio Codec */
  Codec_CtrlInterface_Init();

  /* Keep Codec powered OFF */
  counter += Codec_WriteRegister(0x02, 0x01);

  counter += Codec_WriteRegister(0x04, 0xAF); /* SPK always OFF & HP always ON */

  /* Clock configuration: Auto detection */
  counter += Codec_WriteRegister(0x05, 0x81);

  /* Set the Slave Mode and the audio Standard */
  counter += Codec_WriteRegister(0x06, 0x04);

  /* Set the Master volume */
  Codec_VolumeCtrl(Volume);

  /* Power on the Codec */
  counter += Codec_WriteRegister(0x02, 0x9E);

  /* Additional configuration for the CODEC. These configurations are done to reduce
      the time needed for the Codec to power off. If these configurations are removed,
      then a long delay should be added between powering off the Codec and switching
      off the I2S peripheral MCLK clock (which is the operating clock for Codec).
      If this delay is not inserted, then the codec will not shut down properly and
      it results in high noise after shut down. */

  /* Disable the analog soft ramp */
  counter += Codec_WriteRegister(0x0A, 0x00);
  /* Disable the digital soft ramp */
  counter += Codec_WriteRegister(0x0E, 0x04);
  /* Disable the limiter attack level */
  counter += Codec_WriteRegister(0x27, 0x00);
  /* Adjust Bass and Treble levels */
  counter += Codec_WriteRegister(0x1F, 0x0F);
  /* Adjust PCM volume level */
  counter += Codec_WriteRegister(0x1A, 0x0A);
  counter += Codec_WriteRegister(0x1B, 0x0A);

  /* Configure the I2S peripheral */
  Codec_AudioInterface_Init(AudioFreq);

  /* Return communication control value */
  return counter;
}

/**
  * @brief  Restore the audio codec state to default state and free all used
  *         resources.
  * @param  None
  * @retval 0 if correct communication, else wrong communication
  */
static uint32_t Codec_DeInit(void)
{
  uint32_t counter = 0;

  /* Reset the Codec Registers */
  Codec_Reset();

  /* Keep Codec powered OFF */
  counter += Codec_WriteRegister(0x02, 0x01);

  /* Deinitialize all use GPIOs */
  Codec_GPIO_DeInit();

  /* Disable the Codec control interface */
  Codec_CtrlInterface_DeInit();

  /* Deinitialize the Codec audio interface (I2S) */
  Codec_AudioInterface_DeInit();

  /* Return communication control value */
  return counter;
}

/**
  * @brief  Pauses and resumes playing on the audio codec.
  * @param  Cmd: AUDIO_PAUSE (or 0) to pause, AUDIO_RESUME (or any value different
  *         from 0) to resume.
  * @retval 0 if correct communication, else wrong communication
  */
static uint32_t Codec_PauseResume(uint32_t Cmd)
{
  uint32_t counter = 0;

  /* Pause the audio file playing */
  if (Cmd == AUDIO_PAUSE)
  {
    /* Mute the output first */
    counter += Codec_Mute(AUDIO_MUTE_ON);

    /* Put the Codec in Power save mode */
    counter += Codec_WriteRegister(0x02, 0x01);
  }
  else /* AUDIO_RESUME */
  {
    /* Unmute the output first */
    counter += Codec_Mute(AUDIO_MUTE_OFF);

    counter += Codec_WriteRegister(0x04, 0xAF);

    /* Exit the Power save mode */
    counter += Codec_WriteRegister(0x02, 0x9E);
  }

  return counter;
}

/**
  * @brief  Stops audio Codec playing. It powers down the codec.
  * @param  CodecPdwnMode: selects the  power down mode.
  *          - CODEC_PDWN_SW: only mutes the audio codec. When resuming from this
  *                           mode the codec keeps the previous initialization
  *                           (no need to re-Initialize the codec registers).
  *          - CODEC_PDWN_HW: Physically power down the codec. When resuming from this
  *                           mode, the codec is set to default configuration
  *                           (user should re-Initialize the codec in order to
  *                            play again the audio stream).
  * @retval 0 if correct communication, else wrong communication
  */
static uint32_t Codec_Stop(uint32_t CodecPdwnMode)
{
  uint32_t counter = 0;

  /* Mute the output first */
  Codec_Mute(AUDIO_MUTE_ON);

  if (CodecPdwnMode == CODEC_PDWN_SW)
  {
    /* Power down the DAC and the speaker (PMDAC and PMSPK bits)*/
    counter += Codec_WriteRegister(0x02, 0x9F);
  }
  else /* CODEC_PDWN_HW */
  {
    /* Power down the DAC components */
    counter += Codec_WriteRegister(0x02, 0x9F);

    /* Wait at least 100us */
    Delay(0xFFF);

    /* Reset The pin */
    GPIO_WriteBit(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, Bit_RESET);
  }

  return counter;
}

/**
  * @brief  Sets higher or lower the codec volume level.
  * @param  Volume: a byte value from 0 to 255 (refer to codec registers
  *         description for more details).
  * @retval 0 if correct communication, else wrong communication
  */
static uint32_t Codec_VolumeCtrl(uint8_t Volume)
{
  uint32_t counter = 0;

  if (Volume > 0xE6)
  {
    /* Set the Master volume */
    counter += Codec_WriteRegister(0x20, Volume - 0xE7);
    counter += Codec_WriteRegister(0x21, Volume - 0xE7);
  }
  else
  {
    /* Set the Master volume */
    counter += Codec_WriteRegister(0x20, Volume + 0x19);
    counter += Codec_WriteRegister(0x21, Volume + 0x19);
  }

  return counter;
}

/**
  * @brief  Enables or disables the mute feature on the audio codec.
  * @param  Cmd: AUDIO_MUTE_ON to enable the mute or AUDIO_MUTE_OFF to disable the
  *             mute mode.
  * @retval 0 if correct communication, else wrong communication
  */
static uint32_t Codec_Mute(uint32_t Cmd)
{
  uint32_t counter = 0;

  /* Set the Mute mode */
  if (Cmd == AUDIO_MUTE_ON)
  {
    counter += Codec_WriteRegister(0x04, 0xFF);
  }
  else /* AUDIO_MUTE_OFF Disable the Mute */
  {
    counter += Codec_WriteRegister(0x04, 0xAF);
  }

  return counter;
}

/**
  * @brief  Resets the audio codec. It restores the default configuration of the
  *         codec (this function shall be called before initializing the codec).
  * @note   This function calls an external driver function: The IO Expander driver.
  * @param  None
  * @retval None
  */
static void Codec_Reset(void)
{
  /* Power Down the codec */
  GPIO_WriteBit(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, Bit_RESET);

  /* wait for a delay to insure registers erasing */
  Delay(CODEC_RESET_DELAY);

  /* Power on the codec */
  GPIO_WriteBit(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, Bit_SET);
}

/**
  * @brief  Writes a Byte to a given register into the audio codec through the
            control interface (I2C)
  * @param  RegisterAddr: The address (location) of the register to be written.
  * @param  RegisterValue: the Byte value to be written into destination register.
  * @retval 0 if correct communication, else wrong communication
  */
static uint32_t Codec_WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue)
{
  uint32_t result = 0;

  /*!< While the bus is busy */
  CODECTimeout = CODEC_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BUSY))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /* Start the config sequence */
  I2C_GenerateSTART(CODEC_I2C, ENABLE);

  /* Test on EV5 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(CODEC_I2C, CODEC_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /* Transmit the first address for write operation */
  I2C_SendData(CODEC_I2C, RegisterAddr);

  /* Test on EV8 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /* Prepare the register value to be sent */
  I2C_SendData(CODEC_I2C, RegisterValue);

  /*!< Wait till all data have been physically transferred on the bus */
  CODECTimeout = CODEC_LONG_TIMEOUT;
  while(!I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BTF))
  {
    if((CODECTimeout--) == 0) Codec_TIMEOUT_UserCallback();
  }

  /* End the configuration sequence */
  I2C_GenerateSTOP(CODEC_I2C, ENABLE);

  /* Verify that the data has been correctly written */
  result = (Codec_ReadRegister(RegisterAddr) == RegisterValue)? 0:1;

  /* Return the verifying value: 0 (Passed) or 1 (Failed) */
  return result;
}

/**
  * @brief  Reads and returns the value of an audio codec register through the
  *         control interface (I2C).
  * @param  RegisterAddr: Address of the register to be read.
  * @retval Value of the register to be read or dummy value if the communication
  *         fails.
  */
static uint32_t Codec_ReadRegister(uint8_t RegisterAddr)
{
  uint32_t result = 0;

  /*!< While the bus is busy */
  CODECTimeout = CODEC_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BUSY))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /* Start the config sequence */
  I2C_GenerateSTART(CODEC_I2C, ENABLE);

  /* Test on EV5 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(CODEC_I2C, CODEC_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /* Transmit the register address to be read */
  I2C_SendData(CODEC_I2C, RegisterAddr);

  /* Test on EV8 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_BTF) == RESET)
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /*!< Send START condition a second time */
  I2C_GenerateSTART(CODEC_I2C, ENABLE);

  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(CODEC_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /*!< Send Codec address for read */
  I2C_Send7bitAddress(CODEC_I2C, CODEC_ADDRESS, I2C_Direction_Receiver);

  /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_ADDR) == RESET)
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /*!< Disable Acknowledgment */
  I2C_AcknowledgeConfig(CODEC_I2C, DISABLE);

  /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
  (void)CODEC_I2C->SR2;

  /*!< Send STOP Condition */
  I2C_GenerateSTOP(CODEC_I2C, ENABLE);

  /* Wait for the byte to be received */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(CODEC_I2C, I2C_FLAG_RXNE) == RESET)
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /*!< Read the byte received from the Codec */
  result = I2C_ReceiveData(CODEC_I2C);

  /* Wait to make sure that STOP flag has been cleared */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while(CODEC_I2C->CR1 & I2C_CR1_STOP)
  {
    if((CODECTimeout--) == 0) return Codec_TIMEOUT_UserCallback();
  }

  /*!< Re-Enable Acknowledgment to be ready for another reception */
  I2C_AcknowledgeConfig(CODEC_I2C, ENABLE);

  /* Clear AF flag for next communication */
  I2C_ClearFlag(CODEC_I2C, I2C_FLAG_AF);

  /* Return the byte read from Codec */
  return result;
}

/**
  * @brief  Initializes the Audio Codec control interface (I2C).
  * @param  None
  * @retval None
  */
static void Codec_CtrlInterface_Init(void)
{
  I2C_InitTypeDef I2C_InitStructure;

  /* Enable the CODEC_I2C peripheral clock */
  RCC_APB1PeriphClockCmd(CODEC_I2C_CLK, ENABLE);

  /* CODEC_I2C peripheral configuration */
  I2C_DeInit(CODEC_I2C);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x33;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = CODEC_I2C_SPEED;
  /* Enable the I2C peripheral */
  I2C_Cmd(CODEC_I2C, ENABLE);
  I2C_Init(CODEC_I2C, &I2C_InitStructure);
}

/**
  * @brief  Restore the Audio Codec control interface to its default state.
  *         This function doesn't de-initialize the I2C because the I2C peripheral
  *         may be used by other modules.
  * @param  None
  * @retval None
  */
static void Codec_CtrlInterface_DeInit(void)
{
  /* Disable the I2C peripheral */ /* This step is not done here because
     the I2C interface can be used by other modules */
  /* I2C_DeInit(CODEC_I2C); */
}

/**
  * @brief  Initializes the Audio Codec audio interface (I2S)
  * @note   This function assumes that the I2S input clock (through PLL_R in
  *         Devices RevA/Z and through dedicated PLLI2S_R in Devices RevB/Y)
  *         is already configured and ready to be used.
  * @param  AudioFreq: Audio frequency to be configured for the I2S peripheral.
  * @retval None
  */
static void Codec_AudioInterface_Init(uint32_t AudioFreq)
{
  I2S_InitTypeDef I2S_InitStructure;

  /* Enable the CODEC_I2S peripheral clock */
  RCC_APB1PeriphClockCmd(CODEC_I2S_CLK, ENABLE);

  /* CODEC_I2S peripheral configuration */
  SPI_I2S_DeInit(CODEC_I2S);
  I2S_InitStructure.I2S_AudioFreq = AudioFreq;
  I2S_InitStructure.I2S_Standard = I2S_STANDARD;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
   I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;

  /* Initialize the I2S peripheral with the structure above */
  I2S_Init(CODEC_I2S, &I2S_InitStructure);

  /* The I2S peripheral will be enabled only in the EVAL_AUDIO_Play() function
       or by user functions if DMA mode not enabled */
}

/**
  * @brief  Restores the Audio Codec audio interface to its default state.
  * @param  None
  * @retval None
  */
static void Codec_AudioInterface_DeInit(void)
{
  /* Disable the CODEC_I2S peripheral (in case it hasn't already been disabled) */
  I2SDisable();

  /* Deinitialize the CODEC_I2S peripheral */
  SPI_I2S_DeInit(CODEC_I2S);

  /* Disable the CODEC_I2S peripheral clock */
  RCC_APB1PeriphClockCmd(CODEC_I2S_CLK, DISABLE);
}

/**
  * @brief Initializes IOs used by the Audio Codec (on the control and audio
  *        interfaces).
  * @param  None
  * @retval None
  */
static void Codec_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable Reset GPIO Clock */
  RCC_AHB1PeriphClockCmd(AUDIO_RESET_GPIO_CLK,ENABLE);

  /* Audio reset pin configuration -------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = AUDIO_RESET_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(AUDIO_RESET_GPIO, &GPIO_InitStructure);
  //palSetPadMode(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_PUDR_FLOATING | PAL_STM32_OSPEED_MID2);

  /* Enable I2S and I2C GPIO clocks */
  RCC_AHB1PeriphClockCmd(CODEC_I2C_GPIO_CLOCK | CODEC_I2S_GPIO_CLOCK, ENABLE);

  /* CODEC_I2C SCL and SDA pins configuration -------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = CODEC_I2C_SCL_PIN | CODEC_I2C_SDA_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CODEC_I2C_GPIO, &GPIO_InitStructure);
  //palSetPadMode(CODEC_I2C_GPIO, CODEC_I2C_SCL_PIN, PAL_MODE_OUTPUT_OPENDRAIN | PAL_STM32_PUDR_FLOATING | PAL_STM32_OSPEED_MID2);
  //palSetPadMode(CODEC_I2C_GPIO, CODEC_I2C_SDA_PIN, PAL_MODE_OUTPUT_OPENDRAIN | PAL_STM32_PUDR_FLOATING | PAL_STM32_OSPEED_MID2);
  /* Connect pins to I2C peripheral */
  GPIO_PinAFConfig(CODEC_I2C_GPIO, CODEC_I2S_SCL_PINSRC, CODEC_I2C_GPIO_AF);
  GPIO_PinAFConfig(CODEC_I2C_GPIO, CODEC_I2S_SDA_PINSRC, CODEC_I2C_GPIO_AF);

  /* CODEC_I2S pins configuration: WS, SCK and SD pins -----------------------------*/
  GPIO_InitStructure.GPIO_Pin = CODEC_I2S_SCK_PIN | CODEC_I2S_SD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(CODEC_I2S_GPIO, &GPIO_InitStructure);
  //palSetPadMode(CODEC_I2S_GPIO, CODEC_I2S_SCK_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_PUDR_FLOATING | PAL_STM32_OSPEED_MID2);
  //palSetPadMode(CODEC_I2S_GPIO, CODEC_I2S_SD_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_PUDR_FLOATING | PAL_STM32_OSPEED_MID2);

  /* Connect pins to I2S peripheral  */
  GPIO_PinAFConfig(CODEC_I2S_WS_GPIO, CODEC_I2S_WS_PINSRC, CODEC_I2S_GPIO_AF);
  GPIO_PinAFConfig(CODEC_I2S_GPIO, CODEC_I2S_SCK_PINSRC, CODEC_I2S_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = CODEC_I2S_WS_PIN ;
  GPIO_Init(CODEC_I2S_WS_GPIO, &GPIO_InitStructure);
  //palSetPadMode(CODEC_I2S_WS_GPIO, CODEC_I2S_WS_PIN, PAL_MODE_ALTERNATE(CODEC_I2S_GPIO_AF) | PAL_STM32_OSPEED_MID2);
  GPIO_PinAFConfig(CODEC_I2S_GPIO, CODEC_I2S_SD_PINSRC, CODEC_I2S_GPIO_AF);

  /* CODEC_I2S pins configuration: MCK pin */
  GPIO_InitStructure.GPIO_Pin = CODEC_I2S_MCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(CODEC_I2S_MCK_GPIO, &GPIO_InitStructure);
  /* Connect pins to I2S peripheral  */
  GPIO_PinAFConfig(CODEC_I2S_MCK_GPIO, CODEC_I2S_MCK_PINSRC, CODEC_I2S_GPIO_AF);
  //palSetPadMode(CODEC_I2S_MCK_GPIO, CODEC_I2S_MCK_PIN, PAL_MODE_ALTERNATE(CODEC_I2S_GPIO_AF) | PAL_STM32_OSPEED_MID2);
}

/**
  * @brief  Restores the IOs used by the Audio Codec interface to their default state.
  * @param  None
  * @retval None
  */
static void Codec_GPIO_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Deinitialize all the GPIOs used by the driver */
  GPIO_InitStructure.GPIO_Pin =  CODEC_I2S_SCK_PIN | CODEC_I2S_SD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CODEC_I2S_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = CODEC_I2S_WS_PIN ;
  GPIO_Init(CODEC_I2S_WS_GPIO, &GPIO_InitStructure);

  /* Disconnect pins from I2S peripheral  */
  GPIO_PinAFConfig(CODEC_I2S_WS_GPIO, CODEC_I2S_WS_PINSRC, 0x00);
  GPIO_PinAFConfig(CODEC_I2S_GPIO, CODEC_I2S_SCK_PINSRC, 0x00);
  GPIO_PinAFConfig(CODEC_I2S_GPIO, CODEC_I2S_SD_PINSRC, 0x00);

  /* CODEC_I2S pins deinitialization: MCK pin */
  GPIO_InitStructure.GPIO_Pin = CODEC_I2S_MCK_PIN;
  GPIO_Init(CODEC_I2S_MCK_GPIO, &GPIO_InitStructure);
  /* Disconnect pins from I2S peripheral  */
  GPIO_PinAFConfig(CODEC_I2S_MCK_GPIO, CODEC_I2S_MCK_PINSRC, CODEC_I2S_GPIO_AF);
}

/**
  * @brief  Inserts a delay time (not accurate timing).
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void Delay( __IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}
