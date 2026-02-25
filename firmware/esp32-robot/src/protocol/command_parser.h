#pragma once

#include <stddef.h>
#include <stdint.h>

namespace robrain {

enum class CommandType : uint8_t {
  STOP = 0,
  FORWARD = 1,
  TURN_LEFT = 2,
  TURN_RIGHT = 3,
  QUANTITY
};

struct MotorCommand {
  CommandType cmd;   // 1 byte
  uint8_t intensity; // 0..100 (hoy fijo, mañana variable)
} __attribute__((packed));

constexpr size_t MOTOR_COMMAND_SIZE = sizeof(MotorCommand);

bool parse_motor_command(const uint8_t *data, size_t len, MotorCommand &out);

} // namespace robrain