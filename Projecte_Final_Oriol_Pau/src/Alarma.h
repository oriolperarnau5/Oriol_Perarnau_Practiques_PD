#ifndef ALARMA_H
#define ALARMA_H

#include "PantallaOLED.h"

// Predeclaració per evitar dependència circular
class ServidorWeb;
class SensorPIR;  // Afegit predeclaració

class Alarma {
public:
    Alarma(int pinVerd, int pinGroc, int pinVermell, int pinBoto, PantallaOLED* pantalla);

    void activa();
    void desactiva();

    void inicialitza();
    void actualitza();

    bool estaActiva() const;
    bool estaDisparada() const;

    void dispararAlarma();
    void resetAlarmaDisparada(); // 🆕 Reset manual via botó

    void setServidorWeb(ServidorWeb* web); // 🆕
    void setSensorPIR(SensorPIR* sensor);  // 🆕 Setter per assignar sensor PIR

private:
    int _pinVerd, _pinGroc, _pinVermell, _pinBoto;
    bool _estatAlarma;
    bool _alarmaDisparada;
    bool _botoPrement;

    PantallaOLED* _pantalla;
    ServidorWeb* _web = nullptr; // 🆕
    SensorPIR* _sensor = nullptr; // 🆕 punter al sensor PIR

    void actualitzaLeds();
    void mostrarEstatActual(); // 🆕 Per mostrar pantalla i LEDs d’un sol cop
};

#endif
