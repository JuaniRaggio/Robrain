#pragma once
#include <stdint.h>

namespace serial_proto {
// Samples per channel - must match emg::Reader::ChannelReader::stream_size
constexpr const uint8_t samples_per_channel = 32;
constexpr const uint8_t max_payload_size = samples_per_channel * 2;
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
  size_t send() const;
} __attribute__((packed));

} // namespace serial_proto
