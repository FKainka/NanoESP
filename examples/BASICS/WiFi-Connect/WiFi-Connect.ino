/*
   Verbindung zu dem eingenen WLAN herstellen/WLAN Erstellen
   SSID und PASSWORD ändern

   Change SSID and PASSWORD
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

  //Connect to WiFi
  if (nanoesp.configWifi(STATION, SSID, PASSWORD)) {    //Station Mode: Your Home WiFi
  // if (nanoesp.configWifi(ACCESSPOINT, SSID, PASSWORD)) {  //AP Mode: Module creates WiFi 
  // if (nanoesp.configWifi(DUAL, SSID, PASSWORD)) {     //Dual Mode: Both Modes
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

}
