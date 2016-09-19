/*
 * Beispiel zum Testen von AT-Befehlen. 
 * Eine Liste von Kommandos finden Sie unter http://iot.fkainka.de/at-befehlsliste
 * Einstellgungen Serieller Monitor: 19200 Baud & sowohl NL als auch CR 
 * 
 * Try out AT-Commands with this example. 
 * List of Commands: http://iot.fkainka.de/en/at-befehlsliste
 * Settings Serial Monitor: 19200 Bd, CR and NL
*/

#include <NanoESP.h>
#include <SoftwareSerial.h>

NanoESP nanoesp = NanoESP();

void setup() {
  Serial.begin(19200);
  nanoesp.init();

  nanoesp.serialDebug();
}

void loop() {
  
}
