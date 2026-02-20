#include "command_parser.h"

namespace robrain {

bool parse_motor_command(const uint8_t *data, size_t len, MotorCommand &out) {
  if (data == nullptr) return false;

  // 2 bytes: cmd + intensity
  if (len != MOTOR_COMMAND_SIZE) return false;

  const uint8_t cmd = data[0];
  const uint8_t intensity = data[1];

  if (cmd >= static_cast<uint8_t>(CommandType::QUANTITY)) return false;

  out.cmd = static_cast<CommandType>(cmd);
  out.intensity = (intensity > 100) ? 100 : intensity;

  return true;
}

} // namespace robrain