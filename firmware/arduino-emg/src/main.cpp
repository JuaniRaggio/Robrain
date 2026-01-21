#include "HardwareSerial.h"
#include "emg_reader.h"
#include <Arduino.h>

constexpr uint16_t SAMPLE_RATE_HZ = 5;
constexpr uint32_t SAMPLE_INTERVAL_US = 1'000'000 / SAMPLE_RATE_HZ;

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
