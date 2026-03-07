#include "signal_processor.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>

namespace robrain {

SignalProcessor::SignalProcessor(
    serial::Consumer<serial_proto::Payload, queue_capacity> &consumer,
    serial::Producer<wireless_protocol::MotorPayload, queue_capacity>
        &processed_container)
    : consumable_{consumer}, processed_container_{processed_container} {}

SignalProcessor::~SignalProcessor() {
  stop_async();
}

uint_fast16_t
SignalProcessor::trimmed_mean(const uint8_t (&data)[serial_proto::single_muscle_payload_size]) {
  std::array<uint16_t, samples_count> sorted;
  std::memcpy(sorted.data(), data, sizeof(sorted));
  std::sort(sorted.begin(), sorted.end());

  static constexpr double trim_ratio = 0.2;
  static constexpr size_t trim =
      static_cast<size_t>(samples_count * trim_ratio);
  static constexpr size_t count = samples_count - 2 * trim;

  uint_fast32_t sum = std::accumulate(sorted.begin() + trim,
                                      sorted.end() - trim, uint_fast32_t{0});
  return static_cast<uint_fast16_t>(sum / count);
}

bool SignalProcessor::process_samples() {
  serial_proto::Payload data;
  if (!consumable_.pop(data)) {
    return false;
  }

  uint_fast16_t left_mean = trimmed_mean(data.leftBicep);
  uint_fast16_t right_mean = trimmed_mean(data.rightBicep);

  float range = static_cast<float>(thresholds_.max_value) -
                static_cast<float>(thresholds_.min_value);
  if (range <= 0.0f) {
    range = 1.0f;
  }

  int left = static_cast<int>(
      (static_cast<float>(left_mean) - thresholds_.min_value) / range * 100.0f);
  int right = static_cast<int>(
      (static_cast<float>(right_mean) - thresholds_.min_value) / range *
      100.0f);

  processed_container_.push(wireless_protocol::MotorPayload(
      static_cast<int16_t>(std::clamp(left, 0, 100)),
      static_cast<int16_t>(std::clamp(right, 0, 100))));
  return true;
}

void SignalProcessor::start_async() {
  running_ = true;
  processor_thread_ = std::thread([this]() {
    while (running_.load()) {
      if (calibration_state_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
      }
      if (!process_samples()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    }
  });
}

void SignalProcessor::stop_async() {
  running_ = false;
  if (processor_thread_.joinable()) {
    processor_thread_.join();
  }
}

void SignalProcessor::record_level(uint_fast8_t seconds, uint_fast16_t &output_recorded) {
  calibration_state_ = true;

  // Clear stale data from queue to ensure real-time calibration
  serial_proto::Payload dummy;
  while (consumable_.pop(dummy));

  using namespace std::chrono;
  auto duration = std::chrono::seconds(seconds);

  uint_fast32_t sum = 0;
  uint_fast32_t count = 0;
  auto start = steady_clock::now();
  while (steady_clock::now() - start < duration) {
    serial_proto::Payload data;
    if (!consumable_.pop(data)) {
      continue;
    }
    sum += trimmed_mean(data.leftBicep);
    sum += trimmed_mean(data.rightBicep);
    count += 2;
  }
  if (count > 0) {
    output_recorded = static_cast<uint_fast16_t>(sum / count);
  }
  calibration_state_ = false;

}

void SignalProcessor::record_rest_level(uint_fast8_t seconds) {
  record_level(seconds, thresholds_.min_value);
}

void SignalProcessor::record_max_level(uint_fast8_t seconds) {
  record_level(seconds, thresholds_.max_value);
}

bool SignalProcessor::is_calibrating() const {
  return calibration_state_;
}

SignalProcessor::Thresholds SignalProcessor::get_calibrated_thresholds() const {
  return thresholds_;
}

} // namespace robrain
