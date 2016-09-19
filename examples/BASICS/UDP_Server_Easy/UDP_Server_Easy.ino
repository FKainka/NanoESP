/*
UDP Server Examples.
Easy and quick without error handling
Change SSID and PW
 */
#include <NanoESP.h>
#include <SoftwareSerial.h>

#define SSID "YourSSID"
#define PASSWORD "YourPassword"

#define DEBUG true

NanoESP nanoesp = NanoESP();

void setup()
{
  Serial.begin(19200);

  nanoesp.init();
  nanoesp.configWifi(ACCESSPOINT, "NanoESP", "");

  nanoesp.startUdpServer(0, "192.168.4.255", 90, 90);

  digitalWrite(13, HIGH);
  debug(nanoesp.getIp());

}

void loop() // run over and over
{
  int client = nanoesp.getId();
  if (client >= 0) {
    nanoesp.sendData(client, "Hello UDP Client");
  }

}


void debug(String Msg)
{
  if (DEBUG)
  {
    Serial.println(Msg);
  }
}
