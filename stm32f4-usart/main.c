#include <ctype.h>

#include "stm32f4xx.h"
//#include "st7032i.h"

#include "armcore.h"
#include "delay.h"
#include "usart.h"
#include "i2c.h"

 USART Serial3;
 
int main(void) {
//	int16_t i = 0;
//	const int rxbufsize = 64;
//	char rxbuf[rxbufsize];
	char printbuf[64];
	uint32_t tmp32, rtctime = 0;

	TIM2_timer_start();

	usart_begin(&Serial3, USART3, PC11, PC10, 19200);
	usart_print(&Serial3, "\r\nWelcome to USART3.\r\n\r\n");

	i2c_begin(&Wire1, 100000);
//	ST7032i_Init();

//	ST7032i_Set_Contrast(44);
//	ST7032i_Print_String((const int8_t *) "Welcome to lcd.");

	delay_ms(500);
//	ST7032i_Command_Write(0x01);

	//Receive character from COM and put it on LCD
	while (1) {

		usart_print(&Serial3, ".");
		i2c_receive(&Wire1, B1101000, 0, (uint8_t *) &tmp32, 3);
		if (rtctime != (tmp32 & 0xffffff)) {
			rtctime = tmp32 & 0xffffff;
			sprintf(printbuf, "%02x:%02x:%02x\r\n", UINT8(rtctime>>16),
					UINT8(rtctime>>8), UINT8(rtctime) );
			usart_print(&Serial3, printbuf);
//			ST7032i_Set_DDRAM(((0 * 0x40) % 0x6c) + 0);
//			ST7032i_Print_String((int8_t *) printbuf);
			if ((rtctime & 0xff) == 0) {
				i2c_receive(&Wire1, B1101000, 3, (uint8_t *) &tmp32, 4);
				sprintf(printbuf, "20%02x %02x/%02x (%x)", UINT8(tmp32>>24),
						UINT8(tmp32>>16), UINT8(tmp32>>8), UINT8(tmp32) );
				usart_print(&Serial3, printbuf);
//				ST7032i_Set_DDRAM(((1 * 0x40) % 0x6c) + 0);
//				ST7032i_Print_String((int8_t *) printbuf);
			}
		}
		delay_ms(50);
	}
}

