#include "signal_processor.h"
#include "protocol/serial_packet.h"
#include "serial/scsp.h"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

namespace robrain {

SignalProcessor::SignalProcessor(
    serial::Consumer<serial_proto::Payload, queue_capacity> &consumer)
    : consumable_{consumer} {}

uint_fast64_t SignalProcessor::mean_half() {
    int temp[window_size];

    // Descartar 20% de cada lado y promediar el centro
    int descartar = window_size / 5;  // 4 de cada lado
    long suma = 0;
    for (int i = descartar; i < window_size - descartar; i++) suma += temp[i];
    return suma / (window_size - 2 * descartar);
}

void SignalProcessor::process_samples() {

}

void SignalProcessor::start_async() {
  running_ = true;
  processor_thread_ = std::thread{};
}

void SignalProcessor::stop_async() {
  running_ = false;
  if (processor_thread_.joinable()) {
    processor_thread_.join();
  }
}

void SignalProcessor::calibrate() {
  calibration_state = true;
  std::cout << "=== Calibration ===" << std::endl;
  static constexpr uint_fast64_t limits_size = 2000;

  std::vector<serial_proto::Payload> tops{limits_size};
  std::vector<serial_proto::Payload> bottoms{limits_size};

  using namespace std::chrono;
  static constexpr uint_fast8_t calibration_time{2};
  auto start = steady_clock::now();
  auto duration = seconds(calibration_time);

  std::cout << "Relax muscles" << std::endl;
  while (steady_clock::now() - start < duration) {
    serial_proto::Payload data;
    consumable_.pop(data);
    bottoms.push_back(data);
  }

  thresholds.min_value = std::accumulate(bottoms.begin(), bottoms.end(), 0) / bottoms.size();

  std::cout << "Push muscles" << std::endl;
  start = steady_clock::now();
  while (steady_clock::now() - start < duration) {
    serial_proto::Payload data;
    consumable_.pop(data);
    tops.push_back(data);
  }

  thresholds.max_value = std::accumulate(tops.begin(), tops.end(), 0) / tops.size();

  calibration_state = false;
}

bool SignalProcessor::is_calibrating() const {
  return calibration_state;
}

SignalProcessor::Thresholds SignalProcessor::get_calibrated_thresholds() const {
  return thresholds;
}

} // namespace robrain
