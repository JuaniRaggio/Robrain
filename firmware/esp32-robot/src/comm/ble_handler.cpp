#include "ble_handler.h"
#include "packet.h"
#include <Arduino.h>
#include <NimBLEDevice.h>

namespace ble_handler {

static bool connected = false;
static ConnectCallback connect_cb = nullptr;

// Puntero al WheelPair, se setea en init()
static motor::WheelPair *wheel_pair = nullptr;

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
    // Volvemos a anunciar para permitir reconexion
    NimBLEDevice::startAdvertising();
  }
};

class CmdCharCallbacks : public NimBLECharacteristicCallbacks {

  void onWrite(NimBLECharacteristic *pChar) override {
    const uint8_t *data = pChar->getValue().data();
    size_t len = pChar->getValue().length();

    command::WheelCommand cmd;
    if (!command::parse_wheel_command(data, len, cmd)) {
      Serial.println("[BLE] Paquete invalido, ignorando");
      return;
    }

    Serial.printf("[BLE] L=%d R=%d\n", cmd.left, cmd.right);

    if (wheel_pair) wheel_pair->set_command(cmd);
  }
};

void init(motor::WheelPair &pair) {
  wheel_pair = &pair;

  NimBLEDevice::init(DEVICE_NAME);

  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  NimBLEService *pService = pServer->createService(SERVICE_UUID);

  // PC → ESP32
  NimBLECharacteristic *pCmdChar =
      pService->createCharacteristic(CMD_CHAR_UUID, NIMBLE_PROPERTY::WRITE);
  pCmdChar->setCallbacks(new CmdCharCallbacks());

  // ESP32 → PC
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