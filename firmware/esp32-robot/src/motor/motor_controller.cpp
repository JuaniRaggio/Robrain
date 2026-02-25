#include "motor_controller.h"
#include <Arduino.h>
#include <atomic>

namespace motor {

uint16_t WheelPair::pack(const command::WheelCommand &cmd) {
  return (static_cast<uint16_t>(cmd.left) << 8) | cmd.right;
}

command::WheelCommand WheelPair::unpack(uint16_t data) {
  return {static_cast<uint8_t>(data >> 8), static_cast<uint8_t>(data & 0xFF)};
}

WheelPair::WheelPair()
    : left_(PIN_LEFT_IN1, PIN_LEFT_IN2, PWM_CHANNEL_LEFT),
      right_(PIN_RIGHT_IN1, PIN_RIGHT_IN2, PWM_CHANNEL_RIGHT), current_cmd(0),
      last_cmd_time(0) {}

void WheelPair::init() {
  left_.init();
  right_.init();
}

void set_command(const command::WheelCommand &cmd) {
  current_cmd_.store(pack(cmd)); // void store(T desired, std::memory_order
                                 // order = std::memory_order_seq_cst)
  last_cmd_ms_.store(millis());  // ver si cambiar el memory_order a otro
}

void WheelPair::update() {
  auto cmd = unpack(current_cmd_.load());
  cmd.left == 0 ? left_.stop() : left_.forward(cmd.left);
  cmd.right == 0 ? right_.stop() : right_.forward(cmd.right);
}

void WheelPair::stop() {
  current_cmd_.store(0);
  left_.stop();
  right_.stop();
}

uint32_t WheelPair::last_command_ms() const {
  return last_cmd_ms_.load();
}

} // namespace motor