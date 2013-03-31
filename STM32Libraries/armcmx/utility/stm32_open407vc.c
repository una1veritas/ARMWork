#include "stm32f4_discovery.h"


void STM_EVAL_LEDInit(Led_TypeDef Led) { 
   pinMode(Led, OUTPUT); 
}
void STM_EVAL_LEDOn(Led_TypeDef Led) { digitalWrite(Led, HIGH); }
void STM_EVAL_LEDOff(Led_TypeDef Led) { digitalWrite(Led, LOW); }
void STM_EVAL_LEDToggle(Led_TypeDef Led) { digitalToggle(Led); }
void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode) { pinMode(Button, INPUT); }
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button) { return digitalRead(Button); }
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct) {
#ifndef STDSERIAL
  usart_init(COM, USART3, PB11, PB10);
	usart_begin(COM, 57600);
#endif
}

// for the compatibility
/*
void STM324xG_LCD_Init(void) {
  
}
void LCD_DisplayStringLine(uint16 x, uint8 *y) {
  printf("%s\n",y);
}
//void LCD_GetFont(x) {}
void LCD_SetTextColor(uint16 x) {}
//sFONT *LCD_GetFont(void) { return 0; }
void LCD_Clear(uint16 c) {}
void LCD_SetBackColor(uint16 c) {}
*/
