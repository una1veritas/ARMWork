/*
 * NFCCard.h
 *
 *  Created on: 2012/07/24
 *      Author: sin
 */

#ifndef ISO14443_H_
#define ISO14443_H_

//#include <Print.h>
#include <Printable.h>

namespace NFC {
static const byte BAUDTYPE_106K_A = 0x00;
static const byte BAUDTYPE_212K_F = 0x01;
static const byte BAUDTYPE_424K_F = 0x02;
static const byte BAUDTYPE_106K_B = 0x03;
static const byte BAUDTYPE_JEWEL = 0x04;

static const byte CARDTYPE_GENERICPASSIVE_A = 0x00;
static const byte CARDTYPE_GENERICPASSIVE_212K_F = 0x01;
static const byte CARDTYPE_GENERICPASSIVE_424K_F = 0x02;
static const byte CARDTYPE_GENERICPASSIVE_B = 0x03;
static const byte CARDTYPE_MIFARE = 0x10;
static const byte CARDTYPE_FELICA_212K = 0x11;
static const byte CARDTYPE_FELICA_424K = 0x12;
static const byte CARDTYPE_MIFARE_DESFIRE = 0x20;
static const byte CARDTYPE_EMPTY = 0xff;


static const word ATQA_MIFARE_CLASSIC1K = 0x0004; // SAK = 08
static const word ATQA_MIFARE_CLASSIC4K = 0x0002;
static const word ATQA_MIFARE_ULTRALIGHT = 0x0044;
static const word ATQA_MIFARE_DESFIRE = 0x0344;
static const word ATQA_MIFARE_DESFIRE_EV1 = 0x0344;

static const word FELICA_SYSCODE_COMMON = 0x00FE;
static const word FELICA_SYSCODE_SUICA = 0x0003;
static const word FELICA_SERVICE_SUICA = 0x090F;
static const word FELICA_SERVICE_EDY = 0x170F;
static const word FELICA_SERVICE_FCF = 0x1a8b;
};

struct ISO14443 : public Printable {

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
	word atqa;
	byte sak;

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
		case NFC::CARDTYPE_FELICA_212K: // Felica
			memcpy(IDm, card.IDm, IDLength);
			break;
		default: // Mifare
			memcpy(UID, card.UID, IDLength);
			break;
		}
		atqa = 0;
		sak = 0;
	}

	ISO14443 & operator=(const ISO14443 & c) {
		set(c);
		return *this;
	}

	void set(const byte tp, const byte *data, const byte len) {
		type = tp;
		IDLength = len;
		memcpy(ID, data, len);
		atqa = 0;
		sak = 0;
	}

	void set(const byte tp, const byte * raw) {
		type = tp;
//		byte len;
		switch (type) {
		case NFC::CARDTYPE_FELICA_212K:
		case NFC::CARDTYPE_FELICA_424K:
			IDLength = 8;
//			len = raw[1];
			memcpy(IDm, raw + 3, 8);
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
			memcpy(UID, raw + 5, IDLength);
			break;
		}
	}

	virtual size_t printTo(Print & pr) const {
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
  
  
	void clear() {
		type = NFC::CARDTYPE_EMPTY;
		IDLength = 0;
		memset(ID, 0, 8);
		atqa = 0;
		sak = 0;
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


// Translation template data structure

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

#endif /* NFCCARD_H_ */

