#include "Buzzer.h"
#include <Arduino.h>

Buzzer::Buzzer(int pin) {
    this->pin = pin;
}

void Buzzer::iniciar() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Buzzer::sonar() {
    digitalWrite(pin, HIGH);
}

void Buzzer::parar() {
    digitalWrite(pin, LOW);
}
