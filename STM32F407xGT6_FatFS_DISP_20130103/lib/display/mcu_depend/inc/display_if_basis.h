/********************************************************************************/
/*!
	@file			display_if_basis.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        5.00
    @date           2011.11.05
	@brief          Interface of Display Device Basics for STM32 uC.			@n
					"MCU Depend Layer"

    @section HISTORY
		2010.07.02	V1.00	Restart Here.
		2010.10.01	V2.00	Changed CTRL-Port Contol Procedure.
		2010.12.31	V3.00	Changed Some.
		2011.03.10	V4.00	C++ Ready.
		2011.11.05	V5.00	Improved Portability.
		
    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __DISPLAY_IF_BASIS_H
#define __DISPLAY_IF_BASIS_H	0x0500

#ifdef __cplusplus
 extern "C" {
#endif

/* Device or MCU Depend Includes */
#include "stm32f4xx.h"
#include "systick.h"

#ifndef BSRR
#define BSRR BSRRL			/* Set   */
#define BRR  BSRRH			/* Reset */
#endif

/* FreeRTOS compatibility */
#ifdef INC_FREERTOS_H
#define _delay_ms(x)    vTaskDelay(x/portTICK_RATE_MS);
#define _delay_us(x)    \
    for(volatile uint32_t Count=0;Count<=x;Count++){ 	\
						__NOP();				\
						__NOP();				\
						__NOP();				\
						__NOP();				\
						__NOP();				\
						__NOP();				\
						}
#define __SYSTICK_H
#endif



/* Bus Definition */
/* 16Bit-Bus TYPE PORT SETTINGS */
#if 	defined(USE_ILI932x_TFT)  || defined(USE_SSD2119_TFT)  || defined(USE_HX8347x_TFT)  || defined(USE_SSD1289_TFT)    || \
		defined(USE_R61509x_TFT)  || defined(USE_HX8352x_TFT)  || defined(USE_S1D19122_TFT) || defined(USE_ILI9481_TFT)    || \
		defined(USE_ILI9327_TFT)  || defined(USE_HX8312A_TFT)  || defined(USE_S6D0128_TFT)  || defined(USE_S6E63D6_OLED)   || \
		defined(USE_SSD1963_TFT)  || defined(USE_LGDP4511_TFT) || defined(USE_ILI9132_TFT)  || defined(USE_TL1771_TFT)     || \
		defined(USE_REL225L01_TFT)|| defined(USE_HD66772_TFT)  || defined(USE_ILI934x_TFT)  || defined(USE_HX5051_OLED)    || \
		defined(USE_S6D0144_TFT)  || defined(USE_HX8345A_TFT)  || defined(USE_R61526_TFT)   || defined(USE_D51E5TA7601_TFT)|| \
		defined(USE_S6D0117_TFT)  || defined(USE_HX8357A_TFT)  || defined(USE_ST7787_TFT)   || defined(USE_UPD161704A_TFT) || \
		defined(USE_SPFD54126_TFT)|| defined(USE_R61503U_TFT)  || defined(USE_HX8310A_TFT)  || defined(USE_S6D0154_TFT)    || \
		defined(USE_ILI9342_TFT)

#if defined(STM32F407ZGT6)
/* STM32F407ZGT6 FSMC_Bank1_NORSRAM4 Definition */
/* PORT :  15| 14| 13| 12| 11| 10|  9|  8|  7|  6|  5|  4|  3|  2|  1|  0 */ 
/* GPIOD: D1 |D0 |---|---|---|D15|D14|D13|CS |---|WR |RD |---|---|D3 |D2  */
/* GPIOE: D12|D11|D10|D9 |D8 |D7 |D6 |D5 |D4 |---|---|---|---|---|---|--- */
/* GPIOF: ---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|D/C */
/* GPIOG: ---|---|---|CS |---|---|---|RST|---|---|---|---|---|---|---|--- */
/*CTRL-Definitions*/
#define CTRL_RES				GPIO_Pin_8			/* GPIOG 8     */
#define CTRL_DC					GPIO_Pin_0			/* A0  as PF00 */
#define CTRL_CS					GPIO_Pin_12			/* NE4 as PG12 */
#define CTRL_WR					GPIO_Pin_5			/* NWE as PD05 */
#define CTRL_RD					GPIO_Pin_4			/* NOE as PD04 */
#define FSMC_DC					GPIO_PinSource0
#define FSMC_CS					GPIO_PinSource12
#define FSMC_WR					GPIO_PinSource5
#define FSMC_RD					GPIO_PinSource4

