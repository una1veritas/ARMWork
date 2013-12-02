
#include "LPC11uxx.h"			/* LPC11uxx Peripheral Registers */
#include "type.h"
#include "i2c.h"
#include "gpio.h"
#include "i2clcd.h"
//#include "systick.h"
#include "delay.h"

//const unsigned char contrast = 0x28;//0b00101000;	// 3.0V�� ���l���グ��ƔZ���Ȃ�܂��B


#define I2C_LCD_RST          25
#define I2CLCD_ADDR  0x3e


// �R�}���h�𑗐M���܂��BHD44780�ł���RS=0�ɑ���
uint8_t i2clcd_cmd(unsigned char db)
{
//	return i2c_write(0x7c, 0x00, db);
	return I2C_write16(&i2c, I2CLCD_ADDR, db);
}

// �f�[�^�𑗐M���܂��BHD44780�ł���RS=1�ɑ���
uint8_t i2clcd_data(unsigned char db)
{
//	return i2c_write(0x7c, 0x40, db);
	return I2C_write16(&i2c, I2CLCD_ADDR, 0x40<<8 | db);
}

// �i��Ɂj�������A�����M���܂��B
uint8_t i2clcd_puts(char *s)
{
  uint8_t re = 0;
  
	while(*s) {
    re += i2clcd_data(*s++);
    // if(re<0) return re;
	}
  return re;
}

void i2clcd_clear()
{
  uint8_t re;

  re = i2clcd_cmd(0x01);
  if(re) return; // re;
        
	//wait_ms(2);
  delay(2);
  return; // 0;
}



uint32_t i2clcd_init( unsigned char contrast)
{
//        GPIOSetDir(1, I2C_LCD_RST, 1 );
  pinMode(PIO1_25, 1);
//        GPIOSetBitValue( 1, I2C_LCD_RST, 0 );
  digitalWrite(PIO1_25, 0);
//
//  wait_ms(10);
  delay(10);
//        GPIOSetBitValue( 1, I2C_LCD_RST, 1 );
  digitalWrite(PIO1_25, 1);
	
	// ��������I2C LCD�̏�����s���܂��B
	//wait_ms
  delay(40);
	i2clcd_cmd(0x38);//0b00111000); // function set
	i2clcd_cmd(0x39);//0b00111001); // function set
	i2clcd_cmd(0x14);//0b00010100); // interval osc
	i2clcd_cmd(0x70 | (contrast & 0xF)); // contrast Low
  
	//i2c_cmd(0b01011100 | ((contrast >> 4) & 0x3)); // contast High/icon/power
	i2clcd_cmd(0x5C | ((contrast >> 4) & 0x3)); // contast High/icon/power
	i2clcd_cmd(0x6C); // follower control
	//wait_ms
  delay(300);
  
	i2clcd_cmd(0x38);//0b00111000); // function set
	i2clcd_cmd(0x0c);//0b00001100); // Display On
	
	i2clcd_cmd(0x01);//0b00000001); // Clear Display
	//wait_ms
  delay(2);			 // Clear Display�͒ǉ��E�F�C�g���K�v
  
  return 0;
}



static const unsigned char hex[16]="0123456789ABCDEF";

static unsigned char _hex10[10+1+2];

void i2clcd_hex8(unsigned char c)
{
	i2clcd_data(hex[c>>4]);
	i2clcd_data(hex[c&15]);
}


unsigned int i2clcd_decimal(long d)
{
	unsigned char *p;
	unsigned char n;	

	if(d<0){
		i2clcd_data('-');
		return i2clcd_decimal(-d);
	}
	if(!d){
		i2clcd_data('0');
		return 1;
	}
	n=0;
	p = _hex10 + sizeof(_hex10);
	*--p = '\0';
	while(d){
		*--p = (d % 10)+'0';
		d /= 10;
	}
	while(*p){
		i2clcd_data(*p++); n++;
	}
	return n;
}


extern void i2clcd_cursor(uint8_t r, uint8_t c) {
  i2clcd_cmd(0x80 + (r == 0? 0 : 0x40) +c);
}
