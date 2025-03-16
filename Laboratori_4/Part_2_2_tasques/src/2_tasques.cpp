#include <Arduino.h>  
#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"      


#define LED_PIN 2  // Pin donde está conectado el LED


// Tarea para alternar el estado del LED
void toggleLED(void * parameter) {
  for (;;) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ENCENDIDO");
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Espera 1 segundo


    digitalWrite(LED_PIN, LOW);
    Serial.println("LED APAGADO");
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Espera 1 segundo
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);


  // Crear una única tarea que alterna el LED
  xTaskCreate(toggleLED, "Toggle LED", 2048, NULL, 1, NULL);
}


void loop() {
  // No se necesita código en el loop principal
}
