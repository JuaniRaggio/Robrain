#pragma once

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
  constexpr static uint16_t HISTORY_SIZE = 256;
  struct ChannelReader {
    uint8_t pin;
    uint16_t history[HISTORY_SIZE];
    uint8_t head;
    uint8_t count;
    bool active;

    ChannelReader();
    void read();
    uint16_t latest() const;
    uint8_t copy_last(uint8_t n, uint16_t *out) const;
    bool is_full();
  };

  ChannelReader channels[static_cast<uint8_t>(Muscle::COUNT)];

public:
  Reader();
  ~Reader() = default;

  bool add_reader(Muscle muscle, uint8_t pin);
  void read_all();
  bool is_full(Muscle muscle);
  bool is_full();
  uint16_t get(Muscle muscle) const;
  uint8_t get(Muscle muscle, uint8_t n, uint16_t *out) const;
  uint8_t get_count(Muscle muscle);
  uint8_t get_count();
};

} // namespace emg
