#pragma once

#include <muscles.h>
#include <stdint.h>

namespace emg {

constexpr uint8_t MAX_CHANNELS = 6;
constexpr uint8_t HISTORY_SIZE = 256;

class Reader {
private:
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
  };

  ChannelReader channels[static_cast<uint8_t>(Muscle::COUNT)];

public:
  Reader();
  ~Reader() = default;

  bool add_reader(Muscle muscle, uint8_t pin);
  void read_all();
  uint16_t get(Muscle muscle) const;
  uint8_t get(Muscle muscle, uint8_t n, uint16_t *out) const;
};

} // namespace emg
