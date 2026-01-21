// Simple EMG plotter
// Usar con: pio device monitor (o Serial Plotter del Arduino IDE)

#include "HardwareSerial.h"
#include <Arduino.h>

constexpr uint8_t EMG_PIN_1 = A0;
constexpr uint16_t SAMPLE_RATE_HZ = 5;
constexpr uint32_t SAMPLE_INTERVAL_US = 1000000 / SAMPLE_RATE_HZ;  // 500000 us = 0.5 seg

uint32_t last_sample_us = 0;

// int emgPin = A0;
// int emgValue = 0;

void setup() {
    Serial.begin(115200);
    pinMode(EMG_PIN_1, INPUT);
}

// We are detecting low peaks while user has tense muscle
void loop() {
  // emgValue = analogRead(emgPin);
  // Serial.println(emgValue);
  // delay(100);
    uint32_t now = micros();
    
    if (now - last_sample_us >= SAMPLE_INTERVAL_US) {
        last_sample_us = now;

        uint16_t ch1 = analogRead(EMG_PIN_1);
      
        // Formato para Serial Plotter: "valor1,valor2"
        Serial.print("A0: ");
        Serial.println(ch1);
        // Serial.print(",");
        // Serial.println(ch2);
    }
}
