#pragma once

#include <emg.h>
#include <stdint.h>

namespace serial_proto {

constexpr const uint8_t SUCCESS = 0x00;
constexpr const uint8_t START_BYTE = 0xAA;
constexpr const uint8_t END_BYTE = 0x55;

// This fixes the magic number problem but makes so that our protocol depends on
// emg::Reaader, thats the tradeoff
constexpr const uint8_t max_payload_size = emg::Reader::total_stream_size;
constexpr const uint8_t single_muscle_payload_size = max_payload_size / 2;

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
  Packet &compute_checksum();
  template <typename Filler> Packet &fill_payload(Filler filler_function);
  size_t send() const;
} __attribute__((packed));

template <typename Filler>
Packet &Packet::fill_payload(Filler filler_function) {
  filler_function(emg::Muscle::LeftBicep, this->payload.leftBicep);
  filler_function(emg::Muscle::RightBicep, this->payload.rightBicep);
  return *this;
}

enum class MessageType : uint8_t {
  emgLeftBicep = static_cast<uint8_t>(emg::Muscle::LeftBicep),
  emgRightBicep = static_cast<uint8_t>(emg::Muscle::RightBicep),
  emgAll = 0x11,
  STATUS = 0x10,
};

int8_t send_emg_packet(const emg::Reader &reader);

} // namespace serial_proto
