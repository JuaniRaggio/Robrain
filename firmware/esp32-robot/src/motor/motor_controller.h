#pragma once

#include "wheel.h"
#include <Arduino.h>
#include <atomic>
#include <protocol/wireless_packet.h>

namespace motor {

// Pines (L298N) --> check
constexpr uint8_t PIN_LEFT_IN1 = 4;   // GPIO 4  - cable naranja
constexpr uint8_t PIN_LEFT_IN2 = 5;   // GPIO 5  - cable amarillo
constexpr uint8_t PIN_RIGHT_IN1 = 12; // GPIO 12 - cable verde
constexpr uint8_t PIN_RIGHT_IN2 = 13; // GPIO 13 - cable azul

// Canales PWM del ESP32 (0-15, dos por rueda)
constexpr uint8_t PWM_LEFT_IN1 = 0;
constexpr uint8_t PWM_LEFT_IN2 = 1;
constexpr uint8_t PWM_RIGHT_IN1 = 2;
constexpr uint8_t PWM_RIGHT_IN2 = 3;

constexpr uint32_t MOTOR_TIMEOUT_MS = 500;

class WheelPair {
public:
  WheelPair();

  void init();

  void set_command(const wireless_protocol::MotorPayload &cmd);

  void update();

  uint32_t last_command_ms() const; //-> mas que nada para el safety timeout

  void stop();

  bool is_stopped() const;

private:
  Wheel left_;
  Wheel right_;
  std::atomic<uint16_t> current_cmd;
  std::atomic<uint32_t> last_cmd_time;
  std::atomic<bool> stopped;

  static uint16_t pack(const wireless_protocol::MotorPayload &cmd);
  static wireless_protocol::MotorPayload unpack(uint16_t raw);
};

} // namespace motor
