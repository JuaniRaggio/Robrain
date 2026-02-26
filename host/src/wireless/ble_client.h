#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <simpleble/SimpleBLE.h>

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

    auto adapter = SimpleBLE::Adapter::get_adapters()[0];
    SimpleBLE::Peripheral target;
    bool found = false;

    adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral peripheral) {
        if (peripheral.identifier() == DEVICE_NAME) {
            target = peripheral;
            found = true;
            adapter.scan_stop();
        }
    });

    BleClient();
    ~BleClient();

    // Escanea dispositivos BLE
    // Filtra por nombre si se proporciona
    std::vector<BleDevice> scan(uint32_t timeout_ms = 5000,
                                 const std::string& name_filter = "");

    bool connect(const std::string& device);
    void disconnect();
    bool is_connected() const;
    bool send_motor_command(int16_t left_speed, int16_t right_speed);
    bool send_stop();
    void update();

    // This functions are not yet supported
    void set_status_callback(StatusCallback cb);
    void set_connect_callback(ConnectCallback cb);
    RobotStatus get_last_status() const;

    private:

};

} // namespace robrain
