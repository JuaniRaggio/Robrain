#pragma once

#include <stdint.h>

namespace motor {

/*
  2 pines por motor (IN1, IN2) para controlar direccion:
  IN1=PWM, IN2=LOW → adelante
  IN1=LOW, IN2=LOW → para
*/

class Wheel {
public:
  Wheel(uint8_t in1_pin, uint8_t in2_pin, uint8_t pwm1_channel,
        uint8_t pwm2_channel);

  // Configura los pines y el canal PWM. Llamar una vez en setup().
  void init();

  // Mueve la rueda hacia adelante a la velocidad dada (-100 a 100).
  void move(int16_t speed);

  void stop();

private:
  uint8_t in1_pin_;     // controla PWM (velocidad)
  uint8_t in2_pin_;     // siempre LOW para adelante
  uint8_t pwm1_channel; // canal PWM del ESP32 (0-15)
  uint8_t pwm2_channel;

  // Convierte 0-100 a 0-255 para el PWM
  static uint8_t pwm_speed(uint16_t speed);
};

} // namespace motor
