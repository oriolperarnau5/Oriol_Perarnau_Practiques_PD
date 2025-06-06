#ifndef SENSORPIR_H
#define SENSORPIR_H

#include "Alarma.h"

class SensorPIR {
private:
    int _pinPIR;
    Alarma* _alarma;

    bool _esperaActiva = false;
    unsigned long _momentReinici = 0;

public:
    SensorPIR(int pinPIR, Alarma* alarma);

    void inicialitza();

    void iniciarEspera();

    void comprovaMoviment(bool alarmaActiva);
};

#endif
