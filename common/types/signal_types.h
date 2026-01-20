#pragma once

#include <cstdint>

namespace robrain {

// Constantes del sistema
constexpr uint8_t MAX_EMG_CHANNELS = 4;
constexpr uint16_t DEFAULT_SAMPLE_RATE_HZ = 500;
constexpr uint16_t DEFAULT_ACTIVATION_THRESHOLD = 512;

// Rango de velocidad de motores
constexpr int16_t MOTOR_SPEED_MIN = -100;
constexpr int16_t MOTOR_SPEED_MAX = 100;

// Estados del robot
enum class RobotState : uint8_t {
    IDLE = 0,
    RUNNING = 1,
    ERROR = 2,
    CALIBRATING = 3,
};

// Intencion del usuario derivada de senales EMG
enum class Intent : uint8_t {
    NONE = 0,
    FORWARD = 1,
    BACKWARD = 2,
    LEFT = 3,
    RIGHT = 4,
    STOP = 5,
};

// Mapeo de canal EMG a accion
struct ChannelMapping {
    uint8_t channel;
    Intent intent;
    uint16_t threshold;
};

} // namespace robrain
