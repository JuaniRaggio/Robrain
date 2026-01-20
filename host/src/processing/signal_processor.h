#pragma once

#include <cstdint>
#include <vector>
#include <functional>

namespace robrain {

// Intencion detectada del usuario
enum class UserIntent {
    NONE,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    STOP,
};

// Resultado del procesamiento
struct ProcessingResult {
    UserIntent intent;
    float confidence;      // 0.0 - 1.0
    int16_t left_speed;    // Velocidad sugerida
    int16_t right_speed;
};

// Configuracion del procesador
struct ProcessorConfig {
    uint16_t activation_threshold;  // Umbral de activacion muscular
    uint16_t window_size_ms;        // Ventana de analisis
    float smoothing_factor;         // Factor de suavizado (0.0 - 1.0)
};

class SignalProcessor {
public:
    using IntentCallback = std::function<void(const ProcessingResult&)>;

    SignalProcessor();
    ~SignalProcessor();

    // Configura el procesador
    void configure(const ProcessorConfig& config);

    // Procesa nueva muestra EMG
    // channels: valores de cada canal EMG
    void process_sample(const std::vector<uint16_t>& channels);

    // Obtiene ultimo resultado
    ProcessingResult get_result() const;

    // Registra callback para cambios de intencion
    void set_intent_callback(IntentCallback cb);

    // Mapeo de canales a acciones
    // channel_map[i] = {intent para activacion, intent para desactivacion}
    void set_channel_mapping(uint8_t channel, UserIntent on_active);

    // Calibracion
    void start_calibration();
    void stop_calibration();
    bool is_calibrating() const;

    // Obtiene umbrales calibrados por canal
    std::vector<uint16_t> get_calibrated_thresholds() const;

private:
    struct Impl;
    Impl* pimpl;
};

} // namespace robrain
