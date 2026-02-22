#pragma once

#include <cstddef>
#include <cstdint>

namespace robrain {

//por ahora intensidad 0-100 --> ver si cambiarlo

struct WheelCommand {
    uint8_t left;   
    uint8_t right;  
} __attribute__((packed));

/*
Parsea los bytes recibidos por BLE y llena 'out'.
Retorna false si el buffer es invalido.
*/
bool parse_wheel_cmd(const uint8_t *data, size_t len, WheelCommand &out);

} // namespace robrain