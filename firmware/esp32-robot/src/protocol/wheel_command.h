#pragma once

#include <cstddef>
#include <cstdint>

namespace command {

/*
Parsea los bytes recibidos por BLE y llena 'out'.
Retorna false si el buffer es invalido.
*/
bool parse_wheel_cmd(const uint8_t *data, size_t len, WheelCommand &out);

} // namespace command