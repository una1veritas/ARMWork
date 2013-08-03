
#include "LPC11uxx.h"			/* LPC11uxx Peripheral Registers */
#include "type.h"
#include "delay.h"

volatile uint32_t __counter_micros;
volatile uint32_t __counter_millis;

uint32_t SysTickPeriod;

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler(void)
{
	__counter_micros += 1000;
  __counter_millis++;
}

void init_systick_delay() {  
  SystemCoreClockUpdate();
  SysTickPeriod = SystemCoreClock/1000L;
  // systick initialize
  SysTick_Config(SysTickPeriod);
  // Clear SysTick Counter 
  SysTick->VAL = 0;
  // Enable the SysTick Counter 
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
 }

/*
void delaySysTick(uint32_t tick)
{
  uint32_t timetick;

  // Clear SysTick Counter 
  SysTick->VAL = 0;
  // Enable the SysTick Counter 
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

  timetick = TimeTick;
  while ((TimeTick - timetick) < tick);

  // Disable SysTick Counter
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  // Clear SysTick Counter 
  SysTick->VAL = 0;
  return;
}
*/

void start_systick_delay(uint32_t tick)
{
  // Clear SysTick Counter 
  SysTick->VAL = 0;
  // Enable the SysTick Counter 
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void stop_systick_delay(void) {
  // Disable SysTick Counter
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  // Clear SysTick Counter 
  // SysTick->VAL = 0;
}


uint32_t micros(void) {
// 	return __counter_micros + (1000 - SysTick->VAL/48);
	return __counter_micros + (1024 - ((SysTick->VAL*21)>> 10));
}

uint32_t millis(void) {
	return __counter_millis;
}

void delay_ms(uint32_t w) {
	uint32_t wtill = millis() + w;
	if ( wtill < millis() ) {
		while ( millis() >= wtill );
	}
	while (millis() < wtill) ;
}

void delay_us(uint32_t w) {
	uint32_t tim2 = SysTick->VAL;
	for ( ; w > 0 ; ) {
		if (tim2 == SysTick->VAL) 
			continue;
		tim2 = SysTick->VAL;
		w--;
	}
}

