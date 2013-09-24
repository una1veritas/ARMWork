/*
 * NFCCard.h
 *
 *  Created on: 2012/07/24
 *      Author: sin
 */

#ifndef ISO14443_H_
#define ISO14443_H_

#include <Print.h>
//#include <Printable.h>

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
static const byte CARDTYPE_UNKNOWN = 0x80;
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

struct ISO14443CardInfo /*: public Printable */ {

	const static byte NFCID_MAXLENGTH = 10;
	const static byte CARDINFO_LENGTH = NFCID_MAXLENGTH + 2;
	const static byte TYPENAME_MAXLENGTH = 12;

	const static char Type_Names[];

	//
	byte type;
	byte IDLength;
  byte ID[NFCID_MAXLENGTH];
  //
	word atqa;
	byte sak;

public:

  ISO14443CardInfo() {
		clear();
	}

	ISO14443CardInfo(const byte * raw) {
		set(raw);
	}

	ISO14443CardInfo(const byte & card) {
		set(card);
	}

  void set(const byte tp, const byte *data, const byte len);

	void set(const ISO14443CardInfo & card);

	ISO14443CardInfo & operator=(const ISO14443CardInfo & c);
  
  void clear(void);
  
  bool is_empty();
  
  bool operator==(const ISO14443CardInfo & c) const;
  inline bool operator!=(const ISO14443CardInfo & c) { return !(*this ==(c)); }
  
	void set(const byte tp, const byte * raw);

//	virtual size_t printTo(Print & pr) const;
	size_t printOn(Print & pr) const;
  
};




#endif /* NFCCARD_H_ */

