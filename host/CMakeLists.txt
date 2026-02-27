cmake_minimum_required(VERSION 3.16)
project(robrain_host VERSION 0.1.0 LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Opciones de compilacion
option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)

# Buscar dependencias
find_package(Threads REQUIRED)

# Boost (para Asio - comunicacion serial)
find_package(Boost 1.70 COMPONENTS system QUIET)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/serial/arduino_comm.cpp
    src/wireless/ble_client.mm
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

# CoreBluetooth (macOS nativo via Objective-C++)
target_link_libraries(robrain_host PRIVATE
    "-framework CoreBluetooth"
    "-framework Foundation"
)
message(STATUS "Using CoreBluetooth for BLE communication")

# Flags de compilacion
target_compile_options(robrain_host PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Sanitizers (solo para desarrollo/debug)
if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    target_compile_options(robrain_host PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(robrain_host PRIVATE -fsanitize=address)
endif()

if(ENABLE_UBSAN)
    message(STATUS "UndefinedBehaviorSanitizer enabled")
    target_compile_options(robrain_host PRIVATE -fsanitize=undefined)
    target_link_options(robrain_host PRIVATE -fsanitize=undefined)
endif()

if(ENABLE_TSAN)
    message(STATUS "ThreadSanitizer enabled")
    target_compile_options(robrain_host PRIVATE -fsanitize=thread)
    target_link_options(robrain_host PRIVATE -fsanitize=thread)
endif()

# Instalacion
install(TARGETS robrain_host DESTINATION bin)

# Tests (opcional)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
