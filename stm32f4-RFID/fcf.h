#ifndef _FCF_H_
#define _FCF_H_

#include "armcmx.h"
#include <ctype.h>
#include "RFID/ISO14443.h"

#define dtoi(c) ( isdigit(c) ? (c) - '0' : (c) )

struct IDCardStruct {
  union CardData {
    struct FCFStruct {
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
    struct KTechIDStruct {
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
    uint8 rawdata[64];
  } card;
  uint8 cardtype;
  uint64 CID;
  uint64 MID;
  
  IDCardStruct() : cardtype(Type_Empty) {}
  void clear() { cardtype = Type_Empty; }
  boolean isEmpty(void) { return cardtype != Type_Empty; }
  
};

  static uint32 asctoDate(char asc[8]) {
    return dtoi(asc[0])<<28 | dtoi(asc[1])<<24 | dtoi(asc[2])<< 20 | dtoi(asc[3])<< 16 
          | dtoi(asc[4])<< 12 | dtoi(asc[5])<<8 | dtoi(asc[6])<<4 | dtoi(asc[7]);
  }
  
  static uint32 asctoBCD(char asc[8], uint8 maxlen) { // returns bcd
    uint32 res = 0;
    for(int i = 0; i < maxlen; i++) {
      if ( asc[i] == ' ' ) break;
      res <<= 4;
      res |= dtoi(asc[i]) & 0x0f;
    }
    return res;
  }


#endif
