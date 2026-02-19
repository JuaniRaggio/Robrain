#pragma once

#include <emg.h>
#include <stdint.h>

namespace serial_proto {

#define SUCCESS 0

constexpr const uint8_t START_BYTE = 0xAA;
constexpr const uint8_t END_BYTE = 0x55;

// TODO desacoplar
constexpr const uint8_t single_muscle_payload_size = 32 * 2;
constexpr const uint8_t max_payload_size = single_muscle_payload_size * 2;

struct Payload {
  uint8_t leftBicep[single_muscle_payload_size];
  uint8_t rightBicep[single_muscle_payload_size];

  uint8_t get_sum() const;
} __attribute__((packed));

// START(1) + TYPE(1) + LEN(1) + PAYLOAD(N) + CHECKSUM(1) + END(1)
struct Packet {
  const uint8_t b_start;
  const uint8_t type;
  const uint8_t b_size;
  Payload payload;
  uint8_t check_sum;
  const uint8_t b_end;

  Packet();
  Packet *const compute_checksum();
  template <typename Filler> Packet *const fill_payload(Filler filler_function);
  size_t send() const;
} __attribute__((packed));

template <typename Filler>
Packet *const Packet::fill_payload(Filler filler_function) {
  filler_function(emg::Muscle::LeftBicep, this->payload.leftBicep);
  filler_function(emg::Muscle::RightBicep, this->payload.rightBicep);
  return this;
}

enum class MessageType : uint8_t {
  emgLeftBicep = static_cast<uint8_t>(emg::Muscle::LeftBicep),
  emgRightBicep = static_cast<uint8_t>(emg::Muscle::RightBicep),
  emgAll = 0x11,
  STATUS = 0x10,
};

int8_t send_emg_packet(const emg::Reader &reader);

} // namespace serial_proto
