
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
		i2c i2cbuf;
		GPIOPin sclpin, sdapin;

		uint8 dstaddress;
		uint8 rxbuffer[I2C_BUFFER_SIZE];
		uint8 txbuffer[I2C_BUFFER_SIZE];
		int16 rxposition, txposition;
		int16 rxlength, txlength;
	
  public:
    I2CWire(I2C_TypeDef * I2Cx, GPIOPin scl, GPIOPin sda) : i2cx(I2Cx), sclpin(scl), sdapin(sda) {
		}
    void begin(uint32_t clk = 100000);
	
    void beginTransmission(uint8);
//    void beginRequest(uint8);
		uint8_t endTransmission(uint8 = true);
		uint8_t endRequest(void);

		uint8_t receiveFrom(uint8, uint16);
		uint8_t requestFrom(uint8 addr, uint16 qtty) { return receiveFrom(addr, qtty); } 
	
    virtual size_t write(uint8_t);
		virtual inline size_t write(int8 v) { return write((uint8) v); }
		virtual size_t available(void) {
		uint16 d = (rxlength - rxposition);
			return (d>0? (size_t)d: 0);
		}
    virtual int16 read(void) {
			return rxbuffer[rxposition++];
		}
    virtual int16 peek(void) {
			return rxbuffer[rxposition];
		}
		virtual void flush(void);
//    void onReceive( void (*)(int) );
//    void onRequest( void (*)(void) );
  
    using Print::write;
};

//extern I2CWire Wire1(I2C1);

#endif

