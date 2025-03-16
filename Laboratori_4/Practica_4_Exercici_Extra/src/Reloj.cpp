/*
* ESP32 RTOS - Reloj Digital con Pulsadores y LEDs
*  
* Este proyecto implementa un reloj digital utilizando FreeRTOS en un ESP32.
* Características:
* - Muestra horas, minutos y segundos
* - Permite ajustar el tiempo mediante pulsadores
* - Utiliza LEDs para indicar estados del reloj
* - Implementa múltiples tareas concurrentes con RTOS
*/
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
// Definición de pines
#define LED_SEGUNDOS 2  // LED que parpadea cada segundo
#define LED_MODO 4      // LED que indica modo de ajuste
#define BTN_MODO 16     // Botón para cambiar modo (normal/ajuste hora/ajuste minutos)
#define BTN_INCREMENTO 17 // Botón para incrementar valor en modo ajuste
// Variables para el reloj
volatile int horas = 0;
volatile int minutos = 0;
volatile int segundos = 0;
volatile int modo = 0;  // 0: modo normal, 1: ajuste horas, 2: ajuste minutos
// Variables FreeRTOS
QueueHandle_t botonQueue;
SemaphoreHandle_t relojMutex;
// Estructura para los eventos de botones
typedef struct {
uint8_t boton;
uint32_t tiempo;
} EventoBoton;
// Prototipos de tareas
void TareaReloj(void *pvParameters);
void TareaLecturaBotones(void *pvParameters);
void TareaActualizacionDisplay(void *pvParameters);
void TareaControlLEDs(void *pvParameters);
// Función para manejar interrupciones de botones
void IRAM_ATTR ISR_Boton(void *arg) {
uint8_t numeroPulsador = (uint32_t)arg;
   
  // Crear un evento para el botón
  EventoBoton evento;
  evento.boton = numeroPulsador;
  evento.tiempo = xTaskGetTickCountFromISR();
   
  // Enviar evento a la cola
  xQueueSendFromISR(botonQueue, &evento, NULL);
}
 
void setup() {
  // Inicializar puerto serie
  Serial.begin(115200);
  Serial.println("Inicializando Reloj Digital con RTOS");
   
  // Configurar pines
  pinMode(LED_SEGUNDOS, OUTPUT);
  pinMode(LED_MODO, OUTPUT);
  pinMode(BTN_MODO, INPUT_PULLUP);
  pinMode(BTN_INCREMENTO, INPUT_PULLUP);
   
  // Crear recursos RTOS
  botonQueue = xQueueCreate(10, sizeof(EventoBoton));
  relojMutex = xSemaphoreCreateMutex();
   
  // Configurar interrupciones para los botones
  attachInterruptArg(BTN_MODO, ISR_Boton, (void*)BTN_MODO, FALLING);
  attachInterruptArg(BTN_INCREMENTO, ISR_Boton, (void*)BTN_INCREMENTO,
FALLING);
   
  // Crear tareas
  xTaskCreate(
    TareaReloj,               // Función que implementa la tarea
    "RelojTask",              // Nombre de la tarea
    2048,                     // Tamaño de la pila en palabras
    NULL,                     // Parámetros de entrada
    1,                        // Prioridad
    NULL                      // Handle de la tarea
  );
   
  xTaskCreate(
    TareaLecturaBotones,
    "BotonesTask",
    2048,
    NULL,
    2,                        // Mayor prioridad para respuesta rápida
    NULL
  );
   
  xTaskCreate(
    TareaActualizacionDisplay,
    "DisplayTask",
    2048,
    NULL,
    1,
    NULL
  );
   
  xTaskCreate(
    TareaControlLEDs,
    "LEDsTask",
    1024,
    NULL,
    1,
    NULL
  );
   
  // El planificador RTOS se inicia automáticamente
}
 
void loop() {
  // loop() queda vacío en aplicaciones RTOS
  // Todo el trabajo se hace en las tareas
  vTaskDelay(portMAX_DELAY);
}
 
// Tarea que actualiza el tiempo del reloj
void TareaReloj(void *pvParameters) {
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(1000); // Periodo de 1 segundo
   
  // Inicializar xLastWakeTime con el tiempo actual
  xLastWakeTime = xTaskGetTickCount();
   
  for (;;) {
    // Esperar hasta el siguiente periodo
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
     
    // Obtener semáforo para acceder a las variables del reloj
    if (xSemaphoreTake(relojMutex, portMAX_DELAY) == pdTRUE) {
      // Actualizar solo en modo normal
      if (modo == 0) {
        segundos++;
        if (segundos >= 60) {
          segundos = 0;
          minutos++;
          if (minutos >= 60) {
            minutos = 0;
            horas++;
            if (horas >= 24) {
              horas = 0;
            }
          }
        }
      }
       
      // Liberar el semáforo
      xSemaphoreGive(relojMutex);
    }
  }
}
 
