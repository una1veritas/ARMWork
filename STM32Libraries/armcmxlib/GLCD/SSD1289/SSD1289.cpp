#include "SSD1289.h"
#include "AsciiLib.h"

#define LCD_REG      (*((volatile unsigned short *) 0x60000000)) 
#define LCD_RAM      (*((volatile unsigned short *) 0x60020000)) 
#define LCD_REG_READ      (*((volatile unsigned short *) (0x60000000 | (1<<16)))) 

#define MAX_POLY_CORNERS   200
#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))
#define ABS(X)  ((X) > 0 ? (X) : -(X))     

//****************************************************************************//
//static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed);
//****************************************************************************//
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
//****************************************************************************//
#define  LCD_ILI9320    0x9320
#define  LCD_ILI9325    0x9325
#define  LCD_ILI9328    0x9328
#define  LCD_ILI9331    0x9331
#define  LCD_SSD1298    0x8999
#define  LCD_SSD1289    0x8989
#define  LCD_ST7781     0x7783
#define  LCD_LGDP4531   7  /* 0x4531 */
#define  LCD_SPFD5408B  8  /* 0x5408 */
#define  LCD_R61505U    9  /* 0x1505 0x0505 */	   
#define  LCD_HX8347D    10 /* 0x0047 */
#define  LCD_HX8347A    11 /* 0x0047 */	
#define  LCD_LGDP4535   12 /* 0x4535 */  
#define  LCD_SSD2119    13 /* 3.5 LCD 0x9919 */

