#ifndef _FCF_H_
#define _FCF_H_

struct FCF_IDCard {  
  uint8_t division;
  char pid[12];
  uint8_t issue;
  uint8_t gend;
  uint32_t orgid;
  uint32_t doi;
  uint32_t gdthru;
  
  char gender(void) { return (gend == 0 ? '?' : (gend == 1 ? 'M' : 'F')); }
};

#endif
