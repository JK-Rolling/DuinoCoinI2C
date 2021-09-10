/*
  DoinoCoin_ESP_Slave.ino
  JK-Rolling

  ESP8266
  Code concept taken from duino-coin and ricaun
*/

// edit this LED pin to suit your ESP
// for ESP01 GPIO2 is used for I2C_SCL
#define LED_BUILTIN 2

#define BLINK_SHARE_FOUND    1
#define BLINK_SETUP_COMPLETE 2
#define BLINK_CLIENT_CONNECT 3
#define BLINK_RESET_DEVICE   5

#include <Ticker.h>

Ticker lwdTimer;
#define LWD_TIMEOUT   60000

unsigned long lwdCurrentMillis = 0;
unsigned long lwdTimeOutMillis = LWD_TIMEOUT;

void Blink(uint8_t count, uint8_t pin = LED_BUILTIN) {
  uint8_t state = HIGH;

  for (int x = 0; x < (count << 1); ++x) {
    digitalWrite(pin, state ^= HIGH);
    delay(50);
  }
}

void RestartESP(String msg) {
  Serial.println(msg);
  Serial.println("Resetting ESP...");
  Blink(BLINK_RESET_DEVICE);
  #if ESP8266
    ESP.reset();
  #endif
}

// Our new WDT to help prevent freezes
// code concept taken from https://sigmdel.ca/michel/program/esp8266/arduino/watchdogs2_en.html
void ICACHE_RAM_ATTR lwdtcb(void)
{
  if ((millis() - lwdCurrentMillis > LWD_TIMEOUT) || (lwdTimeOutMillis - lwdCurrentMillis != LWD_TIMEOUT))
    RestartESP("Loop WDT Failed!");
}

void lwdtFeed(void) {
  lwdCurrentMillis = millis();
  lwdTimeOutMillis = lwdCurrentMillis + LWD_TIMEOUT;
}

void handleSystemEvents(void) {
  yield();
}

bool max_micros_elapsed(unsigned long current, unsigned long max_elapsed) {
  static unsigned long _start = 0;

  if ((current - _start) > max_elapsed) {
    _start = current;
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  DuinoCoin_setup();

  lwdtFeed();
  lwdTimer.attach_ms(LWD_TIMEOUT, lwdtcb);
  
  pinMode(LED_BUILTIN, OUTPUT);
  Blink(BLINK_SETUP_COMPLETE);
}

void loop() {
  // 1 minute watchdog
  lwdtFeed();
  
  if (DuinoCoin_loop())
  {
    Serial.print(F("Job Done : "));
    Serial.print(DuinoCoin_response());
    Blink(BLINK_SHARE_FOUND);
  }
}
