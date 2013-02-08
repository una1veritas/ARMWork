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

#include "spi.h"


void spi_init(spi * port, SPI_TypeDef * SPIx, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin nss) {
	port->SPIx = SPIx;
	port->sck = sck;
	port->miso = miso;
	port->mosi = mosi;
	port->nss = nss;
}
							
void spi_begin(spi * port) {	
	uint8_t af; // = GPIO_AF_SPI1;

	/* PCLK2 = HCLK/2 */
	//RCC_PCLK2Config(RCC_HCLK_Div2);
	if ( port->SPIx == SPI2 ) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		af = GPIO_AF_SPI2;
		port->SPIx = SPI2;
		// PB12, 13, 14, 15
	} else if ( port->SPIx == SPI3 ) {
		// SPI3
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
		af = GPIO_AF_SPI3;
		port->SPIx = SPI3;
	}	else { //if (SPIx == SPI1) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		af = GPIO_AF_SPI1;
		port->SPIx = SPI1;
		// sck = PA5 / PB3, miso = PA6/ PB4, mosi = PA7 / PB5, nSS = PA4 / PA15
	} 

	GPIOMode(PinPort(port->sck), PinBit(port->sck), GPIO_Mode_AF, GPIO_Speed_50MHz,
			GPIO_OType_PP, GPIO_PuPd_UP);
	GPIOMode(PinPort(port->miso), PinBit(port->miso), GPIO_Mode_AF, GPIO_Speed_50MHz,
			GPIO_OType_PP, GPIO_PuPd_UP);
	GPIOMode(PinPort(port->mosi), PinBit(port->mosi), GPIO_Mode_AF, GPIO_Speed_50MHz,
			GPIO_OType_PP, GPIO_PuPd_UP);
	GPIO_PinAFConfig(PinPort(port->sck), PinSource(port->sck), af);
	GPIO_PinAFConfig(PinPort(port->miso), PinSource(port->miso), af);
	GPIO_PinAFConfig(PinPort(port->mosi), PinSource(port->mosi), af);
	// nSS by software
	GPIOMode(PinPort(port->nss), PinBit(port->nss), GPIO_Mode_OUT, GPIO_Speed_50MHz,
			GPIO_OType_PP, GPIO_PuPd_UP);
	//GPIO_PinAFConfig(PinPort(nss), PinSource(nss), af);
	
	// setup has completed

	digitalWrite(port->nss, HIGH);
	
	// set default mode
	port->modedef.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	port->modedef.SPI_Mode = SPI_Mode_Master;
	port->modedef.SPI_DataSize = SPI_DataSize_8b;
	port->modedef.SPI_CPOL = SPI_CPOL_Low;
	port->modedef.SPI_CPHA = SPI_CPHA_1Edge;
	port->modedef.SPI_NSS = SPI_NSS_Soft;
	port->modedef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	port->modedef.SPI_FirstBit = SPI_FirstBit_MSB;
	port->modedef.SPI_CRCPolynomial = SPI_CRC_Rx;

	SPI_Init(port->SPIx, &port->modedef);

	SPI_Cmd(port->SPIx, ENABLE);
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

void spi_disable(spi * spiport) {
	if ( spiport->SPIx == SPI2 ) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	} else if ( spiport->SPIx == SPI3 ) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	} else {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	}
  /* DISABLE */ 
  SPI_Cmd(spiport->SPIx, DISABLE);
	if ( spiport->SPIx == SPI2 ) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, DISABLE);
	} else if ( spiport->SPIx == SPI3 ) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, DISABLE);
	} else {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
	}
}

void spi_setMode(spi * spiport, uint16 prescaler, uint16_t cpol, uint16_t cpha,  uint16_t firstbit) {
	switch(prescaler) {
		case SPI_BaudRatePrescaler_2:
		case SPI_BaudRatePrescaler_4:
		case SPI_BaudRatePrescaler_8:
		case SPI_BaudRatePrescaler_16:
		case SPI_BaudRatePrescaler_32:
		case SPI_BaudRatePrescaler_64:
		case SPI_BaudRatePrescaler_256:
		break;
		case SPI_BaudRatePrescaler_128:
		default:
			prescaler = SPI_BaudRatePrescaler_128;
	}
	
//	spiport->initStruct..SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//	spiport->initStruct..SPI_Mode = SPI_Mode_Master;
//	spiport->initStruct..SPI_DataSize = SPI_DataSize_8b;
	spiport->modedef.SPI_CPOL = (cpol == SPI_CPOL_Low ? SPI_CPOL_Low : SPI_CPOL_High);
	spiport->modedef.SPI_CPHA = (cpha == SPI_CPHA_1Edge? SPI_CPHA_1Edge : SPI_CPHA_2Edge);
//	spiport->initStruct..SPI_NSS = SPI_NSS_Soft;
	spiport->modedef.SPI_BaudRatePrescaler = prescaler;
	spiport->modedef.SPI_FirstBit = (firstbit == SPI_FirstBit_MSB ? SPI_FirstBit_MSB : SPI_FirstBit_LSB);
//	spiport->initStruct..SPI_CRCPolynomial = SPI_CRC_Rx;

	SPI_Init(spiport->SPIx, &spiport->modedef);
}

void spi_setDataMode(spi * spiport, uint16 modeid) {
	spiport->modedef.SPI_CPOL = (modeid & 2 ? SPI_CPOL_High : SPI_CPOL_Low);
	spiport->modedef.SPI_CPHA = (modeid & 1 ? SPI_CPHA_2Edge : SPI_CPHA_1Edge );
	SPI_Init(spiport->SPIx, &spiport->modedef);
}

void spi_setClockDivier(spi * spiport, uint16 id) {
	switch(id) {
		case SPI_BaudRatePrescaler_2:
		case SPI_BaudRatePrescaler_4:
		case SPI_BaudRatePrescaler_8:
		case SPI_BaudRatePrescaler_16:
		case SPI_BaudRatePrescaler_32:
		case SPI_BaudRatePrescaler_64:
		case SPI_BaudRatePrescaler_256:
		break;
		case SPI_BaudRatePrescaler_128:
		default:
			id = SPI_BaudRatePrescaler_128;
	}
	spiport->modedef.SPI_BaudRatePrescaler = id;
	SPI_Init(spiport->SPIx, &spiport->modedef);
}

void spi_setBitOrder(spi * spiport, uint16 id) {
	spiport->modedef.SPI_FirstBit = (id == SPI_FirstBit_MSB ? SPI_FirstBit_MSB : SPI_FirstBit_LSB);	
	SPI_Init(spiport->SPIx, &spiport->modedef);
}


uint16 spi_transfer(spi * spiport, uint16 data) {
	/* Wait for SPIx Tx buffer empty */
	while (SPI_I2S_GetFlagStatus(spiport->SPIx, SPI_I2S_FLAG_TXE ) == RESET) ;

	SPI_I2S_SendData(spiport->SPIx, data);
	/* Wait for SPIx data reception */

	while (SPI_I2S_GetFlagStatus(spiport->SPIx, SPI_I2S_FLAG_RXNE ) == RESET) ;
	/* Read SPIy received data */

	return SPI_I2S_ReceiveData(spiport->SPIx);
}
