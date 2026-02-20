#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace emg {

enum class Muscle : uint8_t {
  LeftBicep = 0,
  RightBicep = 1,
  COUNT,
};

class Reader {
private:
  constexpr static uint8_t MAX_CHANNELS = 6;
  struct ChannelReader {
    constexpr static uint16_t stream_size = 32;
    int8_t last_idx;
    uint8_t pin;
    uint16_t stream_data[stream_size];
    bool active;

    ChannelReader();
    void read();
    int8_t latest() const;
    template <size_t N> uint8_t get_copy(uint8_t (&out)[N]) const;
    bool is_full() const;
  };

  ChannelReader channels[static_cast<uint8_t>(Muscle::COUNT)];

public:
  constexpr static uint16_t total_stream_size =
      ChannelReader::stream_size * static_cast<uint8_t>(Muscle::COUNT);

  Reader();

  // @return false if pin is already in use
  bool add_reader(Muscle muscle, uint8_t pin);
  void read_all();
  bool is_full(Muscle muscle);
  bool is_full();

  // template ensures that the output buffer has at least N
  // bytes available
  template <size_t N> uint8_t get_data(Muscle muscle, uint8_t (&out)[N]) const;

  uint8_t get_count(Muscle muscle);
  uint8_t get_count();
};

// --- Template definitions (must be in header) ---

template <size_t N>
uint8_t Reader::ChannelReader::get_copy(uint8_t (&out)[N]) const {
  static_assert(
      N >= 2 * ChannelReader::stream_size,
      "error at get_data: buffer should have at least HISTORY_SIZE size");
  memcpy(out, stream_data, sizeof(stream_data));
  return 0;
}

template <size_t N>
uint8_t Reader::get_data(Muscle muscle, uint8_t (&out)[N]) const {
  uint8_t idx = static_cast<uint8_t>(muscle);
  if (idx >= static_cast<uint8_t>(Muscle::COUNT)) return 0;
  return channels[idx].get_copy(out);
}

} // namespace emg
