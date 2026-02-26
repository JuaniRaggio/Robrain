#include <protocol/wheel_command.h>
#include <string.h>

namespace command {

bool parse_motor_payload(const uint8_t *data, size_t len, wireless_protocol::MotorPayload &out) {
  if (data == nullptr || len != sizeof(wireless_protocol::MotorPayload)) return false;

  memcpy(&out, data, sizeof(wireless_protocol::MotorPayload));

  // clamping q dijimos hacerlo por las dudas
  out.left_speed = out.left_speed > wireless_protocol::LIMIT_SPEED ? wireless_protocol::LIMIT_SPEED : out.left_speed;
  out.right_speed = out.right_speed > wireless_protocol::LIMIT_SPEED ? wireless_protocol::LIMIT_SPEED : out.right_speed;

  return true;
}

} // namespace command