#define DISPLAY_PORT_RES		GPIOG
#define DISPLAY_PORT_DC			GPIOF
#define DISPLAY_PORT_CS			GPIOG
#define DISPLAY_PORT_WR			GPIOD
#define DISPLAY_PORT_RD			GPIOD

#define DISPLAY_CLK_RES			RCC_AHB1Periph_GPIOG
#define DISPLAY_CLK_DC			RCC_AHB1Periph_GPIOF
#define DISPLAY_CLK_CS			RCC_AHB1Periph_GPIOG

#else
	#error "16-bit bus is NOT supported on STM32F4Discovery!!"
#endif

#define DISPLAY_RES_SET()		DISPLAY_PORT_RES->BSRR  = CTRL_RES
#define DISPLAY_RES_CLR()		DISPLAY_PORT_RES->BRR   = CTRL_RES
#if 0 /* GPIO Bus EMU control (test only) */
#define DISPLAY_DC_SET()		DISPLAY_PORT_DC->BSRR  = CTRL_DC
#define DISPLAY_DC_CLR()        DISPLAY_PORT_DC->BRR   = CTRL_DC
#define DISPLAY_CS_SET()		DISPLAY_PORT_CS->BSRR  = CTRL_CS
#define DISPLAY_CS_CLR()        DISPLAY_PORT_CS->BRR   = CTRL_CS
#define DISPLAY_WR_SET()		DISPLAY_PORT_WR->BSRR  = CTRL_WR
#define DISPLAY_WR_CLR()        DISPLAY_PORT_WR->BRR   = CTRL_WR
#else
#define DISPLAY_DC_SET()		/*DISPLAY_PORT_DC->BSRR  = CTRL_DC*/
#define DISPLAY_DC_CLR()        /*DISPLAY_PORT_DC->BRR   = CTRL_DC*/
#define DISPLAY_CS_SET()		/*DISPLAY_PORT_CS->BSRR  = CTRL_CS*/
#define DISPLAY_CS_CLR()        /*DISPLAY_PORT_CS->BRR   = CTRL_CS*/
#define DISPLAY_WR_SET()		/*DISPLAY_PORT_WR->BSRR  = CTRL_WR*/
#define DISPLAY_WR_CLR()        /*DISPLAY_PORT_WR->BRR   = CTRL_WR*/
#endif
/* keep more than 150nSec */
#define DISPLAY_RD_SET()		DISPLAY_PORT_RD->BSRR  = CTRL_RD;DISPLAY_PORT_RD->BSRR  = CTRL_RD;DISPLAY_PORT_RD->BSRR  = CTRL_RD
#define DISPLAY_RD_CLR()        DISPLAY_PORT_RD->BRR   = CTRL_RD;DISPLAY_PORT_RD->BRR   = CTRL_RD;DISPLAY_PORT_RD->BRR   = CTRL_RD



/*DATA-Definitions*/
#define DISPLAY_PORT_DATA
#define DISPLAY_CLK_DATA

/* LCD addresses as seen by the FSMC*/
#define LCD_FSMC_BANKSELECT		FSMC_Bank1_NORSRAM4
#define LCD_CMD_MODE_ADDR   	((uint32_t)(0x60000000 | 0x0C000000))
#define LCD_DATA_MODE_ADDR   	((uint32_t)(0x60000000 | 0x0C000002))


/* LCD is connected to the FSMC Bank1 NOR/SRAMx and NEx is used as ship select signal */
#define DISPLAY_DATAPORT		(*(volatile uint16_t*)LCD_DATA_MODE_ADDR)
#define DISPLAY_CMDPORT			(*(volatile uint16_t*)LCD_CMD_MODE_ADDR)



/* Define Acess Procedure */
#define BUS_ACCESS_16BIT

