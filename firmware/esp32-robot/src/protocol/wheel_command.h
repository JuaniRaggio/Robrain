#pragma once

#include <protocol/wireless_packet.h>
#include <stddef.h>
#include <stdint.h>

namespace command {

bool parse_motor_payload(const uint8_t *data, size_t len, wireless_protocol::MotorPayload &out);

} // namespace command
