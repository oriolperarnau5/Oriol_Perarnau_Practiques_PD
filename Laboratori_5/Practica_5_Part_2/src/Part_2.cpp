#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Configura la mida de la pantalla (SSD1306 de 128x64)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


// Defineix l'objecte de la pantalla OLED amb la seva adreça I2C (0x3C)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() {
    Serial.begin(115200);
   
    // Inicialitza la comunicació I2C amb els pins correctes per ESP32-S3
    Wire.begin(8, 9);  // SDA=GPIO 8, SCL=GPIO 9


    // Inicialitza la pantalla OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  
        Serial.println(F("Error: No s'ha detectat la pantalla SSD1306"));
        while (true); // Atura l'execució si la pantalla no es detecta
    }


    Serial.println(F("Pantalla SSD1306 detectada!"));


    // Esborra la pantalla
    display.clearDisplay();


    // Configura el color del text (blanc sobre fons negre)
    display.setTextColor(SSD1306_WHITE);


    // Escriu un missatge a la pantalla
    display.setTextSize(2);             // Mida del text
    display.setCursor(10, 20);          // Posició del text (X, Y)
    display.println("Oriol i Pau");          // Missatge a mostrar


    display.display();  // Actualitza la pantalla
}


void loop() {
    // Aquí pots afegir més funcions si vols animacions o gràfics
}

