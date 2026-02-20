#!/bin/bash
# Generates .clangd config files for Arduino and Host projects.
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

    cat > "$PROJECT_ROOT/firmware/arduino-emg/.clangd" <<EOF
CompileFlags:
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

# --- Host ---

cat > "$PROJECT_ROOT/host/.clangd" <<EOF
CompileFlags:
  Add:
    - -std=gnu++17
    - -I${COMMON_DIR}
    - -I${PROJECT_ROOT}/host/src
EOF

echo "Generated: host/.clangd"
