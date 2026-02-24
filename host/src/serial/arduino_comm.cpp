#include "arduino_comm.h"
#include "protocol/serial_packet.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <thread>

// === Parser ===

serial::Parser::Parser()
    : state_{make_default_state()}, current_payload{}, offset{0}, size{serial_proto::max_payload_size} {}

void serial::Parser::parse_byte(uint8_t byte) {
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
    handle_complete(byte);
    break;
  default:
    state_ = make_default_state();
    offset = 0;
    break;
  }
}

void serial::Parser::handle_waiting_header(uint8_t byte) {
  state_ = byte == serial_proto::START_BYTE ? ParseState::type_reading
                                            : ParseState::header_waiting;
}

void serial::Parser::handle_type(uint8_t byte) {
  state_ = byte == static_cast<uint8_t>(serial_proto::MessageType::emgAll)
               ? ParseState::length_reading
               : ParseState::error;
}

// We currently support default length only
void serial::Parser::handle_length(uint8_t byte) {
  // size = byte; //
  state_ = byte == serial_proto::max_payload_size ? ParseState::payload_reading
                                                  : ParseState::error;
}

void serial::Parser::handle_payload(uint8_t byte) {
  if (offset == size) {
    state_ = ParseState::checksum_reading;
    return;
  }
  reinterpret_cast<uint8_t *>(&current_payload)[offset++] = byte;
}

void serial::Parser::handle_checksum(uint8_t byte) {
  state_ = byte == current_payload.get_sum() ? ParseState::end_waiting
                                             : ParseState::error;
}

void serial::Parser::handle_waiting_end(uint8_t byte) {
  state_ =
      byte == serial_proto::END_BYTE ? ParseState::complete : ParseState::error;
}

void serial::Parser::handle_complete(uint8_t byte) {}

bool serial::Parser::is_current_state(ParseState state) const {
  return state_ == state;
}

serial::Parser::ParseState serial::Parser::make_default_state() {
  return ParseState::header_waiting;
}

serial_proto::Payload serial::Parser::pop() {
  if (state_ != ParseState::complete) {
    // TODO Handle error
    std::exit(static_cast<int>(ParseState::error));
  }
  state_ = make_default_state();
  return current_payload;
}

// === ArduinoComm ===

serial::ArduinoComm::ArduinoComm(
    Producer<serial_proto::Payload, default_producer_capacity> &producer,
    const std::string &device_path, uint32_t baudrate)
    : baudrate_{baudrate}, device_{device_path}, io{}, port{io, device_path},
      producer_{producer}, buffer_{}, running_{false}, reader_thread_{},
      current_{}, parser_{} {

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

void serial::ArduinoComm::update() {}

void serial::ArduinoComm::read() {
  port.async_read_some(
      boost::asio::buffer(buffer_),
      [this](boost::system::error_code ec, size_t bytes) {
        if (ec) return;
        for (size_t i = 0; i < bytes; ++i) {
          parser_.parse_byte(buffer_[i]);
        }
        if (parser_.is_current_state(Parser::ParseState::complete)) {
          producer_.push(parser_.pop());
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
