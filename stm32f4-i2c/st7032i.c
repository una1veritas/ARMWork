/**
  ******************************************************************************
  * @file    lib_std/UTIL/inc/st7032i.c
  * @author  Yasuo Kawachi
  * @version V1.0.0
  * @date    04/15/2009
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * Copyright 2008-2009 Yasuo Kawachi All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *  1. Redistributions of source code must retain the above copyright notice,
  *  this list of conditions and the following disclaimer.
  *  2. Redistributions in binary form must reproduce the above copyright notice,
  *  this list of conditions and the following disclaimer in the documentation
  *  and/or other materials provided with the distribution.
  *
  * THIS SOFTWARE IS PROVIDED BY YASUO KAWACHI "AS IS" AND ANY EXPRESS OR IMPLIE  D
  * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  * EVENT SHALL YASUO KAWACHI OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  * This software way contain the part of STMicroelectronics firmware.
  * Below notice is applied to the part, but the above BSD license is not.
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * COPYRIGHT 2009 STMicroelectronics
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"

#include "st7032i.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO int8_t DDRAM_Data[ROWS][COLUMNS];
__IO uint8_t DDRAM_Address = 0;
__IO uint8_t Entry_Mode = 0b00000110;
__IO int8_t Increment = 1;
__IO int8_t Shift = 0;
__IO uint8_t Display = 0b00001100;
__IO uint8_t Power_Icon_Contrast =0b01011100;
__IO uint8_t Contrast = 40;
__IO uint8_t IconRAM[16];
__IO uint8_t Icon_Table[13][2] = {
    {0x00, 0b10000},
    {0x02, 0b10000},
    {0x04, 0b10000},
    {0x06, 0b10000},
    {0x07, 0b10000},
    {0x07, 0b01000},
    {0x09, 0b10000},
    {0x0B, 0b10000},
    {0x0D, 0b10000},
    {0x0D, 0b01000},
    {0x0D, 0b00100},
    {0x0D, 0b00010},
    {0x0F, 0b10000}};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Clear Display
  * @param  None
  * @retval None
  */
void ST7032i_Clear(void)
{
  uint32_t i;

  ST7032i_Command_Write(0b00000001);
  DDRAM_Address = 0;
  for(i=0;i<16;i++)
    {
      DDRAM_Data[0][i] =  ' ';
      DDRAM_Data[1][i] =  ' ';
    }
  delay_ms(2);
}

/**
  * @brief  Return to home position
  * @param  None
  * @retval None
  */
void ST7032i_Return(void)
{
  ST7032i_Command_Write(0b00000010);
  DDRAM_Address = 0;
  delay_ms(2);
}

/**
  * @brief  increment address when data is send and put
  * @param  None
  * @retval None
  */
void ST7032i_Increment(void)
{
  ST7032i_Command_Write(( Entry_Mode & 0b11111101) | 0b00000010);
  Increment = 1;
  delay_us(27);
}

/**
  * @brief  decrement address when data is send and put
  * @param  None
  * @retval None
  */
void ST7032i_Decrement(void)
{
  ST7032i_Command_Write(( Entry_Mode & 0b11111101) | 0b00000000);
  Increment = -1;
  delay_us(27);
}

/**
  * @brief  shift entire display data is send
  * @param  None
  * @retval None
  */
void ST7032i_Shift_Enable(void)
{
  ST7032i_Command_Write(( Entry_Mode & 0b11111110) | 0b00000001);
  Shift = 1;
  delay_us(27);
}

/**
  * @brief  disables shift fucntion
  * @param  None
  * @retval None
  */
void ST7032i_Shift_Disable(void)
{
  ST7032i_Command_Write(( Entry_Mode & 0b11111110) | 0b00000000);
  Shift = 0;
  delay_us(27);
}

/**
  * @brief  put on display
  * @param  None
  * @retval None
  */
void ST7032i_Display_On(void)
{
  ST7032i_Command_Write(( Display & 0b11111011) | 0b00000100);
  delay_us(27);
}

/**
  * @brief  put off display while DDRAM is kept
  * @param  None
  * @retval None
  */
void ST7032i_Display_Off(void)
{
  ST7032i_Command_Write(( Display & 0b11111011) | 0b00000000);
  delay_us(27);
}

/**
  * @brief  display under line cursor
  * @param  None
  * @retval None
  */
void ST7032i_Cursor_On(void)
{
  ST7032i_Command_Write(( Display & 0b11111101) | 0b00000010);
  delay_us(27);
}

/**
  * @brief  put off cursor
  * @param  None
  * @retval None
  */
