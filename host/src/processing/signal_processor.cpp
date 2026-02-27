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
SignalProcessor::trimmed_mean(const uint8_t (&data)[samples_per_channel]) {
  std::array<uint8_t, samples_per_channel> sorted;
  std::copy(std::begin(data), std::end(data), sorted.begin());
  std::sort(sorted.begin(), sorted.end());

  static constexpr double trim_ratio = 0.2;
  static constexpr size_t trim =
      static_cast<size_t>(samples_per_channel * trim_ratio);
  static constexpr size_t count = samples_per_channel - 2 * trim;

  uint_fast32_t sum = std::accumulate(sorted.begin() + trim,
                                      sorted.end() - trim, uint_fast32_t{0});
  return static_cast<uint_fast16_t>(sum / count);
}

void SignalProcessor::process_samples() {
  serial_proto::Payload data;
  if (!consumable_.pop(data)) {
    return;
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
}

void SignalProcessor::start_async() {
  running_ = true;
  processor_thread_ = std::thread([this]() {
    while (running_.load()) {
      process_samples();
    }
  });
}

void SignalProcessor::stop_async() {
  running_ = false;
  if (processor_thread_.joinable()) {
    processor_thread_.join();
  }
}

void SignalProcessor::calibrate() {
  calibration_state_ = true;
  std::cout << "=== Calibration ===" << std::endl;

  using namespace std::chrono;
  static constexpr uint_fast8_t calibration_time_s = 2;
  auto duration = seconds(calibration_time_s);

  std::cout << "Relax muscles" << std::endl;
  uint_fast32_t min_sum = 0;
  uint_fast32_t min_count = 0;
  auto start = steady_clock::now();
  while (steady_clock::now() - start < duration) {
    serial_proto::Payload data;
    if (!consumable_.pop(data)) {
      continue;
    }
    min_sum += trimmed_mean(data.leftBicep);
    min_sum += trimmed_mean(data.rightBicep);
    min_count += 2;
  }
  if (min_count > 0) {
    thresholds_.min_value = min_sum / min_count;
  }

  std::cout << "Push muscles" << std::endl;
  uint_fast32_t max_sum = 0;
  uint_fast32_t max_count = 0;
  start = steady_clock::now();
  while (steady_clock::now() - start < duration) {
    serial_proto::Payload data;
    if (!consumable_.pop(data)) {
      continue;
    }
    max_sum += trimmed_mean(data.leftBicep);
    max_sum += trimmed_mean(data.rightBicep);
    max_count += 2;
  }
  if (max_count > 0) {
    thresholds_.max_value = max_sum / max_count;
  }

  calibration_state_ = false;
}

bool SignalProcessor::is_calibrating() const {
  return calibration_state_;
}

SignalProcessor::Thresholds SignalProcessor::get_calibrated_thresholds() const {
  return thresholds_;
}

} // namespace robrain
