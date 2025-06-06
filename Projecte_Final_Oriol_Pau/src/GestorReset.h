#ifndef GESTORRESET_H
#define GESTORRESET_H

#include "Alarma.h"
#include "SensorPIR.h"

class GestorReset {
public:
    GestorReset(int p1, int p2, int p3, Alarma* alarma, SensorPIR* sensor);
    void inicialitza();
    void actualitza();

private:
    int _pins[3];
    Alarma* _alarma;
    SensorPIR* _sensor;

    int _seq[4] = {1, 2, 2, 3};
    int _entrada[4];
    int _posicio = 0;
    bool _estatBotons[3];

    void reiniciaSequencia();
};

#endif
