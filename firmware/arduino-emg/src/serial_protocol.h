#pragma once

#include "../../../common/protocol/serial_packet.h"
#include <emg.h>
#include <stdint.h>

namespace serial_proto {

constexpr const uint8_t SUCCESS = 0x00;
constexpr const uint8_t START_BYTE = 0xAA;
constexpr const uint8_t END_BYTE = 0x55;

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

enum class MessageType : uint8_t {
  emgLeftBicep = static_cast<uint8_t>(emg::Muscle::LeftBicep),
  emgRightBicep = static_cast<uint8_t>(emg::Muscle::RightBicep),
  emgAll = static_cast<uint8_t>(emg::Muscle::COUNT),
};

int8_t send_emg_packet(const emg::Reader &reader);

} // namespace serial_proto
