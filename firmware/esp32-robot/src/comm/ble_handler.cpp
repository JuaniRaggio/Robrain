// TERMINARLO
#include "ble_handler.h"
#include "../motor/motor_controller.h"
#include <Arduino.h>
#include <NimBLEDevice.h>

namespace ble_handler {

static bool connected = false;
static ConnectCallback connect_cb = nullptr;

/*
Se ejecuta cuando un cliente BLE conecta o desconecta.
*/
class ServerCallbacks : public NimBLEServerCallbacks {

  void onConnect(NimBLEServer *server) override {
    connected = true;
    Serial.println("[BLE] Cliente conectado");
    if (connect_cb) connect_cb(true);
  }

  void onDisconnect(NimBLEServer *server) override {
    connected = false;
    Serial.println("[BLE] Cliente desconectado");
    if (connect_cb) connect_cb(false);
    NimBLEDevice::startAdvertising();
  }
};

/*
Se ejecuta cuando el host escribe en CMD_CHAR_UUID.
Parsea los bytes y los manda a la queue del motor (producer).
*/
class CmdCharCallbacks : public NimBLECharacteristicCallbacks {

  void onWrite(NimBLECharacteristic *pChar) override {
    const uint8_t *data = pChar->getValue().data();
    size_t len = pChar->getValue().length();

    command::WheelCommand cmd;
    if (!command::parse_wheel_cmd(data, len, cmd)) {
      Serial.println("[BLE] Paquete invalido, ignorando");
      return;
    }

    Serial.printf("[BLE] L=%d R=%d\n", cmd.left, cmd.right);

    // Manda el comando a la queue → motor task lo consume en Core 0
    motor::send_command(cmd);
  }

  Serial.printf("[BLE] L=%d R=%d\n", cmd.left, cmd.right);

  // Manda el comando a la queue → motor task lo consume en Core 0
  motor::send_command(cmd);
}
};

// API publica --> investigar más

void init() {
  NimBLEDevice::init(DEVICE_NAME);

  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  NimBLEService *pService = pServer->createService(SERVICE_UUID);

  // PC --> ESP32
  NimBLECharacteristic *pCmdChar = pService->createCharacteristic(
      CMD_CHAR_UUID,
      NIMBLE_PROPERTY::WRITE); //---> CHECK si conviene write without response.
                               // creo q mejora latencia..
  pCmdChar->setCallbacks(new CmdCharCallbacks());

  // ESP32--> PC con notificaciones
  pService->createCharacteristic(STATUS_CHAR_UUID, NIMBLE_PROPERTY::NOTIFY |
                                                       NIMBLE_PROPERTY::READ);

  pService->start();

  NimBLEDevice::getAdvertising()->addServiceUUID(SERVICE_UUID);
  NimBLEDevice::startAdvertising();

  Serial.printf("[BLE] Servidor listo: %s\n", DEVICE_NAME);
}

void set_connect_callback(ConnectCallback cb) {
  connect_cb = cb;
}

bool is_connected() {
  return connected;
}

} // namespace ble_handler
