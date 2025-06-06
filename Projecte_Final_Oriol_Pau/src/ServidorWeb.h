#ifndef SERVIDORWEB_H
#define SERVIDORWEB_H

#include <WebServer.h>
#include <WiFi.h>
#include <vector>
#include "Alarma.h"

class ServidorWeb {
public:
    ServidorWeb(Alarma* alarma);
    void iniciar(const char* ssid, const char* password);
    void actualitza();
    void afegeixEsdeveniment(const String& text);  // 🆕

private:
    WebServer server;
    Alarma* alarma;
    std::vector<String> historial;

    void handleRoot();
    void handleToggle();
    void handleHistorial();  // 🆕
    bool autenticar();
};

#endif
