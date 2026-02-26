#include "motor_controller.h"
#include <Arduino.h>
#include <atomic>

namespace motor {

uint16_t WheelPair::pack(const wireless_protocol::MotorPayload &cmd) {
  return (static_cast<uint16_t>(cmd.left_speed) << 8) | cmd.right_speed;
}

wireless_protocol::MotorPayload WheelPair::unpack(uint16_t data) {
  return {static_cast<int16_t>(data >> 8), static_cast<int16_t>(data & 0xFF)};
}

WheelPair::WheelPair()
    : left_(PIN_LEFT_IN1, PIN_LEFT_IN2, PWM_CHANNEL_LEFT),
      right_(PIN_RIGHT_IN1, PIN_RIGHT_IN2, PWM_CHANNEL_RIGHT), current_cmd(0),
      last_cmd_time(0) {}

void WheelPair::init() {
  left_.init();
  right_.init();
}

void WheelPair::set_command(const wireless_protocol::MotorPayload &cmd) {
  current_cmd.store(pack(cmd)); // void store(T desired, std::memory_order
                                 // order = std::memory_order_seq_cst)
  last_cmd_time.store(millis());  // ver si cambiar el memory_order a otro
}

void WheelPair::update() {
  auto cmd = unpack(current_cmd.load());
  cmd.left_speed == 0 ? left_.stop() : left_.forward(cmd.left_speed);
  cmd.right_speed == 0 ? right_.stop() : right_.forward(cmd.right_speed);
}

void WheelPair::stop() {
  current_cmd.store(0);
  left_.stop();
  right_.stop();
}

uint32_t WheelPair::last_command_ms() const {
  return last_cmd_time.load();
}

} // namespace motor
