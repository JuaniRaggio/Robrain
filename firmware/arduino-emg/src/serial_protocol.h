#pragma once

#include "HardwareSerial.h"
#include <emg.h>
#include <stdint.h>

namespace serial_proto {

// START(1) + TYPE(1) + LEN(1) + PAYLOAD(N) + CHECKSUM(1) + END(1)

constexpr uint8_t START_BYTE = 0xAA;
constexpr uint8_t END_BYTE = 0x55;

enum class MessageType : uint8_t {
  emgLeftBicep = static_cast<uint8_t>(emg::Muscle::LeftBicep),
  emgRightBicep = static_cast<uint8_t>(emg::Muscle::RightBicep),
  STATUS = 0x10,
};

void send_emg_raw(emg::Reader reader) {
  Serial.write(
      static_cast<uint8_t *>(reader.get(
          emg::Muscle::LeftBicep, reader.get_count(emg::Muscle::LeftBicep))),
      reader.get_count(emg::Muscle::LeftBicep));
}

void send_muscle_event(uint8_t channel, bool active);

} // namespace serial_proto
