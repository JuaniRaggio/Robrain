#pragma once

#include "protocol/serial_packet.h"
#include "serial/scsp.h"
#include <atomic>
#include <cstdint>
#include <functional>
#include <thread>
#include <vector>

namespace robrain {

enum class UserIntent : uint8_t {
  NONE,
  LEFT_WHEEL_FORWARDS,
  RIGHT_WHEEL_FORWARDS,
  BOTH_WHEEL_FORWARDS,
};

struct ProcessingResult {
  UserIntent intent;
  float confidence;
  int16_t left_speed;
  int16_t right_speed;
};

// Configuracion del procesador
struct ProcessorConfig {
  uint16_t activation_threshold;
  uint16_t window_size_ms;
  float smoothing_factor;
};

// TODO SignalProcessor will be improved, first versions
// will be naive

class SignalProcessor {
private:
  static constexpr uint_fast16_t queue_capacity = 256;
  static constexpr uint_fast8_t window_size = 20;
  struct Thresholds {
    uint_fast16_t min_value;
    uint_fast16_t max_value;

    Thresholds() : min_value(0), max_value(1023){};
  };

  serial::Consumer<serial_proto::Payload, queue_capacity> consumable_;
  std::thread processor_thread_;
  std::atomic_bool running_;
  std::atomic_bool calibration_state{false};
  Thresholds thresholds{};

  uint_fast64_t mean_half();
public:
  SignalProcessor(
      serial::Consumer<serial_proto::Payload, queue_capacity> &consumable);
  ~SignalProcessor();

  void start_async();
  void stop_async();

  // Configura el procesador
  void configure(const ProcessorConfig &config);

  void process_samples();

  ProcessingResult get_result() const;

  void calibrate();
  bool is_calibrating() const;

  Thresholds get_calibrated_thresholds() const;
};

} // namespace robrain
