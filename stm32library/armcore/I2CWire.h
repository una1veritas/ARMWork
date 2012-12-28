
#ifndef I2CWire_h
#define I2CWire_h

#include <inttypes.h>
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
    void begin();
	
    void beginTransmission(uint8);

	uint8_t endTransmission(void);
		uint8_t endTransmission(uint8);

		uint8_t requestFrom(uint8, uint16);
		uint8_t requestFrom(uint8, uint16, uint8);
	
    virtual size_t write(uint8_t);
	virtual size_t available(void);
    virtual int16 read(void);
    virtual int16 peek(void);
		virtual void flush(void);
//    void onReceive( void (*)(int) );
//    void onRequest( void (*)(void) );
  
    using Print::write;
};

extern I2CWire Wire1(I2C1);

#endif

