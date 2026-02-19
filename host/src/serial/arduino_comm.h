#pragma once

#include "../../../common/protocol/serial_packet.h"
#include <atomic>
#include <boost/lockfree/spsc_queue.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include <boost/asio.hpp>

// This implementation of serial client protocol
// uses single consumer, single producer pattern
namespace serial {

struct EmgData {
  uint32_t timestamp_ms;
  uint16_t channels[2];
};

template <typename T, size_t Capacity> class Producer {
private:
  boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Capacity>> &queue_;

public:
  explicit Producer(
      boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Capacity>>
          &queue)
      : queue_{queue} {};

  bool push(const T &data) { return queue_.push(data); }
};

template <typename T, size_t Capacity> class Consumer {
private:
  boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Capacity>> &queue_;

public:
  explicit Consumer(
      boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Capacity>>
          &queue)
      : queue_{queue} {};

  bool pop(const T &data) { return queue_.pop(data); }
};

class ArduinoComm {
private:
#if defined(_WIN32)
#define device_name "COM3"
#elif defined(__APPLE__)
#define device_name "/dev/cu.usbmodem14101"
#elif defined(__linux__)
#define device_name "/dev/ttyUSB0"
#endif

  constexpr const static char *device{device_name};
  constexpr const static uint32_t baudrate{1'000'000};
  constexpr const static uint8_t default_char_size{8};
  boost::asio::io_context io;
  boost::asio::serial_port port;

  struct Impl;
  Impl *pimpl; // Pimpl pattern para ocultar detalles de implementacion

public:
  using EmgCallback = std::function<void(const EmgData &)>;

  ArduinoComm();
  ~ArduinoComm();

  // Conecta al puerto serial
  bool connect(const std::string &port, uint32_t baudrate = 1'000'000);

  // Desconecta
  void disconnect();

  // Verifica si esta conectado
  bool is_connected() const;

  // Registra callback para datos EMG
  void set_emg_callback(EmgCallback cb);

  // Procesa datos entrantes (llamar periodicamente o en hilo separado)
  void update();

  // Inicia hilo de lectura en background
  void start_async();

  // Detiene hilo de lectura
  void stop_async();

  // Envia comando de configuracion
  bool set_sample_rate(uint16_t hz);
  bool set_threshold(uint8_t channel, uint16_t threshold);
  bool send_start();
  bool send_stop();
};

} // namespace serial
