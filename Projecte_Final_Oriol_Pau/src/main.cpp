#include <Arduino.h>
#include <WiFi.h>
#include "Buzzer.h"
#include "Alarma.h"
#include "PantallaOLED.h"
#include "SensorPIR.h"
#include "GestorReset.h"
#include "ServidorWeb.h"
#include "TelegramBot.h"
#include <time.h>

// WiFi
const char* ssid = "MOVISTAR_ACF4";
const char* password = "E03E3069BF14EC99CD10";

// Pins
const int PIN_LED_VERD = 10;
const int PIN_LED_GROC = 11;
const int PIN_LED_VERMELL = 12;
const int PIN_BOTO = 9;
const int PIN_PIR = 13;
const int PIN_BOTO_1 = 4;
const int PIN_BOTO_2 = 5;
const int PIN_BOTO_3 = 17;
const int PIN_BUZZER = 8;

// Objectes globals
PantallaOLED pantalla;
Alarma alarma(PIN_LED_VERD, PIN_LED_GROC, PIN_LED_VERMELL, PIN_BOTO, &pantalla);
SensorPIR sensor(PIN_PIR, &alarma);
GestorReset gestorReset(PIN_BOTO_1, PIN_BOTO_2, PIN_BOTO_3, &alarma, &sensor);
ServidorWeb web(&alarma);
Buzzer buzzer(PIN_BUZZER);

// Variables de control alarma
unsigned long tempsIniciAlarma = 0;
bool alarmaAnteriorDisparada = false;

// Variables temporitzador Telegram
unsigned long ultimaConsultaTelegram = 0;
const unsigned long intervalTelegram = 3000; // cada 3 segons

void setup() {
  Serial.begin(115200);
  buzzer.iniciar();

  pantalla.inicialitza();
  alarma.inicialitza();
  sensor.inicialitza();
  gestorReset.inicialitza();
  randomSeed(analogRead(0));

  // Assignar sensor a alarma per període d'espera
  alarma.setSensorPIR(&sensor);

  // Connexió WiFi i web
  web.iniciar(ssid, password);
  alarma.setServidorWeb(&web);

  // Configura la hora NTP
  configTime(7200, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("⌛ Esperant hora...");
  while (time(nullptr) < 100000) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("\n🕒 Hora sincronitzada");

  // Configura Telegram
  configurarTelegram("7781183717:AAExhvGk1YOv1YqXkJ6nSB09ot_rKx9eBOo", "1757602767");
}

void loop() {
  bool araDisparada = alarma.estaActiva() && alarma.estaDisparada();
  unsigned long tempsActual = millis();

  // Telegram: consulta cada 3 segons
  if (tempsActual - ultimaConsultaTelegram >= intervalTelegram) {
    ultimaConsultaTelegram = tempsActual;
    actualitzaTelegram();
  }

  // Si rep codi correcte → desactiva alarma
  if (codiCorrecteRebut()) {
    alarma.desactiva();
    reiniciaCodi();
  }

  // Si rep "activar" → activa alarma
  if (peticioActivarRebuda()) {
    alarma.activa();
    reiniciaPeticioActivar();
  }

  // Control buzzer segons temps d’alarma disparada
  if (araDisparada) {
    if (!alarmaAnteriorDisparada) {
      tempsIniciAlarma = tempsActual;
      alarmaAnteriorDisparada = true;
    }

    unsigned long tempsTranscorregut = tempsActual - tempsIniciAlarma;

    if (tempsTranscorregut < 5000) {
      if ((tempsActual / 1000) % 2 == 0) buzzer.sonar();
      else buzzer.parar();
    } else if (tempsTranscorregut < 10000) {
      if ((tempsActual / 300) % 2 == 0) buzzer.sonar();
      else buzzer.parar();
    } else {
      buzzer.sonar();
    }
  } else {
    buzzer.parar();
    alarmaAnteriorDisparada = false;
  }

  alarma.actualitza();
  sensor.comprovaMoviment(alarma.estaActiva());
  gestorReset.actualitza();
  web.actualitza();

  delay(50);  // Retard curt per estabilitat en lectura del sensor PIR
}
