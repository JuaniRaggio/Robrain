#pragma once

#include <cstdint>
#include <cstddef>

namespace robrain {

enum class CommandType : uint8_t {
    MOTOR_SET     = 0x01,
    MOTOR_STOP    = 0x02,
    STATUS_REQ    = 0x10,
    STATUS_RESP   = 0x11,
    EMG_DATA      = 0x20,
    HEARTBEAT     = 0x30,
};

struct MotorCommand {
    int16_t left_speed;   // -100 a 100
    int16_t right_speed;  // -100 a 100
};

struct Packet {
    static constexpr uint8_t START_BYTE = 0xAA;
    static constexpr uint8_t END_BYTE = 0x55;

    uint8_t start;
    CommandType cmd;
    uint8_t length;
    uint8_t payload[32];
    uint8_t checksum;
    uint8_t end;
};

// Parsea comando de motor desde buffer
bool parse_motor_command(const uint8_t* data, size_t len, MotorCommand& out);

// Serializa comando de motor a buffer
size_t serialize_motor_command(const MotorCommand& cmd, uint8_t* buffer, size_t max_len);

// Calcula checksum XOR
uint8_t calculate_checksum(const uint8_t* data, size_t len);

} // namespace robrain