void SSD1289::start(void) { 
	// assumes already high
	digitalWrite(PC5, LOW);
	delay_ms(5);
	digitalWrite(PC5, HIGH);
	
	deviceCode = ReadReg(0x0000);		/* Read LCD ID	*/	

	switch(deviceCode) {
    case LCD_SSD1289:
      WriteReg(0x0000,0x0001);    delay_us(50);   // Enable LCD Oscillator
      WriteReg(0x0003,0xA8A4);    delay_us(50);   
	    WriteReg(0x000C,0x0000);    delay_us(50);   
	    WriteReg(0x000D,0x080C);    delay_us(50);   
	    WriteReg(0x000E,0x2B00);    delay_us(50);   
	    WriteReg(0x001E,0x00B0);    delay_us(50);   
	    WriteReg(0x0001,0x2B3F);    delay_us(50);   // 320*240 0x2B3F 
	    WriteReg(0x0002,0x0600);    delay_us(50);
	    WriteReg(0x0010,0x0000);    delay_us(50);
	    WriteReg(0x0011,0x6070);    delay_us(50);
	    WriteReg(0x0005,0x0000);    delay_us(50);
	    WriteReg(0x0006,0x0000);    delay_us(50);
	    WriteReg(0x0016,0xEF1C);    delay_us(50);
	    WriteReg(0x0017,0x0003);    delay_us(50);
	    WriteReg(0x0007,0x0133);    delay_us(50);         
	    WriteReg(0x000B,0x0000);    delay_us(50);
	    WriteReg(0x000F,0x0000);    delay_us(50);
	    WriteReg(0x0041,0x0000);    delay_us(50);
	    WriteReg(0x0042,0x0000);    delay_us(50);
	    WriteReg(0x0048,0x0000);    delay_us(50);
	    WriteReg(0x0049,0x013F);    delay_us(50);
	    WriteReg(0x004A,0x0000);    delay_us(50);
	    WriteReg(0x004B,0x0000);    delay_us(50);
	    WriteReg(0x0044,0xEF00);    delay_us(50);
	    WriteReg(0x0045,0x0000);    delay_us(50);
	    WriteReg(0x0046,0x013F);    delay_us(50);
	    WriteReg(0x0030,0x0707);    delay_us(50);
	    WriteReg(0x0031,0x0204);    delay_us(50);
	    WriteReg(0x0032,0x0204);    delay_us(50);
	    WriteReg(0x0033,0x0502);    delay_us(50);
	    WriteReg(0x0034,0x0507);    delay_us(50);
	    WriteReg(0x0035,0x0204);    delay_us(50);
	    WriteReg(0x0036,0x0204);    delay_us(50);
	    WriteReg(0x0037,0x0502);    delay_us(50);
	    WriteReg(0x003A,0x0302);    delay_us(50);
	    WriteReg(0x003B,0x0302);    delay_us(50);
	    WriteReg(0x0023,0x0000);    delay_us(50);
	    WriteReg(0x0024,0x0000);    delay_us(50);
	    WriteReg(0x0025,0x8000);    delay_us(50);
	    WriteReg(0x004f,0);
	    WriteReg(0x004e,0);
		break;
		default: // SSD1289
			WriteReg(0x0007,0x0021);    Delay(50);
			WriteReg(0x0000,0x0001);    Delay(50);		// osc enable
			WriteReg(0x0007,0x0023);    Delay(50);
			WriteReg(0x0010,0x0000);    Delay(90);
			WriteReg(0x0007,0x0033);    Delay(50);
			WriteReg(0x0011,0x6830);    Delay(50);
			WriteReg(0x0002,0x0600);    Delay(50);
			WriteReg(0x0012,0x6CEB);    Delay(50);
			WriteReg(0x0003,0xA8A4);    Delay(50);                       
			WriteReg(0x000C,0x0000);    Delay(50);
			WriteReg(0x000D,0x080C);    Delay(50);
			WriteReg(0x000E,0x2B00);    Delay(50);
			WriteReg(0x001E,0x00B0);    Delay(50);
			WriteReg(0x0001,0x2b3F);    Delay(50);  //RGB
			WriteReg(0x0005,0x0000);    Delay(50);
			WriteReg(0x0006,0x0000);    Delay(50);
			WriteReg(0x0016,0xEF1C);    Delay(50);
			WriteReg(0x0017,0x0103);    Delay(50);
			WriteReg(0x000B,0x0000);    Delay(50);
			WriteReg(0x000F,0x0000);    Delay(50);  
			WriteReg(0x0041,0x0000);    Delay(50);
			WriteReg(0x0042,0x0000);    Delay(50);
			WriteReg(0x0048,0x0000);    Delay(50);
			WriteReg(0x0049,0x013F);    Delay(50);
			WriteReg(0x004A,0x0000);    Delay(50);
			WriteReg(0x004B,0x0000);    Delay(50);
			WriteReg(0x0044,0xEF00);    Delay(50);
			WriteReg(0x0045,0x0000);    Delay(50);
			WriteReg(0x0046,0x013F);    Delay(50);
			WriteReg(0x0030,0x0707);    Delay(50);
			WriteReg(0x0031,0x0204);    Delay(50);
			WriteReg(0x0032,0x0204);    Delay(50);
			WriteReg(0x0033,0x0502);    Delay(50);
			WriteReg(0x0034,0x0507);    Delay(50);
			WriteReg(0x0035,0x0204);    Delay(50);
			WriteReg(0x0036,0x0204);    Delay(50);
			WriteReg(0x0037,0x0502);    Delay(50);
			WriteReg(0x003A,0x0302);    Delay(50);
			WriteReg(0x002F,0x12BE);    Delay(50);           
			WriteReg(0x003B,0x0302);    Delay(50);
			WriteReg(0x0023,0x0000);    Delay(50);
			WriteReg(0x0024,0x0000);    Delay(50);
			WriteReg(0x0025,0x8000);    Delay(50);
			WriteReg(0x004f,0x0000);    Delay(50);
			WriteReg(0x004e,0x0000);    Delay(50);
			break;
	}
	displayOrientation(PORTRAIT);
}

void SSD1289::WriteRAM_Prepare(void)
{
  LCD_REG = REG_RAM_DATA;
}

void SSD1289::WriteRAM(uint16_t RGB_Code)
{
  LCD_RAM = RGB_Code;
}

void SSD1289::WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  LCD_REG = LCD_Reg;
  LCD_RAM = LCD_RegValue;
}

uint16 SSD1289::ReadReg(uint8_t LCD_Reg)
{
  LCD_REG = LCD_Reg;
  volatile uint16 t = LCD_REG_READ;
	return t;
}

