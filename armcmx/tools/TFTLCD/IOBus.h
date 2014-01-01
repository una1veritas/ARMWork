#ifndef _pin_magic_
#define _pin_magic_

#define DELAY7        \
  asm volatile(       \
    "rjmp .+0" "\n\t" \
    "rjmp .+0" "\n\t" \
    "rjmp .+0" "\n\t" \
    "nop"      "\n"   \
    ::);

// Direct bit access via GPIO registers
#ifdef __AVR__

#define PIN_NOT_DEFINED 0xff
#define bitset(p,b)    { p |= (1<<b); }
#define bitclear(p,b)  { p &= ~(1<<b); }
#define bittest(p,b)   (( p & (1<<b)) != 0)

#define IO8BUS_MODE(m)  { DDRB = (m == OUTPUT ? DDRB | 0x03 : DDRB & ~0x03); DDRC = (m == OUTPUT ? DDRC | 0xfc : DDRC & ~0xfc); }
#define IO8BUS_WRITE(v) { PORTB = (PORTB & ~0x03) | (v & 0x03); PORTC = (PORTC & ~0xfc) | (v & 0xfc); }
#define IO8BUS_READ()   ((PINB & 0x03) | (PINC & 0xfc))

#define OUTPUTMODE     { DDRB |= 0xf0; }
#define CS_ACTIVE 	bitclear(PORTB,4)
#define CS_IDLE 	bitset(PORTB,4)
#define CD_COMMAND 	bitclear(PORTB,5)
#define CD_DATA  	bitset(PORTB,5)
#define WR_ACTIVE 	bitclear(PORTB,6)
#define WR_IDLE 	bitset(PORTB,6)
#define RD_ACTIVE 	bitclear(PORTB,7)
#define RD_IDLE 	bitset(PORTB,7)

#define RST_IDLE
#define RST_ACTIVE
#define RST  PIN_NOT_DEFINED
#define RST_OUTPUT

#endif

  // Data write strobe, ~2 instructions and always inline
#define WR_STROBE { WR_ACTIVE; WR_IDLE; }

/*
// Data write strobe, ~2 instructions and always inline
#define WR_STROBE { WR_ACTIVE; WR_IDLE; }

// These higher-level operations are usually functionalized,
// except on Mega where's there's gobs and gobs of program space.

// Set value of TFT register: 8-bit address, 8-bit value
#define writeRegister8inline(a, d) { \
  CD_COMMAND; write8(a); CD_DATA; write8(d); }

// Set value of TFT register: 16-bit address, 16-bit value
// See notes at top about macro expansion, hence hi & lo temp vars
#define writeRegister16inline(a, d) { \
  uint8_t hi, lo; \
  hi = (a) >> 8; lo = (a); CD_COMMAND; write8(hi); write8(lo); \
  hi = (d) >> 8; lo = (d); CD_DATA   ; write8(hi); write8(lo); }

// Set value of 2 TFT registers: Two 8-bit addresses (hi & lo), 16-bit value
#define writeRegisterPairInline(aH, aL, d) { \
  uint8_t hi = (d) >> 8, lo = (d); \
  CD_COMMAND; write8(aH); CD_DATA; write8(hi); \
  CD_COMMAND; write8(aL); CD_DATA; write8(lo); }
*/

#endif // _pin_magic_
