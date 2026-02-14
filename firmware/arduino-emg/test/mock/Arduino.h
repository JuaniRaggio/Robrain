#pragma once

#include <stdint.h>
#include <string.h>

// Pin mode constants
#define INPUT 0x0
#define OUTPUT 0x1

// Analog pin aliases (match AVR mapping)
#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define A4 18
#define A5 19

// Mock state
constexpr uint8_t MOCK_MAX_PINS = 20;

inline uint16_t mock_analog_values[MOCK_MAX_PINS] = {};

inline void mock_reset() { memset(mock_analog_values, 0, sizeof(mock_analog_values)); }

// Arduino stubs
inline uint16_t analogRead(uint8_t pin) {
  if (pin < MOCK_MAX_PINS) {
    return mock_analog_values[pin];
  }
  return 0;
}

inline void pinMode(uint8_t, uint8_t) {}
