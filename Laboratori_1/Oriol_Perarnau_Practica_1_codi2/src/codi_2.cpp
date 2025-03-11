#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


#define LED_PIN 48  // Cambia a 8 si no funciona
#define NUM_LEDS 1  
#define DELAY_TIME 1000 // 1000 ms = 1 segundo


Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
    Serial.begin(115200);
    strip.begin();
    strip.setBrightness(255); // Asegura brillo máximo
    strip.show();
}


void loop() {
    // Enciende LED y muestra mensaje
    strip.setPixelColor(0, strip.Color(255, 255, 255));  // Blanco
    strip.show();
    Serial.println("ON");
    delay(DELAY_TIME);


    // Apaga LED y muestra mensaje
    strip.setPixelColor(0, strip.Color(0, 0, 0));  // Apagado
    strip.show();
    Serial.println("OFF");
    delay(DELAY_TIME);
}
