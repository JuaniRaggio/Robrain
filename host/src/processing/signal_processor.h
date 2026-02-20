#pragma once

#include "protocol/serial_packet.h"
#include "serial/scsp.h"
#include <cstdint>
#include <vector>
#include <functional>

namespace robrain {

// Intencion detectada del usuario
enum class UserIntent : uint8_t {
    NONE,
    LEFT_WHEEL_FORWARDS,
    RIGHT_WHEEL_FORWARDS,
    BOTH_WHEEL_FORWARDS,
};

struct ProcessingResult {
    UserIntent intent;
    float confidence;
    int16_t left_speed;
    int16_t right_speed;
};

// Configuracion del procesador
struct ProcessorConfig {
    uint16_t activation_threshold;
    uint16_t window_size_ms;
    float smoothing_factor;
};

// TODO SignalProcessor will be improved, first versions
// will be naive
class SignalProcessor {
private:
  serial::Consumer<serial_proto::Payload, 4064> consumable_;

public:
    using IntentCallback = std::function<void(const ProcessingResult&)>;

    SignalProcessor(serial::Consumer<serial_proto::Payload, 4064>& consumable);
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

    void start_calibration();
    void stop_calibration();
    bool is_calibrating() const;

    std::vector<uint16_t> get_calibrated_thresholds() const;

};

} // namespace robrain