void SSD1289::displayOrientation(uint8 mode) {
  uint16 drvout =  DRIVEROUTPUT_BASE; // 0x193F
  uint16 entrymode = ENTRYMODE_BASE;
//  driveroutput = DRIVEROUTPUT_BASE | Bit(BIT_TB,0) | Bit(BIT_RL,0) | Bit(BIT_REV, 0);
//  entrymode = ENTRYMODE_BASE | Bit(BIT_AM,1);
  switch (mode) {
    case PORTRAIT_REV:
//   0x693F ); // driver output control
//  RL=1, REV=1, (,CAD=0, BGR=1, SM=0,)TB=0 
//    //0x6070 ); // entry mode
//   (TY1:0=01,ID1:0 =11,) AM=0,
      drvout |= BIT_RL | BIT_REV;
    // entrymode untouched.
      reg_setx = REG_SET_GDDRAM_X;
      reg_sety = REG_SET_GDDRAM_Y;
      height = LCD_PIXEL_HEIGHT;
      width = LCD_PIXEL_WIDTH;
    break;
    case LANDSCAPE:
  // LANDSCAPE: // works
//  0010100100111111
//  RL=0,REV=1 (,CAD=0, BGR=1, SM=0,)TB=0 (, MUX8:0)
//  0110000001111000
//  (TY1:0=01,ID1:0 =11,) AM=1,
      drvout |= BIT_REV;
      entrymode |= BIT_AM;
      reg_setx = REG_SET_GDDRAM_Y;
      reg_sety = REG_SET_GDDRAM_X;
      height = LCD_PIXEL_WIDTH;
      width = LCD_PIXEL_HEIGHT;
    break;
    case LANDSCAPE_REV:
 //     case LANDSCAPE_REV: // works
 //         WriteCmdData( 0x01, 0x6B3F ); // driver output control
//  rl, rev, bgr, tb
//  am
      drvout |= BIT_RL | BIT_REV | BIT_TB;
      entrymode |= BIT_AM;
      reg_setx = REG_SET_GDDRAM_Y;
      reg_sety = REG_SET_GDDRAM_X;
      height = LCD_PIXEL_WIDTH;
      width = LCD_PIXEL_HEIGHT;
    break;
    case PORTRAIT:
    default:
      // WriteCmdData( 0x01, 0x2B3F ); // driver output control
      // WriteCmdData( 0x11, 0x6070 ); // entry mode
      drvout |= BIT_REV | BIT_TB;
      // entrymode untouched.
      reg_setx = REG_SET_GDDRAM_X;
      reg_sety = REG_SET_GDDRAM_Y;
      height = LCD_PIXEL_HEIGHT;
      width = LCD_PIXEL_WIDTH;
    break;
  }
  WriteReg(REG_DRIVER_OUTPUT_CONTROL, drvout);//0x2B3F);
  delay_ms(1);
  WriteReg(REG_ENTRY_MODE, entrymode); //0x6070);
}

void SSD1289::DisplayOn(void)
{
  WriteReg(REG_DISPLAY_CONTROL, 0x0173); 
}

void SSD1289::DisplayOff(void)
{
  WriteReg(REG_DISPLAY_CONTROL, 0x0000); 
}

void SSD1289::SetColors(uint16_t _TextColor, uint16_t _BackColor) {
  fgColor = _TextColor; 
  bgColor = _BackColor;
}

void SSD1289::GetColors(uint16_t *_TextColor, uint16_t *_BackColor) {
  *_TextColor = fgColor; 
  *_BackColor = bgColor;
}

void SSD1289::SetTextColor(uint16_t Color) {
  fgColor = Color;
}

void SSD1289::SetBackColor(uint16_t Color) {
  bgColor = Color;
}

/* 8x8=8 12x12=12 8x16=16 12x12=14 16x24=24 */
void SSD1289::textSize(uint16_t size) {
  charsize= size;
}

void SSD1289::clear(uint16_t Color) {
  uint32_t index = 0;
  SetCursor(0x00, 0x00);
  WriteRAM_Prepare();
  for(index = 0; index <76800; index++) {
    LCD_RAM = Color;
  }  
}

void SSD1289::SetCursor(uint16_t Xpos, uint16_t Ypos) {
//  WriteReg(REG_SET_GDDRAM_X, Xpos);
//  WriteReg(REG_SET_GDDRAM_Y, Ypos);
  WriteReg(reg_setx, Xpos);
  WriteReg(reg_sety, Ypos);
}

void SSD1289::PutPixel(int16_t x, int16_t y) { 
  PutPixel(x, y, fgColor);
}

void SSD1289::PutPixel(int16_t x, int16_t y, int16_t color)
{ 
  if ( x < 0 || x >= width || y < 0 || y >= height ) return;
	SetCursor(x,y);
	WriteRAM_Prepare();
	WriteRAM(color);
}

