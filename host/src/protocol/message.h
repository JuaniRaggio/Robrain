#pragma once

#include "protocol/serial_packet.h"
#include <protocol/wireless_packet.h>
#include <cstdint>
#include <cstddef>

namespace robrain {

wireless_protocol::MotorPayload make_motor_command(int16_t left, int16_t right);

wireless_protocol::MotorPayload make_stop_command();

wireless_protocol::MotorPayload make_status_request();

} // namespace robrain
