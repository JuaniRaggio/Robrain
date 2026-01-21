#pragma once

#include "Arduino.h"
#include "HardwareSerial.h"
#include <stdint.h>

namespace emg {

constexpr uint8_t MUSCLES = 2;
constexpr uint8_t MAX_CHANNELS = 6;
constexpr uint16_t DEFAULT_SAMPLE_RATE_HZ = 500;

struct Config {
  uint8_t pin_channels[MAX_CHANNELS];
  uint8_t num_channels;
  uint16_t sample_rate_hz;

  Config()
      : pin_channels{0}, num_channels(emg::MAX_CHANNELS),
        sample_rate_hz(emg::DEFAULT_SAMPLE_RATE_HZ) {};

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
  Config config;

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
