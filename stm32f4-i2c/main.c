/**
 ******************************************************************************
 * @file    i2c_st7032i_test/main.c
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
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"

// armcore
#include "delay.h"
#include "gpio.h"
#include "i2c.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ST7032I_ADDR               0x3e
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const int8_t Welcome_Message[] =
//  "\r\nHellow Cortex-M3/STM32 World!\r\n"
		// "Expand your creativity and enjoy making.\r\n\r\n"
		"Initialize and put character on ST7032i LCD.\r\n\r\n";

/* Private function prototypes -----------------------------------------------*/
void ST7032i_command(uint8_t d);
void ST7032i_data(uint8_t d);
void ST7032i_print(char * str);
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program.
 * @param  None
 * @retval : None
 */
int main(void) {
	GPIOPin_Type led = PD12;
	char tmp[32];
	int32_t i = 0;

	portMode(GPIOD, GPIO_Pin_12,
			GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz);
	digitalWrite(led, LOW);
	// Configure board specific setting
	// BoardInit();
	// Setting up COM port for Print function
//  COM_Configuration();

	//Send welcome messages
	// cprintf(Welcome_Message);

	digitalWrite(led, HIGH);
	delay_ms(10);
	digitalWrite(led, LOW);
	delay_ms(5);

	digitalWrite(led, HIGH);
	i2c_begin(I2C1, 100000);

	//Function Set
	ST7032i_command(0x38); //(0b00111000);
	delay_us(27);
	digitalWrite(led, LOW);

	digitalWrite(led, HIGH);
	//Function Set
	ST7032i_command(0x39); //(0b00111001);
	delay_us(27);
	digitalWrite(led, LOW);

	//Bias and OSC frequency
	ST7032i_command(0x14); //(0b00010100);

	delay_us(27);

	//Contrast set
	ST7032i_command(0x70 | (40 & 0x0f));

	delay_us(27);

	//Power/Icon/Contrast control
	ST7032i_command(0x56 | (40 >> 4 & 0x03));

	delay_us(27);

	//Follower control
	ST7032i_command(0x6c);

	delay_ms(200);

	//Function Set
	ST7032i_command(0x38);

	//Display control : on
	ST7032i_command(0x0c);

	delay_us(27);

	//Clear
	ST7032i_command(0x01);

	delay_ms(2);

	ST7032i_print("Hi friends!");

	ST7032i_command(0x80 | 0x40);

	ST7032i_print("My World!");

//  cprintf("Done! Confirm a message is on LCD.");

	while (1) {
		//Clear
		ST7032i_command(0x80 | 0x40);
		ST7032i_print("        ");
		ST7032i_command(0x80 | 0x40);
		sprintf(tmp, (char *)"> %d", (int16_t)i);
		ST7032i_print(tmp);
		i++;
		delay_ms(100);
	}
}

/**
 * @brief  I2C Configuration
 * @param  None
 * @retval None
 */

/**
 * @brief  Write Command to ST7032i
 * @param  Data : Command Data
 * @retval None
 */
void ST7032i_command(uint8_t d) {
	uint8_t data[2];
	data[0] = 0;
	data[1] = d;
	i2c_transmit(I2C1, ST7032I_ADDR, data, 2);
}

/**
 * @brief  Write Data to ST7032i
 * @param  Data : "Data" Data
 * @retval None
 */
void ST7032i_data(uint8_t data) {
	uint8_t t[2];
	t[0] = 0x40 ; //0b01000000;
	t[1] = data;
	i2c_transmit(I2C1, ST7032I_ADDR, t, 2);
}

void ST7032i_print(char * str) {
	while (*str)
		ST7032i_data(*str++);
}
