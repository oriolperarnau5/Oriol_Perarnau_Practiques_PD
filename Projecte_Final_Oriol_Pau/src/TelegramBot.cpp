#include "TelegramBot.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

static String botToken;
static String chatID;
static String codiActual = "";
static bool codiRebutCorrectament = false;
static bool activarRebut = false;
static unsigned long ultimUpdateId = 0;

// WiFiClientSecure amb buffer ampliat a 8192 bytes
static WiFiClientSecure secureClient(8192);

static bool esperantResposta = false;
static unsigned long tempsIniciPeticio = 0;
static const unsigned long timeoutPeticio = 5000; // 5 segons timeout
static String respostaTelegram = "";

void configurarTelegram(const char* token, const char* chatId) {
    botToken = String(token);
    chatID = String(chatId);
    secureClient.setInsecure();  // Desactiva verificació SSL (només per proves)
}

void enviarMissatgeTelegramSimple(const String& missatge) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi no connectat. No s'envia missatge.");
        return;
    }

    if (secureClient.connected()) {
        Serial.println("Tanquem connexió anterior abans d'enviar missatge.");
        secureClient.stop();
    }

    Serial.println("Connectant a Telegram per enviar missatge...");
    if (!secureClient.connect("api.telegram.org", 443)) {
        Serial.println("No s'ha pogut connectar a Telegram via HTTPS");
        return;
    }

    // Evitem salts de línia al missatge per URL
    String missatgeEsc = missatge;
    missatgeEsc.replace("\n", " ");
    missatgeEsc.replace(" ", "%20");

    String url = "/bot" + botToken + "/sendMessage?chat_id=" + chatID +
                 "&text=" + missatgeEsc + "&parse_mode=Markdown";

    secureClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                       "Host: api.telegram.org\r\n" +
                       "Connection: close\r\n\r\n");

    Serial.println("Peticio enviada, esperant resposta...");

    while (secureClient.connected()) {
        String line = secureClient.readStringUntil('\n');
        Serial.println(line);
        if (line == "\r") break; // fi headers
    }

    String responseBody = secureClient.readString();
    Serial.println("Resposta del servidor:");
    Serial.println(responseBody);

    secureClient.stop();
}

void enviarMissatgeAmbCodi(int codi) {
    codiActual = String(codi);
    String missatge = "🚨 Alarma disparada! Envia el codi *" + codiActual + "* per desactivar-la.";
    Serial.println("Enviant missatge amb codi a Telegram...");
    enviarMissatgeTelegramSimple(missatge);
}

void actualitzaTelegram() {
    if (WiFi.status() != WL_CONNECTED) return;

    if (!esperantResposta) {
        if (secureClient.connected()) {
            Serial.println("Tanquem connexió anterior abans de nova petició.");
            secureClient.stop();
        }

        String url = "/bot" + botToken + "/getUpdates?offset=" + String(ultimUpdateId + 1);

        Serial.println("Connectant a Telegram per obtenir actualitzacions...");
        if (!secureClient.connect("api.telegram.org", 443)) {
            Serial.println("No s'ha pogut connectar a Telegram via HTTPS");
            return;
        }

        secureClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                           "Host: api.telegram.org\r\n" +
                           "Connection: close\r\n\r\n");

        esperantResposta = true;
        tempsIniciPeticio = millis();
        respostaTelegram = "";
    } else {
        while (secureClient.available()) {
            char c = secureClient.read();
            respostaTelegram += c;
        }

        if (!secureClient.connected() || (millis() - tempsIniciPeticio) > timeoutPeticio) {
            secureClient.stop();
            esperantResposta = false;

            int pos = respostaTelegram.indexOf("\r\n\r\n");
            if (pos == -1) {
                Serial.println("Resposta incompleta de Telegram");
                return;
            }

            String cosJson = respostaTelegram.substring(pos + 4);

            StaticJsonDocument<2048> doc;
            DeserializationError error = deserializeJson(doc, cosJson);
            if (error) {
                Serial.print("Error JSON: ");
                Serial.println(error.c_str());
                return;
            }

            JsonArray result = doc["result"].as<JsonArray>();
            for (JsonObject update : result) {
                unsigned long id = update["update_id"].as<unsigned long>();
                if (id > ultimUpdateId) ultimUpdateId = id;

                if (update.containsKey("message") && update["message"].containsKey("text")) {
                    const char* text = update["message"]["text"];
                    String missatge = String(text);

                    if (missatge == codiActual) {
                        Serial.println("Codi rebut correcte!");
                        codiRebutCorrectament = true;
                    } else if (missatge.equalsIgnoreCase("activar")) {
                        Serial.println("Comanda activar rebuda!");
                        activarRebut = true;
                    }
                }
            }
        }
    }
}

bool codiCorrecteRebut() {
    return codiRebutCorrectament;
}

void reiniciaCodi() {
    codiActual = "";
    codiRebutCorrectament = false;
}

bool peticioActivarRebuda() {
    return activarRebut;
}

void reiniciaPeticioActivar() {
    activarRebut = false;
}
