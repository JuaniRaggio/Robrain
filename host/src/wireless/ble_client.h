#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace robrain {

struct BleDevice {
    std::string name;
    std::string address;
    int rssi;  // Intensidad de senal
};

struct RobotStatus {
    bool connected;
    int16_t left_speed;
    int16_t right_speed;
    uint8_t state;
    uint32_t last_update_ms;
};

class BleClient {
public:
    using StatusCallback = std::function<void(const RobotStatus&)>;
    using ConnectCallback = std::function<void(bool connected)>;

    BleClient();
    ~BleClient();

    // Escanea dispositivos BLE
    // Filtra por nombre si se proporciona
    std::vector<BleDevice> scan(uint32_t timeout_ms = 5000,
                                 const std::string& name_filter = "");

    // Conecta a dispositivo por nombre o direccion
    bool connect(const std::string& device);

    // Desconecta
    void disconnect();

    // Verifica conexion
    bool is_connected() const;

    // Envia comando de motor
    bool send_motor_command(int16_t left_speed, int16_t right_speed);

    // Detiene motores
    bool send_stop();

    // Registra callback para actualizaciones de estado (notify)
    void set_status_callback(StatusCallback cb);

    // Registra callback para cambios de conexion
    void set_connect_callback(ConnectCallback cb);

    // Procesa eventos BLE (llamar periodicamente)
    void update();

    // Ultimo estado recibido
    RobotStatus get_last_status() const;

private:
    struct Impl;
    Impl* pimpl;
};

} // namespace robrain
