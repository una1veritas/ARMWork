#ifndef _ARM_IO_H_
#define _ARM_IO_H_

#include "glcd/config/ks0108_arm.h"
#include "armcmx.h"

#define __inline__  inline

#define lcdPinMode(pin,mode)  pinMode(pin, mode)

#define	lcdfastWrite(pin,val)  \
  GPIO_WriteBit(PinPort(pin), PinBit(pin), val? Bit_SET : Bit_RESET )

#define lcdDelayMilliseconds(x) delay_ms(x)

#define lcdDataDir(dir) \
  GPIOMode(PinPort(glcdData0Pin), \
    PinBit(glcdData0Pin) | PinBit(glcdData1Pin) | PinBit(glcdData2Pin) | PinBit(glcdData3Pin) \
    | PinBit(glcdData4Pin) | PinBit(glcdData5Pin) | PinBit(glcdData6Pin) | PinBit(glcdData7Pin), \
    dir, MEDSPEED, PUSHPULL, PULLUP)

void lcdDataOut(uint8 val) {
  uint8 mask = PinBit(glcdData0Pin) | PinBit(glcdData1Pin) | PinBit(glcdData2Pin) | PinBit(glcdData3Pin) 
              | PinBit(glcdData4Pin) | PinBit(glcdData5Pin) | PinBit(glcdData6Pin) | PinBit(glcdData7Pin);
  uint8 data = GPIO_ReadOutputData(PinPort(glcdData0Pin)) & ~mask;
  GPIO_Write(PinPort(glcdData0Pin), data | val<<PinSource(glcdData0Pin));
}
// Read status bits
#define lcdDataIn()	 GPIO_ReadInputData(PinPort(glcdData0Pin))

#define lcdDelayNanoseconds(t) \
  for(long i = t; i > 0; i--) { __nop(); }

/*
 * alias to read status bits
 */
#define lcdRdBusystatus()		(GPIO_ReadInputDataBit(PinPort(glcdData7Pin), LCD_BUSY_BIT))
#define lcdRdResetstatus()		(GPIO_ReadInputDataBit(PinPort(glcdData7Pin), LCD_RESET_BIT))

/*
 * alias to check status bits
 */

#define lcdIsBusyStatus(status) (status & LCD_BUSY_FLAG)
#define lcdIsResetStatus(status) (status & LCD_RESET_FLAG)


#endif
