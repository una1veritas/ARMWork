
#if defined (ARDUINO)
#include <avr/pgmspace.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif
#include <Wire.h>
#elif defined (ARMCMX)
#include "armcmx.h"
#include "I2CBus.h"
#define PROGMEM 
#define prog_char char
#endif
#include "Print.h"
#include "ISO14443.h"

PROGMEM const prog_char ISO14443CardInfo::Type_Names[] =
		"\0Standard 1k\0Standard 7 Byte UID 1k\0UltraLight\0Standard 4k\0Standard 7 Byte UID 4k\0DesFire\0\0\0\0\0Unknown";
    
char* ISO14443CardInfo::get_type_name(char * buf, const byte tp) {
	int i, p;
	for (i = 0, p = 0; i < tp; p++) {
#if defined(ARDUINO)
		if (pgm_read_byte(Type_Names + p) == 0)
#elif defined (ARMCMX)
		if ( *(Type_Names + p) == 0)
#endif
			i++;
	}
#if defined(ARDUINO)
	strcpy_P(buf, Type_Names + p);
#elif defined (ARMCMX)
	strcpy(buf, Type_Names + p);
#endif
	return buf;
}

