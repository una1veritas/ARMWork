/**
  ******************************************************************************
  * @file    stm32_adafruit_spi_lcd.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   This file includes the LCD driver for adafruit 1.8" TFT shield.  
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

/* Includes ------------------------------------------------------------------*/
#include "stm32_adafruit_spi_lcd.h"

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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/** @defgroup STM32_ADAFRUIT_SPI_LCD_Private_Variables
  * @{
  */ 
static sFONT *LCD_Currentfonts;
static uint16_t CurrentTextColor   = 0x0000;
static uint16_t CurrentBackColor   = 0xFFFF;

/**
  * @}
  */ 

/* Private function prototypes -----------------------------------------------*/

/** @defgroup STM32_ADAFRUIT_SPI_LCD_Private_Functions
  * @{
  */ 

/**
  * @brief  Initializes the LCD mounted on adafruit 1.8" TFT shield.
  * @note   Before to call this function, the application code must call STM_SPI_Init()
  *         to initialize the SPI Interface used to drive the LCD.      
  * @param  None
  * @retval None
  */
void LCD_ADAFRUIT_Init(void)
{
  /* Configure the LCD control lines (CS and DC) */  
  LCD_CtrlLines_Config();
  
  /* Configure the LCD registers configuration */
  LCD_Setup();
  
  /* Clear the LCD screen */
  LCD_Clear(LCD_COLOR_WHITE); 
  
  /* Set default font */    
  LCD_SetFont(&LCD_DEFAULT_FONT);   
}

/**
  * @brief  Setups the LCD registers configuration.
  * @param  None
  * @retval None
  */
void LCD_Setup(void)
{ 
  LCD_CS_LOW();
 
  LCD_WriteCommand(ST7735_SLPOUT); /* 2: Out of sleep mode, 0 args, w/delay 500 ms delay */
  _delay_(500);
  LCD_WriteCommand(ST7735_FRMCTR1); /* Frame rate ctrl - normal mode, 3 args:Rate = fosc/(1x2+40) * (LINE+2C+2D)*/
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D);
  LCD_WriteCommand(ST7735_FRMCTR2);/* 4: Frame rate control - idle mode, 3 args:Rate = fosc/(1x2+40) * (LINE+2C+2D) */
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D);  
  LCD_WriteCommand(ST7735_FRMCTR3);/* 5: Frame rate ctrl - partial mode, 6 args: Dot inversion mode, Line inversion mode */ 
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D);
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D);
  LCD_WriteCommand(ST7735_INVCTR);/* 6: Display inversion ctrl, 1 arg, no delay: No inversion */
  LCD_WriteData(0x07);
  LCD_WriteCommand(ST7735_PWCTR1);/* 7: Power control, 3 args, no delay: -4.6V , AUTO mode*/
  LCD_WriteData(0xA2);
  LCD_WriteData(0x02);
  LCD_WriteData(0x84);
  LCD_WriteCommand(ST7735_PWCTR2);/* 8: Power control, 1 arg, no delay: VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD */
  LCD_WriteData(0xC5);
  LCD_WriteCommand(ST7735_PWCTR3);/* 9: Power control, 2 args, no delay: Opamp current small, Boost frequency */
  LCD_WriteData(0x0A);
  LCD_WriteData(0x00);  
  LCD_WriteCommand(ST7735_PWCTR4);/* 10: Power control, 2 args, no delay: BCLK/2, Opamp current small & Medium low */
  LCD_WriteData(0x8A);
  LCD_WriteData(0x2A);
  LCD_WriteCommand(ST7735_PWCTR5);/* 11: Power control, 2 args, no delay */
  LCD_WriteData(0x8A);
  LCD_WriteData(0xEE);
  LCD_WriteCommand(ST7735_VMCTR1);/* 12: Power control, 1 arg, no delay */
  LCD_WriteData(0x0E);
  LCD_WriteCommand(ST7735_INVOFF);/* 13: Don't invert display, no args, no delay */
  LCD_WriteCommand(ST7735_MADCTL);/* 14: Memory access control (directions), 1 arg: row addr/col addr, bottom to top refresh */
  LCD_WriteData(0xC8);
  LCD_WriteCommand(ST7735_COLMOD);/* 15: set color mode, 1 arg, no delay: 16-bit color */
  LCD_WriteData(0x05);
  
  LCD_WriteCommand(ST7735_CASET);/* 16: Column addr set, 4 args, no delay: XSTART = 0, XEND = 127 */
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0x7F);
  LCD_WriteCommand(ST7735_RASET);/* 17: Row addr set, 4 args, no delay: XSTART = 0, XEND = 159 */
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0x9F);
  
  LCD_WriteCommand(ST7735_GMCTRP1);/* 18: Magical unicorn dust, 16 args, no delay */
  LCD_WriteData(0x02);  LCD_WriteData(0x1c);  LCD_WriteData(0x07);  LCD_WriteData(0x12);
  LCD_WriteData(0x37);  LCD_WriteData(0x32);  LCD_WriteData(0x29);  LCD_WriteData(0x2d);
  LCD_WriteData(0x29);  LCD_WriteData(0x25);  LCD_WriteData(0x2B);  LCD_WriteData(0x39);  
  LCD_WriteData(0x00);  LCD_WriteData(0x01);  LCD_WriteData(0x03);  LCD_WriteData(0x10);
  
  LCD_WriteCommand(ST7735_GMCTRN1);/* 19: Sparkles and rainbows, 16 args, no delay */
  LCD_WriteData(0x03);  LCD_WriteData(0x1d);  LCD_WriteData(0x07);  LCD_WriteData(0x06);
  LCD_WriteData(0x2E);  LCD_WriteData(0x2C);  LCD_WriteData(0x29);  LCD_WriteData(0x2D);
  LCD_WriteData(0x2E);  LCD_WriteData(0x2E);  LCD_WriteData(0x37);  LCD_WriteData(0x3F);  
  LCD_WriteData(0x00);  LCD_WriteData(0x00);  LCD_WriteData(0x02);  LCD_WriteData(0x10);
  
  LCD_WriteCommand(ST7735_NORON);/* 20: Normal display on, no args, w/delay: 10 ms delay*/
  _delay_(10);
  LCD_WriteCommand(ST7735_DISPON);/* 21: Main screen turn on, no args w/delay: 100 ms delay*/
  _delay_(10);
  
  LCD_WriteCommand(ST7735_MADCTL);
  LCD_WriteData(0xC0);
    
  LCD_CS_HIGH();
}

