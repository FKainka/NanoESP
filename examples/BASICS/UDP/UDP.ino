/*
   Erstellt ein AccesPoint und öffnet eine UDP-Verbindung
   Mit Programmen wie https://packetsender.com/ kann ein UDP-Paket gesendet werden
   Mehr Informationen: http://iot.fkainka.de/day-3

   Creates AccessPoint and opens UDP-Connection
   Send UDP-Messages: https://packetsender.com/
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

  //Create AccessPoint
  if (nanoesp.configWifi(ACCESSPOINT, SSID, PASSWORD)) {  //AP Mode: Module creates WiFi
    Serial.println("Wifi ready");
    digitalWrite(LED_WLAN, HIGH);
  }
  else {
    Serial.println("Wifi not ready");
  }

  //Print IP in Terminal
  Serial.println(nanoesp.getIp());

  //Start UDP-Connection with id=0, IP=Broadcast IP, Send Port & Recieve Port = 55056
  if (nanoesp.startUdpServer(0, "192.168.4.255", 55056, 55056)) {
    Serial.println("UDP Connection OK");
  }
  else {
    Serial.println("UDP Connection not OK");
  }
}

void loop() {
  int client, len;

  //Recieve Data
  if (nanoesp.recvData(client, len)) {
    //Print Recieved Data in Serial Monitor
    Serial.println(nanoesp.readStringUntil('\r'));
    //Respond
    nanoesp.sendData(client, "Hello UDP Client");
  }
}
