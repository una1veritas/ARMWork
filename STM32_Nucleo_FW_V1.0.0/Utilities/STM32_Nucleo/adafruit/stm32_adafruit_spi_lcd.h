/**
  ******************************************************************************
  * @file    stm32_adafruit_spi_lcd.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   This file contains all the functions prototypes for the 
  *          stm32_adafruit_spi_lcd.c driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#ifndef __STM32_ADAFRUIT_SPI_LCD_H
#define __STM32_ADAFRUIT_SPI_LCD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "fonts.h"
#include "main.h" /* for Delay() prototype */

/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32_ADAFRUIT
  * @{
  */
    
/** @defgroup STM32_ADAFRUIT_SPI_LCD 
  * @brief This file includes the ADAFRUIT LCD driver  
  * @{
  */ 

/** @defgroup ADAFRUIT_SPI_LCD_Exported_Constants
  * @{
  */ 
   
#define _delay_     Delay  /* !< Application code should implement Delay() function,
                                   with 1ms time base (using SysTick) */

#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001    
/** 
  * @brief  LCD Registers  
  */ 
#define SWRESET         0x01 
#define SLPOUT          0x11 
#define DISPON          0x29 
#define CASET           0x2A 
#define RASET           0x2B 
#define RAMWR           0x2C 
#define MADCTL          0x36 
#define COLMOD          0x3A

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1


/** 
  * @brief LCD default font 
  */ 
#define LCD_DEFAULT_FONT        Font8x8

/** 
  * @brief  LCD color  
  */ 
#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

/** 
  * @brief  LCD Lines depending on the chosen fonts.  
  */
#define LCD_LINE_0               LINE(0)
#define LCD_LINE_1               LINE(1)
#define LCD_LINE_2               LINE(2)
#define LCD_LINE_3               LINE(3)
#define LCD_LINE_4               LINE(4)
#define LCD_LINE_5               LINE(5)
#define LCD_LINE_6               LINE(6)
#define LCD_LINE_7               LINE(7)
#define LCD_LINE_8               LINE(8)
#define LCD_LINE_9               LINE(9)
#define LCD_LINE_10              LINE(10)
#define LCD_LINE_11              LINE(11)
#define LCD_LINE_12              LINE(12)
#define LCD_LINE_13              LINE(13)
#define LCD_LINE_14              LINE(14)
#define LCD_LINE_15              LINE(15)
#define LCD_LINE_16              LINE(16)
#define LCD_LINE_17              LINE(17)
#define LCD_LINE_18              LINE(18)
#define LCD_LINE_19              LINE(19)


/** 
  * @brief  LCD Size (Width and Height)  
  */ 
#define LCD_PIXEL_WIDTH          0x0080
#define LCD_PIXEL_HEIGHT         0x00A0
/**
  * @}
  */ 

/** @defgroup ADAFRUIT_SPI_LCD_Exported_Functions
  * @{
  */ 
void LCD_ADAFRUIT_Init(void);
void LCD_Setup(void);
void LCD_WriteCommand(uint8_t LCD_Reg);
void LCD_WriteData(uint8_t value);
void LCD_WriteRAM(uint8_t Xpos, uint8_t Ypos, uint32_t color);
void LCD_DrawLine(uint8_t Xpos, uint8_t Ypos, uint8_t Lenght, uint8_t Direction);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width , uint16_t Height);
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width , uint16_t Height);
void LCD_SetDisplayWindow(uint8_t Xpos0, uint8_t Ypos0, uint8_t Xpos1, uint8_t Ypos1);
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c);
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii);
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr);
void LCD_SetTextColor(uint16_t Color);
void LCD_SetBackColor(uint16_t Color);
void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont(void);
void LCD_DrawCircle(int16_t Xpos, int16_t Ypos, int16_t Radius); 
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_Clear(uint16_t Color);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_ADAFRUIT_SPI_LCD_H */

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
