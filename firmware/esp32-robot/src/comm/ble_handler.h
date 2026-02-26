#pragma once

#include <protocol/wheel_command.h>
#include <functional>

namespace ble_handler {

using ConnectCallback = std::function<void(bool connected)>;

void init(motor::WheelPair &pair);

void set_connect_callback(ConnectCallback cb);

bool is_connected();

} // namespace ble_handler
