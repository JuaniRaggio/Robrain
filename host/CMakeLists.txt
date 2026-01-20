cmake_minimum_required(VERSION 3.16)
project(robrain_host VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Opciones de compilacion
option(BUILD_TESTS "Build tests" OFF)

# Buscar dependencias
find_package(Threads REQUIRED)

# Boost (para Asio - comunicacion serial)
find_package(Boost 1.70 COMPONENTS system QUIET)

# SimpleBLE (para BLE en Linux/macOS/Windows)
# https://github.com/OpenBluetoothToolbox/SimpleBLE
find_package(simpleble QUIET)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/serial/arduino_comm.cpp
    src/wireless/ble_client.cpp
    src/processing/signal_processor.cpp
    src/protocol/message.cpp
)

# Ejecutable principal
add_executable(robrain_host ${SOURCES})

target_include_directories(robrain_host PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

target_link_libraries(robrain_host PRIVATE
    Threads::Threads
)

# Boost para serial async
if(Boost_FOUND)
    target_link_libraries(robrain_host PRIVATE Boost::system)
    target_compile_definitions(robrain_host PRIVATE USE_BOOST_ASIO)
    message(STATUS "Boost found - using Boost.Asio for serial I/O")
else()
    message(STATUS "Boost not found - using POSIX serial I/O")
endif()

# SimpleBLE para comunicacion BLE
if(simpleble_FOUND)
    target_link_libraries(robrain_host PRIVATE simpleble::simpleble)
    target_compile_definitions(robrain_host PRIVATE USE_SIMPLEBLE)
    message(STATUS "SimpleBLE found - BLE support enabled")
else()
    message(STATUS "SimpleBLE not found - install from https://github.com/OpenBluetoothToolbox/SimpleBLE")
endif()

# Flags de compilacion
target_compile_options(robrain_host PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Instalacion
install(TARGETS robrain_host DESTINATION bin)

# Tests (opcional)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
