#!/bin/bash
# Sets up LSP environment: generates compile_commands.json and .clangd configs.
# Each developer should run this after cloning and installing dependencies.
#
# Usage:
#   ./scripts/setup-clangd.sh
#   -- or --
#   cd build && make setup-lsp

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
COMMON_DIR="$PROJECT_ROOT/common"

# --- Generate compile_commands.json ---

echo "Generating compile_commands.json..."

# Firmware
if command -v pio &> /dev/null; then
    pio run -t compiledb -d "$PROJECT_ROOT/firmware/arduino-emg" 2>/dev/null
    echo "Generated: firmware/arduino-emg/compile_commands.json"
    pio run -t compiledb -d "$PROJECT_ROOT/firmware/esp32-robot" 2>/dev/null
    echo "Generated: firmware/esp32-robot/compile_commands.json"
else
    echo "Warning: pio not found, skipping firmware compile_commands.json"
fi

# Host
mkdir -p "$PROJECT_ROOT/build"
cmake -S "$PROJECT_ROOT" -B "$PROJECT_ROOT/build" > /dev/null 2>&1
echo "Generated: build/compile_commands.json"

# --- Arduino EMG ---

AVR_TOOLCHAIN="$HOME/.platformio/packages/toolchain-atmelavr"
AVR_FRAMEWORK="$HOME/.platformio/packages/framework-arduino-avr"

if [ ! -d "$AVR_TOOLCHAIN" ]; then
    echo "Warning: AVR toolchain not found at $AVR_TOOLCHAIN"
    echo "Run 'pio run' in firmware/arduino-emg/ first to install dependencies."
else
    GCC_VERSION=$(ls "$AVR_TOOLCHAIN/lib/gcc/avr/" | head -1)
    GCC_INCLUDE="$AVR_TOOLCHAIN/lib/gcc/avr/$GCC_VERSION/include"
    AVR_INCLUDE="$AVR_TOOLCHAIN/avr/include"

    AVR_GXX="$AVR_TOOLCHAIN/bin/avr-g++"

    cat > "$PROJECT_ROOT/firmware/arduino-emg/.clangd" <<EOF
CompileFlags:
  Compiler: ${AVR_GXX}
  Add:
    - -std=gnu++17
    - -D__AVR_ATmega328P__
    - -DF_CPU=16000000L
    - -DARDUINO=10808
    - -DARDUINO_AVR_UNO
    - -DARDUINO_ARCH_AVR
    - -DPLATFORMIO
    - -I${COMMON_DIR}
    - -Isrc
    - -I${AVR_FRAMEWORK}/cores/arduino
    - -I${AVR_FRAMEWORK}/variants/standard
    - -isystem${AVR_INCLUDE}
    - -isystem${GCC_INCLUDE}
  Remove:
    - -std=gnu++11
    - -std=c++11
    - -mmcu=*
    - -flto
    - -fno-fat-lto-objects

Diagnostics:
  Suppress:
    - pp_expr_bad_token_start_expr
    - drv_unsupported_opt_for_target

---

If:
  PathMatch: test/.*
CompileFlags:
  Add:
    - -Itest/mock
    - -I.pio/libdeps/native/Unity/src
  Remove:
    - -I${AVR_FRAMEWORK}/cores/arduino
    - -I${AVR_FRAMEWORK}/variants/standard
EOF

    echo "Generated: firmware/arduino-emg/.clangd"
fi

# --- ESP32 Robot ---

ESP_TOOLCHAIN="$HOME/.platformio/packages/toolchain-xtensa-esp32"
ESP_FRAMEWORK="$HOME/.platformio/packages/framework-arduinoespressif32"

if [ ! -d "$ESP_TOOLCHAIN" ]; then
    echo "Warning: ESP32 toolchain not found at $ESP_TOOLCHAIN"
    echo "Run 'pio run' in firmware/esp32-robot/ first to install dependencies."
else
    ESP_GCC_VERSION=$(ls "$ESP_TOOLCHAIN/lib/gcc/xtensa-esp32-elf/" | head -1)
    ESP_GCC_INCLUDE="$ESP_TOOLCHAIN/lib/gcc/xtensa-esp32-elf/$ESP_GCC_VERSION/include"
    ESP_GXX="$ESP_TOOLCHAIN/bin/xtensa-esp32-elf-g++"

    cat > "$PROJECT_ROOT/firmware/esp32-robot/.clangd" <<EOF
CompileFlags:
  Compiler: ${ESP_GXX}
  Add:
    - -std=gnu++17
    - -DPLATFORMIO
    - -DARDUINO_ESP32_DEV
    - -DARDUINO_ARCH_ESP32
    - -DARDUINO=10808
    - -DESP32
    - -DCORE_DEBUG_LEVEL=3
    - -I${COMMON_DIR}
    - -Isrc
    - -I${ESP_FRAMEWORK}/cores/esp32
    - -I${ESP_FRAMEWORK}/variants/esp32
    - -isystem${ESP_GCC_INCLUDE}
  Remove:
    - -std=gnu++11
    - -std=c++11
    - -mlongcalls
    - -fstrict-volatile-bitfields
    - -fno-tree-switch-conversion
    - -fno-rtti
    - -ffunction-sections
    - -fdata-sections
    - -Wno-frame-address
    - -fstack-protector
    - -fexceptions
    - -Wno-error=*
    - -Wno-missing-field-initializers
    - -Wno-old-style-declaration
    - -Werror=all

Diagnostics:
  Suppress:
    - attribute_section_invalid_for_target
    - pp_hash_error
    - drv_unknown_argument
    - drv_unsupported_opt_for_target

---

If:
  PathMatch: test/.*
CompileFlags:
  Add:
    - -Itest/mock
    - -I.pio/libdeps/native/Unity/src
  Remove:
    - -I${ESP_FRAMEWORK}/cores/esp32
    - -I${ESP_FRAMEWORK}/variants/esp32
EOF

    echo "Generated: firmware/esp32-robot/.clangd"
fi

# --- Host ---

cat > "$PROJECT_ROOT/host/.clangd" <<EOF
CompileFlags:
  Add:
    - -std=gnu++17
    - -I${COMMON_DIR}
    - -I${PROJECT_ROOT}/host/src
EOF

echo "Generated: host/.clangd"
