#pragma once

#include "../protocol/wheel_command.h"
#include "wheel.h"

namespace motor {

// Pines (L298N) --> check 
constexpr uint8_t PIN_LEFT_IN1  = 4;   // GPIO 4  - cable naranja
constexpr uint8_t PIN_LEFT_IN2  = 5;   // GPIO 5  - cable amarillo
constexpr uint8_t PIN_RIGHT_IN1 = 12;  // GPIO 12 - cable verde
constexpr uint8_t PIN_RIGHT_IN2 = 13;  // GPIO 13 - cable azul

// Canales PWM del ESP32 (0-15, uno por rueda)
constexpr uint8_t PWM_CHANNEL_LEFT  = 0;
constexpr uint8_t PWM_CHANNEL_RIGHT = 1;

// ---------------------------------------------------------------------------
// WheelPair
// Controla las dos ruedas juntas.
// Recibe un WheelCommand (left intensity, right intensity) y las aplica.
// ---------------------------------------------------------------------------
class WheelPair {
public:
    WheelPair();

    // Inicializa ambas ruedas. Llamar una vez en setup().
    void init();

    // Aplica un WheelCommand: cada rueda recibe su intensidad.
    void execute(const robrain::WheelCommand &cmd);

    // Para ambas ruedas.
    void stop();

private:
    Wheel left_;
    Wheel right_;
};

// ---------------------------------------------------------------------------
// Motor Task (FreeRTOS - Core 0)
// Lee WheelCommands de la queue y los ejecuta.
// Safety timeout: si no llega ningun comando en TIMEOUT_MS, para.
// ---------------------------------------------------------------------------
constexpr uint32_t MOTOR_TIMEOUT_MS = 500;

// Inicializa la queue y lanza la task en Core 0.
void start_motor_task(WheelPair &pair);

// Envia un WheelCommand a la queue (llamado desde el BLE handler).
bool send_command(const robrain::WheelCommand &cmd);

} // namespace motor