#ifndef __XPT2046_H
#define __XPT2046_H

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

//	static const int PEN  = GPIOD->IDR&(1<<6); //
	static const uint8 CMD_RDY = 0X90;  //0B10010000
	static const uint8 CMD_RDX = 0XD0;  //0B11010000
//	static const uint16 NPEN = !(0x0080&PEN);      //!PEN

public:
	uint16 X0, Y0;
	uint16 X, Y;
	u8 Key_Sta;
	u8 Key_LSta;
	uint8 flag;
	float xfac, yfac;
	int16 xoff, yoff;

	TouchScreen(spi & spix, GPIOPin ncs, GPIOPin irq) :
			bus(spix), ncspin(ncs), irqpin(irq) {
		flag = 0;
	}

	void init(void);

	inline void select(void) {
		spi_setMode(&bus, SPI_BaudRatePrescaler_256, SPI_CPOL_High,
				SPI_CPHA_2Edge, SPI_FirstBit_MSB);
		digitalWrite(ncspin, LOW);
	}
	inline void deselect(void) {
		digitalWrite(ncspin, HIGH);
	}

	uint8 writeByte(uint8 num);
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

