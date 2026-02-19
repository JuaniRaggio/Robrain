#include "emg.h"
#include <HardwareSerial.h>
#include <serial_protocol.h>

uint8_t serial_proto::Payload::get_sum() const {
  uint8_t result = 0;
  const uint8_t *payload_stream = reinterpret_cast<const uint8_t *>(this);
  for (size_t i = 0; i < sizeof(serial_proto::Payload); ++i) {
    result ^= payload_stream[i];
  }
  return result;
}

serial_proto::Packet::Packet()
    : b_start{START_BYTE}, type{static_cast<uint8_t>(MessageType::emgAll)},
      b_size{sizeof(Payload)}, b_end{END_BYTE} {}

serial_proto::Packet *const serial_proto::Packet::compute_checksum() {
  check_sum = payload.get_sum();
  return this;
}

size_t serial_proto::Packet::send() const {
  return Serial.write(reinterpret_cast<const uint8_t *>(this), sizeof(Packet));
}

int8_t serial_proto::send_emg_packet(const emg::Reader &reader) {
  static Packet packet;
  packet
      .fill_payload([&reader](auto muscle, auto &buffer) {
        reader.get_data(muscle, buffer);
      })
      ->compute_checksum()
      ->send();
  return SUCCESS;
}
