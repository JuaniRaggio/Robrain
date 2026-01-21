#include "HardwareSerial.h"
#include "emg_reader.h"
#include <Arduino.h>

constexpr uint8_t EMG_PIN_0 = A0;
constexpr uint8_t EMG_PIN_1 = A1;
constexpr uint16_t SAMPLE_RATE_HZ = 5;
constexpr uint32_t SAMPLE_INTERVAL_US =
    1000000 / SAMPLE_RATE_HZ; // 500000 us = 0.5 seg

constexpr uint32_t baudrate = 115200;

static emg::Reader reader;

uint32_t last_sample_us = 0;

void setup() { Serial.begin(baudrate); }

// We are detecting low peaks while user has tense muscle
void loop() {
  uint32_t now = micros();
  if (now - last_sample_us >= SAMPLE_INTERVAL_US) {
    last_sample_us = now;
    reader.read_sample();
  }
}