void SSD1289::PutChar(int16_t PosX, int16_t PosY, char c) {
	textCursorX = PosX;
	textCursorY = PosY;
	print(c);
}

void SSD1289::print(char c) 
{
	uint8 width = 0;
	uint8 height = 0;
	
  uint8_t i = 0;
  uint8_t j = 0;

  uint8  Buffer8[16];
  uint16  Buffer[24];
  uint16 TmpChar = 0;
	
  if(charsize==8) {		
		width = 8;
		height = 8;
		GetASCIICode1(Buffer8,c);
		for (i=0; i<8 ;i++) {
			TmpChar = Buffer8[i];
			for (j=0; j<8; j++) {
				if ( ((TmpChar >> (7-j)) & 0x01) == 0x01) {
					PutPixel(textCursorX +j, textCursorY + i, fgColor); 
				} else {
					PutPixel(textCursorX + j, textCursorY + i, bgColor);
				}
			}
		}
  }
  //----------------------------------------------------------------------------
    if(charsize==12)
  {
//  uint8_t Buffer[12];
//  uint8_t TmpChar = 0;
		width = 12;
		height = 12;
  
    GetASCIICode2(Buffer8,c);
		for (i=0;i<12;i++) {
			TmpChar = Buffer8[i];
			for (j=0;j<8;j++)
			{
				if ( ((TmpChar >> (7-j)) & 0x01) == 0x01)
				{
					PutPixel(textCursorX +j, textCursorY + i, fgColor); 
				}
				else
				{
          PutPixel(textCursorX + j, textCursorY + i, bgColor);
				}
			}
		}
  }
  //----------------------------------------------------------------------------
    if(charsize==16)
  {
//  unsigned char Buffer[16];
//  uint8_t TmpChar = 0;
		width = 8;
		height = 16;

    GetASCIICode4(Buffer8,c);
		for (i=0;i<16;i++) {
			TmpChar = Buffer8[i];
      SetCursor(textCursorX +j, textCursorY + i);
			for (j=0;j<8;j++) {
				if ( ((TmpChar >> (7-j)) & 0x01) == 0x01)
				{
//          PutPixel(textCursorX +j, textCursorY + i, fgColor); 
          	WriteRAM_Prepare();
            WriteRAM(fgColor);
				} else {
          //PutPixel(textCursorX + j, textCursorY + i, bgColor);
          	WriteRAM_Prepare();
            WriteRAM(bgColor);
				}
			}
		}
  }
  //----------------------------------------------------------------------------
  if(charsize==14) {
//  short int  Buffer[12];
//  uint16_t TmpChar = 0;
		width = 8;
		height = 14;
  
    GetASCIICode3(Buffer,c);
    for (i=0;i<12;i++) {
      TmpChar = Buffer[i];
      for (j=0;j<12;j++) {
        if ( ((TmpChar >> j ) & (0x01)) == 0x01) {
          PutPixel(textCursorX +j, textCursorY + i, fgColor); 
        } else {
          PutPixel(textCursorX + j, textCursorY + i, bgColor);
        }
      }
    }
  }
  //----------------------------------------------------------------------------
  if(charsize==24) {
//  short int  Buffer[24];
//  uint16_t TmpChar = 0;
		width = 16;
		height = 24;
    GetASCIICode5(Buffer,c);
    for (i=0;i<24;i++)
    {
      TmpChar = Buffer[i];
      for (j=0;j<16;j++)
      {
        if (( (TmpChar >>j)&(0x01)) == 0x01) {
          PutPixel(textCursorX +j, textCursorY + i, fgColor); 
        } else {
          PutPixel(textCursorX + j, textCursorY + i, bgColor);
        }
      }
    }
  }
  //----------------------------------------------------------------------------

	textCursorX += width;
	if ( textCursorX > 240 - width or c == 0x0d or c == 0x0a) {
		textCursorX = 0;
		textCursorY += height + 1;
	}
	if ( textCursorY > 320 ) {
		textCursorY = 0;
	}
}

void SSD1289::StringLine(uint16_t PosX, uint16_t PosY, const char *str) {
	textCursorX = PosX;
	textCursorY = PosY;	
	print(str);
}

void SSD1289::print(const char *str) {
	while (*str != 0)	{
		print(*str++);
	}
}



