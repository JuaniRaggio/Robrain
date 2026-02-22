#include "comm/ble_handler.h"
#include "motor/motor_controller.h"
#include <Arduino.h>

static motor::WheelPair wheels;

void setup() {
  Serial.begin(115200);
  Serial.println("[ROBRAIN] Iniciando...");

  wheels.init();

  motor::start_motor_task(wheels);

  // Inicializo BLE
  ble_handler::set_connect_callback([](bool connected) {
    if (!connected) {
      // Si el host se desconecta, parar inmediatamente por seguridad.
      // El safety timeout del motor task tambien cubre este caso,
      // pero esto es mas rapido.
      robrain::WheelCommand stop = {0, 0};
      motor::send_command(stop);
    }
  });

  ble_handler::init();
  Serial.println("[ROBRAIN] Listo, esperando conexion BLE...");
}

void loop() {
  // Todo corre en tasks o callbacks, loop queda libre.
  vTaskDelay(pdMS_TO_TICKS(1000));
}