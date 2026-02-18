#include "Arduino.h"
#include <emg.h>

// --- ChannelReader ---

emg::Reader::ChannelReader::ChannelReader()
    : last_idx{0}, pin{0}, history{}, active{false} {}

void emg::Reader::ChannelReader::read() {
  history[last_idx] = analogRead(pin);
  last_idx = (last_idx + 1) % HISTORY_SIZE;
}

uint16_t emg::Reader::ChannelReader::latest() const {
  return history[last_idx];
}

template <uint64_t N>
uint8_t emg::Reader::ChannelReader::get_copy(uint8_t (&out)[N]) const {
  return get_copy(HISTORY_SIZE, out);
}

template <uint64_t N>
uint8_t emg::Reader::ChannelReader::get_copy(uint8_t n,
                                             uint8_t (&out)[N]) const {
  if (n > HISTORY_SIZE) {
    n = last_idx;
  }
  for (uint8_t i = 0; i < n; i++) {
    out[i] = 0;
    out[i] |= (history[i] << 8) >> 8;
    out[i] |= (history[i] >> 8) << 8;
  }
  return n;
}

bool emg::Reader::ChannelReader::is_full() {
  return last_idx == HISTORY_SIZE - 1;
}

// --- Reader ---

emg::Reader::Reader() : channels{} {}

bool emg::Reader::add_reader(Muscle muscle, uint8_t pin) {
  uint8_t idx = static_cast<uint8_t>(muscle);
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

template <uint64_t N>
uint8_t emg::Reader::get_data(Muscle muscle, uint8_t (&out)[N]) const {
  return get_data(muscle, out, ChannelReader::HISTORY_SIZE);
}

template <uint64_t N>
uint8_t emg::Reader::get_data(Muscle muscle, uint8_t (&out)[N],
                              uint8_t n) const {
  // ensure using templates in compile time that the given buffer has
  // HISTORY_SIZE size (2*) because HISTORY is uint16_t
  static_assert(N == 2 * ChannelReader::HISTORY_SIZE,
                "Buffer debe ser de tamaño HISTORY_SIZE");
  uint8_t idx = static_cast<uint8_t>(muscle);
  if (idx >= static_cast<uint8_t>(Muscle::COUNT)) {
    return 0;
  }
  return channels[idx].get_copy(n, out);
}

uint8_t emg::Reader::get_count(Muscle muscle) {
  return channels[static_cast<uint8_t>(muscle)].last_idx;
}

uint8_t emg::Reader::get_count() {
  return get_count(Muscle::LeftBicep);
}
