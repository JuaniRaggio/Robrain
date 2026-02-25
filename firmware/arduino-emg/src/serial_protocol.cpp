#include "emg.h"
#include <HardwareSerial.h>
#include <serial_protocol.h>

serial_proto::Packet::Packet()
    : b_start{START_BYTE}, type{static_cast<uint8_t>(MessageType::emgAll)},
      b_size{sizeof(Payload)}, b_end{END_BYTE} {}

serial_proto::Packet &serial_proto::Packet::compute_checksum() {
  check_sum = payload.get_sum();
  return *this;
}

size_t serial_proto::Packet::send() const {
  return Serial.write(reinterpret_cast<const uint8_t *>(this), sizeof(Packet));
}

int8_t serial_proto::send_emg_packet(const emg::Reader &reader) {
  static arduinoPacket packet;
  return packet
      .fill_payload([&reader](auto muscle, auto &buffer) {
        reader.get_data(muscle, buffer);
      })
      .compute_checksum()
      .send();
}
