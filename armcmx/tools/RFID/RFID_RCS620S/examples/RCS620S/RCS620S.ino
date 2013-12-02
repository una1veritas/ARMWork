//#include <LiquidCrystal.h>
#include <RCS620S.h>

#define COMMAND_TIMEOUT 400
#define POLLING_INTERVAL 500
#define LED_PIN 13

//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int waitCardReleased = 0;

// define my characters
byte turnA[8] = { B00000, B10001, B11111, B10001, B10001, B01010, B00100, };
byte de[8] = { B01111, B00101, B00101, B01001, B10001, B11111, B10001, };
byte o[8] = { B01110, B01010, B01110, };
byte prime[8] = { B00010, B00100, B01000, };
byte omega[8] = { B00000, B00000, B01010, B10001, B10101, B10101, B01010, };
byte backSlash[8] = { B00000, B10000, B01000, B00100, B00010, B00001, };
byte bar[8] = { B00000, B11111, };

void setup() {
  Serial1.begin(115200);
  lcd.begin(16, 2);
 
  // regist my characters
  lcd.createChar(1, turnA);
  lcd.createChar(2, de);
  lcd.createChar(3, o);
  lcd.createChar(4, prime);
  lcd.createChar(5, omega);
  lcd.createChar(6, backSlash);
  lcd.createChar(7, bar);

  char str[17];
  int ret;
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  getKaomoji(str, 0);
  lcd.setCursor(2, 0);
  lcd.print(str);  // カオ (・v・) モジ
  lcd.setCursor(3, 1);
  lcd.print("SF CHECKER");
  
  delay(1500);      // つ旦
  
  lcd.clear();
  ret = rcs620s.reset();
  ret = rcs620s.initDevice();
  if(!ret) {
    lcd.print("RC-S620/S");
    lcd.setCursor(0, 1);
    lcd.print("    init error!!");
    while(1) {}
  }
  
  lcd.print("Touch your");
  lcd.setCursor(0, 1);
  lcd.print("       IC Card!!");
}

void loop() {
  int ret;
  
  digitalWrite(LED_PIN, HIGH);
  rcs620s.timeout = COMMAND_TIMEOUT;
  ret = rcs620s.polling(0x0003);
  if (!ret) {
    if (waitCardReleased) {
      waitCardReleased = 0;
      lcd.clear();
      lcd.print("Touch your");
      lcd.setCursor(0, 1);
      lcd.print("       IC Card!!");
    }
  } else if (!waitCardReleased) {
    waitCardReleased = 1;

    // Request Service
    ret = requestService(0x090F);
    if(ret) {
      uint16_t balance;
      
      // Read w/o Encryption
      ret = readWithoutEncryption(0x090F, 0, &balance);
      
      if(ret) {
        // 読み取れた
        char kaomoji[16];
        
        if(balance == 0) getKaomoji(kaomoji, 6);           // オワタ
        else if(balance < 750) getKaomoji(kaomoji, 3);     // ゴルァ
        else if(balance < 1500) getKaomoji(kaomoji, 4);    // ショボーン
        else if(balance < 3000) getKaomoji(kaomoji, 5);    // シャキーン
        else if(balance < 5000) getKaomoji(kaomoji, 2);    // ゲッツ
        else if(balance < 10000) getKaomoji(kaomoji, 7);   // キター
        else getKaomoji(kaomoji, 8);                       // エッヘン
        
        lcd.clear();
        lcd.print("\\");
        lcd.print(balance);
        lcd.setCursor(9, 1);
        lcd.print(kaomoji);
      }
    }
    
    
  }
  
  rcs620s.rfOff();
  digitalWrite(LED_PIN, LOW);
  delay(POLLING_INTERVAL);
}

