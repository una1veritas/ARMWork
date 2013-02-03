
#ifndef __XPT2046_H
#define __XPT2046_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f4xx.h"	

#ifdef __cplusplus
}
#endif

#include "cmcore.h"
#include "spi.h"

class TouchScreenController {
	enum {
		ADS7846 = 0,  // by TI
		XPT2046 = 0,  // Comapi?
	} type;

	spi & bus;
	GPIOPin ncspin, irqpin;

	#define CMD_RDY 0X90  //0B10010000
#define CMD_RDX	0XD0  //0B11010000  											 
 
#define NPEN !(0x0080&PEN)      //!PEN
	
public:
	u16 X0;
	u16 Y0;
	u16 X; 
	u16 Y;						   	    
	u8  Key_Sta;
	u8  Key_LSta;

	float xfac, yfac;
	int16 xoff, yoff;

TouchScreenController(spi & spix, GPIOPin ncs, GPIOPin irq) 
		: bus(spix), ncspin(ncs), irqpin(irq) {
	
}

inline void select(void) { GPIO_ResetBits(TOUCH_CS_PORT, TOUCH_CS_PIN); }
inline void deselect(void) { GPIO_SetBits(TOUCH_CS_PORT, TOUCH_CS_PIN); }


unsigned char SPI_WriteByte(u8 num);
void SpiDelay(unsigned int DelayCnt);
u16 TPReadX(void);
u16 TPReadY(void);	   
u8 read_once(void);	
u8 Read_Ads7846(void); 
void Convert_Pos(void);

void EXTI9_5_IRQHandler(void);
void NVIC_TOUCHConfiguration(void);
void touch_init(void);				  
void LCD_ShowNum(uint8_t x,uint16_t y,uint16_t data);

void Touch_Adjust(void);
};

#endif 


















