#include "signal_processor.h"
#include "serial/scsp.h"

robrain::SignalProcessor::SignalProcessor(
    serial::Consumer<serial_proto::Payload, queue_capacity> &consumer)
    : consumable_{consumer} {}

void robrain::SignalProcessor::start_async() {
  running_ = true;
  processor_thread_ = std::thread{};
}

void robrain::SignalProcessor::stop_async() {
  running_ = false;
  if (processor_thread_.joinable()) {
    processor_thread_.join();
  }
}