/* PORTD */
#define LCD_D0      			GPIO_Pin_14
#define LCD_D1      			GPIO_Pin_15
#define LCD_D2      			GPIO_Pin_0
#define LCD_D3      			GPIO_Pin_1
#define LCD_D13     			GPIO_Pin_8
#define LCD_D14     			GPIO_Pin_9
#define LCD_D15     			GPIO_Pin_10
/* PORTE */
#define LCD_D4      			GPIO_Pin_7
#define LCD_D5      			GPIO_Pin_8
#define LCD_D6      			GPIO_Pin_9
#define LCD_D7      			GPIO_Pin_10
#define LCD_D8      			GPIO_Pin_11
#define LCD_D9      			GPIO_Pin_12
#define LCD_D10     			GPIO_Pin_13
#define LCD_D11     			GPIO_Pin_14
#define LCD_D12     			GPIO_Pin_15

#define DATA_PINS

#ifdef BUS_ACCESS_16BIT
/*#define USE_DISPLAY_DMA_TRANSFER*/
#endif
#define FSMCLCD_DMA_STREAM_CLOCK 	RCC_AHB1Periph_DMA2
#define FSMCLCD_DMA_CHANNEL_TX		DMA_Channel_0
#define FSMCLCD_DMA_STREAM_TX     	DMA2_Stream0
#define FSMCLCD_DMA_FLAG_TX_TC      DMA_FLAG_TCIF0
extern void lcd_dma_transfer(const uint8_t *buff,unsigned int btr);
#define DMA_TRANSACTION				lcd_dma_transfer


#if defined(USE_HX8312A_TFT) || defined(USE_S1D19105_TFT)
#define ReadLCDData(x)					\
	x = DISPLAY_CMDPORT;
#else
#define ReadLCDData(x)					\
	x = DISPLAY_DATAPORT;
#endif





/* Bus Definition */
/*  8Bit-Bus TYPE PORT SETTINGS */
#elif	defined(USE_ST7735_TFT)   || defined(USE_SSD1339_OLED) || defined(USE_HX8340B_TFT)  || defined(USE_ILI9225x_TFT) || \
		defined(USE_R61514S_TFT)  || defined(USE_SEPS525_OLED) || defined(USE_ST7735R_TFT)  || defined(USE_S1D19105_TFT) || \
		defined(USE_SSD1286A_TFT) || defined(USE_SPFD54124_TFT)|| defined(USE_LGDP452x_TFT) || defined(USE_S6D0129_TFT)	 || \
		defined(USE_ILI9163x_TFT) || defined(USE_C1L5_06_TFT)  || defined(USE_HX8309A_TFT)  || defined(USE_S6B33B6x_CSTN)|| \
		defined(USE_SSD1351_OLED) || defined(USE_C1E2_04_TFT)  || defined(USE_HX8353x_TFT)  || defined(USE_R61506_TFT)

/* STM32F407ZGT6 FSMC_Bank1_NORSRAM4 Definition */
/* PORT :  15| 14| 13| 12| 11| 10|  9|  8|  7|  6|  5|  4|  3|  2|  1|  0 */ 
/* GPIOD: D1 |D0 |---|---|---|D15|D14|D13|CS |---|WR |RD |---|---|D3 |D2  */
/* GPIOE: D12|D11|D10|D9 |D8 |D7 |D6 |D5 |D4 |---|---|---|---|---|---|RST */
/* GPIOF: ---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|D/C */
/* GPIOG: ---|---|---|CS |---|---|---|RST|---|---|---|---|---|---|---|--- */
/*CTRL-Definitions*/
#if defined(STM32F407ZGT6)
#define CTRL_RES				GPIO_Pin_8			/* GPIOG 8     */
#define CTRL_DC					GPIO_Pin_0			/* A0  as PF00 */
#define CTRL_CS					GPIO_Pin_12			/* NE4 as PG12 */
#define CTRL_WR					GPIO_Pin_5			/* NWE as PD05 */
#define CTRL_RD					GPIO_Pin_4			/* NOE as PD04 */
#define FSMC_DC					GPIO_PinSource0
#define FSMC_CS					GPIO_PinSource12
#define FSMC_WR					GPIO_PinSource5
#define FSMC_RD					GPIO_PinSource4

#define DISPLAY_PORT_RES		GPIOG
#define DISPLAY_PORT_DC			GPIOF
#define DISPLAY_PORT_CS			GPIOG
#define DISPLAY_PORT_WR			GPIOD
#define DISPLAY_PORT_RD			GPIOD

#define DISPLAY_CLK_RES			RCC_AHB1Periph_GPIOG
#define DISPLAY_CLK_DC			RCC_AHB1Periph_GPIOF
#define DISPLAY_CLK_CS			RCC_AHB1Periph_GPIOG

