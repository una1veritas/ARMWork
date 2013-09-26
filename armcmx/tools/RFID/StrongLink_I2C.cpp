/*
 * StrongLink I2C based Mifare Reader
 * SL030 (3v3), SL018 (5v)
 */
#if defined (ARDUINO)
#include <avr/pgmspace.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif
#include <Wire.h>
#elif defined (ARMCMX)
#include "armcmx.h"
#include "I2CBus.h"
#endif
#include "StrongLink_I2C.h"

//#define DEBUG
#ifdef DEBUG
#include "USARTSerial.h"
#endif

PROGMEM const prog_char StrongLink_I2C::Transport_key_A[7] = { 0xAA, 0xA0, 0xA1,
		0xA2, 0xA3, 0xA4, 0xA5 };
PROGMEM const prog_char StrongLink_I2C::Transport_key_B[7] = { 0xBB, 0xB0, 0xB1,
		0xB2, 0xB3, 0xB4, 0xB5 };
PROGMEM const prog_char StrongLink_I2C::Philips_key[7] = { 0xAA, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF };

StrongLink_I2C::StrongLink_I2C(byte addr, byte tag_pin, byte wakeup_pin) {
	address = addr;
	pin_tag = tag_pin;
	pin_input = wakeup_pin;
}

void StrongLink_I2C::init() {
	packet.status = 0;
	if (pin_tag != PIN_NOT_DEFINED)
		pinMode(pin_tag, INPUT);
  if ( pin_input != PIN_NOT_DEFINED ) {
    pinMode(pin_input, OUTPUT);
    digitalWrite(pin_input, HIGH);
    delay(100);
    digitalWrite(pin_input, LOW);
    delay(100);
    digitalWrite(pin_input, HIGH);
  }
	card.clear();
//  event_millis = millis();
//  event_wait = 20;
}

void StrongLink_I2C::send_command(byte cmd, byte len) {
	packet.length = len + 1;
	packet.command = cmd;
	transmit();
}

void StrongLink_I2C::transmit() {
	// wait until at least 20ms passed since last I2C transmission
//	while(t > millis());
//	t = millis() + 20;

// transmit packet with checksum
	Wire.beginTransmission(address);
	twiwrite(packet.length);
	twiwrite(packet.command);
	packet.length--;
	for (int i = 0; i < packet.length; i++) {
		twiwrite(packet.data[i]);
	}
	Wire.endTransmission();
}

byte StrongLink_I2C::receive(byte len) {
	// wait until at least 20ms passed since last I2C transmission
	//	while(t > millis());
	//	t = millis() + 20;
	int i = 0;
	byte * p = &packet.length;
	len += 3;
	// read response
	do {
		Wire.requestFrom(address, len);
    
		delay(5);
		i++;
	} while (i < 7 && Wire.available() == 0);
#ifdef DEBUG
  Serial << ":: receive() " << i << " times " << nl;
  Serial.flush();
#endif
	if (Wire.available() > 0) {
		// get length	of packet
		*p++ = twiread(); // pakcet.length
		*p++ = twiread();
		packet.length--;
		*p++ = twiread();
		packet.length--;
		// get data
		for (i = 0; i < packet.length; i++) {
			*p++ = twiread();
		}
		// return with length of response
		return packet.length;
	}
	return 0;
}

