// TODO: Implementar
#include "arduino_comm.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <thread>

serial::ArduinoComm::ArduinoComm(
    Producer<serial_proto::Payload, default_producer_capacity> &producer,
    const std::string &device_path, uint32_t baudrate)
    : io{}, port{io, device_path}, producer_{producer}, running_{false} {
  port.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
  port.set_option(
      boost::asio::serial_port_base::character_size(default_char_size));
  port.set_option(boost::asio::serial_port_base::parity(
      boost::asio::serial_port_base::parity::none));
  port.set_option(boost::asio::serial_port_base::stop_bits(
      boost::asio::serial_port_base::stop_bits::one));
}

serial::ArduinoComm::~ArduinoComm() {
  disconnect();
}

void serial::ArduinoComm::disconnect() {
  if (is_connected()) port.close();
}

bool serial::ArduinoComm::is_connected() const {
  return port.is_open();
}

void serial::ArduinoComm::parse_byte(uint8_t byte) {}

void serial::ArduinoComm::update() {}

void serial::ArduinoComm::read() {
  port.async_read_some(boost::asio::buffer(buffer_),
                       [this](boost::system::error_code ec, size_t bytes) {
                         if (ec) return;
                         for (size_t i = 0; i < bytes; ++i)
                           parse_byte(buffer_[i]);
                         read();
                       });
}

void serial::ArduinoComm::start_async() {
  running_ = true;
  read();
  producer_thread_ = std::thread([this] { io.run(); });
}

void serial::ArduinoComm::stop_async() {
  running_ = false;
  if (producer_thread_.joinable()) producer_thread_.join();
}