// request service
int requestService(uint16_t serviceCode) {
  int ret;
  uint8_t buf[NewRCS620S_MAX_CARD_RESPONSE_LEN];
  uint8_t responseLen = 0;
  
  buf[0] = 0x02;
  memcpy(buf + 1, rcs620s.idm, 8);
  buf[9] = 0x01;
  buf[10] = (uint8_t)((serviceCode >> 0) & 0xff);
  buf[11] = (uint8_t)((serviceCode >> 8) & 0xff);

  ret = rcs620s.cardCommand(buf, 12, buf, &responseLen);
  
  if (!ret || (responseLen != 12) || (buf[0] != 0x03) ||
      (memcmp(buf + 1, rcs620s.idm, 8) != 0) || ((buf[10] == 0xff) && (buf[11] == 0xff))) {
    return 0;
  }

  return 1;
}

// Read Without Encryption
int readWithoutEncryption(uint16_t serviceCode, uint8_t blockNumber, uint16_t* balance) {
  int ret;
  uint8_t buf[NewRCS620S_MAX_CARD_RESPONSE_LEN];
  uint8_t responseLen = 0;
  
  buf[0] = 0x06;
  memcpy(buf + 1, rcs620s.idm, 8);
  buf[9] = 0x01;      // サービス数
  buf[10] = (uint8_t)((serviceCode >> 0) & 0xff);
  buf[11] = (uint8_t)((serviceCode >> 8) & 0xff);
  buf[12] = 0x01;     // ブロック数
  buf[13] = 0x80;
  buf[14] = blockNumber;

  ret = rcs620s.cardCommand(buf, 15, buf, &responseLen);

  if (!ret || (responseLen != 28) || (buf[0] != 0x07) ||
      (memcmp(buf + 1, rcs620s.idm, 8) != 0)) {
    return 0;
  }

  *balance = buf[23];
  *balance = (*balance << 8) + buf[22];

  return 1;
}

// get Kaomoji
void getKaomoji(char str[], int pattern) {
  switch(pattern) {
    case 0:        // カオ (・v・) モジ : LEN = 11
      str[0] = 0xB6;
      str[1] = 0xB5;
      str[2] = str[8] = 0x20;
      str[3] = 0x28;
      str[4] = str[6] = 0xA5;
      str[5] = 0x76;
      str[7] = 0x29;
      str[9] = 0xD3;
      str[10] = 0xBC;
      str[11] = 0xDE;
      str[12] = 0x00;
      break;
    case 1:      // (・v・)
      str[0] = 0x28;
      str[1] = str[3] = 0xA5;
      str[2] = 0x76;
      str[4] = 0x29;
      str[5] = 0x00;
      break;
    case 2:      // (σ・∀・)σ
      str[0] = 0x28;
      str[1] = str[6] = 0xE5;
      str[2] = str[4] = 0xA5;
      str[3] = 0x01;
      str[5] = 0x29;
      str[7] = 0x00;
      break;
    case 3:      // (ﾟДﾟ)
      str[0] = 0x28;
      str[1] = str[3] = 0x03;
      str[2] = 0x02;
      str[4] = 0x29;
      str[5] = 0x00;
      break;
    case 4:     // (´・ω・｀)
      str[0] = 0x28;
      str[1] = 0x04;
      str[2] = str[4] = 0xA5;
      str[3] = 0x05;
      str[5] = 0x60;
      str[6] = 0x29;
      str[7] = 0x00;
      break;
    case 5:    // (｀・ω・´)
      str[0] = 0x28;
      str[1] = 0x60;
      str[2] = str[4] = 0xA5;
      str[3] = 0x05;
      str[5] = 0x04;
      str[6] = 0x29;
      str[7] = 0x00;
      break;
    case 6:    // ＼(^o^)／
      str[0] = 0x06;
      str[1] = 0x28;
      str[2] = str[4] = 0x5E;
      str[3] = 0x6F;
      str[5] = 0x29;
      str[6] = 0x2F;
      str[7] = 0x00;
      break;
    case 7:     // (ﾟ∀ﾟ)
      str[0] = 0x28;
      str[1] = str[3] = 0x03;
      str[2] = 0x01;
      str[4] = 0x29;
      str[5] = 0x00;
      break;
    default:    // (￣＾￣)
      str[0] = 0x28;
      str[1] = str[3] = 0x07;
      str[2] = 0x5E;
      str[4] = 0x29;
      str[5] = 0x00;
      break;
  }
}
