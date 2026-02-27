# Robrain

BCI (Brain Computer Interface) system for robot control using EMG/EEG signals.

- Negativo con Negativo
- Tierra es puente
- Positivo con Positivo

## TODO List pendiente

- (host) Realizar promedio de muestras de forma vectorizada (en el futuro esto seria procesamiento)
- (host) Enviar indicaciones al esp32 via BLE

### Que hacer ahora?

- [X] Serial com protocol
- [X] Arduino-side serial communication
- [X] Host-side serial communication

- [ ] Configuracion de Conexion BLE
- [ ] Host-side wireless communication
- [X] ESP32-side wireless communication

### Paso "intermedio" para tener algo funcionando

- [~] Implementar un procesamiento MUY simple de señales
- [X] Control de motores

### Para el final (Lo mas jodido)

- [~] Filtrar señales
- [ ] Implementar procesamiento ML-based


## Project Structure

```
robrain/
├── CMakeLists.txt
├── common
│   ├── protocol
│   └── types
├── docs
│   ├── CHECKPOINTLOG
│   └── especificaciones_tecnicas.typ
├── firmware
│   ├── arduino-emg
│   └── esp32-robot
├── host
│   ├── CMakeLists.txt
│   ├── lib
│   └── src
├── README.md
└── scripts
    └── setup-clangd.sh
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
- Boost
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

There are multiple targets for flashing devices and compiling code in the root's [cmake file](CMakeLists.txt),
where you can read the command options you have, for each *<command>* you need to run *from the root directory*:

```sh
mkdir build
cd build
cmake ..
make <command>
# eg commands: 
# firmware-esp32
# firmware-arduino
# firmware-all
# upload-ep32
# upload-arduino
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

## Developer Environment Setup

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

### 3. Editor setup

#### Neovim / Emacs (clangd)

These editors use clangd as LSP. Since clangd doesn't natively understand AVR, the project includes a script that generates everything needed for clangd:

```bash
./scripts/setup-clangd.sh
```

1. Generates `compile_commands.json` for firmware (via `pio run -t compiledb`) and host (via `cmake`)
2. Generates `.clangd` files for `firmware/arduino-emg/` and `host/` with the correct include paths, defines, and diagnostic suppressions

#### VSCode

Install these extensions:

- **PlatformIO IDE**
- **CMake Tools**

```bash
# Build host once to generate compile_commands.json
mkdir build && cd build && cmake ..
```

