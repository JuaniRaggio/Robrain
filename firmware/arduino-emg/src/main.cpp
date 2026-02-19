#include "serial_protocol.h"
#include <Arduino.h>
#include <emg.h>

constexpr uint16_t SAMPLE_RATE_HZ = 1'000;
constexpr uint32_t SAMPLE_INTERVAL_US = 1'000'000ul / SAMPLE_RATE_HZ;
constexpr uint32_t baudrate = 1'000'000;

static emg::Reader reader;

uint32_t last_sample_us = 0;

void setup() {
  Serial.begin(baudrate);
  reader.add_reader(emg::Muscle::LeftBicep, A0);
  reader.add_reader(emg::Muscle::RightBicep, A1);
}

// Tiene una limitacion de delay() pero no saben de cuanto tiempo
void loop() {
  uint32_t now = micros();
  if (now - last_sample_us >= SAMPLE_INTERVAL_US) {
    last_sample_us = now;
    reader.read_all();
    if (reader.is_full()) {
      serial_proto::send_emg_packet(reader);
    }
  }
}
