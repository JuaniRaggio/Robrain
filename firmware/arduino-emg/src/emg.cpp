#include "Arduino.h"
#include "serial_protocol.h"
#include <cstdint>
#include <emg.h>

// --- ChannelReader ---

emg::Reader::ChannelReader::ChannelReader()
    : last_idx{0}, pin{0}, stream_data{}, active{false} {}

void emg::Reader::ChannelReader::read() {
  stream_data[last_idx++] = analogRead(pin);
  last_idx = last_idx % stream_size;
}

int8_t emg::Reader::ChannelReader::latest() const {
  return stream_data[(last_idx + stream_size - 1) % stream_size];
}

template <size_t N>
uint8_t emg::Reader::ChannelReader::get_copy(uint8_t (&out)[N]) const {
  memcpy(out, stream_data, sizeof(stream_data));
  return SUCCESS;
}

bool emg::Reader::ChannelReader::is_full() const {
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
  for (uint8_t i = 0; i < static_cast<uint8_t>(Muscle::COUNT); ++i)
    if (!is_full(static_cast<Muscle>(i))) return false;
  return true;
}

template <size_t N>
uint8_t emg::Reader::get_data(Muscle muscle, uint8_t (&out)[N]) const {
  static_assert(N == 2 * ChannelReader::stream_size,
                "Buffer debe ser de tamaño HISTORY_SIZE");
  uint8_t idx = static_cast<uint8_t>(muscle);
  if (idx >= static_cast<uint8_t>(Muscle::COUNT)) return 0;
  return channels[idx].get_copy(out);
}

uint8_t emg::Reader::get_count(Muscle muscle) {
  return channels[static_cast<uint8_t>(muscle)].last_idx;
}

uint8_t emg::Reader::get_count() {
  return get_count(Muscle::LeftBicep);
}

// Explicit template instantiations for linkage from other translation units
template uint8_t emg::Reader::get_data<64>(Muscle, uint8_t (&)[64]) const;
