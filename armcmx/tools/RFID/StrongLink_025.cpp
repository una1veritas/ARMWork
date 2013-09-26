#include <avr/pgmspace.h>

#include <WProgram.h>
#include "SL025.h"

PROGMEM const prog_char SL025::typeNames[] = 
  "\0Mifare 1k\0\0Mifare UltraLight\0Mifare 4k\0\0Mifare DesFire\0\0\0\0Other Unknown type";

PROGMEM const prog_char SL025::Transport_key_A[7] = {
  0xAA, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5
};
PROGMEM const prog_char SL025::Transport_key_B[7] = {
  0xBB, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5
};
PROGMEM const prog_char SL025::Philips_key[7] = {
  0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};


SL025::SL025(byte rx, byte tx, byte tag, byte wake) : 
rwport(rx, tx){
	pin_tagdetect = tag;
	pin_wakeinput = wake;
}

void SL025::begin() {
	if ( pin_tagdetect != 0xff )
		pinMode(pin_tagdetect, INPUT);
	if ( pin_wakeinput != 0xff )
		pinMode(pin_wakeinput, INPUT);
	rwport.begin(57600); //(115200);
	setKey((const byte*)Philips_key, true);
	sector = 0xff;
}

boolean SL025::detectTag() {
  if ( pin_tagdetect == 0xff ) 
    return true;
  return (digitalRead(pin_tagdetect) == LOW);
}

void SL025::send(byte cmd, byte * data, int n) {
  byte csum = 0;

  csum ^= 0xBA;
  csum ^= (byte)(1+n+1);
  csum ^= cmd;
  rwport.print((byte)0xBA);
  rwport.print((byte)(1+n+1)); // command + data + checksum
  rwport.print(cmd);
  for ( ; n > 0; n--) {
    rwport.print(*data);
    csum ^= *data++;
  }
  rwport.print(csum);
}

byte SL025::receive(byte * buf) {
  byte n, length;
  byte c;

  checksum = 0;
  while ( ! (rwport.available() > 0) );
  c = rwport.read();
  checksum ^= c;
  if ( c != 0xBD ) {
    status = 0xbd;
    return 0;
  }
  c = rwport.read();
  checksum ^= c;
  length = c;
  c = rwport.read();
  checksum ^= c;
  command = c;
  c = rwport.read();
  checksum ^= c;
  status = c;
  length -= 2;
  n = length;
  while ( n > 0 && rwport.available() ) {
    c = rwport.read();
    checksum ^= c;
    *buf++ = c;
    n--;
    //    delayMicroseconds(72);
  }

  // the last byte is checksum and so the result nust be zero.
  if ( checksum )
    status = 0xff;
  return length-1;
}

void SL025::showStatus() {
  Serial.print("command: ");
  Serial.print(command, HEX);
  Serial.print(", status: ");
  Serial.print(status, HEX);
  Serial.print(", checksum: ");
  Serial.print(checksum, HEX);
  Serial.print(", login: ");
  Serial.println(sector, HEX);
}

boolean SL025::select_card() {
	send(0x01, (byte*) 0, 0);
	uid[0] = receive(uid+1);
	type = uid[uid[0]];
  uid[0]--;
	sector = 0xff;
	return status == Operation_succeed;
}

boolean SL025::login_sector(byte sec, byte key[7]) {
  byte n;
  buffer[0] = sec;
  memcpy(buffer+1, key, 7);
  if ( buffer[1] != 0xBB ) 
    buffer[1] = 0xAA;
  send(0x02, (byte*) buffer, 8);
  receive(buffer);
  if ( status == Login_succeed ) {
    sector = sec;
  } 
  else {
    sector = 0xff;
  }
  return status == Login_succeed;
}

boolean SL025::login_sector(byte sec) {
	if ( sector == 0xff || sector != sec ) {
		login_sector(sec, accessKey);
		return status == Login_succeed;
	}
	return true;
}

void SL025::setKey(const byte key[], boolean flash) {
  if ( flash ) 
    memcpy_P((void*) accessKey, (void*) key, 7);
  else
    memcpy((void*) accessKey, (void*) key, 7);
}

byte * SL025::key() {
  return accessKey;
}

byte SL025::read_block(byte blk, byte * data) {
  data[0] = blk;
  send(0x03, data, 1);
  receive(data);
  if ( status != Operation_succeed ) {
    sector = 0xff;
  }
  return status;
}

void SL025::write_block(byte blk, byte * data) {
  buffer[0] = blk;
  memcpy((void*)(buffer+1), (void*) data, 16);
  send(0x04, buffer, 17);
  receive(data);
  if ( status != Operation_succeed ) {
    sector = 0xff;
  }  
}

byte SL025::initialize_value(byte blk, long & value) {
  buffer[0] = blk;
  memcpy((void*)(buffer+1), (void*) &value, 4);
  send(0x06, buffer, 5);
  receive((byte*) &value);
  if ( status != Operation_succeed ) {
    sector = 0xff;
  }
  return status;
}

byte SL025::read_value(byte blk, long & value) {
  buffer[0] = blk;
  send(0x05, buffer, 1);
  receive((byte*) &value);
  if ( status != Operation_succeed ) {
    sector = 0xff;
  }
  return status;
}

byte SL025::decrement_value(byte blk, long & value) {
  buffer[0] = blk;
  memcpy((void*)(buffer+1), (void*) &value, 4);
  send(0x09, buffer, 5);
  receive((byte*) &value);
  if ( status != Operation_succeed ) {
    sector = 0xff;
  }
  return status;
}

byte SL025::increment_value(byte blk, long & value) {
  buffer[0] = blk;
  memcpy((void*)(buffer+1), (void*) &value, 4);
  send(0x08, buffer, 5);
  receive((byte*)&value);
  if ( status != Operation_succeed ) {
    sector = 0xff;
  }
  return status;
}

boolean bitAt(byte a[], int pos) {
  return a[pos>>3]>>(pos & 0b111) & 1;
}

word SL025::getAccessCondition() {
  int c, x;
  word q = 0;

  read_block((sector<<2)+3, buffer);
  for ( x = 0; x < 4; x++ ) {
    q <<= 1;
    for ( c = 0; c < 3; c++) {
      q <<= 1;
      if ( bitAt(buffer+6, 12+x+(c<<2)) ) {
        q |= 1;
      }
    }
  }
  return q;
}

/*
byte SL025::uid_size() {
  switch (type) {
  case 0x01: // Mifare 1k
  case 0x04: //Mifare 4k
    return 4;
  case 0x03: // Mifare UltraLight
  case 0x06: // Mifare DesFire
    return 7;
  case 0x0A: // Other
  default:
    break;
  }
  return 0xff;
}
*/

byte * SL025::getCardTypeName(byte * buf) {
  int i, p;
  for ( p = 0, i = 0; i < type; p++) {
    if ( pgm_read_byte(typeNames + p) == 0 )
      i++;
  }
  strcpy_P((char*)buf, typeNames+p);
  return buf;
}


