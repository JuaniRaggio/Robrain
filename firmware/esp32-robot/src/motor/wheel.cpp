#include "wheel.h"
#include <Arduino.h>
#include <algorithm>
#include <cstdlib>

namespace motor {

constexpr uint32_t PWM_FREQ_HZ = 1000;
constexpr uint8_t PWM_RESOLUTION = 8; // 8 bits → 0-255
constexpr uint8_t PWM_MAX = 255;

Wheel::Wheel(uint8_t in1_pin, uint8_t in2_pin, uint8_t pwm1_channel_,
             uint8_t pwm2_channel_)
    : in1_pin_(in1_pin), in2_pin_(in2_pin), pwm1_channel(pwm1_channel_),
      pwm2_channel(pwm2_channel_) {}

void Wheel::init() {
  // in1, in2--> PWM
  ledcSetup(pwm1_channel, PWM_FREQ_HZ, PWM_RESOLUTION);
  ledcSetup(pwm2_channel, PWM_FREQ_HZ, PWM_RESOLUTION);

  ledcAttachPin(in1_pin_, pwm1_channel);
  ledcAttachPin(in2_pin_, pwm2_channel);

  stop();
}

void Wheel::move(int16_t speed) {
  if (speed == 0) {
    stop();
    return;
  }

  int s = std::clamp((int)speed, -100,
                     100); // template<class T> constexpr const T& clamp( const
                           // T& v, const T& lo, const T& hi );
  uint8_t duty = pwm_speed((uint16_t)std::abs(s));

  if (s > 0) {
    // Avanza: PWM en IN1, IN2=0
    ledcWrite(pwm2_channel, 0);
    ledcWrite(pwm1_channel, duty);
  } else {
    // Retrocede: PWM en IN2, IN1=0
    ledcWrite(pwm1_channel, 0);
    ledcWrite(pwm2_channel, duty);
  }
}

void Wheel::stop() {
  ledcWrite(pwm1_channel, PWM_MAX);
  ledcWrite(pwm2_channel, PWM_MAX);
}

uint8_t Wheel::pwm_speed(uint16_t speed) {
  speed = (uint16_t)std::min<uint16_t>(speed, 100);
  return (uint8_t)(speed * PWM_MAX / 100);
}

} // namespace motor