void ST7032i_Cursor_Off(void)
{
  ST7032i_Command_Write(( Display & 0b11111101) | 0b00000000);
  delay_us(27);
}

/**
  * @brief  make square cursor brink
  * @param  None
  * @retval None
  */
void ST7032i_Cursor_Blink_On(void)
{
  ST7032i_Command_Write(( Display & 0b11111110) | 0b00000001);
  delay_us(27);
}

/**
  * @brief  display square cursor
  * @param  None
  * @retval None
  */
void ST7032i_Cursor_Blink_Off(void)
{
  ST7032i_Command_Write(( Display & 0b11111110) | 0b00000000);
  delay_us(27);
}

/**
  * @brief  set DDRAM address
  * @param  address : DDRAM address
  * @retval None
  */
void ST7032i_Set_DDRAM(uint8_t address)
{
  ST7032i_Command_Write(0b10000000 | address);
  DDRAM_Address = address;
  delay_us(27);
}

/**
  * @brief  Set display contrast. value is to be 0 - 63
  * @param  contrast: contrast
  * @retval None
  */
void ST7032i_Set_Contrast(uint8_t contrast)
{
  //Contrast set
  ST7032i_Command_Write(0b01110000 | (contrast & 0b00001111));

  delay_us(27);

  //Power/Icon/Contrast control
  ST7032i_Command_Write(Power_Icon_Contrast | ( (contrast >> 4) & 0b00000011 ) );

  delay_us(27);
}

/**
  * @brief  Put icon. value is to be 0 - 12
  * @param  numbet : icon number
  * @retval None
  */
void ST7032i_Icon_Set(uint8_t number)
{
  //icon address set
  ST7032i_Command_Write(0b01000000 | Icon_Table[number][0] );
  delay_us(27);

  //icon data set
  ST7032i_Data_Write(IconRAM[number] | Icon_Table[number][1]);
  delay_us(27);

  //restore DDRAM address to ac
  ST7032i_Command_Write(0b10000000 | DDRAM_Address);
  delay_us(27);
}

/**
  * @brief  Clear icon. value is to be 0 - 12
  * @param  numbet : icon number
  * @retval None
  */
void ST7032i_Icon_Clear(uint8_t number)
{
  //icon address set
  ST7032i_Command_Write(0b01000000 | Icon_Table[number][0] );
  delay_us(27);

  //icon data reset
  ST7032i_Data_Write(IconRAM[number] & (~Icon_Table[number][1]));
  delay_us(27);

  //restore DDRAM address to ac
  ST7032i_Command_Write(0b10000000 | DDRAM_Address);
  delay_us(27);

}

/**
  * @brief  Display icon
  * @param  None
  * @retval None
  */
void ST7032i_Icon_On(void)
{
  //Power/Icon/Contrast control
  ST7032i_Command_Write(Power_Icon_Contrast | ( (Contrast >> 4) & 0b00000011 ) | 0b00001000 );
  delay_us(27);
}

/**
  * @brief  Put off icon whili Icon RAM is kept
  * @param  None
  * @retval None
  */
void ST7032i_Icon_Off(void)
{
  //Power/Icon/Contrast control
  ST7032i_Command_Write( (Power_Icon_Contrast | ( (Contrast >> 4) & 0b00000011 )) & 0b11110111);
  delay_us(27);
}

/**
  * @brief  Print string to LCD
  * @param  String: Array which contain string
  * @retval None
  */
void ST7032i_Print_String(const int8_t String[])
{
  uint8_t i = 0;
  while(String[i] != '\0')
    {
      ST7032i_Putchar(String[i]);
      i++;
    }
}

/**
  * @brief  Initialize ST7032i LCD and I2C interface
  * @param  None
  * @retval None
  */
void ST7032i_Init(void)
{

  delay_ms(40);

  I2C_Configuration();

  //Function Set
  ST7032i_Command_Write(0b00111000);

  delay_us(27);

  //Function Set
  ST7032i_Command_Write(0b00111001);

  delay_us(27);

  //Bias and OSC frequency
  ST7032i_Command_Write(0b00010100);

  delay_us(27);

  //Contrast set
  ST7032i_Command_Write(0b01110000);

  delay_us(27);

  //Power/Icon/Contrast control
  ST7032i_Command_Write(Power_Icon_Contrast);

  delay_us(27);

  //Contrast set
  ST7032i_Set_Contrast(Contrast);

  //Follower control
  ST7032i_Command_Write(0b01101100);

  delay_ms(200);

  //Function Set
  ST7032i_Command_Write(0b00111001);

  delay_us(27);

  //Entry mode
  ST7032i_Command_Write(Entry_Mode);

  delay_us(27);

  //Display control : on
  ST7032i_Command_Write(Display);

  delay_us(27);

  //Clear
  ST7032i_Clear();
}

