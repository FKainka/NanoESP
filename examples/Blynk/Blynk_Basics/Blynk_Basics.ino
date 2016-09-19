/*
 * NanoESP and Blynk.cc
 * You'll need this library: https://github.com/blynkkk/blynk-library/releases/tag/v0.3.4
 * 
*/

char ssid[] = "YourSSID";
char password[] = "YourPassword";
char token[] = "YourToken";

//#define BLYNK_DEBUG
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266_SoftSer.h>
#include <BlynkSimpleShieldEsp8266_SoftSer.h>

// Set NanoESP Serial object
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(11, 12); // RX, TX

ESP8266 wifi(EspSerial);

void setup()
{
  // Set console baud rate
  Serial.begin(19200);
  EspSerial.begin(19200);

  Blynk.begin(token, wifi, ssid, password);
}

void loop()
{
  Blynk.run();
}

