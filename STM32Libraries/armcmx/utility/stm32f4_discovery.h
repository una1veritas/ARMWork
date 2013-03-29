/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4_DISCOVERY_H
#define __STM32F4_DISCOVERY_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "armcmx.h"

#define LED1 	PD12
#define LED2 	PD13
#define LED3 	PD14
#define LED4 	PD15
typedef GPIOPin Led_TypeDef;

#define USER_BUTTON2 	PA0
#define USER_BUTTON1  PB1
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


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4_DISCOVERY_H */
