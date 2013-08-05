/*
 * STMiroelectronics L6470 dSPIN stepper motor driver control
 *
 *
 *      (c) 2012 Strawberry Linux Co.,Ltd.
 *              2012/12/31 Masahiro Ochiai
 *
 *      note: must connect SSP1 port
 */


#include "LPC11uxx.h"			/* LPC11uxx Peripheral Registers */
#include "cappuccino.h"
#include "gpio.h"
#include "ssp.h"
#include "i2clcd.h"
#include "xprintf.h"


/*
 *      L6470 ~CS = PORT1_23
 */

#define CS(n)   GPIOSetBitValue( PORT1, 23, (n));	


static void spi_out(unsigned char d)
{
  CS(0);
  SSP_Send(1, &d, 1);
  CS(1);
}

static unsigned char spi_in()
{
  unsigned char buff;

  CS(0);
  SSP_Receive(1, &buff, 1);
  CS(1);

  return buff;
}



static void wait()
{
	volatile long i;
	
	for(i=0;i<20000;i++);
}



void l6470_reset()
{
	int i;
        
	for(i=0;i<5;i++){
		spi_out(0x0);   // dummy write
	}
	spi_out(0xC0);  // L6470 reset command
        wait();
}


void l6470_kval(unsigned char hold, unsigned char run,unsigned char acc,unsigned char dec)
{
	spi_out(0x09);
	spi_out(hold);

	spi_out(0x0A);
	spi_out(run);

	spi_out(0x0B);
	spi_out(acc);

	spi_out(0x0C);
	spi_out(dec);
	
}

void l6470_slope(unsigned short spd, unsigned char start, unsigned char acc, unsigned char dec)
{
	spi_out(0x0D);
	spi_out(spd>>8);
	spi_out(spd);
	
	spi_out(0x0E);
	spi_out(start);

	spi_out(0x0F);
	spi_out(acc);
	
	spi_out(0x10);
	spi_out(dec);
	
}

void l6470_run(unsigned char dir, unsigned long a)
{
	spi_out(0x50 | (dir&1));
	spi_out(a>>16);
	spi_out(a>>8);
	spi_out(a);

}


void l6470_move(unsigned char dir, unsigned long a)
{
	spi_out(0x40 | (dir & 1));
	spi_out(a>>16);
	spi_out(a>>8);
	spi_out(a);
}




unsigned long l6470_getspeed()
{
	unsigned long s;
//	unsigned char adc;
	
	spi_out(0x20 + 0x04);
	s = (unsigned long)spi_in();
	s = (s<<8) | (unsigned long)spi_in();
	s = (s<<8) | (unsigned long)spi_in();
	

//	spi_out(0x20 + 0x12);
//	adc = spi_in();

	
//	i2c_cmd(0x80);
	//i2c_lcd_hex8(adc);
	
	return s;
}

unsigned short l6470_getstatus()
{
	unsigned long s;
	
	spi_out(0x20 + 0x19);
	s = (unsigned short)spi_in();
	s = (s<<8) | (unsigned short)spi_in();
        
	return s;
}


unsigned short l6470_getconfig()
{
	unsigned long s;
	
	spi_out(0x20 + 0x1A);
	s = (unsigned short)spi_in();
	s = (s<<8) | (unsigned short)spi_in();
        
	return s;
}



unsigned char l6470_isstopped()
{
    if(!(l6470_getstatus() & 0x60)) return 1;            // stop
       return 0;
  
}


void l6470_init()
{
	OUTPUT(1, 11);
	HIGH(1, 11);

	HIGH(1, 8);
	SSP_IOConfig(1);
	SSP_Init( 1 );
}



void l6470_demo()
{
  int i;

    l6470_reset();
    
	spi_out(0x7);	// max speed
	spi_out(0x0);
	spi_out(0x20);//c4);
	
	
	spi_out(0x18);	// config
	spi_out(0x2e);//1F);//2F);//1C);
	spi_out(0x88);//A0

	spi_out(0x16);	// step mode
	spi_out(0x7);

	
	spi_out(0x13);	// OCD-TH
	spi_out(0xf);

	spi_out(0x14);	// STALL TH
	spi_out(0x7f);

	spi_out(0x15);
	spi_out(0);			//1
	spi_out(0x27);		//0x26

	
//	l6470_slope(00,00,00,0);
	
//l6470_kval(0x1f,0x1f,110,90);
	l6470_kval(255,255,255,200);

	while(1){
		while(1){
                      i2c_cmd(0x80);
                    xprintf("Forward");

                    l6470_move(0,200*1*128UL);
                    while(!l6470_isstopped());        // ��]���I���܂ő҂�
                    wait();

                      i2c_cmd(0x80);
                    xprintf("Reverse");
                    l6470_move(1,200*1*128UL);
                    while(!l6470_isstopped());        // ��]���I���܂ő҂�
                    wait();

                }

		l6470_move(1,100*128UL);
		while(l6470_getspeed());
		l6470_move(0,200*128UL);
		while(l6470_getspeed());
		l6470_move(1,100*128UL);
		while(l6470_getspeed());
		l6470_move(1,10*128UL);
		while(l6470_getspeed());
		for(i=0;i<20;i++){
			l6470_move(0,200*128UL);
			while(l6470_getspeed());
			wait();
			l6470_move(1,200*128UL);
			while(l6470_getspeed());
			wait();
		}
	}
}



