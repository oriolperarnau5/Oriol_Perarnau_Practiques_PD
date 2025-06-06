#include "SensorPIR.h"
#include <Arduino.h>

SensorPIR::SensorPIR(int pinPIR, Alarma* alarma)
    : _pinPIR(pinPIR), _alarma(alarma) {}

void SensorPIR::inicialitza() {
    pinMode(_pinPIR, INPUT);
    _esperaActiva = false;
}

void SensorPIR::iniciarEspera() {
    _esperaActiva = true;
    _momentReinici = millis();
    Serial.println("SensorPIR: Període d'espera iniciat.");
}

void SensorPIR::comprovaMoviment(bool alarmaActiva) {
    if (!alarmaActiva || _alarma->estaDisparada()) return;

    if (_esperaActiva) {
        if (millis() - _momentReinici < 3000) {
            Serial.println("SensorPIR: Esperant període d'espera, no detecta moviment.");
            return;
        }
        _esperaActiva = false;
        Serial.println("SensorPIR: Període d'espera acabat.");
    }

    bool moviment = digitalRead(_pinPIR) == HIGH;

    if (moviment) {
        Serial.println("SensorPIR: Moviment detectat! Disparant alarma.");
        _alarma->dispararAlarma();
    }
}
