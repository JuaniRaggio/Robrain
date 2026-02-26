#pragma once

#include <stdint.h>

namespace robrain {

// UUIDs deben coincidir con el ESP32
namespace ble_uuids {
    constexpr const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
    constexpr const char* CMD_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
    constexpr const char* STATUS_CHAR_UUID = "beb5483f-36e1-4688-b7f5-ea07361b26a8";
}

constexpr uint8_t MAX_EMG_CHANNELS = 4;
constexpr uint16_t DEFAULT_SAMPLE_RATE_HZ = 500;
constexpr uint16_t DEFAULT_ACTIVATION_THRESHOLD = 512;

constexpr uint32_t baudrate = 1000000;

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