#else
	#error "8-bit bus is NOT supported on STM32F4Discovery!!"
#endif

#define DISPLAY_RES_SET()		DISPLAY_PORT_RES->BSRR  = CTRL_RES
#define DISPLAY_RES_CLR()		DISPLAY_PORT_RES->BRR   = CTRL_RES
#define DISPLAY_DC_SET()		/*__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();*/
#define DISPLAY_DC_CLR()        
#define DISPLAY_CS_SET()		
#define DISPLAY_CS_CLR()        
#define DISPLAY_WR_SET()		
#define DISPLAY_WR_CLR()        
/* keep more than 150nSec */
#define DISPLAY_RD_SET()		DISPLAY_PORT_RD->BSRR  = CTRL_RD;DISPLAY_PORT_RD->BSRR  = CTRL_RD;DISPLAY_PORT_RD->BSRR  = CTRL_RD
#define DISPLAY_RD_CLR()        DISPLAY_PORT_RD->BRR   = CTRL_RD;DISPLAY_PORT_RD->BRR   = CTRL_RD;DISPLAY_PORT_RD->BRR   = CTRL_RD



/*DATA-Definitions*/
#define DISPLAY_PORT_DATA
#define DISPLAY_CLK_DATA

/* LCD addresses as seen by the FSMC*/
#define LCD_FSMC_BANKSELECT		FSMC_Bank1_NORSRAM4
#define LCD_CMD_MODE_ADDR   	((uint32_t)(0x60000000 | 0x0C000000))
#define LCD_DATA_MODE_ADDR   	((uint32_t)(0x60000000 | 0x0C000002))


/* LCD is connected to the FSMC Bank1 NOR/SRAMx and NEx is used as ship select signal */
#define DISPLAY_DATAPORT		(*(volatile uint16_t*)LCD_DATA_MODE_ADDR)
#define DISPLAY_CMDPORT			(*(volatile uint16_t*)LCD_CMD_MODE_ADDR)



/* Define Acess Procedure */
#define BUS_ACCESS_8BIT

/* PORTD */
#define LCD_D0      			GPIO_Pin_14
#define LCD_D1      			GPIO_Pin_15
#define LCD_D2      			GPIO_Pin_0
#define LCD_D3      			GPIO_Pin_1
#define LCD_D13     			GPIO_Pin_8
#define LCD_D14     			GPIO_Pin_9
#define LCD_D15     			GPIO_Pin_10
/* PORTE */
#define LCD_D4      			GPIO_Pin_7
#define LCD_D5      			GPIO_Pin_8
#define LCD_D6      			GPIO_Pin_9
#define LCD_D7      			GPIO_Pin_10
#define LCD_D8      			GPIO_Pin_11
#define LCD_D9      			GPIO_Pin_12
#define LCD_D10     			GPIO_Pin_13
#define LCD_D11     			GPIO_Pin_14
#define LCD_D12     			GPIO_Pin_15

#define DATA_PINS

#ifdef BUS_ACCESS_8BIT
#define USE_DISPLAY_DMA_TRANSFER
#endif
#define FSMCLCD_DMA_STREAM_CLOCK 	RCC_AHB1Periph_DMA2
#define FSMCLCD_DMA_CHANNEL_TX		DMA_Channel_0
#define FSMCLCD_DMA_STREAM_TX     	DMA2_Stream0
#define FSMCLCD_DMA_FLAG_TX_TC      DMA_FLAG_TCIF0
extern void lcd_dma_transfer(const uint8_t *buff,unsigned int btr);
#define DMA_TRANSACTION				lcd_dma_transfer


#if defined(USE_HX8312A_TFT) || defined(USE_S1D19105_TFT)
#define ReadLCDData(x)					\
	x = DISPLAY_CMDPORT;
#else
#define ReadLCDData(x)					\
	x = DISPLAY_DATAPORT;
#endif



