#include <serial_protocol.h>
#include <HardwareSerial.h>

int8_t send_emg_packet(emg::Reader reader) {
  serial_proto::Packet packet = (serial_proto::Packet){
      .b_start = serial_proto::START_BYTE,
      .type = 0,
      .b_size = sizeof(serial_proto::Payload),
      .b_end = serial_proto::END_BYTE,
  };
  reader.get_data(emg::Muscle::LeftBicep, packet.payload.leftBicep);
  reader.get_data(emg::Muscle::RightBicep, packet.payload.rightBicep);
  Serial.write(reinterpret_cast<uint8_t *>(&packet),
               sizeof(serial_proto::Packet));
  return SUCCESS;
}

