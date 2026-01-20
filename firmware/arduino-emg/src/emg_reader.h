#pragma once

#include <stdint.h>

namespace emg {

// Configuracion de canales EMG
constexpr uint8_t MAX_CHANNELS = 2;
constexpr uint16_t DEFAULT_SAMPLE_RATE_HZ = 500;

struct EmgSample {
    uint32_t timestamp_ms;
    uint16_t channels[MAX_CHANNELS];
    uint8_t num_channels;
};

struct Config {
    uint8_t pin_channels[MAX_CHANNELS];  // Pines analogicos
    uint8_t num_channels;
    uint16_t sample_rate_hz;
};

// Inicializa el lector con configuracion
void init(const Config& config);

// Lee una muestra de todos los canales
EmgSample read_sample();

// Obtiene el valor RMS de un canal (ventana de N muestras)
uint16_t get_rms(uint8_t channel);

// Detecta si hay activacion muscular en un canal
bool is_muscle_active(uint8_t channel, uint16_t threshold);

// Obtiene la configuracion actual
const Config& get_config();

} // namespace emg