void SSD1289::DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
  uint32_t i = 0;
  
  SetCursor(Xpos, Ypos);
  if(Direction == LCD_DIR_HORIZONTAL)
  {
    WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(i = 0; i < Length; i++)
    {
      WriteRAM(fgColor);
    }
  }
  else
  {
    for(i = 0; i < Length; i++)
    {
      WriteRAM_Prepare(); /* Prepare to write GRAM */
      WriteRAM(fgColor);
      Xpos++;
      SetCursor(Xpos, Ypos);
    }
  }

}


void SSD1289::DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
 int x,y;
 x=0;
 y=0;
 while(x<Height+1)
 {
   PutPixel(Xpos+x,Ypos);
   PutPixel(Xpos+x,Ypos+Width);
   x++;
 }
 while(y<Width+1)
 {
   PutPixel(Xpos,Ypos+y);
   PutPixel(Xpos+Height,Ypos+y);
   y++;
 }
}

void SSD1289::DrawSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a)
{
 int x,y;
 x=0;
 y=0;
 while(x<a+1)
 {
   PutPixel(Xpos+x,Ypos);
   PutPixel(Xpos+x,Ypos+a);
   x++;
 }
 while(y<a+1)
 {
   PutPixel(Xpos,Ypos+y);
   PutPixel(Xpos+a,Ypos+y);
   y++;
 }
}


void SSD1289::DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int16_t  D;/* Decision Variable */ 
  uint16_t  CurX;/* Current X Value */
  uint16_t  CurY;/* Current Y Value */ 
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  while (CurX <= CurY)
  {
    PutPixel(Xpos + CurX, Ypos + CurY);
    PutPixel(Xpos + CurX, Ypos - CurY);
    PutPixel(Xpos - CurX, Ypos + CurY);
    PutPixel(Xpos - CurX, Ypos - CurY);
    PutPixel(Xpos + CurY, Ypos + CurX);
    PutPixel(Xpos + CurY, Ypos - CurX);
    PutPixel(Xpos - CurY, Ypos + CurX);
    PutPixel(Xpos - CurY, Ypos - CurX);
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

void SSD1289::DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int16_t  D;
  uint16_t  CurX;
  uint16_t  CurY;
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY)
  {
    DrawUniLine(Xpos + CurX, Ypos + CurY,Xpos + CurX, Ypos - CurY);
    DrawUniLine(Xpos - CurX, Ypos + CurY,Xpos - CurX, Ypos - CurY);
    DrawUniLine(Xpos + CurY, Ypos + CurX,Xpos + CurY, Ypos - CurX);
    DrawUniLine(Xpos - CurY, Ypos + CurX,Xpos - CurY, Ypos - CurX);
    
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}
void SSD1289::DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  int x,y;
  x=0;
  y=0;
  while(x<Width)
  {
    while(y<Height)
    {
      PutPixel(Xpos+x,Ypos+y);
      y++;
    }
    y=0;
    x++;
  }
}

void SSD1289::DrawFullSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a)
{
  int x,y;
  x=0;
  y=0;
  while(x<a)
  {
    while(y<a)
    {
      PutPixel(Xpos+x,Ypos+y);
      y++;
    }
    y=0;
    x++;
  }
}



void SSD1289::DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }
  
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    PutPixel(x, y);             /* Draw the current pixel */
    num += numadd;              /* Increase the numerator by the top of the fraction */
    if (num >= den)             /* Check if numerator >= denominator */
    {
      num -= den;               /* Calculate the new numerator value */
      x += xinc1;               /* Change the x as appropriate */
      y += yinc1;               /* Change the y as appropriate */
    }
    x += xinc2;                 /* Change the x as appropriate */
    y += yinc2;                 /* Change the y as appropriate */
  }
}

void SSD1289::init(void) {
  CtrlLinesConfig();
	//  delay_us(300);
  FSMCConfig();
//  delay_us(300);
  TIM_Config();
  BackLight(33);
}