/**
  * @brief  Clears the hole LCD.
  * @param  Color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint16_t Color)
{
  LCD_SetTextColor(Color);
  LCD_DrawFullRect(0, 0, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT);
}

/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: specifies the X position, can be a value from 0 to 128.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 160.
  * @param  c: pointer to the character data.
  * @retval None
  */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, counter = 0, ypos =0;
  uint32_t  Xaddress = 0;
  
  Xaddress = Xpos;
  ypos = Ypos; 
  
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {
    for(counter = 0; counter < LCD_Currentfonts->Width; counter++)
    {
      
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> counter)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
         (((c[index] & (0x1 << counter)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))
      {
        /* Write back color to frame memory */
        LCD_WriteRAM(Ypos, Xaddress, CurrentBackColor);
        Ypos++;
      }
      else
      {
        /* Write data to frame memory */
        LCD_WriteRAM(Ypos, Xaddress, CurrentTextColor);
        Ypos++;
      }      
    }
    Xaddress++;
    Ypos = ypos;    
  }
}

/**
  * @brief  Writes to the LCD RAM.
  * @param  Xpos: specifies the X position, can be a value from 0 to 128.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 160.
  * @param  color: the pixel color in RGB mode (5-6-5).
  * @retval None
  */ 
void LCD_WriteRAM(uint8_t Xpos, uint8_t Ypos, uint32_t color)
{
  if((Xpos >= LCD_PIXEL_WIDTH) || (Ypos >= LCD_PIXEL_HEIGHT)) 
  {
    return;
  }
  
  LCD_SetDisplayWindow(Xpos, Ypos, Xpos, Ypos);
  LCD_CS_LOW();
  
  LCD_WriteData(color >> 8);
  LCD_WriteData(color);
  LCD_CS_HIGH();
}

