#ifndef TELEGRAMBOT_H
#define TELEGRAMBOT_H

#include <Arduino.h>

void configurarTelegram(const char* token, const char* chatId);
void actualitzaTelegram();

void enviarMissatgeAmbCodi(int codi);
bool codiCorrecteRebut();
void reiniciaCodi();

bool peticioActivarRebuda();
void reiniciaPeticioActivar();

#endif
