#pragma once

#include <stdint.h>
#include <stddef.h>

// Protocolo comun compartido entre todos los componentes del sistema
// Este archivo se puede copiar a cada proyecto o usar como referencia

namespace robrain {
namespace ble_protocol {

struct MotorPayload {
    int16_t left_speed;
    int16_t right_speed;
} __attribute__((packed));

} // namespace protocol
} // namespace robrain
