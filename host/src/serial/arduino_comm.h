#pragma once

#include "../../../common/protocol/serial_packet.h"
#include "scsp.h"
#include <array>
#include <atomic>
#include <boost/lockfree/spsc_queue.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

namespace serial {

struct EmgData {
  uint32_t timestamp_ms;
  uint16_t channels[2];
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
  Producer<uint16_t, default_producer_capacity>& producer_;
  std::array<uint8_t, buffer_capacity> buffer_;
  std::atomic_bool running_;
  std::thread producer_thread_;

  inline void parse_byte(uint8_t);

public:
  using EmgCallback = std::function<void(const EmgData &)>;

  ArduinoComm(Producer<uint16_t, default_producer_capacity>& producer, const std::string& device_path, uint32_t baudrate);
  ArduinoComm(Producer<uint16_t, default_producer_capacity>& producer);

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
