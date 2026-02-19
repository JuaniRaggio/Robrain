#include "Arduino.h"
#include <emg.h>

// --- ChannelReader ---

emg::Reader::ChannelReader::ChannelReader()
    : last_idx{0}, pin{0}, stream_data{}, active{false} {}

void emg::Reader::ChannelReader::read() {
  stream_data[last_idx] = analogRead(pin);
  last_idx = (last_idx + 1) % stream_size;
}

uint16_t emg::Reader::ChannelReader::latest() const {
  return stream_data[last_idx];
}

template <size_t N>
uint8_t emg::Reader::ChannelReader::get_copy(uint8_t (&out)[N]) const {
  return get_copy(stream_size, out);
}

template <size_t N>
uint8_t emg::Reader::ChannelReader::get_copy(uint8_t n,
                                             uint8_t (&out)[N]) const {
  if (n > stream_size) {
    n = last_idx;
  }
  memcpy(out, stream_data, sizeof(stream_data));
  return n;
}

bool emg::Reader::ChannelReader::is_full() {
  return last_idx == stream_size - 1;
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

template <size_t N>
uint8_t emg::Reader::get_data(Muscle muscle, uint8_t (&out)[N]) const {
  return get_data(muscle, out, ChannelReader::stream_size);
}

template <size_t N>
uint8_t emg::Reader::get_data(Muscle muscle, uint8_t (&out)[N],
                              uint8_t n) const {
  static_assert(N == 2 * ChannelReader::stream_size,
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
