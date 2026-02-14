#pragma once

#include <cstdint>

namespace motor {

// Pines de los motores (ajustar segun hardware)
constexpr int PIN_MOTOR_LEFT_PWM = 25;
constexpr int PIN_MOTOR_LEFT_DIR = 26;
constexpr int PIN_MOTOR_RIGHT_PWM = 27;
constexpr int PIN_MOTOR_RIGHT_DIR = 14;

// Inicializa los pines y PWM
void init();

// Establece velocidad de motores (-100 a 100)
void set_speed(int16_t left, int16_t right);

// Detiene ambos motores
void stop();

// Obtiene estado actual
struct MotorState {
  int16_t left_speed;
  int16_t right_speed;
};
MotorState get_state();

} // namespace motor
