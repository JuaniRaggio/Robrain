#pragma once

#include "protocol/serial_packet.h"
#include "protocol/wireless_packet.h"
#include "serial/scsp.h"
#include <atomic>
#include <cstdint>
#include <thread>

namespace robrain {

class SignalProcessor {
private:
  static constexpr uint_fast16_t queue_capacity = 256;
  static constexpr size_t samples_count =
      serial_proto::samples_per_channel; // 32

  struct Thresholds {
    uint_fast16_t min_value;
    uint_fast16_t max_value;

    Thresholds() : min_value(0), max_value(1023) {}; // Default to 10-bit range
  };

  serial::Consumer<serial_proto::Payload, queue_capacity> consumable_;
  serial::Producer<wireless_protocol::MotorPayload, queue_capacity>
      processed_container_;
  std::thread processor_thread_;
  std::atomic_bool running_{false};
  std::atomic_bool calibration_state_{false};
  Thresholds thresholds_{};

  uint_fast16_t trimmed_mean(const uint8_t (&data)[serial_proto::single_muscle_payload_size]);
  void record_level(uint_fast8_t seconds, uint_fast16_t &output_recorded);

public:
  SignalProcessor(
      serial::Consumer<serial_proto::Payload, queue_capacity> &consumable,
      serial::Producer<wireless_protocol::MotorPayload, queue_capacity>
          &processed_container);
  ~SignalProcessor();

  void start_async();
  void stop_async();

  void process_samples();

  void record_rest_level(uint_fast8_t seconds);
  void record_max_level(uint_fast8_t seconds);
  
  bool is_calibrating() const;

  Thresholds get_calibrated_thresholds() const;
};

} // namespace robrain
