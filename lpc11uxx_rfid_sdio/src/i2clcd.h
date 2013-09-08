#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t i2clcd_cmd(unsigned char db);
extern uint8_t i2clcd_data(unsigned char db);
extern uint8_t i2clcd_puts(char * s);
extern uint32_t i2clcd_init( unsigned char contrast );
extern void i2clcd_hex8(unsigned char c);
extern unsigned int i2clcd_decimal(long d);

extern void i2clcd_clear(void);
extern void i2clcd_cursor(uint8_t r, uint8_t c);

#ifdef __cplusplus
}
#endif
