#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

namespace robrain {

enum class MessageType : uint8_t {
    // Comandos de motor
    MOTOR_SET       = 0x01,
    MOTOR_STOP      = 0x02,

    // Estado
    STATUS_REQ      = 0x10,
    STATUS_RESP     = 0x11,

    // Datos EMG
    EMG_RAW         = 0x20,
    EMG_RMS         = 0x21,
    EMG_EVENT       = 0x22,

    // Control
    HEARTBEAT       = 0x30,
    CONFIG          = 0x40,
    CONFIG_ACK      = 0x41,
};

struct Message {
    MessageType type;
    std::vector<uint8_t> payload;
};

// Serializa mensaje a buffer con framing
std::vector<uint8_t> serialize(const Message& msg);

// Deserializa buffer a mensaje
// Retorna true si el mensaje es valido
bool deserialize(const uint8_t* data, size_t len, Message& out);

// Calcula checksum
uint8_t checksum(const uint8_t* data, size_t len);

// Helpers para crear mensajes comunes
Message make_motor_command(int16_t left, int16_t right);
Message make_stop_command();
Message make_heartbeat();
Message make_status_request();

} // namespace robrain
