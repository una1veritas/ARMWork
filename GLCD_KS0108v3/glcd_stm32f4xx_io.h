
#include "armcmx.h"

#define lcdPinMode(p, m)  pinMode(p, m)
#define lcdfastWrite(p, v) 		digitalWrite(p, v)
#define lcdDelayMilliseconds(t) 	delay_ms(t)

