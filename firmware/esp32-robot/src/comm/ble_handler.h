#pragma once

#include "../protocol/command_parser.h"
#include <functional>

namespace ble_handler {

// Configuracion BLE
constexpr const char *DEVICE_NAME = "ROBRAIN_ROBOT";

// Servicio principal
constexpr const char *SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
// Caracteristica para recibir comandos (write)
constexpr const char *CMD_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
// Caracteristica para enviar estado (notify)
constexpr const char *STATUS_CHAR_UUID = "beb5483f-36e1-4688-b7f5-ea07361b26a8";

using CommandCallback = std::function<void(const robrain::MotorCommand &)>;
using ConnectCallback = std::function<void(bool connected)>;

// Inicializa BLE server
void init();

// Procesa eventos BLE (llamar en loop. Es opcional)
// void update();

// Registra callback para comandos de motor
void set_command_callback(CommandCallback cb);

// Registra callback para conexion/desconexion
void set_connect_callback(ConnectCallback cb);

// Envia estado del robot (via notify)
void send_status(int16_t left_speed, int16_t right_speed, uint8_t state);

// Verifica si hay cliente conectado
bool is_connected();

} // namespace ble_handler