/**
  * @brief  Displays one character.
  * @param  Line: the Line where to display the character shape
  *   This parameter can be one of the following values:
  *     @arg LCD_LINE_x: where x can be 0..19
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii)
{
  Ascii -= 32;
  
  LCD_DrawChar(Line, Column, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}

/**
  * @brief  Displays a list char on the LCD.
  * @param  Line: the Line where to display the String
  *   This parameter can be one of the following values:
  *     @arg LCD_LINE_x: where x can be 0..19
  * @param  *ptr: pointer to string to display on LCD.
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{  
  uint16_t refcolumn = 0;
  
  LCD_CS_LOW();  

  /* Send the string character by character on lCD */
  while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width)))
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, *ptr);
    /* Decrement the column position by width */
    refcolumn += LCD_Currentfonts->Width;
    /* Point on the next character */
    ptr++;
  }

  LCD_CS_HIGH();
}

/**
  * @brief  Sets the Text color.
  * @param  Color: specifies the Text color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}

/**
  * @brief  Sets the Background color.
  * @param  Color: specifies the Background color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetBackColor(uint16_t Color)
{
  CurrentBackColor = Color;
}

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/**
  * @brief  Gets the Text Font.
  * @param  None.
  * @retval the used font.
  */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}

/**
  * @brief  Set LCD Display Window.
  * @param  Xpos0: specifies the X position, must be less than Xpos1
  * @param  Ypos0: specifies the Y position, must be less than Ypos1
  * @param  Xpos1: specifies the X position, must be greater than Xpos0
  * @param  Ypos1: specifies the Y position, must be greater than Ypos0
  * @retval None
  */
void LCD_SetDisplayWindow(uint8_t Xpos0, uint8_t Ypos0, uint8_t Xpos1, uint8_t Ypos1)
{
  LCD_CS_LOW(); 
  
  LCD_WriteCommand(CASET);
  LCD_WriteData(Xpos0);
  LCD_WriteData(Xpos1);
  LCD_WriteCommand(RASET); 
  LCD_WriteData(Ypos0);
  LCD_WriteData(Ypos1);
  LCD_WriteCommand(RAMWR);
  
  LCD_CS_HIGH();
}

/**
  * @brief  Draws a line.
  * @param  Xpos: specifies the X position, can be a value from 0 to 128.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 160.
  * @param  Length: line length.
  * @param  Direction: line direction.
  *   This parameter can be one of the following values: LCD_DIR_HORIZONTAL or LCD_DIR_VERTICAL.
  * @retval None
  */
void LCD_DrawLine(uint8_t Xpos, uint8_t Ypos, uint8_t Lenght, uint8_t Direction)
{
  uint8_t counter;
  if (Direction == LCD_DIR_HORIZONTAL)
  {
    if(Xpos + Lenght > LCD_PIXEL_WIDTH) return;
    
    for(counter = 0; counter < Lenght; counter++)
    {
      LCD_WriteRAM(Xpos + counter, Ypos, CurrentTextColor);
    }
  }
  else
  {
    if(Ypos + Lenght > LCD_PIXEL_HEIGHT) return;
    for(counter = 0; counter < Lenght; counter++)
    {
      LCD_WriteRAM(Xpos, Ypos + counter, CurrentTextColor);
    } 
  }
}

/**
  * @brief  Displays a rectangle.
  * @param  Xpos: specifies the X position, can be a value from 0 to 128.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 160.
  * @param  Height: display rectangle height, can be a value from 0 to 160.
  * @param  Width: display rectangle width, can be a value from 0 to 128.
  * @retval None
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width , uint16_t Height)
{
  uint16_t x_value = 0, y_value = 0;
  x_value = Xpos + Width;
  y_value = Ypos + Height;
  
  /* draw horizontal lines */
  LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
  LCD_DrawLine(Xpos, y_value, Width, LCD_DIR_HORIZONTAL);    
  /* draw vertical lines */
  LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
  LCD_DrawLine(x_value, Ypos, Height, LCD_DIR_VERTICAL);
  LCD_WriteRAM(x_value, y_value, CurrentTextColor);
}


