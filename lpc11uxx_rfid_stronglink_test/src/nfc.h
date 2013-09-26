
void readcard(PN532 & reader, ISO14443 & card);
  
uint8 get_FCFBlock(PN532 & reader, ISO14443 & card);

uint8 get_MifareBlock(PN532 & reader, ISO14443 & card);
