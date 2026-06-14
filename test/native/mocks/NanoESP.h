// Minimal NanoESP stub for the PlatformIO `native` test environment.
// Replaces the real (SoftwareSerial-based) NanoESP class so NanoESP_MQTT.cpp
// can be compiled on a desktop host. Only the methods that NanoESP_MQTT.cpp
// references are declared, with inline no-op bodies so the link succeeds. The
// unit tests exercise topicCompare()/utf8()/charAdd(), none of which touch
// these methods, so the stub bodies are never run.
#ifndef NANOESP_NATIVE_NANOESP_H
#define NANOESP_NATIVE_NANOESP_H

#include "Arduino.h"

// Connection-type macros normally defined in the real NanoESP.h.
#ifndef TCP
#define TCP "TCP"
#endif
#ifndef UDP
#define UDP "UDP"
#endif

class NanoESP {
 public:
  NanoESP() {}
  bool newConnection(int, const String&, const String&, unsigned int) { return false; }
  int available() { return 0; }
  bool findUntil(const char*, const char*) { return false; }
  bool find(const char*) { return false; }
  long parseInt() { return 0; }
  int readBytes(char*, int) { return 0; }
  void println(const String&) {}
  void write(unsigned char) {}
};

#endif  // NANOESP_NATIVE_NANOESP_H
