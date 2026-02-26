#include "wheel.h"
#include <Arduino.h>

namespace motor {

constexpr uint32_t PWM_FREQ_HZ = 1000;
constexpr uint8_t PWM_RESOLUTION = 8; // 8 bits → 0-255
constexpr uint8_t PWM_MAX = 255;

Wheel::Wheel(uint8_t in1_pin, uint8_t in2_pin, uint8_t pwm_channel)
    : in1_pin_(in1_pin), in2_pin_(in2_pin), pwm_channel_(pwm_channel) {}

void Wheel::init() {
  // in1--> PWM, in2--> digital
  ledcSetup(pwm_channel_, PWM_FREQ_HZ, PWM_RESOLUTION);
  ledcAttachPin(in1_pin_, pwm_channel_);
  pinMode(in2_pin_, OUTPUT);
  stop();
}

void Wheel::forward(uint8_t speed) {
  // IN1=PWM, IN2=LOW → adelante
  ledcWrite(pwm_channel_, pwm_speed(speed));
  digitalWrite(in2_pin_, LOW);
}

void Wheel::stop() {
  // IN1=LOW, IN2=HIGH → freno. Si queremos coast --> IN1=LOW y IN2= LOW
  ledcWrite(pwm_channel_, 0);
  digitalWrite(in2_pin_, HIGH);
}

uint8_t Wheel::pwm_speed(uint8_t speed) {
  return static_cast<uint8_t>(speed * PWM_MAX / 100);
}

} // namespace motor
