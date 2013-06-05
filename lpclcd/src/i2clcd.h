

extern uint8_t i2c_cmd(unsigned char db);
extern uint8_t i2c_data(unsigned char db);
extern uint8_t i2c_puts(unsigned char *s);
extern uint32_t i2clcd_init( unsigned char contrast );
extern void i2c_lcd_hex8(unsigned char c);
extern unsigned int i2c_lcd_decimal(long d);
