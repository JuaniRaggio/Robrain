#pragma once

#include <cstdint>
#include <stddef.h>
#include <stdint.h>

namespace wireless_protocol {

// Info compartida para configuracion host - esp32
constexpr const char * const DEVICE_NAME = "ROBRAIN_ROBOT";
constexpr const char * const SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
constexpr const char * const CMD_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
constexpr const char * const STATUS_CHAR_UUID = "beb5483f-36e1-4688-b7f5-ea07361b26a8";

constexpr int16_t LIMIT_SPEED = 100;

struct MotorPayload {
  int16_t left_speed;
  int16_t right_speed;

  MotorPayload() : left_speed(0), right_speed(0) {}

  MotorPayload(int16_t l, int16_t r) : left_speed(l), right_speed(r) {}

  inline uint8_t checksum() { return 0 ^ left_speed ^ right_speed; }
} __attribute__((packed));

} // namespace wireless_protocol
