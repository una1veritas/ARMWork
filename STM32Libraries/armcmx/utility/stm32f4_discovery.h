/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4_DISCOVERY_H
#define __STM32F4_DISCOVERY_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "armcmx.h"
//#include "stm324xg_eval_lcd.h"
//#include "stm32_eval_legacy.h"

#define LED1 	PD12
#define LED2 	PD13
#define LED3 	PD14
#define LED4 	PD15
typedef GPIOPin Led_TypeDef;

#define USER_BUTTON2 	PA0
#define USER_BUTTON1  PB1
   /*
typedef enum 
{  
  BUTTON_WAKEUP = 0,
  BUTTON_TAMPER = 1,
  BUTTON_KEY = 2,
  BUTTON_RIGHT = 3,
  BUTTON_LEFT = 4,
  BUTTON_UP = 5,
  BUTTON_DOWN = 6,
  BUTTON_SEL = 7
} Button_TypeDef;
*/
typedef GPIOPin Button_TypeDef;
typedef GPIOMode_TypeDef ButtonMode_TypeDef;

typedef usart* COM_TypeDef;

void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);
void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button);
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
/*
void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void); 
void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);
void sEE_LowLevel_DeInit(void);
void sEE_LowLevel_Init(void); 
void sEE_LowLevel_DMAConfig(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction);
*/


#define Red   1
#define White 1
#define Black 0
#define Line0 0
#define Line1 1
#define Line2 2
#define Line3 3
#define Line4 4
#define Line5 5
#define Line6 6
#define Line7 7
#define Line8 8
#define Line9 9

typedef void sFONT;
void STM324xG_LCD_Init(void);
void LCD_DisplayStringLine(uint16 x, uint8 *y);// {} //  DisplayStringLine((x), (y))
//void LCD_GetFont(x) {}
void LCD_SetTextColor(uint16 x); // {}
sFONT * LCD_GetFont(void); // { return 0; }
void LCD_Clear(uint16);
void LCD_SetBackColor(uint16);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4_DISCOVERY_H */
