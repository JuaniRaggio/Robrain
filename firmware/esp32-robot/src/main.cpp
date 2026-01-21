// TODO: Implementar setup() y loop()
#include <Arduino.h>
#include "motor_controller.h"
#include "comm/ble_handler.h"
#include "protocol/command_parser.h"

const int IN1 = 4;   // GPIO 4 - Cable naranja
const int IN2 = 5;   // GPIO 5 - Cable amarillo
const int IN3 = 12;  // GPIO 12 - Cable verde
const int IN4 = 13;  // GPIO 13 - Cable azul

void setup() {
    // TODO
  Serial.begin(115200);
  
  // Configurar pines como salida
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  Serial.println("Test de motores - Solo avanzar");
  delay(2000); // Espera 2 segundos antes de empezar
}

void loop() {
    // TODO
  Serial.println("Motores avanzando...");
  
  // Motor A adelante (IN1 HIGH, IN2 LOW)
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  // Motor B adelante (IN3 HIGH, IN4 LOW)
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  delay(3000); // Avanza 3 segundos
  
  Serial.println("Motores detenidos");
  
  // Detener todo
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  
  delay(2000); // Pausa 2 segundos antes de repetir
}
