#include "arduino_comm.h"
#include "protocol/serial_packet.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <thread>

// === Parser ===

serial::Parser::Parser()
    : state_{make_default_state()}, current_payload{}, offset{0} {}

serial::Parser::ParseState serial::Parser::parse_byte(uint8_t byte) {
  switch (state_) {
  case ParseState::header_waiting:
    handle_waiting_header(byte);
    break;
  case ParseState::type_reading:
    handle_type(byte);
    break;
  case ParseState::length_reading:
    handle_length(byte);
    break;
  case ParseState::payload_reading:
    handle_payload(byte);
    break;
  case ParseState::checksum_reading:
    handle_checksum(byte);
    break;
  case ParseState::end_waiting:
    handle_waiting_end(byte);
    break;
  case ParseState::complete:
    // === Complete state means return state_ === //
    break;
  case ParseState::error:
  default:
    // === ERROR should only restart the parser === //
    state_ = make_default_state();
    offset = 0;
    return parse_byte(byte);
  }
  return state_;
}

void serial::Parser::handle_waiting_header(uint8_t byte) {
  byte == serial_proto::START_BYTE ? set_next_state() : set_error_state();
}

void serial::Parser::handle_type(uint8_t byte) {
  byte == static_cast<uint8_t>(serial_proto::MessageType::emgAll)
      ? set_next_state()
      : set_error_state();
}

// We currently support default length only
void serial::Parser::handle_length(uint8_t byte) {
  // size = byte; //
  byte == serial_proto::max_payload_size ? set_next_state() : set_error_state();
}

void serial::Parser::handle_payload(uint8_t byte) {
  if (offset == size) {
    set_next_state();
    return;
  }
  reinterpret_cast<uint8_t *>(&current_payload)[offset++] = byte;
}

void serial::Parser::handle_checksum(uint8_t byte) {
  byte == current_payload.get_sum() ? set_next_state() : set_error_state();
}

void serial::Parser::handle_waiting_end(uint8_t byte) {
  byte == serial_proto::END_BYTE ? set_next_state() : set_error_state();
}

bool serial::Parser::is_current_state(ParseState state) const {
  return state_ == state;
}

void serial::Parser::set_next_state() {
  state_ = static_cast<ParseState>(static_cast<uint8_t>(state_) + 1);
}

void serial::Parser::set_error_state() {
  state_ = ParseState::error;
}

serial::Parser::ParseState serial::Parser::make_default_state() {
  return ParseState::header_waiting;
}

serial_proto::Payload serial::Parser::pop() {
  state_ = make_default_state();
  return current_payload;
}

// === ArduinoComm ===

serial::ArduinoComm::ArduinoComm(
    Producer<serial_proto::Payload, default_producer_capacity> &producer,
    const std::string &device_path, uint32_t baudrate)
    : baudrate_{baudrate}, device_{device_path}, io{}, port{io, device_path},
      producer_{producer}, buffer_{}, running_{false}, reader_thread_{},
      parser_{} {

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

void serial::ArduinoComm::update() {
  producer_.push(parser_.pop());
}

void serial::ArduinoComm::read() {
  port.async_read_some(boost::asio::buffer(buffer_),
                       [this](boost::system::error_code ec, size_t bytes) {
                         if (ec) return;
                         for (size_t i = 0; i < bytes; ++i) {
                           if (parser_.parse_byte(buffer_[i]) ==
                               Parser::ParseState::complete) {
                             update();
                           }
                         }
                         read();
                       });
}

void serial::ArduinoComm::start_async() {
  running_ = true;
  read();
  reader_thread_ = std::thread([this] { io.run(); });
}

void serial::ArduinoComm::stop_async() {
  running_ = false;
  if (reader_thread_.joinable()) reader_thread_.join();
}
