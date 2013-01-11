/*
 * NFCCard.h
 *
 *  Created on: 2012/07/24
 *      Author: sin
 */

#ifndef ISO14443_H_
#define ISO14443_H_

//#include <Print.h>

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
	static const byte NFCID_size = 8;
	//
	byte type;
	byte IDLength;
	union {
		byte UID[7];
		byte NUID[7];
		byte IDm[8];
		byte ID[NFCID_size];
	};

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
			memcpy(IDm, card.IDm, IDLength);
			break;
		default: // Mifare
			memcpy(UID, card.UID, IDLength);
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
//			len = raw[1];
			memcpy(IDm, raw + 3, 8);
//			memcpy(PMm, raw + 11, 8);
//			if (len == 20)
//				memcpy(SysCode, raw + 19, 2);
			break;
		case Mifare:
		default: // Mifare 106k TypeA
			IDLength = raw[4];
			memcpy(UID, raw + 5, IDLength);
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

#endif /* NFCCARD_H_ */

