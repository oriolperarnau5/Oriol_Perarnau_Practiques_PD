#include "PantallaOLED.h"
#include <Wire.h>
#include <Arduino.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

PantallaOLED::PantallaOLED()
: _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {}

void PantallaOLED::inicialitza() {
    Wire.begin(2, 1); // SDA = GPIO 2, SCL = GPIO 1

    if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("No s'ha detectat la pantalla OLED.");
        return;
    }

    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(0, 0);
    _display.println("Inicialitzant...");
    _display.display();
    delay(1000);
}

void PantallaOLED::mostrarEstat(bool estatAlarma) {
    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(0, 0);

    _display.println("Alarma:");
    _display.setTextSize(2);
    _display.println(estatAlarma ? "ACTIVA" : "INACTIVA");

    _display.display();
}

void PantallaOLED::mostrarAlertaMoviment() {
    _display.clearDisplay();
    _display.setTextSize(2);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(0, 0);
    _display.println("MOVIMENT!");
    _display.setTextSize(1);
    _display.println("Alerta activada");
    _display.display();
}
