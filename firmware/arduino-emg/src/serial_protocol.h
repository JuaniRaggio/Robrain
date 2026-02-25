#pragma once

#include "protocol/serial_packet.h"
#include <emg.h>
#include <stdint.h>

namespace serial_proto {

struct arduinoPacket : Packet {
  template <typename Filler>
  arduinoPacket &fill_payload(Filler filler_function);
};

template <typename Filler>
arduinoPacket &arduinoPacket::fill_payload(Filler filler_function) {
  filler_function(emg::Muscle::LeftBicep, this->payload.leftBicep);
  filler_function(emg::Muscle::RightBicep, this->payload.rightBicep);
  return *this;
}

int8_t send_emg_packet(const emg::Reader &reader);

} // namespace serial_proto
