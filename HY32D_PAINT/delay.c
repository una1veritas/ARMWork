#include "delay.h"

static __IO uint32_t TimingDelay ;

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0)
  {
	  TimingDelay --;
  }
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

