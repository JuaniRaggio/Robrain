#include "comm/ble_handler.h"
#include "motor/motor_controller.h"
#include <Arduino.h>

constexpr uint32_t BAUD_RATE = 115200;

static motor::WheelPair wheels;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("[ROBRAIN] Iniciando...");

  wheels.init();

  ble_handler::set_connect_callback([](bool connected) {
    if (!connected) wheels.stop();
  });

  ble_handler::init(wheels);

  Serial.println("[ROBRAIN] Listo, esperando conexion BLE...");
}

void loop() {
  if (!ble_handler::is_connected()) return;

  if (millis() - wheels.last_command_ms() > motor::MOTOR_TIMEOUT_MS) {
    wheels.stop();
    return;
  }

  // ejecuta el comando actual del atomic
  wheels.update();
}