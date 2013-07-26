#ifndef INC_PS2KBD_H
#define INC_PS2KBD_H

//#include "config.h"
#include "type.h"
//#include "utils.h"

#define KBD_PINSEL  (0x00000001)
//#define KBD_PINMASK (0x00000000)

#define KB_DAT (1<<4)
#define KB_CLK (1<<13)



#define CLR_EINT1_FLAG() (EXTINT=0x02)                             //Clear EINT1 flag
#define DISABLE_EINT1() (VICIntEnClr =(VIC_BIT(VIC_EINT1)))       // EINT1 interrupt disabled
#define ENABLE_EINT1() (VICIntEnable = (VIC_BIT(VIC_EINT1)))      // EINT1 interrupt enabled

#define PAGE_UP     0
#define PAGE_DOWN   1
#define HOME        2
#define END         3
#define UP_ARROW    4
#define DOWN_ARROW  5
#define LEFT_ARROW  6
#define RIGHT_ARROW 7
#define ENTER       8
#define BACKSPACE   9
#define ESC         0X0A
#define INSERT      0X0B
#define DEL         0X0C
#define F1          0x0E
#define F2          0x0F
#define F3          0x10
#define F4          0x11
#define F5          0x12
#define F6          0x13
#define F7          0x14
#define F8          0x15
#define F9          0x16
#define F10         0x17
#define F11         0x18
#define F12         0x19

#define RESEND      0xFE
#define ACK         0xFA
#define BREAKCHAR   0xF0
#define LEFTSHIFT   0x12
#define RIGHTSHIFT  0x59
#define CTRLKEY     0x14
#define ALTKEY      0x11
#define CAPSLOCK    0x58
#define	NUMLOCK     0x77
#define SCROLLLOCK  0x7E
#define DOUBLECODE  0xE1
#define NONUMCODE   0xE0

#define KBLED_ON 1
#define KBLED_OFF 0

#define KBUFFSIZE 4

#define CAPSLED 0x04
#define NUMLED  0x01


uint8_t init_keyboard(void);
void decode(uint8_t sc);
void putkeybuf(uint8_t c);
uint8_t xgetkey(void);
uint8_t keyhit(void);
void SendByte(void);
U8 is_valid_ascii(S8 key);

#endif

