#include "signal_processor.h"
#include "serial/scsp.h"

robrain::SignalProcessor::SignalProcessor(
    serial::Consumer<serial_proto::Payload, 4064> &consumer)
    : consumable_{consumer} {}

void robrain::SignalProcessor::start_async() {
  running_ = true;
  processor_thread_{};
}
