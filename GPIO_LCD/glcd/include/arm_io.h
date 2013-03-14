#ifndef _ARM_IO_H_
#define _ARM_IO_H_

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

#define lcdDataOut(val) \
		GPIO_Write(PinPort(glcdData0Pin), GPIO_ReadOutputData(PinPort(glcdData0Pin)) & ~(PinBit(glcdData0Pin) | PinBit(glcdData1Pin) | PinBit(glcdData2Pin) | PinBit(glcdData3Pin) \
            | PinBit(glcdData4Pin) | PinBit(glcdData5Pin) | PinBit(glcdData6Pin) | PinBit(glcdData7Pin))) 
// Read status bits
#define lcdDataIn()	 GPIO_ReadInputData(PinPort(glcdData0Pin))

#define lcdDelayNanoseconds(t) \
  for(long i = t; i > 0; i--) __nop();

#define lcdIsResetStatus(s)  ((s)& 0x40)
#define lcdIsBusyStatus(s)   ((s)&0x80)
lcdRdBusystatus

#endif
