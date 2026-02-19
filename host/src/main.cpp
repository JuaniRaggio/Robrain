#include "processing/signal_processor.h"
#include "serial/arduino_comm.h"
#include "wireless/ble_client.h"
#include <thread>

int main() {
  boost::lockfree::spsc_queue<serial_proto::Payload,
                              boost::lockfree::capacity<4064>>
      queue;

  serial::Producer<serial_proto::Payload, 4064> producer(queue);
  serial::Consumer<serial_proto::Payload, 4064> consumer(queue);

  serial::ArduinoComm comm(producer);
  comm.start_async(); // lanza el hilo, retorna inmediato

  // aca tu logica principal: consumer.pop() para leer datos
  while (true) {
    serial_proto::Payload data;
    if (consumer.pop(data)) {
      // procesar
    }
  }

  comm.stop_async();
  return 0;
}
