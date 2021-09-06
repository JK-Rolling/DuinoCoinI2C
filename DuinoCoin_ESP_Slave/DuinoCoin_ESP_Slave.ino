/*
  DoinoCoin_ArduinoSlave.ino
  created 10 05 2021
  by Luiz H. Cassettari
  
  Modified by JK-Rolling
*/
#include <Ticker.h>

// Loop WDT... please don't feed me...
// See lwdtcb() and lwdtFeed() below
Ticker lwdTimer;
#define LWD_TIMEOUT   60000

unsigned long lwdCurrentMillis = 0;
unsigned long lwdTimeOutMillis = LWD_TIMEOUT;

void RestartESP(String msg) {
  Serial.println(msg);
  Serial.println("Resetting ESP...");
  Blink();
  ESP.reset();
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


void setup() {
  Serial.begin(115200);
  DuinoCoin_setup();

  lwdtFeed();
  lwdTimer.attach_ms(LWD_TIMEOUT, lwdtcb);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // 1 minute watchdog
  lwdtFeed();
  
  if (DuinoCoin_loop())
  {
    Serial.print(F("Job Done : "));
    Serial.print(DuinoCoin_response());
    Blink();
  }
}

void Blink()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
}
