#include "processing/signal_processor.h"
#include "protocol/wireless_packet.h"
#include "serial/arduino_comm.h"
#include "wireless/ble_client.h"
#include <boost/lockfree/spsc_queue.hpp>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <thread>
#include <atomic>

constexpr uint_fast16_t queue_optimal_capacity = 256;

std::atomic<bool> keep_running{true};

void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n[MAIN] Deteniendo sistema..." << std::endl;
        keep_running = false;
    }
}

int main() {
    std::signal(SIGINT, signal_handler);

    std::cout << "=== ROBRAIN HOST SYSTEM ===" << std::endl;

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

    try {
        std::cout << "[MAIN] Inicializando Serial..." << std::endl;
        serial::ArduinoComm comm{raw_producer};
        
        std::cout << "[MAIN] Inicializando Procesador..." << std::endl;
        robrain::SignalProcessor processor{raw_consumer, processed_producer};
        
        std::cout << "[MAIN] Inicializando BLE Client..." << std::endl;
        robrain::BleClient ble_client{processed_consumer};

        std::cout << "[MAIN] Arrancando threads..." << std::endl;
        comm.start_async();
        processor.start_async();
        ble_client.start_async();

        std::cout << "[MAIN] Sistema operativo. Presione Ctrl+C para salir." << std::endl;
        
        // --- CALIBRACION ORQUESTADA ---
        std::cout << "\n=== CALIBRACION ===" << std::endl;
        std::cout << "1. Relaje los musculos..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        processor.record_rest_level(2);
        
        std::cout << "2. ¡Haga fuerza ahora! (Contraccion maxima)..." << std::endl;
        processor.record_max_level(2);
        
        std::cout << "Calibracion terminada. ¡Control activo!\n" << std::endl;

        while (keep_running) {
            bool ble_connected = ble_client.is_connected();
            bool serial_connected = comm.is_connected();

            std::cout << "\r[STATUS] Serial: " << (serial_connected ? "OK" : "ERR")
                      << " | BLE: " << (ble_connected ? "CONECTADO" : "BUSCANDO...") 
                      << std::flush;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        std::cout << "\n[MAIN] Cerrando conexiones..." << std::endl;
        comm.stop_async();
        processor.stop_async();
        ble_client.stop_async();

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Fallo critico: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "[MAIN] Programa terminado." << std::endl;
    return 0;
}
