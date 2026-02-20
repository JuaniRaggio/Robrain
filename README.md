# Robrain

BCI (Brain Computer Interface) system for robot control using EMG/EEG signals.

## TODO List pendiente

### YA
- (host) Leer info del arduino
- (host) Realizar promedio de muestras de forma vectorizada (en el futuro esto seria procesamiento)
- (host) Enviar indicaciones al esp32 via BLE

- (esp32) Leer indicaciones
- (esp32) Actuar

> [!NOTE]
> Importante tener en cuenta la siguiente data:
> | Baudrate | Tiempo para 256 bytes (aprox.) |
> |---|---|
> |9600|~267 ms|
> | 115200|\~22 ms|
> |1000000|\~2.5 ms|


### Que hacer ahora?

- [ ] Configuracion de Comunicacion serial entre Arduino y Host
- [ ] Protocolo de comunicacion serial
- [ ] Implementar la comunicacion
- [ ] Configuracion de Conexion BLE
- [ ] Protocolos de comunicacion entre esp32 y el host
- [ ] Implementar la comunicacion

### Paso "intermedio" para tener algo funcionando

- [ ] Implementar un procesamiento MUY simple de señales
- [ ] Control de motores

### Para el final (Lo mas jodido)

- [ ] Filtrar señales
- [ ] Implementar procesamiento ML-based


## Project Structure

```
robrain/
├── common/           # Shared protocol between components
├── docs/             # Technical documentation
├── firmware/
│   ├── arduino-emg/  # EMG signal capture (PlatformIO)
│   └── esp32-robot/  # Robot motor control (PlatformIO)
└── host/             # Laptop application (CMake)
```

## Data Flow

```
[Electrodes] --> [Arduino] --serial--> [Laptop] --BLE--> [ESP32] --> [Motors]
                    EMG                 processes         robot
                                        signals
```

## Requirements

### Host (Laptop)

- CMake >= 3.16
- C++17 compiler (GCC, Clang)
- Boost (optional, for async serial)
- SimpleBLE (for BLE communication)

```bash
# Ubuntu/Debian
sudo apt install cmake build-essential libboost-dev libbluetooth-dev

# macOS
brew install cmake boost

# SimpleBLE (all platforms)
# https://github.com/OpenBluetoothToolbox/SimpleBLE
git clone https://github.com/OpenBluetoothToolbox/SimpleBLE.git
cd SimpleBLE && mkdir build && cd build
cmake .. && make && sudo make install
```

### Firmwares (Arduino and ESP32)

- PlatformIO Core (CLI) or PlatformIO IDE

```bash
# Install PlatformIO CLI
pip install platformio

# Or with pipx (recommended)
pipx install platformio
```

## Building

### What each command builds

| Command | What it builds | Build system |
|---------|----------------|--------------|
| `make` | Host only (laptop) | CMake |
| `make firmware-arduino` | Arduino only | PlatformIO (via CMake) |
| `make firmware-esp32` | ESP32 only | PlatformIO (via CMake) |
| `make firmware-all` | Arduino + ESP32 | PlatformIO (via CMake) |

**Note:** The root CMake directly builds only the host. Firmwares are built by calling PlatformIO through custom targets.

### Build everything (host + firmwares)

```bash
cd robrain
mkdir build && cd build
cmake ..
make                 # Build host
make firmware-all    # Build firmwares
```

### Host only (Laptop)

```bash
cd robrain
mkdir build && cd build
cmake ..
make
```

The executable is located at `build/host/robrain_host`.

### ESP32 Firmware only

```bash
# Option 1: Direct with PlatformIO
cd robrain/firmware/esp32-robot
pio run

# Option 2: From CMake
cd robrain/build
make firmware-esp32
```

### Arduino Firmware only

```bash
# Option 1: Direct with PlatformIO
cd robrain/firmware/arduino-emg
pio run

# Option 2: For Arduino Mega
pio run -e mega

# Option 3: From CMake
cd robrain/build
make firmware-arduino
```

## Build Options (Host)

### Sanitizers (development/debug only)

Sanitizers help detect bugs at runtime:

