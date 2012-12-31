
#ifndef I2CWire_h
#define I2CWire_h

#include <inttypes.h>
#include "gpio.h"
#include "i2c.h"
#include "Stream.h"

#define BUFFER_LENGTH I2C_BUFFER_SIZE

class I2CWire : public Stream
{
  private:
		I2C_TypeDef * i2cx;
		I2CBuffer * i2cbuf;
	
  public:
    I2CWire(I2C_TypeDef * i2c) : i2cx(i2c) {
			i2cbuf = &I2C1Buffer;
		}
    void begin(GPIOPin sda, GPIOPin scl, uint32_t clk = 100000);
	
    void beginTransmission(uint8);
//    void beginRequest(uint8);
		uint8_t endTransmission(uint8 = true);
		uint8_t endRequest(void);

		uint8_t receiveFrom(uint8, uint16);
		uint8_t requestFrom(uint8 addr, uint16 qtty) { return receiveFrom(addr, qtty); } 
	
    virtual size_t write(uint8_t);
		virtual inline size_t write(int8 v) { return write((uint8) v); }
		virtual size_t available(void) {
		int32 d = ((int32)i2cbuf->limlen - i2cbuf->position);
		return (d>0? (size_t)d: 0);
	}
    virtual int16 read(void) {
			return i2cbuf->buffer[i2cbuf->position++];
		}
    virtual int16 peek(void) {
			return i2cbuf->buffer[i2cbuf->position];
		}
		virtual void flush(void);
//    void onReceive( void (*)(int) );
//    void onRequest( void (*)(void) );
  
    using Print::write;
};

//extern I2CWire Wire1(I2C1);

#endif

