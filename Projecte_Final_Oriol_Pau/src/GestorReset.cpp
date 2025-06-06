#include "GestorReset.h"
#include <Arduino.h>

#define ANTIREBOT_DELAY 30  // ms, ajustat per millorar detecció

// Array per guardar l’últim temps que es va considerar vàlida la pulsació per cada botó
unsigned long tempsUltimPress[3] = {0, 0, 0};

GestorReset::GestorReset(int p1, int p2, int p3, Alarma* alarma, SensorPIR* sensor)
: _alarma(alarma), _sensor(sensor) {
    _pins[0] = p1;
    _pins[1] = p2;
    _pins[2] = p3;
    for (int i = 0; i < 3; ++i) _estatBotons[i] = false;
}

void GestorReset::inicialitza() {
    for (int i = 0; i < 3; ++i) {
        pinMode(_pins[i], INPUT_PULLUP);
    }
    reiniciaSequencia();
}

void GestorReset::reiniciaSequencia() {
    _posicio = 0;
    for (int i = 0; i < 4; ++i) _entrada[i] = 0;
}

void GestorReset::actualitza() {
    if (!_alarma->estaDisparada()) return;

    unsigned long ara = millis();

    for (int i = 0; i < 3; ++i) {
        bool premut = digitalRead(_pins[i]) == LOW;

        // Detecta només l’inici de la pulsació (canvi de no premut a premut)
        if (premut && !_estatBotons[i]) {
            // Comprova el temps des de l’última pulsació per evitar rebots
            if (ara - tempsUltimPress[i] > ANTIREBOT_DELAY) {
                tempsUltimPress[i] = ara;

                _estatBotons[i] = true;

                _entrada[_posicio] = i + 1;
                _posicio++;

                Serial.print("Botó premut: ");
                Serial.println(i + 1);

                if (_posicio == 4) {
                    bool correcte = true;
                    for (int j = 0; j < 4; ++j) {
                        if (_entrada[j] != _seq[j]) correcte = false;
                    }

                    if (correcte) {
                        Serial.println("✅ Seqüència correcta! Desactivant alarma...");
                        _alarma->desactiva();
                        _sensor->iniciarEspera();
                    } else {
                        Serial.println("❌ Seqüència incorrecta. Reiniciant seqüència...");
                    }

                    reiniciaSequencia();
                }
            }
        }

        // Quan es deixa anar el botó, o si fa massa temps que està premut, permet detectar noves pulsacions
        if (!premut || (ara - tempsUltimPress[i]) > 1000) {
            _estatBotons[i] = false;
        }
    }
}
