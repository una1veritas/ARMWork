#ifndef __ADS7846_H
#define __ADS7846_H

/*
TI ADS7846,
XPT2046
*/

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

void EXTI9_5_IRQHandler(void);
void NVIC_TOUCHConfiguration(void);
//void LCD_ShowNum(uint8_t x,uint16_t y,uint16_t data);

#ifdef __cplusplus
}
#endif

#include "armcmx.h"
#include "spi.h"

class TouchScreen {
	enum {
		ADS7846 = 0,  // by TI
		XPT2046 = 0,  // Comapi?
	} chiptype;
	
	spi & bus;
	GPIOPin ncspin, irqpin;
	uint8 mode;
  uint16 shortside, longside;

//	static const int PEN  = GPIOD->IDR&(1<<6); //
	static const uint8 CMD_RDX = 0x90;  //0B10010000
	static const uint8 CMD_RDY = 0xD0;  //0B11010000
//	static const uint16 NPEN = !(0x0080&PEN);      //!PEN
	
	void getRawXY(void);

public:
	int16 x, y;
	uint16 Xraw, Yraw;
//	u8 Key_Sta;
//	u8 Key_LSta;
//	uint8 flag;
	float xfact, yfact;
	int16 xoffset, yoffset;

	enum {
    PORTRAIT = 0,
    LANDSCAPE,
    PORTRAIT_REV,
    LANDSCAPE_REV
//		Normal = 0,
//		Landscape = 0,
//		Portrait = 1,
//		ReverseX = 2,
//		ReverseY = 4
	};

	TouchScreen(spi & spix, GPIOPin ncs, GPIOPin irq) :
			bus(spix), ncspin(ncs), irqpin(irq) {
        longside = 320;
        shortside = 240;
				mode = PORTRAIT;
				yoffset = 103; 
        yfact = 7.7f;
				xoffset = 104; 
        xfact = 5.56f;
	}

	void init(void);
	void begin(uint16 w, uint16 h, uint8 d = PORTRAIT);
  void setOrientation(uint8 d);
  
	inline void select(void) {
		spi_setMode(&bus, SPI_BaudRatePrescaler_256, SPI_CPOL_High,
				SPI_CPHA_2Edge, SPI_FirstBit_MSB);
		digitalWrite(ncspin, LOW);
	}
	inline void deselect(void) {
		digitalWrite(ncspin, HIGH);
	}

	uint16 readX(void);
	uint16 readY(void);
	uint8 readADC(void);
	void update(void);

//void adjust(void);

};

#endif 

