/*
   Vewrbindet sich mit einem WLAN-Router (SSID und PASSWORD ändern)
   Mit Programmen wie https://packetsender.com/ kann ein TCP-Paket gesendet werden
   Alternativ kann im Browser die IP des Boards geöffnet werden
   Mehr Informationen: http://iot.fkainka.de/day-3

   Connets to WiFi (change SSID and PASSWORD)
   Send TCP-Messages: https://packetsender.com/
   or open the IP in a Browser
   More Infos: http://iot.fkainka.de/en/day-3
*/

#include <NanoESP.h>
#include <SoftwareSerial.h>

#define SSID "YourSSID"
#define PASSWORD "YourPassword"

#define LED_WLAN 13

NanoESP nanoesp = NanoESP();

void setup() {
  Serial.begin(19200);
  nanoesp.init();

  //Connects with your WiFi
  if (nanoesp.configWifi(STATION, SSID, PASSWORD)) {  //AP Mode: Module creates WiFi
    Serial.println("Wifi ready");
    digitalWrite(LED_WLAN, HIGH);
  }
  else {
    Serial.println("Wifi not ready");
  }

  //Print IP in Terminal
  Serial.println(nanoesp.getIp());
}

void loop() {
  Serial.println("Ping Google");
  Serial.println("Respond Time: " + String(nanoesp.ping("www.google.de"))+ " ms");
  delay(1000);
}
