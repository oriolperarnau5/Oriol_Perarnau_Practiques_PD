#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>


// SSID & Password
const char* ssid = "Xiaomi_11T_Pro";  // Ingresa tu SSID
const char* password = "12345678"; // Ingresa tu contrasena


WebServer server(80); // Objeto del servidor HTTP


// Declaracion anticipada de la funcion handle_root
void handle_root();


void setup() {
    Serial.begin(115200);
    Serial.println("Intentando conectar a:");
    Serial.println(ssid);


    WiFi.begin(ssid, password);
   
    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && intentos < 20) {
        delay(1000);
        Serial.print(".");
        intentos++;
    }


    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi conectado exitosamente");
        Serial.print("IP asignada: ");
        Serial.println(WiFi.localIP()); // Muestra la IP asignada al ESP32-S3
    } else {
        Serial.println("\nError: No se pudo conectar al WiFi");
    }


    server.on("/", handle_root);
    server.begin();
    Serial.println("Servidor HTTP iniciado");
}


void loop() {
    server.handleClient();
}


// Manejo de la ruta raiz "/"
void handle_root() {
    String HTML = "<!DOCTYPE html>\
    <html>\
    <head>\
        <title>El Maravilloso Mundo de los Arboles</title>\
        <style>\
            body { font-family: Arial, sans-serif; text-align: center; background-color: #e8f5e9; padding: 20px; }\
            h1 { color: #2e7d32; font-size: 2.5em; }\
            .container { max-width: 900px; margin: auto; background: white; padding: 30px; border-radius: 15px; box-shadow: 0px 0px 15px rgba(0,0,0,0.2); }\
            img { width: 100%; border-radius: 10px; margin-top: 10px; }\
            p { text-align: justify; font-size: 1.2em; line-height: 1.6; }\
            .highlight { color: #1b5e20; font-weight: bold; }\
            .footer { margin-top: 20px; font-size: 1em; color: #555; }\
        </style>\
    </head>\
    <body>\
        <div class='container'>\
            <h1>El Maravilloso Mundo de los Arboles</h1>\
            <img src='https://upload.wikimedia.org/wikipedia/commons/8/8d/Tree_-_King%27s_Park.jpg' alt='Arbol majestuoso'>\
            <p>Los arboles son los gigantes silenciosos de nuestro planeta. Nos proporcionan <span class='highlight'>oxigeno</span>, sombra y hogar para innumerables especies.</p>\
            <p>Desde el imponente <span class='highlight'>baobab</span> en Africa hasta el milenario <span class='highlight'>secuoya</span> en California, cada arbol cuenta una historia unica.</p>\
            <p>Ademas de su belleza, los arboles desempeñan un papel vital en la regulacion del clima y la conservacion de los suelos.</p>\
            <h2>Algunos de los arboles mas impresionantes</h2>\
            <ul>\
                <li><span class='highlight'>El Arbol de Tule</span> - Uno de los troncos mas anchos del mundo, ubicado en Mexico.</li>\
                <li><span class='highlight'>General Sherman</span> - La secuoya mas grande del mundo por volumen.</li>\
                <li><span class='highlight'>Baobab Africano</span> - Conocido por su enorme tronco y su capacidad de almacenar agua.</li>\
            </ul>\
            <p>Si te apasiona la naturaleza, plantar un arbol puede ser una de las mejores formas de contribuir al bienestar del planeta.</p>\
            <div class='footer'>\
                <p>Creado con ESP32 - Explorando la tecnologia y la naturaleza</p>\
            </div>\
        </div>\
    </body>\
    </html>";


    server.send(200, "text/html", HTML);
}
