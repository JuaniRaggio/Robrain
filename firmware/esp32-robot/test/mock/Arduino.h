#pragma once

#include <stdint.h>
#include <string.h>

// Pin mode constants
#define INPUT 0x0
#define OUTPUT 0x1

// PWM / analog stubs
inline void pinMode(uint8_t, uint8_t) {}
inline void digitalWrite(uint8_t, uint8_t) {}
inline void analogWrite(uint8_t, int) {}
inline void ledcSetup(uint8_t, double, uint8_t) {}
inline void ledcAttachPin(uint8_t, uint8_t) {}
inline void ledcWrite(uint8_t, uint32_t) {}

// Time stubs
inline uint32_t millis() {
  return 0;
}
inline uint32_t micros() {
  return 0;
}
inline void delay(uint32_t) {}
