#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

#define I2S_SAMPLE_RATE   44100
#define TONE_FREQUENCY    440  // Hz (La)
#define I2S_BCLK          18
#define I2S_LRCLK         15
#define I2S_DOUT          21

#define PI 3.14159265
#define AMPLITUDE 10000  // Valor màxim per int16_t (ona suau)
#define BUFFER_SIZE 512

void setupI2S() {
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,  // mono
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRCLK,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);
}

void loopTone440Hz() {
  static int16_t buffer[BUFFER_SIZE];
  static float phase = 0.0;
  float phase_increment = 2.0 * PI * TONE_FREQUENCY / I2S_SAMPLE_RATE;

  for (int i = 0; i < BUFFER_SIZE; ++i) {
    buffer[i] = (int16_t)(AMPLITUDE * sin(phase));
    phase += phase_increment;
    if (phase >= 2.0 * PI) phase -= 2.0 * PI;
  }

  size_t bytes_written;
  i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);
}

void setup() {
  Serial.begin(115200);
  setupI2S();
  Serial.println("Generant to de 440 Hz per I2S...");
}

void loop() {
  loopTone440Hz();
}

