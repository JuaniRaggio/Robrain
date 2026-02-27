#include <algorithm>
#include <protocol/wireless_packet.h>
#include <string.h>

namespace command {

bool parse_motor_payload(const uint8_t *data, size_t len,
                         wireless_protocol::MotorPayload &out) {
  if (data == nullptr || len != sizeof(wireless_protocol::MotorPayload))
    return false;

  memcpy(&out, data, sizeof(wireless_protocol::MotorPayload));

  out.left_speed =
      std::clamp(out.left_speed, (int16_t)-wireless_protocol::LIMIT_SPEED,
                 (int16_t)wireless_protocol::LIMIT_SPEED);

  out.right_speed =
      std::clamp(out.right_speed, (int16_t)-wireless_protocol::LIMIT_SPEED,
                 (int16_t)wireless_protocol::LIMIT_SPEED);

  return true;
}

} // namespace command
