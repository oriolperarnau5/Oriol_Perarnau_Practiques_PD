#include "ServidorWeb.h"

const char* WWW_USERNAME = "admin";
const char* WWW_PASSWORD = "esp32";

ServidorWeb::ServidorWeb(Alarma* alarma) : server(80), alarma(alarma) {}

void ServidorWeb::afegeixEsdeveniment(const String& text) {
    time_t ara = time(nullptr);
    struct tm* tm_info = localtime(&ara);
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", tm_info);

    if (historial.size() >= 25) historial.erase(historial.begin());
    historial.push_back(String(buffer) + " - " + text);
}

bool ServidorWeb::autenticar() {
    if (!server.authenticate(WWW_USERNAME, WWW_PASSWORD)) {
        server.requestAuthentication();
        return false;
    }
    return true;
}

void ServidorWeb::iniciar(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.print("Connectant a WiFi");

    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && intentos < 20) {
        delay(500);
        Serial.print(".");
        intentos++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ WiFi connectat!");
        Serial.print("🌐 IP local: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n❌ Error: no s'ha pogut connectar a la WiFi.");
    }

    server.on("/", [this]() {
        if (!autenticar()) return;
        handleRoot();
    });

    server.on("/toggle", [this]() {
        if (!autenticar()) return;

        if (server.method() == HTTP_POST) {
            if (alarma->estaActiva()) {
                if (server.hasArg("code") && server.arg("code") == "1223") {
                    alarma->desactiva(); // ✅ entrada única
                    server.sendHeader("Location", "/");
                    server.send(303);
                } else {
                    server.send(200, "text/html", "<h2 style='color:red;'>Codi incorrecte ❌</h2><a href='/'>← Torna</a>");
                }
            } else {
                alarma->activa(); // ✅ entrada única
                server.sendHeader("Location", "/");
                server.send(303);
            }
        }
    });

    server.on("/historial", [this]() {
        if (!autenticar()) return;
        handleHistorial();
    });

    server.begin();
    Serial.println("🌍 Servidor web iniciat");
}

void ServidorWeb::actualitza() {
    server.handleClient();
}

void ServidorWeb::handleRoot() {
    String html = "<!DOCTYPE html><html lang='ca'><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Alarma ESP32</title>";
    html += "<style>";
    html += "body { font-family: 'Segoe UI', sans-serif; text-align: center; background: #f0f4f8; padding: 20px; }";
    html += ".container { background: white; padding: 30px; border-radius: 10px; max-width: 500px; margin: auto; box-shadow: 0 0 15px rgba(0,0,0,0.1); }";
    html += "h1 { color: #0d47a1; } p { font-size: 1.1em; }";
    html += ".activa { color: green; font-weight: bold; }";
    html += ".inactiva { color: orange; font-weight: bold; }";
    html += ".disparada { background: red; color: white; padding: 10px; border-radius: 8px; animation: blink 1s infinite; font-weight: bold; }";
    html += "@keyframes blink { 0%{background:red;} 50%{background:darkred;} 100%{background:red;} }";
    html += "input[type='password'], input[type='submit'] { padding: 10px; margin: 10px; border-radius: 5px; border: 1px solid #ccc; font-size: 1em; }";
    html += "input[type='submit'] { background-color: #007BFF; color: white; border: none; cursor: pointer; }";
    html += "input[type='submit']:hover { background-color: #0056b3; }";
    html += "a.btn { margin-top: 20px; display: inline-block; text-decoration: none; background: #4CAF50; color: white; padding: 10px 15px; border-radius: 5px; }";
    html += "footer { margin-top: 30px; font-size: 0.9em; color: #777; }";
    html += "</style></head><body><div class='container'>";

    html += "<h1>🔐 Estat de l'Alarma</h1>";
    html += "<p><b>Alarma:</b> ";
    html += alarma->estaActiva() ? "<span class='activa'>ACTIVA ✅</span>" : "<span class='inactiva'>INACTIVA ⚠️</span>";
    html += "</p>";

    html += "<p><b>Estat:</b><br>";
    if (alarma->estaDisparada()) {
        html += "<div class='disparada'>🚨 ALARMA DISPARADA</div>";
    } else {
        html += "🟢 NORMAL";
    }
    html += "</p>";

    html += "<form action='/toggle' method='post'>";
    if (alarma->estaActiva()) {
        html += "<p>🔐 Introdueix el codi per desactivar:</p>";
        html += "<input type='password' name='code' placeholder='Codi'><br>";
    }
    html += "<input type='submit' value='";
    html += alarma->estaActiva() ? "Desactivar Alarma" : "Activar Alarma";
    html += "'>";
    html += "</form>";

    html += "<a class='btn' href='/historial'>📜 Veure historial</a>";
    html += "<footer>IP del dispositiu: " + WiFi.localIP().toString() + "</footer>";
    html += "</div></body></html>";

    server.send(200, "text/html", html);
}

void ServidorWeb::handleHistorial() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Historial</title>";
    html += "<style>body{font-family:sans-serif;background:#f4f4f4;padding:20px;}h1{color:#333;}ul{list-style:none;padding:0;}li{background:white;margin:10px 0;padding:10px;border-radius:5px;box-shadow:0 2px 5px rgba(0,0,0,0.1);}</style></head><body>";
    html += "<h1>📜 Historial d'Esdeveniments</h1><ul>";
    for (int i = historial.size() - 1; i >= 0; --i) {
        html += "<li>" + historial[i] + "</li>";
    }
    html += "</ul><p><a href='/'>← Tornar</a></p></body></html>";
    server.send(200, "text/html", html);
}
