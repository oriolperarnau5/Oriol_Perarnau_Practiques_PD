#include <Arduino.h>
#define LED_PIN 48  // Cambia este pin por uno disponible en tu ESP32-S3
void setup() {
   pinMode(LED_PIN, OUTPUT);
   Serial.begin(115200);
}
// 1. Con Serial y usando funciones de Arduino
void loop() {
   Serial.println("ON");
   digitalWrite(LED_PIN, HIGH);
   Serial.println("OFF");
   digitalWrite(LED_PIN, LOW);
}
