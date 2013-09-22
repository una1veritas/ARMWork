#include <RCS620S.h>
#include <inttypes.h>
#include <string.h>

#define COMMAND_TIMEOUT  400
#define PUSH_TIMEOUT    2100
#define POLLING_INTERVAL 500

#define LED_PIN 13

RCS620S rcs620s;
int waitCardReleased = 0;

void setup()
{
  int ret;

  digitalWrite(LED_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);

  ret = rcs620s.initDevice();
  while (!ret) {}
}

void loop()
{
  int ret;

  // Polling
  digitalWrite(LED_PIN, HIGH);
  rcs620s.timeout = COMMAND_TIMEOUT;
  ret = rcs620s.polling();
  if (!ret) {
    if (waitCardReleased) {
      waitCardReleased = 0;
    }
  } else if (!waitCardReleased) {
    // Push
    uint8_t data[] = {
      0x01, 0x02, 0x2a, 0x00, 0x1d, 0x00, 0x68, 0x74,
      0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x74, 0x77, 0x69,
      0x74, 0x74, 0x65, 0x72, 0x2e, 0x63, 0x6f, 0x6d,
      0x2f, 0x53, 0x44, 0x4b, 0x34, 0x46, 0x65, 0x6c,
      0x69, 0x43, 0x61, 0x46, 0x6f, 0x6c, 0x6c, 0x6f,
      0x77, 0x20, 0x4d, 0x65, 0x21, 0x21, 0xf1, 0xee,
    };
    rcs620s.timeout = PUSH_TIMEOUT;
    ret = rcs620s.push(data, sizeof(data));
    if (ret) {
      waitCardReleased = 1;
    }
  }

  rcs620s.rfOff();
  digitalWrite(LED_PIN, LOW);
  delay(POLLING_INTERVAL);
}
