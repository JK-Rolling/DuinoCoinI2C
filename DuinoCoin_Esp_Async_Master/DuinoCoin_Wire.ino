/*
  DuinoCoin_Wire.ino
  created 10 05 2021
  by Luiz H. Cassettari
  
  Modified by JK Rolling
*/

#include <Wire.h>

#ifdef ESP01
  // ESP-01
  #define SDA 0 // GPIO0
  #define SCL 2 // GPIO2
#else
  // ESP8266
  #define SDA 4 // D2
  #define SCL 5 // D1
#endif

#if ESP32
#define SDA 21
#define SCL 22
#endif

#define WIRE_CLOCK 10000
#define WIRE_MAX 32

void wire_setup()
{
  //pinMode(SDA, INPUT_PULLUP);
  //pinMode(SCL, INPUT_PULLUP);
  Wire.begin(SDA, SCL);
  //Wire.setClockStretchLimit(5000000L); //5ms clock stretching timeout
  Wire.setClock(WIRE_CLOCK);
  wire_readAll();
}

void wire_readAll()
{
  for (byte address = 1; address < WIRE_MAX; address++ )
  {
    if (wire_exists(address))
    {
      Serial.print("Address: ");
      Serial.println(address);
      wire_readLine(address);
    }
  }
}

void wire_SendAll(String message)
{
  for (byte address = 1; address < WIRE_MAX; address++ )
  {
    if (wire_exists(address))
    {
      Serial.print("Address: ");
      Serial.println(address);
      Wire_sendln(address, message);
    }
  }
}

boolean wire_exists(byte address)
{
  Wire.beginTransmission(address);
  byte error = Wire.endTransmission();
  return (error == 0);
}

void wire_sendJob(byte address, String lastblockhash, String newblockhash, int difficulty)
{
  String job = lastblockhash + "," + newblockhash + "," + difficulty;
  Wire_sendln(address, job);
}

void Wire_sendln(byte address, String message)
{
  Wire_send(address, message + "\n");
}

void Wire_send(byte address, String message)
{
  for (int i = 0; i < message.length(); i++)
  {
    Wire.beginTransmission(address);
    Wire.write(message.charAt(i));
    Wire.endTransmission();
  }
}

String wire_readLine(int address)
{
  wire_runEvery(0);
  char end = '\n';
  String str = "";
  boolean done = false;
  while (!done)
  {
    Wire.requestFrom(address, 1);
    if (Wire.available())
    {
      char c = Wire.read();
      //Serial.print(c);
      if (c == end)
      {
        break;
        done = true;
      }
      str += c;
    }
    // timeout for I2CS to response
    // diff*100+1 / hashrate
    // 160MHz around 10s
    // 80MHz around 16s
    if (wire_runEvery(10000)) break;
  }
  //str += end;
  return str;
}

boolean wire_runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
