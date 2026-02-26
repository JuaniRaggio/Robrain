#include "message.h"
#include "protocol/wireless_packet.h"

namespace robrain {

wireless_protocol::MotorPayload make_motor_command(int16_t left, int16_t right) {
  return wireless_protocol::MotorPayload(left, right);
}

wireless_protocol::MotorPayload make_stop_command() {
  return wireless_protocol::MotorPayload();
}

}
