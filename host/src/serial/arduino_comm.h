#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <vector>

namespace robrain {

struct EmgData {
    uint32_t timestamp_ms;
    std::vector<uint16_t> channels;
};

class ArduinoComm {
public:
    using EmgCallback = std::function<void(const EmgData&)>;

    ArduinoComm();
    ~ArduinoComm();

    // Conecta al puerto serial
    bool connect(const std::string& port, uint32_t baudrate = 115200);

    // Desconecta
    void disconnect();

    // Verifica si esta conectado
    bool is_connected() const;

    // Registra callback para datos EMG
    void set_emg_callback(EmgCallback cb);

    // Procesa datos entrantes (llamar periodicamente o en hilo separado)
    void update();

    // Inicia hilo de lectura en background
    void start_async();

    // Detiene hilo de lectura
    void stop_async();

    // Envia comando de configuracion
    bool set_sample_rate(uint16_t hz);
    bool set_threshold(uint8_t channel, uint16_t threshold);
    bool send_start();
    bool send_stop();

private:
    struct Impl;
    Impl* pimpl;  // Pimpl pattern para ocultar detalles de implementacion
};

} // namespace robrain
