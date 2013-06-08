

#include "LPC11uxx.h"			/* LPC11uxx Peripheral Registers */
#include "type.h"
#include "i2c.h"
#include "gpio.h"
#include "i2clcd.h"
#include "systick.h"

//const unsigned char contrast = 0x28;//0b00101000;	// 3.0V時 数値を上げると濃くなります。



#define I2C_LCD_RST          25



extern volatile uint32_t I2CCount;
extern volatile uint8_t I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CMasterState;
extern volatile uint32_t I2CReadLength, I2CWriteLength;




// コマンドを送信します。HD44780でいうRS=0に相当
uint8_t i2c_cmd(unsigned char db)
{
	return i2c_write(0x7c, 0x00, db);
}

// データを送信します。HD44780でいうRS=1に相当
uint8_t i2c_data(unsigned char db)
{
	return i2c_write(0x7c, 0x40, db);
}

// （主に）文字列を連続送信します。
uint8_t i2c_puts(unsigned char *s)
{
        uint8_t re;
  
	while(*s){
          re = i2c_data(*s++);
//          if(re<0) return re;
	}
        return 0;
}

uint8_t i2c_clear()
{
        uint8_t re;
        
  	re = i2c_cmd(0x01);
        if(re) return re;
        
	wait_ms(2);
        return 0;
}



uint32_t i2clcd_init( unsigned char contrast)
{
        GPIOSetDir(1, I2C_LCD_RST, 1 );

        GPIOSetBitValue( 1, I2C_LCD_RST, 0 );
        wait_ms(10);
        GPIOSetBitValue( 1, I2C_LCD_RST, 1 );

	
	// ここからI2C LCDの初期化を行います。
	wait_ms(40);
	i2c_cmd(0x38);//0b00111000); // function set
	i2c_cmd(0x39);//0b00111001); // function set
	i2c_cmd(0x14);//0b00010100); // interval osc
	i2c_cmd(0x70 | (contrast & 0xF)); // contrast Low
        
	//i2c_cmd(0b01011100 | ((contrast >> 4) & 0x3)); // contast High/icon/power
	i2c_cmd(0x5C | ((contrast >> 4) & 0x3)); // contast High/icon/power
	i2c_cmd(0x6C); // follower control
	wait_ms(300);

	i2c_cmd(0x38);//0b00111000); // function set
	i2c_cmd(0x0c);//0b00001100); // Display On
	
	i2c_cmd(0x01);//0b00000001); // Clear Display
	wait_ms(2);			 // Clear Displayは追加ウェイトが必要
  
  
  return 0;
}



static const unsigned char hex[16]="0123456789ABCDEF";

static unsigned char _hex10[10+1+2];

void i2c_lcd_hex8(unsigned char c)
{
	i2c_data(hex[c>>4]);
	i2c_data(hex[c&15]);
}


unsigned int i2c_lcd_decimal(long d)
{
	unsigned char *p;
	unsigned char n;
	

	if(d<0){
		i2c_data('-');
		return i2c_lcd_decimal(-d);
	}
	if(!d){
		i2c_data('0');
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
		i2c_data(*p++); n++;
	}
	return n;
}