/* Bus Definition */
/* Serial Bus TYPE PORT SETTINGS */
#elif 	defined(USE_SSD1283A_SPI_TFT) || defined(USE_ST7735_SPI_TFT)  || defined(USE_SSD1332_SPI_OLED) || defined(USE_ILI932x_SPI_TFT)   || \
		defined(USE_SEPS525_SPI_OLED) || defined(USE_ST7735R_SPI_TFT) || defined(USE_S6E63D6_SPI_OLED) || defined(USE_ILI9163x_SPI_TFT)  || \
		defined(USE_HX8347x_SPI_TFT)  || defined(USE_SSD2119_SPI_TFT) || defined(USE_SPFD54124_SPI_TFT)|| defined(USE_SSD1339_SPI_OLED)  || \
		defined(USE_ILI934x_SPI_TFT)  || defined(USE_HX8340B_SPI_TFT) || defined(USE_S6D0129_SPI_TFT)  || defined(USE_HX8340BN_SPI_TFT)  || \
		defined(USE_R61509x_SPI_TFT)  || defined(USE_S6D0144_SPI_TFT) || defined(USE_S6B33B6x_SPI_CSTN)|| defined(USE_SSD1351_SPI_OLED)  || \
		defined(USE_ILI9225x_SPI_TFT) || defined(USE_ILI9481_SPI_TFT) || defined(USE_UPD161704A_SPI_TFT)

/* Select SPI Protcol Handling Method */
/*#define USE_SOFTWARE_SPI*/
#define USE_HARDWARE_SPI

/* S**king 9-bit SPI-LCD WorkAround! */
#if defined(USE_SPFD54124_SPI_TFT) || defined(USE_HX8340BN_SPI_TFT)
 #undef  USE_HARDWARE_SPI
 #define USE_SOFTWARE_SPI	/* Force Software 9-bit SPI */
#endif


#if defined(STM32F407VGT6)
/* STM32F407VGT6_SOFTWARE_HARDWARE_SPI(MODE3) */
/* RES :PB0
   D/C :PB1
   CS  :PA15
   SCK :PB10
   MOSI:PB14
   MISO:PB15
*/

/*CTRL-Definitions*/
#define CTRL_RES				GPIO_Pin_0			/* Reset   as Output */
#define CTRL_DC					GPIO_Pin_1			/* D/C(RS) as Output */
#define CTRL_CS					GPIO_Pin_15			/* CS      as Output */
#define CTRL_SCK				GPIO_Pin_10			/* SCK     as Output */
#define CTRL_SDO				GPIO_Pin_14			/* SDO     as Input  */
#define CTRL_SDI				GPIO_Pin_15			/* SDI     as Output */

#define DISPLAY_SPI_PORT		SPI2
#define DISPLAY_PERIF_CLK(x)	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,x)
#define SPILCD_DMA_STREAM_CLOCK RCC_AHB1Periph_DMA1
#define SPILCD_DMA_CHANNEL_TX	DMA_Channel_0
#define SPILCD_DMA_STREAM_TX    DMA1_Stream4
#define SPILCD_DMA_FLAG_TX_TC   DMA_FLAG_TCIF4

#define GPIO_AF_SPIX			GPIO_AF_SPI2
#define SRC_CS					GPIO_PinSource15
#define SRC_SCK					GPIO_PinSource10
#define SRC_SDO					GPIO_PinSource14
#define SRC_SDI					GPIO_PinSource15

#define DISPLAY_PORT_RES		GPIOB
#define DISPLAY_PORT_DC			GPIOB
#define DISPLAY_PORT_CS			GPIOA
#define DISPLAY_PORT_SCK		GPIOB
#define DISPLAY_PORT_SDO		GPIOB
#define DISPLAY_PORT_SDI		GPIOB

#define DISPLAY_CLK_RES			RCC_AHB1Periph_GPIOB
#define DISPLAY_CLK_DC			RCC_AHB1Periph_GPIOB
#define DISPLAY_CLK_CS			RCC_AHB1Periph_GPIOA
#define DISPLAY_CLK_SCK			RCC_AHB1Periph_GPIOB
#define DISPLAY_CLK_SDO			RCC_AHB1Periph_GPIOB
#define DISPLAY_CLK_SDI    		RCC_AHB1Periph_GPIOB

#elif defined(STM32F407ZGT6)
/* STM32F407ZGT6_SOFTWARE_SPI(MODE3) */
/* RES :PG8
   D/C :PG7
   CS  :PB12
   SCK :PB13
   MISO:PB14
   MOSI:PB15
*/

