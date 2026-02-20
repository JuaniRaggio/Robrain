cmake_minimum_required(VERSION 3.16)
project(robrain VERSION 0.1.0 LANGUAGES CXX)

# Este es el CMakeLists raiz del proyecto
# Solo compila el componente host (laptop)
# Los firmwares ESP32 y Arduino se compilan con PlatformIO

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Subdirectorios
add_subdirectory(host)

# Target para compilar firmwares con PlatformIO (conveniente)
add_custom_target(firmware-esp32
    COMMAND platformio run
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/firmware/esp32-robot
    COMMENT "Building ESP32 firmware with PlatformIO"
)

add_custom_target(firmware-arduino
    COMMAND platformio run
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/firmware/arduino-emg
    COMMENT "Building Arduino firmware with PlatformIO"
)

add_custom_target(firmware-all
    DEPENDS firmware-esp32 firmware-arduino
    COMMENT "Building all firmwares"
)

# Target para upload de firmwares
add_custom_target(upload-esp32
    COMMAND platformio run --target upload
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/firmware/esp32-robot
    COMMENT "Uploading ESP32 firmware"
)

add_custom_target(upload-arduino
    COMMAND platformio run --target upload
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/firmware/arduino-emg
    COMMENT "Uploading Arduino firmware"
)

# Target para generar .clangd configs (por-dev, necesita PlatformIO instalado)
add_custom_target(setup-lsp
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/setup-clangd.sh
    COMMENT "Generating .clangd configs for all modules"
)

# Informacion del proyecto
message(STATUS "")
message(STATUS "=== Robrain Project ===")
message(STATUS "")
message(STATUS "Componentes:")
message(STATUS "  - host/        : Aplicacion de laptop (CMake)")
message(STATUS "  - firmware/esp32-robot/  : Firmware ESP32 (PlatformIO)")
message(STATUS "  - firmware/arduino-emg/  : Firmware Arduino (PlatformIO)")
message(STATUS "")
message(STATUS "Para compilar firmwares: make firmware-all")
message(STATUS "Para subir ESP32: make upload-esp32")
message(STATUS "Para subir Arduino: make upload-arduino")
message(STATUS "")
