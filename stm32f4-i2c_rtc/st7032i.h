/**
  ******************************************************************************
  * @file    lib_std/UTIL/inc/st7032i.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ST7032I_H
#define __ST7032I_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
//#include "platform_config.h"
#include "delay.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define I2C_CLOCK                  100000
#define ROWS                       2
#define COLUMNS                    16
#define ST7032I_ADDR               0b0111110
/* Exported types ------------------------------------------------------------*/
extern __IO int8_t DDRAM_Data[ROWS][COLUMNS];
extern __IO uint8_t DDRAM_Address;
extern __IO uint8_t Entry_Mode;
extern __IO int8_t Increment;
extern __IO int8_t Shift;
extern __IO uint8_t Display;
extern __IO uint8_t Power_Icon_Contrast;
extern __IO uint8_t Contrast;
extern __IO uint8_t IconRAM[16];
extern __IO uint8_t Icon_Table[13][2];
/* Exported functions ------------------------------------------------------- */
void I2C_Configuration(void);
void ST7032i_Init(void);
void ST7032i_Command_Write(uint8_t Data);
void ST7032i_Data_Write(uint8_t Data);
void ST7032i_Clear(void);
void ST7032i_Return(void);
void ST7032i_Increment(void);
void ST7032i_Decrement(void);
void ST7032i_Shift_Enable(void);
void ST7032i_Shift_Disable(void);
void ST7032i_Display_On(void);
void ST7032i_Display_Off(void);
void ST7032i_Cursor_On(void);
void ST7032i_Cursor_Off(void);
void ST7032i_Cursor_Blink_On(void);
void ST7032i_Cursor_Blink_Off(void);
void ST7032i_Set_DDRAM(uint8_t address);
void ST7032i_Set_Contrast(uint8_t address);
void ST7032i_Icon_Set(uint8_t address);
void ST7032i_Icon_Clear(uint8_t address);
void ST7032i_Icon_On(void);
void ST7032i_Icon_Off(void);
void ST7032i_Putchar(int8_t chardata);
void ST7032i_Print_String(const int8_t[]);



#endif /* __ST7032I_H */
