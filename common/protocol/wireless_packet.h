#pragma once

#include <cstdint>
#include <cstddef>

namespace wireless_protocol {

// Info compartida para configuración host - esp32
constexpr const char *DEVICE_NAME = "ROBRAIN_ROBOT";
constexpr const char *SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
constexpr const char *CMD_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
constexpr const char *STATUS_CHAR_UUID = "beb5483f-36e1-4688-b7f5-ea07361b26a8";

// Payload de comando de motor
struct MotorPayload {
    int16_t left_speed;   // -100 a 100
    int16_t right_speed;  // -100 a 100
};

} // namespace protocol
