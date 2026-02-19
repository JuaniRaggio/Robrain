// TODO: Implementar
#include "arduino_comm.h"

robrain::ArduinoComm::ArduinoComm() : io{}, port{io, device} {
  port.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
  port.set_option(boost::asio::serial_port_base::character_size(default_char_size));
  port.set_option(boost::asio::serial_port_base::parity(
      boost::asio::serial_port_base::parity::none));
  port.set_option(boost::asio::serial_port_base::stop_bits(
      boost::asio::serial_port_base::stop_bits::one));
};
