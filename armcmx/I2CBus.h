
#ifndef I2CBus_h
#define I2CBus_h

#include <inttypes.h>
#include "gpio.h"
#include "i2c.h"
#include "Stream.h"

#define BUFFER_LENGTH I2C_BUFFER_SIZE

class I2CBus : public Stream
{
  private:
		I2CDef * i2cx;
//		i2c i2cbuf;
		GPIOPin sclpin, sdapin;

		uint8_t dstaddress;
		uint8_t rxbuffer[I2C_BUFFER_SIZE];
		uint8_t txbuffer[I2C_BUFFER_SIZE];
		int16_t rxposition, txposition;
		int16_t rxlength, txlength;
  
  public:
    uint32_t status;
	
  public:
    I2CBus(I2CDef * I2Cx, GPIOPin scl = 0, GPIOPin sda = 0) : i2cx(I2Cx), sclpin(scl), sdapin(sda) {
		}
    void begin(uint32_t clk = 100000);
	
    void beginTransmission(uint8_t);
//    void beginRequest(uint8);
		uint8_t endTransmission(uint8_t = true);
		uint8_t endRequest(void);

		uint8_t receiveFrom(uint8_t, uint16_t);
		uint8_t requestFrom(uint8_t addr, uint16_t qtty) { return receiveFrom(addr, qtty); } 
	
    virtual size_t write(uint8_t);
		virtual inline size_t write(int v) { return write((uint8_t) v); }
		virtual int available(void) {
		uint16_t d = (rxlength - rxposition);
			return (d>0? (size_t)d: 0);
		}
    virtual int read(void) {
			return rxbuffer[rxposition++];
		}
    virtual int peek(void) {
			return rxbuffer[rxposition];
		}
		virtual void flush(void);
//    void onReceive( void (*)(int) );
//    void onRequest( void (*)(void) );
  
    using Print::write;
};

extern I2CBus Wire;

#endif

