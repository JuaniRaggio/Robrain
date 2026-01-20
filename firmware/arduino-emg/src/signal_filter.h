#pragma once

#include <cstdint>

namespace filter {

// Filtro pasa-bajos simple (media movil)
class LowPassFilter {
public:
    LowPassFilter(uint8_t window_size);

    uint16_t process(uint16_t sample);
    void reset();

private:
    uint16_t* buffer;
    uint8_t window_size;
    uint8_t index;
    uint32_t sum;
    bool filled;
};

// Filtro pasa-altos (remueve DC offset)
class HighPassFilter {
public:
    HighPassFilter(float alpha);  // alpha: 0.0-1.0, mayor = menos filtrado

    int16_t process(uint16_t sample);
    void reset();

private:
    float alpha;
    float prev_output;
    float prev_input;
};

// Filtro notch para 50/60Hz (interferencia de linea electrica)
class NotchFilter {
public:
    NotchFilter(uint16_t sample_rate, uint16_t notch_freq);

    int16_t process(int16_t sample);
    void reset();

private:
    // Coeficientes del filtro IIR
    float b0, b1, b2;
    float a1, a2;
    float x1, x2;  // Entradas previas
    float y1, y2;  // Salidas previas
};

// Calcula RMS de un buffer
uint16_t calculate_rms(const uint16_t* samples, uint16_t count);

// Detecta picos en senal
bool detect_peak(const uint16_t* samples, uint16_t count, uint16_t threshold);

} // namespace filter
