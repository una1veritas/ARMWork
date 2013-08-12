/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_OPEN407VC_H
#define __STM32_OPEN407VC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "armcmx.h"
#include "stm32f4_discovery.h"

typedef enum { 
  JOY_NONE = NOT_A_PIN,
  JOY_SEL = PC7,
  JOY_DOWN = PC6,
  JOY_LEFT = PB15,
  JOY_RIGHT = PA6,
  JOY_UP = PA4
} JOYState_TypeDef;

/*
void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void); 
void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);
void sEE_LowLevel_DeInit(void);
void sEE_LowLevel_Init(void); 
void sEE_LowLevel_DMAConfig(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction);
*/

/*
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
void LCD_Clear(uint16 c);
void LCD_DisplayStringLine(uint16 x, char *y);// {} //  DisplayStringLine((x), (y))
//void LCD_GetFont(x) {}
void LCD_SetTextColor(uint16 x); // {}
sFONT * LCD_GetFont(void); // { return 0; }
void LCD_Clear(uint16);
void LCD_SetBackColor(uint16);
*/

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4_DISCOVERY_H */
