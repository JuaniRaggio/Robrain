#pragma once

#include "../protocol/wheel_command.h"
#include <functional>

namespace ble_handler {

constexpr const char *DEVICE_NAME = "ROBRAIN_ROBOT";
constexpr const char *SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
constexpr const char *CMD_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
constexpr const char *STATUS_CHAR_UUID = "beb5483f-36e1-4688-b7f5-ea07361b26a8";

using ConnectCallback = std::function<void(bool connected)>;

void init();

void set_connect_callback(ConnectCallback cb);

bool is_connected();

} // namespace ble_handler