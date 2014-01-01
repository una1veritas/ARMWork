
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
//#include "I2CBus.h"
#endif
#include "Print.h"
#include "ISO14443.h"

#define DEBUG
#ifdef DEBUG
#if defined ARMCMX
#include "USARTSerial.h"
#endif
#endif

  void ISO14443Card::set(const byte t, const byte *data, const byte len) {
		type = t;
		IDLength = len;
		memcpy(ID, data, len);
//		atqa = 0;
//		sak = 0;
	}

	void ISO14443Card::set(const ISO14443Card & card) {
		type = card.type;
		IDLength = card.IDLength;
		memcpy(ID, card.ID, NFCID_MAXLENGTH);
//		atqa = card.atqa;
//		sak  = card.sak;
	}

	ISO14443Card & ISO14443Card::operator=(const ISO14443Card & c) {
		set(c);
		return *this;
	}
  
  void ISO14443Card::clear(void) {
    IDLength = 0;
    memset(ID, 0, NFCID_MAXLENGTH);
    type = NFC::CARDTYPE_EMPTY;
  }
  
  bool ISO14443Card::is_empty() {
    return type == NFC::CARDTYPE_EMPTY;
  }
  
  
	void ISO14443Card::set(const byte tp, const byte * raw) {
		type = tp;
//		byte len;
		switch (type) {
		case NFC::CARDTYPE_FELICA_212K:
		case NFC::CARDTYPE_FELICA_424K:
			IDLength = 8;
//			len = raw[1];
			memcpy(ID, raw + 3, 8);
//			memcpy(PMm, raw + 11, 8);
//			if (len == 20)
//				memcpy(SysCode, raw + 19, 2);
//      atqa = 0;
 //     sak = 0;
			break;
		case NFC::CARDTYPE_MIFARE:
		default: // Mifare 106k TypeA
  //    atqa = raw[1]<<8 | raw[2];
   //   sak = raw[3];
			IDLength = raw[4];
			memcpy(ID, raw + 5, IDLength);
			break;
		}
	}

bool ISO14443Card::operator==(const ISO14443Card & c) const {
  if ( type == c.type && IDLength == c.IDLength ) {
    return memcmp(ID, c.ID, NFCID_MAXLENGTH) == 0;
  }
  return false;
}

size_t ISO14443Card::printTo(Print & pr) const {
//size_t ISO14443Card::printOn(Print & pr) const {
  int cnt = 0;

  switch(type) {
  case NFC::CARDTYPE_MIFARE:
  case NFC::CARDTYPE_MIFARE_DESFIRE:
    cnt += pr.print("Mifare");
    break;
  case NFC::CARDTYPE_FELICA_212K:
    cnt += pr.print("FeliCa212kb");
    break;
  case NFC::CARDTYPE_FELICA_424K:
    cnt += pr.print("FeliCa424kb");
    break;
  case NFC::CARDTYPE_EMPTY:
    cnt += pr.print("Empty");
    break;
  default:
    cnt += pr.print("Unknown");
    cnt += pr.print((int)type, HEX);
    break;
  }
  cnt += pr.print(":");
  for(int i = 0; i < IDLength; i++) {
    if ( i > 0 ) pr.print('.');
    pr.print(ID[i]>>4, HEX);
    pr.print(ID[i]&0x0f, HEX);
    cnt += 3;
  }
  return cnt;
}


