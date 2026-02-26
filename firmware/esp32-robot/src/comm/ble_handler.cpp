#include "ble_handler.h"
#include <motor/motor_controller.h>
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

    wireless_protocol::MotorPayload cmd;
    if (!command::parse_motor_payload(data, len, cmd)) {
      Serial.println("[BLE] Paquete invalido, ignorando");
      return;
    }

    Serial.printf("[BLE] L=%d R=%d\n", cmd.left_speed, cmd.right_speed);

    if (wheel_pair) wheel_pair->set_command(cmd);
  }
};

void init(motor::WheelPair &pair) {
  wheel_pair = &pair;

  NimBLEDevice::init(wireless_protocol::DEVICE_NAME);

  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  NimBLEService *pService = pServer->createService(wireless_protocol::SERVICE_UUID);

  // PC -> ESP32
  NimBLECharacteristic *pCmdChar =
      pService->createCharacteristic(wireless_protocol::CMD_CHAR_UUID, NIMBLE_PROPERTY::WRITE);
  pCmdChar->setCallbacks(new CmdCharCallbacks());

  // ESP32 -> PC
  pService->createCharacteristic(wireless_protocol::STATUS_CHAR_UUID, NIMBLE_PROPERTY::NOTIFY |
                                                       NIMBLE_PROPERTY::READ);
  pService->start();

  NimBLEDevice::getAdvertising()->addServiceUUID(wireless_protocol::SERVICE_UUID);
  NimBLEDevice::startAdvertising();

  Serial.printf("[BLE] Servidor listo: %s\n", wireless_protocol::DEVICE_NAME);
}

void set_connect_callback(ConnectCallback cb) {
  connect_cb = cb;
}
bool is_connected() {
  return connected;
}

} // namespace ble_handler
