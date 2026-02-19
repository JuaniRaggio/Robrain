// TODO: Implementar
#include "arduino_comm.h"
#include <cstdint>

serial::ArduinoComm::ArduinoComm(
    Producer<uint16_t, default_producer_capacity> &producer,
    const std::string &device_path, uint32_t baudrate)
    : io{}, port{io, device_path}, producer_{producer} {
  port.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
  port.set_option(
      boost::asio::serial_port_base::character_size(default_char_size));
  port.set_option(boost::asio::serial_port_base::parity(
      boost::asio::serial_port_base::parity::none));
  port.set_option(boost::asio::serial_port_base::stop_bits(
      boost::asio::serial_port_base::stop_bits::one));
}

serial::ArduinoComm::ArduinoComm(
    Producer<uint16_t, default_producer_capacity> &producer)
    : ArduinoComm(producer, device_, default_baudrate) {}

serial::ArduinoComm::~ArduinoComm() {
  disconnect();
}

void serial::ArduinoComm::disconnect() {
  if (is_connected()) port.close();
}

bool serial::ArduinoComm::is_connected() const {
  return port.is_open();
}
