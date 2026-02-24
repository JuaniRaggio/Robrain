#include "wheel_command.h"
#include <string.h>

constexpr uint8_t LIMIT_SPEED = 100;

namespace command {

bool parse_wheel_cmd(const uint8_t *data, size_t len, WheelCommand &out) {
  if (data == nullptr || len != sizeof(WheelCommand)) return false;

  memcpy(&out, data, sizeof(WheelCommand));

  // clamping q dijimos hacerlo por las dudas
  out.left = out.left > LIMIT_SPEED ? LIMIT_SPEED : out.left;
  out.right = out.right > LIMIT_SPEED ? LIMIT_SPEED : out.right;

  return true;
}

} // namespace commmand