void SSD1289::CtrlLinesConfig(void)
{
	GPIOMode(PinPort(PD0), PinBit(PD0) | PinBit(PD1) | PinBit(PD4) | PinBit(PD5) | PinBit(PD7) | PinBit(PD8)
												 | PinBit(PD9) | PinBit(PD10) | PinBit(PD11) | PinBit(PD14) | PinBit(PD15), 
												 ALTFUNC, HIGHSPEED, PUSHPULL, NOPULL);
	GPIOAltFunc(PinPort(PD0), PinBit(PD0) | PinBit(PD1) | PinBit(PD4) | PinBit(PD5) | PinBit(PD7) | PinBit(PD8)
												 | PinBit(PD9) | PinBit(PD10) | PinBit(PD11) | PinBit(PD14) | PinBit(PD15), GPIO_AF_FSMC);
	
	GPIOMode(PinPort(PE0), PinBit(PE7) | PinBit(PE8) | PinBit(PE9) | PinBit(PE10) | PinBit(PE11) | PinBit(PE12)
												 | PinBit(PE13) | PinBit(PE14) | PinBit(PE15), 
												 ALTFUNC, HIGHSPEED, PUSHPULL, NOPULL);
	GPIOAltFunc(PinPort(PE0), PinBit(PE7) | PinBit(PE8) | PinBit(PE9) | PinBit(PE10) | PinBit(PE11)| PinBit(PE12)
												 | PinBit(PE13) | PinBit(PE14) | PinBit(PE15), GPIO_AF_FSMC);
	
	// Reset
	GPIOMode(PinPort(NRSTpin), PinBit(NRSTpin), OUTPUT, MEDSPEED, PUSHPULL, PULLUP);
	digitalWrite(NRSTpin, HIGH);
/*
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF, ENABLE);
 // RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);		// D2
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);		// D3
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);		// NOE -> RD
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);		// NWE -> WR
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);		// NE1 -> CS
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);		// D13
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);		// D14
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);		// D15
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);		// A16 -> RS
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);		// D0
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);		// D1
  
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);		// D4
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);		// D5
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);		// D6
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);		// D7
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);		// D8
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);		// D9
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);		// D10
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);		// D11
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);		// D12

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOE, &GPIO_InitStructure);
*/  
}


