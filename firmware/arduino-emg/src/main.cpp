// Simple EMG plotter
// Usar con: pio device monitor (o Serial Plotter del Arduino IDE)

#include "HardwareSerial.h"
#include <Arduino.h>

constexpr uint8_t EMG_PIN_0 = A0;
constexpr uint8_t EMG_PIN_1 = A1;
constexpr uint16_t SAMPLE_RATE_HZ = 5;
constexpr uint32_t SAMPLE_INTERVAL_US = 1000000 / SAMPLE_RATE_HZ;  // 500000 us = 0.5 seg

uint32_t last_sample_us = 0;

void setup() {
    Serial.begin(115200);
    pinMode(EMG_PIN_0, INPUT);
    pinMode(EMG_PIN_1, INPUT);
}

// We are detecting low peaks while user has tense muscle
void loop() {
    uint32_t now = micros();
    
    if (now - last_sample_us >= SAMPLE_INTERVAL_US) {
        last_sample_us = now;

        uint16_t ch0 = analogRead(EMG_PIN_0);
        uint16_t ch1 = analogRead(EMG_PIN_1);
      
        Serial.print("A0: ");
        Serial.println(ch0);
        Serial.print("A1: ");
        Serial.println(ch1);
    }
}
