/*
 * NFCCard.h
 *
 *  Created on: 2012/07/24
 *      Author: sin
 */

#ifndef ISO14443_H_
#define ISO14443_H_

#include <string.h>
<<<<<<< HEAD
#include "armcmx.h"
#include <Print.h>
=======

#include "armcmx.h"
#include "Print.h"
>>>>>>> origin/@home

//#include "PN532_I2C.h"
static const byte TypeA = 0x00;
static const byte TypeF = 0x01;
static const byte TypeF_212kb = 0x01;
static const byte TypeF_424kb = 0x02;
static const byte TypeB = 0x03;
static const byte TypeJewel = 0x03;
static const byte Mifare = 0x10;
static const byte FeliCa212kb = 0x11;
static const byte FeliCa424kb = 0x12;
static const byte Type_Empty = 0xff;

static const word FELICA_SYSCODE_COMMON = 0x00FE;
static const word FELICA_SYSCODE_SUICA = 0x0003;
static const word FELICA_SERVICE_SUICA = 0x090F;
static const word FELICA_SERVICE_EDY = 0x170F;
static const word FELICA_SERVICE_FCF = 0x1a8b;


struct ISO14443 {
<<<<<<< HEAD
	static const byte CARDID_MAXSIZE = 8;
	//
	byte type;
	byte IDLength;
  byte ID[CARDID_MAXSIZE];
=======
	static const byte NFCID_MAXSIZE = 8;
	//
	byte type;
	byte IDLength;
	byte ID[NFCID_MAXSIZE];
>>>>>>> origin/@home

  
	ISO14443() {
		clear();
	}

	ISO14443(const byte * raw) {
		set(raw);
	}

	ISO14443(const byte & card) {
		set(card);
	}

	void set(const ISO14443 & card) {
		type = card.type;
		IDLength = card.IDLength;
		switch (type) {
		case FeliCa212kb: // Felica
			memcpy(ID, card.ID, IDLength);
			break;
		default: // Mifare
			memcpy(ID, card.ID, IDLength);
			break;
		}
	}

	ISO14443 & operator=(const ISO14443 & c) {
		set(c);
		return *this;
	}

	void set(const byte tp, const byte *data, const byte len) {
		type = tp;
		IDLength = len;
		memcpy(ID, data, len);
	}

	void set(const byte tp, const byte * raw) {
		//PN532::printHexString(raw, 16);
		type = tp;
//		byte len = 0;
		switch (type) {
		case FeliCa212kb:
		case FeliCa424kb:
			IDLength = 8;
<<<<<<< HEAD
//			len = raw[1];
=======
>>>>>>> origin/@home
			memcpy(ID, raw + 3, 8);
//			memcpy(PMm, raw + 11, 8);
//			if (len == 20)
//				memcpy(SysCode, raw + 19, 2);
			break;
		case Mifare:
		default: // Mifare 106k TypeA
			IDLength = raw[4];
			memcpy(ID, raw + 5, IDLength);
			break;
		}
	}

	size_t printOn(Print & pr) {
		int cnt = 0;
		switch(type) {
		case Mifare:
			cnt += pr.print("Mifare");
			break;
		case FeliCa212kb:
			cnt += pr.print("FeliCa212kb");
			break;
		case FeliCa424kb:
			cnt += pr.print("FeliCa424kb");
			break;
		case Type_Empty:
			cnt += pr.print("Empty");
			break;
		default:
			cnt += pr.print("Unknown");
			break;
		}
		for(int i = 0; i < IDLength; i++) {
			pr.print(' ');
			pr.print(ID[i]>>4, HEX);
			pr.print(ID[i]&0x0f, HEX);
			cnt += 3;
		}
		return cnt;
	}

	void clear() {
		type = Type_Empty;
		IDLength = 0;
		memset(ID, 0, 8);
	}

	boolean isEmpty(void) {
		return type == Type_Empty || IDLength == 0;
	}
	
	boolean operator==(const ISO14443 & c) const {
		if (type == c.type && IDLength == c.IDLength) {
			return memcmp(ID, c.ID, IDLength) == 0;
		}
		return false;
	}

	inline boolean operator!=(const ISO14443 & c) const {
		return !(operator==(c));
	}
};


<<<<<<< HEAD
struct IDCard {
  ISO14443 card;
  union {
      struct FCF {
        uint8 division[2];
        uint8 pid[12];
        uint8 reissue;
        uint8 gender;
        uint8 namekana[16];
        uint8 orgid[8];
        uint8 dofissue[8];
        uint8 goodthru[8];
        uint8 issuerdata[8];
      } fcf;
      struct IIZUKA {
        uint8 division[2];
        uint8 pid[8];
        uint8 reissue;
        uint8 reserved1[5];
        uint8 namesjis[16];
        uint8 dofbirth[7];
        uint8 gender;
        uint8 dofissue[7];
        uint8 reserved2;
      } ktech;
      uint8 raw[64];
  } pdata;
};


=======
union IDData {
  struct {
    uint8 division[2];
    uint8 pid[12];
    uint8 issue;
    uint8 gender;
    uint8 namekana[16];
    uint8 orgid[8];
    uint8 dofissue[8];
    uint8 goodthru[8];
    uint8 issuerdata[8];
  } fcf;
  struct {
    uint8 division[2];
    uint8 pid[8];
    uint8 issue;
    uint8 reserved1[5];
    uint8 namesjis[16];
    uint8 dofbirth[7];
    uint8 gender;
    uint8 dofissue[7];
    uint8 reserved2;
  } iizuka;
  uint8 raw[64];
};

>>>>>>> origin/@home
#endif /* NFCCARD_H_ */

