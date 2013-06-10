#include "gpio.h"

void GPIOInit( void )
{
  /* Enable AHB clock to the GPIO domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

  /* Enable AHB clock to the FlexInt, GroupedInt domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= ((1<<19) | (1<<23) | (1<<24));

  return;
}

void pinMode( GPIOPin_Def pin, uint8_t dir ) {
  if( dir ) {
    LPC_GPIO->DIR[pin>>4&0x0f] |= (1<<(pin&0x0f));
  } else {
    LPC_GPIO->DIR[pin>>4&0x0f] &= ~(1<<(pin&0x0f));
  }
  return;
}

void digitalWrite(GPIOPin_Def pin, uint8_t bitVal ) {
  if ( bitVal ) {
    LPC_GPIO->SET[pin>>4&0x0f] = 1<<(pin&0x0f);
  } else {
    LPC_GPIO->CLR[pin>>4&0x0f] = 1<<(pin&0x0f);
  }
  return;
}

uint8_t digitalRead(GPIOPin_Def pin) {
  return (LPC_GPIO->PIN[pin>>4&0x0f] & (1<<(pin&0x0f))) != 0;
}

uint8_t digitalToggle(GPIOPin_Def pin) {
  if ( LPC_GPIO->PIN[pin>>4&0x0f] & (1<<(pin&0x0f)) ) {
    LPC_GPIO->CLR[pin>>4&0x0f] = 1<<(pin&0x0f);
    return 1;
  } else {
    LPC_GPIO->SET[pin>>4&0x0f] = 1<<(pin&0x0f);
    return 0;
  }
}

