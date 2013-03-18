#ifndef _FCF_H_
#define _FCF_H_

#include "armcmx.h"
#include <ctype.h>

#define dtoi(c) ( isdigit(c) ? (c) - '0' : (c) )

struct FCFStruct {
  uint8 division[2];
  uint8 pid[12];
  uint8 reissue;
  uint8 gender;
  uint8 namekana[16];
  const uint8 orgid[8];
  uint8 dofissue[8];
  uint8 goodthru[8];
  const uint8 issuerdata[8];
  
  FCFStruct() : 
    division(""), pid("00000000"), namekana("outis"), orgid(""), issuerdata("") {
    
  }
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
