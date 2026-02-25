#include "processing/signal_processor.h"
#include "serial/arduino_comm.h"
#include "wireless/ble_client.h"
#include <boost/lockfree/spsc_queue.hpp>
#include <cstdint>

constexpr uint_fast16_t queue_optimal_capacity = 256;

int main() {
  boost::lockfree::spsc_queue<serial_proto::Payload,
                              boost::lockfree::capacity<queue_optimal_capacity>>
      queue;
  serial::Producer<serial_proto::Payload, queue_optimal_capacity> raw_producer(queue);
  serial::Consumer<serial_proto::Payload, queue_optimal_capacity> raw_consumer(queue);

  serial::ArduinoComm comm{raw_producer};
  robrain::SignalProcessor processor{raw_consumer};
  robrain::BleClient ble_client{};

  // TODO Ask esp32 if ready

  comm.start_async();
  processor.start_async();
  ble_client.start_async();

  // Consultar estado del robot, en caso de que
  // desee apagarse, cortar el ciclo y hacer el join de los
  // threads
  while (true) {

  }

  comm.stop_async();
  processor.stop_async();
  ble_client.stop_async();
  return 0;
}
