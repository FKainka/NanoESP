/*
  Easy Webserver: Listens on Port 80
  Change yourSSID and your PASSWORD
  Open your Browser and type in the IP shown in the SerialMonitor
*/
#include <NanoESP.h>
#include <NanoESP_HTTP.h> //special library for http-connection
#include <SoftwareSerial.h>

#define SSID "YourSSID"
#define PASSWORD "YourPassowrd"

#define LED_WLAN 13

NanoESP nanoesp = NanoESP();
NanoESP_HTTP http = NanoESP_HTTP(nanoesp); //init http library with an instance of nanoesp

void setup()
{
  Serial.begin(19200);
  nanoesp.init();

  //! Only if you use a new Network
  // nanoesp.configWifi(STATION, SSID, PASSWORD);

  //Station Mode
  nanoesp.configWifiMode(STATION);

  // Wait till Connected to WiFi
  if (nanoesp.wifiConnected())digitalWrite(LED_WLAN, HIGH); else Serial.println(F("Error WLAN Connect"));

  //Start TCP Server
  if (nanoesp.startTcpServer(80))    Serial.println(F("TCP Server Activ")); else   Serial.println(F("TCP Server Error"));

  //Print IP to Terminal
  Serial.println(nanoesp.getIp());
}

void loop() // run over and over
{
  String method, ressource, parameter;
  int id;

  if (http.recvRequest(id, method, ressource, parameter)) { //Incoming request, parameters by reference

    Serial.println("New Request from id :" + String(id) + ", method: " + method +  ", ressource: " + ressource +  " parameter: " + parameter);

    String webpage = "<h1>Hello World!";
    nanoesp.sendDataClose(id, webpage);
  }
}

