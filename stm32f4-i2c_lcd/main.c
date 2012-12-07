/**
 ******************************************************************************
 * @file    i2c_st7032i_lib/main.c
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
#include "stm32f4xx.h"
#include "st7032i.h"

#include "delay.h"
#include "usart.h"
#include "i2c.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const int8_t Welcome_Message[] =
		"\r\nHellow Cortex-M3/STM32 World!\r\n"
				"Expand your creativity and enjoy making.\r\n\r\n"
				"I receive character from COM and put it on LCD.\r\n"
				"Capital letter is a special direction to LCD. Refer source code.\r\n\r\n";
int8_t LCD_Message[] = "Hello mmy World!";
__IO uint8_t RxData;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void ST7032i_print(char * str);

/**
 * @brief  Main program.
 * @param  None
 * @retval : None
 */
int main(void) {
	uint8_t i = 0;
	uint8_t icon_num = 12;

	char tmp[32];

	// Configure board specific setting
//  BoardInit();
	// Setting up COM port for Print function
//  COM_Configuration();
	usart_begin(USART3, PD9, PD8, 19200);

	//Send welcome messages
	usart_print(USART3, (char *) Welcome_Message);
//  cprintf(Welcome_Message);
	delay_ms(100);

	i2c_begin(100000);
	ST7032i_Init();

	//Show message on first line
	ST7032i_Print_String(LCD_Message);

	//Receive character from COM and put it on LCD
	while (1) {
		if (usart_available(USART3) > 0) // RX_BUFFER_IS_NOT_EMPTY)
				{
			RxData = usart_read(USART3); //RECEIVE_DATA;
			if ((RxData >= 0x20 && RxData <= 0x3F)
					|| (RxData >= 0x60 && RxData <= 0x7F)) {
//              cputchar(RxData);
				sprintf(tmp, "%c", RxData);
				usart_print(USART3, tmp);
				ST7032i_Putchar(RxData);
			} else {
				switch (RxData) {
				case 'Q':
					ST7032i_Clear();
					break;
				case 'R':
					ST7032i_Return();
					break;
				case 'I':
					ST7032i_Increment();
					break;
				case 'D':
					ST7032i_Decrement();
					break;
				case 'S':
					ST7032i_Shift_Enable();
					break;
				case 'H':
					ST7032i_Shift_Disable();
					break;
				case 'Z':
					ST7032i_Display_On();
					break;
				case 'X':
					ST7032i_Display_Off();
					break;
				case 'C':
					ST7032i_Cursor_On();
					break;
				case 'V':
					ST7032i_Cursor_Off();
					break;
				case 'B':
					ST7032i_Cursor_Blink_On();
					break;
				case 'N':
					ST7032i_Cursor_Blink_Off();
					break;
				case 'M':
					ST7032i_Clear();
					ST7032i_Set_DDRAM(0x40);
					break;
				case 'J':
					DDRAM_Address--;
					ST7032i_Set_DDRAM(DDRAM_Address);
					break;
				case 'K':
					DDRAM_Address++;
					ST7032i_Set_DDRAM(DDRAM_Address);
					break;
				case 'W':
					Contrast++;
					sprintf(tmp, "Contrast is :%u\r\n", Contrast);
					usart_print(USART3, tmp);
					ST7032i_Set_Contrast(Contrast);
					break;
				case 'E':
					Contrast--;
					sprintf(tmp, "Contrast is :%u\r\n", Contrast);
					usart_print(USART3, tmp);
					ST7032i_Set_Contrast(Contrast);
					break;
				case 'O':
					ST7032i_Icon_On();
					break;
				case 'P':
					ST7032i_Icon_Off();
					break;
				case 'L':
					ST7032i_Icon_Clear(icon_num);
					icon_num++;
					if (icon_num > 12) {
						icon_num = 0;
					}
					ST7032i_Icon_Set(icon_num);
					break;
				case 'A':
					usart_print(USART3,
							"\r\nData stored in DDRAM is :\r\n");
					for (i = 0; i < 16; i++) {
						usart_write(USART3, (uint8_t) DDRAM_Data[0][i]);
					}
					usart_print(USART3, "\r\n");
					for (i = 0; i < 16; i++) {
						usart_write(USART3, (uint8_t) DDRAM_Data[1][i]);
					}
					usart_print(USART3, "\r\n");
					break;
				}
			}
		} else {
			delay_ms(10);
			tmp[0] = 0;
			i2c_requestFrom(0b1101000, 0, (uint8_t *) tmp, 4);
			delay_ms(100);
		}
	}
}

void ST7032i_print(char * str) {
	while (*str)
		ST7032i_Data_Write(*str++);
}
