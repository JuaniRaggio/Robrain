#pragma once

#include <cstdint>
#include <cstddef>

// Protocolo comun compartido entre todos los componentes del sistema
// Este archivo se puede copiar a cada proyecto o usar como referencia

namespace robrain {
namespace protocol {

// Bytes de framing
constexpr uint8_t START_BYTE = 0xAA;
constexpr uint8_t END_BYTE = 0x55;
constexpr size_t MAX_PAYLOAD_SIZE = 32;

// Tipos de comando
enum class CommandType : uint8_t {
    // Control de motores (0x01 - 0x0F)
    MOTOR_SET       = 0x01,
    MOTOR_STOP      = 0x02,

    // Estado y diagnostico (0x10 - 0x1F)
    STATUS_REQ      = 0x10,
    STATUS_RESP     = 0x11,
    ERROR           = 0x1F,

    // Datos EMG (0x20 - 0x2F)
    EMG_RAW         = 0x20,
    EMG_RMS         = 0x21,
    EMG_EVENT       = 0x22,
    EMG_CALIBRATE   = 0x23,

    // Control de sesion (0x30 - 0x3F)
    HEARTBEAT       = 0x30,
    HANDSHAKE       = 0x31,
    DISCONNECT      = 0x32,

    // Configuracion (0x40 - 0x4F)
    CONFIG_SET      = 0x40,
    CONFIG_GET      = 0x41,
    CONFIG_ACK      = 0x42,
};

// Estructura de paquete raw
struct RawPacket {
    uint8_t start;                      // START_BYTE
    uint8_t cmd;                        // CommandType
    uint8_t length;                     // Longitud del payload
    uint8_t payload[MAX_PAYLOAD_SIZE];  // Datos
    uint8_t checksum;                   // XOR de cmd + length + payload
    uint8_t end;                        // END_BYTE
};

// Payload de comando de motor
struct MotorPayload {
    int16_t left_speed;   // -100 a 100
    int16_t right_speed;  // -100 a 100
};

// Payload de datos EMG raw
struct EmgRawPayload {
    uint32_t timestamp_ms;
    uint8_t num_channels;
    uint16_t channels[4];  // Hasta 4 canales
};

// Payload de evento EMG
struct EmgEventPayload {
    uint8_t channel;
    uint8_t active;  // 0 = inactivo, 1 = activo
    uint16_t value;
};

// Payload de estado del robot
struct StatusPayload {
    uint8_t state;         // 0=IDLE, 1=RUNNING, 2=ERROR
    int16_t left_speed;
    int16_t right_speed;
    uint16_t battery_mv;   // Voltaje de bateria en mV
};

} // namespace protocol
} // namespace robrain
