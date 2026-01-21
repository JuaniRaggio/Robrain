#include "emg_reader.h"
#include "Arduino.h"

emg::Reader::Reader(const Config &config) {
  this->config = config;
  this->most_recent = 0;
  // Init INPUT pinModes for every muscle
  for (uint8_t i = A0; i < A0 + emg::MUSCLES; i++) {
    pinMode(i, INPUT);
  }
}

uint8_t emg::Reader::get_most_recently() const {
  return this->most_recent % emg::RECENTLY_SAVED_SAMPLES;
}

void emg::Reader::update_most_recently() {
  this->most_recent++;
}

emg::EmgSample emg::Reader::read_sample() {
  emg::EmgSample sample;
  for (uint8_t i = A0; i < A0 + emg::MUSCLES - 1; i++) {
    sample.channels[i - A0] = analogRead(i);
    recent_samples[get_most_recently()].channels[i] = sample.channels[i];
  }
  update_most_recently();
  return sample;
}

bool emg::Reader::is_muscle_active(uint8_t channel, uint16_t threshold) {
}

