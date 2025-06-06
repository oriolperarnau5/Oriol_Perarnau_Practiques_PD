#ifndef PANTALLAOLED_H
#define PANTALLAOLED_H

#include <Adafruit_SSD1306.h>

class PantallaOLED {
public:
    PantallaOLED();
    void inicialitza();
    void mostrarEstat(bool estatAlarma);
    void mostrarAlertaMoviment();  // 🆕

private:
    Adafruit_SSD1306 _display;
};

#endif