/*CTRL-Definitions*/
#define CTRL_RES				GPIO_Pin_8			/* Reset   as Output */
#define CTRL_DC					GPIO_Pin_7			/* D/C(RS) as Output */
#define CTRL_CS					GPIO_Pin_12			/* CS      as Output */
#define CTRL_SCK				GPIO_Pin_13			/* SCK     as Output */
#define CTRL_SDO				GPIO_Pin_14			/* SDO     as Input  */
#define CTRL_SDI				GPIO_Pin_15			/* SDI     as Output */

#define DISPLAY_SPI_PORT		SPI2
#define DISPLAY_PERIF_CLK(x)	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,x)
#define SPILCD_DMA_STREAM_CLOCK RCC_AHB1Periph_DMA1
#define SPILCD_DMA_CHANNEL_TX	DMA_Channel_0
#define SPILCD_DMA_STREAM_TX    DMA1_Stream4
#define SPILCD_DMA_FLAG_TX_TC   DMA_FLAG_TCIF4

#define GPIO_AF_SPIX			GPIO_AF_SPI2
#define SRC_CS					GPIO_PinSource12
#define SRC_SCK					GPIO_PinSource13
#define SRC_SDO					GPIO_PinSource14
#define SRC_SDI					GPIO_PinSource15

#define DISPLAY_PORT_RES		GPIOG
#define DISPLAY_PORT_DC			GPIOG
#define DISPLAY_PORT_CS			GPIOB
#define DISPLAY_PORT_SCK		GPIOB
#define DISPLAY_PORT_SDO		GPIOB
#define DISPLAY_PORT_SDI		GPIOB

#define DISPLAY_CLK_RES			RCC_AHB1Periph_GPIOG
#define DISPLAY_CLK_DC			RCC_AHB1Periph_GPIOG
#define DISPLAY_CLK_CS			RCC_AHB1Periph_GPIOB
#define DISPLAY_CLK_SCK			RCC_AHB1Periph_GPIOB
#define DISPLAY_CLK_SDO			RCC_AHB1Periph_GPIOB
#define DISPLAY_CLK_SDI    		RCC_AHB1Periph_GPIOB

#endif


#define DISPLAY_RES_SET()		(DISPLAY_PORT_RES->BSRR = CTRL_RES)
#define DISPLAY_RES_CLR()		(DISPLAY_PORT_RES->BRR  = CTRL_RES)
#define DISPLAY_DC_SET()		(DISPLAY_PORT_DC->BSRR  = CTRL_DC)
#define DISPLAY_DC_CLR()        (DISPLAY_PORT_DC->BRR   = CTRL_DC)
#define DISPLAY_CS_SET()		(DISPLAY_PORT_CS->BSRR  = CTRL_CS)
#define DISPLAY_CS_CLR()        (DISPLAY_PORT_CS->BRR   = CTRL_CS)
#define DISPLAY_SCK_SET()		(DISPLAY_PORT_SCK->BSRR = CTRL_SCK)
#define DISPLAY_SCK_CLR()       (DISPLAY_PORT_SCK->BRR  = CTRL_SCK)
#define DISPLAY_SDI_SET()		(DISPLAY_PORT_SDI->BSRR = CTRL_SDI)
#define DISPLAY_SDI_CLR()     	(DISPLAY_PORT_SDI->BRR  = CTRL_SDI)
#define DISPLAY_SDO_SET()		(DISPLAY_PORT_SDO->BSRR = CTRL_SDO)
#define DISPLAY_SDO_CLR()     	(DISPLAY_PORT_SDO->BRR  = CTRL_SDO)

#define SDI_HI()				(DISPLAY_SDI_SET())
#define SDI_LO()				(DISPLAY_SDI_CLR())
#define SCK_HI()				(DISPLAY_SCK_SET())
#define SCK_LO()				(DISPLAY_SCK_CLR())
#define CLK_OUT()				SCK_LO();__NOP();__NOP();	\
								SCK_HI();__NOP();
#define CLK_OUT_RD()			SCK_HI();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); \
								SCK_LO();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP()
#define SDO_CHK()				((DISPLAY_PORT_SDO->IDR & CTRL_SDO) ? 1:0)

#define DISPLAY_ASSART_CS()		DISPLAY_CS_CLR();//DISPLAY_CS_CLR();DISPLAY_CS_CLR()
#define DISPLAY_NEGATE_CS()		DISPLAY_CS_SET();//DISPLAY_CS_SET();DISPLAY_CS_SET()

