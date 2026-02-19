#pragma once

#include <stddef.h>
#include <stdint.h>

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
    uint8_t last_idx;
    uint8_t pin;
    uint16_t stream_data[stream_size];
    bool active;

    ChannelReader();
    void read();
    uint16_t latest() const;
    template <size_t N> uint8_t get_copy(uint8_t (&out)[N]) const;
    template <size_t N> uint8_t get_copy(uint8_t n, uint8_t (&out)[N]) const;
    bool is_full();
  };

  ChannelReader channels[static_cast<uint8_t>(Muscle::COUNT)];

public:
  Reader();
  ~Reader() = default;

  // @return false if pin is already in use
  bool add_reader(Muscle muscle, uint8_t pin);
  void read_all();
  bool is_full(Muscle muscle);
  bool is_full();

  // template ensures that the output buffer has at least N
  // bytes available
  template <size_t N> uint8_t get_data(Muscle muscle, uint8_t (&out)[N]) const;

  template <size_t N>
  uint8_t get_data(Muscle muscle, uint8_t (&out)[N], uint8_t n) const;

  uint8_t get_count(Muscle muscle);
  uint8_t get_count();
};

} // namespace emg