/**
  * @brief  Displays a full rectangle.
  * @param  Xpos: specifies the X position, can be a value from 0 to 128.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 160.
  * @param  Height: display rectangle height, can be a value from 0 to 160.
  * @param  Width: display rectangle width, can be a value from 0 to 128.
  * @retval None
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  uint16_t ycounter;

    for(ycounter = 0; ycounter < Height; ycounter++)
    {
      LCD_DrawLine(Xpos, Ypos + ycounter, Width, LCD_DIR_HORIZONTAL);
    }
}

/**
  * @brief  Draw a circle.
  * @param  Xpos: specifies the X position, can be a value from 0 to 128.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 160.
  * @param  Radius: radius of the circle.
  * @retval None
  */
void LCD_DrawCircle(int16_t Xpos, int16_t Ypos, int16_t Radius) 
{
  int16_t f = 1 - Radius;
  int16_t ddf_x = 1;
  int16_t ddf_y = -2 * Radius;
  int16_t xvalue = 0;
  int16_t yvalue = Radius;

  LCD_WriteRAM(Xpos  , Ypos + Radius, CurrentTextColor);
  LCD_WriteRAM(Xpos  , Ypos - Radius, CurrentTextColor);
  LCD_WriteRAM(Xpos + Radius, Ypos  , CurrentTextColor);
  LCD_WriteRAM(Xpos - Radius, Ypos  , CurrentTextColor);

  while (xvalue <yvalue) 
  {
    if (f >= 0) 
    {
      yvalue--;
      ddf_y += 2;
      f += ddf_y;
    }
    xvalue++;
    ddf_x += 2;
    f += ddf_x;
  
    LCD_WriteRAM(Xpos + xvalue, Ypos + yvalue, CurrentTextColor);
    LCD_WriteRAM(Xpos - xvalue, Ypos + yvalue, CurrentTextColor);
    LCD_WriteRAM(Xpos + xvalue, Ypos - yvalue, CurrentTextColor);
    LCD_WriteRAM(Xpos - xvalue, Ypos - yvalue, CurrentTextColor);
    LCD_WriteRAM(Xpos + yvalue, Ypos + xvalue, CurrentTextColor);
    LCD_WriteRAM(Xpos - yvalue, Ypos + xvalue, CurrentTextColor);
    LCD_WriteRAM(Xpos + yvalue, Ypos - xvalue, CurrentTextColor);
    LCD_WriteRAM(Xpos - yvalue, Ypos - xvalue, CurrentTextColor);
  }
}

/**
  * @brief  Displays a full circle.
  * @param  Xpos: specifies the X position, can be a value from 0 to 240.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
  * @param  Radius: radius of the circle.
  * @retval None
  */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  decision;    /* Decision Variable */ 
  uint32_t  cur_x;       /* Current X Value */
  uint32_t  cur_y;       /* Current Y Value */ 
  
  decision = 3 - (Radius << 1);
  
  cur_x = 0;
  cur_y = Radius;
  
  while (cur_x <= cur_y)
  {
    if(cur_y > 0) 
    {
      LCD_DrawLine(Xpos - cur_x, Ypos - cur_y, 2*cur_y, LCD_DIR_VERTICAL);
      LCD_DrawLine(Xpos + cur_x, Ypos - cur_y, 2*cur_y, LCD_DIR_VERTICAL);
    }
    
    if(cur_x > 0) 
    {
      LCD_DrawLine(Xpos - cur_y, Ypos - cur_x, 2*cur_x, LCD_DIR_VERTICAL);
      LCD_DrawLine(Xpos + cur_y, Ypos - cur_x, 2*cur_x, LCD_DIR_VERTICAL);
    }
    if (decision < 0)
    { 
      decision += (cur_x << 2) + 6;
    }
    else
    {
      decision += ((cur_x - cur_y) << 2) + 10;
      cur_y--;
    }
    cur_x++;
  }
  
  LCD_DrawCircle(Xpos, Ypos, Radius);  
}

/**
  * @brief  Writes command to select the LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void LCD_WriteCommand(uint8_t LCD_Reg)
{
  LCD_DC_LOW();

  STM_SPI_WriteRead(LCD_Reg);
}

/**
  * @brief  Writes data to select the LCD register.
  *         This function must be used after LCD_WriteCommand() function
  * @param  value: data to write to the selected register.
  * @retval None
  */
void LCD_WriteData(uint8_t value)
{
  LCD_DC_HIGH();
  
  STM_SPI_WriteRead(value);
}


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