#ifdef USE_SOFTWARE_SPI
/* Send 8-bit Data Macro */
#define SendSPI(x)							\
		(0x80 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x40 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x20 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x10 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x08 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x04 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x02 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x01 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();
/* Send 16-bit Data Macro */
#define SendSPI16(x)						\
		(0x8000 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x4000 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x2000 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x1000 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0800 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0400 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0200 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0100 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0080 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0040 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0020 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0010 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0008 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0004 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0002 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();							\
		(0x0001 & x) ? SDI_HI() : SDI_LO();	\
		CLK_OUT();
/* Get 18-bit Data Inline Function */
inline static uint8_t RecvSPI(void){

	uint8_t r = 0;

	/* First Fallin' Clock was executed at Last Clock of SendSPIx(x) */
	SDI_HI();				/* Lock SDI 0xFF */
	CLK_OUT_RD();
	if (SDO_CHK()) r++;		/* Sample D7 */
	r <<= 1; CLK_OUT_RD();
	if (SDO_CHK()) r++;		/* Sample D6 */
	r <<= 1; CLK_OUT_RD();
	if (SDO_CHK()) r++;		/* Sample D5 */
	r <<= 1; CLK_OUT_RD();
	if (SDO_CHK()) r++;		/* Sample D4 */
	r <<= 1; CLK_OUT_RD();
	if (SDO_CHK()) r++;		/* Sample D3 */
	r <<= 1; CLK_OUT_RD();
	if (SDO_CHK()) r++;		/* Sample D2 */
	r <<= 1; CLK_OUT_RD();
	if (SDO_CHK()) r++;		/* Sample D1 */
	r <<= 1; CLK_OUT_RD();
	if (SDO_CHK()) r++;		/* Sample D0 */
	SCK_HI();				/* Reset SCK to HI (SPI Mode3)*/

	return r;
}
#else

#ifdef USE_HARDWARE_SPI
#define USE_DISPLAY_DMA_TRANSFER
#endif
extern void lcd_dma_transfer(const uint8_t *buff,unsigned int btr);
#define DMA_TRANSACTION				lcd_dma_transfer
extern void SendSPI(uint8_t dat);
extern void SendSPI16(uint16_t dat);
extern uint8_t RecvSPI(void);
#endif

#endif
 

/* RGB565 Colour Structure */
/* e.g RED-PINK        | Blue     | Green           | Red               */
/* #define FrontG      ((50 >> 3) |((50 >> 2) << 5) |((255 >> 3) << 11))*/
#if !defined(USE_SSD1332_SPI_OLED)
	#define RGB(r,g,b)	(uint16_t)((b >> 3) |((g >> 2) << 5) |((r >> 3) << 11))
#else /* USE_SSD1332_SPI_OLED */
	#ifdef USE_HARDWARE_SPI
		#define SSD1332_PUSH_CLK()	SendSPI(0)
	#else
		#define SSD1332_PUSH_CLK()	CLK_OUT()
	#endif
	#define RGB(r,g,b)	(uint16_t)((r >> 3) |((g >> 2) << 5) |((b >> 3) << 11))
#endif


#define FrontG		COL_REDPINK
#define BackG   	COL_BLACK

#define COL_BLACK	RGB(0,0,0)
#define COL_WHITE	RGB(255,255,255)
#define COL_RED		RGB(255,0,0)
#define COL_BLUE	RGB(0,0,255)
#define COL_GREEN	RGB(0,255,0)

#define COL_YELLOW	RGB(255,255,0)
#define COL_MAGENTA	RGB(255,0,255)
#define COL_AQUA	RGB(0,255,255)

#define COL_PURPLE	RGB(160,32,240)
#define COL_REDPINK RGB(255,50,50)
#define COL_ORANGE  RGB(255,165,0)

/* Potiner Access Macro(Little Endian) */
#define	LD_UINT16(ptr)		(uint16_t)(((uint16_t)*(uint8_t*)((ptr)+1)<<8)|(uint16_t)*(uint8_t*)(ptr))
#define	LD_UINT32(ptr)		(uint32_t)(((uint32_t)*(uint8_t*)((ptr)+3)<<24)|((uint32_t)*(uint8_t*)((ptr)+2)<<16)|((uint16_t)*(uint8_t*)((ptr)+1)<<8)|*(uint8_t*)(ptr))

/* Function Prototypes */
extern void Display_IoInit_If(void);

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_IF_BASIS_H */
