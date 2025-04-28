#include "Audio.h" 
#include "SD.h" 
#include "FS.h" 

// Pins per a la targeta SD (SPI)
#define SD_CS     10
#define SPI_MOSI  11
#define SPI_MISO  13
#define SPI_SCK   12

// Pins per a l'I2S (MAX98357A)
#define I2S_DOUT  21
#define I2S_BCLK  18
#define I2S_LRC   15

Audio audio;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("🎵 Iniciant reproducció d'MP3 des de SD...");

  // Inicialitza SPI i la SD
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

  if (!SD.begin(SD_CS)) {
    Serial.println("No s'ha pogut inicialitzar la targeta SD.");
    return;
  }

  Serial.println("Targeta SD detectada correctament.");

  // Configura I2S
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(12);  // Volum entre 0 i 21

  // Reproduir el fitxer MP3
  audio.connecttoFS(SD, "/fat-kick-drumloop-99bpm-141016.mp3");
}

void loop() {
  audio.loop();
}

// Funcions opcionals per a diagnòstic
void audio_info(const char *info)             { Serial.print("info        "); Serial.println(info); }
void audio_id3data(const char *info)          { Serial.print("id3data     "); Serial.println(info); }
void audio_eof_mp3(const char *info)          { Serial.print("eof_mp3     "); Serial.println(info); }
void audio_showstation(const char *info)      { Serial.print("station     "); Serial.println(info); }
void audio_showstreaminfo(const char *info)   { Serial.print("streaminfo  "); Serial.println(info); }
void audio_showstreamtitle(const char *info)  { Serial.print("streamtitle "); Serial.println(info); }
void audio_bitrate(const char *info)          { Serial.print("bitrate     "); Serial.println(info); }
void audio_commercial(const char *info)       { Serial.print("commercial  "); Serial.println(info); }
void audio_icyurl(const char *info)           { Serial.print("icyurl      "); Serial.println(info); }
void audio_lasthost(const char *info)         { Serial.print("lasthost    "); Serial.println(info); }
void audio_eof_speech(const char *info)       { Serial.print("eof_speech  "); Serial.println(info); }

