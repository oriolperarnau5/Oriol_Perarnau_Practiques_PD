#include <Arduino.h>
#include <soc/gpio_reg.h>
#include <driver/gpio.h>


#define LED_PIN 48  // Asegúrate de usar el pin correcto
#define DELAY_TIME 500 // 1000 ms = 1 segundo


void setup() {
    Serial.begin(115200);
   
    // Configurar el pin como salida
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction((gpio_num_t)LED_PIN, GPIO_MODE_OUTPUT);
}


void loop() {
    volatile uint32_t *gpio_out = (volatile uint32_t *)GPIO_OUT_REG;


    // Encender LED
    *gpio_out |= (1 << LED_PIN);
    Serial.println("ON");
    delay(DELAY_TIME);


    // Apagar LED
    *gpio_out &= ~(1 << LED_PIN);
    Serial.println("OFF");
    delay(DELAY_TIME);
}
