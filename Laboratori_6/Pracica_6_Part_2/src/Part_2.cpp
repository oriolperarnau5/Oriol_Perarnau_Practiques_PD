#include <SPI.h> 
#include <MFRC522.h> 

// Assignació de pins (pots modificar-los si cal)
#define RST_PIN  9    // Pin per Reset del RC522
#define SS_PIN   10   // Pin per Chip Select (SDA del RC522)

// Creació de l'objecte del lector RFID
MFRC522 mfrc522(SS_PIN, RST_PIN); 

void setup() {
  Serial.begin(115200);        // Inicialitza comunicació sèrie
  Serial.println("Iniciant lector RFID...");

  // Inicialitza el bus SPI (pots personalitzar els GPIOs si vols)
  // Format: SPI.begin(SCK, MISO, MOSI, SS);
  // Si no especifiques res, usarà els GPIOs per defecte (VSPI o HSPI)
  SPI.begin(12, 13, 11, 10);  

  // Inicialitza el mòdul RC522
  mfrc522.PCD_Init(); 
  Serial.println("Lector preparat. Apropa una targeta RFID.");
}

void loop() {
  // Comprova si hi ha una nova targeta present
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Si hi ha una targeta, intenta llegir-la
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("UID de la targeta: ");
      // Mostra el UID byte a byte en hexadecimal
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      // Atura la comunicació amb la targeta actual
      mfrc522.PICC_HaltA(); 
    }
  }
}