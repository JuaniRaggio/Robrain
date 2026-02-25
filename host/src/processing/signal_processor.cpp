#include "signal_processor.h"
#include "serial/scsp.h"

robrain::SignalProcessor::SignalProcessor(
    serial::Consumer<serial_proto::Payload, queue_capacity> &consumer)
    : consumable_{consumer} {}

void robrain::SignalProcessor::start_async() {
  running_ = true;
  processor_thread_{};
}
