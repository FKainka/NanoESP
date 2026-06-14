// Minimal Arduino.h stub for the PlatformIO `native` test environment.
// Provides just enough of the Arduino API (types, String, Serial, millis)
// for the real NanoESP_MQTT.cpp translation unit to compile and link on a
// desktop host. None of the hardware-touching stubs are ever executed by the
// unit tests — they only need to satisfy the compiler and linker.
#ifndef NANOESP_NATIVE_ARDUINO_H
#define NANOESP_NATIVE_ARDUINO_H

#include <cstdint>
#include <string>

typedef uint8_t byte;
typedef bool boolean;

#define HEX 16
#define DEC 10
#define BIN 2
#define OCT 8

#define F(string_literal) (string_literal)
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

inline unsigned long millis() { return 0UL; }

// --- Arduino String shim, backed by std::string -----------------------------
class String {
  std::string s;
 public:
  String() {}
  String(const char* c) : s(c ? c : "") {}
  String(char c) : s(1, c) {}
  String(int n) : s(std::to_string(n)) {}
  String(unsigned int n) : s(std::to_string(n)) {}
  String(long n) : s(std::to_string(n)) {}
  String(unsigned long n) : s(std::to_string(n)) {}

  unsigned int length() const { return (unsigned int)s.length(); }
  const char* c_str() const { return s.c_str(); }

  char operator[](int i) const { return s[(size_t)i]; }
  char& operator[](int i) { return s[(size_t)i]; }

  int indexOf(char c) const {
    size_t p = s.find(c);
    return p == std::string::npos ? -1 : (int)p;
  }
  int indexOf(const char* o) const {
    size_t p = s.find(o);
    return p == std::string::npos ? -1 : (int)p;
  }
  int indexOf(const String& o) const { return indexOf(o.c_str()); }

  String substring(int from) const {
    if (from < 0) from = 0;
    if ((size_t)from >= s.size()) return String();
    return String(s.substr((size_t)from).c_str());
  }
  String substring(int from, int to) const {
    if (from < 0) from = 0;
    if ((size_t)from > s.size()) from = (int)s.size();
    if (to < from) to = from;
    if ((size_t)to > s.size()) to = (int)s.size();
    return String(s.substr((size_t)from, (size_t)(to - from)).c_str());
  }

  String& operator+=(char c) { s += c; return *this; }
  String& operator+=(const char* o) { s += o; return *this; }
  String& operator+=(const String& o) { s += o.s; return *this; }

  bool operator==(const String& o) const { return s == o.s; }
  bool operator==(const char* o) const { return s == std::string(o ? o : ""); }
  bool operator!=(const String& o) const { return !(*this == o); }
  bool operator!=(const char* o) const { return !(*this == o); }

  friend String operator+(const String& a, const String& b) {
    String r; r.s = a.s + b.s; return r;
  }
  friend String operator+(const String& a, const char* b) {
    String r; r.s = a.s + std::string(b ? b : ""); return r;
  }
  friend String operator+(const char* a, const String& b) {
    String r; r.s = std::string(a ? a : "") + b.s; return r;
  }
};

// --- Serial stub ------------------------------------------------------------
struct SerialStub {
  void begin(long) {}
  template <class T> void print(const T&) {}
  template <class T> void print(const T&, int) {}
  template <class T> void println(const T&) {}
  template <class T> void println(const T&, int) {}
  void println() {}
};

inline SerialStub Serial;

#endif  // NANOESP_NATIVE_ARDUINO_H