/**
  * @brief  put character on st7032i lcd
  * @param  None
  * @retval None
  */
void ST7032i_Putchar(int8_t chardata)
{
  uint32_t i;

  ST7032i_Data_Write((uint8_t)chardata);
  delay_us(27);
  if(DDRAM_Address < 0x10)
    {
      DDRAM_Data[0][DDRAM_Address] = chardata;
    }
  else if (DDRAM_Address >= 0x40 && DDRAM_Address < 0x50)
    {
      DDRAM_Data[1][DDRAM_Address - 0x40] = chardata;
    }
  if (Shift == 0)
    {
      DDRAM_Address = DDRAM_Address + Increment;
    }
  else if (Shift == 1 && Increment == 1)
    {
      for (i = 0 ; i< 15; i++)
        {
          DDRAM_Data[0][i] = DDRAM_Data[0][i+1];
          DDRAM_Data[1][i] = DDRAM_Data[1][i+1];
        }
      DDRAM_Data[0][15] = ' ';
      DDRAM_Data[1][15] = ' ';
    }
  else if (Shift == 1 && Increment == -1)
    {
      for (i = 15 ; i> 0; i--)
        {
          DDRAM_Data[0][i] = DDRAM_Data[0][i-1];
          DDRAM_Data[1][i] = DDRAM_Data[1][i-1];
        }
      DDRAM_Data[0][0] = ' ';
      DDRAM_Data[1][0] = ' ';
    }

  if (DDRAM_Address == 0x10)
    {
      DDRAM_Address = 0x40;
      ST7032i_Command_Write(0b10000000 | DDRAM_Address);
      delay_us(27);
    }

  if (DDRAM_Address == 0x3F)
    {
      DDRAM_Address = 0x0F;
      ST7032i_Command_Write(0b10000000 | DDRAM_Address);
      delay_us(27);
    }

  if (DDRAM_Address == 0xFF)
    {
      DDRAM_Address = 0x0;
      ST7032i_Command_Write(0b10000000 | DDRAM_Address);
      delay_us(27);
    }

  if (DDRAM_Address == 0x50)
    {
      for(i=0;i<16;i++)
        {
          DDRAM_Data[0][i] =  DDRAM_Data[1][i];
          ST7032i_Command_Write(0b10000000 | (0x00 + i));
          ST7032i_Data_Write(DDRAM_Data[0][i]);
        }
      for(i=0;i<16;i++)
        {
          DDRAM_Data[1][i] =  ' ';
          ST7032i_Command_Write(0b10000000 | (0x40 + i));
          ST7032i_Data_Write(DDRAM_Data[1][i]);
        }

      DDRAM_Address = 0x40;
      ST7032i_Command_Write(0b10000000 | DDRAM_Address);
      delay_us(27);
    }
}

/**
  * @brief  I2C Configuration
  * @param  None
  * @retval None
  */
void I2C_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;

  /* I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(I2C1_RCC, ENABLE);
  /* GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(I2C1_GPIO_RCC, ENABLE);

  /* Configure I2C pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  I2C1_SCL_PIN | I2C1_SDA_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(I2C1_PORT, &GPIO_InitStructure);

#if defined (REMAP_I2C1)
Remap_I2C1_Configuration();
#endif

  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_CLOCK;

  /* I2C Peripheral Enable */
  I2C_Cmd(I2C1, ENABLE);
  /* Apply I2C configuration after enabling it */
  I2C_Init(I2C1, &I2C_InitStructure);
}

/**
  * @brief  Write Command to ST7032i
  * @param  Data : Command Data
  * @retval None
  */
void ST7032i_Command_Write(uint8_t Data)
{

  /* Send STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, ST7032I_ADDR << 1, I2C_Direction_Transmitter);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  /* Send the EEPROM's internal address to write to : MSB of the address first */
  I2C_SendData(I2C1, 0b00000000);
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  /* Send the EEPROM's internal address to write to : MSB of the address first */
  I2C_SendData(I2C1, Data);
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/**
  * @brief  Write Data to ST7032i
  * @param  Data : "Data" Data
  * @retval None
  */
void ST7032i_Data_Write(uint8_t Data)
{

  /* Send STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, ST7032I_ADDR << 1, I2C_Direction_Transmitter);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  /* Send the EEPROM's internal address to write to : MSB of the address first */
  I2C_SendData(I2C1, 0b01000000);
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  /* Send the EEPROM's internal address to write to : MSB of the address first */
  I2C_SendData(I2C1, Data);
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}
