#include <Arduino.h>

#define RXD2 16
#define TXD2 17

String bufferUART0 = "";
String bufferUART2 = "";

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Test loop UART2. Escriu un text i es retornarà.");
}

void loop() {
  // Recollim l'entrada del terminal
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      Serial.print("TX → ");
      Serial.println(bufferUART0);
      Serial2.println(bufferUART0);  // envia amb \n
      bufferUART0 = "";
    } else if (c != '\r') { // evitem caràcter retorn de línia (\r)
      bufferUART0 += c;
    }
  }

  // Recollim el que retorna UART2
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '\n') {
      // Mostrem només quan la línia és completa
      Serial.print("UART2 → ");
      Serial.println(bufferUART2);
      bufferUART2 = "";
    } else if (c >= 32 && c <= 126) {  // només caràcters imprimibles
      bufferUART2 += c;
    }
  }
}



