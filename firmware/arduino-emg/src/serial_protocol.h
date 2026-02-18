#pragma once

#include "HardwareSerial.h"
#include <emg.h>
#include <stdint.h>

namespace serial_proto {

#define SUCCESS 0

constexpr const uint8_t START_BYTE = 0xAA;
constexpr const uint8_t END_BYTE = 0x55;
constexpr const uint8_t single_muscle_payload_size = 32 * 2;
constexpr const uint8_t max_payload_size = single_muscle_payload_size * 2;

struct Payload {
  uint8_t leftBicep[single_muscle_payload_size];
  uint8_t rightBicep[single_muscle_payload_size];
} __attribute__((packed));

// TODO Agregarle metodos al packet para que esten linkeados al mismo,
// como default initializers para que se haga el checksum automaticamente,
// setter para el payload, etc.
struct Packet {
  uint8_t b_start;
  uint8_t type;
  uint8_t b_size;
  Payload payload;
  uint8_t check_sum;
  uint8_t b_end;
} __attribute__((packed));

// START(1) + TYPE(1) + LEN(1) + PAYLOAD(N) + CHECKSUM(1) + END(1)

enum class MessageType : uint8_t {
  emgLeftBicep = static_cast<uint8_t>(emg::Muscle::LeftBicep),
  emgRightBicep = static_cast<uint8_t>(emg::Muscle::RightBicep),
  emgAll = 0x11,
  STATUS = 0x10,
};

int8_t send_emg_packet(emg::Reader reader);

} // namespace serial_proto
