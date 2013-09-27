
extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
  #include "i2c.h"
	#include "delay.h"
}

#include "I2CBus.h"

// Initialize Class Variables //////////////////////////////////////////////////

I2CBus Wire(&i2c);

//uint8_t I2CBus::transmitting = 0;
//void (*I2CBus::user_onRequest)(void);
//void (*I2CBus::user_onReceive)(int);

// Constructors ////////////////////////////////////////////////////////////////


// Public Methods //////////////////////////////////////////////////////////////

void I2CBus::begin(uint32_t clkHz)
{
	status = I2C_init(i2cx, I2CMASTER);
	status = I2C_begin(i2cx);
}

void I2CBus::beginTransmission(uint8_t address) {
	dstaddress = address;
	txposition = 0;
	txlength = 0;
}

uint32 I2CBus::receiveFrom(uint8_t address, uint32 quantity) {
  // clamp to buffer length
  if(quantity > BUFFER_LENGTH)
    quantity = BUFFER_LENGTH;
	rxposition = 0;
 	if ( I2C_receive(i2cx, address, rxbuffer, quantity) ) {
		return (rxlength = quantity);
	}
	return 0;
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to 
//	perform a repeated start. 
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint32 I2CBus::endTransmission(bool sendStop)
{
	//  int8_t ret = 0;
  // transmit buffer (blocking)
	if ( sendStop ) {
		if ( I2C_write(i2cx, dstaddress, txbuffer, txlength) ) {
			return txlength;
		}
	} else {
		if ( I2C_request(i2cx, dstaddress, txbuffer, txlength) ) {
			return txlength;
		}
	}
	return 0;
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint32 I2CBus::endRequest() {
  return endTransmission(false);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t I2CBus::write(uint8_t data)
{
  // don't bother if buffer is full
	if(txlength >= BUFFER_LENGTH){
//      setWriteError();
//      return 0;
  }
  // put byte in tx buffer
  txbuffer[txposition++] = data;
  txlength = txposition;
	return 1;
}

/*
// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t I2CBus::write(const uint8_t *data, size_t quantity)
{
  if(transmitting){
  // in master transmitter mode
    for(size_t i = 0; i < quantity; ++i){
      write(data[i]);
    }
  }else{
  // in slave send mode
    // reply to master
    twi_transmit(data, quantity);
  }
  return quantity;
}
*/
// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int I2CBus::available(void)
{
  int d = rxlength - rxposition;
  return (d < 0? 0 : d);
}
/*
// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int16 I2CBus::read(void)
{
  int16 value = -1;
  
  // get each successive byte on each call
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }

  return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int16 I2CBus::peek(void)
{
  int16 value = -1;
  
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
  }

  return value;
}
*/

void I2CBus::flush(void)
{
  // XXX: to be implemented.
}

/*
// behind the scenes function that is called when data is received
void I2CBus::onReceiveService(uint8_t* inBytes, int numBytes)
{
  // don't bother if user hasn't registered a callback
  if(!user_onReceive){
    return;
  }
  // don't bother if rx buffer is in use by a master requestFrom() op
  // i know this drops data, but it allows for slight stupidity
  // meaning, they may not have read all the master requestFrom() data yet
  if(rxBufferIndex < rxBufferLength){
    return;
  }
  // copy twi rx buffer into local read buffer
  // this enables new reads to happen in parallel
  for(uint8_t i = 0; i < numBytes; ++i){
    rxBuffer[i] = inBytes[i];    
  }
  // set rx iterator vars
  rxBufferIndex = 0;
  rxBufferLength = numBytes;
  // alert user program
  user_onReceive(numBytes);
}

// behind the scenes function that is called when data is requested
void I2CBus::onRequestService(void)
{
  // don't bother if user hasn't registered a callback
  if(!user_onRequest){
    return;
  }
  // reset tx buffer iterator vars
  // !!! this will kill any pending pre-master sendTo() activity
  txBufferIndex = 0;
  txBufferLength = 0;
  // alert user program
  user_onRequest();
}

// sets function called on slave write
void I2CBus::onReceive( void (*function)(int) )
{
  user_onReceive = function;
}

// sets function called on slave read
void I2CBus::onRequest( void (*function)(void) )
{
  user_onRequest = function;
}
*/

// Preinstantiate Objects //////////////////////////////////////////////////////

//I2CBus Wire1 = I2CBus(I2C1);

