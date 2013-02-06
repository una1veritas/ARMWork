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

#include "cmcore.h"
#include "spi.h"

class TouchScreen {
	enum {
		ADS7846 = 0,  // by TI
		XPT2046 = 0,  // Comapi?
	} type;
	
	spi & bus;
	GPIOPin ncspin, irqpin;
	uint8 direction;

//	static const int PEN  = GPIOD->IDR&(1<<6); //
	static const uint8 CMD_RDX = 0x90;  //0B10010000
	static const uint8 CMD_RDY = 0xD0;  //0B11010000
//	static const uint16 NPEN = !(0x0080&PEN);      //!PEN

public:
	int16 X0, Y0;
	uint16 X, Y;
	u8 Key_Sta;
	u8 Key_LSta;
	uint8 flag;
	float xfac, yfac;
	int16 xoff, yoff;

	enum {
		Normal = 0,
		Transpose = 1,
		ReverseX = 2,
		ReverseY = 4
	};

	TouchScreen(spi & spix, GPIOPin ncs, GPIOPin irq) :
			bus(spix), ncspin(ncs), irqpin(irq) {
		flag = 0;
				direction = 0;
	}

	void init(void);
	void begin(uint8 d = Normal);

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
	void readOnce(void) {
		X = readX();
		Y = readY();
	}
	uint8 readAds7846(void);
	void convertPos(void);

//void adjust(void);

};

#endif 

