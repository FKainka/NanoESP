/*
  Easy Wifi Scanner
*/
#include <NanoESP.h>
#include <SoftwareSerial.h>

#define DEBUG true

NanoESP nanoesp = NanoESP();

void setup()
{
  Serial.begin(19200);
  nanoesp.init();

  if (nanoesp.configWifi(ACCESSPOINT, "NanoESP", ""))  digitalWrite(13, HIGH);
}

void loop() // run over and over
{

}

