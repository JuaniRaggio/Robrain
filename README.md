# Robrain

Sistema BCI (Brain Computer Interface) para control de robots mediante senales EMG/EEG.

## Estructura del Proyecto

```
robrain/
├── common/           # Protocolo compartido entre componentes
├── docs/             # Documentacion tecnica
├── firmware/
│   ├── arduino-emg/  # Captura de senales EMG
│   └── esp32-robot/  # Control de motores del robot
└── host/             # Aplicacion de laptop (puente Arduino-ESP32)
```

## Flujo de Datos

```
[Electrodos] --> [Arduino] --serial--> [Laptop] --BLE--> [ESP32] --> [Motores]
                   EMG                  procesa           robot
                                        signals
```

## Requisitos

### Host (Laptop)

- CMake >= 3.16
- Compilador C++17 (GCC, Clang)
- Boost (opcional, para serial async)
- SimpleBLE (para comunicacion BLE)

```bash
# Ubuntu/Debian
sudo apt install cmake build-essential libboost-dev libbluetooth-dev

# macOS
brew install cmake boost

# SimpleBLE (todas las plataformas)
# https://github.com/OpenBluetoothToolbox/SimpleBLE
git clone https://github.com/OpenBluetoothToolbox/SimpleBLE.git
cd SimpleBLE && mkdir build && cd build
cmake .. && make && sudo make install
```

### Firmwares (Arduino y ESP32)

- PlatformIO Core (CLI) o PlatformIO IDE

```bash
# Instalar PlatformIO CLI
pip install platformio

# O con pipx (recomendado)
pipx install platformio
```

## Compilacion

### Host (Laptop)

```bash
cd robrain
mkdir build && cd build
cmake ..
make
```

El ejecutable queda en `build/host/robrain_host`.

### Firmware ESP32

```bash
cd robrain/firmware/esp32-robot

# Compilar
pio run

# O desde la raiz con CMake
cd robrain/build
make firmware-esp32
```

### Firmware Arduino

```bash
cd robrain/firmware/arduino-emg

# Compilar (por defecto usa Arduino Uno)
pio run

# Compilar para Arduino Mega
pio run -e mega

# O desde la raiz con CMake
cd robrain/build
make firmware-arduino
```

### Compilar todos los firmwares

```bash
cd robrain/build
make firmware-all
```

## Upload (Subir a los microcontroladores)

### ESP32

1. Conectar ESP32 por USB
2. Ejecutar:

```bash
cd robrain/firmware/esp32-robot

# Detectar puerto automaticamente
pio run --target upload

# Especificar puerto manualmente
pio run --target upload --upload-port /dev/ttyUSB0

# O desde CMake
cd robrain/build
make upload-esp32
```

### Arduino

1. Conectar Arduino por USB
2. Ejecutar:

```bash
cd robrain/firmware/arduino-emg

# Arduino Uno
pio run --target upload

# Arduino Mega
pio run -e mega --target upload

# Especificar puerto
pio run --target upload --upload-port /dev/ttyACM0

# O desde CMake
cd robrain/build
make upload-arduino
```

## Monitor Serial

Para ver logs y debug de los microcontroladores:

```bash
# ESP32
cd firmware/esp32-robot
pio device monitor

# Arduino
cd firmware/arduino-emg
pio device monitor

# Especificar baudrate y puerto
pio device monitor -b 115200 -p /dev/ttyUSB0
```

## Puertos comunes

| Sistema | ESP32 | Arduino |
|---------|-------|---------|
| Linux | `/dev/ttyUSB0` | `/dev/ttyACM0` |
| macOS | `/dev/cu.usbserial-*` | `/dev/cu.usbmodem*` |
| Windows | `COM3`, `COM4`, etc. | `COM3`, `COM4`, etc. |

Para listar puertos disponibles:

```bash
pio device list
```

## Configuracion BLE (ESP32)

Editar `firmware/esp32-robot/src/comm/ble_handler.h`:

```cpp
constexpr const char* DEVICE_NAME = "ROBRAIN_ROBOT";

// UUIDs del servicio BLE
constexpr const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
constexpr const char* CMD_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
constexpr const char* STATUS_CHAR_UUID = "beb5483f-36e1-4688-b7f5-ea07361b26a8";
```

El ESP32 actua como servidor BLE (peripheral). La laptop se conecta como cliente (central).
