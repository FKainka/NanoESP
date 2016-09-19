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
  digitalWrite(13, HIGH);
  
  nanoesp.configWifiMode(3);
  
  Serial.println(nanoesp.sendCom("AT+GMR"));

  nanoesp.setTimeout(4000);
  Serial.println(nanoesp.sendCom("AT+CWLAP"));
 
}

void loop() // run over and over
{

}

