#pragma once

#include <cstdint>
#include "emg_reader.h"

namespace serial_proto {

// Formato de mensaje serial hacia la laptop
// START(1) + TYPE(1) + LEN(1) + PAYLOAD(N) + CHECKSUM(1) + END(1)

constexpr uint8_t START_BYTE = 0xAA;
constexpr uint8_t END_BYTE = 0x55;

enum class MessageType : uint8_t {
    EMG_RAW     = 0x01,  // Datos EMG crudos
    EMG_RMS     = 0x02,  // Valores RMS procesados
    EMG_EVENT   = 0x03,  // Evento de activacion muscular
    STATUS      = 0x10,  // Estado del sensor
    CONFIG_ACK  = 0x20,  // Confirmacion de configuracion
};

// Envia muestra EMG por serial
void send_emg_sample(const emg::EmgSample& sample);

// Envia valores RMS de todos los canales
void send_rms_values(const uint16_t* rms, uint8_t num_channels);

// Envia evento de activacion muscular
void send_muscle_event(uint8_t channel, bool active);

// Procesa comandos entrantes de la laptop
// Retorna true si hay comando pendiente
bool process_incoming();

// Obtiene ultimo comando recibido
struct IncomingCommand {
    enum Type { NONE, SET_SAMPLE_RATE, SET_THRESHOLD, START, STOP };
    Type type;
    uint16_t value;
};
IncomingCommand get_last_command();

} // namespace serial_proto
