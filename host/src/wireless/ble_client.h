#pragma once

#include <cstdint>
#include "serial/scsp.h"
#include "protocol/wireless_packet.h"

namespace robrain {

class BleClient {
public:
    static constexpr uint_fast16_t queue_capacity = 256;

    explicit BleClient(
        serial::Consumer<wireless_protocol::MotorPayload, queue_capacity>& consumer);
    ~BleClient();

    // Opaque implementation -- defined in ble_client.mm
    struct Impl;

    void start_async();
    void stop_async();
    bool is_connected() const;

private:
    Impl* impl_;
};

} // namespace robrain
