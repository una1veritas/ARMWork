/*
 *          1. Enable peripheral clock using the following functions
 *             RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE) for SPI1
 *             RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE) for SPI2
 *             RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE) for SPI3.
 *
 *          2. Enable SCK, MOSI, MISO and NSS GPIO clocks using RCC_AHB1PeriphClockCmd()
 *             function.
 *             In I2S mode, if an external clock source is used then the I2S CKIN pin GPIO
 *             clock should also be enabled.
 *
 *          3. Peripherals alternate function:
 *                 - Connect the pin to the desired peripherals' Alternate
 *                   Function (AF) using GPIO_PinAFConfig() function
 *                 - Configure the desired pin in alternate function by:
 *                   GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
 *                 - Select the type, pull-up/pull-down and output speed via
 *                   GPIO_PuPd, GPIO_OType and GPIO_Speed members
 *                 - Call GPIO_Init() function
 *              In I2S mode, if an external clock source is used then the I2S CKIN pin
 *              should be also configured in Alternate function Push-pull pull-up mode.
 *
 *          4. Program the Polarity, Phase, First Data, Baud Rate Prescaler, Slave
 *             Management, Peripheral Mode and CRC Polynomial values using the SPI_Init()
 *             function.
 *             In I2S mode, program the Mode, Standard, Data Format, MCLK Output, Audio
 *             frequency and Polarity using I2S_Init() function.
 *             For I2S mode, make sure that either:
 *              - I2S PLL is configured using the functions RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S),
 *                RCC_PLLI2SCmd(ENABLE) and RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY).
 *              or
 *              - External clock source is configured using the function
 *                RCC_I2SCLKConfig(RCC_I2S2CLKSource_Ext) and after setting correctly the define constant
 *                I2S_EXTERNAL_CLOCK_VAL in the stm32f4xx_conf.h file.
 *
 *          5. Enable the NVIC and the corresponding interrupt using the function
 *             SPI_ITConfig() if you need to use interrupt mode.
 *
 *          6. When using the DMA mode
 *                   - Configure the DMA using DMA_Init() function
 *                   - Active the needed channel Request using SPI_I2S_DMACmd() function
 *
 *          7. Enable the SPI using the SPI_Cmd() function or enable the I2S using
 *             I2S_Cmd().
 *
 *          8. Enable the DMA using the DMA_Cmd() function when using DMA mode.
 *
 *          9. Optionally, you can enable/configure the following parameters without
 *             re-initialization (i.e there is no need to call again SPI_Init() function):
 *              - When bidirectional mode (SPI_Direction_1Line_Rx or SPI_Direction_1Line_Tx)
 *                is programmed as Data direction parameter using the SPI_Init() function
 *                it can be possible to switch between SPI_Direction_Tx or SPI_Direction_Rx
 *                using the SPI_BiDirectionalLineConfig() function.
 *              - When SPI_NSS_Soft is selected as Slave Select Management parameter
 *                using the SPI_Init() function it can be possible to manage the
 *                NSS internal signal using the SPI_NSSInternalSoftwareConfig() function.
 *              - Reconfigure the data size using the SPI_DataSizeConfig() function
 *              - Enable or disable the SS output using the SPI_SSOutputCmd() function
 *
 *          10. To use the CRC Hardware calculation feature refer to the Peripheral
 *              CRC hardware Calculation subsection.
 *
 *
 *          It is possible to use SPI in I2S full duplex mode, in this case, each SPI
 *          peripheral is able to manage sending and receiving data simultaneously
 *          using two data lines. Each SPI peripheral has an extended block called I2Sxext
 *          (ie. I2S2ext for SPI2 and I2S3ext for SPI3).
 *          The extension block is not a full SPI IP, it is used only as I2S slave to
 *          implement full duplex mode. The extension block uses the same clock sources
 *          as its master.
 *          To configure I2S full duplex you have to:
 *
 *          1. Configure SPIx in I2S mode (I2S_Init() function) as described above.
 *
 *          2. Call the I2S_FullDuplexConfig() function using the same strucutre passed to
 *             I2S_Init() function.
 *
 *          3. Call I2S_Cmd() for SPIx then for its extended block.
 *
 *          4. To configure interrupts or DMA requests and to get/clear flag status,
 *             use I2Sxext instance for the extension block.
 *
 *          Functions that can be called with I2Sxext instances are:
 *          I2S_Cmd(), I2S_FullDuplexConfig(), SPI_I2S_ReceiveData(), SPI_I2S_SendData(),
 *          SPI_I2S_DMACmd(), SPI_I2S_ITConfig(), SPI_I2S_GetFlagStatus(), SPI_I2S_ClearFlag(),
 *          SPI_I2S_GetITStatus() and SPI_I2S_ClearITPendingBit().
 *
 *          Example: To use SPI3 in Full duplex mode (SPI3 is Master Tx, I2S3ext is Slave Rx):
 *
 *          RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
 *          I2S_StructInit(&I2SInitStruct);
 *          I2SInitStruct.Mode = I2S_Mode_MasterTx;
 *          I2S_Init(SPI3, &I2SInitStruct);
 *          I2S_FullDuplexConfig(SPI3ext, &I2SInitStruct)
 *          I2S_Cmd(SPI3, ENABLE);
 *          I2S_Cmd(SPI3ext, ENABLE);
 *          ...
 *          while (SPI_I2S_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET)
 *          {}
 *          SPI_I2S_SendData(SPI3, txdata[i]);
 *          ...
 *          while (SPI_I2S_GetFlagStatus(I2S3ext, SPI_FLAG_RXNE) == RESET)
 *          {}
 *          rxdata[i] = SPI_I2S_ReceiveData(I2S3ext);
 *          ...
 */
