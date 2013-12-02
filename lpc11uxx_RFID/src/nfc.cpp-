#include <ctype.h>

#include "ISO14443.h"
#include "PN532_I2C.h"
#include "nfc.h"

void readcard(PN532 & reader, ISO14443 & card) {
	if ( card.type == 0x11 ) {
		Serial << "FeliCa %02x" << (uint8)card.IDm[0];
		for(int i = 1; i < 8; i++) {
			printf("-%02x", (uint8)card.IDm[i]);
		}
    printf("\n");
    if ( get_FCFBlock(reader, card) == 0 ) {
      card.clear();
      return;
    }
	} else if ( card.type == 0x10 ) {
		printf("Mifare %02x", card.UID[0]);
		for(int i = 1; i < card.IDLength; i++) {
			printf("-%02x", card.UID[i]);
		}
    printf("\n");
    if ( get_MifareBlock(reader, card) == 0 ) {
      card.clear();
      return;
    }
	}
	return;
}
  
uint8 get_FCFBlock(PN532 & reader, ISO14443 & card) {
  word syscode = 0x00FE;
  int len;
  byte c;

  // Polling command, with system code request.
  len = reader.felica_Polling(card.datablock.raw, syscode);
  if ( len == 0 ) {
    printf("failed polling w/ system code 0x00fe.\n");
    return 0;
  }
  // low-byte first service code.
  // Suica, Nimoca, etc. 0x090f system 0x0300
  // Edy service 0x170f (0x1317), system 0x00FE // 8280
  // FCF 1a8b
  printf("Request Service code: ");
  word servcode = 0x1a8b;
  word scver = reader.felica_RequestService(servcode);
  if ( scver == 0xffff ) 
    return 0;
  printf("%04x ver %04x.\n", servcode, scver);  
  word blist[] = { 0, 1, 2, 3};
  c = reader.felica_ReadBlocksWithoutEncryption(card.datablock.raw, servcode, (byte) 4, blist);
  if ( c == 0 ) {
    printf("\nfailed reading FCF blocks. \n");
    return 0;
  }
  for(int blk = 0; blk < 4; blk++) {
    printf("%02d: ", blk);
    for(int i = 0; i < 16; i++) {
      printf("%02x ",card.datablock.raw[i+blk*16]);
    }
    printf("\n");
    printf("  : ");
    for(int i = 0; i < 16; i++) {
      if ( isprint(card.datablock.raw[i]) ) {
        printf(" %c ", (char) card.datablock.raw[i+blk*16]);
      } else {
        printf("   ");
      }
    }
    printf("\n");
  }
//  printf("\n--- End of FCF reading ---\n\n");
  return 1;
}


uint8 get_MifareBlock(PN532 & reader, ISO14443 & card) {
  uint8 res;
  reader.targetSet(0x10, card.UID, card.IDLength);
  if ( reader.mifare_AuthenticateBlock(4, IizukaKey_b) 
     && reader.getCommandResponse(&res) && res == 0) {
    reader.mifare_ReadDataBlock(4, card.datablock.raw);
    reader.mifare_ReadDataBlock(5, card.datablock.raw+16);
    reader.mifare_ReadDataBlock(6, card.datablock.raw+32);
    idcard.type = Mifare;
    for(int blk = 0; blk < 3; blk++) {
      printf("%02d: ", blk);
      for(int i = 0; i < 16; i++) {
        printf("%02x ",card.datablock.raw[i+blk*16]);
      }
      printf("\n");
      printf("  : ");
      for(int i = 0; i < 16; i++) {
        if ( isprint(card.datablock.raw[i+blk*16]) ) {
          printf(" %c ", (char) card.datablock.raw[i+blk*16]);
        } else {
          printf("   ");
        }
      }
      printf("\n");
    }

  } 
  else {
    printf("Mifare sector 1 Authentication failed.\n");
    return 0;
  }
  return 1;
}
