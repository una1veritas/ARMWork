// (c) Wolfgang Wieser 2012. GPL v2. 

#ifdef TEST_ON_HOST
#include <stdio.h>
#endif

#include "mandelbrot.h"
#include "stm32f4xx_gpio.h"
#include "math.h"


// Eval Mandelbrot at x,y in pixel coos. 
int ComputeMandel_XY(const struct MandelContext *c,int px,int py)
{
	// Compute C plane coos. 
	float a = (float)px/(c->width-1)  * c->xs + c->x0;
	float b = (float)py/(c->height-1) * c->ys + c->y0;
	
	int i=0;
	float x=0,y=0;
	while(i<c->maxit)
	{
		float xx = x*x;
		float yy = y*y;
		x = xx - yy + a;
		y = 2*x*y + b;
		if(xx+yy>4) break;
		++i;
	}
	
	return(i);
}


// Returns number of black pixels. 
int ComputeMandelbrot(int w,int h)
{
	
	struct MandelContext c;
	c.width=w;
	c.height=h;
	
	// Coordinates for the full apple man. 
	c.x0=-2.5f;
	c.y0=-1.5f;
	c.xs=4;
	c.ys=3;
	c.maxit=255;
	
	int x,y,sum=0;
	for(y=0; y<c.height; y++)
	{
		for(x=0; x<c.width; x++)
		{
			int r=ComputeMandel_XY(&c,x,y);
			sum += r>=c.maxit ? 1 : 0;
#ifdef TEST_ON_HOST
			printf("%c", r>=c.maxit ? '#' : ' ');
#else
			if(r>=c.maxit) GPIO_SetBits(GPIOD, GPIO_Pin_13);
			else           GPIO_ResetBits(GPIOD, GPIO_Pin_13);
			
			if(r>=c.maxit) GPIO_ResetBits(GPIOD, GPIO_Pin_14);
			else           GPIO_SetBits(GPIOD, GPIO_Pin_14);
#endif
		}
#ifdef TEST_ON_HOST
		printf("|\n");
#endif
	}
	
	return(sum);
}

#define nop()       __asm__ __volatile__("nop")

int SqrtBenchmark(void)
{
	int i,r=0;
	for(i=0; i<1000000; i++)
	{
		float f=i;
		GPIOD->BSRRL = GPIO_Pin_15;  // Take pin HIGH. 
		volatile float r=sqrtf(f);   // Compute sqrt. 20 cycles. 
		GPIOD->BSRRH = GPIO_Pin_15;  // Take pin LOW. 
		nop();
		nop();
		nop();
		nop();    // 4 nops -> 30ns
		nop();    // 5 nops -> 36ns  -> 168 MHz, single cycle exec
		GPIOD->BSRRL = GPIO_Pin_15; // Take pin HIGH. 
		nop();
		nop();
		nop();
		nop();
		GPIOD->BSRRH = GPIO_Pin_15;
		if(r>10) ++r;
	}
	return(r);
}


#ifdef TEST_ON_HOST
int main()
{
	int sum=ComputeMandelbrot(2/*80*/,2/*25*/);
	printf("sum=%d\n",sum);
}

#endif
