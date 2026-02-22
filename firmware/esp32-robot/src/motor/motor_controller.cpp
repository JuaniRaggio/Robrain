#include "motor_controller.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

constexpr uint8_t QUEUE_SIZE = 10;
constexpr uint32_t TASK_STACK_SIZE  = 2048;

namespace motor {

//BLE handler (producer) & motor task (consumer)
static QueueHandle_t cmd_queue = nullptr;

WheelPair::WheelPair()
    : left_ (PIN_LEFT_IN1,  PIN_LEFT_IN2,  PWM_CHANNEL_LEFT),
      right_(PIN_RIGHT_IN1, PIN_RIGHT_IN2, PWM_CHANNEL_RIGHT) {}

void WheelPair::init() {
    left_.init();
    right_.init();
}

void WheelPair::execute(const robrain::WheelCommand &cmd) {
    cmd.left  == 0 ? left_.stop()  : left_.forward(cmd.left);
    cmd.right == 0 ? right_.stop() : right_.forward(cmd.right);
}

void WheelPair::stop() {  
    left_.stop();
    right_.stop();
}


// Motor Task (corre en Core 0) --> esto es de freeRTOS. podemos no usarlo y cambia el codigo. CHECK

static void motor_task(void *param) {
    WheelPair *pair = static_cast<WheelPair *>(param);
    robrain::WheelCommand cmd;

    while (true) {
        // Espera hasta MOTOR_TIMEOUT_MS por un comando.
        if (xQueueReceive(cmd_queue, &cmd, pdMS_TO_TICKS(MOTOR_TIMEOUT_MS))) {
            pair->execute(cmd);
        } else {
            // Si no llega comando hasta ese tiempo --> para.(safety timeout. Lo podemos omitir pero es mejor en cuanto a eficiencia) CHECK
            pair->stop();
        }
    }
}

void start_motor_task(WheelPair &pair) {

    cmd_queue = xQueueCreate(QUEUE_SIZE, sizeof(robrain::WheelCommand));

    // Lanzar la task en Core 0 con 2KB de stack
    xTaskCreatePinnedToCore(
        motor_task,  // funcion de la task
        "MotorTask", // nombre (para debug)
        TASK_STACK_SIZE,        // stack en bytes
        &pair,       // parametro → WheelPair*
        1,           // prioridad
        nullptr,     // handle (no lo necesitamos)
        0            // Core 0
    );
}

bool send_command(const robrain::WheelCommand &cmd) {
    if (cmd_queue == nullptr) return false;
    return xQueueSend(cmd_queue, &cmd, 0) == pdTRUE;  //xQueueSend(queue, item, ticksToWait)
}

} // namespace motor