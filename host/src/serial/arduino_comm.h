#pragma once

#include "protocol/serial_packet.h"
#include "scsp.h"
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <boost/asio.hpp>

namespace serial {

struct EmgData {
  uint32_t timestamp_ms;
  uint16_t channels[2];
};

class Parser {
public:

  enum class ParseState : uint8_t {
    header_waiting,
    type_reading,
    length_reading,
    payload_reading,
    checksum_reading,
    end_waiting,
    complete,
    error,
  };

  Parser();

  void parse_byte(uint8_t);

  inline void handle_waiting_header(uint8_t);
  inline void handle_type(uint8_t);
  inline void handle_length(uint8_t);
  inline void handle_payload(uint8_t);
  inline void handle_checksum(uint8_t);
  inline void handle_complete(uint8_t);
  inline void handle_waiting_end(uint8_t);

  inline bool is_current_state(ParseState state) const;

  serial_proto::Payload pop();

private:
  ParseState state_;
  serial_proto::Payload current_payload;
  uint8_t offset;
  // === For now we only support fixed-sized payloads ===
  constexpr static uint8_t size = serial_proto::max_payload_size;

  static inline ParseState make_default_state();

};

// template <typename T, size_t Capacity>
class ArduinoComm {
private:
#if defined(_WIN32)
#define device_name "COM3"
#elif defined(__APPLE__)
#define device_name "/dev/cu.usbmodem14101"
#elif defined(__linux__)
#define device_name "/dev/ttyUSB0"
#endif

  constexpr const static uint8_t default_char_size{8};
  constexpr const static size_t default_producer_capacity{4064};
  constexpr const static size_t buffer_capacity{256};
  constexpr const static size_t default_baudrate{1'000'000};

  uint32_t baudrate_;
  const std::string device_;

  boost::asio::io_context io;
  boost::asio::serial_port port;

  Producer<serial_proto::Payload, default_producer_capacity> &producer_;
  std::array<uint8_t, buffer_capacity> buffer_;
  std::atomic_bool running_;
  std::thread reader_thread_;
  serial_proto::Packet current_;
  Parser parser_;

public:
  using EmgCallback = std::function<void(const EmgData &)>;

  ArduinoComm() = delete;
  ArduinoComm(
      Producer<serial_proto::Payload, default_producer_capacity> &producer,
      const std::string &device_path = device_name,
      uint32_t baudrate = default_baudrate);

  ~ArduinoComm();

  void disconnect();
  bool is_connected() const;

  void update();
  void read();
  void start_async();
  void stop_async();

  // TODO permitirle a la computadora enviarle un custom sample_rate
  // al arduino. Esto hace que la PC sea el cerebro del sistema, encargado
  // de sincronizar todo, tanto el rate sample en el que lee el arduino y el
  // rate en el que lee el esp32.
  bool set_sample_rate(uint16_t hz);
  bool set_threshold(uint8_t channel, uint16_t threshold);
  bool send_start();
  bool send_stop();
};

} // namespace serial
