#pragma once

#include <functional>
#include <protocol/wheel_command.h>

namespace ble_handler {

using ConnectCallback = std::function<void(bool connected)>;

void init(motor::WheelPair &pair);

void set_connect_callback(ConnectCallback cb);

bool is_connected();

} // namespace ble_handler
