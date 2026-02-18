#include <HardwareSerial.h>
#include <serial_protocol.h>

int8_t serial_proto::send_emg_packet(emg::Reader reader) {
  static Packet packet = (Packet){
      .b_start = START_BYTE,
      .type = static_cast<uint8_t>(MessageType::emgAll),
      .b_size = sizeof(Payload),
      .b_end = END_BYTE,
  };
  reader.get_data(emg::Muscle::LeftBicep, packet.payload.leftBicep);
  reader.get_data(emg::Muscle::RightBicep, packet.payload.rightBicep);
  Serial.write(reinterpret_cast<uint8_t *>(&packet), sizeof(Packet));
  return SUCCESS;
}
