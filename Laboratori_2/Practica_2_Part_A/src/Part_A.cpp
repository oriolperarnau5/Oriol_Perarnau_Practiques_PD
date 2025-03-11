#include <Arduino.h>


struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};


Button button1 = {10, 0, false};


void IRAM_ATTR isr() {
  button1.numberKeyPresses += 1;
  button1.pressed = true;
}


void setup() {
  Serial.begin(115200);
  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING);
}


void loop() {
  if (button1.pressed) {
      Serial.printf("Button 1 has been pressed %u times\n", button1.numberKeyPresses);
      button1.pressed = false;
  }


  static uint32_t lastMillis = 0;
  if (millis() - lastMillis > 60000) {
    lastMillis = millis();
    detachInterrupt(button1.PIN);
    Serial.println("Interrupt Detached!");
  }
}