// Tarea que gestiona los botones y sus acciones
void TareaLecturaBotones(void *pvParameters) {
  EventoBoton evento;
  uint32_t ultimoTiempoBoton = 0;
  const uint32_t debounceTime = pdMS_TO_TICKS(300); // Tiempo anti-rebote
   
  for (;;) {
    // Esperar a que llegue un evento desde la interrupción
    if (xQueueReceive(botonQueue, &evento, portMAX_DELAY) == pdPASS) {
      // Verificar si ha pasado suficiente tiempo desde la última pulsación (anti-rebote)
      if ((evento.tiempo - ultimoTiempoBoton) >= debounceTime) {
        // Obtener semáforo para modificar variables del reloj
        if (xSemaphoreTake(relojMutex, portMAX_DELAY) == pdTRUE) {
          // Procesar el evento según el botón
          if (evento.boton == BTN_MODO) {
            // Cambiar modo
            modo = (modo + 1) % 3;
            Serial.printf("Cambio de modo: %d\n", modo);
          }  
          else if (evento.boton == BTN_INCREMENTO) {
            // Incrementar valor según el modo actual
            if (modo == 1) { // Ajuste de horas
              horas = (horas + 1) % 24;
              Serial.printf("Horas ajustadas a: %d\n", horas);
            }  
            else if (modo == 2) { // Ajuste de minutos
              minutos = (minutos + 1) % 60;
              segundos = 0; // Reiniciar segundos al cambiar minutos
              Serial.printf("Minutos ajustados a: %d\n", minutos);
            }
          }
           
          // Liberar el semáforo
          xSemaphoreGive(relojMutex);
        }
         
        // Actualizar tiempo de la última pulsación
        ultimoTiempoBoton = evento.tiempo;
      }
    }
  }
}
 
// Tarea que actualiza la información en el display (puerto serie en este caso)
void TareaActualizacionDisplay(void *pvParameters) {
  int horasAnterior = -1;
  int minutosAnterior = -1;
  int segundosAnterior = -1;
  int modoAnterior = -1;
   
  for (;;) {
    // Obtener semáforo para leer variables del reloj
    if (xSemaphoreTake(relojMutex, portMAX_DELAY) == pdTRUE) {
      // Verificar si hay cambios que mostrar
      bool cambios = (horas != horasAnterior) ||  
                     (minutos != minutosAnterior) ||  
                     (segundos != segundosAnterior) ||
                     (modo != modoAnterior);
       
      if (cambios) {
        // Imprimir información del reloj
        Serial.printf("%02d:%02d:%02d", horas, minutos, segundos);
         
        // Mostrar el modo actual
        if (modo == 0) {
          Serial.println(" [Modo Normal]");
        } else if (modo == 1) {
          Serial.println(" [Ajuste Horas]");
        } else if (modo == 2) {
          Serial.println(" [Ajuste Minutos]");
        }
         
        // Actualizar valores anteriores
        horasAnterior = horas;
        minutosAnterior = minutos;
        segundosAnterior = segundos;
        modoAnterior = modo;
      }
       
      // Liberar el semáforo
      xSemaphoreGive(relojMutex);
    }
     
    // Esperar antes de la siguiente actualización
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
 
// Tarea que controla los LEDs según el estado del reloj
void TareaControlLEDs(void *pvParameters) {
  bool estadoLedSegundos = false;
   
  for (;;) {
    // Obtener semáforo para leer variables del reloj
    if (xSemaphoreTake(relojMutex, portMAX_DELAY) == pdTRUE) {
      // LED de segundos parpadea cada segundo
      if (segundos != estadoLedSegundos) {
        estadoLedSegundos = !estadoLedSegundos;
        digitalWrite(LED_SEGUNDOS, estadoLedSegundos);
      }
       
      // LED de modo se enciende solo en modo ajuste
      digitalWrite(LED_MODO, modo > 0);
       
      // Liberar el semáforo
      xSemaphoreGive(relojMutex);
    }
     
    // Esperar antes de la siguiente actualización
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
