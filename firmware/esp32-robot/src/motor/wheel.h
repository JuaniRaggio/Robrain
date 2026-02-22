#pragma once

#include <cstdint>

namespace motor {

/*
  2 pines por motor (IN1, IN2) para controlar direccion:
  IN1=PWM, IN2=LOW → adelante
  IN1=LOW, IN2=LOW → para
*/

class Wheel {
public:
    Wheel(uint8_t in1_pin, uint8_t in2_pin, uint8_t pwm_channel);

    // Configura los pines y el canal PWM. Llamar una vez en setup().
    void init();

    // Mueve la rueda hacia adelante a la velocidad dada (0-100).
    void forward(uint8_t speed);

    // Para la rueda.
    void stop();

private:
    uint8_t in1_pin_;      // controla PWM (velocidad)
    uint8_t in2_pin_;      // siempre LOW para adelante
    uint8_t pwm_channel_;  // canal PWM del ESP32 (0-15)

    // Convierte 0-100 a 0-255 para el PWM
    static uint8_t pwm_speed(uint8_t speed);
};

} // namespace motor