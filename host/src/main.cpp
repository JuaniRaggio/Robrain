#include "processing/signal_processor.h"
#include "protocol/wireless_packet.h"
#include "serial/arduino_comm.h"
#include "wireless/ble_client.h"
#include <boost/lockfree/spsc_queue.hpp>
#include <cstdint>

constexpr uint_fast16_t queue_optimal_capacity = 256;

int main() {
  boost::lockfree::spsc_queue<serial_proto::Payload,
                              boost::lockfree::capacity<queue_optimal_capacity>>
      raw_to_producer_queue;
  serial::Producer<serial_proto::Payload, queue_optimal_capacity> raw_producer(
      raw_to_producer_queue);
  serial::Consumer<serial_proto::Payload, queue_optimal_capacity> raw_consumer(
      raw_to_producer_queue);

  boost::lockfree::spsc_queue<wireless_protocol::MotorPayload,
                              boost::lockfree::capacity<queue_optimal_capacity>>
      processed_data;
  serial::Producer<wireless_protocol::MotorPayload, queue_optimal_capacity>
      processed_producer(processed_data);
  serial::Consumer<wireless_protocol::MotorPayload, queue_optimal_capacity>
      processed_consumer(processed_data);

  serial::ArduinoComm comm{raw_producer};
  robrain::SignalProcessor processor{raw_consumer, processed_producer};
  robrain::BleClient ble_client{processed_consumer};

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
