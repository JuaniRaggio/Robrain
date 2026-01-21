// TODO: Implementar
#include "emg_reader.h"
#include "Arduino.h"

emg::Reader::Reader(const Config &config) {
  this->config = config;
  // Init INPUT pinModes for every muscle
  for (uint8_t i = A0; i < A0 + emg::MUSCLES - 1; i++)
    pinMode(i, INPUT);
}

emg::EmgSample emg::Reader::read_sample() {
  emg::EmgSample sample;
  for (uint8_t i = A0; i < A0 + emg::MUSCLES - 1; i++)
    sample.channels[i] = analogRead(i);
  return sample;
}