boolean StrongLink_I2C::select() {
#ifdef DEBUG
  Serial << "StrongLink::select(); ";
  Serial.flush();
#endif
	send_command(CMD_SELECT);
	delay(20);
	receive(8);
	byte i;
	if (packet.status != Operation_succeed) {
    card.clear();
#ifdef DEBUG
  Serial << " failed; ";
  Serial.flush();
#endif
		return false;
	}
#ifdef DEBUG
  Serial << " received; ";
  Serial.flush();
#endif
	card.IDLength = packet.length - 1;
	for (i = 0; i < card.IDLength; i++) {
		card.ID[i] = packet.data[i];
  }
  // type is the next byte to the last of ID
  switch(packet.data[card.IDLength]) {
    case 0x01:
      card.type = NFC::CARDTYPE_MIFARE;
      card.atqa = NFC::ATQA_MIFARE_CLASSIC1K;
      break;
    case 0x03:
      card.type = NFC::CARDTYPE_MIFARE;
      card.atqa = NFC::ATQA_MIFARE_ULTRALIGHT;
      break;
    case 0x04:
      card.type = NFC::CARDTYPE_MIFARE;
      card.atqa = NFC::ATQA_MIFARE_CLASSIC4K;
      break;
    case 0x06:
      card.type = NFC::CARDTYPE_MIFARE_DESFIRE;
      card.atqa = NFC::ATQA_MIFARE_DESFIRE;
      break;
    case 0x0a:
      card.type = NFC::CARDTYPE_MIFARE;
      card.atqa = 0;
      break;
    default:
      card.type = NFC::CARDTYPE_UNKNOWN;
      card.atqa = 0;
      break;
  }
	for (; i < ISO14443Card::NFCID_MAXLENGTH; i++)
		card.ID[i] = 0;
#ifdef DEBUG
  Serial << " type set. " << nl;
  Serial.flush();
#endif
	return packet.status == Operation_succeed;
}

boolean StrongLink_I2C::set_led(const byte onoff) {
	packet.data[0] = (byte) (onoff != 0);
	send_command(CMD_SET_LED, 1);
	delay(20);
	receive(0);
	return packet.status == Operation_succeed;
}

boolean StrongLink_I2C::auth_sector(const byte sec, const byte * typekey) {
	byte *p = packet.data;
#ifdef DEBUG
  Serial << " ::auth_sector( ) ";
  Serial.flush();
#endif
	*p++ = sec;
	memcpy(p, typekey, 7);
	send_command(CMD_LOGIN, 8);
	delay(20);
	receive(0);
#ifdef DEBUG
  Serial << "; received. " << nl;
  Serial.flush();
#endif
	return packet.status == Login_succeed;
}

boolean StrongLink_I2C::write_master_key(const byte sec, const byte * typekey) {
	byte *p = packet.data;
	*p++ = sec;
	if (*typekey != 0xAA) {
		packet.status = 0x0C; // as load-key-failure
	} else {
		memcpy(p, typekey + 1, 6);
		send_command(CMD_WRITE_KEY, 7);
		delay(20);
		receive(6);
	}
	return packet.status == Operation_succeed;
}

boolean StrongLink_I2C::read_block(byte baddr, byte blk[]) {
#ifdef DEBUG
  Serial << " ::read_block( ) ";
  Serial.flush();
#endif
	packet.data[0] = baddr;
	send_command(CMD_READ16, 1);
#ifdef DEBUG
  Serial << "; ";
  Serial.flush();
#endif
	delay(20);
	receive(16);
#ifdef DEBUG
  Serial << "; received ";
  Serial.flush();
#endif
	memcpy((void*) blk, (void*) packet.data, 16);
#ifdef DEBUG
  Serial << "; copied. " << nl;
  Serial.flush();
#endif
	return packet.status == Operation_succeed;
}

boolean StrongLink_I2C::write_block(byte baddr, const byte blk[]) {
	packet.data[0] = baddr;
	// the first byte of data is block number, after that 16 bytes follow.
	memcpy((void*) (packet.data + 1), (void*) blk, 16);
	send_command(CMD_WRITE16, 17);
	delay(30);
	receive(16);
	return packet.status == Operation_succeed;
}

boolean StrongLink_I2C::read_value(byte baddr, long & lval) {
	packet.data[0] = baddr;
	send_command(CMD_READ_VALUE, 1);
	delay(20);
	receive(4);
	lval = *((long*) packet.data);
	return packet.status == Operation_succeed;
}

boolean StrongLink_I2C::manipulate_value(byte cmd, byte baddr, long & lval) {
	packet.data[0] = baddr;
	memcpy((void*) (packet.data + 1), (void*) &lval, 4);
	send_command(cmd, 5);
	delay(20);
	receive(4);
	lval = *((long*) packet.data);
	return packet.status == Operation_succeed;
}

