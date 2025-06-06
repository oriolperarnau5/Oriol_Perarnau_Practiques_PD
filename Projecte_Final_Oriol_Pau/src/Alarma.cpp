#include "Alarma.h"
#include "ServidorWeb.h"
#include "SensorPIR.h"
#include <Arduino.h>
#include "TelegramBot.h"   // Per enviar missatges Telegram

Alarma::Alarma(int pinVerd, int pinGroc, int pinVermell, int pinBoto, PantallaOLED* pantalla)
  : _pinVerd(pinVerd), _pinGroc(pinGroc), _pinVermell(pinVermell), _pinBoto(pinBoto),
    _estatAlarma(true), _alarmaDisparada(false), _botoPrement(false), _pantalla(pantalla),
    _web(nullptr), _sensor(nullptr) {}

void Alarma::setSensorPIR(SensorPIR* sensor) {
    _sensor = sensor;
}

void Alarma::inicialitza() {
    pinMode(_pinVerd, OUTPUT);
    pinMode(_pinGroc, OUTPUT);
    pinMode(_pinVermell, OUTPUT);
    pinMode(_pinBoto, INPUT_PULLUP);
    actualitzaLeds();

    if (_pantalla) {
        _pantalla->mostrarEstat(_estatAlarma);
    }
}

void Alarma::actualitza() {
    bool botoPremut = digitalRead(_pinBoto) == LOW;

    if (botoPremut && !_botoPrement) {
        _botoPrement = true;

        if (_alarmaDisparada) {
            resetAlarmaDisparada();
        } else {
            _estatAlarma = !_estatAlarma;
            mostrarEstatActual();

            if (_web) {
                _web->afegeixEsdeveniment(_estatAlarma
                    ? "🔔 Alarma activada (botó físic)"
                    : "🔕 Alarma desactivada (botó físic)");
            }

            Serial.println(_estatAlarma ? "Alarma ACTIVADA (LED VERD)" : "Alarma DESACTIVADA (LED GROC)");

            // Si s'activa l'alarma des del botó, iniciem període d'espera al sensor
            if (_estatAlarma && _sensor != nullptr) {
                _sensor->iniciarEspera();
            }
        }
    }

    if (!botoPremut) {
        _botoPrement = false;
    }
}

void Alarma::actualitzaLeds() {
    if (_alarmaDisparada) {
        digitalWrite(_pinVerd, LOW);
        digitalWrite(_pinGroc, LOW);
        digitalWrite(_pinVermell, HIGH);
    } else {
        digitalWrite(_pinVerd, _estatAlarma ? HIGH : LOW);
        digitalWrite(_pinGroc, _estatAlarma ? LOW : HIGH);
        digitalWrite(_pinVermell, LOW);
    }
}

bool Alarma::estaActiva() const {
    return _estatAlarma;
}

bool Alarma::estaDisparada() const {
    return _alarmaDisparada;
}

void Alarma::dispararAlarma() {
    if (!_alarmaDisparada) {
        _alarmaDisparada = true;
        actualitzaLeds();

        if (_pantalla) {
            _pantalla->mostrarAlertaMoviment();
        }

        if (_web) {
            _web->afegeixEsdeveniment("🚨 Alarma disparada per moviment");
        }

        Serial.println("🔴 Alarma disparada!");

        int codi = random(1000, 9999);
        enviarMissatgeAmbCodi(codi);
    }
}

void Alarma::resetAlarmaDisparada() {
    _alarmaDisparada = false;
    mostrarEstatActual();
    Serial.println("🔁 Alarma resetejada");
}

void Alarma::mostrarEstatActual() {
    actualitzaLeds();
    if (_pantalla) {
        _pantalla->mostrarEstat(_estatAlarma);
    }
}

void Alarma::activa() {
    _estatAlarma = true;
    _alarmaDisparada = false;
    mostrarEstatActual();
    Serial.println("✅ Alarma activada manualment");

    if (_sensor != nullptr) {
        _sensor->iniciarEspera();
    }

    if (_web) _web->afegeixEsdeveniment("🔔 Alarma activada (web)");
}

void Alarma::desactiva() {
    _estatAlarma = false;
    _alarmaDisparada = false;
    mostrarEstatActual();
    Serial.println("🟡 Alarma desactivada manualment");

    if (_web) _web->afegeixEsdeveniment("🔕 Alarma desactivada (web)");
}

void Alarma::setServidorWeb(ServidorWeb* web) {
    _web = web;
}
