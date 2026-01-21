#pragma once

#include "Arduino.h"
#include <stdint.h>

namespace emg {

constexpr uint8_t MUSCLES = 2;
constexpr uint8_t MAX_CHANNELS = 6;
constexpr uint8_t RECENTLY_SAVED_SAMPLES = 20;
constexpr uint8_t DEFAULT_SAMPLE_RATE_HZ = 500;

struct Config {
  uint8_t pin_channels[MAX_CHANNELS];
  uint8_t num_channels;
  uint16_t sample_rate_hz;

  void init_pin_channels() {
    for (uint8_t i = A0; i < A0 + num_channels; i++) {
      pin_channels[i] = i;
      pinMode(i, INPUT);
    }
  };

  Config()
      : pin_channels{}, num_channels{emg::MUSCLES},
        sample_rate_hz{emg::DEFAULT_SAMPLE_RATE_HZ} {
    init_pin_channels();
  };

  Config(uint8_t num_channels, uint16_t sample_rate_hz)
      : pin_channels{}, num_channels{num_channels},
        sample_rate_hz{sample_rate_hz} {
    init_pin_channels();
  };
};

struct EmgSample {
  uint32_t timestamp_ms;
  uint16_t channels[MAX_CHANNELS];
  uint8_t num_channels;
};

class Reader {
private:
  emg::Config config;
  emg::EmgSample recent_samples[RECENTLY_SAVED_SAMPLES];
  uint8_t most_recent;

  uint8_t get_most_recently() const;
  void update_most_recently();

public:
  Reader();

  Reader(const Config &config);

  ~Reader() = default;

  // @return sample of the current state of used channels
  EmgSample read_sample();

  // Obtiene el valor RMS de un canal (ventana de N muestras)
  uint16_t get_rms(uint8_t channel);

  // Detecta si hay activacion muscular en un canal
  bool is_muscle_active(uint8_t channel, uint16_t threshold);

  const Config &get_config() { return this->config; }
};

} // namespace emg
