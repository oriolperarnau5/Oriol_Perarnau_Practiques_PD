#include <Arduino.h>  
#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"      
#include "freertos/semphr.h"    


#define LED_PIN 2  // Pin donde está conectado el LED


SemaphoreHandle_t xSemaphoreOn, xSemaphoreWait, xSemaphoreOff;  


// Tarea para encender el LED
void turnOnLED(void * parameter) {
  for (;;) {
    if (xSemaphoreTake(xSemaphoreOn, portMAX_DELAY) == pdTRUE) {  
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ENCENDIDO");
      vTaskDelay(1000 / portTICK_PERIOD_MS); // Espera 1 segundo
     
      xSemaphoreGive(xSemaphoreWait);  // Libera el semáforo de espera
    }
  }
}


// Tarea de espera antes de apagar el LED
void waitTask(void * parameter) {
  for (;;) {
    if (xSemaphoreTake(xSemaphoreWait, portMAX_DELAY) == pdTRUE) {  
      Serial.println("ESPERANDO...");
      vTaskDelay(500 / portTICK_PERIOD_MS); // Espera medio segundo
     
      xSemaphoreGive(xSemaphoreOff);  // Libera el semáforo de apagado
    }
  }
}


// Tarea para apagar el LED
void turnOffLED(void * parameter) {
  for (;;) {
    if (xSemaphoreTake(xSemaphoreOff, portMAX_DELAY) == pdTRUE) {  
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED APAGADO");
      vTaskDelay(1000 / portTICK_PERIOD_MS); // Espera 1 segundo antes de repetir
     
      xSemaphoreGive(xSemaphoreOn);  // Libera el semáforo de encendido para reiniciar el ciclo
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
 
  // Crear los tres semáforos
  xSemaphoreOn = xSemaphoreCreateBinary();
  xSemaphoreWait = xSemaphoreCreateBinary();
  xSemaphoreOff = xSemaphoreCreateBinary();


  // Iniciar la primera tarea
  xSemaphoreGive(xSemaphoreOn);


  // Crear tareas
  xTaskCreate(turnOnLED, "Turn On LED", 2048, NULL, 1, NULL);
  xTaskCreate(waitTask, "Wait Task", 2048, NULL, 1, NULL);
  xTaskCreate(turnOffLED, "Turn Off LED", 2048, NULL, 1, NULL);
}


void loop() {
  // No se necesita código en el loop principal
}
