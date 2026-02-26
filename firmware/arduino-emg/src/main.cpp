#include "serial_protocol.h"
#include "types/signal_types.h"
#include <Arduino.h>
#include <emg.h>

constexpr uint16_t SAMPLE_RATE_HZ = 1'000;
constexpr uint32_t SAMPLE_INTERVAL_US = 1'000'000ul / SAMPLE_RATE_HZ;

static emg::Reader reader;

uint32_t last_sample_us = 0;

void setup() {
  Serial.begin(robrain::baudrate);

  // ADC prescaler 32: clock 500kHz, ~26us per read
  ADCSRA = (ADCSRA & ~((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))) |
           (1 << ADPS2) | (1 << ADPS0);
  // Option B is setting it to 16: clock 1
  // ADC prescaler 16: clock 1MHz, ~13us per read
  // ADCSRA = (ADCSRA & 0xF8) | 0x04;

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