void SSD1289::FSMCConfig(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;
	
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/* FSMC Read/Write Timing */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 5;  /* Address Setup Time  */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	   
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 5;     /* Data Setup Time */
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC Access Mode */
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	
	/* FSMC Write Timing */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 15;//2;//1;   /* Address Setup Time */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	   
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 15;//2;//1;	     /* Data Setup Time */
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC Access Mode */
	
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;	  
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	
	/* Enable FSMC Bank4_SRAM Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  

}



  
void SSD1289::PolyLine(pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0;

  if(PointCount < 2)
  {
    return;
  }

  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    DrawUniLine(X, Y, Points->X, Points->Y);
  }
}

void SSD1289::PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed)
{
  int16_t X = 0, Y = 0;
  pPoint First = Points;

  if(PointCount < 2)
  {
    return;
  }  
  X = Points->X;
  Y = Points->Y;
  while(--PointCount)
  {
    Points++;
    DrawUniLine(X, Y, X + Points->X, Y + Points->Y);
    X = X + Points->X;
    Y = Y + Points->Y;
  }
  if(Closed)
  {
    DrawUniLine(First->X, First->Y, X, Y);
  }  
}

void SSD1289::ClosedPolyLine(pPoint Points, uint16_t PointCount)
{
  PolyLine(Points, PointCount);
  DrawUniLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
}


void SSD1289::PolyLineRelative(pPoint Points, uint16_t PointCount)
{
  PolyLineRelativeClosed(Points, PointCount, 0);
}


void SSD1289::ClosedPolyLineRelative(pPoint Points, uint16_t PointCount)
{
  PolyLineRelativeClosed(Points, PointCount, 1);
}

void SSD1289::FillPolyLine(pPoint Points, uint16_t PointCount)
{
  /*  public-domain code by Darel Rex Finley, 2007 */
  uint16_t  nodes = 0, nodeX[MAX_POLY_CORNERS], pixelX = 0, pixelY = 0, i = 0,
  j = 0, swap = 0;
  uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

  IMAGE_LEFT = IMAGE_RIGHT = Points->X;
  IMAGE_TOP= IMAGE_BOTTOM = Points->Y;

  for(i = 1; i < PointCount; i++)
  {
    pixelX = POLY_X(i);
    if(pixelX < IMAGE_LEFT)
    {
      IMAGE_LEFT = pixelX;
    }
    if(pixelX > IMAGE_RIGHT)
    {
      IMAGE_RIGHT = pixelX;
    }
    
    pixelY = POLY_Y(i);
    if(pixelY < IMAGE_TOP)
    { 
      IMAGE_TOP = pixelY;
    }
    if(pixelY > IMAGE_BOTTOM)
    {
      IMAGE_BOTTOM = pixelY;
    }
  }
  
  SetTextColor(bgColor);  

  /*  Loop through the rows of the image. */
  for (pixelY = IMAGE_TOP; pixelY < IMAGE_BOTTOM; pixelY++) 
  {  
    /* Build a list of nodes. */
    nodes = 0; j = PointCount-1;

    for (i = 0; i < PointCount; i++) 
    {
      if (((POLY_Y(i)<(double) pixelY) && (POLY_Y(j)>=(double) pixelY)) || \
          ((POLY_Y(j)<(double) pixelY) && (POLY_Y(i)>=(double) pixelY)))
      {
        nodeX[nodes++]=(int) (POLY_X(i)+((pixelY-POLY_Y(i))*(POLY_X(j)-POLY_X(i)))/(POLY_Y(j)-POLY_Y(i))); 
      }
      j = i; 
    }
  
    /* Sort the nodes, via a simple "Bubble" sort. */
    i = 0;
    while (i < nodes-1) 
    {
      if (nodeX[i]>nodeX[i+1]) 
      {
        swap = nodeX[i]; 
        nodeX[i] = nodeX[i+1]; 
        nodeX[i+1] = swap; 
        if(i)
        {
          i--; 
        }
      }
      else 
      {
        i++;
      }
    }
  
    /*  Fill the pixels between node pairs. */
    for (i = 0; i < nodes; i+=2) 
    {
      if(nodeX[i] >= IMAGE_RIGHT) 
      {
        break;
      }
      if(nodeX[i+1] > IMAGE_LEFT) 
      {
        if (nodeX[i] < IMAGE_LEFT)
        {
          nodeX[i]=IMAGE_LEFT;
        }
        if(nodeX[i+1] > IMAGE_RIGHT)
        {
          nodeX[i+1] = IMAGE_RIGHT;
        }
        SetTextColor(bgColor);
        DrawLine(pixelY, nodeX[i+1], nodeX[i+1] - nodeX[i], LCD_DIR_HORIZONTAL);
        SetTextColor(fgColor);
        PutPixel(pixelY, nodeX[i+1]);
        PutPixel(pixelY, nodeX[i]);
        /* for (j=nodeX[i]; j<nodeX[i+1]; j++) PutPixel(j,pixelY); */
      }
    }
  } 

  /* draw the edges */
  SetTextColor(fgColor);
}



void SSD1289::BackLight(int procentai)
{
  if (procentai>100)
    {procentai=100;}
  else if(procentai<0)
    {procentai=0;}
  Channel3Pulse =(uint16_t)(((uint32_t)procentai*(TimerPeriod-1))/100);
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);
}

void SSD1289::TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA  , ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
  TimerPeriod = (SystemCoreClock / 17570 ) - 1;
  Channel3Pulse = (uint16_t) (((uint32_t) 99 * (TimerPeriod - 1)) / 100);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);
  TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
        
}

void SSD1289::SetDisplayWindow(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
  if(Xpos >= Height)
  {
    WriteReg(0x0044, (Xpos - Height +1));
  }
  else
  {
    WriteReg(0x0044, 0x0000);
  }
  if(Ypos >= Width)
  {
    WriteReg(0x0045, (Ypos - Width +1));
  }  
  else
  {
    WriteReg(0x0045, 0x0000);
  }
  WriteReg(0x0046, Ypos);
  SetCursor(Xpos, Ypos);

}

void SSD1289::WriteBMP(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, uint8_t *bitmap)
{
  volatile uint32_t index;
  uint16_t *bitmap_ptr = (uint16_t *)bitmap;
  uint16_t i;
  uint32_t size;
  size=(Height * Width);
  SetDisplayWindow(Xpos, Ypos,Height, Width);

  WriteReg(REG_ENTRY_MODE, 0x6048);
 
  WriteRAM_Prepare();
 
  for(index = 0; index < size ; index++)
  {
	WriteRAM(*bitmap_ptr++);
		//for(i=0;i<2;i++);
  }
  WriteReg(REG_ENTRY_MODE, 0x6028);
  SetDisplayWindow(0, 0, 239, 319);
}
