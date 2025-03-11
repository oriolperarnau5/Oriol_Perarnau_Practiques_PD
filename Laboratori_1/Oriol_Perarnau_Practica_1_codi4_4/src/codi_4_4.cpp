#include <Arduino.h>
#define LED_PIN 48  // Cambia este pin por uno disponible en tu ESP32-S3
void setup() {
   pinMode(LED_PIN, OUTPUT);
   Serial.begin(115200);
}
void loop() {
   GPIO.out_w1ts = (1 << LED_PIN);
   GPIO.out_w1tc = (1 << LED_PIN);
}