```bash
cd robrain/build

# AddressSanitizer (buffer overflow, use-after-free, memory leaks)
cmake -DENABLE_ASAN=ON ..
make

# UndefinedBehaviorSanitizer (integer overflow, null pointer, etc)
cmake -DENABLE_UBSAN=ON ..
make

# ThreadSanitizer (data races, deadlocks)
cmake -DENABLE_TSAN=ON ..
make

# Combine ASan + UBSan
cmake -DENABLE_ASAN=ON -DENABLE_UBSAN=ON ..
make
```

| Sanitizer | Detects | Overhead |
|-----------|---------|----------|
| ASan | Memory leaks, buffer overflow, use-after-free | ~2x slower |
| UBSan | Integer overflow, null pointer, alignment | Minimal |
| TSan | Data races, deadlocks | ~5-10x slower |

**Note:** TSan cannot be combined with ASan (they are incompatible).

### Other options

```bash
# Build tests
cmake -DBUILD_TESTS=ON ..
```

## Upload (Flash to microcontrollers)

### ESP32

1. Connect ESP32 via USB
2. Run:

```bash
# Option 1: Direct with PlatformIO
cd robrain/firmware/esp32-robot
pio run --target upload

# Specify port manually
pio run --target upload --upload-port /dev/ttyUSB0

# Option 2: From CMake
cd robrain/build
make upload-esp32
```

### Arduino

1. Connect Arduino via USB
2. Run:

```bash
# Option 1: Direct with PlatformIO
cd robrain/firmware/arduino-emg
pio run --target upload

# For Arduino Mega
pio run -e mega --target upload

# Specify port
pio run --target upload --upload-port /dev/ttyACM0

# Option 2: From CMake
cd robrain/build
make upload-arduino
```

## Serial Monitor

To view logs and debug output from microcontrollers:

```bash
# ESP32
cd firmware/esp32-robot
pio device monitor

# Arduino
cd firmware/arduino-emg
pio device monitor

# Specify baudrate and port
pio device monitor -b 115200 -p /dev/ttyUSB0
```

## Common Ports

| System | ESP32 | Arduino |
|--------|-------|---------|
| Linux | `/dev/ttyUSB0` | `/dev/ttyACM0` |
| macOS | `/dev/cu.usbserial-*` | `/dev/cu.usbmodem*` |
| Windows | `COM3`, `COM4`, etc. | `COM3`, `COM4`, etc. |

To list available ports:

```bash
pio device list
```

## Developer Environment Setup

After cloning the repo, each developer needs to set up their local environment for LSP (clangd) to work correctly. Both `compile_commands.json` and `.clangd` files contain absolute paths specific to each machine, so they are gitignored and must be generated locally.

### 1. Install dependencies

```bash
# PlatformIO (for firmware builds)
pipx install platformio

# Host dependencies (macOS)
brew install cmake boost

# Host dependencies (Ubuntu/Debian)
sudo apt install cmake build-essential libboost-dev libbluetooth-dev
```

### 2. Build firmware once (installs PlatformIO toolchains)

```bash
cd firmware/arduino-emg && pio run
```

### 3. Generate compile_commands.json

```bash
# Arduino
cd firmware/arduino-emg
pio run -t compiledb

# ESP32
cd firmware/esp32-robot
pio run -t compiledb

# Host (generated automatically by CMake via CMAKE_EXPORT_COMPILE_COMMANDS)
cd build && cmake ..
```

### 4. Generate .clangd configs

```bash
# Option 1: Script
./scripts/setup-clangd.sh

# Option 2: CMake target
cd build && make setup-lsp
```

This generates `.clangd` files for `firmware/arduino-emg/` and `host/` with the correct include paths for your machine (AVR toolchain headers, framework headers, and `common/` protocol headers).

## BLE Configuration (ESP32)

Edit `firmware/esp32-robot/src/comm/ble_handler.h`:

```cpp
constexpr const char* DEVICE_NAME = "ROBRAIN_ROBOT";

// BLE service UUIDs
constexpr const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
constexpr const char* CMD_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
constexpr const char* STATUS_CHAR_UUID = "beb5483f-36e1-4688-b7f5-ea07361b26a8";
```

The ESP32 acts as a BLE server (peripheral). The laptop connects as a client (central).
