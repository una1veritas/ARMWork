#ifndef _KS0108_COMPAT_H_
#define _KS0108_COMPAT_H_
	
#define LCD_ON				0x3F
#define LCD_OFF				0x3E
#define LCD_DISP_START		0xC0

#define LCD_SET_ADD			0x40
#define LCD_SET_PAGE		0xB8
#define LCD_BUSY_FLAG		0x80 

// Colors
#define BLACK				0xFF
#define WHITE				0x00

#define CHIP_WIDTH 	64
#define CHIP_HEIGHT 	64
#define DISPLAY_WIDTH 	128
#define DISPLAY_HEIGHT 	64
#define CHIP_PAGES 	8
#define PAGE_COLUMNS 	64

#define COMMAND 	LOW
#define DATA  HIGH
#define READ 	LOW
#define WRITE HIGH

#endif // _GLCD_KS0108_H_
