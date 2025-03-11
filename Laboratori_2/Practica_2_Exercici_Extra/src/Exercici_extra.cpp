#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


// Configuración del NeoPixel
#define LED_PIN 48       // Pin donde está conectado el NeoPixel
#define NUMPIXELS 1     // Número de LEDs en la tira (en este caso, 1 LED)
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);


// Configuración de los pulsadores
#define BUTTON_UP_PIN 18    // Pin para aumentar la frecuencia
#define BUTTON_DOWN_PIN 19  // Pin para disminuir la frecuencia


// Variables para el control de la frecuencia de parpadeo
volatile uint32_t blinkInterval = 500; // Frecuencia inicial de parpadeo (500 ms)
volatile bool updateBlink = false;     // Bandera para actualizar el parpadeo


// Variables para el filtrado de rebotes
volatile bool buttonUpPressed = false;
volatile bool buttonDownPressed = false;
uint32_t lastDebounceTime = 0;
const uint32_t debounceDelay = 50; // Tiempo de filtrado de rebotes (50 ms)


// Configuración del timer
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


// Función de servicio de interrupción (ISR) para el timer
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);


  // Leer los pulsadores con filtrado de rebotes
  if (digitalRead(BUTTON_UP_PIN) == LOW) {
    if (!buttonUpPressed && (millis() - lastDebounceTime) > debounceDelay) {
      buttonUpPressed = true;
      lastDebounceTime = millis();
      if (blinkInterval > 50) { // Limitar la frecuencia mínima
        blinkInterval = blinkInterval/2;    // Aumentar la frecuencia (reducir el intervalo)
        updateBlink = true;
      }
    }
  } else {
    buttonUpPressed = false;
  }


  if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
    if (!buttonDownPressed && (millis() - lastDebounceTime) > debounceDelay) {
      buttonDownPressed = true;
      lastDebounceTime = millis();
      if (blinkInterval < 10000) { // Limitar la frecuencia máxima
        blinkInterval = blinkInterval *2;     // Disminuir la frecuencia (aumentar el intervalo)
        updateBlink = true;
      }
    }
  } else {
    buttonDownPressed = false;
  }


  portEXIT_CRITICAL_ISR(&timerMux);
}


void setup() {
  Serial.begin(115200);


  // Configurar los pines de los pulsadores como entradas con pull-up
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);


  // Inicializar el NeoPixel
  strip.begin();
  strip.show(); // Apagar todos los LEDs al inicio


  // Configurar el timer para leer los pulsadores
  timer = timerBegin(0, 80, true); // Temporizador 0, preescalador de 80, contar hacia arriba
  timerAttachInterrupt(timer, &onTimer, true); // Adjuntar la ISR al temporizador
  timerAlarmWrite(timer, 10000, true); // Configurar alarma para 10,000 microsegundos (10 ms)
  timerAlarmEnable(timer); // Habilitar la alarma
}


void loop() {
  static uint32_t lastBlinkTime = 0;
  static bool ledState = false;


  // Controlar el parpadeo del NeoPixel
  if (millis() - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = millis();
    ledState = !ledState; // Alternar el estado del LED


    if (ledState) {
      strip.setPixelColor(0, strip.Color(255, 0, 0)); // Encender en rojo
    } else {
      strip.setPixelColor(0, strip.Color(0, 0, 0)); // Apagar
    }
    strip.show();
  }


  // Mostrar la frecuencia actual en el monitor serie
  if (updateBlink) {
    updateBlink = false;
    Serial.printf("Nuevo intervalo de parpadeo: %u ms\n", blinkInterval);
  }
}