#include "stm32f4xx_spi.h"

#include "gpio.h"
#include "spi.h"

//SPI_TypeDef * spix[] = { SPI1, SPI2, SPI3 };

void spi_begin(SPI_TypeDef * SPIx, GPIOPin sck, GPIOPin miso, GPIOPin mosi,
		GPIOPin nss) {
	uint8_t af; // = GPIO_AF_SPI1;
	SPI_InitTypeDef SPI_InitStruct;
//	IRQn_Type irq = USART1_IRQn;
//	GPIOPin sck, miso, mosi, nss;

	/* PCLK2 = HCLK/2 */
	//RCC_PCLK2Config(RCC_HCLK_Div2);
//	SPIx = spix[spibus];
	if (SPIx == SPI1) {
//	case SPI1Bus:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		af = GPIO_AF_SPI1;
//		 sck = PA5; // PB3
//		 miso = PA6; // PB4
//		 mosi = PA7; // PB5
//		 nss = PA4; // PA15
//		break;
	} else if ( SPIx == SPI2 ) {
//	case SPI2Bus:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		af = GPIO_AF_SPI2;
		// PB12, 13, 14, 15
//		break;
	} else {
//	case SPI3Bus:
//	default:
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE);
		af = GPIO_AF_SPI3;
//		 sck = PB3;
//		 miso = PB4;
//		 mosi = PB5;
//		 nss = PA15; // PA4;
//		break;
	}

	GPIOMode(PinPort(sck), PinBit(sck), GPIO_Mode_AF, GPIO_Speed_25MHz,
			GPIO_OType_PP, GPIO_PuPd_UP);
	GPIOMode(PinPort(miso), PinBit(miso), GPIO_Mode_AF, GPIO_Speed_25MHz,
			GPIO_OType_PP, GPIO_PuPd_UP);
	GPIOMode(PinPort(mosi), PinBit(mosi), GPIO_Mode_AF, GPIO_Speed_25MHz,
			GPIO_OType_PP, GPIO_PuPd_UP);
	GPIO_PinAFConfig(PinPort(sck), PinSource(sck), af);
	GPIO_PinAFConfig(PinPort(miso), PinSource(miso), af);
	GPIO_PinAFConfig(PinPort(mosi), PinSource(mosi), af);
	// nSS by software
	GPIOMode(PinPort(nss), PinBit(nss), GPIO_Mode_OUT, GPIO_Speed_25MHz,
			GPIO_OType_PP, GPIO_PuPd_UP);
	digitalWrite(nss, HIGH);
	//GPIO_PinAFConfig(PinPort(nss), PinSource(nss), af);

	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = SPI_CRC_Rx;

	SPI_Init(SPIx, &SPI_InitStruct);

	SPI_Cmd(SPIx, ENABLE);
	/*          5. Enable the NVIC and the corresponding interrupt using the function
	 *             SPI_ITConfig() if you need to use interrupt mode.
	 *
	 *          6. When using the DMA mode
	 *                   - Configure the DMA using DMA_Init() function
	 *                   - Active the needed channel Request using SPI_I2S_DMACmd() function
	 *
	 *          7. Enable the SPI using the SPI_Cmd() function or enable the I2S using
	 *             I2S_Cmd().
	 *
	 *          8. Enable the DMA using the DMA_Cmd() function when using DMA mode.
	 *
	 *          9. Optionally, you can enable/configure the following parameters without
	 *             re-initialization (i.e there is no need to call again SPI_Init() function):
	 *              - When bidirectional mode (SPI_Direction_1Line_Rx or SPI_Direction_1Line_Tx)
	 *                is programmed as Data direction parameter using the SPI_Init() function
	 *                it can be possible to switch between SPI_Direction_Tx or SPI_Direction_Rx
	 *                using the SPI_BiDirectionalLineConfig() function.
	 *              - When SPI_NSS_Soft is selected as Slave Select Management parameter
	 *                using the SPI_Init() function it can be possible to manage the
	 *                NSS internal signal using the SPI_NSSInternalSoftwareConfig() function.
	 *              - Reconfigure the data size using the SPI_DataSizeConfig() function
	 *              - Enable or disable the SS output using the SPI_SSOutputCmd() function
	 *
	 *          10. To use the CRC Hardware calculation feature refer to the Peripheral
	 *              CRC hardware Calculation subsection.
	 */
}

void spi_transfer(SPI_TypeDef * SPIx, /*SPIBus spibus,*/ uint8_t * data, uint16_t nbytes) {
//	SPI_TypeDef * SPIx = spix[spibus];
	uint8_t rcvdata;

	for (; nbytes; nbytes--) {
		/* Wait for SPIx Tx buffer empty */
		while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE ) == RESET)
			;
		SPI_I2S_SendData(SPIx, (uint16_t) *data);
		/* Wait for SPIx data reception */
		while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE ) == RESET)
			;
		/* Read SPIy received data */
		rcvdata = SPI_I2S_ReceiveData(SPIx);
		*data = rcvdata;
		data++;
	}
}
