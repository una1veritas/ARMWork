#ifndef _ARM_IO_H_
#define _ARM_IO_H_

#define __inline__  inline

#define lcdPinMode(pin,mode)  pinMode(pin, mode)
#define	lcdfastWrite(pin,val)  digitalWrite(pin,val)
#define lcdDelayMilliseconds(x) delay_ms(x)

#define lcdDataDir(dir)
#define lcdDataOut(val)
#define lcdDataIn()	// Read status bits
#define lcdDelayNanoseconds(t)

#define lcdIsResetStatus(s)  ((s)& 0x40)
#define lcdIsBusyStatus(s)   ((s)&0x80)

#endif
