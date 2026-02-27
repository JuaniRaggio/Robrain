#include "motor_controller.h"
#include <Arduino.h>
#include <algorithm>

namespace motor {

uint16_t WheelPair::pack(const wireless_protocol::MotorPayload &cmd) {
  int8_t l_speed = (int8_t)cmd.left_speed; // ya está en [-100,100]
  int8_t r_speed = (int8_t)cmd.right_speed;

  return (uint16_t(uint8_t(l_speed)) << 8) | uint16_t(uint8_t(r_speed));
}

wireless_protocol::MotorPayload WheelPair::unpack(uint16_t data) {
  int8_t l = (int8_t)(uint8_t)(data >> 8);
  int8_t r = (int8_t)(uint8_t)(data & 0xFF);
  return {(int16_t)l, (int16_t)r};
}

WheelPair::WheelPair()
    : left_(PIN_LEFT_IN1, PIN_LEFT_IN2, PWM_LEFT_IN1, PWM_LEFT_IN2),
      right_(PIN_RIGHT_IN1, PIN_RIGHT_IN2, PWM_RIGHT_IN1, PWM_RIGHT_IN2),
      current_cmd(0), last_cmd_time(0), stopped(true) {}

void WheelPair::init() {
  left_.init();
  right_.init();
}

void WheelPair::set_command(const wireless_protocol::MotorPayload &cmd) {
  current_cmd.store(pack(cmd));  // void store(T desired, std::memory_order
                                 // order = std::memory_order_seq_cst)
  last_cmd_time.store(millis()); // ver si cambiar el memory_order a otro

  if (cmd.left_speed == 0 && cmd.right_speed == 0) {
    stopped.store(true);
  } else {
    stopped.store(false);
  }
}

void WheelPair::update() {
  auto cmd = unpack(current_cmd.load());
  left_.move(cmd.left_speed);
  right_.move(cmd.right_speed);
}

void WheelPair::stop() {
  current_cmd.store(0);
  stopped.store(true);
  left_.stop();
  right_.stop();
}

uint32_t WheelPair::last_command_ms() const {
  return last_cmd_time.load();
}

bool WheelPair::is_stopped() const {
  return stopped.load();
}

} // namespace motor