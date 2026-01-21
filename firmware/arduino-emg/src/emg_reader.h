#pragma once

#include "Arduino.h"
#include <stdint.h>

namespace emg {

constexpr uint8_t MUSCLES = 2;
constexpr uint8_t MAX_CHANNELS = 6;
constexpr uint8_t RECENTLY_SAVED_SAMPLES = 20;

struct Config {
  uint8_t pin_channels[MAX_CHANNELS];
  uint8_t num_channels;
  uint16_t sample_rate_hz;

  Config() : pin_channels{0}, num_channels(emg::MAX_CHANNELS) {};

  Config(uint8_t num_channels, uint16_t sample_rate_hz)
      : pin_channels{0}, num_channels(num_channels),
        sample_rate_hz(sample_rate_hz) {};
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
  Reader(const Config &config);

  ~Reader() = default;

  void init(const Config &config);

  // Lee una muestra de todos los canales
  EmgSample read_sample();

  // Obtiene el valor RMS de un canal (ventana de N muestras)
  uint16_t get_rms(uint8_t channel);

  // Detecta si hay activacion muscular en un canal
  bool is_muscle_active(uint8_t channel, uint16_t threshold);

  const Config &get_config() { return this->config; }
};

} // namespace emg
