#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
  Serial.begin(115200);
  Serial.print("Iniciando SD ...");

  SPI.begin(18, 20, 21, 4);  // SCK, MISO, MOSI, CS

  if (!SD.begin(4)) { 
    Serial.println("No se pudo inicializar"); 
    return;
  } 

  Serial.println("Inicialización exitosa");

  // Abrimos el archivo para escribir
  myFile = SD.open("/archivo.txt", FILE_WRITE);
  
  if (myFile) {
    Serial.println("Escribiendo en archivo.txt...");
    myFile.println("Hola, Cocacola!");  // Escribimos en el archivo
    myFile.close();  // Cerramos el archivo
    Serial.println("Escritura completada.");
  } else {
    Serial.println("Error al abrir /archivo.txt para escritura");
  }

  // Abrimos el archivo para leer
  myFile = SD.open("/archivo.txt");
  if (myFile) { 
    Serial.println("archivo.txt:");
    while (myFile.available()) { 
      Serial.write(myFile.read()); 
    } 
    myFile.close(); // Cerramos el archivo 
  } else { 
    Serial.println("Error al abrir /archivo.txt");
  } 
}

void loop() {
  // No se necesita código en loop para esta tarea
}
