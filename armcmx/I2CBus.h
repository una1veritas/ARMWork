
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
		uint32 rxposition, txposition;
		uint32 rxlength, txlength;
  
  public:
    uint32 status;
	
  public:
    I2CBus(I2CDef * I2Cx, GPIOPin scl = 0, GPIOPin sda = 0) : i2cx(I2Cx), sclpin(scl), sdapin(sda) {
		}
    void begin(uint32 clk = 100000);
	
    void beginTransmission(uint8);
//    void beginRequest(uint8);
		uint32 endTransmission(bool = true);
		uint32 endRequest(void);

		uint32 receiveFrom(uint8_t, uint32);
		uint32 requestFrom(uint8_t addr, uint32 qtty) { return receiveFrom(addr, qtty); } 
	
    virtual size_t write(uint8_t);
		inline virtual size_t write(int v) { return write((uint8_t) v); }
		virtual int available(void);
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

