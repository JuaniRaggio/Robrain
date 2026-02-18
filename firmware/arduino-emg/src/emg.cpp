#include "Arduino.h"
#include <emg.h>

// --- ChannelReader ---

emg::Reader::ChannelReader::ChannelReader()
    : pin{0}, history{}, head{0}, count{0}, active{false} {}

void emg::Reader::ChannelReader::read() {
  history[head] = analogRead(pin);
  // === WARNING!! This sum is not checked just because uint8_t caps in ===
  //                        === HISTORY_SIZE (256) ===
  head++, count++;
}

uint16_t emg::Reader::ChannelReader::latest() const {
  if (count == 0) {
    return 0;
  }
  uint8_t idx = (head == 0) ? HISTORY_SIZE - 1 : head - 1;
  return history[idx];
}

uint8_t emg::Reader::ChannelReader::copy_last(uint8_t n, uint16_t *out) const {
  if (n > count) {
    n = count;
  }
  // Copia desde el mas reciente al mas antiguo
  for (uint8_t i = 0; i < n; i++) {
    uint8_t idx = (head + HISTORY_SIZE - 1 - i) % HISTORY_SIZE;
    out[i] = history[idx];
  }
  return n;
}

bool emg::Reader::ChannelReader::is_full() {
  return count == HISTORY_SIZE - 1;
}

// --- Reader ---

emg::Reader::Reader() : channels{} {}

bool emg::Reader::add_reader(Muscle muscle, uint8_t pin) {
  uint8_t idx = static_cast<uint8_t>(muscle);
  if (idx >= static_cast<uint8_t>(Muscle::COUNT)) {
    return false;
  }
  if (channels[idx].active) {
    return false;
  }
  channels[idx].pin = pin;
  channels[idx].active = true;
  pinMode(pin, INPUT);
  return true;
}

void emg::Reader::read_all() {
  for (uint8_t i = 0; i < static_cast<uint8_t>(Muscle::COUNT); i++) {
    if (channels[i].active) {
      channels[i].read();
    }
  }
}

bool emg::Reader::is_full(Muscle muscle) {
  return channels[static_cast<uint8_t>(muscle)].is_full();
}
bool emg::Reader::is_full() {
  return channels[static_cast<uint8_t>(Muscle::LeftBicep)].is_full() ||
         channels[static_cast<uint8_t>(Muscle::RightBicep)].is_full();
}

uint16_t emg::Reader::get(Muscle muscle) const {
  uint8_t idx = static_cast<uint8_t>(muscle);
  if (idx >= static_cast<uint8_t>(Muscle::COUNT)) {
    return 0;
  }
  return channels[idx].latest();
}

uint8_t emg::Reader::get(Muscle muscle, uint8_t n, uint16_t *out) const {
  uint8_t idx = static_cast<uint8_t>(muscle);
  if (idx >= static_cast<uint8_t>(Muscle::COUNT)) {
    return 0;
  }
  return channels[idx].copy_last(n, out);
}

uint8_t emg::Reader::get_count(Muscle muscle) {
  return channels[static_cast<uint8_t>(muscle)].count;
}

uint8_t emg::Reader::get_count() {
  return get_count(Muscle::LeftBicep);
}
