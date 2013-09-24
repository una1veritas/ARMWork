
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
#include "Print.h"
#include "ISO14443.h"

#define DEBUG
#ifdef DEBUG
#include "USARTSerial.h"
#endif

  void ISO14443CardInfo::set(const byte tp, const byte *data, const byte len) {
		type = tp;
		IDLength = len;
		memcpy(ID, data, len);
		atqa = 0;
		sak = 0;
	}

	void ISO14443CardInfo::set(const ISO14443CardInfo & card) {
		type = card.type;
		IDLength = card.IDLength;
    memcpy(ID, card.ID, NFCID_MAXLENGTH);

		atqa = card.atqa;
		sak  = card.sak;
	}

	ISO14443CardInfo & ISO14443CardInfo::operator=(const ISO14443CardInfo & c) {
		set(c);
		return *this;
	}
  
  void ISO14443CardInfo::clear(void) {
    IDLength = 0;
    memset(ID, 0, NFCID_MAXLENGTH);
    type = NFC::CARDTYPE_EMPTY;
  }
  
  bool ISO14443CardInfo::is_empty() {
    return IDLength == 0;
  }
  
  
	void ISO14443CardInfo::set(const byte tp, const byte * raw) {
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
      atqa = 0;
      sak = 0;
			break;
		case NFC::CARDTYPE_MIFARE:
		default: // Mifare 106k TypeA
      atqa = raw[1]<<8 | raw[2];
      sak = raw[3];
			IDLength = raw[4];
			memcpy(ID, raw + 5, IDLength);
			break;
		}
	}

bool ISO14443CardInfo::operator==(const ISO14443CardInfo & c) const {
  if ( type == c.type && IDLength == c.IDLength ) {
    return memcmp(ID, c.ID, NFCID_MAXLENGTH) == 0;
  }
  return false;
}

//size_t ISO14443CardInfo::printTo(Print & pr) const {
size_t ISO14443CardInfo::printOn(Print & pr) const {
  int cnt = 0;

  switch(type) {
  case NFC::CARDTYPE_MIFARE:
  case NFC::CARDTYPE_MIFARE_DESFIRE:
    cnt += pr.print("Mifare");
    if ( type == NFC::CARDTYPE_MIFARE_DESFIRE ) 
      cnt += pr.print(" DESFire");
    else if ( atqa == NFC::ATQA_MIFARE_ULTRALIGHT )
      cnt += pr.print(" Ultralight");
    else if ( atqa == NFC::ATQA_MIFARE_CLASSIC1K )
      cnt += pr.print(" Classic 1k");
    else if ( atqa == NFC::ATQA_MIFARE_CLASSIC4K )
      cnt += pr.print(" Classic 4k");
    break;
  case NFC::CARDTYPE_FELICA_212K:
    cnt += pr.print("FeliCa 212kb");
    break;
  case NFC::CARDTYPE_FELICA_424K:
    cnt += pr.print("FeliCa 424kb");
    break;
  case NFC::CARDTYPE_EMPTY:
    cnt += pr.print("Type Empty");
    break;
  default:
    cnt += pr.print("Unknown (");
    cnt += pr.print((int)type, DEC);
    cnt += pr.print(")");
    break;
  }
  pr.print(' ');
  for(int i = 0; i < IDLength; i++) {
    if ( i > 0 ) pr.print('-');
    pr.print(ID[i]>>4, HEX);
    pr.print(ID[i]&0x0f, HEX);
    cnt += 3;
  }
  return cnt;
}


