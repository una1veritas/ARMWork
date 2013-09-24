
#include "armcmx.h"
#include "USARTSerial.h"

#include "main.h"
#include "nfcreader.h"

extern const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
extern const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#if defined(USE_PN532)
PN532 nfcreader(Wire, PN532::I2C_ADDRESS, NFC_IRQ, NFC_RSTPD);
const byte NFCPolling[] = {
  NFC::BAUDTYPE_212K_F,
  NFC::BAUDTYPE_106K_A,
};
#elif defined(USE_SL030)
StrongLink_I2C nfcreader(StrongLink_I2C::SL030_ADDRESS, SL030_TAGDETECT, SL030_WAKEUP);
#endif

uint8 getIDInfo(ISO14443CardInfo & card, IDCardFormat & data, const byte authkey[8]) {
  memset(data.raw, 0x20, sizeof(data.raw));
  switch (card.type) {
    case NFC::CARDTYPE_MIFARE:
      if ( get_MifareBlock(card, data, authkey) == 0 ) {
        Serial.println("Unknown Mifare, not an ID.");
        card.clear();
        return 0;
      }
      break;
      /*
    case NFC::CARDTYPE_FELICA_212K:
      if ( get_FCFBlock(card, data) == 0 ) {
        Serial.println("Unknown FeliCa, not an FCF.");
        card.clear();
        return 0;
      }
      break;
      */
    default:
      Serial.println("Not supported as ID card.");
      card.clear();
      return 0;
	}
	return 1;
}


uint8 putIDInfo(ISO14443CardInfo & card, IDCardFormat & data, const byte authkey[8]) {
  uint8 res;
  uint8 tmp[4];
  
  if ( card.type != NFC::CARDTYPE_MIFARE )
    return 0;
  
  if ( nfcreader.auth_sector(4>>2, authkey) 
  /* && nfcreader.getCommandResponse(&res) && res == 0 */) {
    Serial.println("Authenticated.");
/*
    if ( nfcreader.mifare_ReadAccessConditions(7>>2, tmp) ) {

      acc = ACCESSBITS(tmp);
      Serial.println("Original trailer cond.");
      Serial.printBytes(tmp, 16);
      Serial.println();
      Serial.println(acc, BIN);
      acc &= ~TRAILERBITS(B111);
      acc |=  TRAILERBITS(B011);
      acc &= ~(DATABLOCKBITS(B111, 0) | DATABLOCKBITS(B111, 1) | DATABLOCKBITS(B111, 2));
      acc |=  ( DATABLOCKBITS(B110, 0) | DATABLOCKBITS(B110, 1) | DATABLOCKBITS(B110, 2));
      if ( nfcreader.mifare_WriteAccessConditions(7>>2, acc, factory_a+1, IizukaKey_b+1) ) {
        Serial.println("Succeeded to write trailer block.");
        acc = nfcreader.mifare_ReadAccessConditions(7>>2, tmp);
      } else {
        Serial.println("Trailer block write failed.");
        return 0;
      }

      if ( nfcreader.mifare_WriteBlock(4, data.raw) )
        Serial.println("Write to data block succeeded.");
      else {
        Serial.println("Data block write failed.");
        return 0;
      }

    } else {
      Serial.println("Read trailer block failed.");
      return 0;
    }
    */
    return 1;
  } else {
    Serial.println("Auth block failed.");
  }
  return 0;
}


uint8 get_MifareBlock(ISO14443CardInfo & card, IDCardFormat & data, const uint8_t * key) {
  bool result = true;
  //nfcreader.targetSet(0x10, card.ID, card.IDLength);
         /* Note !!! Once failed to authentication, card's state will be back to the initial state, 
        So the InListPassivTarget or InAutoPoll should be issued again. */

  if ( nfcreader.auth_sector(4>>2, key) /*
  && nfcreader.getCommandResponse(&res) && res == 0 */) {
#ifdef DEBUG
    Serial.println("Auth succeeded.");
#endif
    result = nfcreader.read_block(4, data.raw);
    result = result && nfcreader.read_block(5, data.raw+16);
    result = result && nfcreader.read_block(6, data.raw+32);
    if ( result )
      return 1;
    Serial.println("Read data blocks failed. ");
    return 0;
  } 
  Serial.println("Authentication failure.");
  return 0;
}


