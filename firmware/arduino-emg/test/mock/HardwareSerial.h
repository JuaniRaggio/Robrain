#pragma once

#include <stddef.h>
#include <stdint.h>

class HardwareSerial {
public:
  void begin(unsigned long) {}
  size_t write(const uint8_t *, size_t) { return 0; }
  size_t available() { return 0; }
  int read() { return -1; }
};

inline HardwareSerial Serial;
