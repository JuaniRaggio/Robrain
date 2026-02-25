#pragma once

#include <protocol/wheel_command.h>
#include "wheel.h"
#include <Arduino.h>
#include <atomic>

namespace motor {

// Pines (L298N) --> check
constexpr uint8_t PIN_LEFT_IN1 = 4;   // GPIO 4  - cable naranja
constexpr uint8_t PIN_LEFT_IN2 = 5;   // GPIO 5  - cable amarillo
constexpr uint8_t PIN_RIGHT_IN1 = 12; // GPIO 12 - cable verde
constexpr uint8_t PIN_RIGHT_IN2 = 13; // GPIO 13 - cable azul

// Canales PWM del ESP32 (0-15, uno por rueda)
constexpr uint8_t PWM_CHANNEL_LEFT = 0;
constexpr uint8_t PWM_CHANNEL_RIGHT = 1;

constexpr uint32_t MOTOR_TIMEOUT_MS = 500;

class WheelPair {
public:
  WheelPair();

  void init();

  void set_command(const command::WheelCommand &cmd);

  void update();

  uint32_t last_command_ms() const; //-> mas que nada para el safety timeout

  void stop();

private:
  Wheel left_;
  Wheel right_;
  std::atomic<uint16_t> current_cmd;
  std::atomic<uint32_t> last_cmd_time;

  static uint16_t pack(const command::WheelCommand &cmd);
  static command::WheelCommand unpack(uint16_t raw);
};

} // namespace motor
