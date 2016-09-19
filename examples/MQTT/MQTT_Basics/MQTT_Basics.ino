#include <NanoESP.h>
#include <NanoESP_MQTT.h>

#include <SoftwareSerial.h>

/*
  MQTT Basic functions
  Change SSID and PASSWORD.
*/

#define SSID "YourSSID"
#define PASSWORD "YourPassword"

#define LED_WLAN 13
#define DEBUG true

NanoESP nanoesp = NanoESP();
NanoESP_MQTT mqtt = NanoESP_MQTT(nanoesp);

void setup() {
  Serial.begin(19200);

  nanoesp.init();
  nanoesp.configWifiStation(SSID, PASSWORD);

  if (nanoesp.wifiConnected()) {
    Serial.println ("Wifi connected");
    digitalWrite(LED_WLAN, HIGH);
  }
  else {
    Serial.println ("Wifi not Connected");
  }

  //Print IP in Terminal
  Serial.println (nanoesp.getIp());

  //if (mqtt.MQTT_Connect(0, "broker.mqttdashboard.com", 1883, "NanoESP")) { //alternative broker

  if (mqtt.connect(0, "broker.hivemq.com", 1883, "NanoESP")) {

    if (mqtt.publish(0, "a/b", "Hello from NanoESP")) Serial.println ("Published to a/b");
    if (mqtt.publish(0, "test", "Hello from NanoESP")) Serial.println ("Published to test");
    if (mqtt.subscribe(0, "test")) Serial.println ("Subscribed to test");
    if (mqtt.subscribe(0, "a/#")) Serial.println ("Subscribed to a/#");

    //if (MQTT_Unsubscribe(0, "test")) debug("Unsubscribed from test");
    //if (MQTT_Ping(0)) debug("Ping send ");
    //if (MQTT_Disconnect(0)) debug("Disconnect");
  }
}

void loop() {
  int id, len;

  if (nanoesp.recvData(id, len)) {

    String topic, value;
    if (mqtt.recvMQTT(id, len, topic, value)) {
      Serial.println ("New Message:\nTopic=" + topic + " Value=" + value);
    }

  }
    mqtt.stayConnected(0);        //keep the connection to broker alive 
}

