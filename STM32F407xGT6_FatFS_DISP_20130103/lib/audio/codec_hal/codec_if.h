/********************************************************************************/
/*!
	@file			codec_if.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2012.11.30
	@brief          Helix-MP3 Decorder + I2C AudioCodec Lower Layer.
					Based On STM32F4Discovery Examples Thanks!

    @section HISTORY
		2012.11.30	V1.00	Start Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __CODEC_IF_H
#define __CODEC_IF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* General Inclusion */
#include "platform_config.h"
#include "systick.h"

/* Codec output DEVICE */
#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4

/* Volume Levels values */
#define DEFAULT_VOLMIN                0x00
#define DEFAULT_VOLMAX                0xFF
#define DEFAULT_VOLSTEP               0x04

#define AUDIO_PAUSE                   0
#define AUDIO_RESUME                  1

/* Codec POWER DOWN modes */
#define CODEC_PDWN_HW                 1
#define CODEC_PDWN_SW                 2

/* MUTE commands */
#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0
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
#define CODEC_I2S_IRQHandler           SPI3_IRQHandler

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
#define AUDIO_I2S_DMA_IRQHandler       DMA1_Stream7_IRQHandler

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

/* Serect I2S Data Format */
#define I2S_STANDARD                   I2S_Standard_Phillips

/* The 7 bits Codec address (sent through I2C interface) */
#define CODEC_ADDRESS                   0x94  /* b00100111 */

/* I2SDMA Task Event */
enum{
		IDLING 				= 0,
		TRANSFER_FIRST_HALF = 2,
		TRANSFER_LAST_HALF 	= 4,
		KEY_STOP 			= 8,
	};
	
/* DMA Buffer Defines */
enum{
		INIT_RINGBUF 		= 0,
		HALF_RINGBUF,
		FULL_RINGBUF,
	};

/* Samplerates (for table index) */
enum{
		RATE_96000kHz 		= 0,
		RATE_48000kHz,
		RATE_44100kHz,
		RATE_32000kHz,
		RATE_22050kHz,
		RATE_16000kHz,
		RATE_11025kHz,
		RATE_8000kHz,
	};

/* Externs */
extern uint32_t EVAL_AUDIO_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
extern uint32_t EVAL_AUDIO_DeInit(void);
extern uint32_t EVAL_AUDIO_Play(uint16_t* pBuffer, uint32_t unSize);
extern uint32_t EVAL_AUDIO_PauseResume(uint32_t Cmd);
extern uint32_t EVAL_AUDIO_Stop(uint32_t CodecPowerDown_Mode);
extern uint32_t EVAL_AUDIO_VolumeCtl(uint8_t Volume);
extern uint32_t EVAL_AUDIO_Mute(uint32_t Command);
extern void AUDIO_I2S_DMA_IRQHandler(void);
extern uint8_t AUDIO_I2S_DMA_EventWait(void);

/* Codec_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occurs during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...) on the Codec control interface (I2C).
   You can use the default timeout callback implementation by uncommenting the 
   define USE_DEFAULT_TIMEOUT_CALLBACK in stm32f4_discovery_audio_codec.h file.
   Typically the user implementation of this callback should reset I2C peripheral
   and re-initialize communication or in worst case reset all the application. */
extern uint32_t Codec_TIMEOUT_UserCallback(void);
extern void Codec_SetI2SCLK(uint32_t samplerate);

extern volatile uint32_t unDmaBufMode;
extern volatile uint8_t EV_I2SDMA;


#ifdef __cplusplus
}
#endif

#endif	/* __CODEC_IF_H */
