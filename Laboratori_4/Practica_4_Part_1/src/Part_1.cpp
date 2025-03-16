#include <Arduino.h>   // Necessari per a Serial, delay, etc.
#include "freertos/FreeRTOS.h"  // Per utilitzar FreeRTOS amb l'ESP32
#include "freertos/task.h"      // Per a xTaskCreate, vTaskDelete


// Declaració prèvia de la funció
void anotherTask(void * parameter);


void setup() {
  Serial.begin(115200);  // Inicia la comunicació sèrie
  /* Creació d'una nova tasca */
  xTaskCreate(
    anotherTask,   /* Funció de la tasca */
    "another Task", /* Nom de la tasca */
    10000,         /* Mida de la pila */
    NULL,          /* Paràmetre de la tasca */
    1,             /* Prioritat de la tasca */
    NULL           /* Identificador de la tasca */
  );
}


void loop() {
  Serial.println("this is ESP32 Task");
  delay(1000);  // Espera 1 segon
}


/* Implementació de la funció de la tasca */
void anotherTask(void * parameter) {
  for (;;) {
    Serial.println("this is another Task");
    delay(1000);  // Espera 1 segon
  }
  vTaskDelete(NULL);  // Mai s'executarà perquè el bucle és infinit
}