boolean StrongLink_I2C::copy_value(byte addra, byte addrb) {
	packet.data[0] = addra;
	packet.data[1] = addrb;
	send_command(CMD_COPY_VALUE, 2);
	delay(20);
	receive(4);
	return packet.status == Operation_succeed;
}

/*
 boolean bitAt(byte a[], int pos) {
 return a[pos>>3]>>(pos & 0b111) & 1;
 }
 */

boolean StrongLink_I2C::read_access_condition(uint8_t blk, byte mode[]) {
	byte trailer_bytes[16];
	byte c1, c2, c3;
	blk &= ~(0x03); //0b11);
	blk += 3;
	if (!read_block(blk, trailer_bytes))
		return false;
	c1 = trailer_bytes[7] >> 4;
	c2 = trailer_bytes[8] & 0x0f;
	c3 = trailer_bytes[8] >> 4;
	if (( (~c1 & 0x0f) == (trailer_bytes[6] & 0x0f))
			and ( (~c2 & 0x0f) == (trailer_bytes[6] >> 4 & 0x0f))
			and ( (~c3 & 0x0f) == (trailer_bytes[7] & 0x0f)))
		return false;
// 
	for (int i = 0; i < 4; i++) {
		mode[i] = (c1 & 1) << 2;
		mode[i] |= (c2 & 1) << 1;
		mode[i] |= (c3 & 1);
		c1 >>= 1;
		c2 >>= 1;
		c3 >>= 1;
	}
	return true;
}

boolean StrongLink_I2C::write_trailer(const byte blk, const byte typekey_a[7],
		const byte mode[4], const byte typekey_b[7]) {
	byte trailer_bytes[16];
	byte c1 = 0, c2 = 0, c3 = 0;
	byte myMode;
	// setup key A and B
	if (*typekey_a != 0xaa) {
		packet.status = 0x0C;
		return false;
	}
	if (!read_block((blk & ~0x03 /*(0b11)*/ ) | 0x03 /*0b11*/, trailer_bytes))
		return false;

	memcpy(trailer_bytes, typekey_a + 1, 6);
	memcpy(trailer_bytes + 6 + 4, typekey_b + 1, 6);

	for (int i = 0; i < 4; i++) {
		myMode = mode[i];
		//
		c3 |= (myMode & 1) << i;
		myMode >>= 1;
		c2 |= (myMode & 1) << i;
		myMode >>= 1;
		c1 |= (myMode & 1) << i;
	}
	/*
	 Serial.print(c1, BIN);
	 Serial.print(' ');
	 Serial.print(c2, BIN);
	 Serial.print(' ');
	 Serial.print(c3, BIN);
	 Serial.println();
	 */

	trailer_bytes[6] = (~c1 & 0x0f) | ((~c2 << 4) & 0xf0);
	trailer_bytes[7] = (~c3 & 0x0f) | (c1 << 4 & 0xf0);
	trailer_bytes[8] = (c2 & 0x0f) | (c3 << 4 & 0xf0);

	write_block((blk & ~0x03 /*(0b11)*/) | 0x03 /*0b11*/, trailer_bytes);
	/*
	 for (int i = 0; i < 16; i++) {
	 Serial.print(trailer_bytes[i]>>4 & 0x0f, HEX);
	 Serial.print(trailer_bytes[i] & 0x0f, HEX);
	 Serial.print(' ');
	 }
	 Serial.println();
	 */

	return packet.status == Operation_succeed;
}

boolean StrongLink_I2C::get_firmware_version(char buf[]) {
	send_command(CMD_GET_FIRMWARE_VERSION);
	delay(20);
	receive(16);
	memcpy((void*) buf, (void*) packet.data, 16);
	return packet.status == Operation_succeed;
}

byte StrongLink_I2C::firmware_version() {
	char tmp[16];
	if ( get_firmware_version(tmp) ) {
		return (tmp[6] - 0x30)*10 + tmp[8]-0x30;
	}
	return 0x00; // not available
}

